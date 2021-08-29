#include <compile.h>
#include <lib.h>
#include <algorithm.h>
#include "utils.h"

namespace X_ROOT_NS { namespace modules { namespace compile {

    using namespace core;

    typedef token_error_code_t e_t;

    ////////// ////////// ////////// ////////// //////////

    // Constructor.
    token_enumerator_base_t::token_enumerator_base_t(token_reader_context_t & context,
        lang_t * lang, const char_t * code, size_t length, const code_file_t * file)
       : __context(context), __lang(lang), __p(al::cptr(code, length)), __file(file)
       , __logger(context.compile_context, lang)
    {
        _A(lang != nullptr);
    }

    // Returns the last token.
    const token_t * token_enumerator_base_t::__last()
    {
        return __tokens.last();
    }

    // Returns the last value.
    token_value_t token_enumerator_base_t::__last_value()
    {
        const token_t * w = __last();
        return w? w->value : unknown_token_value;
    }

    // Skips whitespaces.
    bool token_enumerator_base_t::__skip_whitespace()
    {
        char_t c;
        while ((c = *__p))
        {
            if (al::is_whitespace(c))
                __p++;
            else
                return true;
        }

        return false;
    }

    // Creates new token error.
    token_error_t * token_enumerator_base_t::__new_token_error(token_error_code_t error_code,
        const char_t * error_message)
    {
        if (error_code != token_error_code_t::__default__)
            return __token_errors.new_obj(error_code, error_message);

        return nullptr;
    }

    // Creates new token data.
    token_data_t * token_enumerator_base_t::__new_token_data(
        token_error_code_t error_code, const char_t * error_message)
    {
        token_data_t * data = __token_datas.new_obj();
        data->error = __new_token_error(error_code, error_message);
        return data;
    }

    // Creates new token data.
    token_data_t * token_enumerator_base_t::__new_token_data(
        cvalue_type_t value_type, const void * value,
        token_error_code_t error_code, const char_t * error_message)
    {
        token_data_t * data = __token_datas.new_obj();
        data->value_type = value_type;
        if (value_type == cvalue_type_t::number)
        {
            data->value = __values.new_obj(*(tvalue_t *)value);
        }
        else if (value_type == cvalue_type_t::string)
        {
            const string_t & s = *(const string_t *)value;
            data->value = __new_string(s);
        }
        else
        {
            data->value = value;
        }

        data->error = __new_token_error(error_code, error_message);
        return data;
    }

    // Creates new token data.
    token_data_t * token_enumerator_base_t::__new_token_data(const tvalue_t & value,
        token_error_code_t error_code, const char_t * error_message)
    {
        return __new_token_data(cvalue_type_t::number, (const void *)&value,
            error_code, error_message);
    }

    // Creates new token data.
    token_data_t * token_enumerator_base_t::__new_token_data(const string_t & value,
        token_error_code_t error_code, const char_t * error_message)
    {
        return __new_token_data(cvalue_type_t::string, (const void *)&value,
            error_code, error_message);
    }

    // Creates a string.
    const char_t * token_enumerator_base_t::__new_string(const string_t & s)
    {
        char_t * s0 = __strings.acquire(s.length() + 1);
        al::strcpy(s0, s.c_str());
        return s0;
    }

    // Reads comment.
    token_data_t * token_enumerator_base_t::__read_line_comment()
    {
        char_t c;
        while ((c = *__p))
        {
            if (al::is_lineend(c))
                break;
            __p++;
        }

        return nullptr;
    }

    // Reads multi-line comment.
    token_data_t * token_enumerator_base_t::__read_multiline_comment()
    {
        char_t c;
        while ((c = *__p++))
        {
            if (c == _T('*') && *__p == _T('/'))
            {
                __p++;
                return nullptr;
            }
        }

        return __new_token_data(e_t::comment_end_missing);
    }

    // Reads a number.
    token_data_t * token_enumerator_base_t::__read_number()
    {
        typedef al::parse_numeric_error_code_t ne_t;

        tvalue_t * value = __values.new_obj();
        ne_t code = al::try_parse_numeric(__p, value);
        e_t error_code;

        switch (code)
        {
            case ne_t::__default__:
                error_code = e_t::__default__;
                break;

            case ne_t::overflow:
                error_code = e_t::numeric_overflow;
                break;

            case ne_t::format_error:
            default:
                error_code = e_t::numeric_format_error;
                break;
        }

        return __new_token_data(*value, error_code);
    }

    // Reads a name.
    token_data_t * token_enumerator_base_t::__read_name()
    {
        while (al::is_word(*__p))
        {
            __p++;
        }

        return nullptr;
    }

    // Reads unknown token.
    token_data_t * token_enumerator_base_t::__read_unknown()
    {
        char_t first_c = *(__p - 1);
        char_t c;

        while ((c = *__p))
        {
            if (c != first_c && !al::is_word(c))
            {
                break;
            }

            __p++;
        }

        return __new_token_data(e_t::unknown_token);
    }

    // Tries to escape char.
    bool token_enumerator_base_t::__try_escape_char(char_t * out_c)
    {
        switch (*__p++)
        {
            case _T('n'):
                *out_c = _T('\n');
                break;

            case _T('r'):
                *out_c = _T('\r');
                break;

            case _T('t'):
                *out_c = _T('\t');
                break;

            case _T('b'):
                *out_c = _T('\b');
                break;

            case _T('a'):
                *out_c = _T('\a');
                break;

            case _T('0'):
                *out_c = _T('\0');
                break;

            case _T('\\'):
                *out_c = _T('\\');
                break;

            case _T('"'):
                *out_c = _T('"');
                break;

            case _T('\''):
                *out_c = _T('\'');
                break;

            case _T('v'):
                *out_c = _T('\v');
                break;

            case _T('?'):
                *out_c = _T('?');
                break;

            case _T('f'):
                *out_c = _T('f');
                break;

            default:
                return false;
        }

        return true;
    }

    // Reads a string.
    token_data_t * token_enumerator_base_t::__read_string()
    {
        char_t c, c1;
        stringstream_t s;
        e_t error_code = e_t::__default__;
        const char_t * error_message = nullptr;

        while (true)
        {
            switch ((c = *__p))
            {
                case _T('\\'):
                    if ((c = *(++__p)) == _T('\0'))
                    {
                        error_code = e_t::string_format_error;
                        error_message = _T("missing terminating \" character");
                        goto __end;
                    }
                    else if (__try_escape_char(&c1))
                    {
                        s << c1;
                    }
                    else
                    {
                        error_code = e_t::string_format_error;
                        error_message = __new_string(_T("unknown escape sequence '\\%1%'"), c);
                    }
                    break;

                case _T('"'):
                    __p++;
                    goto __end;

                case _T('\0'):
                    error_code = e_t::string_format_error;
                    error_message = _T("missing terminating ' character");
                    goto __end;

                default:
                    s << c;
                    __p++;
                    break;
            }
        }

    __end:
        return __new_token_data(s.str(), error_code, error_message);
    }

    // Reads multi-line string.
    token_data_t * token_enumerator_base_t::__read_multiline_string()
    {
        char_t c, c1;
        stringstream_t s;
        e_t error_code = e_t::__default__;
        const char_t * error_message = nullptr;

        while (true)
        {
            switch ((c = *__p))
            {
                case _T('"'):
                    if (*(++__p) == _T('"'))
                        s << *(__p++);
                    else
                        goto __end;

                case _T('\0'):
                    error_code = e_t::string_format_error;
                    error_message = _T("string missing '\"'");
                    goto __end;

                default:
                    s << c;
                    __p++;
                    break;
            }
        }

    __end:
        return __new_token_data(s.str(), error_code, error_message);
    }

    // Reads a char.
    token_data_t * token_enumerator_base_t::__read_char()
    {
        char_t c;
        char_t ret_c = _T('\0');
        int c_count = 0;
        e_t error_code = e_t::__default__;
        const char_t * error_message;

        while (true)
        {
            switch ((c = *__p))
            {
                case _T('\''):
                    __p++;
                    goto __end;

                case _T('\\'):
                    if ((c = *(++__p)) == _T('\0'))
                    {
                        error_code = e_t::char_format_error;
                        error_message = _T("missing terminating ' character");
                        goto __end;
                    }
                    else if (!__try_escape_char(&ret_c))
                    {
                        error_code = e_t::char_format_error;
                        error_message = __new_string(_T("unknown escape sequence '\\%1%'"), c);
                    }
                    c_count++;
                    break;

                case _T('\r'): case _T('\n'): case _T('\0'):
                    error_code = e_t::char_format_error;
                    error_message = _T("missing terminating ' character");
                    break;

                default:
                    ret_c = *__p++;
                    c_count++;
                    break;
            }
        }

    __end:
        if (c_count == 0)
        {
            error_code = e_t::char_format_error;
            error_message = _T("empty character constant");
        }
        else if (c_count > 1)
        {
            error_code = e_t::char_format_error;
            error_message = _T("multi-character character constant");
        }

        tvalue_t * value = __values.new_obj();
        *value = ret_c;
        return __new_token_data(*value, error_code, error_message);
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // X_ROOT_NS::modules::compile
