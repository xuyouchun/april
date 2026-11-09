
#ifndef __LANG_LQ_SERVICE_H__
#define __LANG_LQ_SERVICE_H__

#include <lang_lq.h>
#include "lq_tree.h"
#include "../lang_cs/services.h"

namespace X_ROOT_NS::modules::lang_lq {

    ////////// ////////// ////////// ////////// //////////

    // The services of lq: the services of cs, but the patterns, the values of the nodes and the
    // building of the ast nodes of the clauses of a query, and the contextual keywords of lq.
    class lq_lang_service_t : public lang_service_extension_t
    {
        typedef lang_service_extension_t __super_t;

    public:
        lq_lang_service_t(lang_service_base_t * cs_service) : __super_t(cs_service) { }

        // lang_token_read_service_t
        virtual token_reader_t * create_token_reader(token_reader_context_t & context) override;

        // lang_token_property_service_t
        virtual const string_t get_branch_string(analyze_node_value_t value) override;

        // lang_statement_analyze_service_t
        virtual const string_t get_analyze_pattern(global_context_t & context) override;

        virtual analyze_node_value_t get_node_value(global_context_t & context,
                        const string_t & name, analyze_node_type_t node_type) override;

        // lang_ast_build_service_t
        virtual ast_node_t * build_ast(ast_context_t & context,
                                                    lang_ast_build_args_t & args) override;

        // Detect missing element when compile format error: select is the first that a query needs.
        virtual detect_missing_element_result detect_missing_element(ast_context_t & ast_context,
            analyzer_element_reader_t & reader, const analyze_node_keys_t & possible_keys,
            const code_unit_t * cu) override;

        X_TO_STRING_IMPL(_T("lq_lang_service_t"))
    };

    ////////// ////////// ////////// ////////// //////////

}   // X_ROOT_NS::modules::lang_lq

#endif  // __LANG_LQ_SERVICE_H__
