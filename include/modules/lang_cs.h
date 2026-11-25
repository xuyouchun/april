
#ifndef __LANG_CS_H__
#define __LANG_CS_H__

#include <compile.h>
#include <deque>
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
    class cs_lang_t : public lang_base_t
    {
    public:

        // Constructor.
        cs_lang_t(const lang_create_args_t & args);

        static const char_t * const __default_name__;

        X_TO_STRING_IMPL(_T("cs_lang_t"))
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
        // The tokens that were made and are not given yet (the rest of a string that has holes, see cs_interpolated.cpp).
        std::deque<token_t *> __pending;

        // Reads a string that has holes ($"..{x}.."), or a raw string ("""...""").
        token_t * __read_interpolated(const char_t * p0, int dollars, bool verbatim);

        // Reads a number, or a decimal: the number that ends with m (1.5m) is the text of it, the character 1 is its first one.
        token_data_t * __read_number_or_decimal();

        token_t * __next_token = nullptr;
        bool      __is_pending = false;         // The last token is is.
        bool      __square_pending = false;     // The last token is [.
    };

    typedef ttoken_reader_t<cs_token_enumerator_t> cs_token_reader_t;

    ////////// ////////// ////////// ////////// //////////

}   // X_ROOT_NS::modules::lang_cs

#endif  // __LANG_CS_H__
