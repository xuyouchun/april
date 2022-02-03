#include <rtlib.h>
#include <mm.h>
#include <cmath>

namespace X_ROOT_NS::modules::rtlib {

    ////////// ////////// ////////// ////////// //////////

    // Math arguments
    struct __math_args_t
    {
        // Empty.
    };

    #define __BeginMathFunction_1(_name)                                                \
        struct __math_##_name##_args_t : __math_args_t                                  \
        {                                                                               \
            rdouble_t   val;                                                            \
        };                                                                              \
                                                                                        \
        void math_##_name(rtlib_context_t & ctx)                                        \
        {                                                                               \
            typedef __math_##_name##_args_t args_t;                                     \
            args_t * args = ctx.args<args_t>();                                         \
            double val = *args->val;                                                    \
                                                                                        \
            ctx.return_(


    #define __BeginMathFunction_2(_name)                                                \
        struct __math_##_name##_args_t : __math_args_t                                  \
        {                                                                               \
            rdouble_t   x;                                                              \
            rdouble_t   y;                                                              \
        };                                                                              \
                                                                                        \
        void math_##_name(rtlib_context_t & ctx)                                        \
        {                                                                               \
            typedef __math_##_name##_args_t args_t;                                     \
            args_t * args = ctx.args<args_t>();                                         \
            double x = *args->x;                                                        \
            double y = *args->y;                                                        \
                                                                                        \
            ctx.return_(

    #define __EndMathFunction()                                                         \
            );                                                                          \
        }


    #define __MathFunction_1(_name)                                                     \
        __BeginMathFunction_1(_name)                                                    \
            std::_name(val)                                                             \
        __EndMathFunction()

    #define __MathFunction_2(_name)                                                     \
        __BeginMathFunction_2(_name)                                                    \
            std::_name(x, y)                                                            \
        __EndMathFunction()

    ////////// ////////// ////////// ////////// //////////

    __MathFunction_1(sqrt)
    __MathFunction_2(pow)

    __MathFunction_1(sin)
    __MathFunction_1(cos)
    __MathFunction_1(tan)

    __MathFunction_1(asin)
    __MathFunction_1(acos)
    __MathFunction_1(atan)

    __MathFunction_1(sinh)
    __MathFunction_1(cosh)
    __MathFunction_1(tanh)

    __MathFunction_1(asinh)
    __MathFunction_1(acosh)
    __MathFunction_1(atanh)

    __MathFunction_1(log)
    __MathFunction_1(log10)

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::rtlib

