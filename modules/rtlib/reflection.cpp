#include <rtlib.h>
#include <mm.h>

namespace X_ROOT_NS::modules::rtlib {

    ////////// ////////// ////////// ////////// //////////

    // Array arguments
    struct __reflection_args_t
    {
        robject_t   obj;

        void * get_object() { return (void *)(rt_ref_t)obj; }
    };

    ////////// ////////// ////////// ////////// //////////

    // Get object type.

    struct __reflection_get_type_args_t : __reflection_args_t
    {
        // Empty.
    };

    void reflection_get_type(rtlib_context_t & ctx)
    {
        __reflection_get_type_args_t * args = ctx.args<__reflection_get_type_args_t>();

        void * obj = args->get_object();
        rt_type_t * type = mm::get_object_type(obj);

    ctx.return_(type);
    }

    ////////// ////////// ////////// ////////// //////////

    // Get type name.

    struct __reflection_get_type_name_args_t : __reflection_args_t
    {
        // Empty.
    };

    void reflection_get_type_name(rtlib_context_t & ctx)
    {
        __reflection_get_type_name_args_t * args = ctx.args<__reflection_get_type_name_args_t>();

        rt_type_t * type = (rt_type_t *)args->get_object();
        rt_sid_t name = type->get_name(ctx.analyzer.env);

        ctx.return_(new rt_string_t(_T("AAAA"), 4));
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::rtlib

