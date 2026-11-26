
#ifndef __GC_SPACE_H__
#define __GC_SPACE_H__

// The space of the garbage collector: the memory, the marking and the sweeping, with no knowledge of the types. The
// interpreter (mm/gc_heap.cpp: it knows the types of the objects, the execution stacks) and the native programs
// (native/native_gc.cpp: a conservative collector) both are made on it. It does not include anything of the project, so
// that the native runtime can use it as it is.
//
// The memory is a region that is reserved at the beginning (the address space only) and committed by blocks of 64 KB:
//
//      * the block of an address is its number (address - base) >> 16, so to find what an address points to is a read of
//        a table, not a search;
//      * a small object (a cell up to 8 KB) is in a block of its size class (and of its kind: a block of atomic cells holds
//        objects that have no references, they are marked and never scanned);
//      * a large object has a run of blocks, only the pages it needs are committed;
//      * the memory of the blocks that are free is given back to the system (a few are kept, to be used at once).
//
// The collector does not move objects. It is stop-the-world: the one thread that allocates collects (the threads of the
// interpreter are stopped by the lock of the threads, the native programs stop them with the hooks).
//
// The sweep reads the byte map of a block (an entry per cell), not the cells: the cells that are free are found when they
// are allocated (a cursor per block).

#include <cstddef>
#include <cstdint>
#include <map>
#include <unordered_set>
#include <vector>

namespace april_gc {

    typedef unsigned char byte_t;

    ////////// ////////// ////////// ////////// //////////
    // The layout of the heap.

    // A block: the unit that is committed and that is given back.
    constexpr size_t block_size = 64 * 1024;

    // The grain of the allocation: the cells are multiples of it.
    constexpr size_t granule_size = 16;

    constexpr size_t granules_per_block = block_size / granule_size;

    // A cell is larger than this: it is a large object.
    constexpr size_t max_cell_size = 8 * 1024;

    // An entry of the byte map: the bit 7 is the mark; the bits 0-6 are 0 for a cell that is free, else 1 + the size of the
    // extern area (the type and the lengths of an array, before the object) divided by 8.
    constexpr byte_t mark_mask   = 0x80;
    constexpr byte_t used_mask   = 0x7F;

    constexpr size_t max_small_extern = 125 * 8;

    // The sizes of the cells.
    constexpr uint16_t cell_sizes[] = {
        16,   32,   48,   64,   80,   96,   112,  128,
        160,  192,  224,  256,
        320,  384,  448,  512,
        640,  768,  896,  1024,
        1280, 1536, 1792, 2048,
        2560, 3072, 3584, 4096,
        5120, 6144, 7168, 8192,
    };

    constexpr size_t size_class_count = sizeof(cell_sizes) / sizeof(cell_sizes[0]);

    static_assert(cell_sizes[size_class_count - 1] == max_cell_size, "the last class is the largest cell");
    static_assert(sizeof(void *) == 8, "the collector supposes a 64 bits platform");

    ////////// ////////// ////////// ////////// //////////

    // The parameters of a space.
    struct space_params_t
    {
        // The bytes allocated before the first collection, also the floor of the threshold.
        size_t  initial_threshold   = 16 * 1024 * 1024;

        // The next threshold is the live bytes times this (but not less than the initial one).
        double  growth_factor       = 2.0;

        // Collects at every allocation.
        bool    stress              = false;

        // One line per collection to stderr.
        bool    trace               = false;

        // The cells that are freed are filled with 0xDD.
        bool    poison              = false;

        // A summary when the space ends.
        bool    stats               = false;

        // The address space that is reserved (0: 32 GB, or less when the system does not give it).
        size_t  reserve_bytes       = 0;
    };

    // A block of small objects: it is at the beginning of the memory of the block.
    struct block_t
    {
        static constexpr uint32_t magic_value = 0x6370414C;

        uint32_t        magic;
        uint16_t        size_class;
        bool            atomic;             // The cells hold no references.
        uint32_t        cell_size;
        uint32_t        cell_count;
        uint32_t        first_cell_granule; // Where the first cell is.
        uint64_t        reciprocal;         // 2^32 / (the granules of a cell) + 1 (more than 32 bits for a cell of one granule):
                                            // the index of a cell is a product, not a division.
        uint32_t        cursor;             // The next cell to look at, when something is allocated.
        uint32_t        zero_from;          // The cells from this one were never used since the block was committed: they are 0.
        uint32_t        mark_count;         // The cells marked in the collection.
        block_t *       next;               // The next block that has free cells (of the same class).

        byte_t          byte_map[granules_per_block];

        byte_t * base() { return (byte_t *)this; }

        byte_t * cell_at(size_t index) { return base() + granule_of(index) * granule_size; }

        size_t granule_of(size_t index) { return first_cell_granule + index * (cell_size / granule_size); }

        // The index of the cell that a granule is in (exact for the granules of a block).
        size_t index_of(size_t granule) { return (size_t)(((uint64_t)(granule - first_cell_granule) * reciprocal) >> 32); }

        void initialize(uint16_t size_class_, uint32_t cell_size_, bool atomic_, bool dirty);
    };

    // The header of a large object: it is at the beginning of its first block.
    struct large_t
    {
        size_t          total_size;         // The extern area and the payload.
        uint32_t        extern_size;
        uint32_t        block_count;
        bool            marked;
        bool            atomic;

        static constexpr size_t header_size = 32;

        byte_t * payload() { return (byte_t *)this + header_size; }
    };

    static_assert(sizeof(large_t) <= large_t::header_size, "the header of a large object");

    ////////// ////////// ////////// ////////// //////////

    class gc_space_t
    {
    public:

        explicit gc_space_t(const space_params_t & params);
        virtual ~gc_space_t();

        gc_space_t(const gc_space_t &) = delete;
        gc_space_t & operator = (const gc_space_t &) = delete;

        // Allocates an object: the extern area and the payload, the memory is 0. Returns the address of the object, that is
        // `extern_size` after the beginning of the cell (it is inside the cell, also for an object that has no payload).
        // An atomic object holds no references: it is marked, never scanned.
        void * allocate(size_t extern_size, size_t size, bool atomic);

        // Collects, the registers are saved first so that the references that are held in them are seen. Returns the bytes
        // that were freed.
        size_t collect_here();

        // The weak references: a handle that does not keep the object alive, it is 0 when the object is freed. A handle is
        // not 0.
        uint64_t weak_new(void * object);
        void *   weak_get(uint64_t handle);
        void     weak_free(uint64_t handle);

        // The objects that have a finalizer: when one is not reachable it is not freed, it is queued (take_finalizable), and it
        // is freed by the collection that follows its finalizer.
        void   register_finalizable(void * object);
        bool   unregister_finalizable(void * object);
        void * take_finalizable();
        bool   has_finalizable() const { return !__fin_queue.empty(); }

        // The bytes in use (the objects, the garbage that is not collected yet included) and the number of collections.
        size_t memory_in_use() const { return __in_use; }
        size_t collection_total() const { return __collections; }
        size_t total_pause_us() const { return __total_pause_us; }
        size_t max_pause_us() const { return __max_pause_us; }
        size_t live_bytes() const { return __live_bytes; }

        // Whether the address is in an object of the space, and where the object is.
        struct obj_t
        {
            byte_t *        cell;           // The beginning of the object with its extern area.
            size_t          size;           // The size of the cell, or of the large object.
            uint32_t        extern_size;
            bool            atomic;
            byte_t *        mark_byte;      // The entry of the byte map (a small object), or nullptr.
            large_t *       large;          // The header (a large object), or nullptr.
            block_t *       block;
        };

        bool locate(void * p, obj_t & out);

    protected:

        // The roots of a collection: calls mark_candidate / scan_range for each one.
        virtual void mark_roots(void ** native_stack_top) = 0;

        // Scans the references of an object that was marked (it is not called for an atomic object): the default is
        // all the words of the payload.
        virtual void scan_object(const obj_t & obj);

        // After the marking, before the sweep: the collection of the weak references and of the finalizers is done by now.
        virtual void on_marked() { }

        // After a collection.
        virtual void on_collected() { }

        // A collection that the allocation asks for (the native programs stop the other threads first).
        virtual size_t request_collect() { return collect_here(); }

        // The memory is not enough (the space is full, or the system says no).
        virtual void out_of_memory();

        // Marks the object that the word points to, if any, and queues it to be scanned.
        void mark_candidate(void * p);

        // Reads the words of a range, `unaligned`: the words can be at any multiple of 4 bytes.
        void scan_range(void ** begin, void ** end, bool unaligned = false);

        // Scans the queued objects until there are none.
        void process_mark_stack();

        // Whether the object is marked (an address that is not in the space is).
        bool is_marked(void * p);

        // The high bound of the stack of the thread.
        static void ** native_stack_high();

        const space_params_t &  params() const { return __params; }

    private:

        // The collection, with the approximate top of the native stack.
        size_t __collect(void ** native_stack_top);

        size_t __sweep();

        // The memory.
        bool     __reserve(size_t bytes);
        bool     __commit(void * address, size_t bytes);
        void     __decommit(void * address, size_t bytes);
        size_t   __alloc_run(size_t count);
        void     __release_run(size_t start, size_t count);

        byte_t * __block_address(size_t index) { return __base + index * block_size; }

        block_t * __new_block(size_t size_class, bool atomic);
        void      __release_block(size_t index);
        void *    __alloc_cell(size_t size_class, bool atomic, byte_t entry);
        void *    __alloc_large(size_t extern_size, size_t size, bool atomic);
        void      __free_large(size_t index);

        size_t __size_class_of(size_t cell_size) { return __class_map[(cell_size - 1) / granule_size]; }

        space_params_t      __params;

        // The region.
        byte_t *            __base = nullptr;
        size_t              __capacity = 0;         // The blocks that the region has.
        size_t              __top = 0;              // The blocks from here were never used (or are given back).
        enum : byte_t { __k_free = 0, __k_small = 1, __k_large_head = 2, __k_large_cont = 3 };
        std::vector<byte_t>     __kind;             // Per block.
        std::vector<uint32_t>   __aux;              // The blocks of a large object, or the distance to its head.
        std::map<size_t, size_t> __free_runs;       // The blocks that are given back: start -> count.
        std::vector<size_t>     __warm;             // Free blocks that are still committed.
        std::vector<size_t>     __small_blocks;     // The numbers of the blocks of small objects.
        std::vector<size_t>     __large_heads;      // The numbers of the first blocks of the large objects.
        void *                  __reservation = nullptr;

        // The cells.
        block_t *           __current[2][size_class_count];     // The block that is allocated from.
        block_t *           __head[2][size_class_count];        // The blocks that have free cells.
        byte_t              __class_map[max_cell_size / granule_size];

        // The marking.
        struct __queued_t { byte_t * cell; size_t size; uint32_t extern_size; };
        std::vector<__queued_t> __mark_stack;

        // The weak references and the finalizers.
        std::vector<uintptr_t>          __weaks;        // Handle - 1 -> the address (hidden from the scan), 0 is free.
        std::vector<uint64_t>           __free_weaks;
        std::unordered_set<uintptr_t>   __fin_registry;
        std::vector<uintptr_t>          __fin_queue;    // Reachable until the finalizer ran.

        size_t              __allocated_since_gc = 0;
        size_t              __threshold;
        size_t              __live_bytes = 0;
        size_t              __in_use = 0;

        size_t              __collections = 0;
        size_t              __total_pause_us = 0;
        size_t              __total_mark_us = 0;
        size_t              __total_sweep_us = 0;
        size_t              __max_pause_us = 0;
        size_t              __total_collected = 0;

    protected:

        // The statistics of the last collection.
        size_t              scanned_bytes_ = 0;
        size_t              skipped_bytes_ = 0;

    public:

        size_t scanned_bytes() const { return scanned_bytes_; }
        size_t skipped_bytes() const { return skipped_bytes_; }
    };

}   // namespace april_gc

#endif  // __GC_SPACE_H__
