
#include "lq_service.h"

namespace X_ROOT_NS::modules::lang_lq {

    using namespace core;
    using namespace compile;

    ////////// ////////// ////////// ////////// //////////

    // Constructor: the services of cs, with the patterns, the nodes and the tokens of its own.
    lq_lang_t::lq_lang_t(const lang_create_args_t & args)
        : lang_base_t(args.lang_id, _T("linq"), __default_name__)
    {
        auto * cs_lang = new_obj<lang_cs::cs_lang_t>(args);
        set_service(new_obj<lq_lang_service_t>(new_obj<lang_cs::cs_lang_service_t>(cs_lang)));
    }

    const char_t * const lq_lang_t::__default_name__ = _T("lq");

    ////////// ////////// ////////// ////////// //////////

}   // X_ROOT_NS::modules::lang_lq
