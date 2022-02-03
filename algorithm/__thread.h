
#ifndef __ALGORITHM_THREAD_H__
#define __ALGORITHM_THREAD_H__

namespace X_ROOT_NS::algorithm {

    ////////// ////////// ////////// ////////// //////////

    // Mutex lock
    #define X_LOCK(mtx, args...)    \
            ::std::lock_guard<decltype(mtx)> X_TEMP_VAR(mtx, ##args)
    #define _L(mtx, args...)     X_LOCK(mtx, ##args)

    // Unique lock
    #define X_UNIQUE_LOCK(mtx, name, args...)   \
            ::std::unique_lock<decltype(mtx)> name(mtx, ##args)
    #define _UL(mtx, name, args...)     X_UNIQUE_LOCK(mtx, name, ##args)

    // Object lock
    #define X_LOCK_OBJECT(obj, args...) \
            ::X_ROOT_NS::algorithm::object_lock_guard_t<> X_TEMP_VAR(obj, ##args)

    #define _OL(obj, args...) X_LOCK_OBJECT(obj, ##args)

    // Lock self in class member method.
    #define X_LOCK_THIS X_LOCK_OBJECT(this)

    ////////// ////////// ////////// ////////// //////////

    // The atomic operation for two lockers.
    template<typename lock1_t, typename lock2_t>
    void unlock_and_lock(lock1_t & lock1, lock2_t & lock2)
    {
        // TODO: not a atomic operation
        lock1.unlock();
        lock2.lock();
    }

    ////////// ////////// ////////// ////////// //////////

    // The base class to provide ability of lazy initialize.
    class initializer_t
    {
    protected:
        virtual void __initialize() = 0;

        void __ensure_init()
        {
            std::call_once(__once_flag, &initializer_t::__do_init, this);
        }

    private:
        std::once_flag __once_flag;
        void __do_init();
    };

    ////////// ////////// ////////// ////////// //////////

    // A Semaphore to provide a number of signals to control the concurrency.

    struct semaphore_t
    {
        typedef uint32_t signal_count_t;
        semaphore_t(signal_count_t signal_count=1);
        ~semaphore_t();

        // Acquire a signal.
        template<typename _duration_t>
        bool acquire(const _duration_t & timeout)
        {
            _UL(__mutex, guard);

            if (__try_acquire())
                return true;

            std::unique_lock<std::mutex> lk(__wait_mutex);
            __waiting_count++;

            guard.unlock();

            // TODO: not a atomic operation
            bool success = true;
            if (timeout == _duration_t::zero())
                __condition_var.wait(lk);
            else
                success = __condition_var.wait_for (lk, timeout) == std::cv_status::no_timeout;

            _L(__mutex);
            if (success)
                __signal_count--;

            __waiting_count--;
            return success;
        }

        // Acquire a signal.
        bool acquire();

        // Try to acquire a signal.
        bool try_acquire();

        // Release a signal.
        void release();

    private:
        const signal_count_t __max_signal_count;
        volatile signal_count_t __signal_count;
        volatile signal_count_t __waiting_count = 0;
        std::recursive_mutex __mutex;

        // Try to acquire a signal.
        bool __try_acquire();

        std::condition_variable __condition_var;
        std::mutex __wait_mutex;
    };

    ////////// ////////// ////////// ////////// //////////

    struct auto_reset_event_t
    {

    private:


    };

    ////////// ////////// ////////// ////////// //////////

    template<typename t> class respool_t;
    typedef std::mutex __default_object_lock_mutex_t;

    // Management of object locks.
    template<typename mutex_t=__default_object_lock_mutex_t>
    class object_lock_manager_t
    {
        struct __mutex_wrap_t;
        typedef object_lock_manager_t<mutex_t> __self_t;
        typedef uint_type_t<sizeof(void *)> __key_t;
        typedef respool_t<__mutex_wrap_t> __respool_t;

    public:
        // Acquire the lock for the owner.
        void enter(void * owner)
        {
            _UL(__mutex, guard);

            __mutex_wrap_t & mutex_wrap = __get_mutex_wrap(owner, true);
            mutex_wrap.ref_count++;

            unlock_and_lock(guard, mutex_wrap.mutex);
        }

        // Release the lock for the owner.
        void exit(void * owner)
        {
            _L(__mutex);

            __mutex_wrap_t & mutex_wrap = __get_mutex_wrap(owner);

            if (mutex_wrap.ref_count <= 0)
                throw _EC(invalid_operation, _T("exit too many times"));

            if (--mutex_wrap.ref_count == 0)
                __exited_count++;

            mutex_wrap.mutex.unlock();
        }

        // The sigleton instance.
        static __self_t * instance()
        {
            static __self_t * lm = new __self_t();
            return lm;
        }

    private:

        // The mutex object wrapper.
        struct __mutex_wrap_t : public object_t
        {
            __mutex_wrap_t() { }
            __mutex_wrap_t(const __mutex_wrap_t &) = delete;
            __mutex_wrap_t(__mutex_wrap_t &&) = delete;

            mutex_t           mutex;
            volatile int32_t  ref_count = 0;

            X_TO_STRING_IMPL(_T("__mutex_wrap_t"))
        };

        std::map<__key_t, __mutex_wrap_t *> __mutex_map;
        volatile int32_t __exited_count = 0;
        std::mutex __mutex;
        __respool_t __pool;

        // Returns the mutex wrapper for the owner.
        // Auto create if not exists. ( when auto_create is true. )
        __mutex_wrap_t & __get_mutex_wrap(void * owner, bool auto_create=false)
        {
            if (__exited_count >= 10)
            {
                __clean_up();
                __exited_count = 0;
            }

            __key_t key = (__key_t)owner;
            auto it = __mutex_map.find(key);
            if (it != __mutex_map.end())
                return *it->second;
            
            if (!auto_create)
                throw _EC(invalid_operation, _T("object locker not exists"));

            return *(__mutex_map[key] = __pool.acquire());
        }

        // Cleanup locks where ref_count is 0.
        void __clean_up()
        {
            std::map<__key_t, __mutex_wrap_t *> new_map;
            for (auto &it : __mutex_map)
            {
                if (it.second->ref_count == 0)
                    __pool.release(it.second);
                else
                    new_map.insert(it);
            }

            std::swap(new_map, __mutex_map);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Object locker wrapper. implemenation of lock / unlock.
    template<typename mutex_t = __default_object_lock_mutex_t>
    struct __object_locker_wrapper_t
    {
    private:
        typedef __object_locker_wrapper_t<mutex_t> __self_t;

    public:
        __object_locker_wrapper_t(object_lock_manager_t<mutex_t> & lm, void * owner)
            : __lm(lm), __owner(owner) { }

        __object_locker_wrapper_t(const __self_t & obj) = delete;

        __object_locker_wrapper_t(__self_t && obj)
            : __lm(obj.__lm), __owner(obj.__owner)
        {
            obj.__owner = nullptr;
        }

        // Lock
        void lock()
        {
            __lm.enter(__owner);
        }

        // Unlock
        void unlock()
        {
            if (__owner)
                __lm.exit(__owner);
        }

    private:
        object_lock_manager_t<mutex_t> & __lm;
        void * __owner;
    };
    
    //-------- ---------- ---------- ---------- ----------

    template<
        typename mutex_t = __default_object_lock_mutex_t,
        typename lock_impl_t = ::std::lock_guard<__object_locker_wrapper_t<mutex_t>>
    >
    class __object_lock_impl_t
    {
        typedef object_lock_manager_t<mutex_t> __lock_manager_t;

    public:
        template<typename ... args_t>
        __object_lock_impl_t(__lock_manager_t & lock_manager, void * owner, args_t ... args)
            : __w(lock_manager, owner), __impl(__w, args ...) { }

        template<typename ... args_t>
        __object_lock_impl_t(void * owner, args_t ... args)
            : __object_lock_impl_t(*__lock_manager_t::instance(), owner, args ...) { }

    private:
        __object_locker_wrapper_t<mutex_t> __w;
        lock_impl_t __impl;
    };

    // Object lock guard.
    template<typename _mutex_t = __default_object_lock_mutex_t>
    using object_lock_guard_t = __object_lock_impl_t<
        _mutex_t, ::std::lock_guard<__object_locker_wrapper_t<_mutex_t>>
    >;

    ////////// ////////// ////////// ////////// //////////

    // Provides the ability to lazy initializing.
    template<typename t>
    class lazy_t
    {
        typedef lazy_t<t> __self_t;
        typedef std::function<t()> __init_func_t;

    public:
        template<typename init_func_t>
        lazy_t(init_func_t func) : __init_func(func) { }

        // Returns value, do initialize at the first time.
        operator t()
        {
            std::call_once(__once_flag, &__self_t::__initialize, this);
            return __value;
        }

    private:
        t __value;
        std::once_flag __once_flag;
        __init_func_t __init_func;

        // Do init it.
        void __initialize()
        {
            __value = __init_func();
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Provides the ability to lazy initializing. ( for pointer. )

    template<typename t>
    class lazy_t<t *>
    {
        typedef lazy_t<t *> __self_t;
        typedef std::function<t*()> __init_func_t;

    public:
        template<typename init_func_t>
        lazy_t(init_func_t func) : __init_func(func) { }

        // Returns value, do initialize at the first time.
        operator t*()
        {
            std::call_once(__once_flag, &__self_t::__initialize, this);
            return __value;
        }

        t * operator -> ()
        {
            return (t *)*this;
        }

        // Returns if two objects are equals.
        bool operator == (const t * other)
        {
            return (t *)*this == other;
        }

        // Returns if two objects are not equals.
        bool operator != (const t * other)
        {
            return ! operator == (other);
        }

    private:
        t * __value;
        std::once_flag __once_flag;
        __init_func_t __init_func;

        // Do init it.
        void __initialize()
        {
            __value = __init_func();
        }
    };

    ////////// ////////// ////////// ////////// //////////

}   // X_ROOT_NS::algorithm

#endif // __ALGORITHM_THREAD_H__
