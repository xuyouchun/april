
#include <mm.h>
#include <algorithm.h>


namespace X_ROOT_NS { namespace modules { namespace mm {

    // Sets object type.
    void __set_object_type(void * obj, rt_type_t * type)
    {
        *((rt_type_t **)obj - 1) = type;
    }

    al::xheap_t __default_memory;
    
    ////////// ////////// ////////// ////////// //////////

    // Allocate memory with specified size.
    template<size_t _extern_size> void * __alloc(size_t size)
    {
        return (byte_t *)memory_t::alloc(&__default_memory, size + _extern_size) + _extern_size;
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
    rt_ref_t default_rt_heap_t::new_array(rt_array_type_t * type, dimension_t dimension,
                                                            const array_length_t * lengths)
    {
        typedef const array_length_t * l_t;

        _A(dimension >= 1);

        array_length_t length = *lengths;
        if(dimension == 1)
        {
            const size_t extern_size = _alignf(
                sizeof(rt_type_t *) + sizeof(array_length_t), sizeof(rt_stack_unit_t)
            );

            size_t size = length * type->element_type->get_size();
            void * obj  = __alloc<extern_size>(size);

            __set_object_type(obj, type);
            *get_array_lengths(obj) = length;

            return rt_ref_t(obj);
        }
        else
        {
            for(l_t l = lengths + 1, l_end = lengths + dimension; l < l_end; l++)
            {
                length *= *l;
            }

            return nullptr;
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Creates a heap.
    rt_heap_t * create_heap(memory_t * memory)
    {
        return memory_t::new_obj<default_rt_heap_t>(memory);
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::mm


