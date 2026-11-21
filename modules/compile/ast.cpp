
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

        // A name that declares a variable.
        X_C(declaration_exp,            _T("declaration_exp"))

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

        // Checked expression.
        X_C(checked_exp,                _T("checked_exp"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Ast error codes.
    X_ENUM_INFO(ast_error_code_t)

        // Multi-name empty.
        X_C(mname_empty, _T("mname_empty"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(common_log_code_t)

        // ---- warnings.

        // Hides inherit virtual member.
        X_D(hides_inherit_virtual_member,
            _T("\"%1%\" hides inherited member \"%2%\": add \"override\" to override ")
            _T("it, or \"new\" to hide it"))

        // Hides inherit member.
        X_D(hides_inherit_member,
            _T("\"%1%\" hides inherited member \"%2%\": add \"new\" if hiding is ")
            _T("intended"))

        // An event has the add accessor or the remove accessor, not both.
        X_D(event_accessor_missing, _T("the event \"%1%\" has no \"%2%\" accessor: add and remove go together"))

        // The type of an event is not a delegate.
        X_D(event_type_not_delegate, _T("the type \"%1%\" of the event \"%2%\" is not a delegate (System.Delegate<...>)"))

        // An event is used as a value outside of the type that has it.
        X_D(event_cannot_be_used, _T("the event \"%1%\" can only be added to or removed from (+=, -=) outside of the type that defines it"))

        // The raise accessor is not valid for the event.
        X_D(event_raise_not_valid, _T("the event \"%1%\" cannot have a raise accessor: %2%"))

        // The member does not hide an accessible member. The new keyword is not required.
        X_D(not_hides_inherit_member, _T("\"%1%\" does not hide an accessible member: \"new\" is not required"))

        // A value that can be null is converted to a type that cannot be null.
        X_D(nullable_convert, _T("converting a possibly null value to the non-nullable type \"%1%\""))

        // The null literal is converted to a type that cannot be null.
        X_D(nullable_null_literal, _T("cannot convert the null literal to the non-nullable type \"%1%\""))

        // A value that can be null is dereferenced.
        X_D(nullable_dereference, _T("dereference of a possibly null reference \"%1%\""))

        // A value that can be null is returned by a method that returns a type that cannot be.
        X_D(nullable_return, _T("possible null reference return"))

        // A value that can be null is an argument of a param that cannot be null.
        X_D(nullable_argument, _T("possible null reference argument for parameter \"%1%\" of \"%2%\""))

        // A field that cannot be null is not initialized when a constructor exits.
        X_D(nullable_field_uninitialized,
            _T("non-nullable field \"%1%\" must contain a non-null value when the ")
            _T("constructor exits, consider declaring it as nullable"))

        // The argument of a generic type can be null, the param cannot be.
        X_D(nullable_constraint,
            _T("the nullable type \"%1%\" cannot be used as type parameter \"%2%\" of ")
            _T("\"%3%\", which cannot be null"))

        // ---- AP0100 - AP0199: malformed code (names, missing parts, things that are not allowed there).

        // Name empty.
        X_D(name_empty, _T("%1% name is missing"))

        // Assembly name missing.
        X_D(assembly_name_missing, _T("package name is missing"))

        // Namespace name missing.
        X_D(namespace_name_missing, _T("namespace name is missing"))

        // Type name missing.
        X_D(type_name_missing, _T("the type of %1% is missing"))

        // Argument missing.
        X_D(argument_missing, _T("argument is missing"))

        // Condition missing.
        X_D(condition_missing, _T("%1% condition is missing"))

        // Body missing.
        X_D(body_missing, _T("%1% body is missing"))

        // Iterator missing.
        X_D(iterator_missing, _T("%1% requires an expression to iterate"))

        // Statement missing.
        X_D(statement_missing, _T("statement is missing"))

        // Expression missing.
        X_D(expression_missing, _T("%1% requires an expression"))

        // Catch or finally missing.
        X_D(catch_or_finally_missing, _T("\"try\" requires a \"catch\" or a \"finally\" block"))

        // Array lengths missing.
        X_D(array_lengths_missing, _T("length of array \"%1%\" is missing"))

        // Expected array length.
        X_D(expected_array_length, _T("array length is missing"))

        // Unexpected first char in a name.
        X_D(name_unexpected_first_char, _T("unexpected first character \"%1%\" in name \"%2%\""))

        // Unexpected char in a name.
        X_D(name_unexpected_char, _T("unexpected character \"%1%\" in name \"%2%\""))

        // Argument type error.
        X_D(argument_type_error, _T("%1% argument must be a variable"))

        // Unexpected param default value.
        X_D(unexpected_param_default_value, _T("%1%"))

        // Unexpected params.
        X_D(unexpected_params, _T("unexpected %1% parameters"))

        // Generic arguments redundance in function.
        X_D(function_generic_args_redundance, _T("the generic arguments of the function are redundant"))

        // Unexpected variable defination.
        X_D(unexpected_variable_defination, _T("variables cannot be defined here: \"%1%\""))

        // Unexpected field defination.
        X_D(unexpected_field_defination, _T("fields cannot be defined here: \"%1%\""))

        // Unexpected property defination.
        X_D(unexpected_property_defination, _T("properties cannot be defined here: \"%1%\""))

        // Unexpected method defination.
        X_D(unexpected_method_defination, _T("methods cannot be defined here: \"%1%\""))

        // Unexpected param defination.
        X_D(unexpected_param_defination, _T("parameters cannot be defined here: \"%1%\""))

        // Unexpected typedef decorate.
        X_D(unexpected_type_def_decorate, _T("modifiers on a typedef are only allowed when it is a member of a type"))

        // Unexpected typedef attribute.
        X_D(unexpected_type_def_attribute, _T("attributes cannot be applied to a typedef statement"))

        // Unexpected import.
        X_D(unexpected_import, _T("unexpected import \"%1%\""))

        // Unexpected using namespace.
        X_D(unexpected_using_namespace, _T("unexpected using namespace \"%1%\""))

        // The language has no goto statement.
        X_D(goto_not_supported, _T("\"goto\" is not supported"))

        // Array initializer error.
        X_D(array_initializer_error, _T("invalid array initializer: %1%"))

        // Partial specialization not supported.
        X_D(partial_specialization_not_supported, _T("partial specialization of a generic type is not supported"))

        // ---- AP2100 - AP2199: members and modifiers.

        // The modifier is not valid for this item.
        X_D(modifier_not_valid, _T("the modifier \"%1%\" is not valid for this item"))

        // The modifier is not valid for interface members.
        X_D(modifier_not_valid_for_interface_members, _T("the modifier \"%1%\" is not valid for interface members"))

        // Virtual or abstract members cannot be private
        X_D(virtual_member_cannot_be_private, _T("\"%1%\": virtual or abstract members cannot be private"))

        // A static member cannot be marked as override, virtual, or abstract
        X_D(static_member_cannot_be_virtual,
            _T("static member \"%1%\" cannot be marked as \"override\", \"virtual\" ")
            _T("or \"abstract\""))

        // A member marked as override cannot be marked as new or virtual
        X_D(override_member_cannot_be_new,
            _T("member \"%1%\" marked as \"override\" cannot be marked as \"new\" or ")
            _T("\"virtual\""))

        // Member is abstract but it is contained in non-abstract type.
        X_D(abstract_member_on_non_abstract_type,
            _T("\"%1%\" is abstract but it is contained in the non-abstract type ")
            _T("\"%2%\""))

        // Extern members should be marked as static.
        X_D(extern_member_should_be_static, _T("extern member \"%1%\" must be static"))

        // Extern method cannot declare a body.
        X_D(extern_method_cannot_declare_body, _T("extern method \"%1%\" cannot have a body"))

        // Abstract method cannot declare a body.
        X_D(abstract_method_cannot_declare_body, _T("\"%1%\": an abstract member cannot have a body"))

        // Cannot be sealed because it is not an override member.
        X_D(sealed_member_but_not_override, _T("\"%1%\": only an override member can be sealed"))

        // No suitable member found to override.
        X_D(no_suitable_member_to_override, _T("\"%1%\": no suitable member found to override"))

        // Cannot override inherited member because it is sealed.
        X_D(cannot_override_sealed_members, _T("\"%1%\": cannot override inherited member \"%2%\" because it is sealed"))

        // Return type changed when override inherit members.
        X_D(override_member_return_type_changed,
            _T("\"%1%\": return type must be \"%2%\" to match overridden member ")
            _T("\"%3%\""))

        // Cannot change access modifiers when overriding inherited member.
        X_D(access_modifier_changed,
            _T("\"%1%\": cannot change the access modifier \"%2%\" when overriding ")
            _T("inherited member \"%3%\""))

        // Cannot change return types when override inherited members.
        X_D(return_type_changed,
            _T("\"%1%\": return type must be \"%2%\" to match overridden member ")
            _T("\"%3%\""))

        // Member is inaccessible to its protection level.
        X_D(inaccessible_protection_level, _T("\"%1%\" is inaccessible due to its protection level"))

        // ---- AP2200 - AP2299: types, inheritance and interfaces.

        // No base type.
        X_D(no_base_type, _T("type \"%1%\" has no base type"))

        // Circular base type dependency.
        X_D(circle_base_type_dependency, _T("circular base type dependency between \"%1%\" and \"%2%\""))

        // A type is declared partial in a place, and is not in another.
        X_D(partial_modifier_missing,
            _T("type \"%1%\" is declared partial in another declaration, the ")
            _T("\"partial\" modifier is missing here"))

        // The partial declarations of a type disagree: kind, access, generic params, base class.
        X_D(partial_conflict, _T("the partial declarations of type \"%1%\" have conflicting %2%"))

        // A non-abstract type does not implement an inherited abstract member.
        X_D(abstract_member_not_implemented, _T("\"%1%\" does not implement inherited abstract member \"%2%\""))

        // A non-abstract type does not implement an interface member.
        X_D(interface_member_not_implemented, _T("\"%1%\" does not implement interface member \"%2%\""))

        // Interfaces cannot contain instance fields.
        X_D(interfaces_cannot_contain_instance_fields, _T("interfaces cannot contain instance fields"))

        // Interface members cannot have defination.
        X_D(interface_members_cannot_have_defination, _T("\"%1%\": interface members cannot have a definition"))

        // A static member of an interface that is not abstract must have a body.
        X_D(interface_static_member_needs_body,
            _T("\"%1%\": a static member of an interface must have a body unless it ")
            _T("is abstract"))

        // A static abstract (or virtual) member is used through the interface.
        X_D(interface_static_member_through_interface,
            _T("\"%1%\": a static abstract or virtual member of an interface can be ")
            _T("used only through a generic parameter that has the interface as a ")
            _T("constraint"))

        // Do not need a default constructor.
        X_D(do_not_need_default_constructor, _T("struct \"%1%\" cannot define a parameterless constructor"))

        // ---- AP2300 - AP2399: constructors, properties and constants.

        // Constructor method should no return type.
        X_D(constructor_method_should_no_return_type, _T("constructor \"%1%\" cannot have a return type"))

        // Destructor method should no return type.
        X_D(destructor_method_should_no_return_type, _T("destructor \"%1%\" cannot have a return type"))

        // Static constructor method should no return type.
        X_D(static_constructor_method_should_no_return_type, _T("static constructor \"%1%\" cannot have a return type"))

        // Destructor method should no params.
        X_D(destructor_method_should_no_params, _T("destructor \"%1%\" cannot have parameters"))

        // Static constructor method should no params.
        X_D(static_constructor_method_should_no_params, _T("static constructor \"%1%\" cannot have parameters"))

        // A constructor cannot call itself.
        X_D(constructor_calls_itself, _T("constructor of \"%1%\" calls itself"))

        // Property method body uniformity.
        X_D(property_method_body_missing, _T("property \"%1%\" must declare a body"))

        // Property or indexer must have at least one accessor.
        X_D(property_accessor_missing, _T("property or indexer \"%1%\" must have at least one accessor"))

        // A constant variable require a value to be provided.
        X_D(constant_variable_initialize_missing, _T("constant \"%1%\" requires a value"))

        // The expression beging assigned to constant variable must be constant.
        X_D(constant_variable_required_constant_value, _T("the value assigned to \"%1%\" must be a constant"))

        // Expect constant value.
        X_D(expect_constant_value, _T("the expression \"%1%\" assigned to \"%2%\" must be a constant"))

        // Invalid initialize value.
        X_D(invalid_initialize_value, _T("invalid initial value \"%1%\""))

        // The param "this" is not the first param.
        X_D(this_param_not_first, _T("the modifier \"this\" can only be on the first param"))

        // An extension method is not a static method of a static class.
        X_D(extension_method_wrong_container,
            _T("an extension method must be a static method of a static class that is not generic"))

        // ---- AP2400 - AP2499: enums.

        // Unexpected decorate on enum fields.
        X_D(unexpected_enum_decorate, _T("unexpected modifier \"%1%\" on enum field \"%2%\""))

        // Unexpected type on enum fields.
        X_D(unexpected_enum_type, _T("unexpected type \"%1%\" on enum field \"%2%\""))

        // Enum field prototype error,
        X_D(enum_field_prototype_error, _T("enum field \"%1%\" must be static and const"))

        // Unexpected underlying type.
        X_D(unexpected_underlying_type, _T("the underlying type of an enum must be an integer type"))

        // The enum value is too large
        X_D(enum_value_too_large, _T("the enum value %1% is too large for underlying type \"%2%\""))

        // The enum value is too small
        X_D(enum_value_too_small, _T("the enum value %1% is too small for underlying type \"%2%\""))

        // The evaluation of the constant value involves a circular definition.
        X_D(enum_field_circular_definition,
            _T("the evaluation of the constant value of \"%1%\" involves a circular ")
            _T("definition"))

        // An enum has no field except const ones, no event, and no constructor: its value is the integer, and its static
        // data has no place for them.
        X_D(enum_instance_member_error,
            _T("enum \"%1%\" cannot have the field or event \"%2%\", only const fields are allowed"))

        X_D(enum_constructor_error, _T("enum \"%1%\" cannot have a constructor"))

        // ---- AP2500 - AP2599: operator overloading.

        // Operator overload prototype error.
        X_D(operator_overload_prototype_error, _T("an operator overload must be declared public and static"))

        // Unitary operator overload prototype error: Wrong param count.
        X_D(unitary_operator_overload_wrong_param_count, _T("overloaded unary operator \"%1%\" takes one parameter"))

        // Binary operator overload prototype error: Wrong param count.
        X_D(binary_operator_overload_wrong_param_count, _T("overloaded binary operator \"%1%\" takes two parameters"))

        // Operator overload prototype error: One of the parameters must be the containing type.
        X_D(unitary_operator_overload_wrong_containing_type, _T("the parameter of unary operator \"%1%\" must be the containing type"))

        // Operator overload prototype error: The parameter must be the containing type.
        X_D(binary_operator_overload_wrong_containing_type,
            _T("one of the parameters of binary operator \"%1%\" must be the ")
            _T("containing type"))

        // Operator overload prototype error; User-defined operators cannot return void.
        X_D(operator_overload_cannot_return_void, _T("user-defined operators cannot return void"))

        // Operator cannot ber overloaded.
        X_D(operator_cannot_be_overloaded, _T("operator \"%1%\" cannot be overloaded"))

        // A conversion operator must be implicit or explicit.
        X_D(conversion_kind_missing, _T("a conversion operator must be declared implicit or explicit"))

        // A conversion operator takes one parameter.
        X_D(conversion_wrong_param_count, _T("a conversion operator takes one parameter"))

        // A conversion operator converts from or to the containing type.
        X_D(conversion_wrong_containing_type,
            _T("the parameter or the result of a conversion operator must be the containing type"))

        // ---- AP3100 - AP3199: definitions and duplicates.

        // Duplicated
        X_D(duplicate, _T("duplicate %1% \"%2%\""))

        // Variable defination duplicated.
        X_D(variable_defination_duplicate, _T("a variable named \"%1%\" is already defined in this scope"))

        // Type defination duplicated.
        X_D(type_defination_duplicate, _T("type \"%1%\" is already defined"))

        // Typedef defination duplicated.
        X_D(type_def_defination_duplicate, _T("typedef \"%1%\" is already defined"))

        // Member defination duplicated.
        X_D(member_defination_duplicated, _T("type \"%1%\" already defines a member \"%2%\" with the same name"))

        // Member names cannot be the same as their enclosing type.
        X_D(member_same_as_enclosing_type, _T("member names cannot be the same as their enclosing type"))

        // A static field of a generic type has an initial value.
        X_D(static_init_in_generic_type,
            _T("the static field \"%1%\" of the generic type \"%2%\" cannot have an initial ")
            _T("value, assign it in a method"))

        // ---- AP3200 - AP3299: names and lookup.

        // Variable undefined.
        X_D(variable_undefined, _T("variable \"%1%\" is not defined"))

        // Type undefined.
        X_D(type_undefined, _T("type \"%1%\" is not defined"))

        // Typedef unsolved.
        X_D(type_def_unsolved, _T("typedef \"%1%\" cannot be resolved"))

        // Type undetermind.
        X_D(type_undetermind, _T("cannot determine the type of \"%1%\""))

        // Cannot determine local type.
        X_D(cannot_determine_local_variable_type, _T("cannot determine the type of local variable \"%1%\""))

        // Property index undetermind.
        X_D(property_index_undetermind, _T("cannot determine the property index \"%1%\""))

        // Member not found.
        X_D(member_not_found, _T("member \"%1%\" is not defined in type \"%2%\""))

        // Method not found.
        X_D(method_not_found, _T("method \"%1%\" is not defined in type \"%2%\""))

        // Method not match.
        X_D(method_not_match, _T("no overload of method \"%1%\" in type \"%2%\" matches the call"))

        // Attribute constructor not match.
        X_D(attribute_constructor_not_match, _T("no constructor of attribute \"%1%\" matches (%2%)"))

        // The field or the property of an attribute is not found.
        X_D(attribute_named_argument_not_found, _T("attribute \"%1%\" has no field or property \"%2%\" to assign"))

        // Type consturctor not matched.
        X_D(type_constructor_not_match, _T("no constructor of type \"%1%\" matches (%2%)"))

        // Constructor method not found.
        X_D(constructor_method_not_found, _T("constructor \"%1%(%2%)\" is not found"))

        // Method conflict.
        X_D(method_conflict, _T("method \"%1%\" conflicts in type \"%2%\": %3%"))

        // Cannot find method with prototype.
        X_D(cannot_find_method_with_prototype, _T("cannot find a method with prototype \"%1%\""))

        // Unexpected symbol.
        X_D(unexpected_symbol, _T("unexpected symbol \"%1%\""))

        // A variable that is not a delegate is called.
        X_D(variable_not_callable, _T("\"%1%\" is of the type \"%2%\", it is not a delegate and cannot be called"))
        X_D(property_not_callable, _T("the delegate of the property \"%1%\" cannot be called: read it into a variable first"))

        // ---- AP3300 - AP3399: assemblies.

        // A reference of itself.
        X_D(assembly_reference_itself, _T("assembly \"%1%\" references itself"))

        // Assembly circular reference.
        X_D(assembly_circular_reference, _T("assembly \"%1%\" has a circular reference"))

        // Assembly not found.
        X_D(assembly_not_found, _T("assembly \"%1%\" is not found"))

        // ---- AP5100 - AP5199: methods and statements.

        // Not all code paths return a value.
        X_D(method_no_return, _T("method \"%1%\": not all code paths return a value"))

        // The return keyword must not be followed by an expression in void method.
        X_D(method_unexpected_return_value,
            _T("method \"%1%\": \"return\" cannot be followed by an expression in a ")
            _T("void method"))

        // The return keyword must followed by an expression in non-void method.
        X_D(method_return_value_missing,
            _T("method \"%1%\": \"return\" must be followed by an expression in a ")
            _T("non-void method"))

        // Incompatible return type.
        X_D(method_incompatible_return_value, _T("method \"%1%\": incompatible return type"))

        // The type of the expression of a for each statement is not supported.
        X_D(for_each_type_not_supported, _T("\"foreach\" is not supported for type \"%1%\""))

        // The type of the variable of a using statement has no Dispose().
        X_D(using_type_not_disposable, _T("\"using\" requires a type with Dispose(): \"%1%\""))
        X_D(lock_type_not_reference, _T("\"lock\" requires a reference: \"%1%\""))

        // ---- AP5200 - AP5299: lambdas and iterators.

        // A lambda is only in a method.
        X_D(lambda_outside_method, _T("a lambda can only be used in a method"))

        // The return type of a lambda is that of the delegate it is converted to.
        X_D(lambda_type_undetermined,
            _T("the type of the lambda cannot be determined, it is not converted to a ")
            _T("delegate"))

        // A lambda that uses local variables is not supported here.
        X_D(lambda_capture_not_supported, _T("a lambda that uses local variables is not supported: %1%"))

        // A local variable that a lambda uses is modified.
        X_D(lambda_capture_modified, _T("local variable \"%1%\" is used by a lambda and cannot be modified"))

        // A lambda that uses local variables uses a member of the object.
        X_D(lambda_capture_member,
            _T("a lambda that uses local variables cannot use the members of the ")
            _T("object"))

        // The iterator cannot be made.
        X_D(iterator_not_supported, _T("iterator \"%1%\" is not supported: %2%"))

        // A method with yield returns IEnumerable<T> or IEnumerator<T>.
        X_D(iterator_return_type_invalid,
            _T("\"%1%\": a method that has yield statements must return ")
            _T("IEnumerable<T> or IEnumerator<T>"))

        // A yield statement is only in a method that returns IEnumerable<T> or IEnumerator<T>.
        X_D(yield_outside_iterator,
            _T("\"yield\" can only be used in a method that returns IEnumerable<T> or ")
            _T("IEnumerator<T>"))

        // ---- AP5300 - AP5399: expressions.

        // Unexpected expression.
        X_D(unexpected_expression, _T("unexpected expression \"%1%\""))

        // Invalid expression.
        X_D(invalid_expression, _T("invalid expression \"%1%\""))

        // Cannot implicitly convert a type to another type.
        X_D(invalid_type_cast, _T("cannot implicitly convert type \"%1%\" to \"%2%\""))

        // The type arguments for method cannot be inferred from the usage. Try specifying the type arguments explicitly.
        X_D(type_arguments_cannot_be_inferred,
            _T("the type arguments of method \"%1%\" in type \"%2%\" cannot be ")
            _T("inferred from the usage, specify them explicitly"))

        // Operator overload not defined.
        X_D(operator_overload_not_defined, _T("operator \"%1%\" is not defined for %2%"))

        // The operator is ambiguous: more than one method fits the operands.
        X_D(operator_overload_ambiguous, _T("operator \"%1%\" is ambiguous for %2%"))

        // The value is assigned to a member of ?. .
        X_D(null_conditional_assign,
            _T("the left-hand side of an assignment cannot contain a null-conditional ")
            _T("operator"))

        // The operator ?. is used with the object of a type that cannot be null.
        X_D(null_conditional_wrong_type,
            _T("operator \"?.\" cannot be applied to an operand of type \"%1%\", ")
            _T("which cannot be null"))

        // A constant operation overflows in checked mode.
        X_D(constant_overflow, _T("the operation overflows at compile time in checked mode"))

        // A constant conversion overflows in checked mode.
        X_D(conversion_overflow, _T("the conversion overflows at compile time in checked mode"))

        // An array is indexed by a number of indices that is not one, or its dimension.
        X_D(array_index_count_mismatch,
            _T("the array \"%1%\" cannot be indexed by %2% indices, expected %3%"))

        // Only the items of a params generic parameter, or a tuple of them, can be passed with ... .
        X_D(items_expected,
            _T("only the items of a params generic parameter, or a tuple of them, can be passed ")
            _T("with \"...\""))

        // The items of a params generic parameter are passed with ... .
        X_D(items_without_dots,
            _T("the items of a params generic parameter \"%1%\" are many arguments, they are ")
            _T("passed with ...: \"%1% ...\""))

        // An object initializer of a type that is not a class.
        X_D(object_initializer_not_supported,
            _T("the members of an object of the type \"%1%\" cannot be set by an object initializer, ")
            _T("only the objects of a class can"))

        // An init only property is set where it cannot be.
        X_D(init_only_property_assigned,
            _T("the property \"%1%\" has an init accessor: it is set only in an object initializer, ")
            _T("in a constructor of its type, or by its own accessor"))

        // ---- AP6100 - AP6199: generics and constraints.

        // Class1<T>' does not define type parameter 'K'
        X_D(generic_parameter_not_defined, _T("\"%1%\" does not define type parameter \"%2%\""))

        // The type 'string' cannot be used as type parameter 'T' in the generic type or method 'Class1<T>'. There is no implicit reference conversion from 'string' to 'Project1.Class2'.
        X_D(constraint_no_implicit_conversion,
            _T("type \"%1%\" cannot be used as type parameter \"%2%\" in \"%3%\": ")
            _T("there is no implicit reference conversion from \"%1%\" to \"%4%\""))

        // The type 'string' must be a non-nullable value type in order to use it as parameter 'T' in the generic type or method 'Class1<T>'
        X_D(constraint_expect_value_type,
            _T("type \"%1%\" must be a non-nullable value type to be used as ")
            _T("parameter \"%2%\" in \"%3%\""))

        // The type 'int' must be a reference type in order to use it as parameter 'T' in the generic type or method 'Class1<T>'
        X_D(constraint_expect_reference_type,
            _T("type \"%1%\" must be a reference type to be used as parameter \"%2%\" ")
            _T("in \"%3%\""))

        // 'long' is not a valid constraint. A type used as a constraint must be an interface, a non-sealed class or a type parameter
        X_D(constraint_invalid_type,
            _T("\"%1%\" is not a valid constraint: a constraint type must be an ")
            _T("interface, a non-sealed class or a type parameter"))

        // The 'class', 'struct', 'unmanaged', 'notnull', and 'default' constraints cannot be combined or duplicated.
        X_D(constraint_combined_or_duplicated,
            _T("the \"class\", \"struct\", \"unmanaged\", \"notnull\" and \"default\" ")
            _T("constraints cannot be combined or duplicated"))

        // The 'class', 'struct', 'unmanaged', 'notnull', and 'default' constraints must be specified first in the constraints list.
        X_D(constraint_must_be_first,
            _T("the \"class\", \"struct\", \"unmanaged\", \"notnull\" and \"default\" ")
            _T("constraints must be specified first in the constraints list"))

        // The 'new()' constraint cannot be used with the 'struct' constraint
        X_D(constraint_new_cannot_be_used_with_struct, _T("the \"new()\" constraint cannot be used with the \"struct\" constraint"))

        // 'Class2' must be a non-abstract type with a public parameterless constructor in order to use it as parameter 'T' in the generic type or method 'Class1<T>'
        X_D(constraint_expect_public_parameterless_constructor,
            _T("\"%1%\" must be a non-abstract type with a public parameterless ")
            _T("constructor to be used as parameter \"%2%\" in \"%3%\""))

        // Duplicate constraint 'Class2' for type parameter 'T'
        X_D(constraint_duplicate_types, _T("duplicate constraint \"%1%\" for type parameter \"%2%\""))

        // Constraint cannot be special class 'object'
        X_D(constraint_cannot_be_special_type, _T("constraint cannot be the special class \"%1%\""))

        // A constraint clause has already been specified for type parameter 'T'. All of the constraints for a type parameter must be specified in a single where clause.
        X_D(constraint_duplicate_specified,
            _T("a constraint clause is already specified for type parameter \"%1%\": ")
            _T("all constraints of a type parameter must be in one \"where\" clause"))

        // The class type constraint 'Class3' must come before any other constraints
        X_D(constraint_type_position_error, _T("the class constraint \"%1%\" must come before any other constraints"))

        // Circular constraint dependency involving 'T' and 'K'
        X_D(constraint_circular_dependency, _T("circular constraint dependency involving \"%1%\""))

        // The type 'string' must be a type that has no references in order to use it as parameter 'T' in the generic type or method 'Class1<T>'
        X_D(constraint_expect_unmanaged_type,
            _T("type \"%1%\" must be an unmanaged type (a number, bool, char, pointer, enum, or a struct ")
            _T("that has only them) to be used as parameter \"%2%\" in \"%3%\""))

        // 'Class3': cannot specify both a constraint class and the 'class' or 'struct' constraint
        X_D(constraint_unexpected_class_or_struct,
            _T("\"%1%\": cannot specify both a constraint class and the \"class\" or ")
            _T("\"struct\" constraint"))

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

            // The static members of an interface (methods and properties) are of three kinds:
            //   static          a function of the interface, with a body;
            //   static virtual  has a default body, that a type that implements it can replace;
            //   static abstract has no body, a type that implements it must have the member.
            // They are called through the interface (static), or through a generic param that has
            // the interface as a constraint (all). The others have not the modifiers.
            bool static_member = member->is_static()
                                    && al::in(member_type, m_t::method, m_t::property);

            // Modifier not valid for interface members.
            const char_t * modifier =
                (member->is_virtual() && !static_member)?   _T("virtual") :
                (member->is_static() && !static_member)?    _T("static") :
                (member->is_abstract() && !static_member)?  _T("abstract") :
                member->is_sealed()?    _T("sealed") :
                member->is_const()?     _T("const") :
                member->is_readonly()?  _T("readonly") :
                member->is_extern()?    _T("extern") :
                (member->has_access_modifier() && !static_member)? _T("access modifier") : nullptr
            ;

            if (modifier != nullptr)
            {
                this->__log(ce, c_t::modifier_not_valid_for_interface_members, modifier);
                return false;
            }

            if (member->is_abstract() && member->is_virtual())
            {
                this->__log(ce, c_t::modifier_not_valid_for_interface_members, _T("virtual"));
                return false;
            }

            // The methods: an instance method and a static abstract one have no body, the others
            // (static, static virtual) have it.
            if (member_type == m_t::method)
            {
                method_t * method = (method_t *)member;
                bool need_body = member->is_static() && !member->is_abstract();

                if (method->body != nullptr && !need_body)
                {
                    this->__log(ce, c_t::interface_members_cannot_have_defination,
                                                                    member->to_prototype());
                    return false;
                }

                if (method->body == nullptr && need_body && method->relation_member == nullptr)
                {
                    this->__log(ce, c_t::interface_static_member_needs_body, member->to_prototype());
                    return false;
                }
            }

            // Interface properties cannot have defination, but static ones that are not abstract
            // have it.
            if (member_type == m_t::property)
            {
                property_t * property = (property_t *)member;
                bool need_body = member->is_static() && !member->is_abstract();

                if (need_body)
                {
                    for (method_t * accessor : { property->get_method, property->set_method })
                    {
                        if (accessor != nullptr && accessor->body == nullptr)
                        {
                            this->__log(accessor, c_t::interface_static_member_needs_body,
                                                                    member->to_prototype());
                            return false;
                        }
                    }
                }
                else if (property->get_method != nullptr && property->get_method->body != nullptr)
                {
                    this->__log(property->get_method, c_t::interface_members_cannot_have_defination,
                        property->get_method);

                    return false;
                }

                if (!need_body && property->set_method != nullptr
                                        && property->set_method->body != nullptr)
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
                    // A member that is a member of an interface of the type too is a way to
                    // replace a member that is not virtual (ToString of IStringable and the
                    // ToString of Object): it hides it as it is meant to.
                    bool implements = false;
                    if (!member->is_new())
                    {
                        type->each_super_type([&](type_t * super_type) {
                            if (super_type->this_ttype() == ttype_t::interface_
                                    && super_type->check_duplicate(member, true) != nullptr)
                            {
                                implements = true;
                                return false;
                            }

                            return true;
                        });
                    }

                    if (!member->is_new() && !implements)
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
        {
            this->type = __ascertain_type(context, (general_type_name_t *)this);

            // T? of a reference is the type that can be null, it is the type.
            if (__nullable && this->type != nullptr
                    && (can_be_null(this->type) || is_generic_param(this->type)))
            {
                this->nullable = true;
            }
            // T? is Nullable<T>.
            else if (__nullable && this->type != nullptr)
            {
                type_collection_t types = { typex_t(this->type) };
                xtype_collection_t xtypes(types);

                this->type = context.assembly.types.new_generic_type(
                    __XPool.get_nullable_type(), xtypes, nullptr
                );
            }
        }

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
                return __check_core_constraints();

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

        // The constraints of the types of the core are not kept by the assemblies: the argument of Ptr<T>
        // is an unmanaged type.
        bool __check_core_constraints()
        {
            if constexpr (std::is_same<_entity_t, generic_type_t>::value)
            {
                if (__entity->template_ != __XPool.get_tptr_type())
                    return true;

                bool ret = true;
                for (type_t * arg : __entity->args)
                {
                    if (arg != nullptr && !__is_unmanaged(arg))
                    {
                        this->__log(__c_t::constraint_expect_unmanaged_type, arg, _T("T"), __entity);
                        ret = false;
                    }
                }

                return ret;
            }

            return true;
        }

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

                if (bit_has_flag(constraint->cttype, __f_t::unmanaged_) && !__is_unmanaged(type))
                    __LogAndRet(constraint_expect_unmanaged_type, type, name, __entity);
            }

            return true;
        }

        // Returns whether it's an unmanaged type: a generic param is, when it has the constraint.
        bool __is_unmanaged(type_t * type)
        {
            if (is_generic_param(type))
            {
                generic_param_t * gp = (generic_param_t *)type;
                generic_constraint_t * constraint = __search_constraint(gp->name);

                return constraint != nullptr && bit_has_flag(constraint->cttype, __f_t::unmanaged_);
            }

            return is_unmanaged(type);
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

                if (constraint == nullptr)
                    return false;

                if (bit_has_flag(constraint->cttype, __f_t::class_))
                    return true;

                for (type_name_t * type_name : constraint->type_names)
                {
                    type_t * type0 = type_name->type;
                    _A(type0 != nullptr);

                    if (type0 == __XPool.get_enum_type() || type0 == __XPool.get_object_type())
                        continue;

                    if (is_class(type0))
                        return true;
                }

                return false;
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
                // There is no method when it is the constraint of a type: where T : C<T>.
                method_t * method = to_general(__wctx.current_method());
                generic_constraint_t * constraint = method != nullptr?
                            __search_constraint(method->constraints, name) : nullptr;

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

            // The arguments that can be null (Box<String?>), and the constraints notnull.
            if (nullable_directive_seen)
            {
                check_nullable_constraints((general_type_name_t *)this, this,
                                           this->__context.logger);
            }
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
            // A variable, a field of an object (o.f), an element (a[i]).
            bool is_member = this->expression != nullptr
                && this->expression->this_family() == expression_family_t::binary
                && ((binary_expression_t *)this->expression)->op() == operator_t::member_point;

            if (this->expression != nullptr
                    && this->expression->this_family() != expression_family_t::name
                    && this->expression->this_family() != expression_family_t::index
                    && !is_member)
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
                set_value(cvalue_t((const char_t *)data.value, data.string_length != (size_t)-1?
                                data.string_length : al::strlen((const char_t *)data.value)));
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

    // Sets partial.
    void decorate_ast_node_t::set_partial(__el_t * el)
    {
        this->__assign(__BitField(this, is_partial), true, el, _T("partial"), true);
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

            // A lambda that is not converted to a delegate, with no value to return, is void.
            if (method->is_lambda && method->lambda_no_value && method->type_name != nullptr
                    && method->type_name->type == uncertain_type_t::instance())
            {
                method->type_name->type = __XPool.get_internal_type(vtype_t::void_);
                if (method->variable != nullptr)
                    method->variable->reset_type();
            }

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

        expression_t * exp = to_sys_exp(__get_memory(), __get_lang(), nullptr, value);

        // The position of the value in the code.
        if (node->code_unit != nullptr)
            exp->code_unit = node->code_unit;

        __es.push_back(__el_t(exp));

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
                        // It follows the error that was told when the expression was analyzed.
                        if (this->__context.compile_context.error_count() == 0)
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

    // Appends a member of the namespace.
    bool namespace_ast_node_t::append_member(ast_node_t * node)
    {
        switch ((element_value_t)node->value())
        {
            case (element_value_t)__cvalue_t::type:
                this->append_child(__types__, node);
                return true;

            case (element_value_t)__xcvalue_t::namespace_:
                this->append_child(__namespaces__, node);
                return true;

            case (element_value_t)__xcvalue_t::type_def:
                this->append_child(__type_defs__, node);
                return true;

            default:
                return false;
        }
    }

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
        void revise_constructor(method_t & method, arguments_t * args, __el_t * node = nullptr,
                                                               bool call_this = false)
        {
            // : this(...): another constructor of the type does all the construction.
            if (call_this)
            {
                type_t * type = __wctx.current_type();
                method_t * ctor = find_constructor(this->__context, type, args);

                if (ctor == nullptr)
                    this->__log(node, __c_t::constructor_method_not_found, type, args);
                else if (ctor == &method)
                    this->__log(node, __c_t::constructor_calls_itself, type);
                else
                {
                    convert_arguments(args, ctor);

                    __statements_t statements;
                    statements.push_back(__new_expression_statement<function_expression_t>(
                        ctor, args
                    ));

                    if (method.body == nullptr)
                        method.body = __new_obj<method_body_t>(__wctx.current_region());

                    method.body->push_front(statements);
                }

                return;
            }

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
            __init_fields(statements, method, false);

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

        // Revise static constructor: initializes the static fields.
        void revise_static_constructor(method_t & method)
        {
            __statements_t statements;
            __init_fields(statements, method, true);

            if (statements.size() > 0)
            {
                if (method.body == nullptr)
                    method.body = __new_obj<method_body_t>(__wctx.current_region());

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
            else
                convert_arguments(args, ctor);

            return ctor;
        }

        // Initialize fields.
        void __init_fields(__statements_t & statements, method_t & method, bool is_static)
        {
            // fields initialize
            type_t * type = __wctx.current_type();
            _A(type != nullptr);

            analyze_members_args_t members_args(member_type_t::field, name_t::null);
            type->get_members(members_args);

            for (member_t * member : members_args.out_members)
            {
                field_t * field = (field_t *)member;

                // Static fields are initialized by the static constructor, constants
                // have no storage to initialize.
                if (field->is_static() != is_static || field->is_const())
                    continue;

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

            // A value that is not a constant (new Int32[] { 1, 2 }, new Box(), F()) is made by the
            // constructor, as the constants are.
            cvalue_t cv = this->__execute_expression(exp);
            if (is_nan(cv))
                return true;

            cvalue_t def_cv = default_value_of(type->this_vtype());
            return cv != def_cv;
        }
    };

    // Tag of the delayed analysis of a synthesized static constructor.
    class __static_ctor_tag_t : public object_t
    {
    public:
        __static_ctor_tag_t(method_t * method) : method(method) { }

        method_t * method;
    };

    ////////// ////////// ////////// ////////// //////////
    // type

    // Returns whether the node has a yield statement, the lambdas are other methods.
    static bool __has_yield(ast_node_t * node)
    {
        if (node == nullptr)
            return false;

        ast_value_t value = node->value();
        if (value == (ast_value_t)__xcvalue_t::yield_st)
            return true;

        if (value == (ast_value_t)__xcvalue_t::lambda)
            return false;

        for (size_t index = 0, count = node->child_count(); index < count; index++)
        {
            if (__has_yield(node->child_at(index)))
                return true;
        }

        return false;
    }

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


    // The constraints of the generic params are set to them, to find the members of a generic param.
    static void __link_constraints(generic_params_t * params, generic_constraints_t * constraints)
    {
        if (params == nullptr || constraints == nullptr)
            return;

        for (generic_constraint_t * constraint : *constraints)
        {
            for (generic_param_t * param : *params)
            {
                if (param->name == constraint->param_name)
                    param->constraints.push_back(constraint);
            }
        }
    }

    // Commits this node.
    void type_ast_node_t::on_commit()
    {
        this->params      = __to_eobject<generic_params_t *>(__generic_params__);
        this->decorate    = __to_eobject<decorate_t *>(__decorate__);
        this->attributes  = __to_eobject_with_owner<attributes_t *>(__attributes__,
                                                                (general_type_t *)this);
        this->constraints = __to_eobject<generic_constraints_t *>(__generic_constraints__);
        __link_constraints(this->params, this->constraints);

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
                // The parts of a partial type are walked as the first part, with its region.
                type_ast_node_t * first = __merged_first != nullptr? __merged_first : this;

                if (first->__type_region == nullptr)
                {
                    context.push(first->to_eobject());
                    first->__type_region = context.current_region();
                }
                else
                {
                    context.push(first->to_eobject(), first->__type_region);
                }

                bool r = __walk_default(context, step, tag);
                context.pop();
                return r;
            }

            case walk_step_t::analysis:
                return __walk_analysis(context, (method_t *)tag);

            default: return true;
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // partial types

    namespace {

        typedef std::map<string_t, type_ast_node_t *> __partial_firsts_t;

        // Returns whether the type is declared partial.
        bool __is_partial(type_ast_node_t * type)
        {
            return type->decorate != nullptr && type->decorate->is_partial;
        }

        // Returns the access of a type, or default.
        access_value_t __access_of(type_ast_node_t * type)
        {
            return type->decorate == nullptr? access_value_t::__default__ : type->decorate->access;
        }

        // Returns the key of a type: the namespace or the host type, the name and the generic
        // params count.
        string_t __partial_key(const string_t & scope, type_ast_node_t * type)
        {
            size_t count = type->params == nullptr? 0 : type->params->size();
            return _F(_T("%1%/%2%`%3%"), scope, _str(type->name), count);
        }

        // Collects the type nodes: the namespaces make the scope of the types.
        void __collect_types(ast_node_t * node, const string_t & scope,
                                                    std::vector<std::pair<string_t, type_ast_node_t *>> & types)
        {
            if (node == nullptr)
                return;

            if (type_ast_node_t * type = dynamic_cast<type_ast_node_t *>(node))
            {
                types.push_back(std::make_pair(scope, type));
                return;
            }

            string_t child_scope = scope;
            if (namespace_ast_node_t * ns = dynamic_cast<namespace_ast_node_t *>(node))
            {
                if (ns->name != nullptr)
                    child_scope = scope.empty()? _str(ns->name) : scope + _T(".") + _str(ns->name);
            }

            for (size_t k = 0, count = node->child_count(); k < count; k++)
                __collect_types(node->child_at(k), child_scope, types);
        }

        // Appends the items that are not in the list.
        template<typename list_t, typename items_t>
        void __append_new_type_names(list_t & list, items_t & items)
        {
            for (type_name_t * item : items)
            {
                bool found = false;
                for (type_name_t * exist : list)
                    found = found || _str(exist) == _str(item);

                if (!found)
                    list.push_back(item);
            }
        }

        // Merges the part into the first one.
        void __merge_part(type_ast_node_t * first, type_ast_node_t * part, logger_t & logger)
        {
            typedef common_log_code_t __c_t;

            if (first->ttype != part->ttype)
                ast_log(logger, part, __c_t::partial_conflict, _str(first->name), _T("kinds"));

            access_value_t a1 = __access_of(first), a2 = __access_of(part);
            if (a1 != access_value_t::__default__ && a2 != access_value_t::__default__ && a1 != a2)
                ast_log(logger, part, __c_t::partial_conflict, _str(first->name), _T("accessibility"));

            // abstract, sealed and static of a part are those of the type.
            if (part->decorate != nullptr)
            {
                if (a1 == access_value_t::__default__)
                    first->decorate->access = a2;

                first->decorate->is_abstract = first->decorate->is_abstract || part->decorate->is_abstract;
                first->decorate->is_sealed   = first->decorate->is_sealed   || part->decorate->is_sealed;
                first->decorate->is_static   = first->decorate->is_static   || part->decorate->is_static;
            }

            // The generic params have the same names.
            size_t n1 = first->params == nullptr? 0 : first->params->size();
            for (size_t k = 0; k < n1; k++)
            {
                if ((*first->params)[k]->name != (*part->params)[k]->name)
                {
                    ast_log(logger, part, __c_t::partial_conflict, _str(first->name),
                                                                   _T("type parameter names"));
                    break;
                }
            }

            // The constraints of a part are those of the type.
            if (part->constraints != nullptr)
            {
                for (generic_constraint_t * constraint : *part->constraints)
                {
                    for (size_t k = 0; k < n1; k++)
                    {
                        generic_param_t * param = (*first->params)[k];
                        if (param->name == constraint->param_name)
                            param->constraints.push_back(constraint);
                    }

                    if (first->constraints != nullptr)
                        first->constraints->push_back(constraint);
                }
            }

            // The attributes.
            if (part->attributes != nullptr)
            {
                if (first->attributes == nullptr)
                {
                    first->attributes = part->attributes;
                    first->attributes->set_owner((general_type_t *)first);
                }
                else
                {
                    for (attribute_t * attribute : *part->attributes)
                    {
                        attribute->set_owner((general_type_t *)first);
                        first->attributes->push_back(attribute);
                    }
                }
            }

            __append_new_type_names(first->super_type_names, part->super_type_names);

            // The members: the owner of a member is set when it is appended.
            for (method_t * m : part->methods)          first->methods.push_back(m);
            for (property_t * m : part->properties)     first->properties.push_back(m);
            for (field_t * m : part->fields)            first->fields.push_back(m);
            for (event_t * m : part->events)            first->events.push_back(m);
            for (type_t * m : part->nest_types)         first->nest_types.push_back(m);
            for (type_def_t * m : part->type_defs)      first->type_defs.push_back(m);

            part->methods.clear();
            part->properties.clear();
            part->fields.clear();
            part->events.clear();
            part->nest_types.clear();
            part->type_defs.clear();

            part->set_merged_into(first);
            first->set_parts_count(first->parts_count() + 1);
        }

        // The partial methods of a type: a declaration (no body) that has an implementation in
        // another part is dropped, the others without an implementation do nothing.
        void __merge_partial_methods(type_ast_node_t * type)
        {
            std::map<string_t, std::vector<method_ast_node_t *>> groups;

            for (method_t * method : type->methods)
            {
                method_ast_node_t * node = dynamic_cast<method_ast_node_t *>(method);
                if (node == nullptr || node->decorate == nullptr || !node->decorate->is_partial)
                    continue;

                string_t key = _str(node->name);
                if (node->params != nullptr)
                {
                    for (param_t * param : *node->params)
                        key += _T("|") + _str(param->type_name);
                }

                groups[key].push_back(node);
            }

            std::set<method_t *> dropped;

            for (auto & it : groups)
            {
                method_ast_node_t * declaration = nullptr, * implementation = nullptr;

                for (method_ast_node_t * node : it.second)
                {
                    if (node->body == nullptr)
                        declaration = node;
                    else
                        implementation = node;
                }

                if (declaration == nullptr)
                    continue;

                if (implementation != nullptr)
                {
                    declaration->set_partial_skipped();
                    dropped.insert(declaration);
                }
                else
                {
                    declaration->set_partial_empty();
                }
            }

            for (auto & it : groups)
            {
                for (method_ast_node_t * node : it.second)
                    node->decorate->is_partial = false;
            }

            if (dropped.empty())
                return;

            std::vector<method_t *> kept;
            for (method_t * method : type->methods)
            {
                if (dropped.find(method) == dropped.end())
                    kept.push_back(method);
            }

            type->methods.clear();
            for (method_t * method : kept)
                type->methods.push_back(method);
        }

        // Merges the parts of the types that are in the same scope.
        void __merge_scope(std::vector<std::pair<string_t, type_ast_node_t *>> & types,
                                                                        logger_t & logger)
        {
            typedef common_log_code_t __c_t;

            std::map<string_t, std::vector<type_ast_node_t *>> groups;
            std::vector<string_t> order;

            for (auto & it : types)
            {
                string_t key = __partial_key(it.first, it.second);
                if (groups.find(key) == groups.end())
                    order.push_back(key);

                groups[key].push_back(it.second);
            }

            for (const string_t & key : order)
            {
                std::vector<type_ast_node_t *> & parts = groups[key];

                bool any = false, all = true;
                for (type_ast_node_t * part : parts)
                {
                    any = any || __is_partial(part);
                    all = all && __is_partial(part);
                }

                bool merged = false;

                if (!any)
                {
                    // A type that is not partial: the duplicate is logged later.
                }
                else if (!all)
                {
                    for (type_ast_node_t * part : parts)
                    {
                        if (!__is_partial(part))
                            ast_log(logger, part, __c_t::partial_modifier_missing, _str(part->name));
                    }
                }
                else
                {
                    for (size_t k = 1; k < parts.size(); k++)
                        __merge_part(parts[0], parts[k], logger);

                    merged = true;
                }

                if (any && all)
                    __merge_partial_methods(parts[0]);

                // The nest types of the type: the parts of them are merged too.
                std::vector<std::pair<string_t, type_ast_node_t *>> nest_types;

                for (size_t k = 0; k < (merged? 1 : parts.size()); k++)
                {
                    for (type_t * nest_type : parts[k]->nest_types)
                    {
                        if (type_ast_node_t * node = dynamic_cast<type_ast_node_t *>(nest_type))
                            nest_types.push_back(std::make_pair(key, node));
                    }
                }

                if (!nest_types.empty())
                {
                    __merge_scope(nest_types, logger);

                    // The parts that are merged are not types of the host.
                    for (size_t k = 0; k < (merged? 1 : parts.size()); k++)
                    {
                        std::vector<type_t *> kept;
                        for (type_t * nest_type : parts[k]->nest_types)
                        {
                            type_ast_node_t * node = dynamic_cast<type_ast_node_t *>(nest_type);
                            if (node == nullptr || !node->is_merged_part())
                                kept.push_back(nest_type);
                        }

                        if (kept.size() != parts[k]->nest_types.size())
                        {
                            parts[k]->nest_types.clear();
                            for (type_t * nest_type : kept)
                                parts[k]->nest_types.push_back(nest_type);
                        }
                    }
                }
            }

            // The decorate of the partial types is that of the other types.
            for (auto & it : types)
            {
                if (it.second->decorate != nullptr)
                    it.second->decorate->is_partial = false;
            }
        }
    }

    // Merges the parts of the partial types.
    void merge_partial_types(const std::vector<ast_node_t *> & roots, logger_t & logger)
    {
        std::vector<std::pair<string_t, type_ast_node_t *>> types;

        for (ast_node_t * root : roots)
            __collect_types(root, string_t(), types);

        if (types.empty())
            return;

        __merge_scope(types, logger);
    }

    // Walks default step.
    bool type_ast_node_t::__walk_default(ast_walk_context_t & context, int step, void * tag)
    {
        general_type_t * type = (general_type_t *)to_eobject();
        mname_operate_context_t mctx = to_mname_operate_context();

        // A part of a partial type: its members are walked here (the namespace and the usings of
        // this file), but they belong to the first part.
        if (__merged_first != nullptr)
        {
            // The methods of this part that have yield statements: the new members are those
            // of the type.
            __lower_iterators(context);

            for (method_t * method : this->methods)
                __merged_first->methods.push_back(method);

            this->methods.clear();

            bool r = __super_t::on_walk(context, step, tag);

            type_ast_node_t * first = __merged_first;
            if (++first->__parts_walked >= first->__parts_count)
                r = first->__walk_default_end(context) && r;

            return r;
        }

        type->namespace_ = context.current_namespace();

        if (!context.assembly.types.append_type(type))
            this->__log(this, __c_t::type_defination_duplicate, type);

        __XPool.append_new_type(type);

        // Set type for enum fields.
        if (is_enum(type))
        {
            // An enum is its integer: no event, no constructor.
            for (event_t * event : type->events)
                this->__log(this, __c_t::enum_instance_member_error, type->name, event->name);

            for (field_t * field : type->fields)
            {
                // A member that is not an item: a const field.
                if (!field->is_enum_item)
                {
                    if (field->decorate == nullptr || !field->decorate->is_const)
                        this->__log(this, __c_t::enum_instance_member_error, type->name, field->name);
                    else
                        field->decorate->is_static = true;      // All const fields are static.

                    continue;
                }

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

        // The methods that have yield statements.
        __lower_iterators(context);

        // Walk members.
        __super_t::on_walk(context, step, tag);

        // The parts of a partial type end together, after all of them are walked.
        __parts_walked++;
        if (__parts_walked < __parts_count)
            return true;

        return __walk_default_end(context);
    }

    // Ends the default walk of the type: the default constructors, the static constructor.
    bool type_ast_node_t::__walk_default_end(ast_walk_context_t & context)
    {
        general_type_t * type = (general_type_t *)to_eobject();

        // An enum has no constructor.
        if (is_enum(type))
        {
            for (method_t * method : this->methods)
            {
                if (method->trait == method_trait_t::constructor)
                    this->__log(this, __c_t::enum_constructor_error, type->name);
            }
        }

        // Check default constructors.
        method_t * new_default_constructor = nullptr;

        if (al::in(type->this_ttype(), ttype_t::class_, ttype_t::struct_))
        {
            auto it = al::find_if(this->methods, [](method_t * m) {
                return m->trait == method_trait_t::constructor;
            });

            if (it == std::end(this->methods))  // no constructor
            {
                // The tag is read back as an object_t: convert before it becomes a void *.
                method_t * method = __append_default_constructor(context);
                this->__delay(context, walk_step_t::analysis, (object_t *)method);
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

        this->__delay(context, walk_step_t::analysis, (object_t *)new_default_constructor);

        // Static fields with initial values need a static constructor.
        if (al::in(type->this_ttype(), ttype_t::class_, ttype_t::struct_))
        {
            bool has_static_ctor = al::any_of(this->methods, [](method_t * m) {
                return m->trait == method_trait_t::static_constructor;
            });

            bool has_static_init = al::any_of(this->fields, [](field_t * f) {
                return f->is_static() && !f->is_const() && f->init_value != nullptr;
            });

            // The static constructor is called when the program starts, with the code of the
            // types that are not generic: a generic type has a copy of the static fields for each
            // of its instances, and nothing runs the initial values of them.
            if (has_static_init && type->is_generic())
            {
                for (field_t * f : this->fields)
                {
                    if (f->is_static() && !f->is_const() && f->init_value != nullptr)
                    {
                        code_element_t * ce = dynamic_cast<code_element_t *>(f);
                        this->__log(ce != nullptr? ce : (code_element_t *)(ast_node_t *)this,
                            __c_t::static_init_in_generic_type, f->name, type);
                    }
                }

                has_static_init = false;
            }

            if (!has_static_ctor && has_static_init)
            {
                method_t * method = __new_obj<method_t>();

                method->name     = this->name;
                method->decorate = __new_obj<decorate_t>();
                method->decorate->access    = access_value_t::private_;
                method->decorate->is_static = true;
                method->trait    = method_trait_t::static_constructor;
                method->body     = __new_obj<method_body_t>(context.current_region());
                method->body->push_back(__new_obj<return_statement_t>());

                this->methods.push_back(method);
                this->__mark_generated(context, method, method);
                this->__delay(context, walk_step_t::analysis,
                                    (object_t *)__new_obj<__static_ctor_tag_t>(method));
            }
        }

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
                if (!is_static_const(field) || !__accepts(field))
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

        // Whether the stub fills the field: the items of an enum have their own values.
        virtual bool __accepts(field_t * field) = 0;

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

        virtual bool __accepts(field_t * field) override { return field->is_enum_item; }

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

        virtual bool __accepts(field_t * field) override { return !field->is_enum_item; }

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

                    if (is_class(type0) && type0 != __XPool.get_enum_type() &&
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

    namespace
    {
        // Whether two methods have the same signature. The types that depend on generic
        // arguments are not compared (they are equal as far as it is known).
        bool __same_signature(method_t * m1, method_t * m2)
        {
            if (m1->name != m2->name || m1->generic_param_count() != m2->generic_param_count())
                return false;

            size_t count = m1->param_count();
            if (count != m2->param_count())
                return false;

            for (size_t index = 0; index < count; index++)
            {
                param_t * p1 = m1->param_at(index), * p2 = m2->param_at(index);
                type_t * t1 = p1->get_type(), * t2 = p2->get_type();

                if (t1 == nullptr || t2 == nullptr)
                    continue;

                if (t1 == t2)
                {
                    if (p1->ptype != p2->ptype)
                        return false;

                    continue;
                }

                if (!is_general(t1) || !is_general(t2))
                    continue;

                return false;
            }

            return true;
        }

        // Whether a method of the type or its base types implements the method.
        // The owner of an explicit implementation is its interface.
        bool __is_implemented(type_t * type, method_t * method, type_t * interface_type)
        {
            // A static abstract method needs a static one.
            bool want_static = method->is_static();

            for (type_t * t = type; t != nullptr; t = t->get_base_type())
            {
                auto * like = dynamic_cast<__general_type_like_base_t *>(t);
                if (like == nullptr)
                    return true;    // Unknown: not judged.

                for (method_t * m : like->methods)
                {
                    if (m == method || m->is_static() != want_static)
                        continue;

                    // The static method that implements is not abstract.
                    if (want_static && m->is_abstract())
                        continue;

                    type_t * owner = m->get_owner_type();
                    if (owner != nullptr && owner != interface_type)
                        continue;

                    if (__same_signature(m, method))
                        return true;
                }
            }

            return false;
        }
    }

    // Checks that the abstract members and the interface members are implemented.
    void type_ast_node_t::__check_members_implemented()
    {
        general_type_t * type = (general_type_t *)this;

        if (!al::in(type->this_ttype(), ttype_t::class_, ttype_t::struct_))
            return;

        // The abstract members of the base types: an abstract type needs not implement them.
        for (type_t * base = this->is_abstract()? nullptr : type->get_base_type();
                                            base != nullptr; base = base->get_base_type())
        {
            auto * like = dynamic_cast<__general_type_like_base_t *>(base);
            if (like == nullptr)
                break;

            for (method_t * method : like->methods)
            {
                if (!method->is_abstract())
                    continue;

                // It is overridden in the type, or in a type between.
                bool found = false;
                for (type_t * t = type; t != base && t != nullptr && !found; t = t->get_base_type())
                {
                    auto * like1 = dynamic_cast<__general_type_like_base_t *>(t);
                    if (like1 == nullptr)
                        break;

                    for (method_t * m : like1->methods)
                    {
                        if (m->is_override() && __same_signature(m, method))
                        {
                            found = true;
                            break;
                        }
                    }
                }

                if (!found)
                {
                    this->__log(__name_el, __c_t::abstract_member_not_implemented,
                        type->to_full_name(), method->to_prototype());

                    return;
                }
            }
        }

        // The interface members: the interfaces of the type, and of its base types. An abstract
        // type declares them abstract if it does not implement them.
        std::set<type_t *> walked;
        bool failed = false;

        std::function<bool (type_t *)> check_interface = [&](type_t * interface_type) -> bool {

            if (interface_type == nullptr || interface_type->this_ttype() != ttype_t::interface_
                    || !walked.insert(interface_type).second)
                return true;

            if (auto * like = dynamic_cast<__general_type_like_base_t *>(interface_type))
            {
                for (method_t * method : like->methods)
                {
                    // A static method of an interface that is not abstract (static, static virtual)
                    // needs not to be implemented.
                    if (method->is_static() && !method->is_abstract())
                        continue;

                    if (!__is_implemented(type, method, interface_type))
                    {
                        this->__log(__name_el, __c_t::interface_member_not_implemented,
                            type->to_full_name(), method->to_prototype());

                        failed = true;
                        return false;
                    }
                }
            }

            interface_type->each_super_type(check_interface);
            return !failed;
        };

        for (type_t * t = type; t != nullptr && !failed; t = t->get_base_type())
            t->each_super_type(check_interface);
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

            // Check the abstract members and the interface members are implemented.
            if (circle_type == nullptr)
                __check_members_implemented();

            // Check constraints.
            if (this->constraints != nullptr && this->constraints->size() > 0)
                __check_generic_template_constraints(this->__context, this, this);
        }

        object_t * tag_obj = (object_t *)tag;
        method_t * new_default_constructor = nullptr;
        __fields_init_stub_t * fields_init_stub = nullptr;

        __static_ctor_tag_t * static_ctor_tag = nullptr;

        dynamic_cast_tie(tag_obj, &new_default_constructor, &fields_init_stub,
                                                            &static_ctor_tag);

        if (static_ctor_tag != nullptr)
        {
            __method_utils_t(__context, context).revise_static_constructor(
                                                            *static_ctor_tag->method);
            return true;
        }

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

            // The static const fields that are not items.
            typedef __general_fields_init_stub_t member_stub_t;
            member_stub_t member_stub(this, type);

            if (!member_stub.continue_())
                this->__delay(context, walk_step_t::analysis, __new_obj<member_stub_t>(member_stub));
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

    // Returns whether a string ends with another.
    static bool __ends_with(const string_t & s, const char_t * end)
    {
        size_t length = std::char_traits<char_t>::length(end);
        return s.length() >= length && s.compare(s.length() - length, length, end) == 0;
    }

    // The owner of what the compiler makes: the type, the method, and the types of its params.
    static string_t __generated_owner(const string_t & type_name, method_t * method)
    {
        string_t text = type_name + _T(".") + _str(method->name) + _T("(");

        if (method->params != nullptr)
        {
            bool first = true;
            for (param_t * param : *method->params)
            {
                text += (first? _T("") : _T(", "));
                text += param->type_name != nullptr? string_t(_str(param->type_name)) : string_t();
                first = false;
            }
        }

        return text + _T(")");
    }

    // The name of what the compiler makes: __ap_<usage>_<detail>_<hash> (the detail can be empty).
    // The detail is the serial among those of the same usage that the method has, or the name of
    // what the member is made for. The hash is made of the usage, the owner and the detail: it is
    // the same every time the code is compiled.
    static string_t __generated_name(const char_t * usage, const string_t & detail,
                                     const string_t & owner)
    {
        return to_generated_name(usage, detail, owner);
    }

    // The name of a type or a method that has a serial.
    static string_t __generated_name(const char_t * usage, int serial, const string_t & owner)
    {
        return __generated_name(usage, _F(_T("%1%"), serial), owner);
    }

    // Replaces the marks of a text with the names that are made.
    static void __replace_marks(string_t & text, const char_t * mark, const string_t & name)
    {
        auto length = string_t(mark).size();

        for (auto pos = text.find(mark); pos != string_t::npos; pos = text.find(mark, pos))
        {
            text.replace(pos, length, name);
            pos += name.size();
        }
    }

    // Returns whether the attributes have CompilerGenerated.
    static bool __has_generated_attribute(attributes_t * attributes)
    {
        if (attributes == nullptr)
            return false;

        for (attribute_t * attribute : *attributes)
        {
            if (attribute->type_name != nullptr
                    && _str(attribute->type_name).find(_T("CompilerGenerated")) != string_t::npos)
                return true;
        }

        return false;
    }

    // Makes the iterators of the methods that have yield statements.
    void type_ast_node_t::__lower_iterators(ast_walk_context_t & context)
    {
        static int iterator_count = 0;

        // The methods are found first: the iterators change the members.
        std::vector<method_ast_node_t *> methods;
        this->each_child_node<method_ast_node_t>(__methods__, [&](method_ast_node_t * method) {
            if (method != nullptr
                    && __has_yield(method->child_at(method_ast_node_t::__body__)))
            {
                methods.push_back(method);
            }
        });

        for (method_ast_node_t * method : methods)
        {
            __lower_iterator(context, method, ++iterator_count);
        }
    }

    // The type, with the generic params of a list changed to those that are at the same place of
    // another list (T of a method to T of the class of the state object of an iterator).
    static type_t * __remap_generic_params(type_t * type, generic_params_t * from,
                                                         generic_params_t * to)
    {
        if (type == nullptr || from == nullptr || to == nullptr)
            return type;

        if (is_generic_param(type))
        {
            for (size_t k = 0, count = std::min(from->size(), to->size()); k < count; k++)
            {
                if ((*from)[k] == (generic_param_t *)type)
                    return (*to)[k];
            }

            return type;
        }

        if (is_generic(type))
        {
            generic_type_t * generic_type = (generic_type_t *)type;

            type_collection_t args;
            for (typex_t arg : generic_type->args)
                args.push_back(__remap_generic_params((type_t *)arg, from, to));

            return __XPool.new_generic_type(generic_type->template_, args, generic_type->host_type);
        }

        if (type->this_gtype() == gtype_t::array)
        {
            array_type_t * array_type = (array_type_t *)type;
            type_t * element = __remap_generic_params(array_type->element_type, from, to);

            return element == array_type->element_type? type
                        : __XPool.new_array_type(element, array_type->dimension);
        }

        return type;
    }

    // Makes the iterator of a method. The method that has yield statements makes an object of
    // a class that is made here, the state, that has the params of the method, and is an
    // IEnumerator<T> and an IEnumerable<T> that calls a delegate to go to the next item. The
    // body of the method goes to a new method that is that delegate: its first param is the state
    // object, its local variables are fields of it, and the yield statements are the places
    // where it goes on (see method_ast_node_t::__walk_iterator_end).
    void type_ast_node_t::__lower_iterator(ast_walk_context_t & context,
                                           method_ast_node_t * method, int id)
    {
        #define __Fail(_reason)                                                         \
            do {                                                                        \
                this->__log((ast_node_t *)method, __c_t::iterator_not_supported,        \
                                                    _str(method->name), _T(_reason));   \
                return;                                                                 \
            } while (false)

        // A generic method: the state object is a generic class that has the generic params of
        // the method, the method that does the work is a generic method of the same class (it
        // is called by the state object, a delegate cannot take the generic params).
        bool generic_method = method->generic_params != nullptr && method->generic_params->size() > 0;
        string_t generic_text;

        // The constraints of the params: where T : class, new(), IFoo. The state class and the method
        // that does the work have the same ones.
        string_t constraints_text;

        if (generic_method)
        {
            if (method->constraints != nullptr)
            {
                for (generic_constraint_t * constraint : *method->constraints)
                {
                    string_t items;
                    auto add = [&](const string_t & item) {
                        items += (items.empty()? _T("") : _T(", ")) + item;
                    };

                    if (bit_has_flag(constraint->cttype, generic_constraint_ttype_t::class_))
                        add(_T("class"));

                    if (bit_has_flag(constraint->cttype, generic_constraint_ttype_t::struct_))
                        add(_T("struct"));

                    for (type_name_t * type_name : constraint->type_names)
                        add(_str(type_name));

                    if (bit_has_flag(constraint->cttype, generic_constraint_ttype_t::new_))
                        add(_T("new()"));

                    if (!items.empty())
                    {
                        constraints_text += _T(" where ") + string_t(_str(constraint->param_name))
                                                                                + _T(" : ") + items;
                    }
                }
            }

            for (generic_param_t * gp : *method->generic_params)
            {
                // The items of a params generic param (T ...) are the last ones of the params.
                bool is_pack = gp->param_type == generic_param_type_t::params;
                if (gp->param_type != generic_param_type_t::__default__ && !is_pack)
                    __Fail("a generic method that has an out generic param");

                generic_text += generic_text.empty()? _T("<") : _T(", ");
                generic_text += _str(gp->name);

                if (is_pack)
                    generic_text += _T(" ...");
            }

            generic_text += _T(">");
        }

        // IEnumerable<T> or IEnumerator<T>.
        string_t return_type = method->type_name != nullptr?
                                        string_t(_str(method->type_name)) : string_t();

        size_t lt = return_type.find(_T('<')), gt = return_type.rfind(_T('>'));
        if (lt == string_t::npos || gt == string_t::npos || gt < lt)
        {
            this->__log((ast_node_t *)method, __c_t::iterator_return_type_invalid,
                                                            (method_t *)method);
            return;
        }

        string_t head = return_type.substr(0, lt);
        string_t element = return_type.substr(lt + 1, gt - lt - 1);

        if (!__ends_with(head, _T("IEnumerable")) && !__ends_with(head, _T("IEnumerator")))
        {
            this->__log((ast_node_t *)method, __c_t::iterator_return_type_invalid,
                                                            (method_t *)method);
            return;
        }

        // The params.
        string_t params_text, ctor_params, ctor_assigns, fields_text, args_text;

        // The items of a params generic param (T ... items): the state object keeps them as a tuple,
        // the method that does the work takes them again.
        string_t pack_param_text, pack_tuple_text;

        if (method->params != nullptr)
        {
            for (param_t * param : *method->params)
            {
                // A params array is an array like any other: the state object keeps it.
                if (param->ptype == param_type_t::ref || param->ptype == param_type_t::out)
                    __Fail("a ref or out param");

                string_t type_text = _str(param->type_name);
                string_t name_text = _str(param->name);

                if (param->ptype == param_type_t::extends)
                {
                    if (!pack_param_text.empty())
                        __Fail("more than one params generic param");

                    pack_tuple_text = _T("ITuple<") + type_text + _T(" ...>");
                    pack_param_text = type_text + _T(" ... ") + name_text;

                    if (!params_text.empty())
                    {
                        params_text += _T(", ");
                        args_text += _T(", ");
                    }

                    params_text += pack_param_text;
                    args_text += _T("ITuple.Create(") + name_text + _T(" ...)");

                    ctor_params += _T(", ") + pack_tuple_text + _T(" p_") + name_text;
                    ctor_assigns += _T(" this.@PACK@ = p_") + name_text + _T(";");
                    fields_text += string_t(_T(" public ")) + pack_tuple_text + _T(" @PACK@;");

                    continue;
                }

                if (!params_text.empty())
                {
                    params_text += _T(", ");
                    args_text += _T(", ");
                }

                params_text += type_text + _T(" ") + name_text;
                args_text += name_text;

                ctor_params += _T(", ") + type_text + _T(" p_") + name_text;
                ctor_assigns += _T(" this.") + name_text + _T(" = p_") + name_text + _T(";");
                fields_text += string_t(_T(" public "))
                                + type_text + _T(" ") + name_text + _T(";");
            }
        }

        string_t name = _str(method->name);
        string_t owner_text = __generated_owner(this->to_full_name(), (method_t *)method);
        string_t state_name = __generated_name(_T("iterator"), 0, owner_text);
        string_t move_name = __generated_name(_T("move"), 0, owner_text);
        bool is_static = method->is_static();
        string_t G;

        string_t state_head = G + _T("public class ") + state_name + generic_text
            + _T(" : IEnumerator<") + element + _T(">, IEnumerable<") + element + _T(">")
            + constraints_text + _T(" {");

        string_t state_tail =
            _T(" ") + G + _T("public ") + element + _T(" Current { get { return @CURRENT@; } }");

        string_t state_end =
            _T(" ") + G + _T("public Void Reset() { throw new InvalidOperationException(")
            + _T("\"The iterator cannot be reset.\"); }")
            + _T(" ") + G + _T("public Void Dispose() { }")
            + _T(" ") + G + _T("public Void @FINISH@() { @STATE@ = -1; }")
            + _T(" ") + G + _T("public IEnumerator<") + element
            + _T("> GetEnumerator() { return this; }")
            + _T(" }");

        string_t state_text, methods_text;

        if (generic_method)
        {
            // The method that does the work is a method of the object that has the method of the
            // user, when it is not static: the state object has it, to call it.
            string_t host_text = _str(this->name);
            if (this->params != nullptr && this->params->size() > 0)
            {
                host_text += _T("<");
                for (size_t k = 0; k < this->params->size(); k++)
                    host_text += (k > 0? _T(", ") : _T("")) + string_t(_str((*this->params)[k]->name));

                host_text += _T(">");
            }

            string_t owner_field = is_static? string_t() : _T(" ") + G + _T("public ") + host_text
                                                            + _T(" @HOST@;");
            string_t owner_param = is_static? string_t() : host_text + _T(" host");
            string_t owner_assign = is_static? string_t() : _T(" this.@HOST@ = host;");
            string_t owner_call = is_static? host_text : string_t(_T("@HOST@"));

            // The state class, it calls the method that does the work.
            state_text = state_head
                + _T(" ") + G + _T("public Int32 @STATE@; ") + G + _T("public ") + element
                + _T(" @CURRENT@;") + owner_field + fields_text
                + _T(" ") + G + _T("public ") + state_name + _T("(") + owner_param
                + (owner_param.empty() || ctor_params.empty()? string_t() : string_t(_T(", ")))
                + (ctor_params.empty()? string_t() : ctor_params.substr(2))
                + _T(") {") + owner_assign + ctor_assigns + _T(" }")
                + state_tail
                + _T(" ") + G + _T("public Boolean MoveNext() { return ") + owner_call
                + _T(".") + move_name + generic_text + _T("(this")
                + (pack_param_text.empty()? string_t() : string_t(_T(", this.@PACK@ ...")))
                + _T("); }")
                + state_end;

            // The method of the user, that makes the state object, and the method that does the
            // work.
            methods_text =
                string_t(_T("public class __Snippet { public ")) + return_type + _T(" ") + name
                + generic_text + _T("(") + params_text + _T(")") + constraints_text + _T(" {")
                + _T(" return new ") + state_name + generic_text + _T("(")
                + (is_static? string_t() : string_t(_T("this")))
                + (is_static || args_text.empty()? string_t() : string_t(_T(", "))) + args_text
                + _T("); }")
                + _T(" ") + G + _T("public ") + string_t(is_static? _T("static ") : _T(""))
                + _T("Boolean ") + move_name + generic_text
                + _T("(Object @SOURCE@")
                + (pack_param_text.empty()? string_t() : _T(", ") + pack_param_text)
                + _T(")") + constraints_text + _T(" {")
                + _T(" ((") + state_name + generic_text + _T(")@SOURCE@).@FINISH@();")
                + _T(" return false; } }");
        }
        else
        {
            // The state class.
            state_text = state_head
                + _T(" ") + G + _T("public Int32 @STATE@; ") + G + _T("public ") + element
                + _T(" @CURRENT@;")
                + _T(" ") + G + _T("public Delegate<Boolean, Object> @DELEGATE@;") + fields_text
                + _T(" ") + G + _T("public ") + state_name
                + _T("(Delegate<Boolean, Object> move") + ctor_params
                + _T(") { @DELEGATE@ = move;") + ctor_assigns + _T(" }")
                + state_tail
                + _T(" ") + G + _T("public Boolean MoveNext() { return @DELEGATE@(this); }")
                + state_end;

            // The method of the user, that makes the state object, and the method that does the
            // work.
            methods_text =
                string_t(_T("public class __Snippet { public ")) + return_type + _T(" ") + name + _T("(") + params_text + _T(") {")
                + _T(" Delegate<Boolean, Object> @DELEGATE@ = ") + move_name + _T(";")
                + _T(" return new ") + state_name + _T("(@DELEGATE@")
                + (args_text.empty()? string_t() : _T(", ") + args_text) + _T("); }")
                + _T(" ") + G + _T("private ") + string_t(is_static? _T("static ") : _T(""))
                + _T("Boolean ") + move_name + _T("(Object @SOURCE@) {")
                + _T(" ((") + state_name + _T(")@SOURCE@).@FINISH@(); return false; } }");
        }

        string_t state_field = __generated_name(_T("state"), string_t(), state_name);
        string_t current_field = __generated_name(_T("current"), string_t(), state_name);
        string_t delegate_field = __generated_name(_T("delegate"), string_t(), state_name);
        string_t finish_name = __generated_name(_T("finish"), string_t(), state_name);
        string_t source_name = __generated_name(_T("source"), string_t(), state_name);
        string_t host_field = __generated_name(_T("host"), string_t(), state_name);
        string_t pack_field = __generated_name(_T("pack"), string_t(), state_name);

        for (string_t * text : { &state_text, &methods_text })
        {
            __replace_marks(*text, _T("@STATE@"), state_field);
            __replace_marks(*text, _T("@CURRENT@"), current_field);
            __replace_marks(*text, _T("@DELEGATE@"), delegate_field);
            __replace_marks(*text, _T("@FINISH@"), finish_name);
            __replace_marks(*text, _T("@SOURCE@"), source_name);
            __replace_marks(*text, _T("@HOST@"), host_field);
            __replace_marks(*text, _T("@PACK@"), pack_field);
        }

        ast_node_t * state_root = __parse_snippet(state_text);
        ast_node_t * methods_root = __parse_snippet(methods_text);

        if (state_root == nullptr || methods_root == nullptr
                    || state_root->child_count() == 0 || methods_root->child_count() == 0)
        {
            __Fail("its code cannot be made");
        }

        type_ast_node_t * state_node = (type_ast_node_t *)state_root->child_at(0);
        type_ast_node_t * snippet = (type_ast_node_t *)methods_root->child_at(0);

        ast_node_t * methods_node = snippet->child_at(__methods__);
        if (state_node == nullptr || methods_node == nullptr || methods_node->child_count() < 2)
            __Fail("its code cannot be made");

        method_ast_node_t * new_method = (method_ast_node_t *)methods_node->child_at(0);
        method_ast_node_t * move_method = (method_ast_node_t *)methods_node->child_at(1);

        // The body of the method of the user goes to the method that does the work, and
        // the method of the user has the body that makes the state object.
        ast_node_t * old_body = method->child_at(method_ast_node_t::__body__);
        method_body_t * old_body_object = method->body;

        method->set_child(method_ast_node_t::__body__,
                                    new_method->child_at(method_ast_node_t::__body__));
        method->body = new_method->body;

        // The end of the method that does the work: the iterator is finished.
        method_body_ast_node_t * tail_node =
                    (method_body_ast_node_t *)move_method->child_at(method_ast_node_t::__body__);
        method_body_ast_node_t * old_body_node = (method_body_ast_node_t *)old_body;

        tail_node->each_child_node<ast_node_t>(method_body_ast_node_t::__statements__,
            [&](ast_node_t * statement) {
                old_body_node->append_child(method_body_ast_node_t::__statements__, statement);
            }
        );

        for (statement_t * statement : move_method->body->statements)
        {
            old_body_object->statements.push_back(statement);
        }

        move_method->set_child(method_ast_node_t::__body__, old_body);
        move_method->body = old_body_object;

        iterator_info_t * info = __new_obj<iterator_info_t>();
        info->params = method->params;
        info->user_method = (method_t *)method;
        info->state_type = state_node;
        move_method->iterator_info = info;

        this->append_child(__methods__, move_method);
        this->methods.push_back((method_t *)move_method);

        this->__mark_generated_type(context, (general_type_t *)state_node);
        this->__mark_generated(context, (method_t *)move_method, (method_t *)move_method);

        // The state class is a nest type of this type: it is walked with the members of the type.
        // (A part of a partial type has its own nodes: the type is the first part.)
        this->append_child(__nest_types__, state_node);
        (__merged_first != nullptr? __merged_first : this)->nest_types.push_back((type_t *)state_node);

        #undef __Fail
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
        this->__mark_generated(context, method, method);

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

        // notnull is not a type: the argument cannot be null. unmanaged is not either: the argument
        // has no references.
        name_t notnull = __to_name(_T("notnull"));
        name_t unmanaged = __to_name(_T("unmanaged"));

        std::vector<type_name_t *> kept;

        for (type_name_t * item : this->type_names)
        {
            general_type_name_t * type_name = dynamic_cast<general_type_name_t *>(item);

            if (type_name != nullptr && type_name->units.size() == 1
                    && type_name->units[0] != nullptr && !type_name->units[0]->is_generic()
                    && (type_name->units[0]->name == notnull || type_name->units[0]->name == unmanaged))
            {
                // The name is not resolved as a type.
                type_name->type = uncertain_type_t::instance();

                this->cttype = (generic_constraint_ttype_t)(
                    (int)this->cttype | (int)(type_name->units[0]->name == notnull?
                        generic_constraint_ttype_t::notnull_ : generic_constraint_ttype_t::unmanaged_)
                );

                continue;
            }

            kept.push_back(item);
        }

        if (kept.size() != this->type_names.size())
        {
            this->type_names.clear();
            for (type_name_t * item : kept)
                this->type_names.push_back(item);
        }
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

    // Sets this: the first param of an extension method.
    void param_ast_node_t::set_this(__el_t * el)
    {
        this->__assign(this->is_this, true, el, _T("this"));
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

        // The named arguments follow the others: they assign the fields and the properties of the
        // attribute.
        this->each_child<argument_t *>(__assigns__, [this](argument_t * argument) {
            if (this->arguments == nullptr)
                this->arguments = __new_obj<arguments_t>();

            this->arguments->push_back(argument);
        });
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

        // The constructor takes the arguments that are not named.
        arguments_t * positional = nullptr;
        if (this->arguments != nullptr)
        {
            positional = __new_obj<arguments_t>();

            for (argument_t * argument : *this->arguments)
            {
                if (argument->name_type != argument_name_type_t::field)
                    positional->push_back(argument);
            }
        }

        method_t * method = find_constructor(this->__context, type, positional);
        if (method == nullptr)
        {
            __log(this, __c_t::attribute_constructor_not_match, type,
                to_arg_types_str(this->__context, positional)
            );

            return true;
        }

        // The named arguments are fields or properties of the attribute.
        if (this->arguments != nullptr)
        {
            for (argument_t * argument : *this->arguments)
            {
                if (argument->name_type != argument_name_type_t::field)
                    continue;

                analyze_member_args_t field_args(member_type_t::field, argument->name);
                analyze_member_args_t property_args(member_type_t::property, argument->name);

                if (type->get_member(field_args, true) == nullptr
                        && type->get_member(property_args, true) == nullptr)
                {
                    __log(this, __c_t::attribute_named_argument_not_found, type, argument->name);
                    return true;
                }
            }
        }

        if (this->arguments != nullptr)
        {
            for (argument_t * argument : *this->arguments)
            {
                expression_t * exp = argument->expression;

                // typeof(A) is a type, it is not executed (the right side of a named argument too).
                expression_t * value_exp = exp;
                if (binary_expression_t * assign = as<binary_expression_t *>(exp))
                {
                    if (assign->op() == operator_t::assign)
                        value_exp = assign->exp2();
                }

                if (as<type_of_expression_t *>(value_exp) != nullptr)
                    continue;

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
                                                        _T("variable definition"));

        variable_defination_t vd(this->__context, context, (defination_statement_t *)this);

        for (defination_statement_item_t * var_item : this->items)
        {
            type_name_t * type_name = __to_eobject<type_name_t *>(__type_name__);

            var_item->variable = vd.define_local(
                type_name, var_item->name, this->constant,
                var_item->expression, var_item
            );

            // The variable may be a field of the state object of an iterator.
            method_t * current_method = context.current_method();
            if (current_method != nullptr && current_method->iterator_info != nullptr)
                ((iterator_info_t *)current_method->iterator_info)->defs.push_back(var_item);

            if (var_item->expression != nullptr && !__is_uncertain(type_name))
            {
                var_item->expression->parent = __new_obj<system_expression_t<type_cast_expression_t>>(
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

            // The variable that is defined twice has been reported.
            if (var_item->variable == nullptr)
                continue;

            if (__is_uncertain(var_item->variable->type_name))
            {
                type_t * type = nullptr;
                if (var_item->expression != nullptr)
                {
                    // The type of a lambda is that of the delegate that has the type of its body.
                    analyze_lambda_now(context, var_item->expression);

                    type = var_item->expression->get_type();
                }

                if (type == nullptr)
                    __Log(cannot_determine_local_variable_type, var_item->name);
                else
                    var_item->variable->type_name->type = type;

                // The type of var can be null, as its value.
                var_item->variable->type_name->nullable = true;

                /*
                _PF(_T("%1% %2% (%3%)"), var_item->variable->type_name->type, var_item->name,
                                                                    var_item->expression);
                */
            }

            if (var_item->expression != nullptr)
            {
                type_t * type1 = var_item->variable->type_name->type;
                bind_lambda_to_type(var_item->expression, type1);

                // An implicit operator of the types converts the initial value.
                var_item->expression = insert_user_conversion(var_item->expression, type1);
                type_t * type2 = var_item->expression->get_type();

                if (!is_type_compatible(type2, type1)
                        && !is_constant_compatible(var_item->expression, type1))
                {
                    __L(invalid_type_cast, type2->to_prototype(), type1->to_prototype());
                }
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
        // Jumping to a label is not part of the language: structured statements
        // (loops, break, continue, return, try) do the job.
        this->__log(this, __c_t::goto_not_supported);

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
                    this->__delay(context, walk_step_t::analysis);
                    this->__delay(context, walk_step_t::post_analysis);
                    return true;
                }

                return false;

            case walk_step_t::analysis: {
                // return (int x) => x; : the lambda is converted to the return type.
                method_t * method = context.current_method();
                if (method != nullptr && this->expression != nullptr && method->type_name != nullptr)
                    bind_lambda_to_type(this->expression, method->type_name->type);

                // void F() => G(); : the value is not returned.
                if (this->is_arrow && method != nullptr && (method->type_name == nullptr
                        || method->type_name->type == nullptr
                        || is_void_type(method->type_name->type)))
                    this->discard_value = true;

                return true;
            }

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
            if (this->expression != nullptr && !this->discard_value)
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

                // The type of a lambda that is not converted to a delegate: the type that all the
                // values that it returns are converted to (null and String: String, int and long:
                // long). The type of null alone is object, when the body is walked.
                if (method->is_lambda && return_type != nullptr
                        && return_type != uncertain_type_t::instance()
                        && (method_type == uncertain_type_t::instance()
                                || method->lambda_return_inferred))
                {
                    type_t * common = method_type == uncertain_type_t::instance()?
                                return_type : get_compatible_type(method_type, return_type);

                    if (common == nullptr)
                    {
                        this->__log(this, __c_t::method_incompatible_return_value, method);
                        return true;
                    }

                    method->lambda_return_inferred = true;

                    if (common != method_type)
                    {
                        method->type_name->type = method_type = common;
                        if (method->variable != nullptr)
                            method->variable->reset_type();

                        // The type that is made (T? of null and a number).
                        context.commit_types();
                    }
                }

                // An implicit operator of the types converts the value that is returned.
                if (return_type != nullptr && !is_type_compatible(return_type, method_type))
                {
                    this->expression = insert_user_conversion(this->expression, method_type);
                    return_type = this->expression->get_type();
                }

                if (return_type != nullptr && !is_type_compatible(return_type, method_type))
                    this->__log(this, __c_t::method_incompatible_return_value, method);
            }
        }

        return true;
    }

    ////////// ////////// ////////// ////////// //////////
    // yield_st

    // Commits this node.
    void yield_st_ast_node_t::on_commit()
    {
        this->expression = __to_eobject<expression_t *>(__expression__);
    }

    // Returns this eobject.
    statement_t * yield_st_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node: it is a statement of the method that does the work of an iterator, the
    // method lists it, to make it when the types are known.
    bool yield_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if ((walk_step_t)step != walk_step_t::default_)
            return true;

        if (!this->is_break)
            this->__check_empty(this->expression, this, __c_t::expression_missing, "yield");

        method_t * method = context.current_method();
        if (method == nullptr || method->iterator_info == nullptr)
            this->__log(this, __c_t::yield_outside_iterator);
        else
            ((iterator_info_t *)method->iterator_info)->yields.push_back((yield_statement_t *)this);

        return __super_t::on_walk(context, step, tag);
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

    // Parses a source text, returns the root node.
    ast_node_t * __utils_t::__parse_snippet(const string_t & text)
    {
        global_context_t & global_context = __global_context();
        lang_id_t lang = global_context.lang_id_of(_T("cs"));

        code_t * code = __context.compile_context.new_obj<code_t>(
            global_context, string_t(text), lang, (const code_file_t *)nullptr
        );

        return code->build_ast(__context.compile_context);
    }

    // Gives the attribute CompilerGenerated to a member that the compiler makes.
    void __utils_t::__mark_generated(ast_walk_context_t & context,
                                     with_attributes_object_t * target, eobject_t * owner)
    {
        general_type_t * type = __get_xpool().get_compiler_generated_attribute_type();

        // The assembly System is compiled: its types are not known by the pool yet.
        if (type == nullptr)
        {
            context.assembly.types.each_general_types([&](general_type_t * type0) {
                if (type == nullptr && type0->name == __to_name(_T("CompilerGeneratedAttribute"))
                        && type0->namespace_ != nullptr && type0->namespace_->full_name != nullptr
                        && (string_t)*type0->namespace_->full_name == _T("Runtime.CompilerServices"))
                {
                    type = type0;
                }
            });
        }

        if (type == nullptr || target->has_attribute(type))
            return;

        method_t * constructor = find_default_constructor(type);
        if (constructor == nullptr)
            return;

        attribute_t * attribute = __new_obj<attribute_t>();
        attribute->type_name    = __new_obj<type_name_t>((type_t *)type);
        attribute->constructor  = constructor;

        if (owner != nullptr)
            attribute->set_owner(owner);

        if (target->attributes == nullptr)
            target->attributes = __new_obj<attributes_t>();

        target->attributes->push_back(attribute);
    }

    // Gives the attribute CompilerGenerated to a type that the compiler makes, and to its members.
    void __utils_t::__mark_generated_type(ast_walk_context_t & context, general_type_t * type)
    {
        this->__mark_generated(context, type, type);

        for (field_t * field : type->fields)
            this->__mark_generated(context, field, field);

        for (method_t * method : type->methods)
            this->__mark_generated(context, method, method);

        for (property_t * property : type->properties)
            this->__mark_generated(context, property, property);

        for (event_t * event : type->events)
            this->__mark_generated(context, event, event);
    }

    // Makes: try { body } finally { if (variable != null) variable.Dispose(); }
    // Returns null if the type has no Dispose().
    statement_t * __utils_t::__make_dispose_try(ast_walk_context_t & context,
                    local_variable_t * variable, type_t * type, statement_t * body)
    {
        typedef system_expression_t<binary_expression_t> __binary_t;

        atypes_t no_atypes;
        analyze_member_args_t dispose_args(
            member_type_t::method, __to_name(_T("Dispose")), &no_atypes
        );

        if (type == nullptr || type->get_member(dispose_args, true) == nullptr)
            return nullptr;

        auto name_of = [&]() -> expression_t * {
            return __new_obj<system_expression_t<name_expression_t>>(variable->get_name());
        };

        auto * function = __new_obj<system_expression_t<function_expression_t>>();
        function->set_namex(__new_obj<system_expression_t<name_expression_t>>(
            __to_name(_T("Dispose"))
        ));

        expression_t * call = __new_obj<__binary_t>(
            operator_t::member_point, name_of(), function
        );

        walk_expression(this->__context, context, call);
        statement_t * dispose = __new_obj<expression_statement_t>(call);

        ttype_t ttype = type->this_ttype();
        if (ttype == ttype_t::class_ || ttype == ttype_t::interface_)
        {
            expression_t * condition = __new_obj<__binary_t>(
                operator_t::not_equal, name_of(),
                __new_obj<system_expression_t<cvalue_expression_t>>(
                    __new_obj<cvalue_t>(nullptr)
                )
            );

            walk_expression(this->__context, context, condition);
            dispose = __new_obj<if_statement_t>(condition, dispose, nullptr);
        }

        return __new_obj<try_statement_t>(body, dispose);
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
        if ((walk_step_t)step == walk_step_t::analysis)
            return __walk_analysis(context);

        this->__check_name_empty(this->variable, _T("variable"));
        this->__check_empty(this->iterator, this, __c_t::iterator_missing, "for each");
        this->__check_empty(this->body, this, __c_t::body_missing, _T("for each"));

        // The variables are those of the statement: the element, and the array and the
        // index it goes through.
        static int for_each_count = 0;
        int number = ++for_each_count;

        context.push_new_region();
        __region = context.current_region();

        variable_defination_t vd(this->__context, context, (object_t *)(ast_node_t *)this);

        __variable = vd.define_local(this->type_name, this->variable, false, nullptr,
                                                                    (object_t *)(ast_node_t *)this);

        __array = vd.define_local(
            __new_obj<type_name_t>(uncertain_type_t::instance()),
            __to_name(__generated_name(_T("foreach_array"), _F(_T("%1%"), number), string_t())), false, nullptr, (object_t *)(ast_node_t *)this
        );

        __index = vd.define_local(
            __new_obj<type_name_t>(__XPool.get_internal_type(vtype_t::int32_)),
            __to_name(__generated_name(_T("foreach_index"), _F(_T("%1%"), number), string_t())), false, nullptr, (object_t *)(ast_node_t *)this
        );

        // The analysis is delayed before the children are walked: it comes before the analysis
        // of the body, that needs the type of the variable (var).
        this->__delay(context, walk_step_t::analysis);

        bool r = __super_t::on_walk(context, step, tag);

        context.pop();
        return r;
    }

    // Walks the analysis step of an iterator that has GetEnumerator(): the enumerator has
    // MoveNext() and Current.
    //   { enumerator = iterator.GetEnumerator();
    //     for (; enumerator.MoveNext(); ) { variable = enumerator.Current; body } }
    bool for_each_st_ast_node_t::__walk_analysis_enumerator(ast_walk_context_t & context,
                                                            type_t * iterator_type)
    {
        typedef system_expression_t<binary_expression_t> __binary_t;

        // The members of the type that foreach uses.
        atypes_t no_atypes;
        analyze_member_args_t get_enumerator_args(
            member_type_t::method, __to_name(_T("GetEnumerator")), &no_atypes
        );

        method_t * get_enumerator = (method_t *)iterator_type->get_member(get_enumerator_args, true);
        type_t * enumerator_type = get_enumerator != nullptr? get_enumerator->get_type() : nullptr;

        analyze_member_args_t move_next_args(
            member_type_t::method, __to_name(_T("MoveNext")), &no_atypes
        );

        analyze_member_args_t current_args(member_type_t::property, __to_name(_T("Current")));

        method_t * move_next = nullptr;
        property_t * current = nullptr;

        if (enumerator_type != nullptr)
        {
            move_next = (method_t *)enumerator_type->get_member(move_next_args, true);
            current = (property_t *)enumerator_type->get_member(current_args, true);
        }

        if (get_enumerator == nullptr || move_next == nullptr || current == nullptr)
        {
            this->__log(this, __c_t::for_each_type_not_supported, iterator_type);
            return true;
        }

        type_t * element_type = to_type(current->type_name);

        __array->type_name->type = enumerator_type;     // The enumerator.
        if (__is_uncertain(__variable->type_name))
        {
            __variable->type_name->type = element_type;

            // The type of var can be null, as its value.
            __variable->type_name->nullable = true;
        }

        auto name_of = [this](variable_t * variable) -> expression_t * {
            return __new_obj<system_expression_t<name_expression_t>>(variable->get_name());
        };

        auto call_of = [this](const char_t * name) -> expression_t * {
            auto * function = __new_obj<system_expression_t<function_expression_t>>();
            function->set_namex(__new_obj<system_expression_t<name_expression_t>>(__to_name(name)));
            return function;
        };

        // enumerator = iterator.GetEnumerator()
        expression_t * assign_enumerator = __new_obj<__binary_t>(
            operator_t::assign, name_of(__array),
            __new_obj<__binary_t>(
                operator_t::member_point, this->iterator, call_of(_T("GetEnumerator"))
            )
        );

        // enumerator.MoveNext()
        expression_t * condition = __new_obj<__binary_t>(
            operator_t::member_point, name_of(__array), call_of(_T("MoveNext"))
        );

        // variable = enumerator.Current
        expression_t * assign_variable = __new_obj<__binary_t>(
            operator_t::assign, name_of(__variable),
            __new_obj<__binary_t>(
                operator_t::member_point, name_of(__array),
                __new_obj<system_expression_t<name_expression_t>>(__to_name(_T("Current")))
            )
        );

        for (expression_t * exp : { assign_enumerator, condition, assign_variable })
            walk_expression(this->__context, context, exp);

        statement_group_t * loop_body = __new_obj<statement_group_t>();
        loop_body->push_back(__new_obj<expression_statement_t>(assign_variable));
        loop_body->push_back(this->body);

        for_statement_t * for_statement = __new_obj<for_statement_t>(
            nullptr, nullptr, condition, nullptr, loop_body
        );

        // The enumerator is disposed when the loop exits.
        statement_t * loop = this->__make_dispose_try(context, __array, enumerator_type,
                                                                            for_statement);
        if (loop == nullptr)
            loop = for_statement;

        statement_group_t * lowered = __new_obj<statement_group_t>();
        lowered->variable_region = __region;
        lowered->push_back(__new_obj<expression_statement_t>(assign_enumerator));
        lowered->push_back(loop);

        this->lowered = lowered;
        return true;
    }

    // Walks the analysis step: a for statement that goes through an array by an index.
    //   { array = iterator; index = 0;
    //     for (; index < array.Length; index = index + 1) { variable = array[index]; body } }
    bool for_each_st_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        // The type of the index, when the core types did not exist (the code of System is
        // analyzed): its place is a field when the foreach is in an iterator.
        if (__index != nullptr && __index->type_name != nullptr && __index->type_name->type == nullptr)
            __index->type_name->type = __XPool.get_internal_type(vtype_t::int32_);

        if (this->iterator == nullptr || this->body == nullptr || __variable == nullptr)
            return true;

        if (!this->iterator->walked)
            walk_expression(this->__context, context, this->iterator);

        type_t * iterator_type = this->iterator->get_type();
        if (iterator_type == nullptr)
        {
            // It follows the error that was told when the expression was analyzed.
            if (this->__context.compile_context.error_count() == 0)
                this->__log(this, __c_t::invalid_expression, this->iterator);
            return true;
        }

        if (iterator_type->this_gtype() != gtype_t::array)
            return __walk_analysis_enumerator(context, iterator_type);

        // An array of any dimension is one-dimensional for the loop, like for its base type
        // Array<T>: array.Length is the number of the items, array[index] is the item.
        type_t * element_type = ((array_type_t *)iterator_type)->element_type;

        __array->type_name->type = iterator_type;
        if (__is_uncertain(__variable->type_name))
        {
            __variable->type_name->type = element_type;

            // The type of var can be null, as its value.
            __variable->type_name->nullable = true;
        }

        // The names are resolved like the names of the programs.
        auto name_of = [this](variable_t * variable) -> expression_t * {
            return __new_obj<system_expression_t<name_expression_t>>(variable->get_name());
        };

        typedef system_expression_t<binary_expression_t> __binary_t;

        // array = iterator
        expression_t * assign_array = __new_obj<__binary_t>(
            operator_t::assign, name_of(__array), this->iterator
        );

        // index = 0
        expression_t * assign_index = __new_obj<__binary_t>(
            operator_t::assign, name_of(__index), __to_value_expression((int32_t)0)
        );

        // index < array.Length
        expression_t * condition = __new_obj<__binary_t>(
            operator_t::less, name_of(__index),
            __new_obj<__binary_t>(
                operator_t::member_point, name_of(__array),
                __new_obj<system_expression_t<name_expression_t>>(__to_name(_T("Length")))
            )
        );

        // index = index + 1
        expression_t * increase = __new_obj<__binary_t>(
            operator_t::assign, name_of(__index),
            __new_obj<__binary_t>(
                operator_t::add, name_of(__index), __to_value_expression((int32_t)1)
            )
        );

        // variable = array[index]
        arguments_t * arguments = __new_obj<arguments_t>();
        argument_t * argument = __new_obj<argument_t>();
        argument->expression = name_of(__index);
        arguments->push_back(argument);

        auto * element = __new_obj<system_expression_t<index_expression_t>>();
        element->set_namex(name_of(__array));
        element->set_arguments(arguments);

        expression_t * assign_variable = __new_obj<__binary_t>(
            operator_t::assign, name_of(__variable), element
        );

        for (expression_t * exp : { assign_array, assign_index, condition, increase,
                                    assign_variable })
        {
            walk_expression(this->__context, context, exp);
        }

        // The body of the loop, one element at a time.
        statement_group_t * loop_body = __new_obj<statement_group_t>();
        loop_body->push_back(__new_obj<expression_statement_t>(assign_variable));
        loop_body->push_back(this->body);

        for_statement_t * for_statement = __new_obj<for_statement_t>(
            nullptr, nullptr, condition, increase, loop_body
        );

        statement_group_t * lowered = __new_obj<statement_group_t>();
        lowered->variable_region = __region;
        lowered->push_back(__new_obj<expression_statement_t>(assign_array));
        lowered->push_back(__new_obj<expression_statement_t>(assign_index));
        lowered->push_back(for_statement);

        this->lowered = lowered;
        return true;
    }

    ////////// ////////// ////////// ////////// //////////
    // using_st

    // Set variables.
    void using_st_ast_node_t::set_variable(name_t name, __el_t * el)
    {
        this->__assign_name(this->variable, name, el, _T("var"));
    }

    // Makes this a using declaration.
    void using_st_ast_node_t::set_declaration(ast_node_t * group)
    {
        __declaration_group = group;
        this->set_child(__body__, group);
    }

    // Commits this node.
    void using_st_ast_node_t::on_commit()
    {
        this->initializer = __to_eobject<expression_t *>(__initializer__);
        this->type_name   = __to_eobject<type_name_t *>(__type_name__);
        this->body        = __to_eobject<statement_t *>(__body__);

        // The object has no variable: using new DisposableObject(); , a name that nothing else
        // has is its variable.
        if (this->type_name == nullptr && this->variable == name_t::null)
        {
            static int __count = 0;

            this->variable  = __to_name(__generated_name(_T("using"), _F(_T("%1%"), ++__count), string_t()));
            this->type_name = __new_obj<type_name_t>(uncertain_type_t::instance());
        }
    }

    // Returns this eobject.
    statement_t * using_st_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool using_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if ((walk_step_t)step == walk_step_t::analysis)
            return __walk_analysis(context);

        // The statements of the group were appended after this node was committed.
        if (__declaration_group != nullptr)
            __declaration_group->on_commit();

        this->__check_name_empty(this->variable, _T("variable"));
        this->__check_empty(this->initializer, this, __c_t::expression_missing, _T("using"));
        this->__check_empty(this->body, this, __c_t::body_missing, _T("using"));

        context.push_new_region();
        __region = context.current_region();

        variable_defination_t vd(this->__context, context, (object_t *)(ast_node_t *)this);

        __variable = vd.define_local(this->type_name, this->variable, false, nullptr,
                                                                    (object_t *)(ast_node_t *)this);

        // The analysis is delayed before the children are walked.
        this->__delay(context, walk_step_t::analysis);

        bool r = __super_t::on_walk(context, step, tag);

        context.pop();
        return r;
    }

    // Walks the analysis step:
    //   { variable = initializer; try { body } finally { if (variable != null) variable.Dispose(); } }
    bool using_st_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        typedef system_expression_t<binary_expression_t> __binary_t;

        if (this->initializer == nullptr || this->body == nullptr || __variable == nullptr)
            return true;

        if (!this->initializer->walked)
            walk_expression(this->__context, context, this->initializer);

        type_t * initializer_type = this->initializer->get_type();
        if (initializer_type == nullptr)
        {
            // It follows the error that was told when the expression was analyzed.
            if (this->__context.compile_context.error_count() == 0)
                this->__log(this, __c_t::invalid_expression, this->initializer);
            return true;
        }

        if (__is_uncertain(__variable->type_name))
            __variable->type_name->type = initializer_type;

        type_t * variable_type = to_type(__variable->type_name);

        expression_t * assign = __new_obj<__binary_t>(
            operator_t::assign,
            __new_obj<system_expression_t<name_expression_t>>(__variable->get_name()),
            this->initializer
        );

        walk_expression(this->__context, context, assign);

        // lock (obj) body: { variable = obj; Monitor.Enter(variable); try { body } finally { Monitor.Exit(variable); } }
        if (__is_lock)
            return __walk_analysis_lock(context, assign, variable_type);

        statement_t * protected_body = this->__make_dispose_try(context, __variable,
                                                                variable_type, this->body);
        if (protected_body == nullptr)
        {
            this->__log(this, __c_t::using_type_not_disposable, variable_type);
            return true;
        }

        statement_group_t * lowered = __new_obj<statement_group_t>();
        lowered->variable_region = __region;
        lowered->push_back(__new_obj<expression_statement_t>(assign));
        lowered->push_back(protected_body);

        this->lowered = lowered;
        return true;
    }

    // Walks the analysis step of a lock statement.
    bool using_st_ast_node_t::__walk_analysis_lock(ast_walk_context_t & context, expression_t * assign,
                                                   type_t * variable_type)
    {
        if (!is_ref_type(variable_type) || variable_type->this_gtype() == gtype_t::null)
        {
            this->__log(this, __c_t::lock_type_not_reference, variable_type);
            return true;
        }

        general_type_t * monitor = __XPool.get_monitor_type();
        method_t * enter = nullptr, * exit = nullptr;

        for (method_t * m : monitor->methods)
        {
            if (m->name == __to_name(_T("Enter")) && m->param_count() == 1)
                enter = m;
            else if (m->name == __to_name(_T("Exit")) && m->param_count() == 1)
                exit = m;
        }

        if (enter == nullptr || exit == nullptr)
        {
            this->__log(this, __c_t::invalid_expression, this->initializer);
            return true;
        }

        auto make_call = [&](method_t * method) -> statement_t * {
            argument_t * argument = __new_obj<argument_t>();
            argument->expression = __new_obj<system_expression_t<name_expression_t>>(__variable->get_name());
            walk_expression(this->__context, context, argument->expression);

            arguments_t * arguments = __new_obj<arguments_t>();
            arguments->push_back(argument);

            expression_t * call = __new_system_expression<function_expression_t>(method, arguments);
            walk_expression(this->__context, context, call);

            return __new_obj<expression_statement_t>(call);
        };

        statement_t * enter_statement = make_call(enter);
        statement_t * protected_body = __new_obj<try_statement_t>(this->body, make_call(exit));

        statement_group_t * lowered = __new_obj<statement_group_t>();
        lowered->variable_region = __region;
        lowered->push_back(__new_obj<expression_statement_t>(assign));
        lowered->push_back(enter_statement);
        lowered->push_back(protected_body);

        this->lowered = lowered;
        return true;
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
            this->name = __XPool.to_name(__generated_name(_T("catch"), string_t(), string_t()));

        context.push_new_region();

        variable_defination_t vd(this->__context, context, __variable_el);
        this->variable = vd.define_local(this->type_name, this->name);
        if (this->variable != nullptr && this->variable->this_type() == variable_type_t::local)
            ((local_variable_t *)this->variable)->no_hoist = true;

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

        // The lambdas in checked { ... } are checked, or not.
        int saved = context.checked_mode;
        if (this->has_checked)
            context.checked_mode = this->checked? 1 : 2;

        this->variable_region = context.current_region();
        bool r = __super_t::on_walk(context, step, tag);

        context.checked_mode = saved;
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
    // declaration_exp

    // Commits this node.
    void declaration_exp_ast_node_t::on_commit()
    {
        this->declare_type_name = __to_eobject<type_name_t *>(__type_name__);
    }

    // Returns this eobject.
    expression_t * declaration_exp_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool declaration_exp_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
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
    // checked_exp

    // Commits this node.
    void checked_exp_ast_node_t::on_commit()
    {
        this->set_expression(__to_eobject<expression_t *>(__expression__));
    }

    // Returns this eobject.
    expression_t * checked_exp_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node.
    bool checked_exp_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        // The lambdas in it are checked, or not.
        int saved = context.checked_mode;
        context.checked_mode = this->checked? 1 : 2;

        bool r = __super_t::on_walk(context, step, tag);

        context.checked_mode = saved;
        return r;
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

        // The members that are set: new T { Name = value }.
        this->each_child_node<argument_ast_t>(__inits__, [this](argument_ast_t * init) {
            this->inits.push_back(init->to_eobject());
        });
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
                // The names of the arguments are bound before the constructor is found by the types of them: the new
                // expression that is an element of an array initializer is analyzed before the array binds its elements.
                if (this->arguments() != nullptr)
                {
                    for (argument_t * argument : *this->arguments())
                    {
                        if (argument != nullptr && argument->expression != nullptr)
                            walk_expression(this->__context, context, argument->expression);
                    }
                }

                type_t * type = to_type(this->type_name);
                if (type != nullptr && !is_generic_param(type))
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
                    else
                    {
                        // The arguments that an implicit operator converts to the types of the params.
                        convert_arguments(this->arguments(), this->constructor);
                    }
                }

                if (type != nullptr && !this->inits.empty())
                    __walk_inits(context, type);

                return true;

            }

            default: return true;
        }
    }

    // Walks the members that an object initializer sets: each is the assignment of the member of the
    // object that is on the top of the stack (the target) to its value.
    void new_ast_node_t::__walk_inits(ast_walk_context_t & context, type_t * type)
    {
        typedef system_expression_t<binary_expression_t> __binary_t;

        if (type->this_ttype() != ttype_t::class_)
        {
            this->__log((ast_node_t *)this, __c_t::object_initializer_not_supported, type);
            return;
        }

        for (argument_t * init : this->inits)
        {
            auto * target = __new_obj<system_expression_t<this_expression_t>>();
            target->set_new_target(type);

            expression_t * member = __new_obj<system_expression_t<name_expression_t>>(init->name);
            expression_t * access = __new_obj<__binary_t>(operator_t::member_point, target, member);
            expression_t * assign = __new_obj<__binary_t>(operator_t::assign, access, init->expression);

            walk_expression(this->__context, context, assign);
            this->init_assigns.push_back(assign);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // new_array

    // Commits this node.
    void new_array_ast_node_t::on_commit()
    {
        this->type_name = __to_eobject<type_name_t *>(__type_name__);

        // new String?[2]
        if (this->element_nullable && this->type_name != nullptr)
            this->type_name->nullable = true;

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
            // An empty initializer has no length of the others: they are 0 too.
            if (initializer->empty())
                return 0;

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
    // lambda

    // Commits this node.
    void lambda_ast_node_t::on_commit()
    {
    }

    // Returns this eobject.
    expression_t * lambda_ast_node_t::to_eobject()
    {
        return this;
    }

    // Walks this node: the params and the body are not walked as its children, but as those
    // of the method that is made.
    bool lambda_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if ((walk_step_t)step == walk_step_t::default_)
            __walk_default(context);
        else if ((walk_step_t)step == walk_step_t::analysis)
            analyze_now(context);
        else if ((walk_step_t)step == walk_step_t::end)
            __walk_end(context);

        return true;
    }

    // Walks the items of the lambda that were kept apart.
    void lambda_ast_node_t::analyze_now(ast_walk_context_t & context)
    {
        context.run_pending(__pending);
    }

    // Returns whether the node has a return statement that returns a value, the lambdas in it
    // are other methods.
    static bool __has_return_value(ast_node_t * node)
    {
        if (node == nullptr)
            return false;

        ast_value_t value = node->value();
        if (value == (ast_value_t)__xcvalue_t::return_st)
            return node->child_count() > 0 && node->child_at(0) != nullptr;

        if (value == (ast_value_t)__xcvalue_t::lambda)
            return false;

        for (size_t index = 0, count = node->child_count(); index < count; index++)
        {
            if (__has_return_value(node->child_at(index)))
                return true;
        }

        return false;
    }

    // Collects the expressions of an expression and those below it.
    static void __collect_sub_expressions(expression_t * expression,
                                          std::vector<expression_t *> & expressions)
    {
        if (expression == nullptr
                || std::find(expressions.begin(), expressions.end(), expression)
                                                                != expressions.end())
        {
            return;
        }

        expressions.push_back(expression);

        for (size_t index = 0, count = expression->expression_count(); index < count; index++)
        {
            __collect_sub_expressions(expression->expression_at(index), expressions);
        }
    }

    // Collects the expressions of a node and its children. The expressions of the lambdas that
    // are in it are collected too, if deep is true, or the lambdas only.
    static void __collect_expressions(ast_node_t * node, std::vector<expression_t *> & expressions,
                                                                         bool deep = true)
    {
        if (node == nullptr)
            return;

        if (expression_ast_t * expression_ast = as<expression_ast_t *>(node))
            __collect_sub_expressions(expression_ast->to_eobject(), expressions);

        if (!deep && node->value() == (ast_value_t)__xcvalue_t::lambda)
            return;

        for (size_t index = 0, count = node->child_count(); index < count; index++)
        {
            __collect_expressions(node->child_at(index), expressions, deep);
        }
    }

    // Returns the type name of a local variable or a param.
    static type_name_t * __type_name_of(variable_t * variable)
    {
        if (variable->this_type() == variable_type_t::param)
            return ((param_variable_t *)variable)->param->type_name;

        return ((local_variable_t *)variable)->type_name;
    }

    // Makes the method of the lambda, a method of the type that has the current method: it
    // is static if that one is. It is walked as a method of the type, so it cannot see the
    // variables of the method that has the lambda. The lambda is then the name of it.
    void lambda_ast_node_t::__walk_default(ast_walk_context_t & context)
    {
        type_t * type = context.current_type();
        method_t * outer = context.current_method();

        if (type == nullptr || type->this_gtype() != gtype_t::general || outer == nullptr)
        {
            this->__log((ast_node_t *)this, __c_t::lambda_outside_method);
            return;
        }

        // The serial of the lambda is the one among those of the method.
        static std::map<method_t *, int> lambda_serials;
        int lambda_serial = lambda_serials[outer]++;

        string_t owner_text = __generated_owner(((general_type_t *)type)->to_full_name(), outer);
        name_t name = __to_name(__generated_name(_T("lambda"), lambda_serial, owner_text));

        // The params that have no types.
        params_t * untyped_params = nullptr;
        if (!__param_names.empty())
        {
            untyped_params = __new_obj<params_t>();
            for (name_t param_name : __param_names)
            {
                untyped_params->push_back(__new_obj<param_t>(
                    __new_obj<type_name_t>(uncertain_type_t::instance()), param_name
                ));
            }
        }

        // The local variables of the method that has the lambda, that it uses.
        __collect_expressions(this->child_at(__body__), __used);
        __collect_expressions(this->child_at(__expression__), __used);
        __collect_expressions(this->child_at(__body__), __own, false);
        __collect_expressions(this->child_at(__expression__), __own, false);

        {
            params_t * typed_params = __to_eobject<params_t *>(__params__);

            auto is_param = [&](name_t used_name) {
                for (name_t param_name : __param_names)
                {
                    if (param_name == used_name)
                        return true;
                }

                if (typed_params != nullptr)
                {
                    for (param_t * param : *typed_params)
                    {
                        if (param->name == used_name)
                            return true;
                    }
                }

                return false;
            };

            variable_region_t * region = context.current_region();
            for (expression_t * exp : __used)
            {
                if (exp->this_family() != expression_family_t::name || region == nullptr)
                    continue;

                name_t used_name = ((name_expression_t *)exp)->name;
                if (used_name.empty() || is_param(used_name))
                    continue;

                // The params of the method are defined after its body is walked.
                bool outer_param = false;
                if (((method_t *)outer)->params != nullptr)
                {
                    for (param_t * param : *((method_t *)outer)->params)
                        outer_param = outer_param || param->name == used_name;
                }

                // The variables of the method that has the lambda that has this one.
                for (name_t outer_name : ((method_t *)outer)->lambda_captured_names)
                    outer_param = outer_param || outer_name == used_name;

                variable_t * variable = region->get(used_name);
                bool local = variable != nullptr
                    && (variable->this_type() == variable_type_t::local
                        || variable->this_type() == variable_type_t::param);

                if (!local && !outer_param)
                    continue;

                if (std::find(__captured_names.begin(), __captured_names.end(), used_name)
                                                                    == __captured_names.end())
                {
                    __captured_names.push_back(used_name);
                }
            }
        }

        // The class of the object that has the values of the variables.
        if (!__captured_names.empty())
        {
            string_t env_name = __generated_name(_T("env"), lambda_serial, owner_text);
            string_t fields_text, params_text, assigns_text;
            string_t G;

            for (size_t index = 0; index < __captured_names.size(); index++)
            {
                string_t i = _F(_T("%1%"), (int)index);
                string_t field = __generated_name(_T("captured"),
                                            _str(__captured_names[index]), env_name);

                fields_text += _T(" ") + G + _T("public Object ") + field + _T(";");
                params_text += string_t(index > 0? _T(", ") : _T("")) + _T("Object p") + i;
                assigns_text += _T(" this.") + field + _T(" = p") + i + _T(";");
            }

            string_t env_text = G + _T("public class ") + env_name + _T(" {") + fields_text
                + _T(" ") + G + _T("public ") + env_name + _T("(") + params_text + _T(") {")
                + assigns_text + _T(" } }");

            ast_node_t * root = this->__parse_snippet(env_text);
            if (root == nullptr || root->child_count() == 0)
            {
                this->__log((ast_node_t *)this, __c_t::lambda_capture_not_supported,
                    _T("because its code cannot be made"));
                __captured_names.clear();
            }
            else
            {
                type_ast_node_t * env_node = (type_ast_node_t *)root->child_at(0);

                this->__mark_generated_type(context, (general_type_t *)env_node);

                // The class is a nest type of the type that has the method.
                ((general_type_t *)type)->nest_types.push_back((type_t *)env_node);

                auto * env_layer = context.enter_type_level();
                env_node->on_walk(context, (int)walk_step_t::default_, nullptr);
                context.leave_type_level(env_layer);

                __env_node = env_node;
            }
        }

        bool captures = !__captured_names.empty();

        method_ast_node_t * method = __new_obj<method_ast_node_t>(this->__context);
        method->checked_mode = context.checked_mode;
        method->lambda_no_value = this->child_at(__body__) != nullptr
                                    && !__has_return_value(this->child_at(__body__));
        method->init_lambda(name, outer->is_static() && !captures, this->child_at(__params__),
            this->child_at(__body__), this->child_at(__expression__),
            __to_eobject<expression_t *>(__expression__), (ast_node_t *)this, untyped_params
        );

        ((general_type_t *)type)->append_member((method_t *)method);
        this->__mark_generated(context, (method_t *)method, (method_t *)method);

        // The body is analyzed after the statements that give the types of the params.
        // The items of the body are kept apart (see begin_pending), they are ordered among
        // themselves: the bias of the lambda that has it (a lambda in a lambda) does not add up,
        // or the steps of the body would be those of the later steps.
        int saved_bias = context.delay_bias;
        context.delay_bias = untyped_params != nullptr? 1 : 0;

        if (captures)
        {
            // The variables of the method are seen by the lambda.
            method->lambda_walk_inside = true;
            method->lambda_captured_names = __captured_names;

            __pending = context.begin_pending();
            method->on_walk(context, (int)walk_step_t::default_, nullptr);
            context.end_pending(__pending);

            {
                auto * layer = context.enter_type_level();
                variable_defination_t vd(this->__context, context, (method_t *)method);
                vd.define_method(method);
                context.leave_type_level(layer);
            }

            __method = method;
            __outer  = outer;

            this->__delay(context, walk_step_t::end);
        }
        else
        {
            auto * layer = context.enter_type_level();

            __pending = context.begin_pending();
            method->on_walk(context, (int)walk_step_t::default_, nullptr);
            context.end_pending(__pending);

            context.leave_type_level(layer);
        }

        context.delay_bias = saved_bias;

        // The body is analyzed after the statement that has the lambda: it gives the type of the
        // delegate that it is converted to, or the types of the params. Or earlier, when the
        // type of the lambda is needed.
        method->lambda_pending = __pending;

        context.delay_bias = saved_bias + (untyped_params != nullptr? 2 : 1);
        context.delay(this, (int)walk_step_t::analysis);
        context.delay_bias = saved_bias;

        // The name is that of the method: a lambda that is returned by a lambda is bound to the
        // type of the delegate when the one that has it is, before it is analyzed.
        if (method->variable != nullptr)
            this->set(method->variable);

        this->name = name;
    }

    // The end of the lambda that uses local variables. The class of its environment has a
    // field for each of them: they are the fields that the lambda uses, and the delegate has an
    // object of that class that has the values of the variables when the lambda is made.
    void lambda_ast_node_t::__walk_end(ast_walk_context_t & context)
    {
        if (__env_node == nullptr || __method == nullptr)
            return;

        general_type_t * env = __env_node;

        // The variables: the params of the method are defined now.
        variable_region_t * region = context.current_region();
        for (name_t captured_name : __captured_names)
        {
            variable_t * variable = region != nullptr? region->get(captured_name) : nullptr;
            if (variable == nullptr || (variable->this_type() != variable_type_t::local
                                    && variable->this_type() != variable_type_t::param))
            {
                this->__log((ast_node_t *)this, __c_t::lambda_capture_not_supported,
                    _T("because its code cannot be made"));
                return;
            }

            __captured.push_back(variable);
        }

        // The fields and the constructor.
        std::vector<field_t *> fields;
        for (size_t index = 0; index < __captured.size(); index++)
        {
            name_t field_name = __to_name(__generated_name(_T("captured"),
                                    _str(__captured_names[index]), _str(env->name)));
            field_t * found = nullptr;

            for (field_t * field : env->fields)
            {
                if (field->name == field_name)
                    found = field;
            }

            fields.push_back(found);
        }

        method_t * constructor = nullptr;
        for (method_t * method : env->methods)
        {
            if (method->trait == method_trait_t::constructor)
                constructor = method;
        }

        bool complete = constructor != nullptr && constructor->params != nullptr
                    && constructor->params->size() == __captured.size();

        for (field_t * field : fields)
            complete = complete && field != nullptr;

        if (!complete)
        {
            this->__log((ast_node_t *)this, __c_t::lambda_capture_not_supported,
                _T("because its code cannot be made"));
            return;
        }

        // The types of the variables.
        for (size_t index = 0; index < __captured.size(); index++)
        {
            type_name_t * type_name = __type_name_of(__captured[index]);

            fields[index]->type_name = type_name;
            (*constructor->params)[index]->type_name = type_name;
        }

        // The variables are not modified.
        auto is_written = [&](expression_t * exp) -> bool {
            expression_t * parent = exp->parent;
            op_expression_base_t * op_exp = dynamic_cast<op_expression_base_t *>(parent);
            if (op_exp == nullptr)
                return false;

            switch (op_exp->get_operator_property()->op)
            {
                case operator_t::assign:
                case operator_t::add_assign: case operator_t::sub_assign:
                case operator_t::mul_assign: case operator_t::div_assign:
                case operator_t::mod_assign:
                case operator_t::left_shift_assign: case operator_t::right_shift_assign:
                case operator_t::bit_and_assign: case operator_t::bit_or_assign:
                case operator_t::bit_xor_assign: case operator_t::null_coalescing_assign:
                    return parent->expression_at(0) == exp;

                case operator_t::left_increment: case operator_t::left_decrement:
                case operator_t::right_increment: case operator_t::right_decrement:
                    return true;

                default:
                    return false;
            }
        };

        std::vector<expression_t *> all(__used);
        method_ast_node_t * outer_ast = (method_ast_node_t *)__outer;
        __collect_expressions(outer_ast->child_at(method_ast_node_t::__body__), all);

        bool modified = false;
        for (expression_t * exp : all)
        {
            if (modified || exp->this_family() != expression_family_t::name)
                continue;

            name_expression_t * name_exp = (name_expression_t *)exp;
            if (name_exp->expression_type != name_expression_type_t::variable
                    || name_exp->variable == nullptr || !is_written(exp))
            {
                continue;
            }

            if (std::find(__captured.begin(), __captured.end(), name_exp->variable)
                                                                    != __captured.end())
            {
                this->__log((ast_node_t *)this, __c_t::lambda_capture_modified,
                                                    _str(name_exp->variable->get_name()));
                modified = true;
            }
        }

        // The expressions of the lambda, not those of the lambdas in it: they have their own
        // environment, that is made here with the values of the variables of this one.
        std::vector<expression_t *> own(__own);
        for (expression_t * exp : __own)
        {
            lambda_ast_node_t * nested = dynamic_cast<lambda_ast_node_t *>(exp);
            expression_t * env_new = nested != nullptr? nested->env_new() : nullptr;

            if (env_new != nullptr && env_new->this_family() == expression_family_t::new_)
            {
                arguments_t * nested_args = ((new_expression_t *)env_new)->arguments();
                for (argument_t * argument : *nested_args)
                    own.push_back(argument->expression);
            }
        }

        // The members of the object are not seen: the object of the lambda is the environment.
        for (expression_t * exp : own)
        {
            bool member = exp->this_family() == expression_family_t::this_
                       || exp->this_family() == expression_family_t::base;

            if (exp->this_family() == expression_family_t::name)
            {
                name_expression_t * name_exp = (name_expression_t *)exp;
                variable_t * variable = name_exp->variable;

                if (name_exp->expression_type == name_expression_type_t::variable
                        && variable != nullptr)
                {
                    variable_type_t vt = variable->this_type();
                    if (vt == variable_type_t::field || vt == variable_type_t::property
                            || vt == variable_type_t::event || vt == variable_type_t::method)
                    {
                        member_t * relation = variable->get_relation_member();
                        bool right = false;

                        if (exp->parent != nullptr)
                        {
                            binary_expression_t * binary =
                                        dynamic_cast<binary_expression_t *>(exp->parent);

                            right = binary != nullptr
                                    && binary->op() == operator_t::member_point
                                    && binary->exp2() == exp;
                        }

                        bool lambda = vt == variable_type_t::method
                                    && ((method_variable_t *)variable)->method != nullptr
                                    && ((method_variable_t *)variable)->method->is_lambda;

                        member = relation != nullptr && !relation->is_static()
                                    && !right && !lambda;
                    }
                }
            }

            if (member)
            {
                this->__log((ast_node_t *)this, __c_t::lambda_capture_member);
                break;
            }
        }

        // The names are the fields.
        for (expression_t * exp : own)
        {
            if (exp->this_family() != expression_family_t::name)
                continue;

            name_expression_t * name_exp = (name_expression_t *)exp;
            if (name_exp->expression_type != name_expression_type_t::variable)
                continue;

            for (size_t index = 0; index < __captured.size(); index++)
            {
                if (name_exp->variable != __captured[index])
                    continue;

                field_t * field = fields[index];
                if (field->variable == nullptr)
                    field->variable = __new_obj<field_variable_t>(field);

                name_exp->variable = field->variable;
            }
        }

        // The object of the delegate: new Env(variable, ...)
        arguments_t * arguments = __new_obj<arguments_t>();
        for (variable_t * variable : __captured)
        {
            argument_t * argument = __new_obj<argument_t>();
            argument->expression = __new_obj<system_expression_t<name_expression_t>>(
                variable->get_name()
            );

            arguments->push_back(argument);
        }

        auto * new_exp = __new_obj<system_expression_t<new_expression_t>>();
        new_exp->type_name = __new_obj<type_name_t>((type_t *)env);
        new_exp->set_arguments(arguments);

        // The object is the value of an expression that is effective: it is not popped.
        __new_obj<system_expression_t<binary_expression_t>>(operator_t::assign,
            __new_obj<system_expression_t<name_expression_t>>(__captured[0]->get_name()), new_exp
        );

        walk_expression(this->__context, context, new_exp);

        new_exp->constructor = find_constructor(this->__context, (type_t *)env, arguments);
        if (new_exp->constructor == nullptr)
        {
            this->__log((ast_node_t *)this, __c_t::lambda_capture_not_supported,
                _T("because its code cannot be made"));
            return;
        }

        __method->lambda_env_new = new_exp;
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

        if (__name_el == nullptr)
            __name_el = el;

        const char_t * op_name = op_property->name;
        _A(op_name != nullptr);

        this->name = __to_name(_F(__op_checked? _T("op_Checked%1%") : _T("op_%1%"), op_name));
    }

    // Sets that the method is a conversion: the name tells the kind (op_Implicit, op_Explicit, op_CheckedExplicit).
    void method_ast_node_t::set_conversion(__el_t * el)
    {
        __is_conversion = true;

        // The place of the name, for the messages (a duplicated conversion).
        if (__name_el == nullptr)
            __name_el = el;

        if (__conversion_kind == 0)
        {
            this->__log(el, __c_t::conversion_kind_missing);
            this->name = __to_name(_T("op_Explicit"));
            return;
        }

        if (!this->name.empty() || __op_property != nullptr)
        {
            __log(el, __c_t::duplicate, _F(_T("method name / operator overload")), _T("conversion"));
            return;
        }

        this->name = __to_name(__conversion_kind == 1? _T("op_Implicit")
                                    : (__op_checked? _T("op_CheckedExplicit") : _T("op_Explicit")));
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
        __link_constraints(this->generic_params, this->constraints);
        this->attributes      = __to_eobject_with_owner<attributes_t *>(__attributes__,
                                                                (method_t *)this);
    }

    // Returns whether the first param is (this Type name): the method is an extension method.
    bool method_ast_node_t::has_this_param() const
    {
        if (this->params == nullptr || this->params->empty())
            return false;

        param_ast_node_t * first = dynamic_cast<param_ast_node_t *>((*this->params)[0]);
        return first != nullptr && first->is_this;
    }

    // Returns this eobject.
    method_t * method_ast_node_t::to_eobject()
    {
        return this;
    }

    // Makes this method the method of a lambda.
    void method_ast_node_t::init_lambda(name_t name, bool is_static, ast_node_t * params,
        ast_node_t * body, ast_node_t * expression_node, expression_t * expression,
        ast_node_t * lambda_node, params_t * untyped_params)
    {
        if (params != nullptr)
            this->set_child(__params__, params);

        if (body != nullptr)
            this->set_child(__body__, body);

        this->on_commit();

        if (untyped_params != nullptr)
        {
            this->params = untyped_params;
            this->lambda_implicit = true;
        }

        this->name      = name;
        this->trait     = method_trait_t::normal;
        this->is_lambda = true;
        this->lambda_expression = expression;
        this->lambda_expression_node = expression_node;
        this->lambda_node = lambda_node;

        // The return type is the one of the delegate that the lambda is converted to.
        this->type_name = __new_obj<type_name_t>(uncertain_type_t::instance());

        this->decorate = __new_obj<decorate_t>(decorate_value_t::private_);
        this->decorate->is_static = is_static;
    }

    // Walks this node.
    bool method_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_: {
                if (__partial_skipped)
                    return true;

                __check_extension(context);

                context.push(this->to_eobject());

                // The method that does the work of an iterator: the params of the method of
                // the user are local variables, that are fields of the state object.
                if (this->iterator_info != nullptr && ((iterator_info_t *)this->iterator_info)->params != nullptr)
                {
                    variable_defination_t vd(this->__context, context,
                                                        (object_t *)(ast_node_t *)this);

                    // The params of a generic method have the generic params of the method of the
                    // user: the code has those of this method.
                    iterator_info_t * info = (iterator_info_t *)this->iterator_info;
                    generic_params_t * user_params = info->user_method != nullptr?
                                                        info->user_method->generic_params : nullptr;

                    for (param_t * param : *info->params)
                    {
                        // The items of a params generic parameter are params of this method.
                        if (param->ptype == param_type_t::extends)
                            continue;

                        type_name_t * type_name = param->type_name;

                        // The types are not known yet: they are given in the step pre_analysis.
                        if (user_params != nullptr && this->generic_params != nullptr
                                && type_name != nullptr)
                        {
                            type_name_t * copy = __new_obj<type_name_t>();
                            info->param_remaps.push_back({ type_name, copy });
                            type_name = copy;
                        }

                        vd.define_local(type_name, param->name, false, nullptr,
                                                        (object_t *)(ast_node_t *)this);
                    }

                    if (!info->param_remaps.empty())
                        this->__delay(context, walk_step_t::pre_analysis);
                }

                // The types of the params of the lambda are checked before its body is analyzed.
                if (this->lambda_implicit)
                    this->__delay(context, walk_step_t::analysis, (void *)this->params);

                __super_t::on_walk(context, step, tag);

                // A partial method that has no implementation does nothing.
                if (__partial_empty && this->body == nullptr)
                {
                    this->body = __new_obj<method_body_t>(context.current_region());
                    this->body->push_back(__new_obj<return_statement_t>());
                }

                // The expression that is the body of a lambda.
                if (this->lambda_expression_node != nullptr)
                    this->lambda_expression_node->on_walk(context, step, tag);

                bool r = __walk_default(context);
                if (r)
                {
                    this->__delay(context, walk_step_t::analysis);

                    // The return type of a lambda is given by the statement that has it,
                    // in its analysis step.
                    if (this->is_lambda)
                        this->__delay(context, walk_step_t::post_analysis);

                    // The iterator is made when all the types are known.
                    if (this->iterator_info != nullptr)
                        this->__delay(context, walk_step_t::end);
                }

                context.pop();

                if (this->trait == method_trait_t::normal && !this->lambda_walk_inside)
                {
                    variable_defination_t vd(this->__context, context, (method_t *)this);
                    vd.define_method(this);
                }

                return r;
            }

            case walk_step_t::analysis:
                if (tag != nullptr)
                {
                    __fix_implicit_lambda(context);
                    return true;
                }

                return __walk_analysis(context);

            case walk_step_t::pre_analysis:
                __attach_extension(context);
                __remap_iterator_params();
                return true;

            case walk_step_t::post_analysis:
                __walk_lambda_analysis(context);
                return true;

            case walk_step_t::end:
                __walk_iterator_end(context);
                return true;

            default: return true;
        }
    }

    // Checks the extension method: the param "this" is the first, and the method is a static
    // method of a static class that is not generic.
    void method_ast_node_t::__check_extension(ast_walk_context_t & context)
    {
        if (this->params == nullptr)
            return;

        for (size_t index = 1, count = this->params->size(); index < count; index++)
        {
            param_ast_node_t * param = dynamic_cast<param_ast_node_t *>((*this->params)[index]);
            if (param != nullptr && param->is_this)
                this->__log(param, __c_t::this_param_not_first);
        }

        if (!this->has_this_param())
            return;

        type_t * type = context.current_type();
        bool valid = this->is_static() && type != nullptr && type->this_gtype() == gtype_t::general
                        && ((general_type_t *)type)->is_static()
                        && !((general_type_t *)type)->is_generic();

        if (!valid)
        {
            this->__log(this, __c_t::extension_method_wrong_container);
            return;
        }

        // The attribute is written when the types are known.
        __extension_type = (general_type_t *)type;
        this->__delay(context, walk_step_t::pre_analysis);
    }

    // The types of the params of the method of the user are those of the generic params of the
    // method that does the work of an iterator.
    void method_ast_node_t::__remap_iterator_params()
    {
        iterator_info_t * info = (iterator_info_t *)this->iterator_info;
        if (info == nullptr || info->user_method == nullptr)
            return;

        for (auto & item : info->param_remaps)
        {
            item.second->type = __remap_generic_params(
                item.first->type, info->user_method->generic_params, this->generic_params
            );

            item.second->nullable = item.first->nullable;
        }
    }

    // Marks the extension method, its class and its assembly with the attribute
    // System.Runtime.CompilerServices.ExtensionAttribute, as .NET does: the extension methods
    // are found by the assemblies and the classes that have the attribute.
    void method_ast_node_t::__attach_extension(ast_walk_context_t & context)
    {
        general_type_t * extension_type = __get_xpool().get_extension_attribute_type();

        // The assembly System is compiled: its types are not known by the pool yet.
        if (extension_type == nullptr)
        {
            context.assembly.types.each_general_types([&](general_type_t * type) {
                if (extension_type == nullptr && type->name == __to_name(_T("ExtensionAttribute"))
                        && type->namespace_ != nullptr && type->namespace_->full_name != nullptr
                        && (string_t)*type->namespace_->full_name == _T("Runtime.CompilerServices"))
                {
                    extension_type = type;
                }
            });
        }

        if (extension_type == nullptr || __extension_type == nullptr)
            return;

        method_t * constructor = find_default_constructor(extension_type);
        if (constructor == nullptr)
            return;

        auto mark = [&](with_attributes_object_t * target, eobject_t * owner) {
            if (target->has_attribute(extension_type))
                return;

            attribute_t * attribute = __new_obj<attribute_t>();
            attribute->type_name    = __new_obj<type_name_t>((type_t *)extension_type);
            attribute->constructor  = constructor;

            if (owner != nullptr)
                attribute->set_owner(owner);

            if (target->attributes == nullptr)
                target->attributes = __new_obj<attributes_t>();

            target->attributes->push_back(attribute);
        };

        mark((method_t *)this, (method_t *)this);
        mark(__extension_type, __extension_type);
        mark(&context.assembly, nullptr);
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
                param_variable_t * variable = vd.define_param(param);

                // The state object of an iterator: the first param.
                if (this->iterator_info != nullptr && ((iterator_info_t *)this->iterator_info)->base == nullptr)
                    ((iterator_info_t *)this->iterator_info)->base = variable;
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

    // The end of the method that does the work of an iterator: all the types are known. The
    // local variables are fields of the state object, the names of them are those fields, the
    // yield statements are the places where the method goes on at the next call:
    //     if (state == -1) return false;
    //     if (state == 1) goto label1; ...
    //     ... current = x; state = 1; return true; label1: ...
    //     state = -1; return false;
    void method_ast_node_t::__walk_iterator_end(ast_walk_context_t & context)
    {
        typedef system_expression_t<binary_expression_t> __binary_t;

        iterator_info_t * info = (iterator_info_t *)this->iterator_info;
        general_type_t * state = info->state_type;

        if (info->base == nullptr || state == nullptr || this->body == nullptr)
            return;

        auto find_field = [&](name_t name) -> field_t * {
            for (field_t * field : state->fields)
            {
                if (field->name == name)
                    return field;
            }

            return nullptr;
        };

        // The state object of a generic method has the generic params, that are not those of this
        // method: the fields are declared with those of the state object, and the code uses the
        // fields of the state object that has those of this method (State<T>), like it uses the
        // fields of any generic object.
        bool generic_state = state->params != nullptr && state->params->size() > 0
                                && this->generic_params != nullptr;

        type_t * state_instance = nullptr;

        // The field of the state object that the code uses.
        auto code_field_of = [&](field_t * field) -> field_t * {
            if (!generic_state)
                return field;

            if (state_instance == nullptr)
            {
                type_collection_t args;
                for (generic_param_t * gp : *this->generic_params)
                    args.push_back((type_t *)gp);

                // The state is nested in the type that has the method: it is the host of the
                // instance (Box<U>.State<T>), the type itself is Box<U> in its own methods.
                type_t * host = state->host_type;

                state_instance = __XPool.new_generic_type(state, args, host);
                context.commit_types();

                // The fields that were made after it.
                if (state_instance->this_gtype() == gtype_t::generic)
                    ((generic_type_t *)state_instance)->sync_template_fields();
            }

            analyze_member_args_t member_args(member_type_t::field, field->name);
            field_t * found = (field_t *)state_instance->get_member(member_args, true);
            return found != nullptr? found : field;
        };

        // The variable of a field of the state object: the object is the first param.
        auto variable_of = [&](field_t * field) -> field_variable_t * {
            auto * variable = __new_obj<field_variable_t>(code_field_of(field));
            variable->hoist_base = info->base;
            return variable;
        };

        auto name_of = [&](field_t * field) -> expression_t * {
            return __new_obj<system_expression_t<name_expression_t>>(variable_of(field));
        };

        auto assign_of = [&](field_t * field, expression_t * value) -> expression_t * {
            expression_t * assign = __new_obj<__binary_t>(operator_t::assign,
                                                          name_of(field), value);
            walk_expression(this->__context, context, assign);
            return assign;
        };

        // The local variables are fields. (All the fields are made before the variables that
        // use them: the fields of a generic object are known when it is made.)
        int index = 0;
        std::vector<std::pair<local_variable_t *, field_t *>> hoisted;

        for (local_variable_t * variable : info->locals)
        {
            if (variable->no_hoist)
                continue;

            field_t * field = find_field(variable->name);   // The params.
            if (field == nullptr)
            {
                field = __new_obj<field_t>();
                field->name = __to_name(__generated_name(_T("local"),
                    _F(_T("%1%_%2%"), ++index, _str(variable->name)), _str(state->name)));
                field->type_name = variable->type_name;

                if (generic_state && variable->type_name != nullptr)
                {
                    field->type_name = __new_obj<type_name_t>(__remap_generic_params(
                        variable->type_name->type, this->generic_params, state->params
                    ));
                }

                field->decorate = __new_obj<decorate_t>(decorate_value_t::public_);
                field->host_type = state;
                field->variable = __new_obj<field_variable_t>(field);

                state->append_member(field);
                this->__mark_generated(context, field, field);
            }

            hoisted.push_back({ variable, field });
        }

        for (auto & item : hoisted)
            item.first->hoisted = variable_of(item.second);

        context.commit_types();

        // The names are the fields.
        for (name_expression_t * name_exp : info->refs)
        {
            if (name_exp->variable == nullptr
                    || name_exp->variable->this_type() != variable_type_t::local)
            {
                continue;
            }

            local_variable_t * variable = (local_variable_t *)name_exp->variable;
            if (variable->hoisted != nullptr)
                name_exp->variable = variable->hoisted;
        }

        // The delegates that are called.
        for (function_expression_t * call : info->calls)
        {
            variable_t * called = call->get_variable();
            if (called != nullptr && called->this_type() == variable_type_t::local
                    && ((local_variable_t *)called)->hoisted != nullptr)
            {
                call->set_variable(((local_variable_t *)called)->hoisted);
            }
        }

        // The initial values are assigned to the fields.
        for (defination_statement_item_t * item : info->defs)
        {
            if (item->variable == nullptr || item->variable->hoisted == nullptr
                    || item->expression == nullptr)
            {
                continue;
            }

            expression_t * assign = __new_obj<__binary_t>(operator_t::assign,
                __new_obj<system_expression_t<name_expression_t>>(item->variable->hoisted),
                item->expression
            );

            walk_expression(this->__context, context, assign);
            item->hoist_assign = assign;
        }

        // The yield statements.
        field_t * state_field = find_field(__to_name(__generated_name(_T("state"), string_t(), _str(state->name))));
        field_t * current_field = find_field(__to_name(__generated_name(_T("current"), string_t(), _str(state->name))));
        if (state_field == nullptr || current_field == nullptr)
            return;

        auto value_of = [&](int32_t value) -> expression_t * {
            return __to_value_expression(value);
        };

        auto return_of = [&](bool value) -> statement_t * {
            return __new_obj<return_statement_t>(__to_value_expression(value));
        };

        auto statement_of = [&](expression_t * exp) -> statement_t * {
            return __new_obj<expression_statement_t>(exp);
        };

        iterator_dispatch_statement_t * dispatch = __new_obj<iterator_dispatch_statement_t>();
        int state_index = 0;

        for (yield_statement_t * yield : info->yields)
        {
            statement_group_t * group = __new_obj<statement_group_t>();
            group->variable_region = this->body->variable_region;

            if (yield->is_break)
            {
                group->push_back(statement_of(assign_of(state_field, value_of(-1))));
                group->push_back(return_of(false));
            }
            else
            {
                yield->state = ++state_index;

                group->push_back(statement_of(assign_of(current_field, yield->expression)));
                group->push_back(statement_of(assign_of(state_field, value_of(yield->state))));
                group->push_back(return_of(true));

                expression_t * condition = __new_obj<__binary_t>(operator_t::equal,
                    name_of(state_field), value_of(yield->state)
                );

                walk_expression(this->__context, context, condition);

                dispatch->yields.push_back(yield);
                dispatch->conditions.push_back(condition);
            }

            yield->lowered = group;
        }

        // if (state == -1) return false;
        expression_t * ended = __new_obj<__binary_t>(operator_t::equal,
            name_of(state_field), value_of(-1)
        );

        walk_expression(this->__context, context, ended);
        dispatch->end_check = __new_obj<if_statement_t>(ended, return_of(false), nullptr);

        this->body->push_front((statement_t *)dispatch);

        context.commit_types();
    }

    // The params of a lambda that has no types are the ones of the delegate, if it is not
    // converted to a delegate they have no types: they are objects, to analyze the body.
    void method_ast_node_t::__fix_implicit_lambda(ast_walk_context_t & context)
    {
        bool logged = false;

        for (param_t * param : *this->params)
        {
            if (param->type_name == nullptr
                    || param->type_name->type != uncertain_type_t::instance())
            {
                continue;
            }

            if (!logged)
            {
                this->__log(this->lambda_node, __c_t::lambda_type_undetermined);
                logged = true;
            }

            param->type_name->type = __XPool.get_object_type();
        }
    }

    // Walks the analysis step of the method of a lambda: the return type is known now, the body
    // of a lambda that is an expression is made.
    void method_ast_node_t::__walk_lambda_analysis(ast_walk_context_t & context)
    {
        // A block that returns only null: its type is object.
        if (this->lambda_return_inferred && this->type_name != nullptr
                && is_null(this->type_name->type))
        {
            this->type_name->type = __XPool.get_object_type();
            if (this->variable != nullptr)
                this->variable->reset_type();
        }

        type_t * return_type = this->type_name != nullptr? this->type_name->type : nullptr;
        if (return_type == nullptr || return_type == uncertain_type_t::instance())
        {
            // The type of the body, an expression, or void for a block that returns no value.
            type_t * inferred = nullptr;

            if (this->lambda_expression != nullptr)
            {
                if (!this->lambda_expression->walked)
                    walk_expression(this->__context, context, this->lambda_expression);

                // The type of null is object.
                inferred = this->lambda_expression->get_type();
                if (inferred != nullptr && is_null(inferred))
                    inferred = __XPool.get_object_type();

                if (inferred == uncertain_type_t::instance())
                    inferred = nullptr;
            }
            else if (this->lambda_no_value)
            {
                inferred = __XPool.get_internal_type(vtype_t::void_);
            }

            if (inferred != nullptr)
            {
                return_type = this->type_name->type = inferred;
                if (this->variable != nullptr)
                    this->variable->reset_type();
            }
            else
            {
                this->__log(this->lambda_node, __c_t::lambda_type_undetermined);

                return_type = __XPool.get_internal_type(vtype_t::void_);
                this->type_name->type = return_type;
            }
        }

        if (this->lambda_expression == nullptr)
            return;

        expression_t * exp = this->lambda_expression;
        if (!exp->walked)
            walk_expression(this->__context, context, exp);

        method_body_t * body = __new_obj<method_body_t>(context.current_region());

        if (is_void_type(return_type))
        {
            body->statements.push_back(__new_obj<expression_statement_t>(exp));
            body->statements.push_back(__new_obj<return_statement_t>());
        }
        else
        {
            type_t * type = exp->get_type();
            if (type == nullptr || (!is_type_compatible(type, return_type)
                                    && !is_constant_compatible(exp, return_type)))
            {
                this->__log(this->lambda_node, __c_t::method_incompatible_return_value,
                                                                    (method_t *)this);
            }

            body->statements.push_back(__new_obj<return_statement_t>(exp));
        }

        this->body = body;
    }

    // Walks the analysis step.
    bool method_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        // Construct, static constructor, destructor.
        switch (this->trait)
        {
            case method_trait_t::static_constructor:
                __method_utils_t(__context, context).revise_static_constructor(*this);
                break;

            case method_trait_t::constructor:
                __method_utils_t(__context, context).revise_constructor(*this,
                    __to_eobject<arguments_t *>(__base_ctor_args__),
                    this->child_at(__base_ctor_args__), __ctor_this
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

        // A conversion operator: public and static, one parameter, from or to the containing type.
        if (__is_conversion)
        {
            type_t * host_type = context.current_type();

            // The members of an interface are public, its conversions are static abstract (or virtual).
            if (!this->is_static() || (!this->is_public() && !(host_type != nullptr && is_interface(host_type))))
                this->__log(this, __c_t::operator_overload_prototype_error);

            if (this->param_count() != 1)
            {
                this->__log(this, __c_t::conversion_wrong_param_count);
            }
            else
            {
                type_t * from_type = this->params->at(0)->get_type();
                type_t * to_type = ((__w_t<method_t> *)this)->get_type();

                // In an interface, a generic param (TSelf) is the type that implements it.
                bool in_interface = host_type != nullptr && is_interface(host_type);

                auto is_host = [&](type_t * type) {
                    return type == host_type
                        || (type != nullptr && type->this_gtype() == gtype_t::generic
                                && ((generic_type_t *)type)->template_ == host_type)
                        || (in_interface && type != nullptr && is_generic_param(type));
                };

                if (!is_host(from_type) && !is_host(to_type))
                    this->__log(this, __c_t::conversion_wrong_containing_type);
            }

            return true;
        }

        // Operator overload, checks prototype.
        if (__op_property == nullptr)
            return true;

        // The members of an interface are public, its operators are static abstract (or virtual).
        type_t * host_type = context.current_type();
        bool in_interface = host_type != nullptr && is_interface(host_type);

        if (!this->is_static() || (!this->is_public() && !in_interface))
            this->__log(this, __c_t::operator_overload_prototype_error);

        if (this->param_count() != __op_property->arity)
        {
            this->__log(this, (__op_property->arity == 1)?
                    __c_t::unitary_operator_overload_wrong_param_count :
                    __c_t::binary_operator_overload_wrong_param_count, __op_property->op
            );
        }

        // One of the parameters is the containing type, or in an interface, a generic param
        // (TSelf) that is the type that implements it.
        // In a generic type, it is the type made of the generic params of the type (List<T>).
        bool has_containing_type = al::any_of(*this->params, [&](param_t * param) {
            type_t * param_type = param->get_type();
            return param_type == context.current_type()
                || (param_type != nullptr && param_type->this_gtype() == gtype_t::generic
                        && ((generic_type_t *)param_type)->template_ == context.current_type())
                || (in_interface && param_type != nullptr && is_generic_param(param_type));
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

        // The accessors of a property that the compiler makes (the one of an iterator).
        if (__has_generated_attribute(this->attributes))
        {
            if (this->get_method != nullptr)
                this->__mark_generated(context, this->get_method, this->get_method);

            if (this->set_method != nullptr)
                this->__mark_generated(context, this->set_method, this->set_method);
        }

        if (type->this_ttype() != ttype_t::interface_ && !this->is_abstract())
            __generate_ignored_method_bodies(context, value_param);
    }

    // Walks analysis step.
    bool property_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        // An indexer has no name in the code: the position is that of the property.
        code_element_t * el = __name_el != nullptr? (code_element_t *)__name_el
                                                   : (code_element_t *)(ast_node_t *)this;

        this->__check_member(context.current_type(), (property_t *)this, el);
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
        field->name = __to_name(__generated_name(_T("backing"), _str(this->name),
                                    ((general_type_t *)context.current_type())->to_full_name()));

        decorate_value_t dv = decorate_value_t::default_value;
        dv.is_static = this->is_static();
        dv.access = access_value_t::private_;

        field->decorate = __new_obj<decorate_t>(dv);

        __append_member(this->__context, context, field);

        // What the compiler makes: the field and the accessors, and the property.
        this->__mark_generated(context, field, field);
        this->__mark_generated(context, (property_t *)this, (property_t *)this);

        if (this->get_method != nullptr)
            this->__mark_generated(context, this->get_method, this->get_method);

        if (this->set_method != nullptr)
            this->__mark_generated(context, this->set_method, this->set_method);

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
        this->raise_method    = __to_eobject<method_t *>(__raise_method__);
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
                    this->__delay(context, walk_step_t::confirm);
                    this->__delay(context, walk_step_t::analysis);
                    return true;
                }

                return false;

            case walk_step_t::confirm:
                return __walk_confirm(context);

            case walk_step_t::analysis:
                return __walk_analysis(context);

            default: return true;
        }
    }

    // Makes the add and remove methods of an event that has no accessors, and the field that keeps
    // the handlers: `event Handler Changed;` is a field of the list of the handlers, add_Changed
    // and remove_Changed. The accessors of an abstract event, and of an event of an interface, have
    // no body and no field.
    void event_ast_node_t::__make_default_accessors(ast_walk_context_t & context,
                                                    bool abstract_accessors)
    {
        xpool_t & xpool = __get_xpool();

        decorate_value_t dv = decorate_value_t::default_value;
        dv.is_static = this->is_static();
        dv.access = access_value_t::private_;

        if (!abstract_accessors)
        {
            // The type of the field is that of the handlers of the type of the event: it is made
            // when the type of the event is known.
            __list_type_name = __new_obj<type_name_t>();

            this->field = __new_obj<field_t>();
            this->field->type_name = __list_type_name;
            this->field->name = __to_name(__generated_name(_T("backing"), _str(this->name),
                                    ((general_type_t *)context.current_type())->to_full_name()));
            this->field->decorate = __new_obj<decorate_t>(dv);
            this->field->variable = __new_obj<field_variable_t>(this->field);

            __append_member(this->__context, context, this->field);
            this->__mark_generated(context, this->field, this->field);
        }

        __value_param = __new_obj<param_t>(this->type_name, __to_name(_T("value")));

        auto make_method = [&](const char_t * prefix, operator_t op) {
            method_t * method = __new_obj<method_t>();

            method->name            = __to_name(_F(_T("%1%_%2%"), prefix, this->name));
            method->type_name       = xpool.to_type_name(xpool.get_void_type());
            method->decorate        = this->decorate;
            method->relation_member = this;
            method->owner_type_name = this->owner_type_name;

            params_t * params = __new_obj<params_t>();
            params->push_back(__value_param);
            method->params = params;

            if (!abstract_accessors)
            {
                // field = field + value;  field = field - value;
                expression_statement_t * statement = (expression_statement_t *)
                    __new_expression_statement<binary_expression_t>(
                        operator_t::assign,
                        __new_field_expression(this->field),
                        __new_system_expression<binary_expression_t>(op,
                            __new_field_expression(this->field),
                            __new_param_expression(__value_param)
                        )
                    );

                (op == operator_t::add? __add_expression : __remove_expression) = statement->expression;

                method->body = __new_obj<method_body_t>(std::initializer_list<statement_t *>{
                    statement, __new_obj<return_statement_t>()
                }, context.current_region());
            }

            __append_member(this->__context, context, method);

            if (!abstract_accessors)
                this->__mark_generated(context, method, method);

            return method;
        };

        this->add_method    = make_method(_T("add"), operator_t::add);
        this->remove_method = make_method(_T("remove"), operator_t::sub);

        if (abstract_accessors)
            return;

        // The raise method: its params and its body are made when the type is known.
        __return_type_name = __new_obj<type_name_t>();
        __default_raise = true;

        decorate_value_t raise_dv = decorate_value_t::default_value;
        raise_dv.is_static = this->is_static();
        raise_dv.access = access_value_t::private_;

        method_t * raise = __new_obj<method_t>();
        raise->name            = __to_name(_F(_T("raise_%1%"), this->name));
        raise->type_name       = __return_type_name;
        raise->decorate        = __new_obj<decorate_t>(raise_dv);
        raise->relation_member = this;
        raise->params          = __new_obj<params_t>();

        this->raise_method = raise;
        __append_member(this->__context, context, raise);
        this->__mark_generated(context, raise, raise);
    }

    // Makes the body of the raise method that an event that has no accessors has: the handlers are
    // called if there is any, the result is the one of the last of them.
    void event_ast_node_t::__make_default_raise(generic_type_t * delegate_type)
    {
        xpool_t & xpool = __get_xpool();
        method_t * raise = this->raise_method;

        // The params: those of the delegate.
        arguments_t * arguments = __new_obj<arguments_t>();
        for (size_t k = 1, count = delegate_type->args.size(); k < count; k++)
        {
            param_t * param = __new_obj<param_t>(
                xpool.to_type_name(delegate_type->type_at(k)), __to_name(_F(_T("arg%1%"), k - 1))
            );

            raise->params->push_back(param);

            argument_t * argument = __new_obj<argument_t>();
            argument->expression = __new_param_expression(param);
            arguments->push_back(argument);
        }

        type_t * return_type = delegate_type->type_at(0);

        auto * call = __new_system_expression<function_expression_t>();
        call->set_variable(this->field->variable);
        call->set_arguments(arguments);

        // if (field != null) [return] field(args);
        statement_t * call_statement;
        if (is_void_type(return_type))
            call_statement = __new_obj<expression_statement_t>(call);
        else
            call_statement = __new_obj<return_statement_t>(call);

        statement_t * check = __new_obj<if_statement_t>(
            __new_system_expression<binary_expression_t>(
                operator_t::not_equal, __new_field_expression(this->field),
                __to_value_expression(nullptr)
            ),
            call_statement, nullptr
        );

        raise->body = __new_obj<method_body_t>(__region);
        raise->body->push_back(check);

        if (is_void_type(return_type))
        {
            raise->body->push_back(__new_obj<return_statement_t>());
        }
        else
        {
            raise->body->push_back(__new_obj<return_statement_t>(
                __new_system_expression<default_value_expression_t>(__return_type_name)
            ));
        }
    }

    // Walks default step.
    bool event_ast_node_t::__walk_default(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(this->name, _T("event"));

        type_t * host_type = context.current_type();
        bool in_interface = host_type != nullptr && host_type->this_ttype() == ttype_t::interface_;

        __region = context.current_region();

        bool custom = this->add_method != nullptr || this->remove_method != nullptr
                                                  || this->raise_method != nullptr;

        if (custom)
        {
            if (this->add_method == nullptr)
                this->__log(this, __c_t::event_accessor_missing, this->name, _T("add"));
            else if (this->remove_method == nullptr)
                this->__log(this, __c_t::event_accessor_missing, this->name, _T("remove"));

            if (this->raise_method != nullptr && in_interface)
            {
                this->__log(this, __c_t::event_raise_not_valid, this->name,
                                                    _T("the events of an interface are only added and removed"));
            }
        }
        else
        {
            __make_default_accessors(context, in_interface || this->is_abstract());
        }

        xpool_t & xpool = __get_xpool();

        if (custom)
        {
            __value_param = __new_obj<param_t>(this->type_name, __to_name(_T("value")));

            method_t * accessors[] = { this->add_method, this->remove_method };
            const char_t * prefixes[] = { _T("add"), _T("remove") };

            for (int k = 0; k < 2; k++)
            {
                method_t * method = accessors[k];
                if (method == nullptr)
                    continue;

                method->name = __to_name(_F(_T("%1%_%2%"), prefixes[k], this->name));
                method->type_name = xpool.to_type_name(xpool.get_void_type());
                method->relation_member = this;
                method->owner_type_name = this->owner_type_name;

                if (method->decorate == nullptr)
                    method->decorate = this->decorate;

                params_t * params = __new_obj<params_t>();
                params->push_back(__value_param);
                method->params = params;

                __append_member(this->__context, context, method);
            }

            // The raise method: the type that it returns is that of the delegate.
            if (this->raise_method != nullptr)
            {
                __return_type_name = __new_obj<type_name_t>();

                this->raise_method->name = __to_name(_F(_T("raise_%1%"), this->name));
                this->raise_method->type_name = __return_type_name;
                this->raise_method->relation_member = this;
                this->raise_method->owner_type_name = this->owner_type_name;

                if (this->raise_method->decorate == nullptr)
                    this->raise_method->decorate = this->decorate;

                if (this->raise_method->params == nullptr)
                    this->raise_method->params = __new_obj<params_t>();

                __append_member(this->__context, context, this->raise_method);
            }
        }

        // The variable of the event, that the code names.
        if (!this->name.empty())
        {
            variable_defination_t vd(this->__context, context, (event_t *)this);
            vd.define_event(this);
        }

        context.push(this->to_eobject());
        __super_t::on_walk(context, step, tag);
        context.pop();

        return true;
    }

    // Walks confirm step: the type of the event is known, before the types that are made of
    // the generic types are committed.
    bool event_ast_node_t::__walk_confirm(ast_walk_context_t & context)
    {
        xpool_t & xpool = __get_xpool();

        type_t * type = to_type(this->type_name);
        if (type == nullptr)
            return true;

        generic_type_t * delegate_type = nullptr;
        if (type->this_gtype() == gtype_t::generic
                && ((generic_type_t *)type)->template_ == xpool.get_delegate_type())
        {
            delegate_type = (generic_type_t *)type;
        }

        if (delegate_type == nullptr)
        {
            this->__log(__name_el != nullptr? (code_element_t *)__name_el : (code_element_t *)this,
                __c_t::event_type_not_delegate, type, this->name);
            return true;
        }

        // The type of the field is the list of the handlers, the return type of the raise method
        // is that of the delegate.
        if (__list_type_name != nullptr)
        {
            __list_type_name->type = xpool.new_generic_type(
                xpool.get_multicast_delegate_type(), delegate_type->args, nullptr
            );
        }

        if (__return_type_name != nullptr)
            __return_type_name->type = delegate_type->type_at(0);

        if (__default_raise)
        {
            __make_default_raise(delegate_type);
        }
        else if (this->raise_method != nullptr)
        {
            // The params of a raise accessor are those of the delegate.
            size_t count = this->raise_method->params != nullptr? this->raise_method->params->size() : 0;
            bool match = count + 1 == delegate_type->args.size();

            for (size_t k = 0; match && k < count; k++)
            {
                type_t * param_type = to_type((*this->raise_method->params)[k]->type_name);
                match = param_type == delegate_type->type_at(k + 1);
            }

            if (!match)
            {
                this->__log(this->raise_method, __c_t::event_raise_not_valid, this->name,
                    _F(_T("its params are not those of \"%1%\""), type));
            }
        }

        return true;
    }

    // Walks analysis step.
    bool event_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        // The operators of the bodies of the add and remove methods.
        walk_expression(this->__context, context, __add_expression);
        walk_expression(this->__context, context, __remove_expression);

        this->__check_member(context.current_type(), (event_t *)this, __name_el);
        return true;
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::compile

