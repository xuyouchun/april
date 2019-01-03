
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

    class default_rt_heap_t : public object_t, public rt_heap_t
    {
    public:
        virtual rt_ref_t new_obj(rt_type_t * type) override;

        virtual rt_ref_t new_array(rt_array_type_t * type, dimension_t dimension,
                          const array_length_t * lengths) override;
    };

    ////////// ////////// ////////// ////////// //////////

    rt_heap_t * create_heap(memory_t * memory);

    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::mm


#endif // __MM_H__

