
#include <compile.h>
#include "utils.h"

namespace X_ROOT_NS { namespace modules { namespace compile {

    typedef expression_compile_context_t __cctx_t;
    typedef compile_error_code_t __e_t;

    template<typename _expression_t>
    using __sys_t = system_expression_t<_expression_t>;

    ////////// ////////// ////////// ////////// //////////

    static type_t * __get_host_type(member_t * member)
    {
        _A(member != nullptr);

        type_t * host_type = member->host_type;
        if(host_type == nullptr)
            throw _ED(__e_t::host_type_missing, member);

        return host_type;
    }

    static bool __is_member_expression(expression_t * exp)
    {
        return exp != nullptr && exp->this_family() == expression_family_t::binary
            && ((binary_expression_t *)exp)->op() == operator_t::member_point;
    }

    static bool __is_assign_left(expression_t * exp)
    {
        return exp != nullptr && exp->this_family() == expression_family_t::binary
            && ((binary_expression_t *)exp)->op() == operator_t::assign
            && ((binary_expression_t *)exp)->exp1() == exp;
    }

    static void __validate_get_method(expression_compile_context_t & ctx, method_t * method)
    {

    }

    static void __validate_set_method(expression_compile_context_t & ctx, method_t * method)
    {

    }

    void __compile_number(__cctx_t & ctx, xil_pool_t & pool, const tvalue_t & value);

    static method_t * __this_method(__cctx_t & cctx)
    {
        return cctx.statement_ctx.method;
    }

    static xil_type_t __to_xil_type(type_t * type)
    {
        if(type == nullptr)
            return xil_type_t::empty;

        return to_xil_type(type->this_vtype());
    }

    ////////// ////////// ////////// ////////// //////////

    struct __al_xil_t : xil_extra_t<al_xil_t>
    {
        typedef xil_extra_t<al_xil_t> __super_t;

        using __super_t::__super_t;
    };

    //-------- ---------- ---------- ---------- ----------

    struct __bit_xil_t : xil_extra_t<bit_xil_t>
    {
        typedef xil_extra_t<bit_xil_t> __super_t;

        using __super_t::__super_t;
    };

    //-------- ---------- ---------- ---------- ----------

    struct __logic_xil_t : xil_extra_t<logic_xil_t>
    {
        typedef xil_extra_t<logic_xil_t> __super_t;

        using __super_t::__super_t;
    };

    //-------- ---------- ---------- ---------- ----------

    struct __cmp_xil_t : xil_extra_t<cmp_xil_t>
    {
        typedef xil_extra_t<cmp_xil_t> __super_t;

        using __super_t::__super_t;
    };

    //-------- ---------- ---------- ---------- ----------

    struct __call_xil_t : xil_extra_t<call_xil_t>
    {
        typedef xil_extra_t<call_xil_t> __super_t;

        using __super_t::__super_t;
    };

    //-------- ---------- ---------- ---------- ----------

    struct __pop_empty_xil_t : xil_extra_t<pop_xil_t>
    {
        typedef xil_extra_t<pop_xil_t> __super_t;

        __pop_empty_xil_t(ref_t struct_type_ref) : __super_t(struct_type_ref) { }

        __pop_empty_xil_t(xil_type_t xil_type)
            : __super_t(xil_storage_type_t::empty, xil_type) { }
    };

    //-------- ---------- ---------- ---------- ----------

    struct __new_xil_t : xil_extra_t<new_xil_t>
    {
        typedef xil_extra_t<new_xil_t> __super_t;

        __new_xil_t(ref_t type_ref) : __super_t(xil_new_type_t::default_)
        {
            this->set_type_ref(type_ref);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    struct __new_array_xil_t : xil_extra_t<new_xil_t>
    {
        typedef xil_extra_t<new_xil_t> __super_t;

        __new_array_xil_t(ref_t type_ref) : __super_t(xil_new_type_t::array)
        {
            this->set_type_ref(type_ref);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    xil_type_t __xil_type(expression_t * exp)
    {
        vtype_t vtype = exp->get_vtype();
        if(vtype == vtype_t::__unknown__ || vtype == vtype_t::mobject)
            return xil_type_t::__unknown__;

        return to_xil_type(vtype);
    }

    //-------- ---------- ---------- ---------- ----------

    struct __push_variable_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        __push_variable_xil_t(xil_storage_type_t storage_type, xil_type_t xtype, msize_t identity)
            : __super_t(storage_type, xtype)
        {
            this->set_identity(identity);
        }
    };

    struct __push_this_ref_xil_t : __push_variable_xil_t
    {
        typedef __push_variable_xil_t __super_t;

        __push_this_ref_xil_t()
            : __super_t(xil_storage_type_t::argument, xil_type_t::object, 0)
        { }
    };

    struct __push_this_val_xil_t : __push_variable_xil_t
    {
        typedef __push_variable_xil_t __super_t;

        __push_this_val_xil_t()
            : __super_t(xil_storage_type_t::argument, xil_type_t::ptr, 0)
        { }
    };

    static void __push_this(expression_compile_context_t & ctx, xil_pool_t & pool)
    {
        method_t * method = __this_method(ctx);
        _A(method != nullptr);

        type_t * host_type = __get_host_type(method);

        if(is_struct(host_type))
            pool.append<__push_this_val_xil_t>();
        else
            pool.append<__push_this_ref_xil_t>();
    }

    struct __push_field_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        __push_field_xil_t(ref_t field_ref, xil_type_t dtype)
            : __super_t(xil_storage_type_t::field, dtype)
        {
            this->set_field_ref(field_ref);
        }
    };

    struct __push_duplicate_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        __push_duplicate_xil_t() : __super_t(xil_storage_type_t::duplicate) { }
    };

    //-------- ---------- ---------- ---------- ----------

    ////////// ////////// ////////// ////////// //////////
    // binary expressions

    static void __compile_add(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<__al_xil_t>(
            xil_al_command_t::add, __xil_type(exp1), __xil_type(exp2)
        );
    }

    static void __compile_sub(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<__al_xil_t>(
            xil_al_command_t::sub, __xil_type(exp1), __xil_type(exp2)
        );
    }

    static void __compile_mul(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<__al_xil_t>(
            xil_al_command_t::mul, __xil_type(exp1), __xil_type(exp2)
        );
    }

    static void __compile_div(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<__al_xil_t>(
            xil_al_command_t::div, __xil_type(exp1), __xil_type(exp2)
        );
    }

    static void __compile_mod(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<__al_xil_t>(
            xil_al_command_t::mod, __xil_type(exp1), __xil_type(exp2)
        );
    }

    static void __compile_bit_and(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<__bit_xil_t>(
            xil_bit_command_t::bit_and, __xil_type(exp1), __xil_type(exp2)
        );
    }

    static void __compile_bit_or(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<__bit_xil_t>(
            xil_bit_command_t::bit_or, __xil_type(exp1), __xil_type(exp2)
        );
    }

    static void __compile_bit_xor(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<__bit_xil_t>(
            xil_bit_command_t::bit_xor, __xil_type(exp1), __xil_type(exp2)
        );
    }

    static void __compile_left_shift(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<__bit_xil_t>(
            xil_bit_command_t::left_shift, __xil_type(exp1), __xil_type(exp2)
        );
    }

    static void __compile_right_shift(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<__bit_xil_t>(
            xil_bit_command_t::right_shift, __xil_type(exp1), __xil_type(exp2)
        );
    }

    static void __compile_member_point(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);

        switch(exp2->this_family())
        {
            case expression_family_t::name:
                exp2->compile(ctx, pool);
                break;

            case expression_family_t::function:
                exp2->compile(ctx, pool);
                break;

            default:
                throw _ED(__e_t::unexpected_member, exp2);
        }
    }

    static void __compile_logic_and(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<__logic_xil_t>(xil_logic_command_t::and_);
    }

    static void __compile_logic_or(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<__logic_xil_t>(xil_logic_command_t::or_);
    }

    static void __compile_logic_not(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<__logic_xil_t>(xil_logic_command_t::not_);
    }

    static void __compile_less(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<__cmp_xil_t>(
            xil_cmp_command_t::less, __xil_type(exp1), __xil_type(exp2)
        );
    }

    static void __compile_less_equal(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<__cmp_xil_t>(
            xil_cmp_command_t::less_equal, __xil_type(exp1), __xil_type(exp2)
        );
    }

    static void __compile_greater(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<__cmp_xil_t>(
            xil_cmp_command_t::greater, __xil_type(exp1), __xil_type(exp2)
        );
    }

    static void __compile_greater_equal(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<__cmp_xil_t>(
            xil_cmp_command_t::greater_equal, __xil_type(exp1), __xil_type(exp2)
        );
    }

    static void __compile_equal(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<__cmp_xil_t>(
            xil_cmp_command_t::equal, __xil_type(exp1), __xil_type(exp2)
        );
    }

    static void __compile_not_equal(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<__cmp_xil_t>(
            xil_cmp_command_t::not_equal, __xil_type(exp1), __xil_type(exp2)
        );
    }

    enum class __assign_to_type_t { default_, pop, pick };

    static variable_t * __pick_var(expression_t * exp)
    {
        if(exp->this_family() != expression_family_t::name)
            throw _ED(__e_t::assign_expression_type_error, exp);

        name_expression_t * name_exp = (name_expression_t *)exp;
        if(name_exp->expression_type != name_expression_type_t::variable)
            throw _ED(__e_t::assign_expression_type_error, exp);

        variable_t * var = name_exp->variable;
        _A(var != nullptr);

        return var;
    }

    struct __compile_assign_t
    {
        expression_t * this_;
        variable_t   * var;
        expression_t * exp;
    };

    static void __push_this(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        if(exp == nullptr)
            __push_this(ctx, pool);
        else
            exp->compile(ctx, pool);
    }

    static __compile_assign_t __pre_compile_assign_to(__cctx_t & ctx, xil_pool_t & pool,
                                                                  expression_t * exp)
    {
        expression_t * r_exp = exp;
        expression_t * this_exp = nullptr;

        if(__is_member_expression(exp))
        {
            this_exp = ((binary_expression_t *)exp)->exp1();
            r_exp    = ((binary_expression_t *)exp)->exp2();
        }

        variable_t * var = __pick_var(r_exp);
        if(var->this_type() == variable_type_t::property)
            __push_this(ctx, pool, this_exp);

        return __compile_assign_t { this_exp, var, exp };
    }

    static void __compile_assign_to(__cctx_t & ctx, xil_pool_t & pool, __compile_assign_t & ca,
        __assign_to_type_t assign_type = __assign_to_type_t::default_)
    {
        expression_t * exp = ca.exp;
        bool pick = (assign_type == __assign_to_type_t::default_)?
            is_effective(exp->parent->parent) : (assign_type == __assign_to_type_t::pick);

        variable_t * var = ca.var;
        switch(var->this_type())
        {
            case variable_type_t::local:
                xil::write_assign_xil(ctx, pool, (local_variable_t *)var, pick);
                break;

            case variable_type_t::param:
                xil::write_assign_xil(ctx, pool, (param_variable_t *)var, pick);
                break;

            case variable_type_t::field:
                __push_this(ctx, pool, ca.this_);
                xil::write_assign_xil(ctx, pool, (field_variable_t *)var, pick);
                break;

            case variable_type_t::property: {
                property_t * property = ((property_variable_t *)var)->property;
                if(property == nullptr)
                    throw _ED(__e_t::unknown_property, var);

                method_t * method = property->set_method;
                if(method == nullptr)
                    throw _ED(__e_t::property_cannot_be_write, var);

                __validate_set_method(ctx, method);

                ref_t method_ref = __search_method_ref(ctx, method);
                pool.append<__call_xil_t>(xil_call_type_t::instance, method_ref);

            } break;

            default:
                X_UNEXPECTED();
        }
    }

    static void __compile_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        exp2->compile(ctx, pool);
        __compile_assign_to(ctx, pool, ca);
    }

    static void __compile_add_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_add(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca);
    }

    static void __compile_sub_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_sub(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca);
    }

    static void __compile_mul_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_mul(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca);
    }

    static void __compile_div_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_div(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca);
    }

    static void __compile_mod_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_mod(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca);
    }

    static void __compile_bit_and_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_bit_and(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca);
    }

    static void __compile_bit_or_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_bit_or(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca);
    }

    static void __compile_bit_xor_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_bit_xor(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca);
    }

    static void __compile_left_shift_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_left_shift(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca);
    }

    static void __compile_right_shift_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_right_shift(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca);
    }

    enum class __inc_t
    {
        left = 1 << 1, right = 1 << 2, increment = 1 << 3, decrement = 1 << 4,
    };

    static void __compile_inc(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp,
                                                                __inc_t flag)
    {
        #define __Is(v) enum_has_flag(flag, __inc_t::v)

        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp);
        variable_t * var = ca.var;

        if(__Is(right))
        {
            if(is_effective(exp->parent->parent))
                exp->compile(ctx, pool);

            exp->compile(ctx, pool);
            __compile_number(ctx, pool, 1);

            pool.append<__al_xil_t>(
                __Is(increment)? xil_al_command_t::add : xil_al_command_t::sub,
                __xil_type(exp), xil_type_t::int32
            );

            __compile_assign_to(ctx, pool, ca, __assign_to_type_t::pop);
        }
        else
        {
            exp->compile(ctx, pool);
            __compile_number(ctx, pool, 1);

            pool.append<__al_xil_t>(
                __Is(increment)? xil_al_command_t::add : xil_al_command_t::sub,
                __xil_type(exp), xil_type_t::int32
            );

            __compile_assign_to(ctx, pool, ca);
        }

        #undef __Is
    }

    static void __compile_left_increment(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        __compile_inc(ctx, pool, exp, enum_or(__inc_t::left, __inc_t::increment));
    }

    static void __compile_left_decrement(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        __compile_inc(ctx, pool, exp, enum_or(__inc_t::left, __inc_t::decrement));
    }

    static void __compile_right_increment(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        __compile_inc(ctx, pool, exp, enum_or(__inc_t::right, __inc_t::increment));
    }

    static void __compile_right_decrement(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        __compile_inc(ctx, pool, exp, enum_or(__inc_t::right, __inc_t::decrement));
    }

    ////////// ////////// ////////// ////////// //////////

    cvalue_t __sys_t<binary_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }

    void __sys_t<binary_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool)
    {
        expression_t * exp1 = expression_at(0), * exp2 = expression_at(1);

        _A(exp1 != nullptr);
        _A(exp2 != nullptr);

        if(!is_effective(this))
        {
            exp1->compile(ctx, pool);
            exp2->compile(ctx, pool);
        }
        else
        {
            #define __Case(op_)                                 \
                case operator_t::op_:                           \
                    __compile_##op_(ctx, pool, exp1, exp2);     \
                    break;

            switch(op())
            {
                __Case(add)
                __Case(sub)
                __Case(mul)
                __Case(div)
                __Case(mod)

                __Case(add_assign)
                __Case(sub_assign)
                __Case(mul_assign)
                __Case(div_assign)
                __Case(mod_assign)

                __Case(bit_and)
                __Case(bit_or)
                __Case(bit_xor)
                __Case(left_shift)
                __Case(right_shift)

                __Case(bit_and_assign)
                __Case(bit_or_assign)
                __Case(bit_xor_assign)
                __Case(left_shift_assign)
                __Case(right_shift_assign)

                __Case(logic_and)
                __Case(logic_or)

                __Case(less)
                __Case(less_equal)
                __Case(greater)
                __Case(greater_equal)
                __Case(equal)
                __Case(not_equal)

                __Case(member_point)
                __Case(assign)

                default:
                    throw _ECF(not_supported, _T("unknown operator \"%1%\""), op());
            }

            #undef __Case
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // unitary expressions

    void __compile_positive(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        exp->compile(ctx, pool);

        pool.append<__al_xil_t>(xil_al_command_t::positive, __xil_type(exp));
    }

    void __compile_minus(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        exp->compile(ctx, pool);

        pool.append<__al_xil_t>(xil_al_command_t::minus, __xil_type(exp));
    }

    void __compile_logic_not(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        exp->compile(ctx, pool);

        pool.append<__logic_xil_t>(xil_logic_command_t::not_);
    }

    void __compile_bit_not(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        exp->compile(ctx, pool);

        pool.append<__bit_xil_t>(xil_bit_command_t::bit_not, __xil_type(exp));
    }

    ////////// ////////// ////////// ////////// //////////
    // unitary_expression_t

    cvalue_t __sys_t<unitary_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }

    void __sys_t<unitary_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool)
    {
        expression_t * exp = expression_at(0);
        _A(exp != nullptr);

        if(!is_effective(this))
        {
            exp->compile(ctx, pool);
        }
        else
        {
            #define __Case(op_)                                 \
                case operator_t::op_:                           \
                    __compile_##op_(ctx, pool, exp);            \
                    break;

            switch(op())
            {
                __Case(positive)
                __Case(minus)
                __Case(logic_not)
                __Case(bit_not)

                __Case(left_increment)
                __Case(left_decrement)
                __Case(right_increment)
                __Case(right_decrement)

                default:
                    throw _ECF(not_supported, _T("unknown operator \"%1%\""), op());
            }

            #undef __Case
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // name_expression_t

    cvalue_t __sys_t<name_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }

    void __sys_t<name_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool)
    {
        if(!is_effective(this))
            return;

        switch(this->expression_type)
        {
            case name_expression_type_t::variable:
                __compile_variable(ctx, pool);
                break;

            case name_expression_type_t::type:
                __compile_type(ctx, pool);
                break;

            case name_expression_type_t::type_def:
                __compile_type_def(ctx, pool);
                break;

            default:
                X_UNEXPECTED();
        }
    }

    void __sys_t<name_expression_t>::__compile_variable(
                                    expression_compile_context_t & ctx, xil_pool_t & pool)
    {
        variable_t * variable = this->variable; 
        _A(variable != nullptr);

        switch(variable->this_type())
        {
            case variable_type_t::local:
                __compile_local_variable(ctx, pool, (local_variable_t *)variable);
                break;

            case variable_type_t::param:
                __compile_param_variable(ctx, pool, (param_variable_t *)variable);
                break;

            case variable_type_t::field:
                __compile_field_variable(ctx, pool, (field_variable_t *)variable);
                break;

            case variable_type_t::property:
                __compile_property_variable(ctx, pool, (property_variable_t *)variable);
                break;

            default:
                X_UNEXPECTED();
        }
    }

    void __sys_t<name_expression_t>::__compile_local_variable(
            expression_compile_context_t & ctx, xil_pool_t & pool, local_variable_t * variable)
    {
        vtype_t vtype = variable->get_vtype();
        xil_type_t xil_type = to_xil_type(vtype);

        pool.append<__push_variable_xil_t>(
            xil_storage_type_t::local, xil_type, variable->identity
        );
    }

    void __sys_t<name_expression_t>::__compile_param_variable(
            expression_compile_context_t & ctx, xil_pool_t & pool, param_variable_t * variable)
    {
        vtype_t vtype = variable->get_vtype();
        xil_type_t xil_type = to_xil_type(vtype);

        msize_t index = variable->param->index;
        if(call_type_of_method(ctx.statement_ctx.method) != xil_call_type_t::static_)
            index++;

        pool.append<__push_variable_xil_t>(
            xil_storage_type_t::argument, xil_type, index
        );
    }

    void __sys_t<name_expression_t>::__compile_field_variable(
            expression_compile_context_t & ctx, xil_pool_t & pool, field_variable_t * field_var)
    {
        if(!__is_member_expression(this->parent))
            __push_this(ctx, pool);

        field_t * field = field_var->field;
        if(field == nullptr)
            throw _ED(__e_t::unknown_field, field_var);

        ref_t field_ref = __search_field_ref(ctx, field);
        type_t * field_type = to_type(field->type_name);
        if(field_type == nullptr)
            throw _ED(__e_t::unknown_field_type, field);

        xil_type_t xil_type = __to_xil_type(field_type);
        pool.append<__push_field_xil_t>(field_ref, xil_type);
    }

    void __sys_t<name_expression_t>::__compile_property_variable(
            expression_compile_context_t & ctx, xil_pool_t & pool,
            property_variable_t * property_var)
    {
        property_t * property = property_var->property;
        if(property == nullptr)
            throw _ED(__e_t::unknown_property, property_var);

        if(!__is_member_expression(this->parent))
            __push_this(ctx, pool);

        method_t * method = property->get_method;
        if(method == nullptr)
            throw _ED(__e_t::property_cannot_be_read, property);

        __validate_get_method(ctx, method);

        ref_t method_ref = __search_method_ref(ctx, method);
        pool.append<__call_xil_t>(xil_call_type_t::instance, method_ref);
    }

    void __sys_t<name_expression_t>::__compile_type(
                                    expression_compile_context_t & ctx, xil_pool_t & pool)
    {

    }

    void __sys_t<name_expression_t>::__compile_type_def(
                                    expression_compile_context_t & ctx, xil_pool_t & pool)
    {

    }

    ////////// ////////// ////////// ////////// //////////
    // cvalue_expression_t

    cvalue_t __sys_t<cvalue_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }

    template<typename _value_t>
    struct __push_const_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        __push_const_xil_t(xil_type_t xtype, const tvalue_t & value)
            : __super_t(xil_storage_type_t::constant, xtype)
        {
            this->set_extra(value.get_value<_value_t>());
        }
    };

    struct __push_string_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        __push_string_xil_t(res_t res)
            : __super_t(xil_storage_type_t::constant, xil_type_t::string)
        {
            this->set_extra(res);
        }
    };

    void __compile_number(__cctx_t & ctx, xil_pool_t & pool, const tvalue_t & value)
    {
        switch(value.type)
        {
		    case value_type_t::int8_:
                pool.append<__push_const_xil_t<int8_t>>(xil_type_t::int8, value);
                break;

            case value_type_t::uint8_:
                pool.append<__push_const_xil_t<uint8_t>>(xil_type_t::uint8, value);
                break;

            case value_type_t::int16_:
                pool.append<__push_const_xil_t<int16_t>>(xil_type_t::int16, value);
                break;

            case value_type_t::uint16_:
                pool.append<__push_const_xil_t<uint16_t>>(xil_type_t::uint16, value);
                break;

            case value_type_t::int32_:
                pool.append<__push_const_xil_t<int32_t>>(xil_type_t::int32, value);
                break;

            case value_type_t::uint32_:
                pool.append<__push_const_xil_t<uint32_t>>(xil_type_t::uint32, value);
                break;

            case value_type_t::int64_:
                pool.append<__push_const_xil_t<int64_t>>(xil_type_t::int64, value);
                break;

            case value_type_t::uint64_:
                pool.append<__push_const_xil_t<uint64_t>>(xil_type_t::uint64, value);
                break;

            case value_type_t::float_:
                pool.append<__push_const_xil_t<float>>(xil_type_t::float_, value);
                break;

            case value_type_t::double_:
                pool.append<__push_const_xil_t<double>>(xil_type_t::double_, value);
                break;

            case value_type_t::bool_:
                pool.append<__push_const_xil_t<bool>>(xil_type_t::bool_, value);
                break;

            case value_type_t::char_:
                pool.append<__push_const_xil_t<char_t>>(xil_type_t::char_, value);
                break;

            default:
            case value_type_t::ldouble_:
                throw _EC(unexpected);
        }
    }

    void __compile_string(__cctx_t & ctx, xil_pool_t & pool, const char_t * string)
    {
        pool.append<__push_string_xil_t>(ctx.statement_ctx.assembly_layout().res_of(string));
    }

    void __compile_null(__cctx_t & ctx, xil_pool_t & pool)
    {

    }

    void __compile_type(__cctx_t & ctx, xil_pool_t & pool, type_t * type)
    {

    }

    void __compile_cvalue(__cctx_t & ctx, xil_pool_t & pool, cvalue_t cvalue)
    {
        switch(cvalue.value_type)
        {
            case cvalue_type_t::string:
                __compile_string(ctx, pool, cvalue.string);
                break;

            case cvalue_type_t::number:
                __compile_number(ctx, pool, cvalue.number);
                break;

            case cvalue_type_t::null:
                __compile_null(ctx, pool);
                break;

            case cvalue_type_t::type:
                __compile_type(ctx, pool, (type_t *)cvalue.mobject);
                break;

            case cvalue_type_t::nan:
            default:
                throw _EC(unexpected);
        }
    }

    void __sys_t<cvalue_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool)
    {
        if(is_effective(this))
        {
            cvalue_t cvalue = *this->value;

            __compile_cvalue(ctx, pool, cvalue);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // function_expression_t

    cvalue_t __sys_t<function_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }
    
    static bool __left_is_base(expression_t * exp)
    {
        if(!__is_member_expression(exp))
            return false;

        binary_expression_t * bin_exp = (binary_expression_t *)exp;
        return bin_exp->exp1()->this_family() == expression_family_t::base;
    }

    void __sys_t<function_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool)
    {
        method_t * method = this->method;
        _A(method != nullptr);

        bool effective = is_effective(this);
        xil_call_type_t call_type = call_type_of_method(method);

        if(effective && !__is_member_expression(this->parent) &&
                call_type != xil_call_type_t::static_)
        {
            __push_this(ctx, pool);
        }

        if(this->arguments() != nullptr)
        {
            for(argument_t * argument : *this->arguments())
            {
                expression_t * exp = argument->expression;
                _A(exp != nullptr);

                exp->compile(ctx, pool);
            }
        }

        if(effective)
        {
            ref_t method_ref = __search_method_ref(ctx, method);
            if(__left_is_base(this->parent) && call_type == xil_call_type_t::virtual_)
                pool.append<__call_xil_t>(xil_call_type_t::instance, method_ref);
            else
                pool.append<__call_xil_t>(call_type, method_ref);

            if(!is_effective(this->parent))
            {
                type_t * ret_type = to_type(method->type_name);
                vtype_t vtype;

                if(ret_type != nullptr && (vtype = ret_type->this_vtype()) != vtype_t::void_)
                {
                    xil_type_t xil_type = to_xil_type(vtype);

                    if(xil_type == xil_type_t::object && is_struct(ret_type))
                    {
                        ref_t type_ref = __ref_of(ctx, ret_type);
                        _A(type_ref != ref_t::null);
                        pool.append<__pop_empty_xil_t>(type_ref);
                    }
                    else
                    {
                        pool.append<__pop_empty_xil_t>(
                            to_xil_type(ret_type->this_vtype())
                        );
                    }
                }
            }
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // type_cast_expression_t

    cvalue_t __sys_t<type_cast_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }
    
    void __sys_t<type_cast_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool)
    {

    }

    ////////// ////////// ////////// ////////// //////////
    // type_name_expression_t

    cvalue_t __sys_t<type_name_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }
    
    void __sys_t<type_name_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool)
    {

    }

    ////////// ////////// ////////// ////////// //////////
    // index_expression_t

    cvalue_t __sys_t<index_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }
    
    void __sys_t<index_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool)
    {

    }

    ////////// ////////// ////////// ////////// //////////
    // new_expression_t

    cvalue_t __sys_t<new_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }
    
    static xil_call_type_t __get_constructor_calltype(__cctx_t & ctx, type_t * host_type,
                                                                method_t * constructor)
    {
        #define __EConstructor(name)                                            \
            throw _ED(__e_t::unexpected_constructor_prototype__##name,          \
                                                            constructor);

        type_t * type = to_type(constructor->type_name);
        if(type != nullptr && !is_void_type(type))
            __EConstructor(should_no_return_type);

        xil_call_type_t call_type = call_type_of_method(constructor);
        switch(call_type)
        {
            case xil_call_type_t::static_:
                __EConstructor(should_no_static);
                break;

            case xil_call_type_t::virtual_:
                __EConstructor(should_no_virtual);
                break;

            default: break;
        }

        if(constructor->host_type != host_type)
            throw _ED(__e_t::unexpected_host_type, constructor, host_type);

        #undef __EConstructor

        return call_type;
    }

    void __sys_t<new_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool)
    {
        type_t * type = to_type(this->type_name);
        if(type == nullptr)
            throw _ED(__e_t::type_missing);

        ref_t type_ref = __ref_of(ctx, type);
        _A(type_ref != ref_t::null);

        pool.append<__new_xil_t>(type_ref);

        bool is_parent_effective = is_effective(this->parent);
        if(this->constructor != nullptr && is_parent_effective)
        {
            pool.append<__push_duplicate_xil_t>();
        }

        if(this->arguments() != nullptr)
        {
            if(this->constructor == nullptr)
                throw _ED(__e_t::constructor_missing, type);

            for(argument_t * argument : *this->arguments())
            {
                expression_t * exp = argument->expression;
                _A(exp != nullptr);

                exp->compile(ctx, pool);
            }
        }

        if(this->constructor != nullptr)
        {
            xil_call_type_t call_type = __get_constructor_calltype(ctx, type, this->constructor);
            ref_t method_ref = __search_method_ref(ctx, this->constructor);
            pool.append<__call_xil_t>(call_type, method_ref);
        }
        else if(!is_parent_effective)
        {
            pool.append<__pop_empty_xil_t>(xil_type_t::object);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // new_array_expression_t

    cvalue_t __sys_t<new_array_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }

    void __sys_t<new_array_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool)
    {
        // lengths
        if(this->lengths() != nullptr)
        {
            for(expression_t * expression : *this->lengths())
            {
                if(expression == nullptr)
                    throw _ED(__e_t::unknown_array_length, this);

                // TODO: must be int type

                expression->compile(ctx, pool);
            }
        }

        // new
        type_t * type = this->to_array_type(ctx.statement_ctx.xpool());

        if(type == nullptr)
            throw _ED(__e_t::type_missing);

        ref_t type_ref = __ref_of(ctx, type);
        _A(type_ref != ref_t::null);

        pool.append<__new_array_xil_t>(type_ref);

        // initializer
        if(!is_effective(this->parent))
            pool.append<__pop_empty_xil_t>(xil_type_t::object);
    }

    ////////// ////////// ////////// ////////// //////////
    // default_value_expression_t

    cvalue_t __sys_t<default_value_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }
    
    void __sys_t<default_value_expression_t>::compile(__cctx_t & ctx,
                                                            xil_pool_t & pool)
    {
        type_t * type = to_type(this->type_name);
        if(type == nullptr)
            throw _ED(__e_t::type_missing);

        vtype_t vtype = type->this_vtype();
        cvalue_t value = default_value_of(vtype);

        if(is_nan(value))
            throw _ED(__e_t::no_default_value, type);

        if(is_effective(this->parent))
            __compile_cvalue(ctx, pool, value);
    }

    ////////// ////////// ////////// ////////// //////////
    // type_of_expression_t

    cvalue_t __sys_t<type_of_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }
    
    void __sys_t<type_of_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool)
    {

    }

    ////////// ////////// ////////// ////////// //////////
    // this_expression_t

    cvalue_t __sys_t<this_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }
    
    static bool __is_this_effective(expression_t * exp)
    {
        if(is_effective(exp->parent))
            return true;

        if(__is_member_expression(exp->parent))
        {
            binary_expression_t * binary_exp = (binary_expression_t *)exp->parent;
            return is_effective(binary_exp->exp2());
        }

        return false;
    }

    void __sys_t<this_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool)
    {
        if(__is_this_effective(this))
        {
            __push_this(ctx, pool);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // base_expression_t

    cvalue_t __sys_t<base_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }
    
    void __sys_t<base_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool)
    {
        if(__is_this_effective(this))
        {
            __push_this(ctx, pool);
        }
    }

    ////////// ////////// ////////// ////////// //////////

    bool is_effective(expression_t * exp)
    {
        if(exp == nullptr)
            return false;

        switch(exp->get_behaviour())
        {
            case expression_behaviour_t::assign:
            case expression_behaviour_t::execute:
            case expression_behaviour_t::new_:
                return true;

            default:
                return is_effective(exp->parent);
        }
    }

    ////////// ////////// ////////// ////////// //////////

} } }

