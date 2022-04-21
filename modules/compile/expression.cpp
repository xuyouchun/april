#include <compile.h>
#include "utils.h"

namespace X_ROOT_NS::modules::compile {

    typedef expression_compile_context_t __cctx_t;
    typedef compile_error_code_t __e_t;

    template<typename _expression_t>
    using __sys_t = system_expression_t<_expression_t>;

    ////////// ////////// ////////// ////////// //////////

    // Compiles constant value.
    void __compile_cvalue(__cctx_t & ctx, xil_pool_t & pool, cvalue_t cvalue,
        xil_type_t dtype = xil_type_t::empty);

    // Pre call a method.
    static void __pre_call_method(__cctx_t & ctx, xil_pool_t & pool,
        expression_t * exp, method_base_t * method);

    // Compiles variable.
    static void __compile_variable(__cctx_t & ctx, xil_pool_t & pool, variable_t * variable,
                                xil_type_t dtype, expression_t * owner_exp = nullptr);

    ////////// ////////// ////////// ////////// //////////

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

    // Returns whether it's static call type.
    X_ALWAYS_INLINE static bool __is_static(xil_call_type_t call_type)
    {
        return call_type == xil_call_type_t::static_ || call_type == xil_call_type_t::internal;
    }

    // Returns whether it's a static method.
    X_ALWAYS_INLINE static bool __is_static(method_t * method)
    {
        _A(method != nullptr);

        return __is_static(call_type_of_method(method));
    }

    // Returns this method.
    X_ALWAYS_INLINE static method_t * __this_method(__cctx_t & cctx)
    {
        return cctx.statement_ctx.method;
    }

    // Returns xil type.
    X_ALWAYS_INLINE static xil_type_t __to_xil_type(type_t * type)
    {
        if (type == nullptr || is_generic_param(type))
            return xil_type_t::empty;

        return to_xil_type(type->this_vtype());
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns host type of a member.
    static type_t * __get_host_type(member_t * member)
    {
        _A(member != nullptr);

        type_t * host_type = member->host_type;
        __FailedWhenNull(host_type, "member '%1%' host type missing", member);

        return host_type;
    }

    // Executes the expression, returns nan when it is not a constant value.
    static cvalue_t __execute_expression(expression_t * exp)
    {
        _A(exp != nullptr);

        return exp->execute();
    }

    // Tries compile expression if it's a constant value.
    // Returns false when compile failed. ( not a constant value. )
    static bool __try_compile_constant_expression(__cctx_t & ctx, xil_pool_t & pool,
                    expression_t * expression, xil_type_t dtype = xil_type_t::empty)
    {
        if (!is_optimize(ctx, compile_optimize_code_t::compute_constant_values))
            return false;

        cvalue_t cvalue = __execute_expression(expression);

        if (!is_nan(cvalue) && cvalue.value_type != cvalue_type_t::__default__)
        {
            if (is_effective(ctx, expression))
                __compile_cvalue(ctx, pool, cvalue, dtype);

            return true;
        }

        return false;
    }

    // Returns whether it's a function call expression.
    bool __is_call_expression(expression_t * exp)
    {
        expression_behaviour_t behaviour = exp->get_behaviour();

        return behaviour == expression_behaviour_t::execute ||
               behaviour == expression_behaviour_t::new_;
    }

    // Try to fetch method variable of expression specified.
    static method_variable_t * __try_fetch_method_variable(expression_t * exp)
    {
        typedef expression_family_t ef_t;
        _A(al::in(exp->this_family(), ef_t::name, ef_t::name_unit));

        name_expression_t * name_exp = (name_expression_t *)exp;
        if (name_exp->expression_type == name_expression_type_t::variable)
        {
            variable_t * variable = name_exp->variable;
            if (variable->this_type() == variable_type_t::method)
                return (method_variable_t *)variable;
        }

        return nullptr;
    }

    #define __FakeVariable_PushCallingBottom            ((variable_t *)1)
    #define __Is_FakeVariable_PushCallingBottom(var)    (var == __FakeVariable_PushCallingBottom)

    // Try to fetch method variable of expression specified.
    static method_variable_t * __try_fetch_method_variable(expression_t * exp,
                                                           expression_t ** out_instance)
    {
        expression_family_t family = exp->this_family();

        if (family == expression_family_t::name)
        {
            al::assign(out_instance, nullptr);
            return __try_fetch_method_variable(exp);
        }

        if (family == expression_family_t::binary)
        {
            typedef expression_family_t ef_t;

            binary_expression_t * binary_exp = (binary_expression_t *)exp;
            if (binary_exp->op() == operator_t::member_point &&
                al::in(binary_exp->exp2()->this_family(), ef_t::name, ef_t::name_unit))
            {
                method_variable_t * variable = __try_fetch_method_variable(binary_exp->exp2());
                al::assign(out_instance, binary_exp->exp1());

                return variable;
            }
        }

        return nullptr;
    }

    // Real expression means it will generate xils when compiling.
    static bool __is_instance(expression_t * expression)
    {
        if (expression == nullptr)
            return false;

        switch (expression->this_family())
        {
            case expression_family_t::name:
            case expression_family_t::name_unit:
                return ((name_expression_t *)expression)->expression_type
                                        == name_expression_type_t::variable;

            default:
                return true;
        }
    }

    // Pre assign to a variable for delegate type.
    void __pre_delegate_assign(expression_compile_context_t & ctx, xil_pool_t & pool,
        method_variable_t * method_var, expression_t * instance)
    {
        _A(method_var != nullptr && method_var->method != nullptr);

        // Pushes instance.
        if (method_var->method->is_static())
        {
            if (__is_instance(instance))
            {
                __Failed("Instance reference '%1%' cannot be accessed"
                    "on static member '%2%'", instance, method_var->method);
            }

            if (instance == nullptr)
                pool.append<x_push_null_xil_t>();
            else
                instance->compile(ctx, pool);
        }
        else if (instance == nullptr)
        {
            pool.append<x_push_this_ref_xil_t>();
        }
        else
        {
            instance->compile(ctx, pool);
        }

        // Pushes method info.
        _A(method_var->method != nullptr);
        ref_t method_ref = __search_method_ref(ctx, method_var->method);
        pool.append<x_push_object_xil_t>(xil_storage_object_type_t::method_info, method_ref);

        type_t * delegate_type = method_var->get_type();
        // _PP(delegate_type);

        // TODO: check delegate prototype.

        // Check and call delegate.

        atypes_t atypes = {
            atype_t(__XPool.get_object_type()), atype_t(__XPool.get_method_type())
        };

        if (instance != nullptr && instance->this_family() == expression_family_t::base)
        {
            // base.xxx
            atypes.push_back(atype_t(__XPool.get_internal_type(vtype_t::int32_)));
            pool.append<x_push_const_xil_t<int32_t>>(xil_type_t::int32,
                tvalue_t((int32_t)xil_call_type_t::instance)
            );
        }

        analyze_member_args_t args(member_type_t::method, name_t::null, &atypes);
        args.method_trait = method_trait_t::constructor;
        method_t * constructor = (method_t *)delegate_type->get_member(args, true);
        _A(constructor != nullptr);

        ref_t constructor_method_ref = __search_method_ref(ctx, constructor);
        pool.append<x_method_call_xil_t>(xil_call_type_t::instance, constructor_method_ref);
    }

    // Try to do assignment for delegate types.
    bool __try_delegate_assign(expression_compile_context_t & ctx, xil_pool_t & pool,
        variable_t * variable, expression_t * expression)
    {
        expression_t * instance;
        method_variable_t * method_var = __try_fetch_method_variable(expression, &instance);

        if (method_var == nullptr)
            return false;

        if (__Is_FakeVariable_PushCallingBottom(variable))
            pool.append<x_push_calling_bottom_xil_t>();
        else if (variable != nullptr)
            __push_variable_address(ctx, pool, variable);

        __pre_delegate_assign(ctx, pool, method_var, instance);
        return true;
    }

    // Pre append custom struct for assign.
    void __pre_custom_struct_assign(expression_compile_context_t & ctx, xil_pool_t & pool,
        variable_t * variable, expression_t * expression)
    {
        type_t * type = expression->get_type();
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
        else if (__try_delegate_assign(ctx, pool, variable, expression))
        {
            // OK
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
        type_t * type = expression->get_type();
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
        else if (__try_delegate_assign(ctx, pool, __FakeVariable_PushCallingBottom, expression))
        {
            // OK
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

    bool __is_effective(expression_behaviour_t behaviour)
    {
        switch (behaviour)
        {
            case expression_behaviour_t::assign:
            case expression_behaviour_t::execute:
            case expression_behaviour_t::new_:
                return true;

            default:
                return false;
        }
    }

    // Returns whether it's effective.
    // Xils will not generated if the expression is not effective.
    bool is_effective(expression_t * exp)
    {
        if (exp == nullptr)
            return false;

        if (__is_effective(exp->get_behaviour()))
            return true;

        return is_effective(exp->parent);
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

    // Returns whether it is a member expression.
    static bool __is_member_expression(expression_t * exp, expression_t ** out_exp1,
                                                           expression_t ** out_exp2 = nullptr)
    {
        if (!__is_member_expression(exp))
            return false;

        binary_expression_t * binary_exp = (binary_expression_t *)exp;

        if (out_exp1 != nullptr)
            *out_exp1 = binary_exp->exp1();

        if (out_exp2 != nullptr)
            *out_exp2 = binary_exp->exp2();

        return true;
    }

    // Returns whether it's the left member expression.
    static bool __is_left_member(expression_t * exp, expression_t * exp1)
    {
        expression_t * exp0;
        return __is_member_expression(exp, &exp0, nullptr) && exp0 == exp1;
    }

    // Returns whether it's the left member expression.
    static bool __is_left_member(expression_t * exp, expression_t * exp1, bool recursion)
    {
        expression_t * exp0;
        if (!__is_member_expression(exp, &exp0))
            return false;

        if (exp0 == exp1)
            return true;

        expression_t * parent_exp;
        return !recursion || (parent_exp = exp->parent) == nullptr? false :
            __is_left_member(parent_exp, exp, true);
    }

    // Returns whether it's the right member expression.
    static bool __is_right_member(expression_t * exp, expression_t * exp2)
    {
        expression_t * exp0;
        return __is_member_expression(exp, nullptr, &exp0) && exp0 == exp2;
    }

    // Returns whether it's effective.
    static bool __is_this_effective(__cctx_t & ctx, expression_t * exp)
    {
        if (is_effective(ctx, exp->parent))
            return true;

        if (__is_member_expression(exp->parent))
        {
            binary_expression_t * binary_exp = (binary_expression_t *)exp->parent;
            return is_effective(ctx, binary_exp->exp2());
        }

        return false;
    }

    // Returns whether it's function namex. (function name or delegate expression)
    static bool __is_function_namex(expression_t * exp)
    {
        expression_t * parent_exp;
        if (exp == nullptr || (parent_exp = exp->parent) == nullptr)
            return false;

        if (parent_exp->this_family() != expression_family_t::function)
            return false;

        function_expression_t * func_exp = (function_expression_t *)parent_exp;
        return func_exp->namex == exp;
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

    ////////// ////////// ////////// ////////// //////////
    // Binary expressions

    static bool __is_optimize_basic_algorighm(__cctx_t & ctx)
    {
        return is_optimize(ctx, compile_optimize_code_t::optimize_basic_algorighm);
    }

    // Compiles add expression.
    static void __compile_add(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        if (__is_optimize_basic_algorighm(ctx))
        {
            if (__execute_expression(exp1) == cvalue_t(0))  // x + 0
            {
                exp2->compile(ctx, pool);
                return;
            }

            if (__execute_expression(exp2) == cvalue_t(0)) // 0 + x
            {
                exp1->compile(ctx, pool);
                return;
            }
        }

        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_al_xil_t>(
            xil_al_command_t::add, __xil_type(exp1), __xil_type(exp2)
        );
    }

    // Compiles sub expression.
    static void __compile_sub(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        if (__is_optimize_basic_algorighm(ctx))
        {
            if (__execute_expression(exp1) == cvalue_t(0))  // 0 * x
            {
                exp2->compile(ctx, pool);
                pool.append<x_al_xil_t>(xil_al_command_t::minus, __xil_type(exp1));
                return;
            }

            if (__execute_expression(exp2) == cvalue_t(0))  // x * 0
            {
                exp1->compile(ctx, pool);
                return;
            }
        }

        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_al_xil_t>(
            xil_al_command_t::sub, __xil_type(exp1), __xil_type(exp2)
        );
    }

    // Compiles mul expression.
    static void __compile_mul(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        if (__is_optimize_basic_algorighm(ctx))
        {
            if (__execute_expression(exp1) == cvalue_t(1)) // 1 * x
            {
                exp2->compile(ctx, pool);
                return;
            }

            if (__execute_expression(exp2) == cvalue_t(1))  // x * 1
            {
                exp1->compile(ctx, pool);
                return;
            }
        }

        // TODO x * 0 || 0 * x

        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_al_xil_t>(
            xil_al_command_t::mul, __xil_type(exp1), __xil_type(exp2)
        );
    }

    // Compiles div expression.
    static void __compile_div(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        if (__is_optimize_basic_algorighm(ctx))
        {
            if (__execute_expression(exp2) == cvalue_t(1))      // x / 1
            {
                exp1->compile(ctx, pool);
                return;
            }
        }

        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_al_xil_t>(
            xil_al_command_t::div, __xil_type(exp1), __xil_type(exp2)
        );
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
        if (__is_optimize_basic_algorighm(ctx))
        {
            if (__execute_expression(exp1) == cvalue_t(0))     // 0 | x
            {
                exp2->compile(ctx, pool);
                return;
            }

            if (__execute_expression(exp2) == cvalue_t(0))     // x | 0
            {
                exp1->compile(ctx, pool);
                return;
            }
        }

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
        if (__is_optimize_basic_algorighm(ctx))
        {
            if (__execute_expression(exp2) == cvalue_t(0))     // x << 0
            {
                exp1->compile(ctx, pool);
                return;
            }
        }

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
        if (__is_optimize_basic_algorighm(ctx))
        {
            if (__execute_expression(exp2) == cvalue_t(0))     // x >> 0
            {
                exp1->compile(ctx, pool);
                return;
            }
        }

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
        if (__is_call_expression(exp2))
        {
            // exp1 will compile during exp2 compiling.
            exp2->compile(ctx, pool);
        }
        else
        {
            exp1->compile(ctx, pool);
            exp2->compile(ctx, pool);
        }
    }

    // Compiles logic and expression.
    static void __compile_logic_and(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        if (__is_optimize_basic_algorighm(ctx))
        {
            if (__execute_expression(exp1) == cvalue_t(true))     // true && x
            {
                exp2->compile(ctx, pool);
                return;
            }

            if (__execute_expression(exp2) == cvalue_t(true))     // x && true
            {
                exp1->compile(ctx, pool);
                return;
            }
        }

        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_logic_xil_t>(xil_logic_command_t::and_);
    }

    // Compiles logic or expression.
    static void __compile_logic_or(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        if (__is_optimize_basic_algorighm(ctx))
        {
            if (__execute_expression(exp1) == cvalue_t(false))     // false || x
            {
                exp2->compile(ctx, pool);
                return;
            }

            if (__execute_expression(exp2) == cvalue_t(false))     // x || false
            {
                exp1->compile(ctx, pool);
                return;
            }
        }

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

    // Picks variable.
    static variable_t * __pick_var(variable_expression_t * var_exp)
    {
        variable_t * var = var_exp->get_variable();
        if (var == nullptr)
            __Failed("variable '%1%' connot determined", var_exp->to_string());

        return var;
    }

    // Picks variable.
    static variable_t * __pick_var(expression_t * exp)
    {
        variable_expression_t * var_exp = as<variable_expression_t *>(exp);
        if (var_exp == nullptr || !var_exp->is_variable_expression())
            __Failed("assign left expression should be a variable/index: found '%1%'", exp);

        return __pick_var(var_exp);
    }

    // Compile argument.
    static void __compile_argument(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp,
        param_types_t * param_types, int index)
    {
        _A(exp != nullptr);

        param_type_t param_type = param_type_t::default_;
        type_t * atype;

        if (param_types != nullptr)
        {
            typex_t typex = param_types->param_type_at(index);
            atype = (type_t *)typex;
            param_type = (param_type_t)typex;
        }
        else
        {
            atype = exp->get_type();
        }

        _A(atype != nullptr);

        // Ref or out argument.
        if (__is_addr(param_type))
        {
            variable_t * variable = __pick_var(exp);
            if (!variable->is_lvalue())
                __Failed("variable '%1%' cannot used as ref or out argument", variable);

            if (__is_addr(variable))
                __compile_variable(ctx, pool, variable, xil_type_t::ptr, exp);
            else
                __push_variable_address(ctx, pool, variable);

            return;
        }

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
            else if (__try_delegate_assign(ctx, pool, nullptr, exp))
            {
                // OK
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
            if (param_types != nullptr)
                vtype = param_types->param_vtype_at(index);

            xil_type_t xil_type = to_xil_type(vtype);

            if (!__try_compile_constant_expression(ctx, pool, exp, xil_type))
                exp->compile(ctx, pool, xil_type);
        }
    }

    // Compile arguments.
    static void __compile_arguments(__cctx_t & ctx, xil_pool_t & pool, arguments_t * arguments,
        param_types_t * param_types = nullptr)
    {
        if (arguments == nullptr)
            return;

        for (int index = 0, count = arguments->size(); index < count; index++)
        {
            argument_t * argument = (*arguments)[index];
            expression_t * exp = argument->expression;

            __compile_argument(ctx, pool, exp, param_types, index);
        }
    }

    enum class __assign_to_type_t { default_, pop, pick };

    // Returns array element type.
    static type_t * __array_element_type_of(__cctx_t & ctx, index_expression_t * exp)
    {
        variable_t * var = __pick_var((variable_expression_t *)exp);
        type_t * type = var->get_type();

        if (type == nullptr)
            __Failed("index '%1%' type undetermined", var);

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
        bool custom_struct = is_custom_struct(var->get_type());

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
                    __Failed("unknown property '%1%'", var);

                __compile_arguments(ctx, pool,
                    ((property_index_variable_t *)var)->arguments, property->set_method
                );
            }   break;

            default:
                break;
        }

        return __compile_assign_t { this_exp, var, exp, custom_struct };
    }


    // Compiles assign to expression.
    static void __compile_assign_to(__cctx_t & ctx, xil_pool_t & pool, __compile_assign_t & ca,
        expression_t * exp2, __assign_to_type_t assign_type = __assign_to_type_t::default_,
        xil_type_t dtype = xil_type_t::empty)
    {
        expression_t * exp = ca.exp;
        bool pick = (assign_type == __assign_to_type_t::default_)?
            is_effective(ctx, exp->parent->parent) : (assign_type == __assign_to_type_t::pick);

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
                    __Failed("unknown property '%1%'", var);

                method_t * method = property->set_method;
                if (method == nullptr)
                    __Failed("property '%1%' cannot be write", var);

                __validate_set_method(ctx, method);

                if (ca.custom_struct)
                    __compile_argument(ctx, pool, exp2, method, 0);

                ref_t method_ref = __search_method_ref(ctx, method);
                pool.append<x_method_call_xil_t>(call_type_of_method(method), method_ref);

            }   break;

            case variable_type_t::array_index: {
                array_index_variable_t * arr_var = (array_index_variable_t *)var;
                expression_t * body = arr_var->body;
                arguments_t * args = arr_var->arguments;

                if (body == nullptr)
                    __Failed("index '%1%' body missing", var);

                if (args == nullptr)
                    __Failed("index '%1%' arguments missing", var);

                __compile_arguments(ctx, pool, args);
                body->compile(ctx, pool);

                type_t * element_type = arr_var->get_type();
                type_t * array_type = __XPool.new_array_type(
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
                    __Failed("unknown property '%1%'", var);

                method_t * method = property->set_method;
                if (method == nullptr)
                    __Failed("property '%1%' cannot be write", var);

                __validate_set_method(ctx, method);

                ref_t method_ref = __search_method_ref(ctx, method);
                pool.append<x_method_call_xil_t>(xil_call_type_t::instance, method_ref);

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

    // Returns type if specified expression is name_expression_t or name_unit_expression_t,
    // and it is a type or type_def.
    static type_t * __to_actual_type(expression_t * exp)
    {
        _A(exp != nullptr);

        if (type_t * type = to_actual_type(exp); type != nullptr)
            return type;

        __Failed("%1% is not a type", exp);
    }

    // Throws exception when specified expression is not ref type.
    void __expect_ref_type(expression_t * exp)
    {
        if (!is_ref_type(exp->get_type()))
            __Failed("%1% is not a reference type expression", exp);
    }

    // Returns a variable when it's a variable expression, otherwise, throws exception
    variable_t * __expect_variable(expression_t * exp)
    {
        _A(exp != nullptr);

        if (is_name_expression(exp))
        {
            name_expression_t * name_exp = (name_expression_t *)exp;
            if (name_exp->expression_type == name_expression_type_t::variable)
                return name_exp->get_variable();
        }

        __Failed("%1% is not a variable expression", exp);
    }

    // Compiles null coalescing expression: ??
    static void __compile_null_coalescing(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __expect_ref_type(exp1);
        __expect_ref_type(exp2);

        if (__is_optimize_basic_algorighm(ctx))
        {
            if (__execute_expression(exp1) == cvalue_t(nullptr))
            {
                exp2->compile(ctx, pool);
                return;
            }

            if (__execute_expression(exp2) == cvalue_t(nullptr))
            {
                exp1->compile(ctx, pool);
                return;
            }
        }

        statement_compile_context_t & sctx = ctx.statement_ctx;

        exp1->compile(ctx, pool);

        // Test it's null?
        pool.append<x_push_duplicate_xil_t>();
        pool.append<x_push_null_xil_t>();
        pool.append<x_cmp_xil_t>(xil_cmp_command_t::equal, xil_type_t::object, xil_type_t::object);

        jmp_xil_t * finally_jmp_xil = xil::append_jmp_xil(pool, xil_jmp_model_t::false_);

        // Compiles exp2
        pool.append<x_pop_empty_xil_t>(xil_type_t::object);
        exp2->compile(ctx, pool);

        xil::append_label_xil(sctx, pool, finally_jmp_xil);
    }

    // Compiles null coalescing expression: ??=
    static void __compile_null_coalescing_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __expect_ref_type(exp1);
        __expect_ref_type(exp2);

        variable_t * var = __expect_variable(exp1);

        if (__is_optimize_basic_algorighm(ctx))
        {
            if (__execute_expression(exp2) == cvalue_t(nullptr))
                return;
        }

        statement_compile_context_t & sctx = ctx.statement_ctx;

        exp1->compile(ctx, pool);

        // Test it's null?
        pool.append<x_push_null_xil_t>();
        pool.append<x_cmp_xil_t>(xil_cmp_command_t::equal, xil_type_t::object, xil_type_t::object);

        jmp_xil_t * finally_jmp_xil = xil::append_jmp_xil(pool, xil_jmp_model_t::false_);

        // Compiles exp2
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        exp2->compile(ctx, pool);
        __compile_assign_to(ctx, pool, ca, exp2);

        xil::append_label_xil(sctx, pool, finally_jmp_xil);
    }

    // Compiles as expression.
    static void __compile_as(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);

        type_t * type = __to_actual_type(exp2);
        ref_t type_ref = __ref_of(ctx, type);
        _A(type_ref != ref_t::null);

        pool.append<x_cast_xil_t>(xil_cast_command_t::as, type_ref);
    }

    // Compiles is expression.
    static void __compile_is(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        exp1->compile(ctx, pool);

        type_t * type = __to_actual_type(exp2);
        ref_t type_ref = __ref_of(ctx, type);
        _A(type_ref != ref_t::null);

        pool.append<x_cast_xil_t>(xil_cast_command_t::is, type_ref);
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
        #define __Is(v) bit_has_flag(flag, __inc_t::v)

        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp);
        variable_t * var = ca.var;

        if (__Is(right))
        {
            if (is_effective(ctx, exp->parent->parent))
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
        __compile_inc(ctx, pool, exp, bit_or(__inc_t::left, __inc_t::increment));
    }

    // Compiles left decrement.
    static void __compile_left_decrement(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        __compile_inc(ctx, pool, exp, bit_or(__inc_t::left, __inc_t::decrement));
    }

    // Compiles right increment.
    static void __compile_right_increment(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        __compile_inc(ctx, pool, exp, bit_or(__inc_t::right, __inc_t::increment));
    }

    // Compiles right decrement.
    static void __compile_right_decrement(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        __compile_inc(ctx, pool, exp, bit_or(__inc_t::right, __inc_t::decrement));
    }

    ////////// ////////// ////////// ////////// //////////

    xil_type_t __to_xil_type(__cctx_t & ctx, variable_t * variable)
    {
        type_t * type = variable->get_type();
        if (type->this_gtype() == gtype_t::generic_param)
            return xil_type_t::empty;

        vtype_t vtype = variable->get_vtype();
        return to_xil_type(vtype);
    }

    // Compiles local variable.
    static void __compile_local_variable(__cctx_t & ctx, xil_pool_t & pool,
                    local_variable_t * variable, xil_type_t dtype = xil_type_t::empty)
    {
        //_PF(_T("-------- variable: %1%, %2%, %3%"), variable,
        //                variable->read_count, variable->write_count);

        xil_type_t xil_type = __to_xil_type(ctx, variable);

        if (variable->constant)
        {
            cvalue_t cvalue = __execute_expression(variable->expression);
            _A(cvalue != cvalue_t::nan);

            __compile_cvalue(ctx, pool, cvalue, dtype);
        }
        else
        {
            if (dtype == xil_type_t::empty)
                dtype = xil_type;

            type_t * type = variable->get_type();

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
            return;
        }

        msize_t index = variable->param->index;
        if (!__is_static(ctx.statement_ctx.method))
            index++;

        type_t * type = variable->get_type();

        // TODO: when member point, int, long ... types also should be ptr type.
        if (__is_addr(variable))
        {
            if (dtype == xil_type_t::ptr)
                pool.append<x_push_argument_xil_t>(xil_type_t::ptr, index);
            else
                pool.append<x_push_argument_content_xil_t>(__to_xil_type(ctx, variable), index);

            return;
        }

        if (is_custom_struct(type))
            pool.append<x_push_argument_addr_xil_t>(index);
        else
            pool.append<x_push_argument_xil_t>(__to_xil_type(ctx, variable), index);
    }

    // Compiles field variable.
    static void __compile_field_variable(__cctx_t & ctx, xil_pool_t & pool,
                                    field_variable_t * field_var, xil_type_t dtype)
    {
        field_t * field = field_var->field;
        if (field == nullptr)
            __Failed("unknown field '%1%'", field_var);

        ref_t field_ref = __search_field_ref(ctx, field);
        type_t * field_type = to_type(field->type_name);

        if (field_type == nullptr)
            __Failed("unknown field '%1%' type", field);

        // TODO: when member point, int, long ... types also should be ptr type.
        if (is_custom_struct(field_type))
        {
            pool.append<x_push_field_addr_xil_t>(field_ref);
        }
        else
        {
            xil_type_t xil_type = (dtype != xil_type_t::__unknown__)?
                                            dtype : __to_xil_type(field_type);
            pool.append<x_push_field_xil_t>(field_ref, xil_type);
        }
    }

    // Pre call a method.
    static void __pre_call_method(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp,
                                                                type_t * ret_type)
    {
        _A(exp != nullptr);

        // When return type is custom struct, but not assign to a variable.
        if (is_custom_struct(ret_type))
        {
            if (exp->parent == nullptr)
            {
                local_variable_t * variable = ctx.define_temp_local(ret_type);
                __compile_local_variable(ctx, pool, variable);
            }
            else if (__is_left_member(exp->parent, exp, true) || __is_function_namex(exp))
            {
                local_variable_t * variable = ctx.define_temp_local(ret_type);
                __compile_local_variable(ctx, pool, variable);

                if (is_effective(ctx, exp->parent))
                    pool.append<x_push_duplicate_xil_t>();
            }
        }

    }

    // Pre call a method.
    static void __pre_call_method(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp,
                                                                 method_base_t * method)
    {
        _A(method != nullptr);

        type_t * ret_type = method->get_type();
        __pre_call_method(ctx, pool, exp, ret_type);
    }

    // Post call a method.
    static void __post_call_method(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp,
                                                                method_base_t * method)
    {
        // Empty.
    }

    // Returns whether it's effective, for the whole expression.
    bool __is_whole_effective(expression_t * exp)
    {
        if (exp == nullptr)
            return false;

        if (__is_member_expression(exp))
            return __is_whole_effective(exp->parent);

        return __is_effective(exp->get_behaviour()) || __is_whole_effective(exp->parent);
    }

    // Pops return value from stack
    static bool __try_pop_empty_for_method(__cctx_t & ctx, xil_pool_t & pool,
                                        method_base_t * method, expression_t * owner_exp)
    {
        if (__is_whole_effective(owner_exp->parent))
            return false;

        type_t * ret_type = method->get_type();
        vtype_t vtype;

        if ((ret_type != nullptr && (vtype = ret_type->this_vtype()) != vtype_t::void_)
            && !is_custom_struct(ret_type))
        {
            xil_type_t xil_type = to_xil_type(vtype);
            pool.append<x_pop_empty_xil_t>(xil_type);
            return true;
        }

        return false;
    }

    // Compile property variable.
    static void __compile_property_variable(__cctx_t & ctx, xil_pool_t & pool,
        property_t * property, arguments_t * arguments, xil_type_t dtype, expression_t * owner_exp)
    {
        method_t * method = property->get_method;
        if (method == nullptr)
            __Failed("property '%1%' cannot be read", property);

        __validate_get_method(ctx, method);

        __compile_arguments(ctx, pool, arguments, method);

        ref_t method_ref = __search_method_ref(ctx, method);
        pool.append<x_method_call_xil_t>(call_type_of_method(method), method_ref);

        __try_pop_empty_for_method(ctx, pool, method, owner_exp);
    }

    // Compile property variable.
    static void __compile_property_variable(__cctx_t & ctx, xil_pool_t & pool,
            property_variable_t * property_var, xil_type_t dtype, expression_t * owner_exp)
    {
        property_t * property = property_var->property;
        if (property == nullptr)
            __Failed("unknown property '%1%'", property_var);

        __compile_property_variable(ctx, pool, property, nullptr, dtype, owner_exp);
    }

    // Compiles property index variable.
    static void __compile_property_index_variable(__cctx_t & ctx, xil_pool_t & pool,
            property_index_variable_t * property_var, xil_type_t dtype, expression_t * owner_exp)
    {
        property_t * property = property_var->property;
        if (property == nullptr)
            __Failed("unknown property '%1%'", property_var);

        __compile_property_variable(ctx, pool, property, property_var->arguments, dtype, owner_exp);
    }

    // Compiles method variable.
    static void __compile_method_variable(__cctx_t & ctx, xil_pool_t & pool,
            method_variable_t * method_var, xil_type_t dtype)
    {
        method_t * method = method_var->method;

        if (method == nullptr)
            __Failed("unknown method '%1%'", method_var);
    }

    // Check whether static member has instance object reference.
    static void __check_static_member(__cctx_t & ctx, expression_t * owner_exp,
                                                      variable_t * variable)
    {
        member_t * member;
        if (variable == nullptr || (member = variable->get_relation_member()) == nullptr)
            return;

        expression_t * left_exp;
        bool has_instance;

        if (__is_member_expression(owner_exp->parent, &left_exp))
            has_instance = __is_instance(left_exp);
        else
            has_instance = !ctx.statement_ctx.method->is_static();

        bool is_static = member->is_static();

        // _P(_T("__check_static_member"), owner_exp, variable, is_static, has_instance);

        if (is_static && has_instance)
            __Failed("instance reference '%1%' cannot be accessed on static member '%2%'",
                                                                        left_exp, member);

        if (!is_static && !has_instance)
            __Failed("An object reference is required for the non-static member '%1%'", member);
    }

    // Pushes this with check.
    static void __push_this_with_check(__cctx_t & ctx, xil_pool_t & pool,
                        expression_t * owner_exp, variable_t * variable)
    {
        __check_static_member(ctx, owner_exp, variable);

        if (owner_exp != nullptr && !__is_member_expression(owner_exp->parent))
            pool.append<x_push_this_ref_xil_t>();
    }

    // Pushes this with check for function.
    static void __push_this_with_check_for_function(__cctx_t & ctx, xil_pool_t & pool,
                                        expression_t * owner_exp, variable_t * variable)
    {
        if (owner_exp == nullptr)
            return;

        __check_static_member(ctx, owner_exp, variable);

        expression_t * exp1;
        if (__is_member_expression(owner_exp->parent, &exp1))
            exp1->compile(ctx, pool);
        else
            pool.append<x_push_this_ref_xil_t>();
    }

    // Compiles static const field.
    static void __compile_static_const_field(__cctx_t & ctx, xil_pool_t & pool,
                    field_t * field, xil_type_t dtype)
    {
        cvalue_t cv;
        if (field->init_value != nullptr)
            cv = field->init_value->execute();
        else
            cv = default_value_of(field->get_vtype());

        if (is_nan(cv))
            __Failed("expected constant value for static const field '%1%'", field->name);

        __compile_cvalue(ctx, pool, cv, dtype);
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

            case variable_type_t::field: {
                
                field_t * field = ((field_variable_t *)variable)->field;
                if (is_static_const(field))
                {
                    __compile_static_const_field(ctx, pool, field, dtype);
                }
                else
                {
                    __push_this_with_check(ctx, pool, owner_exp, variable);
                    __compile_field_variable(ctx, pool, (field_variable_t *)variable, dtype);
                }

            }   break;

            case variable_type_t::property: {
                property_variable_t * property_var = (property_variable_t *)variable;
                __pre_call_method(ctx, pool, owner_exp, property_var->property->get_method);
                __push_this_with_check_for_function(ctx, pool, owner_exp, variable);
                __compile_property_variable(ctx, pool, property_var, dtype, owner_exp);
            }   break;

            case variable_type_t::method:
                __push_this_with_check_for_function(ctx, pool, owner_exp, variable);
                __compile_method_variable(ctx, pool, (method_variable_t *)variable, dtype);
                break;

            default:
                X_UNEXPECTED();
        }
    }

    void __post_compile_expression(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp,
                  xil_type_t dtype, xil_type_t exp_dtype = xil_type_t::__unknown__)
    {
        xil_type_t xil_type = (exp_dtype == xil_type_t::__unknown__)? __xil_type(exp) : exp_dtype;

        if (xil_type != xil_type_t::empty && xil_type != xil_type_t::__unknown__)
        {
            if (exp->parent == nullptr && !__is_effective(exp->get_behaviour()))
                pool.append<x_pop_empty_xil_t>(__xil_type(exp));
            else if (xil_type != dtype)
                __try_append_convert_xil(pool, xil_type, dtype);
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Executes binary expression.
    cvalue_t __sys_t<binary_expression_t>::execute()
    {
        return __super_t::execute();
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
        if (!is_effective(ctx, this))
        {
            exp1->compile(ctx, pool);
            exp2->compile(ctx, pool);
        }
        else
        {
            #define __Case(op_)                                                         \
                case operator_t::op_:                                                   \
                    __compile_##op_(ctx, pool, exp1, exp2);                             \
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
                __Case(null_coalescing)
                __Case(null_coalescing_assign)

                __Case(as)
                __Case(is)

                default:
                    throw _ECF(not_supported, _T("unknown operator \"%1%\""), op());
            }

            #undef __Case

            __post_compile_expression(ctx, pool, this, dtype);
        }
    }

    // Checks overload method prototype.
    void __check_overload_prototype(__cctx_t & ctx, method_t * method, int param_count,
                                                    expression_t ** expressions)
    {
        _A(method != nullptr);

        if (method->param_count() != param_count)
            __Failed("operator overload method '%1%' prototype error: Param count error", method);

        if (method->is_generic())
            __Failed("operator overload method '%1%' prototype error: should not be a generic type",
                                                                                        method);

        if (!method->is_static())
            __Failed("Operator overload method '%1%' prototype error: should be static", method);

        for (int k = 0; k < param_count; k++)
        {
            expression_t * exp = expressions[k];
            _A(exp != nullptr);

            param_t * param = method->param_at(k);
            _A(param != nullptr);

            type_t * exp_type = exp->get_type();
            type_t * param_type = param->get_type();

            if (!is_type_compatible(exp_type, param_type))
            {
                __Failed("operator overload method '%1%(%2%)' argument type error: "
                         "cannot assign %1%(%2%) to param %3%(%4%)",
                    exp, exp_type, param, param_type
                );
            }
        }
    }

    // Compiles operator overload..
    void __sys_t<binary_expression_t>::__compile_operator_overload(__cctx_t & ctx, xil_pool_t & pool)
    {
        method_t * method = this->overload_method;

        // Checks method prototype.
        __check_overload_prototype(ctx, method, 2, this->exps);

        __pre_call_method(ctx, pool, this, method);

        // Compile arguments.
        __compile_argument(ctx, pool, expression_at(0), method, 0);
        __compile_argument(ctx, pool, expression_at(1), method, 1);

        ref_t method_ref = __search_method_ref(ctx, method);
        pool.append<x_method_call_xil_t>(xil_call_type_t::static_, method_ref);

        __try_pop_empty_for_method(ctx, pool, method, this);
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
    cvalue_t __sys_t<unitary_expression_t>::execute()
    {
        return __super_t::execute();
    }

    // Compiles unitary expression.
    void __sys_t<unitary_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool, xil_type_t dtype)
    {
        if (__try_compile_constant_expression(ctx, pool, this, dtype))
            return;

        expression_t * exp = expression_at(0);
        _A(exp != nullptr);

        if (this->overload_method != nullptr)
        {
            __compile_operator_overload(ctx, pool);
            return;
        }

        if (!is_effective(ctx, this))
        {
            exp->compile(ctx, pool);
        }
        else
        {
            #define __Case(op_)                                                         \
                case operator_t::op_:                                                   \
                    __compile_##op_(ctx, pool, exp);                                    \
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

            __post_compile_expression(ctx, pool, this, dtype);
        }
    }

    // Compiles operator overload..
    void __sys_t<unitary_expression_t>::__compile_operator_overload(__cctx_t & ctx,
                                                                    xil_pool_t & pool)
    {
        method_t * method = this->overload_method;

        // Checks method prototype.
        __check_overload_prototype(ctx, method, 1, this->exps);

        __pre_call_method(ctx, pool, this, method);

        // Compile arguments.
        __compile_argument(ctx, pool, expression_at(0), method, 0);

        ref_t method_ref = __search_method_ref(ctx, method);
        pool.append<x_method_call_xil_t>(xil_call_type_t::static_, method_ref);

        __try_pop_empty_for_method(ctx, pool, method, this);
    }

    ////////// ////////// ////////// ////////// //////////
    // name_expression_t

    // Executes name expression.
    cvalue_t __sys_t<name_expression_t>::execute()
    {
        return __super_t::execute();
    }

    // Compiles name expression.
    void __sys_t<name_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool, xil_type_t dtype)
    {
        if (!__is_this_effective(ctx, this))
            return;

        switch (this->expression_type)
        {
            case name_expression_type_t::variable:
                __compile_variable(ctx, pool, this->variable, dtype, this);
                __post_compile_expression(ctx, pool, this, dtype);
                break;

            case name_expression_type_t::type:
            case name_expression_type_t::type_def:
                break;

            default:
                _PP(this);
                _PP(this->expression_type);
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // name_unit_expression_t

    // Executes name expression.
    cvalue_t __sys_t<name_unit_expression_t>::execute()
    {
        return __super_t::execute();
    }

    // Compiles name unit expression.
    void __sys_t<name_unit_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool,
                                                                  xil_type_t dtype)
    {
        if (!__is_this_effective(ctx, this))
            return;

        switch (this->expression_type)
        {
            case name_expression_type_t::variable:
                __compile_variable(ctx, pool, this->variable, dtype, this);
                __post_compile_expression(ctx, pool, this, dtype);
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
    cvalue_t __sys_t<cvalue_expression_t>::execute()
    {
        return __super_t::execute();
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
                X_UNEXPECTED_F("unexpected value type '%1%'", value.type);
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
                X_UNEXPECTED_F("unexpected cvalue type '%1%'", cvalue.value_type);
        }
    }

    // Compile constant value.
    void __sys_t<cvalue_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool, xil_type_t dtype)
    {
        if (!__is_this_effective(ctx, this))
            return;

        cvalue_t cvalue = *this->value;
        __compile_cvalue(ctx, pool, cvalue, dtype);

        __post_compile_expression(ctx, pool, this, dtype, dtype);
    }

    ////////// ////////// ////////// ////////// //////////
    // function_expression_t

    // Executes function expression.
    cvalue_t __sys_t<function_expression_t>::execute()
    {
        return __super_t::execute();
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

            case function_expression_type_t::__default__:
                __compile_default(ctx, pool, dtype);
                break;

            default:
                X_UNEXPECTED();
                break;
        }
    }

    static xil_call_command_t __to_command_command(xil_call_type_t call_type, name_t name)
    {
        if (call_type != xil_call_type_t::internal)
            return xil_call_command_t::none;

    #define __IsInternalCommand(_name, _command)                                        \
        {                                                                               \
            static name_t static_##_name;                                               \
                                                                                        \
            if (static_##_name == name_t::null)                                         \
                static_##_name = __XPool.to_name(_S(_name));                            \
                                                                                        \
            if (static_##_name == name)                                                 \
                return xil_call_command_t::_command;                                    \
        }

        __IsInternalCommand(Delegate_Invoke, delegate_invoke)
        __IsInternalCommand(Delegate_Init, delegate_init)
        __IsInternalCommand(Delegate_InitWithCallType, delegate_init_with_call_type)

        return xil_call_command_t::none;
    }

    // Compile method calling expression.
    void __sys_t<function_expression_t>::__compile_method(__cctx_t & ctx, xil_pool_t & pool,
                                                          xil_type_t dtype)
    {
        method_base_t * method = this->get_method();
        _A(method != nullptr);

        xil_call_type_t call_type = call_type_of_method(method);
        bool effective = is_effective(ctx, this);

        if (effective)
        {
            if (__is_right_member(this->parent, this))
            {
                __pre_call_method(ctx, pool, this->parent, method);
                ((binary_expression_t *)this->parent)->exp1()->compile(ctx, pool);
            }
            else
            {
                bool is_static = __is_static(call_type);

                if (!is_static || __is_function_namex(this))
                    __pre_call_method(ctx, pool, this, method);

                if (!is_static)
                    pool.append<x_push_this_ref_xil_t>();
            }
            
            __compile_arguments(ctx, pool, this->arguments(), method);

            ref_t method_ref = __search_method_ref(ctx, method);

            // _PF(_T("==== function call: %1%, %2%, %3%, %4%"), call_type, method->to_string(),
            //     (mt_member_extra_t)method_ref.extra, method_ref
            // );

            if (__left_is_base(this->parent) && call_type == xil_call_type_t::virtual_)
            {
                pool.append<x_method_call_xil_t>(xil_call_type_t::instance, method_ref);
            }
            else
            {
                xil_call_command_t cmd = __to_command_command(call_type, method->get_name());
                if (cmd != xil_call_command_t::none)
                    pool.append<x_command_call_xil_t>(cmd);
                else
                    pool.append<x_method_call_xil_t>(call_type, method_ref);
            }

            if (dtype != xil_type_t::empty)
                __try_append_convert_xil(pool, method->get_type(), dtype);
            else
                __try_pop_empty_for_method(ctx, pool, method, this);
        }
        else
        {
            expression_t * exp1;

            if (__is_member_expression(this->parent, &exp1))
                exp1->compile(ctx, pool);

            __compile_arguments(ctx, pool, this->arguments(), method);
        }
    }

    // Compile delegate calling expression.
    void __sys_t<function_expression_t>::__compile_delegate(__cctx_t & ctx, xil_pool_t & pool,
                                                            xil_type_t dtype)
    {
        // Validate delegate type.
        variable_t * variable = this->get_variable();
        _A(variable != nullptr);

        type_t * type = variable->get_type();
        if (type->this_gtype() != gtype_t::generic)
            __Failed("delegate type error, should be System.Deletate<...>, but not %1%", type);

        generic_type_t * gtype = (generic_type_t *)type;
        general_type_t * template_ = gtype->template_;

        if (template_ != __XPool.get_delegate_type())
            __Failed("delegate type error, should be System.Deletate<...>, but not %1%", gtype);

        __compile_variable(ctx, pool, variable, xil_type_t::ptr, this);

        analyze_member_args_t args(member_type_t::method, __XPool.to_name(_T("Invoke")));
        method_t * method = (method_t *)gtype->get_member(args, true);
        _A(method != nullptr);

        __compile_arguments(ctx, pool, this->arguments(), method);

        ref_t method_ref = __search_method_ref(ctx, method);
        pool.append<x_method_call_xil_t>(xil_call_type_t::instance, method_ref);

        if (dtype != xil_type_t::empty)
            __try_append_convert_xil(pool, method->get_type(), dtype);
        else
            __try_pop_empty_for_method(ctx, pool, method, this);
    }

    // Compile delegate calling expression, for complex namex expressions.
    void __sys_t<function_expression_t>::__compile_default(__cctx_t & ctx, xil_pool_t & pool,
                                                              xil_type_t dtype)
    {
        if (this->namex == nullptr)     // TODO: why nullptr?
            return;

        type_t * type = this->namex->get_type();
        _A(type != nullptr);

        if (type->this_gtype() != gtype_t::generic)
            __Failed("delegate type error, should be System.Deletate<...>, but not %1%", type);

        generic_type_t * gtype = (generic_type_t *)type;
        general_type_t * template_ = gtype->template_;

        if (template_ != __XPool.get_delegate_type())
            __Failed("delegate type error, should be System.Deletate<...>, but not %1%", gtype);

        analyze_member_args_t args(member_type_t::method, __XPool.to_name(_T("Invoke")));
        method_t * method = (method_t *)gtype->get_member(args, true);
        _A(method != nullptr);

        __pre_call_method(ctx, pool, namex, method);
        namex->compile(ctx, pool);

        __compile_arguments(ctx, pool, this->arguments(), method);

        ref_t method_ref = __search_method_ref(ctx, method);
        pool.append<x_method_call_xil_t>(xil_call_type_t::instance, method_ref);

        if (dtype != xil_type_t::empty)
            __try_append_convert_xil(pool, method->get_type(), dtype);
        else
            __try_pop_empty_for_method(ctx, pool, method, this);
    }

    // Compile delegate calling expression.
    void __compile_delegate(expression_compile_context_t & ctx, xil_pool_t & pool);

    ////////// ////////// ////////// ////////// //////////
    // type_cast_expression_t

    // Executes type cast expression.
    cvalue_t __sys_t<type_cast_expression_t>::execute()
    {
        return __super_t::execute();
    }
    
    // Compiles type cast expression.
    void __sys_t<type_cast_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool,
                                                                    xil_type_t dtype)
    {
        if (!is_effective(ctx, this))
            return;

        expression_t * exp = this->expression();
        __FailedWhenNull(exp, _T("expresion empty"));

        type_t * type = this->get_type();
        __FailedWhenNull(type, _T("type empty"));

        if (is_object(type))
        {
            exp->compile(ctx, pool);

            ref_t type_ref = __ref_of(ctx, type);
            pool.append<x_cast_xil_t>(xil_cast_command_t::default_, type_ref);
        }
        else
        {
            exp->compile(ctx, pool, __xil_type(type_name));
        }

        __post_compile_expression(ctx, pool, this, dtype);
    }

    ////////// ////////// ////////// ////////// //////////
    // type_name_expression_t

    // Executes type name expression.
    cvalue_t __sys_t<type_name_expression_t>::execute()
    {
        return __super_t::execute();
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
    cvalue_t __sys_t<index_expression_t>::execute()
    {
        return __super_t::execute();
    }
    
    // compiles index expression.
    void __sys_t<index_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool, xil_type_t dtype)
    {
        expression_t * namex = this->namex();
        if (namex == nullptr)
            __Failed("index main expression missing");

        if (is_effective(ctx, this->parent))
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
        type_t * array_type = __XPool.new_array_type(
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
    cvalue_t __sys_t<new_expression_t>::execute()
    {
        return __super_t::execute();
    }
    
    // Returns constructor call type.
    xil_call_type_t __get_constructor_calltype(type_t * host_type, method_t * constructor)
    {
        type_t * type = to_type(constructor->type_name);
        if (type != nullptr && !is_void_type(type))
            __Failed("constructor '%1%' should no return type", constructor);

        xil_call_type_t call_type = call_type_of_method(constructor);
        switch (call_type)
        {
            case xil_call_type_t::static_:
                __Failed("constructor '%1%' should no static", constructor);

            case xil_call_type_t::virtual_:
                __Failed("constructor '%1%' should no virtual", constructor);

            default: break;
        }

        if (host_type != nullptr && !is_type_compatible(host_type, constructor->host_type))
            __Failed("member '%1%' has unexpected host type '%2%'", constructor, host_type);

        #undef __EConstructor

        return call_type;
    }

    // Compiles new expression.
    void __sys_t<new_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool, xil_type_t dtype)
    {
        type_t * type = to_type(this->type_name);
        if (type == nullptr)
            __Failed("type missing");

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
                __Failed("Cannot create instance of type '%1%', because it's a '%2%' type",
                                                                            type, ttype);
        }
    }

    // Creates instance of class type.
    void __sys_t<new_expression_t>::__compile_new_class_object(__cctx_t & ctx, xil_pool_t & pool,
                                                                        type_t * type)
    {
        ref_t type_ref = __ref_of(ctx, type);
        _A(type_ref != ref_t::null);

        pool.append<x_new_xil_t>(type_ref);

        bool is_parent_effective = is_effective(ctx, this->parent);
        if (this->constructor != nullptr && is_parent_effective)
        {
            pool.append<x_push_duplicate_xil_t>();
        }

        if (this->arguments() != nullptr)
        {
            if (this->constructor == nullptr)
                __Failed("constructor of type '%1%' missing", type);

            __compile_arguments(ctx, pool, this->arguments(), this->constructor);
        }

        if (this->constructor != nullptr)
        {
            xil_call_type_t call_type = __get_constructor_calltype(type, this->constructor);
            ref_t method_ref = __search_method_ref(ctx, this->constructor);
            pool.append<x_method_call_xil_t>(call_type, method_ref);
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
        bool need_call_constructor = this->constructor != nullptr
                                  && this->constructor->param_count() > 0;

        if (need_call_constructor)
        {
            if (__is_left_member(this->parent, this))
            {
                local_variable_t * variable = ctx.define_temp_local(type);
                __compile_local_variable(ctx, pool, variable);

                if (is_effective(ctx, this->parent))
                    pool.append<x_push_duplicate_xil_t>();
            }
        }

        if (this->arguments() != nullptr)
        {
            if (this->constructor == nullptr)
                __Failed("constructor of type '%1%' missing", type);

            __compile_arguments(ctx, pool, this->arguments(), this->constructor);
        }

        // custom constructor.
        if (need_call_constructor)
        {
            xil_call_type_t call_type = __get_constructor_calltype(type, this->constructor);
            ref_t method_ref = __search_method_ref(ctx, this->constructor);
            pool.append<x_method_call_xil_t>(call_type, method_ref);
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
    cvalue_t __sys_t<new_array_expression_t>::execute()
    {
        return __super_t::execute();
    }

    // Returns new array expression.
    void __sys_t<new_array_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool,
                                                                    xil_type_t dtype)
    {
        type_t * element_type = this->get_element_type();
        if (element_type == nullptr)
            __Failed("index element type underterminded");

        bool this_effective = __is_this_effective(ctx, this);

        // lengths
        if (this->lengths() != nullptr)
        {
            for (expression_t * expression : *this->lengths())
            {
                if (expression == nullptr)
                    __Failed("unknown array length: '%1%'", this);

                // TODO: must be int type

                expression->compile(ctx, pool);
            }
        }

        // new
        type_t * type = this->to_array_type();

        if (type == nullptr)
            __Failed("type missing");

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
                    __Failed("index element init expression empty");

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
    cvalue_t __sys_t<default_value_expression_t>::execute()
    {
        return __super_t::execute();
    }
    
    // Compiles default value expression.
    void __sys_t<default_value_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool,
                                                                        xil_type_t dtype)
    {
        type_t * type = to_type(this->type_name);

        if (type == nullptr)
            __Failed("type missing");

        if (is_void_type(type))
            return;

        if (!is_effective(ctx, this->parent))
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
                    __Failed("type '%1%' has no default value", type);

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
    cvalue_t __sys_t<type_of_expression_t>::execute()
    {
        return __super_t::execute();
    }
    
    // Compiles typeof expression.
    void __sys_t<type_of_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool, xil_type_t dtype)
    {
        // TODO
    }

    ////////// ////////// ////////// ////////// //////////
    // this_expression_t

    // Executes this expression.
    cvalue_t __sys_t<this_expression_t>::execute()
    {
        return __super_t::execute();
    }
    
    // Compiles this expression.
    void __sys_t<this_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool, xil_type_t dtype)
    {
        if (!__is_this_effective(ctx, this))
            return;

        if (__is_left_member(this->parent, this))
        {
            pool.append<x_push_this_ref_xil_t>();
        }
        else
        {
            method_t * method = ctx.statement_ctx.method;
            vtype_t vtype = method->host_type->this_vtype();

            if (is_system_value_type(vtype))
            {
                if (dtype == xil_type_t::empty)
                    dtype = to_xil_type(vtype);

                pool.append<x_push_this_content_xil_t>(dtype);
            }
            else
            {
                pool.append<x_push_this_ref_xil_t>();
            }
        }

        __post_compile_expression(ctx, pool, this, dtype);
    }

    ////////// ////////// ////////// ////////// //////////
    // base_expression_t

    // Executes base expression.
    cvalue_t __sys_t<base_expression_t>::execute()
    {
        return __super_t::execute();
    }
    
    // Compiles base expression.
    void __sys_t<base_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool, xil_type_t dtype)
    {
        if (__is_this_effective(ctx, this))
            pool.append<x_push_this_ref_xil_t>();
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::compile

