
#include <compile.h>
#include "utils.h"

namespace X_ROOT_NS { namespace modules { namespace compile {

    typedef expression_compile_context_t __cctx_t;
    typedef compile_error_code_t __e_t;

    template<typename _expression_t>
    using __sys_t = system_expression_t<_expression_t>;

    ////////// ////////// ////////// ////////// //////////

    void __compile_cvalue(__cctx_t & ctx, xil_pool_t & pool, cvalue_t cvalue,
        xil_type_t dtype = xil_type_t::empty);

    // Returns xil type of the expression.
    xil_type_t __xil_type(expression_t * exp)
    {
        vtype_t vtype = exp->get_vtype();
        return to_xil_type(vtype);
    }

    // Returns xil type of type_name.
    xil_type_t __xil_type(type_name_t * type_name)
    {
        type_t * type = to_type(type_name);
        vtype_t vtype = type->this_vtype();

        return to_xil_type(vtype);
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns host type of a member.
    static type_t * __get_host_type(member_t * member)
    {
        _A(member != nullptr);

        type_t * host_type = member->host_type;
        if (host_type == nullptr)
            throw _ED(__e_t::host_type_missing, member);

        return host_type;
    }

    // Returns xpool of expression_compile_context_t;
    X_ALWAYS_INLINE static xpool_t & __xpool(__cctx_t & ctx)
    {
        return ctx.statement_ctx.xpool();
    }

    // Executes the expression, returns nan when it is not a constant value.
    static cvalue_t __execute_expression(__cctx_t & ctx, expression_t * exp)
    {
        _A(exp != nullptr);

        expression_execute_context_t e_ctx(__xpool(ctx));
        return exp->execute(e_ctx);
    }

    // Tries compile expression if it's a constant value.
    // Returns false when compile failed. ( not a constant value. )
    static bool __try_compile_constant_expression(__cctx_t & ctx, xil_pool_t & pool,
                    expression_t * expression, xil_type_t dtype = xil_type_t::empty)
    {
        cvalue_t cvalue = __execute_expression(ctx, expression);

        if (!is_nan(cvalue))
        {
            if (is_effective(expression))
                __compile_cvalue(ctx, pool, cvalue, dtype);

            return true;
        }

        return false;
    }

    // Returns whether it's a function call expression.
    bool __is_call_expression(expression_t * exp)
    {
        expression_family_t family = exp->this_family();
        if (family == expression_family_t::function)
            return true;

        if (family == expression_family_t::binary)
        {
            binary_expression_t * binary_exp = (binary_expression_t *)exp;

            if (binary_exp->overload_method != nullptr)
                return true;

            return binary_exp->exp2()->this_family() == expression_family_t::function;
        }

        if (family == expression_family_t::unitary)
        {
            unitary_expression_t * unitary_exp = (unitary_expression_t *)exp;
            return unitary_exp->overload_method != nullptr;
        }

        return false;
    }

    // Pre append custom struct for assign.
    void __pre_custom_struct_assign(expression_compile_context_t & ctx, xil_pool_t & pool,
        variable_t * variable, expression_t * expression)
    {
        type_t * type = expression->get_type(__xpool(ctx));
        _A( is_custom_struct(type) );

        expression_family_t family = expression->this_family();

        if (family == expression_family_t::new_)
        {
            // Do not need to assign, only put address of this local variable,
            //   and then execute constructor on this local variable.
            // See also: __compile_new_struct_object() in expression.cpp

            new_expression_t * new_exp = (new_expression_t *)expression;

            if (new_exp->constructor != nullptr)
                __push_variable_address(ctx, pool, variable);

            expression->compile(ctx, pool);
        }
        else if (__is_call_expression(expression))  // function
        {
            __push_variable_address(ctx, pool, variable);
            expression->compile(ctx, pool);
        }
        else // assign
        {
            expression->compile(ctx, pool);
            __push_variable_address(ctx, pool, variable);
            pool.append<x_object_copy_xil_t>(__ref_of(ctx, type));
        }
    }

    // Pre append custom struct for return.
    void __do_custom_struct_return(expression_compile_context_t & ctx, xil_pool_t & pool,
        expression_t * expression)
    {
        type_t * type = expression->get_type(__xpool(ctx));
        _A( is_custom_struct(type) );

        expression_family_t family = expression->this_family();
        if (family == expression_family_t::new_)
        {
            // Do not need to assign, only put address of this local variable,
            //   and then execute constructor on this local variable.
            // See also: __compile_new_struct_object() in expression.cpp

            new_expression_t * new_exp = (new_expression_t *)expression;

            if (new_exp->constructor != nullptr)
                pool.append<x_push_calling_bottom_xil_t>();

            expression->compile(ctx, pool);
        }
        else if (__is_call_expression(expression))  // function
        {
            pool.append<x_push_calling_bottom_xil_t>();
            expression->compile(ctx, pool);
        }
        else  // variables.
        {
            expression->compile(ctx, pool);
            pool.append<x_push_calling_bottom_xil_t>();
            pool.append<x_object_copy_xil_t>(__ref_of(ctx, type));
        }
    }

    // Push address of variable.
    void __push_variable_address(expression_compile_context_t & ctx, xil_pool_t & pool,
        variable_t * variable)
    {
        switch (variable->this_type())
        {
            case variable_type_t::local:
                pool.append<x_push_local_addr_xil_t>(
                    ((local_variable_t *)variable)->identity
                );
                break;

            case variable_type_t::param:
                pool.append<x_push_argument_addr_xil_t>(
                    ((param_variable_t *)variable)->param->index
                );
                break;

            case variable_type_t::field:
                pool.append<x_push_field_addr_xil_t>(
                    __search_field_ref(ctx, ((field_variable_t *)variable)->field)
                );
                break;

            default:
                X_UNEXPECTED();
        }
    }

    // Returns whether it's effective.
    // Xils will not generated if the expression is not effective.
    bool is_effective(expression_t * exp)
    {
        if (exp == nullptr)
            return false;

        switch (exp->get_behaviour())
        {
            case expression_behaviour_t::assign:
            case expression_behaviour_t::execute:
            case expression_behaviour_t::new_:
                return true;

            default:
                return is_effective(exp->parent);
        }
    }

    // Try to append a push convert xil.
    bool __try_append_convert_xil(xil_pool_t & pool, xil_type_t from_dtype, xil_type_t to_dtype)
    {
        if (from_dtype != xil_type_t::empty && to_dtype != xil_type_t::empty
            && from_dtype != to_dtype)
        {
            pool.append<x_push_convert_xil_t>(from_dtype, to_dtype);
            return true;
        }

        return false;
    }

    // Try to append a push convert xil.
    bool __try_append_convert_xil(xil_pool_t & pool, vtype_t from_vtype, xil_type_t to_dtype)
    {
        if (from_vtype != vtype_t::void_ && from_vtype != vtype_t::__default__
            && from_vtype != vtype_t::__unknown__)
        {
            xil_type_t from_dtype = to_xil_type(from_vtype);
            return __try_append_convert_xil(pool, from_dtype, to_dtype);
        }

        return false;
    }

    // Try to append a push convert xil.
    bool __try_append_convert_xil(xil_pool_t & pool, type_t * from_type, xil_type_t to_dtype)
    {
        vtype_t from_vtype;

        if (from_type != nullptr && (from_vtype = from_type->this_vtype()) != vtype_t::void_)
        {
            return __try_append_convert_xil(pool, from_vtype, to_dtype);
        }

        return false;
    }

    // Returns whether it is a member expression.
    static bool __is_member_expression(expression_t * exp)
    {
        return exp != nullptr && exp->this_family() == expression_family_t::binary
            && ((binary_expression_t *)exp)->op() == operator_t::member_point;
    }

    // Returns whether it's effective.
    static bool __is_this_effective(expression_t * exp)
    {
        if (is_effective(exp->parent))
            return true;

        if (__is_member_expression(exp->parent))
        {
            binary_expression_t * binary_exp = (binary_expression_t *)exp->parent;
            return is_effective(binary_exp->exp2());
        }

        return false;
    }

    // Returns whether the expression assign equals, e.g. +=, -=.
    static bool __is_assign_left(expression_t * exp)
    {
        return exp != nullptr && exp->this_family() == expression_family_t::binary
            && ((binary_expression_t *)exp)->op() == operator_t::assign
            && ((binary_expression_t *)exp)->exp1() == exp;
    }

    // Validates get method.
    static void __validate_get_method(expression_compile_context_t & ctx, method_t * method)
    {
        // TODO
    }

    // Varidates set method.
    static void __validate_set_method(expression_compile_context_t & ctx, method_t * method)
    {
        // TODO
    }

    // Compiles number.
    void __compile_number(__cctx_t & ctx, xil_pool_t & pool, const tvalue_t & value);

    // Returns whether it's static call type.
    X_ALWAYS_INLINE static bool __is_static(xil_call_type_t call_type)
    {
        return call_type == xil_call_type_t::static_ || call_type == xil_call_type_t::internal;
    }

    // Returns this method.
    X_ALWAYS_INLINE static method_t * __this_method(__cctx_t & cctx)
    {
        return cctx.statement_ctx.method;
    }

    // Returns xil type.
    X_ALWAYS_INLINE static xil_type_t __to_xil_type(type_t * type)
    {
        if (type == nullptr)
            return xil_type_t::empty;

        return to_xil_type(type->this_vtype());
    }

    ////////// ////////// ////////// ////////// //////////
    // Binary expressions

    // Compiles add expression.
    static void __compile_add(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        if (__execute_expression(ctx, exp1) == cvalue_t(0))  // x + 0
        {
            exp2->compile(ctx, pool);
        }
        else if (__execute_expression(ctx, exp2) == cvalue_t(0)) // 0 + x
        {
            exp1->compile(ctx, pool);
        }
        else
        {
            exp1->compile(ctx, pool);
            exp2->compile(ctx, pool);

            pool.append<x_al_xil_t>(
                xil_al_command_t::add, __xil_type(exp1), __xil_type(exp2)
            );
        }
    }

    // Compiles sub expression.
    static void __compile_sub(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        if (__execute_expression(ctx, exp1) == cvalue_t(0))  // 0 * x
        {
            exp2->compile(ctx, pool);
            pool.append<x_al_xil_t>(xil_al_command_t::minus, __xil_type(exp1));
        }
        else if (__execute_expression(ctx, exp2) == cvalue_t(0))  // x * 0
        {
            exp1->compile(ctx, pool);
        }
        else
        {
            exp1->compile(ctx, pool);
            exp2->compile(ctx, pool);

            pool.append<x_al_xil_t>(
                xil_al_command_t::sub, __xil_type(exp1), __xil_type(exp2)
            );
        }
    }

    // Compiles mul expression.
    static void __compile_mul(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        cvalue_t cvalue1 = __execute_expression(ctx, exp1), cvalue2;
        if (cvalue1 == cvalue_t(1))     // 1 * x
        {
            exp2->compile(ctx, pool);
        }
        else if ((cvalue2 = __execute_expression(ctx, exp2)) == cvalue_t(1))  // x * 1
        {
            exp1->compile(ctx, pool);
        }
        // TODO x * 0 || 0 * x
        else
        {
            exp1->compile(ctx, pool);
            exp2->compile(ctx, pool);

            pool.append<x_al_xil_t>(
                xil_al_command_t::mul, __xil_type(exp1), __xil_type(exp2)
            );
        }
    }

    // Compiles div expression.
    static void __compile_div(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        cvalue_t cvalue2 = __execute_expression(ctx, exp2);
        if (cvalue2 == cvalue_t(1))      // x / 1
        {
            exp1->compile(ctx, pool);
        }
        else
        {
            exp1->compile(ctx, pool);
            exp2->compile(ctx, pool);

            pool.append<x_al_xil_t>(
                xil_al_command_t::div, __xil_type(exp1), __xil_type(exp2)
            );
        }
    }

    // Compiles mod expression.
    static void __compile_mod(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_al_xil_t>(
            xil_al_command_t::mod, __xil_type(exp1), __xil_type(exp2)
        );
    }

    // Compiles bit and expression.
    static void __compile_bit_and(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_bit_xil_t>(
            xil_bit_command_t::bit_and, __xil_type(exp1), __xil_type(exp2)
        );
    }

    // Compiles bit or expression.
    static void __compile_bit_or(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_bit_xil_t>(
            xil_bit_command_t::bit_or, __xil_type(exp1), __xil_type(exp2)
        );
    }

    // Compiles bit xor expression.
    static void __compile_bit_xor(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_bit_xil_t>(
            xil_bit_command_t::bit_xor, __xil_type(exp1), __xil_type(exp2)
        );
    }

    // Compiles left shift expression.
    static void __compile_left_shift(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_bit_xil_t>(
            xil_bit_command_t::left_shift, __xil_type(exp1), __xil_type(exp2)
        );
    }

    // Compiles right shift expression.
    static void __compile_right_shift(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_bit_xil_t>(
            xil_bit_command_t::right_shift, __xil_type(exp1), __xil_type(exp2)
        );
    }

    // Compiles member point expression.
    static void __compile_member_point(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);

        switch (exp2->this_family())
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

    // Compiles logic and expression.
    static void __compile_logic_and(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_logic_xil_t>(xil_logic_command_t::and_);
    }

    // Compiles logic or expression.
    static void __compile_logic_or(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_logic_xil_t>(xil_logic_command_t::or_);
    }

    // Compiles logic not expression.
    static void __compile_logic_not(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_logic_xil_t>(xil_logic_command_t::not_);
    }

    // Compiles less expression.
    static void __compile_less(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_cmp_xil_t>(
            xil_cmp_command_t::less, __xil_type(exp1), __xil_type(exp2)
        );
    }

    // Compiles less equal expression.
    static void __compile_less_equal(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_cmp_xil_t>(
            xil_cmp_command_t::less_equal, __xil_type(exp1), __xil_type(exp2)
        );
    }

    // Compiles gerater expression.
    static void __compile_greater(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_cmp_xil_t>(
            xil_cmp_command_t::greater, __xil_type(exp1), __xil_type(exp2)
        );
    }

    // Compiles greater equals expression.
    static void __compile_greater_equal(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_cmp_xil_t>(
            xil_cmp_command_t::greater_equal, __xil_type(exp1), __xil_type(exp2)
        );
    }

    // Compiles equals expression.
    static void __compile_equal(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_cmp_xil_t>(
            xil_cmp_command_t::equal, __xil_type(exp1), __xil_type(exp2)
        );
    }

    // Compiles not equals expression.
    static void __compile_not_equal(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_cmp_xil_t>(
            xil_cmp_command_t::not_equal, __xil_type(exp1), __xil_type(exp2)
        );
    }

    // Compile argument.
    static void __compile_argument(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp,
        method_base_t * method, int index)
    {
        _A(exp != nullptr);

        type_t * atype = method != nullptr?
            method->get_param_type(index) : exp->get_type(__xpool(ctx));

        _A(atype != nullptr);

        if (is_custom_struct(atype))
        {
            ref_t type_ref = __ref_of(ctx, atype);
            pool.append<x_stack_alloc_xil_t>(type_ref);

            expression_family_t family = exp->this_family();

            if (family == expression_family_t::new_)    // new expression
            {
                new_expression_t * new_exp = (new_expression_t *)exp;
                new_exp->compile(ctx, pool);
            }
            else if (__is_call_expression(exp)) // function call expression
            {
                exp->compile(ctx, pool);
            }
            else  // assign
            {
                exp->compile(ctx, pool);
                pool.append<x_object_copy_xil_t>(type_ref, xil_copy_kind_t::reverse);
            }
        }
        else
        {
            vtype_t vtype = vtype_t::__default__;
            if (method != nullptr)
                vtype = method->get_param_vtype(index);

            xil_type_t xil_type = to_xil_type(vtype);
            if (!__try_compile_constant_expression(ctx, pool, exp, xil_type))
                exp->compile(ctx, pool, xil_type);
        }

    }

    // Compile arguments.
    static void __compile_arguments(__cctx_t & ctx, xil_pool_t & pool, arguments_t * arguments,
        method_base_t * method = nullptr)
    {
        if (arguments == nullptr)
            return;

        for (int index = 0, count = arguments->size(); index < count; index++)
        {
            argument_t * argument = (*arguments)[index];
            expression_t * exp = argument->expression;

            __compile_argument(ctx, pool, exp, method, index);
        }
    }

    enum class __assign_to_type_t { default_, pop, pick };

    // Picks variable.
    static variable_t * __pick_var(variable_expression_t * var_exp)
    {
        variable_t * var = var_exp->get_variable();
        if (var == nullptr)
            throw _ED(__e_t::variable_cannot_determined, var_exp->to_string());

        return var;
    }

    // Picks variable.
    static variable_t * __pick_var(expression_t * exp)
    {
        variable_expression_t * var_exp = as<variable_expression_t *>(exp);
        if (var_exp == nullptr || !var_exp->is_variable_expression())
            throw _ED(__e_t::assign_expression_type_error, exp);

        return __pick_var(var_exp);
    }

    // Returns array element type.
    static type_t * __array_element_type_of(__cctx_t & ctx, index_expression_t * exp)
    {
        variable_t * var = __pick_var((variable_expression_t *)exp);
        type_t * type = var->get_type(__xpool(ctx));
        if (type == nullptr)
            throw _ED(__e_t::index_type_undetermined, var);

        return type;
    }

    // Compile assign argument.
    struct __compile_assign_t
    {
        expression_t * this_;
        variable_t   * var;
        expression_t * exp;
        bool           custom_struct;
    };

    // Push this expression.
    static void __push_this(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        if (exp == nullptr)
            pool.append<x_push_this_ref_xil_t>();
        else
            exp->compile(ctx, pool);
    }

    // Pre compile assign.
    static __compile_assign_t __pre_compile_assign_to(__cctx_t & ctx, xil_pool_t & pool,
                                                                  expression_t * exp)
    {
        expression_t * r_exp = exp;
        expression_t * this_exp = nullptr;

        if (__is_member_expression(exp))
        {
            this_exp = ((binary_expression_t *)exp)->exp1();
            r_exp    = ((binary_expression_t *)exp)->exp2();
        }

        variable_t * var = __pick_var(r_exp);
        bool custom_struct = is_custom_struct(var->get_type(__xpool(ctx)));

        switch (var->this_type())
        {
            case variable_type_t::property:
                __push_this(ctx, pool, this_exp);
                break;

            case variable_type_t::property_index: {
                _A(exp->this_family() == expression_family_t::index);
                ((index_expression_t *)exp)->namex()->compile(ctx, pool);

                property_t * property = ((property_variable_t *)var)->property;
                if (property == nullptr)
                    throw _ED(__e_t::unknown_property, var);

                __compile_arguments(ctx, pool,
                    ((property_index_variable_t *)var)->arguments, property->set_method
                );
            }   break;

            default:
                break;
        }

        return __compile_assign_t { this_exp, var, exp, custom_struct };
    }

    // Compiles variable.
    static void __compile_variable(__cctx_t & ctx, xil_pool_t & pool, variable_t * variable,
                                xil_type_t dtype, expression_t * owner_exp = nullptr);

    // Compiles assign to expression.
    static void __compile_assign_to(__cctx_t & ctx, xil_pool_t & pool, __compile_assign_t & ca,
        expression_t * exp2, __assign_to_type_t assign_type = __assign_to_type_t::default_,
        xil_type_t dtype = xil_type_t::empty)
    {
        expression_t * exp = ca.exp;
        bool pick = (assign_type == __assign_to_type_t::default_)?
            is_effective(exp->parent->parent) : (assign_type == __assign_to_type_t::pick);

        variable_t * var = ca.var;
        switch (var->this_type())
        {
            case variable_type_t::local:
                if (!ca.custom_struct)
                    xil::write_assign_xil(ctx, pool, (local_variable_t *)var, dtype, pick);
                else
                    __pre_custom_struct_assign(ctx, pool, var, exp2);
                break;

            case variable_type_t::param:
                if (!ca.custom_struct)
                    xil::write_assign_xil(ctx, pool, (param_variable_t *)var, dtype, pick);
                else
                    __pre_custom_struct_assign(ctx, pool, var, exp2);
                break;

            case variable_type_t::field:
                __push_this(ctx, pool, ca.this_);
                if (!ca.custom_struct)
                    xil::write_assign_xil(ctx, pool, (field_variable_t *)var, dtype, pick);
                else
                    __pre_custom_struct_assign(ctx, pool, var, exp2);
                break;

            case variable_type_t::property: {
                property_t * property = ((property_variable_t *)var)->property;
                if (property == nullptr)
                    throw _ED(__e_t::unknown_property, var);

                method_t * method = property->set_method;
                if (method == nullptr)
                    throw _ED(__e_t::property_cannot_be_write, var);

                __validate_set_method(ctx, method);

                ref_t method_ref = __search_method_ref(ctx, method);
                pool.append<x_call_xil_t>(xil_call_type_t::instance, method_ref);

            }   break;

            case variable_type_t::array_index: {
                array_index_variable_t * arr_var = (array_index_variable_t *)var;
                expression_t * body = arr_var->body;
                arguments_t * args = arr_var->arguments;

                if (body == nullptr)
                    throw _ED(__e_t::index_body_missing, var);

                if (args == nullptr)
                    throw _ED(__e_t::index_arguments_missing, var);

                __compile_arguments(ctx, pool, args);
                body->compile(ctx, pool);

                type_t * element_type = arr_var->get_type(__xpool(ctx));
                type_t * array_type = __xpool(ctx).new_array_type(
                    element_type, arr_var->dimension()
                );

                ref_t type_ref = __ref_of(ctx, array_type);
                pool.append<x_pop_array_element_xil_t>(
                    __to_xil_type(element_type), type_ref
                );

            }   break;

            case variable_type_t::property_index: {
                property_index_variable_t * index_var = (property_index_variable_t *)var;
                property_t * property = index_var->property;

                if (property == nullptr)
                    throw _ED(__e_t::unknown_property, var);

                method_t * method = property->set_method;
                if (method == nullptr)
                    throw _ED(__e_t::property_cannot_be_write, var);

                __validate_set_method(ctx, method);

                ref_t method_ref = __search_method_ref(ctx, method);
                pool.append<x_call_xil_t>(xil_call_type_t::instance, method_ref);

            }   break;

            default:
                X_UNEXPECTED();
        }
    }

    // Compiles assign expression.
    static void __compile_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);

        if (!ca.custom_struct)
            exp2->compile(ctx, pool);

        __compile_assign_to(ctx, pool, ca, exp2);
    }

    // Compiles add assign expression.
    static void __compile_add_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_add(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca, exp2);
    }

    // Compiles sub assign expression.
    static void __compile_sub_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_sub(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca, exp2);
    }

    // Compiles mul assign expression.
    static void __compile_mul_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_mul(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca, exp2);
    }

    // Compiles div assign expression.
    static void __compile_div_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_div(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca, exp2);
    }

    // Compiles mod assign expression.
    static void __compile_mod_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_mod(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca, exp2);
    }

    // Compiles bit and assign expression.
    static void __compile_bit_and_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_bit_and(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca, exp2);
    }

    // Compiles bit or assign expression.
    static void __compile_bit_or_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_bit_or(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca, exp2);
    }

    // Compiles bit xor assign expression.
    static void __compile_bit_xor_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_bit_xor(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca, exp2);
    }

    // Compiles left shift assign expression.
    static void __compile_left_shift_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_left_shift(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca, exp2);
    }

    // Compiles right shift assign expression.
    static void __compile_right_shift_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_right_shift(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca, exp2);
    }

    // Increment type.
    enum class __inc_t
    {
        left = 1 << 1, right = 1 << 2, increment = 1 << 3, decrement = 1 << 4,
    };

    // Compiles increment expression.
    static void __compile_inc(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp,
                                                                __inc_t flag)
    {
        #define __Is(v) enum_has_flag(flag, __inc_t::v)

        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp);
        variable_t * var = ca.var;

        if (__Is(right))
        {
            if (is_effective(exp->parent->parent))
                exp->compile(ctx, pool);

            exp->compile(ctx, pool);
            __compile_number(ctx, pool, 1);

            pool.append<x_al_xil_t>(
                __Is(increment)? xil_al_command_t::add : xil_al_command_t::sub,
                __xil_type(exp), xil_type_t::int32
            );

            __compile_assign_to(ctx, pool, ca, nullptr, __assign_to_type_t::pop);
        }
        else
        {
            exp->compile(ctx, pool);
            __compile_number(ctx, pool, 1);

            pool.append<x_al_xil_t>(
                __Is(increment)? xil_al_command_t::add : xil_al_command_t::sub,
                __xil_type(exp), xil_type_t::int32
            );

            __compile_assign_to(ctx, pool, ca, nullptr);
        }

        #undef __Is
    }

    // Compiles left increment.
    static void __compile_left_increment(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        __compile_inc(ctx, pool, exp, enum_or(__inc_t::left, __inc_t::increment));
    }

    // Compiles left decrement.
    static void __compile_left_decrement(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        __compile_inc(ctx, pool, exp, enum_or(__inc_t::left, __inc_t::decrement));
    }

    // Compiles right increment.
    static void __compile_right_increment(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        __compile_inc(ctx, pool, exp, enum_or(__inc_t::right, __inc_t::increment));
    }

    // Compiles right decrement.
    static void __compile_right_decrement(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        __compile_inc(ctx, pool, exp, enum_or(__inc_t::right, __inc_t::decrement));
    }

    ////////// ////////// ////////// ////////// //////////

    xil_type_t __to_xil_type(__cctx_t & ctx, variable_t * variable)
    {
        type_t * type = variable->get_type(__xpool(ctx));
        if (type->this_gtype() == gtype_t::generic_param)
            return xil_type_t::empty;

        vtype_t vtype = variable->get_vtype();
        return to_xil_type(vtype);
    }

    // Compiles local variable.
    static void __compile_local_variable(__cctx_t & ctx, xil_pool_t & pool,
                                    local_variable_t * variable, xil_type_t dtype)
    {
        //_PF(_T("-------- variable: %1%, %2%, %3%"), variable,
        //                variable->read_count, variable->write_count);

        xil_type_t xil_type = __to_xil_type(ctx, variable);

        if (variable->constant)
        {
            cvalue_t cvalue = __execute_expression(ctx, variable->expression);
            _A(cvalue != cvalue_t::nan);

            __compile_cvalue(ctx, pool, cvalue, dtype);
        }
        else
        {
            if (dtype == xil_type_t::empty)
                dtype = xil_type;

            type_t * type = variable->get_type(__xpool(ctx));

            // TODO: when member point, int, long ... types also should be ptr type.
            if (is_custom_struct(type))
                pool.append<x_push_local_addr_xil_t>(variable->identity);
            else
                pool.append<x_push_local_xil_t>(dtype, variable->identity);
        }
    }

    // Compiles param variable.
    static void __compile_param_variable(__cctx_t & ctx, xil_pool_t & pool,
                                    param_variable_t * variable, xil_type_t dtype)
    {
        param_t * param = variable->param;

        if (param->ptype == param_type_t::extends)
        {
            pool.append<x_push_params_xil_t>();
        }
        else
        {
            msize_t index = variable->param->index;
            if (!__is_static(call_type_of_method(ctx.statement_ctx.method)))
                index++;

            type_t * type = variable->get_type(__xpool(ctx));

            // TODO: when member point, int, long ... types also should be ptr type.
            if (is_custom_struct(type))
                pool.append<x_push_argument_addr_xil_t>(index);
            else
                pool.append<x_push_argument_xil_t>(__to_xil_type(ctx, variable), index);
        }
    }

    // Compiles field variable.
    static void __compile_field_variable(__cctx_t & ctx, xil_pool_t & pool,
                                    field_variable_t * field_var, xil_type_t dtype)
    {
        field_t * field = field_var->field;
        if (field == nullptr)
            throw _ED(__e_t::unknown_field, field_var);

        ref_t field_ref = __search_field_ref(ctx, field);
        type_t * field_type = to_type(field->type_name);

        if (field_type == nullptr)
            throw _ED(__e_t::unknown_field_type, field);

        // TODO: when member point, int, long ... types also should be ptr type.
        if (is_custom_struct(field_type))
        {
            pool.append<x_push_field_addr_xil_t>(field_ref);
        }
        else
        {
            xil_type_t xil_type = (dtype != xil_type_t::empty)? dtype : __to_xil_type(field_type);
            pool.append<x_push_field_xil_t>(field_ref, xil_type);
        }
    }

    // Pops return value from stack
    static void __pop_empty_for_method(__cctx_t & ctx, xil_pool_t & pool,
                                        method_base_t * method, expression_t * owner_exp)
    {
        if (is_effective(owner_exp->parent))
            return;

        type_t * ret_type = method->get_type();
        vtype_t vtype;

        if ( (ret_type != nullptr && (vtype = ret_type->this_vtype()) != vtype_t::void_)
            || is_custom_struct(ret_type) )
        {
            xil_type_t xil_type = to_xil_type(vtype);

            if (xil_type == xil_type_t::object && is_struct(ret_type))
            {
                ref_t type_ref = __ref_of(ctx, ret_type);
                _A(type_ref != ref_t::null);
                pool.append<x_pop_empty_xil_t>(type_ref);
            }
            else
            {
                pool.append<x_pop_empty_xil_t>(to_xil_type(ret_type->this_vtype()));
            }
        }
    }

    // Compile property variable.
    static void __compile_property_variable(__cctx_t & ctx, xil_pool_t & pool,
        property_t * property, arguments_t * arguments, xil_type_t dtype, expression_t * owner_exp)
    {
        method_t * method = property->get_method;
        if (method == nullptr)
            throw _ED(__e_t::property_cannot_be_read, property);

        __validate_get_method(ctx, method);

        __compile_arguments(ctx, pool, arguments, method);

        ref_t method_ref = __search_method_ref(ctx, method);
        pool.append<x_call_xil_t>(xil_call_type_t::instance, method_ref);

        __pop_empty_for_method(ctx, pool, method, owner_exp);
    }

    // Compile property variable.
    static void __compile_property_variable(__cctx_t & ctx, xil_pool_t & pool,
            property_variable_t * property_var, xil_type_t dtype, expression_t * owner_exp)
    {
        property_t * property = property_var->property;
        if (property == nullptr)
            throw _ED(__e_t::unknown_property, property_var);

        __compile_property_variable(ctx, pool, property, nullptr, dtype, owner_exp);
    }

    // Compiles property index variable.
    static void __compile_property_index_variable(__cctx_t & ctx, xil_pool_t & pool,
            property_index_variable_t * property_var, xil_type_t dtype, expression_t * owner_exp)
    {
        property_t * property = property_var->property;
        if (property == nullptr)
            throw _ED(__e_t::unknown_property, property_var);

        __compile_property_variable(ctx, pool, property, property_var->arguments, dtype, owner_exp);
    }

    // Compiles method variable.
    static void __compile_method_variable(__cctx_t & ctx, xil_pool_t & pool,
            method_variable_t * method_var, xil_type_t dtype)
    {
        method_t * method = method_var->method;

        if (method == nullptr)
            throw _ED(__e_t::unknown_method, method_var);
    }

    // Pushes this with check.
    static void __push_this_with_check(__cctx_t & ctx, xil_pool_t & pool, expression_t * owner_exp)
    {
        if (owner_exp != nullptr && !__is_member_expression(owner_exp->parent))
            pool.append<x_push_this_ref_xil_t>();
    }

    // Compiles variable.
    static void __compile_variable(__cctx_t & ctx, xil_pool_t & pool,
                        variable_t * variable, xil_type_t dtype, expression_t * owner_exp)
    {
        _A(variable != nullptr);

        switch (variable->this_type())
        {
            case variable_type_t::local:
                __compile_local_variable(ctx, pool, (local_variable_t *)variable, dtype);
                break;

            case variable_type_t::param:
                __compile_param_variable(ctx, pool, (param_variable_t *)variable, dtype);
                break;

            case variable_type_t::field:
                __push_this_with_check(ctx, pool, owner_exp);
                __compile_field_variable(ctx, pool, (field_variable_t *)variable, dtype);
                break;

            case variable_type_t::property:
                __push_this_with_check(ctx, pool, owner_exp);
                __compile_property_variable(ctx, pool, (property_variable_t *)variable,
                                                                        dtype, owner_exp);
                break;

            case variable_type_t::method:
                __push_this_with_check(ctx, pool, owner_exp);
                __compile_method_variable(ctx, pool, (method_variable_t *)variable, dtype);
                break;

            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Executes binary expression.
    cvalue_t __sys_t<binary_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }

    // Compiles binary expression.
    void __sys_t<binary_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool,
                                                               xil_type_t dtype)
    {
        if (__try_compile_constant_expression(ctx, pool, this, dtype))
            return;

        expression_t * exp1 = expression_at(0), * exp2 = expression_at(1);

        _A(exp1 != nullptr);
        _A(exp2 != nullptr);

        // Operators overload.
        if (this->overload_method != nullptr)
        {
            __compile_operator_overload(ctx, pool);
            return;
        }

        // Basic data types.
        if (!is_effective(this))
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

            switch (op())
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

            __try_append_convert_xil(pool, this->get_vtype(), dtype);
        }
    }

    // Checks overload method prototype.
    void __check_overload_prototype(__cctx_t & ctx, method_t * method, int param_count,
                                                    expression_t ** expressions)
    {
        _A(method != nullptr);

        if (method->param_count() != param_count)
            throw _ED(__e_t::operator_overload__wrong_param_count, method);

        if (method->is_generic())
            throw _ED(__e_t::operator_overload__no_generic, method);

        if (!method->is_static())
            throw _ED(__e_t::operator_overload__wrong_prototype, method);

        for (int k = 0; k < param_count; k++)
        {
            expression_t * exp = expressions[k];
            _A(exp != nullptr);

            param_t * param = method->param_at(k);
            _A(param != nullptr);

            type_t * exp_type = exp->get_type(__xpool(ctx));
            type_t * param_type = param->get_type();

            if (!is_type_compatible(exp_type, param_type))
                throw _ED(__e_t::operator_overload__wrong_argument_type,
                    _F(_T("%1%(%2%)"), exp, exp_type), _F(_T("%1%(%2%"), param, param_type)
                );
        }
    }

    // Compiles operator overload..
    void __sys_t<binary_expression_t>::__compile_operator_overload(__cctx_t & ctx, xil_pool_t & pool)
    {
        method_t * method = this->overload_method;

        // Checks method prototype.
        __check_overload_prototype(ctx, method, 2, this->exps);

        // Compile arguments.
        __compile_argument(ctx, pool, expression_at(0), method, 0);
        __compile_argument(ctx, pool, expression_at(1), method, 1);

        ref_t method_ref = __search_method_ref(ctx, method);
        pool.append<x_call_xil_t>(xil_call_type_t::static_, method_ref);

        __pop_empty_for_method(ctx, pool, method, this);
    }

    ////////// ////////// ////////// ////////// //////////
    // unitary expressions

    // Compiles positive expression.
    void __compile_positive(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        exp->compile(ctx, pool);

        pool.append<x_al_xil_t>(xil_al_command_t::positive, __xil_type(exp));
    }

    // Compiles minus expression.
    void __compile_minus(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        exp->compile(ctx, pool);

        pool.append<x_al_xil_t>(xil_al_command_t::minus, __xil_type(exp));
    }

    // Compiles logic node expression.
    void __compile_logic_not(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        exp->compile(ctx, pool);

        pool.append<x_logic_xil_t>(xil_logic_command_t::not_);
    }

    // Compiles bit node expression.
    void __compile_bit_not(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        exp->compile(ctx, pool);

        pool.append<x_bit_xil_t>(xil_bit_command_t::bit_not, __xil_type(exp));
    }

    ////////// ////////// ////////// ////////// //////////
    // unitary_expression_t

    // Executes unitary expression.
    cvalue_t __sys_t<unitary_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }

    // Compiles unitary expression.
    void __sys_t<unitary_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool, xil_type_t dtype)
    {
        if (__try_compile_constant_expression(ctx, pool, this, dtype))
            return;

        expression_t * exp = expression_at(0);
        _A(exp != nullptr);

        if (!is_effective(this))
        {
            exp->compile(ctx, pool);
        }
        else
        {
            #define __Case(op_)                                 \
                case operator_t::op_:                           \
                    __compile_##op_(ctx, pool, exp);            \
                    break;

            switch (op())
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

            __try_append_convert_xil(pool, this->get_vtype(), dtype);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // name_expression_t

    // Executes name expression.
    cvalue_t __sys_t<name_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }

    // Compiles name expression.
    void __sys_t<name_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool, xil_type_t dtype)
    {
        if (!__is_this_effective(this))
            return;

        switch (this->expression_type)
        {
            case name_expression_type_t::variable:
                __compile_variable(ctx, pool, this->variable, dtype, this);
                break;

            case name_expression_type_t::type:
            case name_expression_type_t::type_def:
                break;

            default:
                _PP(this->expression_type);
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // cvalue_expression_t

    // Executes value expression.
    cvalue_t __sys_t<cvalue_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }

    // Compiles numeric.
    void __compile_number(__cctx_t & ctx, xil_pool_t & pool, const tvalue_t & value)
    {
        switch (value.type)
        {
            case value_type_t::int8_:
                pool.append<x_push_const_xil_t<int8_t>>(xil_type_t::int8, value);
                break;

            case value_type_t::uint8_:
                pool.append<x_push_const_xil_t<uint8_t>>(xil_type_t::uint8, value);
                break;

            case value_type_t::int16_:
                pool.append<x_push_const_xil_t<int16_t>>(xil_type_t::int16, value);
                break;

            case value_type_t::uint16_:
                pool.append<x_push_const_xil_t<uint16_t>>(xil_type_t::uint16, value);
                break;

            case value_type_t::int32_:
                pool.append<x_push_const_xil_t<int32_t>>(xil_type_t::int32, value);
                break;

            case value_type_t::uint32_:
                pool.append<x_push_const_xil_t<uint32_t>>(xil_type_t::uint32, value);
                break;

            case value_type_t::int64_:
                pool.append<x_push_const_xil_t<int64_t>>(xil_type_t::int64, value);
                break;

            case value_type_t::uint64_:
                pool.append<x_push_const_xil_t<uint64_t>>(xil_type_t::uint64, value);
                break;

            case value_type_t::float_:
                pool.append<x_push_const_xil_t<float>>(xil_type_t::float_, value);
                break;

            case value_type_t::double_:
                pool.append<x_push_const_xil_t<double>>(xil_type_t::double_, value);
                break;

            case value_type_t::bool_:
                pool.append<x_push_const_xil_t<bool>>(xil_type_t::bool_, value);
                break;

            case value_type_t::char_:
                pool.append<x_push_const_xil_t<char_t>>(xil_type_t::char_, value);
                break;

            default:
            case value_type_t::ldouble_:
                _PP(value.type);
                throw _EC(unexpected);
        }
    }

    // Compiles string.
    void __compile_string(__cctx_t & ctx, xil_pool_t & pool, const char_t * string)
    {
        pool.append<x_push_string_xil_t>(ctx.statement_ctx.assembly_layout().res_of(string));
    }

    // Compiles null.
    void __compile_null(__cctx_t & ctx, xil_pool_t & pool)
    {
        pool.append<x_push_null_xil_t>();
    }

    // Compiles type.
    void __compile_type(__cctx_t & ctx, xil_pool_t & pool, type_t * type)
    {
        pool.append<x_push_type_xil_t>(__ref_of(ctx, type));
    }

    // Compile constant value.
    void __compile_cvalue(__cctx_t & ctx, xil_pool_t & pool, cvalue_t cvalue, xil_type_t dtype)
    {
        switch (cvalue.value_type)
        {
            case cvalue_type_t::string:
                __compile_string(ctx, pool, cvalue.string);
                break;

            case cvalue_type_t::number:
                if (dtype != xil_type_t::empty)
                    cvalue = cvalue.change_type(to_vtype(dtype));
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

    // Compile constant value.
    void __sys_t<cvalue_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool, xil_type_t dtype)
    {
        if (!__is_this_effective(this))
            return;

        cvalue_t cvalue = *this->value;
        __compile_cvalue(ctx, pool, cvalue, dtype);
    }

    ////////// ////////// ////////// ////////// //////////
    // function_expression_t

    // Executes function expression.
    cvalue_t __sys_t<function_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }
    
    // Returns whether left is a base expression.
    static bool __left_is_base(expression_t * exp)
    {
        if (!__is_member_expression(exp))
            return false;

        binary_expression_t * bin_exp = (binary_expression_t *)exp;
        return bin_exp->exp1()->this_family() == expression_family_t::base;
    }

    // Compiles function expression.
    void __sys_t<function_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool, xil_type_t dtype)
    {
        function_expression_type_t ftype = this->get_ftype();

        switch (ftype)
        {
            case function_expression_type_t::method:
                __compile_method(ctx, pool, dtype);
                break;

            case function_expression_type_t::variable:  // Delegate
                __compile_delegate(ctx, pool, dtype);
                break;

            default:
                break;
        }
    }

    // Compile method calling expression.
    void __sys_t<function_expression_t>::__compile_method(__cctx_t & ctx, xil_pool_t & pool,
                                                          xil_type_t dtype)
    {
        method_base_t * method = this->get_method();
        _A(method != nullptr);

        bool effective = is_effective(this);
        xil_call_type_t call_type = call_type_of_method(method);
        type_t * ret_type = method->get_type();

        if (effective)
        {
            if (!__is_member_expression(this->parent) && !__is_static(call_type))
                pool.append<x_push_this_ref_xil_t>();

            // When return type is custom struct, but not assign to a variable.
            if (this->parent == nullptr && is_custom_struct(ret_type))
                pool.append<x_stack_alloc_xil_t>(__ref_of(ctx, ret_type));
        }

        __compile_arguments(ctx, pool, this->arguments(), method);

        if (effective)
        {
            ref_t method_ref = __search_method_ref(ctx, method);

            // _PF(_T("==== function call: %1%, %2%, %3%, %4%"), call_type, method->to_string(),
            //     (mt_member_extra_t)method_ref.extra, method_ref
            // );

            if (__left_is_base(this->parent) && call_type == xil_call_type_t::virtual_)
                pool.append<x_call_xil_t>(xil_call_type_t::instance, method_ref);
            else
                pool.append<x_call_xil_t>(call_type, method_ref);

            if (dtype != xil_type_t::empty)
                __try_append_convert_xil(pool, method->get_type(), dtype);
            else
                __pop_empty_for_method(ctx, pool, method, this);
        }
    }

    // Compile delegate calling expression.
    void __sys_t<function_expression_t>::__compile_delegate(__cctx_t & ctx, xil_pool_t & pool,
                                                            xil_type_t dtype)
    {
        variable_t * variable = this->get_variable();
        _A(variable != nullptr);

        type_t * type = variable->get_type(__xpool(ctx));
        if (type->this_gtype() != gtype_t::generic)
            throw _ED(__e_t::delegate_type_error, type);

        generic_type_t * gtype = (generic_type_t *)type;
        general_type_t * template_ = gtype->template_;

        if (template_ != __xpool(ctx).get_delegate_type())
            throw _ED(__e_t::delegate_type_error, gtype);

        _PP(type);
    }

    // Compile delegate calling expression.
    void __compile_delegate(expression_compile_context_t & ctx, xil_pool_t & pool);

    ////////// ////////// ////////// ////////// //////////
    // type_cast_expression_t

    // Executes type cast expression.
    cvalue_t __sys_t<type_cast_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }
    
    // Compiles type cast expression.
    void __sys_t<type_cast_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool,
                                                                    xil_type_t dtype)
    {
        expression_t * exp = this->expression();
        _A(exp != nullptr);

        /*
        variable_t * var;

        if (dtype1 == xil_type_t::ptr
            && exp->this_family() == expression_family_t::name
            && (var = ((name_expression_t *)exp)->get_variable()) != nullptr)
        {
            switch (var->this_type())
            {
                case variable_type_t::param: {

                    param_variable_t * param_var = (param_variable_t *)var;
                    int param_index = param_var->param->index;

                    method_t * method = ctx.statement_ctx.method;
                    if (!method->get_decorate()->is_static)
                        param_index++;

                    _PP(param_index);

                }   break;

                default:
                    X_UNEXPECTED();
            }
        }
        */

        exp->compile(ctx, pool, __xil_type(type_name));
    }

    ////////// ////////// ////////// ////////// //////////
    // type_name_expression_t

    // Executes type name expression.
    cvalue_t __sys_t<type_name_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }
    
    // Compiles type name expression.
    void __sys_t<type_name_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool,
                                                                        xil_type_t dtype)
    {
        // Type name.
    }

    ////////// ////////// ////////// ////////// //////////
    // index_expression_t

    // Executes index expression.
    cvalue_t __sys_t<index_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }
    
    // compiles index expression.
    void __sys_t<index_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool, xil_type_t dtype)
    {
        expression_t * namex = this->namex();
        if (namex == nullptr)
            throw _ED(__e_t::index_main_expression_missing);

        if (is_effective(this->parent))
        {
            variable_t * variable = __pick_var((variable_expression_t *)this);
            switch (variable->this_type())
            {
                case variable_type_t::array_index:
                    __compile_array_index(ctx, pool, (array_index_variable_t *)variable,
                                                                            namex, dtype);
                    break;

                case variable_type_t::property_index:
                    __compile_property_index(ctx, pool, (property_index_variable_t *)variable,
                                                                            namex, dtype);
                    break;

                default:
                    X_UNEXPECTED();
            }
        }
        else
        {
            __compile_arguments(ctx, pool, this->arguments());
        }
    }

    // Compiles array index expression.
    void __sys_t<index_expression_t>::__compile_array_index(__cctx_t & ctx, xil_pool_t & pool,
                array_index_variable_t * variable, expression_t * namex, xil_type_t dtype)
    {
        __compile_arguments(ctx, pool, this->arguments());
        namex->compile(ctx, pool);

        type_t * element_type = __array_element_type_of(ctx, this);
        type_t * array_type = __xpool(ctx).new_array_type(
            element_type, variable->dimension()
        );

        ref_t array_type_ref = __ref_of(ctx, array_type);

        pool.append<x_push_array_element_xil_t>(
            __to_xil_type(element_type), array_type_ref
        );
    }

    // Compiles property index expression.
    void __sys_t<index_expression_t>::__compile_property_index(__cctx_t & ctx, xil_pool_t & pool,
                property_index_variable_t * variable, expression_t * namex, xil_type_t dtype)
    {
        namex->compile(ctx, pool);
        __compile_property_index_variable(ctx, pool, variable, dtype, this);
    }

    ////////// ////////// ////////// ////////// //////////
    // new_expression_t

    // Executes name expression.
    cvalue_t __sys_t<new_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }
    
    // Returns constructor call type.
    xil_call_type_t __get_constructor_calltype(type_t * host_type, method_t * constructor)
    {
        #define __EConstructor(name)                                            \
            throw _ED(__e_t::unexpected_constructor_prototype__##name,          \
                                                            constructor);

        type_t * type = to_type(constructor->type_name);
        if (type != nullptr && !is_void_type(type))
            __EConstructor(should_no_return_type);

        xil_call_type_t call_type = call_type_of_method(constructor);
        switch (call_type)
        {
            case xil_call_type_t::static_:
                __EConstructor(should_no_static);
                break;

            case xil_call_type_t::virtual_:
                __EConstructor(should_no_virtual);
                break;

            default: break;
        }

        if (host_type != nullptr && !is_type_compatible(host_type, constructor->host_type))
            throw _ED(__e_t::unexpected_host_type, constructor, host_type);

        #undef __EConstructor

        return call_type;
    }

    // Compiles new expression.
    void __sys_t<new_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool, xil_type_t dtype)
    {
        type_t * type = to_type(this->type_name);
        if (type == nullptr)
            throw _ED(__e_t::type_missing);

        ttype_t ttype = type->this_ttype();

        switch (ttype)
        {
            case ttype_t::class_:
                __compile_new_class_object(ctx, pool, type);
                break;

            case ttype_t::struct_:
                __compile_new_struct_object(ctx, pool, type);
                break;

            default:
                throw _ED(__e_t::create_instance_type_error, type, ttype);
        }
    }

    // Creates instance of class type.
    void __sys_t<new_expression_t>::__compile_new_class_object(__cctx_t & ctx, xil_pool_t & pool,
                                                                        type_t * type)
    {
        ref_t type_ref = __ref_of(ctx, type);
        _A(type_ref != ref_t::null);

        pool.append<x_new_xil_t>(type_ref);

        bool is_parent_effective = is_effective(this->parent);
        if (this->constructor != nullptr && is_parent_effective)
        {
            pool.append<x_push_duplicate_xil_t>();
        }

        if (this->arguments() != nullptr)
        {
            if (this->constructor == nullptr)
                throw _ED(__e_t::constructor_missing, type);

            __compile_arguments(ctx, pool, this->arguments(), this->constructor);
        }

        if (this->constructor != nullptr)
        {
            xil_call_type_t call_type = __get_constructor_calltype(type, this->constructor);
            ref_t method_ref = __search_method_ref(ctx, this->constructor);
            pool.append<x_call_xil_t>(call_type, method_ref);
        }
        else if (!is_parent_effective)
        {
            pool.append<x_pop_empty_xil_t>(xil_type_t::object);
        }
    }

    // Creates instance of struct type.
    void __sys_t<new_expression_t>::__compile_new_struct_object(__cctx_t & ctx, xil_pool_t & pool,
                                                                        type_t * type)
    {
        if (this->arguments() != nullptr)
        {
            if (this->constructor == nullptr)
                throw _ED(__e_t::constructor_missing, type);

            __compile_arguments(ctx, pool, this->arguments(), this->constructor);
        }

        // custom constructor.
        if (this->constructor != nullptr && this->constructor->param_count() > 0)
        {
            xil_call_type_t call_type = __get_constructor_calltype(type, this->constructor);
            ref_t method_ref = __search_method_ref(ctx, this->constructor);
            pool.append<x_call_xil_t>(call_type, method_ref);
        }
        else    // default constructor.
        {
            ref_t type_ref = __ref_of(ctx, type);
            pool.append<x_object_copy_xil_t>(type_ref, xil_copy_kind_t::zero);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // new_array_expression_t

    // Executes new array expression.
    cvalue_t __sys_t<new_array_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }

    // Returns new array expression.
    void __sys_t<new_array_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool,
                                                                    xil_type_t dtype)
    {
        type_t * element_type = this->get_element_type();
        if (element_type == nullptr)
            throw _ED(__e_t::element_type_undeterminded);

        bool this_effective = __is_this_effective(this);

        // lengths
        if (this->lengths() != nullptr)
        {
            for (expression_t * expression : *this->lengths())
            {
                if (expression == nullptr)
                    throw _ED(__e_t::unknown_array_length, this);

                // TODO: must be int type

                expression->compile(ctx, pool);
            }
        }

        // new
        type_t * type = this->to_array_type(__xpool(ctx));

        if (type == nullptr)
            throw _ED(__e_t::type_missing);

        ref_t type_ref = __ref_of(ctx, type);
        _A(type_ref != ref_t::null);

        if (this_effective)
            pool.append<x_new_array_xil_t>(type_ref);

        // initializer
        array_initializer_t * initializer = this->initializer();
        if (initializer != nullptr && !initializer->empty())
        {
            xil_type_t dtype = __to_xil_type(element_type);

            if (this_effective)
                pool.append<x_array_begin_xil_t>(dtype);

            initializer->each_element([&](expression_t * element_exp) -> bool {

                if (element_exp == nullptr)
                    throw _ED(__e_t::element_init_expression_empty);

                // TODO: check init expression type.

                element_exp->compile(ctx, pool);

                if (this_effective)
                    pool.append<x_array_element_xil_t>(dtype);  // TODO: struct?

                return true;
            });

            if (this_effective)
                pool.append<x_array_end_xil_t>(dtype);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // default_value_expression_t

    // Executes default value expression.
    cvalue_t __sys_t<default_value_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }
    
    // Compiles default value expression.
    void __sys_t<default_value_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool,
                                                                        xil_type_t dtype)
    {
        type_t * type = to_type(this->type_name);

        if (type == nullptr)
            throw _ED(__e_t::type_missing);

        if (!is_effective(this->parent))
            return;

        switch (type->this_gtype())
        {
            case gtype_t::generic_param: {

                pool.append<x_push_generic_const_xil_t>(
                    __ref_of(ctx, (generic_param_t *)type)
                );

            }   break;

            case gtype_t::general: {

                vtype_t vtype = type->this_vtype();
                cvalue_t value = default_value_of(vtype);

                if (is_nan(value))
                    throw _ED(__e_t::no_default_value, type);

                __compile_cvalue(ctx, pool, value);

            }   break;

            case gtype_t::generic: {

                __compile_cvalue(ctx, pool, cvalue_t(nullptr)); // TODO: struct?

            }   break;

            case gtype_t::array: {

                __compile_cvalue(ctx, pool, cvalue_t(nullptr));

            }   break;

            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // type_of_expression_t

    // Executes typeof expression.
    cvalue_t __sys_t<type_of_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }
    
    // Compiles typeof expression.
    void __sys_t<type_of_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool, xil_type_t dtype)
    {
        // TODO
    }

    ////////// ////////// ////////// ////////// //////////
    // this_expression_t

    // Executes this expression.
    cvalue_t __sys_t<this_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }
    
    // Compiles this expression.
    void __sys_t<this_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool, xil_type_t dtype)
    {
        if (__is_this_effective(this))
            pool.append<x_push_this_ref_xil_t>();
    }

    ////////// ////////// ////////// ////////// //////////
    // base_expression_t

    // Executes base expression.
    cvalue_t __sys_t<base_expression_t>::execute(expression_execute_context_t & ctx)
    {
        return __super_t::execute(ctx);
    }
    
    // Compiles base expression.
    void __sys_t<base_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool, xil_type_t dtype)
    {
        if (__is_this_effective(this))
            pool.append<x_push_this_ref_xil_t>();
    }

    ////////// ////////// ////////// ////////// //////////

} } }

