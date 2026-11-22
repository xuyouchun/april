#include <thread_manager.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>

namespace X_ROOT_NS::modules::rtlib {

    ////////// ////////// ////////// ////////// //////////

    std::atomic<bool> thread_yield_requested { false };
    std::atomic<bool> thread_parallel { false };

    namespace
    {
        using __clock_t = std::chrono::steady_clock;

        // The time that a thread waits for the lock before it asks the one that runs to give it.
        const std::chrono::milliseconds __time_slice(10);

        // The state of a thread in a monitor.
        enum __state_t
        {
            __idle      = 0,
            __entering  = 1,        // In the queue of the entry of a monitor.
            __waiting   = 2,        // In the queue of the waiting (Monitor.Wait) of a monitor.
            __granted   = 3,        // The monitor was given to it.
        };

        // The lock of an object.
        struct __monitor_t
        {
            thread_t *              owner = nullptr;
            int32_t                 count = 0;
            std::deque<thread_t *>  entry_queue;
            std::deque<thread_t *>  wait_queue;
        };

        thread_local thread_t * __current_thread = nullptr;

        const thread_switch_hooks_t * __hooks = nullptr;
    }

    // Sets the hooks.
    void set_thread_switch_hooks(const thread_switch_hooks_t * hooks)
    {
        __hooks = hooks;
    }

    ////////// ////////// ////////// ////////// //////////

    // The data of the manager.
    struct thread_manager_data_t
    {
        // The global lock: the thread that holds it runs, the others wait in the queue (in the order they came).
        std::mutex                  gil_mutex;
        std::condition_variable     gil_cv;
        thread_t *                  owner = nullptr;
        std::deque<thread_t *>      ready;

        // The threads that run the managed code (parallel mode), and the collector that asks them to stop.
        std::mutex                  state_mutex;
        std::condition_variable     state_cv;
        int                         running = 0;
        bool                        gc_requested = false;

        // The rest is used by the thread that holds the global lock only.
        std::vector<std::unique_ptr<thread_t>>      threads;
        int64_t                                     next_id = 1;
        int                                         foreground_running = 0;
        std::vector<thread_t *>                     exit_waiters;
        std::unordered_map<void *, __monitor_t>     monitors;

        // Takes the global lock.
        void acquire(thread_t * thread)
        {
            std::unique_lock<std::mutex> lock(gil_mutex);
            ready.push_back(thread);

            while (owner != nullptr || ready.front() != thread)
            {
                // The threads that run in parallel do not ask for a turn.
                if (gil_cv.wait_for(lock, __time_slice) == std::cv_status::timeout && !thread_parallel.load())
                    thread_yield_requested.store(true, std::memory_order_relaxed);
            }

            ready.pop_front();
            owner = thread;

            if (!thread_parallel.load())
                thread_yield_requested.store(false, std::memory_order_relaxed);

            lock.unlock();

            if (__hooks != nullptr && __hooks->resumed != nullptr)
                __hooks->resumed(thread);
        }

        // Gives the global lock: the thread is the one that gives it (null when it ends).
        void release(thread_t * thread)
        {
            if (thread != nullptr && __hooks != nullptr && __hooks->stopping != nullptr)
                __hooks->stopping(thread);

            {
                std::lock_guard<std::mutex> lock(gil_mutex);
                owner = nullptr;
            }

            gil_cv.notify_all();
        }

        // Wakes up a thread that is parked (or that is about to be).
        static void unpark(thread_t * thread)
        {
            {
                std::lock_guard<std::mutex> lock(thread->__mutex);
                thread->__wake = true;
            }

            thread->__cv.notify_one();
        }

        // The thread waits until it is woken up, the time is in milliseconds (-1 is for ever), the others run. It
        // returns false when the time was over; the global lock is held when it returns.
        bool park(thread_t * self, int64_t timeout)
        {
            bool woken = true;

            release(self);

            {
                std::unique_lock<std::mutex> lock(self->__mutex);

                if (timeout < 0)
                    self->__cv.wait(lock, [&] { return self->__wake; });
                else
                    woken = self->__cv.wait_for(lock, std::chrono::milliseconds(timeout), [&] { return self->__wake; });

                self->__wake = false;
            }

            acquire(self);
            return woken;
        }

        // The milliseconds that are left to a deadline: -1 is for ever, 0 is that the time is over.
        static int64_t remaining(int64_t timeout, const __clock_t::time_point & begin)
        {
            if (timeout < 0)
                return -1;

            auto spent = std::chrono::duration_cast<std::chrono::milliseconds>(__clock_t::now() - begin).count();
            return spent >= timeout? 0 : timeout - (int64_t)spent;
        }

        // Gives the monitor to the thread that waits the first at its entry.
        void grant(__monitor_t & monitor)
        {
            if (monitor.entry_queue.empty())
                return;

            thread_t * next = monitor.entry_queue.front();
            monitor.entry_queue.pop_front();

            monitor.owner = next;
            monitor.count = next->__pending_count;
            next->__state = __granted;

            unpark(next);
        }

        // Forgets the monitor of an object that nobody holds nor waits for.
        void cleanup(void * obj, __monitor_t & monitor)
        {
            if (monitor.owner == nullptr && monitor.entry_queue.empty() && monitor.wait_queue.empty())
                monitors.erase(obj);
        }

        template<typename _container_t>
        static void remove_from(_container_t & queue, thread_t * thread)
        {
            for (auto it = queue.begin(); it != queue.end(); ++it)
            {
                if (*it == thread)
                {
                    queue.erase(it);
                    return;
                }
            }
        }

        // Waits at the entry of the monitor until it is given, or until the time is over (the thread is not in the
        // queue then). The monitor is not held by another thread when the thread comes, or it is.
        bool enter_queue(void * obj, __monitor_t & monitor, thread_t * self, int32_t count, int64_t timeout)
        {
            auto begin = __clock_t::now();

            self->__state = __entering;
            self->__pending_count = count;
            monitor.entry_queue.push_back(self);

            while (self->__state != __granted)
            {
                int64_t left = remaining(timeout, begin);

                // A thread that was pulsed waits for the monitor with no end: it has been woken up.
                if (left == 0 && self->__state == __entering)
                {
                    // The time was over: it leaves the queue (the monitor is not touched while the global lock is held).
                    remove_from(monitor.entry_queue, self);
                    self->__state = __idle;
                    return false;
                }

                park(self, left);
            }

            self->__state = __idle;
            return true;
        }
    };

    ////////// ////////// ////////// ////////// //////////

    thread_manager_t::thread_manager_t() : __data(new thread_manager_data_t()) { }

    thread_manager_t::~thread_manager_t() { }

    // The only manager (it is never destroyed: the threads that are running use it).
    thread_manager_t & thread_manager_t::instance()
    {
        static thread_manager_t * manager = new thread_manager_t();
        return *manager;
    }

    // Makes a thread of the running OS thread (it holds the global lock).
    static thread_t * __attach(thread_manager_data_t & data, thread_t * thread, void * stack_base)
    {
        __current_thread = thread;

        data.acquire(thread);

        // It holds the lock: the back end changes its tables.
        if (__hooks != nullptr && __hooks->attached != nullptr)
            __hooks->attached(thread, stack_base);

        return thread;
    }

    // The threads that run the managed code: the thread has been in the runtime, it is not now.
    static void __running_enter(thread_manager_data_t & data)
    {
        {
            std::lock_guard<std::mutex> lock(data.state_mutex);
            data.running++;
        }
    }

    // The thread leaves the managed code (it enters the runtime or it stops): the collector does not wait for it.
    static void __running_leave(thread_manager_data_t & data)
    {
        {
            std::lock_guard<std::mutex> lock(data.state_mutex);
            data.running--;
        }

        data.state_cv.notify_all();
    }

    // The thread calls the runtime: it gives the code of the program, takes the global lock.
    void thread_manager_t::runtime_enter()
    {
        thread_t * self = __current_thread;
        if (self == nullptr || self->__depth++ > 0)
            return;

        // It is stopped (its stack and its registers are kept) while it waits for the lock.
        if (__hooks != nullptr && __hooks->stopping != nullptr)
            __hooks->stopping(self);

        __running_leave(*__data);
        __data->acquire(self);
    }

    // The thread goes back to the code of the program.
    void thread_manager_t::runtime_leave()
    {
        thread_t * self = __current_thread;
        if (self == nullptr || --self->__depth > 0)
            return;

        // It runs again before the lock is given: a collection that begins then waits for it.
        __running_enter(*__data);
        __data->release(nullptr);
    }

    int32_t thread_manager_t::runtime_suspend()
    {
        thread_t * self = __current_thread;
        if (self == nullptr || self->__depth == 0)
            return 0;

        int32_t depth = self->__depth;

        __running_enter(*__data);
        self->__depth = 0;
        __data->release(nullptr);

        return depth;
    }

    void thread_manager_t::runtime_resume(int32_t depth)
    {
        thread_t * self = __current_thread;
        if (self == nullptr || depth == 0)
            return;

        if (__hooks != nullptr && __hooks->stopping != nullptr)
            __hooks->stopping(self);

        __running_leave(*__data);
        __data->acquire(self);

        self->__depth = depth;
    }

    // A thread that runs the managed code stops until the collection ends.
    void thread_manager_t::__stop_for_collection(thread_t * self)
    {
        thread_manager_data_t & data = *__data;

        {
            std::unique_lock<std::mutex> lock(data.state_mutex);
            if (!data.gc_requested)
                return;
        }

        if (__hooks != nullptr && __hooks->stopping != nullptr)
            __hooks->stopping(self);

        __running_leave(data);

        {
            std::unique_lock<std::mutex> lock(data.state_mutex);

            // It runs again when no collection waits (a new one that begins now waits for it).
            data.state_cv.wait(lock, [&data] { return !data.gc_requested; });
            data.running++;
        }

        if (__hooks != nullptr && __hooks->resumed != nullptr)
            __hooks->resumed(self);
    }

    // The collector asks the threads that run the managed code to stop, and waits until they have.
    void thread_manager_t::stop_world()
    {
        if (!thread_parallel.load())
            return;

        {
            std::unique_lock<std::mutex> lock(__data->state_mutex);
            __data->gc_requested = true;
            thread_yield_requested.store(true);

            __data->state_cv.wait(lock, [this] { return __data->running == 0; });
        }
    }

    void thread_manager_t::resume_world()
    {
        if (!thread_parallel.load())
            return;

        {
            std::lock_guard<std::mutex> lock(__data->state_mutex);
            __data->gc_requested = false;
            thread_yield_requested.store(false);
        }

        __data->state_cv.notify_all();
    }

    // Makes the running OS thread the main thread.
    thread_t * thread_manager_t::attach_main(void * stack_base)
    {
        if (__current_thread != nullptr)
            return __current_thread;

        std::unique_ptr<thread_t> thread(new thread_t());
        thread->__id = __data->next_id++;

        thread_t * t = thread.get();
        __data->threads.push_back(std::move(thread));

        return __attach(*__data, t, stack_base);
    }

    // The thread of the running OS thread (it is made when there is none: a thread that the program did not start).
    thread_t * thread_manager_t::current()
    {
        return __current_thread;
    }

    #define __Self()                                                                    \
        thread_t * self = __current_thread != nullptr? __current_thread : attach_main()

    // The main thread ended: waits for the foreground threads.
    void thread_manager_t::wait_for_foreground()
    {
        __Self();

        while (__data->foreground_running > 0)
        {
            __data->exit_waiters.push_back(self);
            __data->park(self, -1);
            thread_manager_data_t::remove_from(__data->exit_waiters, self);
        }
    }

    // Starts a thread.
    thread_t * thread_manager_t::start(void * entry, bool background)
    {
        __Self();

        std::unique_ptr<thread_t> thread(new thread_t());
        thread->__id         = __data->next_id++;
        thread->__entry      = entry;
        thread->__background = background;

        thread_t * t = thread.get();
        __data->threads.push_back(std::move(thread));

        if (!background)
            __data->foreground_running++;

        thread_manager_data_t * data = __data;
        thread_host_t * host = __host;

        // The first thread of a program that can run them in parallel: the one that starts it goes on without the lock when
        // it goes back to the program (its call of the runtime ends).
        if (host != nullptr && host->parallel() && !thread_parallel.load())
        {
            self->__depth = 1;
            thread_parallel.store(true);
        }

        std::thread([data, host, t] {

            int stack_mark = 0;
            __attach(*data, t, &stack_mark);

            bool parallel = thread_parallel.load();

            // The code of the thread runs without the lock.
            if (parallel)
            {
                t->__depth = 1;
                __running_enter(*data);
                data->release(nullptr);
                t->__depth = 0;
            }

            bool ok = host != nullptr && host->run(t);

            if (parallel)
            {
                // Back in the runtime: it ends holding the lock.
                t->__depth = 1;
                if (__hooks != nullptr && __hooks->stopping != nullptr)
                    __hooks->stopping(t);

                __running_leave(*data);
                data->acquire(t);
            }

            if (!ok)
            {
                // An exception that nothing catches ends the program.
                std::fflush(nullptr);
                std::wcerr.flush();
                std::_Exit(1);
            }

            t->__finished = true;
            t->__entry = nullptr;

            for (thread_t * joiner : t->__joiners)
                thread_manager_data_t::unpark(joiner);

            t->__joiners.clear();

            if (!t->__background)
                data->foreground_running--;

            for (thread_t * waiter : data->exit_waiters)
                thread_manager_data_t::unpark(waiter);

            data->exit_waiters.clear();

            if (__hooks != nullptr && __hooks->detached != nullptr)
                __hooks->detached(t);

            t->__depth = 0;
            data->release(nullptr);

        }).detach();

        return t;
    }

    // The thread of an id.
    thread_t * thread_manager_t::find(int64_t id)
    {
        for (auto & thread : __data->threads)
        {
            if (thread->__id == id)
                return thread.get();
        }

        return nullptr;
    }

    // Sets whether the program does not wait for the thread.
    void thread_manager_t::set_background(thread_t * thread, bool background)
    {
        if (thread->__background == background)
            return;

        thread->__background = background;

        // The main thread does not count.
        if (thread->__id == 1 || thread->__finished)
            return;

        __data->foreground_running += background? -1 : 1;

        for (thread_t * waiter : __data->exit_waiters)
            thread_manager_data_t::unpark(waiter);

        __data->exit_waiters.clear();
    }

    // Waits for the end of a thread.
    bool thread_manager_t::join(thread_t * thread, int64_t timeout)
    {
        __Self();

        auto begin = __clock_t::now();

        while (!thread->__finished)
        {
            int64_t left = thread_manager_data_t::remaining(timeout, begin);
            if (left == 0)
                return false;

            thread->__joiners.push_back(self);
            __data->park(self, left);
            thread_manager_data_t::remove_from(thread->__joiners, self);
        }

        return true;
    }

    // Waits some milliseconds.
    void thread_manager_t::sleep(int64_t milliseconds)
    {
        __Self();

        __data->release(self);

        if (milliseconds > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        else
            std::this_thread::yield();

        __data->acquire(self);
    }

    // Gives the lock to the threads that wait for it.
    void thread_manager_t::yield()
    {
        __Self();

        __data->release(self);
        std::this_thread::yield();
        __data->acquire(self);
    }

    // The objects that the threads are made of.
    void thread_manager_t::each_gc_root(const std::function<void (void *)> & f)
    {
        for (auto & thread : __data->threads)
        {
            if (!thread->__finished && thread->__entry != nullptr)
                f(thread->__entry);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // Monitors.

    // Takes the lock of the object.
    void thread_manager_t::monitor_enter(void * obj)
    {
        monitor_try_enter(obj, -1);
    }

    // Takes the lock of the object if it is free for the time.
    bool thread_manager_t::monitor_try_enter(void * obj, int64_t timeout)
    {
        __Self();

        __monitor_t & monitor = __data->monitors[obj];

        if (monitor.owner == nullptr)
        {
            monitor.owner = self;
            monitor.count = 1;
            return true;
        }

        if (monitor.owner == self)
        {
            monitor.count++;
            return true;
        }

        if (timeout == 0)
            return false;

        bool taken = __data->enter_queue(obj, monitor, self, 1, timeout);

        // The table does not move the monitors: the reference is still the one of the object.
        if (!taken)
            __data->cleanup(obj, monitor);

        return taken;
    }

    // Releases the lock of the object.
    bool thread_manager_t::monitor_exit(void * obj)
    {
        __Self();

        auto it = __data->monitors.find(obj);
        if (it == __data->monitors.end() || it->second.owner != self)
            return false;

        __monitor_t & monitor = it->second;
        if (--monitor.count > 0)
            return true;

        monitor.owner = nullptr;
        monitor.count = 0;

        __data->grant(monitor);
        __data->cleanup(obj, monitor);

        return true;
    }

    // Releases the lock, waits for a Pulse, takes the lock again.
    monitor_wait_result_t thread_manager_t::monitor_wait(void * obj, int64_t timeout)
    {
        __Self();

        auto it = __data->monitors.find(obj);
        if (it == __data->monitors.end() || it->second.owner != self)
            return monitor_wait_result_t::not_owner;

        __monitor_t & monitor = it->second;
        int32_t saved = monitor.count;
        auto begin = __clock_t::now();

        self->__state = __waiting;
        self->__pending_count = saved;
        monitor.wait_queue.push_back(self);

        monitor.owner = nullptr;
        monitor.count = 0;
        __data->grant(monitor);

        for (;;)
        {
            if (self->__state == __granted)
            {
                self->__state = __idle;
                return monitor_wait_result_t::pulsed;
            }

            // A thread that was pulsed waits for the monitor, as long as it needs.
            int64_t left = self->__state == __waiting? thread_manager_data_t::remaining(timeout, begin) : -1;

            if (left == 0)
            {
                // The time was over before a Pulse: it takes the lock again as a thread that enters.
                thread_manager_data_t::remove_from(monitor.wait_queue, self);
                self->__state = __idle;

                if (monitor.owner == nullptr)
                {
                    monitor.owner = self;
                    monitor.count = saved;
                }
                else
                {
                    __data->enter_queue(obj, monitor, self, saved, -1);
                }

                return monitor_wait_result_t::timed_out;
            }

            __data->park(self, left);
        }
    }

    // Wakes up the threads that wait in the object.
    bool thread_manager_t::monitor_pulse(void * obj, bool all)
    {
        __Self();

        auto it = __data->monitors.find(obj);
        if (it == __data->monitors.end() || it->second.owner != self)
            return false;

        __monitor_t & monitor = it->second;

        while (!monitor.wait_queue.empty())
        {
            thread_t * waiter = monitor.wait_queue.front();
            monitor.wait_queue.pop_front();

            // It waits for the monitor with the others that enter (the owner gives it when it exits).
            waiter->__state = __entering;
            monitor.entry_queue.push_back(waiter);

            if (!all)
                break;
        }

        return true;
    }

    // Whether the running thread holds the lock of the object.
    bool thread_manager_t::monitor_is_entered(void * obj)
    {
        __Self();

        auto it = __data->monitors.find(obj);
        return it != __data->monitors.end() && it->second.owner == self;
    }

    ////////// ////////// ////////// ////////// //////////

    // The slow part of the safepoint.
    void thread_safepoint()
    {
        thread_manager_t & manager = thread_manager_t::instance();
        thread_t * self = manager.current();

        if (self == nullptr)
        {
            thread_yield_requested.store(false, std::memory_order_relaxed);
            return;
        }

        // The threads run in parallel: a collection asks the thread that runs to stop (the flag may be old).
        if (thread_parallel.load())
            manager.__stop_for_collection(self);
        else
            manager.yield();
    }

}   // namespace X_ROOT_NS::modules::rtlib
