
#include "native_gc.h"

#include <gc_space.h>

#include <csetjmp>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

// The garbage collector of native programs: a conservative, non moving mark and sweep, made on the space of the collector
// (include/modules/gc_space.h: the memory, the marking and the sweeping, that the interpreter uses too).
//
// A word that points into an object (also in its middle: the program holds the addresses of fields and elements) keeps it
// alive; the words of the live objects are scanned the same way, so no knowledge of the types is needed (an object that
// holds no addresses is atomic: it is never scanned). The roots are the native stack (and the registers, saved by setjmp) and
// the registered globals.
//
// Environment variables, for debugging and tests:
//
//      APRIL_GC_STRESS=1       collects at every allocation
//      APRIL_GC_THRESHOLD=n    collects when n bytes were allocated since the last time
//      APRIL_GC_TRACE=1        prints a line for every collection
//      APRIL_GC_STATS=1        prints a summary when the program ends
//      APRIL_GC_POISON=1       fills the cells that are freed with 0xDD: a live object that was freed by mistake shows up at
//                              once, not by the luck of the heap

namespace april_native_gc {

    struct thread_record_t
    {
        uintptr_t       base = 0;               // The highest address of the stack that has frames.
        uintptr_t       sp = 0;                 // The top of the stack when the thread stopped.
        std::jmp_buf    registers;
        void **         exception_slot = nullptr;   // The variable of the exception that is pending in the thread.
        bool            stopped = false;        // It is created by the thread that runs.
    };

    namespace
    {
        bool __env_flag(const char * name)
        {
            const char * value = std::getenv(name);
            return value != nullptr && *value == '1';
        }

        class __space_t;

        std::vector<void **>            __roots;
        std::vector<thread_record_t *>  __threads;
        root_provider_t                 __root_provider = nullptr;
        void (* __stop_world)() = nullptr;
        void (* __resume_world)() = nullptr;
        thread_local thread_record_t *  __running_thread = nullptr;

        uintptr_t   __stack_base = 0;
        size_t      __peak_bytes = 0;
        __space_t * __space = nullptr;
        bool        __stats = false;

        class __space_t : public april_gc::gc_space_t
        {
        public:
            explicit __space_t(const april_gc::space_params_t & params) : april_gc::gc_space_t(params) { }

            // Marks the object that the address points to, if any.
            void mark_word(uintptr_t word) { mark_candidate((void *)word); }

            // A collection now (the threads that run in parallel stop first).
            size_t collect_now() { return request_collect(); }

        protected:

            // The roots: the stack of the thread that runs, the stacks of the threads that stopped, their pending
            // exceptions, the objects of the threads, the globals.
            virtual void mark_roots(void ** native_stack_top) override
            {
                // The stack of the running thread, up to its base (the registers are on it: collect_here saved them
                // below the frames of the callers).
                uintptr_t base = __running_thread != nullptr? __running_thread->base : __stack_base;
                scan_range(native_stack_top, (void **)base);

                for (thread_record_t * record : __threads)
                {
                    if (record == __running_thread || !record->stopped)
                        continue;

                    scan_range((void **)&record->registers, (void **)((char *)&record->registers + sizeof(record->registers)));
                    scan_range((void **)record->sp, (void **)record->base);

                    if (record->exception_slot != nullptr)
                        mark_candidate(*record->exception_slot);
                }

                if (__running_thread != nullptr && __running_thread->exception_slot != nullptr)
                    mark_candidate(*__running_thread->exception_slot);

                if (__root_provider != nullptr)
                    __root_provider([](void * address) { __space->mark_word((uintptr_t)address); });

                for (void ** slot : __roots)
                    mark_candidate(*slot);
            }

            // The elements of an array of structs are as large as the struct (12 bytes for a reference and an Int32), so a
            // reference is not always at a multiple of 8: the words of an object are read by steps of 4.
            virtual void scan_object(const obj_t & obj) override
            {
                scan_range((void **)(obj.cell + obj.extern_size), (void **)(obj.cell + obj.size), true);
            }

            // The threads that run in parallel stop first.
            virtual size_t request_collect() override
            {
                if (__stack_base == 0)
                    return 0;                   // Not initialized.

                if (__stop_world != nullptr)
                    __stop_world();

                size_t collected = collect_here();

                if (__resume_world != nullptr)
                    __resume_world();

                return collected;
            }

            virtual void out_of_memory() override
            {
                std::fputs("native: out of memory\n", stderr);
                std::abort();
            }
        };
    }

    void init(void * stack_base)
    {
        __stack_base = (uintptr_t)stack_base;

        april_gc::space_params_t params;
        params.initial_threshold = 8u << 20;
        params.stress   = __env_flag("APRIL_GC_STRESS");
        params.trace    = __env_flag("APRIL_GC_TRACE");
        params.poison   = __env_flag("APRIL_GC_POISON");
        __stats         = __env_flag("APRIL_GC_STATS");

        if (const char * value = std::getenv("APRIL_GC_THRESHOLD"))
        {
            if (*value != 0)
                params.initial_threshold = (size_t)std::strtoull(value, nullptr, 10);
        }

        __space = new __space_t(params);
    }

    void set_root_provider(root_provider_t provider)
    {
        __root_provider = provider;
    }

    void set_world_hooks(void (* stop)(), void (* resume)())
    {
        __stop_world = stop;
        __resume_world = resume;
    }

    thread_record_t * attach_thread(void * stack_base, void ** exception_slot)
    {
        thread_record_t * record = new thread_record_t();
        record->exception_slot = exception_slot;
        record->base = (uintptr_t)stack_base;
        record->sp = record->base;

        __threads.push_back(record);
        __running_thread = record;

        return record;
    }

    void detach_thread(thread_record_t * record)
    {
        for (auto it = __threads.begin(); it != __threads.end(); ++it)
        {
            if (*it == record)
            {
                __threads.erase(it);
                break;
            }
        }

        if (__running_thread == record)
            __running_thread = nullptr;

        delete record;
    }

    // The registers are saved below the frames of the callers.
    __attribute__((noinline)) void thread_stopping(thread_record_t * record)
    {
        setjmp(record->registers);

        volatile char marker = 0;
        record->sp = (uintptr_t)&marker;
        record->stopped = true;
    }

    void thread_resumed(thread_record_t * record)
    {
        record->stopped = false;
    }

    void shutdown()
    {
        if (__stats && __space != nullptr)
        {
            std::fprintf(stderr, "native gc: collections=%llu peak=%llu live=%llu\n",
                         (unsigned long long)__space->collection_total(), (unsigned long long)__peak_bytes,
                         (unsigned long long)__space->live_bytes());
        }
    }

    void add_root(void ** slot)
    {
        __roots.push_back(slot);
    }

    size_t collect()
    {
        return __space->collect_now();
    }

    size_t total_memory()
    {
        return __space->memory_in_use();
    }

    size_t collection_count()
    {
        return __space->collection_total();
    }

    uint64_t weak_new(void * object)
    {
        return __space->weak_new(object);
    }

    void * weak_get(uint64_t handle)
    {
        return __space->weak_get(handle);
    }

    void weak_free(uint64_t handle)
    {
        __space->weak_free(handle);
    }

    void * alloc(size_t size, bool has_refs)
    {
        void * p = __space->allocate(0, size, !has_refs);

        size_t used = __space->memory_in_use();
        if (used > __peak_bytes)
            __peak_bytes = used;

        return p;
    }

}   // namespace april_native_gc
