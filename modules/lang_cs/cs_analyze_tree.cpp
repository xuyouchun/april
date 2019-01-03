
#include "cs_analyze_tree.h"

namespace X_ROOT_NS { namespace modules { namespace lang_cs {

    using namespace compile;

    #include "text.csec"
    const char_t * const __cs_pattern = __pattern;

    ////////// ////////// ////////// ////////// //////////

    analyze_node_value_t get_node_value(const string_t & name, analyze_node_type_t node_type)
    {
        typedef analyze_node_value_t __node_value_t;
        #define __NodeKeyOfName(type_t, name) ((__node_value_t)(type_t)X_ET(type_t, name))

        switch(node_type)
        {
            case __node_type_t::token:
                return __NodeKeyOfName(cs_token_value_t, name);

            case __node_type_t::branch:
                return __NodeKeyOfName(cs_branch_type_t, name);

            default:
                return -1;
        }

        #undef __NodeKeyOfName
    }

    const char_t * get_analyze_tree_pattern()
    {
        return __cs_pattern;
    }

    ////////// ////////// ////////// ////////// //////////

    X_BRANCH_ENUM_INFO(cs_branch_type_t)

        X_C(_attribute_group,           _T("_attribute_group"))
        X_C(_attribute_group_item,      _T("_attribute_group.__item"))
        X_C(_attribute_group_assign,    _T("_attribute_group.__assign"))
        X_C(_attribute_type_name,       _T("_attribute_type_name"))

        X_C(_decorate_complex,          _T("decorate.__complex"))
        X_C(_defination_st_item,        _T("defination_st.__item"))
        X_C(_fake_method,               _T("_fake_method"))

        X_C(_enum_fields,               _T("_enum_fields"))
        X_C(_enum_fields_item,          _T("_enum_fields.__item"))

        X_C(_fields,                    _T("_fields"))
        X_C(_fields_item,               _T("_fields.__item"))

        X_C(_single_expression,         _T("_single_expression"))
        X_C(_function_name_expression,  _T("_function_name_expression"))

    X_BRANCH_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

} } }  // X_ROOT_NS::modules::lang_cs

