
#ifndef __LANG_CS_ANALYZE_TREE_H__
#define __LANG_CS_ANALYZE_TREE_H__

#include <lang_cs.h>
#include <compile.h>

namespace X_ROOT_NS { namespace modules { namespace lang_cs {

    namespace
    {
        using namespace core;
        using namespace compile;
    }

    ////////// ////////// ////////// ////////// //////////

    analyze_node_value_t get_node_value(const string_t & name, analyze_node_type_t node_type);
    const char_t * get_analyze_tree_pattern();

    ////////// ////////// ////////// ////////// //////////

    X_BRANCH_ENUM(cs_branch_type_t)

        _decorate_complex,
        
        _attribute_group,

        _attribute_group_item,

        _attribute_group_assign,

        _attribute_type_name,

        _defination_st_item,

        _fake_method,

        _enum_fields,

        _enum_fields_item,

        _fields,

        _fields_item,

        _single_expression,

        _function_name_expression,

        _operator,

    X_BRANCH_ENUM_END

    ////////// ////////// ////////// ////////// //////////

} } }  // X_ROOT_NS::modules::lang_cs

#endif  // __LANG_CS_ANALYZE_TREE_H__

