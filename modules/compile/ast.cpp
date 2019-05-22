
#include <compile.h>

namespace X_ROOT_NS { namespace modules { namespace compile {

    typedef common_log_code_t __c_t;
    typedef code_element_t    __el_t;

    using namespace core;

    #define __This          (__this())
    #define __Node          subnode_t

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

        // The params parameter must be a single dimensional array.
        X_D(params_parameter_type_error,
                        _T("a params parameter must be a single dimensional array."))

        // A params parameter must be the last parameter in a formal parameter list.
        X_D(params_parameter_must_at_last,
                _T("a params parameter must be the last parameter in a formal parameter list"))

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
        switch((walk_step_t)step)
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
        new ((void *)__this_emname()) emname_t(mname);
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
        this->transform_child_to(__Node::expressions, *__This);
    }

    // Returns this eobject.
    expression_t * expressions_ast_node_t::to_eobject()
    {
        if(__This->expression_count() == 1)
        {
            expression_t * exp = __This->expression_at(0);
            return exp->parent = nullptr, exp;
        }

        return this;
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
        if(args == nullptr)
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
        for(expression_t * exp : dimensions)
        {
            if(index++ > 0)
                ss << _T(",");

            if(exp != 0)
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

        if(args.size() <= 1)
            return false;

        for(generic_arg_t * arg : args)
        {
            if(is_empty == unknown)
            {
                is_empty = (arg? yes : no);
            }
            else if((arg? yes : no) != is_empty)
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
        __This->global_type = global_type;
    }

    // Commits this node.
    void general_type_name_ast_node_t::on_commit()
    {
        this->transform_child_to(__Node::units, __This->units);
    }

    // Walks this node.
    void general_type_name_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch((walk_step_t)step)
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
        if(__This->type == nullptr)
        {
            __This->type = __ascertain_type(context, __This);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // type_name_unit

    // Sets name.
    void type_name_unit_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__This->name, name, el, _T("type"));
    }

    // Commits this node.
    void type_name_unit_ast_node_t::on_commit()
    {
        __This->args = __to_eobject<generic_args_t *>(__Node::generic_args);
    }

    // Walks this node.
    void type_name_unit_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__This->name, this, __c_t::name_empty, _T("type name unit"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // array_type_name

    // Commits this node.
    void array_type_name_ast_node_t::on_commit()
    {
        this->transform_child_to(__Node::dimensions, __This->dimensions);

        __This->element_type_name = __to_eobject<type_name_t *>(__Node::element_type_name);
    }

    // Walks this node.
    void array_type_name_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch((walk_step_t)step)
        {
            case walk_step_t::default_:
                this->__check_empty(__This->element_type_name, this,
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
        if(__This->element_type_name != nullptr && __This->type == nullptr)
        {
            __This->type = __ascertain_type(context, __This);
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
        this->__assign_name(__This->name, name, el, _T("typedef param"));
    }

    // Commits this node.
    void type_def_param_ast_node_t::on_commit()
    {

    }

    // Walks this node.
    void type_def_param_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__This->name, _T("typedef param"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // type_def_params

    // Commits this node.
    void type_def_params_ast_node_t::on_commit()
    {
        this->transform_child_to(params, *__This);
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
        this->__assign_name(__This->name, name, el, _T("typedef"));
    }

    // Commits this node.
    void type_def_ast_node_t::on_commit()
    {
        __This->type_name  = __to_eobject<type_name_t *>(__Node::type_name);
        __This->params     = __to_eobject<type_def_params_t *>(__Node::params);
    }

    // Walks this node.
    void type_def_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        context.push(to_eobject());

        __This->namespace_ = context.current_namespace();

        this->__check_empty(__This->name, this, __c_t::type_name_missing, _T("typedef"));

        const mname_t * ns = ns_to_full_name(context.current_namespace());

        type_def_t * type_def = to_eobject();
        if(!context.assembly.types.append_type_def(to_sid(ns), type_def))
            this->__log(this, __c_t::type_def_defination_duplicate, type_def);

        __super_t::on_walk(context, step, tag);
        context.pop();
    }

    ////////// ////////// ////////// ////////// //////////
    // type_of

    // Commits this node.
    void type_of_ast_node_t::on_commit()
    {
        __This->type_name = __to_eobject<type_name_t *>(__Node::type_name);
    }

    // Walks this node.
    void type_of_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__This->type_name, this, __c_t::type_name_missing, _T("typeof"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // argument

    // Sets name.
    void argument_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__This->name, name, el, _T("argument"));
    }

    // Sets argument type.
    void argument_ast_node_t::set_argument_type(param_type_t atype, __el_t * el)
    {
        this->__assign(__This->atype, atype, el, _T("argument type"));
    }

    // Commits this node.
    void argument_ast_node_t::on_commit()
    {
        __This->expression = __to_eobject<expression_t *>(__Node::expression);
        __This->attributes = __to_eobject_with_owner<attributes_t *>(__Node::attributes, __This);
    }

    // Walks this node.
    void argument_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);

        this->__check_empty(__This->expression, this, __c_t::argument_missing);

        typedef param_type_t t;
        if(__This->atype == t::ref || __This->atype == t::out)
        {
            if(__This->expression != nullptr
                    && __This->expression->this_family() != expression_family_t::name)
            {
                this->__log(this, __c_t::argument_type_error, _title(__This->atype));
            }
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // arguments

    // Commits this node.
    void arguments_ast_node_t::on_commit()
    {
        this->transform_child_to(__Node::arguments, *__This);
    }

    // Walks this node.
    void arguments_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // cvalue

    // Walks this node.
    void cvalue_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    // Sets value.
    void cvalue_ast_node_t::set_value(const cvalue_t & value)
    {
        *__This = value;
    }

    // Sets value.
    void cvalue_ast_node_t::set_value(const token_data_t & data)
    {
        switch(data.value_type)
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
        this->__assign(__BitField((*__This), access), value, el, _T("access"));
    }

    // Sets static.
    void decorate_ast_node_t::set_static(__el_t * el)
    {
        this->__assign(__BitField((*__This), is_static), true, el, _T("static"));
    }

    // Sets sealed.
    void decorate_ast_node_t::set_sealed(__el_t * el)
    {
        this->__assign(__BitField((*__This), is_sealed), true, el, _T("sealed"));
    }

    // Sets override.
    void decorate_ast_node_t::set_override(__el_t * el)
    {
        this->__assign(__BitField((*__This), is_override), true, el, _T("override"));
    }

    // Sets virtual.
    void decorate_ast_node_t::set_virtual(__el_t * el)
    {
        this->__assign(__BitField((*__This), is_virtual), true, el, _T("virtual"));
    }

    // Sets abstract.
    void decorate_ast_node_t::set_abstract(__el_t * el)
    {
        this->__assign(__BitField((*__This), is_abstract), true, el, _T("abstract"));
    }

    // Sets new.
    void decorate_ast_node_t::set_new(__el_t * el)
    {
        this->__assign(__BitField((*__This), is_new), true, el, _T("new"));
    }

    // Sets readonly.
    void decorate_ast_node_t::set_readonly(__el_t * el)
    {
        this->__assign(__BitField((*__This), is_readonly), true, el, _T("readonly"));
    }

    // Sets constant.
    void decorate_ast_node_t::set_const(__el_t * el)
    {
        this->__assign(__BitField((*__This), is_const), true, el, _T("const"));
    }

    // Sets extern.
    void decorate_ast_node_t::set_extern(__el_t * el)
    {
        this->__assign(__BitField((*__This), is_extern), true, el, _T("extern"));
    }

    // Commits this node.
    void decorate_ast_node_t::on_commit()
    {

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
        switch((walk_step_t)step)
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

        if(exit_type.has(__exit_type_t::pass) || exit_type == __exit_type_t::none)
        {
            method_t * method = context.current_method();
            _A(method != nullptr);

            if(method->type_name == nullptr || is_void_type(method->type_name->type))
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
        switch((walk_step_t)step)
        {
            case walk_step_t::default_:
                if(is_top_expression(__expression))
                    this->__delay(context, walk_step_t::analysis);
                __super_t::on_walk(context, step, tag);
                break;

            case walk_step_t::analysis: {
                expression_t * exp = this->to_eobject();
                if(!exp->walked)
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
        if(this->child_at(underlying) == nullptr)
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

        if(!__import.assembly)
        {
            this->__log(this, __c_t::assembly_name_missing);
        }
        else
        {
            document_t * document = context.current_document();
            if(document == nullptr)
            {
                this->__log(this, __c_t::unexpected_import, _str(import));
            }
            else
            {
                #define __LogAssembly(code) \
                    this->__log(this, __c_t::code, _str(import->assembly))
                    
                if(!import->package && context.assembly.name == to_mname(import->assembly))
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
                        switch(e.code)
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
        if(!__using_namespace.ns)
        {
            this->__log(this, __c_t::namespace_name_missing);
        }
        else
        {
            document_t * document = context.current_document();
            if(document == nullptr)
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
        template<typename _code_element_t>
        void revise_constructor(method_t & method, arguments_t * args, _code_element_t * node)
        {
            method_t * base_ctor = __get_base_contructor(args, node);
            __statements_t statements;

            // super ctor
            if(base_ctor != nullptr)
            {
                statements.push_back(__new_expression_statement<function_expression_t>(
                    base_ctor, args
                ));
            }

            // Initialize fields.
            __init_fields(statements, method);

            // Appends to method body.
            if(statements.size() > 0)
            {
                if(method.body == nullptr)
                    method.body = __new_obj<method_body_t>();

                method.body->push_front(statements);
            }
        }

    private:
        ast_walk_context_t & __wctx;

        // Returns base constructor.
        template<typename _code_element_t>
        method_t * __get_base_contructor(arguments_t * args, _code_element_t * node)
        {
            type_t * type = __wctx.current_type();
            _A(type != nullptr);

            type_t * base_type = type->get_base_type();

            if(base_type == nullptr)
            {
                if(!is_empty(args))
                {
                    this->__log(select_type<code_element_t *>(node, type),
                        __c_t::constructor_method_not_found, _T(">"), args
                    );
                }

                return nullptr;
            }

            method_t * ctor = find_constructor(this->__context, base_type, args);
            if(ctor == nullptr)
            {
                this->__log(select_type<code_element_t *>(node, type),
                    __c_t::constructor_method_not_found, base_type, args
                );
            }

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

            for(member_t * member : members_args.out_members)
            {
                field_t * field = (field_t *)member;
                expression_t * init_exp = field->init_value;
                type_t * field_type = to_type(field->type_name);

                if(field_type != nullptr && __check_default_value(field_type, init_exp))
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

            if(exp == nullptr)
                return false;

            cvalue_t cv = this->__execute_expression(exp);
            if(is_nan(cv))
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
        this->__assign_name(__This->name, name, el, _T("type"));
    }

    // Sets ttype.
    void type_ast_node_t::set_ttype(ttype_t ttype, __el_t * el)
    {
        __This->ttype = ttype;
    }

    // Appends member.
    void type_ast_node_t::append_member(member_t * member)
    {
        __This->append_member(member);
    }

    // Commits this node.
    void type_ast_node_t::on_commit()
    {
        __This->params     = __to_eobject<generic_params_t *>(__Node::generic_params);
        __This->decorate   = __to_eobject<decorate_t *>(__Node::decorate);
        __This->attributes = __to_eobject_with_owner<attributes_t *>(__Node::attributes,
                                                                    __This);

        this->transform_child_to(__Node::methods,           __This->methods);
        this->transform_child_to(__Node::properties,        __This->properties);
        this->transform_child_to(__Node::fields,            __This->fields);
        this->transform_child_to(__Node::events,            __This->events);
        this->transform_child_to(__Node::nest_types,        __This->nest_types);
        this->transform_child_to(__Node::type_defs,         __This->type_defs);
        this->transform_child_to(__Node::super_type_names,  __This->super_type_names);
    }

    // Walks this node.
    void type_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch((walk_step_t)step)
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

        if(!context.assembly.types.append_type(type))
            this->__log(this, __c_t::type_defination_duplicate, type);

        context.xpool.append_new_type(type);

        __super_t::on_walk(context, step, tag);

        bool has_constructor = al::any_of(__This->methods, [](method_t * m) {
            return m->trait == method_trait_t::constructor;
        });

        if(!has_constructor && al::in(type->this_gtype(), gtype_t::general, gtype_t::generic))
        {
            method_t * method = __append_default_constructor();
            this->__delay(context, walk_step_t::analysis, method);
        }
    }

    // Walks analysis step.
    void type_ast_node_t::__walk_analysis(ast_walk_context_t & context, method_t * method)
    {
        __method_utils_t(__context, context).revise_constructor(*method, nullptr, method);
    }

    // Appends default constructor.
    method_t * type_ast_node_t::__append_default_constructor()
    {
        method_t * method = __new_obj<method_t>();

        method->name     = __This->name;
        method->decorate = __new_obj<decorate_t>();
        method->decorate->access = access_value_t::public_;
        method->trait    = method_trait_t::constructor;
        method->body     = __new_obj<method_body_t>();

        method->body->push_back(__new_obj<return_statement_t>());

        __This->methods.push_back(method);

        return method;
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_param

    // Sets name.
    void generic_param_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__This->name, name, el, _T("generic param"));
    }

    // Sets param type.
    void generic_param_ast_node_t::set_param_type(generic_param_type_t type, __el_t * el)
    {
        this->__assign(__This->param_type, type, el, _T("generic param type"));
    }

    // Commits this node.
    void generic_param_ast_node_t::on_commit()
    {
        __This->attributes = __to_eobject_with_owner<attributes_t *>(__Node::attributes, __This);
    }

    // Walks this node.
    void generic_param_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__This->name, _T("template param"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_params

    // Commits this node.
    void generic_params_ast_node_t::on_commit()
    {
        this->transform_child_to(__Node::params, *__This);
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
        this->__assign(__This->cttype, cttype, el, _T("constraint type"), true);
    }

    // Sets name.
    void generic_constraint_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__This->param_name, name, el, _T("generic param"));
    }

    // Commits this node.
    void generic_constraint_ast_node_t::on_commit()
    {
        this->transform_child_to(__Node::type_names, __This->type_names);
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
        this->transform_child_to(__Node::constraints, *__This);
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
        __This->type_name = __to_eobject<type_name_t *>(__Node::type_name);
    }

    // Walks this node.
    void generic_arg_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__This->type_name, this, __c_t::type_name_missing,
                                                     _T("generic argument"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_args

    // Commits this node.
    void generic_args_ast_node_t::on_commit()
    {
        this->transform_child_to(__Node::generic_args, *__This);
    }

    // Walks this node.
    void generic_args_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if(__This->empty())
            this->__log(this, __c_t::type_name_missing, _T("generic arguments"));

        if(__is_partial_specialization(*__This))
            this->__log(this, __c_t::partial_specialization_not_supported);

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // param

    // Sets name.
    void param_ast_node_t::set_name(const name_t & name, __el_t * el)
    {
        this->__assign_name(__This->name, name, el, _T("param"));
    }

    // Sets param type.
    void param_ast_node_t::set_param_type(param_type_t ptype, __el_t * el)
    {
        this->__assign(__This->ptype, ptype, el, _T("param type"));
    }

    // Commits this node.
    void param_ast_node_t::on_commit()
    {
        __This->type_name     = __to_eobject<type_name_t *>(__Node::type_name);
        __This->default_value = __to_eobject<expression_t *>(__Node::default_value);
        __This->attributes    = __to_eobject_with_owner<attributes_t *>(__Node::attributes, __This);
    }

    // Walks this node.
    void param_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch((walk_step_t)step)
        {
            case walk_step_t::default_:
                this->__check_name_empty(__This->name, _T("param"));
                this->__check_empty(__This->type_name, this, __c_t::type_name_missing, _T("param"));

                this->__delay(context, walk_step_t::pre_analysis);
                __super_t::on_walk(context, step, tag);
                break;

            case walk_step_t::pre_analysis:
                if(__This->default_value != nullptr)
                {
                    expression_t * exp = __This->default_value;

                    try
                    {
                        cvalue_t cv = this->__execute_expression(exp);

                        if(is_nan(cv))
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

                // Checks params type.
                if(__This->ptype == param_type_t::params)
                {
                    type_t * type = __This->get_type();
                    if(type != nullptr && !is_array(type))
                    {
                        this->__log(__This, __c_t::params_parameter_type_error);
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
        this->transform_child_to(__Node::params, *__This);
    }

    // Walks this node.
    void params_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch((walk_step_t)step)
        {
            case walk_step_t::default_:
                this->__delay(context, walk_step_t::pre_analysis);
                __super_t::on_walk(context, step, tag);
                break;

            case walk_step_t::pre_analysis:
                for(int k = 0, count = __This->size(); k < count; k++)
                {
                    param_t * param = (*__This)[k];
                    if(param->ptype == param_type_t::params && k != count - 1)
                    {
                        this->__log(param, __c_t::params_parameter_must_at_last);
                    }
                }

                break;

            default: break;
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // attribute

    // Commits this node.
    void attribute_ast_node_t::on_commit()
    {
        __This->type_name = __to_eobject<type_name_t *>(__Node::type_name);
        __This->arguments = __to_eobject<arguments_t *>(__Node::arguments);
    }

    // Returns this eobject.
    attribute_t * attribute_ast_node_t::to_eobject()
    {
        if(__is_compile_time_attribute(__This))
            return __get_compile_time_attribute();

        return __This;
    }

    // Walks this node.
    void attribute_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch((walk_step_t)step)
        {
            case walk_step_t::default_:
                context.current_module()->register_commit(this->to_eobject());
                this->__delay(context, walk_step_t::post_confirm);
                __super_t::on_walk(context, step, tag);
                break;

            case walk_step_t::post_confirm:
                if(__is_compile_time_attribute(__This))
                    __get_compile_time_attribute()->type_name = __This->type_name;

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
        type_name_t * type_name = __This->type_name;
        type_t * type;

        if(type_name == nullptr || (type = type_name->type) == nullptr)
            return;

        method_t * method = find_constructor(this->__context, type, __This->arguments);
        if(method == nullptr)
        {
            __log(__This, __c_t::attribute_constructor_not_match, type,
                to_arg_types_str(this->__context, __This->arguments)
            );

            return;
        }

        if(__This->arguments != nullptr)
        {
            expression_execute_context_t ctx(__get_xpool());
            for(argument_t * argument : *__This->arguments)
            {
                expression_t * exp = argument->expression;
                if(exp != nullptr)
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
        this->transform_child_to(__Node::attributes, *__This);
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
        __This->expression = __to_eobject<expression_t *>(__Node::expression);
    }

    // Walks this node.
    void expression_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__This->expression, this, __c_t::expression_missing,
                                                          _T("statement"));
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // type_def_st

    // Commits this node.
    void type_def_st_ast_node_t::on_commit()
    {
        __This->type_def = __to_eobject<type_def_t *>(__Node::type_def);
    }

    // Walks this node.
    void type_def_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__This->type_def, this, __c_t::statement_missing);

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // defination_st

    // Commits this node.
    void defination_st_ast_node_t::on_commit()
    {
        __This->type_name = __to_eobject<type_name_t *>(__Node::type_name);
        this->transform_child_to(__Node::items, *__This);
    }

    // Set constant.
    void defination_st_ast_node_t::set_constant(bool constant)
    {
        __This->constant = constant;
    }

    ////////// ////////// ////////// ////////// //////////

    // Walks this node.
    void defination_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch((walk_step_t)step)
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
        this->__check_empty(__This->type_name, this, __c_t::type_name_missing,
                                                        _T("variable defination"));
        variable_defination_t vd(this->__context, context,
            static_cast<code_element_t *>(this)
        );

        for(defination_statement_item_t * var_item : __This->items)
        {
            var_item->variable = vd.define_local(
                __This->type_name, var_item->name, __This->constant,
                var_item->expression, var_item
            );
        }
    }

    // Walks analysis step.
    void defination_st_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        if(!__This->constant)
            return;

        for(defination_statement_item_t * var_item : __This->items)
        {
            if(var_item->expression == nullptr)
            {
                this->__log(this, __c_t::constant_variable_initialize_missing, var_item->name);
            }
            else if(__execute_expression(var_item->expression) == cvalue_t::nan)
            {
                this->__log(this, __c_t::constant_variable_required_constant_value, var_item->name);
            }
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // break_st

    // Walks this node.
    void break_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // continue_st

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
        __This->expression = __to_eobject<expression_t *>(__Node::expression);
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
        this->__assign_name(__This->label, label, el, _T("label"));
    }

    // Commits this node.
    void goto_st_ast_node_t::on_commit()
    {

    }

    // Walks this node.
    void goto_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__This->label, _T("label"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // return_st

    // Commits this node.
    void return_st_ast_node_t::on_commit()
    {
        __This->expression = __to_eobject<expression_t *>(__Node::expression);
    }

    // Wallks this node.
    void return_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch((walk_step_t)step)
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

        if(method->type_name == nullptr || is_void_type(method->type_name->type))
        {
            if(__This->expression != nullptr)
                this->__log(this, __c_t::method_unexpected_return_value, method);
        }
        else
        {
            if(__This->expression == nullptr)
            {
                this->__log(this, __c_t::method_return_value_missing, method);
            }
            else
            {
                type_t * return_type = __This->expression->get_type(__get_xpool());
                type_t * method_type = to_type(method->type_name);

                if(!is_type_compatible(return_type, method_type))
                    this->__log(this, __c_t::method_incompatible_return_value, method);
            }
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // do_while_st

    // Commits this node.
    void do_while_st_ast_node_t::on_commit()
    {
        __This->condition = __to_eobject<expression_t *>(__Node::condition);
        __This->body      = __to_eobject<statement_t *>(__Node::body);
    }

    // Walks this node.
    void do_while_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__This->condition, this, __c_t::condition_missing, _T("do while"));
        this->__check_empty(__This->body, this, __c_t::body_missing, _T("do while"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // loop_until_st

    // Commits this node.
    void loop_until_st_ast_node_t::on_commit()
    {
        __This->condition = __to_eobject<expression_t *>(__Node::condition);
        __This->body      = __to_eobject<statement_t *>(__Node::body);
    }

    // Walks this node.
    void loop_until_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__This->condition, this, __c_t::condition_missing, _T("loop until"));
        this->__check_empty(__This->body, this, __c_t::body_missing, _T("loop until"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // while_st

    // Commits this node.
    void while_st_ast_node_t::on_commit()
    {
        __This->condition = __to_eobject<expression_t *>(__Node::condition);
        __This->body      = __to_eobject<statement_t *>(__Node::body);
    }

    // Walks this node.
    void while_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__This->condition, this, __c_t::condition_missing, _T("while"));
        this->__check_empty(__This->body, this, __c_t::body_missing, _T("while"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // for_st

    // Commits this node.
    void for_st_ast_node_t::on_commit()
    {
        auto init_statement = __try_to_eobject<statement_t *>(__Node::initialize);
        if(init_statement != nullptr)
            __This->defination_initialize = _M(defination_statement_t *, init_statement);
        else
            __This->initialize = __to_eobject<expression_t *>(__Node::initialize);

        __This->condition  = __to_eobject<expression_t *>(__Node::condition);
        __This->increase   = __to_eobject<expression_t *>(__Node::increase);
        __This->body       = __to_eobject<statement_t *>(__Node::body);
    }

    // Walks this node.
    void for_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__This->body, this, __c_t::body_missing, _T("while"));

        if(__This->defination_initialize != nullptr)
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
        this->__assign_name(__This->variable, name, el, _T("var"));
    }

    // Commits this node.
    void for_each_st_ast_node_t::on_commit()
    {
        __This->iterator   = __to_eobject<expression_t *>(__Node::iterator);
        __This->type_name  = __to_eobject<type_name_t *>(__Node::type_name);
        __This->body       = __to_eobject<statement_t *>(__Node::body);
    }

    // Walks this node.
    void for_each_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__This->variable, _T("variable"));
        this->__check_empty(__This->iterator, this, __c_t::iterator_missing, "for each");
        this->__check_empty(__This->body, this, __c_t::body_missing, _T("for each"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // if_st

    // Commits this node.
    void if_st_ast_node_t::on_commit()
    {
        __This->condition   = __to_eobject<expression_t *>(__Node::condition);
        __This->if_body     = __to_eobject<statement_t *>(__Node::if_body);
        __This->else_body   = __to_eobject<statement_t *>(__Node::else_body);

        this->__check_empty(__This->condition, this, __c_t::condition_missing, "if");
        this->__check_empty(__This->if_body, this, __c_t::body_missing, _T("if"));
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
        __This->expression = __to_eobject<expression_t *>(__Node::expression);
        this->transform_child_to(__Node::cases, __This->cases);
    }

    // Walks this node.
    void switch_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__This->expression, this, __c_t::expression_missing, _T("switch"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // case

    // Appends default expression.
    void case_ast_node_t::append_default(__el_t * el)
    {
        auto * exp_node = to_fake_ast<(__value_t)__cvalue_t::expression>(
            (expression_t *)nullptr, __get_memory(), el->get_code_unit()
        );

        this->append_child(__Node::constants, exp_node);
    }

    // Commits this node.
    void case_ast_node_t::on_commit()
    {
        this->transform_child_to(__Node::constants, __This->constants);
        __This->statements = __to_eobject<statements_t *>(__Node::statements);
    }

    // Walks this node.
    void case_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__This->constants, this, __c_t::expression_missing, _T("case"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // try_st

    // Commits this node.
    void try_st_ast_node_t::on_commit()
    {
        __This->try_statement     = __to_eobject<statement_t *>(__Node::try_);
        __This->finally_statement = __to_eobject<statement_t *>(__Node::finally);

        this->transform_child_to(__Node::catches, __This->catches);
    }

    // Walks this node.
    void try_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if(__This->catches.empty() && __This->finally_statement == nullptr)
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
        this->__assign_name(__This->name, name, el, _T("variable"));
        __variable_el = el;
    }

    // Commits this node.
    void catch_ast_node_t::on_commit()
    {
        __This->type_name = __to_eobject<type_name_t *>(__Node::type_name);
        __This->body      = __to_eobject<statement_t *>(__Node::body);
    }

    // Walks this node.
    void catch_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if(__This->type_name == nullptr && !__This->name.empty())
        {
            this->__log(__variable_el, __c_t::type_name_missing, _T("catch statement"));
        }
        else
        {
            variable_defination_t vd(this->__context, context, __variable_el);
            __This->variable = vd.define_local(__This->type_name, __This->name);
        }

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // empty_st

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
        this->transform_child_to(__Node::statements, *__This);
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
        __This->type_name = __to_eobject<type_name_t *>(__Node::type_name);
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
        __This->type_name  = __to_eobject<type_name_t *>(__Node::type_name);
        __This->expression = __to_eobject<expression_t *>(__Node::expression);
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
        __This->namex         = __to_eobject<expression_t *>(__Node::namex);
        __This->generic_args  = __to_eobject<generic_args_t *>(__Node::generic_args);

        __This->set_arguments(__to_eobject<arguments_t *>(__Node::arguments));
    }

    // Walks this node.
    void function_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__This->namex, _T("function"));

        if(__This->generic_args != nullptr && __This->get_name() == name_t::null)
        {
            this->__log(
                this->child_at(__Node::generic_args), __c_t::function_generic_args_redundance
            );
        }

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // index

    // Commits this node.
    void index_ast_node_t::on_commit()
    {
        __This->set_namex(__to_eobject<expression_t *>(__Node::namex));

        __This->set_arguments(__to_eobject<arguments_t *>(__Node::arguments));
    }

    // Walks this node.
    void index_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__This->namex(), _T("index"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // new

    // Commits this node.
    void new_ast_node_t::on_commit()
    {
        __This->type_name = __to_eobject<type_name_t *>(__Node::type_name);
        __This->set_arguments(__to_eobject<arguments_t *>(__Node::arguments));
    }

    // Walks this node.
    void new_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__This->type_name, this, __c_t::type_name_missing,
                                                        _T("object creation"));
        __super_t::on_walk(context, step, tag);

        switch((walk_step_t)step)
        {
            case walk_step_t::default_:
                this->__delay(context, walk_step_t::analysis);
                __super_t::on_walk(context, step, tag);
                break;

            case walk_step_t::analysis: {
                type_t * type = to_type(__This->type_name);
                if(type != nullptr)
                {
                    __This->constructor = find_constructor(
                        this->__context, type, __This->arguments()
                    );

                    if(__This->constructor == nullptr)
                    {
                        __log(__This, __c_t::type_constructor_not_match, type,
                            to_arg_types_str(this->__context, __This->arguments())
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
        __This->type_name = __to_eobject<type_name_t *>(__Node::type_name);
        __This->set_lengths(__to_eobject<array_lengths_t *>(__Node::lengths));
        __This->set_initializer(__to_eobject<array_initializer_t *>(__Node::initializer));
    }

    const array_length_t unkown_array_length = -1;

    // Walks this eobject.
    void new_array_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch((walk_step_t)step)
        {
            case walk_step_t::default_:
                __walk_default(context);

                __super_t::on_walk(context, step, tag);

                this->__delay(context, walk_step_t::confirm);

                if(__This->initializer() != nullptr)
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
        this->__check_empty(__This->type_name, this, __c_t::type_name_missing,
                                                            _T("array creation"));
        this->__check_empty(__This->lengths(), this, __c_t::array_lengths_missing);

        // dimension
        array_lengths_t * p_lengths = this->__to_eobject<array_lengths_t *>(__Node::lengths);
        if(p_lengths != nullptr)
        {
            array_lengths_t & lengths = *p_lengths;
            for(int dimension = 0, dimensions = lengths.size(); dimension < dimensions;
                                                                  dimension++)
            {
                expression_t * exp = lengths[dimension];
                if(exp == nullptr)
                {
                    array_length_t length = __get_length(dimension);
                    if(length != unkown_array_length)
                        __This->set_length(dimension, __to_value_expression(length));
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
        __This->to_array_type(this->__get_xpool());
    }

    // Walks analysis step.
    void new_array_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        array_initializer_t * initializer = __This->initializer();
        if(initializer != nullptr)
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
        array_initializer_t * initializer = __This->initializer();

        while(dimension-- > 0 && initializer != nullptr)
        {
            array_initialize_element_t element = (*initializer)[0];
            if(element.type != array_initialize_element_type_t::initializer)
                return unkown_array_length;

            initializer = element.initializer;
        }

        if(initializer == nullptr)
            return unkown_array_length;

        return initializer->size();
    }

    ////////// ////////// ////////// ////////// //////////
    // array_initializer

    // Commits this node.
    void array_initializer_ast_node_t::on_commit()
    {
        this->each_child_node<ast_node_t>(__Node::elements, [&](ast_node_t * node) {

            expression_ast_t * expression_ast;
            array_initializer_ast_t * initializer_ast;

            if((expression_ast = as<expression_ast_t *>(node)) != nullptr)
            {
                __This->append(expression_ast->to_eobject());
            }
            else if((initializer_ast = as<array_initializer_ast_t *>(node)) != nullptr)
            {
                __This->append(initializer_ast->to_eobject());
            }
            else
            {
                X_UNEXPECTED();
            }
        });
    }

    // Walks this node.
    void array_initializer_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        try
        {
            __This->check();
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
        this->transform_child_to(__Node::lengths, *__This);
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
        __This->type_name = __to_eobject<type_name_t *>(__Node::type_name);
    }

    // Walks this node.
    void default_value_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__This->type_name, this, __c_t::type_name_missing,
                                                        _T("default value"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // field

    // Sets name.
    void field_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__This->name, name, el, _T("field"));
    }

    // Commits this node.
    void field_ast_node_t::on_commit()
    {
        __This->decorate   = __to_eobject<decorate_t *>(__Node::decorate);
        __This->type_name  = __to_eobject<type_name_t *>(__Node::type_name);
        __This->init_value = __to_eobject<expression_t *>(__Node::init_value);
        __This->attributes = __to_eobject_with_owner<attributes_t *>(__Node::attributes,
                                                                    __This);
    }

    // Walks this node.
    void field_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        context.push(this->to_eobject());
        __super_t::on_walk(context, step, tag);
        context.pop();

        this->__check_empty(__This->type_name, this, __c_t::type_name_missing,
                _F(_T("field %1%"), _str(__This->name)));
        this->__check_empty(__This->name, this, __c_t::name_empty, _T("field"));

        variable_defination_t vd(this->__context, context,
            static_cast<code_element_t *>(this)
        );

        vd.define_field(__This);
    }

    ////////// ////////// ////////// ////////// //////////
    // method

    // Sets name.
    void method_ast_node_t::set_name(name_t name, __el_t * el)
    {
        if(__op_property != nullptr)
        {
            __log(el, __c_t::duplicate, _T("method name / operator overload"), name);
            return;
        }

        this->__assign_name(__This->name, name, el, _T("method"));
    }

    // Sets operator. ( for operator overloading. )
    void method_ast_node_t::set_operator(const operator_property_t * op_property, __el_t * el)
    {
        _A(op_property != nullptr);

        if(__op_property != nullptr)
        {
            __log(el, __c_t::duplicate, _F(_T("operator overload")), op_property);
            return;
        }

        if(!__This->name.empty())
        {
            __log(el, __c_t::duplicate, _F(_T("method name / operator overload")), op_property);
            return;
        }

        this->__op_property = op_property;

        const char_t * op_name = op_property->name;
        _A(op_name != nullptr);

        __This->name = __to_name(_F(_T("op_%1%"), op_name));
    }

    // Sets trait.
    void method_ast_node_t::set_trait(method_trait_t trait, __el_t * el)
    {
        this->__assign(__This->trait, trait, el, _T("method trait"));
    }

    // Commits it.
    void method_ast_node_t::on_commit()
    {
        __This->type_name       = __to_eobject<type_name_t *>(__Node::type_name);
        __This->decorate        = __to_eobject<decorate_t *>(__Node::decorate);
        __This->params          = __to_eobject<params_t *>(__Node::params);
        __This->generic_params  = __to_eobject<generic_params_t *>(__Node::generic_params);
        __This->owner_type_name = __to_eobject<type_name_t *>(__Node::owner_type_name);
        __This->attributes      = __to_eobject_with_owner<attributes_t *>(__Node::attributes,
                                                                        __This);
        __This->body            = __to_eobject<method_body_t *>(__Node::body);
    }

    // Walks this node.
    void method_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch((walk_step_t)step)
        {
            case walk_step_t::default_:
                context.push(__This);

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
        if(type != nullptr && __This->name == type->get_name()
                                && __This->owner_type_name == nullptr)  // constructor
        {
            if(__This->trait == method_trait_t::__default__)
            {
                if(__This->decorate && __This->decorate->is_static)
                    __This->trait = method_trait_t::static_constructor;
                else
                    __This->trait = method_trait_t::constructor;
            }
        }
        else
        {
            if(__This->trait == method_trait_t::__default__)
                __This->trait = method_trait_t::normal;

            this->__check_empty(__This->type_name, this, __c_t::type_name_missing,
                _F(_T("method %1%"), _str(__This->name))
            );
        }

        if(__This->type_name != nullptr)
        {
            switch(__This->trait)
            {
                case method_trait_t::constructor:
                    this->__log(this, __c_t::constructor_method_should_no_return_type,
                                                        _str(__This->name));
                    break;

                case method_trait_t::static_constructor:
                    this->__log(this, __c_t::static_constructor_method_should_no_return_type,
                                                        _str(__This->name));
                    break;

                case method_trait_t::destructor:
                    this->__log(this, __c_t::destructor_method_should_no_return_type,
                                                        _str(__This->name));
                    break;

                default:
                    break;
            }
        }

        this->__check_empty(__This->name, this, __c_t::name_empty, _T("method"));

        if(__This->params && !__This->params->empty())
        {
            variable_defination_t vd(this->__context, context,
                static_cast<code_element_t *>(this)
            );

            for(param_t * param : *__This->params)
            {
                vd.define_param(param);
            }

            switch(__This->trait)
            {
                case method_trait_t::static_constructor:
                    this->__log(this, __c_t::static_constructor_method_should_no_params,
                                                        _str(__This->name));
                    break;

                case method_trait_t::destructor:
                    this->__log(this, __c_t::destructor_method_should_no_params,
                                                        _str(__This->name));
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
        switch(__This->trait)
        {
            case method_trait_t::static_constructor:
                break;

            case method_trait_t::constructor:
                __method_utils_t(__context, context).revise_constructor(*__This,
                    __to_eobject<arguments_t *>(base_ctor_args), this->child_at(base_ctor_args)
                );
                break;

            case method_trait_t::destructor:
                break;

            default:
                break;
        }

        // Operator overload, checks prototype.
        if(__op_property == nullptr)
            return;

        if(!__This->is_static() || !__This->is_public())
            this->__log(this, __c_t::operator_overload_prototype_error);

        if(__This->param_count() != __op_property->arity)
        {
            this->__log(this, (__op_property->arity == 1)?
                    __c_t::unitary_operator_overload_wrong_param_count :
                    __c_t::binary_operator_overload_wrong_param_count, __op_property->op
            );
        }

        bool has_containing_type = al::any_of(*__This->params, [&](param_t * param) {
            return param->get_type() == context.current_type();
        });

        if(!has_containing_type)
        {
            this->__log(this, (__op_property->arity == 1)?
                    __c_t::unitary_operator_overload_wrong_containing_type :
                    __c_t::binary_operator_overload_wrong_containing_type, __op_property->op
            );
        }

        type_t * return_type = __This->get_type();
        if(return_type == nullptr || is_void_type(return_type))
        {
            this->__log(this, __c_t::operator_overload_cannot_return_void);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // property

    // Sets name.
    void property_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__This->name, name, el, _T("property"));
    }

    // Commits this node.
    void property_ast_node_t::on_commit()
    {
        __This->type_name  = __to_eobject<type_name_t *>(__Node::type_name);
        __This->decorate   = __to_eobject<decorate_t *>(__Node::decorate);
        __This->get_method = __to_eobject<method_t *>(__Node::get_method);
        __This->set_method = __to_eobject<method_t *>(__Node::set_method);
        __This->params     = __to_eobject<params_t *>(__Node::params);
        __This->owner_type_name = __to_eobject<type_name_t *>(__Node::owner_type_name);
        __This->attributes = __to_eobject_with_owner<attributes_t *>(__Node::attributes, __This);
    }

    // Appends method.
    bool __append_member(ast_context_t & cctx, ast_walk_context_t & wctx, member_t * member)
    {
        type_t * type = wctx.current_type();
        if(type == nullptr || type->this_gtype() != gtype_t::general)
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
        if(!__This->name.empty())
        {
            if(__This->params != nullptr)
                this->__log(this->child_at(__Node::params), __c_t::unexpected_params, "property");
        }
        else
        {
            if(__This->params == nullptr)
                this->__log(this, __c_t::name_empty, _T("property"));
        }

        if(__This->get_method != nullptr)
        {
            __This->get_method->name = __to_method_name(_T("get"));
            __This->get_method->type_name = __This->type_name;
            __This->get_method->decorate  = __This->decorate;
            __This->get_method->params    = __This->params;
            __append_member(this->__context, context, __This->get_method);
        }

        param_t * value_param = nullptr;

        if(__This->set_method != nullptr)
        {
            __This->set_method->name = __to_method_name(_T("set"));
            __This->set_method->type_name = context.to_type_name(vtype_t::void_);
            __This->set_method->decorate  = __This->decorate;

            params_t * params = __new_obj<params_t>();

            if(__This->params != nullptr)
                al::copy(*__This->params, std::back_inserter(*params));

            value_param = __new_obj<param_t>(__This->type_name, __to_name(_T("value")));
            params->push_back(value_param);

            __This->set_method->params = params;
            __append_member(this->__context, context, __This->set_method);
        }

        __generate_ignored_method_bodies(context, value_param);

        variable_region_t * region = context.current_region();

        variable_defination_t vd(this->__context, context,
            static_cast<code_element_t *>(this)
        );

        vd.define_property(__This);

        context.push(this->to_eobject());
        __super_t::on_walk(context, step, tag);
        context.pop();
    }

    // Generates ignored method bodies.
    void property_ast_node_t::__generate_ignored_method_bodies(ast_walk_context_t & context,
                                                               param_t * value_param)
    {
        #define __AccessorEmpty(name)                                           \
            (__This->name##_method == nullptr)

        #define __BodyEmpty(name)                                               \
            (__This->name##_method->body == nullptr)

        #define __BodyIgnored(name)                                             \
            (!__AccessorEmpty(name)  && __BodyEmpty(name))

        if(__AccessorEmpty(get) && __AccessorEmpty(set))
        {
            this->__log(__This, __c_t::property_accessor_missing, __This);
            return;
        }

        if(!__AccessorEmpty(get) && !__AccessorEmpty(set))
        {
            if(__BodyEmpty(get) != __BodyEmpty(set))
            {
                this->__log(__This, __c_t::property_method_body_missing,
                    __BodyEmpty(set)? _T("set") : _T("get")
                );

                return;
            }
        }

        if(!__BodyIgnored(get) && !__BodyIgnored(set))
            return;

        // Appends a field for the property.
        field_t * field = __new_obj<field_t>();
        field->type_name = __This->type_name;
        field->name = __to_name(_F(_T("__%1%__"), __This->name));

        decorate_value_t dv = decorate_value_t::default_value;
        dv.is_static = __This->is_static();
        dv.access = access_value_t::private_;

        field->decorate = __new_obj<decorate_t>(dv);

        __append_member(this->__context, context, field);

        // Appends get method body.
        if(__BodyIgnored(get))
        {
            statement_t * statement = __new_obj<return_statement_t>(
                __new_field_expression(field)
            );

            __This->get_method->body = __new_obj<method_body_t>();
            __This->get_method->body->push_back(statement);
        }

        if(__BodyIgnored(set))
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

            __This->set_method->body = body;
        }

        #undef __AccessorEmpty
        #undef __BodyEmpty
        #undef __BodyIgnored
    }

    // Gets get/set method name.
    name_t property_ast_node_t::__to_method_name(const char_t * prefix)
    {
        if(__This->name != name_t::null)
            return __to_name(_F(_T("%1%_%2%"), prefix, __This->name));

        return __to_name(_F(_T("%1%_Item"), prefix));
    }

    ////////// ////////// ////////// ////////// //////////
    // event

    // Sets name.
    void event_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__This->name, name, el, _T("event"));
    }

    // Commits this node.
    void event_ast_node_t::on_commit()
    {
        __This->type_name       = __to_eobject<type_name_t *>(__Node::type_name);
        __This->decorate        = __to_eobject<decorate_t *>(__Node::decorate);
        __This->add_method      = __to_eobject<method_t *>(__Node::add_method);
        __This->remove_method   = __to_eobject<method_t *>(__Node::remove_method);
        __This->owner_type_name = __to_eobject<type_name_t *>(__Node::owner_type_name);
        __This->attributes      = __to_eobject_with_owner<attributes_t *>(__Node::attributes,
                                                                        __This);
    }

    // Walks this node.
    void event_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__This->name, _T("event"));

        if(__This->add_method != nullptr)
        {
            __This->add_method->name = __to_name(_F(_T("add_%1%"), __This->name));
            __This->add_method->type_name = __This->type_name;
            __append_member(this->__context, context, __This->add_method);
        }

        if(__This->remove_method != nullptr)
        {
            __This->remove_method->name = __to_name(_F(_T("remove_%1%"), __This->name));
            __This->remove_method->type_name = __This->type_name;
            __append_member(this->__context, context, __This->remove_method);
        }

        context.push(this->to_eobject());
        __super_t::on_walk(context, step, tag);
        context.pop();
    }

    ////////// ////////// ////////// ////////// //////////

    #undef __This
    #undef __Node

} } }  // namespace X_ROOT_NS::modules::compile

