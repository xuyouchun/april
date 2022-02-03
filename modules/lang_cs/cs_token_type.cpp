
#include <lang_cs.h>

namespace X_ROOT_NS::modules::lang_cs {

    #define __K(name)        X_C(k_##name, _S(name))
    #define __C(name, desc)  X_C(name, desc)

    // Cs Token values.
    X_ENUM_INFO(cs_token_value_t)

        __C(empty, _T("empty"))

        // keywords: a...
        __K(abstract)    __K(as)       __K(add)

        // keywords: b...
        __K(base)        __K(bool)     __K(break)     __K(byte)

        // keywords: c...
        __K(case)        __K(catch)    __K(char)     __K(checked)
        __K(class)       __K(const)    __K(continue)
        
        // keywords: d...
        __K(decimal)     __K(default)  __K(delegate)  __K(do)       __K(double)

        // keywords: e...
        __K(else)        __K(enum)     __K(event)     __K(explicit) __K(extern)
        
        // keywords: f...
        __K(false)       __K(finally)  __K(fixed)     __K(float)    __K(for)
        __K(foreach)     __K(from)
        
        // keywords: g...
        __K(goto)       __K(get)       __K(global)

        // keywords: i...
        __K(if)          __K(implicit) __K(in)        __K(int)      __K(interface)
        __K(internal)    __K(is)    __K(import)
        
        // keywords: l...
        __K(lock)        __K(long)
        
        // keywords: n...
        __K(namespace)   __K(new)     __K(null)
        
        // keywords: o...
        __K(object)      __K(operator) __K(out)     __K(override)
        
        // keywords: p...
        __K(params)      __K(private) __K(protected)  __K(public)

        // keywords: r...
        __K(readonly)    __K(ref)     __K(return)   __K(remove)
        
        // keywords: s...
        __K(sbyte)       __K(sealed)  __K(short)   __K(sizeof)  __K(stackalloc)
        __K(static)      __K(string)  __K(struct)  __K(switch)  __K(set)
        
        // keywords: t...
        __K(this)        __K(throw)   __K(true)    __K(try)     __K(typeof) __K(typedef)
        
        // keywords: u...
        __K(uint)        __K(ulong)   __K(unchecked) __K(unsafe)  __K(ushort)
        __K(using)
        
        // keywords: v...
        __K(virtual)     __K(void)    __K(volatile) __K(var)
        
        // keywords: w...
        __K(while)      __K(where)

        // algorithm
        __C(add, _T("+"))  __C(sub, _T("-"))   __C(mul, _T("*"))   __C(div, _T("/"))
        __C(mod, _T("%"))                                           // + - * / %

        __C(minus, _T("`-"))        __C(positive, _T("`+"))         // +, -

        __C(add_assign, _T("+="))   __C(sub_assign, _T("-="))   __C(mul_assign, _T("*="))
        __C(div_assign, _T("/="))   __C(mod_assign, _T("%="))       // += -= *= /= %=

        __C(left_increment,  _T("`++"))  __C(left_decrement,  _T("`--"))  // ++ --
        __C(right_increment, _T("++"))   __C(right_decrement, _T("--"))   // ++ --

        __C(left_shift, _T("<<"))   __C(right_shift,_T(">>"))
        __C(left_shift_assign, _T("<<=")) __C(right_shift_assign, _T(">>=")) // << >> <<= >>=

        __C(bit_and, _T("&"))       __C(bit_or, _T("|"))    __C(bit_not, _T("~"))
        __C(bit_xor, _T("^"))                                       // & | ~ ^

        __C(bit_and_assign, _T("&="))   __C(bit_or_assign,  _T("|="))
        __C(bit_xor_assign, _T("^="))   __C(assign, _T("="))        // &= |= ^=

        //
        __C(greater, _T("`>"))      __C(greater_equal, _T(">="))    // > >=
        __C(less,  _T("`<"))        __C(less_equal,  _T("<="))      // < <=
        __C(equal, _T("=="))        __C(not_equal,   _T("!="))      // == !=

        // logic
        __C(logic_and,  _T("&&"))  __C(logic_or, _T("||"))  __C(logic_not, _T("!"))     // && || !

        __C(member_point, _T("."))                                  // .
        __C(question_mark, _T("?"))  __C(question_colon, _T("`:"))  // `: |

        // brackets
        __C(left_bracket, _T("("))   __C(right_bracket, _T(")"))    // ( )
        __C(left_square,  _T("["))   __C(right_square,  _T("]"))    // [ ]
        __C(left_angle,   _T("<"))   __C(right_angle,   _T(">"))    // < >
        __C(left_brace,   _T("{"))   __C(right_brace,   _T("}"))    // { }

        // others ...
        __C(name,  _T("name"))      __C(cvalue, _T("cvalue"))
        __C(comment, _T("comment"))

        __C(semicolon, _T(";"))     __C(comma, _T(","))             // ; ,
        __C(dollar, _T("$"))        __C(ampersand, _T("@"))         // $ @
        __C(base_mark, _T(":"))     __C(double_colon, _T("::"))     // : ::
        __C(three_dots, _T("..."))                                  // ...

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Cs Operators.
    X_ENUM_INFO(cs_operator_t)

        // ?
        X_C(question_mark,  _T("question_mark"),    _T("QuestionMark"))

        // :
        X_C(question_colon, _T("question_colon"),   _T("QuestionColon"))

        // as
        X_C(as,             _T("as"),               _T("As"))

        // is
        X_C(is,             _T("is"),               _T("Is"))

        // ,
        X_C(comma,          _T("comma"),            _T("Comma"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::lang_cs

