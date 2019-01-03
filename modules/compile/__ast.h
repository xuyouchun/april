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

    enum class walk_step_t
    {
        default_        = 0,

        
        pre_validate    = 8,

        validate        = 10,

        post_validate   = 12,


        pre_confirm     = 16,

        confirm         = 18,

        post_confirm    = 20,

        
        pre_analysis    = 24,

        analysis        = 26,

        post_analysis   = 28,

    };

    ////////// ////////// ////////// ////////// //////////

    #define __BRANCH_ENUM_ITEM(name, t)     ((int_type_t<sizeof(t)>)t::name)
    #define __C_BRANCH_ENUM_ITEM(name)      \
        __BRANCH_ENUM_ITEM(name, ::X_ROOT_NS::modules::core::common_branch_value_t)

    X_ENUM_(xbranch_value_t, sizeof(code_element_t))

        asts = __C_BRANCH_ENUM_ITEM(__user_defined__),

        namespace_,

        mname,

        general_type_name,

        type_name_unit,

        array_type_name,

        uncertain_type_name,

        type_def_param,

        type_def_params,

        type_def,

        type_of,

        expressions,

        argument,

        arguments,

        param,

        params,

        generic_param,

        generic_params,

        generic_arg,

        generic_args,

        generic_constraint,

        generic_constraints,

        decorate,

        method_body,

        statements,

        expression_st,

        type_def_st,

        defination_st,

        break_st,

        continue_st,

        throw_st,

        goto_st,

        return_st,

        do_while_st,

        while_st,

        for_st,

        for_each_st,

        if_st,

        switch_st,

        case_,

        try_st,

        catch_,

        empty_st,

        statement_group_st,

        type_name_exp,

        type_cast_exp,

        function,

        index,

        new_,

        array_initializer,

        array_lengths,

        new_array,

        default_value,

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

    X_ENUM(ast_error_code_t)

        mname_empty,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_(common_log_code_t, sizeof(compile_log_code_value_t))

        __info__            = 1000,

        __warning__         = 4000,

        __error__           = 7000,

        duplicate                   = 7001,

        name_empty,
        name_unexpected_first_char,
        name_unexpected_char,

        assembly_name_missing,
        namespace_name_missing,

        type_name_missing,
        argument_missing,
        argument_type_error,
        unexpected_param_default_value,
        condition_missing,
        body_missing,
        iterator_missing,
        statement_missing,
        expression_missing,
        catch_or_finally_missing,
        array_lengths_missing,
        unexpected_params,

        function_generic_args_redundance,

        unexpected_variable_defination,
        unexpected_field_defination,
        unexpected_param_defination,
        unexpected_property_defination,
        unexpected_using_namespace,
        unexpected_import,
        assembly_reference_itself,
        assembly_circular_reference,
        assembly_not_found,

        variable_defination_duplicate,
        variable_undefined,
        type_undefined,
        type_def_unsolved,
        type_defination_duplicate,
        type_def_defination_duplicate,

        type_undetermind,
        unexpected_symbol,
        member_not_found,
        method_not_found,
        method_not_match,
        method_conflict,
        attribute_constructor_not_match,
        type_constructor_not_match,
        unexpected_expression,

        constructor_method_should_no_return_type,
        destructor_method_should_no_return_type,
        static_constructor_method_should_no_return_type,

        destructor_method_should_no_params,
        static_constructor_method_should_no_params,

        constructor_method_not_found,

        partial_specialization_not_supported,

        no_base_type,

        expected_array_length,

        __the_end__         = 10000,

    X_ENUM_END

    namespace
    {
        typedef common_log_code_t __log_code_t, __c_t;
    }

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        template<size_t _child_count>
            using __array_node_base_t = array_ast_node_t<_child_count>;

        ////////// ////////// ////////// ////////// //////////

        template<size_t _child_count, typename value_t, value_t _value>
        class __array_node_t : public __array_node_base_t<_child_count>
        {
        public:
            virtual __value_t value() const override
            {
                return (__value_t)_value;
            }

            virtual const string_t to_string() const override
            {
                return _title(_value);
            }
        };

        ////////// ////////// ////////// ////////// //////////

        namespace
        {
            template<typename t>
            struct __empty_value_t
            {
                static bool is_empty(const t & v) { return v.empty(); }
            };

            template<typename t>
            struct __empty_value_t<t *>
            {
                static bool is_empty(const t * v) { return v == nullptr; }
            };

            template<typename t> bool __is_empty_value(const t & v)
            {
                typedef std::remove_const_t<t> t0;
                return __empty_value_t<t0>::is_empty(v);
            }
        }

        ////////// ////////// ////////// ////////// //////////

        class __utils_t
        {
        public:
            __utils_t(ast_context_t & context) : __context(context) { }

        protected:
            ast_context_t & __context;

            compile_context_t & __compile_context() { return __context.compile_context; }

            global_context_t & __global_context() { return __compile_context().global_context; }

            memory_t * __get_memory() const
            {
                return __context.compile_context.get_memory();
            }

            lang_t * __get_lang() const
            {
                return __context.lang;
            }

            template<typename t, typename ... args_t>
            t * __new_obj(args_t && ... args)
            {
                return __context.compile_context.new_obj<t>(std::forward<args_t>(args) ...);
            }

            template<typename _exp_t, typename ... args_t>
            system_expression_t<_exp_t> * __new_system_expression(args_t && ... args)
            {
                return __new_obj<system_expression_t<_exp_t>>(std::forward<args_t>(args) ...);
            }

            template<typename _value_t>
            system_expression_t<cvalue_expression_t> * __to_value_expression(_value_t value)
            {
                return __new_system_expression<cvalue_expression_t>(
                    __new_obj<cvalue_t>(value)
                );
            }

            template<typename _exp_t, typename ... args_t>
            statement_t * __new_expression_statement(args_t && ... args)
            {
                return __new_obj<expression_statement_t>(
                    __new_system_expression<_exp_t>(std::forward<args_t>(args) ...)
                );
            }

            name_expression_t * __new_field_expression(field_t * field)
            {
                return __new_obj<name_expression_t>(
                    __new_obj<field_variable_t>(field)
                );
            }

            template<typename _it_t>
            expression_t * __parse_expression(_it_t begin, _it_t end)
            {
                return parse_expression(__get_memory(), __context.lang, begin, end);
            }

            template<typename _its_t>
            expression_t * __parse_expression(_its_t && its)
            {
                return __parse_expression(std::begin(its), std::end(its));
            }

            xpool_t & __get_xpool()
            {
                return __context.compile_context.global_context.xpool;
            }

            spool_t & __get_spool()
            {
                return __get_xpool().spool;
            }

            sid_t __to_sid(const string_t & s)
            {
                return __get_spool().to_sid(s);
            }

            name_t __to_name(const string_t & s)
            {
                return name_t(this->__to_sid(s));
            }

            name_t __to_name(token_t * token)
            {
                _A(token != nullptr);
                return __to_name(_str(token));
            }

            template<typename _code_element_t, typename ... args_t>
            void __log(_code_element_t * element, args_t && ... args)
            {
                xlogger_t(__context.logger).logf(
                    as<code_element_t *>(element), std::forward<args_t>(args) ...
                );
            }

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

            bool __validate_name(sid_t sid, __el_t * el, const char_t * title = _T(""))
            {
                return __validate_name(sid.c_str(), el, title);
            }

            bool __validate_name(const name_t & name, __el_t * el, const char_t * title = _T(""))
            {
                return __validate_name(name.sid, el, title);
            }

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

            template<typename _name_t>
            bool __check_name_empty(const _name_t & name, const char_t * title)
            {
                return __check_empty(name, (__el_t *)this, __c_t::name_empty,
                                                    title ? title : _T(""));
            }

            const mname_t * __combine_name(mname_t * name1, mname_t * name2)
            {
                global_context_t & gctx = __global_context();
                return mname_t::combine(
                    to_mname_operate_context(gctx.xpool), name1, name2
                );
            }

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

    template<typename node_data_t>
    class node_wrapper_t : public __ArrayNodeT(node_data_t)
                         , public node_data_t, public __utils_t
    {
        typedef decltype(node_data_t::__end__) __index_t;
        typedef __ArrayNodeT(node_data_t) __base_t;

    public:
        template<typename ... args_t>
        node_wrapper_t(ast_context_t & context, args_t && ... args)
            : __base_t(std::forward<args_t>(args) ...), __utils_t(context)
        { }

        __node_t * child_at(__index_t index) const
        {
            return __base_t::child_at((size_t)index);
        }

        __node_t * operator[](__index_t index) const
        {
            return __base_t::child_at(index);
        }

        void set_child(__index_t index, __node_t * node)
        {
            __base_t::set_child((size_t)index, node);
            node_data_t::on_set_child((size_t)index, node);
        }

        template<typename code_t, typename ... args_t>
        void set_child_with_check(__index_t index, __node_t * node,
                                    code_t duplicate_code, args_t && ... args)
        {
            if(has_child(index))
                this->__log(node, duplicate_code, std::forward<args_t>(args) ...);
            else
                this->set_child(index, node);
        }

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

        void append_child(__index_t index, __node_t * node)
        {
            __multipy_ast_node_at(index)->append(node);
        }

        size_t child_count(__index_t index) const
        {
            __node_t * node = child_at(index);
            return node == nullptr? 0 : node->child_count();
        }

        __node_t * child_at(__index_t index, size_t child_index) const
        {
            __node_t * node = child_at(index);
            return node? node->child_at(child_index) : nullptr;
        }

        void set_child(__index_t index, size_t child_index, __node_t * node)
        {
            multipy_ast_node_t * nodes = __multipy_ast_node_at(index);
            while(nodes->child_count() <= child_index)
            {
                nodes->set_child(child_index, node);
            }
        }

        bool child_empty(__index_t index) const
        {
            return child_count(index) == 0;
        }

        bool has_child(__index_t index) const
        {
            return child_at(index) != nullptr;
        }

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

        template<typename etype_t, typename f_t>
        void each_child(__index_t index, f_t f) const
        {
            typedef teobject_ast_base_t<etype_t> node_t;

            this->each_child_node<node_t>(index, [this, &f](node_t * node) {
                __do_call_f<etype_t>(f, node);
            });
        }

        template<typename etype_t, typename output_itor_t>
        void transform_child(__index_t index, output_itor_t itor) const
        {
            each_child<etype_t>(index, [&itor](etype_t eobject) {
                *itor++ = eobject;
            });
        }

        template<typename container_t, typename etype_t = typename container_t::value_type>
        void transform_child_to(__index_t index, container_t & container) const
        {
            transform_child<typename container_t::value_type>(index, std::back_inserter(container));
        }

        template<typename etype_t, typename output_itor_t, typename pred_t>
        void transform_child(__index_t index, output_itor_t itor, pred_t pred) const
        {
            each_child<etype_t>(index, [&itor, &pred](etype_t eobject) {
                pred(eobject) && (*itor++ = eobject, true);
            });
        }

        template<typename container_t, typename etype_t = typename container_t::value_type,
                 typename pred_t>
        void transform_child_to(__index_t index, container_t & container, pred_t pred) const
        {
            transform_child<typename container_t::value_type>(
                index, std::back_inserter(container), pred
            );
        }

        template<typename etype_t>
        etype_t __to_eobject(__index_t index)
        {
            ast_node_t * child = this->child_at(index);
            if(child != nullptr)
                return _M(eobject_ast_t<etype_t> *, child)->to_eobject();
            return nullptr;
        }

        template<typename etype_t>
        etype_t __try_to_eobject(__index_t index)
        {
            typedef eobject_ast_t<etype_t> ast_t;
            ast_t * ast = as<ast_t *>(this->child_at(index));
            if(ast != nullptr)
                return ast->to_eobject();
            return nullptr;
        }

        template<typename etype_t, typename owner_t>
        etype_t __to_eobject_with_owner(__index_t index, owner_t owner)
        {
            etype_t eobj = __to_eobject<etype_t>(index);
            if(eobj != nullptr)
                eobj->set_owner(owner);

            return eobj;
        }

        virtual __value_t value() const override final
        {
            return (__value_t)node_data_t::value;
        }

    protected:
        void __delay(ast_walk_context_t & context, walk_step_t step, void * tag = nullptr)
        {
            context.delay(this, (int)step, tag);
        }

    private:
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

    template<typename _self_t, typename value_t, value_t _value>
    struct node_data_base_t
    {
        static const value_t value = _value;

        void on_set_child(size_t index, __node_t * node) { }
    };

    ////////// ////////// ////////// ////////// //////////

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

    class __module_ast_node_t : public module_ast_node_t
    {
        typedef module_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
        virtual void do_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // mname

    __EmptyAst(mname, __xcvalue_t::mname)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

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

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
        virtual void on_commit() override;

        void append_part(name_t name, __el_t * el);

        virtual emname_t * to_eobject() override;

    private:
        union
        {
            byte_t  __mname[sizeof(mname_t)];
            byte_t  __emname[sizeof(__w_t<emname_t>)];
        };

        mname_t *  __this_mname()  { return (mname_t *)__mname;   }
        emname_t * __this_emname() { return (emname_t *)__emname; }
    };

    ////////// ////////// ////////// ////////// //////////
    // expressions

    Ast(expressions, __xcvalue_t::expressions)

        expressions,

    EndAst(expressions)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class expressions_ast_node_t : public __expressions_ast_node_t
                                 , public expression_ast_t
    {
        typedef __expressions_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void on_commit() override;
        virtual expression_t * to_eobject() override;

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        expressions_t __expressions;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_name

    Ast(type_name, __cvalue_t::type_name)

        underlying,

    EndAst(type_name)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class type_name_ast_node_t : public __type_name_ast_node_t
                               , public type_name_ast_t
    {
        typedef __type_name_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void on_commit() override;
        virtual type_name_t * to_eobject() override;

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_name_unit_t

    struct type_name_unit_t : public eobject_t
    {
        type_name_unit_t() = default;
        type_name_unit_t(const name_t & name, generic_args_t * args = nullptr)
            : name(name), args(args) { }

        typedef type_name_unit_t * itype_t;

        name_t           name    = name_t::null;
        generic_args_t * args    = nullptr;

        bool operator == (const name_t & name) const
        {
            return !is_generic() && name == this->name;
        }

        bool operator == (const sid_t & sid) const
        {
            return !is_generic() && (sid_t)this->name == sid;
        }

        bool operator != (const name_t & name) const
        {
            return !operator == (name);
        }

        bool operator != (const sid_t & sid) const
        {
            return !operator == (sid);
        }

        generic_arg_t * operator [] (size_t index) const
        {
            return args && index < args->size()? (*args)[index] : nullptr;
        }

        operator string_t() const;
        virtual const string_t to_string() const override { return (string_t)*this; }

        size_t generic_args_count() const { return args? args->size() : 0; }
        bool is_generic() const { return args && !args->empty(); }
        bool is_partial_specialization() const;
        bool is_no_specialization() const { return !args || al::all_of_empty(*args); }
    };

    typedef eobject_ast_t<typename type_name_unit_t::itype_t> type_name_unit_ast_t;

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    X_ENUM(global_type_t)

        root,

        global,

    X_ENUM_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class general_type_name_t : public type_name_t
    {
    public:
        typedef al::small_vector_t<type_name_unit_t *, 3> unit_list_t;
        typedef typename unit_list_t::iterator unit_iterator_t;

        unit_list_t     units;
        global_type_t   global_type = global_type_t::__default__;

        auto begin() const { return units.begin(); }
        auto end()   const { return units.end();   }
        bool empty() const { return units.empty(); }

        template<size_t n> bool equals(const char_t (&s)[n])
        {
            type_name_unit_t * unit;

            return units.size() == 1 && (unit = units[0]) != nullptr
                && al::equals(((sid_t)unit->name).c_str(), s);
        }

        X_TO_STRING
    };

    //-------- ---------- ---------- ---------- ----------

    class array_type_name_t : public type_name_t
    {
    public:
        type_name_t * element_type_name = nullptr;
        al::small_vector_t<expression_t *, 2> dimensions;

        X_TO_STRING
    };

    ////////// ////////// ////////// ////////// //////////

    template<typename _super_t>
    class __type_name_ast_node_base_t : public _super_t
    {
        typedef _super_t __super_t;

    public:
        using __super_t::__super_t;

    protected:
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

    Ast(general_type_name, __xcvalue_t::general_type_name)

        units,

    EndAst(general_type_name)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class general_type_name_ast_node_t
            : public __type_name_ast_node_base_t<__general_type_name_ast_node_t>
            , public type_name_ast_t
    {
        typedef __type_name_ast_node_base_t<__general_type_name_ast_node_t> __super_t;

    public:
        using __super_t::__super_t;

        void set_global_type(global_type_t global_type);

        virtual void on_commit() override;
        virtual type_name_t * to_eobject() override;

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<general_type_name_t> __type_name;

        void __walk_confirm(ast_walk_context_t & context);
    };

    ////////// ////////// ////////// ////////// //////////

    Ast(type_name_unit, __xcvalue_t::type_name_unit)

        generic_args,

    EndAst(type_name_unit)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class type_name_unit_ast_node_t : public __type_name_unit_ast_node_t
                                    , public type_name_unit_ast_t
    {
        typedef __type_name_unit_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        void set_name(name_t name, __el_t * el);

        virtual void on_commit() override;
        virtual type_name_unit_t * to_eobject() override;

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<type_name_unit_t> __type_name_unit;
    };

    ////////// ////////// ////////// ////////// //////////
    // array_type_name

    Ast(array_type_name, __xcvalue_t::array_type_name)

        element_type_name, dimensions,

    EndAst(array_type_name)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class array_type_name_ast_node_t
            : public __type_name_ast_node_base_t<__array_type_name_ast_node_t>
            , public type_name_ast_t
    {
        typedef __type_name_ast_node_base_t<__array_type_name_ast_node_t> __super_t;

    public:
        using __super_t::__super_t;

        virtual void on_commit() override;
        virtual type_name_t * to_eobject() override;

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<array_type_name_t> __type_name;

        void __walk_confirm(ast_walk_context_t & context);
    };

    ////////// ////////// ////////// ////////// //////////
    // uncertain_type_name

    __EmptyAst(uncertain_type_name, __xcvalue_t::uncertain_type_name)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class uncertain_type_name_ast_node_t
            : public __type_name_ast_node_base_t<__uncertain_type_name_ast_node_t>
            , public type_name_ast_t
    {
        typedef __type_name_ast_node_base_t<__uncertain_type_name_ast_node_t> __super_t;

    public:
        using __super_t::__super_t;

        virtual void on_commit() override;
        virtual type_name_t * to_eobject() override;

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        type_name_t * __type_name = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_def_param

    Ast(type_def_param, __xcvalue_t::type_def_param)

    EndAst(type_def_param)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class type_def_param_ast_node_t : public __type_def_param_ast_node_t
                                    , public type_def_param_ast_t
    {
        typedef __type_def_param_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        void set_name(name_t name, __el_t * el);

        virtual void on_commit() override;
        virtual type_def_param_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<type_def_param_t> __type_def_param;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_def_params

    Ast(type_def_params, __xcvalue_t::type_def_params)

        params,

    EndAst(type_def_params)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class type_def_params_ast_node_t : public __type_def_params_ast_node_t
                                     , public type_def_params_ast_t
    {
        typedef __type_def_params_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void on_commit() override;
        virtual type_def_params_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        type_def_params_t __type_def_params;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_def

    Ast(type_def, __xcvalue_t::type_def)

        type_name, params,

    EndAst(type_def)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class type_def_ast_node_t : public __type_def_ast_node_t
                              , public type_def_ast_t
    {
        typedef __type_def_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        void set_name(name_t name, __el_t * el);

        virtual void on_commit() override;
        virtual type_def_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        type_def_t __type_def;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_of

    Ast(type_of, __xcvalue_t::type_of)

        type_name,

    EndAst(type_of)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class type_of_ast_node_t : public __type_of_ast_node_t
                             , public expression_ast_t
    {
        typedef __type_of_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void on_commit() override;
        virtual expression_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __wexp_t<type_of_expression_t> __type_of;
    };

    ////////// ////////// ////////// ////////// //////////
    // argument

    Ast(argument, __xcvalue_t::argument)

        expression, attributes,

    EndAst(argument)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class argument_ast_node_t : public __argument_ast_node_t
                              , public argument_ast_t
    {
        typedef __argument_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        void set_name(name_t name, __el_t * el);
        void set_argument_type(param_type_t atype, __el_t * el);

        virtual void on_commit() override;

        virtual argument_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        argument_t __argument;
    };

    ////////// ////////// ////////// ////////// //////////
    // arguments

    Ast(arguments, __xcvalue_t::arguments)

        arguments,

    EndAst(arguments)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class arguments_ast_node_t : public __arguments_ast_node_t
                               , public arguments_ast_t
    {
        typedef __arguments_ast_node_t __super_t;

    public:
        using __arguments_ast_node_t::__arguments_ast_node_t;

        virtual void on_commit() override;

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
        virtual arguments_t * to_eobject() override;

    private:
        arguments_t __arguments;
    };

    ////////// ////////// ////////// ////////// //////////
    // param

    Ast(param, __xcvalue_t::param)

        type_name, attributes, default_value,

    EndAst(param)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class param_ast_node_t : public __param_ast_node_t
                           , public param_ast_t
    {
        typedef __param_ast_node_t __super_t;

    public:
        using __param_ast_node_t::__param_ast_node_t;

        void set_name(const name_t & name, __el_t * el);
        void set_param_type(param_type_t ptype, __el_t * el);

        virtual void on_commit() override;
        virtual param_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        param_t __param;
    };

    ////////// ////////// ////////// ////////// //////////
    // params

    Ast(params, __xcvalue_t::params)

        params,

    EndAst(params)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class params_ast_node_t : public __params_ast_node_t
                            , public params_ast_t
    {
        typedef __params_ast_node_t __super_t;

    public:
        using __params_ast_node_t::__params_ast_node_t;

        virtual void on_commit() override;
        virtual params_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        params_t __params;
    };

    ////////// ////////// ////////// ////////// //////////
    // generic_param

    Ast(generic_param, __xcvalue_t::generic_param)

        attributes,

    EndAst(generic_param)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class generic_param_ast_node_t : public __generic_param_ast_node_t
                                   , public generic_param_ast_t
    {
        typedef __generic_param_ast_node_t __super_t;

    public:
        using __generic_param_ast_node_t::__generic_param_ast_node_t;

        void set_name(name_t name, __el_t * el);

        virtual void on_commit() override;
        virtual generic_param_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        generic_param_t __param;
    };

    ////////// ////////// ////////// ////////// //////////
    // generic_params

    Ast(generic_params, __xcvalue_t::generic_params)

        params,

    EndAst(generic_params)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class generic_params_ast_node_t : public __generic_params_ast_node_t
                                    , public generic_params_ast_t
    {
        typedef __generic_params_ast_node_t __super_t;

    public:
        using __generic_params_ast_node_t::__generic_params_ast_node_t;

        virtual void on_commit() override;
        virtual generic_params_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        generic_params_t __generic_params;
    };
   
    ////////// ////////// ////////// ////////// //////////
    // generic_constraint

    Ast(generic_constraint, __xcvalue_t::generic_constraint)

        type_names,

    EndAst(generic_constraint)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class generic_constraint_ast_node_t : public __generic_constraint_ast_node_t
                                        , public generic_constraint_ast_t
    {
        typedef __generic_constraint_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        void set_constraint(generic_constraint_ttype_t cttype, __el_t * el);
        void set_name(name_t name, __el_t * el);

        virtual void on_commit() override;
        virtual generic_constraint_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
       generic_constraint_t __generic_constraint;
    };

    ////////// ////////// ////////// ////////// //////////
    // generic_constraints

    Ast(generic_constraints, __xcvalue_t::generic_constraints)

        constraints,

    EndAst(generic_constraints)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class generic_constraints_ast_node_t : public __generic_constraints_ast_node_t
                                         , public generic_constraints_ast_t
    {
        typedef __generic_constraints_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void on_commit() override;
        virtual generic_constraints_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
       generic_constraints_t __generic_constraints;
    };

    ////////// ////////// ////////// ////////// //////////
    // generic_arg

    Ast(generic_arg, __xcvalue_t::generic_arg)

        type_name,

    EndAst(generic_arg)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class generic_arg_ast_node_t : public __generic_arg_ast_node_t
                                 , public generic_arg_ast_t
    {
        typedef __generic_arg_ast_node_t __super_t;

    public:
        using __generic_arg_ast_node_t::__generic_arg_ast_node_t;

        virtual void on_commit() override;
        virtual generic_arg_t * to_eobject() override;

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        generic_arg_t       __arg;
    };

    ////////// ////////// ////////// ////////// //////////
    // generic_args

    Ast(generic_args, __xcvalue_t::generic_args)

        generic_args,

    EndAst(generic_args)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class generic_args_ast_node_t : public __generic_args_ast_node_t
                                  , public generic_args_ast_t
    {
        typedef __generic_args_ast_node_t __super_t;

    public:
        using __generic_args_ast_node_t::__generic_args_ast_node_t;

        virtual void on_commit() override;
        virtual generic_args_t * to_eobject() override;

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        generic_args_t __args;
    };

    ////////// ////////// ////////// ////////// //////////
    // attribute

    Ast(attribute, __cvalue_t::attribute)

        type_name, arguments,

    EndAst(attribute)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class attribute_ast_node_t : public __attribute_ast_node_t
                               , public attribute_ast_t
    {
        typedef __attribute_ast_node_t __super_t;

    public:
        using __attribute_ast_node_t::__attribute_ast_node_t;

        virtual void on_commit() override;

        virtual attribute_t * to_eobject() override;

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        attribute_t __attr;
        attribute_t * __get_compile_time_attribute();
        void __analyze_attribute_constructor();
    };

    ////////// ////////// ////////// ////////// //////////
    // attributes

    Ast(attributes, __cvalue_t::attributes)

        attributes,

    EndAst(attributes)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class attributes_ast_node_t : public __attributes_ast_node_t
                                , public attributes_ast_t
    {
        typedef __attributes_ast_node_t __super_t;

    public:
        using __attributes_ast_node_t::__attributes_ast_node_t;

        virtual void on_commit() override;

        virtual attributes_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        attributes_t __attributes;
    };

    ////////// ////////// ////////// ////////// //////////
    // cvalue

    __EmptyAst(cvalue, __cvalue_t::cvalue)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class cvalue_ast_node_t : public __cvalue_ast_node_t
                            , public cvalue_ast_t
    {
        typedef __cvalue_ast_node_t __super_t;

    public:
        using __cvalue_ast_node_t::__cvalue_ast_node_t; 

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
        virtual cvalue_t * to_eobject() override;

        void set_value(const cvalue_t & value);
        void set_value(const token_data_t & data);

    private:
        cvalue_t __value;
    };

    ////////// ////////// ////////// ////////// //////////
    // decorate

    __EmptyAst(decorate, __xcvalue_t::decorate)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class decorate_ast_node_t : public __decorate_ast_node_t
                              , public decorate_ast_t
    {
        typedef __decorate_ast_node_t __super_t;

    public:
        using __decorate_ast_node_t::__decorate_ast_node_t;

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

        void set_access(access_value_t value, __el_t * el);

        void set_static(__el_t * el);
        void set_sealed(__el_t * el);
        void set_override(__el_t * el);
        void set_virtual(__el_t * el);
        void set_abstract(__el_t * el);
        void set_new(__el_t * el);
        void set_readonly(__el_t * el);
        void set_const(__el_t * el);
        void set_extern(__el_t * el);

        virtual void on_commit() override;
        virtual decorate_t * to_eobject() override;

    private:
        decorate_t __decorate = decorate_t::default_value;
    };

    ////////// ////////// ////////// ////////// //////////
    // statements

    Ast(statements, __xcvalue_t::statements)

        statements,

    EndAst(statements)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class statements_ast_node_t : public __statements_ast_node_t
                                , public statements_ast_t
    {
        typedef __statements_ast_node_t __super_t;

    public:
        using __statements_ast_node_t::__statements_ast_node_t;

        virtual void on_commit() override;
        virtual statements_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        statements_t __statements;
    };

    ////////// ////////// ////////// ////////// //////////
    // method_body

    Ast(method_body, __xcvalue_t::method_body)

        statements,

    EndAst(method_body)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class method_body_ast_node_t : public __method_body_ast_node_t
                                 , public method_body_ast_t
    {
        typedef __method_body_ast_node_t __super_t;

    public:
        using __method_body_ast_node_t::__method_body_ast_node_t;

        virtual void on_commit() override;
        virtual method_body_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        method_body_t __body;
    };

    ////////// ////////// ////////// ////////// //////////
    // expression

    Ast(expression, __cvalue_t::expression)

        nodes,

    EndAst(expression)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class expression_ast_node_t : public __expression_ast_node_t
                                , public expression_ast_t
    {
        typedef analyze_stack_element_t         __el_t;
        typedef al::small_vector_t<__el_t, 3>   __es_t;
        typedef expression_ast_node_t       __self_t;
        typedef __expression_ast_node_t     __super_t;

    public:
        using __expression_ast_node_t::__expression_ast_node_t;

        void append_token(token_t * token);
        void append_name(name_t name, token_t * token);
        void append_expression(ast_node_t * node);
        void append_cvalue(ast_node_t * node);

        virtual void on_commit() override;
        virtual expression_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __es_t __es;
        expression_t * __expression = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////
    // statement

    Ast(statement, __cvalue_t::statement)

        underlying,

    EndAst(statement)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class statement_ast_node_t : public __statement_ast_node_t
                               , public statement_ast_t
    {
        typedef __statement_ast_node_t __super_t;

    public:
        using __statement_ast_node_t::__statement_ast_node_t;

        virtual void on_commit() override;
        virtual statement_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // import

    Ast(import, __cvalue_t::import)

        package, assembly,
        
    EndAst(import)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class import_ast_node_t : public __import_ast_node_t
                            , public import_ast_t
    {
        typedef __import_ast_node_t __super_t;

    public:
        using __import_ast_node_t::__import_ast_node_t;

        void set_alias(name_t name, __el_t * el);

        virtual void on_commit() override;
        virtual import_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        import_t __import;
    };

    ////////// ////////// ////////// ////////// //////////
    // using

    Ast(using, __cvalue_t::using_)

        ns,
        
    EndAst(using)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class using_ast_node_t : public __using_ast_node_t
                           , public using_namespace_ast_t
    {
        typedef __using_ast_node_t __super_t;

    public:
        using __using_ast_node_t::__using_ast_node_t;

        void set_alias(name_t name, __el_t * el);

        virtual void on_commit() override;
        virtual using_namespace_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        using_namespace_t __using_namespace;
    };

    ////////// ////////// ////////// ////////// //////////
    // namespace

    Ast(namespace, __xcvalue_t::namespace_)

        name, type_defs, types, namespaces, 
        
    EndAst(namespace)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class namespace_ast_node_t : public __namespace_ast_node_t
                               , public namespace_ast_t
    {
        typedef __namespace_ast_node_t __super_t;

    public:
        using __namespace_ast_node_t::__namespace_ast_node_t;

        virtual void on_commit() override;
        virtual namespace_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        namespace_t __namespace;
    };

    ////////// ////////// ////////// ////////// //////////
    // type

    Ast(type, __cvalue_t::type)

        attributes, decorate, generic_params, super_type_names,
        type_defs, fields, methods, properties, events, nest_types,
        
    EndAst(type)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class type_ast_node_t : public __type_ast_node_t
                          , public type_ast_t
    {
        typedef __type_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        void set_name(const name_t & name, __el_t * el);
        void set_ttype(ttype_t ttype, __el_t * el);

        void append_member(member_t * member);

        virtual void on_commit() override;
        type_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<general_type_t> __type;

        void __walk_default(ast_walk_context_t & context, int step, void * tag);
        method_t * __append_default_constructor();
        void __walk_analysis(ast_walk_context_t & context, method_t * method);
    };

    ////////// ////////// ////////// ////////// //////////
    // if

    Ast(if, __cvalue_t::statement)

        condition, body,
        
    EndAst(if)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class if_ast_node_t : public __if_ast_node_t
    {
        typedef __if_ast_node_t __super_t;

    public:
        using __if_ast_node_t::__if_ast_node_t;

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // for

    Ast(for, __cvalue_t::statement)

        initializer, condition, expression, body,
        
    EndAst(for)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class for_ast_node_t : public __for_ast_node_t
    {
        typedef __for_ast_node_t __super_t;

    public:
        using __for_ast_node_t::__for_ast_node_t;

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // while

    Ast(while, __cvalue_t::statement)

        condition, body,
        
    EndAst(while)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class while_ast_node_t : public __while_ast_node_t
    {
        typedef __while_ast_node_t __super_t;

    public:
        using __while_ast_node_t::__while_ast_node_t;

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // switch

    Ast(switch, __cvalue_t::statement)

        expression,
        
    EndAst(switch)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class switch_ast_node_t : public __switch_ast_node_t
    {
        typedef __switch_ast_node_t __super_t;

    public:
        using __switch_ast_node_t::__switch_ast_node_t;

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // loop

    Ast(loop, __cvalue_t::statement)

        condition, body,
        
    EndAst(loop)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class loop_ast_node_t : public __loop_ast_node_t
    {
        typedef __loop_ast_node_t __super_t;

    public:
        using __loop_ast_node_t::__loop_ast_node_t;

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // break

    __EmptyAst(break, __cvalue_t::statement)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class break_ast_node_t : public __break_ast_node_t
    {
        typedef __break_ast_node_t __super_t;

    public:
        using __break_ast_node_t::__break_ast_node_t;

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // continue

    __EmptyAst(continue, __cvalue_t::statement)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class continue_ast_node_t : public __continue_ast_node_t
    {
        typedef __continue_ast_node_t __super_t;

    public:
        using __continue_ast_node_t::__continue_ast_node_t;

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // return

    Ast(return, __cvalue_t::statement)

        expression,
        
    EndAst(return)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class return_ast_node_t : public __return_ast_node_t
    {
        typedef __return_ast_node_t __super_t;

    public:
        using __return_ast_node_t::__return_ast_node_t;

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // expression_st

    Ast(expression_st, __xcvalue_t::expression_st)

        expression,
        
    EndAst(expression_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class expression_st_ast_node_t : public __expression_st_ast_node_t
                                   , public statement_ast_t
    {
        typedef __expression_st_ast_node_t __super_t;

    public:
        using __expression_st_ast_node_t::__expression_st_ast_node_t;

        virtual void on_commit() override;
        virtual statement_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<expression_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_def_st

    Ast(type_def_st, __xcvalue_t::type_def_st)

        type_def,
        
    EndAst(type_def_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class type_def_st_ast_node_t : public __type_def_st_ast_node_t
                                 , public statement_ast_t
    {
        typedef __type_def_st_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void on_commit() override;
        virtual statement_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<type_def_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // defination_st

    Ast(defination_st, __xcvalue_t::defination_st)

        type_name, items,

    EndAst(defination_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class defination_st_ast_node_t : public __defination_st_ast_node_t
                                   , public statement_ast_t
    {
        typedef __defination_st_ast_node_t __super_t;

    public:
        using __defination_st_ast_node_t::__defination_st_ast_node_t;

        virtual void on_commit() override;

        virtual statement_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<defination_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // break_st

    __EmptyAst(break_st, __xcvalue_t::break_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class break_st_ast_node_t : public __break_st_ast_node_t
                              , public statement_ast_t
    {
        typedef __break_st_ast_node_t __super_t;

    public:
        using __break_st_ast_node_t::__break_st_ast_node_t;

        virtual statement_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<break_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // continue_st

    __EmptyAst(continue_st, __xcvalue_t::continue_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class continue_st_ast_node_t : public __continue_st_ast_node_t
                                 , public statement_ast_t
    {
        typedef __continue_st_ast_node_t __super_t;

    public:
        using __continue_st_ast_node_t::__continue_st_ast_node_t;

        virtual statement_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<continue_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // throw_st

    Ast(throw_st, __xcvalue_t::throw_st)

        expression,

    EndAst(throw_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class throw_st_ast_node_t : public __throw_st_ast_node_t
                              , public statement_ast_t
    {
        typedef __throw_st_ast_node_t __super_t;

    public:
        using __throw_st_ast_node_t::__throw_st_ast_node_t;

        virtual void on_commit() override;

        virtual statement_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<throw_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // goto_st

    __EmptyAst(goto_st, __xcvalue_t::goto_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class goto_st_ast_node_t : public __goto_st_ast_node_t
                             , public statement_ast_t
    {
        typedef __goto_st_ast_node_t __super_t;

    public:
        using __goto_st_ast_node_t::__goto_st_ast_node_t;

        void set_label(const name_t & label, __el_t * el);
        virtual void on_commit() override;

        virtual statement_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<goto_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // return_st

    Ast(return_st, __xcvalue_t::return_st)

        expression,

    EndAst(return_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class return_st_ast_node_t : public __return_st_ast_node_t
                               , public statement_ast_t
    {
        typedef __return_st_ast_node_t __super_t;

    public:
        using __return_st_ast_node_t::__return_st_ast_node_t;

        virtual void on_commit() override;

        virtual statement_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<return_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // do_while_st

    Ast(do_while_st, __xcvalue_t::do_while_st)

        condition, body,

    EndAst(do_while_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class do_while_st_ast_node_t : public __do_while_st_ast_node_t
                                 , public statement_ast_t
    {
        typedef __do_while_st_ast_node_t __super_t;

    public:
        using __do_while_st_ast_node_t::__do_while_st_ast_node_t;

        virtual void on_commit() override;

        virtual statement_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<do_while_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // while_st

    Ast(while_st, __xcvalue_t::while_st)

        condition, body,

    EndAst(while_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class while_st_ast_node_t : public __while_st_ast_node_t
                              , public statement_ast_t
    {
        typedef __while_st_ast_node_t __super_t;

    public:
        using __while_st_ast_node_t::__while_st_ast_node_t;

        virtual void on_commit() override;

        virtual statement_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<while_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // for_st

    Ast(for_st, __xcvalue_t::for_st)

        initialize, condition, increase, body,

    EndAst(for_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class for_st_ast_node_t : public __for_st_ast_node_t
                            , public statement_ast_t
    {
        typedef __for_st_ast_node_t __super_t;

    public:
        using __for_st_ast_node_t::__for_st_ast_node_t;

        virtual void on_commit() override;

        virtual statement_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<for_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // for_each_st

    Ast(for_each_st, __xcvalue_t::for_each_st)

        type_name, iterator, body,

    EndAst(for_each_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class for_each_st_ast_node_t : public __for_each_st_ast_node_t
                                 , public statement_ast_t
    {
        typedef __for_each_st_ast_node_t __super_t;

    public:
        using __for_each_st_ast_node_t::__for_each_st_ast_node_t;

        void set_variable(name_t name, __el_t * el);

        virtual void on_commit() override;

        virtual statement_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<for_each_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // if_st

    Ast(if_st, __xcvalue_t::if_st)

        condition, if_body, else_body,

    EndAst(if_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class if_st_ast_node_t : public __if_st_ast_node_t
                           , public statement_ast_t
    {
        typedef __if_st_ast_node_t __super_t;

    public:
        using __if_st_ast_node_t::__if_st_ast_node_t;

        virtual void on_commit() override;

        virtual statement_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<if_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // switch_st

    Ast(switch_st, __xcvalue_t::switch_st)

        expression, cases,

    EndAst(switch_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class switch_st_ast_node_t : public __switch_st_ast_node_t
                               , public statement_ast_t
    {
        typedef __switch_st_ast_node_t __super_t;

    public:
        using __switch_st_ast_node_t::__switch_st_ast_node_t;

        virtual void on_commit() override;

        virtual statement_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<switch_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // case

    Ast(case, __xcvalue_t::case_)

        constants, statements,

    EndAst(case)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class case_ast_node_t : public __case_ast_node_t
                          , public case_ast_t
    {
        typedef __case_ast_node_t __super_t;

    public:
        using __case_ast_node_t::__case_ast_node_t;

        void append_default(__el_t * el);

        virtual void on_commit() override;

        virtual case_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<case_t> __case;
    };

    ////////// ////////// ////////// ////////// //////////
    // try_st

    Ast(try_st, __xcvalue_t::try_st)

        try_, catches, finally,
        
    EndAst(try_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class try_st_ast_node_t : public __try_st_ast_node_t
                            , public statement_ast_t
    {
        typedef __try_st_ast_node_t __super_t;

    public:
        using __try_st_ast_node_t::__try_st_ast_node_t;

        virtual void on_commit() override;

        virtual statement_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<try_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // catch

    Ast(catch, __xcvalue_t::catch_)

        type_name, body,
        
    EndAst(catch)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class catch_ast_node_t : public __catch_ast_node_t
                           , public catch_ast_t
    {
        typedef __catch_ast_node_t __super_t;

    public:
        using __catch_ast_node_t::__catch_ast_node_t;

        void set_variable(name_t variable, __el_t * el);

        virtual void on_commit() override;

        virtual catch_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __el_t * __variable_el;
        __w_t<catch_t> __catch;
    };

    ////////// ////////// ////////// ////////// //////////
    // empty_st

    __EmptyAst(empty_st, __xcvalue_t::empty_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class empty_st_ast_node_t : public __empty_st_ast_node_t
                              , public statement_ast_t
    {
        typedef __empty_st_ast_node_t __super_t;

    public:
        using __empty_st_ast_node_t::__empty_st_ast_node_t;

        virtual statement_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<empty_statement_t> __statement;
    };

    ////////// ////////// ////////// ////////// //////////
    // statement_group_st

    Ast(statement_group_st, __xcvalue_t::statement_group_st)

        statements,

    EndAst(statement_group_st)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class statement_group_st_ast_node_t : public __statement_group_st_ast_node_t
                                        , public statement_ast_t
    {
        typedef __statement_group_st_ast_node_t __super_t;

    public:
        using __statement_group_st_ast_node_t::__statement_group_st_ast_node_t;

        virtual void on_commit() override;

        virtual statement_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<statement_group_t> __statement_group;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_name_exp

    Ast(type_name_exp, __xcvalue_t::type_name_exp)

        type_name,

    EndAst(type_name_exp)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class type_name_exp_ast_node_t : public __type_name_exp_ast_node_t
                                   , public expression_ast_t
    {
        typedef __type_name_exp_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void on_commit() override;

        virtual expression_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __wexp_t<type_name_expression_t> __expression;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_cast_exp 

    Ast(type_cast_exp, __xcvalue_t::type_cast_exp)

        type_name, expression,

    EndAst(type_cast_exp)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class type_cast_exp_ast_node_t : public __type_cast_exp_ast_node_t
                                   , public expression_ast_t
    {
        typedef __type_cast_exp_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void on_commit() override;

        virtual expression_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __wexp_t<type_cast_expression_t> __expression;
    };

    ////////// ////////// ////////// ////////// //////////
    // function

    Ast(function, __xcvalue_t::function)

        namex, generic_args, arguments,

    EndAst(function)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class function_ast_node_t : public __function_ast_node_t
                              , public expression_ast_t
    {
        typedef __function_ast_node_t __super_t;

    public:
        using __function_ast_node_t::__function_ast_node_t;

        virtual void on_commit() override;

        virtual expression_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __wexp_t<function_expression_t> __expression;
    };

    ////////// ////////// ////////// ////////// //////////
    // index

    Ast(index, __xcvalue_t::index)

        namex, arguments,

    EndAst(index)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class index_ast_node_t : public __index_ast_node_t
                           , public expression_ast_t
    {
        typedef __index_ast_node_t __super_t;

    public:
        using __index_ast_node_t::__index_ast_node_t;

        virtual void on_commit() override;

        virtual expression_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __wexp_t<index_expression_t> __expression;
    };

    ////////// ////////// ////////// ////////// //////////
    // new

    Ast(new, __xcvalue_t::new_)

        type_name, arguments,

    EndAst(new)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class new_ast_node_t : public __new_ast_node_t
                         , public expression_ast_t
    {
        typedef __new_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void on_commit() override;

        virtual expression_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __wexp_t<new_expression_t> __expression;
    };

    ////////// ////////// ////////// ////////// //////////
    // new_array

    Ast(new_array, __xcvalue_t::new_array)

        type_name, lengths, initializer,

    EndAst(new_array)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class new_array_ast_node_t : public __new_array_ast_node_t
                               , public expression_ast_t
    {
        typedef __new_array_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void on_commit() override;

        virtual expression_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __wexp_t<new_array_expression_t> __expression;
        array_length_t __get_length(dimension_t dimension);
    };

    ////////// ////////// ////////// ////////// //////////
    // array_initializer

    Ast(array_initializer, __xcvalue_t::array_initializer)

        elements,

    EndAst(array_initializer)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class array_initializer_ast_node_t : public __array_initializer_ast_node_t
                                       , public array_initializer_ast_t
    {
        typedef __array_initializer_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void on_commit() override;

        virtual array_initializer_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        array_initializer_t __initializer;
    };

    ////////// ////////// ////////// ////////// //////////
    // array_lengths

    Ast(array_lengths, __xcvalue_t::array_lengths)

        lengths,

    EndAst(array_lengths)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class array_lengths_ast_node_t : public __array_lengths_ast_node_t
                                   , public array_lengths_ast_t
    {
        typedef __array_lengths_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void on_commit() override;

        virtual array_lengths_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        array_lengths_t __lengths;
    };


    ////////// ////////// ////////// ////////// //////////
    // default_value

    Ast(default_value, __xcvalue_t::default_value)

        type_name,

    EndAst(default_value)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class default_value_ast_node_t : public __default_value_ast_node_t
                                   , public expression_ast_t
    {
        typedef __default_value_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void on_commit() override;

        virtual expression_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __wexp_t<default_value_expression_t> __expression;
    };

    ////////// ////////// ////////// ////////// //////////
    // field

    Ast(field, __cvalue_t::field)

        attributes, decorate, type_name, init_value,
        
    EndAst(field)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class field_ast_node_t : public __field_ast_node_t
                           , public field_ast_t
    {
        typedef __field_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        void set_name(name_t name, __el_t * el);

        virtual void on_commit() override;
        virtual field_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<field_t> __field;
    };

    ////////// ////////// ////////// ////////// //////////
    // method

    Ast(method, __cvalue_t::method)

        attributes, decorate, owner_type_name, type_name, generic_params, params,
        base_ctor_args, body,

    EndAst(method)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class method_ast_node_t : public __method_ast_node_t
                            , public method_ast_t
    {
        typedef __method_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        void set_name(name_t name, __el_t * el);
        void set_trait(method_trait_t trait, __el_t * el);

        virtual void on_commit() override;
        virtual method_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<method_t> __method;

        void __walk_default(ast_walk_context_t & context);
        void __walk_analysis(ast_walk_context_t & context);
    };

    ////////// ////////// ////////// ////////// //////////
    // property

    Ast(property, __cvalue_t::property)

        attributes, decorate, owner_type_name, type_name, get_method, set_method, params,
        
    EndAst(property)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class property_ast_node_t : public __property_ast_node_t
                              , public property_ast_t
    {
        typedef __property_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        void set_name(name_t name, __el_t * el);

        virtual void on_commit() override;
        virtual property_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<property_t> __property;
    };

    ////////// ////////// ////////// ////////// //////////
    // event

    Ast(event, __cvalue_t::event)

        attributes, decorate, owner_type_name, type_name, add_method, remove_method,
        
    EndAst(event)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class event_ast_node_t : public __event_ast_node_t
                           , public event_ast_t
    {
        typedef __event_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        void set_name(name_t name, __el_t * el);

        virtual void on_commit() override;
        virtual event_t * to_eobject() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        __w_t<event_t> __event;
    };

    ////////// ////////// ////////// ////////// //////////
    // asts

    template<element_value_t value>
    class asts_node_t : public tmultipy_ast_node_t<value>
                      , public asts_t, public __utils_t
    {
        typedef tmultipy_ast_node_t<value> __super_t;

    public:
        template<typename ... args_t>
        asts_node_t(ast_context_t & context, args_t && ... args)
            : __utils_t(context) { }

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override
        {
            __super_t::on_walk(context, step, tag);
        }

        virtual size_t eobject_count() const override
        {
            return this->child_count();
        }

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

