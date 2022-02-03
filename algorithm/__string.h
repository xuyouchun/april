
#ifndef __ALGORITHM_STRING_H__
#define __ALGORITHM_STRING_H__

namespace X_ROOT_NS::algorithm {

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        template<typename c_t> struct __to_string_t { };

        //-------- ---------- ---------- ---------- ----------

        // Converts a value to string.
        template<>
        struct __to_string_t<char>
        {
            template<typename v_t>
            static std::string to_str(const v_t & v)
            {
                return std::to_string(v);
            }
        };

        //-------- ---------- ---------- ---------- ----------

        // Converts a value to a wchar string.
        template<>
        struct __to_string_t<wchar_t>
        {
            template<typename v_t>
            static std::wstring to_str(const v_t & v)
            {
                return std::to_wstring(v);
            }
        };
    }

    //-------- ---------- ---------- ---------- ----------

    // Converts a value to a specified character string.
    template<typename c_t = char_t, typename v_t> auto to_str(const v_t & v)
    {
        return __to_string_t<c_t>::template to_str<v_t>(v);
    }

    ////////// ////////// ////////// ////////// //////////

    // Character property flags.
    enum class __char_flag_t
    {
        whitespace = (1 << 0),                  // Whitespace
        upper      = (1 << 1),                  // Upper case: 'A' - 'Z'
        lower      = (1 << 2),                  // Lower case: 'a' - 'z'
        digit      = (1 << 3),                  // Numeric: '0' - '9'
        letter     = (int)upper | (int)lower,   // Litter: 'A' - 'Z', 'a' - 'z'
        word       = (1 << 4),                  // Word
        lineend    = (1 << 5),                  // Line end, '\r', '\n'
        xdigit     = (1 << 6),                  // Hex Digit, '0' - '9', 'A' - 'F', 'a' - 'f'
        odigit     = (1 << 7),                  // Oct digit, '0' - '7'
        bdigit     = (1 << 8),                  // Binary digit, '0', '1'
    };

    //-------- ---------- ---------- ---------- ----------

    // Char Properties.
    struct __char_property_t
        : public flag_property_t<__char_property_t, char, __char_flag_t>
    {
        char        lower;                      // Lower case
        char        upper;                      // Upper case
        int8_t      digit_value;                // Digit value.

        static void init(enumerator_t & e);
    };

    //-------- ---------- ---------- ---------- ----------

    typedef __char_property_t::properties_t<std::numeric_limits<char>::max()> __char_properties_t;

    extern const __char_properties_t __char_properties;

    // Returns whether a character has specified flag.
    X_INLINE bool __char_has_flag(char_t c, __char_flag_t flag, bool default_value=false)
    {
        return (size_t)c >= __char_properties_t::size? default_value
                : __char_properties[(char)c].has_flag(flag);
    }

    // Returns wheather a character is a whitespace.
    X_INLINE bool is_whitespace(char_t c)
    {
        return __char_has_flag(c, __char_flag_t::whitespace);
    }

    // Returns wheather a character is upper case.
    X_INLINE bool is_upper(char_t c)
    {
        return __char_has_flag(c, __char_flag_t::upper);
    }

    // Returns wheather a character is lower case.
    X_INLINE bool is_lower(char_t c)
    {
        return __char_has_flag(c, __char_flag_t::lower);
    }

    // Returns wheather a character is a letter.
    X_INLINE bool is_letter(char_t c)
    {
        return __char_has_flag(c, __char_flag_t::letter);
    }

    // Returns wheather a character is a digit.
    X_INLINE bool is_digit(char_t c)
    {
        return __char_has_flag(c, __char_flag_t::digit);
    }

    // Returns wheather a character is a hex digit.
    X_INLINE bool is_xdigit(char_t c)
    {
        return __char_has_flag(c, __char_flag_t::xdigit);
    }

    // Returns wheather a character is a oct digit.
    X_INLINE bool is_odigit(char_t c)
    {
        return __char_has_flag(c, __char_flag_t::odigit);
    }

    // Returns wheather a character is a binary digit.
    X_INLINE bool is_bdigit(char_t c)
    {
        return __char_has_flag(c, __char_flag_t::bdigit);
    }

    // Returns the digit value of a character.
    X_INLINE int digit_value(char_t c)
    {
        return __char_properties_t::overlimit(c)? -1 : __char_properties[c].digit_value;
    }

    // Returns wheather a character is a digit of specified radix.
    X_INLINE bool is_digit(char_t c, int radix)
    {
        return (uint_t)digit_value(c) < radix;
    }

    // Returns wheather a character is a word.
    X_INLINE bool is_word(char_t c)
    {
        return __char_has_flag(c, __char_flag_t::word);
    }

    // Returns wheather a character is a word or digit.
    X_INLINE bool is_word_or_digit(char_t c)
    {
        return __char_has_flag(c, enum_or(__char_flag_t::word, __char_flag_t::digit));
    }

    // Returns wheather a character is a line end.
    X_INLINE bool is_lineend(char_t c)
    {
        return __char_has_flag(c, __char_flag_t::lineend);
    }

    // Returns wheather a character is upper case.
    X_INLINE char_t to_upper(char_t c)
    {
        return __char_properties_t::overlimit(c)? c : __char_properties[c].upper;
    }

    // Returns wheather a character is lower case.
    X_INLINE char_t to_lower(char_t c)
    {
        return __char_properties_t::overlimit(c)? c : __char_properties[c].lower;
    }

    ////////// ////////// ////////// ////////// //////////

    // String/Array operations, like equals, has prefix, etc.

    namespace
    {
        // The default equals operation.
        template<typename type_t>
        struct __default_equals_t
        {
            X_INLINE static bool equals(type_t a, type_t b)
            {
                return a == b;
            }
        };

        // Whether an array starts with the specified prefix.
        template<typename type_t, size_t prefix_size, typename equals_t>
        struct __starts_with_t
        {
            X_INLINE static bool starts_with(const type_t * arr, const type_t * prefix)
            {
                typedef __starts_with_t<type_t, prefix_size - 1, equals_t> t;
                return equals_t::equals(*arr, *prefix) && t::starts_with(arr + 1, prefix + 1);
            }
        };

        // Whether an array starts with the specified prefix.
        template<typename type_t, typename equals_t>
        struct __starts_with_t<type_t, 0, equals_t>
        {
            X_INLINE static bool starts_with(const type_t * arr, const type_t * prefix)
            {
                return true;
            }
        };
    }

    // Returns whether an array starts with the specified prefix.
    template<typename type_t, size_t prefix_size, typename equals_t=__default_equals_t<type_t>>
    bool starts_with(const type_t * arr, size_t arr_size, const type_t (&prefix)[prefix_size])
    {
        _A(arr != nullptr);

        if (arr_size < prefix_size)
            return false;

        return __starts_with_t<type_t, prefix_size, equals_t>::starts_with(arr, prefix);
    }

    // Returns whether a string starts with the specified prefix.
    template<typename _c_t, size_t prefix_size, typename equals_t=__default_equals_t<_c_t>>
    X_INLINE bool starts_with(const _c_t * s, const _c_t * prefix)
    {
        _A(s != nullptr);

        return __starts_with_t<_c_t, prefix_size - 1, equals_t>::starts_with(s, prefix);
    }

    // Returns whether a string starts with the specified prefix.
    template<typename _c_t, size_t prefix_size, typename equals_t=__default_equals_t<_c_t>>
    X_INLINE bool starts_with(const _c_t * s, const _c_t (&prefix)[prefix_size])
    {
        return starts_with<_c_t, prefix_size, equals_t>(s, (const _c_t *)prefix);
    }

    // Returns whether a string equals to the specified string.
    template<typename _c_t, size_t prefix_size, typename equals_t=__default_equals_t<_c_t>>
    X_INLINE bool equals(const _c_t * s, const _c_t * prefix)
    {
        return starts_with<_c_t, prefix_size, equals_t>(s, prefix) && s[prefix_size - 1] == 0;
    }

    // Returns whether a string equals to the specified string.
    template<typename _c_t, size_t prefix_size, typename equals_t=__default_equals_t<_c_t>>
    X_INLINE bool equals(const _c_t * s, const _c_t (&prefix)[prefix_size])
    {
        return equals<_c_t, prefix_size, equals_t>(s, (const _c_t *)prefix);
    }

    // Converts a string to is escape format.
    string_t escape_string(const char_t * s, int length = -1);

    // Converts a string to is escape format.
    X_INLINE string_t escape_string(const string_t & s)
    {
        return escape_string(s.c_str());
    }

    ////////// ////////// ////////// ////////// //////////
    // String operations, like strstr, strcmp.

    // Returns the length of a string.
    size_t strlen(const char_t * s);

    // Copys a string to specified destination.
    char_t * strcpy(char_t * dst, const char_t * src);

    // Connects two strings.
    char_t * strcat(char_t * dst, const char_t * src);

    // Returns the position of the substring.
    // Returns nullptr if not found.
    const char_t * strstr(const char_t * s, const char_t * s0);

    // Returns the position of the substring, ignore case.
    // Returns nullptr if not found.
    const char_t * stristr(const char_t * s, const char_t * s0);

    // Returns the position of the specified char.
    // Returns nullptr if not found.
    const char_t * strchr(const char_t * s, char_t c);

    // Returns the position of the specified char, ignore case. 
    // Returns nullptr if not found.
    const char_t * strichr(const char_t * s, char_t c);

    // Compare results.
    X_ENUM(cmp_t)

        // Less
        less        = -1,

        // Equals
        equals      = 0,

        // Greater
        greater     = 1,

    X_ENUM_END

    // Compares two strings.
    cmp_t strcmp(const char_t * s1, const char_t * s2);

    // Compares two strings, ignore case.
    cmp_t stricmp(const char_t * s1, const char_t * s2);

    // Finds and replaces a substring to another.
    template<typename c_t = char_t>
    void string_replace(std::basic_string<c_t> & s,
        const std::basic_string<c_t> & src, const std::basic_string<c_t> & dst)
    {
        typedef std::basic_string<c_t> s_t;
        typedef typename s_t::size_type pos_t;

        pos_t pos;
        while ((pos = s.find(src, pos)) != s_t::npos)
        {
            s.replace(pos, src.size(), dst);
            pos += dst.size();
        }
    } 

    // Finds and replaces a substring to another.
    template<typename c_t = char_t>
    void string_replace(std::basic_string<c_t> & s, const char_t * src, const char_t * dst)
    {
        _A(src != nullptr);
        _A(dst != nullptr);

        typedef std::basic_string<c_t> s_t;
        string_replace(s, s_t(src), s_t(dst));
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns if a string is empty.
    X_INLINE bool is_empty(const char_t * s)
    {
        return s == nullptr || s[0] == _T('\0');
    }

    // Returns if a string is empty.
    X_INLINE bool is_empty(const string_t & s)
    {
        return s.length() == 0;
    }

    ////////// ////////// ////////// ////////// //////////

    // Joins strings with the specified separator.
    template<typename itor_t>
    void join_str(stringstream_t & ss, itor_t begin, itor_t end, const string_t & sep)
    {
        for (itor_t it = begin; it != end; it++)
        {
            if (it != begin)
                ss << sep.c_str();

            ss << *it;
        }
    }

    // Joins strings with the specified separator.
    template<typename itor_t>
    string_t join_str(itor_t begin, itor_t end, const string_t & sep)
    {
        stringstream_t ss;
        join_str(ss, begin, end, sep);
        return ss.str();
    }

    // Joins strings with the specified separator.
    template<typename itor_t, typename strfunc_t>
    void join_str(stringstream_t & ss, itor_t begin, itor_t end, const string_t & sep,
                                                                    strfunc_t strfunc)
    {
        for (itor_t it = begin; it != end; it++)
        {
            if (it != begin)
                ss << sep.c_str();

            ss << strfunc(*it);
        }
    }

    // Joins strings with the specified separator.
    template<typename itor_t, typename strfunc_t>
    string_t join_str(itor_t begin, itor_t end, const string_t & sep, strfunc_t strfunc)
    {
        stringstream_t ss;
        join_str(ss, begin, end, sep, strfunc);
        return ss.str();
    }

    namespace
    {
        void __join(stringstream_t & ss, const string_t & sep) { }

        template<typename first_t, typename ... rest_t>
        void __join(stringstream_t & ss, const string_t & sep, first_t first, rest_t ... rest)
        {
            ss << sep.c_str();
            ss << first;

            __join(ss, sep, rest...);
        }

        template<typename first_t, typename ... rest_t>
        void __join_head(stringstream_t & ss, const string_t & sep, first_t first, rest_t ... rest)
        {
            ss << first;

            __join(ss, sep, rest ...);
        }
    }

    // Joins values to a string with the specified separator.
    template<typename ... values_t>
    string_t join(const string_t & sep, values_t ... values)
    {
        stringstream_t ss;
        if (sizeof ... (values) > 0)
        {
            __join_head(ss, sep, values ...);
        }

        return ss.str();
    }

    ////////// ////////// ////////// ////////// //////////

    // A character sequence specified with start and end position.

    template<typename c_t = char_t>
    class cptr_t : public compare_operators_t<cptr_t<c_t>, const c_t *>
    {
        typedef cptr_t<c_t> __self_t;
        cptr_t(const c_t * p, const c_t * p_end)
            : __p(p), __p_end(p_end) { }

    public:

        // Constructor with start position and length.
        cptr_t(const c_t * p, size_t length)
            : __p(p), __p_end(p + length) { }

        // Empty cptr
        cptr_t(std::nullptr_t)
            : __p(nullptr), __p_end(nullptr) { }

        // Returns the character value, '\0' and the end.
        c_t operator * () const
        {
            return __p < __p_end? *__p : _T('\0');
        }

        // Moves to the next character.
        cptr_t & operator ++()
        {
            ++__p;
            return *this;
        }

        // Moves to the next character.
        cptr_t operator ++(int)
        {
            __self_t self = *this;
            ++__p;
            return self;
        }

        // Moves to the previous character.
        cptr_t & operator --()
        {
            --__p;
            return *this;
        }

        // Moves to he previous character.
        cptr_t operator --(int)
        {
            __self_t self = *this;
            --__p;
            return self;
        }

        // Returns the differences of two iterators.
        size_t operator - (const __self_t & other) const
        {
            return __p - other.__p;
        }

        // Returns a new position decrement by the offset.
        __self_t operator - (size_t offset) const
        {
            return __self_t(__p - offset, __p_end);
        }

        // Move to a new position decrement by the offset.
        __self_t operator -= (size_t offset)
        {
            __p -= offset;
            return *this;
        }

        // Returns a new position increment by the offset
        __self_t operator + (size_t offset) const
        {
            return __self_t(__p + offset, __p_end);
        }

        // Moves a new position increment by the offset
        __self_t operator += (size_t offset)
        {
            __p += offset;
            return *this;
        }

        // Returns the character pointer.
        operator const c_t *() const { return __p; }

        // Returns whether two iterators are equals.
        __self_t & operator = (const __self_t & other)
        {
            __p = other.__p;
            __p_end = other.__p_end;

            return *this;
        }

        // Assigns a new position.
        __self_t & operator = (const c_t * p)
        {
            __p = p;
            return *this;
        }

        // Returns whether two pointers are equals.
        bool operator == (const char_t * p) const
        {
            return __p == p;
        }

        // Returns whether two iterators are equals.
        bool operator == (const __self_t & other) const
        {
            return __p == other.__p;
        }

        // Returns whether two pointers are not equals.
        bool operator != (const char_t * p) const
        {
            return __p != p;
        }

        // Returns whether two itertors are not equals.
        bool operator != (const __self_t & other) const
        {
            return __p != other.__p;
        }

    private:
        const c_t * __p, * const __p_end;
    };

    // Creates a new cptr_t.
    template<typename c_t = char_t>
    cptr_t<c_t> cptr(const c_t * p, size_t length)
    {
        return cptr_t<c_t>(p, length);
    }

    ////////// ////////// ////////// ////////// //////////

    // A string id of type iterator.
    // Be used for storing a string to a pool, same strings have the same id.
    // Compare two strings only by its ids, if they are in the same pool.

    template<typename c_t = char_t>
    struct tsid_t : compare_operators_t<tsid_t<c_t>, const std::basic_string<c_t> *>
    {
        typedef std::basic_string<c_t> __string_t;
        typedef const __string_t * __sid_value_t;

        tsid_t(__sid_value_t value) : value(value) { }
        tsid_t() : tsid_t(nullptr) { }

        __sid_value_t value;

        operator __sid_value_t () const { return value; }

        // Returns whether the string is nullptr.
        bool is_null() const  { return value == nullptr; }

        // Returns whether the string is empty.
        bool is_empty() const { return length() == 0; }

        // Returns string value.
        operator __string_t() const
        {
            return value == nullptr? empty_str : *value;
        }

        // Returns character pointer.
        const c_t * c_str() const { return value == nullptr? nullptr : value->c_str(); }

        // Returns void * pointer.
        operator void * () const { return (void *)c_str(); }

        // Returns length of the string.
        size_t length() const { return value == nullptr? 0 : value->length(); }

        // The constant null string id.
        static const tsid_t<c_t> null;
    };

    // The constant null string id.
    template<typename c_t> const tsid_t<c_t> tsid_t<c_t>::null(nullptr);

    // Writing a string id to a stream.
    template<typename stream_t, typename c_t>
    stream_t & operator << (stream_t & stream, const tsid_t<c_t> & sid)
    {
        return stream << sid.c_str(), stream;
    }

    //-------- ---------- ---------- ---------- ----------

    // String pool, The same strings having the same string id.

    template<typename c_t = char_t>
    class tspool_t : public object_t, public no_copy_ctor_t
    {
        typedef std::basic_string<c_t> __string_t;

    public:
        typedef tsid_t<c_t> sid_t;

        tspool_t() { }

        // Puts a string into the pool, and returns a string id.
        sid_t to_sid(const __string_t & s)
        {
            auto r = __string_set.insert(s);
            return sid_t(&(*r.first));
        }

        X_TO_STRING_IMPL(_T("tspool_t"))

    private:
        std::set<const __string_t> __string_set;
    };

    typedef tsid_t<char_t>   sid_t;
    typedef tspool_t<char_t> spool_t;

    ////////// ////////// ////////// ////////// //////////
    // Regex to match strings.
    // The useful functions of packing std::regex.

    namespace
    {
        template<typename c_t = char_t>
        class __regex_operations_t
        {
            typedef std::basic_string<c_t> __string_t;
            typedef std::basic_regex<c_t> __regex_t;
            typedef typename __string_t::const_iterator __string_itor_t;
            typedef std::regex_iterator<__string_itor_t> __regex_itor_t;

        public:
            typedef __string_itor_t string_itor_t;
            __regex_operations_t(const __regex_t & regex) : __regex(regex) { }

            // Matches string
            template<typename callback_t>
            void match(__string_itor_t begin, __string_itor_t end, callback_t callback)
            {
                for (__regex_itor_t it(begin, end, __regex), it_end; it != it_end; it++)
                {
                    callback(it->str());
                }
            }

            // Returns whether the string is matched.
            bool is_match(__string_itor_t begin, __string_itor_t end)
            {
                for (__regex_itor_t it(begin, end, __regex), it_end; it != it_end; it++)
                {
                    return true;
                }

                return false;
            }

            // Matches the first string.
            string_t match_one(__string_itor_t begin, __string_itor_t end)
            {
                for (__regex_itor_t it(begin, end, __regex), it_end; it != it_end; it++)
                {
                    return it->str();
                }

                return _T("");
            }

        private:
            const __regex_t & __regex;
        };

        template<typename c_t>
        using __string_itor_t = typename __regex_operations_t<c_t>::string_itor_t;
    }

    // Matches string.
    // Return matched strings by calling callback function.
    template<typename callback_t, typename c_t = char_t>
    void regex_match(const std::basic_string<c_t> & s,
                            const std::basic_regex<c_t> & regex, callback_t callback)
    {
        __regex_operations_t<c_t>(regex).match(s.begin(), s.end(), callback);
    }

    // Matches string.
    // Return matched strings by calling callback function.
    template<typename callback_t, typename c_t = char_t>
    void regex_match(const std::basic_string<c_t> & s,
                            const string_t & regex, callback_t callback)
    {
        typedef std::basic_regex<char_t> regex_t;
        __regex_operations_t<c_t>(regex_t(regex)).match(s.begin(), s.end(), callback);
    }

    // Returns whether the string is match the regex.
    template<typename c_t = char_t>
    bool regex_is_match(const std::basic_string<c_t> & s, const std::basic_regex<c_t> & regex)
    {
        return __regex_operations_t<c_t>(regex).is_match(s.begin(), s.end());
    }

    // Returns whether the string is match the regex.
    template<typename c_t = char_t>
    bool regex_is_match(const std::basic_string<c_t> & s, const string_t & regex)
    {
        typedef std::basic_regex<char_t> regex_t;
        return __regex_operations_t<c_t>(regex_t(regex)).is_match(s.begin(), s.end());
    }

    // Matches the first string.
    // Returns empty string if fault.
    template<typename c_t = char_t>
    bool regex_match_one(const std::basic_string<c_t> & s, const std::basic_regex<c_t> & regex)
    {
        return __regex_operations_t<c_t>(regex).match_one(s.begin(), s.end());
    }

    // Matches the first string.
    // Returns empty string if fault.
    template<typename c_t = char_t>
    bool regex_match_one(const std::basic_string<c_t> & s, const string_t & regex)
    {
        typedef std::basic_regex<char_t> regex_t;
        return __regex_operations_t<c_t>(regex_t(regex)).match_one(s.begin(), s.end());
    }

    ////////// ////////// ////////// ////////// //////////

}   // X_ROOT_NS::algorithm

#endif  // __ALGORITHM_STRING_H__

