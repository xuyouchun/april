#ifndef __NATIVE_GC_H__
#define __NATIVE_GC_H__

// The garbage collector of native programs (see native_gc.cpp). Standard library only.

#include <cstddef>

namespace april_native_gc {

    // Initializes the collector. The stack is scanned up to `stack_base`, an address in
    // the frame of `main`.
    void init(void * stack_base);

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
