
#include <mm.h>


namespace X_ROOT_NS { namespace modules { namespace mm {

    ////////// ////////// ////////// ////////// //////////

    rt_ref_t default_rt_heap_t::new_obj(rt_type_t * type)
    {
        void * obj = memory_t::alloc(nullptr, type->get_size() + sizeof(rt_type_t *));
        *((rt_type_t **)obj) = type;

        return rt_ref_t((byte_t *)obj + sizeof(rt_type_t *));
    }

    rt_ref_t default_rt_heap_t::new_array(rt_array_type_t * type, dimension_t dimension,
                                                            const array_length_t * lengths)
    {
        typedef const array_length_t * l_t;

        _A(dimension >= 1);

        array_length_t length = *lengths;

        if(dimension == 1)
        {
            const size_t extern_size = _alignf(
                sizeof(rt_type_t *) + sizeof(array_length_t),
                sizeof(rt_type_t)
            );

            size_t size = length * type->get_element_type()->get_size() + extern_size;
            void * obj = memory_t::alloc(nullptr, size);

            *((array_length_t *)obj) = length;
            *((rt_type_t **)obj + 1) = type;

            return rt_ref_t((byte_t *)obj + extern_size);
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

    rt_heap_t * create_heap(memory_t * memory)
    {
        return memory_t::new_obj<default_rt_heap_t>(memory);
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::mm


