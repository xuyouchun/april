
#include <compile.h>
#include <algorithm.h>
#include <lang_cs.h>
#include "utils.h"

namespace X_ROOT_NS { namespace modules { namespace lang_cs {

    using namespace core;
    using namespace compile;

    typedef token_error_code_t e_t;
    typedef cs_token_value_t w_t;

    ////////// ////////// ////////// ////////// //////////

    #define __Wt(value)  ((core::token_value_t)cs_token_value_t::value)

    #define __NewObj(value, data)                                           \
        __new_token(value, p0, __p - p0, data) 

    #define __Nw_(value, data) (d = data, __NewObj(__Wt(value), d))         \

    #define __Nw(value)  __Nw_(value, nullptr)

    #define __Nws(value, values...)                                         \
        __NewObj(__Wt(__unknown__), __new_token_values_data(value, ##values))

    #define __Eqk_(keyword, data)                                           \
        do {                                                                \
            if (__equals_keyword(__p, _S(keyword)))                         \
            {                                                               \
                const size_t keyword_size = string_length(_S(keyword));     \
                __p += keyword_size - 1;                                    \
                return __new_token(__Wt(k_##keyword),                       \
                        p0, keyword_size, data);                            \
            }                                                               \
        } while (false)

    #define __Eqk(keyword)  __Eqk_(keyword, nullptr)

    #define __Eqks(keyword, value, values...)                               \
        do {                                                                \
            if (__equals_keyword(__p, _S(keyword)))                         \
            {                                                               \
                const size_t keyword_size = string_length(_S(keyword));     \
                __p += keyword_size - 1;                                    \
                return __Nws(value, ##values);                              \
            }                                                               \
        } while (false)


    // Returns the next token.
    token_t * cs_token_enumerator_t::next()
    {
        if (!__skip_whitespace())
            return nullptr;

        char_t c;
        token_data_t * d;
        const __cptr_t p0 = __p;
        cs_token_value_t value;

        switch ((c = *__p++))
        {
            case _T('a'):
                __Eqk(abstract);
                __Eqk(as);
                __Eqks(add, __Wt(k_add), __Wt(name));
                break;

            case _T('b'):
                __Eqk(base);
                __Eqk(break);
                __Eqk(byte);
                __Eqk(bool);
                break;

            case _T('c'):
                __Eqk(case);
                __Eqk(catch);
                __Eqk(char);
                __Eqk(checked);
                __Eqk(class);
                __Eqk(const);
                __Eqk(continue);
                break;

            case _T('d'):
                __Eqk(decimal);
                __Eqk(default);
                __Eqk(delegate);
                __Eqk(do);
                __Eqk(double);
                break;

            case _T('e'):
                __Eqk(else);
                __Eqk(enum);
                __Eqk(event);
                __Eqk(explicit);
                __Eqk(extern);
                break;

            case _T('f'):
                __Eqk(false);
                __Eqk(finally);
                __Eqk(fixed);
                __Eqk(float);
                __Eqk(for);
                __Eqk(foreach);
                __Eqk(from);
                break;

            case _T('g'):
                __Eqk(goto);
                __Eqks(get, __Wt(k_get), __Wt(name));
                __Eqks(global, __Wt(k_global), __Wt(name));
                break;

            case _T('i'):
                __Eqk(if);
                __Eqk(implicit);
                __Eqk(in);
                __Eqk(int);
                __Eqk(interface);
                __Eqk(internal);
                __Eqk(is);
                __Eqk(import);
                break;

            case _T('l'):
                __Eqk(lock);
                __Eqk(long);
                break;

            case _T('n'):
                __Eqk(namespace);
                __Eqk(new);
                __Eqk(null);
                break;

            case _T('o'):
                __Eqk(object);
                __Eqk(operator);
                __Eqk(out);
                __Eqk(override);
                break;

            case _T('p'):
                __Eqk(params);
                __Eqk(private);
                __Eqk(protected);
                __Eqk(public);
                break;

            case _T('r'):
                __Eqk(readonly);
                __Eqk(ref);
                __Eqk(return);
                __Eqks(remove, __Wt(k_remove), __Wt(name));
                break;

            case _T('s'):
                __Eqk(sbyte);
                __Eqk(sealed);
                __Eqk(short);
                __Eqk(sizeof);
                __Eqk(stackalloc);
                __Eqk(static);
                __Eqk(string);
                __Eqk(struct);
                __Eqk(switch);
                __Eqks(set, __Wt(k_set), __Wt(name));
                break;

            case _T('t'):
                __Eqk(this);
                __Eqk(throw);
                __Eqk(true);
                __Eqk(try);
                __Eqk(typeof);
                __Eqk(typedef);
                break;

            case _T('u'):
                __Eqk(uint);
                __Eqk(ulong);
                __Eqk(unchecked);
                __Eqk(unsafe);
                __Eqk(ushort);
                __Eqk(using);
                break;

            case _T('v'):
                __Eqk(virtual);
                __Eqk(void);
                __Eqk(volatile);
                __Eqk(var);
                break;

            case _T('w'):
                __Eqk(while);
                __Eqks(where, __Wt(k_where), __Wt(name));
                break;

            case _T('('):   // (
                return __Nw(left_bracket);

            case _T(')'):   // )
                return __Nw(right_bracket);

            case _T('['):   // [
                return __Nw(left_square);

            case _T(']'):   // ]
                return __Nw(right_square);

            case _T('{'):   // {
                return __Nw(left_brace);

            case _T('}'):   // }
                return __Nw(right_brace);

            case _T('.'):   // .
                if (*__p == _T('.') && *(__p + 1) == _T('.'))
                {
                    __p += 2;
                    return __Nw(three_dots);
                }

                return __Nw(member_point);

            case _T(';'):   // ;
                return __Nw(semicolon);

            case _T(','):   // ,
                return __Nw(comma);

            case _T('?'):   // ?
                return __Nw(question_mark);

            case _T(':'):   // :
                switch (*__p)
                {
                    case _T(':'):
                        __p++;
                        return __Nw(double_colon);

                    default:
                        return __Nws(__Wt(question_colon), __Wt(base_mark));
                }
                break;

            case _T('$'):   // ?
                return __Nw(dollar);

            case _T('<'):   // < <= << <<=
                switch (*__p)
                {
                    case _T('<'):
                        if (*++__p == _T('='))                   // <<=
                        {
                            __p++;
                            return __Nw(left_shift_assign);
                        }
                        else                                    // <<
                        {
                            return __Nw(left_shift);
                        }

                    case _T('='):
                        __p++;
                        return __Nw(less_equal);                // <=

                    default:
                        return __Nws(__Wt(less), __Wt(left_angle));
                }
                break;

            case _T('>'):   // > >= >> >>=
                switch (*__p)
                {
                    case _T('>'):
                        if (*++__p == _T('='))                   // >>=
                        {
                            __p++;
                            return __Nw(right_shift_assign);
                        }
                        else                                    // >>
                        {
                            return __Nw(right_shift);
                        }

                    case _T('='):
                        __p++;
                        return __Nw(greater_equal);             // >=

                    default:
                        return __Nws(__Wt(greater), __Wt(right_angle));
                }
                break;

            case _T('+'):   // + ++ +=
                switch (*__p)
                {
                    case _T('+'):   // ++
                        __p++;
                        return __Nws(__Wt(left_increment), __Wt(right_increment));

                    case _T('='):   // +=
                        __p++;
                        return __Nw(add_assign);

                    default:
                        if (is_numeric_prefix(*__p)
                            && (value = (w_t)__last_value()) != w_t::name && value != w_t::cvalue)
                        {
                            __p--;
                            return __Nw_(cvalue, __read_number());
                        }

                        return __Nws(__Wt(add), __Wt(positive));
                }
                break;

            case _T('-'):   // - -- -=
                switch (*__p)
                {
                    case _T('-'):   // --
                        __p++;
                        return __Nws(__Wt(left_decrement), __Wt(right_decrement));

                    case _T('='):   // -=
                        __p++;
                        return __Nw(sub_assign);

                    default:
                        if (is_numeric_prefix(*__p)
                            && (value = (w_t)__last_value()) != w_t::name && value != w_t::cvalue)
                        {
                            __p--;
                            return __Nw_(cvalue, __read_number());
                        }

                        return __Nws(__Wt(sub), __Wt(minus));
                }
                break;

            case _T('*'):   // * *=
                switch (*__p)
                {
                    case _T('='):   // *=
                        __p++;
                        return __Nw(mul_assign);

                    default:
                        return __Nw(mul);
                }
                break;

            case _T('/'):   // // /* /= /
                switch (*__p)
                {
                    case _T('/'):   // //
                        __p++;
                        return __Nw_(comment, __read_line_comment());

                    case _T('*'):   // /*
                        __p++;
                        return __Nw_(comment, __read_multiline_comment());

                    case _T('='):   // /=
                        __p++;
                        return __Nw(div_assign);

                    default:        // /
                        return __Nw(div);
                }
                break;

            case _T('%'):   // % %=
                switch (*__p)
                {
                    case _T('='):   // %=
                        __p++;
                        return __Nw(mod_assign);

                    default:
                        return __Nw(mod);
                }
                break;

            case _T('"'):   // string
                return __Nw_(cvalue, __read_string());

            case _T('@'):   // multiline string
                if (*__p == _T('"'))
                {
                    __p++;
                    return __Nw_(cvalue, __read_multiline_string());
                }
                else if (is_name_prefix(*__p))
                {
                    return __Nw_(name, __read_name());
                }
                break;

            case _T('&'):   // & && &=
                switch (*__p)
                {
                    case _T('&'):
                        __p++;
                        return __Nw(logic_and);

                    case _T('='):
                        __p++;
                        return __Nw(bit_and_assign);

                    default:
                        return __Nw(bit_and);
                }
                break;

            case _T('|'):   // | || |=
                switch (*__p)
                {
                    case _T('|'):
                        __p++;
                        return __Nw(logic_or);

                    case _T('='):
                        __p++;
                        return __Nw(bit_or_assign);

                    default:
                        return __Nw(bit_or);
                }
                break;

            case _T('~'):   // ~
                return __Nw(bit_not);

            case _T('^'):   // ^ ^=
                switch (*__p)
                {
                    case _T('='):
                        __p++;
                        return __Nw(bit_xor_assign);

                    default:
                        return __Nw(bit_xor);
                }
                break;

            case _T('!'):   // ! !=
                switch (*__p)
                {
                    case _T('='):
                        __p++;
                        return __Nw(not_equal);

                    default:
                        return __Nw(logic_not);
                }
                break;

            case _T('='):   // = ==
                switch (*__p)
                {
                    case _T('='):
                        __p++;
                        return __Nw(equal);

                    default:
                        return __Nw(assign);
                }
                break;

            case _T('\''):  // char
                return __Nw_(cvalue, __read_char());

        }

        if (is_numeric_prefix(c))
        {
            __p--;
            return __Nw_(cvalue, __read_number());
        }

        if (is_name_prefix(c))
            return __Nw_(name, __read_name());

        return __Nw_(__default__, __read_unknown());
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // X_ROOT_NS::modules::lang_cs
