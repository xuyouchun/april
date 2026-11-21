#ifndef __NATIVE_GC_H__
#define __NATIVE_GC_H__

// The garbage collector of native programs (see native_gc.cpp). Standard library only.

#include <cstddef>

namespace april_native_gc {

    // Initializes the collector. The stack is scanned up to `stack_base`, an address in
    // the frame of `main`.
    void init(void * stack_base);

    // The stack of a thread of the program. The threads run one at a time: the one that does not run is stopped at a place
    // that is known, with its registers and the top of its stack kept here, and the collector scans them.
    struct thread_record_t;

    // A thread begins (it is the running one): its stack is scanned up to `stack_base`.
    thread_record_t * attach_thread(void * stack_base);

    // The thread ended.
    void detach_thread(thread_record_t * record);

    // The collector asks the roots that it does not know (the objects of the threads): the function marks each by `mark`.
    typedef void (* root_provider_t)(void (* mark)(void * address));
    void set_root_provider(root_provider_t provider);

    // The thread stops (it gives its turn, or it waits): keeps its registers and the top of its stack. `exception` is the
    // exception that is pending, it is a root while the thread is stopped.
    void thread_stopping(thread_record_t * record, void * exception);

    // The thread runs again: returns the exception that was kept.
    void * thread_resumed(thread_record_t * record);

    // Prints the statistics if they were asked (APRIL_GC_STATS).
    void shutdown();

    // Registers a global that holds an address: it is scanned at every collection.
    void add_root(void ** slot);

    // Allocates a zeroed block of `size` bytes, which lives while an address inside it
    // can be found from the roots. `has_refs` is false for blocks that hold no addresses.
    // May collect.
    void * alloc(size_t size, bool has_refs);

}   // namespace april_native_gc

#endif  // __NATIVE_GC_H__
