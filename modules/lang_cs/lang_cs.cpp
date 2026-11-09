
#include <lang_cs.h>
#include "cs_analyze_tree.h"
#include "services.h"

namespace X_ROOT_NS::modules::lang_cs {

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
    cs_lang_t::cs_lang_t(const lang_create_args_t & args)
        : lang_base_t(args.lang_id, _T("csharp"), __default_name__)
    {
        set_service(new_obj<cs_lang_service_t>(this));
    }

    const char_t * const cs_lang_t::__default_name__ = _T("cs");

    ////////// ////////// ////////// ////////// //////////

}   // X_ROOT_NS::modules::lang_cs

