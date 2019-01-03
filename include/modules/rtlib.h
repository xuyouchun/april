#ifndef __RTLIB_H__
#define __RTLIB_H__

#include <common.h>
#include <algorithm.h>
#include <jc.h>

namespace X_ROOT_NS { namespace modules { namespace rtlib {

    struct rtlib_context_t;

    typedef void (*libfunc_t)(rtlib_context_t & ctx);

    libfunc_t get_libfunc(const string_t & name);

    namespace
    {
        typedef rt_stack_unit_t __unit_t;
    }

    ////////// ////////// ////////// ////////// //////////

    template<typename t> struct rt_arg_t
    {
        rt_arg_t() = default;
        rt_arg_t(t value) : __value(value) { }

        static const int unit_size = _alignf(sizeof(t), sizeof(__unit_t)) / sizeof(__unit_t);

        union
        {
            __unit_t __units[unit_size];
            t        __value;
        };

        operator t() { return (t)__value; }
        t operator *() { return (t)__value; }
    };

    //-------- ---------- ---------- ---------- ----------

    typedef rt_arg_t<const char_t *>    rstring_t;
    typedef rt_arg_t<char_t>            rchar_t;

    typedef rt_arg_t<int8_t>            rint8_t;
    typedef rt_arg_t<uint8_t>           ruint8_t;

    typedef rt_arg_t<int16_t>           rint16_t;
    typedef rt_arg_t<uint16_t>          ruint16_t;

    typedef rt_arg_t<int32_t>           rint32_t;
    typedef rt_arg_t<uint32_t>          ruint32_t;

    typedef rt_arg_t<int64_t>           rint64_t;
    typedef rt_arg_t<uint64_t>          ruint64_t;

    typedef rt_arg_t<float>             rfloat_t;
    typedef rt_arg_t<double>            rdouble_t;

    typedef rt_arg_t<bool>              rbool_t;
    typedef rt_arg_t<void *>            rptr_t;

    ////////// ////////// ////////// ////////// //////////

    struct rtlib_context_t
    {
        rtlib_context_t() = default;
        rtlib_context_t(rt_stack_unit_t * argument) : argument(argument) { }

        rt_stack_unit_t * argument        = nullptr;
        int               ret_code        = 0;
        string_t          ret_message;

        template<typename t> t * args()
        {
            return (t *)argument;
        }

        template<typename t> void return_(t value)
        {
            typedef rt_arg_t<t> rt;
            *(rt *)argument = rt(value);
        }
    };

    ////////// ////////// ////////// ////////// //////////

    #define __DefineFunc(name) void name(rtlib_context_t & ctx)

    // system
    __DefineFunc(write);
    __DefineFunc(test_add);
    __DefineFunc(write_int32);
    __DefineFunc(write_boolean);

    ////////// ////////// ////////// ////////// //////////

} } }

#endif  // __RTLIB_H__
