
#ifndef __LANG_LQ_H__
#define __LANG_LQ_H__

#include <compile.h>
#include <modules/lang_cs.h>

namespace X_ROOT_NS::modules::lang_lq {

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        using namespace compile;
        using namespace core;
    }

    ////////// ////////// ////////// ////////// //////////

    class lq_lang_service_t;

    // Language lq: the query expressions, it is embedded in the code of another language
    // (<%lq from x in xs select x %>). Its code is that of cs, with the clauses of a query.
    class lq_lang_t : public lang_base_t
    {
    public:

        // Constructor.
        lq_lang_t(const lang_create_args_t & args);

        static const char_t * const __default_name__;

        X_TO_STRING_IMPL(_T("lq_lang_t"))
    };

    ////////// ////////// ////////// ////////// //////////

}   // X_ROOT_NS::modules::lang_lq

#endif  // __LANG_LQ_H__
