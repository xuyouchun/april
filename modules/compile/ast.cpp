
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

        // The type arguments for method cannot be inferred from the usage.
        // Try specifying the type arguments explicitly.
        X_D(type_arguments_cannot_be_inferred,
            _T("The type arguments for method '%1%' in type '%2%' ")
            _T("cannot be inferred from the usage. ")
            _T("Try specifying the type arguments explicitly"))

        // The type 'string' cannot be used as type parameter 'T' in the
        //  generic type or method 'Class1<T>'. There is no implicit reference conversion
        //  from 'string' to 'Project1.Class2'.
        X_D(constraint_no_implicit_conversion,
            _T("The type '%1%' cannot be used as type parameter '%2%' in the")
            _T(" generic type or method '%3%'. There is no implicit reference conversion")
            _T(" from '%1%' to '%4%'"))
            
        // The type 'string' must be a non-nullable value type
        //  in order to use it as parameter 'T' in the generic type or method 'Class1<T>'
        X_D(constraint_expect_value_type,
            _T("The type '%1%' must be a non-nullable value type")
            _T(" in order to use it as parameter '%2%' in the generic type or method '%3%'"))

        // The type 'int' must be a reference type in order to
        //  use it as parameter 'T' in the generic type or method 'Class1<T>'
        X_D(constraint_expect_reference_type,
            _T("The type '%1%' must be a reference type in order to")
            _T(" use it as parameter '%2%' in the generic type or method '%3%'"))

        // 'long' is not a valid constraint. A type used as a constraint
        //  must be an interface, a non-sealed class or a type parameter
        X_D(constraint_invalid_type,
            _T("'%1%' is not a valid constraint. A type used as a constraint")
            _T(" must be an interface, a non-sealed class or a type parameter"))

        // The 'class', 'struct', 'unmanaged', 'notnull', and 'default' constraints
        //  cannot be combined or duplicated.
        X_D(constraint_combined_or_duplicated,
            _T("The 'class', 'struct', 'unmanaged', 'notnull', and 'default' constraints")
            _T("cannot be combined or duplicated"))

        // The 'class', 'struct', 'unmanaged', 'notnull', and 'default' constraints
        //  must be specified first in the constraints list.
        X_D(constraint_must_be_first,
            _T("The 'class', 'struct', 'unmanaged', 'notnull', and 'default' constraints")
            _T(" must be specified first in the constraints list."))

        // The 'new()' constraint cannot be used with the 'struct' constraint
        X_D(constraint_new_cannot_be_used_with_struct,
            _T("The 'new()' constraint cannot be used with the 'struct' constraint"))

        // 'Class2' must be a non-abstract type with a public parameterless constructor
        //  in order to use it as parameter 'T' in the generic type or method 'Class1<T>'
        X_D(constraint_expect_public_parameterless_constructor,
            _T("'%1%' must be a non-abstract type with a public parameterless constructor")
            _T(" in order to use it as parameter '%2%' in the generic type or method '%3%'"))

        // Duplicate constraint 'Class2' for type parameter 'T'
        X_D(constraint_duplicate_types,
            _T("Duplicate constraint '%1%' for type parameter '%2%'"))

        // Constraint cannot be special class 'object'
        X_D(constraint_cannot_be_special_type,
            _T("Constraint cannot be special class '%1%'"))

        // A constraint clause has already been specified for type parameter 'T'.
        // All of the constraints for a type parameter must be specified in a single where clause.
        X_D(constraint_duplicate_specified,
            _T("A constraint clause has already been specified for type parameter '%1%'.")
            _T(" All of the constraints for a type parameter must be specified in a single")
            _T(" where clause"))

        // The class type constraint 'Class3' must come before any other constraints
        X_D(constraint_type_position_error,
            _T("The class type constraint '%1%' must come before any other constraints"))

        // Circular constraint dependency involving 'T' and 'K'
        X_D(constraint_circular_dependency,
            _T("Circular constraint dependency involving '%1%'"))

        // 'Class3': cannot specify both a constraint class and the 'class' or 'struct' constraint
        X_D(constraint_unexpected_class_or_struct,
            _T("'%1%': cannot specify both a constraint class")
            _T(" and the 'class' or 'struct' constraint"))

        // Class1<T>' does not define type parameter 'K'
        X_D(generic_parameter_not_defined,
            _T("%1%' does not define type parameter '%2%'"))

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
        this->transform_child_to(__expressions__, *this);
    }

    // Returns this eobject.
    expression_t * expressions_ast_node_t::to_eobject()
    {
        if (this->expression_count() == 1)
        {
            expression_t * exp = this->expression_at(0);
            return exp->parent = nullptr, exp;
        }

        return this;
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
        return __to_eobject<type_name_t *>(__underlying__);
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
        if (type != nullptr)
            return _str(type);

        return al::join_str(units.begin(), units.end(), _T("."),
            [](auto it) { return _str(it); }
        );
    }

    // Converts to string.
    X_DEFINE_TO_STRING(array_type_name_t)
    {
        if (type != nullptr)
            return _str(type);

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
        this->global_type = global_type;
    }

    // Commits this node.
    void general_type_name_ast_node_t::on_commit()
    {
        this->transform_child_to(__units__, this->units);
    }

    // Returns this eobject.
    type_name_t * general_type_name_ast_node_t::to_eobject()
    {
        return this;
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
        if (this->type == nullptr)
            this->type = __ascertain_type(context, (general_type_name_t *)this);

        return true;
    }

    class __generic_param_types_t
    {
    public:

        // Fill generic type map.
        template<typename _entity_t>
        void fill_by_generic(_entity_t * entity)
        {
            _A(entity != nullptr);
            __fill(entity->template_, entity->args, entity->host_type);
        }

        // Fill generic type map.
        template<typename _entity_t>
        void fill_by_general(_entity_t * entity)
        {
            _A(entity != nullptr);
            type_t * host_type = entity->host_type;

            if (is_generic(host_type))
                fill_by_generic((generic_type_t *)host_type);
        }

        // Returns type of specified name.
        type_t * get_type(name_t name)
        {
            auto it = __type_map.find(name);
            if (it == std::end(__type_map))
                return nullptr;

            return it->second;
        }

        // Transform type.
        type_t * get_type(type_t * type)
        {
            return transform_type(type, [this](name_t name) { return this->get_type(name); });
        }

    private:

        std::map<name_t, type_t *> __type_map;

        // Fill generic type map.
        void __fill(general_type_t * type, type_collection_t & types, type_t * host_type)
        {
            _A(type != nullptr);
            _A(types.size() >= type->params_count());

            __fill(type->params, types, host_type);
        }

        // Fill generic type map.
        void __fill(method_t * method, type_collection_t & types, type_t * host_type)
        {
            _A(method != nullptr);
            _A(types.size() >= method->generic_param_count());

            __fill(method->generic_params, types, host_type);
        }

        void __fill(generic_params_t * params, type_collection_t & types, type_t * host_type)
        {
            _A(params != nullptr);

            int index = 0;
            for (generic_param_t * gp : *params)
            {
                type_t * type = types[index++];
                __type_map[gp->name] = type;
            }

            if (host_type != nullptr && is_generic(host_type))
                fill_by_generic((generic_type_t *)host_type);
        }
    };

    template<typename _entity_t, typename _code_element_t>
    class __check_generic_constraints_t
    {
        typedef generic_constraint_ttype_t __f_t;

    public:

        __check_generic_constraints_t(ast_context_t & cctx, ast_walk_context_t & wctx,
                        _entity_t * entity, _code_element_t * code_element)
            : __cctx(cctx), __wctx(wctx), __entity(entity), __code_element(code_element)
        {
            _A(entity != nullptr);
        }

        // Check constraints.
        bool check()
        {
            generic_constraints_t * constraints = __entity->template_->constraints;
            if (constraints == nullptr)
                return true;

            __param_types.fill_by_generic(__entity);

            bool ret = true;
            for (generic_constraint_t * constraint : *constraints)
            {
                if (!__check(constraint))
                    ret = false;
            }

            return ret;
        }

    private:

        ast_walk_context_t &    __wctx;
        ast_context_t &         __cctx;
        _entity_t *             __entity;
        _code_element_t *       __code_element;

        __generic_param_types_t __param_types;

        #define __LogAndRet(_code, _args ...)    do {                                   \
                this->__log(__c_t::_code, ##_args);                                     \
                return false;                                                           \
            } while (false)

        bool __check(generic_constraint_t * constraint)
        {
            name_t name = constraint->param_name;
            type_t * type = __param_types.get_type(name);

            // Check type compatible.
            {
                for (type_name_t * type_name : constraint->type_names)
                {
                    type_t * type0 = __param_types.get_type(type_name->type);
                    if (type0 == nullptr)
                        continue;

                    if (is_struct(type0))
                        __LogAndRet(constraint_invalid_type, type0);

                    if (!__is_type_compatible(type, type0))
                        __LogAndRet(constraint_no_implicit_conversion, type, name, __entity, type0);
                }
            }

            // Check cttype 
            {
                if (bit_has_flag(constraint->cttype, __f_t::class_) && !__is_ref_type(type))
                    __LogAndRet(constraint_expect_value_type, type, name, __entity);

                if (bit_has_flag(constraint->cttype, __f_t::struct_) && !__is_value_type(type))
                    __LogAndRet(constraint_expect_reference_type, type, name, __entity);

                if (bit_has_flag(constraint->cttype, __f_t::new_) &&
                    !__has_public_default_constructor(type))
                    __LogAndRet(constraint_expect_public_parameterless_constructor, type, name,
                                                                                __entity);
            }

            return true;
        }

        bool __is_type_compatible(type_t * from_type, type_t * to_type)
        {
            if (is_generic_param(from_type))
            {
                generic_param_t * gp = (generic_param_t *)from_type;
                generic_constraint_t * constraint = __search_constraint(gp->name);

                if (constraint == nullptr)
                    return false;

                for (type_name_t * type_name : constraint->type_names)
                {
                    type_t * type0 = type_name->type;
                    _A(type0 != nullptr);

                    if (is_type_compatible(type0, to_type))
                        return true;
                }

                return false;
            }

            return is_type_compatible(from_type, to_type);
        }

        // Returns whether it's a reference type.
        bool __is_ref_type(type_t * type)
        {
            if (is_generic_param(type))
            {
                generic_param_t * gp = (generic_param_t *)type;
                generic_constraint_t * constraint = __search_constraint(gp->name);

                return constraint != nullptr
                    && bit_has_flag(constraint->cttype, __f_t::class_);
            }

            return is_ref_type(type);
        }

        // Returns whether it's a value type.
        bool __is_value_type(type_t * type)
        {
            return !__is_ref_type(type);
        }

        // Returns whether a type has public default constructor.
        bool __has_public_default_constructor(type_t * type)
        {
            if (is_generic_param(type))
            {
                generic_param_t * gp = (generic_param_t *)type;
                generic_constraint_t * constraint = __search_constraint(gp->name);

                return constraint != nullptr && bit_has_flag(constraint->cttype, __f_t::new_);
            }

            return (is_class(type) || is_struct(type)) && has_public_default_constructor(type);
        }

        // Search constraint by name.
        generic_constraint_t * __search_constraint(name_t name)
        {
            // Search method
            {
                method_t * method = to_general(__wctx.current_method());
                generic_constraint_t * constraint = __search_constraint(method->constraints, name);

                if (constraint != nullptr)
                    return constraint;
            }

            // Search type
            {
                general_type_t * type = _M(general_type_t *, to_general(__wctx.current_type()));
                for (; type != nullptr; type = _M(general_type_t *, to_general(type->host_type)))
                {
                    generic_constraint_t * constraint = __search_constraint(type->constraints, name);

                    if (constraint != nullptr)
                        return constraint;
                }
            }

            return nullptr;
        }

        // Search constraint by name.
        generic_constraint_t * __search_constraint(generic_constraints_t * constraints, name_t name)
        {
            if (constraints == nullptr)
                return nullptr;

            for (generic_constraint_t * constraint : *constraints)
            {
                if (constraint->param_name == name)
                    return constraint;
            }

            return nullptr;
        }

        // Output and record logs.
        template<typename ... _args_t>
        void __log(common_log_code_t code, _args_t ... args)
        {
            ast_log(__cctx, __code_element, code, std::forward<_args_t>(args) ...);
        }

        #undef __LogAndRet
    };

    template<typename _entity_t, typename _code_element_t>
    bool __check_generic_constraints(ast_context_t & cctx, ast_walk_context_t & wctx,
                        _entity_t * entity, _code_element_t * code_element)
    {
        __check_generic_constraints_t c(cctx, wctx, entity, code_element);
        return c.check();
    }

    // Walks analysis step.
    bool general_type_name_ast_node_t::__walk_analysis(ast_walk_context_t & context)
   {
        type_t * type = this->type;

        if (type == nullptr)
            return true;

        // Check access modifier.
        this->__check_access(context, type, (general_type_name_t *)this);

        // Check 
        if (is_generic(type))
        {
            generic_type_t * generic_type = (generic_type_t *)type;
            __check_generic_constraints(this->__context, context, generic_type, this);
        }

        return true;
    }

    ////////// ////////// ////////// ////////// //////////
    // name_unit

    // Sets name.
    void name_unit_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(this->name, name, el, _T("type"));
    }

    // Commits this node.
    void name_unit_ast_node_t::on_commit()
    {
        this->args = __to_eobject<generic_args_t *>(__generic_args__);
    }

    // Returns this eobject.
    name_unit_t * name_unit_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool name_unit_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(this->name, this, __c_t::name_empty, _T("type name unit"));

        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // array_type_name

    // Commits this node.
    void array_type_name_ast_node_t::on_commit()
    {
        this->transform_child_to(__dimensions__, this->dimensions);

        this->element_type_name = __to_eobject<type_name_t *>(__element_type_name__);
    }

    // Returns this eobject.
    type_name_t * array_type_name_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool array_type_name_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                this->__check_empty(this->element_type_name, this,
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
        if (this->element_type_name != nullptr && this->type == nullptr)
            this->type = __ascertain_type(context, (array_type_name_t *)this);

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
        this->__assign_name(this->name, name, el, _T("typedef param"));
    }

    // Sets if it's a extends type.
    void type_def_param_ast_node_t::set_extends(bool extends)
    {
        this->extends = extends;
    }

    // Commits this node.
    void type_def_param_ast_node_t::on_commit()
    {
        // Empty.
    }

    // Returns this eobject.
    type_def_param_t * type_def_param_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool type_def_param_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(this->name, _T("typedef param"));

        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // type_def_params

    // Commits this node.
    void type_def_params_ast_node_t::on_commit()
    {
        this->transform_child_to(__params__, *this);
    }

    // Returns this eobject.
    type_def_params_t * type_def_params_ast_node_t::to_eobject()
    {
        return this;
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
        this->__assign_name(this->name, name, el, _T("typedef"));
    }

    // Commits this node.
    void type_def_ast_node_t::on_commit()
    {
        this->decorate   = __to_eobject<decorate_t *>(__decorate__);
        this->attributes = __to_eobject_with_owner<attributes_t *>(__attributes__,
                                                        (type_def_t *)this);
        this->type_name  = __to_eobject<type_name_t *>(__type_name__);
        this->params     = __to_eobject<type_def_params_t *>(__params__);
    }

    // Returns this eobject.
    type_def_t * type_def_ast_node_t::to_eobject()
    {
        return this;
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

        this->namespace_ = context.current_namespace();

        this->__check_empty(this->name, this, __c_t::type_name_missing, _T("typedef"));

        const mname_t * ns = ns_to_full_name(context.current_namespace());

        type_def_t * type_def = to_eobject();
        if (!context.assembly.types.append_type_def(to_sid(ns), type_def))
            this->__log(this, __c_t::type_def_defination_duplicate, type_def);

        if (context.current_type() == nullptr)   // outside types.
        {
            if (this->decorate != nullptr)
                this->__log(this, __c_t::unexpected_type_def_decorate);
        }

        if (context.current_method() != nullptr) // in a method, typedef statement.
        {
            if (this->decorate != nullptr)
                this->__log(this, __c_t::unexpected_type_def_decorate);

            if (this->attributes != nullptr)
                this->__log(this, __c_t::unexpected_type_def_attribute);
        }

        bool r = __super_t::on_walk(context, step, tag);
        context.pop();

        return r;
    }

    // Walks analysis step.
    bool type_def_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        this->__check_member(context.current_type(), (type_def_t *)this, __name_el);
        return true;
    }

    ////////// ////////// ////////// ////////// //////////
    // type_of

    // Commits this node.
    void type_of_ast_node_t::on_commit()
    {
        this->type_name = __to_eobject<type_name_t *>(__type_name__);
    }

    // Returns this eobject.
    expression_t * type_of_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool type_of_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(this->type_name, (ast_node_t *)this, __c_t::type_name_missing,
                                                                 _T("typeof"));
        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // argument

    // Sets name.
    void argument_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(this->name, name, el, _T("argument"));
    }

    // Sets argument type.
    void argument_ast_node_t::set_argument_type(param_type_t atype, __el_t * el)
    {
        this->__assign(this->atype, atype, el, _T("argument type"));
    }

    // Commits this node.
    void argument_ast_node_t::on_commit()
    {
        this->expression = __to_eobject<expression_t *>(__expression__);
        this->attributes = __to_eobject_with_owner<attributes_t *>(__attributes__,
                                                            (argument_t *)this);
    }

    // Returns this eobject.
    argument_t * argument_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool argument_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if (!__super_t::on_walk(context, step, tag))
            return false;

        this->__check_empty(this->expression, this, __c_t::argument_missing);

        typedef param_type_t t;
        if (this->atype == t::ref || this->atype == t::out)
        {
            if (this->expression != nullptr
                    && this->expression->this_family() != expression_family_t::name)
            {
                this->__log(this, __c_t::argument_type_error, _title(this->atype));
            }
        }

        return true;
    }

    ////////// ////////// ////////// ////////// //////////
    // arguments

    // Commits this node.
    void arguments_ast_node_t::on_commit()
    {
        this->transform_child_to(__arguments__, *this);
    }

    // Walks this node.
    bool arguments_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
    }

    // Returns this eobject.
    arguments_t * arguments_ast_node_t::to_eobject()
    {
        return this;
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
        return this;
    }

    // Sets value.
    void cvalue_ast_node_t::set_value(const cvalue_t & value)
    {
        *(cvalue_t *)this = value;
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
                [this]() { return (obj)->field; },                                      \
                [this](auto value) { (obj)->field = value; }                            \
        )

    // Walks this node.
    bool decorate_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
    }

    // Sets access.
    void decorate_ast_node_t::set_access(access_value_t value, __el_t * el)
    {
        this->__assign(__BitField(this, access), value, el, _T("access"), true);
    }

    // Get access.
    access_value_t decorate_ast_node_t::get_access()
    {
        return this->access;
    }

    // Sets static.
    void decorate_ast_node_t::set_static(__el_t * el)
    {
        this->__assign(__BitField(this, is_static), true, el, _T("static"), true);
    }

    // Sets sealed.
    void decorate_ast_node_t::set_sealed(__el_t * el)
    {
        this->__assign(__BitField(this, is_sealed), true, el, _T("sealed"), true);
    }

    // Sets override.
    void decorate_ast_node_t::set_override(__el_t * el)
    {
        this->__assign(__BitField(this, is_override), true, el, _T("override"), true);
    }

    // Sets virtual.
    void decorate_ast_node_t::set_virtual(__el_t * el)
    {
        this->__assign(__BitField(this, is_virtual), true, el, _T("virtual"), true);
    }

    // Sets abstract.
    void decorate_ast_node_t::set_abstract(__el_t * el)
    {
        this->__assign(__BitField(this, is_abstract), true, el, _T("abstract"), true);
    }

    // Sets new.
    void decorate_ast_node_t::set_new(__el_t * el)
    {
        this->__assign(__BitField(this, is_new), true, el, _T("new"), true);
    }

    // Sets readonly.
    void decorate_ast_node_t::set_readonly(__el_t * el)
    {
        this->__assign(__BitField(this, is_readonly), true, el, _T("readonly"), true);
    }

    // Sets constant.
    void decorate_ast_node_t::set_const(__el_t * el)
    {
        this->__assign(__BitField(this, is_const), true, el, _T("const"), true);
    }

    // Sets extern.
    void decorate_ast_node_t::set_extern(__el_t * el)
    {
        this->__assign(__BitField(this, is_extern), true, el, _T("extern"), true);
    }

    // Get decorate value.
    decorate_value_t decorate_ast_node_t::get_decorate()
    {
        return (decorate_value_t)*(decorate_t *)this;
    }

    // Commits this node.
    void decorate_ast_node_t::on_commit()
    {
        // Empty.
    }

    // Returns this eobject.
    decorate_t * decorate_ast_node_t::to_eobject()
    {
        return this;
    }

    ////////// ////////// ////////// ////////// //////////
    // statements

    // Commits this node.
    void statements_ast_node_t::on_commit()
    {
        this->transform_child_to(__statements__, *this);
    }

    // Returns this eobject.
    statements_t * statements_ast_node_t::to_eobject()
    {
        return this;
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
        this->transform_child_to(__statements__, this->statements);
    }

    // Returns this eobject.
    method_body_t * method_body_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool method_body_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                this->variable_region = context.current_region();
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
        __e_exit_type_t exit_type = exit_type_of(ctx, &this->statements);

        // method_t * method = context.current_method();
        // _PF(_T("%1%: %2%"), method, _eflags(exit_type));

        if (exit_type.has(__exit_type_t::pass) || exit_type == __exit_type_t::none)
        {
            method_t * method = context.current_method();
            _A(method != nullptr);

            if (method->type_name == nullptr || method->type_name->type == nullptr
                                             || is_void_type(method->type_name->type))
            {
                this->push_back(__new_obj<return_statement_t>());
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
            this->append_child(__nodes__, node);
    }

    // Appends expression.
    void expression_ast_node_t::append_expression(ast_node_t * node)
    {
        _A(node != nullptr);

        expression_t * expression = _M(expression_ast_t *, node)->to_eobject();
        __es.push_back(__el_t(expression));

        this->append_child(__nodes__, node);
    }

    // Appends constant value.
    void expression_ast_node_t::append_cvalue(ast_node_t * node)
    {
        _A(node != nullptr);

        cvalue_t * value = _M(cvalue_ast_t *, node)->to_eobject();
        __es.push_back(__el_t(to_sys_exp(__get_memory(), __get_lang(), nullptr, value)));

        this->append_child(__nodes__, node);
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
        if (this->child_at(__underlying__) == nullptr)
            this->__log(this, __c_t::statement_missing);
    }

    // Returns this eobject.
    statement_t * statement_ast_node_t::to_eobject()
    {
        return __to_eobject<statement_t *>(__underlying__);
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
        this->__assign_name(this->alias, alias, el, _T("import alias"));
    }

    // Commits this node.
    void import_ast_node_t::on_commit()
    {
        this->package  = __to_eobject<emname_t *>(__package__);
        this->assembly = __to_eobject<emname_t *>(__assembly__);
    }

    // Returns this eobject.
    import_t * import_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool import_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        import_t * import = to_eobject();

        if (!this->assembly)
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
        this->alias = name;
    }

    // Commits this node.
    void using_ast_node_t::on_commit()
    {
        this->ns = __to_eobject<emname_t *>(__ns__);
    }

    // Returns this eobject.
    using_namespace_t * using_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool using_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if (!this->ns)
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
        this->name = __to_eobject<emname_t *>(__name__);

        this->transform_child_to(__types__,      this->types);
        this->transform_child_to(__namespaces__, this->namespaces);
        this->transform_child_to(__type_defs__,  this->type_defs);
    }

    // Returns this eobject.
    namespace_t * namespace_ast_node_t::to_eobject()
    {
        return this;
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
        this->__assign_name(this->name, name, el, _T("type name"));
    }

    // Sets ttype.
    void type_ast_node_t::set_ttype(ttype_t ttype, __el_t * el)
    {
        this->ttype = ttype;
    }

    // Appends member.
    void type_ast_node_t::append_member(member_t * member)
    {
        this->append_member(member);
    }

    // Commits this node.
    void type_ast_node_t::on_commit()
    {
        this->params      = __to_eobject<generic_params_t *>(__generic_params__);
        this->decorate    = __to_eobject<decorate_t *>(__decorate__);
        this->attributes  = __to_eobject_with_owner<attributes_t *>(__attributes__,
                                                                (general_type_t *)this);
        this->constraints = __to_eobject<generic_constraints_t *>(__generic_constraints__);

        this->transform_child_to(__methods__,           this->methods);
        this->transform_child_to(__properties__,        this->properties);
        this->transform_child_to(__fields__,            this->fields);
        this->transform_child_to(__events__,            this->events);
        this->transform_child_to(__nest_types__,        this->nest_types);
        this->transform_child_to(__type_defs__,         this->type_defs);
        this->transform_child_to(__super_type_names__,  this->super_type_names);
    }

    // Returns this eobject.
    general_type_t * type_ast_node_t::to_eobject()
    {
        return this;
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
                    field->decorate->access    = access_value_t::public_;
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
            auto it = al::find_if(this->methods, [](method_t * m) {
                return m->trait == method_trait_t::constructor;
            });

            if (it == std::end(this->methods))  // no constructor
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

    template<typename _entity_t, typename _code_element_t>
    class __check_generic_template_constraints_t
    {
        typedef generic_constraint_ttype_t __f_t;

    public:

        __check_generic_template_constraints_t(ast_context_t & cctx, _entity_t * entity,
                                                _code_element_t * code_element)
            : __cctx(cctx), __entity(entity), __code_element(code_element)
        {
            _A(entity != nullptr);
        }

        #define __LogAndRet(_code, _element, _args ...)    do {                         \
                this->__log(__c_t::_code, _element, ##_args);                           \
                return false;                                                           \
            } while (false)

        bool check()
        {
            generic_constraints_t * constraints = __entity->constraints;
            if (constraints == nullptr)
                return true;

            // __param_types.fill_by_general(__entity);

            bool ret = true;
            std::set<name_t> names;

            for (generic_constraint_t * constraint : *constraints)
            {
                if (!set_insert(names, constraint->param_name))
                {
                    __LogAndRet(constraint_duplicate_specified, constraint, constraint->param_name);
                    continue;
                }

                if (!__check(constraint))
                    ret = false;
            }

            // Circle dependency checking.
            __check_circle_dependency();

            return ret;
        }

    private:

        ast_context_t &         __cctx;
        _entity_t *             __entity;
        _code_element_t *       __code_element;

        // __generic_param_types_t     __param_types;
        typedef std::list<name_t>   __names_t;

        struct __dependency_t
        {
            generic_constraint_t *  constraint;
            __names_t               names;
        };

        std::vector<name_t> __param_dependency_names;
        std::map<name_t, __dependency_t> __param_dependencies;

        bool __check(generic_constraint_t * constraint)
        {
            std::set<type_t *> types;

            // Check types.
            {
                int index = 0;

                for (type_name_t * type_name : constraint->type_names)
                {
                    type_t * type0 = to_type(type_name);
                    _A(type0 != nullptr);

                    // Duplicate?
                    if (!al::set_insert(types, type0))
                        __LogAndRet(constraint_duplicate_types, type_name, type0,
                                                                constraint->param_name);

                    if (is_class(type0) &&
                        bit_has_flag(constraint->cttype, __f_t::class_, __f_t::struct_))
                        __LogAndRet(constraint_unexpected_class_or_struct, type_name, type0);

                    // Cannot be specified types.
                    if (type0 == __XPool.get_object_type())
                        __LogAndRet(constraint_cannot_be_special_type, type_name, type0);
                    
                    // Should not be sealed types.
                    if (type0->is_sealed())
                        __LogAndRet(constraint_invalid_type, type_name, type0);

                    // Class types position.
                    if (is_class(type0))
                    {
                        if (index >= 1)
                            __LogAndRet(constraint_type_position_error, type_name, type0);
                    }

                    // Circle dependency checking.
                    if (is_generic_param(type0))
                    {
                        __append_dependency(constraint->param_name,
                            ((generic_param_t *)type0)->name, constraint
                        );
                    }

                    index++;
                }
            }

            // Duplicate?
            {
                int c = 0;
                #define __HasFlag(_flag) bit_has_flag(constraint->cttype, __f_t::_flag)

                #define __ValidateFlag(_flag) do { if (__HasFlag(_flag)) c++; } while (false)

                __ValidateFlag(class_);
                __ValidateFlag(struct_);

                if (c > 1)
                    __LogAndRet(constraint_combined_or_duplicated, constraint);

                if (__HasFlag(struct_) && __HasFlag(new_))
                    __LogAndRet(constraint_new_cannot_be_used_with_struct, constraint);

                #undef __ValidateFlag
                #undef __HasFlag
            }

            return true;
        }

        void __append_dependency(name_t name, name_t dependency_name,
                                                    generic_constraint_t * constraint)
        {
            bool created_new;
            __dependency_t & dependency = al::map_get(__param_dependencies, name, &created_new);

            if (created_new)
                __param_dependency_names.push_back(name);

            dependency.constraint = constraint;
            dependency.names.push_back(dependency_name);
        }

        struct __check_circle_dependency_context_t
        {
            std::set<name_t>    walked_names;
            std::list<name_t>   path;
        };

        void __check_circle_dependency()
        {
            for (name_t name : __param_dependency_names)
            {
                __dependency_t & dependency = __param_dependencies[name];

                __check_circle_dependency_context_t context;
                context.walked_names.insert(name);
                context.path.push_back(name);

                if (!__check_circle_dependency(context, dependency.names))
                {
                    auto & path = context.path;
                    string_t s_path = al::join_str(std::begin(path), std::end(path), _T("->"),
                        [](auto && it) { return (string_t)it; }
                    );
                    __log(__c_t::constraint_circular_dependency, dependency.constraint, s_path);
                    return;
                }
            }
        }

        bool __check_circle_dependency(__check_circle_dependency_context_t & ctx,
                                        __names_t & dependency_names)
        {
            for (name_t dependency_name : dependency_names)
            {
                if (!__check_circle_dependency(ctx, dependency_name))
                    return false;
            }

            return true;
        }

        bool __check_circle_dependency(__check_circle_dependency_context_t & ctx,
                                        name_t dependency_name)
        {
            ctx.path.push_back(dependency_name);

            if (!al::set_insert(ctx.walked_names, dependency_name))
                return false;

            auto it = __param_dependencies.find(dependency_name);
            if (it != std::end(__param_dependencies))
            {
                __dependency_t & dependency = it->second;
                if (!__check_circle_dependency(ctx, dependency.names))
                    return false;
            }

            return true;
        }

        template<typename _element_t, typename ... _args_t>
        void __log(common_log_code_t code, _element_t * element, _args_t ... args)
        {
            ast_log(__cctx, element, code, std::forward<_args_t>(args) ...);
        }

        #undef __LogAndRet
    };

    template<typename _entity_t, typename _code_element_t>
    bool __check_generic_template_constraints(ast_context_t & cctx, _entity_t * entity,
                                     _code_element_t * code_element)
    {
        __check_generic_template_constraints_t c(cctx, entity, code_element);
        return c.check();
    }

    // Walks analysis step.
    bool type_ast_node_t::__walk_analysis(ast_walk_context_t & context, void * tag)
    {
        if (tag == nullptr)
        {
            // Check circle base types.
            type_t * circle_type = __check_circle_base_types();
            if (circle_type != nullptr)
            {
                this->__log(__name_el, __c_t::circle_base_type_dependency,
                    this->to_full_name(), circle_type->to_full_name()
                );
            }

            // Check constraints.
            if (this->constraints != nullptr && this->constraints->size() > 0)
                __check_generic_template_constraints(this->__context, this, this);
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

        method->name     = this->name;
        method->decorate = __new_obj<decorate_t>();
        method->decorate->access = access_value_t::public_;
        method->trait    = method_trait_t::constructor;
        method->body     = __new_obj<method_body_t>(context.current_region());

        method->body->push_back(__new_obj<return_statement_t>());

        this->methods.push_back(method);

        return method;
    }

    // Check circle base types.
    type_t * type_ast_node_t::__check_circle_base_types()
    {
        return this->each_super_type([this](type_t * super_type) {

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

        if (super_type == this)
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
        this->__assign_name(this->name, name, el, _T("generic param"));
    }

    // Sets param type.
    void generic_param_ast_node_t::set_param_type(generic_param_type_t type, __el_t * el)
    {
        this->__assign(this->param_type, type, el, _T("generic param type"));
    }

    // Commits this node.
    void generic_param_ast_node_t::on_commit()
    {
       this->attributes = __to_eobject_with_owner<attributes_t *>(__attributes__,
                                                            (generic_param_t *)this);
    }

    // Returns this eobject.
    generic_param_t * generic_param_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool generic_param_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(this->name, _T("template param"));

        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_params

    // Commits this node.
    void generic_params_ast_node_t::on_commit()
    {
        this->transform_child_to(__params__, *this);
    }

    // Returns this eobject.
    generic_params_t * generic_params_ast_node_t::to_eobject()
    {
        return this;
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
        this->__assign_bit(this->cttype, cttype, el, _T("constraint type"), true);

        switch (cttype)
        {
            case generic_constraint_ttype_t::class_:
            case generic_constraint_ttype_t::struct_:
                if (this->has_child(__type_names__))
                    this->__log(el, __c_t::constraint_must_be_first);
                break;

            case generic_constraint_ttype_t::new_:
            default:
                break;
        }
    }

    // Sets name.
    void generic_constraint_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(this->param_name, name, el, _T("generic param"));
    }

    // Commits this node.
    void generic_constraint_ast_node_t::on_commit()
    {
        this->transform_child_to(__type_names__, this->type_names);
    }

    // Returns this eobject.
    generic_constraint_t * generic_constraint_ast_node_t::to_eobject()
    {
        return this;
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
        this->transform_child_to(__constraints__, *this);
    }

    // Returns this eobject.
    generic_constraints_t * generic_constraints_ast_node_t::to_eobject()
    {
        return this;
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
        this->atype = type;
    }

    // Commits this node.
    void generic_arg_ast_node_t::on_commit()
    {
        this->type_name = __to_eobject<type_name_t *>(__type_name__);
    }

    // Returns this eobject.
    generic_arg_t * generic_arg_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool generic_arg_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(this->type_name, this, __c_t::type_name_missing, _T("generic argument"));

        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_args

    // Commits this node.
    void generic_args_ast_node_t::on_commit()
    {
        this->transform_child_to(__generic_args__, *this);
    }

    // Returns this eobject.
    generic_args_t * generic_args_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool generic_args_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if (this->empty())
            this->__log(this, __c_t::type_name_missing, _T("generic arguments"));

        if (__is_partial_specialization(*this))
            this->__log(this, __c_t::partial_specialization_not_supported);

        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // param

    // Sets name.
    void param_ast_node_t::set_name(const name_t & name, __el_t * el)
    {
        this->__assign_name(this->name, name, el, _T("param"));
    }

    // Sets param type.
    void param_ast_node_t::set_param_type(param_type_t ptype, __el_t * el)
    {
        this->__assign(this->ptype, ptype, el, _T("param type"));
    }

    // Commits this node.
    void param_ast_node_t::on_commit()
    {
        this->type_name     = __to_eobject<type_name_t *>(__type_name__);
        this->default_value = __to_eobject<expression_t *>(__default_value__);
        this->attributes    = __to_eobject_with_owner<attributes_t *>(__attributes__,
                                                                 (param_t *)this);
    }

    // Returns this eobject.
    param_t * param_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool param_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                this->__check_name_empty(this->name, _T("param"));
                this->__check_empty(this->type_name, this, __c_t::type_name_missing, _T("param"));

                this->__delay(context, walk_step_t::pre_analysis);
                return __super_t::on_walk(context, step, tag);

            case walk_step_t::pre_analysis:
                if (this->default_value != nullptr)
                {
                    expression_t * exp = this->default_value;

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
        this->transform_child_to(__params__, *this);
    }

    // Returns this eobject.
    params_t * params_ast_node_t::to_eobject()
    {
        return this;
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
        this->type_name = __to_eobject<type_name_t *>(__type_name__);
        this->arguments = __to_eobject<arguments_t *>(__arguments__);
    }

    // Returns this eobject.
    attribute_t * attribute_ast_node_t::to_eobject()
    {
        if (__is_compile_time_attribute(this))
            return __get_compile_time_attribute();

        return this;
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
                if (__is_compile_time_attribute(this))
                    __get_compile_time_attribute()->type_name = this->type_name;

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
        type_name_t * type_name = this->type_name;
        type_t * type;

        if (type_name == nullptr || (type = type_name->type) == nullptr || is_unknown(type))
            return true;

        method_t * method = find_constructor(this->__context, type, this->arguments);
        if (method == nullptr)
        {
            __log(this, __c_t::attribute_constructor_not_match, type,
                to_arg_types_str(this->__context, this->arguments)
            );

            return true;
        }

        if (this->arguments != nullptr)
        {
            for (argument_t * argument : *this->arguments)
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
        this->transform_child_to(__attributes__, *this);
    }

    // Returns this eobject.
    attributes_t * attributes_ast_node_t::to_eobject()
    {
        return this;
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
        this->expression = __to_eobject<expression_t *>(__expression__);
    }

    // Returns this eobject.
    statement_t * expression_st_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool expression_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(this->expression, this, __c_t::expression_missing,
                                                          _T("statement"));
        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // type_def_st

    // Commits this node.
    void type_def_st_ast_node_t::on_commit()
    {
        this->type_def = __to_eobject<type_def_t *>(__type_def__);
    }

    // Returns this eobject.
    statement_t * type_def_st_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool type_def_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(this->type_def, this, __c_t::statement_missing);

        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // defination_st

    // Commits this node.
    void defination_st_ast_node_t::on_commit()
    {
        this->type_name = __to_eobject<type_name_t *>(__type_name__);

        this->transform_child_to(__items__, *this);
    }

    // Returns this eobject.
    statement_t * defination_st_ast_node_t::to_eobject()
    {
        return this;
    }

    // Set constant.
    void defination_st_ast_node_t::set_constant(bool constant)
    {
        this->constant = constant;
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
        this->__check_empty(this->type_name, this, __c_t::type_name_missing,
                                                        _T("variable defination"));

        variable_defination_t vd(this->__context, context, (defination_statement_t *)this);

        for (defination_statement_item_t * var_item : this->items)
        {
            type_name_t * type_name = __to_eobject<type_name_t *>(__type_name__);

            var_item->variable = vd.define_local(
                type_name, var_item->name, this->constant,
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
            this->__log(this->child_at(__items__, index), __c_t::_code, ##_args)

        for (int index = 0, count = this->items.size(); index < count; ++index)
        {
            defination_statement_item_t * var_item = this->items[index];
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

        if (this->constant)
        {
            for (int index = 0, count = this->items.size(); index < count; ++index)
            {
                defination_statement_item_t * var_item = this->items[index];

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
        return this;
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
        return this;
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
        this->expression = __to_eobject<expression_t *>(__expression__);
    }

    // Returns this eobject.
    statement_t * throw_st_ast_node_t::to_eobject()
    {
        return this;
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
        this->__assign_name(this->label, label, el, _T("label"));
    }

    // Commits this node.
    void goto_st_ast_node_t::on_commit()
    {

    }

    // Returns this eobject.
    statement_t * goto_st_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool goto_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(this->label, _T("label"));

        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // return_st

    // Commits this node.
    void return_st_ast_node_t::on_commit()
    {
        this->expression = __to_eobject<expression_t *>(__expression__);
    }

    // Returns this eobject.
    statement_t * return_st_ast_node_t::to_eobject()
    {
        return this;
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
            if (this->expression != nullptr)
                this->__log(this, __c_t::method_unexpected_return_value, method);
        }
        else
        {
            if (this->expression == nullptr)
            {
                this->__log(this, __c_t::method_return_value_missing, method);
            }
            else
            {
                type_t * return_type = this->expression->get_type();
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
        this->condition = __to_eobject<expression_t *>(__condition__);
        this->body      = __to_eobject<statement_t *>(__body__);
    }

    // Returns this eobject.
    statement_t * do_while_st_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool do_while_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(this->condition, this, __c_t::condition_missing, _T("do while"));
        this->__check_empty(this->body, this, __c_t::body_missing, _T("do while"));

        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // loop_until_st

    // Commits this node.
    void loop_until_st_ast_node_t::on_commit()
    {
        this->condition = __to_eobject<expression_t *>(__condition__);
        this->body      = __to_eobject<statement_t *>(__body__);
    }

    // Returns this eobject.
    statement_t * loop_until_st_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool loop_until_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(this->condition, this, __c_t::condition_missing, _T("loop until"));
        this->__check_empty(this->body, this, __c_t::body_missing, _T("loop until"));

        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // while_st

    // Commits this node.
    void while_st_ast_node_t::on_commit()
    {
        this->condition = __to_eobject<expression_t *>(__condition__);
        this->body      = __to_eobject<statement_t *>(__body__);
    }

    // Returns this eobject.
    statement_t * while_st_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool while_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(this->condition, this, __c_t::condition_missing, _T("while"));
        this->__check_empty(this->body, this, __c_t::body_missing, _T("while"));

        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // for_st

    // Commits this node.
    void for_st_ast_node_t::on_commit()
    {
        auto init_statement = __try_to_eobject<statement_t *>(__initialize__);
        if (init_statement != nullptr)
            this->defination_initialize = _M(defination_statement_t *, init_statement);
        else
            this->initialize = __to_eobject<expression_t *>(__initialize__);

        this->condition  = __to_eobject<expression_t *>(__condition__);
        this->increase   = __to_eobject<expression_t *>(__increase__);
        this->body       = __to_eobject<statement_t *>(__body__);
    }

    // Returns this eobject.
    statement_t * for_st_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool for_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(this->body, this, __c_t::body_missing, _T("while"));

        if (this->defination_initialize != nullptr)
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
        this->__assign_name(this->variable, name, el, _T("var"));
    }

    // Commits this node.
    void for_each_st_ast_node_t::on_commit()
    {
        this->iterator   = __to_eobject<expression_t *>(__iterator__);
        this->type_name  = __to_eobject<type_name_t *>(__type_name__);
        this->body       = __to_eobject<statement_t *>(__body__);
    }

    // Returns this eobject.
    statement_t * for_each_st_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool for_each_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(this->variable, _T("variable"));
        this->__check_empty(this->iterator, this, __c_t::iterator_missing, "for each");
        this->__check_empty(this->body, this, __c_t::body_missing, _T("for each"));

        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // if_st

    // Commits this node.
    void if_st_ast_node_t::on_commit()
    {
        this->condition   = __to_eobject<expression_t *>(__condition__);
        this->if_body     = __to_eobject<statement_t *>(__if_body__);
        this->else_body   = __to_eobject<statement_t *>(__else_body__);

        this->__check_empty(this->condition, this, __c_t::condition_missing, "if");
        this->__check_empty(this->if_body, this, __c_t::body_missing, _T("if"));
    }

    // Returns this eobject.
    statement_t * if_st_ast_node_t::to_eobject()
    {
        return this;
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
        this->expression = __to_eobject<expression_t *>(__expression__);
        this->transform_child_to(__cases__, this->cases);
    }

    // Returns this eobject.
    statement_t * switch_st_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool switch_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(this->expression, this, __c_t::expression_missing, _T("switch"));

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

        this->append_child(__constants__, exp_node);
    }

    // Commits this node.
    void case_ast_node_t::on_commit()
    {
        this->transform_child_to(__constants__, this->constants);
        this->statements = __to_eobject<statements_t *>(__statements__);
    }

    // Returns this eobject.
    case_t * case_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool case_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(this->constants, this, __c_t::expression_missing, _T("case"));

        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // try_st

    // Commits this node.
    void try_st_ast_node_t::on_commit()
    {
        this->try_statement     = __to_eobject<statement_t *>(__try__);
        this->finally_statement = __to_eobject<statement_t *>(__finally__);

        this->transform_child_to(__catches__, this->catches);
    }

    // Returns this eobject.
    statement_t * try_st_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool try_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if (this->catches.empty() && this->finally_statement == nullptr)
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
        this->__assign_name(this->name, name, el, _T("variable"));
        __variable_el = el;
    }

    // Commits this node.
    void catch_ast_node_t::on_commit()
    {
        this->type_name = __to_eobject<type_name_t *>(__type_name__);
        this->body      = __to_eobject<statement_t *>(__body__);
    }

    // Returns this eobject.
    catch_t * catch_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool catch_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if (this->type_name == nullptr)
            this->type_name = __XPool.get_object_type_name();

        if (this->name.empty())
            this->name = __XPool.to_name(_T("__catch_exception_temp_variable__"));

        context.push_new_region();

        variable_defination_t vd(this->__context, context, __variable_el);
        this->variable = vd.define_local(this->type_name, this->name);

        bool r = __super_t::on_walk(context, step, tag);

        context.pop();

        return r;
    }

    ////////// ////////// ////////// ////////// //////////
    // empty_st

    // Returns this eobject.
    statement_t * empty_st_ast_node_t::to_eobject()
    {
        return this;
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
        this->transform_child_to(__statements__, *this);
    }

    // Returns this eobject.
    statement_t * statement_group_st_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool statement_group_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        context.push_new_region();

        this->variable_region = context.current_region();
        bool r = __super_t::on_walk(context, step, tag);

        context.pop();

        return r;
    }

    ////////// ////////// ////////// ////////// //////////
    // type_name_exp

    // Commits this node.
    void type_name_exp_ast_node_t::on_commit()
    {
        this->type_name = __to_eobject<type_name_t *>(__type_name__);
    }

    // Returns this eobject.
    expression_t * type_name_exp_ast_node_t::to_eobject()
    {
        return this;
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
        this->type_name  = __to_eobject<type_name_t *>(__type_name__);
        this->set_expression(__to_eobject<expression_t *>(__expression__));
    }

    // Returns this eobject.
    expression_t * type_cast_exp_ast_node_t::to_eobject()
    {
        return this;
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
        this->set_namex(__to_eobject<expression_t *>(__namex__));
        this->generic_args  = __to_eobject<generic_args_t *>(__generic_args__);

        this->set_arguments(__to_eobject<arguments_t *>(__arguments__));
    }

    // Returns this eobject.
    expression_t * function_ast_node_t::to_eobject()
    {
        return this;
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
        this->__check_name_empty(this->namex, _T("function"));

        if (this->generic_args != nullptr && this->get_name() == name_t::null)
        {
            this->__log(this->child_at(__generic_args__), __c_t::function_generic_args_redundance);
        }

        return true;
    }

    // Walks analysis step.
    bool function_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        // Check permissions.
        if (this->get_ftype() != function_expression_type_t::method)
            return true;

        method_base_t * method = this->get_method();
        if (method == nullptr)
            return true;

        this->__check_access(context, method, this->child_at(__namex__));

        if (method->this_family() == member_family_t::generic)
        {
            generic_method_t * generic_method = (generic_method_t *)method;
            method_t * template_ = generic_method->template_;
            _A(template_ != nullptr);

            if (template_->constraints != nullptr && template_->constraints->size() > 0)
                __check_generic_constraints(this->__context, context, generic_method,
                                                                    (ast_node_t *)this);
        }

        return true;
    }

    ////////// ////////// ////////// ////////// //////////
    // index

    // Commits this node.
    void index_ast_node_t::on_commit()
    {
        this->set_namex(__to_eobject<expression_t *>(__namex__));
        this->set_arguments(__to_eobject<arguments_t *>(__arguments__));
    }

    // Returns this eobject.
    expression_t * index_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool index_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(this->namex(), _T("index"));

        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // new

    // Commits this node.
    void new_ast_node_t::on_commit()
    {
        this->type_name = __to_eobject<type_name_t *>(__type_name__);
        this->set_arguments(__to_eobject<arguments_t *>(__arguments__));
    }

    // Returns this eobject.
    expression_t * new_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool new_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(this->type_name, (ast_node_t *)this, __c_t::type_name_missing,
                                                            _T("object creation"));
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                this->__delay(context, walk_step_t::analysis);
                return __super_t::on_walk(context, step, tag);

            case walk_step_t::analysis: {
                type_t * type = to_type(this->type_name);
                if (type != nullptr)
                {
                    this->constructor = find_constructor(
                        this->__context, type, this->arguments()
                    );

                    if (this->constructor == nullptr)
                    {
                        __log((ast_node_t *)this, __c_t::type_constructor_not_match, type,
                            to_arg_types_str(this->__context, this->arguments())
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
        this->type_name = __to_eobject<type_name_t *>(__type_name__);
        this->set_lengths(__to_eobject<array_lengths_t *>(__lengths__));
        this->set_initializer(__to_eobject<array_initializer_t *>(__initializer__));
    }

    // Returns this eobject.
    expression_t * new_array_ast_node_t::to_eobject()
    {
        return this;
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

                    if (this->initializer() != nullptr)
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
        this->__check_empty(this->type_name, (ast_node_t *)this, __c_t::type_name_missing,
                                                            _T("array creation"));
        this->__check_empty(this->lengths(), (ast_node_t *)this, __c_t::array_lengths_missing);

        // dimension
        array_lengths_t * p_lengths = this->__to_eobject<array_lengths_t *>(__lengths__);
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
                        this->set_length(dimension, __to_value_expression(length));
                    else
                        this->__log((ast_node_t *)this, __c_t::expected_array_length);
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
        this->to_array_type();
        return true;
    }

    // Walks analysis step.
    bool new_array_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        array_initializer_t * initializer = this->initializer();
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
        array_initializer_t * initializer = this->initializer();

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
        this->each_child_node<ast_node_t>(__elements__, [&, this](ast_node_t * node) {

            expression_ast_t * expression_ast;
            array_initializer_ast_t * initializer_ast;

            if ((expression_ast = as<expression_ast_t *>(node)) != nullptr)
            {
                this->append(expression_ast->to_eobject());
            }
            else if ((initializer_ast = as<array_initializer_ast_t *>(node)) != nullptr)
            {
                this->append(initializer_ast->to_eobject());
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
        return this;
    }

    // Walks this node.
    bool array_initializer_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        try
        {
            this->check();
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
        this->transform_child_to(__lengths__, *this);
    }

    // Returns this eobject.
    array_lengths_t * array_lengths_ast_node_t::to_eobject()
    {
        return this;
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
        this->type_name = __to_eobject<type_name_t *>(__type_name__);
    }

    // Returns this eobject.
    expression_t * default_value_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool default_value_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(this->type_name, (ast_node_t *)this, __c_t::type_name_missing,
                                                        _T("default value"));

        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // field

    // Sets name.
    void field_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__name_el = el;
        this->__assign_name(this->name, name, el, _T("field"));
    }

    // Commits this node.
    void field_ast_node_t::on_commit()
    {
        this->decorate   = __to_eobject<decorate_t *>(__decorate__);
        this->type_name  = __to_eobject<type_name_t *>(__type_name__);
        this->init_value = __to_eobject<expression_t *>(__init_value__);
        this->attributes = __to_eobject_with_owner<attributes_t *>(__attributes__,
                                                        (field_t *)this);
    }

    // Returns this eobject.
    field_t * field_ast_node_t::to_eobject()
    {
        return this;
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
            this->__check_empty(this->type_name, this, __c_t::type_name_missing,
                    _F(_T("field %1%"), _str(this->name)));
            this->__check_empty(this->name, this, __c_t::name_empty, _T("field"));

            variable_defination_t vd(this->__context, context, (field_t *)this);
            vd.define_field(this);
        }

        return r;
    }

    // Walks analysis step.
    bool field_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        this->__check_member(context.current_type(), (field_t *)this, __name_el);
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

        this->__assign_name(this->name, name, el, _T("method"));
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

        if (!this->name.empty())
        {
            __log(el, __c_t::duplicate, _F(_T("method name / operator overload")), op_property);
            return;
        }

        this->__op_property = op_property;

        const char_t * op_name = op_property->name;
        _A(op_name != nullptr);

        this->name = __to_name(_F(_T("op_%1%"), op_name));
    }

    // Sets trait.
    void method_ast_node_t::set_trait(method_trait_t trait, __el_t * el)
    {
        this->__assign(this->trait, trait, el, _T("method trait"));
    }

    // Commits it.
    void method_ast_node_t::on_commit()
    {
        this->type_name       = __to_eobject<type_name_t *>(__type_name__);
        this->decorate        = __to_eobject<decorate_t *>(__decorate__);
        this->params          = __to_eobject<params_t *>(__params__);
        this->generic_params  = __to_eobject<generic_params_t *>(__generic_params__);
        this->owner_type_name = __to_eobject<type_name_t *>(__owner_type_name__);
        this->body            = __to_eobject<method_body_t *>(__body__);
        this->constraints     = __to_eobject<generic_constraints_t *>(__generic_constraints__);
        this->attributes      = __to_eobject_with_owner<attributes_t *>(__attributes__,
                                                                (method_t *)this);
    }

    // Returns this eobject.
    method_t * method_ast_node_t::to_eobject()
    {
        return this;
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

                if (this->trait == method_trait_t::normal)
                {
                    variable_defination_t vd(this->__context, context, (method_t *)this);
                    vd.define_method(this);
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
        if (type != nullptr && this->name == type->get_name()
                                && this->owner_type_name == nullptr)  // constructor
        {
            if (this->trait == method_trait_t::__default__)
            {
                if (this->decorate && this->decorate->is_static)
                    this->trait = method_trait_t::static_constructor;
                else
                    this->trait = method_trait_t::constructor;
            }
        }
        else
        {
            if (this->trait == method_trait_t::__default__)
                this->trait = method_trait_t::normal;

            this->__check_empty(this->type_name, this, __c_t::type_name_missing,
                _F(_T("method %1%"), _str(this->name))
            );
        }

        if (this->type_name != nullptr)
        {
            switch (this->trait)
            {
                case method_trait_t::constructor:
                    this->__log(this, __c_t::constructor_method_should_no_return_type,
                                                        _str(this->name));
                    break;

                case method_trait_t::static_constructor:
                    this->__log(this, __c_t::static_constructor_method_should_no_return_type,
                                                        _str(this->name));
                    break;

                case method_trait_t::destructor:
                    this->__log(this, __c_t::destructor_method_should_no_return_type,
                                                        _str(this->name));
                    break;

                default:
                    break;
            }
        }

        this->__check_empty(this->name, this, __c_t::name_empty, _T("method"));

        if (this->params && !this->params->empty())
        {
            variable_defination_t vd(this->__context, context, (method_t *)this);

            for (param_t * param : *this->params)
            {
                vd.define_param(param);
            }

            switch (this->trait)
            {
                case method_trait_t::static_constructor:
                    this->__log(this, __c_t::static_constructor_method_should_no_params,
                                                        _str(this->name));
                    break;

                case method_trait_t::destructor:
                    this->__log(this, __c_t::destructor_method_should_no_params,
                                                        _str(this->name));
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
        switch (this->trait)
        {
            case method_trait_t::static_constructor:
                break;

            case method_trait_t::constructor:
                __method_utils_t(__context, context).revise_constructor(*this,
                    __to_eobject<arguments_t *>(__base_ctor_args__),
                    this->child_at(__base_ctor_args__)
                );
                break;

            case method_trait_t::destructor:
                break;

            default:
                break;
        }

        // Check duplicated.
        if (this->relation_member == nullptr)
            this->__check_member(context.current_type(), (method_t *)this, __name_el);

        // Check constraints.
        if (this->constraints != nullptr && this->constraints->size() > 0)
            __check_generic_template_constraints(this->__context, this, this);

        // Operator overload, checks prototype.
        if (__op_property == nullptr)
            return true;

        if (!this->is_static() || !this->is_public())
            this->__log(this, __c_t::operator_overload_prototype_error);

        if (this->param_count() != __op_property->arity)
        {
            this->__log(this, (__op_property->arity == 1)?
                    __c_t::unitary_operator_overload_wrong_param_count :
                    __c_t::binary_operator_overload_wrong_param_count, __op_property->op
            );
        }

        bool has_containing_type = al::any_of(*this->params, [&](param_t * param) {
            return param->get_type() == context.current_type();
        });

        if (!has_containing_type)
        {
            this->__log(this, (__op_property->arity == 1)?
                    __c_t::unitary_operator_overload_wrong_containing_type :
                    __c_t::binary_operator_overload_wrong_containing_type, __op_property->op
            );
        }

        type_t * return_type = ((__w_t<method_t> *)this)->get_type();
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

        this->__assign_name(this->name, name, el, _T("property"));
    }

    // Commits this node.
    void property_ast_node_t::on_commit()
    {
        this->type_name  = __to_eobject<type_name_t *>(__type_name__);
        this->decorate   = __to_eobject<decorate_t *>(__decorate__);
        this->get_method = __to_eobject<method_t *>(__get_method__);
        this->set_method = __to_eobject<method_t *>(__set_method__);
        this->params     = __to_eobject<params_t *>(__params__);
        this->owner_type_name = __to_eobject<type_name_t *>(__owner_type_name__);
        this->attributes = __to_eobject_with_owner<attributes_t *>(__attributes__,
                                                            (property_t *)this);
    }

    // Returns this eobject.
    property_t * property_ast_node_t::to_eobject()
    {
        return this;
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
        bool has_error = false;

        if (!this->name.empty())
        {
            if (this->params != nullptr)
            {
                has_error = true;
                this->__log(this->child_at(__params__), __c_t::unexpected_params, "property");
            }
        }
        else
        {
            if (this->params == nullptr)
            {
                has_error = true;
                this->__log(this, __c_t::name_empty, _T("property"));
            }
            else
            {
                this->name = __XPool.to_name(PropertyIndexName);
            }
        }

        this->__append_default_methods(context);

        if (!has_error)
        {
            variable_region_t * region = context.current_region();

            variable_defination_t vd(this->__context, context, (property_t *)this);
            vd.define_property(this);
        }

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

        if (this->get_method != nullptr)
        {
            this->get_method->name      = __to_method_name(_T("get"));
            this->get_method->type_name = this->type_name;
            this->get_method->decorate  = this->decorate;
            this->get_method->params    = this->params;
            this->get_method->relation_member = this;
            this->get_method->owner_type_name = this->owner_type_name;

            __append_member(this->__context, context, this->get_method);
        }

        param_t * value_param = nullptr;
        xpool_t & xpool = __get_xpool();

        if (this->set_method != nullptr)
        {
            this->set_method->name      = __to_method_name(_T("set"));
            this->set_method->type_name = xpool.to_type_name(xpool.get_void_type());
            this->set_method->decorate  = this->decorate;
            this->set_method->relation_member = this;
            this->set_method->owner_type_name = this->owner_type_name;

            params_t * params = __new_obj<params_t>();

            if (this->params != nullptr)
                al::copy(*this->params, std::back_inserter(*params));

            value_param = __new_obj<param_t>(this->type_name, __to_name(_T("value")));
            params->push_back(value_param);

            this->set_method->params = params;
            __append_member(this->__context, context, this->set_method);
        }

        if (type->this_ttype() != ttype_t::interface_ && !this->is_abstract())
            __generate_ignored_method_bodies(context, value_param);
    }

    // Walks analysis step.
    bool property_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        this->__check_member(context.current_type(), (property_t *)this, __name_el);
        return true;
    }

    // Generates ignored method bodies.
    void property_ast_node_t::__generate_ignored_method_bodies(ast_walk_context_t & context,
                                                               param_t * value_param)
    {
        #define __AccessorEmpty(name)                                                   \
            (this->name##_method == nullptr)

        #define __BodyEmpty(name)                                                       \
            (this->name##_method->body == nullptr)

        #define __BodyIgnored(name)                                                     \
            (!__AccessorEmpty(name) && __BodyEmpty(name))

        if (__AccessorEmpty(get) && __AccessorEmpty(set))
        {
            this->__log(this, __c_t::property_accessor_missing, (property_t *)this);
            return;
        }

        if (!__AccessorEmpty(get) && !__AccessorEmpty(set))
        {
            if (__BodyEmpty(get) != __BodyEmpty(set))
            {
                this->__log(this, __c_t::property_method_body_missing,
                    __BodyEmpty(set)? _T("set") : _T("get")
                );

                return;
            }
        }

        if (!__BodyIgnored(get) && !__BodyIgnored(set))
            return;

        // Appends a field for the property.
        field_t * field = __new_obj<field_t>();
        field->type_name = this->type_name;
        field->name = __to_name(_F(_T("__%1%__"), this->name));

        decorate_value_t dv = decorate_value_t::default_value;
        dv.is_static = this->is_static();
        dv.access = access_value_t::private_;

        field->decorate = __new_obj<decorate_t>(dv);

        __append_member(this->__context, context, field);

        // Appends get method body.
        if (__BodyIgnored(get))
        {
            statement_t * statement = __new_obj<return_statement_t>(
                __new_field_expression(field)
            );

            this->get_method->body = __new_obj<method_body_t>(context.current_region());
            this->get_method->body->push_back(statement);
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

            this->set_method->body = body;
        }

        #undef __AccessorEmpty
        #undef __BodyEmpty
        #undef __BodyIgnored
    }

    // Gets get/set method name.
    name_t property_ast_node_t::__to_method_name(const char_t * prefix)
    {
        if (this->name != name_t::null)
            return __to_name(_F(_T("%1%_%2%"), prefix, this->name));

        return __to_name(_F(_T("%1%_Item"), prefix));
    }

    ////////// ////////// ////////// ////////// //////////
    // event

    // Sets name.
    void event_ast_node_t::set_name(name_t name, __el_t * el)
    {
        __name_el = el;
        this->__assign_name(this->name, name, el, _T("event"));
    }

    // Commits this node.
    void event_ast_node_t::on_commit()
    {
        this->type_name       = __to_eobject<type_name_t *>(__type_name__);
        this->decorate        = __to_eobject<decorate_t *>(__decorate__);
        this->add_method      = __to_eobject<method_t *>(__add_method__);
        this->remove_method   = __to_eobject<method_t *>(__remove_method__);
        this->owner_type_name = __to_eobject<type_name_t *>(__owner_type_name__);
        this->attributes      = __to_eobject_with_owner<attributes_t *>(__attributes__,
                                                                (event_t *)this);
    }

    // Returns this eobject.
    event_t * event_ast_node_t::to_eobject()
    {
        return this;
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
        this->__check_name_empty(this->name, _T("event"));

        if (this->add_method != nullptr)
        {
            this->add_method->name = __to_name(_F(_T("add_%1%"), this->name));
            this->add_method->type_name = this->type_name;
            this->add_method->relation_member = this;
            this->add_method->owner_type_name = this->owner_type_name;
            __append_member(this->__context, context, this->add_method);
        }

        if (this->remove_method != nullptr)
        {
            this->remove_method->name = __to_name(_F(_T("remove_%1%"), this->name));
            this->remove_method->type_name = this->type_name;
            this->remove_method->relation_member = this;
            this->remove_method->owner_type_name = this->owner_type_name;
            __append_member(this->__context, context, this->remove_method);
        }

        context.push(this->to_eobject());
        __super_t::on_walk(context, step, tag);
        context.pop();

        return true;
    }

    // Walks analysis step.
    bool event_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        this->__check_member(context.current_type(), (event_t *)this, __name_el);
        return true;
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::compile

