
#include "expression_executor.h"

namespace X_ROOT_NS::modules::core {

    typedef expression_execute_error_code_t __e_t;

    #define __VT(name)   vtype_t::name##_

    ////////// ////////// ////////// ////////// //////////
    // unitary_expression_t

    // Returns vtype of unitary expression.
    vtype_t unitary_expression_t::get_vtype() const
    {
        vtype_t vtype0 = __super_t::get_vtype();
        if (vtype0 != vtype_t::__unknown__)
            return vtype0;

        expression_t * e = exp();
        if (e == nullptr)
            return vtype_t::__unknown__;

        vtype_t vtype = e->get_vtype();
        if (vtype == vtype_t::__unknown__)
            return vtype_t::__unknown__;

        return get_unitary_vtype(this->op(), vtype);
    }

    #define __C(type)                                                      \
        case (int)__VT(type):                                              \
            *out_value = unitary_operator_t<_op, __VT(type)>()((type##_t)v);    \
            return true;

    enum class __operate_region_t
    {
        none        =   0,

        default_    =   1 << 0,

        float_      =   1 << 1,

        bool_       =   1 << 2,

        char_       =   1 << 3,

        all         =   -1,
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    template<
        operator_t _op,
        __operate_region_t _flag, __operate_region_t _region
    >
    struct __unitary_execute_super_t;

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Unitary expression executor
    template<
         operator_t _op,
        __operate_region_t _region = __operate_region_t::all,

        typename __super_default_t =
            __unitary_execute_super_t<_op, __operate_region_t::default_, _region>,

        typename __super_float_t =
            __unitary_execute_super_t<_op, __operate_region_t::float_, _region>,

        typename __super_bool_t = 
            __unitary_execute_super_t<_op, __operate_region_t::bool_, _region>,

        typename __super_char_t =
            __unitary_execute_super_t<_op, __operate_region_t::char_, _region>
    >
    struct __unitary_execute_t : __super_default_t, __super_float_t, __super_bool_t
                               , __super_char_t
    {
        static bool try_execute(tvalue_t v, tvalue_t * out_value)
        {
            return __super_default_t::try_execute(v, out_value)
                || __super_float_t::try_execute(v, out_value)
                || __super_bool_t::try_execute(v, out_value)
                || __super_char_t::try_execute(v, out_value);
        }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Base class of unitary executor.
    template<
         operator_t _op,
        __operate_region_t _flag, __operate_region_t _region
    >
    struct __unitary_execute_super_t :
        __unitary_execute_t<
            _op,
            bit_has_flag(_region, _flag)? _flag : __operate_region_t::none
        >
    { };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Unitary executor ( For none operate region ).
    template<operator_t _op>
    struct __unitary_execute_t<_op, __operate_region_t::none>
    {
        static bool try_execute(tvalue_t v, tvalue_t * out_value)
        {
            return false;
        }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Unitary executor ( For default operate region ).
    template<operator_t _op>
    struct __unitary_execute_t<_op, __operate_region_t::default_>
    {
        static bool try_execute(tvalue_t v, tvalue_t * out_value)
        {
            switch ((int)v.type)
            {
                __C(int8)
                __C(uint8)
                __C(int16)
                __C(uint16)
                __C(int32)
                __C(uint32)
                __C(int64)
                __C(uint64)

                default:
                    return false;
            }
        }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Unitary executor ( For float types operate region .)
    template<operator_t _op>
    struct __unitary_execute_t<_op, __operate_region_t::float_>
    {
        static bool try_execute(tvalue_t v, tvalue_t * out_value)
        {
            switch ((int)v.type)
            {
                __C(float)
                __C(double)

                default:
                    return false;
            }
        }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Unitary executor ( For bool type operate region .)
    template<operator_t _op>
    struct __unitary_execute_t<_op, __operate_region_t::bool_>
    {
        static bool try_execute(tvalue_t v, tvalue_t * out_value)
        {
            switch ((int)v.type)
            {
                __C(bool)

                default:
                    return false;
            }
        }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Unitary Executor ( For char type operate region. )
    template<operator_t _op>
    struct __unitary_execute_t<_op, __operate_region_t::char_>
    {
        static bool try_execute(tvalue_t v, tvalue_t * out_value)
        {
            return false;
        }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    #undef __C

    // Executes unitary expression.
    template<operator_t _op, __operate_region_t _region = __operate_region_t::all>
    cvalue_t __unitary_execute(cvalue_t cv)
    {
        typedef __unitary_execute_t<_op, _region> executor_t;

        tvalue_t v;
        if (executor_t::try_execute(cv.number, &v))
            return cvalue_t(v);

        throw _ED(__e_t::invalid_unitary_operator, _op, cv.number.type);
    }

    // Executes unitary expression.
    cvalue_t unitary_expression_t::execute()
    {
        expression_t * e = exp();
        if (e == nullptr)
            return cvalue_t::null;

        cvalue_t cv = e->execute();
        if (!is_number(cv))
            return cvalue_t::nan;

        typedef __operate_region_t g_t;
        const __operate_region_t df = bit_xor(g_t::default_, g_t::float_);

        switch (this->op())
        {
            case operator_t::minus:
                return __unitary_execute<operator_t::minus, df>(cv);

            case operator_t::positive:
                return __unitary_execute<operator_t::positive, df>(cv);

            case operator_t::left_increment:
                return __unitary_execute<operator_t::left_increment, df>(cv);

            case operator_t::left_decrement:
                return __unitary_execute<operator_t::right_increment, df>(cv);

            case operator_t::right_increment:
                return __unitary_execute<operator_t::left_decrement, df>(cv);

            case operator_t::right_decrement:
                return __unitary_execute<operator_t::right_decrement, df>(cv);

            case operator_t::bit_not:
                return __unitary_execute<operator_t::bit_not, g_t::default_>(cv);

            case operator_t::logic_not:
                return __unitary_execute<operator_t::logic_not, g_t::bool_>(cv);

            default:
                return cvalue_t::nan;
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // binary_expression_t

    // Returns vtype of binary expression.
    vtype_t binary_expression_t::get_vtype() const
    {
        vtype_t vtype = __super_t::get_vtype();
        if (vtype != vtype_t::__unknown__)
            return vtype;

        expression_t * e1 = exp1(), * e2 = exp2();
        if (e1 == nullptr || e2 == nullptr)
            return vtype_t::__unknown__;

        vtype_t vtype1 = e1->get_vtype(), vtype2 = e2->get_vtype();

        if (vtype1 == vtype_t::__unknown__ || vtype2 == vtype_t::__unknown__)
            return vtype_t::__unknown__;

        return get_binary_vtype(this->op(), vtype1, vtype2);
    }

    #define __C1(type1, type2)                                                  \
        case ((int)__VT(type1) << 8) | (int)__VT(type2):                        \
            *out_value = tvalue_t(                                              \
                binary_operator_t<_op, __VT(type1), __VT(type2)>()(             \
                    (type1##_t)v1, (type2##_t)v2)                               \
            );                                                                  \
            return true;

    #define __C2(type1, type2)                                                  \
        __C1(type1, type2)                                                      \
        __C1(type2, type1)

    //#undef __DefineBinaryOperate

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Base class of binary executor.
    template<
        operator_t _op, __operate_region_t _flag, __operate_region_t _region
    >
    struct __binary_execute_super_t;

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Binary executor.
    template<
        operator_t _op,
        __operate_region_t _region = __operate_region_t::all,

        typename __super_default_t =
            __binary_execute_super_t<_op, __operate_region_t::default_, _region>,

        typename __super_float_t =
            __binary_execute_super_t<_op, __operate_region_t::float_, _region>,

        typename __super_bool_t = 
            __binary_execute_super_t<_op, __operate_region_t::bool_, _region>,

        typename __super_char_t = 
            __binary_execute_super_t<_op, __operate_region_t::char_, _region>
    >
    struct __binary_execute_t : __super_default_t, __super_float_t, __super_bool_t
                              , __super_char_t
    {
        static bool try_execute(tvalue_t v1, tvalue_t v2, tvalue_t * out_value)
        {
            return __super_default_t::try_execute(v1, v2, out_value)
                || __super_float_t::try_execute(v1, v2, out_value)
                || __super_bool_t::try_execute(v1, v2, out_value)
                || __super_char_t::try_execute(v1, v2, out_value);
        }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    template<operator_t _op, __operate_region_t _flag, __operate_region_t _region>
    struct __binary_execute_super_t :
        __binary_execute_t<
            _op, bit_has_flag(_region, _flag)? _flag : __operate_region_t::none
        >
    { };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Binary expression executor. ( For none region. )
    template<operator_t _op>
    struct __binary_execute_t<_op, __operate_region_t::none>
    {
        static bool try_execute(tvalue_t v1, tvalue_t v2, tvalue_t * out_value)
        {
            return false;
        }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Binary expression executor. ( For float region. )
    template<operator_t _op>
    struct __binary_execute_t<_op, __operate_region_t::float_>
    {
        static bool try_execute(tvalue_t v1, tvalue_t v2, tvalue_t * out_value)
        {
            switch (((int)v1.type << 8) | (int)v2.type)
            {
                __C2(float,     int8)
                __C2(float,     uint8)
                __C2(float,     int16)
                __C2(float,     uint16)
                __C2(float,     int32)
                __C2(float,     uint32)
                __C2(float,     int64)
                __C2(float,     uint64)
                __C1(float,     float)

                __C2(double,    int8)
                __C2(double,    uint8)
                __C2(double,    int16)
                __C2(double,    uint16)
                __C2(double,    int32)
                __C2(double,    uint32)
                __C2(double,    int64)
                __C2(double,    uint64)
                __C2(double,    float)
                __C1(double,    double)

                default:
                    return false;
            }
        }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Binary expression executor. ( For float char. )
    template<operator_t _op>
    struct __binary_execute_t<_op, __operate_region_t::char_>
    {
        static bool try_execute(tvalue_t v1, tvalue_t v2, tvalue_t * out_value)
        {
            switch (_op)
            {
                case operator_t::sub:
                case operator_t::equal:
                case operator_t::not_equal:

                    switch (((int)v1.type << 8) | (int)v2.type)
                    {
                        __C1(char, char)
                    }

                    // continue case operator_t::add

                case operator_t::add:
                case operator_t::greater:
                case operator_t::greater_equal:
                case operator_t::less:
                case operator_t::less_equal:

                    switch (((int)v1.type << 8) | (int)v2.type)
                    {
                        __C2(char,  int8);
                        __C2(char,  uint8);
                        __C2(char,  int16);
                        __C2(char,  uint16);
                        __C2(char,  int32);
                        __C2(char,  uint32);
                        __C2(char,  int64);
                        __C2(char,  uint64);
                    }

                    return false;
                    
                default:
                    return false;
            }
        }
       
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Binary expression executor. ( For default region. )
    template<operator_t _op>
    struct __binary_execute_t<_op, __operate_region_t::default_>
    {
        static bool try_execute(tvalue_t v1, tvalue_t v2, tvalue_t * out_value)
        {
            switch (((int)v1.type << 8) | (int)v2.type)
            {
                __C1(int8,      int8)
                __C2(int8,      uint8)

                __C1(uint8,     uint8)

                __C2(int16,     int8)
                __C2(int16,     uint8)
                __C1(int16,     int16)
                __C2(int16,     uint16)

                __C2(uint16,    int8)
                __C2(uint16,    uint8)
                __C1(uint16,    uint16)

                __C2(int32,     int8)
                __C2(int32,     uint8)
                __C2(int32,     int16)
                __C2(int32,     uint16)
                __C1(int32,     int32)

                __C2(uint32,    int8)
                __C2(uint32,    uint8)
                __C2(uint32,    int16)
                __C2(uint32,    uint16)
                __C2(uint32,    int32)
                __C1(uint32,    uint32)

                __C2(int64,     int8)
                __C2(int64,     uint8)
                __C2(int64,     int16)
                __C2(int64,     uint16)
                __C2(int64,     int32)
                __C2(int64,     uint32)
                __C1(int64,     int64)

                __C2(uint64,    int8)
                __C2(uint64,    uint8)
                __C2(uint64,    int16)
                __C2(uint64,    uint16)
                __C2(uint64,    int32)
                __C2(uint64,    uint32)
                __C2(uint64,    int64)
                __C1(uint64,    uint64)

                default:
                    return false;
            }
        }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Binary expression executor. ( For bool region. )
    template<operator_t _op>
    struct __binary_execute_t<_op, __operate_region_t::bool_>
    {
        static bool try_execute(tvalue_t v1, tvalue_t v2, tvalue_t * out_value)
        {
            switch (((int)v1.type << 8) | (int)v2.type)
            {
                __C1(bool, bool)

                default:
                    return false;
            }
        }
    };

    #undef __C

    // Executes a binary expression.
    template<operator_t _op, __operate_region_t _region=__operate_region_t::all>
    cvalue_t __binary_execute(cvalue_t cv1, cvalue_t cv2)
    {
        typedef __binary_execute_t<_op, _region> executor_t;

        tvalue_t value;
        if (executor_t::try_execute(cv1.number, cv2.number, &value))
            return cvalue_t(value);

        throw _ED(__e_t::invalid_binary_operator, _op, cv1.number.type, cv2.number.type);
    }

    // Executes a binary expression.
    cvalue_t binary_expression_t::execute()
    {
        expression_t * e1 = exp1(), * e2 = exp2();

        if (e1 == nullptr || e2 == nullptr)
            return cvalue_t::nan;

        cvalue_t cv1 = e1->execute(), cv2 = e2->execute();
        if (is_string(cv1) || is_string(cv2))
        {
            string_t s = _F(_T("%1%%2%"), cv1, cv2);
            return cvalue_t(__SPool.to_sid(s).c_str());
        }

        if (!is_number(cv1) || !is_number(cv2))
            return cvalue_t::nan;

        typedef __operate_region_t g_t;
        const __operate_region_t d   = g_t::default_;
        const __operate_region_t df  = bit_xor(g_t::default_, g_t::float_);
        const __operate_region_t dfc = bit_xor(g_t::default_, g_t::float_, g_t::char_);

        switch (this->op())
        {
            case operator_t::add:
                return __binary_execute<operator_t::add, dfc>(cv1, cv2);

            case operator_t::sub:
                return __binary_execute<operator_t::sub, dfc>(cv1, cv2);

            case operator_t::mul:
                return __binary_execute<operator_t::mul, df>(cv1, cv2);

            case operator_t::div:
                return __binary_execute<operator_t::div, df>(cv1, cv2);

            case operator_t::mod:
                return __binary_execute<operator_t::mod, d>(cv1, cv2);

            case operator_t::bit_and:
                return __binary_execute<operator_t::bit_and, d>(cv1, cv2);

            case operator_t::bit_or:
                return __binary_execute<operator_t::bit_or, d>(cv1, cv2);

            case operator_t::bit_xor:
                return __binary_execute<operator_t::bit_xor, d>(cv1, cv2);

            case operator_t::add_assign:
                return __binary_execute<operator_t::add_assign, df>(cv1, cv2);

            case operator_t::sub_assign:
                return __binary_execute<operator_t::sub_assign, df>(cv1, cv2);

            case operator_t::mul_assign:
                return __binary_execute<operator_t::mul_assign, df>(cv1, cv2);

            case operator_t::div_assign:
                return __binary_execute<operator_t::div_assign, df>(cv1, cv2);

            case operator_t::mod_assign:
                return __binary_execute<operator_t::mod_assign, d>(cv1, cv2);

            case operator_t::bit_and_assign:
                return __binary_execute<operator_t::bit_and_assign, d>(cv1, cv2);

            case operator_t::bit_or_assign:
                return __binary_execute<operator_t::bit_or_assign, d>(cv1, cv2);

            case operator_t::bit_xor_assign:
                return __binary_execute<operator_t::bit_xor_assign, d>(cv1, cv2);

            case operator_t::greater:
                return __binary_execute<operator_t::greater, df>(cv1, cv2);

            case operator_t::greater_equal:
                return __binary_execute<operator_t::greater_equal, df>(cv1, cv2);

            case operator_t::less:
                return __binary_execute<operator_t::less, df>(cv1, cv2);

            case operator_t::less_equal:
                return __binary_execute<operator_t::less_equal, df>(cv1, cv2);

            case operator_t::equal:
                return __binary_execute<operator_t::equal>(cv1, cv2);

            case operator_t::not_equal:
                return __binary_execute<operator_t::not_equal, df>(cv1, cv2);

            case operator_t::logic_and:
                return __binary_execute<operator_t::logic_and, g_t::bool_>(cv1, cv2);

            case operator_t::logic_or:
                return __binary_execute<operator_t::logic_or, g_t::bool_>(cv1, cv2);

            case operator_t::member_point:
                if (is_type_expression(e1))
                    return e2->execute();
                return cvalue_t::nan;

            case operator_t::assign:
            default:
                return cvalue_t::nan;
        }

        return cvalue_t::nan;
    }

    // Executes a binary expression.
    cvalue_t type_cast_expression_t::execute()
    {
        expression_t * exp = this->expression();
        if (exp == nullptr)
            return cvalue_t::nan;

        cvalue_t v = exp->execute();
        if (v.is_nan() || type_name == nullptr)
            return v;

        vtype_t vtype = this->get_vtype();
        return v.change_type(vtype);
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::core
