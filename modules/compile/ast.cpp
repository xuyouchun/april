
#include <compile.h>

namespace X_ROOT_NS { namespace modules { namespace compile {

    typedef common_log_code_t __c_t;
    typedef code_element_t    __el_t;

    using namespace core;

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(xbranch_value_t)

        // Asts
        X_C(asts,                       _T("asts"))

        // Namespace
        X_C(namespace_,                 _T("namespace"))

        // Multi-name
        X_C(mname,                      _T("mname"))

        // Array type name
        X_C(array_type_name,            _T("array_type_name"))

        // General type name.
        X_C(general_type_name,          _T("general_type_name"))

        // Type name unit.
        X_C(type_name_unit,             _T("type_name_unit"))

        // Uncertain type name.
        X_C(uncertain_type_name,        _T("uncertain_type_name"))

        // Typedef.
        X_C(type_def,                   _T("type_def"))

        // Typedef param.
        X_C(type_def_param,             _T("type_def_param"))

        // Typedef params.
        X_C(type_def_params,            _T("type_def_params"))

        // Typeof
        X_C(type_of,                    _T("type_of"))

        // Expressions.
        X_C(expressions,                _T("expressions"))

        // Argument.
        X_C(argument,                   _T("argument"))

        // Arguments.
        X_C(arguments,                  _T("arguments"))

        // Param.
        X_C(param,                      _T("param"))

        // Params.
        X_C(params,                     _T("params"))

        // Generic param.
        X_C(generic_param,              _T("generic_param"))

        // Generic params.
        X_C(generic_params,             _T("generic_params"))

        // Generic constraint.
        X_C(generic_constraint,         _T("generic_constraint"))

        // Generic constraints.
        X_C(generic_constraints,        _T("generic_constraints"))

        // Generic argument.
        X_C(generic_arg,                _T("generic_arg"))

        // Generic arguments.
        X_C(generic_args,               _T("generic_args"))

        // Decorate.
        X_C(decorate,                   _T("decorate"))

        // Method body.
        X_C(method_body,                _T("method_body"))

        // Statements.
        X_C(statements,                 _T("statements"))

        // Expression statement.
        X_C(expression_st,              _T("expression_st"))

        // Typedef statement.
        X_C(type_def_st,                _T("type_def_st"))

        // Defination statement.
        X_C(defination_st,              _T("defination_st"))

        // Break statement.
        X_C(break_st,                   _T("break_st"))

        // Continue statement.
        X_C(continue_st,                _T("continue_st"))

        // Throw statement.
        X_C(throw_st,                   _T("throw_st"))

        // Goto statement.
        X_C(goto_st,                    _T("goto_st"))

        // Return statement.
        X_C(return_st,                  _T("return_st"))

        // Do...while statement.
        X_C(do_while_st,                _T("do_while_st"))

        // Loop...until statement.
        X_C(loop_until_st,              _T("loop_until_st"))

        // While statement.
        X_C(while_st,                   _T("while_st"))

        // For statement.
        X_C(for_st,                     _T("for_st"))

        // For...each statement.
        X_C(for_each_st,                _T("for_each_st"))

        // If statement.
        X_C(if_st,                      _T("if_st"))

        // Switch statement.
        X_C(switch_st,                  _T("switch_st"))

        // Case statement.
        X_C(case_,                      _T("case_"))

        // Try statement.
        X_C(try_st,                     _T("try_st"))

        // Catch statement.
        X_C(catch_,                     _T("catch_"))

        // Statement group statement.
        X_C(statement_group_st,         _T("statement_group_st"))

        // Typename expression.
        X_C(type_name_exp,              _T("type_name_exp"))

        // Type cast expression.
        X_C(type_cast_exp,              _T("type_cast_exp"))

        // Function calling expression.
        X_C(function,                   _T("function"))

        // Index expression.
        X_C(index,                      _T("index"))

        // New expression.
        X_C(new_,                       _T("new"))

        // Array initializer.
        X_C(array_initializer,          _T("array_initializer"))

        // Array lengths.
        X_C(array_lengths,              _T("array_lengths"))

        // New array expression.
        X_C(new_array,                  _T("new_array"))

        // Default value expression.
        X_C(default_value,              _T("default_value"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Ast error codes.
    X_ENUM_INFO(ast_error_code_t)

        // Multi-name empty.
        X_C(mname_empty, _T("mname_empty"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(common_log_code_t)

        // Duplicated.
        X_D(duplicate,  _T("duplicate %1% \"%2%\""))

        // Name empty.
        X_D(name_empty, _T("%1% name missing"))

        // Unexpected first char in a name.
        X_D(name_unexpected_first_char,
                _T("unexpected first character \"%1%\" in name \"%2%\""))

        // Unexpected char in a name.
        X_D(name_unexpected_char,
                _T("unexpected character \"%1%\" in name \"%2%\""))

        // Assembly name missing.
        X_D(assembly_name_missing, _T("package name missing"))

        // Namespace name missing.
        X_D(namespace_name_missing, _T("namespace missing"))

        // Type name missing.
        X_D(type_name_missing,  _T("%1% type name missing"))

        // Argument missing.
        X_D(argument_missing,   _T("argument missing"))

        // Condition missing.
        X_D(condition_missing,  _T("%1% condition missing"))

        // Argument type error.
        X_D(argument_type_error,    _T("%1% argument type must be variable"))

        // Unexpected param in default value.
        X_D(unexpected_param_default_value, _T("%1%"))

        // Body missing.
        X_D(body_missing,  _T("%1% body missing"))

        // Iterator missing.
        X_D(iterator_missing, _T("%1% iterator missing"))

        // Statement missing.
        X_D(statement_missing, _T("statement missing"))

        // Expression missing.
        X_D(expression_missing, _T("%1% expression missing"))

        // Catch or finally missing.
        X_D(catch_or_finally_missing, _T("try and finally blocks are both empty"))

        // Array lengths missing.
        X_D(array_lengths_missing, _T("array '%1%' length missing"))

        // Unexpected params.
        X_D(unexpected_params, _T("unexpected %1% params"))

        // Generic arguments redundance in function.
        X_D(function_generic_args_redundance, _T("function generic arguments redundance"))

        // Unexpected variable defination.
        X_D(unexpected_variable_defination, _T("variables cannot be defined here: \"%1%\""))

        // Unexpected field defination.
        X_D(unexpected_field_defination, _T("fields cannot be defined here: \"%1%\""))

        // Unexpected property defination.
        X_D(unexpected_property_defination, _T("properties cannot be defined here: \"%1%\""))

        // A constant variable require a value to be provided.
        X_D(constant_variable_initialize_missing,
                            _T("A constant variable \"%1%\" require a value to be provided"))

        // The expression beging assigned to constant variable must be constant.
        X_D(constant_variable_required_constant_value,
                            _T("The expression beging assigned to \"%1%\" must be constant."))

        // Unexpected param defination.
        X_D(unexpected_param_defination, _T("parameters cannot be define hered: \"%1%\""))

        // Unexpected import.
        X_D(unexpected_import, _T("unexpected import \"%1%\""))

        // A reference of itself.
        X_D(assembly_reference_itself, _T("assembly \"%1%\" reference to itself"))

        // Assembly circular reference.
        X_D(assembly_circular_reference, _T("assembly \"%1%\" circle reference"))

        // Assembly not found.
        X_D(assembly_not_found, _T("assembly \"%1%\" not found"))

        // Unexpected using namespace.
        X_D(unexpected_using_namespace, _T("unexpected using namespace \"%1%\""))

        // Variable defination duplicated.
        X_D(variable_defination_duplicate, _T("variable \"%1%\" duplicate"))

        // Variable undefined.
        X_D(variable_undefined, _T("variable \"%1%\" undefined"))

        // Type undefined.
        X_D(type_undefined, _T("type \"%1%\" undefined"))

        // Typedef unsolved.
        X_D(type_def_unsolved, _T("typedef \"%1%\" unsolved"))

        // Type defination duplicated.
        X_D(type_defination_duplicate, _T("type \"%1%\" defination duplicate"))

        // Typedef defination duplicated.
        X_D(type_def_defination_duplicate, _T("typedef \"%1%\" defination duplicate"))

        // Type undetermind.
        X_D(type_undetermind, _T("type undetermind for \"%1%\""))

        // Unexpected symbol.
        X_D(unexpected_symbol, _T("unexpected symbol, \"%1%\""))

        // Unexpected expression.
        X_D(unexpected_expression, _T("unexpected expression, \"%1%\""))

        // Member not found.
        X_D(member_not_found, _T("member \"%1%\" undefined in type \"%2%\""))

        // Method not found.
        X_D(method_not_found, _T("method \"%1%\" undefined in type \"%2%\""))

        // Method not match.
        X_D(method_not_match, _T("no matching member function for call to \"%1%\" in type \"%2%\""))

        // Attribute constructor not match.
        X_D(attribute_constructor_not_match, _T("no matching attribute \"%1%\" constructor: (%2%)"))

        // Type consturctor not matched.
        X_D(type_constructor_not_match, _T("no matching type \"%1%\" constructor: (%2%)"))

        // Method conflict.
        X_D(method_conflict, _T("method \"%1%\" conflict in type \"%2%\": %3%"))

        // Not all code paths return a value.
        X_D(method_no_return, _T("method \"%1%\": not all code paths return a value"))

        // The return keyword must not be followed by an expression in void method.
        X_D(method_unexpected_return_value, _T("method \"%1%\": The return keyword must not ")
                                            _T("be followed by an expression in void method"))

        // The return keyword must followed by an expression in non-void method.
        X_D(method_return_value_missing, _T("method \"%1%\": The return keyword must followed ")
                                         _T("by an expression in non-void method."))

        X_D(method_incompatible_return_value, _T("method \"%1%: Incompatible return type."))

        // Property index undetermind.
        X_D(property_index_undetermind, _T("property index \"%1%\" undetermind"))

        // Property method body missing.
        X_D(property_method_body_missing, _T("property \"%1%\" must declared a body"))

        // Property or indexer must have at least one accessor.
        X_D(property_accessor_missing, _T("property or indexer \"%1%\" must have at least ")
                                       _T("one accessor."))

        // Constructor method should no return type.
        X_D(constructor_method_should_no_return_type,
                                _T("constructor method %1% should no return type"))

        // Destructor method should no return type.
        X_D(destructor_method_should_no_return_type,
                                _T("destructor method %1% should no return type"))

        // Static constructor method should no return type.
        X_D(static_constructor_method_should_no_return_type,
                                _T("static constructor method %1% should no return type"))

        // Destructor method should no params.
        X_D(destructor_method_should_no_params,
                                _T("destructor method %1% should no params"))

        // Static constructor method should no params.
        X_D(static_constructor_method_should_no_params,
                                _T("static constructor method %1% should no params"))

        // Constructor method not found.
        X_D(constructor_method_not_found,
                                _T("constructor method %1%(%2%) not found"))

        // Partial specialization not supported.
        X_D(partial_specialization_not_supported,
                                _T("partial specialization of generic type not supported"))

        // No base type.
        X_D(no_base_type, _T("type '%1%' has no base type"))

        // Expected array length.
        X_D(expected_array_length, _T("expected array length"))

        // Array initializer error.
        X_D(array_initializer_error, _T("array initializer error: %1%"))

        // Invalid initialize value.
        X_D(invalid_initialize_value, _T("Invalid initialize value \"%1%\""))

        // Operator overload prototype error.
        X_D(operator_overload_prototype_error, _T("Must be declared public and static"))

        // Operator overload prototype error: wrong param count.
        X_D(unitary_operator_overload_wrong_param_count,
                _T("Overloaded unitary operator '%1%' takes one parameter"))

        X_D(binary_operator_overload_wrong_param_count,
                _T("Overloaded binary operator '%1%' takes two parameters"))

        // One of the parameters must be the containing type.
        X_D(unitary_operator_overload_wrong_containing_type,
                _T("The parameter of unitary operator '%1%' overloaded must be the containing type"))

        // The parameter must be the containing type.
        X_D(binary_operator_overload_wrong_containing_type,
                _T("One of the parameters of binary operator '%1%' overloaded must be the containing type"))


        // User-defined operators cannot return void.
        X_D(operator_overload_cannot_return_void, _T("User-defined operators cannot return void"))

        // Operator cannot ber overloaded.
        X_D(operator_cannot_be_overloaded, _T("Operator '%1%' cannot be overloaded"))

        // Operator overload not defined.
        X_D(operator_overload_not_defined, _T("Operator '%1%' not defined for %2%"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // common functions

    // Returns whether it's a compile time attribute.
    static bool __is_compile_time_attribute(attribute_t * attr)
    {
        general_type_name_t * type_name = as<general_type_name_t *>(attr->type_name);
        return type_name && type_name->equals(JC_COMPILE_ATTRIBUTE_NAME);
    }

    ////////// ////////// ////////// ////////// //////////
    // module

    // Walk this node.
    void __module_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                __super_t::on_walk(context, step, tag);
                break;

            case walk_step_t::post_confirm: {
                eobject_commit_context_t ctx(context.xpool, context.logger);
                __module->commit(ctx);
                context.xpool.commit_types(context.logger);
            }   break;

            default: break;
        }
    }

    // Walk this node.
    void __module_ast_node_t::do_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::do_walk(context, step, tag);
        context.delay(this, (int)walk_step_t::post_confirm, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // mname

    // Appends a part.
    void mname_ast_node_t::append_part(name_t name, __el_t * el)
    {
        this->__validate_name(name, el);
        __this_mname()->append_part(name);
    }

    // Commits this node.
    void mname_ast_node_t::on_commit()
    {
        __this_mname()->commit(__global_context().xpool.spool);
        const mname_t * mname =  mname_t::to_internal(
            to_mname_operate_context(__global_context().xpool), __this_mname()
        );

        __this_mname()->~mname_t();
        new ((void *)__emname) emname_t(mname);
    }

    // Returns this eobject.
    emname_t * mname_ast_node_t::to_eobject()
    {
        return __this_emname();
    }

    // Walks this node.
    void mname_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // expressions

    // Commits this node.
    void expressions_ast_node_t::on_commit()
    {
        this->transform_child_to(expressions, __expressions);
    }

    // Returns this eobject.
    expression_t * expressions_ast_node_t::to_eobject()
    {
        if (__expressions.expression_count() == 1)
        {
            expression_t * exp = __expressions.expression_at(0);
            return exp->parent = nullptr, exp;
        }

        return &__expressions;
    }

    // Walk this node.
    void expressions_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // type_name

    // Commits this node.
    void type_name_ast_node_t::on_commit()
    {
        this->__check_empty(to_eobject(), this, __c_t::type_name_missing, _T(""));
    }

    // Returns this eobject.
    type_name_t * type_name_ast_node_t::to_eobject()
    {
        return __to_eobject<type_name_t *>(underlying);
    }

    // Walks this node.
    void type_name_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////

    // Global types
    X_ENUM_INFO(global_type_t)

        // Root. e.g. System.Text
        X_C(root,       _T("root"))

        // Global, e.g. ::System.Text
        X_C(global,     _T("global"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Converts to string.
    type_name_unit_t::operator string_t() const
    {
        if (args == nullptr)
            return _str(name);

        return _F(_T("%1%<%2%>"), _str(name), _str(args));
    }

    // Converts to string.
    X_DEFINE_TO_STRING(general_type_name_t)
    {
        return al::join_str(units.begin(), units.end(), _T("."),
            [](auto it) { return _str(it); }
        );
    }

    // Converts to string.
    X_DEFINE_TO_STRING(array_type_name_t)
    {
        stringstream_t ss;
        ss << _str(element_type_name) << _T("[");

        int index = 0;
        for (expression_t * exp : dimensions)
        {
            if (index++ > 0)
                ss << _T(",");

            if (exp != 0)
                ss << _str(exp);
        }

        ss << _T("]");

        return ss.str();
    }

    ////////// ////////// ////////// ////////// //////////
    // type_name_unit_t

    // Returns whether it's partial specialization.
    template<typename args_t>
    bool __is_partial_specialization(const args_t & args)
    {
        enum { unknown, yes, no } is_empty = unknown;

        if (args.size() <= 1)
            return false;

        for (generic_arg_t * arg : args)
        {
            if (is_empty == unknown)
            {
                is_empty = (arg? yes : no);
            }
            else if ((arg? yes : no) != is_empty)
            {
                return true;
            }
        }

        return false;
    }

    // Returns whether it's partial specialization.
    bool type_name_unit_t::is_partial_specialization() const
    {
        return args != nullptr && __is_partial_specialization(*args);
    }

    ////////// ////////// ////////// ////////// //////////
    // general_type_name

    // Sets global type.
    void general_type_name_ast_node_t::set_global_type(global_type_t global_type)
    {
        __type_name.global_type = global_type;
    }

    // Commits this node.
    void general_type_name_ast_node_t::on_commit()
    {
        this->transform_child_to(units, __type_name.units);
    }

    // Returns this eobject.
    type_name_t * general_type_name_ast_node_t::to_eobject()
    {
        return &__type_name;
    }

    // Walks this node.
    void general_type_name_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                __super_t::on_walk(context, step, tag);
                this->__delay(context, walk_step_t::confirm);
                break;

            case walk_step_t::confirm:
                __walk_confirm(context);
                break;

            default: break;
        }
    }

    // Walks confirm step.
    void general_type_name_ast_node_t::__walk_confirm(ast_walk_context_t & context)
    {
        if (__type_name.type == nullptr)
        {
            __type_name.type = __ascertain_type(context, &__type_name);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // type_name_unit

    // Sets name.
    void type_name_unit_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__type_name_unit.name, name, el, _T("type"));
    }

    // Commits this node.
    void type_name_unit_ast_node_t::on_commit()
    {
        __type_name_unit.args = __to_eobject<generic_args_t *>(generic_args);
    }

    // Returns this eobject.
    type_name_unit_t * type_name_unit_ast_node_t::to_eobject()
    {
        return &__type_name_unit;
    }

    // Walks this node.
    void type_name_unit_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__type_name_unit.name, this, __c_t::name_empty, _T("type name unit"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // array_type_name

    // Commits this node.
    void array_type_name_ast_node_t::on_commit()
    {
        this->transform_child_to(dimensions, __type_name.dimensions);

        __type_name.element_type_name = __to_eobject<type_name_t *>(element_type_name);
    }

    // Returns this eobject.
    type_name_t * array_type_name_ast_node_t::to_eobject()
    {
        return &__type_name;
    }

    // Walks this node.
    void array_type_name_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                this->__check_empty(__type_name.element_type_name, this,
                                    __c_t::type_name_missing, _T("array"));
                __super_t::on_walk(context, step, tag);
                this->__delay(context, walk_step_t::confirm);
                break;

            case walk_step_t::confirm:
                __walk_confirm(context);
                break;

            default: break;
        }
    }

    // Walks confirm step.
    void array_type_name_ast_node_t::__walk_confirm(ast_walk_context_t & context)
    {
        array_type_name_t * type_name = &__type_name;

        if (type_name->element_type_name != nullptr && type_name->type == nullptr)
        {
            type_name->type = __ascertain_type(context, type_name);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // uncertain_type_name

    // Commits the node.
    void uncertain_type_name_ast_node_t::on_commit() { }

    // Returns this eobject.
    type_name_t * uncertain_type_name_ast_node_t::to_eobject()
    {
        return __type_name;
    }

    // Walks this node.
    void uncertain_type_name_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // type_def_param

    // Sets name.
    void type_def_param_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__type_def_param.name, name, el, _T("typedef param"));
    }

	// Sets if it's a extends type.
	void type_def_param_ast_node_t::set_extends(bool extends)
	{
		__type_def_param.extends = extends;
	}

    // Commits this node.
    void type_def_param_ast_node_t::on_commit()
    {

    }

    // Returns this eobject.
    type_def_param_t * type_def_param_ast_node_t::to_eobject()
    {
        return &__type_def_param;
    }

    // Walks this node.
    void type_def_param_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__type_def_param.name, _T("typedef param"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // type_def_params

    // Commits this node.
    void type_def_params_ast_node_t::on_commit()
    {
        this->transform_child_to(params, __type_def_params);
    }

    // Returns this eobject.
    type_def_params_t * type_def_params_ast_node_t::to_eobject()
    {
        return &__type_def_params;
    }

    // Walks this node.
    void type_def_params_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // type_def

    // Sets name.
    void type_def_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__type_def.name, name, el, _T("typedef"));
    }

    // Commits this node.
    void type_def_ast_node_t::on_commit()
    {
        __type_def.type_name  = __to_eobject<type_name_t *>(type_name);
        __type_def.params     = __to_eobject<type_def_params_t *>(params);
    }

    // Returns this eobject.
    type_def_t * type_def_ast_node_t::to_eobject()
    {
        return &__type_def;
    }

    // Walks this node.
    void type_def_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        context.push(to_eobject());

        __type_def.namespace_ = context.current_namespace();

        this->__check_empty(__type_def.name, this, __c_t::type_name_missing, _T("typedef"));

        const mname_t * ns = ns_to_full_name(context.current_namespace());

        type_def_t * type_def = to_eobject();
        if (!context.assembly.types.append_type_def(to_sid(ns), type_def))
            this->__log(this, __c_t::type_def_defination_duplicate, type_def);

        __super_t::on_walk(context, step, tag);
        context.pop();
    }

    ////////// ////////// ////////// ////////// //////////
    // type_of

    // Commits this node.
    void type_of_ast_node_t::on_commit()
    {
        __type_of.type_name = __to_eobject<type_name_t *>(type_name);
    }

    // Returns this eobject.
    expression_t * type_of_ast_node_t::to_eobject()
    {
        return &__type_of;
    }

    // Walks this node.
    void type_of_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__type_of.type_name, this, __c_t::type_name_missing, _T("typeof"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // argument

    // Sets name.
    void argument_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__argument.name, name, el, _T("argument"));
    }

    // Sets argument type.
    void argument_ast_node_t::set_argument_type(param_type_t atype, __el_t * el)
    {
        this->__assign(__argument.atype, atype, el, _T("argument type"));
    }

    // Commits this node.
    void argument_ast_node_t::on_commit()
    {
        __argument.expression = __to_eobject<expression_t *>(expression);
        __argument.attributes = __to_eobject_with_owner<attributes_t *>(attributes, &__argument);
    }

    // Returns this eobject.
    argument_t * argument_ast_node_t::to_eobject()
    {
        return &__argument;
    }

    // Walks this node.
    void argument_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);

        this->__check_empty(__argument.expression, this, __c_t::argument_missing);

        typedef param_type_t t;
        if (__argument.atype == t::ref || __argument.atype == t::out)
        {
            if (__argument.expression != nullptr
                    && __argument.expression->this_family() != expression_family_t::name)
            {
                this->__log(this, __c_t::argument_type_error, _title(__argument.atype));
            }
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // arguments

    // Commits this node.
    void arguments_ast_node_t::on_commit()
    {
        this->transform_child_to(arguments, __arguments);
    }

    // Walks this node.
    void arguments_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    // Returns this eobject.
    arguments_t * arguments_ast_node_t::to_eobject()
    {
        return &__arguments;
    }

    ////////// ////////// ////////// ////////// //////////
    // cvalue

    // Walks this node.
    void cvalue_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    // Returns this eobject.
    cvalue_t * cvalue_ast_node_t::to_eobject()
    {
        return &__value;
    }

    // Sets value.
    void cvalue_ast_node_t::set_value(const cvalue_t & value)
    {
        __value = value;
    }

    // Sets value.
    void cvalue_ast_node_t::set_value(const token_data_t & data)
    {
        switch (data.value_type)
        {
            case cvalue_type_t::number:
                set_value(cvalue_t(*(tvalue_t *)data.value));
                break;

            case cvalue_type_t::string:
                set_value(cvalue_t((const char_t *)data.value));
                break;

            case cvalue_type_t::null:
                set_value(cvalue_t::null);
                break;
            
            default:
                break;
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // decorate

    // Bit field wrapper.
    template<typename getf_t, typename setf_t>
    struct __bit_field_wrap_t
    {
        __bit_field_wrap_t(getf_t getf, setf_t setf) : __getf(getf), __setf(setf) { }

        getf_t __getf;
        setf_t __setf;

        typedef decltype(__getf()) __value_t;

        operator __value_t() const { return __getf(); }
        __value_t operator = (const __value_t & value) { __setf(value); return value; }
        bool operator == (const __value_t & value) const { return (__value_t)*this == value; }
        bool operator != (const __value_t & value) const { return !operator == (value); }
    };

    // Bit field wrapper.
    template<typename getf_t, typename setf_t>
    auto __bit_field_wrap(getf_t getf, setf_t setf)
    {
        return __bit_field_wrap_t<getf_t, setf_t>(getf, setf);
    }

    #define __BitField(obj, field)                              \
        __bit_field_wrap(                                       \
                [this]() { return obj.field; },                 \
                [this](auto value) { obj.field = value; }       \
        )

    // Walks this node.
    void decorate_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    // Sets access.
    void decorate_ast_node_t::set_access(access_value_t value, __el_t * el)
    {
        this->__assign(__BitField(__decorate, access), value, el, _T("access"));
    }

    // Sets static.
    void decorate_ast_node_t::set_static(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_static), true, el, _T("static"));
    }

    // Sets sealed.
    void decorate_ast_node_t::set_sealed(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_sealed), true, el, _T("sealed"));
    }

    // Sets override.
    void decorate_ast_node_t::set_override(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_override), true, el, _T("override"));
    }

    // Sets virtual.
    void decorate_ast_node_t::set_virtual(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_virtual), true, el, _T("virtual"));
    }

    // Sets abstract.
    void decorate_ast_node_t::set_abstract(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_abstract), true, el, _T("abstract"));
    }

    // Sets new.
    void decorate_ast_node_t::set_new(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_new), true, el, _T("new"));
    }

    // Sets readonly.
    void decorate_ast_node_t::set_readonly(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_readonly), true, el, _T("readonly"));
    }

    // Sets constant.
    void decorate_ast_node_t::set_const(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_const), true, el, _T("const"));
    }

    // Sets extern.
    void decorate_ast_node_t::set_extern(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_extern), true, el, _T("extern"));
    }

    // Commits this node.
    void decorate_ast_node_t::on_commit()
    {

    }

    // Returns this eobject.
    decorate_t * decorate_ast_node_t::to_eobject()
    {
        return &__decorate;
    }

    ////////// ////////// ////////// ////////// //////////
    // statements

    // Commits this node.
    void statements_ast_node_t::on_commit()
    {
        this->transform_child_to(statements, __statements);
    }

    // Returns this eobject.
    statements_t * statements_ast_node_t::to_eobject()
    {
        return &__statements;
    }

    // Walks this node.
    void statements_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // method_body

    // Commits this node.
    void method_body_ast_node_t::on_commit()
    {
        this->transform_child_to(statements, __body.statements);
    }

    // Returns this eobject.
    method_body_t * method_body_ast_node_t::to_eobject()
    {
        return &__body;
    }

    // Walks this node.
    void method_body_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                __super_t::on_walk(context, step, tag);
                this->__delay(context, walk_step_t::post_analysis);
                break;

            case walk_step_t::post_analysis:
                __walk_post_analysis(context);
                break;

            default: break;
        }
    }

    // Walks post analysis step.
    void method_body_ast_node_t::__walk_post_analysis(ast_walk_context_t & context)
    {
        typedef statement_exit_type_t       __exit_type_t;
        typedef enum_t<__exit_type_t>       __e_exit_type_t;

        statement_exit_type_context_t ctx(this->__get_xpool());
        __e_exit_type_t exit_type = exit_type_of(ctx, &__body.statements);

        // method_t * method = context.current_method();
        // _PF(_T("%1%: %2%"), method, _eflags(exit_type));

        if (exit_type.has(__exit_type_t::pass) || exit_type == __exit_type_t::none)
        {
            method_t * method = context.current_method();
            _A(method != nullptr);

            if (method->type_name == nullptr || is_void_type(method->type_name->type))
            {
                __body.push_back(__new_obj<return_statement_t>());
            }
            else
            {
                this->__log(this, __c_t::method_no_return, method);
            }
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // expression

    // Appends token.
    void expression_ast_node_t::append_token(token_t * token)
    {
        _A(token != nullptr);

        __es.push_back(__el_t(token));
    }

    // Appends name.
    void expression_ast_node_t::append_name(name_t name, token_t * token)
    {
        __es.push_back(__el_t(token, name));
    }

    // Appends expression.
    void expression_ast_node_t::append_expression(ast_node_t * node)
    {
        _A(node != nullptr);

        expression_t * expression = _M(expression_ast_t *, node)->to_eobject();
        __es.push_back(__el_t(expression));

        this->append_child(nodes, node);
    }

    // Appends constant value.
    void expression_ast_node_t::append_cvalue(ast_node_t * node)
    {
        _A(node != nullptr);

        cvalue_t * value = _M(cvalue_ast_t *, node)->to_eobject();
        __es.push_back(__el_t(to_sys_exp(__get_memory(), __get_lang(), nullptr, value)));

        this->append_child(nodes, node);
    }

    // Commits this node.
    void expression_ast_node_t::on_commit()
    {
        // TODO: why parse twice?
        __expression = __parse_expression(__es);
    }

    // Returns this eobject.
    expression_t * expression_ast_node_t::to_eobject()
    {
        return __expression;
    }

    // Walks this node.
    void expression_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                if (is_top_expression(__expression))
                    this->__delay(context, walk_step_t::analysis);
                __super_t::on_walk(context, step, tag);
                break;

            case walk_step_t::analysis: {
                expression_t * exp = this->to_eobject();
                if (!exp->walked)
                    walk_expression(this->__context, context, exp);
            }   break;

            default: break;
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // statement

    // Commits this node.
    void statement_ast_node_t::on_commit()
    {
        if (this->child_at(underlying) == nullptr)
            this->__log(this, __c_t::statement_missing);
    }

    // Returns this eobject.
    statement_t * statement_ast_node_t::to_eobject()
    {
        return __to_eobject<statement_t *>(underlying);
    }

    // Walks this node.
    void statement_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // import

    // Sets alias.
    void import_ast_node_t::set_alias(name_t alias, __el_t * el)
    {
        this->__assign_name(__import.alias, alias, el, _T("import alias"));
    }

    // Commits this node.
    void import_ast_node_t::on_commit()
    {
        __import.package  = __to_eobject<emname_t *>(package);
        __import.assembly = __to_eobject<emname_t *>(assembly);
    }

    // Returns this eobject.
    import_t * import_ast_node_t::to_eobject()
    {
        return &__import;
    }

    // Walks this node.
    void import_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        import_t * import = to_eobject();

        if (!__import.assembly)
        {
            this->__log(this, __c_t::assembly_name_missing);
        }
        else
        {
            document_t * document = context.current_document();
            if (document == nullptr)
            {
                this->__log(this, __c_t::unexpected_import, _str(import));
            }
            else
            {
                #define __LogAssembly(code) \
                    this->__log(this, __c_t::code, _str(import->assembly))
                    
                if (!import->package && context.assembly.name == to_mname(import->assembly))
                {
                    __LogAssembly(assembly_reference_itself);
                }
                else
                {
                    document->add_import(import);

                    try
                    {
                        assembly_t * assembly = context.load_assembly(
                            to_mname(import->package), to_mname(import->assembly)
                        );

                        _A(assembly != nullptr);
                        context.assembly.references.add_reference(
                            to_mname(import->package), assembly
                        );
                    }
                    catch(const logic_error_t<compile_error_code_t> & e)
                    {
                        switch (e.code)
                        {
                            case compile_error_code_t::assembly_circular_reference:
                                __LogAssembly(assembly_circular_reference);
                                break;

                            case compile_error_code_t::assembly_not_found:
                                __LogAssembly(assembly_not_found);
                                break;

                            default:
                                break;
                        }
                    }
                }

                #undef __LogAssembly
            }
        }

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // using

    // Sets alias.
    void using_ast_node_t::set_alias(name_t name, __el_t * el)
    {
        this->__validate_name(name, el);
        __using_namespace.alias = name;
    }

    // Commits this node.
    void using_ast_node_t::on_commit()
    {
        __using_namespace.ns = __to_eobject<emname_t *>(ns);
    }

    // Returns this eobject.
    using_namespace_t * using_ast_node_t::to_eobject()
    {
        return &__using_namespace;
    }

    // Walks this node.
    void using_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if (!__using_namespace.ns)
        {
            this->__log(this, __c_t::namespace_name_missing);
        }
        else
        {
            document_t * document = context.current_document();
            if (document == nullptr)
            {
                this->__log(this, __c_t::unexpected_using_namespace, _str(this->to_eobject()));
            }
            else
            {
                document->add_using(this->to_eobject());
            }
        }

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // namespace

    // Commits this node.
    void namespace_ast_node_t::on_commit()
    {
        __namespace.name = __to_eobject<emname_t *>(name);

        this->transform_child_to(types,      __namespace.types);
        this->transform_child_to(namespaces, __namespace.namespaces);
        this->transform_child_to(type_defs,  __namespace.type_defs);
    }

    // Returns this eobject.
    namespace_t * namespace_ast_node_t::to_eobject()
    {
        return &__namespace;
    }

    // Walks this node.
    void namespace_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        context.push(this->to_eobject());
        __super_t::on_walk(context, step, tag);
        context.pop();
    }

    ////////// ////////// ////////// ////////// //////////

    // Method utils.
    class __method_utils_t : public __utils_t
    {
        typedef __utils_t __super_t;
        typedef al::svector_t<statement_t *, 16> __statements_t;

    public:

        // Constructor.
        __method_utils_t(ast_context_t & ctx, ast_walk_context_t & wctx)
            : __super_t(ctx), __wctx(wctx)
        { }

        // Revise constructor.
        void revise_constructor(method_t & method, arguments_t * args, __el_t * node = nullptr)
        {
            method_t * base_ctor = __get_base_contructor(args, node);
            __statements_t statements;

            // super ctor
            if (base_ctor != nullptr)
            {
                statements.push_back(__new_expression_statement<function_expression_t>(
                    base_ctor, args
                ));
            }

            // Initialize fields.
            __init_fields(statements, method);

            // Appends to method body.
            if (statements.size() > 0)
            {
                if (method.body == nullptr)
                    method.body = __new_obj<method_body_t>();

                method.body->push_front(statements);
            }
        }

    private:
        ast_walk_context_t & __wctx;

        // Returns base constructor.
        method_t * __get_base_contructor(arguments_t * args, __el_t * node)
        {
            type_t * type = __wctx.current_type();
            _A(type != nullptr);

            type_t * base_type = type->get_base_type();

            if (base_type == nullptr)
            {
                if (!is_empty(args))
                    this->__log(node, __c_t::constructor_method_not_found, _T(">"), args);

                return nullptr;
            }

            method_t * ctor = find_constructor(this->__context, base_type, args);
            if (ctor == nullptr)
                this->__log(node, __c_t::constructor_method_not_found, base_type, args);

            return ctor;
        }

        // Initialize fields.
        void __init_fields(__statements_t & statements, method_t & method)
        {
            // fields initialize
            type_t * type = __wctx.current_type();
            _A(type != nullptr);

            analyze_members_args_t members_args(member_type_t::field, name_t::null);
            type->get_members(members_args);

            for (member_t * member : members_args.out_members)
            {
                field_t * field = (field_t *)member;
                expression_t * init_exp = field->init_value;
                type_t * field_type = to_type(field->type_name);

                if (field_type != nullptr && __check_default_value(field_type, init_exp))
                {
                    statements.push_back(__new_expression_statement<binary_expression_t>(
                        operator_t::assign, __new_field_expression(field), init_exp
                    ));
                }
            }
        }

        // Returns whether it's a default value of specified type.
        bool __check_default_value(type_t * type, expression_t * exp)
        {
            _A(type != nullptr);

            if (exp == nullptr)
                return false;

            cvalue_t cv = this->__execute_expression(exp);
            if (is_nan(cv))
                throw _ED(__c_t::invalid_initialize_value, exp);

            cvalue_t def_cv = default_value_of(type->this_vtype());
            return cv != def_cv;
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // type

    // Sets name.
    void type_ast_node_t::set_name(const name_t & name, __el_t * el)
    {
        this->__assign_name(__type.name, name, el, _T("type"));
    }

    // Sets ttype.
    void type_ast_node_t::set_ttype(ttype_t ttype, __el_t * el)
    {
        __type.ttype = ttype;
    }

    // Appends member.
    void type_ast_node_t::append_member(member_t * member)
    {
        __type.append_member(member);
    }

    // Commits this node.
    void type_ast_node_t::on_commit()
    {
        __type.params     = __to_eobject<generic_params_t *>(generic_params);
        __type.decorate   = __to_eobject<decorate_t *>(decorate);
        __type.attributes = __to_eobject_with_owner<attributes_t *>(attributes, &__type);

        this->transform_child_to(methods,           __type.methods);
        this->transform_child_to(properties,        __type.properties);
        this->transform_child_to(fields,            __type.fields);
        this->transform_child_to(events,            __type.events);
        this->transform_child_to(nest_types,        __type.nest_types);
        this->transform_child_to(type_defs,         __type.type_defs);
        this->transform_child_to(super_type_names,  __type.super_type_names);
    }

    // Returns this eobject.
    type_t * type_ast_node_t::to_eobject()
    {
        return &__type;
    }

    // Walks this node.
    void type_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                context.push(this->to_eobject());
                __walk_default(context, step, tag);
                context.pop();
                break;

            case walk_step_t::analysis:
                __walk_analysis(context, (method_t *)tag);
                break;

            default: break;
        }
    }

    // Walks default step.
    void type_ast_node_t::__walk_default(ast_walk_context_t & context, int step, void * tag)
    {
        general_type_t * type = (general_type_t *)to_eobject();
        mname_operate_context_t mctx = to_mname_operate_context(__global_context().xpool);
        type->namespace_ = context.current_namespace();

        if (!context.assembly.types.append_type(type))
            this->__log(this, __c_t::type_defination_duplicate, type);

        context.xpool.append_new_type(type);

        __super_t::on_walk(context, step, tag);

        bool has_constructor = al::any_of(__type.methods, [](method_t * m) {
            return m->trait == method_trait_t::constructor;
        });

        if (!has_constructor && al::in(type->this_gtype(), gtype_t::general, gtype_t::generic))
        {
            method_t * method = __append_default_constructor();
            this->__delay(context, walk_step_t::analysis, method);
        }
    }

    // Walks analysis step.
    void type_ast_node_t::__walk_analysis(ast_walk_context_t & context, method_t * method)
    {
        __method_utils_t(__context, context).revise_constructor(*method, nullptr);
    }

    // Appends default constructor.
    method_t * type_ast_node_t::__append_default_constructor()
    {
        method_t * method = __new_obj<method_t>();

        method->name     = __type.name;
        method->decorate = __new_obj<decorate_t>();
        method->decorate->access = access_value_t::public_;
        method->trait    = method_trait_t::constructor;
        method->body     = __new_obj<method_body_t>();

        method->body->push_back(__new_obj<return_statement_t>());

        __type.methods.push_back(method);

        return method;
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_param

    // Sets name.
    void generic_param_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__param.name, name, el, _T("generic param"));
    }

    // Sets param type.
    void generic_param_ast_node_t::set_param_type(generic_param_type_t type, __el_t * el)
    {
        this->__assign(__param.param_type, type, el, _T("generic param type"));
    }

    // Commits this node.
    void generic_param_ast_node_t::on_commit()
    {
        __param.attributes = __to_eobject_with_owner<attributes_t *>(attributes, &__param);
    }

    // Returns this eobject.
    generic_param_t * generic_param_ast_node_t::to_eobject()
    {
        return &__param;
    }

    // Walks this node.
    void generic_param_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__param.name, _T("template param"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_params

    // Commits this node.
    void generic_params_ast_node_t::on_commit()
    {
        this->transform_child_to(params, __generic_params);
    }

    // Returns this eobject.
    generic_params_t * generic_params_ast_node_t::to_eobject()
    {
        return &__generic_params;
    }

    // Walks this node.
    void generic_params_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_constraint

    // Sets constraint.
    void generic_constraint_ast_node_t::set_constraint(generic_constraint_ttype_t cttype,
                                                            __el_t * el)
    {
        this->__assign(__generic_constraint.cttype, cttype, el, _T("constraint type"), true);
    }

    // Sets name.
    void generic_constraint_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__generic_constraint.param_name, name, el, _T("generic param"));
    }

    // Commits this node.
    void generic_constraint_ast_node_t::on_commit()
    {
        this->transform_child_to(type_names, __generic_constraint.type_names);
    }

    // Returns this eobject.
    generic_constraint_t * generic_constraint_ast_node_t::to_eobject()
    {
        return &__generic_constraint;
    }

    // Walks this node.
    void generic_constraint_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_constraints

    // Commits this node.
    void generic_constraints_ast_node_t::on_commit()
    {
        this->transform_child_to(constraints, __generic_constraints);
    }

    // Returns this eobject.
    generic_constraints_t * generic_constraints_ast_node_t::to_eobject()
    {
        return &__generic_constraints;
    }

    // Walks this node.
    void generic_constraints_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_arg

    // Commits this node.
    void generic_arg_ast_node_t::on_commit()
    {
        __arg.type_name = __to_eobject<type_name_t *>(type_name);
    }

    // Returns this eobject.
    generic_arg_t * generic_arg_ast_node_t::to_eobject()
    {
        return &__arg;
    }

    // Walks this node.
    void generic_arg_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__arg.type_name, this, __c_t::type_name_missing, _T("generic argument"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_args

    // Commits this node.
    void generic_args_ast_node_t::on_commit()
    {
        this->transform_child_to(generic_args, __args);
    }

    // Returns this eobject.
    generic_args_t * generic_args_ast_node_t::to_eobject()
    {
        return &__args;
    }

    // Walks this node.
    void generic_args_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if (__args.empty())
            this->__log(this, __c_t::type_name_missing, _T("generic arguments"));

        if (__is_partial_specialization(__args))
            this->__log(this, __c_t::partial_specialization_not_supported);

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // param

    // Sets name.
    void param_ast_node_t::set_name(const name_t & name, __el_t * el)
    {
        this->__assign_name(__param.name, name, el, _T("param"));
    }

    // Sets param type.
    void param_ast_node_t::set_param_type(param_type_t ptype, __el_t * el)
    {
        this->__assign(__param.ptype, ptype, el, _T("param type"));
    }

    // Commits this node.
    void param_ast_node_t::on_commit()
    {
        __param.type_name     = __to_eobject<type_name_t *>(type_name);
        __param.default_value = __to_eobject<expression_t *>(default_value);
        __param.attributes    = __to_eobject_with_owner<attributes_t *>(attributes, &__param);
    }

    // Returns this eobject.
    param_t * param_ast_node_t::to_eobject()
    {
        return &__param;
    }

    // Walks this node.
    void param_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                this->__check_name_empty(__param.name, _T("param"));
                this->__check_empty(__param.type_name, this, __c_t::type_name_missing, _T("param"));

                this->__delay(context, walk_step_t::pre_analysis);
                __super_t::on_walk(context, step, tag);
                break;

            case walk_step_t::pre_analysis:
                if (__param.default_value != nullptr)
                {
                    expression_t * exp = __param.default_value;

                    try
                    {
                        cvalue_t cv = this->__execute_expression(exp);

                        if (is_nan(cv))
                        {
                            this->__log(exp, __c_t::unexpected_param_default_value,
                                                _T("unexpected constant expression"));
                        }
                    }
                    catch(logic_error_t<expression_execute_error_code_t> & e)
                    {
                        this->__log(exp, __c_t::unexpected_param_default_value, e.get_message());
                    }
                }

                break;

            default: break;
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // params

    // Commits this node.
    void params_ast_node_t::on_commit()
    {
        this->transform_child_to(params, __params);
    }

    // Returns this eobject.
    params_t * params_ast_node_t::to_eobject()
    {
        return &__params;
    }

    // Walks this node.
    void params_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // attribute

    // Commits this node.
    void attribute_ast_node_t::on_commit()
    {
        __attr.type_name = __to_eobject<type_name_t *>(type_name);
        __attr.arguments = __to_eobject<arguments_t *>(arguments);
    }

    // Returns this eobject.
    attribute_t * attribute_ast_node_t::to_eobject()
    {
        if (__is_compile_time_attribute(&__attr))
            return __get_compile_time_attribute();

        return &__attr;
    }

    // Walks this node.
    void attribute_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                context.current_module()->register_commit(this->to_eobject());
                this->__delay(context, walk_step_t::post_confirm);
                __super_t::on_walk(context, step, tag);
                break;

            case walk_step_t::post_confirm:
                if (__is_compile_time_attribute(&__attr))
                    __get_compile_time_attribute()->type_name = __attr.type_name;

                this->__delay(context, walk_step_t::analysis);
                break;

            case walk_step_t::analysis:
                __analyze_attribute_constructor();
                break;

            default: break;
        }
    }

    // Returns compile time attribute.
    attribute_t * attribute_ast_node_t::__get_compile_time_attribute()
    {
        return __get_xpool().get_compile_time_attribute();
    }

    // Analyze attribute constructor.
    void attribute_ast_node_t::__analyze_attribute_constructor()
    {
        type_name_t * type_name = __attr.type_name;
        type_t * type;

        if (type_name == nullptr || (type = type_name->type) == nullptr)
            return;

        method_t * method = find_constructor(this->__context, type, __attr.arguments);
        if (method == nullptr)
        {
            __log(&__attr, __c_t::attribute_constructor_not_match, type,
                to_arg_types_str(this->__context, __attr.arguments)
            );

            return;
        }

        if (__attr.arguments != nullptr)
        {
            expression_execute_context_t ctx(__get_xpool());
            for (argument_t * argument : *__attr.arguments)
            {
                expression_t * exp = argument->expression;
                if (exp != nullptr)
                {
                    try
                    {
                        exp->execute(ctx);
                    }
                    catch(const error_t & e)
                    {
                        _PP(e);
                    }
                }
            }
        }

        this->to_eobject()->constructor = method;
    }

    ////////// ////////// ////////// ////////// //////////
    // attributes

    // Commits this node.
    void attributes_ast_node_t::on_commit()
    {
        this->transform_child_to(attributes, __attributes);
    }

    // Returns this eobject.
    attributes_t * attributes_ast_node_t::to_eobject()
    {
        return &__attributes;
    }

    // Walks this node.
    void attributes_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // expression_st

    // Walks this node.
    void expression_st_ast_node_t::on_commit()
    {
        __statement.expression = __to_eobject<expression_t *>(expression);
    }

    // Returns this eobject.
    statement_t * expression_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    // Walks this node.
    void expression_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__statement.expression, this, __c_t::expression_missing,
                                                          _T("statement"));
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // type_def_st

    // Commits this node.
    void type_def_st_ast_node_t::on_commit()
    {
        __statement.type_def = __to_eobject<type_def_t *>(type_def);
    }

    // Returns this eobject.
    statement_t * type_def_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    // Walks this node.
    void type_def_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__statement.type_def, this, __c_t::statement_missing);

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // defination_st

    // Commits this node.
    void defination_st_ast_node_t::on_commit()
    {
        __statement.type_name = __to_eobject<type_name_t *>(type_name);
        this->transform_child_to(items, __statement);
    }

    // Returns this eobject.
    statement_t * defination_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    // Set constant.
    void defination_st_ast_node_t::set_constant(bool constant)
    {
        __statement.constant = constant;
    }

    // Walks this node.
    void defination_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                __super_t::on_walk(context, step, tag);
                __walk_default(context);
                this->__delay(context, walk_step_t::analysis);
                break;

            case walk_step_t::analysis:
                __walk_analysis(context);
                break;

            default: break;
        }
    }

    // Walks default step.
    void defination_st_ast_node_t::__walk_default(ast_walk_context_t & context)
    {
        this->__check_empty(__statement.type_name, this, __c_t::type_name_missing,
                                                        _T("variable defination"));
        variable_defination_t vd(this->__context, context, &__statement);

        for (defination_statement_item_t * var_item : __statement.items)
        {
            var_item->variable = vd.define_local(
                __statement.type_name, var_item->name, __statement.constant,
                var_item->expression, var_item
            );
        }
    }

    // Walks analysis step.
    void defination_st_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        if (!__statement.constant)
            return;

        for (defination_statement_item_t * var_item : __statement.items)
        {
            if (var_item->expression == nullptr)
            {
                this->__log(this, __c_t::constant_variable_initialize_missing, var_item->name);
            }
            else if (__execute_expression(var_item->expression) == cvalue_t::nan)
            {
                this->__log(this, __c_t::constant_variable_required_constant_value, var_item->name);
            }
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // break_st

    // Returns this eobject.
    statement_t * break_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    // Walks this node.
    void break_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // continue_st

    // Returns this eobject.
    statement_t * continue_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    // Walks this node. 
    void continue_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // throw_st

    // Commits this node.
    void throw_st_ast_node_t::on_commit()
    {
        __statement.expression = __to_eobject<expression_t *>(expression);
    }

    // Returns this eobject.
    statement_t * throw_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    // Walks this node.
    void throw_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // goto_st

    // Sets label.
    void goto_st_ast_node_t::set_label(const name_t & label, __el_t * el)
    {
        this->__assign_name(__statement.label, label, el, _T("label"));
    }

    // Commits this node.
    void goto_st_ast_node_t::on_commit()
    {

    }

    // Returns this eobject.
    statement_t * goto_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    // Walks this node.
    void goto_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__statement.label, _T("label"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // return_st

    // Commits this node.
    void return_st_ast_node_t::on_commit()
    {
        __statement.expression = __to_eobject<expression_t *>(expression);
    }

    // Returns this eobject.
    statement_t * return_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    // Wallks this node.
    void return_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                __super_t::on_walk(context, step, tag);
                this->__delay(context, walk_step_t::post_analysis);
                break;

            case walk_step_t::post_analysis:
                __walk_post_analysis(context);
                break;

            default: break;
        }
    }

    // Walks post analysis step.
    void return_st_ast_node_t::__walk_post_analysis(ast_walk_context_t & context)
    {
        method_t * method = context.current_method();
        _A(method != nullptr);

        if (method->type_name == nullptr || is_void_type(method->type_name->type))
        {
            if (__statement.expression != nullptr)
                this->__log(this, __c_t::method_unexpected_return_value, method);
        }
        else
        {
            if (__statement.expression == nullptr)
            {
                this->__log(this, __c_t::method_return_value_missing, method);
            }
            else
            {
                type_t * return_type = __statement.expression->get_type(__get_xpool());
                type_t * method_type = method->type_name->type;

                if (!is_type_compatible(return_type, method_type))
                    this->__log(this, __c_t::method_incompatible_return_value, method);
            }
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // do_while_st

    // Commits this node.
    void do_while_st_ast_node_t::on_commit()
    {
        __statement.condition = __to_eobject<expression_t *>(condition);
        __statement.body      = __to_eobject<statement_t *>(body);
    }

    // Returns this eobject.
    statement_t * do_while_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    // Walks this node.
    void do_while_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__statement.condition, this, __c_t::condition_missing, _T("do while"));
        this->__check_empty(__statement.body, this, __c_t::body_missing, _T("do while"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // loop_until_st

    // Commits this node.
    void loop_until_st_ast_node_t::on_commit()
    {
        __statement.condition = __to_eobject<expression_t *>(condition);
        __statement.body      = __to_eobject<statement_t *>(body);
    }

    // Returns this eobject.
    statement_t * loop_until_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    // Walks this node.
    void loop_until_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__statement.condition, this, __c_t::condition_missing, _T("loop until"));
        this->__check_empty(__statement.body, this, __c_t::body_missing, _T("loop until"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // while_st

    // Commits this node.
    void while_st_ast_node_t::on_commit()
    {
        __statement.condition = __to_eobject<expression_t *>(condition);
        __statement.body      = __to_eobject<statement_t *>(body);
    }

    // Returns this eobject.
    statement_t * while_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    // Walks this node.
    void while_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__statement.condition, this, __c_t::condition_missing, _T("while"));
        this->__check_empty(__statement.body, this, __c_t::body_missing, _T("while"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // for_st

    // Commits this node.
    void for_st_ast_node_t::on_commit()
    {
        auto init_statement = __try_to_eobject<statement_t *>(initialize);
        if (init_statement != nullptr)
            __statement.defination_initialize = _M(defination_statement_t *, init_statement);
        else
            __statement.initialize = __to_eobject<expression_t *>(initialize);

        __statement.condition  = __to_eobject<expression_t *>(condition);
        __statement.increase   = __to_eobject<expression_t *>(increase);
        __statement.body       = __to_eobject<statement_t *>(body);
    }

    // Returns this eobject.
    statement_t * for_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    // Walks this node.
    void for_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__statement.body, this, __c_t::body_missing, _T("while"));

        if (__statement.defination_initialize != nullptr)
        {
            context.push_new_region();
            __super_t::on_walk(context, step, tag);
            context.pop();
        }
        else
        {
            __super_t::on_walk(context, step, tag);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // for_each_st

    // Set variables.
    void for_each_st_ast_node_t::set_variable(name_t name, __el_t * el)
    {
        this->__assign_name(__statement.variable, name, el, _T("var"));
    }

    // Commits this node.
    void for_each_st_ast_node_t::on_commit()
    {
        __statement.iterator   = __to_eobject<expression_t *>(iterator);
        __statement.type_name  = __to_eobject<type_name_t *>(type_name);
        __statement.body       = __to_eobject<statement_t *>(body);
    }

    // Returns this eobject.
    statement_t * for_each_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    // Walks this node.
    void for_each_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__statement.variable, _T("variable"));
        this->__check_empty(__statement.iterator, this, __c_t::iterator_missing, "for each");
        this->__check_empty(__statement.body, this, __c_t::body_missing, _T("for each"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // if_st

    // Commits this node.
    void if_st_ast_node_t::on_commit()
    {
        __statement.condition   = __to_eobject<expression_t *>(condition);
        __statement.if_body     = __to_eobject<statement_t *>(if_body);
        __statement.else_body   = __to_eobject<statement_t *>(else_body);

        this->__check_empty(__statement.condition, this, __c_t::condition_missing, "if");
        this->__check_empty(__statement.if_body, this, __c_t::body_missing, _T("if"));
    }

    // Returns this eobject.
    statement_t * if_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    // Walks this node.
    void if_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // switch_st

    // Commits this node.
    void switch_st_ast_node_t::on_commit()
    {
        __statement.expression = __to_eobject<expression_t *>(expression);
        this->transform_child_to(cases, __statement.cases);
    }

    // Returns this eobject.
    statement_t * switch_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    // Walks this node.
    void switch_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__statement.expression, this, __c_t::expression_missing, _T("switch"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // case

    // Appends default expression.
    void case_ast_node_t::append_default(__el_t * el)
    {
        auto * exp_node = to_fake_ast<(__value_t)__cvalue_t::expression>(
            (expression_t *)nullptr, __get_memory(), el->code_unit
        );

        this->append_child(constants, exp_node);
    }

    // Commits this node.
    void case_ast_node_t::on_commit()
    {
        this->transform_child_to(constants, __case.constants);
        __case.statements = __to_eobject<statements_t *>(statements);
    }

    // Returns this eobject.
    case_t * case_ast_node_t::to_eobject()
    {
        return &__case;
    }

    // Walks this node.
    void case_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__case.constants, this, __c_t::expression_missing, _T("case"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // try_st

    // Commits this node.
    void try_st_ast_node_t::on_commit()
    {
        __statement.try_statement     = __to_eobject<statement_t *>(try_);
        __statement.finally_statement = __to_eobject<statement_t *>(finally);

        this->transform_child_to(catches, __statement.catches);
    }

    // Returns this eobject.
    statement_t * try_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    // Walks this node.
    void try_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if (__statement.catches.empty() && __statement.finally_statement == nullptr)
        {
            this->__log(this, __c_t::catch_or_finally_missing);
        }

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // catch

    // Sets variable.
    void catch_ast_node_t::set_variable(name_t name, __el_t * el)
    {
        this->__assign_name(__catch.name, name, el, _T("variable"));
        __variable_el = el;
    }

    // Commits this node.
    void catch_ast_node_t::on_commit()
    {
        __catch.type_name = __to_eobject<type_name_t *>(type_name);
        __catch.body      = __to_eobject<statement_t *>(body);
    }

    // Returns this eobject.
    catch_t * catch_ast_node_t::to_eobject()
    {
        return &__catch;
    }

    // Walks this node.
    void catch_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if (__catch.type_name == nullptr && !__catch.name.empty())
        {
            this->__log(__variable_el, __c_t::type_name_missing, _T("catch statement"));
        }
        else
        {
            variable_defination_t vd(this->__context, context, __variable_el);
            __catch.variable = vd.define_local(__catch.type_name, __catch.name);
        }

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // empty_st

    // Returns this eobject.
    statement_t * empty_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    // Walks this node.
    void empty_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // statement_group_st

    // Commits this node.
    void statement_group_st_ast_node_t::on_commit()
    {
        this->transform_child_to(statements, __statement_group);
    }

    // Returns this eobject.
    statement_t * statement_group_st_ast_node_t::to_eobject()
    {
        return &__statement_group;
    }

    // Walks this node.
    void statement_group_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        context.push_new_region();
        __super_t::on_walk(context, step, tag);
        context.pop();
    }

    ////////// ////////// ////////// ////////// //////////
    // type_name_exp

    // Commits this node.
    void type_name_exp_ast_node_t::on_commit()
    {
        __expression.type_name = __to_eobject<type_name_t *>(type_name);
    }

    // Returns this eobject.
    expression_t * type_name_exp_ast_node_t::to_eobject()
    {
        return &__expression;
    }

    // Walks this node.
    void type_name_exp_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // type_cast_exp

    // Commits this node.
    void type_cast_exp_ast_node_t::on_commit()
    {
        __expression.type_name  = __to_eobject<type_name_t *>(type_name);
        __expression.set_expression(__to_eobject<expression_t *>(expression));
    }

    // Returns this eobject.
    expression_t * type_cast_exp_ast_node_t::to_eobject()
    {
        return &__expression;
    }

    // Walks this node.
    void type_cast_exp_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // function

    // Commits this node.
    void function_ast_node_t::on_commit()
    {
        __expression.namex         = __to_eobject<expression_t *>(namex);
        __expression.generic_args  = __to_eobject<generic_args_t *>(generic_args);

        __expression.set_arguments(__to_eobject<arguments_t *>(arguments));
    }

    // Returns this eobject.
    expression_t * function_ast_node_t::to_eobject()
    {
        return &__expression;
    }

    // Walks this node.
    void function_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__expression.namex, _T("function"));

        if (__expression.generic_args != nullptr && __expression.get_name() == name_t::null)
        {
            this->__log(this->child_at(generic_args), __c_t::function_generic_args_redundance);
        }

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // index

    // Commits this node.
    void index_ast_node_t::on_commit()
    {
        __expression.set_namex(__to_eobject<expression_t *>(namex));

        __expression.set_arguments(__to_eobject<arguments_t *>(arguments));
    }

    // Returns this eobject.
    expression_t * index_ast_node_t::to_eobject()
    {
        return &__expression;
    }

    // Walks this node.
    void index_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__expression.namex(), _T("index"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // new

    // Commits this node.
    void new_ast_node_t::on_commit()
    {
        __expression.type_name = __to_eobject<type_name_t *>(type_name);
        __expression.set_arguments(__to_eobject<arguments_t *>(arguments));
    }

    // Returns this eobject.
    expression_t * new_ast_node_t::to_eobject()
    {
        return &__expression;
    }

    // Walks this node.
    void new_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__expression.type_name, this, __c_t::type_name_missing,
                                                            _T("object creation"));
        __super_t::on_walk(context, step, tag);

        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                this->__delay(context, walk_step_t::analysis);
                __super_t::on_walk(context, step, tag);
                break;

            case walk_step_t::analysis: {
                type_t * type = to_type(__expression.type_name);
                if (type != nullptr)
                {
                    __expression.constructor = find_constructor(
                        this->__context, type, __expression.arguments()
                    );

                    if (__expression.constructor == nullptr)
                    {
                        __log(&__expression, __c_t::type_constructor_not_match, type,
                            to_arg_types_str(this->__context, __expression.arguments())
                        );
                    }
                }

            }   break;

            default: break;
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // new_array

    // Commits this node.
    void new_array_ast_node_t::on_commit()
    {
        __expression.type_name = __to_eobject<type_name_t *>(type_name);
        __expression.set_lengths(__to_eobject<array_lengths_t *>(lengths));
        __expression.set_initializer(__to_eobject<array_initializer_t *>(initializer));
    }

    // Returns this eobject.
    expression_t * new_array_ast_node_t::to_eobject()
    {
        return &__expression;
    }

    const array_length_t unkown_array_length = -1;

    // Walks this eobject.
    void new_array_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                __walk_default(context);

                __super_t::on_walk(context, step, tag);

                this->__delay(context, walk_step_t::confirm);

                if (__expression.initializer() != nullptr)
                    this->__delay(context, walk_step_t::analysis);

                break;

            case walk_step_t::confirm:
                __walk_confirm(context);
                break;

            case walk_step_t::analysis:
                __walk_analysis(context);
                break;

            default:
                break;
        }
    }

    // Walks default step.
    void new_array_ast_node_t::__walk_default(ast_walk_context_t & context)
    {
        this->__check_empty(__expression.type_name, this, __c_t::type_name_missing,
                                                            _T("array creation"));
        this->__check_empty(__expression.lengths(), this, __c_t::array_lengths_missing);

        // dimension
        array_lengths_t * p_lengths = this->__to_eobject<array_lengths_t *>(lengths);
        if (p_lengths != nullptr)
        {
            array_lengths_t & lengths = *p_lengths;
            for (int dimension = 0, dimensions = lengths.size(); dimension < dimensions;
                                                                  dimension++)
            {
                expression_t * exp = lengths[dimension];
                if (exp == nullptr)
                {
                    array_length_t length = __get_length(dimension);
                    if (length != unkown_array_length)
                        __expression.set_length(dimension, __to_value_expression(length));
                    else
                        this->__log(this, __c_t::expected_array_length);
                }
            }
        }
    }

    // Walks confirm step.
    void new_array_ast_node_t::__walk_confirm(ast_walk_context_t & context)
    {
        // To ensure the array type be pushed into xpool before execute xpool_t::commit_types().
        // Then used by some expressions, like (new int[10]).Length;
        // Otherwise, An error of 'type not found' will be occued.
        __expression.to_array_type(this->__get_xpool());
    }

    // Walks analysis step.
    void new_array_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        array_initializer_t * initializer = __expression.initializer();
        if (initializer != nullptr)
        {
            initializer->each_element([&, this](expression_t * exp) -> bool {
                walk_expression(this->__context, context, exp);
                return true;
            });
        }
    }

    // Returns length of specified dimension.
    array_length_t new_array_ast_node_t::__get_length(dimension_t dimension)
    {
        array_initializer_t * initializer = __expression.initializer();

        while (dimension-- > 0 && initializer != nullptr)
        {
            array_initialize_element_t element = (*initializer)[0];
            if (element.type != array_initialize_element_type_t::initializer)
                return unkown_array_length;

            initializer = element.initializer;
        }

        if (initializer == nullptr)
            return unkown_array_length;

        return initializer->size();
    }

    ////////// ////////// ////////// ////////// //////////
    // array_initializer

    // Commits this node.
    void array_initializer_ast_node_t::on_commit()
    {
        this->each_child_node<ast_node_t>(elements, [&](ast_node_t * node) {

            expression_ast_t * expression_ast;
            array_initializer_ast_t * initializer_ast;

            if ((expression_ast = as<expression_ast_t *>(node)) != nullptr)
            {
                __initializer.append(expression_ast->to_eobject());
            }
            else if ((initializer_ast = as<array_initializer_ast_t *>(node)) != nullptr)
            {
                __initializer.append(initializer_ast->to_eobject());
            }
            else
            {
                X_UNEXPECTED();
            }
        });
    }

    // Returns this eobject.
    array_initializer_t * array_initializer_ast_node_t::to_eobject()
    {
        return &__initializer;
    }

    // Walks this node.
    void array_initializer_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        try
        {
            __initializer.check();
        }
        catch(logic_error_t<array_initialize_check_error_code_t> & e)
        {
            this->__log(this, __c_t::array_initializer_error, _str(e));
        }

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // array_lengths

    // Commits this node.
    void array_lengths_ast_node_t::on_commit()
    {
        this->transform_child_to(lengths, __lengths);
    }

    // Returns this eobject.
    array_lengths_t * array_lengths_ast_node_t::to_eobject()
    {
        return &__lengths;
    }

    // Walks this node.
    void array_lengths_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // default_value

    // Commits this node.
    void default_value_ast_node_t::on_commit()
    {
        __expression.type_name = __to_eobject<type_name_t *>(type_name);
    }

    // Returns this eobject.
    expression_t * default_value_ast_node_t::to_eobject()
    {
        return &__expression;
    }

    // Walks this node.
    void default_value_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__expression.type_name, this, __c_t::type_name_missing,
                                                        _T("default value"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // field

    // Sets name.
    void field_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__field.name, name, el, _T("field"));
    }

    // Commits this node.
    void field_ast_node_t::on_commit()
    {
        __field.decorate   = __to_eobject<decorate_t *>(decorate);
        __field.type_name  = __to_eobject<type_name_t *>(type_name);
        __field.init_value = __to_eobject<expression_t *>(init_value);
        __field.attributes = __to_eobject_with_owner<attributes_t *>(attributes, &__field);
    }

    // Returns this eobject.
    field_t * field_ast_node_t::to_eobject()
    {
        return &__field;
    }

    // Walks this node.
    void field_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        context.push(this->to_eobject());
        __super_t::on_walk(context, step, tag);
        context.pop();

        this->__check_empty(__field.type_name, this, __c_t::type_name_missing,
                _F(_T("field %1%"), _str(__field.name)));
        this->__check_empty(__field.name, this, __c_t::name_empty, _T("field"));

        variable_defination_t vd(this->__context, context, &__field);
        vd.define_field(&__field);
    }

    ////////// ////////// ////////// ////////// //////////
    // method

    // Sets name.
    void method_ast_node_t::set_name(name_t name, __el_t * el)
    {
        if (__op_property != nullptr)
        {
            __log(el, __c_t::duplicate, _T("method name / operator overload"), name);
            return;
        }

        this->__assign_name(__method.name, name, el, _T("method"));
    }

    // Sets operator. ( for operator overloading. )
    void method_ast_node_t::set_operator(const operator_property_t * op_property, __el_t * el)
    {
        _A(op_property != nullptr);

        if (__op_property != nullptr)
        {
            __log(el, __c_t::duplicate, _F(_T("operator overload")), op_property);
            return;
        }

        if (!__method.name.empty())
        {
            __log(el, __c_t::duplicate, _F(_T("method name / operator overload")), op_property);
            return;
        }

        this->__op_property = op_property;

        const char_t * op_name = op_property->name;
        _A(op_name != nullptr);

        __method.name = __to_name(_F(_T("op_%1%"), op_name));
    }

    // Sets trait.
    void method_ast_node_t::set_trait(method_trait_t trait, __el_t * el)
    {
        this->__assign(__method.trait, trait, el, _T("method trait"));
    }

    // Commits it.
    void method_ast_node_t::on_commit()
    {
        __method.type_name       = __to_eobject<type_name_t *>(type_name);
        __method.decorate        = __to_eobject<decorate_t *>(decorate);
        __method.params          = __to_eobject<params_t *>(params);
        __method.generic_params  = __to_eobject<generic_params_t *>(generic_params);
        __method.owner_type_name = __to_eobject<type_name_t *>(owner_type_name);
        __method.attributes      = __to_eobject_with_owner<attributes_t *>(attributes, &__method);
        __method.body            = __to_eobject<method_body_t *>(body);
    }

    // Returns this eobject.
    method_t * method_ast_node_t::to_eobject()
    {
        return &__method;
    }

    // Walks this node.
    void method_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                context.push(this->to_eobject());

                __super_t::on_walk(context, step, tag);
                __walk_default(context);
                this->__delay(context, walk_step_t::analysis);

                context.pop();
                break;

            case walk_step_t::analysis:
                __walk_analysis(context);
                break;

            default: break;
        }
    }

    // Walks the default step.
    void method_ast_node_t::__walk_default(ast_walk_context_t & context)
    {
        type_t * type = context.current_type();
        if (type != nullptr && __method.name == type->get_name()
                                && __method.owner_type_name == nullptr)  // constructor
        {
            if (__method.trait == method_trait_t::__default__)
            {
                if (__method.decorate && __method.decorate->is_static)
                    __method.trait = method_trait_t::static_constructor;
                else
                    __method.trait = method_trait_t::constructor;
            }
        }
        else
        {
            if (__method.trait == method_trait_t::__default__)
                __method.trait = method_trait_t::normal;

            this->__check_empty(__method.type_name, this, __c_t::type_name_missing,
                _F(_T("method %1%"), _str(__method.name))
            );
        }

        if (__method.type_name != nullptr)
        {
            switch (__method.trait)
            {
                case method_trait_t::constructor:
                    this->__log(this, __c_t::constructor_method_should_no_return_type,
                                                        _str(__method.name));
                    break;

                case method_trait_t::static_constructor:
                    this->__log(this, __c_t::static_constructor_method_should_no_return_type,
                                                        _str(__method.name));
                    break;

                case method_trait_t::destructor:
                    this->__log(this, __c_t::destructor_method_should_no_return_type,
                                                        _str(__method.name));
                    break;

                default:
                    break;
            }
        }

        this->__check_empty(__method.name, this, __c_t::name_empty, _T("method"));

        if (__method.params && !__method.params->empty())
        {
            variable_defination_t vd(this->__context, context, &__method);

            for (param_t * param : *__method.params)
            {
                vd.define_param(param);
            }

            switch (__method.trait)
            {
                case method_trait_t::static_constructor:
                    this->__log(this, __c_t::static_constructor_method_should_no_params,
                                                        _str(__method.name));
                    break;

                case method_trait_t::destructor:
                    this->__log(this, __c_t::destructor_method_should_no_params,
                                                        _str(__method.name));
                    break;

                default:
                    break;
            }
        }
    }

    // Walks the analysis step.
    void method_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        // Construct, static constructor, destructor.
        switch (__method.trait)
        {
            case method_trait_t::static_constructor:
                break;

            case method_trait_t::constructor:
                __method_utils_t(__context, context).revise_constructor(__method,
                    __to_eobject<arguments_t *>(base_ctor_args), this->child_at(base_ctor_args)
                );
                break;

            case method_trait_t::destructor:
                break;

            default:
                break;
        }

        // Operator overload, checks prototype.
        if (__op_property == nullptr)
            return;

        if (!__method.is_static() || !__method.is_public())
            this->__log(this, __c_t::operator_overload_prototype_error);

        if (__method.param_count() != __op_property->arity)
        {
            this->__log(this, (__op_property->arity == 1)?
                    __c_t::unitary_operator_overload_wrong_param_count :
                    __c_t::binary_operator_overload_wrong_param_count, __op_property->op
            );
        }

        bool has_containing_type = al::any_of(*__method.params, [&](param_t * param) {
            return param->get_type() == context.current_type();
        });

        if (!has_containing_type)
        {
            this->__log(this, (__op_property->arity == 1)?
                    __c_t::unitary_operator_overload_wrong_containing_type :
                    __c_t::binary_operator_overload_wrong_containing_type, __op_property->op
            );
        }

        type_t * return_type = __method.get_type();
        if (return_type == nullptr || is_void_type(return_type))
        {
            this->__log(this, __c_t::operator_overload_cannot_return_void);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // property

    // Sets name.
    void property_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__property.name, name, el, _T("property"));
    }

    // Commits this node.
    void property_ast_node_t::on_commit()
    {
        __property.type_name  = __to_eobject<type_name_t *>(type_name);
        __property.decorate   = __to_eobject<decorate_t *>(decorate);
        __property.get_method = __to_eobject<method_t *>(get_method);
        __property.set_method = __to_eobject<method_t *>(set_method);
        __property.params     = __to_eobject<params_t *>(params);
        __property.owner_type_name = __to_eobject<type_name_t *>(owner_type_name);
        __property.attributes = __to_eobject_with_owner<attributes_t *>(attributes, &__property);
    }

    // Returns this eobject.
    property_t * property_ast_node_t::to_eobject()
    {
        return &__property;
    }

    // Appends method.
    bool __append_member(ast_context_t & cctx, ast_walk_context_t & wctx, member_t * member)
    {
        type_t * type = wctx.current_type();
        if (type == nullptr || type->this_gtype() != gtype_t::general)
        {
            ast_log(cctx, member, __c_t::type_undetermind, member);
            return false;
        }

        ((general_type_t *)type)->append_member(member);
        return true;
    }

    // Walk this node.
    void property_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if (!__property.name.empty())
        {
            if (__property.params != nullptr)
                this->__log(this->child_at(params), __c_t::unexpected_params, "property");
        }
        else
        {
            if (__property.params == nullptr)
                this->__log(this, __c_t::name_empty, _T("property"));
        }

        if (__property.get_method != nullptr)
        {
            __property.get_method->name = __to_method_name(_T("get"));
            __property.get_method->type_name = __property.type_name;
            __property.get_method->decorate  = __property.decorate;
            __property.get_method->params    = __property.params;
            __append_member(this->__context, context, __property.get_method);
        }

        param_t * value_param = nullptr;

        if (__property.set_method != nullptr)
        {
            __property.set_method->name = __to_method_name(_T("set"));
            __property.set_method->type_name = context.to_type_name(vtype_t::void_);
            __property.set_method->decorate  = __property.decorate;

            params_t * params = __new_obj<params_t>();

            if (__property.params != nullptr)
                al::copy(*__property.params, std::back_inserter(*params));

            value_param = __new_obj<param_t>(__property.type_name, __to_name(_T("value")));
            params->push_back(value_param);

            __property.set_method->params = params;
            __append_member(this->__context, context, __property.set_method);
        }

        __generate_ignored_method_bodies(context, value_param);

        variable_region_t * region = context.current_region();

        variable_defination_t vd(this->__context, context, &__property);
        vd.define_property(&__property);

        context.push(this->to_eobject());
        __super_t::on_walk(context, step, tag);
        context.pop();
    }

    // Generates ignored method bodies.
    void property_ast_node_t::__generate_ignored_method_bodies(ast_walk_context_t & context,
                                                               param_t * value_param)
    {
        #define __AccessorEmpty(name)                                           \
            (__property.name##_method == nullptr)

        #define __BodyEmpty(name)                                               \
            (__property.name##_method->body == nullptr)

        #define __BodyIgnored(name)                                             \
            (!__AccessorEmpty(name)  && __BodyEmpty(name))

        if (__AccessorEmpty(get) && __AccessorEmpty(set))
        {
            this->__log(&__property, __c_t::property_accessor_missing, &__property);
            return;
        }

        if (!__AccessorEmpty(get) && !__AccessorEmpty(set))
        {
            if (__BodyEmpty(get) != __BodyEmpty(set))
            {
                this->__log(&__property, __c_t::property_method_body_missing,
                    __BodyEmpty(set)? _T("set") : _T("get")
                );

                return;
            }
        }

        if (!__BodyIgnored(get) && !__BodyIgnored(set))
            return;

        // Appends a field for the property.
        field_t * field = __new_obj<field_t>();
        field->type_name = __property.type_name;
        field->name = __to_name(_F(_T("__%1%__"), __property.name));

        decorate_value_t dv = decorate_value_t::default_value;
        dv.is_static = __property.is_static();
        dv.access = access_value_t::private_;

        field->decorate = __new_obj<decorate_t>(dv);

        __append_member(this->__context, context, field);

        // Appends get method body.
        if (__BodyIgnored(get))
        {
            statement_t * statement = __new_obj<return_statement_t>(
                __new_field_expression(field)
            );

            __property.get_method->body = __new_obj<method_body_t>();
            __property.get_method->body->push_back(statement);
        }

        if (__BodyIgnored(set))
        {
            _A(value_param != nullptr);

            method_body_t * body = __new_obj<method_body_t>(std::initializer_list<statement_t *>{

                // Assign value.
                __new_expression_statement<binary_expression_t>(
                    operator_t::assign,
                    __new_field_expression(field), __new_param_expression(value_param)
                ),

                // Returns.
                __new_obj<return_statement_t>()
            });

            __property.set_method->body = body;
        }

        #undef __AccessorEmpty
        #undef __BodyEmpty
        #undef __BodyIgnored
    }

    // Gets get/set method name.
    name_t property_ast_node_t::__to_method_name(const char_t * prefix)
    {
        if (__property.name != name_t::null)
            return __to_name(_F(_T("%1%_%2%"), prefix, __property.name));

        return __to_name(_F(_T("%1%_Item"), prefix));
    }

    ////////// ////////// ////////// ////////// //////////
    // event

    // Sets name.
    void event_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__event.name, name, el, _T("event"));
    }

    // Commits this node.
    void event_ast_node_t::on_commit()
    {
        __event.type_name       = __to_eobject<type_name_t *>(type_name);
        __event.decorate        = __to_eobject<decorate_t *>(decorate);
        __event.add_method      = __to_eobject<method_t *>(add_method);
        __event.remove_method   = __to_eobject<method_t *>(remove_method);
        __event.owner_type_name = __to_eobject<type_name_t *>(owner_type_name);
        __event.attributes      = __to_eobject_with_owner<attributes_t *>(attributes, &__event);
    }

    // Returns this eobject.
    event_t * event_ast_node_t::to_eobject()
    {
        return &__event;
    }

    // Walks this node.
    void event_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__event.name, _T("event"));

        if (__event.add_method != nullptr)
        {
            __event.add_method->name = __to_name(_F(_T("add_%1%"), __event.name));
            __event.add_method->type_name = __event.type_name;
            __append_member(this->__context, context, __event.add_method);
        }

        if (__event.remove_method != nullptr)
        {
            __event.remove_method->name = __to_name(_F(_T("remove_%1%"), __event.name));
            __event.remove_method->type_name = __event.type_name;
            __append_member(this->__context, context, __event.remove_method);
        }

        context.push(this->to_eobject());
        __super_t::on_walk(context, step, tag);
        context.pop();
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::compile

