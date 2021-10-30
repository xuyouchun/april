#ifndef __COMPILE_TOKEN_READER_H__
#define __COMPILE_TOKEN_READER_H__

#include <core.h>
#include <algorithm.h>
#include <lib.h>


namespace X_ROOT_NS { namespace modules { namespace compile {

    namespace
    {
        using namespace core;
        using namespace compile;
    }

    ////////// ////////// ////////// ////////// //////////

    // Token reader.
    template<typename token_enumerator_t>
    class ttoken_reader_t : public token_reader_t
    {
    public:

        // Constructor.
        ttoken_reader_t(token_reader_context_t & context, lang_t * lang)
            : __context(context), __lang(lang)
        { }

        // Returns token enumerator.
        virtual token_enumerator_t * read(const char_t * code, size_t length,
                                          const code_file_t * file) override
        {
            return __context.compile_context.new_obj<token_enumerator_t>(
                __context, __lang, code, length, file
            );
        }

    private:
        token_reader_context_t & __context;
        lang_t * __lang;
    };

    ////////// ////////// ////////// ////////// //////////

    // Base class of token enumerator.
    class token_enumerator_base_t : public token_enumerator_t, public object_t
    {
    public:

        // Constructor.
        token_enumerator_base_t(token_reader_context_t & context, lang_t * lang,
                const char_t * code, size_t length, const code_file_t * file);
        
        X_TO_STRING_IMPL(_T("token_enumerator_base_t"))

    protected:
        token_reader_context_t & __context;
        lang_t *                 __lang;
        compile_logger_t         __logger;
        const code_file_t *      __file;

        typedef al::cptr_t<char_t> __cptr_t;
        __cptr_t __p;

        std::map<token_value_t, const token_value_t *> __possible_values_cache;

        const core::token_t * __last();
        token_value_t __last_value();

        // Returns whether equals the keyword.
        template<size_t n>
        X_INLINE static bool __equals_keyword(const char_t * p, const char_t (&keyword)[n])
        {
            return al::starts_with<char_t, n - 1>(p, keyword + 1) && !al::is_word(p[n - 2]);
        }

        // Returns new token error.
        token_error_t * __new_token_error(token_error_code_t error_code,
            const char_t * error_message=nullptr);

        // Returns new token data.
        token_data_t * __new_token_data(token_error_code_t error_code,
            const char_t * error_message=nullptr);

        // Returns new token data.
        token_data_t * __new_token_data(
            cvalue_type_t value_type, const void * value,
            token_error_code_t error_code=token_error_code_t::__default__,
            const char_t * error_message=nullptr);

        // Returns new token data.
        token_data_t * __new_token_data(const tvalue_t & value,
            token_error_code_t error_code=token_error_code_t::__default__,
            const char_t * error_message=nullptr);

        // Returns new token data.
        token_data_t * __new_token_data(const string_t & value,
            token_error_code_t error_code=token_error_code_t::__default__,
            const char_t * error_message=nullptr);

        // Returns new string.
        template<typename ... args_t>
        const char_t * __new_string(const char_t * format, args_t ... args)
        {
            return __new_string(sprintf(format, args ...));
        }

        // Returns new string.
        const char_t * __new_string(const string_t & s);

        // Skips whitespace.
        bool __skip_whitespace();

        // Reads comment.
        token_data_t * __read_line_comment();

        // Reads multi-line commit.
        token_data_t * __read_multiline_comment();

        // Reads number.
        token_data_t * __read_number();

        // Reads name.
        token_data_t * __read_name();

        // Reads unknown token.
        token_data_t * __read_unknown();

        // Reads string.
        token_data_t * __read_string();

        // Reads multi-line string.
        token_data_t * __read_multiline_string();

        // Reads char.
        token_data_t * __read_char();

        // Creates new token values.
        template<typename ... values_t>
        const token_value_t * __new_token_values(token_value_t first, values_t ... values)
        {
            auto it = __possible_values_cache.find(first);
            if (it != __possible_values_cache.end())
                return it->second;

            size_t size = sizeof...(values) + 2;

            token_value_t * value_array = __token_values.acquire(size);
            al::assign(value_array, first, values...);
            value_array[size - 1] = unknown_token_value;

            __possible_values_cache[first] = value_array;
            return value_array;
        }

        // Creates new token data with values.
        template<typename ... values_t>
        token_data_t * __new_token_values_data(values_t ... values)
        {
            token_data_t * data = __token_datas.new_obj();
            data->possible_values = __new_token_values(values...);
            return data;
        }

        // Tries to escape char.
        bool __try_escape_char(char_t * out_c);

        // Returns new token.
        token_t * __new_token(token_value_t value, const char_t * s, int32_t length,
                token_data_t * data = nullptr)
        {
            return __tokens.new_obj(value, s, length, data, __file);
        }

    private:
        al::heap_t<token_t>             __tokens;
        al::heap_t<tvalue_t>            __values;
        al::heap_t<char_t[]>            __strings;
        al::heap_t<token_error_t>       __token_errors;
        al::heap_t<token_data_t>        __token_datas;
        al::heap_t<token_value_t[]>     __token_values;
    };

    ////////// ////////// ////////// ////////// //////////

} } }


#endif // __COMPILE_TOKEN_READER_H__
