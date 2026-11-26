
#include <rtlib.h>

namespace X_ROOT_NS::modules::rtlib {

    ////////// ////////// ////////// ////////// //////////
    // The garbage collector: System.GC and WeakReference. The back end gives the services (the interpreter, a native
    // program), the functions do nothing without them.

    namespace
    {
        gc_services_t   __services;
    }

    // Sets the services (nullptr: none).
    void set_gc_services(const gc_services_t * services)
    {
        __services = services != nullptr? *services : gc_services_t();
    }

    // GC_Collect().
    void gc_collect(rtlib_context_t & ctx)
    {
        if (__services.collect != nullptr)
            __services.collect();
    }

    // GC_CollectionCount(): the number of collections.
    void gc_collection_count(rtlib_context_t & ctx)
    {
        ctx.return_<int32_t>(__services.collection_count != nullptr? (int32_t)__services.collection_count() : 0);
    }

    // GC_TotalMemory(): the bytes of the objects that are in use.
    void gc_total_memory(rtlib_context_t & ctx)
    {
        ctx.return_<int64_t>(__services.total_memory != nullptr? (int64_t)__services.total_memory() : 0);
    }

    // GC_WeakNew(object): a handle of a weak reference to the object (0 for null).
    void gc_weak_new(rtlib_context_t & ctx)
    {
        struct args_t { robject_t object; };
        void * object = (void *)(rt_ref_t)ctx.args<args_t>()->object;

        ctx.return_<int64_t>(object != nullptr && __services.weak_new != nullptr?
                                (int64_t)__services.weak_new(object) : 0);
    }

    // GC_WeakGet(handle): the object, null when it was collected.
    void gc_weak_get(rtlib_context_t & ctx)
    {
        struct args_t { rint64_t handle; };
        int64_t handle = *ctx.args<args_t>()->handle;

        ctx.return_<void *>(__services.weak_get != nullptr? __services.weak_get((uint64_t)handle) : nullptr);
    }

    // GC_WeakFree(handle).
    void gc_weak_free(rtlib_context_t & ctx)
    {
        struct args_t { rint64_t handle; };
        int64_t handle = *ctx.args<args_t>()->handle;

        if (__services.weak_free != nullptr)
            __services.weak_free((uint64_t)handle);
    }

    // GC_SuppressFinalize(object, suppress).
    void gc_suppress_finalize(rtlib_context_t & ctx)
    {
        struct args_t { robject_t object; rbool_t suppress; };
        args_t * args = ctx.args<args_t>();

        void * object = (void *)(rt_ref_t)args->object;
        if (object != nullptr && __services.suppress_finalize != nullptr)
            __services.suppress_finalize(object, *args->suppress);
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::rtlib
