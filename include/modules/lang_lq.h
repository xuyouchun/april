
#ifndef __LANG_LQ_H__
#define __LANG_LQ_H__

#include <compile.h>

namespace X_ROOT_NS { namespace modules { namespace lang_lq {

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        using namespace compile;
        using namespace core;
    }

    ////////// ////////// ////////// ////////// //////////

    class lq_lang_t : public lang_t, public object_t
    {
    public:
        lq_lang_t(const lang_create_args_t & args);

        virtual lang_service_t * get_service(lang_service_type_t service_type) override;
        virtual lang_id_t get_id() override;

        static const char_t * const __default_name__;

    private:
        lang_id_t __lang_id;
    };

    ////////// ////////// ////////// ////////// //////////

    /*
    class lq_codedom_analyzer_t : public __codedom_analyzer_t
    {
    public:
        lq_codedom_analyzer_t(__compile_context_t * context) : __context(context)
        {
            _A(context != nullptr);
        }

        virtual __codedom_node_t * push(__code_section_t * section,
                                    __codedom_node_t * current) override;
        virtual __codedom_node_t * push(__codedom_node_t * node) override;
        virtual __codedom_node_t * commit() override;

    private:
        static __analyze_tree_t * __analyze_tree();
        __compile_context_t * __context;
    };*/

    ////////// ////////// ////////// ////////// //////////

} } }  // X_ROOT_NS::modules::lang_lq

#endif  // __LANG_LQ_H__
