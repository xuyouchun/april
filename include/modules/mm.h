
#ifndef __MM_H__
#define __MM_H__

#include <core.h>
#include <rt.h>

namespace X_ROOT_NS { namespace modules { namespace mm {

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

        X_TO_STRING_IMPL(_T("default_rt_heap_t"))
    };

    ////////// ////////// ////////// ////////// //////////

    // Creates heap.
    rt_heap_t * create_heap(memory_t * memory);

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

} } }  // namespace X_ROOT_NS::modules::mm


#endif // __MM_H__

