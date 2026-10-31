
#ifndef __MM_GC_HEAP_H__
#define __MM_GC_HEAP_H__

#include <mm.h>

#include <cstdlib>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace X_ROOT_NS::modules::mm {

    namespace
    {
        using namespace core;
        using namespace rt;
    }

    ////////// ////////// ////////// ////////// //////////
    // Heap layout constants.

    // Block: an aligned chunk carved into fixed-size cells of one size class.
    constexpr size_t __gc_block_size    = 64 * 1024;

    // Granule: the allocation grain; cells are multiples of it.
    constexpr size_t __gc_granule_size  = 16;

    // Granules per block (byte map length).
    constexpr size_t __gc_granules_per_block = __gc_block_size / __gc_granule_size;

    // Allocations with cell size above this go to the large object space.
    constexpr size_t __gc_max_cell_size = 8 * 1024;

    // Byte map entry: bit 7 is the mark bit, bits 0~6 encode extern_size / 8
    // of the cell starting at that granule; 0 means "not a cell start".
    constexpr byte_t __gc_mark_mask     = 0x80;
    constexpr byte_t __gc_extern_mask   = 0x7F;

    // Max extern size encodable in a byte map entry.
    constexpr size_t __gc_max_small_extern = (size_t)__gc_extern_mask * 8;

    // Cell sizes of each size class.
    constexpr uint16_t __gc_cell_sizes[] = {
        16,   32,   48,   64,   80,   96,   112,  128,
        160,  192,  224,  256,
        320,  384,  448,  512,
        640,  768,  896,  1024,
        1280, 1536, 1792, 2048,
        2560, 3072, 3584, 4096,
        5120, 6144, 7168, 8192,
    };

    constexpr size_t __gc_size_class_count =
                        sizeof(__gc_cell_sizes) / sizeof(__gc_cell_sizes[0]);

    static_assert(__gc_cell_sizes[__gc_size_class_count - 1] == __gc_max_cell_size,
                        "last size class must equal __gc_max_cell_size");

    static_assert(sizeof(void *) == 8, "gc heap supposes a 64bits platform");

    ////////// ////////// ////////// ////////// //////////
    // __gc_block_t

    // A 64KB aligned block carved into fixed-size cells.
    struct __gc_block_t
    {
        static constexpr uint32_t __magic_value = 0x6370'414C;  // "LApc"

        uint32_t        magic;              // Identifies a gc block.
        uint16_t        size_class;         // Size class index.
        uint32_t        cell_size;          // Cell size in bytes.
        uint32_t        cell_count;         // Count of cells in this block.
        uint32_t        first_cell_granule; // Granule index of the first cell.
        uint32_t        used_count;         // Cells in use (allocated).
        void *          free_list;          // Free cells chain, first word is next.
        __gc_block_t *  next_partial;       // Next block with free cells of same class.

        // Byte map, one entry per granule.
        byte_t          byte_map[__gc_granules_per_block];

        // Returns block base address.
        X_ALWAYS_INLINE byte_t * base() { return (byte_t *)this; }

        // Returns cell address of specified cell index.
        X_ALWAYS_INLINE byte_t * cell_at(size_t index)
        {
            return base() + (first_cell_granule + index * (cell_size / __gc_granule_size))
                                * __gc_granule_size;
        }

        // Returns granule index of specified cell index.
        X_ALWAYS_INLINE size_t granule_of(size_t index)
        {
            return first_cell_granule + index * (cell_size / __gc_granule_size);
        }

        // Initializes the block for the specified size class.
        void initialize(uint16_t size_class_, uint32_t cell_size_);

        // Takes a cell from the free list, returns nullptr when full.
        void * take_cell();

        // Returns the block that contains the specified pointer,
        // supposed it is a pointer into some block.
        X_ALWAYS_INLINE static void * block_base_of(void * p)
        {
            return (void *)((uintptr_t)p & ~(uintptr_t)(__gc_block_size - 1));
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // __gc_large_t

    // A large object, allocated separately with its own header.
    struct __gc_large_t
    {
        size_t          total_size;     // Payload size (extern area included).
        uint32_t        extern_size;    // Extern size (type info, array lengths).
        bool            marked;         // Mark flag.

        // Returns payload address (cell-equivalent, object data follows extern area).
        X_ALWAYS_INLINE byte_t * payload()
        {
            return (byte_t *)this + _alignf(sizeof(__gc_large_t), __gc_granule_size);
        }

        // Returns header address from payload address.
        X_ALWAYS_INLINE static __gc_large_t * from_payload(void * payload)
        {
            return (__gc_large_t *)(
                (byte_t *)payload - _alignf(sizeof(__gc_large_t), __gc_granule_size)
            );
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // gc_heap_t

    // Garbage collected runtime heap.
    // new_obj / new_array allocations are managed by a stop-the-world
    // mark-sweep collector: roots are scanned conservatively (execution
    // stacks of registered root sources and the native thread stack),
    // object payloads are traced conservatively as well.
    // alloc / free keep manual semantics, exactly like default_rt_heap_t.
    class gc_heap_t : public object_t, public rt_heap_t
    {
    public:

        // Constructor.
        gc_heap_t(const gc_params_t & params);

        // Destructor, releases all blocks and large objects.
        virtual ~gc_heap_t() override;

        // Creates a new object.
        virtual rt_ref_t new_obj(rt_type_t * type) override;

        // Creates a new array.
        virtual rt_ref_t new_array(rt_array_type_t * type, const array_length_t * lengths)
                                                   override;

        // Allocates a raw memory space (manual, not garbage collected).
        virtual void * alloc(size_t size, void * realloc = nullptr) override;

        // Frees a raw memory space.
        virtual void free(void * ptr) override;

        // Registers a gc root source.
        virtual void add_root_source(gc_root_source_t * source) override;

        // Removes a gc root source.
        virtual void remove_root_source(gc_root_source_t * source) override;

        // Sets the gc type resolver, clears the cached scan infos.
        virtual void set_gc_type_resolver(const gc_type_resolver_t & resolver) override;

        // Executes a garbage collection, returns bytes collected.
        virtual size_t collect() override;

        X_TO_STRING_IMPL(_T("gc_heap_t"))

    private:

        // A located gc object.
        struct __obj_t
        {
            byte_t *        cell;           // Cell start (small) / large payload start.
            size_t          size;           // Cell size (small) / payload size (large).
            uint32_t        extern_size;    // Extern size: object = cell + extern_size.
            byte_t *        mark_byte;      // Mark byte in the block byte map (small).
            __gc_large_t *  large;          // The large object header, or nullptr.
        };

        // Validates that p points into a live-or-garbage gc object, and locates it.
        bool __locate(void * p, __obj_t & out);

        // Returns the scan info of an object (cached, resolved lazily).
        const gc_scan_info_t & __scan_info_of(rt_type_t * type);

        // Scans the payload of a marked object according to its scan info.
        void __scan_object(const __obj_t & obj);

        // Verify mode: reports skipped words that point to gc objects.
        void __verify_skipped(const __obj_t & obj, rt_type_t * type,
                              void ** begin, void ** end);

        // Allocation. ---------- ---------- ----------

        // Collects when the allocated bytes exceed the threshold.
        void __maybe_collect();

        // Allocates a gc-managed space: extern area + payload of given size.
        // Returns the object address, which is cell + extern_size.
        void * __acquire(size_t extern_size, size_t size);

        // Allocates a cell of the specified size class.
        void * __alloc_cell(size_t size_class);

        // Allocates a large object.
        void * __alloc_large(size_t total_size, size_t extern_size);

        // Creates a new block of the specified size class.
        __gc_block_t * __new_block(size_t size_class);

        // Returns size class of the specified cell size.
        X_ALWAYS_INLINE size_t __size_class_of(size_t cell_size)
        {
            return __class_map[(cell_size - 1) / __gc_granule_size];
        }

        // Collection. ---------- ---------- ----------

        // The collection, with the approximate native stack top.
        size_t __collect(void ** native_stack_top);

        // Marks roots of all registered root sources.
        void __mark_roots(void ** native_stack_top);

        // Conservatively scans a memory range, words that look like
        // pointers into the gc heap mark the objects they point to.
        void __scan_range(void ** begin, void ** end);

        // Checks whether the pointer points into the gc heap,
        // marks the object and queues its payload for scanning.
        void __mark_candidate(void * p);

        // Scans queued payloads until the mark stack drains.
        void __process_mark_stack();

        // Sweeps unmarked cells and large objects, returns bytes collected.
        size_t __sweep();

        // Returns the high bound of the current thread stack.
        void ** __native_stack_high();

        // State. ---------- ---------- ----------

        gc_params_t     __params;

        // All blocks; the set answers "is this a gc block" in O(1).
        std::unordered_set<void *>   __block_set;
        std::vector<__gc_block_t *>  __blocks;

        // Blocks with free cells, per size class.
        __gc_block_t *  __partials[__gc_size_class_count];

        // Large objects, keyed by header address (ordered for interior pointers).
        std::map<byte_t *, __gc_large_t *> __larges;

        // Size class lookup: (cell_size - 1) / granule -> class index.
        byte_t          __class_map[__gc_max_cell_size / __gc_granule_size];

        // Mark stack: marked objects whose payload is pending a scan.
        std::vector<__obj_t> __mark_stack;

        // Type resolver and the cache of resolved scan infos.
        gc_type_resolver_t                              __resolver;
        std::unordered_map<rt_type_t *, gc_scan_info_t> __scan_infos;
        gc_scan_info_t                                  __conservative_info;

        // Statistics of the last collection (for tracing / tests).
        size_t          __scanned_bytes = 0;        // Payload bytes scanned.
        size_t          __skipped_bytes = 0;        // Payload bytes skipped by scan info.
        size_t          __verify_reports = 0;       // Suspicious skipped words found.

    public:

        // Statistics of the last collection.
        size_t scanned_bytes()   const { return __scanned_bytes; }
        size_t skipped_bytes()   const { return __skipped_bytes; }
        size_t verify_reports()  const { return __verify_reports; }

    private:

        // Registered root sources (execution contexts).
        std::vector<gc_root_source_t *> __root_sources;

        size_t          __allocated_since_gc = 0;   // Bytes allocated since last gc.
        size_t          __gc_threshold;             // Next gc when allocated exceeds it.
        size_t          __live_bytes = 0;           // Live bytes after last gc.

        void **         __stack_high = nullptr;     // Cached thread stack high bound.
    };

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::mm

#endif  // __MM_GC_HEAP_H__
