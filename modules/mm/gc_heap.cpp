
#if defined(_WIN32)
#include <malloc.h>

// Declared here instead of including <windows.h>, whose macros pollute the project.
extern "C" __declspec(dllimport) void __stdcall
    GetCurrentThreadStackLimits(unsigned long long * low, unsigned long long * high);
#elif defined(__linux__)
#include <pthread.h>
#endif

#include <mm.h>
#include <algorithm.h>
#include <arch.h>

#include "__gc_heap.h"

#include <csetjmp>
#include <cstdio>
#include <cstdlib>
#include <cstring>


namespace X_ROOT_NS::modules::mm {

    using namespace rt;

    ////////// ////////// ////////// ////////// //////////
    // Object layout helpers (same layout as default_rt_heap_t).

    namespace
    {
        // Sets object type.
        X_ALWAYS_INLINE void __set_object_type(void * obj, rt_type_t * type)
        {
            *((rt_type_t **)obj - 1) = type;
        }

        // Sets array length.
        X_ALWAYS_INLINE void __set_array_length(void * arr_obj, array_length_t length)
        {
            *((array_length_t *)((rt_type_t **)arr_obj - 1) - 1) = length;
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // __gc_block_t

    // Initializes the block for the specified size class.
    void __gc_block_t::initialize(uint16_t size_class_, uint32_t cell_size_)
    {
        magic       = __magic_value;
        size_class  = size_class_;
        cell_size   = cell_size_;
        used_count  = 0;
        free_list   = nullptr;
        next_partial = nullptr;

        size_t header_size = _alignf(sizeof(__gc_block_t), __gc_granule_size);
        first_cell_granule = header_size / __gc_granule_size;
        cell_count = (uint32_t)(
            (__gc_block_size - header_size) / cell_size_
        );

        std::memset(byte_map, 0, sizeof(byte_map));

        // Chains all cells into the free list, last cell first.
        for (size_t index = cell_count; index > 0; index--)
        {
            void * cell = cell_at(index - 1);
            *(void **)cell = free_list;
            free_list = cell;
        }
    }

    // Takes a cell from the free list, returns nullptr when full.
    void * __gc_block_t::take_cell()
    {
        void * cell = free_list;

        if (cell != nullptr)
        {
            free_list = *(void **)cell;
            used_count++;
        }

        return cell;
    }

    ////////// ////////// ////////// ////////// //////////
    // gc_heap_t: construction.

    gc_heap_t::gc_heap_t(const gc_params_t & params)
        : __params(params)
    {
        __gc_threshold = params.initial_threshold;

        std::memset(__partials, 0, sizeof(__partials));

        // Builds the size class lookup table.
        size_t size_class = 0;
        for (size_t index = 0, count = __gc_max_cell_size / __gc_granule_size;
                                                index < count; index++)
        {
            size_t cell_size = (index + 1) * __gc_granule_size;
            while (__gc_cell_sizes[size_class] < cell_size)
                size_class++;

            __class_map[index] = (byte_t)size_class;
        }
    }

    // Allocates / frees a block of aligned memory. (aligned_alloc is missing on Windows)
    static void * __block_alloc(size_t size)
    {
    #ifdef _WIN32
        return ::_aligned_malloc(size, size);
    #else
        return std::aligned_alloc(size, size);
    #endif
    }

    static void __block_free(void * block)
    {
    #ifdef _WIN32
        ::_aligned_free(block);
    #else
        std::free(block);
    #endif
    }

    gc_heap_t::~gc_heap_t()
    {
        for (__gc_block_t * block : __blocks)
        {
            __block_free(block);
        }

        for (auto && it : __larges)
        {
            std::free(it.second);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // gc_heap_t: allocation.

    // Creates a new object.
    rt_ref_t gc_heap_t::new_obj(rt_type_t * type)
    {
        const size_t extern_size = sizeof(rt_type_t *);
        size_t size = _alignf(type->get_size(), sizeof(rt_stack_unit_t));

        void * obj = __acquire(extern_size, size);
        __set_object_type(obj, type);

        return rt_ref_t(obj);
    }

    // Creates a new array.
    rt_ref_t gc_heap_t::new_array(rt_array_type_t * type, const array_length_t * lengths)
    {
        typedef array_length_t l_t;

        dimension_t dimension = type->dimension;

        array_length_t length = *lengths;
        // A struct is as large as its fields: the elements are that far from each other.
        size_t element_size = al::max<size_t>(
            unit_align(1, type->element_type->get_size()), type->element_type->get_size()
        );

        void * obj;
        if (dimension == 1)
        {
            // type info + array length.
            const size_t extern_size = _alignf(
                sizeof(rt_type_t *) + sizeof(array_length_t), sizeof(rt_stack_unit_t)
            );

            size_t size = _alignf(length * element_size, sizeof(rt_stack_unit_t));
            obj = __acquire(extern_size, size);

            __set_object_type(obj, type);
            __set_array_length(obj, length);
        }
        else if (dimension >= 2)
        {
            for (const l_t * l = lengths + 1, * l_end = lengths + dimension; l < l_end; l++)
            {
                length *= *l;
            }

            // type info + length of each dimensions + array length.
            const size_t extern_size = _alignf(
                sizeof(rt_type_t *) + sizeof(array_length_t) * dimension + sizeof(array_length_t),
                sizeof(rt_stack_unit_t)
            );

            size_t size = _alignf(length * element_size, sizeof(rt_stack_unit_t));
            obj = __acquire(extern_size, size);

            __set_object_type(obj, type);
            __set_array_length(obj, length);

            l_t * the_lengths = get_array_lengths(obj);
            l_t base = 1;

            for (const l_t * l = lengths + dimension - 1, * l_end = lengths; l >= l_end;
                                                l--, the_lengths--)
            {
                *the_lengths = *l * base;
                base = *the_lengths;
            }
        }
        else
        {
            throw _ED(memory_error_code_t::array_dimension_error, dimension);
        }

        return rt_ref_t(obj);
    }

    // Allocates a raw memory space (manual, not garbage collected).
    void * gc_heap_t::alloc(size_t size, void * realloc)
    {
        if (realloc != nullptr)
            this->free(realloc);

        return std::malloc(size);
    }

    // Frees a raw memory space.
    void gc_heap_t::free(void * ptr)
    {
        if (ptr != nullptr)
            std::free(ptr);
    }

    // Collects when the allocated bytes exceed the threshold.
    void gc_heap_t::__maybe_collect()
    {
        if (__params.stress || __allocated_since_gc >= __gc_threshold)
            collect();
    }

    // Allocates a gc-managed space: extern area + payload of given size.
    void * gc_heap_t::__acquire(size_t extern_size, size_t size)
    {
        __maybe_collect();

        size_t cell_size = _alignf(extern_size + size, __gc_granule_size);

        void * cell;
        if (cell_size <= __gc_max_cell_size && extern_size <= __gc_max_small_extern)
        {
            size_t size_class = __size_class_of(cell_size);
            size_t real_cell_size = __gc_cell_sizes[size_class];

            cell = __alloc_cell(size_class);
            std::memset(cell, 0, real_cell_size);

            // Records the cell start and its extern size in the byte map.
            __gc_block_t * block = (__gc_block_t *)__gc_block_t::block_base_of(cell);
            size_t granule = ((byte_t *)cell - block->base()) / __gc_granule_size;
            block->byte_map[granule] = (byte_t)(extern_size / 8);

            __allocated_since_gc += real_cell_size;
        }
        else
        {
            size_t total_size = extern_size + _alignf(size, __gc_granule_size);
            cell = __alloc_large(total_size, (uint32_t)extern_size);
            std::memset(cell, 0, total_size);

            __allocated_since_gc += total_size;
        }

        return (byte_t *)cell + extern_size;
    }

    // Allocates a cell of the specified size class.
    void * gc_heap_t::__alloc_cell(size_t size_class)
    {
        __gc_block_t * block = __partials[size_class];

        if (block == nullptr)
        {
            block = __new_block(size_class);
            block->next_partial = __partials[size_class];
            __partials[size_class] = block;
        }

        void * cell = block->take_cell();
        _A(cell != nullptr);

        // Full blocks leave the partial list.
        if (block->free_list == nullptr)
        {
            __partials[size_class] = block->next_partial;
            block->next_partial = nullptr;
        }

        return cell;
    }

    // Creates a new block of the specified size class.
    __gc_block_t * gc_heap_t::__new_block(size_t size_class)
    {
        void * memory = __block_alloc(__gc_block_size);
        if (memory == nullptr)
            throw _E(common_error_code_t::no_memory);

        __gc_block_t * block = (__gc_block_t *)memory;
        block->initialize((uint16_t)size_class, __gc_cell_sizes[size_class]);

        __block_set.insert(memory);
        __blocks.push_back(block);

        return block;
    }

    // Allocates a large object.
    void * gc_heap_t::__alloc_large(size_t total_size, size_t extern_size)
    {
        size_t header_size = _alignf(sizeof(__gc_large_t), __gc_granule_size);

        __gc_large_t * large = (__gc_large_t *)std::malloc(header_size + total_size);
        if (large == nullptr)
            throw _E(common_error_code_t::no_memory);

        large->total_size  = total_size;
        large->extern_size = (uint32_t)extern_size;
        large->marked      = false;

        __larges[(byte_t *)large] = large;

        return large->payload();
    }

    ////////// ////////// ////////// ////////// //////////
    // gc_heap_t: root sources.

    // Registers a gc root source.
    void gc_heap_t::add_root_source(gc_root_source_t * source)
    {
        __root_sources.push_back(source);
    }

    // Removes a gc root source.
    void gc_heap_t::remove_root_source(gc_root_source_t * source)
    {
        for (auto it = __root_sources.begin(); it != __root_sources.end(); it++)
        {
            if (*it == source)
            {
                __root_sources.erase(it);
                break;
            }
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // gc_heap_t: collection.

    // Executes a garbage collection, returns bytes collected.
    size_t gc_heap_t::collect()
    {
        // Spills callee-saved registers onto the native stack, so that
        // references held only in registers are seen by the scan below.
        std::jmp_buf registers;
        setjmp(registers);

        void ** native_stack_top = (void **)&registers;
        return __collect(native_stack_top);
    }

    // The collection, with the approximate native stack top.
    size_t gc_heap_t::__collect(void ** native_stack_top)
    {
        __mark_stack.clear();
        __scanned_bytes = 0;
        __skipped_bytes = 0;

        __mark_roots(native_stack_top);
        __process_mark_stack();

        size_t collected = __sweep();

        __allocated_since_gc = 0;
        __gc_threshold = al::max(
            __params.initial_threshold,
            (size_t)(__live_bytes * __params.growth_factor)
        );

        if (__params.trace)
        {
            std::fprintf(stderr,
                "[gc] collected %zu, live %zu, payload scanned %zu / skipped %zu, "
                "next threshold %zu\n",
                collected, __live_bytes, __scanned_bytes, __skipped_bytes, __gc_threshold
            );
        }

        return collected;
    }

    // Marks roots of all registered root sources.
    void gc_heap_t::__mark_roots(void ** native_stack_top)
    {
        // Execution stacks and precise roots of registered sources.
        for (gc_root_source_t * source : __root_sources)
        {
            source->each_gc_root_ranges([this](void ** begin, void ** end) {
                __scan_range(begin, end);
            });

            source->each_gc_roots([this](rt_ref_t ref) {
                __mark_candidate((void *)ref);
            });
        }

        // The native thread stack: references may live only in C++ locals
        // of command implementations while they allocate.
        if (__params.scan_native_stack)
        {
            void ** stack_high = __native_stack_high();
            if (stack_high != nullptr && native_stack_top != nullptr)
                __scan_range(native_stack_top, stack_high);
        }
    }

    // Conservatively scans a memory range.
    void gc_heap_t::__scan_range(void ** begin, void ** end)
    {
        begin = (void **)(((uintptr_t)begin + sizeof(void *) - 1) & ~(sizeof(void *) - 1));

        for (void ** p = begin; p < end; p++)
        {
            __mark_candidate(*p);
        }
    }

    // Validates that p points into a live-or-garbage gc object, and locates it.
    bool gc_heap_t::__locate(void * p, __obj_t & out)
    {
        if (p == nullptr)
            return false;

        void * block_base = __gc_block_t::block_base_of(p);

        if (__block_set.find(block_base) != __block_set.end())
        {
            __gc_block_t * block = (__gc_block_t *)block_base;

            size_t granule = ((byte_t *)p - block->base()) / __gc_granule_size;
            if (granule < block->first_cell_granule)
                return false;

            // Cells sit on a fixed grid: normalize to the cell start granule.
            size_t cell_granules = block->cell_size / __gc_granule_size;
            size_t cell_index = (granule - block->first_cell_granule) / cell_granules;
            if (cell_index >= block->cell_count)
                return false;

            byte_t & entry = block->byte_map[block->granule_of(cell_index)];

            if ((entry & __gc_extern_mask) == 0)    // A free cell.
                return false;

            out.cell        = block->cell_at(cell_index);
            out.size        = block->cell_size;
            out.extern_size = (uint32_t)(entry & __gc_extern_mask) * 8;
            out.mark_byte   = &entry;
            out.large       = nullptr;

            return true;
        }

        if (!__larges.empty())
        {
            // Large objects: ordered lookup supports interior pointers.
            auto it = __larges.upper_bound((byte_t *)p);
            if (it == __larges.begin())
                return false;

            __gc_large_t * large = (--it)->second;
            byte_t * payload = large->payload();

            if ((byte_t *)p < payload || (byte_t *)p >= payload + large->total_size)
                return false;

            out.cell        = payload;
            out.size        = large->total_size;
            out.extern_size = large->extern_size;
            out.mark_byte   = nullptr;
            out.large       = large;

            return true;
        }

        return false;
    }

    // Checks whether the pointer points into the gc heap,
    // marks the object and queues its payload for scanning.
    void gc_heap_t::__mark_candidate(void * p)
    {
        __obj_t obj;
        if (!__locate(p, obj))
            return;

        if (obj.large != nullptr)
        {
            if (obj.large->marked)
                return;

            obj.large->marked = true;
        }
        else
        {
            if (*obj.mark_byte & __gc_mark_mask)    // Already marked.
                return;

            *obj.mark_byte |= __gc_mark_mask;
        }

        __mark_stack.push_back(obj);
    }

    // Scans queued payloads until the mark stack drains.
    void gc_heap_t::__process_mark_stack()
    {
        while (!__mark_stack.empty())
        {
            __obj_t obj = __mark_stack.back();
            __mark_stack.pop_back();

            __scan_object(obj);
        }
    }

    // Sets the gc type resolver, clears the cached scan infos.
    void gc_heap_t::set_gc_type_resolver(const gc_type_resolver_t & resolver)
    {
        __resolver = resolver;
        __scan_infos.clear();
    }

    // Returns the scan info of an object (cached, resolved lazily).
    const gc_scan_info_t & gc_heap_t::__scan_info_of(rt_type_t * type)
    {
        if (type == nullptr || !__resolver)
            return __conservative_info;

        auto it = __scan_infos.find(type);
        if (it != __scan_infos.end())
            return it->second;

        gc_scan_info_t info;
        bool resolved = false;

        try
        {
            resolved = __resolver(type, info);
        }
        catch (...)
        {
            resolved = false;       // Whatever goes wrong: stay conservative.
        }

        if (!resolved)
            info = gc_scan_info_t();

        return __scan_infos.emplace(type, std::move(info)).first->second;
    }

    // Scans the payload of a marked object according to its scan info.
    void gc_heap_t::__scan_object(const __obj_t & obj)
    {
        // The object starts after the extern area; its type is stored right before it.
        byte_t * object  = obj.cell + obj.extern_size;
        byte_t * end     = obj.cell + obj.size;
        rt_type_t * type = *((rt_type_t **)object - 1);

        const gc_scan_info_t & info = __scan_info_of(type);
        size_t payload_size = end - object;

        switch (info.kind)
        {
            case gc_scan_info_t::kind_t::none:
                __skipped_bytes += payload_size;
                if (__params.verify)
                    __verify_skipped(obj, type, (void **)object, (void **)end);
                break;

            case gc_scan_info_t::kind_t::ranges: {
                byte_t * cursor = object;

                for (auto && range : info.ranges)
                {
                    byte_t * begin = object + range.first;
                    byte_t * stop  = begin + range.second;

                    if (stop > end)                 // Never reads out of the cell.
                        stop = end;

                    if (begin < cursor || begin >= stop)
                        continue;

                    if (__params.verify && begin > cursor)
                        __verify_skipped(obj, type, (void **)cursor, (void **)begin);

                    __skipped_bytes += begin - cursor;
                    __scanned_bytes += stop - begin;
                    __scan_range((void **)begin, (void **)stop);

                    cursor = stop;
                }

                if (__params.verify && cursor < end)
                    __verify_skipped(obj, type, (void **)cursor, (void **)end);

                __skipped_bytes += end - cursor;
                break;
            }

            default:
                __scanned_bytes += payload_size;
                __scan_range((void **)object, (void **)end);
                break;
        }
    }

    // Verify mode: reports skipped words that point to gc objects.
    void gc_heap_t::__verify_skipped(const __obj_t & obj, rt_type_t * type,
                                     void ** begin, void ** end)
    {
        begin = (void **)(((uintptr_t)begin + sizeof(void *) - 1) & ~(sizeof(void *) - 1));

        for (void ** p = begin; p < end; p++)
        {
            __obj_t target;
            if (__locate(*p, target))
            {
                __verify_reports++;

                std::fprintf(stderr,
                    "[gc-verify] type %p: skipped word at +%zu of object %p points to "
                    "gc object %p: the scan info of this type may be wrong\n",
                    (void *)type, (size_t)((byte_t *)p - (obj.cell + obj.extern_size)),
                    (void *)(obj.cell + obj.extern_size), *p
                );
            }
        }
    }

    // Sweeps unmarked cells and large objects, returns bytes collected.
    size_t gc_heap_t::__sweep()
    {
        size_t collected = 0;
        size_t live_bytes = 0;

        std::memset(__partials, 0, sizeof(__partials));

        std::vector<__gc_block_t *> survived_blocks;
        survived_blocks.reserve(__blocks.size());

        for (__gc_block_t * block : __blocks)
        {
            block->free_list = nullptr;
            block->next_partial = nullptr;

            uint32_t used = 0;

            for (size_t index = 0, count = block->cell_count; index < count; index++)
            {
                byte_t & entry = block->byte_map[block->granule_of(index)];

                if (entry & __gc_mark_mask)         // Live: clear the mark.
                {
                    entry &= __gc_extern_mask;
                    live_bytes += block->cell_size;
                    used++;
                }
                else                                // Dead or free: chain to free list.
                {
                    if ((entry & __gc_extern_mask) != 0)
                    {
                        entry = 0;
                        collected += block->cell_size;
                    }

                    void * cell = block->cell_at(index);
                    *(void **)cell = block->free_list;
                    block->free_list = cell;
                }
            }

            block->used_count = used;

            if (used == 0)                          // Empty block: release it.
            {
                __block_set.erase(block);
                __block_free(block);
            }
            else
            {
                survived_blocks.push_back(block);

                if (block->free_list != nullptr)
                {
                    block->next_partial = __partials[block->size_class];
                    __partials[block->size_class] = block;
                }
            }
        }

        __blocks = std::move(survived_blocks);

        // Large objects.
        for (auto it = __larges.begin(); it != __larges.end(); )
        {
            __gc_large_t * large = it->second;

            if (large->marked)
            {
                large->marked = false;
                live_bytes += large->total_size;
                it++;
            }
            else
            {
                collected += large->total_size;
                it = __larges.erase(it);
                std::free(large);
            }
        }

        __live_bytes = live_bytes;
        return collected;
    }

    // Returns the high bound of the current thread stack.
    void ** gc_heap_t::__native_stack_high()
    {
        if (__stack_high == nullptr)
        {
        #if defined(__linux__)
            pthread_attr_t attr;
            if (pthread_getattr_np(pthread_self(), &attr) == 0)
            {
                void * stack_addr = nullptr;
                size_t stack_size = 0;

                if (pthread_attr_getstack(&attr, &stack_addr, &stack_size) == 0)
                    __stack_high = (void **)((byte_t *)stack_addr + stack_size);

                pthread_attr_destroy(&attr);
            }
        #elif defined(_WIN32)
            unsigned long long low = 0, high = 0;
            ::GetCurrentThreadStackLimits(&low, &high);
            __stack_high = (void **)high;
        #endif
        }

        return __stack_high;
    }

    ////////// ////////// ////////// ////////// //////////

    // Creates a garbage collected heap.
    rt_heap_t * create_gc_heap(memory_t * memory, const gc_params_t & params)
    {
        gc_params_t ps = params;

        // APRIL_GC_STRESS=1: collect on every allocation (debugging).
        const char * stress = std::getenv("APRIL_GC_STRESS");
        if (stress != nullptr && *stress == '1')
            ps.stress = true;

        // APRIL_GC_VERIFY=1: checks the regions skipped by precise scan infos.
        const char * verify = std::getenv("APRIL_GC_VERIFY");
        if (verify != nullptr && *verify == '1')
            ps.verify = true;

        // APRIL_GC_TRACE=1: one line of statistics per collection.
        const char * trace = std::getenv("APRIL_GC_TRACE");
        if (trace != nullptr && *trace == '1')
            ps.trace = true;

        return memory_t::new_obj<gc_heap_t>(memory, ps);
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::mm
