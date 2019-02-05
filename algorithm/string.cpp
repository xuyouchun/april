
#include <algorithm.h>


namespace X_ROOT_NS { namespace algorithm {

    ////////// ////////// ////////// ////////// //////////

    // Initializes the character properties.
    void __char_property_t::init(enumerator_t & e)
    {
        typedef flag_t f_t;
        set_flag(e, 'A', 'Z', enum_or(f_t::upper, f_t::word));
        set_flag(e, 'a', 'z', enum_or(f_t::lower, f_t::word));
        set_flag(e, '0', '9', enum_or(f_t::digit, f_t::word, f_t::xdigit));
        set_flag(e, 'A', 'F', f_t::xdigit);
        set_flag(e, 'a', 'f', f_t::xdigit);
        set_flag(e, '0', '7', f_t::odigit);
        set_flag(e, '0', '1', f_t::bdigit);
        set_flag(e, '_', f_t::word);

        set_flag(e, f_t::whitespace, ' ', '\t', '\r', '\n', '\0');
        set_flag(e, f_t::lineend, '\r', '\n', '\0');

        e.each([](const char & c, __char_property_t & p) {
            p.lower = c;
            p.upper = c;
            p.digit_value = -1;
        });

        e.each('A', 'Z', [](const char & c, __char_property_t & p) {
            p.lower = c + ('a' - 'A');
        });

        e.each('a', 'z', [](const char & c, __char_property_t & p) {
            p.upper = c + ('A' - 'a');
        });

        e.each('0', '9', [](const char & c, __char_property_t & p) {
            p.digit_value = c - '0';
        });

        e.each('a', 'f', [](const char & c, __char_property_t & p) {
            p.digit_value = c - 'a' + 10;
        });

        e.each('A', 'F', [](const char & c, __char_property_t & p) {
            p.digit_value = c - 'A' + 10;
        });
    }

    const __char_properties_t __char_properties;

    ////////// ////////// ////////// ////////// //////////

    // Returns the length of a string.
    size_t strlen(const char_t * s)
    {
        _A(s != nullptr);

        for(const char_t * p = s; ; p++)
        {
            if(!*p)
                return p - s;
        }
    }

    // Copys a string to specified destination.
    char_t * strcpy(char_t * dst, const char_t * src)
    {
        _A(dst != nullptr);
        _A(src != nullptr);

        while((*dst = *src++))
        {
            dst++;
        }

        return dst;
    }

    // Connects two strings.
    char_t * strcat(char_t * dst, const char_t * src)
    {
        _A(dst != nullptr);
        _A(src != nullptr);

        return strcpy(dst + strlen(dst), src);
    }

    namespace
    {
        // String compare results.
        enum __str_equals_ret
        {
            yes, no, nolonger   // equals, not equals, no longer equals.
        };

        // Returns whether two strings are equals.
        template<typename equals_t>
        X_INLINE __str_equals_ret __str_equals(const char_t * s, const char_t * s0)
        {
            for(; *s && *s0; s++, s0++)
            {
                if(!equals_t::equals(*s, *s0))
                    return __str_equals_ret::no;
            }

            return *s0? __str_equals_ret::nolonger : __str_equals_ret::yes;
        }

        // Finds the substring, returns nullptr if not found.
        template<typename equals_t>
        const char_t * __strstr(const char_t * s, const char_t * s0)
        {
            typedef __str_equals_ret r_t;

            _A(s != nullptr);
            _A(s0 != nullptr);

            if(!*s0)
                return s;

            for(; *s; s++)
            {
                r_t r;
                if(equals_t::equals(*s, *s0) 
                        && (r = __str_equals<equals_t>(s + 1, s0 + 1)) != r_t::no)
                {
                    return r == r_t::yes? s : nullptr; 
                }
            }

            return nullptr;
        }

        // Finds the characher, returns nullptr if not found.
        template<typename equals_t>
        const char_t * __strchr(const char_t * s, char_t c)
        {
            _A(s != nullptr);

            while(*s)
            {
                if(equals_t::equals(*s, c))
                    return s;
                s++;
            }

            return nullptr;
        }

        // Compares two strings.
        template<typename equals_t>
        const cmp_t __strcmp(const char_t * s1, const char_t * s2)
        {
            _A(s1 != nullptr);
            _A(s2 != nullptr);

            for(; ; s1++, s2++)
            {
                if(equals_t::equals(*s1, *s2))
                {
                    if(*s1)
                        continue;

                    return cmp_t::equals;
                }

                return equals_t::greater_than(*s1, *s2)? cmp_t::greater : cmp_t::less;
            }
        }

        // Strings operation.
        struct __char_op_t
        {
            // Returns whether two char are equals.
            X_INLINE static bool equals(char_t c1, char_t c2)
            {
                return c1 == c2;
            }

            // Returns whether a char is greather than another.
            X_INLINE static bool greater_than(char_t c1, char_t c2)
            {
                return c1 > c2;
            }
        };

        // Strings operation. ( ignore case )
        struct __char_ignorecase_op_t
        {
            // Returns whether two char are equals. ignore case.
            X_INLINE static bool equals(char_t c1, char_t c2)
            {
                return c1 == c2 || to_upper(c1) == to_upper(c2);
            }

            // Returns whether a char is greather than another. ignore case.
            X_INLINE static bool greater_than(char_t c1, char_t c2)
            {
                return to_upper(c1) > to_upper(c2);
            }
        };
    }

    // Returns the position of the substring.
    // Returns nullptr if not found.
    const char_t * strstr(const char_t * s, const char_t * s0)
    {
        return __strstr<__char_op_t>(s, s0);
    }

    // Returns the position of the substring, ignore case.
    // Returns nullptr if not found.
    const char_t * stristr(const char_t * s, const char_t * s0)
    {
        return __strstr<__char_ignorecase_op_t>(s, s0);
    }

    // Returns the position of the specified char.
    // Returns nullptr if not found.
    const char_t * strchr(const char_t * s, char_t c)
    {
        return __strchr<__char_op_t>(s, c);
    }

    // Returns the position of the specified char, ignore case. 
    // Returns nullptr if not found.
    const char_t * strichr(const char_t * s, char_t c)
    {
        return __strchr<__char_ignorecase_op_t>(s, c);
    }

    // Compare results.
    X_ENUM_INFO(cmp_t)

        // Equals
        X_C(equals,     _T("equals"))

        // Greater
        X_C(greater,    _T("greater"))

        // Less
        X_C(less,       _T("less"))

    X_ENUM_INFO_END

    // Compares two strings.
    cmp_t strcmp(const char_t * s1, const char_t * s2)
    {
        return __strcmp<__char_op_t>(s1, s2);
    }

    // Compares two strings, ignore case.
    cmp_t stricmp(const char_t * s1, const char_t * s2)
    {
        return __strcmp<__char_ignorecase_op_t>(s1, s2);
    }

    // Write a escape format of char to a stream.
    X_ALWAYS_INLINE void __escape_char(stringstream_t & ss, char_t c)
    {
        switch(c)
        {
            case _T('\n'):
                ss << _T("\\n");
                break;

            case _T('\r'):
                ss << _T("\\r");
                break;

            case _T('\t'):
                ss << _T("\\t");
                break;

            case _T('\a'):
                ss << _T("\\a");
                break;

            case _T('\b'):
                ss << _T("\\b");
                break;

            case _T('\f'):
                ss << _T("\\f");
                break;

            case _T('\v'):
                ss << _T("\\v");
                break;

            case _T('\\'):
                ss << _T("\\\\");
                break;

            case _T('\?'):
                ss << _T("\\?");
                break;

            case _T('"'):
                ss << _T("\\\"");
                break;

            case _T('\''):
                ss << _T("\\\'");
                break;

            default:
                if((int)c < 32)
                {
                    ss << _T("\\x")
                       << (char_t)(((int)c) >> 4)
                       << (char_t)(((int)c) & 0x0F);
                }
                else
                {
                    char_t s[] = { c, _T('\0') };
                    ss << s;
                }
                break;
        }
    }

    // Converts a string to is escape format.
    string_t escape_string(const char_t * s, int length)
    {
        if(s == nullptr || s[0] == _T('\0'))
            return string_t();

        stringstream_t ss;

        if(length >= 0)
        {
            for(const char_t * s_end = s + length; s < s_end; s++)
            {
                __escape_char(ss, *s);
            }
        }
        else
        {
            char_t c;
            while((c = *s++) != _T('\0'))
            {
                __escape_char(ss, c);
            }
        }

        return ss.str();
    }

    ////////// ////////// ////////// ////////// //////////


} }  // X_ROOT_NS::algorithm

