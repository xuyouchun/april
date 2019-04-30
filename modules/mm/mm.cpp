
#include <mm.h>
#include <algorithm.h>


namespace X_ROOT_NS { namespace modules { namespace mm {

    using namespace rt;

    ////////// ////////// ////////// ////////// //////////

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

    template<size_t _size, typename _t> struct __zero_t
    {
        X_ALWAYS_INLINE static void zero(_t * p)
        {
            *p = (_t)0;
            __zero_t<_size - 1, _t>::zero(p + 1);
        }
    };

    template<typename _t> struct __zero_t<0, _t>
    {
        X_ALWAYS_INLINE static void zero(_t * p)
        {
            // Do nothing.
        }
    };

    template<size_t _size, typename _t>
    X_ALWAYS_INLINE void __zero(_t * p)
    {
        __zero_t<_size, _t>::zero(p);
    }

    const size_t __max_zero_size = 16;

    template<typename _t>
    X_ALWAYS_INLINE bool __zero(size_t size, _t * p)
    {
        switch(size)
        {
            #define __Case(_n)      case _n: __zero<_n>(p); return true;

            __Case(0)  __Case(1)  __Case(2)  __Case(3)  __Case(4)
            __Case(5)  __Case(6)  __Case(7)  __Case(8)  __Case(9)
            __Case(10) __Case(11) __Case(12) __Case(13) __Case(14)
            __Case(15) __Case(16)

            default:
                __zero<__max_zero_size>(p);
                return false;

            #undef __Case
        }
    }

    // Sets memory zero.
    template<typename _t> void zero_memory(_t * p, size_t size)
    {
        while(!__zero(size, p))
        {
            p    += __max_zero_size;
            size -= __max_zero_size;
        }
    }

    typedef arch_uint_t __uint_t;

    // Creates a new object.
    rt_ref_t default_rt_heap_t::new_obj(rt_type_t * type)
    {
        const size_t extern_size = sizeof(rt_type_t *);
        size_t size = _alignf(type->get_size(), sizeof(rt_stack_unit_t));
        void * obj = __alloc<extern_size>(size);

        __set_object_type(obj, type);
        zero_memory((__uint_t *)obj, size / sizeof(__uint_t));

        return rt_ref_t(obj);
    }

    // Creates a new array.
    rt_ref_t default_rt_heap_t::new_array(rt_array_type_t * type, const array_length_t * lengths)
    {
        typedef array_length_t l_t;

        dimension_t dimension = type->dimension;

        array_length_t length = *lengths;
        size_t element_size = unit_align(1, type->element_type->get_size());
        size_t size = _alignf(length * element_size, sizeof(rt_stack_unit_t));

        void * obj;
        if(dimension == 1)
        {
            const size_t extern_size = _alignf(
                sizeof(rt_type_t *) + sizeof(array_length_t), sizeof(rt_stack_unit_t)
            );

            obj  = __alloc<extern_size>(size);

            __set_object_type(obj, type);
            __set_array_length(obj, length);
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

            obj = __alloc(extern_size, size);

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
        else
        {
            throw _ED(__e_t::array_dimension_error, dimension);
        }

        zero_memory((__uint_t *)obj, size / sizeof(__uint_t));
        return rt_ref_t(obj);
    }

    ////////// ////////// ////////// ////////// //////////

    // Creates a heap.
    rt_heap_t * create_heap(memory_t * memory)
    {
        return memory_t::new_obj<default_rt_heap_t>(memory);
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::mm


