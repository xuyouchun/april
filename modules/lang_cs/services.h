#ifndef __LANG_CS_SERVICES_H__
#define __LANG_CS_SERVICES_H__

#include <modules/lang_cs.h>

namespace X_ROOT_NS { namespace modules { namespace lang_cs {

    namespace
    {
        using namespace core;
        using namespace compile;
    }

    ////////// ////////// ////////// ////////// //////////

    class cs_lang_service_t
        : public object_t
        , public lang_token_property_service_t
        , public lang_operator_property_service_t
        , public lang_expression_build_service_t
        , public lang_token_read_service_t
        , public lang_statement_analyze_service_t
        , public lang_ast_build_service_t
    {
    public:
        cs_lang_service_t(cs_lang_t * lang) : __lang(lang)
        {
            _A(lang != nullptr);
        }

        // lang_token_read_service_t
        virtual token_reader_t * create_token_reader(token_reader_context_t & context) override;

        // lang_token_property_service_t
        virtual const token_property_t * get_token_property(token_value_t value) override;

        // lang_operator_property_service_t
        virtual const operator_property_t * get_operator_property(
                                                        token_value_t token_value) override;

        // lang_expression_build_service_t
        virtual const expression_box_property_t * get_expression_box_property(
                                                            token_value_t value) override;

        virtual expression_t * build_expression(lang_expression_build_context_t & ctx,
                                    const lang_expression_build_args_t & args) override;

        // lang_statement_analyze_service_t
        virtual const string_t get_analyze_pattern(global_context_t & context) override;

        virtual analyze_node_value_t get_node_value(global_context_t & context,
                        const string_t & name, analyze_node_type_t node_type) override;

        // lang_ast_build_service_t
        virtual ast_node_t * build_ast(ast_context_t & context,
                                                    lang_ast_build_args_t & args) override;

        X_TO_STRING_IMPL(_T("cs_lang_service_t"))

    private:
        cs_lang_t * __lang;
    };

    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::lang_cs

#endif //__LANG_CS_SERVICES_H__
