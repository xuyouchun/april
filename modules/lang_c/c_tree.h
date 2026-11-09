
#ifndef __LANG_C_TREE_H__
#define __LANG_C_TREE_H__

#include <lang_c.h>

namespace X_ROOT_NS::modules::lang_c {

    namespace
    {
        using namespace core;
        using namespace compile;
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns the patterns of the language: those of cs, with the patterns of c.
    const string_t get_c_analyze_pattern();

    // Returns the value of a node of c, or -1 when the name is not one of c.
    analyze_node_value_t get_c_node_value(const string_t & name);

    // The token -> of c: p->x is p.Value.x. Its value is not one of the tokens of cs.
    const token_value_t c_ptr_member_token_value = 4096;

    ////////// ////////// ////////// ////////// //////////

    // The branches of c: their values are after those of cs, and those of lq.
    X_BRANCH_ENUM(c_branch_type_t)

        c_stmt = __user_defined__ + 1024,

        c_deref,

        c_address,

        c_pointer_type_name,

        c_sizeof,

        c_stackalloc,

    X_BRANCH_ENUM_END

    // Returns whether the value is one of the branches of c.
    X_INLINE bool is_c_branch(analyze_node_value_t value)
    {
        return (long)value >= (long)c_branch_type_t::c_stmt;
    }

    ////////// ////////// ////////// ////////// //////////

    // Builds the ast node of a branch of c.
    ast_node_t * build_c_ast(ast_context_t & context, lang_ast_build_args_t & args);

    // Builds the ast node of a simple expression of cs (it has the token ->).
    ast_node_t * build_c_single_expression(ast_context_t & context, lang_ast_build_args_t & args);

    ////////// ////////// ////////// ////////// //////////

}   // X_ROOT_NS::modules::lang_c

#endif  // __LANG_C_TREE_H__
