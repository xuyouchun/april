#ifndef __COMPILE_AST_UTILS_H__
#define __COMPILE_AST_UTILS_H__

namespace X_ROOT_NS { namespace modules { namespace compile {

    namespace
    {
        using namespace core;
    }

    ////////// ////////// ////////// ////////// //////////

    // Converts namespace to multi-name.
    const mname_t * ns_to_full_name(namespace_t * ns);

    ////////// ////////// ////////// ////////// //////////

    // Ascertaint type error codes.
    X_ENUM(ascertain_type_error_t)

        // Unknown type.
        unknown_type,

        // Element type empty.
        element_type_empty,

        // Element type not supported.
        element_type_not_supported,

        // Typedef unsolved.
        type_def_unsolved,

    X_ENUM_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class general_type_name_t;
    class array_type_name_t;

    // Ascertains type.
    type_t * ascertain_type(ast_context_t & cctx, ast_walk_context_t & wctx,
                                            general_type_name_t * type_name);

    // Ascertains type.
    type_t * ascertain_type(ast_context_t & cctx, ast_walk_context_t & wctx,
                                            array_type_name_t * type_name);

    // Walks expression.
    void walk_expression(ast_context_t & cctx, ast_walk_context_t & wctx,
                                            expression_t * expression);

    // Gets expression type.
    type_t * get_expression_type(ast_context_t & cctx, expression_t * expression);

	// Picks type from current context.
	type_t * pick_type_from_current_context(xpool_t & xpool, expression_t * expression);

    // Logs message.
    template<typename _code_element_t, typename ... args_t>
    void ast_log(ast_context_t & cctx, _code_element_t * element, args_t && ... args)
    {
        xlogger_t(cctx.logger).logf(as<code_element_t *>(element), std::forward<args_t>(args) ...);
    }

    ////////// ////////// ////////// ////////// //////////

    // Finds method.
    method_t * find_method(ast_context_t & cctx, type_t * type, method_trait_t trait, name_t name,
                generic_args_t * generic_args, atypes_t * atypes);

    // Finds method.
    method_t * find_method(ast_context_t & cctx, type_t * type, method_trait_t trait, name_t name,
                generic_args_t * generic_args, arguments_t * arguments);

    // Finds constructor.
    method_t * find_constructor(ast_context_t & cctx, type_t * type, atypes_t * atypes);

    // Finds constructors.
    method_t * find_constructor(ast_context_t & cctx, type_t * type, arguments_t * arguments);

    // Finds static constructor.
    method_t * find_static_constructor(ast_context_t & cctx, type_t * type);

    // Finds destructor.
    method_t * find_destructor(ast_context_t & cctx, type_t * type);

    // Finds methods.
    void find_methods(ast_context_t & cctx, type_t * type, method_trait_t trait, name_t name,
                generic_args_t * generic_args, arguments_t * arguments);

    // Converts to argument types string.
    string_t to_arg_types_str(ast_context_t & cctx, arguments_t * arguments);

    ////////// ////////// ////////// ////////// //////////

    // An element wrapper.
    class code_element_like_t : public code_element_t
    {
        enum __type_t { __type_null, __type_object, __type_element };

    public:

        // Constructors.
        code_element_like_t() : __type(__type_null) { }
        code_element_like_t(std::nullptr_t) : __type(__type_null) { }

        // Constructors.
        explicit code_element_like_t(object_t * object)
            : __object(object), __type(__type_object) { }

        code_element_like_t(code_element_t * element)
            : __element(element), __type(__type_element) { }

        // Converts to bool value.
        operator bool() const { return (code_element_t *)*this != nullptr; }

        // Returns whether it's empty.
        bool operator == (std::nullptr_t) const { return (bool)*this; }

        // Returns whether it's not empty.
        bool operator != (std::nullptr_t) const { return !(bool)*this; }

        // Converts to code_element_t *.
        operator code_element_t *() const
        {
            switch(__type)
            {
                case __type_null:       return nullptr;
                case __type_object:     return as<code_element_t *>(__object);
                case __type_element:    return __element;
                default:                X_UNEXPECTED();
            }
        }

    private:
        union
        {
            object_t       * __object;
            code_element_t * __element;
        };

        __type_t __type;
    };

    //-------- ---------- ---------- ---------- ----------

    // Variable defination util.
    class variable_defination_t : public object_t
    {
        typedef object_t                __super_t;
        typedef variable_defination_t   __self_t;
        typedef code_element_like_t     __code_element_t;

    public:

        // Constructors.
        variable_defination_t(ast_context_t & cctx, ast_walk_context_t & wctx,
                                                      __code_element_t element)
            : __cctx(cctx), __wctx(wctx), __element(element)
        {
            __region = __wctx.current_region();
        }

        // Constructors.
        variable_defination_t(ast_context_t & cctx, ast_walk_context_t & wctx,
                                                      object_t * object)
            : __self_t(cctx, wctx, __code_element_t(object))
        { }

        // Defines local variable.
        local_variable_t * define_local(type_name_t * type_name, name_t name, bool constant = false, 
                    expression_t * expression = nullptr, __code_element_t element = nullptr);

        // Defines local variable.
        local_variable_t * define_local(type_name_t * type_name, name_t name, bool constant,
                    expression_t * expression, object_t * element)
        {
            return define_local(type_name, name, constant, expression, __code_element_t(element));
        }

        // Defines param variable.
        param_variable_t * define_param(param_t * param);

        // Defines field variable.
        field_variable_t * define_field(field_t * field);

        // Defines property variable.
        property_variable_t * define_property(property_t * property);

		// Defines method variable.
		method_variable_t * define_method(method_t * method);

        // Returns whether it has no error.
        operator bool() const { return __region != nullptr; }

    private:
        ast_context_t      & __cctx;
        ast_walk_context_t & __wctx;
        __code_element_t     __element;
        variable_region_t  * __region;

        // Logs
        template<typename _code_t>
        void __log(_code_t code, const string_t & name, __code_element_t element = nullptr)
        {
            code_element_t * e = (code_element_t *)element;
            if(e != nullptr)
                e = (code_element_t *)__element;

            ast_log(__cctx, e, code, name);
        }

        // Deals defination error.
        void __deal_error(const logic_error_t<ast_error_t> & e, name_t name,
                                                                __code_element_t element);

        // Deals defination error.
        void __deal_error(const logic_error_t<ast_error_t> & e, name_t name, object_t * element)
        {
            __deal_error(e, name, __code_element_t(element));
        }
    };

    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::compile

#endif  // __COMPILE_AST_UTILSH__
