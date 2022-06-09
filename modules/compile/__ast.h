#ifndef __COMPILE_AST_H__
#define __COMPILE_AST_H__

namespace X_ROOT_NS::modules::compile {

    namespace
    {
        using namespace core;

        template<typename _obj_t> class __w_t : public _obj_t
        {
            typedef _obj_t __super_t;

        public:
            using __super_t::__super_t;
        };

        template<typename _exp_t>
        using __wexp_t = __w_t<system_expression_t<_exp_t>>;
    }

    ////////// ////////// ////////// ////////// //////////

    // Walking step.
    enum class walk_step_t
    {
        // The first step.
        default_        = 0,

        
        // Pre validate.
        pre_validate    = 8,

        // Validate.
        validate        = 10,

        // Post validate.
        post_validate   = 12,


        // Pre confirm.
        pre_confirm     = 16,

        // Confirm.
        confirm         = 18,

        // Post confirm.
        post_confirm    = 20,

        
        // Pre analysis.
        pre_analysis    = 24,

        // Analysis.
        analysis        = 26,

        // Post analysis.
        post_analysis   = 28,

        // All completed.
        end             = 30,
    };

    ////////// ////////// ////////// ////////// //////////

    #define __BRANCH_ENUM_ITEM(name, t)     ((int_type_t<sizeof(t)>)t::name)
    #define __C_BRANCH_ENUM_ITEM(name)      \
        __BRANCH_ENUM_ITEM(name, ::X_ROOT_NS::modules::core::common_branch_value_t)

    // Branch values.
    X_ENUM_(xbranch_value_t, sizeof(code_element_t))

        asts = __C_BRANCH_ENUM_ITEM(__user_defined__),

        namespace_,                 // Namespace

        mname,                      // Multi name, e.g. System.Text

        general_type_name,          // General type name.

        name_unit,                  // Name unit, e.g. Text in System.Text.

        array_type_name,            // Array type name, e.g. Int32[].

        uncertain_type_name,        // Uncertain type name, may be a general or array type.

        type_def_param,             // Typedef param.

        type_def_params,            // Typedef params.

        type_def,                   // Typedef, e.g. typedef int INT.

        type_of,                    // Typeof, e.g. typeof(int).

        expressions,                // Expressions, e.g. a + b - c.

        argument,                   // Argument.

        arguments,                  // Arguments.

        param,                      // Param.

        params,                     // Params.

        generic_param,              // Generic param.

        generic_params,             // Generic params.

        generic_arg,                // Generic argument.

        generic_args,               // Generic arguments.

        generic_constraint,         // Generic constraint, e.g. where T : class.

        generic_constraints,        // Generic constraints.

        decorate,                   // Decorate of types and members. e.g. public static ...

        method_body,                // Method body.

        statements,                 // Statements.

        expression_st,              // A simple expression statement.

        type_def_st,                // A typedef statement.

        defination_st,              // A defination statement, e.g. int a = 1.

        break_st,                   // A break statement, e.g. break;

        continue_st,                // A continue statement, e.g. continue;

        throw_st,                   // A throw statement, e.g. throw new Exception("...")

        goto_st,                    // A goto statement, e.g. goto __label1.

        return_st,                  // A return statement, e.g. return 1;

        do_while_st,                // do ... while statement, e.g. do { } while (...);

        loop_until_st,              // loop ... unit statememt.

        while_st,                   // while statement.

        for_st,                     // for statement.

        for_each_st,                // for ... each statement.

        if_st,                      // if ... else statement.

        switch_st,                  // switch ... case statement.

        case_,                      // case statement.

        try_st,                     // try ... catch ... finally statement.

        catch_,                     // catch statement.

        empty_st,                   // Empty statement. A simple ";".

        statement_group_st,         // Statement group. " { ... } "

        type_name_exp,              // Typename expression.

        type_cast_exp,              // Type case expression.

        function,                   // Function call expression.

        function_name,              // Function name expression.

        index,                      // Index expression.

        new_,                       // New expression.

        array_initializer,          // Array initializer.

        array_lengths,              // Array lengths.

        new_array,                  // New array.

        default_value,              // Default value. e.g. default(int).

        __user_defined__ = __C_BRANCH_ENUM_ITEM(__user_defined__) + 1024,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    #define __X_BRANCH_ENUM_ITEM(name)  \
        __BRANCH_ENUM_ITEM(name, ::X_ROOT_NS::modules::compile::xbranch_value_t)

    #define X_BRANCH_ENUM(name)  X_ENUM_(name, sizeof(code_element_t))                  \
                                                                                        \
        /* common branch values */                                                      \
                                                                                        \
        import              =   __C_BRANCH_ENUM_ITEM(import),                           \
                                                                                        \
        using_              =   __C_BRANCH_ENUM_ITEM(using_),                           \
                                                                                        \
        expression          =   __C_BRANCH_ENUM_ITEM(expression),                       \
                                                                                        \
        statement           =   __C_BRANCH_ENUM_ITEM(statement),                        \
                                                                                        \
        type_name           =   __C_BRANCH_ENUM_ITEM(type_name),                        \
                                                                                        \
        cvalue              =   __C_BRANCH_ENUM_ITEM(cvalue),                           \
                                                                                        \
        attribute           =   __C_BRANCH_ENUM_ITEM(attribute),                        \
                                                                                        \
        attributes          =   __C_BRANCH_ENUM_ITEM(attributes),                       \
                                                                                        \
        type                =   __C_BRANCH_ENUM_ITEM(type),                             \
                                                                                        \
        field               =   __C_BRANCH_ENUM_ITEM(field),                            \
                                                                                        \
        property            =   __C_BRANCH_ENUM_ITEM(property),                         \
                                                                                        \
        method              =   __C_BRANCH_ENUM_ITEM(method),                           \
                                                                                        \
        event               =   __C_BRANCH_ENUM_ITEM(event),                            \
                                                                                        \
        /* xcommon branch values */                                                     \
                                                                                        \
        asts                =   __X_BRANCH_ENUM_ITEM(asts),                             \
                                                                                        \
        namespace_          =   __X_BRANCH_ENUM_ITEM(namespace_),                       \
                                                                                        \
        array_type_name     =   __X_BRANCH_ENUM_ITEM(array_type_name),                  \
                                                                                        \
        general_type_name   =   __X_BRANCH_ENUM_ITEM(general_type_name),                \
                                                                                        \
        name_unit           =   __X_BRANCH_ENUM_ITEM(name_unit),                        \
                                                                                        \
        uncertain_type_name =   __X_BRANCH_ENUM_ITEM(uncertain_type_name),              \
                                                                                        \
        type_def            =   __X_BRANCH_ENUM_ITEM(type_def),                         \
                                                                                        \
        type_def_param      =   __X_BRANCH_ENUM_ITEM(type_def_param),                   \
                                                                                        \
        type_def_params     =   __X_BRANCH_ENUM_ITEM(type_def_params),                  \
                                                                                        \
        type_of             =   __X_BRANCH_ENUM_ITEM(type_of),                          \
                                                                                        \
        mname               =   __X_BRANCH_ENUM_ITEM(mname),                            \
                                                                                        \
        expressions         =   __X_BRANCH_ENUM_ITEM(expressions),                      \
                                                                                        \
        argument            =   __X_BRANCH_ENUM_ITEM(argument),                         \
                                                                                        \
        arguments           =   __X_BRANCH_ENUM_ITEM(arguments),                        \
                                                                                        \
        param               =   __X_BRANCH_ENUM_ITEM(param),                            \
                                                                                        \
        params              =   __X_BRANCH_ENUM_ITEM(params),                           \
                                                                                        \
        generic_param       =   __X_BRANCH_ENUM_ITEM(generic_param),                    \
                                                                                        \
        generic_params      =   __X_BRANCH_ENUM_ITEM(generic_params),                   \
                                                                                        \
        generic_constraint  =   __X_BRANCH_ENUM_ITEM(generic_constraint),               \
                                                                                        \
        generic_constraints =   __X_BRANCH_ENUM_ITEM(generic_constraints),              \
                                                                                        \
        generic_arg         =   __X_BRANCH_ENUM_ITEM(generic_arg),                      \
                                                                                        \
        generic_args        =   __X_BRANCH_ENUM_ITEM(generic_args),                     \
                                                                                        \
        decorate            =   __X_BRANCH_ENUM_ITEM(decorate),                         \
                                                                                        \
        method_body         =   __X_BRANCH_ENUM_ITEM(method_body),                      \
                                                                                        \
        statements          =   __X_BRANCH_ENUM_ITEM(statements),                       \
                                                                                        \
        expression_st       =   __X_BRANCH_ENUM_ITEM(expression_st),                    \
                                                                                        \
        type_def_st         =   __X_BRANCH_ENUM_ITEM(type_def_st),                      \
                                                                                        \
        defination_st       =   __X_BRANCH_ENUM_ITEM(defination_st),                    \
                                                                                        \
        break_st            =   __X_BRANCH_ENUM_ITEM(break_st),                         \
                                                                                        \
        continue_st         =   __X_BRANCH_ENUM_ITEM(continue_st),                      \
                                                                                        \
        throw_st            =   __X_BRANCH_ENUM_ITEM(throw_st),                         \
                                                                                        \
        goto_st             =   __X_BRANCH_ENUM_ITEM(goto_st),                          \
                                                                                        \
        return_st           =   __X_BRANCH_ENUM_ITEM(return_st),                        \
                                                                                        \
        do_while_st         =   __X_BRANCH_ENUM_ITEM(do_while_st),                      \
                                                                                        \
        loop_until_st       =   __X_BRANCH_ENUM_ITEM(loop_until_st),                    \
                                                                                        \
        while_st            =   __X_BRANCH_ENUM_ITEM(while_st),                         \
                                                                                        \
        for_st              =   __X_BRANCH_ENUM_ITEM(for_st),                           \
                                                                                        \
        for_each_st         =   __X_BRANCH_ENUM_ITEM(for_each_st),                      \
                                                                                        \
        if_st               =   __X_BRANCH_ENUM_ITEM(if_st),                            \
                                                                                        \
        switch_st           =   __X_BRANCH_ENUM_ITEM(switch_st),                        \
                                                                                        \
        case_               =   __X_BRANCH_ENUM_ITEM(case_),                            \
                                                                                        \
        try_st              =   __X_BRANCH_ENUM_ITEM(try_st),                           \
                                                                                        \
        catch_              =   __X_BRANCH_ENUM_ITEM(catch_),                           \
                                                                                        \
        empty_st            =   __X_BRANCH_ENUM_ITEM(empty_st),                         \
                                                                                        \
        statement_group_st  =   __X_BRANCH_ENUM_ITEM(statement_group_st),               \
                                                                                        \
        type_name_exp       =   __X_BRANCH_ENUM_ITEM(type_name_exp),                    \
                                                                                        \
        type_cast_exp       =   __X_BRANCH_ENUM_ITEM(type_cast_exp),                    \
                                                                                        \
        function            =   __X_BRANCH_ENUM_ITEM(function),                         \
                                                                                        \
        function_name       =   __X_BRANCH_ENUM_ITEM(function_name),                    \
                                                                                        \
        index               =   __X_BRANCH_ENUM_ITEM(index),                            \
                                                                                        \
        new_                =   __X_BRANCH_ENUM_ITEM(new_),                             \
                                                                                        \
        array_initializer   =   __X_BRANCH_ENUM_ITEM(array_initializer),                \
                                                                                        \
        array_lengths       =   __X_BRANCH_ENUM_ITEM(array_lengths),                    \
                                                                                        \
        new_array           =   __X_BRANCH_ENUM_ITEM(new_array),                        \
                                                                                        \
        default_value       =   __X_BRANCH_ENUM_ITEM(default_value),                    \
                                                                                        \
        __user_defined__    =  __X_BRANCH_ENUM_ITEM(__user_defined__),


    #define X_BRANCH_ENUM_END   X_ENUM_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    #define X_BRANCH_ENUM_INFO(name)  X_ENUM_INFO(name)                                 \
        X_C(import,                 _T("import"))                                       \
        X_C(using_,                 _T("using"))                                        \
        X_C(expression,             _T("expression"))                                   \
        X_C(statement,              _T("statement"))                                    \
        X_C(type_name,              _T("type_name"))                                    \
        X_C(cvalue,                 _T("cvalue"))                                       \
        X_C(attribute,              _T("attribute"))                                    \
        X_C(attributes,             _T("attributes"))                                   \
        X_C(type,                   _T("type"))                                         \
        X_C(field,                  _T("field"))                                        \
        X_C(property,               _T("property"))                                     \
        X_C(method,                 _T("method"))                                       \
        X_C(event,                  _T("event"))                                        \
                                                                                        \
        X_C(asts,                   _T("asts"))                                         \
        X_C(namespace_,             _T("namespace"))                                    \
        X_C(array_type_name,        _T("array_type_name"))                              \
        X_C(general_type_name,      _T("general_type_name"))                            \
        X_C(name_unit,              _T("name_unit"))                                    \
        X_C(uncertain_type_name,    _T("uncertain_type_name"))                          \
        X_C(type_def_param,         _T("type_def_param"))                               \
        X_C(type_def_params,        _T("type_def_params"))                              \
        X_C(type_def,               _T("type_def"))                                     \
        X_C(type_of,                _T("type_of"))                                      \
        X_C(mname,                  _T("mname"))                                        \
        X_C(expressions,            _T("expressions"))                                  \
        X_C(argument,               _T("argument"))                                     \
        X_C(arguments,              _T("arguments"))                                    \
        X_C(param,                  _T("param"))                                        \
        X_C(params,                 _T("params"))                                       \
        X_C(generic_param,          _T("generic_param"))                                \
        X_C(generic_params,         _T("generic_params"))                               \
        X_C(generic_constraint,     _T("generic_constraint"))                           \
        X_C(generic_constraints,    _T("generic_constraints"))                          \
        X_C(generic_arg,            _T("generic_arg"))                                  \
        X_C(generic_args,           _T("generic_args"))                                 \
        X_C(decorate,               _T("decorate"))                                     \
        X_C(method_body,            _T("method_body"))                                  \
                                                                                        \
        X_C(statements,             _T("statements"))                                   \
        X_C(expression_st,          _T("expression_st"))                                \
        X_C(type_def_st,            _T("type_def_st"))                                  \
        X_C(defination_st,          _T("defination_st"))                                \
        X_C(break_st,               _T("break_st"))                                     \
        X_C(continue_st,            _T("continue_st"))                                  \
        X_C(throw_st,               _T("throw_st"))                                     \
        X_C(goto_st,                _T("goto_st"))                                      \
        X_C(return_st,              _T("return_st"))                                    \
        X_C(do_while_st,            _T("do_while_st"))                                  \
        X_C(loop_until_st,          _T("loop_until_st"))                                \
        X_C(while_st,               _T("while_st"))                                     \
        X_C(for_st,                 _T("for_st"))                                       \
        X_C(for_each_st,            _T("for_each_st"))                                  \
        X_C(if_st,                  _T("if_st"))                                        \
        X_C(switch_st,              _T("switch_st"))                                    \
        X_C(case_,                  _T("case"))                                         \
        X_C(try_st,                 _T("try_st"))                                       \
        X_C(catch_,                 _T("catch"))                                        \
        X_C(empty_st,               _T("empty_st"))                                     \
        X_C(statement_group_st,     _T("statement_group_st"))                           \
                                                                                        \
        X_C(type_name_exp,          _T("type_name_exp"))                                \
        X_C(type_cast_exp,          _T("type_cast_exp"))                                \
        X_C(function,               _T("function"))                                     \
        X_C(function_name,          _T("function_name"))                                \
        X_C(index,                  _T("index"))                                        \
        X_C(new_,                   _T("new"))                                          \
        X_C(array_initializer,      _T("array_initializer"))                            \
        X_C(array_lengths,          _T("array_lengths"))                                \
        X_C(new_array,              _T("new_array"))                                    \
        X_C(default_value,          _T("default_value"))                                \


    #define X_BRANCH_ENUM_INFO_END  X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        typedef ast_node_t              __node_t;
        typedef code_element_t          __el_t;
        typedef ast_value_t             __value_t;
        typedef common_branch_value_t   __cvalue_t;
        typedef xbranch_value_t         __xcvalue_t;
    }

    ////////// ////////// ////////// ////////// //////////

    // Ast error code.
    X_ENUM(ast_error_code_t)

        // Multi-name empty.
        mname_empty,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    // Common log codes.
    X_ENUM_(common_log_code_t, sizeof(compile_log_code_value_t))

        __info__            = 1000,

        __warning__         = 4000,

        // Hides inherit virtual member.
        hides_inherit_virtual_member,

        // Hides inherit member.
        hides_inherit_member,

        // The member does not hide an accessible member. The new keyword is not required.
        not_hides_inherit_member,

        __error__           = 7000,

        // Duplicated
        duplicate           = 7001,

        // Name empty.
        name_empty,

        // Unexpected first char in a name.
        name_unexpected_first_char,

        // Unexpected char in a name.
        name_unexpected_char,

        // Assembly name missing.
        assembly_name_missing,

        // Namespace name missing.
        namespace_name_missing,

        // Type name missing.
        type_name_missing,

        // Argument missing.
        argument_missing,

        // Argument type error.
        argument_type_error,

        // Unexpected param default value.
        unexpected_param_default_value,

        // Condition missing.
        condition_missing,

        // Body missing.
        body_missing,

        // Iterator missing.
        iterator_missing,

        // Statement missing.
        statement_missing,

        // Expression missing.
        expression_missing,

        // Catch or finally missing.
        catch_or_finally_missing,

        // Array lengths missing.
        array_lengths_missing,

        // Unexpected params.
        unexpected_params,

        // Generic arguments redundance in function.
        function_generic_args_redundance,

        // Unexpected variable defination.
        unexpected_variable_defination,

        // Unexpected field defination.
        unexpected_field_defination,

        // Unexpected param defination.
        unexpected_param_defination,

        // Unexpected property defination.
        unexpected_property_defination,

        // Unexpected method defination.
        unexpected_method_defination,

        // Unexpected typedef decorate.
        unexpected_type_def_decorate,

        // Unexpected typedef attribute.
        unexpected_type_def_attribute,

        // A constant variable require a value to be provided.
        constant_variable_initialize_missing,

        // The expression beging assigned to constant variable must be constant.
        constant_variable_required_constant_value,

        // Unexpected using namespace.
        unexpected_using_namespace,

        // Unexpected import.
        unexpected_import,

        // A reference of itself.
        assembly_reference_itself,

        // Assembly circular reference.
        assembly_circular_reference,

        // Assembly not found.
        assembly_not_found,

        // Variable defination duplicated.
        variable_defination_duplicate,

        // Variable undefined.
        variable_undefined,

        // Type undefined.
        type_undefined,

        // Typedef unsolved.
        type_def_unsolved,

        // Type defination duplicated.
        type_defination_duplicate,

        // Typedef defination duplicated.
        type_def_defination_duplicate,

        // Type undetermind.
        type_undetermind,

        // Unexpected symbol.
        unexpected_symbol,

        // Member not found.
        member_not_found,

        // Member defination duplicated.
        member_defination_duplicated,

        // Member names cannot be the same as their enclosing type.
        member_same_as_enclosing_type,

        // Method not found.
        method_not_found,

        // Method not match.
        method_not_match,

        // Method conflict.
        method_conflict,

        // Not all code paths return a value.
        method_no_return,

        // The return keyword must not be followed by an expression in void method.
        method_unexpected_return_value,

        // The return keyword must followed by an expression in non-void method.
        method_return_value_missing,

        // Incompatible return type.
        method_incompatible_return_value,

        // Property index undetermind.
        property_index_undetermind,

        // Property method body uniformity.
        property_method_body_missing,

        // Property or indexer must have at least one accessor.
        property_accessor_missing,

        // Attribute constructor not match.
        attribute_constructor_not_match,

        // Type consturctor not matched.
        type_constructor_not_match,

        // Unexpected expression.
        unexpected_expression,

        // Invalid expression.
        invalid_expression,

        // Constructor method should no return type.
        constructor_method_should_no_return_type,

        // Destructor method should no return type.
        destructor_method_should_no_return_type,

        // Static constructor method should no return type.
        static_constructor_method_should_no_return_type,

        // Destructor method should no params.
        destructor_method_should_no_params,

        // Static constructor method should no params.
        static_constructor_method_should_no_params,

        // Constructor method not found.
        constructor_method_not_found,

        // Do not need a default constructor.
        do_not_need_default_constructor,

        // Partial specialization not supported.
        partial_specialization_not_supported,

        // No base type.
        no_base_type,

        // Circular base type dependency.
        circle_base_type_dependency,

        // Expected array length.
        expected_array_length,

        // Array initializer error.
        array_initializer_error,

        // Invalid initialize value.
        invalid_initialize_value,

        // Operator overload prototype error.
        operator_overload_prototype_error,

        // Unitary operator overload prototype error: Wrong param count.
        unitary_operator_overload_wrong_param_count,

        // Binary operator overload prototype error: Wrong param count.
        binary_operator_overload_wrong_param_count,

        // Operator overload prototype error: One of the parameters must be the containing type.
        unitary_operator_overload_wrong_containing_type,

        // Operator overload prototype error: The parameter must be the containing type.
        binary_operator_overload_wrong_containing_type,

        // Operator overload prototype error; User-defined operators cannot return void.
        operator_overload_cannot_return_void,

        // Operator cannot ber overloaded.
        operator_cannot_be_overloaded,

        // Operator overload not defined.
        operator_overload_not_defined,

        // Cannot determine local type.
        cannot_determine_local_variable_type,

        // Cannot find method with prototype.
        cannot_find_method_with_prototype,

        // Unexpected decorate on enum fields.
        unexpected_enum_decorate,

        // Unexpected type on enum fields.
        unexpected_enum_type,

        // Enum field prototype error,
        enum_field_prototype_error,

        // Expect constant value.
        expect_constant_value,

        // Unexpected underlying type.
        unexpected_underlying_type,

        // The enum value is too large
        enum_value_too_large,

        // The enum value is too small
        enum_value_too_small,

        // The evaluation of the constant value involves a circular definition.
        enum_field_circular_definition,

        // Member is inaccessible to its protection level.
        inaccessible_protection_level,

        // Cannot change access modifiers when overriding inherited member.
        access_modifier_changed,

        // Cannot change return types when override inherited members.
        return_type_changed,

        // Virtual or abstract members cannot be private
        virtual_member_cannot_be_private,

        // A static member cannot be marked as override, virtual, or abstract
        static_member_cannot_be_virtual,

        // A member marked as override cannot be marked as new or virtual
        override_member_cannot_be_new,

        // Member is abstract but it is contained in non-abstract type.
        abstract_member_on_non_abstract_type,

        // The modifier is not valid for this item.
        modifier_not_valid,

        // The modifier is not valid for interface members.
        modifier_not_valid_for_interface_members,

        // Extern members should be marked as static.
        extern_member_should_be_static,

        // Extern method cannot declare a body.
        extern_method_cannot_declare_body,

        // Abstract method cannot declare a body.
        abstract_method_cannot_declare_body,

        // Cannot be sealed because it is not an override member.
        sealed_member_but_not_override,

        // No suitable member found to override.
        no_suitable_member_to_override,

        // Cannot override inherited member because it is sealed.
        cannot_override_sealed_members,

        // Return type changed when override inherit members.
        override_member_return_type_changed,

        // Interface members cannot have defination.
        interface_members_cannot_have_defination,

        // Interfaces cannot contain instance fields.
        interfaces_cannot_contain_instance_fields,

        // Cannot implicitly convert a type to another type.
        invalid_type_cast,

        // The type arguments for method cannot be inferred from the usage.
        // Try specifying the type arguments explicitly.
        type_arguments_cannot_be_inferred,

        // The type 'string' cannot be used as type parameter 'T' in the
        //  generic type or method 'Class1<T>'. There is no implicit reference conversion
        //  from 'string' to 'Project1.Class2'.
        constraint_no_implicit_conversion,

        // The type 'string' must be a non-nullable value type
        //  in order to use it as parameter 'T' in the generic type or method 'Class1<T>'
        constraint_expect_value_type,

        // The type 'int' must be a reference type in order to
        //  use it as parameter 'T' in the generic type or method 'Class1<T>'
        constraint_expect_reference_type,

        // 'long' is not a valid constraint. A type used as a constraint
        //  must be an interface, a non-sealed class or a type parameter
        constraint_invalid_type,

        // The 'class', 'struct', 'unmanaged', 'notnull', and 'default' constraints
        //  cannot be combined or duplicated.
        constraint_combined_or_duplicated,

        // The 'class', 'struct', 'unmanaged', 'notnull', and 'default' constraints
        //  must be specified first in the constraints list.
        constraint_must_be_first,

        // The 'new()' constraint cannot be used with the 'struct' constraint
        constraint_new_cannot_be_used_with_struct,

        // 'Class2' must be a non-abstract type with a public parameterless constructor
        //  in order to use it as parameter 'T' in the generic type or method 'Class1<T>'
        constraint_expect_public_parameterless_constructor,

        // Duplicate constraint 'Class2' for type parameter 'T'
        constraint_duplicate_types,

        // At most one class type can be specified for type parameter 'T'.
        constraint_at_most_one_class_type,

        // Constraint cannot be special class 'object'
        constraint_cannot_be_special_type,

        // A constraint clause has already been specified for type parameter 'T'.
        // All of the constraints for a type parameter must be specified in a single where clause.
        constraint_duplicate_specified,

        // Class1<T>' does not define type parameter 'K'
        generic_parameter_not_defined,


        __the_end__         = 10000,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        typedef common_log_code_t __log_code_t, __c_t;

        template<size_t _child_count>
            using __array_node_base_t = array_ast_node_t<_child_count>;

        ////////// ////////// ////////// ////////// //////////

        // Node with specified child count.
        template<size_t _child_count, typename value_t, value_t _value>
        class __array_node_t : public __array_node_base_t<_child_count>
        {
        public:

            // Returns node value.
            virtual __value_t value() const override
            {
                return (__value_t)_value;
            }

            // Converts to a string.
            virtual const string_t to_string() const override
            {
                return _title(_value);
            }
        };

        ////////// ////////// ////////// ////////// //////////

        namespace
        {
            // Empty value of object.
            template<typename t>
            struct __empty_value_t
            {
                static bool is_empty(const t & v) { return v.empty(); }
            };

            // Empty pointer.
            template<typename t>
            struct __empty_value_t<t *>
            {
                static bool is_empty(const t * v) { return v == nullptr; }
            };

            // Returns whether it's an empty value.
            template<typename t> bool __is_empty_value(const t & v)
            {
                typedef std::remove_const_t<t> t0;
                return __empty_value_t<t0>::is_empty(v);
            }
        }

        ////////// ////////// ////////// ////////// //////////

        // Node utils.
        class __utils_t
        {
        public:

            // Constructor.
            __utils_t(ast_context_t & context) : __context(context) { }

        protected:
            ast_context_t & __context;

            // Returns compile context.
            compile_context_t & __compile_context() { return __context.compile_context; }

            // Returns global context.
            global_context_t & __global_context() { return __compile_context().global_context; }

            // Returns memory management.
            memory_t * __get_memory() const
            {
                return __context.compile_context.get_memory();
            }

            // Returns language.
            lang_t * __get_lang() const
            {
                return __context.lang;
            }

            // Creates new object.
            template<typename _t, typename ... _args_t>
            _t * __new_obj(_args_t && ... args)
            {
                return __context.compile_context.new_obj<_t>(std::forward<_args_t>(args) ...);
            }

            // Creates a new system expression.
            template<typename _exp_t, typename ... _args_t>
            system_expression_t<_exp_t> * __new_system_expression(_args_t && ... args)
            {
                return __new_obj<system_expression_t<_exp_t>>(std::forward<_args_t>(args) ...);
            }

            template<typename _exp_t, typename ... _args_t>
            __wexp_t<_exp_t> * __new_wexp(_args_t && ... args)
            {
                return __new_obj<__wexp_t<_exp_t>>(std::forward<_args_t>(args) ...);
            }

            // Creates a new constant expression.
            template<typename _value_t>
            system_expression_t<cvalue_expression_t> * __to_value_expression(_value_t value)
            {
                return __new_system_expression<cvalue_expression_t>(
                    __new_obj<cvalue_t>(value)
                );
            }

            // Creates a new expression statement.
            template<typename _exp_t, typename ... _args_t>
            statement_t * __new_expression_statement(_args_t && ... args)
            {
                return __new_obj<expression_statement_t>(
                    __new_system_expression<_exp_t>(std::forward<_args_t>(args) ...)
                );
            }

            // Creates a name expression for field.
            system_expression_t<name_expression_t> * __new_field_expression(field_t * field)
            {
                return __new_system_expression<name_expression_t>(
                    __new_obj<field_variable_t>(field)
                );
            }

            // Creates a name expression for param.
            system_expression_t<name_expression_t> * __new_param_expression(param_t * param)
            {
                return __new_system_expression<name_expression_t>(
                    __new_obj<param_variable_t>(param)
                );
            }

            // Creates a name expression for param.
            system_expression_t<name_expression_t> * __new_local_expression(
                                                                local_variable_t * variable)
            {
                return __new_system_expression<name_expression_t>(variable);
            }

            // Parses expressions.
            template<typename _it_t>
            expression_t * __parse_expression(_it_t begin, _it_t end)
            {
                return parse_expression(__get_memory(), __context.lang, begin, end);
            }

            // Parses expressions.
            template<typename _its_t>
            expression_t * __parse_expression(_its_t && its)
            {
                return __parse_expression(std::begin(its), std::end(its));
            }

            // Returns the xpool.
            xpool_t & __get_xpool()
            {
                return __context.compile_context.global_context.xpool;
            }

            // Returns the string pool.
            spool_t & __get_spool()
            {
                return __get_xpool().spool;
            }

            // Converts to string id.
            sid_t __to_sid(const string_t & s)
            {
                return __get_spool().to_sid(s);
            }

            // Converts to name_t.
            name_t __to_name(const string_t & s)
            {
                return name_t(this->__to_sid(s));
            }

            // Converts to name_t.
            name_t __to_name(token_t * token)
            {
                _A(token != nullptr);
                return __to_name(_str(token));
            }

            // Logs message.
            template<typename _code_element_t, typename ... _args_t>
            void __log(_code_element_t * element, _args_t && ... args)
            {
                ast_log(__context, element, std::forward<_args_t>(args) ...); 
            }

            // Validates name.
            bool __validate_name(const char_t * name, __el_t * el, const char_t * title = _T(""))
            {
                if (name == nullptr && name[0] == _T('\0'))
                {
                    __log(el, __c_t::name_empty, title);
                    return false;
                }

                const char_t c = name[0];
                if (!al::is_word(c) && c != _T('_'))
                {
                    __log(el, __c_t::name_unexpected_first_char, c, name);
                    return false;
                }

                for (const char_t * p = name + 1; *p; p++)
                {
                    if (!al::is_word_or_digit(*p) && *p != _T('_'))
                    {
                        __log(el, __c_t::name_unexpected_char, *p, name);
                        return false;
                    }
                }

                return true;
            }

            // Validates name.
            bool __validate_name(sid_t sid, __el_t * el, const char_t * title = _T(""))
            {
                return __validate_name(sid.c_str(), el, title);
            }

            // Validates name.
            bool __validate_name(const name_t & name, __el_t * el, const char_t * title = _T(""))
            {
                return __validate_name(name.sid, el, title);
            }

            // Assigns name.
            bool __assign_name(name_t & dst, const name_t & src, __el_t * el,
                                    const char_t * title = _T(""), bool check_duplicate = false)
            {
                if (!__validate_name(src, el, title))
                    return false;

                if (check_duplicate && dst != name_t::null)
                {
                    __log(el, __c_t::duplicate, _F(_T("name %1%"), title), src);
                    return false;
                }

                dst = src;
                return true;
            }

            // Assigns a value.
            template<typename dst_t, typename src_t>
            bool __assign(dst_t && dst, const src_t & src, __el_t * el,
                                    const char_t * title = _T(""), bool check_duplicate = false)
            {
                if (check_duplicate && dst != _D(src_t))
                {
                    __log(el, __c_t::duplicate, title, src);
                    return false;
                }

                dst = src;
                return true;
            }

            // Assigns a bit value.
            template<typename dst_t, typename src_t>
            bool __assign_bit(dst_t && dst, const src_t & src, __el_t * el,
                                    const char_t * title = _T(""), bool check_duplicate = false)
            {
                if (check_duplicate && bit_has_flag(dst, src))
                {
                    __log(el, __c_t::duplicate, title, src);
                    return false;
                }

                bit_add_flag(dst, src);
                return true;
            }

            // Checks whether it's empty.
            template<typename t, typename _log_code_t, typename ... _args_t>
            bool __check_empty(const t & value, __el_t * el, _log_code_t code, _args_t && ... args)
            {
                if (__is_empty_value(value))
                {
                    __log(el? el : (__el_t *)this, code, std::forward<_args_t>(args) ...);
                    return false;
                }

                return true;
            }

            // Check whether the name is empty.
            template<typename _name_t>
            bool __check_name_empty(const _name_t & name, const char_t * title)
            {
                return __check_empty(name, (__el_t *)this, __c_t::name_empty,
                                                    title ? title : _T(""));
            }

            // Combines names.
            const mname_t * __combine_name(mname_t * name1, mname_t * name2)
            {
                global_context_t & gctx = __global_context();
                return mname_t::combine(
                    to_mname_operate_context(), name1, name2
                );
            }

            // Converts to multi-name.
            const mname_t * __to_mname(const string_t & s)
            {
                global_context_t & gctx = __global_context();
                return mname_t::parse(
                    to_mname_operate_context(), s
                );
            }

            // Execute expression.
            cvalue_t __execute_expression(expression_t * exp)
            {
                _A(exp != nullptr);

                return exp->execute();
            }

            // Check whether has permission to access a member or type.
            template<typename _member_t>
            bool __check_access(ast_walk_context_t & wctx, _member_t * member)
            {
                _A(member != nullptr);

                type_t * current_type = wctx.current_type();
                if (current_type != nullptr)
                    return check_access(current_type, member);

                assembly_t * current_assembly = wctx.current_assembly();
                _A(current_assembly != nullptr);
                return check_access(current_assembly, member);
            }

            // Check whether has permission to access a member or type.
            template<typename _member_t, typename _element_t>
            bool __check_access(ast_walk_context_t & wctx, _member_t * member,
                                                           _element_t && element)
            {
                if (!__check_access(wctx, member))
                {
                    this->__log(element,
                        common_log_code_t::inaccessible_protection_level, element
                    );
                    return false;
                }

                return true;
            }

            // Check member duplicate or format correct.
            template<typename _code_element_t>
            bool __check_duplicate(type_t * type, member_t * member,
                                                _code_element_t * code_element);

            // Check member decorate corrected.
            template<typename _member_t, typename _code_element_t>
            bool __check_modifier(type_t * type, _member_t * member,
                                                _code_element_t * code_element);

            // Check member duplicate and decorate corrected.
            template<typename _member_t, typename _code_element_t>
            bool __check_member(type_t * type, _member_t * member,
                                                _code_element_t * code_element);
        };
    }

    ////////// ////////// ////////// ////////// //////////

    template<element_value_t value> class asts_node_t;

    #define __ArrayNodeT(t) __array_node_t<(size_t)t::__end__, decltype(t::value), t::value>

    // Node wrapper.
    template<typename node_data_t>
    class node_wrapper_t : public __ArrayNodeT(node_data_t)
                         , public node_data_t, public __utils_t
    {
        typedef decltype(node_data_t::__end__) __index_t;
        typedef __ArrayNodeT(node_data_t) __base_t;

    public:

        // Constructors.
        template<typename ... _args_t>
        node_wrapper_t(ast_context_t & context, _args_t && ... args)
            : __base_t(std::forward<_args_t>(args) ...), __utils_t(context)
        { }

        // Returns index.
        __node_t * child_at(__index_t index) const
        {
            return __base_t::child_at((size_t)index);
        }

        // Returns node at specified index.
        __node_t * operator[](__index_t index) const
        {
            return __base_t::child_at(index);
        }

        // Sets node at specified index.
        void set_child(__index_t index, __node_t * node)
        {
            __base_t::set_child((size_t)index, node);
            node_data_t::on_set_child((size_t)index, node);
        }

        // Sets child with check.
        template<typename code_t, typename ... _args_t>
        void set_child_with_check(__index_t index, __node_t * node,
                                    code_t duplicate_code, _args_t && ... args)
        {
            if (has_child(index))
                this->__log(node, duplicate_code, std::forward<_args_t>(args) ...);
            else
                this->set_child(index, node);
        }

        // Returns multipy node at specified index.
        multipy_ast_node_t * __multipy_ast_node_at(__index_t index)
        {
            typedef asts_node_t<(element_value_t)__xcvalue_t::asts> __asts_node_t;

            __node_t * child = child_at(index);
            if (child == nullptr)
            {
                child = __context.compile_context.new_obj<__asts_node_t>(__context);
                set_child(index, child);
            }

            return ((multipy_ast_node_t *)child);
        }

        // Appends a child.
        void append_child(__index_t index, __node_t * node)
        {
            __multipy_ast_node_at(index)->append(node);
        }

        // Returns child count.
        size_t child_count(__index_t index) const
        {
            __node_t * node = child_at(index);
            return node == nullptr? 0 : node->child_count();
        }

        // Returns node at index.
        __node_t * child_at(__index_t index, size_t child_index) const
        {
            __node_t * node = child_at(index);
            return node? node->child_at(child_index) : nullptr;
        }

        // Sets child at index.
        void set_child(__index_t index, size_t child_index, __node_t * node)
        {
            multipy_ast_node_t * nodes = __multipy_ast_node_at(index);
            while (nodes->child_count() <= child_index)
            {
                nodes->set_child(child_index, node);
            }
        }

        // Returns whether child is empty.
        bool child_empty(__index_t index) const
        {
            return child_count(index) == 0;
        }

        // Returns whether it has the child.
        bool has_child(__index_t index) const
        {
            return child_at(index) != nullptr;
        }

        // Walks each child for applying to specified function.
        template<typename node_t, typename f_t>
        void each_child_node(__index_t index, f_t f) const
        {
            __node_t * node = child_at(index);
            if (node != nullptr)
            {
                for (size_t index = 0, count = node->child_count(); index < count; index++)
                {
                    ast_node_t * child = node->child_at(index);
                    f(child != nullptr? _M(node_t *, child) : nullptr);
                }
            }
        }

        // Walks each child for applying to specifed function.
        template<typename etype_t, typename f_t>
        void each_child(__index_t index, f_t f) const
        {
            typedef teobject_ast_base_t<etype_t> node_t;

            this->each_child_node<node_t>(index, [this, &f](node_t * node) {
                __do_call_f<etype_t>(f, node);
            });
        }

        // Transform child and store to the output iterator.
        template<typename etype_t, typename output_itor_t>
        void transform_child(__index_t index, output_itor_t itor) const
        {
            each_child<etype_t>(index, [&itor](etype_t eobject) {
                *itor++ = eobject;
            });
        }

        // Transform child and store to the output iterator.
        template<typename container_t, typename etype_t = typename container_t::value_type>
        void transform_child_to(__index_t index, container_t & container) const
        {
            transform_child<typename container_t::value_type>(index, std::back_inserter(container));
        }

        // Transform child if matches the specified predicate function.
        //   and store to the output iterator.
        template<typename etype_t, typename output_itor_t, typename pred_t>
        void transform_child(__index_t index, output_itor_t itor, pred_t pred) const
        {
            each_child<etype_t>(index, [&itor, &pred](etype_t eobject) {
                pred(eobject) && (*itor++ = eobject, true);
            });
        }

        // Transform child if matches the specifed predicate function.
        //   and store to the output iterator.
        template<typename container_t, typename etype_t = typename container_t::value_type,
                 typename pred_t>
        void transform_child_to(__index_t index, container_t & container, pred_t pred) const
        {
            transform_child<typename container_t::value_type>(
                index, std::back_inserter(container), pred
            );
        }

        // Converts child to specified etype_t.
        template<typename etype_t>
        etype_t __to_eobject(__index_t index)
        {
            ast_node_t * child = this->child_at(index);
            if (child != nullptr)
                return _M(eobject_ast_t<etype_t> *, child)->to_eobject();
            return nullptr;
        }

        // Tries convert child to specified etype_t.
        // Returns nullptr if failed to convert.
        template<typename etype_t>
        etype_t __try_to_eobject(__index_t index)
        {
            typedef eobject_ast_t<etype_t> ast_t;
            ast_t * ast = as<ast_t *>(this->child_at(index));
            if (ast != nullptr)
                return ast->to_eobject();
            return nullptr;
        }

        // Tries convert child to specified etype_t with owner.
        // Returns nullptr if failed to convert.
        template<typename etype_t, typename owner_t>
        etype_t __to_eobject_with_owner(__index_t index, owner_t owner)
        {
            etype_t eobj = __to_eobject<etype_t>(index);
            if (eobj != nullptr)
                eobj->set_owner(owner);

            return eobj;
        }

        // Returns node value.
        virtual __value_t value() const override
        {
            return (__value_t)node_data_t::value;
        }

    protected:

        // Delaies to walk this node with specified step and tag.
        void __delay(ast_walk_context_t & context, walk_step_t step, void * tag = nullptr)
        {
            context.delay(this, (int)step, tag);
        }

    private:

        // Do call function with specified node.
        template<typename etype_t, typename f_t>
        void __do_call_f(f_t f, teobject_ast_base_t<etype_t> * node) const
        {
            if (node == nullptr)
            {
                f(nullptr);
                return;
            }

            switch (node->get_type())
            {
                case eobject_ast_type_t::eobject:
                    f(((eobject_ast_t<etype_t> *)node)->to_eobject());
                    break;

                case eobject_ast_type_t::eobjects: {
                    auto nodes = (eobjects_ast_t<etype_t> *)node;
                    for (size_t index = 0, count = nodes->eobject_count(); index < count; index++)
                    {
                        f(nodes->eobject_at(index));
                    }
                }   break;

                default: break;
            }
        }

    };

    ////////// ////////// ////////// ////////// //////////

    // Base class for node data.
    template<typename _self_t, typename value_t, value_t _value>
    struct node_data_base_t
    {
        static const value_t value = _value;

        // When setting child.
        void on_set_child(size_t index, __node_t * node) { }
    };

    ////////// ////////// ////////// ////////// //////////

    // Definations for defines a ast node.

    #define RawAstName(name)   __##name##_ast_node_t

    #define Ast(name, value)                                                            \
        namespace                                                                       \
        {                                                                               \
            struct __##name##_ast_node_data_t                                           \
                : public ::X_ROOT_NS::modules::compile::node_data_base_t<               \
                    __##name##_ast_node_data_t,                                         \
                    decltype(value), value>                                             \
            {                                                                           \
                enum {


    #define EndAst(name)                                                                \
                __end__ };                                                              \
            };                                                                          \
        }                                                                               \
                                                                                        \
        typedef node_wrapper_t<__##name##_ast_node_data_t> RawAstName(name);

    #define __EmptyAst(name, value)                                                     \
        Ast(name, value)                                                                \
        EndAst(name)

    
    ////////// ////////// ////////// ////////// //////////
    // module

    // Modules ast node.
    class __module_ast_node_t : public module_ast_node_t
    {
        typedef module_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

        // Walks this node.
        virtual bool do_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // mname

    __EmptyAst(mname, __xcvalue_t::mname)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Multi-name ast node.
    class mname_ast_node_t : public __mname_ast_node_t
                           , public mname_ast_t
    {
        typedef __mname_ast_node_t __super_t;

    public:
        template<typename ... _args_t>
        mname_ast_node_t(_args_t && ... args) : __super_t(std::forward<_args_t>(args) ...)
        {
            new ((void *)__mname) mname_t();
        }

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

        // Commits this node.
        virtual void on_commit() override;

        // Append part.
        void append_part(name_t name, __el_t * el);

        // Returns eobject.
        virtual emname_t * to_eobject() override;

    private:
        union
        {
            byte_t  __mname[sizeof(mname_t)];
            byte_t  __emname[sizeof(__w_t<emname_t>)];
        };

        // Returns this multi-name.
        mname_t *  __this_mname()  { return (mname_t *)__mname;   }

        // Returns this eobject.
        emname_t * __this_emname() { return (emname_t *)__emname; }
    };

    ////////// ////////// ////////// ////////// //////////
    // expressions

    Ast(expressions, __xcvalue_t::expressions)

        // Expressions.
        __expressions__,

    EndAst(expressions)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Expressions ast node.
    class expressions_ast_node_t : public __expressions_ast_node_t
                                 , public expression_ast_t
                                 , public expressions_t
    {
        typedef __expressions_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual expression_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_name

    Ast(type_name, __cvalue_t::type_name)

        // Underlying.
        __underlying__,

    EndAst(type_name)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Typename ast node.
    class type_name_ast_node_t : public __type_name_ast_node_t
                               , public type_name_ast_t
    {
        typedef __type_name_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual type_name_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // name_unit_t

    // Name unit.
    struct name_unit_t : public eobject_t
    {
        // Constructor.
        name_unit_t() = default;
        name_unit_t(const name_t & name, generic_args_t * args = nullptr)
            : name(name), args(args) { }

        typedef name_unit_t * itype_t;

        name_t           name    = name_t::null;    // Name
        generic_args_t * args    = nullptr;         // Arguments.

        // Returns whether it's equals to a name.
        bool operator == (const name_t & name) const
        {
            return !is_generic() && name == this->name;
        }

        // Returns whether it's equals to a sid.
        bool operator == (const sid_t & sid) const
        {
            return !is_generic() && (sid_t)this->name == sid;
        }

        // Returns whether it's not equals to a name.
        bool operator != (const name_t & name) const
        {
            return !operator == (name);
        }

        // Returns whether it's not equals to a sid.
        bool operator != (const sid_t & sid) const
        {
            return !operator == (sid);
        }

        // Returns generic argument at specified index.
        generic_arg_t * operator [] (size_t index) const
        {
            return args && index < args->size()? (*args)[index] : nullptr;
        }

        // Converts to a string.
        operator string_t() const;

        // Converts to a string.
        virtual const string_t to_string() const override { return (string_t)*this; }

        // Returns generic argument count.
        size_t generic_args_count() const { return args? args->size() : 0; }

        // Returns whether it's generic.
        bool is_generic() const { return args && !args->empty(); }

        // Returns whether it's partial specialization.
        bool is_partial_specialization() const;

        // Returns whether it's no specialization.
        bool is_no_specialization() const { return !args || al::all_of_empty(*args); }
    };

    typedef eobject_ast_t<typename name_unit_t::itype_t> name_unit_ast_t;

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Global types.
    X_ENUM(global_type_t)

        // Root.        e.g. System.Text
        root,

        // Global.      e.g. ::System.Text
        global,

    X_ENUM_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // General typename.
    class general_type_name_t : public type_name_t
    {
    public:
        typedef al::svector_t<name_unit_t *, 3> unit_list_t;
        typedef typename unit_list_t::iterator unit_iterator_t;

        // Typename units.
        unit_list_t     units;

        // Global type.
        global_type_t   global_type = global_type_t::__default__;

        // Returns begin iterator.
        auto begin() const { return units.begin(); }

        // Returns end iterator.
        auto end()   const { return units.end();   }

        // Returns whether it's empty.
        bool empty() const { return units.empty(); }

        // Returns whether it's equals to a string.
        template<size_t n> bool equals(const char_t (&s)[n])
        {
            name_unit_t * unit;

            return units.size() == 1 && (unit = units[0]) != nullptr
                && al::equals(((sid_t)unit->name).c_str(), s);
        }

        // Converts to a string.
        X_TO_STRING
    };

    //-------- ---------- ---------- ---------- ----------

    // Array type name.
    class array_type_name_t : public type_name_t
    {
    public:
        type_name_t * element_type_name = nullptr;
        al::svector_t<expression_t *, 2> dimensions;

        // Converts to a string.
        X_TO_STRING
    };

    ////////// ////////// ////////// ////////// //////////

    // Base class of typename ast node.
    template<typename _super_t>
    class type_name_ast_node_base_t : public _super_t
    {
        typedef _super_t __super_t;

    public:
        using __super_t::__super_t;

    protected:

        // Ascertain type.
        template<typename _type_name_t>
        type_t * __ascertain_type(ast_walk_context_t & wctx, _type_name_t * type_name)
        {
            try
            {
                type_t * type = ascertain_type(this->__context, wctx, type_name);

                #ifdef __TraceTypeNameAscertain

                string_t s_type_name = _str(type_name);
                if (!al::starts_with(s_type_name.c_str(), _T("System.")))
                {
                    _PF(_T("+type %1%: \n      %2% (%3%)"), _str(type_name),
                                                    _str(type), (void *)type);
                }

                #endif

                return type;
            }
            catch (const logic_error_t<ascertain_type_error_t> & e)
            {
                this->__log(e.code, type_name);
            }

            return nullptr;
        }

        using __super_t::__log;

        // Log message.
        template<typename _type_name_t>
        void __log(ascertain_type_error_t error_code, _type_name_t * type_name)
        {
            switch (error_code)
            {
                case ascertain_type_error_t::unknown_type:
                    this->__log(this, __c_t::type_undefined, type_name);
                    break;

                case ascertain_type_error_t::type_def_unsolved:
                    this->__log(this, __c_t::type_def_unsolved, type_name);
                    break;

                default: break;
            }
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // general_type_name

    // General type name.
    Ast(general_type_name, __xcvalue_t::general_type_name)

        // Units.
        __units__,

    EndAst(general_type_name)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // General type name ast node.
    class general_type_name_ast_node_t
            : public type_name_ast_node_base_t<__general_type_name_ast_node_t>
            , public type_name_ast_t
            , public __w_t<general_type_name_t>
    {
        typedef type_name_ast_node_base_t<__general_type_name_ast_node_t> __super_t;

    public:
        using __super_t::__super_t;

        // Set global type.
        void set_global_type(global_type_t global_type);

        // Set if it's a extends type.
        void set_extends(bool extends);

        // On commits it.
        virtual void on_commit() override;

        // Returns eobject.
        virtual type_name_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:

        // Walks confirm step.
        bool __walk_confirm(ast_walk_context_t & context);

        // Walks analysis step.
        bool __walk_analysis(ast_walk_context_t & context);
    };

    ////////// ////////// ////////// ////////// //////////

    // Typename unit.
    Ast(name_unit, __xcvalue_t::name_unit)

        // Generic arguments.
        __generic_args__,

    EndAst(name_unit)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Typename unit ast node.
    class name_unit_ast_node_t : public __name_unit_ast_node_t
                               , public name_unit_ast_t
                               , public __w_t<name_unit_t>
    {
        typedef __name_unit_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Sets name.
        void set_name(name_t name, __el_t * el);

        // Commits it.
        virtual void on_commit() override;

        // Returns eobject.
        virtual name_unit_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // array_type_name

    // Array typename.
    Ast(array_type_name, __xcvalue_t::array_type_name)

        // Array element typename.
        __element_type_name__,
        
        // Array dimensions.
        __dimensions__,

    EndAst(array_type_name)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Array typename ast node.
    class array_type_name_ast_node_t
            : public type_name_ast_node_base_t<__array_type_name_ast_node_t>
            , public type_name_ast_t
            , public __w_t<array_type_name_t>
    {
        typedef type_name_ast_node_base_t<__array_type_name_ast_node_t> __super_t;

    public:
        using __super_t::__super_t;

        // Commits it.
        virtual void on_commit() override;

        // Returns the eobject.
        virtual type_name_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:

        // Walks the confirm step.
        bool __walk_confirm(ast_walk_context_t & context);
    };

    ////////// ////////// ////////// ////////// //////////
    // uncertain_type_name

    __EmptyAst(uncertain_type_name, __xcvalue_t::uncertain_type_name)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Uncertain type name ast node.
    class uncertain_type_name_ast_node_t
            : public type_name_ast_node_base_t<__uncertain_type_name_ast_node_t>
            , public type_name_ast_t
    {
        typedef type_name_ast_node_base_t<__uncertain_type_name_ast_node_t> __super_t;

    public:
        using __super_t::__super_t;

        // Commit this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual type_name_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_def_param

    // Typedef param.
    Ast(type_def_param, __xcvalue_t::type_def_param)

    EndAst(type_def_param)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Typedef param ast node.
    class type_def_param_ast_node_t : public __type_def_param_ast_node_t
                                    , public type_def_param_ast_t
                                    , public __w_t<type_def_param_t>
    {
        typedef __type_def_param_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Sets name.
        void set_name(name_t name, __el_t * el);

        // Sets if it's a extends type.
        void set_extends(bool extends);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual type_def_param_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_def_params

    // Typedef params.
    Ast(type_def_params, __xcvalue_t::type_def_params)

        // Params.
        __params__,

    EndAst(type_def_params)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Typedef params ast node.
    class type_def_params_ast_node_t : public __type_def_params_ast_node_t
                                     , public type_def_params_ast_t
                                     , public type_def_params_t
    {
        typedef __type_def_params_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual type_def_params_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_def

    // Typedef.
    Ast(type_def, __xcvalue_t::type_def)

        // Attributes.
        __attributes__,
        
        // Decorate.
        __decorate__,

        // Typename
        __type_name__,
        
        // Params.
        __params__,

    EndAst(type_def)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Typedef ast node.
    class type_def_ast_node_t : public __type_def_ast_node_t
                              , public type_def_ast_t
                              , public type_def_t
    {
        typedef __type_def_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Sets name.
        void set_name(name_t name, __el_t * el);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual type_def_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __el_t *    __name_el = nullptr;

        // Walks default step.
        bool __walk_default(ast_walk_context_t & context, int step, void * tag);

        // Walks analysis step.
        bool __walk_analysis(ast_walk_context_t & context);
    };

    ////////// ////////// ////////// ////////// //////////
    // type_of

    // Typeof.
    Ast(type_of, __xcvalue_t::type_of)

        // Typename.
        __type_name__,

    EndAst(type_of)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Typeof ast node.
    class type_of_ast_node_t : public __type_of_ast_node_t
                             , public expression_ast_t
                             , public __wexp_t<type_of_expression_t>
    {
        typedef __type_of_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual expression_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // argument

    // Argument.
    Ast(argument, __xcvalue_t::argument)

        // Expression
        __expression__,
        
        // Attributes.
        __attributes__,

    EndAst(argument)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Argument ast node.
    class argument_ast_node_t : public __argument_ast_node_t
                              , public argument_ast_t
                              , public argument_t
    {
        typedef __argument_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Sets name.
        void set_name(name_t name, __el_t * el);

        // Sets argument type.
        void set_argument_type(param_type_t atype, __el_t * el);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual argument_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // arguments

    // Arguments.
    Ast(arguments, __xcvalue_t::arguments)

        // Arguments.
        __arguments__,

    EndAst(arguments)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Arguments ast node.
    class arguments_ast_node_t : public __arguments_ast_node_t
                               , public arguments_ast_t
                               , public arguments_t
    {
        typedef __arguments_ast_node_t __super_t;

    public:
        using __arguments_ast_node_t::__arguments_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

        // Returns this eobject.
        virtual arguments_t * to_eobject() override;
    };

    ////////// ////////// ////////// ////////// //////////
    // param

    // Param.
    Ast(param, __xcvalue_t::param)

        // Typename
        __type_name__,
        
        // Attributes.
        __attributes__,
        
        // Default value.
        __default_value__,

    EndAst(param)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Param ast node.
    class param_ast_node_t : public __param_ast_node_t
                           , public param_ast_t
                           , public param_t
    {
        typedef __param_ast_node_t __super_t;

    public:
        using __param_ast_node_t::__param_ast_node_t;

        // Sets name.
        void set_name(const name_t & name, __el_t * el);

        // Sets param type.
        void set_param_type(param_type_t ptype, __el_t * el);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual param_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // params

    // Params.
    Ast(params, __xcvalue_t::params)

        // Params.
        __params__,

    EndAst(params)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Params ast node.
    class params_ast_node_t : public __params_ast_node_t
                            , public params_ast_t
                            , public params_t
    {
        typedef __params_ast_node_t __super_t;

    public:
        using __params_ast_node_t::__params_ast_node_t;

        // Commits it.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual params_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // generic_param

    // Generic param.
    Ast(generic_param, __xcvalue_t::generic_param)

        // Attributes.
        __attributes__,

    EndAst(generic_param)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Generic param ast node.
    class generic_param_ast_node_t : public __generic_param_ast_node_t
                                   , public generic_param_ast_t
                                   , public generic_param_t
    {
        typedef __generic_param_ast_node_t __super_t;

    public:
        using __generic_param_ast_node_t::__generic_param_ast_node_t;

        // Sets name.
        void set_name(name_t name, __el_t * el);

        // Sets param type.
        void set_param_type(generic_param_type_t type, __el_t * el);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this node.
        virtual generic_param_t * to_eobject() override;

        // Wallks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // generic_params

    // Generic params.
    Ast(generic_params, __xcvalue_t::generic_params)

        // Params.
        __params__,

    EndAst(generic_params)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Generic params ast node.
    class generic_params_ast_node_t : public __generic_params_ast_node_t
                                    , public generic_params_ast_t
                                    , public generic_params_t
    {
        typedef __generic_params_ast_node_t __super_t;

    public:
        using __generic_params_ast_node_t::__generic_params_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual generic_params_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };
   
    ////////// ////////// ////////// ////////// //////////
    // generic_constraint

    // Generic constraint.
    Ast(generic_constraint, __xcvalue_t::generic_constraint)

        // Typenames.
        __type_names__,

    EndAst(generic_constraint)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Generic constraint ast node.
    class generic_constraint_ast_node_t : public __generic_constraint_ast_node_t
                                        , public generic_constraint_ast_t
                                        , public generic_constraint_t
    {
        typedef __generic_constraint_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Sets constraint.
        void set_constraint(generic_constraint_ttype_t cttype, __el_t * el);

        // Sets name.
        void set_name(name_t name, __el_t * el);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual generic_constraint_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // generic_constraints

    // Generic constraints.
    Ast(generic_constraints, __xcvalue_t::generic_constraints)

        // Constraints.
        __constraints__,

    EndAst(generic_constraints)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Generic constraints ast node.
    class generic_constraints_ast_node_t : public __generic_constraints_ast_node_t
                                         , public generic_constraints_ast_t
                                         , public generic_constraints_t
    {
        typedef __generic_constraints_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits it.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual generic_constraints_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // generic_arg

    // Generic argument.
    Ast(generic_arg, __xcvalue_t::generic_arg)

        // Typename.
        __type_name__,

    EndAst(generic_arg)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Generic argument ast node.
    class generic_arg_ast_node_t : public __generic_arg_ast_node_t
                                 , public generic_arg_ast_t
                                 , public generic_arg_t
    {
        typedef __generic_arg_ast_node_t __super_t;

    public:
        using __generic_arg_ast_node_t::__generic_arg_ast_node_t;

        // Set param type, ref, out, params ...
        void set_atype(generic_arg_type_t atype);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual generic_arg_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // generic_args

    // Generic arguments.
    Ast(generic_args, __xcvalue_t::generic_args)

        // Generic arguments.
        __generic_args__,

    EndAst(generic_args)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Generic arguments ast node.
    class generic_args_ast_node_t : public __generic_args_ast_node_t
                                  , public generic_args_ast_t
                                  , public generic_args_t
    {
        typedef __generic_args_ast_node_t __super_t;

    public:
        using __generic_args_ast_node_t::__generic_args_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual generic_args_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // attribute

    // Attribute.
    Ast(attribute, __cvalue_t::attribute)

        // Typename
        __type_name__,
        
        // Arguments.
        __arguments__,

    EndAst(attribute)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Attribute ast node.
    class attribute_ast_node_t : public __attribute_ast_node_t
                               , public attribute_ast_t
                               , public attribute_t
    {
        typedef __attribute_ast_node_t __super_t;

    public:
        using __attribute_ast_node_t::__attribute_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual attribute_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        // Returns compile time attribute.
        attribute_t * __get_compile_time_attribute();

        // Analyze attribute constructor.
        bool __analyze_attribute_constructor();
    };

    ////////// ////////// ////////// ////////// //////////
    // attributes

    // Attributes.
    Ast(attributes, __cvalue_t::attributes)

        // Attributes.
        __attributes__,

    EndAst(attributes)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Attributes ast node.
    class attributes_ast_node_t : public __attributes_ast_node_t
                                , public attributes_ast_t
                                , public attributes_t
    {
        typedef __attributes_ast_node_t __super_t;

    public:
        using __attributes_ast_node_t::__attributes_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual attributes_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // cvalue

    __EmptyAst(cvalue, __cvalue_t::cvalue)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Constant ast node.
    class cvalue_ast_node_t : public __cvalue_ast_node_t
                            , public cvalue_ast_t
                            , public cvalue_t
    {
        typedef __cvalue_ast_node_t __super_t;

    public:
        using __cvalue_ast_node_t::__cvalue_ast_node_t; 

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

        // Returns this eobject.
        virtual cvalue_t * to_eobject() override;

        // Sets value.
        void set_value(const cvalue_t & value);

        // Sets value.
        void set_value(const token_data_t & data);
    };

    ////////// ////////// ////////// ////////// //////////
    // decorate

    __EmptyAst(decorate, __xcvalue_t::decorate)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Decorate ast node.
    class decorate_ast_node_t : public __decorate_ast_node_t
                              , public decorate_ast_t
                              , public decorate_t
    {
        typedef __decorate_ast_node_t __super_t;

    public:

        using __decorate_ast_node_t::__decorate_ast_node_t;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

        // Set access.
        void set_access(access_value_t value, __el_t * el);

        // Get access.
        access_value_t get_access();

        // Sets static decorate.
        void set_static(__el_t * el);

        // Sets sealed decorate.
        void set_sealed(__el_t * el);

        // Sets override decorate.
        void set_override(__el_t * el);

        // Sets virtual decorate.
        void set_virtual(__el_t * el);

        // Sets abstract decorate.
        void set_abstract(__el_t * el);

        // Sets new decorate.
        void set_new(__el_t * el);

        // Sets readonly decorate.
        void set_readonly(__el_t * el);

        // Sets const decorate.
        void set_const(__el_t * el);

        // Sets extern decorate.
        void set_extern(__el_t * el);

        // Get decorate value.
        decorate_value_t get_decorate();

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual decorate_t * to_eobject() override;
    };

    ////////// ////////// ////////// ////////// //////////
    // statements

    // Statements.
    Ast(statements, __xcvalue_t::statements)

        // Statements.
        __statements__,

    EndAst(statements)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Statements ast node.
    class statements_ast_node_t : public __statements_ast_node_t
                                , public statements_ast_t
                                , public statements_t
    {
        typedef __statements_ast_node_t __super_t;

    public:
        using __statements_ast_node_t::__statements_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statements_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // method_body

    // Method body.
    Ast(method_body, __xcvalue_t::method_body)

        // Statements.
        __statements__,

    EndAst(method_body)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Method body ast node.
    class method_body_ast_node_t : public __method_body_ast_node_t
                                 , public method_body_ast_t
                                 , public method_body_t
    {
        typedef __method_body_ast_node_t __super_t;

    public:
        using __method_body_ast_node_t::__method_body_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual method_body_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        // Walks post analysis step.
        bool __walk_post_analysis(ast_walk_context_t & context);
    };

    ////////// ////////// ////////// ////////// //////////
    // expression

    // Expression.
    Ast(expression, __cvalue_t::expression)

        // Nodes.
        __nodes__,

    EndAst(expression)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Expression ast node.
    class expression_ast_node_t : public __expression_ast_node_t
                                , public expression_ast_t
    {
        typedef analyze_stack_element_t     __el_t;
        typedef al::svector_t<__el_t, 3>    __es_t;
        typedef expression_ast_node_t       __self_t;
        typedef __expression_ast_node_t     __super_t;

    public:
        using __expression_ast_node_t::__expression_ast_node_t;

        // Appends token.
        void append_token(token_t * token);

        // Appends name.
        void append_name(name_t name, token_t * token);

        // Appends name unit.
        void append_name_unit(name_unit_ast_node_t * node);

        // Appends expression.
        void append_expression(ast_node_t * node);

        // Appends constant value.
        void append_cvalue(ast_node_t * node);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual expression_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __es_t __es;
        expression_t * __expression = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////
    // statement

    // Statement.
    Ast(statement, __cvalue_t::statement)

        // Underlying statement.
        __underlying__,

    EndAst(statement)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Statement ast node.
    class statement_ast_node_t : public __statement_ast_node_t
                               , public statement_ast_t
    {
        typedef __statement_ast_node_t __super_t;

    public:
        using __statement_ast_node_t::__statement_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // import

    // Import
    Ast(import, __cvalue_t::import)

        // Package.
        __package__,
        
        // Assembly.
        __assembly__,
        
    EndAst(import)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Import ast node.
    class import_ast_node_t : public __import_ast_node_t
                            , public import_ast_t
                            , public import_t
    {
        typedef __import_ast_node_t __super_t;

    public:
        using __import_ast_node_t::__import_ast_node_t;

        // Set alias name.
        void set_alias(name_t name, __el_t * el);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual import_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // using

    // Using
    Ast(using, __cvalue_t::using_)

        // Namespace.
        __ns__,
        
    EndAst(using)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Using ast node.
    class using_ast_node_t : public __using_ast_node_t
                           , public using_namespace_ast_t
                           , public using_namespace_t
    {
        typedef __using_ast_node_t __super_t;

    public:
        using __using_ast_node_t::__using_ast_node_t;

        // Set alias.
        void set_alias(name_t name, __el_t * el);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual using_namespace_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // namespace

    // Namespace
    Ast(namespace, __xcvalue_t::namespace_)

        // Name.
        __name__,
        
        // Typedefs.
        __type_defs__,
        
        // Types.
        __types__,
        
        // Namespaces.
        __namespaces__, 
        
    EndAst(namespace)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Namespace ast node.
    class namespace_ast_node_t : public __namespace_ast_node_t
                               , public namespace_ast_t
                               , public namespace_t
    {
        typedef __namespace_ast_node_t __super_t;

    public:
        using __namespace_ast_node_t::__namespace_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual namespace_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // type

    // Type
    Ast(type, __cvalue_t::type)

        // Attributes.
        __attributes__,
        
        // Decorate.
        __decorate__,
        
        // Generic param.
        __generic_params__,
        
        // Super typenames.
        __super_type_names__,

        // Typedefs.
        __type_defs__,
        
        // Fields.
        __fields__,
        
        // Methods.
        __methods__,
        
        // Properties.
        __properties__,
        
        // Events.
        __events__,
        
        // Nest types.
        __nest_types__,

        // Generic constraints,
        __generic_constraints__,
        
    EndAst(type)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Type ast node.
    class type_ast_node_t : public __type_ast_node_t
                          , public type_ast_t
                          , public __w_t<general_type_t>
    {
        typedef __type_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Sets name.
        void set_name(const name_t & name, __el_t * el);

        // Sets ttype.
        void set_ttype(ttype_t ttype, __el_t * el);

        // Appends member.
        void append_member(member_t * member);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        general_type_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        class __fields_init_stub_t;
        template<vtype_t _vtype> class __enum_fields_init_stub_t;
        class __general_fields_init_stub_t;

        __el_t * __name_el = nullptr;

        // Walks default step.
        bool __walk_default(ast_walk_context_t & context, int step, void * tag);

        // Appends default constructors.
        method_t * __append_default_constructor(ast_walk_context_t & context);

        // Walks analysis step.
        bool __walk_analysis(ast_walk_context_t & context, void * tag);

        // Walks init fields.
        bool __walk_analysis_init_fields(ast_walk_context_t & context);

        // Check circle base types.
        type_t * __check_circle_base_types();

        // Check circle base types.
        bool __check_circle_base_types(std::set<type_t *> & types, type_t * super_type);
    };

    ////////// ////////// ////////// ////////// //////////
    // expression_st

    // Expression.
    Ast(expression_st, __xcvalue_t::expression_st)

        // Expression.
        __expression__,
        
    EndAst(expression_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Expression statement ast node.
    class expression_st_ast_node_t : public __expression_st_ast_node_t
                                   , public statement_ast_t
                                   , public __w_t<expression_statement_t>
    {
        typedef __expression_st_ast_node_t __super_t;

    public:
        using __expression_st_ast_node_t::__expression_st_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_def_st

    // Typedef
    Ast(type_def_st, __xcvalue_t::type_def_st)

        // Typedef.
        __type_def__,
        
    EndAst(type_def_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Typedef statement ast node.
    class type_def_st_ast_node_t : public __type_def_st_ast_node_t
                                 , public statement_ast_t
                                 , public __w_t<type_def_statement_t>
    {
        typedef __type_def_st_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // defination_st

    // Defination statement.
    Ast(defination_st, __xcvalue_t::defination_st)

        // Typename.
        __type_name__,
        
        // Items.
        __items__,

    EndAst(defination_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Defination statement ast node.
    class defination_st_ast_node_t : public __defination_st_ast_node_t
                                   , public statement_ast_t
                                   , public __w_t<defination_statement_t>
    {
        typedef __defination_st_ast_node_t __super_t;

    public:
        using __defination_st_ast_node_t::__defination_st_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

        // Set constant.
        void set_constant(bool constant);

    private:
        // Walks default step.
        bool __walk_default(ast_walk_context_t & context);

        // Walks analysis step.
        bool __walk_analysis(ast_walk_context_t & context);
    };

    ////////// ////////// ////////// ////////// //////////
    // break_st

    __EmptyAst(break_st, __xcvalue_t::break_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Break statements ast node.
    class break_st_ast_node_t : public __break_st_ast_node_t
                              , public statement_ast_t
                              , public __w_t<break_statement_t>
    {
        typedef __break_st_ast_node_t __super_t;

    public:
        using __break_st_ast_node_t::__break_st_ast_node_t;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // continue_st

    // Continue statement.
    __EmptyAst(continue_st, __xcvalue_t::continue_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Continue statement ast node.
    class continue_st_ast_node_t : public __continue_st_ast_node_t
                                 , public statement_ast_t
                                 , public __w_t<continue_statement_t>
    {
        typedef __continue_st_ast_node_t __super_t;

    public:
        using __continue_st_ast_node_t::__continue_st_ast_node_t;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // throw_st

    // Throw statement.
    Ast(throw_st, __xcvalue_t::throw_st)

        // Expression.
        __expression__,

    EndAst(throw_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Throw statement ast node.
    class throw_st_ast_node_t : public __throw_st_ast_node_t
                              , public statement_ast_t
                              , public __w_t<throw_statement_t>
    {
        typedef __throw_st_ast_node_t __super_t;

    public:
        using __throw_st_ast_node_t::__throw_st_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // goto_st

    // Goto statement.
    __EmptyAst(goto_st, __xcvalue_t::goto_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Goto statement ast node.
    class goto_st_ast_node_t : public __goto_st_ast_node_t
                             , public statement_ast_t
                             , public __w_t<goto_statement_t>
    {
        typedef __goto_st_ast_node_t __super_t;

    public:
        using __goto_st_ast_node_t::__goto_st_ast_node_t;

        // Sets label.
        void set_label(const name_t & label, __el_t * el);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // return_st

    // Return statement.
    Ast(return_st, __xcvalue_t::return_st)

        // Expression.
        __expression__,

    EndAst(return_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Return statement ast node.
    class return_st_ast_node_t : public __return_st_ast_node_t
                               , public statement_ast_t
                               , public __w_t<return_statement_t>
    {
        typedef __return_st_ast_node_t __super_t;

    public:
        using __return_st_ast_node_t::__return_st_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        // Walks post analysis step.
        bool __walk_post_analysis(ast_walk_context_t & context);
    };

    ////////// ////////// ////////// ////////// //////////
    // do_while_st

    Ast(do_while_st, __xcvalue_t::do_while_st)

        // Condition
        __condition__,
        
        // Body
        __body__,

    EndAst(do_while_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Do ... while statement ast node.
    class do_while_st_ast_node_t : public __do_while_st_ast_node_t
                                 , public statement_ast_t
                                 , public __w_t<do_while_statement_t>
    {
        typedef __do_while_st_ast_node_t __super_t;

    public:
        using __do_while_st_ast_node_t::__do_while_st_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // loop_until_st

    // Loop ... unitl statement ast node.
    Ast(loop_until_st, __xcvalue_t::loop_until_st)

        // Condition.
        __condition__,
        
        // Body.
        __body__,

    EndAst(loop_until_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Loop ... until statement ast node.
    class loop_until_st_ast_node_t : public __loop_until_st_ast_node_t
                                   , public statement_ast_t
                                   , public __w_t<loop_until_statement_t>
    {
        typedef __loop_until_st_ast_node_t __super_t;

    public:
        using __loop_until_st_ast_node_t::__loop_until_st_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // while_st

    // While statement.
    Ast(while_st, __xcvalue_t::while_st)

        // Condition.
        __condition__,
        
        // Body.
        __body__,

    EndAst(while_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // While statement ast node.
    class while_st_ast_node_t : public __while_st_ast_node_t
                              , public statement_ast_t
                              , public __w_t<while_statement_t>
    {
        typedef __while_st_ast_node_t __super_t;

    public:
        using __while_st_ast_node_t::__while_st_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // for_st

    // For statement.
    Ast(for_st, __xcvalue_t::for_st)

        // Initialize
        __initialize__,
        
        // Condition.
        __condition__,
        
        // Increase.
        __increase__,
        
        // Body.
        __body__,

    EndAst(for_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // For statement ast node.
    class for_st_ast_node_t : public __for_st_ast_node_t
                            , public statement_ast_t
                            , public __w_t<for_statement_t>
    {
        typedef __for_st_ast_node_t __super_t;

    public:
        using __for_st_ast_node_t::__for_st_ast_node_t;

        // Commits it.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // for_each_st

    Ast(for_each_st, __xcvalue_t::for_each_st)

        // Typename
        __type_name__,
        
        // Iterator expression.
        __iterator__,
        
        // Body.
        __body__,

    EndAst(for_each_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // For...each statement ast node.
    class for_each_st_ast_node_t : public __for_each_st_ast_node_t
                                 , public statement_ast_t
                                 , public __w_t<for_each_statement_t>
    {
        typedef __for_each_st_ast_node_t __super_t;

    public:
        using __for_each_st_ast_node_t::__for_each_st_ast_node_t;

        // Sets variable.
        void set_variable(name_t name, __el_t * el);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // if_st

    // If statement.
    Ast(if_st, __xcvalue_t::if_st)

        // Condition.
        __condition__,
        
        // If body.
        __if_body__,
        
        // Else body.
        __else_body__,

    EndAst(if_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // If statement ast node.
    class if_st_ast_node_t : public __if_st_ast_node_t
                           , public statement_ast_t
                           , public __w_t<if_statement_t>
    {
        typedef __if_st_ast_node_t __super_t;

    public:
        using __if_st_ast_node_t::__if_st_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // switch_st

    // Switch statement.
    Ast(switch_st, __xcvalue_t::switch_st)

        // Expression.
        __expression__,
        
        // Cases.
        __cases__,

    EndAst(switch_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Switch statement ast node.
    class switch_st_ast_node_t : public __switch_st_ast_node_t
                               , public statement_ast_t
                               , public __w_t<switch_statement_t>
    {
        typedef __switch_st_ast_node_t __super_t;

    public:
        using __switch_st_ast_node_t::__switch_st_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // case

    // Case
    Ast(case, __xcvalue_t::case_)

        // Constants.
        __constants__,
        
        // Statements.
        __statements__,

    EndAst(case)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Case ast node.
    class case_ast_node_t : public __case_ast_node_t
                          , public case_ast_t
                          , public __w_t<case_t>
    {
        typedef __case_ast_node_t __super_t;

    public:
        using __case_ast_node_t::__case_ast_node_t;

        // Appends default expression.
        void append_default(__el_t * el);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual case_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // try_st

    // Try statement.
    Ast(try_st, __xcvalue_t::try_st)

        // Try.
        __try__,
        
        // Catches.
        __catches__,
        
        // Finally.
        __finally__,
        
    EndAst(try_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Try statement ast node.
    class try_st_ast_node_t : public __try_st_ast_node_t
                            , public statement_ast_t
                            , public __w_t<try_statement_t>
    {
        typedef __try_st_ast_node_t __super_t;

    public:
        using __try_st_ast_node_t::__try_st_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walk this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // catch

    // Catch
    Ast(catch, __xcvalue_t::catch_)

        // Typename.
        __type_name__,
        
        // Body.
        __body__,
        
    EndAst(catch)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Catch ast node.
    class catch_ast_node_t : public __catch_ast_node_t
                           , public catch_ast_t
                           , public __w_t<catch_t>
    {
        typedef __catch_ast_node_t __super_t;

    public:
        using __catch_ast_node_t::__catch_ast_node_t;

        // Sets variable.
        void set_variable(name_t variable, __el_t * el);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual catch_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __el_t * __variable_el;
    };

    ////////// ////////// ////////// ////////// //////////
    // empty_st

    // Empty
    __EmptyAst(empty_st, __xcvalue_t::empty_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Empty statement ast node.
    class empty_st_ast_node_t : public __empty_st_ast_node_t
                              , public statement_ast_t
                              , public __w_t<empty_statement_t>
    {
        typedef __empty_st_ast_node_t __super_t;

    public:
        using __empty_st_ast_node_t::__empty_st_ast_node_t;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // statement_group_st

    // Statement group statement.
    Ast(statement_group_st, __xcvalue_t::statement_group_st)

        // Statements.
        __statements__,

    EndAst(statement_group_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Statement group statement ast node.
    class statement_group_st_ast_node_t : public __statement_group_st_ast_node_t
                                        , public statement_ast_t
                                        , public __w_t<statement_group_t>
    {
        typedef __statement_group_st_ast_node_t __super_t;

    public:
        using __statement_group_st_ast_node_t::__statement_group_st_ast_node_t;

        // Commits it.
        virtual void on_commit() override;

        // Returns this statement.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_name_exp

    // Typename expression.
    Ast(type_name_exp, __xcvalue_t::type_name_exp)

        // Typename.
        __type_name__,

    EndAst(type_name_exp)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Typename expression ast node.
    class type_name_exp_ast_node_t : public __type_name_exp_ast_node_t
                                   , public expression_ast_t
                                   , public __wexp_t<type_name_expression_t>
    {
        typedef __type_name_exp_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual expression_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_cast_exp 

    // Type cast expression.
    Ast(type_cast_exp, __xcvalue_t::type_cast_exp)

        // Typename
        __type_name__,
        
        // Expression.
        __expression__,

    EndAst(type_cast_exp)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Type cast expression ast node.
    class type_cast_exp_ast_node_t : public __type_cast_exp_ast_node_t
                                   , public expression_ast_t
                                   , public __wexp_t<type_cast_expression_t>
    {
        typedef __type_cast_exp_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual expression_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // function

    // Function
    Ast(function, __xcvalue_t::function)

        // Namex or an expression.
        __namex__,
        
        // Generic arguments.
        __generic_args__,
        
        // Arguments.
        __arguments__,

    EndAst(function)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Functions ast node.
    class function_ast_node_t : public __function_ast_node_t
                              , public expression_ast_t
                              , public __wexp_t<function_expression_t>
    {
        typedef __function_ast_node_t __super_t;

    public:
        using __function_ast_node_t::__function_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual expression_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:

        // Walks default step.
        bool __walk_default(ast_walk_context_t & context);

        // Walks analysis step.
        bool __walk_analysis(ast_walk_context_t & context);
    };

    ////////// ////////// ////////// ////////// //////////
    // index

    // Index.
    Ast(index, __xcvalue_t::index)

        // Name or an expression.
        __namex__,
        
        // Arguments.
        __arguments__,

    EndAst(index)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Index ast node.
    class index_ast_node_t : public __index_ast_node_t
                           , public expression_ast_t
                           , public __wexp_t<index_expression_t>
    {
        typedef __index_ast_node_t __super_t;

    public:
        using __index_ast_node_t::__index_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns thid eobject.
        virtual expression_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // new

    // New
    Ast(new, __xcvalue_t::new_)

        // Typename
        __type_name__,
        
        // Arguments.
        __arguments__,

    EndAst(new)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // New ast node.
    class new_ast_node_t : public __new_ast_node_t
                         , public expression_ast_t
                         , public __wexp_t<new_expression_t>
    {
        typedef __new_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual expression_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // new_array

    // New array.
    Ast(new_array, __xcvalue_t::new_array)

        // Typename
        __type_name__,
        
        // Lengths.
        __lengths__,
        
        // Initializer.
        __initializer__,

    EndAst(new_array)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // New array ast node.
    class new_array_ast_node_t : public __new_array_ast_node_t
                               , public expression_ast_t
                               , public __wexp_t<new_array_expression_t>
    {
        typedef __new_array_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual expression_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        array_length_t __get_length(dimension_t dimension);

        // Walks default step.
        bool __walk_default(ast_walk_context_t & context);

        // Walks confirm step.
        bool __walk_confirm(ast_walk_context_t & context);

        // Walks analysis step.
        bool __walk_analysis(ast_walk_context_t & context);
    };

    ////////// ////////// ////////// ////////// //////////
    // array_initializer

    // Array initializer.
    Ast(array_initializer, __xcvalue_t::array_initializer)

        // Elements.
        __elements__,

    EndAst(array_initializer)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Array initializer ast node.
    class array_initializer_ast_node_t : public __array_initializer_ast_node_t
                                       , public array_initializer_ast_t
                                       , public array_initializer_t
    {
        typedef __array_initializer_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual array_initializer_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // array_lengths

    // Array lengths.
    Ast(array_lengths, __xcvalue_t::array_lengths)

        // Lengths.
        __lengths__,

    EndAst(array_lengths)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Array lengths ast node.
    class array_lengths_ast_node_t : public __array_lengths_ast_node_t
                                   , public array_lengths_ast_t
                                   , public array_lengths_t
    {
        typedef __array_lengths_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commit this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual array_lengths_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };


    ////////// ////////// ////////// ////////// //////////
    // default_value

    // Default value.
    Ast(default_value, __xcvalue_t::default_value)

        // Typename.
        __type_name__,

    EndAst(default_value)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Default value ast node.
    class default_value_ast_node_t : public __default_value_ast_node_t
                                   , public expression_ast_t
                                   , public __wexp_t<default_value_expression_t>
    {
        typedef __default_value_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual expression_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // field

    // Field.
    Ast(field, __cvalue_t::field)

        // Attributes.
        __attributes__,
        
        // Decorate.
        __decorate__,
        
        // Typename
        __type_name__,
        
        // Initialize value.
        __init_value__,
        
    EndAst(field)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Field ast node.
    class field_ast_node_t : public __field_ast_node_t
                           , public field_ast_t
                           , public __w_t<field_t>
    {
        typedef __field_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Sets name.
        void set_name(name_t name, __el_t * el);

        // Commit this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual field_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __el_t *        __name_el = nullptr;

        // Walks default step.
        bool __walk_default(ast_walk_context_t & context, int step, void * tag);

        // Walks analysis step.
        bool __walk_analysis(ast_walk_context_t & context);
    };

    ////////// ////////// ////////// ////////// //////////
    // method

    // Method ast node.
    Ast(method, __cvalue_t::method)

        // Attributes.
        __attributes__,
        
        // Decorate.
        __decorate__,
        
        // Owner type name.
        __owner_type_name__,
        
        // Typename
        __type_name__,

        // Conversion typename.
        __conversion_type_name__,
        
        // Generic params.
        __generic_params__,
        
        // Params.
        __params__,

        // Base constructor arguments.
        __base_ctor_args__,
        
        // Body.
        __body__,

        // Generic constraints,
        __generic_constraints__,

    EndAst(method)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Method ast node.
    class method_ast_node_t : public __method_ast_node_t
                            , public method_ast_t
                            , public __w_t<method_t>
    {
        typedef __method_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Sets name.
        void set_name(name_t name, __el_t * el);

        // Sets operator. ( for operator overloading. )
        void set_operator(const operator_property_t * op_property, __el_t * el);

        // Sets trait.
        void set_trait(method_trait_t trait, __el_t * el);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual method_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        const operator_property_t * __op_property = nullptr;
        __el_t * __name_el = nullptr;

        // Walks default step.
        bool __walk_default(ast_walk_context_t & context);

        // Walks analysis step.
        bool __walk_analysis(ast_walk_context_t & context);
    };

    ////////// ////////// ////////// ////////// //////////
    // property

    // Property.
    Ast(property, __cvalue_t::property)

        // Attributes.
        __attributes__,
        
        // Decorate.
        __decorate__,
        
        // Owner type name.
        __owner_type_name__,
        
        // Type name.
        __type_name__,
        
        // Get method.
        __get_method__,
        
        // Set method.
        __set_method__,
        
        // Params.
        __params__,
        
    EndAst(property)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Property ast node.
    class property_ast_node_t : public __property_ast_node_t
                              , public property_ast_t
                              , public __w_t<property_t>
    {
        typedef __property_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Sets name.
        void set_name(name_t name, __el_t * el);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual property_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __el_t * __name_el = nullptr;

        // Walks default step.
        bool __walk_default(ast_walk_context_t & context, int step, void * tag);

        // Append default methods for get/set methods.
        void __append_default_methods(ast_walk_context_t & context);

        // Walks analysis step.
        bool __walk_analysis(ast_walk_context_t & context);

        // Gets get/set method name.
        name_t __to_method_name(const char_t * prefix);

        // Generates ignored method bodies.
        void __generate_ignored_method_bodies(ast_walk_context_t & context, param_t * value_param);
    };

    ////////// ////////// ////////// ////////// //////////
    // event

    // Event.
    Ast(event, __cvalue_t::event)

        // Attributes.
        __attributes__,
        
        // Decorate.
        __decorate__,
        
        // Owner type name.
        __owner_type_name__,
        
        // Typename.
        __type_name__,
        
        // Add method.
        __add_method__,
        
        // Remove method.
        __remove_method__,
        
    EndAst(event)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Events ast node.
    class event_ast_node_t : public __event_ast_node_t
                           , public event_ast_t
                           , public __w_t<event_t>
    {
        typedef __event_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Sets name.
        void set_name(name_t name, __el_t * el);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual event_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __el_t *       __name_el = nullptr;

        // Walks default step.
        bool __walk_default(ast_walk_context_t & context, int step, void * tag);

        // Walks analysis step.
        bool __walk_analysis(ast_walk_context_t & context);
    };

    ////////// ////////// ////////// ////////// //////////
    // asts

    // Asts node.
    template<element_value_t value>
    class asts_node_t : public tmultipy_ast_node_t<value>
                      , public asts_t, public __utils_t
    {
        typedef tmultipy_ast_node_t<value> __super_t;

    public:

        // Constructor.
        template<typename ... _args_t>
        asts_node_t(ast_context_t & context, _args_t && ... args)
            : __utils_t(context) { }

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override
        {
            return __super_t::on_walk(context, step, tag);
        }

        // Returns eobject count.
        virtual size_t eobject_count() const override
        {
            return this->child_count();
        }

        // Returns eobject at specified index.
        virtual ast_node_t * eobject_at(size_t index) const override
        {
            return this->child_at(index);
        }
    };

    ////////// ////////// ////////// ////////// //////////

    #undef __EmptyAst
    #undef __ArrayNodeT

}   // namespace X_ROOT_NS::modules::compile


#endif // __COMPILE_AST_H__

