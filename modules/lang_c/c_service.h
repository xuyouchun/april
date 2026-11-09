
#ifndef __LANG_C_SERVICE_H__
#define __LANG_C_SERVICE_H__

#include <lang_c.h>
#include "c_tree.h"
#include "../lang_cs/services.h"

namespace X_ROOT_NS::modules::lang_c {

    ////////// ////////// ////////// ////////// //////////

    // The services of c: the services of cs, but the patterns, the values of the nodes and the building of the
    // ast nodes of the syntax of the pointers.
    class c_lang_service_t : public lang_service_extension_t
    {
        typedef lang_service_extension_t __super_t;

    public:
        c_lang_service_t(lang_service_base_t * cs_service) : __super_t(cs_service) { }

        // lang_token_read_service_t
        virtual token_reader_t * create_token_reader(token_reader_context_t & context) override;

        // lang_token_property_service_t
        virtual const token_property_t * get_token_property(token_value_t value) override;

        virtual const string_t get_token_string(token_value_t value) override;

        virtual const string_t get_branch_string(analyze_node_value_t value) override;

        // lang_statement_analyze_service_t
        virtual const string_t get_analyze_pattern(global_context_t & context) override;

        virtual analyze_node_value_t get_node_value(global_context_t & context,
                        const string_t & name, analyze_node_type_t node_type) override;

        // lang_ast_build_service_t
        virtual ast_node_t * build_ast(ast_context_t & context,
                                                    lang_ast_build_args_t & args) override;

        X_TO_STRING_IMPL(_T("c_lang_service_t"))
    };

    ////////// ////////// ////////// ////////// //////////

}   // X_ROOT_NS::modules::lang_c

#endif  // __LANG_C_SERVICE_H__
