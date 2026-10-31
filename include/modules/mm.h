
#ifndef __MM_H__
#define __MM_H__

#include <core.h>
#include <rt.h>

namespace X_ROOT_NS::modules::mm {

    namespace
    {
        using namespace core;
        using namespace rt;
    }

    ////////// ////////// ////////// ////////// //////////

    // Memory error codes.
    X_ENUM(memory_error_code_t)

        // Array dimension error.
        array_dimension_error,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    // Default runtime memory heap.
    class default_rt_heap_t : public object_t, public rt_heap_t
    {
    public:

        // Creates new object.
        virtual rt_ref_t new_obj(rt_type_t * type) override;

        // Creates new array.
        virtual rt_ref_t new_array(rt_array_type_t * type, const array_length_t * lengths)
                                                   override;
        // allocate a memory space.
        virtual void * alloc(size_t size, void * realloc) override;

        // free a memory space.
        virtual void free(void * ptr) override;

        X_TO_STRING_IMPL(_T("default_rt_heap_t"))
    };

    ////////// ////////// ////////// ////////// //////////

    // Creates heap.
    rt_heap_t * create_heap(memory_t * memory);

    //-------- ---------- ---------- ---------- ----------

    // Garbage collection parameters.
    struct gc_params_t
    {
        // Bytes allocated before the first collection; also the floor
        // of the dynamic threshold.
        size_t  initial_threshold   = 16 * 1024 * 1024;

        // Next threshold = live_bytes * growth_factor (but at least
        // initial_threshold).
        double  growth_factor       = 2.0;

        // Collects on every allocation; for debugging, also enabled by
        // the environment variable APRIL_GC_STRESS=1.
        bool    stress              = false;

        // Conservatively scans the native thread stack: required for
        // correctness while command implementations hold references in
        // C++ locals across allocations; only tests may turn it off.
        bool    scan_native_stack   = true;

        // Verify mode (also APRIL_GC_VERIFY=1): while marking, words in the
        // regions that precise scan infos skip are checked, and the ones that
        // point to gc objects are reported to stderr: a hint that a scan info
        // is wrong. Slow, for debugging only.
        bool    verify              = false;

        // Prints one line of statistics per collection to stderr
        // (also APRIL_GC_TRACE=1).
        bool    trace               = false;
    };

    // Creates a garbage collected heap: new_obj / new_array allocations are
    // reclaimed by a stop-the-world mark-sweep collector; alloc / free keep
    // manual semantics.
    rt_heap_t * create_gc_heap(memory_t * memory, const gc_params_t & params = gc_params_t());

    ////////// ////////// ////////// ////////// //////////

    // Gets object runtime type.
    X_INLINE rt_type_t * get_object_type(void * obj)
    {
        return *((rt_type_t **)obj - 1);
    }

    // Gets object runtime type.
    X_INLINE rt_type_t * get_object_type(rt_ref_t obj)
    {
        return get_object_type((void *)obj);
    }

    // Gets array lengths.
    X_INLINE array_length_t * get_array_lengths(void * arr_obj)
    {
        return (array_length_t *)((rt_type_t **)arr_obj - 1) - 2;
    }

    // Gets array lengths.
    X_INLINE array_length_t * get_array_lengths(rt_ref_t arr_obj)
    {
        return get_array_lengths((void *)arr_obj);
    }

    // Gets array dimension.
    X_INLINE dimension_t get_array_dimension(void * arr_obj)
    {
        rt_array_type_t * rt_type = (rt_array_type_t *)get_object_type(arr_obj);
        return rt_type->dimension;
    }

    // Gets array dimension.
    X_INLINE dimension_t get_array_dimension(rt_ref_t arr_obj)
    {
        return get_array_dimension((void *)arr_obj);
    }

    // Gets array lengths.
    X_INLINE array_length_t get_array_length(void * arr_obj)
    {
        return *((array_length_t *)((rt_type_t **)arr_obj - 1) - 1);
    }

    // Gets array lengths.
    X_INLINE array_length_t get_array_length(rt_ref_t arr_obj)
    {
        return get_array_length((void *)arr_obj);
    }

    // Gets array length of specified dimension.
    X_INLINE array_length_t get_array_length_of_dimension(void * arr_obj, dimension_t dimension)
    {
        dimension_t rank = get_array_dimension(arr_obj);
        if (dimension == 0)
        {
            if (rank == 1)
                return get_array_length(arr_obj);
        }

        array_length_t * lengths = get_array_lengths(arr_obj);
        int index = rank - dimension - 1;
        if (index == 0)
            return *lengths;
            
        return lengths[-index] / lengths[-index + 1];
    }

    // Gets array length of specified dimension.
    X_INLINE array_length_t get_array_length_of_dimension(rt_ref_t arr_obj, dimension_t dimension)
    {
        return get_array_length_of_dimension((void *)arr_obj, dimension);
    }

    // Gets array element.
    template<typename _t> _t get_array_element(void * arr_obj, array_length_t index)
    {
        return *((_t *)arr_obj + index);
    }

    // Gets array element.
    template<typename _t> _t get_array_element(rt_ref_t arr_obj, array_length_t index)
    {
        return get_array_element<_t>((void *)arr_obj, index);
    }

    // Sets array element.
    template<typename _t> _t set_array_element(void * arr_obj, array_length_t index, const _t & v)
    {
        return *((_t *)arr_obj + index) = v;
    }

    // Sets array element.
    template<typename _t> _t set_array_element(rt_ref_t arr_obj, array_length_t index, const _t & v)
    {
        return set_array_element<_t>((void *)arr_obj, index, v);
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::mm


#endif // __MM_H__

