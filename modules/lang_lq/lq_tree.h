
#ifndef __LANG_LQ_TREE_H__
#define __LANG_LQ_TREE_H__

#include <lang_lq.h>

namespace X_ROOT_NS::modules::lang_lq {

    namespace
    {
        using namespace core;
        using namespace compile;
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns the patterns of the language: those of cs, with the patterns of lq.
    const string_t get_lq_analyze_pattern();

    // Returns the value of a node of lq, or -1 when the name is not one of lq.
    analyze_node_value_t get_lq_node_value(const string_t & name);

    ////////// ////////// ////////// ////////// //////////

    // The branches of lq: their values are after those of cs.
    X_BRANCH_ENUM(lq_branch_type_t)

        lq_query = __user_defined__ + 512,

        lq_from,

        lq_let,

        lq_where,

        lq_join,

        lq_ordering,

        lq_orderby,

        lq_select,

        lq_group,

        lq_into,

    X_BRANCH_ENUM_END

    // Returns whether the value is one of the branches of lq.
    X_INLINE bool is_lq_branch(analyze_node_value_t value)
    {
        return (long)value >= (long)lq_branch_type_t::lq_query;
    }

    ////////// ////////// ////////// ////////// //////////

    // Builds the ast node of a branch of lq.
    ast_node_t * build_lq_ast(ast_context_t & context, lang_ast_build_args_t & args);

    ////////// ////////// ////////// ////////// //////////

}   // X_ROOT_NS::modules::lang_lq

#endif  // __LANG_LQ_TREE_H__
