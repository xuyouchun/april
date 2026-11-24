#include <compile.h>
#include "utils.h"

namespace X_ROOT_NS::modules::compile {

    typedef expression_compile_context_t        __cctx_t;
    typedef expression_compile_environment_t    __environment_t;

    typedef compile_error_code_t __e_t;

    template<typename _expression_t>
    using __sys_t = system_expression_t<_expression_t>;

    ////////// ////////// ////////// ////////// //////////

    // Compiles constant value.
    void __compile_cvalue(__cctx_t & ctx, xil_pool_t & pool, cvalue_t cvalue,
            const __environment_t & env = __environment_t::empty);

    // Pre call a method.
    static void __pre_call_method(__cctx_t & ctx, xil_pool_t & pool,
            expression_t * exp, method_base_t * method);

    // Compiles variable.
    static void __compile_variable(__cctx_t & ctx, xil_pool_t & pool, variable_t * variable,
            const __environment_t & env = __environment_t::empty,
            expression_t * owner_exp = nullptr);

    // Compiles arguments.
    static void __compile_arguments(__cctx_t & ctx, xil_pool_t & pool, arguments_t * arguments,
                                                        param_types_t * param_types = nullptr);

    // Compiles an argument.
    static void __compile_argument(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp,
        param_types_t * param_types, int index, bool expand);

    // Pushes the address of p.Value, p[i] of a pointer p (the pointer itself, the pointer moved by i): the argument ref of
    // Ptr.AddressOf(ref p[i]). Returns false when the variable is not one of them.
    static bool __try_push_pointer_member_address(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp,
                                                  expression_t * this_exp, variable_t * variable);

    // Returns whether the property is one of a generic param (t.Value, where T : IFoo), that is
    // not static.
    static bool __is_constrained_property(property_t * property)
    {
        if (property == nullptr || property->is_static())
            return false;

        type_t * host = property->host_type;
        return host != nullptr && is_generic_param(host);
    }

    // Returns whether the expression is the param that is the items of a params generic
    // parameter (TArgs ... args).
    static bool __is_pack_variable(expression_t * exp)
    {
        if (exp == nullptr || exp->this_family() != expression_family_t::name)
            return false;

        variable_t * variable = ((name_expression_t *)exp)->variable;
        return variable != nullptr && variable->this_type() == variable_type_t::param
            && ((param_variable_t *)variable)->param->ptype == param_type_t::extends;
    }

    // Returns whether the expression is a variable (a param, a local, a field) whose value is
    // a generic param: it is the address of the value.
    static bool __is_generic_value_variable(expression_t * exp)
    {
        if (exp == nullptr || exp->this_family() != expression_family_t::name)
            return false;

        variable_t * variable = ((name_expression_t *)exp)->variable;
        return variable != nullptr && is_real_variable(variable);
    }

    // Returns whether the expression is a field that is read through an object (this.Value,
    // other.Value): its value is a variable too.
    static bool __is_generic_value_field(expression_t * exp)
    {
        if (exp == nullptr || exp->this_family() != expression_family_t::binary)
            return false;

        binary_expression_t * binary = (binary_expression_t *)exp;
        return binary->op() == operator_t::member_point
                    && __is_generic_value_variable(binary->exp2());
    }

    static bool __is_property_name(expression_t * exp);

    // Compiles a field that is read through an object (this.Value): the object, and the field as
    // a variable, without the copy of the value that the expression makes.
    static void __compile_generic_field(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        binary_expression_t * binary = (binary_expression_t *)exp;

        binary->exp1()->compile(ctx, pool);
        __compile_variable(ctx, pool, ((name_expression_t *)binary->exp2())->variable,
                                                    __environment_t::empty, binary->exp2());
    }

    // Returns whether the expression is the result of a call: a function, a member that is a
    // function, an operator that is a method.
    static bool __is_generic_call_result(expression_t * exp)
    {
        if (exp == nullptr)
            return false;

        // c? a : b: its value is in the stack, the branches are loaded or called.
        if (exp->is_condition())
            return true;

        switch (exp->this_family())
        {
            case expression_family_t::function:
                return true;

            // A property, or an indexer: its getter is a method.
            case expression_family_t::name:
                return __is_property_name(exp);

            case expression_family_t::index: {
                variable_expression_t * var_exp = as<variable_expression_t *>(exp);
                variable_t * variable = var_exp != nullptr? var_exp->get_variable() : nullptr;
                return variable != nullptr && variable->this_type() == variable_type_t::property_index;
            }

            case expression_family_t::binary: {
                binary_expression_t * binary = (binary_expression_t *)exp;
                if (binary->op() == operator_t::member_point)
                    return __is_generic_call_result(binary->exp2());

                return binary->overload_method != nullptr && !binary->op_property->is_assign;
            }

            case expression_family_t::unitary:
                return ((unitary_expression_t *)exp)->overload_method != nullptr;

            // ((a + b)): the value of what is in the parentheses.
            case expression_family_t::expressions:
                return exp->expression_count() == 1 && __is_generic_call_result(exp->expression_at(0));

            // checked(a + b): the value of the expression that is in it.
            case expression_family_t::checked:
                return __is_generic_call_result(((checked_expression_t *)exp)->expression());

            // (T)x that a conversion of the interface of T makes: the conversion is a call that returns T.
            case expression_family_t::type_cast: {
                type_cast_expression_t * cast = (type_cast_expression_t *)exp;
                type_t * to = cast->get_type();

                if (!is_generic_param(to) || cast->expression() == nullptr)
                    return false;

                // (T)obj: the value that the object is (see the cast of a generic param).
                type_t * from = cast->expression()->get_type();
                if (from != nullptr && is_ref_type(from) && !is_generic_param(from))
                    return true;

                return find_user_conversion(from, to, !cast->is_implicit, false) != nullptr;
            }

            default:
                return false;
        }
    }

    // Compiles the object of a call of a member of a generic param (t.ToString(), T.Zero.ToString()): the
    // object is a reference, a value, or the address of a struct. A variable is its address. The value that a
    // call returned (T.Zero, T.Abs(a), (a + b)) is held in a local variable first, as the variable is: the
    // value that is the object of a member has the place of a struct that the call writes at, that is not made
    // when the generic param is a number, so it is compiled as the value in the parentheses is.
    static void __compile_constrained_receiver(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp1,
                                                                    type_t * host_type = nullptr)
    {
        type_t * type = exp1->get_type();

        if (is_generic_param(type) && __is_generic_call_result(exp1))
        {
            local_variable_t * temp = ctx.define_temp_local(type);

            if (exp1->this_family() == expression_family_t::expressions)
            {
                // What is in the parentheses is assigned, as the initial value of a variable is.
                compile_assign(ctx, pool, temp, exp1->expression_at(0));
            }
            else
            {
                // The same, with the parentheses that are not written: the call is not the left member.
                expression_t * parent = exp1->parent;

                expressions_t wrapper;
                wrapper.append(exp1);
                wrapper.parent = parent;

                compile_assign(ctx, pool, temp, exp1);

                exp1->parent = parent;
            }

            __push_variable_address(ctx, pool, temp);
        }
        else if (host_type != nullptr)
        {
            exp1->compile(ctx, pool, host_type);
        }
        else
        {
            exp1->compile(ctx, pool);
        }

        pool.append<x_generic_copy_xil_t>(__ref_of(ctx, type), xil_copy_kind_t::receiver);
    }

    // Returns whether the expression is the name of a property.
    static bool __is_property_name(expression_t * exp)
    {
        if (exp == nullptr || exp->this_family() != expression_family_t::name)
            return false;

        variable_t * variable = ((name_expression_t *)exp)->variable;
        return variable != nullptr && variable->this_type() == variable_type_t::property;
    }

    // Returns whether it's a reference type.
    static bool __is_ref_type(type_t * type)
    {
        return type != nullptr && is_ref_type(type);
    }

    // If env.type is ref type, returns xil_type_t::empty
    static xil_type_t __only_value_type(const __environment_t & env)
    {
        if (__is_ref_type(env.type))
            return xil_type_t::empty;

        return env.dtype;
    }

    // Returns call type of method of runtime.
    static xil_call_type_t __call_type_of_method(method_base_t * method)
    {
        return call_type_of_method(method, true);
    }

    // Returns whether the type is custom struct of generic param.
    static bool __is_custom_struct_like(type_t * type)
    {
        return is_custom_struct(type) || is_generic_param(type);
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns xil type of the expression.
    static xil_type_t __xil_type(expression_t * exp)
    {
        vtype_t vtype = exp->get_vtype();
        return to_xil_type(vtype);
    }

    // Returns xil type of type_name.
    static xil_type_t __xil_type(type_name_t * type_name)
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

        return __is_static(__call_type_of_method(method));
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

    // Returns param index.
    static int __param_index(expression_compile_context_t & ctx, param_variable_t * variable)
    {
        int index = ((param_variable_t *)variable)->param->index;

        if (!__is_static(ctx.statement_ctx.method))
            index++;

        return index;
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
            expression_t * expression, const __environment_t & env = __environment_t::empty)
    {
        if (!is_optimize(ctx, compile_optimize_code_t::compute_constant_values))
            return false;

        cvalue_t cvalue = __execute_expression(expression);

        if (!is_nan(cvalue) && cvalue.value_type != cvalue_type_t::__default__)
        {
            if (is_effective(ctx, expression))
                __compile_cvalue(ctx, pool, cvalue, env);

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

    // Returns whether it's a new expression.
    bool __is_new_expression(expression_t * exp)
    {
        expression_behaviour_t behaviour = exp->get_behaviour();
        return behaviour == expression_behaviour_t::new_;
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
            al::assign_value(out_instance, nullptr);
            return __try_fetch_method_variable(exp);
        }

        if (family == expression_family_t::binary)
        {
            typedef expression_family_t ef_t;

            binary_expression_t * binary_exp = (binary_expression_t *)exp;
            if (binary_exp->op() == operator_t::member_point &&
                is_name_expression(binary_exp->exp2()))
            {
                method_variable_t * variable = __try_fetch_method_variable(binary_exp->exp2());
                al::assign_value(out_instance, binary_exp->exp1());

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
    void __do_delegate_assign(expression_compile_context_t & ctx, xil_pool_t & pool,
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
            // The environment of a lambda that uses local variables.
            if (method_var->method->lambda_env_new != nullptr)
                method_var->method->lambda_env_new->compile(ctx, pool);
            else
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

        __do_delegate_assign(ctx, pool, method_var, instance);
        return true;
    }

    // Push this expression.
    static void __push_this(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        if (exp == nullptr)
            pool.append<x_push_this_ref_xil_t>();
        else
            exp->compile(ctx, pool);
    }

    // Returns whether the variable is a static field.
    static bool __is_static_field(variable_t * variable)
    {
        return variable != nullptr && variable->this_type() == variable_type_t::field
            && ((field_variable_t *)variable)->field != nullptr
            && ((field_variable_t *)variable)->field->is_static();
    }

    // Pushes the object that holds the static fields of the type of the field. It is used
    // in place of the instance, the field is accessed with its offset like the ones of an
    // instance.
    static void __push_static_data(__cctx_t & ctx, xil_pool_t & pool, field_t * field)
    {
        type_t * host_type = field->host_type;
        __FailedWhenNull(host_type, "static field '%1%' has no host type", field);

        // [ThreadStatic]: each thread has its own copy of the static fields of the type (the data of the other fields of
        // the type are in the copies too, they are not used: the shared ones are in the object of the others).
        pool.append<x_push_object_xil_t>(
            is_thread_static_field(field)? xil_storage_object_type_t::thread_static_data
                                         : xil_storage_object_type_t::static_data,
            __ref_of(ctx, host_type)
        );
    }

    // Pushes the object of a field: the instance, or the static data.
    // Pushes the state object of an iterator, when the variable is a field of it.
    static bool __push_hoist_base(__cctx_t & ctx, xil_pool_t & pool, variable_t * variable)
    {
        if (variable == nullptr || variable->this_type() != variable_type_t::field)
            return false;

        variable_t * base = ((field_variable_t *)variable)->hoist_base;
        if (base == nullptr)
            return false;

        switch (base->this_type())
        {
            case variable_type_t::local:        // The cell of a variable that a lambda modifies.
                pool.append<x_push_local_xil_t>(xil_type_t::object, ((local_variable_t *)base)->identity);
                break;

            case variable_type_t::field:        // The field of the environment of a lambda that has the cell.
                pool.append<x_push_this_ref_xil_t>();
                pool.append<x_push_field_xil_t>(
                    __search_field_ref(ctx, ((field_variable_t *)base)->field), xil_type_t::object
                );
                break;

            default:
                pool.append<x_push_argument_xil_t>(
                    xil_type_t::object, __param_index(ctx, (param_variable_t *)base)
                );
                break;
        }

        return true;
    }

    static void __push_field_owner(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp,
                                                                      variable_t * variable)
    {
        if (__is_static_field(variable))
            __push_static_data(ctx, pool, ((field_variable_t *)variable)->field);
        else if (!__push_hoist_base(ctx, pool, variable))
            __push_this(ctx, pool, exp);
    }

    // Returns whether need to call constructor of a struct.
    bool __need_call_constructor(new_expression_t * exp)
    {
        _A(exp != nullptr);

        if (exp->constructor == nullptr)
            return false;

        type_t * type = exp->get_type();
        _A(type != nullptr);

        if (is_value_type(type))
            return exp->constructor->param_count() > 0;

        return true;
    }

    static bool __try_compile_nullable_value(__cctx_t & ctx, xil_pool_t & pool, type_t * to,
                                                                        expression_t * exp);

    static bool __try_compile_nullable_null_compare(__cctx_t & ctx, xil_pool_t & pool,
                            expression_t * exp1, expression_t * exp2, bool equal);

    // Pre append custom struct for return.
    void __do_custom_struct_return(expression_compile_context_t & ctx, xil_pool_t & pool,
        expression_t * expression, const __environment_t & env)
    {
        _A(expression != nullptr);

        pool.append<x_push_calling_bottom_xil_t>();

        // A value, or null, returned as a nullable.
        if (get_nullable_underlying(env.type) != nullptr
                && get_nullable_underlying(expression->get_type()) == nullptr)
        {
            __try_compile_nullable_value(ctx, pool, env.type, expression);
            return;
        }

        _A(__is_custom_struct_like(expression->get_type()));
        expression->compile(ctx, pool, env);
    }

    // param_types_t with specified param types.
    class __static_param_types_t : public param_types_t
    {
    public:

        __static_param_types_t(type_t * type, size_t param_count)
            : __type(type), __param_count(param_count)
        {
            _A(type != nullptr);
        }

        // The indices of an array are integers, not of the type of the elements.
        __static_param_types_t(array_index_variable_t * arr_var)
            : __static_param_types_t(__XPool.get_internal_type(vtype_t::int32_),
                                                                arr_var->dimension())
        { }

        // Returns type count
        virtual size_t param_count() const override
        {
            return __param_count;
        }

        // Returns type at specified index.
        virtual typex_t param_type_at(size_t index) const override
        {
            return __type;
        }

    private:
        type_t * __type;
        size_t   __param_count;
    };

    // Push address of variable.
    void __push_variable_address(expression_compile_context_t & ctx, xil_pool_t & pool,
        variable_t * variable, expression_t * this_)
    {
        switch (variable->this_type())
        {
            case variable_type_t::local:
                pool.append<x_push_local_addr_xil_t>(
                    ((local_variable_t *)variable)->identity
                );
                break;

            case variable_type_t::param: {
                if (__is_addr(variable))
                {
                    pool.append<x_push_argument_xil_t>(
                        xil_type_t::ptr,
                        __param_index(ctx, (param_variable_t *)variable)
                    );
                }
                else
                {
                    pool.append<x_push_argument_addr_xil_t>(
                        __param_index(ctx, (param_variable_t *)variable)
                    );
                }
            }   break;

            case variable_type_t::field:
                __push_field_owner(ctx, pool, this_, variable);
                pool.append<x_push_field_addr_xil_t>(
                    __search_field_ref(ctx, ((field_variable_t *)variable)->field)
                );
                break;

            case variable_type_t::array_index: {
                array_index_variable_t * var = (array_index_variable_t *)variable;

                __FailedWhenNull(var->arguments, "index '%1%' arguments missing", var);
                __FailedWhenNull(var->body, "index '%1%' body missing", var);

                __static_param_types_t param_types(var);
                __compile_arguments(ctx, pool, var->arguments, &param_types);
                var->body->compile(ctx, pool);

                pool.append<x_push_array_element_addr_xil_t>(
                    __ref_of(ctx, var->get_array_type())
                );
            }   break;

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

    // Returns the expression that holds the value of exp, as the member sees it: ((T)new T(..)).X, the cast of
    // a struct to its own type changes nothing, the value is still what the member is read from. A tuple literal
    // is what it is lowered to (the new expression is its content).
    static expression_t * __see_through(expression_t * exp)
    {
        while (exp->parent != nullptr)
        {
            expression_t * parent = exp->parent;

            if (parent->this_family() == expression_family_t::tuple)
            {
                exp = parent;
                continue;
            }

            if (parent->this_family() != expression_family_t::type_cast)
                break;

            type_t * type = parent->get_type(), * exp_type = exp->get_type();
            if (type == nullptr || exp_type == nullptr || !is_custom_struct(type) || type != exp_type)
                break;

            exp = parent;
        }

        return exp;
    }

    // Returns whether it's the left member expression.
    static bool __is_left_member(expression_t * exp)
    {
        exp = __see_through(exp);

        if (exp->parent == nullptr)
            return false;

        return __is_left_member(exp->parent, exp);
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

    // Returns whether left is a base expression.
    static bool __left_is_base(expression_t * exp)
    {
        if (!__is_member_expression(exp))
            return false;

        binary_expression_t * bin_exp = (binary_expression_t *)exp;
        return bin_exp->exp1()->this_family() == expression_family_t::base;
    }

    // Returns whether it's the right member expression.
    static bool __is_right_member(expression_t * exp, expression_t * exp2,
                                  expression_t ** out_exp1 = nullptr)
    {
        expression_t * exp0;
        return __is_member_expression(exp, out_exp1, &exp0) && exp0 == exp2;
    }

    // Returns whether it's the right member expression.
    static bool __is_right_member(expression_t * exp, expression_t ** out_exp1)
    {
        if (exp->parent == nullptr)
            return false;

        return __is_right_member(exp->parent, exp, out_exp1);
    }

    // Returns whether it's the left member of calling a method.
    static bool __is_calling_method_owner(expression_t * exp)
    {
        if (exp->parent == nullptr)
            return false;

        expression_t * exp2;
        if (!__is_member_expression(exp->parent, nullptr, &exp2))
            return false;

        _A(exp2 != nullptr);
        return exp2->this_family() == expression_family_t::function;
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
    // function namex is like: myfunc()()
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

    // Returns whether the expression is the object that an indexer is read from (items.Slice(1)[0]): the object of a call.
    static bool __is_index_namex(expression_t * exp)
    {
        expression_t * parent_exp;
        if (exp == nullptr || (parent_exp = exp->parent) == nullptr)
            return false;

        if (parent_exp->this_family() != expression_family_t::index)
            return false;

        index_expression_t * index_exp = (index_expression_t *)parent_exp;
        return index_exp->namex() == exp && index_exp->variable != nullptr
            && index_exp->variable->this_type() == variable_type_t::property_index;
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

    // Checks that a property that has an init accessor is set where it can be: in an object
    // initializer, in a constructor of its type, or by the accessor of it.
    static void __check_init_only(expression_compile_context_t & ctx, property_t * property)
    {
        if (!property->is_init_only || !ctx.initializer_targets.empty())
            return;

        method_t * current = ctx.statement_ctx.method;
        if (current != nullptr && current->host_type == property->host_type
                && (current->trait == method_trait_t::constructor || current == property->set_method))
            return;

        __Failed("the property '%1%' has an init accessor: it is set only in an object initializer, "
                 "in a constructor of its type, or by its own accessor", property);
    }

    // Compiles number.
    void __compile_number(__cctx_t & ctx, xil_pool_t & pool, const tvalue_t & value);

    ////////// ////////// ////////// ////////// //////////
    // Binary expressions

    static bool __is_optimize_basic_algorighm(__cctx_t & ctx)
    {
        return is_optimize(ctx, compile_optimize_code_t::optimize_basic_algorighm);
    }

    // Returns the range of an integer type.
    static bool __integer_range(vtype_t vtype, __int128 & lo, __int128 & hi)
    {
        switch (vtype)
        {
            case vtype_t::int8_:    lo = INT8_MIN;  hi = INT8_MAX;      return true;
            case vtype_t::uint8_:   lo = 0;         hi = UINT8_MAX;     return true;
            case vtype_t::int16_:   lo = INT16_MIN; hi = INT16_MAX;     return true;
            case vtype_t::uint16_:  lo = 0;         hi = UINT16_MAX;    return true;
            case vtype_t::int32_:   lo = INT32_MIN; hi = INT32_MAX;     return true;
            case vtype_t::uint32_:  lo = 0;         hi = UINT32_MAX;    return true;
            case vtype_t::int64_:   lo = INT64_MIN; hi = INT64_MAX;     return true;
            case vtype_t::uint64_:  lo = 0;         hi = UINT64_MAX;    return true;
            default:                return false;
        }
    }

    // Wraps a value to an integer type, like unchecked.
    static __int128 __wrap_integer(vtype_t vtype, __int128 value)
    {
        uint64_t u = (uint64_t)value;

        switch (vtype)
        {
            case vtype_t::int8_:    return (int8_t)u;
            case vtype_t::uint8_:   return (uint8_t)u;
            case vtype_t::int16_:   return (int16_t)u;
            case vtype_t::uint16_:  return (uint16_t)u;
            case vtype_t::int32_:   return (int32_t)u;
            case vtype_t::uint32_:  return (uint32_t)u;
            case vtype_t::int64_:   return (int64_t)u;
            default:                return u;
        }
    }

    // Calculates the value of a constant integer expression: + - * of constants.
    // Returns false if it is not such an expression. `overflow` is set if the value of the
    // expression or of one of its parts, in checked, does not fit the type.
    static bool __constant_integer(expression_t * exp, __int128 & value, bool & overflow)
    {
        __int128 lo, hi;
        if (exp == nullptr || !__integer_range(exp->get_vtype(), lo, hi))
            return false;

        switch (exp->this_family())
        {
            case expression_family_t::checked: {
                checked_expression_t * checked_exp = (checked_expression_t *)exp;

                bool inner_overflow = false;
                if (!__constant_integer(checked_exp->expression(), value, inner_overflow))
                    return false;

                if (checked_exp->checked)
                    overflow = overflow || inner_overflow;
                else
                    value = __wrap_integer(exp->get_vtype(), value);

                return true;
            }

            case expression_family_t::binary: {
                binary_expression_t * binary_exp = (binary_expression_t *)exp;
                operator_t op = binary_exp->op();

                if (op != operator_t::add && op != operator_t::sub && op != operator_t::mul)
                    break;

                __int128 a, b;
                if (!__constant_integer(binary_exp->exp1(), a, overflow)
                        || !__constant_integer(binary_exp->exp2(), b, overflow))
                {
                    return false;
                }

                value = op == operator_t::add? a + b : op == operator_t::sub? a - b : a * b;
                overflow = overflow || value < lo || value > hi;
                return true;
            }

            case expression_family_t::unitary: {
                unitary_expression_t * unitary_exp = (unitary_expression_t *)exp;
                if (unitary_exp->op() != operator_t::minus)
                    break;

                __int128 a;
                if (!__constant_integer(unitary_exp->expression_at(0), a, overflow))
                    return false;

                value = -a;
                overflow = overflow || value < lo || value > hi;
                return true;
            }

            default:
                break;
        }

        cvalue_t cvalue = __execute_expression(exp);
        if (!is_number(cvalue))
            return false;

        value = is_signed_vtype(exp->get_vtype())? (__int128)(int64_t)cvalue
                                                  : (__int128)(uint64_t)cvalue;
        return true;
    }

    // In checked, the operation of constants that does not fit its type is an error.
    static void __check_constant_overflow(__cctx_t & ctx, operator_t op,
                                          expression_t * exp1, expression_t * exp2 = nullptr)
    {
        if (!ctx.checked)
            return;

        __int128 a, b = 0, value, lo, hi;
        bool overflow = false;

        if (!__constant_integer(exp1, a, overflow)
                || (exp2 != nullptr && !__constant_integer(exp2, b, overflow)))
        {
            return;
        }

        if (exp2 == nullptr)    // - a
        {
            // The minimum of a signed type has no opposite.
            if (__integer_range(exp1->get_vtype(), lo, hi) && lo != 0 && a == lo)
                overflow = true;
        }
        else
        {
            vtype_t vtype = get_binary_vtype(op, exp1->get_vtype(), exp2->get_vtype());
            if (!__integer_range(vtype, lo, hi))
                return;

            value = op == operator_t::add? a + b : op == operator_t::sub? a - b : a * b;
            overflow = overflow || value < lo || value > hi;
        }

        if (overflow)
        {
            cpl_log_errorf(ctx.statement_ctx.mctx.logger, exp1,
                                            common_log_code_t::constant_overflow);
        }
    }

    // Returns the command of an arithmetic: the one that raises OverflowException when it is in
    // checked(expression) or checked { ... }, and the operands are integers.
    static xil_al_command_t __al_command(__cctx_t & ctx, xil_al_command_t cmd,
                    xil_type_t xtype1, xil_type_t xtype2 = xil_type_t::__default__)
    {
        if (!ctx.checked)
            return cmd;

        if (!is_integer_vtype(to_vtype(xtype1))
                || (xtype2 != xil_type_t::__default__ && !is_integer_vtype(to_vtype(xtype2))))
        {
            return cmd;
        }

        switch (cmd)
        {
            case xil_al_command_t::add:     return xil_al_command_t::checked_add;
            case xil_al_command_t::sub:     return xil_al_command_t::checked_sub;
            case xil_al_command_t::mul:     return xil_al_command_t::checked_mul;
            case xil_al_command_t::minus:   return xil_al_command_t::checked_minus;
            default:                        return cmd;
        }
    }

    // The virtual machine has no arithmetic of Char: c + 1 and c - 'a' are made of Int32 (the Char is converted
    // first), and the result of c + 1, a Char, is converted back (it wraps at 16 bits, as (Char)(c + 1)).
    // Returns false when no operand is a Char.
    static bool __compile_char_arithmetic(__cctx_t & ctx, xil_pool_t & pool, xil_al_command_t cmd,
                                          operator_t op, expression_t * exp1, expression_t * exp2)
    {
        xil_type_t xtype1 = __xil_type(exp1), xtype2 = __xil_type(exp2);
        if (xtype1 != xil_type_t::char_ && xtype2 != xil_type_t::char_)
            return false;

        vtype_t result = get_binary_vtype(op, to_vtype(xtype1), to_vtype(xtype2));

        xil_type_t wide1 = xtype1 == xil_type_t::char_? xil_type_t::int32 : xtype1;
        xil_type_t wide2 = xtype2 == xil_type_t::char_? xil_type_t::int32 : xtype2;

        exp1->compile(ctx, pool);
        __try_append_convert_xil(pool, xtype1, wide1);

        exp2->compile(ctx, pool);
        __try_append_convert_xil(pool, xtype2, wide2);

        pool.append<x_al_xil_t>(__al_command(ctx, cmd, wide1, wide2), wide1, wide2);

        if (result == vtype_t::char_)
        {
            __try_append_convert_xil(pool, get_binary_vtype(op, to_vtype(wide1), to_vtype(wide2)),
                                                                            xil_type_t::char_);
        }

        return true;
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

        __check_constant_overflow(ctx, operator_t::add, exp1, exp2);

        if (__compile_char_arithmetic(ctx, pool, xil_al_command_t::add, operator_t::add, exp1, exp2))
            return;

        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_al_xil_t>(
            __al_command(ctx, xil_al_command_t::add, __xil_type(exp1), __xil_type(exp2)),
            __xil_type(exp1), __xil_type(exp2)
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
                pool.append<x_al_xil_t>(
                    __al_command(ctx, xil_al_command_t::minus, __xil_type(exp1)), __xil_type(exp1)
                );
                return;
            }

            if (__execute_expression(exp2) == cvalue_t(0))  // x * 0
            {
                exp1->compile(ctx, pool);
                return;
            }
        }

        __check_constant_overflow(ctx, operator_t::sub, exp1, exp2);

        if (__compile_char_arithmetic(ctx, pool, xil_al_command_t::sub, operator_t::sub, exp1, exp2))
            return;

        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_al_xil_t>(
            __al_command(ctx, xil_al_command_t::sub, __xil_type(exp1), __xil_type(exp2)),
            __xil_type(exp1), __xil_type(exp2)
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

        __check_constant_overflow(ctx, operator_t::mul, exp1, exp2);

        exp1->compile(ctx, pool);
        exp2->compile(ctx, pool);

        pool.append<x_al_xil_t>(
            __al_command(ctx, xil_al_command_t::mul, __xil_type(exp1), __xil_type(exp2)),
            __xil_type(exp1), __xil_type(exp2)
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

        // Booleans: both are evaluated.
        if (exp1->get_vtype() == vtype_t::bool_ && exp2->get_vtype() == vtype_t::bool_)
        {
            pool.append<x_logic_xil_t>(xil_logic_command_t::and_);
            return;
        }

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

        // Booleans: both are evaluated.
        if (exp1->get_vtype() == vtype_t::bool_ && exp2->get_vtype() == vtype_t::bool_)
        {
            pool.append<x_logic_xil_t>(xil_logic_command_t::or_);
            return;
        }

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

        // Booleans: they are different.
        if (exp1->get_vtype() == vtype_t::bool_ && exp2->get_vtype() == vtype_t::bool_)
        {
            pool.append<x_cmp_xil_t>(
                xil_cmp_command_t::not_equal, xil_type_t::bool_, xil_type_t::bool_
            );
            return;
        }

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
                                expression_t * exp1, expression_t * exp2,
                                const __environment_t & env = __environment_t::empty)
    {
        // The member is what the whole is: the value of a generic param that is returned is
        // the value of its member.
        __environment_t member_env = env.is_value_mark()?
                                __environment_t(env.type_mark) : __environment_t::empty;

        if (__is_call_expression(exp2))
        {
            // exp1 will compile during exp2 compiling.
            exp2->compile(ctx, pool, member_env);
        }
        else
        {
            exp1->compile(ctx, pool);
            exp2->compile(ctx, pool, member_env);
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

        // Short circuit: the second operand is not evaluated when the first is false.
        statement_compile_context_t & sctx = ctx.statement_ctx;

        exp1->compile(ctx, pool);
        jmp_xil_t * false_jmp_xil = xil::append_jmp_xil(pool, xil_jmp_model_t::false_);

        exp2->compile(ctx, pool);
        jmp_xil_t * end_jmp_xil = xil::append_jmp_xil(pool, xil_jmp_model_t::none);

        xil::append_label_xil(sctx, pool, false_jmp_xil);
        __compile_cvalue(ctx, pool, cvalue_t(false));

        xil::append_label_xil(sctx, pool, end_jmp_xil);
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

        // Short circuit: the second operand is not evaluated when the first is true.
        statement_compile_context_t & sctx = ctx.statement_ctx;

        exp1->compile(ctx, pool);
        jmp_xil_t * true_jmp_xil = xil::append_jmp_xil(pool, xil_jmp_model_t::true_);

        exp2->compile(ctx, pool);
        jmp_xil_t * end_jmp_xil = xil::append_jmp_xil(pool, xil_jmp_model_t::none);

        xil::append_label_xil(sctx, pool, true_jmp_xil);
        __compile_cvalue(ctx, pool, cvalue_t(true));

        xil::append_label_xil(sctx, pool, end_jmp_xil);
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
        if (__try_compile_nullable_null_compare(ctx, pool, exp1, exp2, true))
            return;

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
        if (__try_compile_nullable_null_compare(ctx, pool, exp1, exp2, false))
            return;

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
    // Converts an expression to a nullable type: the address of the destination is on the top
    // of the stack, and it is consumed. Returns false if it is not a conversion to Nullable<T>.
    static bool __try_compile_nullable_value(__cctx_t & ctx, xil_pool_t & pool, type_t * to,
                                                                        expression_t * exp)
    {
        type_t * underlying = get_nullable_underlying(to);
        if (underlying == nullptr)
            return false;

        type_t * from = exp->get_type();
        if (get_nullable_underlying(from) != nullptr)
            return false;

        // null: no value.
        if (from->this_gtype() == gtype_t::null)
        {
            pool.append<x_object_copy_xil_t>(__ref_of(ctx, to), xil_copy_kind_t::zero);
            return true;
        }

        // The constructor takes the value.
        atypes_t atypes = { atype_t(underlying) };

        analyze_member_args_t args(member_type_t::method, name_t::null, &atypes);
        args.method_trait = method_trait_t::constructor;

        method_t * constructor = (method_t *)to->get_member(args, true);
        __FailedWhenNull(constructor, "constructor of '%1%' not found", to);

        // A struct is passed as the arguments of a call are (a copy of it).
        if (__is_custom_struct_like(underlying))
            __compile_argument(ctx, pool, exp, constructor, 0, false);
        else
            exp->compile(ctx, pool, underlying);

        pool.append<x_method_call_xil_t>(
            xil_call_type_t::instance, __search_method_ref(ctx, constructor)
        );

        return true;
    }

    static void __compile_argument(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp,
        param_types_t * param_types, int index, bool expand = false)
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
            // A field of an object (o.f): the object is pushed first.
            expression_t * this_exp = nullptr, * r_exp = exp;
            if (__is_member_expression(exp))
            {
                this_exp = ((binary_expression_t *)exp)->exp1();
                r_exp    = ((binary_expression_t *)exp)->exp2();
            }

            variable_t * variable = __pick_var(r_exp);
            if (!variable->is_lvalue())
                __Failed("variable '%1%' cannot used as ref or out argument", variable);

            variable_type_t vtype = variable->this_type();
            if (vtype == variable_type_t::property || vtype == variable_type_t::property_index)
            {
                if (__try_push_pointer_member_address(ctx, pool, exp, this_exp, variable))
                    return;

                __Failed("the property '%1%' cannot be used as a ref or out argument", variable);
            }

            if (__is_addr(variable))
                __compile_variable(ctx, pool, variable, xil_type_t::ptr, exp);
            else
                __push_variable_address(ctx, pool, variable, this_exp);

            return;
        }

        // The items of a params generic parameter that are the arguments of the call: f(items ...).
        if (expand)
        {
            if (__is_pack_variable(exp))
            {
                pool.append<x_push_params_values_xil_t>();
                return;
            }

            // A tuple of the items (tuple ...): the items of it are the arguments.
            generic_param_t * pack = get_tuple_pack_param(exp->get_type());
            if (pack == nullptr)
            {
                __Failed("only the items of a params generic parameter, or a tuple of them, can be "
                         "passed with ...");
            }

            type_collection_t tuple_args;
            tuple_args.push_back(pack);

            exp->compile(ctx, pool);
            pool.append<x_push_tuple_values_xil_t>(__ref_of(ctx,
                    __XPool.new_generic_type(__XPool.get_tuple_type(), tuple_args, nullptr)));
            return;
        }

        // The items without ...: it is the address of them, for the methods of the runtime.
        if (__is_pack_variable(exp) && is_generic_param(atype)
                && ((generic_param_t *)atype)->param_type == generic_param_type_t::params)
        {
            __Failed("the items of a params generic parameter are passed to a method that takes "
                     "them with ..., (items ...)");
        }

        // The value of a generic param that a call returns (Pass(Get<T>(x))): the argument is
        // not made before the call, it is made by the protocol of the value of a call. A struct is
        // written at the place that is made for it, any other value is in the stack: it is not
        // known when the code is compiled, and a place that is not used moves the object of the
        // call of a delegate.
        if (is_generic_param(atype) && __is_generic_call_result(exp))
        {
            exp->compile(ctx, pool, __environment_t(type_mark_t::argument));
            return;
        }

        // A generic param passed to a reference (F(Object o) called with a T): the object it is,
        // made by the copy of the value (a boxed value, or the reference).
        if (exp->get_type() != nullptr && is_generic_param(exp->get_type())
                && is_ref_type(atype) && !is_generic_param(atype)
                && (__is_generic_value_variable(exp) || __is_generic_value_field(exp)))
        {
            if (__is_generic_value_variable(exp))
            {
                __compile_variable(ctx, pool, ((name_expression_t *)exp)->variable,
                                                        __environment_t::empty, exp);
            }
            else    // A field of an object: this.Value.
            {
                __compile_generic_field(ctx, pool, exp);
            }

            pool.append<x_generic_copy_xil_t>(__ref_of(ctx, exp->get_type()), xil_copy_kind_t::box);
            return;
        }

        // A struct passed to a reference (F(Object o) called with a struct): it is boxed, like the
        // assignment of a struct to a reference does.
        if (exp->get_type() != nullptr && is_custom_struct(exp->get_type())
                && is_ref_type(atype) && !__is_custom_struct_like(atype))
        {
            pool.append<x_new_xil_t>(__ref_of(ctx, exp->get_type()));
            pool.append<x_push_duplicate_xil_t>();

            exp->compile(ctx, pool);
            return;
        }

        // Custom struct.
        if (__is_custom_struct_like(atype))
        {
            ref_t type_ref = __ref_of(ctx, atype);

            // A value, or null, for a nullable argument: it is made in a local variable
            // that is copied, like the other structs.
            if (get_nullable_underlying(atype) != nullptr
                    && get_nullable_underlying(exp->get_type()) == nullptr)
            {
                local_variable_t * temp = ctx.define_temp_local(atype);

                __push_variable_address(ctx, pool, temp);
                __try_compile_nullable_value(ctx, pool, atype, exp);

                pool.append<x_stack_alloc_xil_t>(type_ref);
                __push_variable_address(ctx, pool, temp);
                pool.append<x_object_copy_xil_t>(type_ref, xil_copy_kind_t::reverse);
            }
            else
            {
                pool.append<x_stack_alloc_xil_t>(type_ref);
                exp->compile(ctx, pool);
            }
        }
        else
        {
            vtype_t vtype = vtype_t::__default__;
            if (param_types != nullptr)
                vtype = param_types->param_vtype_at(index);

            xil_type_t xil_type = to_xil_type(vtype);

            if (!__try_compile_constant_expression(ctx, pool, exp, xil_type))
                exp->compile(ctx, pool, atype);
        }
    }

    // Calls the getter of a property of a nullable type: the address of the value is on the
    // top of the stack.
    static void __call_nullable_getter(__cctx_t & ctx, xil_pool_t & pool, type_t * type,
                                                                        const char_t * name)
    {
        analyze_member_args_t args(member_type_t::property, __XPool.to_name(name));
        property_t * property = (property_t *)type->get_member(args, true);
        __FailedWhenNull(property, "property '%1%' of nullable type '%2%' not found", name, type);

        pool.append<x_method_call_xil_t>(
            xil_call_type_t::instance, __search_method_ref(ctx, property->get_method)
        );
    }

    // Holds the value of a nullable expression in a local variable, and pushes the address
    // of the variable.
    static void __push_nullable_address(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
    {
        local_variable_t * temp = ctx.define_temp_local(exp->get_type());

        compile_assign(ctx, pool, temp, exp);
        __push_variable_address(ctx, pool, temp);
    }

    // Pushes whether a nullable expression has a value.
    static void __compile_nullable_has_value(__cctx_t & ctx, xil_pool_t & pool,
                                                                expression_t * exp)
    {
        __push_nullable_address(ctx, pool, exp);
        __call_nullable_getter(ctx, pool, exp->get_type(), _T("HasValue"));
    }

    // Compiles x == null, x != null of a nullable x. Returns false if it is not.
    static bool __try_compile_nullable_null_compare(__cctx_t & ctx, xil_pool_t & pool,
                            expression_t * exp1, expression_t * exp2, bool equal)
    {
        expression_t * nullable_exp;
        if (get_nullable_underlying(exp1->get_type()) != nullptr && is_null(exp2->get_type()))
            nullable_exp = exp1;
        else if (get_nullable_underlying(exp2->get_type()) != nullptr && is_null(exp1->get_type()))
            nullable_exp = exp2;
        else
            return false;

        __compile_nullable_has_value(ctx, pool, nullable_exp);

        // The value is null when it has no value.
        if (equal)
            pool.append<x_logic_xil_t>(xil_logic_command_t::not_);

        return true;
    }

    // Compile arguments.
    static void __compile_arguments(__cctx_t & ctx, xil_pool_t & pool, arguments_t * arguments,
                                                        param_types_t * param_types)
    {
        int count = arguments != nullptr? arguments->size() : 0;

        // The params that have default values, and the params array of the method.
        size_t param_count = param_types != nullptr? param_types->param_count() : 0;
        param_t * last = param_count > 0? param_types->param_object_at(param_count - 1) : nullptr;

        bool variadic = last != nullptr && last->ptype == param_type_t::params;
        int fixed_count = variadic? (int)param_count - 1 : (int)param_count;

        if (param_types == nullptr || (!variadic && count >= fixed_count))
        {
            for (int index = 0; index < count; index++)
            {
                argument_t * argument = (*arguments)[index];
                __compile_argument(ctx, pool, argument->expression, param_types, index,
                                   argument->atype == param_type_t::extends);
            }

            return;
        }

        // The arguments that are written, for the params before the params array.
        for (int index = 0; index < count && index < fixed_count; index++)
            __compile_argument(ctx, pool, (*arguments)[index]->expression, param_types, index);

        // The default values of the others.
        for (int index = count; index < fixed_count; index++)
        {
            // The setter of an indexer takes the value apart from the arguments: the params
            // without default value are not the business of the arguments.
            param_t * param = param_types->param_object_at(index);
            if (param == nullptr || param->default_value == nullptr)
                return;

            type_t * type = param->get_type();
            if (__is_custom_struct_like(type))
                __Failed("the default value of a struct param is not supported");

            // The default value is a constant.
            cvalue_t cvalue = __execute_expression(param->default_value);
            if (is_nan(cvalue))
                __Failed("the default value of the param '%1%' is not a constant", param->name);

            __compile_cvalue(ctx, pool, cvalue, __environment_t(type));
        }

        if (!variadic)
            return;

        // The params array: the argument is the array, or the arguments are its elements.
        // The type of a generic method has its real types (params T[] items of Count<String>).
        type_t * array_type = (type_t *)param_types->param_type_at(param_count - 1);
        if (array_type == nullptr)
            array_type = last->get_type();

        if (array_type->this_gtype() != gtype_t::array)
            __Failed("the type of the params '%1%' is not an array", last->name);

        if (count == (int)param_count
                && is_type_compatible((*arguments)[count - 1]->expression->get_type(), array_type))
        {
            __compile_argument(ctx, pool, (*arguments)[count - 1]->expression, param_types,
                                                                                count - 1);
            return;
        }

        type_t * element_type = ((array_type_t *)array_type)->element_type;
        if (__is_custom_struct_like(element_type))
            __Failed("params of structs are not supported");

        xil_type_t dtype = __to_xil_type(element_type);
        int element_count = count > fixed_count? count - fixed_count : 0;

        pool.append<x_push_const_xil_t<int32_t>>(xil_type_t::int32, tvalue_t((int32_t)element_count));
        pool.append<x_new_array_xil_t>(__ref_of(ctx, array_type));

        if (element_count > 0)
        {
            pool.append<x_array_begin_xil_t>(dtype);

            for (int index = fixed_count; index < count; index++)
            {
                expression_t * exp = (*arguments)[index]->expression;
                if (!__try_compile_constant_expression(ctx, pool, exp, dtype))
                    exp->compile(ctx, pool, element_type);

                pool.append<x_array_element_xil_t>(dtype);
            }

            pool.append<x_array_end_xil_t>(dtype);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // Pointers: Ptr (void*), Ptr<T> (T*).

    // The reads and writes of the memory (p.Value, p[i]) and the operators of the pointers are made by instructions (no call):
    // the pointers are the members of the core types Ptr and Ptr<T> (core_lib/System/Ptr.cs), their bodies are never
    // executed.

    // Returns whether a type is Ptr or Ptr<T>.
    static bool __is_pointer_type(type_t * type)
    {
        if (type == nullptr)
            return false;

        if (type == __XPool.get_ptr_type())
            return true;

        return is_generic(type) && ((generic_type_t *)type)->template_ == __XPool.get_tptr_type();
    }

    // Returns the type that a pointer points to: T of Ptr<T>, null for Ptr.
    static type_t * __pointer_target(type_t * pointer_type)
    {
        if (is_generic(pointer_type) && !((generic_type_t *)pointer_type)->args.empty())
            return ((generic_type_t *)pointer_type)->args[0];

        return nullptr;
    }

    // Returns whether a pointer points to a custom struct.
    static bool __pointer_to_struct(type_t * pointer_type)
    {
        type_t * target = __pointer_target(pointer_type);
        return target != nullptr && !is_generic_param(target) && is_custom_struct(target);
    }

    // Returns the size of a value that a pointer points to (a pointer to nothing points to bytes): a constant, or -1 when the
    // size is made by an instruction (a struct, a generic param).
    static int64_t __pointer_target_size(__cctx_t & ctx, type_t * pointer_type, expression_t * exp)
    {
        type_t * target = __pointer_target(pointer_type);
        if (target == nullptr)
            return 1;

        if (is_generic_param(target) || is_custom_struct(target))
            return -1;

        vtype_t vtype = target->this_vtype();
        if (vtype == vtype_t::void_ || vtype == vtype_t::string_ || vtype == vtype_t::mobject_)
            __Failed("a pointer to the type '%1%' is not supported", target);

        return (int64_t)get_vtype_size(vtype);
    }

    // Pushes the size of a value that a pointer points to: an int64.
    static void __push_pointer_target_size(__cctx_t & ctx, xil_pool_t & pool, type_t * pointer_type, expression_t * exp)
    {
        int64_t size = __pointer_target_size(ctx, pointer_type, exp);
        if (size >= 0)
            pool.append<x_push_const_xil_t<int64_t>>(xil_type_t::int64, tvalue_t((int64_t)size));
        else
            pool.append<x_push_type_size_xil_t>(__ref_of(ctx, __pointer_target(pointer_type)));
    }

    // Returns the xil type of the value that a pointer points to.
    static xil_type_t __pointer_target_xil_type(type_t * pointer_type)
    {
        type_t * target = __pointer_target(pointer_type);
        if (target == nullptr)
            return xil_type_t::int8;

        return to_xil_type(target->this_vtype());
    }

    enum class __pointer_op_t
    {
        none, get_value, set_value, get_item, set_item,
        add, sub, diff, increment, decrement,
        equal, not_equal, less, less_equal, greater, greater_equal,
    };

    // Returns what a method of a pointer is made by the instructions, none if it is not one.
    static __pointer_op_t __pointer_op_of(method_base_t * method)
    {
        if (method == nullptr)
            return __pointer_op_t::none;

        method_t * m = dynamic_cast<method_t *>(method);
        if (m == nullptr || !__is_pointer_type(m->get_host_type()))
            return __pointer_op_t::none;

        string_t name = m->get_name();
        auto is = [&](const char_t * s) { return name == s; };

        if (is(_T("get_Value")))        return __pointer_op_t::get_value;
        if (is(_T("set_Value")))        return __pointer_op_t::set_value;
        if (is(_T("get_Item")))         return __pointer_op_t::get_item;
        if (is(_T("set_Item")))         return __pointer_op_t::set_item;

        if (is(_T("op_Add")))           return __pointer_op_t::add;
        if (is(_T("op_Equal")))         return __pointer_op_t::equal;
        if (is(_T("op_NotEqual")))      return __pointer_op_t::not_equal;
        if (is(_T("op_Less")))          return __pointer_op_t::less;
        if (is(_T("op_LessEqual")))     return __pointer_op_t::less_equal;
        if (is(_T("op_Greater")))       return __pointer_op_t::greater;
        if (is(_T("op_GreaterEqual")))  return __pointer_op_t::greater_equal;

        if (is(_T("op_Sub")))
        {
            return m->params != nullptr && m->params->size() == 2
                    && __is_pointer_type(m->params->at(1)->get_type())?
                __pointer_op_t::diff : __pointer_op_t::sub;
        }

        if (name.find(_T("Increment")) != string_t::npos)   return __pointer_op_t::increment;
        if (name.find(_T("Decrement")) != string_t::npos)   return __pointer_op_t::decrement;

        return __pointer_op_t::none;
    }

    // [address][count: int64] => [address + count * size] (a pointer).
    static void __compile_pointer_move(__cctx_t & ctx, xil_pool_t & pool, type_t * pointer_type, expression_t * exp,
                                                                                                bool add)
    {
        if (__pointer_target_size(ctx, pointer_type, exp) != 1)
        {
            __push_pointer_target_size(ctx, pool, pointer_type, exp);
            pool.append<x_al_xil_t>(xil_al_command_t::mul, xil_type_t::int64, xil_type_t::int64);
        }

        pool.append<x_al_xil_t>(add? xil_al_command_t::add : xil_al_command_t::sub,
                                xil_type_t::int64, xil_type_t::int64);
        pool.append<x_push_convert_xil_t>(xil_type_t::int64, xil_type_t::ptr);
    }

    // [address] => [address + size] (or - size): ++p, --p.
    static void __compile_pointer_step(__cctx_t & ctx, xil_pool_t & pool, type_t * pointer_type, expression_t * exp,
                                                                                                bool add)
    {
        __push_pointer_target_size(ctx, pool, pointer_type, exp);
        pool.append<x_al_xil_t>(add? xil_al_command_t::add : xil_al_command_t::sub,
                                xil_type_t::int64, xil_type_t::int64);
        pool.append<x_push_convert_xil_t>(xil_type_t::int64, xil_type_t::ptr);
    }

    // Returns whether the value of p.Value, p[i] is the object of a member (p.Value.X, p[i].X): the member is of the value that is
    // in the memory, not of a copy of it.
    static bool __is_pointer_value_of_member(expression_t * owner_exp)
    {
        if (owner_exp == nullptr)
            return false;

        // p.Value: the name is the right operand of the member expression, that is the object of the next member.
        expression_t * value_exp = owner_exp;
        if (value_exp->parent != nullptr && __is_member_expression(value_exp->parent)
                && ((binary_expression_t *)value_exp->parent)->exp2() == value_exp)
        {
            value_exp = value_exp->parent;
        }

        return value_exp->parent != nullptr && __is_member_expression(value_exp->parent)
                && ((binary_expression_t *)value_exp->parent)->exp1() == value_exp;
    }

    // The instructions of a method of a pointer that is made by the compiler. The operands are in the stack, in the order
    // of the arguments (the pointer, the other operand, the value).
    // Returns false when the method is not one of them.
    static bool __try_compile_pointer_method(__cctx_t & ctx, xil_pool_t & pool, method_base_t * method,
                                                                            expression_t * exp)
    {
        __pointer_op_t op = __pointer_op_of(method);
        if (op == __pointer_op_t::none)
            return false;

        type_t * pointer_type = ((method_t *)method)->get_host_type();
        xil_type_t xt = __pointer_target_xil_type(pointer_type);

        // A struct is copied between the memory and its destination (that is below the address); the value of a generic
        // param is not made yet.
        bool is_struct = __pointer_to_struct(pointer_type);

        if (__pointer_target(pointer_type) != nullptr && is_generic_param(__pointer_target(pointer_type))
                && (op == __pointer_op_t::get_value || op == __pointer_op_t::get_item
                    || op == __pointer_op_t::set_value || op == __pointer_op_t::set_item))
        {
            __Failed("the value that a pointer to the generic param '%1%' points to is not supported yet",
                     __pointer_target(pointer_type));
        }

        switch (op)
        {
            case __pointer_op_t::get_value:
            case __pointer_op_t::get_item:
                if (op == __pointer_op_t::get_item)
                    __compile_pointer_move(ctx, pool, pointer_type, exp, true);

                if (is_struct)
                {
                    // The address is the object of the member (p.Value.X): no copy.
                    if (!__is_pointer_value_of_member(exp))
                    {
                        pool.append<x_object_copy_xil_t>(__ref_of(ctx, __pointer_target(pointer_type)),
                                                                                    xil_copy_kind_t::reverse);
                    }
                }
                else
                {
                    __pointer_target_size(ctx, pointer_type, exp);      // Is the type supported.
                    pool.append<x_push_ptr_content_xil_t>(xt);
                }
                break;

            case __pointer_op_t::set_value:
            case __pointer_op_t::set_item:
                __pointer_target_size(ctx, pointer_type, exp);
                pool.append<x_pop_ptr_content_xil_t>(xt);
                break;

            case __pointer_op_t::add:
                __compile_pointer_move(ctx, pool, pointer_type, exp, true);
                break;

            case __pointer_op_t::sub:
                __compile_pointer_move(ctx, pool, pointer_type, exp, false);
                break;

            case __pointer_op_t::diff: {
                pool.append<x_al_xil_t>(xil_al_command_t::sub, xil_type_t::int64, xil_type_t::int64);

                if (__pointer_target_size(ctx, pointer_type, exp) != 1)
                {
                    __push_pointer_target_size(ctx, pool, pointer_type, exp);
                    pool.append<x_al_xil_t>(xil_al_command_t::div, xil_type_t::int64, xil_type_t::int64);
                }
            }   break;

            case __pointer_op_t::increment:
                __compile_pointer_step(ctx, pool, pointer_type, exp, true);
                break;

            case __pointer_op_t::decrement:
                __compile_pointer_step(ctx, pool, pointer_type, exp, false);
                break;

            case __pointer_op_t::equal:         case __pointer_op_t::not_equal:
            case __pointer_op_t::less:          case __pointer_op_t::less_equal:
            case __pointer_op_t::greater:       case __pointer_op_t::greater_equal: {
                xil_cmp_command_t cmd =
                    op == __pointer_op_t::equal?         xil_cmp_command_t::equal
                  : op == __pointer_op_t::not_equal?     xil_cmp_command_t::not_equal
                  : op == __pointer_op_t::less?          xil_cmp_command_t::less
                  : op == __pointer_op_t::less_equal?    xil_cmp_command_t::less_equal
                  : op == __pointer_op_t::greater?       xil_cmp_command_t::greater
                                                         : xil_cmp_command_t::greater_equal;

                // The addresses are compared as unsigned numbers.
                pool.append<x_cmp_xil_t>(cmd, xil_type_t::uint64, xil_type_t::uint64);
            }   break;

            default:
                return false;
        }

        return true;
    }

    // Ptr.AddressOf(array), Ptr.AddressOf(array, index), Ptr.AddressOf(ref x): the address is made by the instructions of the
    // arrays and of the variables (the address of a ref argument), no call. The array is kept in a variable of the method (a
    // variable that holds an object is a root of the collection): the pointer does not outlive the method, and the array does
    // not go while the method runs.
    // Returns false when the call is not one of them.
    static bool __try_compile_address_of(__cctx_t & ctx, xil_pool_t & pool, function_expression_t * exp,
                                                                            method_base_t * method)
    {
        if (method == nullptr || method->get_host_type() != __XPool.get_ptr_type())
            return false;

        string_t name = method->get_name();
        if (name != _T("AddressOf"))
            return false;

        arguments_t * args = exp->arguments();
        if (args == nullptr || args->size() < 1 || args->size() > 2)
            return false;

        param_types_t * param_types = method;
        argument_t * first = (*args)[0];

        // The address of a variable, of a field, of an element: the address of the ref argument is the pointer.
        if (first->atype == param_type_t::ref)
        {
            __compile_argument(ctx, pool, first->expression, param_types, 0);
            return true;
        }

        type_t * array_type = first->expression->get_type();
        if (!is_array(array_type))
            return false;                       // A string: the method of the library.

        if (((array_type_t *)array_type)->dimension != 1)
            __Failed("the address of a value of the array of more than one dimension '%1%' is not supported", array_type);

        // The index is below the array in the stack.
        if (args->size() == 2)
        {
            (*args)[1]->expression->compile(ctx, pool, __XPool.get_internal_type(vtype_t::int32_));
        }
        else
        {
            pool.append<x_push_const_xil_t<int32_t>>(xil_type_t::int32, tvalue_t((int32_t)0));
        }

        first->expression->compile(ctx, pool);

        // The array is kept in a variable of the method.
        local_variable_t * pin = ctx.define_temp_local(array_type);
        pool.append<x_push_duplicate_xil_t>();
        pool.append<x_pop_local_xil_t>(xil_type_t::object, pin->identity);

        pool.append<x_push_array_element_addr_xil_t>(__ref_of(ctx, array_type));
        return true;
    }

    // Pushes the address of p.Value, p[i] of a pointer p.
    static bool __try_push_pointer_member_address(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp,
                                                  expression_t * this_exp, variable_t * variable)
    {
        method_base_t * getter = nullptr;

        if (variable->this_type() == variable_type_t::property)
            getter = ((property_variable_t *)variable)->property->get_method;
        else
            getter = ((property_index_variable_t *)variable)->property->get_method;

        __pointer_op_t op = __pointer_op_of(getter);
        type_t * pointer_type = op == __pointer_op_t::none? nullptr : ((method_t *)getter)->get_host_type();

        // &p.Value: the address is the pointer.
        if (op == __pointer_op_t::get_value && this_exp != nullptr)
        {
            this_exp->compile(ctx, pool);
            return true;
        }

        // &p[i]: the pointer moved by i.
        if (op == __pointer_op_t::get_item && exp->this_family() == expression_family_t::index)
        {
            ((index_expression_t *)exp)->namex()->compile(ctx, pool);
            __compile_arguments(ctx, pool, ((property_index_variable_t *)variable)->arguments, getter);
            __compile_pointer_move(ctx, pool, pointer_type, exp, true);
            return true;
        }

        return false;
    }

    // NativeMemory.SizeOf<T>(): the size of the type, an instruction (the size of the instance when T is a generic param).
    static bool __try_compile_size_of(__cctx_t & ctx, xil_pool_t & pool, function_expression_t * exp, method_base_t * method)
    {
        generic_method_t * generic_method = dynamic_cast<generic_method_t *>(method);
        if (generic_method == nullptr || generic_method->args.size() != 1)
            return false;

        string_t name = method->get_name();
        if (name != _T("SizeOf"))
            return false;

        type_t * host = method->get_host_type();
        if (host == nullptr || host->to_identity().find(_T("NativeMemory")) == string_t::npos)
            return false;

        pool.append<x_push_type_size_xil_t>(__ref_of(ctx, generic_method->args[0]));
        return true;
    }

    // NativeMemory.StackAlloc<T>(count): memory for count values of T that lives while the method runs, as long as the
    // variable that holds it: a buffer of bytes, kept by a variable of the method (it is not cleared by the program, it is
    // not guaranteed to be). One instruction of the allocation of an array, so far, in the three back ends.
    static bool __try_compile_stack_alloc(__cctx_t & ctx, xil_pool_t & pool, function_expression_t * exp,
                                                                                    method_base_t * method)
    {
        generic_method_t * generic_method = dynamic_cast<generic_method_t *>(method);
        if (generic_method == nullptr || generic_method->args.size() != 1)
            return false;

        string_t name = method->get_name();
        if (name != _T("StackAlloc"))
            return false;

        type_t * host = method->get_host_type();
        if (host == nullptr || host->to_identity().find(_T("NativeMemory")) == string_t::npos)
            return false;

        arguments_t * args = exp->arguments();
        if (args == nullptr || args->size() != 1)
            return false;

        // The bytes: count * size + 1 (an array of no byte has no address).
        (*args)[0]->expression->compile(ctx, pool, __XPool.get_internal_type(vtype_t::int64_));
        pool.append<x_push_type_size_xil_t>(__ref_of(ctx, generic_method->args[0]));
        pool.append<x_al_xil_t>(xil_al_command_t::mul, xil_type_t::int64, xil_type_t::int64);
        pool.append<x_push_const_xil_t<int64_t>>(xil_type_t::int64, tvalue_t((int64_t)1));
        pool.append<x_al_xil_t>(xil_al_command_t::add, xil_type_t::int64, xil_type_t::int64);
        pool.append<x_push_convert_xil_t>(xil_type_t::int64, xil_type_t::int32);

        type_t * bytes_type = __XPool.new_array_type(__XPool.get_internal_type(vtype_t::uint8_), 1);
        pool.append<x_new_array_xil_t>(__ref_of(ctx, bytes_type));

        // The buffer is kept by a variable of the method; the address of its first byte is the result.
        local_variable_t * buffer = ctx.define_temp_local(bytes_type);
        pool.append<x_pop_local_xil_t>(xil_type_t::object, buffer->identity);

        pool.append<x_push_const_xil_t<int32_t>>(xil_type_t::int32, tvalue_t((int32_t)0));
        pool.append<x_push_local_xil_t>(xil_type_t::object, buffer->identity);
        pool.append<x_push_array_element_addr_xil_t>(__ref_of(ctx, bytes_type));
        return true;
    }

    // The local variables that hold a pointer to the memory of the method (see is_scoped_pointer).
    static std::set<local_variable_t *> __scoped_pointer_locals;

    // Returns whether the expression is a pointer to the memory of the method.
    bool is_scoped_pointer(expression_t * exp)
    {
        if (exp == nullptr)
            return false;

        switch (exp->this_family())
        {
            case expression_family_t::expressions:
                return exp->expression_count() == 1 && is_scoped_pointer(exp->expression_at(0));

            case expression_family_t::name: {
                variable_t * variable = ((name_expression_t *)exp)->variable;
                return variable != nullptr && variable->this_type() == variable_type_t::local
                    && __scoped_pointer_locals.count((local_variable_t *)variable) > 0;
            }

            case expression_family_t::type_cast:
                return is_scoped_pointer(exp->expression_at(0));

            case expression_family_t::binary: {
                binary_expression_t * binary = (binary_expression_t *)exp;

                // p + n, p - n: the method of the operator.
                if (binary->op() != operator_t::member_point)
                {
                    method_t * overload = binary->overload_method;
                    if (overload != nullptr && __is_pointer_type(overload->get_type())
                            && (__pointer_op_of(overload) == __pointer_op_t::add
                                || __pointer_op_of(overload) == __pointer_op_t::sub))
                    {
                        return is_scoped_pointer(binary->exp1());
                    }

                    return false;
                }

                function_expression_t * call = as<function_expression_t *>(binary->exp2());
                if (call == nullptr || call->get_ftype() != function_expression_type_t::method || call->get_method() == nullptr)
                    return false;

                method_base_t * method = call->get_method();
                string_t name = method->get_name();
                type_t * host = method->get_host_type();
                if (host == nullptr)
                    return false;

                // p.As<T>(), p.ToPtr(): the same address.
                if ((name == _T("As") || name == _T("ToPtr")) && __is_pointer_type(host))
                    return is_scoped_pointer(binary->exp1());

                // Ptr.AddressOf of an array, of a ref argument; NativeMemory.StackAlloc.
                if (name == _T("StackAlloc") && host->to_identity().find(_T("NativeMemory")) != string_t::npos)
                    return true;

                if (name == _T("AddressOf") && host == __XPool.get_ptr_type())
                {
                    arguments_t * args = call->arguments();
                    if (args == nullptr || args->size() < 1)
                        return false;

                    argument_t * first = (*args)[0];
                    return first->atype == param_type_t::ref || is_array(first->expression->get_type());
                }

                return false;
            }

            default:
                return false;
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
        bool custom_struct = __is_custom_struct_like(var->get_type());

        switch (var->this_type())
        {
            case variable_type_t::property:
                __push_this(ctx, pool, this_exp);

                // A property of a generic param: the object is a reference, or the address of
                // a struct.
                if (this_exp != nullptr
                        && __is_constrained_property(((property_variable_t *)var)->property))
                {
                    pool.append<x_generic_copy_xil_t>(
                        __ref_of(ctx, this_exp->get_type()), xil_copy_kind_t::receiver
                    );
                }

                break;

            case variable_type_t::property_index: {
                _A(exp->this_family() == expression_family_t::index);
                ((index_expression_t *)exp)->namex()->compile(ctx, pool);

                property_t * property = _M(property_index_variable_t *, var)->property;
                if (property == nullptr)
                    __Failed("unknown property '%1%'", var);

                __compile_arguments(ctx, pool,
                    ((property_index_variable_t *)var)->arguments, property->set_method
                );

                // p[i] = value: the address is made before the value, that is above it.
                if (__pointer_op_of(property->set_method) == __pointer_op_t::set_item)
                {
                    __compile_pointer_move(ctx, pool, property->set_method->get_host_type(), exp, true);
                }
            }   break;

            default:
                break;
        }

        return __compile_assign_t { this_exp, var, exp, custom_struct };
    }

    // The value that a setter is called with is the value of the assignment, when it is used:
    // it is kept in a temp variable (the value is on the top of the stack, and stays there for
    // the call).
    static local_variable_t * __keep_assigned_value(__cctx_t & ctx, xil_pool_t & pool, bool pick,
                                                                            type_t * type)
    {
        if (!pick || type == nullptr || __is_custom_struct_like(type))
            return nullptr;

        local_variable_t * temp = ctx.define_temp_local(type);

        pool.append<x_push_duplicate_xil_t>();
        pool.append<x_pop_local_xil_t>(to_xil_type(type), temp->identity);

        return temp;
    }

    // Pushes the value that was kept.
    static void __push_assigned_value(xil_pool_t & pool, local_variable_t * kept)
    {
        if (kept != nullptr)
            pool.append<x_push_local_xil_t>(to_xil_type(kept->get_type()), kept->identity);
    }

    // Compiles assign to expression.
    static void __compile_assign_to(__cctx_t & ctx, xil_pool_t & pool, __compile_assign_t & ca,
        expression_t * exp2, __assign_to_type_t assign_type = __assign_to_type_t::default_)
    {
        expression_t * exp = ca.exp;

        bool pick;

        if (assign_type == __assign_to_type_t::default_)
        {
            _A(exp != nullptr && exp->parent != nullptr);
            pick = is_effective(ctx, exp->parent->parent);
        }
        else
        {
            pick = (assign_type == __assign_to_type_t::pick);
        }

        xil_type_t xt = xil_type_t::empty;

        // An event that has a field (in the type that has it): the handlers are assigned.
        variable_t * var = ca.var;
        if (var->this_type() == variable_type_t::event)
        {
            event_t * event = ((event_variable_t *)var)->event;
            if (event->field == nullptr)
                __Failed("the event '%1%' is only added and removed", event->name);

            var = ca.var = event->field->variable;
        }

        switch (var->this_type())
        {
            case variable_type_t::local:
                xil::write_assign_xil(ctx, pool, (local_variable_t *)var, xt, pick);
                break;

            case variable_type_t::param:
                xil::write_assign_xil(ctx, pool, (param_variable_t *)var, xt, pick);
                break;

            case variable_type_t::field:
                __push_field_owner(ctx, pool, ca.this_, var);
                xil::write_assign_xil(ctx, pool, (field_variable_t *)var, xt, pick);
                break;

            case variable_type_t::property: {
                property_t * property = ((property_variable_t *)var)->property;
                if (property == nullptr)
                    __Failed("unknown property '%1%'", var);

                method_t * method = property->set_method;
                if (method == nullptr)
                    __Failed("property '%1%' cannot be write", var);

                __check_init_only(ctx, property);
                __validate_set_method(ctx, method);

                // p.Value = s of a struct: s is written at the address that is in the stack.
                if (exp2 != nullptr && __pointer_op_of(method) == __pointer_op_t::set_value
                        && __pointer_to_struct(method->get_host_type()))
                {
                    exp2->compile(ctx, pool, __pointer_target(method->get_host_type()));
                    break;
                }

                // The value of x += y, x++ is in the stack.
                if (exp2 != nullptr)
                    __compile_argument(ctx, pool, exp2, method, 0);

                // The value of the assignment is the value that is assigned.
                local_variable_t * kept = __keep_assigned_value(ctx, pool, pick, var->get_type());

                if (!__try_compile_pointer_method(ctx, pool, method, exp))
                {
                    ref_t method_ref = __search_method_ref(ctx, method);
                    pool.append<x_method_call_xil_t>(__call_type_of_method(method), method_ref);
                }

                __push_assigned_value(pool, kept);

            }   break;

            case variable_type_t::array_index: {
                array_index_variable_t * arr_var = (array_index_variable_t *)var;
                expression_t * body = arr_var->body;
                arguments_t * args = arr_var->arguments;

                __FailedWhenNull(body, "index '%1%' body missing", var);
                __FailedWhenNull(args, "index '%1%' arguments missing", var);

                // The value of the assignment is the value that is assigned: a copy of it stays
                // below the array and its indices.
                if (pick && !ca.custom_struct)
                    pool.append<x_push_duplicate_xil_t>();

                __static_param_types_t param_types(arr_var);
                __compile_arguments(ctx, pool, args);
                body->compile(ctx, pool);

                type_t * element_type = arr_var->get_type();
                type_t * array_type = arr_var->get_array_type();

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

                // p[i] = s of a struct: s is written at the address that is in the stack.
                if (exp2 != nullptr && __pointer_op_of(method) == __pointer_op_t::set_item
                        && __pointer_to_struct(method->get_host_type()))
                {
                    exp2->compile(ctx, pool, __pointer_target(method->get_host_type()));
                    break;
                }

                // The value is the argument that follows the indices.
                int index_count = index_var->arguments != nullptr? index_var->arguments->size() : 0;
                if (exp2 != nullptr)
                    __compile_argument(ctx, pool, exp2, method, index_count);

                local_variable_t * kept = __keep_assigned_value(ctx, pool, pick, var->get_type());

                // A virtual setter, or the setter of an interface, is called by its object.
                if (!__try_compile_pointer_method(ctx, pool, method, exp))
                {
                    ref_t method_ref = __search_method_ref(ctx, method);
                    pool.append<x_method_call_xil_t>(__call_type_of_method(method), method_ref);
                }

                __push_assigned_value(pool, kept);

            }   break;

            default:
                X_UNEXPECTED_F("unexpected variable type '%1%'", var->this_type());
        }
    }

    // Compiles assign expression.
    static void __compile_assign(__cctx_t & ctx, xil_pool_t & pool, __compile_assign_t ca,
        expression_t * exp2, __assign_to_type_t assign_type = __assign_to_type_t::default_)
    {
        type_t * type1 = ca.var->get_type();
        type_t * type2 = exp2->get_type();

        // A pointer to the memory of the method: a local variable can hold it, the others are the places that live longer.
        if (is_scoped_pointer(exp2))
        {
            switch (ca.var->this_type())
            {
                case variable_type_t::local:
                    __scoped_pointer_locals.insert((local_variable_t *)ca.var);
                    break;

                case variable_type_t::field:
                case variable_type_t::property:
                case variable_type_t::property_index:
                case variable_type_t::array_index:
                    __Failed("the pointer '%1%' is to the memory of the method (AddressOf, StackAlloc): it is not stored in '%2%', "
                             "it lives while the method runs", exp2, ca.var);

                default:
                    break;
            }
        }

        // A value, or null, assigned to a nullable variable.
        if (is_real_variable(ca.var) && get_nullable_underlying(type1) != nullptr
                && get_nullable_underlying(type2) == nullptr)
        {
            __push_variable_address(ctx, pool, ca.var, ca.this_);
            __try_compile_nullable_value(ctx, pool, type1, exp2);
            return;
        }

        if (is_real_variable(ca.var))
        {
            // A generic param assigned to a reference (Object o = x): the object it is, made by
            // the copy of the expression.
            if (is_generic_param(type2) && is_ref_type(type1) && !is_generic_param(type1)
                    && (__is_generic_value_variable(exp2) || __is_generic_value_field(exp2)))
            {
                if (__is_generic_value_variable(exp2))
                {
                    __compile_variable(ctx, pool, ((name_expression_t *)exp2)->variable,
                                                        __environment_t::empty, exp2);
                }
                else    // A field of an object: this.Value.
                {
                    __compile_generic_field(ctx, pool, exp2);
                }

                pool.append<x_generic_copy_xil_t>(__ref_of(ctx, type2), xil_copy_kind_t::box);

                __compile_assign_to(ctx, pool, ca, exp2, assign_type);
            }
            else if (__is_custom_struct_like(type2))
            {
                // Box: a struct assigned to a reference. (A generic param is not boxed.)
                if (is_ref_type(type1) && !is_generic_param(type1))
                {
                    pool.append<x_new_xil_t>(__ref_of(ctx, type2));
                    pool.append<x_push_duplicate_xil_t>();

                    exp2->compile(ctx, pool);
                    __compile_assign_to(ctx, pool, ca, exp2, assign_type);
                }
                else
                {
                    if (type1 != type2)
                    {
                        __Failed("Custom struct assign failed, cannot assign '%1%' to '%2%'",
                                                                            type2, type1);
                    }

                    __push_variable_address(ctx, pool, ca.var, ca.this_);
                    exp2->compile(ctx, pool, type1);

                    // A call returns a generic param: a struct is written at the destination
                    // below, else the value is stored there.
                    if (is_generic_param(type2) && __is_generic_call_result(exp2))
                    {
                        pool.append<x_generic_copy_xil_t>(
                            __ref_of(ctx, type2), xil_copy_kind_t::store
                        );
                    }
                }
            }
            else
            {
                exp2->compile(ctx, pool, type1);
                __compile_assign_to(ctx, pool, ca, exp2, assign_type);
            }
        }
        else
        {
            __compile_assign_to(ctx, pool, ca, exp2, assign_type);
        }
    }

    // Compiles assign expression.
    static void __compile_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_assign(ctx, pool, ca, exp2);
    }

    // Compiles assign expression.
    void compile_assign(__cctx_t & ctx, xil_pool_t & pool, local_variable_t * var,
                                                           expression_t * exp)
    {
        __compile_assign_t ca = { nullptr, var, nullptr, __is_custom_struct_like(var->get_type()) };
        __compile_assign(ctx, pool, ca, exp, __assign_to_type_t::pop);
    }

    // Returns the event that the left expression of an assignment is (E, obj.E), or null.
    static event_variable_t * __event_of_assign(expression_t * exp1)
    {
        expression_t * r_exp = __is_member_expression(exp1)?
                                    ((binary_expression_t *)exp1)->exp2() : exp1;

        variable_expression_t * var_exp = as<variable_expression_t *>(r_exp);
        if (var_exp == nullptr || !var_exp->is_variable_expression())
            return nullptr;

        variable_t * variable = var_exp->get_variable();
        if (variable == nullptr || variable->this_type() != variable_type_t::event)
            return nullptr;

        return (event_variable_t *)variable;
    }

    // Compiles the call of an accessor of an event: the object, then the handler.
    static void __compile_event_accessor(__cctx_t & ctx, xil_pool_t & pool,
                    expression_t * exp1, expression_t * exp2, method_t * method)
    {
        if (method == nullptr)
            __Failed("the event '%1%' has no accessor for it", exp1);

        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);

        // The object of the event.
        if (!method->is_static())
            __push_this(ctx, pool, ca.this_);

        __compile_argument(ctx, pool, exp2, method, 0);

        ref_t method_ref = __search_method_ref(ctx, method);
        pool.append<x_method_call_xil_t>(__call_type_of_method(method), method_ref);
    }

    // Compiles add assign expression.
    static void __compile_add_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        event_variable_t * event_var = __event_of_assign(exp1);
        if (event_var != nullptr)
        {
            __compile_event_accessor(ctx, pool, exp1, exp2, event_var->event->add_method);
            return;
        }

        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_add(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca, nullptr);
    }

    // Compiles sub assign expression.
    static void __compile_sub_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        event_variable_t * event_var = __event_of_assign(exp1);
        if (event_var != nullptr)
        {
            __compile_event_accessor(ctx, pool, exp1, exp2, event_var->event->remove_method);
            return;
        }

        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_sub(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca, nullptr);
    }

    // Compiles mul assign expression.
    static void __compile_mul_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_mul(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca, nullptr);
    }

    // Compiles div assign expression.
    static void __compile_div_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_div(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca, nullptr);
    }

    // Compiles mod assign expression.
    static void __compile_mod_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_mod(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca, nullptr);
    }

    // Compiles bit and assign expression.
    static void __compile_bit_and_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_bit_and(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca, nullptr);
    }

    // Compiles bit or assign expression.
    static void __compile_bit_or_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_bit_or(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca, nullptr);
    }

    // Compiles bit xor assign expression.
    static void __compile_bit_xor_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_bit_xor(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca, nullptr);
    }

    // Compiles left shift assign expression.
    static void __compile_left_shift_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_left_shift(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca, nullptr);
    }

    // Compiles right shift assign expression.
    static void __compile_right_shift_assign(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
        __compile_right_shift(ctx, pool, exp1, exp2);
        __compile_assign_to(ctx, pool, ca, nullptr);
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
        // A string and an array can be null, as the classes.
        if (!can_be_null(exp->get_type()))
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
        // x ?? y of a nullable x: x.HasValue? x.Value : y
        if (type_t * underlying = get_nullable_underlying(exp1->get_type()); underlying != nullptr)
        {
            if (get_nullable_underlying(exp2->get_type()) != nullptr)
                __Failed("null coalescing of two nullable types is not supported");

            statement_compile_context_t & sctx = ctx.statement_ctx;

            // The value is held in a local variable, read by the methods of the type.
            local_variable_t * temp = ctx.define_temp_local(exp1->get_type());
            compile_assign(ctx, pool, temp, exp1);

            __push_variable_address(ctx, pool, temp);
            __call_nullable_getter(ctx, pool, exp1->get_type(), _T("HasValue"));

            jmp_xil_t * else_jmp_xil = xil::append_jmp_xil(pool, xil_jmp_model_t::false_);

            __push_variable_address(ctx, pool, temp);
            __call_nullable_getter(ctx, pool, exp1->get_type(), _T("Value"));

            jmp_xil_t * finally_jmp_xil = xil::append_jmp_xil(pool, xil_jmp_model_t::none);

            // No value: y is the result.
            xil::append_label_xil(sctx, pool, else_jmp_xil);
            exp2->compile(ctx, pool, underlying);

            xil::append_label_xil(sctx, pool, finally_jmp_xil);
            return;
        }

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
        __compile_assign_to(ctx, pool, ca, nullptr);

        xil::append_label_xil(sctx, pool, finally_jmp_xil);
    }

    // Compiles as expression.
    static void __compile_as(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        type_t * type = __to_actual_type(exp2);
        ref_t type_ref = __ref_of(ctx, type);
        _A(type_ref != ref_t::null);

        // o as String: a String in an object is boxed, the String is its value (null when the object is not a String).
        type_t * type1 = exp1->get_type();
        if (type->this_vtype() == vtype_t::string_ && type1 != nullptr && type1->this_vtype() != vtype_t::string_
                && is_ref_type(type1))
        {
            statement_compile_context_t & sctx = ctx.statement_ctx;

            local_variable_t * temp = ctx.define_temp_local(type1);
            xil_type_t xtype1 = to_xil_type(type1);

            exp1->compile(ctx, pool);
            pool.append<x_pop_local_xil_t>(xtype1, temp->identity);

            pool.append<x_push_local_xil_t>(xtype1, temp->identity);
            pool.append<x_cast_xil_t>(xil_cast_command_t::is, type_ref);
            jmp_xil_t * null_jmp_xil = xil::append_jmp_xil(pool, xil_jmp_model_t::false_);

            pool.append<x_push_local_xil_t>(xtype1, temp->identity);
            pool.append<x_push_unbox_xil_t>(type_ref);
            jmp_xil_t * end_jmp_xil = xil::append_jmp_xil(pool, xil_jmp_model_t::none);

            xil::append_label_xil(sctx, pool, null_jmp_xil);
            pool.append<x_push_null_xil_t>();

            xil::append_label_xil(sctx, pool, end_jmp_xil);
            return;
        }

        exp1->compile(ctx, pool);

        pool.append<x_cast_xil_t>(xil_cast_command_t::as, type_ref);
    }

    // Compiles is expression that declares a variable: o is string s. The value of o is kept in
    // a temp variable, if it is of the type the variable is assigned (s = (string)o).
    static void __compile_is_declaration(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, name_expression_t * declaration)
    {
        statement_compile_context_t & sctx = ctx.statement_ctx;

        variable_t * variable = declaration->variable;
        __FailedWhenNull(variable, "the variable of '%1%' is missing", declaration);

        type_t * type = variable->get_type();
        type_t * type1 = exp1->get_type();
        __FailedWhenNull(type, "the type of '%1%' is missing", declaration);
        __FailedWhenNull(type1, "the type of '%1%' is missing", exp1);

        ref_t type_ref = __ref_of(ctx, type);

        // The value is kept.
        local_variable_t * temp = ctx.define_temp_local(type1);
        xil_type_t xtype1 = to_xil_type(type1);

        if (get_nullable_underlying(type1) != nullptr)
        {
            // n is int v: n has a value (a nullable is a struct, it is not an object that is of a type).
            compile_assign(ctx, pool, temp, exp1);

            __push_variable_address(ctx, pool, temp);
            __call_nullable_getter(ctx, pool, type1, _T("HasValue"));
        }
        else
        {
            exp1->compile(ctx, pool);
            pool.append<x_pop_local_xil_t>(xtype1, temp->identity);

            pool.append<x_push_local_xil_t>(xtype1, temp->identity);
            pool.append<x_cast_xil_t>(xil_cast_command_t::is, type_ref);
        }

        // false: it is the value of the expression.
        pool.append<x_push_duplicate_xil_t>();
        jmp_xil_t * end_jmp_xil = xil::append_jmp_xil(pool, xil_jmp_model_t::false_);
        pool.append<x_pop_empty_xil_t>(xil_type_t::bool_);

        // true: s = (string)o
        auto * temp_exp = __XPool.new_obj<system_expression_t<name_expression_t>>(temp);
        auto * cast_exp = __XPool.new_obj<system_expression_t<type_cast_expression_t>>(
            declaration->declare_type_name, (expression_t *)temp_exp
        );

        auto * variable_exp = __XPool.new_obj<system_expression_t<name_expression_t>>(
            (local_variable_t *)variable
        );

        auto * assign_exp = __XPool.new_obj<system_expression_t<binary_expression_t>>(
            operator_t::assign, variable_exp, cast_exp
        );

        assign_exp->compile(ctx, pool, __environment_t::empty);

        pool.append<x_push_const_xil_t<bool>>(xil_type_t::bool_, tvalue_t(true));

        xil::append_label_xil(sctx, pool, end_jmp_xil);
    }

    // Compiles is expression.
    static void __compile_is(__cctx_t & ctx, xil_pool_t & pool,
                                expression_t * exp1, expression_t * exp2)
    {
        // o is string s
        if (exp2->this_family() == expression_family_t::name
                && ((name_expression_t *)exp2)->declare_type_name != nullptr)
        {
            __compile_is_declaration(ctx, pool, exp1, (name_expression_t *)exp2);
            return;
        }

        // n is int: n has a value.
        if (type_t * type1 = exp1->get_type(); type1 != nullptr && get_nullable_underlying(type1) != nullptr
                && to_actual_type(exp2) != nullptr)
        {
            local_variable_t * temp = ctx.define_temp_local(type1);
            compile_assign(ctx, pool, temp, exp1);

            __push_variable_address(ctx, pool, temp);
            __call_nullable_getter(ctx, pool, type1, _T("HasValue"));
            return;
        }

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

    // Returns whether an expression is a property or an indexer (o.P, o[i]): it is read by a getter, written by a setter.
    static bool __is_accessor_expression(expression_t * exp)
    {
        expression_t * r_exp = __is_member_expression(exp)? ((binary_expression_t *)exp)->exp2() : exp;

        variable_t * var = __pick_var(r_exp);
        return var != nullptr && (var->this_type() == variable_type_t::property
                               || var->this_type() == variable_type_t::property_index);
    }

    // Adds 1 to (or subtracts 1 from) the value that is at the top of the stack. A Char is a number of Int32 for it.
    static void __compile_inc_step(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp, bool increment)
    {
        xil_type_t xtype = __xil_type(exp);
        xil_type_t wide = xtype == xil_type_t::char_? xil_type_t::int32 : xtype;

        __try_append_convert_xil(pool, xtype, wide);
        __compile_number(ctx, pool, 1);

        pool.append<x_al_xil_t>(
            __al_command(ctx, increment? xil_al_command_t::add : xil_al_command_t::sub, wide, xil_type_t::int32),
            wide, xil_type_t::int32
        );

        __try_append_convert_xil(pool, wide, xtype);
    }

    // Compiles increment expression.
    static void __compile_inc(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp,
                                                                __inc_t flag)
    {
        #define __Is(v) bit_has_flag(flag, __inc_t::v)

        // x = o.P++: the setter takes the new value, the old one is the value of the expression: it is kept before.
        local_variable_t * old_value = nullptr;
        if (__Is(right) && is_effective(ctx, exp->parent->parent) && __is_accessor_expression(exp))
        {
            old_value = ctx.define_temp_local(exp->get_type());

            exp->compile(ctx, pool);
            pool.append<x_pop_local_xil_t>(__to_xil_type(exp->get_type()), old_value->identity);
        }

        __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp);
        variable_t * var = ca.var;

        if (__Is(right))
        {
            if (old_value == nullptr && is_effective(ctx, exp->parent->parent))
                exp->compile(ctx, pool);

            exp->compile(ctx, pool);
            __compile_inc_step(ctx, pool, exp, __Is(increment));

            __compile_assign_to(ctx, pool, ca, nullptr, __assign_to_type_t::pop);

            if (old_value != nullptr)
                pool.append<x_push_local_xil_t>(__to_xil_type(exp->get_type()), old_value->identity);
        }
        else
        {
            exp->compile(ctx, pool);
            __compile_inc_step(ctx, pool, exp, __Is(increment));

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
        xil_type_t xil_type = __to_xil_type(ctx, variable);

        // The value is pushed with the type of the variable: the conversion to the type that
        // the expression needs is made once, by __post_compile_expression.
        dtype = xil_type;

        if (variable->constant)
        {
            cvalue_t cvalue = __execute_expression(variable->expression);
            _A(cvalue != cvalue_t::nan);

            __compile_cvalue(ctx, pool, cvalue, dtype);
        }
        else
        {
            type_t * type = variable->get_type();

            if (__is_custom_struct_like(type))
                pool.append<x_push_local_addr_xil_t>(variable->identity);
            else
                pool.append<x_push_local_xil_t>(dtype, variable->identity);
        }
    }

    // Compiles param variable.
    static void __compile_param_variable(__cctx_t & ctx, xil_pool_t & pool,
                param_variable_t * variable, xil_type_t dtype, expression_t * owner_exp)
    {
        param_t * param = variable->param;

        if (param->ptype == param_type_t::extends)
        {
            pool.append<x_push_params_xil_t>();
            return;
        }

        int index = __param_index(ctx, variable);
        type_t * type = variable->get_type();

        if (__is_addr(variable))    // ref, out param
        {
            if (dtype == xil_type_t::ptr)
                pool.append<x_push_argument_xil_t>(xil_type_t::ptr, index);
            else if (__is_custom_struct_like(type))
                pool.append<x_push_argument_xil_t>(xil_type_t::ptr, index);
            else
                pool.append<x_push_argument_content_xil_t>(__to_xil_type(ctx, variable), index);
        }
        else if (vtype_t vtype = variable->get_vtype();     // when call ((int)v).MyMethod()
            is_system_value_type(vtype) && __is_left_member(owner_exp))
        {
            pool.append<x_push_argument_xil_t>(__to_xil_type(ctx, variable), index);
            // pool.append<x_push_box_xil_t>(__ref_of(ctx, type));
        }
        else if (__is_custom_struct_like(type))    // custom struct
        {
            pool.append<x_push_argument_addr_xil_t>(index);
            /*
            if (__is_calling_method_owner(owner_exp))
                pool.append<x_push_box_xil_t>(__ref_of(ctx, type));
            */
        }
        else if (type->this_gtype() == gtype_t::generic_param)
        {
            pool.append<x_push_argument_addr_xil_t>(index);
        }
        else
        {
            pool.append<x_push_argument_xil_t>(__to_xil_type(ctx, variable), index);
        }
    }

    // Compiles field variable.
    static void __compile_field_variable(__cctx_t & ctx, xil_pool_t & pool,
                                    field_variable_t * field_var, xil_type_t dtype)
    {
        field_t * field = field_var->field;
        __FailedWhenNull(field, "unknown field '%1%'", field_var);

        ref_t field_ref = __search_field_ref(ctx, field);
        type_t * field_type = to_type(field->type_name);
        __FailedWhenNull(field_type, "unknown field '%1%' type", field);

        // TODO: when member point, int, long ... types also should be ptr type.
        if (__is_custom_struct_like(field_type))
        {
            pool.append<x_push_field_addr_xil_t>(field_ref);
        }
        else
        {
            // The type of the field: see __compile_local_variable.
            dtype = __to_xil_type(field_type);

            pool.append<x_push_field_xil_t>(field_ref, dtype);
        }
    }

    // Pre call a method.
    static void __pre_call_method(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp,
                                                                type_t * ret_type)
    {
        _A(exp != nullptr);

        // When return type is custom struct, but not assign to a variable.
        if (__is_custom_struct_like(ret_type))
        {
            if (exp->parent == nullptr)
            {
                local_variable_t * variable = ctx.define_temp_local(ret_type);
                __compile_local_variable(ctx, pool, variable);
            }
            else if (__is_left_member(__see_through(exp)->parent, __see_through(exp), true) || __is_function_namex(exp) || __is_index_namex(exp))
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

        // The result is as the caller sees it: a generic param of the callee is replaced by
        // its argument, the callee knows nothing of the local variable of the caller.
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

        // The behaviour of an await is that of the call that waits (that is its operand): what is done with its value
        // depends on what holds it.
        if (exp->this_family() == expression_family_t::checked && ((checked_expression_t *)exp)->is_await)
            return __is_whole_effective(exp->parent);

        return __is_effective(exp->get_behaviour()) || __is_whole_effective(exp->parent);
    }

    // Pops return value from stack
    static bool __try_pop_empty_for_method(__cctx_t & ctx, xil_pool_t & pool,
                                        method_base_t * method, expression_t * owner_exp)
    {
        // The parentheses that hold the call do not count: (a + b).ToString().
        expression_t * outer = owner_exp;
        while (outer->parent != nullptr && outer->parent->this_family() == expression_family_t::expressions
                    && outer->parent->expression_count() == 1)
        {
            outer = outer->parent;
        }

        if (__is_whole_effective(outer->parent))
            return false;

        // The value is the object of the member that follows (a.F().G();).
        for (expression_t * e = outer, * p = e->parent; __is_member_expression(p);
                                                                    e = p, p = p->parent)
        {
            if (((binary_expression_t *)p)->exp1() == e)
                return false;
        }

        type_t * ret_type = method->get_type();
        vtype_t vtype;

        // The result is a value of a generic param (R of Delegate<R, ...>): the call was made as
        // if it returned a custom struct (see __pre_call_method), what is left depends on the type
        // that the param is when the code runs: nothing for a struct, the address and the value
        // for the others (the value is nothing for Void).
        if (ret_type != nullptr && ret_type->this_gtype() == gtype_t::generic_param
                && owner_exp->parent == nullptr)
        {
            pool.append<x_pop_empty_xil_t>(__ref_of(ctx, ret_type), true);
            return true;
        }

        if ((ret_type != nullptr && (vtype = ret_type->this_vtype()) != vtype_t::void_)
            && !__is_custom_struct_like(ret_type))
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

        if (!__try_compile_pointer_method(ctx, pool, method, owner_exp))
        {
            ref_t method_ref = __search_method_ref(ctx, method);
            pool.append<x_method_call_xil_t>(__call_type_of_method(method), method_ref);
        }

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
            method_variable_t * method_var, xil_type_t dtype, xil_call_type_t call_type)
    {
        method_t * method = method_var->method;

        if (method == nullptr)
            __Failed("unknown method '%1%'", method_var);

        if (method->is_static())
            pool.append<x_push_null_xil_t>();

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

        if (call_type != xil_call_type_t::empty)
        {
            _A(call_type == xil_call_type_t::instance);

            atypes.push_back(atype_t(__XPool.get_internal_type(vtype_t::int32_)));
            pool.append<x_push_const_xil_t<int32_t>>(
                xil_type_t::int32, tvalue_t((int32_t)call_type)
            );
        }

        analyze_member_args_t args(member_type_t::method, name_t::null, &atypes);
        args.method_trait = method_trait_t::constructor;
        method_t * constructor = (method_t *)delegate_type->get_member(args, true);
        _A(constructor != nullptr);

        ref_t constructor_method_ref = __search_method_ref(ctx, constructor);
        pool.append<x_method_call_xil_t>(xil_call_type_t::instance, constructor_method_ref);
    }

    // Check whether static member has instance object reference.
    static void __check_static_member(__cctx_t & ctx, expression_t * owner_exp,
                              variable_t * variable, bool * out_member_is_static = nullptr)
    {
        member_t * member;
        if (variable == nullptr || (member = variable->get_relation_member()) == nullptr)
            return;

        // The fields of the state object of an iterator have their object.
        if (variable->this_type() == variable_type_t::field
                && ((field_variable_t *)variable)->hoist_base != nullptr)
        {
            al::assign_value(out_member_is_static, false);
            return;
        }

        expression_t * left_exp;
        bool has_instance, is_member_access = false;

        // The member is accessed through an object only when it is the right operand (obj.Member): a name that is the
        // left operand (Table.Length, where Table is a static field) is the object, it is used by its simple name.
        if (__is_member_expression(owner_exp->parent, &left_exp) && __is_right_member(owner_exp->parent, owner_exp))
        {
            has_instance = __is_instance(left_exp);
            is_member_access = true;
        }
        else
            has_instance = !ctx.statement_ctx.method->is_static();

        bool is_static = member->is_static();
        al::assign_value(out_member_is_static, is_static);

        // _P(_T("__check_static_member"), owner_exp, variable, is_static, has_instance);

        // A static member used by its simple name is fine in an instance method.
        if (is_static && has_instance && is_member_access)
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

        if (owner_exp != nullptr && !__is_right_member(owner_exp->parent, owner_exp))
        {
            if (!__push_hoist_base(ctx, pool, variable))
                pool.append<x_push_this_ref_xil_t>();
        }
    }

    // Pushes this with check for function.
    static void __push_this_with_check_for_function(__cctx_t & ctx, xil_pool_t & pool,
                                        expression_t * owner_exp, variable_t * variable)
    {
        if (owner_exp == nullptr)
            return;

        bool is_static;
        __check_static_member(ctx, owner_exp, variable, &is_static);

        expression_t * exp1;

        // The object is the left operand of obj.Property. A property that is the left operand itself (Property.Member) is
        // read from this: it is not the object of itself.
        if (__is_member_expression(owner_exp->parent, &exp1) && __is_right_member(owner_exp->parent, owner_exp))
        {
            // A property of a generic param (t.Value, where T : IFoo): the object is a
            // reference, or the address of a struct.
            if (variable->this_type() == variable_type_t::property
                    && __is_constrained_property(((property_variable_t *)variable)->property))
            {
                __compile_constrained_receiver(ctx, pool, exp1);
            }
            else
            {
                exp1->compile(ctx, pool);
            }
        }
        else if (!is_static)
        {
            pool.append<x_push_this_ref_xil_t>();
        }
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

        // The value is pushed with the type of the field (const Double D = 1; is a double): the conversion to the type that the
        // expression needs, (Double)Big, is made once, by __post_compile_expression, from the type of the field. Converting it
        // here to the needed type too would convert it twice.
        if (cv.value_type == cvalue_type_t::number)
        {
            vtype_t field_vtype = field->get_vtype();
            if (is_system_value_type(field_vtype))
                cv = cv.change_type(field_vtype);
        }

        __compile_cvalue(ctx, pool, cv, __environment_t::empty);
    }

    // Compiles variable.
    static void __compile_variable(__cctx_t & ctx, xil_pool_t & pool,
            variable_t * variable, const __environment_t & env, expression_t * owner_exp)
    {
        _A(variable != nullptr);

        switch (variable->this_type())
        {
            case variable_type_t::local:
                __compile_local_variable(ctx, pool, (local_variable_t *)variable, env);
                break;

            case variable_type_t::param:
                __compile_param_variable(ctx, pool, (param_variable_t *)variable, env, owner_exp);
                break;

            case variable_type_t::field: {
                
                field_t * field = ((field_variable_t *)variable)->field;
                if (is_static_const(field))
                {
                    __compile_static_const_field(ctx, pool, field, env);
                }
                else if (field->is_static())
                {
                    __check_static_member(ctx, owner_exp, variable);
                    __push_static_data(ctx, pool, field);
                    __compile_field_variable(ctx, pool, (field_variable_t *)variable, env);
                }
                else
                {
                    __push_this_with_check(ctx, pool, owner_exp, variable);
                    __compile_field_variable(ctx, pool, (field_variable_t *)variable, env);
                }

            }   break;

            case variable_type_t::property: {

                property_variable_t * property_var = (property_variable_t *)variable;

                // p.Value of a pointer to a struct, whose member is read or written (p.Value.X): the address of the value is the
                // object of the member, there is no destination.
                method_base_t * getter = property_var->property->get_method;
                bool pointer_address = getter != nullptr && __pointer_op_of(getter) == __pointer_op_t::get_value
                                && __pointer_to_struct(getter->get_host_type()) && __is_pointer_value_of_member(owner_exp);

                if (!pointer_address)
                    __pre_call_method(ctx, pool, owner_exp, property_var->property->get_method);

                __push_this_with_check_for_function(ctx, pool, owner_exp, variable);
                __compile_property_variable(ctx, pool, property_var, env, owner_exp);

            }   break;

            case variable_type_t::event: {

                // The handlers are those of the field of the event.
                event_t * event = ((event_variable_t *)variable)->event;
                if (event->field == nullptr)
                {
                    __Failed("the event '%1%' has no field: it is only added and removed",
                                                                            event->name);
                }

                __compile_variable(ctx, pool, event->field->variable, env, owner_exp);

            }   break;

            case variable_type_t::method:
                // A method of the object, that is not that of an expression (obj.Method): the
                // object is this.
                if (!((method_variable_t *)variable)->method->is_static()
                        && !__is_right_member(owner_exp->parent, owner_exp))
                {
                    method_t * method = ((method_variable_t *)variable)->method;

                    if (method->lambda_env_new != nullptr)
                        method->lambda_env_new->compile(ctx, pool);
                    else
                        pool.append<x_push_this_ref_xil_t>();
                }

                __compile_method_variable(ctx, pool, (method_variable_t *)variable, env,
                    __left_is_base(owner_exp->parent)? xil_call_type_t::instance
                                                     : xil_call_type_t::empty
                );
                break;

            default:
                X_UNEXPECTED();
        }
    }

    void __post_compile_expression(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp,
            const __environment_t & env = __environment_t::empty,
            type_t * exp_type = nullptr       // expression type maybe changed.
    )  
    {
        #define __ExpType   _N(exp_type, exp->get_type())

        // When result is not required.
        if (exp->parent == nullptr && !__is_effective(exp->get_behaviour()))
        {
            xil_type_t xt = to_xil_type(__ExpType);
            pool.append<x_pop_empty_xil_t>(xt);

            return;
        }

        // When need to box.
        if ((is_value_type(__ExpType) || is_enum(__ExpType)) && env.is_ref())
        {
            pool.append<x_push_box_xil_t>(__ref_of(ctx, exp_type));
            return;
        }

        // A generic param is an object: a reference, or a boxed value (T to Object).
        if (is_generic_param(__ExpType) && env.is_ref()
                && (__is_generic_value_variable(exp) || __is_generic_value_field(exp)))
        {
            pool.append<x_generic_copy_xil_t>(__ref_of(ctx, __ExpType), xil_copy_kind_t::box);
            return;
        }

        // When need to convert.
        if (type_t * type = env.type; type != nullptr && is_value_type(type))
        {
            if (env.dtype != xil_type_t::empty && is_value_type(__ExpType))
            {
                xil_type_t xil_type = to_xil_type(exp_type);
                __try_append_convert_xil(pool, xil_type, env.dtype);
            }
        }

        #undef __ExpType
    }

    ////////// ////////// ////////// ////////// //////////

    // Executes binary expression.
    cvalue_t __sys_t<binary_expression_t>::execute()
    {
        return __super_t::execute();
    }

    static void __compile_null_conditional(__cctx_t & ctx, xil_pool_t & pool,
                    expression_t * top, const __environment_t & env);

    // Compiles a chain of members that has the members of ?. (a?.b.c): the object of the member
    // is set to a temp variable, when it is null the value of the chain is null, else the chain
    // is compiled with the temp variable as the object.
    static void __compile_null_conditional(__cctx_t & ctx, xil_pool_t & pool,
                    expression_t * top, const __environment_t & env)
    {
        // The first member of ?. or ?[ of the chain.
        expression_t * member = nullptr;
        bool member_is_index = false;

        for (expression_t * e = top; e != nullptr && null_conditional_object(e) != nullptr;
                                                        e = null_conditional_object(e))
        {
            if (e->this_family() == expression_family_t::index)
            {
                if (((index_expression_t *)e)->null_conditional)
                    member = e, member_is_index = true;
            }
            else if (((binary_expression_t *)e)->null_conditional)
            {
                member = e, member_is_index = false;
            }
        }

        _A(member != nullptr);

        statement_compile_context_t & sctx = ctx.statement_ctx;

        expression_t * object = null_conditional_object(member);
        type_t * object_type = object->get_type();
        __FailedWhenNull(object_type, "type of the object of ?. missing");

        type_t * result_type = top->get_type();
        // Whether the value of the chain is used: a statement does not use it.
        bool effective = __is_whole_effective(top->parent);
        bool is_void = result_type == nullptr || is_void_type(result_type);
        bool is_nullable_value = !is_void && get_nullable_underlying(result_type) != nullptr;

        if (!is_void && !is_nullable_value && !can_be_null(result_type))
        {
            __Failed("the member of '%1%' that is accessed by ?. cannot be null: '%2%'",
                                                                  object_type, result_type);
        }

        // The object is set to a temp variable.
        local_variable_t * temp = ctx.define_temp_local(object_type);
        xil_type_t object_xtype = to_xil_type(object_type);

        object->compile(ctx, pool);
        pool.append<x_pop_local_xil_t>(object_xtype, temp->identity);

        // Whether it is not null.
        pool.append<x_push_local_xil_t>(object_xtype, temp->identity);
        pool.append<x_push_null_xil_t>();
        pool.append<x_cmp_xil_t>(xil_cmp_command_t::equal, xil_type_t::object, xil_type_t::object);

        jmp_xil_t * not_null_jmp_xil = xil::append_jmp_xil(pool, xil_jmp_model_t::false_);

        // The chain is compiled with the temp variable, that is the object of the member.
        auto compile_chain = [&](auto && compile) {
            name_expression_t * temp_exp = __XPool.new_obj<system_expression_t<name_expression_t>>(temp);
            temp_exp->parent = member;

            if (member_is_index)
            {
                index_expression_t * index = (index_expression_t *)member;
                array_index_variable_t * array_var = index->variable != nullptr
                    && index->variable->this_type() == variable_type_t::array_index?
                        (array_index_variable_t *)index->variable : nullptr;

                index->set_namex(temp_exp);
                if (array_var != nullptr)
                    array_var->body = temp_exp;

                index->null_conditional = false;

                compile();

                index->set_namex(object);
                if (array_var != nullptr)
                    array_var->body = object;

                index->null_conditional = true;
            }
            else
            {
                binary_expression_t * binary = (binary_expression_t *)member;

                binary->exps[0] = temp_exp;
                binary->null_conditional = false;

                compile();

                binary->exps[0] = object;
                binary->null_conditional = true;
                object->parent = member;
            }
        };

        local_variable_t * result = nullptr;

        if (is_void || !effective)
        {
            // Null: the chain is skipped.
            jmp_xil_t * end_jmp_xil;

            end_jmp_xil = xil::append_jmp_xil(pool, xil_jmp_model_t::none);
            xil::append_label_xil(sctx, pool, not_null_jmp_xil);

            compile_chain([&]() { top->compile(ctx, pool, env); });

            xil::append_label_xil(sctx, pool, end_jmp_xil);
            return;
        }

        ref_t result_ref = __ref_of(ctx, result_type);

        if (is_nullable_value)
        {
            // Null: the nullable has no value.
            result = ctx.define_temp_local(result_type);

            pool.append<x_push_local_addr_xil_t>(result->identity);
            pool.append<x_object_copy_xil_t>(result_ref, xil_copy_kind_t::zero);

            jmp_xil_t * end_jmp_xil = xil::append_jmp_xil(pool, xil_jmp_model_t::none);
            xil::append_label_xil(sctx, pool, not_null_jmp_xil);

            // Not null: the value of the chain, that is nullable.
            compile_chain([&]() {
                pool.append<x_push_local_addr_xil_t>(result->identity);

                if (!__try_compile_nullable_value(ctx, pool, result_type, top))
                    top->compile(ctx, pool, result_type);
            });

            xil::append_label_xil(sctx, pool, end_jmp_xil);

            // The value is the variable.
            name_expression_t * result_exp = __XPool.new_obj<system_expression_t<name_expression_t>>(result);
            result_exp->parent = top->parent;
            result_exp->compile(ctx, pool, env);
            return;
        }

        // A reference: null is the value of the chain, when the object is null.
        pool.append<x_push_null_xil_t>();
        jmp_xil_t * end_jmp_xil = xil::append_jmp_xil(pool, xil_jmp_model_t::none);
        xil::append_label_xil(sctx, pool, not_null_jmp_xil);

        compile_chain([&]() { top->compile(ctx, pool, env); });

        xil::append_label_xil(sctx, pool, end_jmp_xil);
    }

    // A chain of + of strings (a + b + c + d) is the texts of its operands joined by one call of String.Concat (a call of
    // the operator + makes a string for each +, and copies the ones that were made before). Each operand that is not a string is
    // changed to its text by StringHelper.Text(operand), the same text as the operator + of String has for it. The order of the
    // evaluation is the same (from the left to the right).
    static bool __is_string_plus(expression_t * exp)
    {
        if (exp == nullptr || exp->this_family() != expression_family_t::binary)
            return false;

        binary_expression_t * binary = (binary_expression_t *)exp;
        if (binary->op() != operator_t::add || binary->overload_method == nullptr || binary->op_property->is_assign)
            return false;

        type_t * host = binary->overload_method->get_host_type();
        return host != nullptr && is_string_type(host);
    }

    // The operands of a chain, from the left to the right (the parentheses are not operands).
    static void __collect_string_operands(expression_t * exp, std::vector<expression_t *> & operands)
    {
        while (exp->this_family() == expression_family_t::expressions && exp->expression_count() == 1)
            exp = exp->expression_at(0);

        if (__is_string_plus(exp))
        {
            __collect_string_operands(exp->expression_at(0), operands);
            __collect_string_operands(exp->expression_at(1), operands);
            return;
        }

        operands.push_back(exp);
    }

    // Returns the method of a type that has the name and the types of the arguments (the best that the arguments are converted to).
    static method_t * __find_static_method(type_t * type, const char_t * name, const std::vector<type_t *> & types)
    {
        atypes_t atypes;
        for (type_t * t : types)
            atypes.push_back(atype_t(t));

        analyze_member_args_t args(member_type_t::method, __XPool.to_name(name), &atypes);
        return (method_t *)type->get_member(args, true);
    }

    static bool __try_compile_string_concat(__cctx_t & ctx, xil_pool_t & pool, binary_expression_t * top)
    {
        if (!__is_string_plus(top))
            return false;

        std::vector<expression_t *> operands;
        __collect_string_operands(top, operands);

        // Two operands are what the operator does.
        if (operands.size() < 3)
            return false;

        type_t * string_type = top->overload_method->get_host_type();
        type_t * helper_type = __XPool.get_string_helper_type();
        if (helper_type == nullptr)
            return false;

        // The methods: the text of each operand (StringHelper.Text), Concat of the number of the texts.
        std::vector<method_t *> texts(operands.size(), nullptr);
        for (size_t k = 0; k < operands.size(); k++)
        {
            type_t * type = operands[k]->get_type();
            if (type == nullptr || is_string_type(type))
                continue;

            texts[k] = __find_static_method(helper_type, _T("Text"), { type });
            if (texts[k] == nullptr)
                return false;
        }

        auto concat_of = [&](size_t count) -> method_t * {
            return __find_static_method(string_type, _T("Concat"), std::vector<type_t *>(count, string_type));
        };

        for (size_t count = 2; count <= 8; count++)
        {
            if (concat_of(count) == nullptr)
                return false;
        }

        // The texts are pushed and joined by groups of 8: the result of a group is the first text of the next one.
        size_t index = 0;
        bool first_group = true;

        while (index < operands.size())
        {
            size_t room = first_group? 8 : 7;
            size_t count = std::min<size_t>(room, operands.size() - index);

            // The texts of the group: the result of the group before (it is the first one), and the operands.
            size_t total = count + (first_group? 0 : 1);

            method_t * concat = concat_of(total);

            for (size_t k = 0; k < count; k++)
            {
                expression_t * operand = operands[index + k];
                size_t position = k + (first_group? 0 : 1);

                if (texts[index + k] != nullptr)
                {
                    __compile_argument(ctx, pool, operand, texts[index + k], 0);
                    pool.append<x_method_call_xil_t>(xil_call_type_t::static_, __search_method_ref(ctx, texts[index + k]));
                }
                else
                {
                    __compile_argument(ctx, pool, operand, concat, (int)position);
                }
            }

            pool.append<x_method_call_xil_t>(xil_call_type_t::static_, __search_method_ref(ctx, concat));

            index += count;
            first_group = false;
        }

        return true;
    }

    // Compiles binary expression.
    void __sys_t<binary_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool,
                                                       const __environment_t & env)
    {
        if (__try_compile_constant_expression(ctx, pool, this, env))
            return;

        // A chain that has a member of ?.
        if (this->op() == operator_t::member_point && this->is_null_conditional_chain_top())
        {
            __compile_null_conditional(ctx, pool, this, env);
            return;
        }

        expression_t * exp1 = expression_at(0), * exp2 = expression_at(1);

        _A(exp1 != nullptr);
        _A(exp2 != nullptr);

        // Operators overload.
        if (this->overload_method != nullptr)
        {
            if (this->op_property->is_assign && ctx.overload_assign_value != this)
            {
                // x += y: x = x + y, the method is that of +.
                ctx.overload_assign_value = this;

                __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp1);
                __compile_assign(ctx, pool, ca, this);

                ctx.overload_assign_value = nullptr;
            }
            else
            {
                // A chain of + of strings: one Concat of all the texts.
                if (__try_compile_string_concat(ctx, pool, this))
                    return;

                // The value of x += y is made by the method, assigned by the caller.
                bool is_value = ctx.overload_assign_value == this;
                __compile_operator_overload(ctx, pool, !is_value, !is_value, env);
            }

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
            // The value of a generic param that a getter returns (return T.MaxValue): when the
            // param is a custom struct, the getter writes it at a destination that is below, and
            // the value is copied to the destination of the caller.
            type_t * member_type = this->get_type();
            bool generic_value_return = op() == operator_t::member_point
                    && is_generic_param(member_type)
                    && env.is_value_mark() && __is_property_name(exp2);

            if (generic_value_return)
                pool.append<x_generic_stack_alloc_xil_t>(__ref_of(ctx, member_type));

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

                case operator_t::member_point:
                    __compile_member_point(ctx, pool, exp1, exp2, env);
                    break;

                __Case(assign)
                __Case(null_coalescing)
                __Case(null_coalescing_assign)

                __Case(as)
                __Case(is)

                default:
                    throw _ECF(not_supported, _T("unknown operator \"%1%\""), op());
            }

            #undef __Case

            if (generic_value_return && env.type_mark != type_mark_t::argument)
            {
                pool.append<x_generic_copy_xil_t>(
                    __ref_of(ctx, member_type), xil_copy_kind_t::call_ret
                );
            }

            __post_compile_expression(ctx, pool, this, env);
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

    // The method of an operator for a checked context: the one that is declared with checked (operator checked +)
    // is taken when a type of the operands has it, else the operator is the same in a checked context.
    static method_t * __checked_operator_method(__cctx_t & ctx, method_t * method,
                                                expression_t ** exps, int count)
    {
        if (!ctx.checked || method == nullptr)
            return method;

        string_t name = method->get_name();
        if (name.compare(0, 3, _T("op_")) != 0 || name.compare(0, 10, _T("op_Checked")) == 0)
            return method;

        name_t checked_name = __XPool.to_name(string_t(_T("op_Checked")) + name.substr(3));

        atypes_t atypes;
        for (int k = 0; k < count; k++)
            atypes.push_back(atype_t(exps[k]->get_type()));

        for (atype_t atype : atypes)
        {
            if (!is_object(atype.type) && !is_string_type(atype.type) && !is_generic_param(atype.type))
                continue;

            method_t * checked_method = nullptr;

            try
            {
                checked_method = find_method(atype.type, method_trait_t::default_, checked_name, nullptr, &atypes);
            }
            catch (const method_compare_error_t &) { }

            if (checked_method != nullptr && checked_method->is_static())
                return checked_method;
        }

        return method;
    }

    // Compiles operator overload..
    void __sys_t<binary_expression_t>::__compile_operator_overload(__cctx_t & ctx, xil_pool_t & pool,
                                                                   bool pop_result, bool pre_call,
                                                                   const __environment_t & env)
    {
        method_t * method = __checked_operator_method(ctx, this->overload_method, this->exps, 2);

        // Checks method prototype.
        __check_overload_prototype(ctx, method, 2, this->exps);

        // The value of a generic param is returned: when the param is a custom struct, the
        // method writes it at a destination that is below.
        bool generic_value_return = pre_call && is_generic_param(method->get_type())
                                        && env.is_value_mark();

        if (generic_value_return)
            pool.append<x_generic_stack_alloc_xil_t>(__ref_of(ctx, method->get_type()));

        if (pre_call)
            __pre_call_method(ctx, pool, this, method);

        // Compile arguments.
        __compile_argument(ctx, pool, expression_at(0), method, 0);
        __compile_argument(ctx, pool, expression_at(1), method, 1);

        if (__try_compile_pointer_method(ctx, pool, method, this))
        {
            if (pop_result)
                __try_pop_empty_for_method(ctx, pool, method, this);

            return;
        }

        ref_t method_ref = __search_method_ref(ctx, method);
        pool.append<x_method_call_xil_t>(xil_call_type_t::static_, method_ref);

        if (generic_value_return && env.type_mark != type_mark_t::argument)
        {
            pool.append<x_generic_copy_xil_t>(
                __ref_of(ctx, method->get_type()), xil_copy_kind_t::call_ret
            );
        }

        if (pop_result)
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
        __check_constant_overflow(ctx, operator_t::minus, exp);

        exp->compile(ctx, pool);

        pool.append<x_al_xil_t>(
            __al_command(ctx, xil_al_command_t::minus, __xil_type(exp)), __xil_type(exp)
        );
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
    void __sys_t<unitary_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool, 
                                                            const __environment_t & env)
    {
        if (__try_compile_constant_expression(ctx, pool, this, env))
            return;

        expression_t * exp = expression_at(0);
        _A(exp != nullptr);

        // x! is x.
        if (this->op() == operator_t::null_forgiving)
        {
            exp->compile(ctx, pool, env);
            return;
        }

        if (this->overload_method != nullptr)
        {
            if (this->op_property->is_assign && ctx.overload_assign_value != this)
            {
                // x++, ++x: x = operator(x). The value of x++ is the old value of x.
                bool right = op() == operator_t::right_increment
                                || op() == operator_t::right_decrement;

                ctx.overload_assign_value = this;

                __compile_assign_t ca = __pre_compile_assign_to(ctx, pool, exp);

                if (right && is_effective(ctx, this->parent))
                    exp->compile(ctx, pool);

                __compile_assign(ctx, pool, ca, this,
                    right? __assign_to_type_t::pop : __assign_to_type_t::default_
                );

                ctx.overload_assign_value = nullptr;
            }
            else
            {
                bool is_value = ctx.overload_assign_value == this;
                __compile_operator_overload(ctx, pool, !is_value, !is_value, env);
            }

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

            __post_compile_expression(ctx, pool, this, env);
        }
    }

    // Compiles operator overload..
    void __sys_t<unitary_expression_t>::__compile_operator_overload(__cctx_t & ctx,
                                                                    xil_pool_t & pool,
                                                                    bool pop_result,
                                                                    bool pre_call,
                                                                    const __environment_t & env)
    {
        method_t * method = __checked_operator_method(ctx, this->overload_method, this->exps, 1);

        // Checks method prototype.
        __check_overload_prototype(ctx, method, 1, this->exps);

        bool generic_value_return = pre_call && is_generic_param(method->get_type())
                                        && env.is_value_mark();

        if (generic_value_return)
            pool.append<x_generic_stack_alloc_xil_t>(__ref_of(ctx, method->get_type()));

        if (pre_call)
            __pre_call_method(ctx, pool, this, method);

        // Compile arguments.
        __compile_argument(ctx, pool, expression_at(0), method, 0);

        if (__try_compile_pointer_method(ctx, pool, method, this))
        {
            if (pop_result)
                __try_pop_empty_for_method(ctx, pool, method, this);

            return;
        }

        ref_t method_ref = __search_method_ref(ctx, method);
        pool.append<x_method_call_xil_t>(xil_call_type_t::static_, method_ref);

        if (generic_value_return && env.type_mark != type_mark_t::argument)
        {
            pool.append<x_generic_copy_xil_t>(
                __ref_of(ctx, method->get_type()), xil_copy_kind_t::call_ret
            );
        }

        if (pop_result)
            __try_pop_empty_for_method(ctx, pool, method, this);
    }

    ////////// ////////// ////////// ////////// //////////
    // name_expression_t

    // Executes name expression.
    cvalue_t __sys_t<name_expression_t>::execute()
    {
        return __super_t::execute();
    }

    // Returns whether the variable is local, field, param or array item.
    bool is_real_variable(variable_t * variable)
    {
        return al::in(variable->this_type(),
            variable_type_t::local, variable_type_t::param, variable_type_t::field,
            variable_type_t::array_index
        );
    }

    // Returns whether a variable need object copy.
    static bool __need_object_copy(expression_t * exp)
    {
        expression_t * parent_exp = exp->parent;

        for (expression_t * parent_exp; (parent_exp = exp->parent) != nullptr; exp = parent_exp)
        {
            switch (parent_exp->this_family())
            {
                case expression_family_t::binary:
                {
                    binary_expression_t * binary_exp = (binary_expression_t *)parent_exp;

                    // The operand of an operator that is a method is an argument of a call.
                    if (binary_exp->overload_method != nullptr)
                        return true;

                    switch (binary_exp->op())
                    {
                        case operator_t::member_point:
                        case operator_t::assign:
                            if (binary_exp->exp1() == exp)
                                return false;

                        default:
                            break;
                    }

                    break;
                }

                // The same for the operand of a unary operator that is a method ((-a).ToString()).
                case expression_family_t::unitary:
                    if (((unitary_expression_t *)parent_exp)->overload_method != nullptr)
                        return true;

                    break;

                // The operand of a cast to a reference type is boxed: the box reads the struct from its
                // address, there is no destination to copy to ((Object)p).
                case expression_family_t::type_cast:
                {
                    type_t * cast_type = ((type_cast_expression_t *)parent_exp)->get_type();
                    if (cast_type != nullptr && is_ref_type(cast_type) && is_custom_struct(exp->get_type()))
                    {
                        // A new object and a call write at the destination that the cast makes (see the cast).
                        expression_behaviour_t behaviour = exp->get_behaviour();
                        if (behaviour != expression_behaviour_t::new_
                                && behaviour != expression_behaviour_t::execute)
                            return false;
                    }

                    break;
                }

                // An argument of a call is a value: what the call is, does not matter to it
                // (Plain(p).V copies p to the argument).
                case expression_family_t::function:
                    return true;

                // The same for the arguments of a constructor (new Box(1, values[i]).Get()): they are not
                // the left of the member that follows the new expression.
                case expression_family_t::new_:
                    return true;

                // The same for the arguments of an index (lookup[key].GetEnumerator()), not for
                // the object that is indexed.
                case expression_family_t::index:
                {
                    index_expression_t * index_exp = (index_expression_t *)parent_exp;
                    if (index_exp->namex() != exp)
                        return true;

                    // The object that an indexer is read from is the object of a call: a struct is its address.
                    if (index_exp->variable != nullptr
                            && index_exp->variable->this_type() == variable_type_t::property_index)
                        return false;

                    break;
                }

                default:
                    break;
            }
        }

        return true;
    }

    static void __append_object_copy_xil(__cctx_t & ctx, xil_pool_t & pool, type_t * type,
                                                            xil_copy_kind_t copy_kind)
    {
        if (is_generic_param(type))
            pool.append<x_generic_copy_xil_t>(__ref_of(ctx, type), copy_kind);
        else
            pool.append<x_object_copy_xil_t>(__ref_of(ctx, type), copy_kind);
    }

    // Compiles name expression.
    void __sys_t<name_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool,
                                                        const __environment_t & env)
    {
        if (!__is_this_effective(ctx, this))
            return;

        switch (this->expression_type)
        {
            case name_expression_type_t::variable:
            {
                type_t * type = this->get_type();

                __compile_variable(ctx, pool, this->variable, __only_value_type(env), this);

                if (__is_custom_struct_like(type) && is_real_variable(this->variable)
                                           && __need_object_copy(this))
                {
                    // A generic param that is a value (e.g. returned) is loaded into the
                    // stack, not copied to a destination.
                    if (is_generic_param(type) && env.is_value_mark())
                        __append_object_copy_xil(ctx, pool, type, xil_copy_kind_t::ret);
                    else
                        __append_object_copy_xil(ctx, pool, type, xil_copy_kind_t::reverse);
                }
                    
                __post_compile_expression(ctx, pool, this, env);
            }   break;

            case name_expression_type_t::type:
            case name_expression_type_t::type_def:
                break;

            default:
                X_UNEXPECTED_F("unexpected name expression type '%1%'", this->expression_type);
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
                                                          const __environment_t & env)
    {
        if (!__is_this_effective(ctx, this))
            return;

        switch (this->expression_type)
        {
            case name_expression_type_t::variable:
                break;

            case name_expression_type_t::type:
            case name_expression_type_t::type_def:
                break;

            default:
                X_UNEXPECTED_F("unexpected name-unit expression type '%1%'", this->expression_type);
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
                pool.append<x_push_const_xil_t<rt_char_t>>(xil_type_t::char_, value);
                break;

            default:
            case value_type_t::ldouble_:
                X_UNEXPECTED_F("unexpected value type '%1%'", value.type);
        }
    }

    // Compiles string.
    void __compile_string(__cctx_t & ctx, xil_pool_t & pool, const string_t & string)
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
    void __compile_cvalue(__cctx_t & ctx, xil_pool_t & pool, cvalue_t cvalue,
                                                const __environment_t & env)
    {
        switch (cvalue.value_type)
        {
            case cvalue_type_t::string:
                __compile_string(ctx, pool, cvalue.get_string());
                break;

            case cvalue_type_t::number: {
                xil_type_t dtype = env.dtype;
                if (dtype != xil_type_t::empty)
                    cvalue = cvalue.change_type(to_vtype(dtype));
                __compile_number(ctx, pool, cvalue.number);
            }    break;

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
    void __sys_t<cvalue_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool,
                                                    const __environment_t & env)
    {
        if (!__is_this_effective(ctx, this))
            return;

        cvalue_t cvalue = *this->value;

        xil_type_t dtype = env.dtype;
        if (is_number(cvalue) && dtype == xil_type_t::object)    // Box.
        {
            dtype = xil_type_t::empty;
            __compile_cvalue(ctx, pool, cvalue, dtype);

            type_t * type = this->get_type();
            __FailedWhenNull(type, "type empty when compile cvalue_expression_t");

            // pool.append<x_push_box_xil_t>(__ref_of(ctx, type));
        }
        else
        {
            // The natural type of the constant; the conversion is made by the post compile.
            __compile_cvalue(ctx, pool, cvalue, __environment_t::empty);
        }

        __post_compile_expression(ctx, pool, this, env);
    }

    ////////// ////////// ////////// ////////// //////////
    // function_expression_t

    // Executes function expression.
    cvalue_t __sys_t<function_expression_t>::execute()
    {
        return __super_t::execute();
    }
    
    // Compiles function expression.
    void __sys_t<function_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool,
                                                    const __environment_t & env)
    {
        type_t * type = this->get_type();
        if (__is_ref_type(env.type) && __is_custom_struct_like(type))
        {
            pool.append<x_new_xil_t>(__ref_of(ctx, type));
            pool.append<x_push_duplicate_xil_t>();
        }

        function_expression_type_t ftype = this->get_ftype();

        switch (ftype)
        {
            case function_expression_type_t::method:
                __compile_method(ctx, pool, env);
                break;

            case function_expression_type_t::variable:  // Delegate
                __compile_delegate(ctx, pool, env);
                break;

            case function_expression_type_t::__default__:
                __compile_default(ctx, pool, env);
                break;

            default:
                X_UNEXPECTED_F("unexpected function expression type '%1%'", ftype);
        }
    }

    enum __xil_call_command_t
    {
        none                = (int)xil_call_command_t::none,

        array_get_value     = -1000,

        array_set_value,
    };

    static __xil_call_command_t __to_command_command(xil_call_type_t call_type, name_t name)
    {
        if (call_type != xil_call_type_t::internal)
            return __xil_call_command_t::none;

    #define __IsInternalCommand(_name, _command)                                        \
        {                                                                               \
            static name_t static_##_name;                                               \
                                                                                        \
            if (static_##_name == name_t::null)                                         \
                static_##_name = __XPool.to_name(_S(_name));                            \
                                                                                        \
            if (static_##_name == name)                                                 \
                return (__xil_call_command_t)_command;                                  \
        }

        typedef xil_call_command_t      t1;

        __IsInternalCommand(Delegate_Invoke,    t1::delegate_invoke)
        __IsInternalCommand(Delegate_Init,      t1::delegate_init)
        __IsInternalCommand(Delegate_InitWithCallType, t1::delegate_init_with_call_type)

        typedef __xil_call_command_t    t2;
        __IsInternalCommand(Array_GetValue,     t2::array_get_value)
        __IsInternalCommand(Array_SetValue,     t2::array_set_value)

        return __xil_call_command_t::none;
    }

    // Get a generic array type.
    static type_t * __get_generic_array_type()
    {
        static type_t * array_type;

        if (array_type == nullptr)
        {
            general_type_t * tarray_type = __XPool.get_tarray_type();
            type_t * element_type = tarray_type->param_at(0);
            array_type = __XPool.new_array_type(element_type, 1);
        }

        return array_type;
    }

    // Compile method calling expression.
    void __sys_t<function_expression_t>::__compile_method(__cctx_t & ctx, xil_pool_t & pool,
                                                          const __environment_t & env)
    {
        method_base_t * method = this->get_method();
        _A(method != nullptr);

        xil_call_type_t call_type = __call_type_of_method(method);
        bool effective = is_effective(ctx, this);

        type_t * method_type = method->get_type();
        bool generic_value_return = is_generic_param(method_type)
                && call_type != xil_call_type_t::internal
                && env.is_value_mark();

        if (effective)
        {
            if (__is_right_member(this->parent, this))
            {
                // The destination is below the object, that is the first argument.
                if (generic_value_return)
                    pool.append<x_generic_stack_alloc_xil_t>(__ref_of(ctx, method->get_type()));

                __pre_call_method(ctx, pool, this->parent, method);

                type_t * host_type = method->get_host_type();
                _A(host_type != nullptr);

                expression_t * exp1 = ((binary_expression_t *)this->parent)->exp1();

                // A member of a generic param: the object is a reference, or the address of a
                // struct.
                if (call_type == xil_call_type_t::constrained)
                    __compile_constrained_receiver(ctx, pool, exp1, host_type);
                else
                    exp1->compile(ctx, pool, host_type);
            }
            else
            {
                bool is_static = __is_static(call_type);

                // The value of a generic param is returned (return Make<T>()): when the param is
                // a custom struct, the call writes it at a destination that is below.
                if (generic_value_return)
                    pool.append<x_generic_stack_alloc_xil_t>(__ref_of(ctx, method->get_type()));

                __pre_call_method(ctx, pool, this, method);

                if (!is_static)
                {
                    // A lambda that uses variables is called at once: its object is the
                    // environment.
                    method_t * lambda = dynamic_cast<method_t *>(method);

                    if (lambda != nullptr && lambda->lambda_env_new != nullptr)
                        lambda->lambda_env_new->compile(ctx, pool);
                    else
                        pool.append<x_push_this_ref_xil_t>();
                }
            }
            
            bool address_of = __try_compile_address_of(ctx, pool, this, method)
                                || __try_compile_size_of(ctx, pool, this, method)
                                || __try_compile_stack_alloc(ctx, pool, this, method);
            if (!address_of)
                __compile_arguments(ctx, pool, this->arguments(), method);

            ref_t method_ref = __search_method_ref(ctx, method);

            // _PF(_T("==== function call: %1%, %2%, %3%, %4%"), call_type, method->to_string(),
            //     (mt_member_extra_t)method_ref.extra, method_ref
            // );

            if (address_of)
            {
                // The instructions are those of the address.
            }
            else if (__left_is_base(this->parent) && call_type == xil_call_type_t::virtual_)
            {
                pool.append<x_method_call_xil_t>(xil_call_type_t::instance, method_ref);
            }
            else
            {
                __xil_call_command_t cmd = __to_command_command(call_type, method->get_name());

                switch (cmd)
                {
                    // Array get value.
                    case __xil_call_command_t::array_get_value:
                        // The address of the element, and the copy of the value, as for any value of a generic param: a struct is
                        // copied to the place of the result, a number and a reference are loaded.
                        pool.append<x_push_array_element_addr_xil_t>(__ref_of(ctx, __get_generic_array_type()));
                        pool.append<x_generic_copy_xil_t>(__ref_of(ctx, method->get_type()), xil_copy_kind_t::ret);
                        break;

                    // Array set value.
                    case __xil_call_command_t::array_set_value:
                        pool.append<x_pop_array_element_xil_t>(
                            env.dtype, __ref_of(ctx, __get_generic_array_type())
                        );
                        break;

                    // Call method.
                    case __xil_call_command_t::none:
                        pool.append<x_method_call_xil_t>(call_type, method_ref);
                        break;

                    // Call command.
                    default:
                        pool.append<x_command_call_xil_t>((xil_call_command_t)cmd);
                        break;
                }
            }

            if (generic_value_return && env.type_mark != type_mark_t::argument)
            {
                // A struct is copied to the destination of the caller of this method.
                pool.append<x_generic_copy_xil_t>(
                    __ref_of(ctx, method_type), xil_copy_kind_t::call_ret
                );
            }

            if (env.dtype != xil_type_t::empty && !generic_value_return)
            {
                type_t * ret_type = method->get_type();

                // A value that a method returns (a number, a string; T = Int32 of a generic method) is used as an Object: it is
                // boxed, like (Object)Method().
                if (env.dtype == xil_type_t::object && ret_type != nullptr && ret_type->this_vtype() != vtype_t::void_
                        && (is_system_value_type(ret_type->this_vtype()) || ret_type->this_vtype() == vtype_t::string_))
                {
                    pool.append<x_push_box_xil_t>(__ref_of(ctx, ret_type));
                }
                else
                    __try_append_convert_xil(pool, ret_type, env);
            }
            else if (env.dtype == xil_type_t::empty)
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
                                                            const __environment_t & env)
    {
        // Validate delegate type.
        variable_t * variable = this->get_variable();
        _A(variable != nullptr);

        type_t * type = variable->get_type();
        if (type->this_gtype() != gtype_t::generic)
            __Failed("delegate type error, should be System.Deletate<...>, but not %1%", type);

        generic_type_t * gtype = (generic_type_t *)type;
        general_type_t * template_ = gtype->template_;

        if (template_ != __XPool.get_delegate_type() && template_ != __XPool.get_multicast_delegate_type())
            __Failed("delegate type error, should be System.Deletate<...>, but not %1%", gtype);

        analyze_member_args_t args(member_type_t::method, __XPool.to_name(_T("Invoke")));
        method_t * method = (method_t *)gtype->get_member(args, true);
        _A(method != nullptr);

        // A struct result needs a place to be written.
        __pre_call_method(ctx, pool, this, method->get_type());

        // obj.Field(args): the object that has the field is pushed first, the delegate is read from it.
        if (__is_right_member(this->parent, this))
        {
            type_t * host_type = nullptr;
            if (variable->this_type() == variable_type_t::field)
                host_type = ((field_variable_t *)variable)->field->host_type;

            expression_t * exp1 = ((binary_expression_t *)this->parent)->exp1();

            if (host_type != nullptr)
                exp1->compile(ctx, pool, host_type);
            else
                exp1->compile(ctx, pool);
        }

        __compile_variable(ctx, pool, variable, xil_type_t::ptr, this);

        __compile_arguments(ctx, pool, this->arguments(), method);

        ref_t method_ref = __search_method_ref(ctx, method);
        pool.append<x_method_call_xil_t>(xil_call_type_t::instance, method_ref);

        if (env.dtype != xil_type_t::empty)
            __try_append_convert_xil(pool, method->get_type(), env);
        else
            __try_pop_empty_for_method(ctx, pool, method, this);
    }

    // Compile delegate calling expression, for complex namex expressions.
    void __sys_t<function_expression_t>::__compile_default(__cctx_t & ctx, xil_pool_t & pool,
                                                              const __environment_t & env)
    {
        if (this->namex == nullptr)     // TODO: why nullptr?
            return;

        type_t * type = this->namex->get_type();
        _A(type != nullptr);

        if (type->this_gtype() != gtype_t::generic)
            __Failed("delegate type error, should be System.Deletate<...>, but not %1%", type);

        generic_type_t * gtype = (generic_type_t *)type;
        general_type_t * template_ = gtype->template_;

        if (template_ != __XPool.get_delegate_type() && template_ != __XPool.get_multicast_delegate_type())
            __Failed("delegate type error, should be System.Deletate<...>, but not %1%", gtype);

        analyze_member_args_t args(member_type_t::method, __XPool.to_name(_T("Invoke")));
        method_t * method = (method_t *)gtype->get_member(args, true);
        _A(method != nullptr);

        __pre_call_method(ctx, pool, namex, method);
        namex->compile(ctx, pool);

        __compile_arguments(ctx, pool, this->arguments(), method);

        ref_t method_ref = __search_method_ref(ctx, method);
        pool.append<x_method_call_xil_t>(xil_call_type_t::instance, method_ref);

        if (env.dtype != xil_type_t::empty)
            __try_append_convert_xil(pool, method->get_type(), env);
        else
            __try_pop_empty_for_method(ctx, pool, method, this);
    }

    // Compile delegate calling expression.
    void __compile_delegate(expression_compile_context_t & ctx, xil_pool_t & pool);

    ////////// ////////// ////////// ////////// //////////
    // checked_expression_t

    // Executes checked expression.
    cvalue_t __sys_t<checked_expression_t>::execute()
    {
        return __super_t::execute();
    }

    // Compiles checked expression: the arithmetic of the expression raises OverflowException
    // when the result does not fit its type, or not.
    void __sys_t<checked_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool,
                                                        const __environment_t & env)
    {
        expression_t * exp = this->expression();
        __FailedWhenNull(exp, _T("expression empty"));

        bool checked = ctx.checked;
        if (!this->is_await)
            ctx.checked = this->checked;

        exp->compile(ctx, pool, env);

        ctx.checked = checked;
    }

    ////////// ////////// ////////// ////////// //////////
    // type_cast_expression_t

    // Executes type cast expression.
    cvalue_t __sys_t<type_cast_expression_t>::execute()
    {
        return __super_t::execute();
    }
    
    // Returns whether it is a conversion between numbers of different types.
    static bool __is_numeric_cast(type_t * from, type_t * to)
    {
        if (from == nullptr || to == nullptr)
            return false;

        vtype_t vt1 = from->this_vtype(), vt2 = to->this_vtype();

        auto is_number_vtype = [](vtype_t vt) {
            return is_numberic_vtype(vt) || vt == vtype_t::char_;
        };

        return vt1 != vt2 && is_number_vtype(vt1) && is_number_vtype(vt2);
    }

    // Compiles the call of the method of a user conversion: the value of the expression is its argument.
    static void __compile_conversion_call(__cctx_t & ctx, xil_pool_t & pool, expression_t * owner,
                                          expression_t * operand, method_t * method,
                                          const __environment_t & env)
    {
        // The value of a generic param is made (a conversion to T through an interface): when the param is a
        // custom struct, the method writes it at a destination that is below.
        bool generic_value_return = is_generic_param(method->get_type()) && env.is_value_mark();

        if (generic_value_return)
            pool.append<x_generic_stack_alloc_xil_t>(__ref_of(ctx, method->get_type()));

        __pre_call_method(ctx, pool, owner, method);
        __compile_argument(ctx, pool, operand, method, 0);

        pool.append<x_method_call_xil_t>(xil_call_type_t::static_, __search_method_ref(ctx, method));

        if (generic_value_return && env.type_mark != type_mark_t::argument)
        {
            pool.append<x_generic_copy_xil_t>(
                __ref_of(ctx, method->get_type()), xil_copy_kind_t::call_ret
            );
        }

        __try_pop_empty_for_method(ctx, pool, method, owner);
    }

    // Compiles type cast expression.
    void __sys_t<type_cast_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool,
                                                        const __environment_t & env)
    {
        if (!is_effective(ctx, this))
            return;

        expression_t * exp = this->expression();
        __FailedWhenNull(exp, _T("expresion empty"));

        type_t * type = this->get_type();
        __FailedWhenNull(type, _T("type empty"));

        type_t * exp_type = exp->get_type();
        __FailedWhenNull(exp_type, _T("type empty"));

        // A conversion that the types declare (explicit operator, implicit operator): in a checked context, the
        // checked one when there is.
        if (method_t * conversion = find_user_conversion(exp_type, type, !this->is_implicit, ctx.checked))
        {
            __compile_conversion_call(ctx, pool, this, exp, conversion, env);
            return;
        }

        // (T)x of a nullable x: the value, it raises if there is none.
        if (type_t * underlying = get_nullable_underlying(exp_type);
                underlying != nullptr && get_nullable_underlying(type) == nullptr)
        {
            __push_nullable_address(ctx, pool, exp);
            __call_nullable_getter(ctx, pool, exp_type, _T("Value"));

            __post_compile_expression(ctx, pool, this, env);
            return;
        }

        // (T?)x: the value is held in a local variable.
        if (get_nullable_underlying(type) != nullptr && get_nullable_underlying(exp_type) == nullptr)
        {
            local_variable_t * temp = ctx.define_temp_local(type);

            __push_variable_address(ctx, pool, temp);
            __try_compile_nullable_value(ctx, pool, type, exp);

            // The destination, that is below, takes the value like the other structs.
            __push_variable_address(ctx, pool, temp);
            pool.append<x_object_copy_xil_t>(__ref_of(ctx, type), xil_copy_kind_t::reverse);
            return;
        }

        // (T)obj, T is a generic param: the value that the object is, or the object itself when T is a reference.
        if (is_generic_param(type) && is_ref_type(exp_type) && !is_generic_param(exp_type))
        {
            exp->compile(ctx, pool);
            pool.append<x_cast_xil_t>(xil_cast_command_t::default_, __ref_of(ctx, type));
            pool.append<x_generic_copy_xil_t>(__ref_of(ctx, type), xil_copy_kind_t::unbox);

            __post_compile_expression(ctx, pool, this, env);
            return;
        }

        if (is_ref_type(type))
        {
            if (is_value_type(exp_type) || is_enum(exp_type))    // Box.
            {
                expression_behaviour_t behaviour = exp->get_behaviour();
                if (behaviour == expression_behaviour_t::new_)
                {
                    exp->compile(ctx, pool, type);
                    pool.append<x_cast_xil_t>(xil_cast_command_t::default_, __ref_of(ctx, type));
                }
                else if (behaviour == expression_behaviour_t::execute &&
                        __is_custom_struct_like(exp_type))
                {
                    pool.append<x_new_xil_t>(__ref_of(ctx, exp_type));
                    pool.append<x_push_duplicate_xil_t>();
                    exp->compile(ctx, pool, type);
                }
                else
                {
                    exp->compile(ctx, pool);
                    pool.append<x_push_box_xil_t>(__ref_of(ctx, exp_type));
                }
            }
            else
            {
                // A generic param is an address: the object that it is, is made.
                if (is_generic_param(exp_type)
                        && (__is_generic_value_variable(exp) || __is_generic_value_field(exp)))
                {
                    if (__is_generic_value_variable(exp))
                    {
                        __compile_variable(ctx, pool, ((name_expression_t *)exp)->variable,
                                                            __environment_t::empty, exp);
                    }
                    else    // A field of an object: this.Value.
                    {
                        __compile_generic_field(ctx, pool, exp);
                    }

                    pool.append<x_generic_copy_xil_t>(
                        __ref_of(ctx, exp_type), xil_copy_kind_t::box
                    );
                }
                else
                {
                    exp->compile(ctx, pool);
                }

                pool.append<x_cast_xil_t>(xil_cast_command_t::default_, __ref_of(ctx, type));
            }
        }
        else    // value type.
        {
            if (is_ref_type(exp_type))      // Unbox.
            {
                if (__is_custom_struct_like(type)) // Unbox custom struct.
                {
                    exp->compile(ctx, pool);

                    // The object of a member (((P)o).V) is the box itself, its data is the
                    // struct; else the struct is copied to the destination that is below.
                    if (__need_object_copy(this))
                    {
                        pool.append<x_object_copy_xil_t>(
                            __ref_of(ctx, type), xil_copy_kind_t::reverse
                        );
                    }
                }
                else    // internal value types.
                {
                    exp->compile(ctx, pool);
                    pool.append<x_push_unbox_xil_t>(__ref_of(ctx, type));
                }
            }
            else if (ctx.checked && __is_numeric_cast(exp_type, type))
            {
                // The value must fit the type: (int)longValue raises OverflowException.
                __int128 value, lo, hi;
                bool overflow = false;

                if (__constant_integer(exp, value, overflow) && __integer_range(type->this_vtype(), lo, hi))
                {
                    if (overflow || value < lo || value > hi)
                    {
                        cpl_log_errorf(ctx.statement_ctx.mctx.logger, exp,
                                            common_log_code_t::conversion_overflow);
                    }
                }

                exp->compile(ctx, pool);
                pool.append<x_push_convert_xil_t>(__xil_type(exp), to_xil_type(type->this_vtype()), true);
            }
            else
            {
                exp->compile(ctx, pool, type);

                // The value is converted to the underlying type of the enum ((Sign)k, an Int32 to an enum of Int64: the
                // number is extended with its sign): __post_compile_expression converts only to a value type.
                if (is_enum(type) && (is_value_type(exp_type) || is_enum(exp_type)))
                    __try_append_convert_xil(pool, exp_type, to_xil_type(type));
            }
        }

        __post_compile_expression(ctx, pool, this, env);
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
                                                        const __environment_t & env)
    {
        return;
    }

    ////////// ////////// ////////// ////////// //////////
    // index_expression_t

    // Executes index expression.
    cvalue_t __sys_t<index_expression_t>::execute()
    {
        return __super_t::execute();
    }
    
    // compiles index expression.
    void __sys_t<index_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool,
                                                    const __environment_t & env)
    {
        expression_t * namex = this->namex();
        if (namex == nullptr)
            __Failed("index main expression missing");

        // A chain that has a member of ?[
        if (this->is_null_conditional_chain_top())
        {
            __compile_null_conditional(ctx, pool, this, env);
            return;
        }

        if (is_effective(ctx, this->parent))
        {
            variable_t * variable = __pick_var((variable_expression_t *)this);
            switch (variable->this_type())
            {
                case variable_type_t::array_index:
                    __compile_array_index(ctx, pool, (array_index_variable_t *)variable,
                                                                            namex, env);
                    break;

                case variable_type_t::property_index: {
                    // The value of a generic param that a getter returns (list[0], where the
                    // items are T): as the call of a method, the getter writes it at a
                    // destination that is below, when the param is a custom struct.
                    type_t * type = this->get_type();
                    bool generic_value_return = is_generic_param(type)
                                                && env.is_value_mark();

                    if (generic_value_return)
                        pool.append<x_generic_stack_alloc_xil_t>(__ref_of(ctx, type));

                    __compile_property_index(ctx, pool, (property_index_variable_t *)variable,
                                                                            namex, env);

                    if (generic_value_return && env.type_mark != type_mark_t::argument)
                    {
                        pool.append<x_generic_copy_xil_t>(
                            __ref_of(ctx, type), xil_copy_kind_t::call_ret
                        );
                    }
                }   break;

                default:
                    X_UNEXPECTED_F("unexpected variable type '%1%'", variable->this_type());
            }
        }
        else
        {
            __compile_arguments(ctx, pool, this->arguments());
        }
    }

    // Returns whether the xil type is a number (not a bool, a char or a reference).
    static bool __is_number_xil_type(xil_type_t type)
    {
        switch (type)
        {
            case xil_type_t::int8:   case xil_type_t::uint8:
            case xil_type_t::int16:  case xil_type_t::uint16:
            case xil_type_t::int32:  case xil_type_t::uint32:
            case xil_type_t::int64:  case xil_type_t::uint64:
            case xil_type_t::float_: case xil_type_t::double_:
                return true;

            default:
                return false;
        }
    }

    // Compiles array index expression.
    void __sys_t<index_expression_t>::__compile_array_index(__cctx_t & ctx, xil_pool_t & pool,
                array_index_variable_t * variable, expression_t * namex,
                const __environment_t & env)
    {
        __compile_arguments(ctx, pool, this->arguments());
        namex->compile(ctx, pool);

        type_t * element_type = __array_element_type_of(ctx, this);
        type_t * array_type = __XPool.new_array_type(
            element_type, variable->dimension()
        );

        ref_t array_type_ref = __ref_of(ctx, array_type);

        if (__is_custom_struct_like(element_type))
        {
            pool.append<x_push_array_element_addr_xil_t>(
                __ref_of(ctx, array_type)
            );

            if (__need_object_copy(this))
            {
                // The element is copied to the destination that is below, or it is loaded into
                // the stack when it is a generic param returned by value.
                xil_copy_kind_t kind = is_generic_param(element_type)
                                            && env.is_value_mark()?
                                            xil_copy_kind_t::ret : xil_copy_kind_t::reverse;

                __append_object_copy_xil(ctx, pool, element_type, kind);
            }
        }
        else
        {
            // The element is converted when it is read, as the value of a variable is: an Int64
            // that is read as a Double ((Double)longs[0], Double d = longs[0]) is a number, not
            // its bits.
            // A Char is read as a number too ((Int32)chars[0]): the native programs need the value of the
            // type of the variable that takes it.
            xil_type_t dtype = __to_xil_type(element_type);
            if (__is_number_xil_type(env.dtype) && (__is_number_xil_type(dtype) || dtype == xil_type_t::char_))
                dtype = env.dtype;

            pool.append<x_push_array_element_xil_t>(dtype, array_type_ref);
        }
    }

    // Compiles property index expression.
    void __sys_t<index_expression_t>::__compile_property_index(__cctx_t & ctx, xil_pool_t & pool,
                property_index_variable_t * variable, expression_t * namex,
                const __environment_t & env)
    {
        // The object is compiled for the host type of the getter, as the object of a method call is: a call that
        // returns a struct (items.Slice(1)[0]) writes it at a destination that is made for it.
        method_base_t * getter = variable->property != nullptr? variable->property->get_method : nullptr;
        type_t * host_type = getter != nullptr? getter->get_host_type() : nullptr;

        // A getter that returns a struct, whose member is read (items[k].X): the struct is written at a destination
        // that is below the object, as the one of a method call is. (The index of a pointer gives the address of the value:
        // the member is the one of the value that is in the memory, there is no destination.)
        bool pointer_address = getter != nullptr && __pointer_op_of(getter) == __pointer_op_t::get_item
                                && __pointer_to_struct(getter->get_host_type()) && __is_pointer_value_of_member(this);

        if (getter != nullptr && !pointer_address)
            __pre_call_method(ctx, pool, this, getter);

        if (host_type != nullptr)
            namex->compile(ctx, pool, host_type);
        else
            namex->compile(ctx, pool);

        __compile_property_index_variable(ctx, pool, variable, env, this);

        // The value of the getter is converted to the type that is wanted ((Int32)s[k], Int64 n = s[k]), as
        // the value of a method call is: the native programs need the type of the variable that takes it.
        if (env.dtype != xil_type_t::empty && variable->property != nullptr
                && variable->property->get_method != nullptr)
        {
            // (A value of a generic param is what the param is when the code runs: it is not converted here.)
            type_t * type = variable->property->get_method->get_type();

            if (type != nullptr && !is_generic_param(type) && is_value_type(type))
            {
                // A number that is taken as an object (Object o = items[k]) is boxed: it is not a conversion of numbers. (The
                // other values, a struct, are copied or loaded as they were.)
                vtype_t vtype = type->this_vtype();
                bool is_number = vtype != vtype_t::void_ && vtype != vtype_t::__default__ && vtype != vtype_t::__unknown__
                                    && to_xil_type(vtype) != xil_type_t::object;

                if (env.dtype == xil_type_t::object && is_number)
                    pool.append<x_push_box_xil_t>(__ref_of(ctx, type));
                else
                    __try_append_convert_xil(pool, type, env.dtype);
            }
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // new_expression_t

    // Executes name expression.
    cvalue_t __sys_t<new_expression_t>::execute()
    {
        return __super_t::execute();
    }
    
    // Returns constructor call type.
    void __check_constructor_calltype(type_t * host_type, method_t * constructor)
    {
        type_t * type = to_type(constructor->type_name);
        if (type != nullptr && !is_void_type(type))
            __Failed("constructor '%1%' should no return type", constructor);

        xil_call_type_t call_type = __call_type_of_method(constructor);
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
    }

    // Compiles new expression.
    void __sys_t<new_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool,
                                                    const __environment_t & env)
    {
        type_t * type = to_type(this->type_name);
        if (type == nullptr)
            __Failed("type missing");

        if (is_generic_param(type))
        {
            __compile_generic_param(ctx, pool, (generic_param_t *)type);
            return;
        }

        ttype_t ttype = type->this_ttype();
        switch (ttype)
        {
            case ttype_t::class_:
                __compile_new_class_object(ctx, pool, type);
                break;

            case ttype_t::struct_:
                // When returns/assigns as an ref type.
                if (env.type != nullptr && is_ref_type(env.type))
                {
                    __compile_new_class_object(ctx, pool, type);
                    type = env.type;
                }
                else
                {
                    __compile_new_struct_object(ctx, pool, type);
                }

                break;

            default:
                __Failed("Cannot create instance of type '%1%', because it's a '%2%' type",
                                                                            type, ttype);
        }

        __post_compile_expression(ctx, pool, this, env, type);
    }

    // Creates instance of class type.
    void __sys_t<new_expression_t>::__compile_new_class_object(__cctx_t & ctx,
                                        xil_pool_t & pool, type_t * type)
    {
        // New xil.
        ref_t type_ref = __ref_of(ctx, type);
        _A(type_ref != ref_t::null);

        pool.append<x_new_xil_t>(type_ref);
        pool.append<x_push_duplicate_xil_t>();

        // Arguments
        if (this->arguments() != nullptr)
        {
            if (this->constructor == nullptr)
                __Failed("constructor of type '%1%' missing", type);

            __compile_arguments(ctx, pool, this->arguments(), this->constructor);
        }


        // Constructor.
        __check_constructor_calltype(type, this->constructor);
        ref_t constructor_ref = __search_method_ref(ctx, this->constructor);
        pool.append<x_method_call_xil_t>(xil_call_type_t::instance, constructor_ref);

        // The members that an object initializer sets: the object is kept in a local variable, the
        // assignments take it from there, and it is the value when it is used.
        if (!this->init_assigns.empty())
        {
            local_variable_t * target = ctx.define_temp_local(type);
            xil::write_assign_xil(ctx, pool, target, xil_type_t::object, false);

            ctx.initializer_targets.push_back(target);
            for (expression_t * assign : this->init_assigns)
                assign->compile(ctx, pool);

            ctx.initializer_targets.pop_back();

            if (is_effective(ctx, this->parent))
                __compile_local_variable(ctx, pool, target);

            return;
        }

        // Pop empty.
        if (!is_effective(ctx, this->parent))
        {
            pool.append<x_pop_empty_xil_t>(xil_type_t::object);
        }
    }

    // Creates instance of struct type.
    void __sys_t<new_expression_t>::__compile_new_struct_object(__cctx_t & ctx,
                                            xil_pool_t & pool, type_t * type)
    {
        bool need_call_constructor = __need_call_constructor(this);

        if (need_call_constructor)
        {
            if (__is_left_member(this))
            {
                local_variable_t * variable = ctx.define_temp_local(type);
                __compile_local_variable(ctx, pool, variable);

                if (is_effective(ctx, this->parent))
                    pool.append<x_push_duplicate_xil_t>();
            }
            else if (this->parent == nullptr)
            {
                // A statement (new S(1, 2);): the struct is made in a temp variable, and nothing uses it.
                local_variable_t * variable = ctx.define_temp_local(type);
                __compile_local_variable(ctx, pool, variable);
            }
        }
        else
        {
            // Used by its members (new S().X): there is no destination, it is a temp variable.
            if (__is_left_member(this))
            {
                local_variable_t * variable = ctx.define_temp_local(type);
                __compile_local_variable(ctx, pool, variable);

                if (is_effective(ctx, this->parent))
                    pool.append<x_push_duplicate_xil_t>();
            }
            else if (this->parent == nullptr)
            {
                local_variable_t * variable = ctx.define_temp_local(type);
                __compile_local_variable(ctx, pool, variable);
            }

            // The struct has no constructor: its memory is zero, like a new object.
            pool.append<x_object_copy_xil_t>(__ref_of(ctx, type), xil_copy_kind_t::zero);
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
            __check_constructor_calltype(type, this->constructor);
            ref_t method_ref = __search_method_ref(ctx, this->constructor);
            pool.append<x_method_call_xil_t>(xil_call_type_t::instance, method_ref);
        }

        /*
        else    // default constructor.
        {
            ref_t type_ref = __ref_of(ctx, type);
            pool.append<x_object_copy_xil_t>(type_ref, xil_copy_kind_t::zero);
        }
        */
    }

    // Creates instance of a generic param type: new T() (the constructor of the actual type is not known when the method
    // is compiled: no virtual machine has the command that makes it yet).
    void __sys_t<new_expression_t>::__compile_generic_param(__cctx_t & ctx, xil_pool_t & pool,
                                                generic_param_t * generic_param)
    {
        __Failed("creating an object of the generic param '%1%' (new %1%()) is not supported", generic_param);
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
                                                        const __environment_t & env)
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

        // The items that are structs: each is written at the address of its place in the array, as the assignment
        // of an item does (the array is kept in a temp variable, the items have no command that stores them).
        if (initializer != nullptr && !initializer->empty() && __is_custom_struct_like(element_type))
        {
            if (((array_type_t *)type)->dimension != 1)
                __Failed("the initializer of an array of structs that has more than one dimension is not supported");

            if (!this_effective)
                return;

            local_variable_t * array = ctx.define_temp_local(type);
            pool.append<x_pop_local_xil_t>(xil_type_t::object, array->identity);

            int32_t index = 0;
            initializer->each_element([&](expression_t * element_exp) -> bool {

                if (element_exp == nullptr)
                    __Failed("index element init expression empty");

                pool.append<x_push_const_xil_t<int32_t>>(xil_type_t::int32, tvalue_t(index++));
                pool.append<x_push_local_xil_t>(xil_type_t::object, array->identity);
                pool.append<x_push_array_element_addr_xil_t>(type_ref);

                element_exp->compile(ctx, pool, element_type);

                // A call returns a generic param: a struct is written at the destination below, else the value is
                // stored there.
                type_t * exp_type = element_exp->get_type();
                if (exp_type != nullptr && is_generic_param(exp_type) && __is_generic_call_result(element_exp))
                    pool.append<x_generic_copy_xil_t>(__ref_of(ctx, exp_type), xil_copy_kind_t::store);

                return true;
            });

            pool.append<x_push_local_xil_t>(xil_type_t::object, array->identity);
            return;
        }

        if (initializer != nullptr && !initializer->empty())
        {
            xil_type_t dtype = __to_xil_type(element_type);

            if (this_effective)
                pool.append<x_array_begin_xil_t>(dtype);

            initializer->each_element([&](expression_t * element_exp) -> bool {

                if (element_exp == nullptr)
                    __Failed("index element init expression empty");

                // TODO: check init expression type.

                // The element is converted to the type of the elements (an Int32 to an Int64, with
                // its sign; a value to an Object, boxed).
                if (__is_custom_struct_like(element_type))
                {
                    element_exp->compile(ctx, pool);
                }
                else if (element_exp->get_type() != nullptr && is_custom_struct(element_exp->get_type())
                            && is_ref_type(element_type) && !is_generic_param(element_type))
                {
                    // A struct that is an item of an array of references (Object[] { p }): it is
                    // boxed, like the assignment of a struct to a reference does.
                    pool.append<x_new_xil_t>(__ref_of(ctx, element_exp->get_type()));
                    pool.append<x_push_duplicate_xil_t>();

                    element_exp->compile(ctx, pool);
                }
                else
                {
                    element_exp->compile(ctx, pool, element_type);
                }

                if (this_effective)
                    pool.append<x_array_element_xil_t>(dtype);  // TODO: struct?

                return true;
            });

            if (this_effective)
                pool.append<x_array_end_xil_t>(dtype);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // tuple_expression_t

    // Executes tuple expression.
    cvalue_t __sys_t<tuple_expression_t>::execute()
    {
        return cvalue_t::nan;
    }

    // Compiles tuple expression: the expression that makes the tuple.
    void __sys_t<tuple_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool,
                                                            const __environment_t & env)
    {
        __FailedWhenNull(this->lowered, _T("tuple expression is not analyzed"));
        this->lowered->compile(ctx, pool, env);
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
                                                            const __environment_t & env)
    {
        type_t * type = to_type(this->type_name);

        if (type == nullptr)
            __Failed("type missing");

        if (is_void_type(type))
            return;

        if (!is_effective(ctx, this->parent))
            return;

        // A custom struct is zero: a local variable that is zero is copied to the destination.
        if (is_custom_struct(type) && !env.is_value_mark())
        {
            local_variable_t * variable = ctx.define_temp_local(type);
            ref_t type_ref = __ref_of(ctx, type);

            pool.append<x_push_local_addr_xil_t>(variable->identity);
            pool.append<x_object_copy_xil_t>(type_ref, xil_copy_kind_t::zero);

            pool.append<x_push_local_addr_xil_t>(variable->identity);

            if (__need_object_copy(this))
                pool.append<x_object_copy_xil_t>(type_ref, xil_copy_kind_t::reverse);

            return;
        }

        switch (type->this_gtype())
        {
            case gtype_t::generic_param: {

                // It is read from a local variable that is zero: the value of a generic param is
                // an address and a copy, whatever the argument is (a struct is not a number).
                local_variable_t * variable = ctx.define_temp_local(type);
                ref_t type_ref = __ref_of(ctx, type);

                pool.append<x_push_local_addr_xil_t>(variable->identity);
                pool.append<x_generic_copy_xil_t>(type_ref, xil_copy_kind_t::zero);

                pool.append<x_push_local_addr_xil_t>(variable->identity);

                if (__need_object_copy(this))
                {
                    pool.append<x_generic_copy_xil_t>(type_ref,
                        env.is_value_mark()? xil_copy_kind_t::ret
                                                            : xil_copy_kind_t::reverse);
                }

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
    void __sys_t<type_of_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool,
                                                        const __environment_t & env)
    {
        type_t * type = to_type(this->type_name);
        __FailedWhenNull(type, "type empty in typeof expression");

        ref_t type_ref = __ref_of(ctx, type);
        pool.append<x_push_object_xil_t>(xil_storage_object_type_t::type_info, type_ref);
    }

    ////////// ////////// ////////// ////////// //////////
    // this_expression_t

    // Executes this expression.
    cvalue_t __sys_t<this_expression_t>::execute()
    {
        return __super_t::execute();
    }
    
    // Compiles this expression.
    void __sys_t<this_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool,
                                                        const __environment_t & env)
    {
        // The target of an object initializer: the object that was made, it is in a local variable.
        if (this->is_new_target())
        {
            if (ctx.initializer_targets.empty())
                __Failed("the target of an object initializer is missing");

            __compile_local_variable(ctx, pool, ctx.initializer_targets.back());
            return;
        }

        if (!__is_this_effective(ctx, this))
            return;

        method_t * method = ctx.statement_ctx.method;
        vtype_t vtype = method->host_type->this_vtype();

        if (vtype == vtype_t::string_)
        {
            // A string is a pointer to its characters, not an object.
            pool.append<x_push_this_xil_t>(xil_type_t::string);
        }
        else if (__is_left_member(this->parent, this))
        {
            pool.append<x_push_this_ref_xil_t>();
        }
        else
        {
            // The this of Ptr and Ptr<T> is the pointer itself.
            if (is_system_value_type(vtype) || vtype == vtype_t::ptr_)
            {
                // The value is read as its type (the memory of a Single is not a Double): the conversion to the type that
                // the expression needs is made by __post_compile_expression, as it is for the variables.
                pool.append<x_push_this_xil_t>(to_xil_type(vtype));
            }
            else if (__is_custom_struct_like(method->host_type))
            {
                pool.append<x_push_this_xil_t>(xil_type_t::ptr);
            }
            else
            {
                pool.append<x_push_this_ref_xil_t>();
            }
        }

        __post_compile_expression(ctx, pool, this, env);
    }

    ////////// ////////// ////////// ////////// //////////
    // base_expression_t

    // Executes base expression.
    cvalue_t __sys_t<base_expression_t>::execute()
    {
        return __super_t::execute();
    }
    
    // Compiles base expression.
    void __sys_t<base_expression_t>::compile(__cctx_t & ctx, xil_pool_t & pool,
                                                            const __environment_t & env)
    {
        if (__is_this_effective(ctx, this))
            pool.append<x_push_this_ref_xil_t>();
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::compile

