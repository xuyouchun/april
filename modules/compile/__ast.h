#ifndef __COMPILE_AST_H__
#define __COMPILE_AST_H__


namespace X_ROOT_NS { namespace modules { namespace compile {

    namespace
    {
        using namespace core;

        template<typename obj_t>
        using __w_t = code_element_wrapper_t<obj_t>;

        template<typename exp_t>
        using __wexp_t = __w_t<system_expression_t<exp_t>>;
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

        type_name_unit,             // Type name unit, e.g. Text in System.Text.

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

        do_while_st,                // do ... while statement, e.g. do { } while(...);

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

    #define X_BRANCH_ENUM(name)  X_ENUM_(name, sizeof(code_element_t))      \
                                                                            \
        /* common branch values */                                          \
                                                                            \
        import              =   __C_BRANCH_ENUM_ITEM(import),               \
                                                                            \
        using_              =   __C_BRANCH_ENUM_ITEM(using_),               \
                                                                            \
        expression          =   __C_BRANCH_ENUM_ITEM(expression),           \
                                                                            \
        statement           =   __C_BRANCH_ENUM_ITEM(statement),            \
                                                                            \
        type_name           =   __C_BRANCH_ENUM_ITEM(type_name),            \
                                                                            \
        cvalue              =   __C_BRANCH_ENUM_ITEM(cvalue),               \
                                                                            \
        attribute           =   __C_BRANCH_ENUM_ITEM(attribute),            \
                                                                            \
        attributes          =   __C_BRANCH_ENUM_ITEM(attributes),           \
                                                                            \
        type                =   __C_BRANCH_ENUM_ITEM(type),                 \
                                                                            \
        field               =   __C_BRANCH_ENUM_ITEM(field),                \
                                                                            \
        property            =   __C_BRANCH_ENUM_ITEM(property),             \
                                                                            \
        method              =   __C_BRANCH_ENUM_ITEM(method),               \
                                                                            \
        event               =   __C_BRANCH_ENUM_ITEM(event),                \
                                                                            \
        /* xcommon branch values */                                         \
                                                                            \
        asts                =   __X_BRANCH_ENUM_ITEM(asts),                 \
                                                                            \
        namespace_          =   __X_BRANCH_ENUM_ITEM(namespace_),           \
                                                                            \
        array_type_name     =   __X_BRANCH_ENUM_ITEM(array_type_name),      \
                                                                            \
        general_type_name   =   __X_BRANCH_ENUM_ITEM(general_type_name),    \
                                                                            \
        type_name_unit      =   __X_BRANCH_ENUM_ITEM(type_name_unit),       \
                                                                            \
        uncertain_type_name =   __X_BRANCH_ENUM_ITEM(uncertain_type_name),  \
                                                                            \
        type_def            =   __X_BRANCH_ENUM_ITEM(type_def),             \
                                                                            \
        type_def_param      =   __X_BRANCH_ENUM_ITEM(type_def_param),       \
                                                                            \
        type_def_params     =   __X_BRANCH_ENUM_ITEM(type_def_params),      \
                                                                            \
        type_of             =   __X_BRANCH_ENUM_ITEM(type_of),              \
                                                                            \
        mname               =   __X_BRANCH_ENUM_ITEM(mname),                \
                                                                            \
        expressions         =   __X_BRANCH_ENUM_ITEM(expressions),          \
                                                                            \
        argument            =   __X_BRANCH_ENUM_ITEM(argument),             \
                                                                            \
        arguments           =   __X_BRANCH_ENUM_ITEM(arguments),            \
                                                                            \
        param               =   __X_BRANCH_ENUM_ITEM(param),                \
                                                                            \
        params              =   __X_BRANCH_ENUM_ITEM(params),               \
                                                                            \
        generic_param       =   __X_BRANCH_ENUM_ITEM(generic_param),        \
                                                                            \
        generic_params      =   __X_BRANCH_ENUM_ITEM(generic_params),       \
                                                                            \
        generic_constraint  =   __X_BRANCH_ENUM_ITEM(generic_constraint),   \
                                                                            \
        generic_constraints =   __X_BRANCH_ENUM_ITEM(generic_constraints),  \
                                                                            \
        generic_arg         =   __X_BRANCH_ENUM_ITEM(generic_arg),          \
                                                                            \
        generic_args        =   __X_BRANCH_ENUM_ITEM(generic_args),         \
                                                                            \
        decorate            =   __X_BRANCH_ENUM_ITEM(decorate),             \
                                                                            \
        method_body         =   __X_BRANCH_ENUM_ITEM(method_body),          \
                                                                            \
        statements          =   __X_BRANCH_ENUM_ITEM(statements),           \
                                                                            \
        expression_st       =   __X_BRANCH_ENUM_ITEM(expression_st),        \
                                                                            \
        type_def_st         =   __X_BRANCH_ENUM_ITEM(type_def_st),          \
                                                                            \
        defination_st       =   __X_BRANCH_ENUM_ITEM(defination_st),        \
                                                                            \
        break_st            =   __X_BRANCH_ENUM_ITEM(break_st),             \
                                                                            \
        continue_st         =   __X_BRANCH_ENUM_ITEM(continue_st),          \
                                                                            \
        throw_st            =   __X_BRANCH_ENUM_ITEM(throw_st),             \
                                                                            \
        goto_st             =   __X_BRANCH_ENUM_ITEM(goto_st),              \
                                                                            \
        return_st           =   __X_BRANCH_ENUM_ITEM(return_st),            \
                                                                            \
        do_while_st         =   __X_BRANCH_ENUM_ITEM(do_while_st),          \
                                                                            \
        loop_until_st       =   __X_BRANCH_ENUM_ITEM(loop_until_st),        \
                                                                            \
        while_st            =   __X_BRANCH_ENUM_ITEM(while_st),             \
                                                                            \
        for_st              =   __X_BRANCH_ENUM_ITEM(for_st),               \
                                                                            \
        for_each_st         =   __X_BRANCH_ENUM_ITEM(for_each_st),          \
                                                                            \
        if_st               =   __X_BRANCH_ENUM_ITEM(if_st),                \
                                                                            \
        switch_st           =   __X_BRANCH_ENUM_ITEM(switch_st),            \
                                                                            \
        case_               =   __X_BRANCH_ENUM_ITEM(case_),                \
                                                                            \
        try_st              =   __X_BRANCH_ENUM_ITEM(try_st),               \
                                                                            \
        catch_              =   __X_BRANCH_ENUM_ITEM(catch_),               \
                                                                            \
        empty_st            =   __X_BRANCH_ENUM_ITEM(empty_st),             \
                                                                            \
        statement_group_st  =   __X_BRANCH_ENUM_ITEM(statement_group_st),   \
                                                                            \
        type_name_exp       =   __X_BRANCH_ENUM_ITEM(type_name_exp),        \
                                                                            \
        type_cast_exp       =   __X_BRANCH_ENUM_ITEM(type_cast_exp),        \
                                                                            \
        function            =   __X_BRANCH_ENUM_ITEM(function),             \
                                                                            \
        index               =   __X_BRANCH_ENUM_ITEM(index),                \
                                                                            \
        new_                =   __X_BRANCH_ENUM_ITEM(new_),                 \
                                                                            \
        array_initializer   =   __X_BRANCH_ENUM_ITEM(array_initializer),    \
                                                                            \
        array_lengths       =   __X_BRANCH_ENUM_ITEM(array_lengths),        \
                                                                            \
        new_array           =   __X_BRANCH_ENUM_ITEM(new_array),            \
                                                                            \
        default_value       =   __X_BRANCH_ENUM_ITEM(default_value),        \
                                                                            \
        __user_defined__    =  __X_BRANCH_ENUM_ITEM(__user_defined__),


    #define X_BRANCH_ENUM_END   X_ENUM_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    #define X_BRANCH_ENUM_INFO(name)  X_ENUM_INFO(name)                     \
        X_C(import,                 _T("import"))                           \
        X_C(using_,                 _T("using"))                            \
        X_C(expression,             _T("expression"))                       \
        X_C(statement,              _T("statement"))                        \
        X_C(type_name,              _T("type_name"))                        \
        X_C(cvalue,                 _T("cvalue"))                           \
        X_C(attribute,              _T("attribute"))                        \
        X_C(attributes,             _T("attributes"))                       \
        X_C(type,                   _T("type"))                             \
        X_C(field,                  _T("field"))                            \
        X_C(property,               _T("property"))                         \
        X_C(method,                 _T("method"))                           \
        X_C(event,                  _T("event"))                            \
                                                                            \
        X_C(asts,                   _T("asts"))                             \
        X_C(namespace_,             _T("namespace"))                        \
        X_C(array_type_name,        _T("array_type_name"))                  \
        X_C(general_type_name,      _T("general_type_name"))                \
        X_C(type_name_unit,         _T("type_name_unit"))                   \
        X_C(uncertain_type_name,    _T("uncertain_type_name"))              \
        X_C(type_def_param,         _T("type_def_param"))                   \
        X_C(type_def_params,        _T("type_def_params"))                  \
        X_C(type_def,               _T("type_def"))                         \
        X_C(type_of,                _T("type_of"))                          \
        X_C(mname,                  _T("mname"))                            \
        X_C(expressions,            _T("expressions"))                      \
        X_C(argument,               _T("argument"))                         \
        X_C(arguments,              _T("arguments"))                        \
        X_C(param,                  _T("param"))                            \
        X_C(params,                 _T("params"))                           \
        X_C(generic_param,          _T("generic_param"))                    \
        X_C(generic_params,         _T("generic_params"))                   \
        X_C(generic_constraint,     _T("generic_constraint"))               \
        X_C(generic_constraints,    _T("generic_constraints"))              \
        X_C(generic_arg,            _T("generic_arg"))                      \
        X_C(generic_args,           _T("generic_args"))                     \
        X_C(decorate,               _T("decorate"))                         \
        X_C(method_body,            _T("method_body"))                      \
                                                                            \
        X_C(statements,             _T("statements"))                       \
        X_C(expression_st,          _T("expression_st"))                    \
        X_C(type_def_st,            _T("type_def_st"))                      \
        X_C(defination_st,          _T("defination_st"))                    \
        X_C(break_st,               _T("break_st"))                         \
        X_C(continue_st,            _T("continue_st"))                      \
        X_C(throw_st,               _T("throw_st"))                         \
        X_C(goto_st,                _T("goto_st"))                          \
        X_C(return_st,              _T("return_st"))                        \
        X_C(do_while_st,            _T("do_while_st"))                      \
        X_C(loop_until_st,          _T("loop_until_st"))                    \
        X_C(while_st,               _T("while_st"))                         \
        X_C(for_st,                 _T("for_st"))                           \
        X_C(for_each_st,            _T("for_each_st"))                      \
        X_C(if_st,                  _T("if_st"))                            \
        X_C(switch_st,              _T("switch_st"))                        \
        X_C(case_,                  _T("case"))                             \
        X_C(try_st,                 _T("try_st"))                           \
        X_C(catch_,                 _T("catch"))                            \
        X_C(empty_st,               _T("empty_st"))                         \
        X_C(statement_group_st,     _T("statement_group_st"))               \
                                                                            \
        X_C(type_name_exp,          _T("type_name_exp"))                    \
        X_C(type_cast_exp,          _T("type_cast_exp"))                    \
        X_C(function,               _T("function"))                         \
        X_C(index,                  _T("index"))                            \
        X_C(new_,                   _T("new"))                              \
        X_C(array_initializer,      _T("array_initializer"))                \
        X_C(array_lengths,          _T("array_lengths"))                    \
        X_C(new_array,              _T("new_array"))                        \
        X_C(default_value,          _T("default_value"))                    \


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

        __error__           = 7000,

        // Duplicated
        duplicate                   = 7001,

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

        // Method not found.
        method_not_found,

        // Method not match.
        method_not_match,

        // Method conflict.
        method_conflict,

        // Property index undetermind.
        property_index_undetermind,

        // Property method body uniformity.
        property_method_body_missing,

        // Attribute constructor not match.
        attribute_constructor_not_match,

        // Type consturctor not matched.
        type_constructor_not_match,

        // Unexpected expression.
        unexpected_expression,

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

        // Partial specialization not supported.
        partial_specialization_not_supported,

        // No base type.
        no_base_type,

        // Expected array length.
        expected_array_length,

        // Array initializer error.
        array_initializer_error,

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
            template<typename t, typename ... args_t>
            t * __new_obj(args_t && ... args)
            {
                return __context.compile_context.new_obj<t>(std::forward<args_t>(args) ...);
            }

            // Creates a new system expression.
            template<typename _exp_t, typename ... args_t>
            system_expression_t<_exp_t> * __new_system_expression(args_t && ... args)
            {
                return __new_obj<system_expression_t<_exp_t>>(std::forward<args_t>(args) ...);
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
            template<typename _exp_t, typename ... args_t>
            statement_t * __new_expression_statement(args_t && ... args)
            {
                return __new_obj<expression_statement_t>(
                    __new_system_expression<_exp_t>(std::forward<args_t>(args) ...)
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
            template<typename _code_element_t, typename ... args_t>
            void __log(_code_element_t * element, args_t && ... args)
            {
                xlogger_t(__context.logger).logf(
                    as<code_element_t *>(element), std::forward<args_t>(args) ...
                );
            }

            // Validates name.
            bool __validate_name(const char_t * name, __el_t * el, const char_t * title = _T(""))
            {
                if(name == nullptr && name[0] == _T('\0'))
                {
                    __log(el, __c_t::name_empty, title);
                    return false;
                }

                const char_t c = name[0];
                if(!al::is_word(c) && c != _T('_'))
                {
                    __log(el, __c_t::name_unexpected_first_char, c, name);
                    return false;
                }

                for(const char_t * p = name + 1; *p; p++)
                {
                    if(!al::is_word_or_digit(*p) && *p != _T('_'))
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
                if(!__validate_name(src, el, title))
                    return false;

                if(check_duplicate && dst != name_t::null)
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
                if(check_duplicate && dst != _D(src_t))
                {
                    __log(el, __c_t::duplicate, title, src);
                    return false;
                }

                dst = src;
                return true;
            }

            // Checks whether it's empty.
            template<typename t, typename _log_code_t, typename ... args_t>
            bool __check_empty(const t & value, __el_t * el, _log_code_t code, args_t && ... args)
            {
                if(__is_empty_value(value))
                {
                    __log(el? el : (__el_t *)this, code, std::forward<args_t>(args) ...);
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
                    to_mname_operate_context(gctx.xpool), name1, name2
                );
            }

            // Converts to multi-name.
            const mname_t * __to_mname(const string_t & s)
            {
                global_context_t & gctx = __global_context();
                return mname_t::parse(
                    to_mname_operate_context(gctx.xpool), s
                );
            }
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
        template<typename ... args_t>
        node_wrapper_t(ast_context_t & context, args_t && ... args)
            : __base_t(std::forward<args_t>(args) ...), __utils_t(context)
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
        template<typename code_t, typename ... args_t>
        void set_child_with_check(__index_t index, __node_t * node,
                                    code_t duplicate_code, args_t && ... args)
        {
            if(has_child(index))
                this->__log(node, duplicate_code, std::forward<args_t>(args) ...);
            else
                this->set_child(index, node);
        }

        // Returns multipy node at specified index.
        multipy_ast_node_t * __multipy_ast_node_at(__index_t index)
        {
            typedef asts_node_t<(element_value_t)__xcvalue_t::asts> __asts_node_t;

            __node_t * child = child_at(index);
            if(child == nullptr)
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
            while(nodes->child_count() <= child_index)
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
            if(node != nullptr)
            {
                for(size_t index = 0, count = node->child_count(); index < count; index++)
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
            if(child != nullptr)
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
            if(ast != nullptr)
                return ast->to_eobject();
            return nullptr;
        }

        // Tries convert child to specified etype_t with owner.
        // Returns nullptr if failed to convert.
        template<typename etype_t, typename owner_t>
        etype_t __to_eobject_with_owner(__index_t index, owner_t owner)
        {
            etype_t eobj = __to_eobject<etype_t>(index);
            if(eobj != nullptr)
                eobj->set_owner(owner);

            return eobj;
        }

        // Returns node value.
        virtual __value_t value() const override final
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
            if(node == nullptr)
            {
                f(nullptr);
                return;
            }

            switch(node->get_type())
            {
                case eobject_ast_type_t::eobject:
                    f(((eobject_ast_t<etype_t> *)node)->to_eobject());
                    break;

                case eobject_ast_type_t::eobjects: {
                    auto nodes = (eobjects_ast_t<etype_t> *)node;
                    for(size_t index = 0, count = nodes->eobject_count(); index < count; index++)
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

    #define Ast(name, value)                                                    \
        namespace                                                               \
        {                                                                       \
            struct __##name##_ast_node_data_t                                   \
                : public ::X_ROOT_NS::modules::compile::node_data_base_t<       \
                    __##name##_ast_node_data_t,                                 \
                    decltype(value), value>                                     \
            {                                                                   \
                enum {


    #define EndAst(name)                                                        \
                __end__ };                                                      \
            };                                                                  \
        }                                                                       \
                                                                                \
        typedef node_wrapper_t<__##name##_ast_node_data_t> RawAstName(name);

    #define __EmptyAst(name, value)                                             \
        Ast(name, value)                                                        \
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
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

        // Walks this node.
        virtual void do_walk(ast_walk_context_t & context, int step, void * tag) override;
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
        template<typename ... args_t>
        mname_ast_node_t(args_t && ... args) : __super_t(std::forward<args_t>(args) ...)
        {
            new ((void *)__mname) mname_t();
        }

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

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
        expressions,

    EndAst(expressions)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Expressions ast node.
    class expressions_ast_node_t : public __expressions_ast_node_t
                                 , public expression_ast_t
    {
        typedef __expressions_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual expression_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        expressions_t __expressions;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_name

    Ast(type_name, __cvalue_t::type_name)

        // Underlying.
        underlying,

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
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_name_unit_t

    // Typename unit.
    struct type_name_unit_t : public eobject_t
    {
        // Constructor.
        type_name_unit_t() = default;
        type_name_unit_t(const name_t & name, generic_args_t * args = nullptr)
            : name(name), args(args) { }

        typedef type_name_unit_t * itype_t;

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

    typedef eobject_ast_t<typename type_name_unit_t::itype_t> type_name_unit_ast_t;

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
        typedef al::svector_t<type_name_unit_t *, 3> unit_list_t;
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
            type_name_unit_t * unit;

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
    class __type_name_ast_node_base_t : public _super_t
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
                if(!al::starts_with(s_type_name.c_str(), _T("System.")))
                {
                    _PF(_T("+type %1%: \n      %2% (%3%)"), _str(type_name),
                                                    _str(type), (void *)type);
                }

                #endif

                return type;
            }
            catch(const logic_error_t<ascertain_type_error_t> & e)
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
            switch(error_code)
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
        units,

    EndAst(general_type_name)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // General type name ast node.
    class general_type_name_ast_node_t
            : public __type_name_ast_node_base_t<__general_type_name_ast_node_t>
            , public type_name_ast_t
    {
        typedef __type_name_ast_node_base_t<__general_type_name_ast_node_t> __super_t;

    public:
        using __super_t::__super_t;

        // Set global type.
        void set_global_type(global_type_t global_type);

        // On commits it.
        virtual void on_commit() override;

        // Returns eobject.
        virtual type_name_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<general_type_name_t> __type_name;

        // Walks confirm step.
        void __walk_confirm(ast_walk_context_t & context);
    };

    ////////// ////////// ////////// ////////// //////////

    // Typename unit.
    Ast(type_name_unit, __xcvalue_t::type_name_unit)

        // Generic arguments.
        generic_args,

    EndAst(type_name_unit)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Typename unit ast node.
    class type_name_unit_ast_node_t : public __type_name_unit_ast_node_t
                                    , public type_name_unit_ast_t
    {
        typedef __type_name_unit_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Sets name.
        void set_name(name_t name, __el_t * el);

        // Commits it.
        virtual void on_commit() override;

        // Returns eobject.
        virtual type_name_unit_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<type_name_unit_t> __type_name_unit;
    };

    ////////// ////////// ////////// ////////// //////////
    // array_type_name

    // Array typename.
    Ast(array_type_name, __xcvalue_t::array_type_name)

        // Array element typename.
        element_type_name,
        
        // Array dimensions.
        dimensions,

    EndAst(array_type_name)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Array typename ast node.
    class array_type_name_ast_node_t
            : public __type_name_ast_node_base_t<__array_type_name_ast_node_t>
            , public type_name_ast_t
    {
        typedef __type_name_ast_node_base_t<__array_type_name_ast_node_t> __super_t;

    public:
        using __super_t::__super_t;

        // Commits it.
        virtual void on_commit() override;

        // Returns the eobject.
        virtual type_name_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<array_type_name_t> __type_name;

        // Walks the confirm step.
        void __walk_confirm(ast_walk_context_t & context);
    };

    ////////// ////////// ////////// ////////// //////////
    // uncertain_type_name

    __EmptyAst(uncertain_type_name, __xcvalue_t::uncertain_type_name)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Uncertain type name ast node.
    class uncertain_type_name_ast_node_t
            : public __type_name_ast_node_base_t<__uncertain_type_name_ast_node_t>
            , public type_name_ast_t
    {
        typedef __type_name_ast_node_base_t<__uncertain_type_name_ast_node_t> __super_t;

    public:
        using __super_t::__super_t;

        // Commit this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual type_name_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        type_name_t * __type_name = nullptr;
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
    {
        typedef __type_def_param_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Sets name.
        void set_name(name_t name, __el_t * el);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual type_def_param_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<type_def_param_t> __type_def_param;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_def_params

    // Typedef params.
    Ast(type_def_params, __xcvalue_t::type_def_params)

        // Params.
        params,

    EndAst(type_def_params)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Typedef params ast node.
    class type_def_params_ast_node_t : public __type_def_params_ast_node_t
                                     , public type_def_params_ast_t
    {
        typedef __type_def_params_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual type_def_params_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        type_def_params_t __type_def_params;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_def

    // Typedef.
    Ast(type_def, __xcvalue_t::type_def)

        // Typename
        type_name,
        
        // Params.
        params,

    EndAst(type_def)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Typedef ast node.
    class type_def_ast_node_t : public __type_def_ast_node_t
                              , public type_def_ast_t
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
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        type_def_t __type_def;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_of

    // Typeof.
    Ast(type_of, __xcvalue_t::type_of)

        // Typename.
        type_name,

    EndAst(type_of)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Typeof ast node.
    class type_of_ast_node_t : public __type_of_ast_node_t
                             , public expression_ast_t
    {
        typedef __type_of_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual expression_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __wexp_t<type_of_expression_t> __type_of;
    };

    ////////// ////////// ////////// ////////// //////////
    // argument

    // Argument.
    Ast(argument, __xcvalue_t::argument)

        // Expression
        expression,
        
        // Attributes.
        attributes,

    EndAst(argument)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Argument ast node.
    class argument_ast_node_t : public __argument_ast_node_t
                              , public argument_ast_t
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
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        argument_t __argument;
    };

    ////////// ////////// ////////// ////////// //////////
    // arguments

    // Arguments.
    Ast(arguments, __xcvalue_t::arguments)

        // Arguments.
        arguments,

    EndAst(arguments)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Arguments ast node.
    class arguments_ast_node_t : public __arguments_ast_node_t
                               , public arguments_ast_t
    {
        typedef __arguments_ast_node_t __super_t;

    public:
        using __arguments_ast_node_t::__arguments_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

        // Returns this eobject.
        virtual arguments_t * to_eobject() override;

    private:
        arguments_t __arguments;
    };

    ////////// ////////// ////////// ////////// //////////
    // param

    // Param.
    Ast(param, __xcvalue_t::param)

        // Typename
        type_name,
        
        // Attributes.
        attributes,
        
        // Default value.
        default_value,

    EndAst(param)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Param ast node.
    class param_ast_node_t : public __param_ast_node_t
                           , public param_ast_t
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
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        param_t __param;
    };

    ////////// ////////// ////////// ////////// //////////
    // params

    // Params.
    Ast(params, __xcvalue_t::params)

        // Params.
        params,

    EndAst(params)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Params ast node.
    class params_ast_node_t : public __params_ast_node_t
                            , public params_ast_t
    {
        typedef __params_ast_node_t __super_t;

    public:
        using __params_ast_node_t::__params_ast_node_t;

        // Commits it.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual params_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        params_t __params;
    };

    ////////// ////////// ////////// ////////// //////////
    // generic_param

    // Generic param.
    Ast(generic_param, __xcvalue_t::generic_param)

        // Attributes.
        attributes,

    EndAst(generic_param)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Generic param ast node.
    class generic_param_ast_node_t : public __generic_param_ast_node_t
                                   , public generic_param_ast_t
    {
        typedef __generic_param_ast_node_t __super_t;

    public:
        using __generic_param_ast_node_t::__generic_param_ast_node_t;

        // Sets name.
        void set_name(name_t name, __el_t * el);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this node.
        virtual generic_param_t * to_eobject() override;

        // Wallks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        generic_param_t __param;
    };

    ////////// ////////// ////////// ////////// //////////
    // generic_params

    // Generic params.
    Ast(generic_params, __xcvalue_t::generic_params)

        // Params.
        params,

    EndAst(generic_params)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Generic params ast node.
    class generic_params_ast_node_t : public __generic_params_ast_node_t
                                    , public generic_params_ast_t
    {
        typedef __generic_params_ast_node_t __super_t;

    public:
        using __generic_params_ast_node_t::__generic_params_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual generic_params_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        generic_params_t __generic_params;
    };
   
    ////////// ////////// ////////// ////////// //////////
    // generic_constraint

    // Generic constraint.
    Ast(generic_constraint, __xcvalue_t::generic_constraint)

        // Typenames.
        type_names,

    EndAst(generic_constraint)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Generic constraint ast node.
    class generic_constraint_ast_node_t : public __generic_constraint_ast_node_t
                                        , public generic_constraint_ast_t
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
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
       generic_constraint_t __generic_constraint;
    };

    ////////// ////////// ////////// ////////// //////////
    // generic_constraints

    // Generic constraints.
    Ast(generic_constraints, __xcvalue_t::generic_constraints)

        // Constraints.
        constraints,

    EndAst(generic_constraints)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Generic constraints ast node.
    class generic_constraints_ast_node_t : public __generic_constraints_ast_node_t
                                         , public generic_constraints_ast_t
    {
        typedef __generic_constraints_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits it.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual generic_constraints_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
       generic_constraints_t __generic_constraints;
    };

    ////////// ////////// ////////// ////////// //////////
    // generic_arg

    // Generic argument.
    Ast(generic_arg, __xcvalue_t::generic_arg)

        // Typename.
        type_name,

    EndAst(generic_arg)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Generic argument ast node.
    class generic_arg_ast_node_t : public __generic_arg_ast_node_t
                                 , public generic_arg_ast_t
    {
        typedef __generic_arg_ast_node_t __super_t;

    public:
        using __generic_arg_ast_node_t::__generic_arg_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual generic_arg_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        generic_arg_t       __arg;
    };

    ////////// ////////// ////////// ////////// //////////
    // generic_args

    // Generic arguments.
    Ast(generic_args, __xcvalue_t::generic_args)

        // Generic arguments.
        generic_args,

    EndAst(generic_args)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Generic arguments ast node.
    class generic_args_ast_node_t : public __generic_args_ast_node_t
                                  , public generic_args_ast_t
    {
        typedef __generic_args_ast_node_t __super_t;

    public:
        using __generic_args_ast_node_t::__generic_args_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual generic_args_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        generic_args_t __args;
    };

    ////////// ////////// ////////// ////////// //////////
    // attribute

    // Attribute.
    Ast(attribute, __cvalue_t::attribute)

        // Typename
        type_name,
        
        // Arguments.
        arguments,

    EndAst(attribute)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Attribute ast node.
    class attribute_ast_node_t : public __attribute_ast_node_t
                               , public attribute_ast_t
    {
        typedef __attribute_ast_node_t __super_t;

    public:
        using __attribute_ast_node_t::__attribute_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual attribute_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        attribute_t __attr;

        // Returns compile time attribute.
        attribute_t * __get_compile_time_attribute();

        // Analyze attribute constructor.
        void __analyze_attribute_constructor();
    };

    ////////// ////////// ////////// ////////// //////////
    // attributes

    // Attributes.
    Ast(attributes, __cvalue_t::attributes)

        // Attributes.
        attributes,

    EndAst(attributes)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Attributes ast node.
    class attributes_ast_node_t : public __attributes_ast_node_t
                                , public attributes_ast_t
    {
        typedef __attributes_ast_node_t __super_t;

    public:
        using __attributes_ast_node_t::__attributes_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual attributes_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        attributes_t __attributes;
    };

    ////////// ////////// ////////// ////////// //////////
    // cvalue

    __EmptyAst(cvalue, __cvalue_t::cvalue)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Constant ast node.
    class cvalue_ast_node_t : public __cvalue_ast_node_t
                            , public cvalue_ast_t
    {
        typedef __cvalue_ast_node_t __super_t;

    public:
        using __cvalue_ast_node_t::__cvalue_ast_node_t; 

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

        // Returns this eobject.
        virtual cvalue_t * to_eobject() override;

        // Sets value.
        void set_value(const cvalue_t & value);

        // Sets value.
        void set_value(const token_data_t & data);

    private:
        cvalue_t __value;
    };

    ////////// ////////// ////////// ////////// //////////
    // decorate

    __EmptyAst(decorate, __xcvalue_t::decorate)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Decorate ast node.
    class decorate_ast_node_t : public __decorate_ast_node_t
                              , public decorate_ast_t
    {
        typedef __decorate_ast_node_t __super_t;

    public:
        using __decorate_ast_node_t::__decorate_ast_node_t;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

        // Set access.
        void set_access(access_value_t value, __el_t * el);

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

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual decorate_t * to_eobject() override;

    private:
        decorate_t __decorate = decorate_t::default_value;
    };

    ////////// ////////// ////////// ////////// //////////
    // statements

    // Statements.
    Ast(statements, __xcvalue_t::statements)

        // Statements.
        statements,

    EndAst(statements)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Statements ast node.
    class statements_ast_node_t : public __statements_ast_node_t
                                , public statements_ast_t
    {
        typedef __statements_ast_node_t __super_t;

    public:
        using __statements_ast_node_t::__statements_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statements_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        statements_t __statements;
    };

    ////////// ////////// ////////// ////////// //////////
    // method_body

    // Method body.
    Ast(method_body, __xcvalue_t::method_body)

        // Statements.
        statements,

    EndAst(method_body)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Method body ast node.
    class method_body_ast_node_t : public __method_body_ast_node_t
                                 , public method_body_ast_t
    {
        typedef __method_body_ast_node_t __super_t;

    public:
        using __method_body_ast_node_t::__method_body_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual method_body_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        method_body_t __body;
    };

    ////////// ////////// ////////// ////////// //////////
    // expression

    // Expression.
    Ast(expression, __cvalue_t::expression)

        // Nodes.
        nodes,

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

        // Appends expression.
        void append_expression(ast_node_t * node);

        // Appends constant value.
        void append_cvalue(ast_node_t * node);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual expression_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __es_t __es;
        expression_t * __expression = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////
    // statement

    // Statement.
    Ast(statement, __cvalue_t::statement)

        // Underlying statement.
        underlying,

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
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // import

    // Import
    Ast(import, __cvalue_t::import)

        // Package.
        package,
        
        // Assembly.
        assembly,
        
    EndAst(import)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Import ast node.
    class import_ast_node_t : public __import_ast_node_t
                            , public import_ast_t
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
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        import_t __import;
    };

    ////////// ////////// ////////// ////////// //////////
    // using

    // Using
    Ast(using, __cvalue_t::using_)

        // Namespace.
        ns,
        
    EndAst(using)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Using ast node.
    class using_ast_node_t : public __using_ast_node_t
                           , public using_namespace_ast_t
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
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        using_namespace_t __using_namespace;
    };

    ////////// ////////// ////////// ////////// //////////
    // namespace

    // Namespace
    Ast(namespace, __xcvalue_t::namespace_)

        // Name.
        name,
        
        // Typedefs.
        type_defs,
        
        // Types.
        types,
        
        // Namespaces.
        namespaces, 
        
    EndAst(namespace)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Namespace ast node.
    class namespace_ast_node_t : public __namespace_ast_node_t
                               , public namespace_ast_t
    {
        typedef __namespace_ast_node_t __super_t;

    public:
        using __namespace_ast_node_t::__namespace_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual namespace_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        namespace_t __namespace;
    };

    ////////// ////////// ////////// ////////// //////////
    // type

    // Type
    Ast(type, __cvalue_t::type)

        // Attributes.
        attributes,
        
        // Decorate.
        decorate,
        
        // Generic param.
        generic_params,
        
        // Super typenames.
        super_type_names,

        // Typedefs.
        type_defs,
        
        // Fields.
        fields,
        
        // Methods.
        methods,
        
        // Properties.
        properties,
        
        // Events.
        events,
        
        // Nest types.
        nest_types,
        
    EndAst(type)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Type ast node.
    class type_ast_node_t : public __type_ast_node_t
                          , public type_ast_t
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
        type_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<general_type_t> __type;

        // Walks default step.
        void __walk_default(ast_walk_context_t & context, int step, void * tag);

        // Appends default constructors.
        method_t * __append_default_constructor();

        // Walks analysis step.
        void __walk_analysis(ast_walk_context_t & context, method_t * method);
    };

    ////////// ////////// ////////// ////////// //////////
    // if

    // If statement.
    Ast(if, __cvalue_t::statement)

        // Condition.
        condition,
        
        // Body.
        body,
        
    EndAst(if)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // If ast node.
    class if_ast_node_t : public __if_ast_node_t
    {
        typedef __if_ast_node_t __super_t;

    public:
        using __if_ast_node_t::__if_ast_node_t;

        // Walk this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // for

    // For ast node.
    Ast(for, __cvalue_t::statement)

        // Initializer.
        initializer,
        
        // Condition.
        condition,
        
        // Expression.
        expression,
        
        // Body.
        body,
        
    EndAst(for)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // For ast node.
    class for_ast_node_t : public __for_ast_node_t
    {
        typedef __for_ast_node_t __super_t;

    public:
        using __for_ast_node_t::__for_ast_node_t;

        // Walk this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // while

    // While ast node.
    Ast(while, __cvalue_t::statement)

        // Condition.
        condition,

        // Body.
        body,
        
    EndAst(while)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // While ast node.
    class while_ast_node_t : public __while_ast_node_t
    {
        typedef __while_ast_node_t __super_t;

    public:
        using __while_ast_node_t::__while_ast_node_t;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // switch

    // Switch ast node.
    Ast(switch, __cvalue_t::statement)

        // Expression.
        expression,
        
    EndAst(switch)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Switch ast node.
    class switch_ast_node_t : public __switch_ast_node_t
    {
        typedef __switch_ast_node_t __super_t;

    public:
        using __switch_ast_node_t::__switch_ast_node_t;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // loop

    // Loop statement.
    Ast(loop, __cvalue_t::statement)

        // Condition.
        condition,
        
        // Body.
        body,
        
    EndAst(loop)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Loop ast node.
    class loop_ast_node_t : public __loop_ast_node_t
    {
        typedef __loop_ast_node_t __super_t;

    public:
        using __loop_ast_node_t::__loop_ast_node_t;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // break

    __EmptyAst(break, __cvalue_t::statement)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Break ast node.
    class break_ast_node_t : public __break_ast_node_t
    {
        typedef __break_ast_node_t __super_t;

    public:
        using __break_ast_node_t::__break_ast_node_t;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // continue

    __EmptyAst(continue, __cvalue_t::statement)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Continue ast node.
    class continue_ast_node_t : public __continue_ast_node_t
    {
        typedef __continue_ast_node_t __super_t;

    public:
        using __continue_ast_node_t::__continue_ast_node_t;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // return

    // Return.
    Ast(return, __cvalue_t::statement)

        // Expression.
        expression,
        
    EndAst(return)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Returns ast node.
    class return_ast_node_t : public __return_ast_node_t
    {
        typedef __return_ast_node_t __super_t;

    public:
        using __return_ast_node_t::__return_ast_node_t;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // expression_st

    // Expression.
    Ast(expression_st, __xcvalue_t::expression_st)

        // Expression.
        expression,
        
    EndAst(expression_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Expression statement ast node.
    class expression_st_ast_node_t : public __expression_st_ast_node_t
                                   , public statement_ast_t
    {
        typedef __expression_st_ast_node_t __super_t;

    public:
        using __expression_st_ast_node_t::__expression_st_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<expression_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_def_st

    // Typedef
    Ast(type_def_st, __xcvalue_t::type_def_st)

        // Typedef.
        type_def,
        
    EndAst(type_def_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Typedef statement ast node.
    class type_def_st_ast_node_t : public __type_def_st_ast_node_t
                                 , public statement_ast_t
    {
        typedef __type_def_st_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<type_def_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // defination_st

    // Defination statement.
    Ast(defination_st, __xcvalue_t::defination_st)

        // Typename.
        type_name,
        
        // Items.
        items,

    EndAst(defination_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Defination statement ast node.
    class defination_st_ast_node_t : public __defination_st_ast_node_t
                                   , public statement_ast_t
    {
        typedef __defination_st_ast_node_t __super_t;

    public:
        using __defination_st_ast_node_t::__defination_st_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<defination_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // break_st

    __EmptyAst(break_st, __xcvalue_t::break_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Break statements ast node.
    class break_st_ast_node_t : public __break_st_ast_node_t
                              , public statement_ast_t
    {
        typedef __break_st_ast_node_t __super_t;

    public:
        using __break_st_ast_node_t::__break_st_ast_node_t;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<break_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // continue_st

    // Continue statement.
    __EmptyAst(continue_st, __xcvalue_t::continue_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Continue statement ast node.
    class continue_st_ast_node_t : public __continue_st_ast_node_t
                                 , public statement_ast_t
    {
        typedef __continue_st_ast_node_t __super_t;

    public:
        using __continue_st_ast_node_t::__continue_st_ast_node_t;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<continue_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // throw_st

    // Throw statement.
    Ast(throw_st, __xcvalue_t::throw_st)

        // Expression.
        expression,

    EndAst(throw_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Throw statement ast node.
    class throw_st_ast_node_t : public __throw_st_ast_node_t
                              , public statement_ast_t
    {
        typedef __throw_st_ast_node_t __super_t;

    public:
        using __throw_st_ast_node_t::__throw_st_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<throw_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // goto_st

    // Goto statement.
    __EmptyAst(goto_st, __xcvalue_t::goto_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Goto statement ast node.
    class goto_st_ast_node_t : public __goto_st_ast_node_t
                             , public statement_ast_t
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
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<goto_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // return_st

    // Return statement.
    Ast(return_st, __xcvalue_t::return_st)

        // Expression.
        expression,

    EndAst(return_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Return statement ast node.
    class return_st_ast_node_t : public __return_st_ast_node_t
                               , public statement_ast_t
    {
        typedef __return_st_ast_node_t __super_t;

    public:
        using __return_st_ast_node_t::__return_st_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<return_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // do_while_st

    Ast(do_while_st, __xcvalue_t::do_while_st)

        // Condition
        condition,
        
        // Body
        body,

    EndAst(do_while_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Do ... while statement ast node.
    class do_while_st_ast_node_t : public __do_while_st_ast_node_t
                                 , public statement_ast_t
    {
        typedef __do_while_st_ast_node_t __super_t;

    public:
        using __do_while_st_ast_node_t::__do_while_st_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<do_while_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // loop_until_st

    // Loop ... unitl statement ast node.
    Ast(loop_until_st, __xcvalue_t::loop_until_st)

        // Condition.
        condition,
        
        // Body.
        body,

    EndAst(loop_until_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Loop ... until statement ast node.
    class loop_until_st_ast_node_t : public __loop_until_st_ast_node_t
                                   , public statement_ast_t
    {
        typedef __loop_until_st_ast_node_t __super_t;

    public:
        using __loop_until_st_ast_node_t::__loop_until_st_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<loop_until_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // while_st

    // While statement.
    Ast(while_st, __xcvalue_t::while_st)

        // Condition.
        condition,
        
        // Body.
        body,

    EndAst(while_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // While statement ast node.
    class while_st_ast_node_t : public __while_st_ast_node_t
                              , public statement_ast_t
    {
        typedef __while_st_ast_node_t __super_t;

    public:
        using __while_st_ast_node_t::__while_st_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<while_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // for_st

    // For statement.
    Ast(for_st, __xcvalue_t::for_st)

        // Initialize
        initialize,
        
        // Condition.
        condition,
        
        // Increase.
        increase,
        
        // Body.
        body,

    EndAst(for_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // For statement ast node.
    class for_st_ast_node_t : public __for_st_ast_node_t
                            , public statement_ast_t
    {
        typedef __for_st_ast_node_t __super_t;

    public:
        using __for_st_ast_node_t::__for_st_ast_node_t;

        // Commits it.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<for_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // for_each_st

    Ast(for_each_st, __xcvalue_t::for_each_st)

        // Typename
        type_name,
        
        // Iterator expression.
        iterator,
        
        // Body.
        body,

    EndAst(for_each_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // For...each statement ast node.
    class for_each_st_ast_node_t : public __for_each_st_ast_node_t
                                 , public statement_ast_t
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
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<for_each_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // if_st

    // If statement.
    Ast(if_st, __xcvalue_t::if_st)

        // Condition.
        condition,
        
        // If body.
        if_body,
        
        // Else body.
        else_body,

    EndAst(if_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // If statement ast node.
    class if_st_ast_node_t : public __if_st_ast_node_t
                           , public statement_ast_t
    {
        typedef __if_st_ast_node_t __super_t;

    public:
        using __if_st_ast_node_t::__if_st_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<if_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // switch_st

    // Switch statement.
    Ast(switch_st, __xcvalue_t::switch_st)

        // Expression.
        expression,
        
        // Cases.
        cases,

    EndAst(switch_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Switch statement ast node.
    class switch_st_ast_node_t : public __switch_st_ast_node_t
                               , public statement_ast_t
    {
        typedef __switch_st_ast_node_t __super_t;

    public:
        using __switch_st_ast_node_t::__switch_st_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<switch_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // case

    // Case
    Ast(case, __xcvalue_t::case_)

        // Constants.
        constants,
        
        // Statements.
        statements,

    EndAst(case)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Case ast node.
    class case_ast_node_t : public __case_ast_node_t
                          , public case_ast_t
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
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<case_t> __case;
    };

    ////////// ////////// ////////// ////////// //////////
    // try_st

    // Try statement.
    Ast(try_st, __xcvalue_t::try_st)

        // Try.
        try_,
        
        // Catches.
        catches,
        
        // Finally.
        finally,
        
    EndAst(try_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Try statement ast node.
    class try_st_ast_node_t : public __try_st_ast_node_t
                            , public statement_ast_t
    {
        typedef __try_st_ast_node_t __super_t;

    public:
        using __try_st_ast_node_t::__try_st_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walk this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<try_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // catch

    // Catch
    Ast(catch, __xcvalue_t::catch_)

        // Typename.
        type_name,
        
        // Body.
        body,
        
    EndAst(catch)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Catch ast node.
    class catch_ast_node_t : public __catch_ast_node_t
                           , public catch_ast_t
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
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __el_t * __variable_el;
        __w_t<catch_t> __catch;
    };

    ////////// ////////// ////////// ////////// //////////
    // empty_st

    // Empty
    __EmptyAst(empty_st, __xcvalue_t::empty_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Empty statement ast node.
    class empty_st_ast_node_t : public __empty_st_ast_node_t
                              , public statement_ast_t
    {
        typedef __empty_st_ast_node_t __super_t;

    public:
        using __empty_st_ast_node_t::__empty_st_ast_node_t;

        // Returns this eobject.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<empty_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // statement_group_st

    // Statement group statement.
    Ast(statement_group_st, __xcvalue_t::statement_group_st)

        // Statements.
        statements,

    EndAst(statement_group_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Statement group statement ast node.
    class statement_group_st_ast_node_t : public __statement_group_st_ast_node_t
                                        , public statement_ast_t
    {
        typedef __statement_group_st_ast_node_t __super_t;

    public:
        using __statement_group_st_ast_node_t::__statement_group_st_ast_node_t;

        // Commits it.
        virtual void on_commit() override;

        // Returns this statement.
        virtual statement_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<statement_group_t> __statement_group;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_name_exp

    // Typename expression.
    Ast(type_name_exp, __xcvalue_t::type_name_exp)

        // Typename.
        type_name,

    EndAst(type_name_exp)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Typename expression ast node.
    class type_name_exp_ast_node_t : public __type_name_exp_ast_node_t
                                   , public expression_ast_t
    {
        typedef __type_name_exp_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual expression_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __wexp_t<type_name_expression_t> __expression;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_cast_exp 

    // Type cast expression.
    Ast(type_cast_exp, __xcvalue_t::type_cast_exp)

        // Typename
        type_name,
        
        // Expression.
        expression,

    EndAst(type_cast_exp)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Type cast expression ast node.
    class type_cast_exp_ast_node_t : public __type_cast_exp_ast_node_t
                                   , public expression_ast_t
    {
        typedef __type_cast_exp_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual expression_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __wexp_t<type_cast_expression_t> __expression;
    };

    ////////// ////////// ////////// ////////// //////////
    // function

    // Function
    Ast(function, __xcvalue_t::function)

        // Namex or an expression.
        namex,
        
        // Generic arguments.
        generic_args,
        
        // Arguments.
        arguments,

    EndAst(function)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Functions ast node.
    class function_ast_node_t : public __function_ast_node_t
                              , public expression_ast_t
    {
        typedef __function_ast_node_t __super_t;

    public:
        using __function_ast_node_t::__function_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual expression_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __wexp_t<function_expression_t> __expression;
    };

    ////////// ////////// ////////// ////////// //////////
    // index

    // Index.
    Ast(index, __xcvalue_t::index)

        // Name or an expression.
        namex,
        
        // Arguments.
        arguments,

    EndAst(index)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Index ast node.
    class index_ast_node_t : public __index_ast_node_t
                           , public expression_ast_t
    {
        typedef __index_ast_node_t __super_t;

    public:
        using __index_ast_node_t::__index_ast_node_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns thid eobject.
        virtual expression_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __wexp_t<index_expression_t> __expression;
    };

    ////////// ////////// ////////// ////////// //////////
    // new

    // New
    Ast(new, __xcvalue_t::new_)

        // Typename
        type_name,
        
        // Arguments.
        arguments,

    EndAst(new)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // New ast node.
    class new_ast_node_t : public __new_ast_node_t
                         , public expression_ast_t
    {
        typedef __new_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual expression_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __wexp_t<new_expression_t> __expression;
    };

    ////////// ////////// ////////// ////////// //////////
    // new_array

    // New array.
    Ast(new_array, __xcvalue_t::new_array)

        // Typename
        type_name,
        
        // Lengths.
        lengths,
        
        // Initializer.
        initializer,

    EndAst(new_array)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // New array ast node.
    class new_array_ast_node_t : public __new_array_ast_node_t
                               , public expression_ast_t
    {
        typedef __new_array_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual expression_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __wexp_t<new_array_expression_t> __expression;
        array_length_t __get_length(dimension_t dimension);

        // Walks default step.
        void __walk_default(ast_walk_context_t & context);

        // Walks confirm step.
        void __walk_confirm(ast_walk_context_t & context);

        // Walks analysis step.
        void __walk_analysis(ast_walk_context_t & context);

    };

    ////////// ////////// ////////// ////////// //////////
    // array_initializer

    // Array initializer.
    Ast(array_initializer, __xcvalue_t::array_initializer)

        // Elements.
        elements,

    EndAst(array_initializer)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Array initializer ast node.
    class array_initializer_ast_node_t : public __array_initializer_ast_node_t
                                       , public array_initializer_ast_t
    {
        typedef __array_initializer_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual array_initializer_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        array_initializer_t __initializer;
    };

    ////////// ////////// ////////// ////////// //////////
    // array_lengths

    // Array lengths.
    Ast(array_lengths, __xcvalue_t::array_lengths)

        // Lengths.
        lengths,

    EndAst(array_lengths)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Array lengths ast node.
    class array_lengths_ast_node_t : public __array_lengths_ast_node_t
                                   , public array_lengths_ast_t
    {
        typedef __array_lengths_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commit this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual array_lengths_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        array_lengths_t __lengths;
    };


    ////////// ////////// ////////// ////////// //////////
    // default_value

    // Default value.
    Ast(default_value, __xcvalue_t::default_value)

        // Typename.
        type_name,

    EndAst(default_value)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Default value ast node.
    class default_value_ast_node_t : public __default_value_ast_node_t
                                   , public expression_ast_t
    {
        typedef __default_value_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual expression_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __wexp_t<default_value_expression_t> __expression;
    };

    ////////// ////////// ////////// ////////// //////////
    // field

    // Field.
    Ast(field, __cvalue_t::field)

        // Attributes.
        attributes,
        
        // Decorate.
        decorate,
        
        // Typename
        type_name,
        
        // Initialize value.
        init_value,
        
    EndAst(field)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Field ast node.
    class field_ast_node_t : public __field_ast_node_t
                           , public field_ast_t
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
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<field_t> __field;
    };

    ////////// ////////// ////////// ////////// //////////
    // method

    // Method ast node.
    Ast(method, __cvalue_t::method)

        // Attributes.
        attributes,
        
        // Decorate.
        decorate,
        
        // Owner type name.
        owner_type_name,
        
        // Typename
        type_name,
        
        // Generic params.
        generic_params,
        
        // Params.
        params,

        // Base constructor arguments.
        base_ctor_args,
        
        // Body.
        body,

    EndAst(method)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Method ast node.
    class method_ast_node_t : public __method_ast_node_t
                            , public method_ast_t
    {
        typedef __method_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Sets name.
        void set_name(name_t name, __el_t * el);

        // Sets trait.
        void set_trait(method_trait_t trait, __el_t * el);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual method_t * to_eobject() override;

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<method_t> __method;

        void __walk_default(ast_walk_context_t & context);
        void __walk_analysis(ast_walk_context_t & context);
    };

    ////////// ////////// ////////// ////////// //////////
    // property

    // Property.
    Ast(property, __cvalue_t::property)

        // Attributes.
        attributes,
        
        // Decorate.
        decorate,
        
        // Owner type name.
        owner_type_name,
        
        // Type name.
        type_name,
        
        // Get method.
        get_method,
        
        // Set method.
        set_method,
        
        // Params.
        params,
        
    EndAst(property)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Property ast node.
    class property_ast_node_t : public __property_ast_node_t
                              , public property_ast_t
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
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<property_t> __property;

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
        attributes,
        
        // Decorate.
        decorate,
        
        // Owner type name.
        owner_type_name,
        
        // Typename.
        type_name,
        
        // Add method.
        add_method,
        
        // Remove method.
        remove_method,
        
    EndAst(event)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Events ast node.
    class event_ast_node_t : public __event_ast_node_t
                           , public event_ast_t
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
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<event_t> __event;
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
        template<typename ... args_t>
        asts_node_t(ast_context_t & context, args_t && ... args)
            : __utils_t(context) { }

        // Walks this node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override
        {
            __super_t::on_walk(context, step, tag);
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

} } }  // namespace X_ROOT_NS::modules::compile


#endif // __COMPILE_AST_H__

