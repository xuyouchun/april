#ifndef __NATIVE_GC_H__
#define __NATIVE_GC_H__

// The garbage collector of native programs (see native_gc.cpp). Standard library only.

#include <cstddef>
#include <cstdint>

namespace april_native_gc {

    // Initializes the collector. The stack is scanned up to `stack_base`, an address in
    // the frame of `main`.
    void init(void * stack_base);

    // The stack of a thread of the program. The threads run one at a time: the one that does not run is stopped at a place
    // that is known, with its registers and the top of its stack kept here, and the collector scans them.
    struct thread_record_t;

    // A thread begins (it is the running one): its stack is scanned up to `stack_base`.
    // `exception_slot` is the variable of the exception that is pending in the thread (a root).
    thread_record_t * attach_thread(void * stack_base, void ** exception_slot);

    // The collector asks the others to stop, and lets them go on (the threads that run in parallel): called around a collection.
    void set_world_hooks(void (* stop)(), void (* resume)());

    // The thread ended.
    void detach_thread(thread_record_t * record);

    // The collector asks the roots that it does not know (the objects of the threads): the function marks each by `mark`.
    typedef void (* root_provider_t)(void (* mark)(void * address));
    void set_root_provider(root_provider_t provider);

    // The thread stops (it gives its turn, or it waits): keeps its registers and the top of its stack.
    void thread_stopping(thread_record_t * record);

    // The thread runs again.
    void thread_resumed(thread_record_t * record);

    // Prints the statistics if they were asked (APRIL_GC_STATS).
    void shutdown();

    // Registers a global that holds an address: it is scanned at every collection.
    void add_root(void ** slot);

    // System.GC and WeakReference: a collection now, the bytes in use, the number of collections; a weak reference (the
    // handle is not 0, the object is nullptr when it was freed).
    size_t   collect();
    size_t   total_memory();
    size_t   collection_count();
    uint64_t weak_new(void * object);
    void *   weak_get(uint64_t handle);
    void     weak_free(uint64_t handle);

    // Allocates a zeroed block of `size` bytes, which lives while an address inside it
    // can be found from the roots. `has_refs` is false for blocks that hold no addresses.
    // May collect.
    void * alloc(size_t size, bool has_refs);

}   // namespace april_native_gc

#endif  // __NATIVE_GC_H__
