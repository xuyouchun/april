
#include <mm.h>
#include <algorithm.h>


namespace X_ROOT_NS { namespace modules { namespace mm {

    // Memory error codes.
    X_ENUM_INFO(memory_error_code_t)

        // Array dimension error.
        X_C(array_dimension_error,  _T("array dimension must large than 1, specified %1%"))

    X_ENUM_INFO_END

    typedef memory_error_code_t __e_t;

    ////////// ////////// ////////// ////////// //////////

    // Sets object type.
    X_ALWAYS_INLINE void __set_object_type(void * obj, rt_type_t * type)
    {
        *((rt_type_t **)obj - 1) = type;
    }

    // Gets array lengths.
    X_ALWAYS_INLINE void __set_array_length(void * arr_obj, array_length_t length)
    {
        *((array_length_t *)((rt_type_t **)arr_obj - 1) - 1) = length;
    }

    al::xheap_t __default_memory;
    
    ////////// ////////// ////////// ////////// //////////

    // Allocate memory with specified size.
    X_ALWAYS_INLINE void * __alloc_memory(size_t size)
    {
        return memory_t::alloc(&__default_memory, size);
    }

    // Allocate memory with specified size/extern size.
    X_ALWAYS_INLINE void * __alloc(size_t extern_size, size_t size)
    {
        return (byte_t *)__alloc_memory(size + extern_size) + extern_size;
    }

    // Allocate memory with specified size/extern size.
    template<size_t _extern_size> X_ALWAYS_INLINE void * __alloc(size_t size)
    {
        return (byte_t *)__alloc_memory(size + _extern_size) + _extern_size;
    }

    // Creates a new object.
    rt_ref_t default_rt_heap_t::new_obj(rt_type_t * type)
    {
        const size_t extern_size = sizeof(rt_type_t *);
        void * obj = __alloc<extern_size>(type->get_size());
        __set_object_type(obj, type);

        return rt_ref_t(obj);
    }

    // Creates a new array.
    rt_ref_t default_rt_heap_t::new_array(rt_array_type_t * type, const array_length_t * lengths)
    {
        typedef array_length_t l_t;

        dimension_t dimension = type->dimension;

        array_length_t length = *lengths;
        if(dimension == 1)
        {
            const size_t extern_size = _alignf(
                sizeof(rt_type_t *) + sizeof(array_length_t), sizeof(rt_stack_unit_t)
            );

            size_t size = length * type->element_type->get_size();
            void * obj  = __alloc<extern_size>(size);

            __set_object_type(obj, type);
            __set_array_length(obj, length);

            return rt_ref_t(obj);
        }
        else if(dimension >= 2)
        {
            for(const l_t * l = lengths + 1, * l_end = lengths + dimension; l < l_end; l++)
            {
                length *= *l;
            }

            const size_t extern_size = _alignf(
                sizeof(rt_type_t *) + sizeof(array_length_t) * dimension, sizeof(rt_stack_unit_t)
            );

            size_t size = length * type->element_type->get_size();
            void * obj = __alloc(extern_size, size);

            __set_object_type(obj, type);
            __set_array_length(obj, length);

            l_t * the_lengths = get_array_lengths(obj);
            l_t base = 1;

            for(const l_t * l = lengths + dimension - 1, * l_end = lengths; l >= l_end; l--)
            {
                *the_lengths = *l * base;
                base = *the_lengths--;
            }

            return rt_ref_t(obj);
        }

        throw _ED(__e_t::array_dimension_error, dimension);
    }

    ////////// ////////// ////////// ////////// //////////

    // Creates a heap.
    rt_heap_t * create_heap(memory_t * memory)
    {
        return memory_t::new_obj<default_rt_heap_t>(memory);
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::mm


