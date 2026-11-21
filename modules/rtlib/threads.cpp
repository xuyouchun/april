#include <rtlib.h>
#include <thread_manager.h>

namespace X_ROOT_NS::modules::rtlib {

    ////////// ////////// ////////// ////////// //////////

    // The functions of System.Threading: the threads are in thread_manager.cpp.

    namespace
    {
        struct __object_args_t
        {
            robject_t   obj;
        };

        struct __object_int32_args_t
        {
            robject_t   obj;
            rint32_t    value;
        };

        struct __object_bool_args_t
        {
            robject_t   obj;
            rbool_t     value;
        };

        struct __id_args_t
        {
            rint64_t    id;
        };

        struct __id_int32_args_t
        {
            rint64_t    id;
            rint32_t    value;
        };

        struct __id_bool_args_t
        {
            rint64_t    id;
            rbool_t     value;
        };

        struct __int32_args_t
        {
            rint32_t    value;
        };

        static void * __object_of(robject_t & obj)
        {
            return (void *)(rt_ref_t)obj;
        }

        static thread_manager_t & __manager()
        {
            return thread_manager_t::instance();
        }
    }

    // Thread_Start(thread, background): starts the thread, returns its id.
    void thread_start(rtlib_context_t & ctx)
    {
        __object_bool_args_t * args = ctx.args<__object_bool_args_t>();
        thread_t * thread = __manager().start(__object_of(args->obj), *args->value);

        ctx.return_<int64_t>(thread->id());
    }

    // Thread_Join(id, milliseconds): waits for the end of the thread, returns whether it ended.
    void thread_join(rtlib_context_t & ctx)
    {
        __id_int32_args_t * args = ctx.args<__id_int32_args_t>();

        thread_t * thread = __manager().find(*args->id);
        bool ended = thread == nullptr || __manager().join(thread, *args->value);

        ctx.return_<bool>(ended);
    }

    // Thread_Sleep(milliseconds).
    void thread_sleep(rtlib_context_t & ctx)
    {
        __manager().sleep(*ctx.args<__int32_args_t>()->value);
    }

    // Thread_Yield().
    void thread_yield(rtlib_context_t & ctx)
    {
        __manager().yield();
    }

    // Thread_CurrentId(): the id of the running thread.
    void thread_current_id(rtlib_context_t & ctx)
    {
        thread_t * thread = __manager().current();
        ctx.return_<int64_t>(thread != nullptr? thread->id() : 1);
    }

    // Thread_GetCurrent(): the object of the running thread, null for the one that started the program.
    void thread_get_current(rtlib_context_t & ctx)
    {
        thread_t * thread = __manager().current();
        ctx.return_<void *>(thread != nullptr? thread->entry() : nullptr);
    }

    // Thread_IsAlive(id).
    void thread_is_alive(rtlib_context_t & ctx)
    {
        thread_t * thread = __manager().find(*ctx.args<__id_args_t>()->id);
        ctx.return_<bool>(thread != nullptr && !thread->is_finished());
    }

    // Thread_SetBackground(id, background).
    void thread_set_background(rtlib_context_t & ctx)
    {
        __id_bool_args_t * args = ctx.args<__id_bool_args_t>();

        if (thread_t * thread = __manager().find(*args->id))
            __manager().set_background(thread, *args->value);
    }

    // Thread_IsBackground(id).
    void thread_is_background(rtlib_context_t & ctx)
    {
        thread_t * thread = __manager().find(*ctx.args<__id_args_t>()->id);
        ctx.return_<bool>(thread != nullptr && thread->is_background());
    }

    ////////// ////////// ////////// ////////// //////////

    // Monitor_Enter(obj).
    void monitor_enter(rtlib_context_t & ctx)
    {
        __manager().monitor_enter(__object_of(ctx.args<__object_args_t>()->obj));
    }

    // Monitor_TryEnter(obj, milliseconds): returns whether the lock was taken.
    void monitor_try_enter(rtlib_context_t & ctx)
    {
        __object_int32_args_t * args = ctx.args<__object_int32_args_t>();
        bool taken = __manager().monitor_try_enter(__object_of(args->obj), *args->value);

        ctx.return_<bool>(taken);
    }

    // Monitor_Exit(obj): returns 0, or 1 when the thread does not hold the lock.
    void monitor_exit(rtlib_context_t & ctx)
    {
        bool released = __manager().monitor_exit(__object_of(ctx.args<__object_args_t>()->obj));
        ctx.return_<int32_t>(released? 0 : 1);
    }

    // Monitor_Wait(obj, milliseconds): returns 0 (the thread does not hold the lock), 1 (pulsed), 2 (time over).
    void monitor_wait(rtlib_context_t & ctx)
    {
        __object_int32_args_t * args = ctx.args<__object_int32_args_t>();
        monitor_wait_result_t result = __manager().monitor_wait(__object_of(args->obj), *args->value);

        ctx.return_<int32_t>((int32_t)result);
    }

    // Monitor_Pulse(obj, all): returns whether the thread holds the lock.
    void monitor_pulse(rtlib_context_t & ctx)
    {
        __object_bool_args_t * args = ctx.args<__object_bool_args_t>();
        bool done = __manager().monitor_pulse(__object_of(args->obj), *args->value);

        ctx.return_<bool>(done);
    }

    // Monitor_IsEntered(obj).
    void monitor_is_entered(rtlib_context_t & ctx)
    {
        bool entered = __manager().monitor_is_entered(__object_of(ctx.args<__object_args_t>()->obj));
        ctx.return_<bool>(entered);
    }

}   // namespace X_ROOT_NS::modules::rtlib
