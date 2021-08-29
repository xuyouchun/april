#ifndef __CORE_VTYPE_TRAITS_H__
#define __CORE_VTYPE_TRAITS_H__

namespace X_ROOT_NS { namespace modules { namespace core {

    #define __VT(name)   vtype_t::name##_

    ////////// ////////// ////////// ////////// //////////

    // Picks first type that not void.
    template<typename ... ts>
    using __pick_type_t = pick_type_t<ts ...>;

    namespace
    {
        #define __VTypeT static const vtype_t

        template<vtype_t v, vtype_t ... vs> struct __vtypes_t { __VTypeT vtype = v; };
        template<vtype_t ... vs> struct __vtypes_t<vtype_t::void_, vs ...> : __vtypes_t<vs ...> { };
        template<> struct __vtypes_t<vtype_t::void_> { __VTypeT vtype = vtype_t::void_; };

        #undef __VTypeT

        struct __runtime_ptr_t { };

        typedef struct { } * mobject, * mobject_t;
        typedef struct { } * ptr, * ptr_t;
    }

    template<vtype_t ... vs>
    constexpr vtype_t __pick_vtype = __vtypes_t<vs ...>::vtype;

    ////////// ////////// ////////// ////////// //////////

    // Defines each vtypes.
    #define __EachVTypes                \
        __VType(int8)                   \
        __VType(uint8)                  \
        __VType(int16)                  \
        __VType(uint16)                 \
        __VType(int32)                  \
        __VType(uint32)                 \
        __VType(int64)                  \
        __VType(uint64)                 \
        __VType(float)                  \
        __VType(double)                 \
        __VType(bool)                   \
        __VType(char)                   \
        __VType(mobject)                \
        __VType(ptr)

    ////////// ////////// ////////// ////////// //////////

    // Defines binary operations.
    #define __EachBinaryOperators               \
        __Op(operator_t::add)                   \
        __Op(operator_t::sub)                   \
        __Op(operator_t::mul)                   \
        __Op(operator_t::div)                   \
        __Op(operator_t::mod)                   \
        __Op(operator_t::add_assign)            \
        __Op(operator_t::sub_assign)            \
        __Op(operator_t::mul_assign)            \
        __Op(operator_t::div_assign)            \
        __Op(operator_t::mod_assign)            \
        __Op(operator_t::left_shift)            \
        __Op(operator_t::right_shift)           \
        __Op(operator_t::left_shift_assign)     \
        __Op(operator_t::right_shift_assign)    \
        __Op(operator_t::bit_and)               \
        __Op(operator_t::bit_or)                \
        __Op(operator_t::bit_xor)               \
        __Op(operator_t::bit_and_assign)        \
        __Op(operator_t::bit_or_assign)         \
        __Op(operator_t::bit_xor_assign)        \
        __Op(operator_t::greater)               \
        __Op(operator_t::greater_equal)         \
        __Op(operator_t::less)                  \
        __Op(operator_t::less_equal)            \
        __Op(operator_t::equal)                 \
        __Op(operator_t::not_equal)             \
        __Op(operator_t::logic_and)             \
        __Op(operator_t::logic_or)

    // Defines unitary operations.
    #define __EachUnitaryOperators              \
        __Op(operator_t::left_increment)        \
        __Op(operator_t::right_increment)       \
        __Op(operator_t::left_decrement)        \
        __Op(operator_t::right_decrement)       \
        __Op(operator_t::minus)                 \
        __Op(operator_t::positive)              \
        __Op(operator_t::bit_not)               \
        __Op(operator_t::logic_not)

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        // vtype trait
        template<vtype_t _vtype>
        struct __vtype_trait_t
        {
            static const vtype_t vtype = _vtype;
            typedef void numeric_t;
        };

        // Defines vtype traits.
        #define __DefineVTypeTrait(name)                            \
            template<>                                              \
            struct __vtype_trait_t<vtype_t::name##_>                \
            {                                                       \
                static const vtype_t vtype = vtype_t::name##_;      \
                typedef name##_t numeric_t;                         \
            };

        #define __VType __DefineVTypeTrait
        __EachVTypes
        #undef __VType

        template<vtype_t vtype>
        using __vnum_t = typename __vtype_trait_t<vtype>::numeric_t;
    }

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        template<typename numeric_t> struct __rvtype_trait_t { };

        // Defines rvtype traits.
        #define __DefineRVTypeTrait(name)                           \
            template<>                                              \
            struct __rvtype_trait_t<name##_t>                       \
            {                                                       \
                static const vtype_t vtype = vtype_t::name##_;      \
                typedef name##_t numeric_t;                         \
            };

        #define __VType __DefineRVTypeTrait
        __EachVTypes
        #undef __VType
    }

    // vtype of numeric.
    template<typename numeric_t>
    constexpr vtype_t vtype = __rvtype_trait_t<numeric_t>::vtype;

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        template<operator_t> struct __unitary_op_t { };

        #define __BeginDefineUnitaryOp(name, op)                    \
            template<> struct __unitary_op_t<operator_t::name>      \
            {                                                       \
                template<typename vnum_t>                           \
                auto operator()(vnum_t v)                           \
                {

        #define __EndDefineUnitaryOp()                              \
                }                                                   \
            };

        #define __DefineUnitaryOp(name, op)                         \
            __BeginDefineUnitaryOp(name, op)                        \
                return op v;                                        \
            __EndDefineUnitaryOp()

        // Unitary operations.
        __DefineUnitaryOp(positive, +) 
        __DefineUnitaryOp(minus, -) 
        __DefineUnitaryOp(bit_not, ~) 
        __DefineUnitaryOp(logic_not, !) 
        __DefineUnitaryOp(left_increment, ++) 
        __DefineUnitaryOp(left_decrement, --) 

        #define __DefineUnitaryOpSuffix(name, op)                   \
            __BeginDefineUnitaryOp(name, op)                        \
                return v op;                                        \
            __EndDefineUnitaryOp()

        __DefineUnitaryOpSuffix(right_increment, ++)
        __DefineUnitaryOpSuffix(right_decrement, --)

        #undef __DefineUnitaryOpSuffix
        #undef __BeginDefineUnitaryOp
        #undef __EndDefineUnitaryOp
        #undef __DefineUnitaryOp
    }

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        template<operator_t> struct __binary_op_t { };

        #define __DefineBinaryOp(name, op)                          \
            template<> struct __binary_op_t<operator_t::name>       \
            {                                                       \
                template<typename vnum1_t, typename vnum2_t>        \
                auto operator()(vnum1_t v1, vnum2_t v2)             \
                {                                                   \
                    return v1 op v2;                                \
                }                                                   \
            };

        // Defines binary operations.
        __DefineBinaryOp(add, +)
        __DefineBinaryOp(sub, -)
        __DefineBinaryOp(mul, *)
        __DefineBinaryOp(div, /)
        __DefineBinaryOp(mod, %)

        __DefineBinaryOp(add_assign, +=)
        __DefineBinaryOp(sub_assign, -=)
        __DefineBinaryOp(mul_assign, *=)
        __DefineBinaryOp(div_assign, /=)
        __DefineBinaryOp(mod_assign, %=)

        __DefineBinaryOp(greater, >)
        __DefineBinaryOp(greater_equal, >=)
        __DefineBinaryOp(less, <)
        __DefineBinaryOp(less_equal, <=)
        __DefineBinaryOp(equal, ==)
        __DefineBinaryOp(not_equal, !=)

        __DefineBinaryOp(bit_and, &)
        __DefineBinaryOp(bit_or, |)
        __DefineBinaryOp(bit_xor, ^)
        __DefineBinaryOp(left_shift, <<)
        __DefineBinaryOp(right_shift, >>)

        __DefineBinaryOp(bit_and_assign, &=)
        __DefineBinaryOp(bit_or_assign, |=)
        __DefineBinaryOp(bit_xor_assign, ^=)
        __DefineBinaryOp(left_shift_assign, <<=)
        __DefineBinaryOp(right_shift_assign, >>=)

        __DefineBinaryOp(logic_and, &&)
        __DefineBinaryOp(logic_or, ||)
    }

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        // Base class of unitary operation.
        template<operator_t _op, vtype_t _t, vtype_t _t1>
        struct __unitary_operator_base_t
        {
            static const operator_t op = _op;

            static const vtype_t t1 = _t1;
            static const vtype_t t  = _t;

            typedef __vnum_t<t1> vnum1_t;
            typedef __vnum_t<t>  vnum_t;

            typedef __unitary_op_t<_op> __op_t;

            vnum_t operator() (vnum1_t v1)
            {
                return (vnum_t)__op_t()(v1);
            }
        };

        //-------- ---------- ---------- ---------- ----------

        // Unitary operation.
        template<operator_t _op, vtype_t _t1>
        struct __unitary_operator_t
            : __unitary_operator_base_t<_op, vtype_t::__unknown__, _t1>
        {
            typedef __unitary_operator_base_t<_op, vtype_t::__unknown__, _t1> __super_t;

            using typename __super_t::vnum1_t;
            using typename __super_t::vnum_t;

            int operator() (vnum1_t v1) { return 0; }
        };

        #define __DefineUnitaryOperation(_op, _t, _t1)                                      \
            template<>                                                                      \
            struct __unitary_operator_t<_op, _t1>                                           \
                : __unitary_operator_base_t<_op, _t, _t1>                                   \
            { };

        #define __U1(op, vret, vtype1)                                                      \
            __UnitaryOp(operator_t::op, vret, __VT(vtype1))

        #define __IncrementOps(op)                                                          \
            __U1(op, __VT(int8),    int8)                                                   \
            __U1(op, __VT(uint8),   uint8)                                                  \
            __U1(op, __VT(int16),   int16)                                                  \
            __U1(op, __VT(uint16),  uint16)                                                 \
            __U1(op, __VT(int32),   int32)                                                  \
            __U1(op, __VT(uint32),  uint32)                                                 \
            __U1(op, __VT(int64),   int64)                                                  \
            __U1(op, __VT(uint64),  uint64)                                                 \
            __U1(op, __VT(float),   float)                                                  \
            __U1(op, __VT(double),  double)                                                 \


        #define X_DEFINE_UNITARY_OPS                                                        \
            __U1(positive,  __VT(int32),    int8)                                           \
            __U1(positive,  __VT(int32),    uint8)                                          \
            __U1(positive,  __VT(int32),    int16)                                          \
            __U1(positive,  __VT(int32),    uint16)                                         \
            __U1(positive,  __VT(int32),    int32)                                          \
            __U1(positive,  __VT(uint32),   uint32)                                         \
            __U1(positive,  __VT(int64),    int64)                                          \
            __U1(positive,  __VT(uint64),   uint64)                                         \
            __U1(positive,  __VT(float),    float)                                          \
            __U1(positive,  __VT(double),   double)                                         \
                                                                                            \
            __U1(minus,     __VT(int32),    int8)                                           \
            __U1(minus,     __VT(int32),    uint8)                                          \
            __U1(minus,     __VT(int32),    int16)                                          \
            __U1(minus,     __VT(int32),    uint16)                                         \
            __U1(minus,     __VT(int32),    int32)                                          \
            __U1(minus,     __VT(int64),    uint32)                                         \
            __U1(minus,     __VT(int64),    int64)                                          \
            __U1(minus,     __VT(float),    float)                                          \
            __U1(minus,     __VT(double),   double)                                         \
                                                                                            \
            __U1(bit_not,  __VT(int32),    int8)                                            \
            __U1(bit_not,  __VT(int32),    uint8)                                           \
            __U1(bit_not,  __VT(int32),    int16)                                           \
            __U1(bit_not,  __VT(int32),    uint16)                                          \
            __U1(bit_not,  __VT(int32),    int32)                                           \
            __U1(bit_not,  __VT(uint32),   uint32)                                          \
            __U1(bit_not,  __VT(int64),    int64)                                           \
            __U1(bit_not,  __VT(uint64),   uint64)                                          \
            __U1(bit_not,  __VT(float),    float)                                           \
            __U1(bit_not,  __VT(double),   double)                                          \
                                                                                            \
            __U1(logic_not, __VT(bool),   bool)                                             \
                                                                                            \
            __IncrementOps(left_increment)                                                  \
            __IncrementOps(right_increment)                                                 \
            __IncrementOps(left_decrement)                                                  \
            __IncrementOps(right_decrement)

        #define __UnitaryOp __DefineUnitaryOperation
        X_DEFINE_UNITARY_OPS
        #undef __UnitaryOp
    }

    // Returns vtype of unitary expression.
    constexpr vtype_t get_unitary_vtype(operator_t op, vtype_t vtype)
    {
        #define __V(op, vtype)  (((uint32_t)op << 16) | (uint32_t)vtype)

        switch (__V(op, vtype))
        {
            #define __UnitaryOp(op, r, t1)                                                  \
                case __V(op, t1):                                                           \
                    return __unitary_operator_t<op, t1>::t;

            X_DEFINE_UNITARY_OPS
            #undef __UnitaryOp

            default:
                return vtype_t::__unknown__;
        }

        #undef __V
    }

    #undef __U1

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        // Base class of binary operation.
        template<operator_t _op, vtype_t _t, vtype_t _t1, vtype_t _t2>
        struct __binary_operator_base_t
        {
            static const operator_t op = _op;

            static const vtype_t t1 = _t1;
            static const vtype_t t2 = _t2;
            static const vtype_t t  = _t;

            typedef __vnum_t<t1> vnum1_t;
            typedef __vnum_t<t2> vnum2_t;
            typedef __vnum_t<t>  vnum_t;

            typedef __binary_op_t<_op> __op_t;

            vnum_t operator() (vnum1_t v1, vnum2_t v2)
            {
                return (vnum_t)__op_t()(v1, v2);
            }
        };

        //-------- ---------- ---------- ---------- ----------

        // Binary operation.
        template<operator_t _op, vtype_t _t1, vtype_t _t2>
        struct __binary_operator_t
            : __binary_operator_base_t<_op, vtype_t::__unknown__, _t1, _t2>
        {
            typedef __binary_operator_base_t<_op, vtype_t::__unknown__, _t1, _t2> __super_t;

            using typename __super_t::vnum1_t;
            using typename __super_t::vnum2_t;
            using typename __super_t::vnum_t;

            int operator() (vnum1_t v1, vnum2_t v2) { return 0; }
        };

        #define __DefineBinaryOperation(_op, _t, _t1, _t2)                                  \
            template<>                                                                      \
            struct __binary_operator_t<_op, _t1, _t2>                                       \
                : __binary_operator_base_t<_op, _t, _t1, _t2>                               \
            { };

        const vtype_t __use_vtype1 = (vtype_t)-100;
        const vtype_t __use_vtype2 = (vtype_t)-101;
        const vtype_t __use_default_ret = vtype_t::void_;

        // Returns return vtype.
        static constexpr vtype_t __pick_ret_vtype(vtype_t ret, vtype_t vtype1, vtype_t vtype2)
        {
            if (ret == __use_vtype1)
                return vtype1;

            if (ret == __use_vtype2)
                return vtype2;

            return ret;
        }

        #define __B1(op, vret, vtype1, vtype2)                                              \
            __BinaryOp(operator_t::op,                                                      \
                __pick_ret_vtype(vret, __VT(vtype1), __VT(vtype2)),                         \
                __VT(vtype1), __VT(vtype2)                                                  \
            )

        #define __B2(op, vret, vtype1, vtype2)                                              \
            __B1(op, vret, vtype1, vtype2)                                                  \
            __B1(op, vret, vtype2, vtype1)


        #define __PickVType(ret1, ret2) (__pick_vtype<ret1, __VT(ret2)>)

        #define __IntOps(op, r)                                                             \
            __B1(op, __PickVType(r, int32), int8,   int8)                                   \
            __B2(op, __PickVType(r, int32), int8,   uint8)                                  \
                                                                                            \
            __B1(op, __PickVType(r, int32), uint8,  uint8)                                  \
                                                                                            \
            __B2(op, __PickVType(r, int32), int16,  int8)                                   \
            __B2(op, __PickVType(r, int32), int16,  uint8)                                  \
            __B1(op, __PickVType(r, int32), int16,  int16)                                  \
            __B2(op, __PickVType(r, int32), int16,  uint16)                                 \
                                                                                            \
            __B2(op, __PickVType(r, int32), uint16, int8)                                   \
            __B2(op, __PickVType(r, int32), uint16, uint8)                                  \
            __B1(op, __PickVType(r, int32), uint16, uint16)                                 \
                                                                                            \
            __B2(op, __PickVType(r, int32), int32,  int8)                                   \
            __B2(op, __PickVType(r, int32), int32,  uint8)                                  \
            __B2(op, __PickVType(r, int32), int32,  int16)                                  \
            __B2(op, __PickVType(r, int32), int32,  uint16)                                 \
            __B1(op, __PickVType(r, int32), int32,  int32)                                  \
                                                                                            \
            __B2(op, __PickVType(r, int64),  uint32, int8)                                  \
            __B2(op, __PickVType(r, uint32), uint32, uint8)                                 \
            __B2(op, __PickVType(r, int64),  uint32, int16)                                 \
            __B2(op, __PickVType(r, uint32), uint32, uint16)                                \
            __B2(op, __PickVType(r, int64),  uint32, int32)                                 \
            __B1(op, __PickVType(r, uint32), uint32, uint32)                                \
                                                                                            \
            __B2(op, __PickVType(r, int64), int64,  int8)                                   \
            __B2(op, __PickVType(r, int64), int64,  uint8)                                  \
            __B2(op, __PickVType(r, int64), int64,  int16)                                  \
            __B2(op, __PickVType(r, int64), int64,  uint16)                                 \
            __B2(op, __PickVType(r, int64), int64,  int32)                                  \
            __B2(op, __PickVType(r, int64), int64,  uint32)                                 \
            __B1(op, __PickVType(r, int64), int64,  int64)                                  \
                                                                                            \
            __B2(op, __PickVType(r, uint64), uint64,  int8)                                 \
            __B2(op, __PickVType(r, uint64), uint64,  uint8)                                \
            __B2(op, __PickVType(r, uint64), uint64,  int16)                                \
            __B2(op, __PickVType(r, uint64), uint64,  uint16)                               \
            __B2(op, __PickVType(r, uint64), uint64,  int32)                                \
            __B2(op, __PickVType(r, uint64), uint64,  uint32)                               \
            __B2(op, __PickVType(r, uint64), uint64,  int64)                                \
            __B1(op, __PickVType(r, uint64), uint64,  uint64)

        #define __FloatOps(op, r)                                                           \
            __B2(op, __PickVType(r, float), float,  int8)                                   \
            __B2(op, __PickVType(r, float), float,  uint8)                                  \
            __B2(op, __PickVType(r, float), float,  int16)                                  \
            __B2(op, __PickVType(r, float), float,  uint16)                                 \
            __B2(op, __PickVType(r, float), float,  int32)                                  \
            __B2(op, __PickVType(r, float), float,  uint32)                                 \
            __B2(op, __PickVType(r, float), float,  int64)                                  \
            __B2(op, __PickVType(r, float), float,  uint64)                                 \
            __B1(op, __PickVType(r, float), float,  float)                                  \
                                                                                            \
            __B2(op, __PickVType(r, double), double,  int8)                                 \
            __B2(op, __PickVType(r, double), double,  uint8)                                \
            __B2(op, __PickVType(r, double), double,  int16)                                \
            __B2(op, __PickVType(r, double), double,  uint16)                               \
            __B2(op, __PickVType(r, double), double,  int32)                                \
            __B2(op, __PickVType(r, double), double,  uint32)                               \
            __B2(op, __PickVType(r, double), double,  int64)                                \
            __B2(op, __PickVType(r, double), double,  uint64)                               \
            __B2(op, __PickVType(r, double), double,  float)                                \
            __B1(op, __PickVType(r, double), double,  double)

        #define __CharOps(op, r)                                                            \
            __B1(op, __PickVType(r, char), char, int8)                                      \
            __B1(op, __PickVType(r, char), char, uint8)                                     \
            __B1(op, __PickVType(r, char), char, int16)                                     \
            __B1(op, __PickVType(r, char), char, uint16)                                    \
            __B1(op, __PickVType(r, char), char, int32)                                     \
            __B1(op, __PickVType(r, char), char, uint32)                                    \
            __B1(op, __PickVType(r, char), char, int64)                                     \
            __B1(op, __PickVType(r, char), char, uint64)

        #define __CharOps2(op, r)                                                           \
            __B2(op, __PickVType(r, bool), char, int8)                                      \
            __B2(op, __PickVType(r, bool), char, uint8)                                     \
            __B2(op, __PickVType(r, bool), char, int16)                                     \
            __B2(op, __PickVType(r, bool), char, uint16)                                    \
            __B2(op, __PickVType(r, bool), char, int32)                                     \
            __B2(op, __PickVType(r, bool), char, uint32)                                    \
            __B2(op, __PickVType(r, bool), char, int64)                                     \
            __B2(op, __PickVType(r, bool), char, uint64)                                    \
            __B1(op, __PickVType(r, bool), char, char)


        #define __BoolOps(op, r)                                                            \
            __B1(op, __PickVType(r, bool), bool, bool)

        #define __ObjectOps(op, r)                                                          \
            __B1(op, __PickVType(r, mobject), mobject, mobject)

        #define __PtrOps(op, r)                                                             \
            __B1(op, __PickVType(r, ptr), ptr, ptr)

        #define __AlIntBinaryOps(op, r)     __IntOps(op, r)
        #define __AlFloatBinaryOps(op, r)   __FloatOps(op, r)
        #define __AlCharBinaryOps(op, r)    __CharOps(op, r)
        #define __BitIntBinaryOps(op, r)    __IntOps(op, r)
        #define __CmpIntBinaryOps(op)       __IntOps(op, vtype_t::bool_)
        #define __CmpFloatBinaryOps(op)     __FloatOps(op, vtype_t::bool_)
        #define __CmpBoolBinaryOps(op)      __BoolOps(op, vtype_t::bool_)
        #define __CmpCharBinaryOps(op)      __CharOps2(op, vtype_t::void_)
        #define __CmpObjectBinaryOps(op)    __ObjectOps(op, vtype_t::mobject_)
        #define __CmpPtrBinaryOps(op)       __PtrOps(op, vtype_t::ptr_)
        #define __LogicBoolBinaryOps(op)    __BoolOps(op, vtype_t::bool_)

        #define X_DEFINE_BINARY_OPS                                                         \
                                                                                            \
        /* Al */                                                                            \
        __AlIntBinaryOps(add, __use_default_ret)                                            \
        __AlIntBinaryOps(sub, __use_default_ret)                                            \
        __AlIntBinaryOps(mul, __use_default_ret)                                            \
        __AlIntBinaryOps(div, __use_default_ret)                                            \
        __AlIntBinaryOps(mod, __use_default_ret)                                            \
                                                                                            \
        __AlIntBinaryOps(add_assign, __use_vtype1)                                          \
        __AlIntBinaryOps(sub_assign, __use_vtype1)                                          \
        __AlIntBinaryOps(mul_assign, __use_vtype1)                                          \
        __AlIntBinaryOps(div_assign, __use_vtype1)                                          \
        __AlIntBinaryOps(mod_assign, __use_vtype1)                                          \
                                                                                            \
        __AlFloatBinaryOps(add, __use_default_ret)                                          \
        __AlFloatBinaryOps(sub, __use_default_ret)                                          \
        __AlFloatBinaryOps(mul, __use_default_ret)                                          \
        __AlFloatBinaryOps(div, __use_default_ret)                                          \
                                                                                            \
        __AlFloatBinaryOps(add_assign, __use_vtype1)                                        \
        __AlFloatBinaryOps(sub_assign, __use_vtype1)                                        \
        __AlFloatBinaryOps(mul_assign, __use_vtype1)                                        \
        __AlFloatBinaryOps(div_assign, __use_vtype1)                                        \
                                                                                            \
        __AlCharBinaryOps(add, __use_default_ret)                                           \
        __AlCharBinaryOps(sub, __use_default_ret)                                           \
        __B1(sub, __VT(int32), char, char)                                                  \
                                                                                            \
        /* Bit */                                                                           \
        __BitIntBinaryOps(bit_and, __use_default_ret)                                       \
        __BitIntBinaryOps(bit_or,  __use_default_ret)                                       \
        __BitIntBinaryOps(bit_xor, __use_default_ret)                                       \
        __BitIntBinaryOps(left_shift, __use_default_ret)                                    \
        __BitIntBinaryOps(right_shift, __use_default_ret)                                   \
                                                                                            \
        __BitIntBinaryOps(bit_and_assign, __use_vtype1)                                     \
        __BitIntBinaryOps(bit_or_assign,  __use_vtype1)                                     \
        __BitIntBinaryOps(bit_xor_assign, __use_vtype1)                                     \
        __BitIntBinaryOps(left_shift_assign, __use_vtype1)                                  \
        __BitIntBinaryOps(right_shift_assign, __use_vtype1)                                 \
                                                                                            \
        /* Cmp */                                                                           \
        __CmpIntBinaryOps(greater)                                                          \
        __CmpIntBinaryOps(greater_equal)                                                    \
        __CmpIntBinaryOps(less)                                                             \
        __CmpIntBinaryOps(less_equal)                                                       \
        __CmpIntBinaryOps(equal)                                                            \
        __CmpIntBinaryOps(not_equal)                                                        \
                                                                                            \
        __CmpFloatBinaryOps(greater)                                                        \
        __CmpFloatBinaryOps(greater_equal)                                                  \
        __CmpFloatBinaryOps(less)                                                           \
        __CmpFloatBinaryOps(less_equal)                                                     \
        __CmpFloatBinaryOps(equal)                                                          \
        __CmpFloatBinaryOps(not_equal)                                                      \
                                                                                            \
        __CmpCharBinaryOps(greater)                                                         \
        __CmpCharBinaryOps(greater_equal)                                                   \
        __CmpCharBinaryOps(less)                                                            \
        __CmpCharBinaryOps(less_equal)                                                      \
        __CmpCharBinaryOps(equal)                                                           \
        __CmpCharBinaryOps(not_equal)                                                       \
                                                                                            \
        __CmpBoolBinaryOps(equal)                                                           \
        __CmpBoolBinaryOps(not_equal)                                                       \
                                                                                            \
        __CmpObjectBinaryOps(equal)                                                         \
        __CmpObjectBinaryOps(not_equal)                                                     \
                                                                                            \
        __CmpPtrBinaryOps(equal)                                                            \
        __CmpPtrBinaryOps(not_equal)                                                        \
                                                                                            \
        /* Logic */                                                                         \
        __LogicBoolBinaryOps(logic_and)                                                     \
        __LogicBoolBinaryOps(logic_or)

        #define __BinaryOp    __DefineBinaryOperation
        X_DEFINE_BINARY_OPS
        #undef __BinaryOp
    }

    // Returns vtype of binary expression.
    constexpr vtype_t get_binary_vtype(operator_t op, vtype_t vtype1, vtype_t vtype2)
    {
        #define __V(op, vtype1, vtype2) \
            (((uint32_t)op << 16) | ((uint32_t)vtype1 << 8) | (uint32_t)vtype2)

        switch (op)
        {
            case operator_t::member_point:
                return vtype2;

            case operator_t::assign:
                return vtype1;

            default:
                switch (__V(op, vtype1, vtype2))
                {
                    #define __BinaryOp(op, r, t1, t2)                                   \
                        case __V(op, t1, t2):                                           \
                            return __binary_operator_t<op, t1, t2>::t;

                    X_DEFINE_BINARY_OPS
                    #undef __BinaryOp

                    default:
                        return vtype_t::__unknown__;
                }
        }

        #undef __V
    }

    #undef __B1

    ////////// ////////// ////////// ////////// //////////

    // Returns operator of a xil_al_command.
    constexpr operator_t to_operator(xil_al_command_t cmd)
    {
        switch (cmd)
        {
            case xil_al_command_t::add:
                return operator_t::add;

            case xil_al_command_t::sub:
                return operator_t::sub;

            case xil_al_command_t::mul:
                return operator_t::mul;

            case xil_al_command_t::div:
                return operator_t::div;

            case xil_al_command_t::mod:
                return operator_t::mod;

            case xil_al_command_t::minus:
                return operator_t::minus;

            case xil_al_command_t::positive:
                return operator_t::positive;

            default:
                return operator_t::__unknown__;
        }
    }

    // Returns operator of a xil_bit_command.
    constexpr operator_t to_operator(xil_bit_command_t cmd)
    {
        switch (cmd)
        {
            case xil_bit_command_t::bit_and:
                return operator_t::bit_and;

            case xil_bit_command_t::bit_or:
                return operator_t::bit_or;

            case xil_bit_command_t::bit_not:
                return operator_t::bit_not;

            case xil_bit_command_t::bit_xor:
                return operator_t::bit_xor;

            case xil_bit_command_t::left_shift:
                return operator_t::left_shift;

            case xil_bit_command_t::right_shift:
                return operator_t::right_shift;

            default:
                return operator_t::__unknown__;
        }
    }

    // Returns operator of a xil_cmp_command.
    constexpr operator_t to_operator(xil_cmp_command_t cmd)
    {
        switch (cmd)
        {
            case xil_cmp_command_t::greater:
                return operator_t::greater;

            case xil_cmp_command_t::greater_equal:
                return operator_t::greater_equal;

            case xil_cmp_command_t::less:
                return operator_t::less;

            case xil_cmp_command_t::less_equal:
                return operator_t::less_equal;

            case xil_cmp_command_t::equal:
                return operator_t::equal;

            case xil_cmp_command_t::not_equal:
                return operator_t::not_equal;

            default:
                return operator_t::__unknown__;
        }
    }

    // Returns a operator of a logic command.
    constexpr operator_t to_operator(xil_logic_command_t cmd)
    {
        switch (cmd)
        {
            case xil_logic_command_t::and_:
                return operator_t::logic_and;

            case xil_logic_command_t::or_:
                return operator_t::logic_or;

            case xil_logic_command_t::not_:
                return operator_t::logic_not;

            default:
                return operator_t::__unknown__;
        }
    }

    // Return vtype of a xil data type.
    constexpr vtype_t to_vtype(xil_type_t dtype)
    {
        switch (dtype)
        {
            case xil_type_t::int8:
                return vtype_t::int8_;

            case xil_type_t::uint8:
                return vtype_t::uint8_;

            case xil_type_t::int16:
                return vtype_t::int16_;

            case xil_type_t::uint16:
                return vtype_t::uint16_;

            case xil_type_t::int32:
                return vtype_t::int32_;

            case xil_type_t::uint32:
                return vtype_t::uint32_;

            case xil_type_t::int64:
                return vtype_t::int64_;

            case xil_type_t::uint64:
                return vtype_t::uint64_;

            case xil_type_t::float_:
                return vtype_t::float_;

            case xil_type_t::double_:
                return vtype_t::double_;

            case xil_type_t::bool_:
                return vtype_t::bool_;

            case xil_type_t::char_:
                return vtype_t::char_;

            case xil_type_t::string:
                return vtype_t::string_;

            case xil_type_t::object:
                return vtype_t::mobject_;

            case xil_type_t::ptr:
                return vtype_t::ptr_;

            default:
                return vtype_t::__unknown__;
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Unitary operator.
    template<operator_t _op, vtype_t _t1>
    using unitary_operator_t = __unitary_operator_t<_op, _t1>;

    // Binary operator.
    template<operator_t _op, vtype_t _t1, vtype_t _t2>
    using binary_operator_t = __binary_operator_t<_op, _t1, _t2>;

    template<vtype_t _t> using vnum_t = __vnum_t<_t>;

    ////////// ////////// ////////// ////////// //////////

} } }

#endif // __CORE_VTYPE_TRAITS_H__
