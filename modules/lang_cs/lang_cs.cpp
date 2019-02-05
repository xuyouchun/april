
#include <lang_cs.h>
#include "cs_analyze_tree.h"
#include "services.h"

namespace X_ROOT_NS { namespace modules { namespace lang_cs {

    using namespace core;
    using namespace compile;

    typedef analyze_node_key_t __node_key_t;
    typedef compile::analyze_stack_element_t __stack_element_t;

    ////////// ////////// ////////// ////////// //////////

    // Cs log codes.
    X_ENUM_INFO(cs_log_code_t)

        // Unexpected.
        X_D(unexpected,     _T("unexpected \"%1%\""))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Constructor.
    cs_lang_t::cs_lang_t(const lang_create_args_t & args) : __lang_id(args.lang_id)
    {
        __service = memory_t::new_obj<cs_lang_service_t>(
            &__pool, this
        );
    }

    // Returns service of specified type.
    lang_service_t * cs_lang_t::get_service(lang_service_type_t service_type)
    {
        typedef lang_service_type_t t;

        switch(service_type)
        {
            case t::token_read:
                return __get_service<lang_token_read_service_t>();

            case t::token_property:
                return __get_service<lang_token_property_service_t>();

            case t::operator_property:
                return __get_service<lang_operator_property_service_t>();

            case t::expression_build:
                return __get_service<lang_expression_build_service_t>();

            case t::statement_analyze:
                return __get_service<lang_statement_analyze_service_t>();

            case t::ast_build:
                return __get_service<lang_ast_build_service_t>();

            default:
                return nullptr;
        }
    }

    // Returns service.
    template<typename service_t>
    service_t * cs_lang_t::__get_service()
    {
        return (service_t *)(cs_lang_service_t *)__service;
    }

    // Returns language id.
    lang_id_t cs_lang_t::get_id()
    {
        return __lang_id;
    }

    const char_t * const cs_lang_t::__default_name__ = _T("cs");

    ////////// ////////// ////////// ////////// //////////

} } }  // X_ROOT_NS::modules::lang_cs
