
#include <mm.h>
#include <algorithm.h>
#include <arch.h>

#include "__gc_heap.h"

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
    // gc_heap_t: construction.

    gc_heap_t::gc_heap_t(const gc_params_t & params)
        : __space_t(params), __params(params)
    {
        // An array that has no scan info may hold structs that are not a multiple of 8 bytes large.
        __unaligned_info.unaligned = true;
    }

    gc_heap_t::~gc_heap_t()
    {
    }

    void gc_heap_t::out_of_memory()
    {
        throw _E(common_error_code_t::no_memory);
    }

    ////////// ////////// ////////// ////////// //////////
    // gc_heap_t: allocation.

    // Creates a new object.
    rt_ref_t gc_heap_t::new_obj(rt_type_t * type)
    {
        const size_t extern_size = sizeof(rt_type_t *);
        size_t size = _alignf(type->get_size(), sizeof(rt_stack_unit_t));

        // An object that holds no references is atomic: marked, never scanned (verify mode scans it, to check the info).
        const gc_scan_info_t & info = __scan_info_of(type);
        bool atomic = info.kind == gc_scan_info_t::kind_t::none && !__params.verify;

        void * obj = allocate(extern_size, size, atomic);
        __set_object_type(obj, type);

        if (info.finalizable)
            register_finalizable(obj);

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

        const gc_scan_info_t & info = __scan_info_of(type);
        bool atomic = info.kind == gc_scan_info_t::kind_t::none && !__params.verify;

        void * obj;
        if (dimension == 1)
        {
            // type info + array length.
            const size_t extern_size = _alignf(
                sizeof(rt_type_t *) + sizeof(array_length_t), sizeof(rt_stack_unit_t)
            );

            size_t size = _alignf(length * element_size, sizeof(rt_stack_unit_t));
            obj = allocate(extern_size, size, atomic);

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
            obj = allocate(extern_size, size, atomic);

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

    // Creates a space that holds no references (garbage collected, never scanned).
    void * gc_heap_t::new_atomic(size_t size)
    {
        return allocate(0, size, true);
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
        return collect_here();
    }

    // Marks roots of all registered root sources.
    void gc_heap_t::mark_roots(void ** native_stack_top)
    {
        // Execution stacks and precise roots of registered sources.
        for (gc_root_source_t * source : __root_sources)
        {
            source->each_gc_root_ranges([this](void ** begin, void ** end) {
                scan_range(begin, end);
            });

            source->each_gc_roots([this](rt_ref_t ref) {
                mark_candidate((void *)ref);
            });
        }

        // The native thread stack: references may live only in C++ locals
        // of command implementations while they allocate.
        if (__params.scan_native_stack)
        {
            void ** stack_high = native_stack_high();
            if (stack_high != nullptr && native_stack_top != nullptr)
                scan_range(native_stack_top, stack_high);
        }
    }

    // Tells the program that finalizers are waiting.
    void gc_heap_t::on_collected()
    {
        if (has_finalizable() && __notify)
            __notify();
    }

    // The finalizer of an object is not made (or is made again).
    void gc_heap_t::suppress_finalize(void * object, bool suppress)
    {
        if (suppress)
        {
            unregister_finalizable(object);
        }
        else
        {
            register_finalizable(object);
        }
    }

    // Sets the gc type resolver, clears the cached scan infos.
    void gc_heap_t::set_gc_type_resolver(const gc_type_resolver_t & resolver)
    {
        __resolver = resolver;
        __scan_infos.clear();

        for (__cache_entry_t & entry : __cache)
            entry = __cache_entry_t();
    }

    // Returns the scan info of an object (cached, resolved lazily).
    const gc_scan_info_t & gc_heap_t::__scan_info_of(rt_type_t * type)
    {
        if (type == nullptr)
            return __conservative_info;

        if (!__resolver)
            return type->get_kind() == rt_type_kind_t::array? __unaligned_info : __conservative_info;

        __cache_entry_t & entry = __cache[((uintptr_t)type >> 4) % __cache_size];
        if (entry.type == type)
            return *entry.info;

        const gc_scan_info_t * result;

        auto it = __scan_infos.find(type);
        if (it != __scan_infos.end())
        {
            result = &it->second;
        }
        else
        {
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
            {
                info = gc_scan_info_t();

                // An array that was not resolved may hold structs of a size that is not a multiple of 8.
                info.unaligned = type->get_kind() == rt_type_kind_t::array;
            }

            result = &__scan_infos.emplace(type, std::move(info)).first->second;
        }

        entry.type = type;
        entry.info = result;

        return *result;
    }

    // Scans the payload of a marked object according to its scan info.
    void gc_heap_t::scan_object(const obj_t & obj)
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
                skipped_bytes_ += payload_size;
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

                    skipped_bytes_ += begin - cursor;
                    scanned_bytes_ += stop - begin;
                    scan_range((void **)begin, (void **)stop, info.unaligned);

                    cursor = stop;
                }

                if (__params.verify && cursor < end)
                    __verify_skipped(obj, type, (void **)cursor, (void **)end);

                skipped_bytes_ += end - cursor;
                break;
            }

            default:
                scanned_bytes_ += payload_size;
                scan_range((void **)object, (void **)end, info.unaligned);
                break;
        }
    }

    // Verify mode: reports skipped words that point to gc objects.
    void gc_heap_t::__verify_skipped(const obj_t & obj, rt_type_t * type, void ** begin, void ** end)
    {
        begin = (void **)(((uintptr_t)begin + sizeof(void *) - 1) & ~(sizeof(void *) - 1));

        for (void ** p = begin; p < end; p++)
        {
            obj_t target;
            if (locate(*p, target))
            {
                __verify_reports++;

                std::fprintf(stderr,
                    "[gc-verify] type %p: skipped word at +%llu of object %p points to "
                    "gc object %p: the scan info of this type may be wrong\n",
                    (void *)type, (unsigned long long)((byte_t *)p - (obj.cell + obj.extern_size)),
                    (void *)(obj.cell + obj.extern_size), *p
                );
            }
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Creates a garbage collected heap.
    rt_heap_t * create_gc_heap(memory_t * memory, const gc_params_t & params)
    {
        gc_params_t ps = params;

        auto flag = [](const char * name) {
            const char * value = std::getenv(name);
            return value != nullptr && *value == '1';
        };

        // APRIL_GC_STRESS=1: collect on every allocation (debugging).
        if (flag("APRIL_GC_STRESS"))
            ps.stress = true;

        // APRIL_GC_VERIFY=1: checks the regions skipped by precise scan infos.
        if (flag("APRIL_GC_VERIFY"))
            ps.verify = true;

        // APRIL_GC_TRACE=1: one line of statistics per collection.
        if (flag("APRIL_GC_TRACE"))
            ps.trace = true;

        // APRIL_GC_POISON=1: the freed cells are filled with 0xDD.
        if (flag("APRIL_GC_POISON"))
            ps.poison = true;

        // APRIL_GC_STATS=1: a summary when the heap ends.
        if (flag("APRIL_GC_STATS"))
            ps.stats = true;

        // APRIL_GC_THRESHOLD=n[K|M]: bytes allocated before a collection (also the floor of the threshold).
        const char * threshold = std::getenv("APRIL_GC_THRESHOLD");
        if (threshold != nullptr && *threshold != 0)
        {
            char * end = nullptr;
            unsigned long long value = std::strtoull(threshold, &end, 10);

            if (end != threshold)
            {
                if (*end == 'K' || *end == 'k')
                    value <<= 10;
                else if (*end == 'M' || *end == 'm')
                    value <<= 20;

                if (value > 0)
                    ps.initial_threshold = (size_t)value;
            }
        }

        // APRIL_GC_GROWTH=f: the next threshold is the live bytes times f.
        const char * growth = std::getenv("APRIL_GC_GROWTH");
        if (growth != nullptr && *growth != 0)
        {
            double value = std::strtod(growth, nullptr);
            if (value >= 1.0)
                ps.growth_factor = value;
        }

        // APRIL_GC_RESERVE=n[M|G]: the address space that the heap reserves.
        const char * reserve = std::getenv("APRIL_GC_RESERVE");
        if (reserve != nullptr && *reserve != 0)
        {
            char * end = nullptr;
            unsigned long long value = std::strtoull(reserve, &end, 10);

            if (end != reserve)
            {
                if (*end == 'M' || *end == 'm')
                    value <<= 20;
                else if (*end == 'G' || *end == 'g')
                    value <<= 30;

                if (value > 0)
                    ps.reserve_bytes = (size_t)value;
            }
        }

        return memory_t::new_obj<gc_heap_t>(memory, ps);
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::mm
