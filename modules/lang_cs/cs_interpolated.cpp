#include <compile.h>
#include <algorithm.h>
#include <lang_cs.h>
#include "utils.h"

namespace X_ROOT_NS::modules::lang_cs {

    using namespace core;
    using namespace compile;

    typedef token_error_code_t e_t;

    #define __Wt(value)  ((core::token_value_t)cs_token_value_t::value)

    ////////// ////////// ////////// ////////// //////////

    // The strings that have holes ($"Error: {ex.Message}"), and the raw strings ("""..."""), are read by the lexer:
    //
    //   $"a{x}b{y:N2}"    ->  the tokens of  String.Format("a{0}b{1:N2}", x, y)     (no holes: a string)
    //
    // The tokens of an expression of a hole are the ones that the lexer makes from the characters of the hole (they have their own
    // places in the source). The forms:
    //   $"..."  $@"..."  @$"..."     \ escapes (not in the verbatim ones), {{ and }} are the braces; "" is a quote in the verbatim ones
    //   """..."""                    a raw string: three quotes or more, the end is the same number of quotes; what is between is
    //                                the text, without escapes; when the quotes are alone in their lines the text is the lines
    //                                between, without the indentation of the closing quotes
    //   $"""...{x}..."""  $$"""...{{x}}..."""   a raw string that has holes: the number of $ is the number of braces of a hole,
    //                                the braces that are fewer are text

    namespace
    {
        // A part of an interpolated string: text, or a hole.
        struct __part_t
        {
            string_t        text;
            bool            hole = false;
            const char_t *  begin = nullptr;    // The characters of the expression of the hole.
            const char_t *  end = nullptr;
            string_t        alignment;
            string_t        format;
        };

        bool __is_space(char_t c)
        {
            return c == _T(' ') || c == _T('\t');
        }

        string_t __trim(const char_t * b, const char_t * e)
        {
            while (b < e && al::is_whitespace(*b))
                b++;

            while (e > b && al::is_whitespace(e[-1]))
                e--;

            return string_t(b, e);
        }
    }

    // Reads a string that has holes, or a raw string. The characters before the first quote were read (p0 is the beginning);
    // __p is at the first quote: `dollars` is the number of $, `verbatim` is @ (not a raw string), the quotes are counted here.
    token_t * cs_token_enumerator_t::__read_interpolated(const char_t * p0, int dollars, bool verbatim)
    {
        // The quotes that open the string.
        int quotes = 0;
        while (*__p == _T('"'))
        {
            quotes++;
            __p++;
        }

        // A string of two quotes is an empty string (the quotes were taken as the opening).
        bool empty_string = false;
        if (quotes == 2)
        {
            quotes = 1;
            empty_string = true;
        }

        bool raw = quotes >= 3;

        std::vector<__part_t> parts;
        parts.push_back(__part_t());

        e_t error_code = e_t::__default__;
        const char_t * error_message = nullptr;

        auto fail = [&](const char_t * message) {
            error_code = e_t::string_format_error;
            error_message = message;
        };

        auto text = [&]() -> string_t & {
            if (parts.back().hole)
                parts.push_back(__part_t());

            return parts.back().text;
        };

        // The raw strings: where the text ends, and the indentation that the lines have.
        const char_t * raw_end = nullptr;       // The first character that is not text.
        const char_t * closing_end = nullptr;   // The first character after the closing quotes.
        string_t indent;
        bool multiline = false;

        if (empty_string)
        {
            // "" : nothing to read.
        }
        else if (raw)
        {
            // The closing quotes: the first run that has as many quotes as the opening.
            const char_t * q = __p;
            const char_t * close = nullptr;

            while (*q != _T('\0'))
            {
                if (*q == _T('"'))
                {
                    int n = 0;
                    while (q[n] == _T('"'))
                        n++;

                    if (n >= quotes)
                    {
                        close = q;
                        if (n > quotes)
                            fail(_T("a raw string cannot end with more quotes than it has at the beginning"));

                        closing_end = q + quotes;
                        break;
                    }

                    q += n;
                    continue;
                }

                q++;
            }

            if (close == nullptr)
            {
                fail(_T("unterminated raw string literal"));
                raw_end = __p;
                closing_end = __p;
            }
            else
            {
                // Is it a text of lines? The first line has nothing, the last one has only spaces before the closing quotes.
                const char_t * first = __p;
                while (first < close && __is_space(*first))
                    first++;

                const char_t * last_line = close;
                while (last_line > __p && last_line[-1] != _T('\n'))
                    last_line--;

                bool first_ends = first < close && (*first == _T('\n') || *first == _T('\r'));
                bool last_spaces = true;

                for (const char_t * k = last_line; k < close; k++)
                {
                    if (!__is_space(*k))
                        last_spaces = false;
                }

                if (first_ends && last_line > first && last_spaces)
                {
                    multiline = true;
                    indent = string_t(last_line, close);

                    // The text begins after the first line end, and ends before the last one.
                    if (*first == _T('\r'))
                        first++;

                    if (*first == _T('\n'))
                        first++;

                    __p = first;
                    raw_end = last_line - 1;                // The "\n" before the last line.

                    if (raw_end > __p && raw_end[-1] == _T('\r'))
                        raw_end--;

                    if (raw_end < __p)
                        raw_end = __p;
                }
                else
                {
                    raw_end = close;
                }
            }
        }

        bool at_line_start = multiline;

        auto skip_indent = [&]() {
            // The lines that have less indentation than the closing quotes (not the empty ones) are an error.
            size_t k = 0;
            const char_t * q = __p;

            while (k < indent.size() && q < raw_end && *q == indent[k])
            {
                k++;
                q++;
            }

            if (k < indent.size())
            {
                const char_t * line = q;
                while (line < raw_end && __is_space(*line))
                    line++;

                if (line < raw_end && *line != _T('\n') && *line != _T('\r'))
                    fail(_T("a line of a raw string has less indentation than the closing quotes"));
            }

            __p = q;
        };

        // The holes.
        while (!empty_string && error_code == e_t::__default__)
        {
            char_t c = *__p;

            if (raw)
            {
                if (__p >= raw_end)
                {
                    __p = closing_end;
                    break;
                }

                if (at_line_start)
                {
                    at_line_start = false;
                    skip_indent();

                    if (error_code != e_t::__default__ || __p >= raw_end)
                        continue;

                    c = *__p;
                }
            }
            else if (c == _T('\0'))
            {
                fail(_T("unterminated string literal"));
                break;
            }
            else if (c == _T('"'))
            {
                if (verbatim && __p[1] == _T('"'))
                {
                    text() += _T('"');
                    __p += 2;
                    continue;
                }

                __p++;
                break;
            }

            if (dollars > 0 && c == _T('{'))
            {
                int run = 0;
                while (__p[run] == _T('{'))
                    run++;

                int literal = 0;
                bool open = false;

                if (!raw)
                {
                    literal = run / 2;
                    open = run % 2 == 1;
                }
                else if (run < dollars)
                {
                    literal = run;
                }
                else if (run >= 2 * dollars)
                {
                    fail(_T("too many opening braces in an interpolated raw string"));
                    break;
                }
                else
                {
                    literal = run - dollars;
                    open = true;
                }

                for (int k = 0; k < literal; k++)
                    text() += _T('{');

                __p += run;

                if (!open)
                    continue;

                // The hole: expression [, alignment] [: format] and the closing braces.
                const char_t * begin = __p;
                const char_t * expr_end = nullptr;
                const char_t * align_begin = nullptr, * align_end = nullptr;
                const char_t * format_begin = nullptr;
                const char_t * hole_end = nullptr;
                int depth = 0;

                for (;;)
                {
                    char_t h = *__p;

                    if (h == _T('\0') || (raw && __p >= raw_end))
                    {
                        fail(_T("unterminated interpolation in a string"));
                        break;
                    }

                    if (format_begin == nullptr)
                    {
                        // Strings and characters of the expression.
                        if (h == _T('"'))
                        {
                            int n = 0;
                            while (__p[n] == _T('"'))
                                n++;

                            if (n >= 3)
                            {
                                // A raw string: until the same number of quotes.
                                __p += n;
                                while (*__p != _T('\0'))
                                {
                                    int m = 0;
                                    while (__p[m] == _T('"'))
                                        m++;

                                    if (m >= n)
                                    {
                                        __p += n;
                                        break;
                                    }

                                    __p += m > 0? m : 1;
                                }
                            }
                            else
                            {
                                __p++;
                                while (*__p != _T('\0') && *__p != _T('"'))
                                    __p += (*__p == _T('\\') && __p[1] != _T('\0'))? 2 : 1;

                                if (*__p == _T('"'))
                                    __p++;
                            }

                            continue;
                        }

                        if (h == _T('\''))
                        {
                            __p++;
                            while (*__p != _T('\0') && *__p != _T('\''))
                                __p += (*__p == _T('\\') && __p[1] != _T('\0'))? 2 : 1;

                            if (*__p == _T('\''))
                                __p++;

                            continue;
                        }

                        if (h == _T('(') || h == _T('[') || h == _T('{'))
                        {
                            depth++;
                            __p++;
                            continue;
                        }

                        if (h == _T(')') || h == _T(']'))
                        {
                            depth--;
                            __p++;
                            continue;
                        }

                        if (h == _T(',') && depth == 0 && align_begin == nullptr)
                        {
                            expr_end = __p;
                            align_begin = ++__p;
                            continue;
                        }

                        if (h == _T(':') && depth == 0 && __p[1] != _T(':'))
                        {
                            if (expr_end == nullptr)
                                expr_end = __p;
                            else if (align_end == nullptr)
                                align_end = __p;

                            format_begin = ++__p;
                            continue;
                        }
                    }

                    if (h == _T('}'))
                    {
                        if (format_begin == nullptr && depth > 0)
                        {
                            depth--;
                            __p++;
                            continue;
                        }

                        // The closing braces: one, or the number of $ in a raw string.
                        int close = raw? dollars : 1;
                        int n = 0;
                        while (n < close && __p[n] == _T('}'))
                            n++;

                        if (n == close)
                        {
                            hole_end = __p;
                            __p += close;
                            break;
                        }

                        __p++;
                        continue;
                    }

                    __p++;
                }

                if (error_code != e_t::__default__)
                    break;

                __part_t part;
                part.hole = true;
                part.begin = begin;
                part.end = expr_end != nullptr? expr_end : hole_end;

                if (align_begin != nullptr)
                    part.alignment = __trim(align_begin, align_end != nullptr? align_end : hole_end);

                if (format_begin != nullptr)
                    part.format = string_t(format_begin, hole_end);

                // An expression that has nothing is not valid.
                if (__trim(part.begin, part.end).empty())
                {
                    fail(_T("an interpolation of a string has no expression"));
                    break;
                }

                parts.push_back(part);

                continue;
            }

            if (dollars > 0 && c == _T('}'))
            {
                int run = 0;
                while (__p[run] == _T('}'))
                    run++;

                if (!raw)
                {
                    if (run % 2 == 1)
                    {
                        fail(_T("a } of an interpolated string must be doubled"));
                        break;
                    }

                    for (int k = 0; k < run / 2; k++)
                        text() += _T('}');
                }
                else if (run < dollars)
                {
                    for (int k = 0; k < run; k++)
                        text() += _T('}');
                }
                else
                {
                    fail(_T("unexpected } in an interpolated raw string"));
                    break;
                }

                __p += run;
                continue;
            }

            if (!raw && !verbatim && c == _T('\\'))
            {
                char_t c1, c2;

                if (*(++__p) == _T('\0'))
                {
                    fail(_T("unterminated string literal"));
                    break;
                }

                if (__try_escape_char(&c1, &c2))
                {
                    text() += c1;

                    if (c2 != _T('\0'))
                        text() += c2;
                }
                else
                {
                    fail(_T("unknown escape sequence in a string"));
                    break;
                }

                continue;
            }

            if (raw)
            {
                // The line ends of the text are "\n".
                if (c == _T('\r') && __p[1] == _T('\n'))
                {
                    __p++;
                    continue;
                }

                if (c == _T('\n') && multiline)
                {
                    text() += c;
                    __p++;
                    at_line_start = true;
                    continue;
                }
            }

            text() += c;
            __p++;
        }

        // The text of the string, or the call of String.Format.
        int holes = 0;
        for (const __part_t & part : parts)
        {
            if (part.hole)
                holes++;
        }

        auto text_of = [&](bool escape_braces) {
            string_t result;
            int index = 0;

            for (const __part_t & part : parts)
            {
                if (!part.hole)
                {
                    for (char_t ch : part.text)
                    {
                        if (escape_braces && (ch == _T('{') || ch == _T('}')))
                            result += ch;

                        result += ch;
                    }

                    continue;
                }

                result += _T('{');
                result += _F(_T("%1%"), index++);

                if (!part.alignment.empty())
                    result += _T(',') + part.alignment;

                if (!part.format.empty())
                    result += _T(':') + part.format;

                result += _T('}');
            }

            return result;
        };

        if (error_code != e_t::__default__ || holes == 0)
        {
            token_data_t * data = __new_token_data(text_of(false), error_code, error_message);
            return __new_token(__Wt(cvalue), p0, __p - p0, data);
        }

        // String . Format ( "format" , expression ...
        std::vector<token_t *> tokens;

        int32_t length = (int32_t)(__p - p0);

        tokens.push_back(__new_token_with_text(__Wt(name), p0, length, _T("String")));
        tokens.push_back(__new_token_with_text(__Wt(member_point), p0, length, _T(".")));
        tokens.push_back(__new_token_with_text(__Wt(name), p0, length, _T("Format")));
        tokens.push_back(__new_token_with_text(__Wt(left_bracket), p0, length, _T("(")));
        tokens.push_back(__new_token(__Wt(cvalue), p0, __p - p0, __new_token_data(text_of(true))));

        for (const __part_t & part : parts)
        {
            if (!part.hole)
                continue;

            tokens.push_back(__new_token_with_text(__Wt(comma), p0, length, _T(",")));

            cs_token_enumerator_t * reader = __context.compile_context.new_obj<cs_token_enumerator_t>(
                __context, __lang, part.begin, (size_t)(part.end - part.begin), __file
            );

            while (token_t * token = reader->next())
                tokens.push_back(token);
        }

        tokens.push_back(__new_token_with_text(__Wt(right_bracket), p0, length, _T(")")));

        // Not made by this reader before the last one: the last token is the one that this reader made last.
        for (size_t k = 1; k < tokens.size(); k++)
            __pending.push_back(tokens[k]);

        return tokens[0];
    }

}   // X_ROOT_NS::modules::lang_cs
