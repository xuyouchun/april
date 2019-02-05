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

} } }  // namespace X_ROOT_NS::modules::compile

#endif  // __COMPILE_AST_UTILSH__
