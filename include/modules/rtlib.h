#ifndef __RTLIB_H__
#define __RTLIB_H__

#include <common.h>
#include <algorithm.h>
#include <jc.h>
#include <rt.h>

namespace X_ROOT_NS { namespace modules { namespace rtlib {

    using namespace rt;

    struct rtlib_context_t;

    typedef void (*libfunc_t)(rtlib_context_t & ctx);

    // Gets libfunc of specified name.
    libfunc_t get_libfunc(const string_t & name);

    namespace
    {
        typedef rt_stack_unit_t __unit_t;
    }

    ////////// ////////// ////////// ////////// //////////

    // Runtime argument.
    template<typename t> struct rt_arg_t
    {
        // Constructor.
        rt_arg_t() = default;
        rt_arg_t(t value) : __value(value) { }

        static const int unit_size = _alignf(sizeof(t), sizeof(__unit_t)) / sizeof(__unit_t);

        union
        {
            __unit_t __units[unit_size];
            t        __value;
        };

        // Returns the raw value.
        operator t() { return (t)__value; }
        t operator *() { return (t)__value; }
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime times.

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

    typedef rt_arg_t<rt_ref_t>          robject_t;

    ////////// ////////// ////////// ////////// //////////

    // Runtime lib context.
    struct rtlib_context_t
    {
        // Constructor.
        rtlib_context_t(assembly_analyzer_t & analyzer, rt_stack_unit_t * argument) _NE
                : analyzer(analyzer), argument(argument) { }

        assembly_analyzer_t & analyzer;
        rt_stack_unit_t     * argument;
        int                   ret_code        = 0;
        string_t              ret_message;

        // Arguments.
        template<typename t> t * args() _NE
        {
            return (t *)argument;
        }

        // Returns specified value.
        template<typename t> void return_(t value) _NE
        {
            typedef rt_arg_t<t> rt;
            *(rt *)argument = rt(value);
        }
    };

    ////////// ////////// ////////// ////////// //////////

    #define __DefineFunc(name) void name(rtlib_context_t & ctx)

    // System functions.
    __DefineFunc(console_write_string);
    __DefineFunc(console_write_char);
    __DefineFunc(console_write_int8);
    __DefineFunc(console_write_uint8);
    __DefineFunc(console_write_int16);
    __DefineFunc(console_write_uint16);
    __DefineFunc(console_write_int32);
    __DefineFunc(console_write_uint32);
    __DefineFunc(console_write_int64);
    __DefineFunc(console_write_uint64);
    __DefineFunc(console_write_float);
    __DefineFunc(console_write_double);

    __DefineFunc(console_write_string_line);
    __DefineFunc(console_write_char_line);
    __DefineFunc(console_write_int8_line);
    __DefineFunc(console_write_uint8_line);
    __DefineFunc(console_write_int16_line);
    __DefineFunc(console_write_uint16_line);
    __DefineFunc(console_write_int32_line);
    __DefineFunc(console_write_uint32_line);
    __DefineFunc(console_write_int64_line);
    __DefineFunc(console_write_uint64_line);
    __DefineFunc(console_write_float_line);
    __DefineFunc(console_write_double_line);

    // Array functions.
    __DefineFunc(array_get_rank);
    __DefineFunc(array_get_length);
    __DefineFunc(array_get_length_of_dimension);

    // Tuple functions.
    __DefineFunc(tuple_set_range);
    __DefineFunc(tuple_get);
    __DefineFunc(tuple_get_count);

    // Reflection functions.
    __DefineFunc(reflection_get_type);
    __DefineFunc(reflection_get_type_name);

    ////////// ////////// ////////// ////////// //////////

} } }

#endif  // __RTLIB_H__
