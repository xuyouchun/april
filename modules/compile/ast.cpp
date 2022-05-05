
#include <compile.h>

namespace X_ROOT_NS::modules::compile {

    typedef common_log_code_t __c_t;
    typedef code_element_t    __el_t;

    using namespace core;

    #define __Log(_code, _args...)                                                      \
        this->__log(this, __c_t::_code, ##_args)

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

        // Name unit.
        X_C(name_unit,                  _T("name_unit"))

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

        // Hides inherit virtual member..
        X_D(hides_inherit_virtual_member, _T("'%1%' hides inherited member '%2%', ")
                    _T("to make the current member override that implemenation, ")
                    _T("add the override keyword, otherwise add the new keyword"))

        // Missing 'new'
        X_D(hides_inherit_member, _T("'%1%' hides inherited member '%2%', ")
                    _T("use the new keyword if hiding is intended"))

        // The member does not hide an accessible member. The new keyword is not required.
        X_D(not_hides_inherit_member, _T("The member '%1%' does not hide an accessible member. ")
                                _T("The new keyword is not required."))

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

        // Unexpected typedef decorate.
        X_D(unexpected_type_def_decorate,
                _T("decorates can only be defined on type_def when it is a member of types"))

        // Unexpected typedef decorate.
        X_D(unexpected_type_def_attribute, _T("attributes cannot be defined on type_def statements"))

        // Unexpected method defination.
        X_D(unexpected_method_defination, _T("methods cannot be defined here: \"%1%\""))

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

        // Invalid expression.
        X_D(invalid_expression, _T("invalid expression %1%"))

        // Member not found.
        X_D(member_not_found, _T("member \"%1%\" undefined in type \"%2%\""))

        // Member defination duplicated.
        X_D(member_defination_duplicated,
                            _T("Type '%1%' already defines a member '%2%' with the same name"))

        // Member names cannot be the same as their enclosing type.
        X_D(member_same_as_enclosing_type,
                            _T("Member names cannot be the same as their enclosing type."))

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
        X_D(constructor_method_not_found, _T("constructor method %1%(%2%) not found"))

        // Costom struct type should not define a default constructor.
        X_D(do_not_need_default_constructor,
                        _T("custom struct type %1% should not define a default constructor"))

        // Partial specialization not supported.
        X_D(partial_specialization_not_supported,
                                _T("partial specialization of generic type not supported"))

        // No base type.
        X_D(no_base_type, _T("type '%1%' has no base type"))

        // Circular base type dependency.
        X_D(circle_base_type_dependency,
                                _T("Circular base type dependency involving '%1% and '%2%'"))

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
                _T("The parameter of unitary operator '%1%' overloaded must be the ")
                _T("containing type"))

        // The parameter must be the containing type.
        X_D(binary_operator_overload_wrong_containing_type,
                _T("One of the parameters of binary operator '%1%' overloaded must be the ")
                _T("containing type"))

        // User-defined operators cannot return void.
        X_D(operator_overload_cannot_return_void, _T("User-defined operators cannot return void"))

        // Operator cannot ber overloaded.
        X_D(operator_cannot_be_overloaded, _T("Operator '%1%' cannot be overloaded"))

        // Operator overload not defined.
        X_D(operator_overload_not_defined, _T("Operator '%1%' not defined for %2%"))

        // Cannot determine local type.
        X_D(cannot_determine_local_variable_type,
                                            _T("Cannot determine type of local variable '%1%'"))

        // Cannot find method with prototype.
        X_D(cannot_find_method_with_prototype,
                        _T("Cannot find method with prototype %1%"))

        // Unexpected decorate on enum fields.
        X_D(unexpected_enum_decorate, _T("Unexpected keyword '%1%' on enum field %2%"))

        // Unexpected type on enum fields.
        X_D(unexpected_enum_type, _T("Unexpected type '%1%' on enum field %2%"))

        // Enum field prototype error.
        X_D(enum_field_prototype_error, _T("Enum field '%1%' should be static and const"))

        // Expect constant value.
        X_D(expect_constant_value,
                            _T("The expression '%1%' assigned to %2% must be constant"))

        // Type int or unsigned int type expected.
        X_D(unexpected_underlying_type,
                _T("Type int or unsigned int type expected, (include short or long types)"))

        // The enum value is too large
        X_D(enum_value_too_large,
                _T("The enum value %1% is too large for underlying type %2%"))

        // The enum value is too small
        X_D(enum_value_too_small,
                _T("The enum value %1% is too small for underlying type %2%"))

        // The evaluation of the constant value involves a circular definition.
        X_D(enum_field_circular_definition,
                _T("The evaluation of the constant value for '%1%' involves a circular definition"))

        // Member is inaccessible to its protection level.
        X_D(inaccessible_protection_level,
                _T("%1% is inaccessible to its protection level"))

        // Cannot change access modifiers when overriding inherited member.
        X_D(access_modifier_changed,
                _T("%1%: cannot change access modifiers when overriding '%2%' inherited ")
                _T("member '%2%'"))

        // Cannot change return types when override inherited members.
        X_D(return_type_changed,
                _T("%1%: return type must be '%2%' to match overridden member '%3%'"))

        // Virtual or abstract members cannot be private.
        X_D(virtual_member_cannot_be_private,
                _T("%1%: virtual or abstract members cannot be private"))

        // A static member cannot be marked as override, virtual, or abstract.
        X_D(static_member_cannot_be_virtual,
                _T("A static member '%1%' cannot be marked as override, virtual, or abstract"))

        // A member marked as override cannot be marked as new or virtual
        X_D(override_member_cannot_be_new,
                _T("A member '%1%' marked as override cannot be marked as new or virtual"))

        // member is abstract but it is contained in non-abstract type.
        X_D(abstract_member_on_non_abstract_type,
                _T("'%1%' is abstract but it is contained in non-abstract type '%2%'"))

        // The modifier is not valid for this item.
        X_D(modifier_not_valid,
                _T("The modifier '%1%' is not valid for this item"))

        // The modifier is not valid for interface members.
        X_D(modifier_not_valid_for_interface_members,
                _T("The modifier '%1%' is not valid for interface members"))

        // Extern members should be marked as static.
        X_D(extern_member_should_be_static,
                _T("Extern member '%1%' should be marked as static"))

        // Extern method cannot declare a body.
        X_D(extern_method_cannot_declare_body,
                _T("Extern method '%1%' cannot declare a body"))

        // Abstract method cannot declare a body.
        X_D(abstract_method_cannot_declare_body,
                _T("'%1%': cannot declare a body because it's marked as abstract"))

        // Cannot be sealed because it is not an override member.
        X_D(sealed_member_but_not_override,
                _T("'%1%': cannot be sealed because it is not an override member."))

        // No suitable member found to override       
        X_D(no_suitable_member_to_override,
                _T("'%1%': no suitable member found to override"))

        // cannot override inherited member because it is sealed.
        X_D(cannot_override_sealed_members,
                _T("'%1%': cannot override inherited member '%2%' because it is sealed"))

        // Return type changed when override inherit members.
        X_D(override_member_return_type_changed,
                _T("'%1%': return type must be '%2%' to match overridden member '%3%'"))

        // Interface method cannot declare a body.
        X_D(interface_members_cannot_have_defination,
                _T("'%1%': interface members cannot have a defination"))

        // Interfaces cannot contain instance fields.
        X_D(interfaces_cannot_contain_instance_fields,
                _T("interfaces cannot contain instance fields"))

        // Cannot implicitly convert a type to another type.
        X_D(invalid_type_cast, _T("Cannot implicitly convert type '%1%' to '%2%'"))

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
    // __utils_t

    // Check member duplicate or format correct.
    template<typename _code_element_t>
    bool __utils_t::__check_duplicate(type_t * type, member_t * member,
                                                _code_element_t * code_element)
    {
        _A(type != nullptr);
        _A(member != nullptr);

        typedef common_log_code_t c_t;

        code_element_t * ce = dynamic_cast<code_element_t *>(code_element);
        if (ce == nullptr)
            ce = (code_element_t *)this;

        // Same as type name.
        if (member->get_name() == type->get_name())
        {
            if (member->this_type() != member_type_t::method ||
                !is_constructor_or_destructor((method_t *)member))
            {
                this->__log(ce, c_t::member_same_as_enclosing_type);
                return false;
            }
        }

        // Check duplicate with self class.
        {
            member_t * member1 = type->check_duplicate(member);
            if (member1 != nullptr)
            {
                this->__log(ce, c_t::member_defination_duplicated, type, member1);
                return false;
            }
        }

        return true;

        #undef __Return
    }

    // Check member decorate corrected.
    template<typename _member_t, typename _code_element_t>
    bool __utils_t::__check_modifier(type_t * type, _member_t * member,
                                                _code_element_t * code_element)
    {
        typedef common_log_code_t c_t;
        typedef member_type_t m_t;

        code_element_t * ce = dynamic_cast<code_element_t *>(code_element);
        if (ce == nullptr)
            ce = (code_element_t *)this;

        member_type_t member_type = member->this_type();

        type_t * base_type = type->get_base_type();
        member_t * base_member = nullptr;

        if (base_type != nullptr)
        {
            member_t * base_member1 = base_type->check_duplicate(member, true);
            if (base_member1 != nullptr && base_member1->host_type != type)
                base_member = base_member1;
        }

        // Interfaces.
        if (type->this_ttype() == ttype_t::interface_)
        {
            // Interfaces cannot contain instance fields.
            if (al::in(member_type, m_t::field))
            {
                this->__log(ce, c_t::interfaces_cannot_contain_instance_fields);
                return false;
            }

            // Modifier not valid for interface members.
            const char_t * modifier = 
                member->is_virtual()?   _T("virtual") :
                member->is_static()?    _T("static") :
                member->is_abstract()?  _T("abstract") :
                member->is_sealed()?    _T("sealed") :
                member->is_const()?     _T("const") :
                member->is_readonly()?  _T("readonly") :
                member->is_extern()?    _T("extern") :
                member->has_access_modifier()? _T("access modifier") : nullptr
            ;

            if (modifier != nullptr)
            {
                this->__log(ce, c_t::modifier_not_valid_for_interface_members, modifier);
                return false;
            }

            // Interface methods cannot have defination.
            if (member_type == m_t::method)
            {
                method_t * method = (method_t *)member;
                if (method->body != nullptr)
                {
                    this->__log(ce, c_t::interface_members_cannot_have_defination);
                    return false;
                }
            }

            // Interface properties cannot have defination.
            if (member_type == m_t::property)
            {
                property_t * property = (property_t *)member;

                if (property->get_method != nullptr && property->get_method->body != nullptr)
                {
                    this->__log(property->get_method, c_t::interface_members_cannot_have_defination,
                        property->get_method);

                    return false;
                }

                if (property->set_method != nullptr && property->set_method->body != nullptr)
                {
                    this->__log(property->set_method, c_t::interface_members_cannot_have_defination,
                        property->set_method);

                    return false;
                }
            }
        }

        // Check virtual members.
        else if (al::in(member_type, m_t::property, m_t::method, m_t::event))
        {
            if (is_virtual_or_abstract_or_override(member))
            {
                // Private member cannot be virtual, abstract or override.
                if (is_private(member))
                {
                    this->__log(ce, c_t::virtual_member_cannot_be_private, member->to_prototype());
                    return false;
                }

                // Static member cannot be virtual, abstract or override.
                if (member->is_static())
                {
                    this->__log(ce, c_t::static_member_cannot_be_virtual, member->to_prototype());
                    return false;
                }
            }

            if (member->is_override())
            {
                // Override member cannot be new, virtual.
                if (member->is_virtual() || member->is_new())
                {
                    this->__log(ce, c_t::override_member_cannot_be_new, member->to_prototype());
                    return false;
                }

                // No suitable member to override.
                if (base_member == nullptr || !is_virtual_or_abstract_or_override(base_member))
                {
                    this->__log(ce, c_t::no_suitable_member_to_override, member->to_prototype());
                    return false;
                }

                if (base_member != nullptr)
                {
                    // Cannot override because it is sealed.
                    if (base_member->is_sealed())
                    {
                        this->__log(ce, c_t::cannot_override_sealed_members, member->to_prototype(),
                            base_member->to_prototype());
                        return false;
                    }

                    // Return type changed.
                    if (base_member->get_type() != member->get_type())
                    {
                        this->__log(ce, c_t::return_type_changed, member->to_prototype(),
                            base_member->get_type(), base_member->to_prototype());
                        return false;
                    }
                }
            }

            // Member is abstract but in non-abstract type.
            if (member->is_abstract() && !type->is_abstract())
            {
                this->__log(ce, c_t::abstract_member_on_non_abstract_type, member->to_prototype(),
                    type->to_prototype());

                return false;
            }

            // Sealed member but not override.
            if (member->is_sealed() && !member->is_override())
            {
                this->__log(ce, c_t::sealed_member_but_not_override, member->to_prototype());
                return false;
            }

            // Other modifiers.
            const char_t * modifier = 
                member->is_readonly()?  _T("readonly") :
                member->is_const()?     _T("const") : nullptr;

            if (modifier != nullptr)
            {
                this->__log(ce, c_t::modifier_not_valid, modifier);
                return false;
            }

            // Extern method.
            if (member->is_extern())
            {
                if (member_type != m_t::method)
                {
                    this->__log(ce, c_t::modifier_not_valid, modifier);
                    return false;
                }

                if (!member->is_static())
                {
                    this->__log(ce, c_t::extern_member_should_be_static, member->to_prototype());
                    return false;
                }

                method_t * method = (method_t *)member;
                if (method->body != nullptr)
                {
                    this->__log(ce, c_t::extern_method_cannot_declare_body, member->to_prototype());
                    return false;
                }
            }
        }
        else    // Field, typedef
        {
            const char_t * modifier = 
                member->is_virtual()?   _T("virtual") :
                member->is_abstract()?  _T("abstract") :
                member->is_override()?  _T("override") :
                member->is_extern() ?   _T("extern") :
                member->is_sealed() ?   _T("sealed") : nullptr;

            if (modifier != nullptr)
            {
                this->__log(ce, c_t::modifier_not_valid, modifier);
                return false;
            }

            if (member_type == member_type_t::type_def)
            {
                const char_t * modifier1 =
                    member->is_const()?     _T("const") :
                    member->is_readonly()?  _T("readonly") :
                    member->is_static()?    _T("static") : nullptr;

                if (modifier1 != nullptr)
                {
                    this->__log(ce, c_t::modifier_not_valid, modifier1);
                    return false;
                }
            }
        }

        // Check duplicate with base class.
        if (base_member != nullptr && !is_private(base_member))
        {
            if (!is_private(member))
            {
                if (member->this_type() == base_member->this_type()
                    && is_virtual_or_abstract_or_override(base_member))
                {
                    if (!member->is_override() && !member->is_new())
                    {
                        // Missing override or new keywords.
                        this->__log(ce, c_t::hides_inherit_virtual_member,
                            member->to_prototype(), base_member->to_prototype()
                        );
                    }
                    else if (!is_same_access_modifier(member, base_member))
                    {
                        // Access modifier changed.
                        this->__log(ce, c_t::access_modifier_changed,
                            member->to_prototype(), base_member->get_access_value(),
                            base_member->to_prototype());

                        return false;
                    }
                }
                else
                {
                    if (!member->is_new())
                    {
                        // Missing new keyword.
                        this->__log(ce, c_t::hides_inherit_member,
                            member->to_prototype(), base_member->to_prototype()
                        );
                    }
                }
            }
            else if (member->is_new())
            {
                // new keyword is not required.
                this->__log(ce, c_t::not_hides_inherit_member, member->to_prototype());
            }
        }

        return true;
    }

    // Check member duplicate and decorate corrected.
    template<typename _member_t, typename _code_element_t>
    bool __utils_t::__check_member(type_t * type, _member_t * member,
                                                    _code_element_t * code_element)
    {
        // Outside types.
        if (type == nullptr)
        {
            // TODO: when outside types.
            return true;
        }

        if (!__check_duplicate(type, member, code_element))
            return false;

        if (!__check_modifier(type, member, code_element))
            return false;

        return true;
    }

    ////////// ////////// ////////// ////////// //////////
    // module

    // Walk this node.
    bool __module_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                return __super_t::on_walk(context, step, tag);

            case walk_step_t::post_confirm: {
                eobject_commit_context_t ctx(context.logger);
                __module->commit(ctx);
                context.commit_types();
                context.delay(this, (int)walk_step_t::end, tag);
                return true;
            }

            case walk_step_t::end:
                context.commit_types();
                return true;

            default: return true;
        }
    }

    // Walk this node.
    bool __module_ast_node_t::do_walk(ast_walk_context_t & context, int step, void * tag)
    {
        bool r = __super_t::do_walk(context, step, tag);
        context.delay(this, (int)walk_step_t::post_confirm, tag);
        return r;
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
            to_mname_operate_context(), __this_mname()
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
    bool mname_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
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
            expression_t * exp = __expressions. expression_at(0);
            return exp->parent = nullptr, exp;
        }

        return &__expressions;
    }

    // Walk this node.
    bool expressions_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
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
    bool type_name_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
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
    name_unit_t::operator string_t() const
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
    // name_unit_t

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
    bool name_unit_t::is_partial_specialization() const
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
    bool general_type_name_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                if (__super_t::on_walk(context, step, tag))
                {
                    this->__delay(context, walk_step_t::confirm);
                    return true;
                }

                return false;

            case walk_step_t::confirm:
                if (__walk_confirm(context))
                {
                    this->__delay(context, walk_step_t::analysis);
                    return true;
                }

                return false;

            case walk_step_t::analysis:
                return __walk_analysis(context);

            default: return true;
        }
    }

    // Walks confirm step.
    bool general_type_name_ast_node_t::__walk_confirm(ast_walk_context_t & context)
    {
        if (__type_name.type == nullptr)
            __type_name.type = __ascertain_type(context, &__type_name);

        return true;
    }

    // Walks analysis step.
    bool general_type_name_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        type_t * type = __type_name.type;

        if (type != nullptr)
            this->__check_access(context, type, this, __type_name);

        return true;
    }

    ////////// ////////// ////////// ////////// //////////
    // name_unit

    // Sets name.
    void name_unit_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__name_unit.name, name, el, _T("type"));
    }

    // Commits this node.
    void name_unit_ast_node_t::on_commit()
    {
        __name_unit.args = __to_eobject<generic_args_t *>(generic_args);
    }

    // Returns this eobject.
    name_unit_t * name_unit_ast_node_t::to_eobject()
    {
        return &__name_unit;
    }

    // Walks this node.
    bool name_unit_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__name_unit.name, this, __c_t::name_empty, _T("type name unit"));

        return __super_t::on_walk(context, step, tag);
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
    bool array_type_name_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                this->__check_empty(__type_name.element_type_name, this,
                                    __c_t::type_name_missing, _T("array"));
                if (__super_t::on_walk(context, step, tag))
                {
                    this->__delay(context, walk_step_t::confirm);
                    return true;
                }

                return false;

            case walk_step_t::confirm:
                return __walk_confirm(context);

            default: return true;
        }
    }

    // Walks confirm step.
    bool array_type_name_ast_node_t::__walk_confirm(ast_walk_context_t & context)
    {
        array_type_name_t * type_name = &__type_name;

        if (type_name->element_type_name != nullptr && type_name->type == nullptr)
        {
            type_name->type = __ascertain_type(context, type_name);
        }

        return true;
    }

    ////////// ////////// ////////// ////////// //////////
    // uncertain_type_name

    // Commits the node.
    void uncertain_type_name_ast_node_t::on_commit() { }

    // Returns this eobject.
    type_name_t * uncertain_type_name_ast_node_t::to_eobject()
    {
        return __new_obj<type_name_t>(uncertain_type_t::instance());
    }

    // Walks this node.
    bool uncertain_type_name_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
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
        // Empty.
    }

    // Returns this eobject.
    type_def_param_t * type_def_param_ast_node_t::to_eobject()
    {
        return &__type_def_param;
    }

    // Walks this node.
    bool type_def_param_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__type_def_param.name, _T("typedef param"));

        return __super_t::on_walk(context, step, tag);
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
    bool type_def_params_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // type_def

    // Sets name.
    void type_def_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__name_el = el;
        this->__assign_name(__type_def.name, name, el, _T("typedef"));
    }

    // Commits this node.
    void type_def_ast_node_t::on_commit()
    {
        __type_def.decorate   = __to_eobject<decorate_t *>(decorate);
        __type_def.attributes = __to_eobject_with_owner<attributes_t *>(attributes, &__type_def);
        __type_def.type_name  = __to_eobject<type_name_t *>(type_name);
        __type_def.params     = __to_eobject<type_def_params_t *>(params);
    }

    // Returns this eobject.
    type_def_t * type_def_ast_node_t::to_eobject()
    {
        return &__type_def;
    }

    // Walks this node.
    bool type_def_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                if (__walk_default(context, step, tag))
                {
                    this->__delay(context, walk_step_t::analysis);
                    return true;
                }

                return false;

            case walk_step_t::analysis:
                return __walk_analysis(context);

            default: return true;
        }
    }

    // Walks default step.
    bool type_def_ast_node_t::__walk_default(ast_walk_context_t & context, int step, void * tag)
    {
        context.push(to_eobject());

        __type_def.namespace_ = context.current_namespace();

        this->__check_empty(__type_def.name, this, __c_t::type_name_missing, _T("typedef"));

        const mname_t * ns = ns_to_full_name(context.current_namespace());

        type_def_t * type_def = to_eobject();
        if (!context.assembly.types.append_type_def(to_sid(ns), type_def))
            this->__log(this, __c_t::type_def_defination_duplicate, type_def);

        if (context.current_type() == nullptr)   // outside types.
        {
            if (__type_def.decorate != nullptr)
                this->__log(this, __c_t::unexpected_type_def_decorate);
        }

        if (context.current_method() != nullptr) // in a method, typedef statement.
        {
            if (__type_def.decorate != nullptr)
                this->__log(this, __c_t::unexpected_type_def_decorate);

            if (__type_def.attributes != nullptr)
                this->__log(this, __c_t::unexpected_type_def_attribute);
        }

        bool r = __super_t::on_walk(context, step, tag);
        context.pop();

        return r;
    }

    // Walks analysis step.
    bool type_def_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        this->__check_member(context.current_type(), &__type_def, __name_el);
        return true;
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
    bool type_of_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__type_of.type_name, this, __c_t::type_name_missing, _T("typeof"));

        return __super_t::on_walk(context, step, tag);
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
    bool argument_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if (!__super_t::on_walk(context, step, tag))
            return false;

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

        return true;
    }

    ////////// ////////// ////////// ////////// //////////
    // arguments

    // Commits this node.
    void arguments_ast_node_t::on_commit()
    {
        this->transform_child_to(arguments, __arguments);
    }

    // Walks this node.
    bool arguments_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
    }

    // Returns this eobject.
    arguments_t * arguments_ast_node_t::to_eobject()
    {
        return &__arguments;
    }

    ////////// ////////// ////////// ////////// //////////
    // cvalue

    // Walks this node.
    bool cvalue_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
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
    template<typename _getf_t, typename _setf_t>
    struct __bit_field_wrap_t
    {
        __bit_field_wrap_t(_getf_t getf, _setf_t setf) : __getf(getf), __setf(setf) { }

        _getf_t __getf;
        _setf_t __setf;

        typedef decltype(__getf()) __value_t;

        operator __value_t() const { return __getf(); }
        __value_t operator = (const __value_t & value) { __setf(value); return value; }
        bool operator == (const __value_t & value) const { return (__value_t)*this == value; }
        bool operator != (const __value_t & value) const { return !operator == (value); }
    };

    #define __BitField(obj, field)                                                      \
        __bit_field_wrap_t(                                                             \
                [this]() { return obj.field; },                                         \
                [this](auto value) { obj.field = value; }                               \
        )

    // Walks this node.
    bool decorate_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
    }

    // Sets access.
    void decorate_ast_node_t::set_access(access_value_t value, __el_t * el)
    {
        this->__assign(__BitField(__decorate, access), value, el, _T("access"), true);
    }

    // Get access.
    access_value_t decorate_ast_node_t::get_access()
    {
        return __decorate.access;
    }

    // Sets static.
    void decorate_ast_node_t::set_static(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_static), true, el, _T("static"), true);
    }

    // Sets sealed.
    void decorate_ast_node_t::set_sealed(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_sealed), true, el, _T("sealed"), true);
    }

    // Sets override.
    void decorate_ast_node_t::set_override(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_override), true, el, _T("override"), true);
    }

    // Sets virtual.
    void decorate_ast_node_t::set_virtual(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_virtual), true, el, _T("virtual"), true);
    }

    // Sets abstract.
    void decorate_ast_node_t::set_abstract(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_abstract), true, el, _T("abstract"), true);
    }

    // Sets new.
    void decorate_ast_node_t::set_new(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_new), true, el, _T("new"), true);
    }

    // Sets readonly.
    void decorate_ast_node_t::set_readonly(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_readonly), true, el, _T("readonly"), true);
    }

    // Sets constant.
    void decorate_ast_node_t::set_const(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_const), true, el, _T("const"), true);
    }

    // Sets extern.
    void decorate_ast_node_t::set_extern(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_extern), true, el, _T("extern"), true);
    }

    // Get decorate value.
    decorate_value_t decorate_ast_node_t::get_decorate()
    {
        return (decorate_value_t)__decorate;
    }

    // Commits this node.
    void decorate_ast_node_t::on_commit()
    {
        // Empty.
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
    bool statements_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
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
    bool method_body_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                __body.variable_region = context.current_region();
                if (__super_t::on_walk(context, step, tag))
                {
                    this->__delay(context, walk_step_t::post_analysis);
                    return true;
                }

                return false;

            case walk_step_t::post_analysis:
                return __walk_post_analysis(context);

            default: return true;
        }
    }

    // Walks post analysis step.
    bool method_body_ast_node_t::__walk_post_analysis(ast_walk_context_t & context)
    {
        typedef statement_exit_type_t       __exit_type_t;
        typedef enum_t<__exit_type_t>       __e_exit_type_t;

        statement_exit_type_context_t ctx;
        __e_exit_type_t exit_type = exit_type_of(ctx, &__body.statements);

        // method_t * method = context.current_method();
        // _PF(_T("%1%: %2%"), method, _eflags(exit_type));

        if (exit_type.has(__exit_type_t::pass) || exit_type == __exit_type_t::none)
        {
            method_t * method = context.current_method();
            _A(method != nullptr);

            if (method->type_name == nullptr || method->type_name->type == nullptr
                                             || is_void_type(method->type_name->type))
            {
                __body.push_back(__new_obj<return_statement_t>());
            }
            else
            {
                this->__log(this, __c_t::method_no_return, method);
            }
        }

        return true;
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

    // Appends name unit.
    void expression_ast_node_t::append_name_unit(name_unit_ast_node_t * node)
    {
        _A(node != nullptr);

        name_unit_t * name_unit = node->to_eobject();

        auto exp = __new_wexp<name_unit_expression_t>(name_unit->name, name_unit->args);
        exp->code_unit = this->code_unit;
        __es.push_back(__el_t(exp));

        if (node != nullptr)
            this->append_child(nodes, node);
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

    template<typename _es_t>
    string_t __join_es(const _es_t & es)
    {
        return al::join_str(
            std::begin(es), std::end(es), _T(""), [](auto && it) { return _str(it); }
        );
    }

    // Commits this node.
    void expression_ast_node_t::on_commit()
    {
        // Prints this expression.
        // _PP( __join_es(__es) );

        __expression = __parse_expression(__es);
    }

    // Returns this eobject.
    expression_t * expression_ast_node_t::to_eobject()
    {
        return __expression;
    }

    // Walks this node.
    bool expression_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                if (is_top_expression(__expression))
                    this->__delay(context, walk_step_t::analysis);
                return __super_t::on_walk(context, step, tag);

            case walk_step_t::analysis: {
                expression_t * exp = this->to_eobject();
                if (!exp->walked)
                {
                    walk_expression(this->__context, context, exp);
                    type_t * type = exp->get_type();

                    if (type == nullptr)
                        this->__log(this, __c_t::invalid_expression, exp);
                }

                return true;
            }

            default: return true;
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
    bool statement_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
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
    bool import_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
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
                    catch (const logic_error_t<compile_error_code_t> & e)
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

        return __super_t::on_walk(context, step, tag);
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
    bool using_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
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

        return __super_t::on_walk(context, step, tag);
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
    bool namespace_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        context.push(this->to_eobject());
        bool r = __super_t::on_walk(context, step, tag);
        context.pop();

        return r;
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
                    method.body = __new_obj<method_body_t>(__wctx.current_region());

                method.body->push_front(statements);
            }

            /*  // Prints all statements.
            _P(_T("--method"), method, method.body->statements.size());
            if (method.body->statements.size() > 0)
            {
                for (statement_t * st : method.body->statements)
                {
                    _PP(st);
                }
            }
            */
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
        __name_el = el;
        this->__assign_name(__type.name, name, el, _T("type name"));
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
    general_type_t * type_ast_node_t::to_eobject()
    {
        return &__type;
    }

    // Walks this node.
    bool type_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_: {
                context.push(this->to_eobject());
                bool r = __walk_default(context, step, tag);
                context.pop();
                return r;
            }

            case walk_step_t::analysis:
                return __walk_analysis(context, (method_t *)tag);

            default: return true;
        }
    }

    // Walks default step.
    bool type_ast_node_t::__walk_default(ast_walk_context_t & context, int step, void * tag)
    {
        general_type_t * type = (general_type_t *)to_eobject();
        mname_operate_context_t mctx = to_mname_operate_context();
        type->namespace_ = context.current_namespace();

        if (!context.assembly.types.append_type(type))
            this->__log(this, __c_t::type_defination_duplicate, type);

        __XPool.append_new_type(type);

        // Set type for enum fields.
        if (is_enum(type))
        {
            for (field_t * field : type->fields)
            {
                if (field->type_name != nullptr)
                    this->__log(this, __c_t::unexpected_enum_type, field->type_name, field->name);

                field->type_name = __XPool.to_type_name(type);

                if (field->decorate != nullptr)
                {
                    decorate_value_t dv(*(decorate_value_t *)field->decorate);

                    if (dv != decorate_value_t::default_value)
                        this->__log(this, __c_t::unexpected_enum_decorate, dv, field->name);

                    if (!field->decorate->is_static || !field->decorate->is_const)
                        this->__log(this, __c_t::enum_field_prototype_error, field->name);
                }
                else
                {
                    field->decorate = __new_obj<decorate_t>(decorate_t::default_value);
                    field->decorate->is_static = true;
                    field->decorate->is_const  = true;
                }
            }
        }
        else
        {
            for (field_t * field : type->fields)
            {
                if (field->decorate != nullptr)
                {
                    if (field->decorate->is_const)      // All const fields should be static.
                        field->decorate->is_static = true;
                }
            }
        }

        // Walk members.
        __super_t::on_walk(context, step, tag);

        // Check default constructors.
        method_t * new_default_constructor = nullptr;

        if (al::in(type->this_ttype(), ttype_t::class_, ttype_t::struct_))
        {
            auto it = al::find_if(__type.methods, [](method_t * m) {
                return m->trait == method_trait_t::constructor;
            });

            if (it == std::end(__type.methods))  // no constructor
            {
                method_t * method = __append_default_constructor(context);
                this->__delay(context, walk_step_t::analysis, method);
            }
            else
            {
                // struct type do not need a default constructor.
                method_t * constructor = *it;
                if (type->this_ttype() == ttype_t::struct_)
                {
                    if (constructor->param_count() == 0)
                        this->__log(this, __c_t::do_not_need_default_constructor, type);
                    else
                        new_default_constructor = __append_default_constructor(context);
                }
            }
        }

        this->__delay(context, walk_step_t::analysis, new_default_constructor);

        return true;
    }

    class type_ast_node_t::__fields_init_stub_t : public object_t
    {
    public:
        __fields_init_stub_t(type_ast_node_t * owner, general_type_t * type)
            : __owner(owner), __type(type) { }

        bool continue_()
        {
            int index = __break_index;
            __break_index = -1;

            for (int count = __type->fields.size(); index < count; index++)
            {
                field_t * field = __type->fields[index];
                if (!is_static_const(field))
                    continue;

                switch (__fill_value(field))
                {
                    case __fill_ret_t::error:
                        field->init_value = __get_default_value(field);
                        break;

                    case __fill_ret_t::break_:
                        if (__break_index == -1)
                            __break_index = index;

                        // Skip next non-value enum fields.
                        for (; index < count - 1; index++)
                        {
                            if (__type->fields[index + 1]->init_value != nullptr)
                                break;
                        }
                        break;

                    case __fill_ret_t::succeed:
                    default:
                        break;
                }
            }

            return __break_index == -1;
        }

    protected:
        type_ast_node_t *   __owner;
        general_type_t  *   __type;

        int __break_index = 0;

        enum class __fill_ret_t { succeed, error, break_ };

        virtual __fill_ret_t __fill_value(field_t * field) = 0;
        virtual expression_t * __get_default_value(field_t * field) = 0;

        bool __is_all_fields_initialized(field_t * field)
        {
            std::vector<field_t *> fields;

            bool r = __deep_each_field(field, [&](field_t * field0) {

                fields.push_back(field0);

                if (field0->init_value == nullptr)
                    return false;

                return field0->init_value->execute() != cvalue_t::nan;

            });

            return r;
        }

        bool __is_circular_defination(field_t * field)
        {
            __fields_t fields;
            return __is_circular_defination(fields, field);
        }

        template<typename ... _args_t> void __log(_args_t && ... args)
        {
            __owner->__log(__owner, std::forward<_args_t>(args) ...);
        }

    private:

        typedef al::svector_t<field_t *> __fields_t;

        template<typename _f_t>
        bool __deep_each_field(field_t * field, _f_t f)
        {
             std::function<bool (field_t *)> callback = [&](field_t * field0) {

                if (!f(field0))
                    return false;

                if (field0->init_value != nullptr && !__deep_each_field(field0, callback))
                    return false;

                return true;
            }; 

            return __each_field(field->init_value, callback);
        }

        template<typename _f_t> bool __each_field(expression_t * expression, _f_t f)
        {
            if (expression == nullptr)
                return true;

            field_variable_t * field_var;

            if (is_field_variable_expression(expression, &field_var))
            {
                field_t * field = field_var->field;
                _A(field != nullptr);

                if (!f(field))
                    return false;
            }

            return each_expression(expression, [&](expression_t * exp) {
                return __each_field(exp, f);
            }, true);
        }

        bool __is_circular_defination(__fields_t & fields, field_t * field)
        {
            size_t size = fields.size();
            if (al::contains(fields, field))
                return true;

            fields.push_back(field);

            if (__do_is_circular_defination(fields, field))
                return true;

            fields.truncate(size);

            return false;
        }

        bool __do_is_circular_defination(__fields_t & fields, field_t * field)
        {
            field_t * prev_field;
            if (field->init_value == nullptr
                && (prev_field = __get_enum_previous_field(field)) != nullptr)
            {
                if (__is_circular_defination(fields, prev_field))
                    return true;
            }

            return !__each_field(field->init_value, [&](field_t * field0) {
                return !__is_circular_defination(fields, field0);
            });
        }

        field_t * __get_enum_previous_field(field_t * field)
        {
            type_t * host_type = field->host_type;
            _A(host_type != nullptr);

            if (host_type->this_ttype() == ttype_t::enum_)
            {
                auto & fields = ((general_type_t *)host_type)->fields;
                auto it = al::find(fields, field);
                _A(it != std::end(fields));

                if (it != std::begin(fields))
                    return *(it - 1);
            }

            return nullptr;
        }
    };

    // __enum_fields_init_stub_t
    template<vtype_t _vtype>
    class type_ast_node_t::__enum_fields_init_stub_t : public __fields_init_stub_t
    {
        typedef __fields_init_stub_t __super_t;
        typedef vnum_t<_vtype> __integer_type_t;

    public:
        using __super_t::__super_t;

    protected:

        virtual __fill_ret_t __fill_value(field_t * field) override
        {
            #define __Exp(_v)  _F(_T("'%1% = %2%'"), field->name, _v)

            if (field->init_value != nullptr)
            {
                if (__is_circular_defination(field))
                {
                    __log(__c_t::enum_field_circular_definition, field->name);
                    return __fill_ret_t::error;
                }

                cvalue_t v = field->init_value->execute();

                if (v == cvalue_t::nan)
                {
                    if (__is_all_fields_initialized(field))
                    {
                        __log(__c_t::expect_constant_value, field->init_value, field->name);
                        return __fill_ret_t::error;
                    }

                    return __fill_ret_t::break_;
                }

                if (v > __cvalue_max)
                {
                    __log(__c_t::enum_value_too_large, __Exp(v), _vtype);
                    return __fill_ret_t::error;
                }

                if (v < __cvalue_min)
                {
                    __log(__c_t::enum_value_too_small, __Exp(v), _vtype);
                    return __fill_ret_t::error;
                }

                cvalue_t v2 = v.change_type(_vtype);
                field->init_value = __XPool.get_cvalue_expression(v2);

                __value = (__integer_type_t)v.number;
                __is_next_value = false;

                return __fill_ret_t::succeed;
            }
            else
            {
                if (__is_next_value)
                {
                    field->init_value = __XPool.get_cvalue_expression((cvalue_t)__value);
                    __is_next_value = false;
                    return __fill_ret_t::succeed;
                }

                if (__value == __max)
                {
                    __log(__c_t::enum_value_too_large, __Exp(__value), _vtype);
                    return __fill_ret_t::error;
                }

                field->init_value = __XPool.get_cvalue_expression((cvalue_t)(++__value));
                return __fill_ret_t::succeed;
            }

            #undef __Exp
        }

        virtual expression_t * __get_default_value(field_t * field) override
        {
            return __XPool.get_cvalue_expression(default_value_of(_vtype));
        }

    private:
        __integer_type_t __value = 0;

        bool __is_next_value = true;

        static constexpr __integer_type_t __min = min_value<__integer_type_t>();
        static constexpr __integer_type_t __max = max_value<__integer_type_t>();

        static const cvalue_t __cvalue_min;
        static const cvalue_t __cvalue_max;
    };

    template<vtype_t _vtype>
    const cvalue_t type_ast_node_t::__enum_fields_init_stub_t<_vtype>::__cvalue_min(__min);

    template<vtype_t _vtype>
    const cvalue_t type_ast_node_t::__enum_fields_init_stub_t<_vtype>::__cvalue_max(__max);

    class type_ast_node_t::__general_fields_init_stub_t : public __fields_init_stub_t
    {
        typedef __fields_init_stub_t __super_t;

    public:
        using __super_t::__super_t;

        virtual __fill_ret_t __fill_value(field_t * field) override
        {
            if (field->init_value == nullptr)
            {
                field->init_value = __get_default_value(field);
                return __fill_ret_t::succeed;
            }

            if (__is_circular_defination(field))
            {
                __log(__c_t::enum_field_circular_definition, field->name);
                return __fill_ret_t::error;
            }

            cvalue_t v = field->init_value->execute();
            if (v == cvalue_t::nan)
            {
                if (__is_all_fields_initialized(field))
                {
                    __log(__c_t::expect_constant_value, field->init_value, field->name);
                    return __fill_ret_t::error;
                }

                return __fill_ret_t::break_;
            }
            else
            {
                field->init_value = __XPool.get_cvalue_expression(v);
                return __fill_ret_t::succeed;
            }
        }

        virtual expression_t * __get_default_value(field_t * field) override
        {
            return __XPool.get_cvalue_expression(
                default_value_of(field->get_vtype())
            );
        }
    };

    // Walks analysis step.
    bool type_ast_node_t::__walk_analysis(ast_walk_context_t & context, void * tag)
    {
        // Check circle base types.
        if (tag == nullptr)
        {
            type_t * circle_type = __check_circle_base_types();
            if (circle_type != nullptr)
            {
                this->__log(__name_el, __c_t::circle_base_type_dependency,
                    __type.to_full_name(), circle_type->to_full_name()
                );
            }
        }

        object_t * tag_obj = (object_t *)tag;
        method_t * new_default_constructor = nullptr;
        __fields_init_stub_t * fields_init_stub = nullptr;

        dynamic_cast_tie(tag_obj, &new_default_constructor, &fields_init_stub);

        // Revise constructor.
        if (new_default_constructor != nullptr)
        {
            __method_utils_t(__context, context).revise_constructor(*new_default_constructor,
                                                                    nullptr);
        }

        // Init field constant values.
        if (fields_init_stub != nullptr)
        {
            if (!fields_init_stub->continue_())
            {
                this->__delay(context, walk_step_t::analysis, fields_init_stub);
            }
        }
        else
        {
            return __walk_analysis_init_fields(context);
        }

        return true;
    }

    // Walks init fields.
    bool type_ast_node_t::__walk_analysis_init_fields(ast_walk_context_t & context)
    {
        general_type_t * type = (general_type_t *)to_eobject();

        // Sets enum values.
        if (is_enum(type))
        {
            vtype_t underlying_vtype = type->get_underlying_vtype();

            if (!is_integer(underlying_vtype))
            {
                this->__log(this, __c_t::unexpected_underlying_type);
                underlying_vtype = default_enum_underlying_vtype;
            }

            type->vtype = underlying_vtype;
            __fields_init_stub_t * stub_ = nullptr;

            switch (underlying_vtype)
            {
                #define __Case(_type)                                                   \
                    case vtype_t::_type##_: {                                           \
                        typedef __enum_fields_init_stub_t<vtype_t::_type##_> stub_t;    \
                        stub_t stub(this, type);                                        \
                        if (!stub.continue_())                                          \
                            stub_ = __new_obj<stub_t>(stub);                            \
                    }  break;

                __Case(int8)
                __Case(uint8)
                __Case(int16)
                __Case(uint16)
                __Case(int32)
                __Case(uint32)
                __Case(int64)
                __Case(uint64)

                #undef __Case

                default:
                    X_UNEXPECTED();
            }

            if (stub_ != nullptr)
                this->__delay(context, walk_step_t::analysis, stub_);
        }
        else
        {
            typedef __general_fields_init_stub_t stub_t;
            stub_t stub(this, type);

            if (!stub.continue_())
                this->__delay(context, walk_step_t::analysis, __new_obj<stub_t>(stub));
        }

        return true;
    }

    // Appends default constructor.
    method_t * type_ast_node_t::__append_default_constructor(ast_walk_context_t & context)
    {
        method_t * method = __new_obj<method_t>();

        method->name     = __type.name;
        method->decorate = __new_obj<decorate_t>();
        method->decorate->access = access_value_t::public_;
        method->trait    = method_trait_t::constructor;
        method->body     = __new_obj<method_body_t>(context.current_region());

        method->body->push_back(__new_obj<return_statement_t>());

        __type.methods.push_back(method);

        return method;
    }

    // Check circle base types.
    type_t * type_ast_node_t::__check_circle_base_types()
    {
        return __type.each_super_type([this](type_t * super_type) {

            std::set<type_t *> walked_types;
            return !__check_circle_base_types(walked_types, super_type);

        });
    }

    // Check circle base types.
    bool type_ast_node_t::__check_circle_base_types(std::set<type_t *> & walked_types,
                                                    type_t * super_type)
    {
        super_type = to_general(super_type);

        if (!walked_types.insert(super_type).second)
            return false;

        if (super_type == &__type)
            return true;

        // Super types.
        type_t * circle_type = super_type->each_super_type([&, this](type_t * super_type1) {

            if (__check_circle_base_types(walked_types, super_type1))
                return false;

            return true;

        });

        if (circle_type != nullptr)
            return true;

        /*
        // Nest types.
        if (super_type->this_gtype() == gtype_t::general)
        {
            general_type_t * general_type = (general_type_t *)super_type;
            for (type_t * nest_type : general_type->nest_types)
            {
                _PP(nest_type);
                if (__check_circle_base_types(walked_types, nest_type))
                    return true;
            }
        }
        */

        return false;
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
    bool generic_param_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__param.name, _T("template param"));

        return __super_t::on_walk(context, step, tag);
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
    bool generic_params_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
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
    bool generic_constraint_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
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
    bool generic_constraints_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_arg

    // Set param type, ref, out, params ...
    void generic_arg_ast_node_t::set_atype(generic_arg_type_t type)
    {
        __arg.atype = type;
    }

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
    bool generic_arg_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__arg.type_name, this, __c_t::type_name_missing, _T("generic argument"));

        return __super_t::on_walk(context, step, tag);
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
    bool generic_args_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if (__args.empty())
            this->__log(this, __c_t::type_name_missing, _T("generic arguments"));

        if (__is_partial_specialization(__args))
            this->__log(this, __c_t::partial_specialization_not_supported);

        return __super_t::on_walk(context, step, tag);
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
    bool param_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                this->__check_name_empty(__param.name, _T("param"));
                this->__check_empty(__param.type_name, this, __c_t::type_name_missing, _T("param"));

                this->__delay(context, walk_step_t::pre_analysis);
                return __super_t::on_walk(context, step, tag);

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
                    catch (logic_error_t<expression_execute_error_code_t> & e)
                    {
                        this->__log(exp, __c_t::unexpected_param_default_value, e.get_message());
                    }
                }

                return true;

            default: return true;
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
    bool params_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
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
    bool attribute_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                context.current_module()->register_commit(this->to_eobject());
                this->__delay(context, walk_step_t::post_confirm);
                return __super_t::on_walk(context, step, tag);

            case walk_step_t::post_confirm:
                if (__is_compile_time_attribute(&__attr))
                    __get_compile_time_attribute()->type_name = __attr.type_name;

                this->__delay(context, walk_step_t::analysis);
                return true;

            case walk_step_t::analysis:
                return __analyze_attribute_constructor();

            default: return true;
        }
    }

    // Returns compile time attribute.
    attribute_t * attribute_ast_node_t::__get_compile_time_attribute()
    {
        return __get_xpool().get_compile_time_attribute();
    }

    // Analyze attribute constructor.
    bool attribute_ast_node_t::__analyze_attribute_constructor()
    {
        type_name_t * type_name = __attr.type_name;
        type_t * type;

        if (type_name == nullptr || (type = type_name->type) == nullptr || is_unknown(type))
            return true;

        method_t * method = find_constructor(this->__context, type, __attr.arguments);
        if (method == nullptr)
        {
            __log(&__attr, __c_t::attribute_constructor_not_match, type,
                to_arg_types_str(this->__context, __attr.arguments)
            );

            return true;
        }

        if (__attr.arguments != nullptr)
        {
            for (argument_t * argument : *__attr.arguments)
            {
                expression_t * exp = argument->expression;
                if (exp != nullptr)
                {
                    try
                    {
                        exp->execute();
                    }
                    catch (const error_t & e)
                    {
                        _PP(e);
                    }
                }
            }
        }

        this->to_eobject()->constructor = method;

        return true;
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
    bool attributes_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
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
    bool expression_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__statement.expression, this, __c_t::expression_missing,
                                                          _T("statement"));
        return __super_t::on_walk(context, step, tag);
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
    bool type_def_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__statement.type_def, this, __c_t::statement_missing);

        return __super_t::on_walk(context, step, tag);
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
    bool defination_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                __super_t::on_walk(context, step, tag);
                if (__walk_default(context))
                {
                    this->__delay(context, walk_step_t::analysis);
                    return true;
                }

                return false;

            case walk_step_t::analysis:
                return __walk_analysis(context);

            default: return true;
        }
    }

    static bool __is_uncertain(type_name_t * type_name)
    {
        return type_name->type != nullptr
            && type_name->type->this_gtype() == gtype_t::uncertain;
    }

    // Walks default step.
    bool defination_st_ast_node_t::__walk_default(ast_walk_context_t & context)
    {
        this->__check_empty(__statement.type_name, this, __c_t::type_name_missing,
                                                        _T("variable defination"));

        variable_defination_t vd(this->__context, context, &__statement);

        for (defination_statement_item_t * var_item : __statement.items)
        {
            type_name_t * type_name = __to_eobject<type_name_t *>(this->type_name);

            var_item->variable = vd.define_local(
                type_name, var_item->name, __statement.constant,
                var_item->expression, var_item
            );

            if (var_item->expression != nullptr && !__is_uncertain(type_name))
            {
                var_item->expression->parent = __new_obj<type_cast_expression_t>(
                    type_name, var_item->expression
                );
            }
        }

        return true;
    }

    // Walks analysis step.
    bool defination_st_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        #define __L(_code, _args...)                                                    \
            this->__log(this->child_at(items, index), __c_t::_code, ##_args)

        for (int index = 0, count = __statement.items.size(); index < count; ++index)
        {
            defination_statement_item_t * var_item = __statement.items[index];
            if (__is_uncertain(var_item->variable->type_name))
            {
                type_t * type = nullptr;
                if (var_item->expression != nullptr)
                    type = var_item->expression->get_type();

                if (type == nullptr)
                    __Log(cannot_determine_local_variable_type, var_item->name);
                else
                    var_item->variable->type_name->type = type;

                /*
                _PF(_T("%1% %2% (%3%)"), var_item->variable->type_name->type, var_item->name,
                                                                    var_item->expression);
                */
            }

            if (var_item->expression != nullptr)
            {
                type_t * type1 = var_item->variable->type_name->type;
                type_t * type2 = var_item->expression->get_type();

                if (!is_type_compatible(type2, type1))
                    __L(invalid_type_cast, type2->to_prototype(), type1->to_prototype());
            }
        }

        if (__statement.constant)
        {
            for (int index = 0, count = __statement.items.size(); index < count; ++index)
            {
                defination_statement_item_t * var_item = __statement.items[index];

                if (var_item->expression == nullptr)
                    __L(constant_variable_initialize_missing, var_item->name);
                else if (__execute_expression(var_item->expression) == cvalue_t::nan)
                    __L(constant_variable_required_constant_value, var_item->name);
            }
        }

        return true;

        #undef __L
    }

    ////////// ////////// ////////// ////////// //////////
    // break_st

    // Returns this eobject.
    statement_t * break_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    // Walks this node.
    bool break_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // continue_st

    // Returns this eobject.
    statement_t * continue_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    // Walks this node. 
    bool continue_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
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
    bool throw_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
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
    bool goto_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__statement.label, _T("label"));

        return __super_t::on_walk(context, step, tag);
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
    bool return_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                if (__super_t::on_walk(context, step, tag))
                {
                    this->__delay(context, walk_step_t::post_analysis);
                    return true;
                }

                return false;

            case walk_step_t::post_analysis:
                return __walk_post_analysis(context);

            default: return true;
        }
    }

    // Walks post analysis step.
    bool return_st_ast_node_t::__walk_post_analysis(ast_walk_context_t & context)
    {
        method_t * method = context.current_method();
        _A(method != nullptr);

        if (method->type_name == nullptr || method->type_name->type == nullptr
                                         || is_void_type(method->type_name->type))
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
                type_t * return_type = __statement.expression->get_type();
                type_t * method_type = method->type_name->type;

                if (return_type != nullptr && !is_type_compatible(return_type, method_type))
                    this->__log(this, __c_t::method_incompatible_return_value, method);
            }
        }

        return true;
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
    bool do_while_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__statement.condition, this, __c_t::condition_missing, _T("do while"));
        this->__check_empty(__statement.body, this, __c_t::body_missing, _T("do while"));

        return __super_t::on_walk(context, step, tag);
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
    bool loop_until_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__statement.condition, this, __c_t::condition_missing, _T("loop until"));
        this->__check_empty(__statement.body, this, __c_t::body_missing, _T("loop until"));

        return __super_t::on_walk(context, step, tag);
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
    bool while_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__statement.condition, this, __c_t::condition_missing, _T("while"));
        this->__check_empty(__statement.body, this, __c_t::body_missing, _T("while"));

        return __super_t::on_walk(context, step, tag);
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
    bool for_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__statement.body, this, __c_t::body_missing, _T("while"));

        if (__statement.defination_initialize != nullptr)
        {
            context.push_new_region();
            bool r = __super_t::on_walk(context, step, tag);
            context.pop();

            return r;
        }
        else
        {
            return __super_t::on_walk(context, step, tag);
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
    bool for_each_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__statement.variable, _T("variable"));
        this->__check_empty(__statement.iterator, this, __c_t::iterator_missing, "for each");
        this->__check_empty(__statement.body, this, __c_t::body_missing, _T("for each"));

        return __super_t::on_walk(context, step, tag);
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
    bool if_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
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
    bool switch_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__statement.expression, this, __c_t::expression_missing, _T("switch"));

        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // case

    // Appends default expression.
    void case_ast_node_t::append_default(__el_t * el)
    {
        auto * exp_node = to_fake_ast<(__value_t)__cvalue_t::expression>(
            (expression_t *)nullptr, __get_memory(), el->get_code_unit()
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
    bool case_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__case.constants, this, __c_t::expression_missing, _T("case"));

        return __super_t::on_walk(context, step, tag);
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
    bool try_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if (__statement.catches.empty() && __statement.finally_statement == nullptr)
        {
            this->__log(this, __c_t::catch_or_finally_missing);
        }

        return __super_t::on_walk(context, step, tag);
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
    bool catch_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
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

        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // empty_st

    // Returns this eobject.
    statement_t * empty_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    // Walks this node.
    bool empty_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
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
    bool statement_group_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        context.push_new_region();

        __statement_group.variable_region = context.current_region();
        bool r = __super_t::on_walk(context, step, tag);

        context.pop();

        return r;
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
    bool type_name_exp_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
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
    bool type_cast_exp_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // function

    // Commits this node.
    void function_ast_node_t::on_commit()
    {
        __expression.set_namex(__to_eobject<expression_t *>(namex));
        __expression.generic_args  = __to_eobject<generic_args_t *>(generic_args);

        __expression.set_arguments(__to_eobject<arguments_t *>(arguments));
    }

    // Returns this eobject.
    expression_t * function_ast_node_t::to_eobject()
    {
        return &__expression;
    }

    // Walks this node.
    bool function_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                this->__delay(context, walk_step_t::analysis);
                __walk_default(context);

                return __super_t::on_walk(context, step, tag);

            case walk_step_t::analysis:
                return __walk_analysis(context);

            default:
                return true;
        }
    }

    // Walks default step.
    bool function_ast_node_t::__walk_default(ast_walk_context_t & context)
    {
        this->__check_name_empty(__expression.namex, _T("function"));

        if (__expression.generic_args != nullptr && __expression.get_name() == name_t::null)
        {
            this->__log(this->child_at(generic_args), __c_t::function_generic_args_redundance);
        }

        return true;
    }

    // Walks analysis step.
    bool function_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        // Check permissions.
        function_expression_t * exp = &__expression;
        if (exp->get_ftype() != function_expression_type_t::method)
            return true;

        method_base_t * method = exp->get_method();
        if (method == nullptr)
            return true;

        this->__check_access(context, method, this->child_at(namex));
        return true;
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
    bool index_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__expression.namex(), _T("index"));

        return __super_t::on_walk(context, step, tag);
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
    bool new_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__expression.type_name, this, __c_t::type_name_missing,
                                                            _T("object creation"));
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                this->__delay(context, walk_step_t::analysis);
                return __super_t::on_walk(context, step, tag);

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

                return true;

            }

            default: return true;
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
    bool new_array_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                __walk_default(context);

                if (__super_t::on_walk(context, step, tag))
                {
                    this->__delay(context, walk_step_t::confirm);

                    if (__expression.initializer() != nullptr)
                        this->__delay(context, walk_step_t::analysis);

                    return true;
                }

                return false;

            case walk_step_t::confirm:
                return __walk_confirm(context);

            case walk_step_t::analysis:
                return __walk_analysis(context);

            default:
                return true;
        }
    }

    // Walks default step.
    bool new_array_ast_node_t::__walk_default(ast_walk_context_t & context)
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

        return true;
    }

    // Walks confirm step.
    bool new_array_ast_node_t::__walk_confirm(ast_walk_context_t & context)
    {
        // To ensure the array type be pushed into xpool before execute xpool_t::commit_types().
        // Then used by some expressions, like (new int[10]).Length;
        // Otherwise, An error of 'type not found' will be occued.
        __expression.to_array_type();
        return true;
    }

    // Walks analysis step.
    bool new_array_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        array_initializer_t * initializer = __expression.initializer();
        if (initializer != nullptr)
        {
            initializer->each_element([&, this](expression_t * exp) -> bool {
                walk_expression(this->__context, context, exp);
                return true;
            });
        }

        return true;
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
    bool array_initializer_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        try
        {
            __initializer.check();
        }
        catch (logic_error_t<array_initialize_check_error_code_t> & e)
        {
            this->__log(this, __c_t::array_initializer_error, _str(e));
        }

        return __super_t::on_walk(context, step, tag);
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
    bool array_lengths_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
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
    bool default_value_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__expression.type_name, this, __c_t::type_name_missing,
                                                        _T("default value"));

        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // field

    // Sets name.
    void field_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__name_el = el;
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
    bool field_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                if (__walk_default(context, step, tag))
                {
                    this->__delay(context, walk_step_t::analysis);
                    return true;
                }

                return false;

            case walk_step_t::analysis:
                return __walk_analysis(context);

            default: return true;
        }
    }

    // Walks default step.
    bool field_ast_node_t::__walk_default(ast_walk_context_t & context, int step, void * tag)
    {
        context.push(this->to_eobject());
        bool r = __super_t::on_walk(context, step, tag);
        context.pop();

        if (r)
        {
            this->__check_empty(__field.type_name, this, __c_t::type_name_missing,
                    _F(_T("field %1%"), _str(__field.name)));
            this->__check_empty(__field.name, this, __c_t::name_empty, _T("field"));

            variable_defination_t vd(this->__context, context, &__field);
            vd.define_field(&__field);
        }

        return r;
    }

    // Walks analysis step.
    bool field_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        this->__check_member(context.current_type(), &__field, __name_el);
        return true;
    }

    ////////// ////////// ////////// ////////// //////////
    // method

    // Sets name.
    void method_ast_node_t::set_name(name_t name, __el_t * el)
    {
        __name_el = el;

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
    bool method_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_: {
                context.push(this->to_eobject());

                __super_t::on_walk(context, step, tag);
                bool r = __walk_default(context);
                if (r)
                    this->__delay(context, walk_step_t::analysis);

                context.pop();

                if (__method.trait == method_trait_t::normal)
                {
                    variable_defination_t vd(this->__context, context, &__method);
                    vd.define_method(&__method);
                }

                return r;
            }

            case walk_step_t::analysis:
                return __walk_analysis(context);

            default: return true;
        }
    }

    // Walks the default step.
    bool method_ast_node_t::__walk_default(ast_walk_context_t & context)
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

        return true;
    }

    // Walks the analysis step.
    bool method_ast_node_t::__walk_analysis(ast_walk_context_t & context)
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

        // Check duplicated.
        if (__method.relation_member == nullptr)
            this->__check_member(context.current_type(), &__method, __name_el);

        // Operator overload, checks prototype.
        if (__op_property == nullptr)
            return true;

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

        return true;
    }

    ////////// ////////// ////////// ////////// //////////
    // property

    // Sets name.
    void property_ast_node_t::set_name(name_t name, __el_t * el)
    {
        __name_el = el;

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
    bool property_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                if (__walk_default(context, step, tag))
                {
                    this->__delay(context, walk_step_t::analysis);
                    return true;
                }

                return false;

            case walk_step_t::analysis:
                return __walk_analysis(context);

            default: return true;
        }
    }

    // Walks default step.
    bool property_ast_node_t::__walk_default(ast_walk_context_t & context, int step, void * tag)
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

        this->__append_default_methods(context);

        variable_region_t * region = context.current_region();

        variable_defination_t vd(this->__context, context, &__property);
        vd.define_property(&__property);

        context.push(this->to_eobject());
        __super_t::on_walk(context, step, tag);
        context.pop();

        return true;
    }

    // Append default methods for get/set methods.
    void property_ast_node_t::__append_default_methods(ast_walk_context_t & context)
    {
        type_t * type = context.current_type();
        _A(type != nullptr);

        if (__property.get_method != nullptr)
        {
            __property.get_method->name      = __to_method_name(_T("get"));
            __property.get_method->type_name = __property.type_name;
            __property.get_method->decorate  = __property.decorate;
            __property.get_method->params    = __property.params;
            __property.get_method->relation_member = &__property;
            __property.get_method->owner_type_name = __property.owner_type_name;

            __append_member(this->__context, context, __property.get_method);
        }

        param_t * value_param = nullptr;
        xpool_t & xpool = __get_xpool();

        if (__property.set_method != nullptr)
        {
            __property.set_method->name      = __to_method_name(_T("set"));
            __property.set_method->type_name = xpool.to_type_name(xpool.get_void_type());
            __property.set_method->decorate  = __property.decorate;
            __property.set_method->relation_member = &__property;
            __property.set_method->owner_type_name = __property.owner_type_name;

            params_t * params = __new_obj<params_t>();

            if (__property.params != nullptr)
                al::copy(*__property.params, std::back_inserter(*params));

            value_param = __new_obj<param_t>(__property.type_name, __to_name(_T("value")));
            params->push_back(value_param);

            __property.set_method->params = params;
            __append_member(this->__context, context, __property.set_method);
        }

        if (type->this_ttype() != ttype_t::interface_ && !__property.is_abstract())
            __generate_ignored_method_bodies(context, value_param);
    }

    // Walks analysis step.
    bool property_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        this->__check_member(context.current_type(), &__property, __name_el);
        return true;
    }

    // Generates ignored method bodies.
    void property_ast_node_t::__generate_ignored_method_bodies(ast_walk_context_t & context,
                                                               param_t * value_param)
    {
        #define __AccessorEmpty(name)                                                   \
            (__property.name##_method == nullptr)

        #define __BodyEmpty(name)                                                       \
            (__property.name##_method->body == nullptr)

        #define __BodyIgnored(name)                                                     \
            (!__AccessorEmpty(name) && __BodyEmpty(name))

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

            __property.get_method->body = __new_obj<method_body_t>(context.current_region());
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

            }, context.current_region());

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
        __name_el = el;
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
    bool event_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                if (__walk_default(context, step, tag))
                {
                    this->__delay(context, walk_step_t::analysis);
                    return true;
                }

                return false;

            case walk_step_t::analysis:
                return __walk_analysis(context);

            default: return true;
        }
    }

    // Walks default step.
    bool event_ast_node_t::__walk_default(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__event.name, _T("event"));

        if (__event.add_method != nullptr)
        {
            __event.add_method->name = __to_name(_F(_T("add_%1%"), __event.name));
            __event.add_method->type_name = __event.type_name;
            __event.add_method->relation_member = &__event;
            __event.add_method->owner_type_name = __event.owner_type_name;
            __append_member(this->__context, context, __event.add_method);
        }

        if (__event.remove_method != nullptr)
        {
            __event.remove_method->name = __to_name(_F(_T("remove_%1%"), __event.name));
            __event.remove_method->type_name = __event.type_name;
            __event.remove_method->relation_member = &__event;
            __event.remove_method->owner_type_name = __event.owner_type_name;
            __append_member(this->__context, context, __event.remove_method);
        }

        context.push(this->to_eobject());
        __super_t::on_walk(context, step, tag);
        context.pop();

        return true;
    }

    // Walks analysis step.
    bool event_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        this->__check_member(context.current_type(), &__event, __name_el);
        return true;
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::compile

