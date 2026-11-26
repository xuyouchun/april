#ifndef __THREAD_MANAGER_H__
#define __THREAD_MANAGER_H__

#include <common.h>

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

// The threads of the managed programs, for the interpreter and for the native programs.
//
// There are two modes. In the first one (the interpreter, and a native program that has not started a thread) one managed
// thread runs at a time: the one that holds the global lock. In the second one (a native program that started a thread: the
// host says that it can) the managed code runs in parallel, and the lock is held only while a thread is in the runtime: a thread
// takes it when it calls a function of the runtime (the allocation, the functions of the library, the threads, the locks),
// gives it when it goes back to the code of the program. A collection stops the world: the thread that collects asks the
// others to stop (they look at the flag at the safepoints), and waits until each of them is stopped (running managed code is
// the only state that is not).
//
// The mode that is first:
// The OS threads are real, but one managed thread runs at a time: the one that holds the global lock (the GIL). A thread
// gives it to the others at the safepoints (the back ends call thread_poll() at the calls and at the jumps that go back),
// and when it blocks (Sleep, Join, Monitor.Enter that waits, Monitor.Wait). So the heap, the garbage collector and the
// tables that are made when they are first needed do not need a lock of their own, and a collection finds the other
// threads stopped at the places that are known.

namespace X_ROOT_NS::modules::rtlib {

    class thread_manager_t;
    struct thread_manager_data_t;

    ////////// ////////// ////////// ////////// //////////

    // A managed thread.
    class thread_t
    {
        friend class thread_manager_t;
        friend struct thread_manager_data_t;

    public:
        // The number of the thread: 1 is the thread that started the program.
        int64_t id() const { return __id; }

        // Whether the program waits for the thread to end (a foreground thread).
        bool is_background() const { return __background; }

        // The managed object of the thread (System.Threading.Thread), null for the thread that started the program,
        // until the object is asked for.
        void * entry() const { return __entry; }

        // Whether the thread has ended.
        bool is_finished() const { return __finished; }

        // The thread has a place for the data of the back end (the context of the execution).
        void * host_data = nullptr;

        // How many times the thread entered the runtime (the calls of the runtime that are in each other): the lock is taken by
        // the first and given by the last.
        int32_t depth() const { return __depth; }

    private:
        int64_t         __id = 0;
        bool            __background = false;
        bool            __finished = false;
        void *          __entry = nullptr;

        // Wakes up a thread that is parked: the flag is kept until the thread waits.
        std::mutex                  __mutex;
        std::condition_variable     __cv;
        bool                        __wake = false;

        std::vector<thread_t *>     __joiners;          // The threads that wait for the end of this one.

        // The state in a monitor (see thread_manager.cpp).
        int             __state = 0;
        int32_t         __pending_count = 0;

        int32_t         __depth = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    // What the back end does when a thread starts: it runs the managed method of the thread. It returns false when an
    // exception ended it (the process is ended then: an exception that nothing catches ends the program).
    class thread_host_t
    {
    public:
        virtual ~thread_host_t() = default;

        virtual bool run(thread_t * thread) = 0;

        // Whether the managed code of the threads can run in parallel (see the beginning of this file).
        virtual bool parallel() { return false; }
    };

    ////////// ////////// ////////// ////////// //////////

    // What a back end does when the threads switch: the native programs keep the stack and the registers of a thread that
    // stops, for the garbage collector, and the exception that is pending (it is one variable of the program).
    struct thread_switch_hooks_t
    {
        // A thread began, it holds the lock: `stack_base` is the highest address of its stack that has frames
        // of the program.
        void (* attached)(thread_t * thread, void * stack_base) = nullptr;

        // The thread gives the lock (it waits, or it gives its turn): its stack and registers must be kept.
        void (* stopping)(thread_t * thread) = nullptr;

        // The thread took the lock again.
        void (* resumed)(thread_t * thread) = nullptr;

        // The thread ended, it still holds the lock.
        void (* detached)(thread_t * thread) = nullptr;
    };

    // Sets the hooks (they are not copied).
    void set_thread_switch_hooks(const thread_switch_hooks_t * hooks);

    ////////// ////////// ////////// ////////// //////////

    // The result of Monitor.Wait.
    enum class monitor_wait_result_t
    {
        not_owner   = 0,        // The thread does not hold the lock.
        pulsed      = 1,        // Pulse (or PulseAll) woke it up.
        timed_out   = 2,        // The time was over.
    };

    // The threads, the lock of the objects.
    class thread_manager_t
    {
    public:
        // The only manager.
        static thread_manager_t & instance();

        // Makes the running OS thread the main thread (the one that starts the program), it takes the global lock.
        // `stack_base` is the highest address of its stack that has frames of the program (for the hooks).
        thread_t * attach_main(void * stack_base = nullptr);

        // The main thread ended: waits for the foreground threads.
        void wait_for_foreground();

        // The managed thread of the running OS thread, null if it is not one.
        thread_t * current();

        // Sets the back end that runs the threads.
        void set_host(thread_host_t * host) { __host = host; }

        // The code of the program that runs in parallel calls the runtime between runtime_enter and runtime_leave (they nest).
        // When the threads do not run in parallel they do nothing.
        void runtime_enter();
        void runtime_leave();

        // A function of the runtime that calls the code of the program (Method.Invoke) gives the lock for the time of the call:
        // runtime_suspend gives it whatever the depth is (it returns the depth), runtime_resume takes it back.
        int32_t runtime_suspend();
        void runtime_resume(int32_t depth);

        // Whether the host runs the managed code of the threads in parallel.
        bool host_is_parallel() const { return __host != nullptr && __host->parallel(); }

        // The collector (that holds the lock) stops the other threads, and lets them go on.
        void stop_world();
        void resume_world();

        // Starts a thread: `entry` is the managed object, that the back end passes to the method of the thread.
        // The new thread runs when the starting one gives the lock.
        thread_t * start(void * entry, bool background);

        // The thread of an id, null if there is none.
        thread_t * find(int64_t id);

        // Sets whether the program does not wait for the thread.
        void set_background(thread_t * thread, bool background);

        // Waits for the end of a thread: the time is in milliseconds, -1 is for ever. Returns whether it ended.
        bool join(thread_t * thread, int64_t timeout);

        // Waits some milliseconds, the others run.
        void sleep(int64_t milliseconds);

        // Runs a function that waits for the system (a file, the console): the lock is given back while it runs, so the other
        // threads go on (it must not use the objects of the program).
        void blocking(const std::function<void ()> & f);

        // Gives the lock to the threads that wait for it.
        void yield();

        // The garbage collector reads the objects that the threads that are running are made of.
        void each_gc_root(const std::function<void (void *)> & f);

        ////////// ////////// ////////// ////////// //////////
        // The lock of the objects (System.Threading.Monitor).

        // Takes the lock of the object, waits until it is free.
        void monitor_enter(void * obj);

        // Takes the lock of the object if it is free for the time (milliseconds, -1 is for ever), returns whether it was taken.
        bool monitor_try_enter(void * obj, int64_t timeout);

        // Releases the lock of the object, returns false when the thread does not hold it.
        bool monitor_exit(void * obj);

        // Releases the lock of the object, waits for a Pulse, takes the lock again.
        monitor_wait_result_t monitor_wait(void * obj, int64_t timeout);

        // Wakes up one thread (or all) that waits in the object, returns false when the thread does not hold the lock.
        bool monitor_pulse(void * obj, bool all);

        // Returns whether the running thread holds the lock of the object.
        bool monitor_is_entered(void * obj);

        // A thread that runs the managed code stops, while a collection is made (used by thread_safepoint).
        void __stop_for_collection(thread_t * thread);

    private:
        thread_manager_t();
        ~thread_manager_t();

        thread_manager_data_t *     __data;
        thread_host_t * __host = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////

    // A thread that waits for the lock asks the one that runs to give it, after a time slice.
    extern std::atomic<bool> thread_yield_requested;

    // Whether the threads run in parallel (it is set when the first thread of a program that can start).
    extern std::atomic<bool> thread_parallel;

    // The slow part of the safepoint: gives the lock to the threads that wait; when the threads run in parallel, a thread that
    // runs stops there when the collector asks it.
    void thread_safepoint();

    // A function that the thread that runs calls at the safepoint when the flag of the yield is set (the back end sets it when it
    // has something to do between two commands: the interpreter runs the destructors of the objects that the collector found).
    void set_safepoint_hook(void (* hook)());

    // A call of the runtime (see thread_manager_t::runtime_enter).
    struct runtime_guard_t
    {
        runtime_guard_t()
        {
            if (thread_parallel.load(std::memory_order_relaxed))
                thread_manager_t::instance().runtime_enter();
        }

        ~runtime_guard_t()
        {
            if (thread_parallel.load(std::memory_order_relaxed))
                thread_manager_t::instance().runtime_leave();
        }
    };

    // The safepoint: the back ends call it at the calls and at the jumps that go back.
    inline void thread_poll()
    {
        if (thread_yield_requested.load(std::memory_order_relaxed)) [[unlikely]]
            thread_safepoint();
    }

}   // namespace X_ROOT_NS::modules::rtlib

#endif  // __THREAD_MANAGER_H__
