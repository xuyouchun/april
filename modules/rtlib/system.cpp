#include <rtlib.h>

namespace X_ROOT_NS { namespace modules { namespace rtlib {

    ////////// ////////// ////////// ////////// //////////

    // Write.
    struct __write_args_t
    {
        rstring_t s;
    };

    void write(rtlib_context_t & ctx)
    {
        __write_args_t * arg = ctx.args<__write_args_t>();
        std::wcout << *arg->s;
    }

    ////////// ////////// ////////// ////////// //////////

    // TestAdd.
    struct __test_add_args_t
    {
        rint32_t    a, b;
    };

    void test_add(rtlib_context_t & ctx)
    {
        __test_add_args_t * arg = ctx.args<__test_add_args_t>();

        int32_t value = *arg->a + *arg->b;
        ctx.return_(value);
    }

    ////////// ////////// ////////// ////////// //////////

    // WriteInt32.
    struct __write_int32_t
    {
        rint32_t    v;
    };

    void write_int32(rtlib_context_t & ctx)
    {
        __write_int32_t * arg = ctx.args<__write_int32_t>();
        std::wcout << *arg->v;
    }

    ////////// ////////// ////////// ////////// //////////

    // WriteBoolean.
    struct __write_boolean_t
    {
        rbool_t    v;
    };

    void write_boolean(rtlib_context_t & ctx)
    {
        __write_boolean_t * arg = ctx.args<__write_boolean_t>();
        if(*arg->v)
            std::wcout << _T("true");
        else
            std::wcout << _T("false");
    }

    ////////// ////////// ////////// ////////// //////////

} } }


