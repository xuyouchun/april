
#ifndef __LANG_CS_H__
#define __LANG_CS_H__

#include <compile.h>
#include <modules/lang_cs/__cs_token_type.h>

namespace X_ROOT_NS::modules::lang_cs {

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        using namespace core;
        using namespace compile;
    }

    ////////// ////////// ////////// ////////// //////////

    // CS language.
    class cs_lang_t : public object_t, public lang_t
    {
    public:

        // Constructor.
        cs_lang_t(const lang_create_args_t & args);

        // Returns service of type.
        virtual lang_service_t * get_service(lang_service_type_t service_type) override;

        // Returns language id.
        virtual lang_id_t get_id() override;

        // Returns language information.
        virtual const lang_info_t * get_info() override;

        static const char_t * const __default_name__;

        X_TO_STRING_IMPL(_T("cs_lang_t"))

    private:
        object_t *  __service;
        pool_t      __pool;
        lang_id_t   __lang_id;
        lang_info_t __lang_info;

        template<typename service_t> service_t * __get_service();
    };

    ////////// ////////// ////////// ////////// //////////

    // Cs log code.
    X_ENUM_(cs_log_code_t, sizeof(common_log_code_t))

        // informations
        __info__    = 10000,

        // warnings
        __warning__ = 14000,

        // errors
        __error__   = 17000,

        // Unexpected.
        unexpected  = 17100,

        // the end
        __the_end__ = 20000,

    X_ENUM_END

    #undef __LogCode

    ////////// ////////// ////////// ////////// //////////

    // Cs token enumerator.
    class cs_token_enumerator_t : public token_enumerator_base_t
    {
    public:

        // Constructor.
        template<typename ... _args_t>
        cs_token_enumerator_t(_args_t && ... args)
            : token_enumerator_base_t(std::forward<_args_t>(args) ...)
        { }

        // Returns next token.
        virtual token_t * next() override;

        X_TO_STRING_IMPL(_T("cs_token_enumerator_t"))

    private:
        token_t * __next_token = nullptr;
    };

    typedef ttoken_reader_t<cs_token_enumerator_t> cs_token_reader_t;

    ////////// ////////// ////////// ////////// //////////

}   // X_ROOT_NS::modules::lang_cs

#endif  // __LANG_CS_H__
