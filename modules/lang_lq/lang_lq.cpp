
#include <lang_lq.h>

namespace X_ROOT_NS { namespace modules { namespace lang_lq {

    using namespace core;
    using namespace compile;

    ////////// ////////// ////////// ////////// //////////

    /*codedom_analyzer_t * lq_lang_t::create_analyzer(compile_context_t * context)
    {
        return context->new_obj<lq_codedom_analyzer_t>(context);
    }*/

    // Constructor.
    lq_lang_t::lq_lang_t(const lang_create_args_t & args) : __lang_id(args.lang_id)
    {

    }

    // Gets service.
    lang_service_t * lq_lang_t::get_service(lang_service_type_t service_type)
    {
        return nullptr;
    }

    // Gets language id.
    lang_id_t lq_lang_t::get_id()
    {
        return __lang_id;
    }

    const char_t * const lq_lang_t::__default_name__ = _T("lq");

    ////////// ////////// ////////// ////////// //////////

    /*
    codedom_node_t * lq_codedom_analyzer_t::push(code_section_t * section,
                                                 codedom_node_t * current)
    {
        return nullptr;
    }

    codedom_node_t * lq_codedom_analyzer_t::push(codedom_node_t * node)
    {
        return nullptr;
    }

    __codedom_node_t * lq_codedom_analyzer_t::commit()
    {
        return nullptr;
    }
    */

    ////////// ////////// ////////// ////////// //////////

} } }  // X_ROOT_NS::modules::lang_lq

