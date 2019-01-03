
#ifndef __ALGORITHM_STRING_H__
#define __ALGORITHM_STRING_H__

namespace X_ROOT_NS { namespace algorithm {

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        template<typename c_t> struct __to_string_t { };

        //-------- ---------- ---------- ---------- ----------

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

    template<typename c_t = char_t, typename v_t> auto to_str(const v_t & v)
    {
        return __to_string_t<c_t>::template to_str<v_t>(v);
    }

    ////////// ////////// ////////// ////////// //////////

    enum class __char_flag_t
    {
        whitespace = (1 << 0),
        upper      = (1 << 1),
        lower      = (1 << 2),
        digit      = (1 << 3),
        letter     = (int)upper | (int)lower,
        word       = (1 << 4),
        lineend    = (1 << 5),
        xdigit     = (1 << 6),
        odigit     = (1 << 7),
        bdigit     = (1 << 8),
    };

    //-------- ---------- ---------- ---------- ----------

    struct __char_property_t
        : public flag_property_t<__char_property_t, char, __char_flag_t>
    {
        char        lower;
        char        upper;
        int8_t      digit_value;

        static void init(enumerator_t & e);
    };

    //-------- ---------- ---------- ---------- ----------

    typedef __char_property_t::properties_t<std::numeric_limits<char>::max()> __char_properties_t;

    extern const __char_properties_t __char_properties;

    X_INLINE bool __char_has_flag(char_t c, __char_flag_t flag, bool default_value=false)
    {
        return (size_t)c >= __char_properties_t::size? default_value
                : __char_properties[(char)c].has_flag(flag);
    }

    X_INLINE bool is_whitespace(char_t c)
    {
        return __char_has_flag(c, __char_flag_t::whitespace);
    }

    X_INLINE bool is_upper(char_t c)
    {
        return __char_has_flag(c, __char_flag_t::upper);
    }

    X_INLINE bool is_lower(char_t c)
    {
        return __char_has_flag(c, __char_flag_t::lower);
    }

    X_INLINE bool is_letter(char_t c)
    {
        return __char_has_flag(c, __char_flag_t::letter);
    }

    X_INLINE bool is_digit(char_t c)
    {
        return __char_has_flag(c, __char_flag_t::digit);
    }

    X_INLINE bool is_xdigit(char_t c)
    {
        return __char_has_flag(c, __char_flag_t::xdigit);
    }

    X_INLINE bool is_odigit(char_t c)
    {
        return __char_has_flag(c, __char_flag_t::odigit);
    }

    X_INLINE bool is_bdigit(char_t c)
    {
        return __char_has_flag(c, __char_flag_t::bdigit);
    }

    X_INLINE int digit_value(char_t c)
    {
        return __char_properties_t::overlimit(c)? -1 : __char_properties[c].digit_value;
    }

    X_INLINE bool is_digit(char_t c, int radix)
    {
        return (uint_t)digit_value(c) < radix;
    }

    X_INLINE bool is_word(char_t c)
    {
        return __char_has_flag(c, __char_flag_t::word);
    }

    X_INLINE bool is_word_or_digit(char_t c)
    {
        return __char_has_flag(c, enum_or(__char_flag_t::word, __char_flag_t::digit));
    }

    X_INLINE bool is_lineend(char_t c)
    {
        return __char_has_flag(c, __char_flag_t::lineend);
    }

    X_INLINE char_t to_upper(char_t c)
    {
        return __char_properties_t::overlimit(c)? c : __char_properties[c].upper;
    }

    X_INLINE char_t to_lower(char_t c)
    {
        return __char_properties_t::overlimit(c)? c : __char_properties[c].lower;
    }

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        template<typename type_t>
        struct __default_equals_t
        {
            X_INLINE static bool equals(type_t a, type_t b)
            {
                return a == b;
            }
        };

        template<typename type_t, size_t prefix_size, typename equals_t>
        struct __starts_with_t
        {
            X_INLINE static bool starts_with(const type_t * arr, const type_t * prefix)
            {
                typedef __starts_with_t<type_t, prefix_size - 1, equals_t> t;
                return equals_t::equals(*arr, *prefix) && t::starts_with(arr + 1, prefix + 1);
            }
        };

        template<typename type_t, typename equals_t>
        struct __starts_with_t<type_t, 0, equals_t>
        {
            X_INLINE static bool starts_with(const type_t * arr, const type_t * prefix)
            {
                return true;
            }
        };
    }

    template<typename type_t, size_t prefix_size, typename equals_t=__default_equals_t<type_t>>
    bool starts_with(const type_t * arr, size_t arr_size, const type_t (&prefix)[prefix_size])
    {
        _A(arr != nullptr);

        if(arr_size < prefix_size)
            return false;

        return __starts_with_t<type_t, prefix_size, equals_t>::starts_with(arr, prefix);
    }

    template<size_t prefix_size, typename equals_t=__default_equals_t<char_t>>
    X_INLINE bool starts_with(const char_t * s, const char_t * prefix)
    {
        _A(s != nullptr);

        return __starts_with_t<char_t, prefix_size - 1,equals_t>::starts_with(s, prefix);
    }

    template<size_t prefix_size, typename equals_t=__default_equals_t<char_t>>
    X_INLINE bool starts_with(const char_t * s, const char_t (&prefix)[prefix_size])
    {
        return starts_with<prefix_size, equals_t>(s, (const char_t *)prefix);
    }

    template<size_t prefix_size, typename equals_t=__default_equals_t<char_t>>
    X_INLINE bool equals(const char_t * s, const char_t * prefix)
    {
        return starts_with<prefix_size, equals_t>(s, prefix) && s[prefix_size - 1] == _T('\0');
    }

    template<size_t prefix_size, typename equals_t=__default_equals_t<char_t>>
    X_INLINE bool equals(const char_t * s, const char_t (&prefix)[prefix_size])
    {
        return equals<prefix_size, equals_t>(s, (const char_t *)prefix);
    }

    string_t escape_string(const char_t * s, int length = -1);

    X_INLINE string_t escape_string(const string_t & s)
    {
        return escape_string(s.c_str());
    }

    ////////// ////////// ////////// ////////// //////////
    // String

    size_t strlen(const char_t * s);
    char_t * strcpy(char_t * dst, const char_t * src);
    char_t * strcat(char_t * dst, const char_t * src);
    const char_t * strstr(const char_t * s, const char_t * s0);
    const char_t * stristr(const char_t * s, const char_t * s0);
    const char_t * strchr(const char_t * s, char_t c);
    const char_t * strichr(const char_t * s, char_t c);

    X_ENUM(cmp_t)

        less        = -1,

        equals      = 0,

        greater     = 1,

    X_ENUM_END

    cmp_t strcmp(const char_t * s1, const char_t * s2);
    cmp_t stricmp(const char_t * s1, const char_t * s2);

    template<typename c_t = char_t>
    void string_replace(std::basic_string<c_t> & s,
        const std::basic_string<c_t> & src, const std::basic_string<c_t> & dst)
    {
        typedef std::basic_string<c_t> s_t;
        typedef typename s_t::size_type pos_t;

        pos_t pos;
        while((pos = s.find(src, pos)) != s_t::npos)
        {
            s.replace(pos, src.size(), dst);
            pos += dst.size();
        }
    } 

    template<typename c_t = char_t>
    void string_replace(std::basic_string<c_t> & s, const char_t * src, const char_t * dst)
    {
        _A(src != nullptr);
        _A(dst != nullptr);

        typedef std::basic_string<c_t> s_t;
        string_replace(s, s_t(src), s_t(dst));
    }

    ////////// ////////// ////////// ////////// //////////

    X_INLINE bool is_empty(const char_t * s)
    {
        return s == nullptr || s[0] == _T('\0');
    }

    X_INLINE bool is_empty(const string_t & s)
    {
        return s.length() == 0;
    }

    ////////// ////////// ////////// ////////// //////////

    template<typename itor_t>
    void join_str(stringstream_t & ss, itor_t begin, itor_t end, const string_t & sep)
    {
        for(itor_t it = begin; it != end; it++)
        {
            if(it != begin)
                ss << sep.c_str();

            ss << *it;
        }
    }

    template<typename itor_t>
    string_t join_str(itor_t begin, itor_t end, const string_t & sep)
    {
        stringstream_t ss;
        join_str(ss, begin, end, sep);
        return ss.str();
    }

    template<typename itor_t, typename strfunc_t>
    void join_str(stringstream_t & ss, itor_t begin, itor_t end, const string_t & sep,
                                                                    strfunc_t strfunc)
    {
        for(itor_t it = begin; it != end; it++)
        {
            if(it != begin)
                ss << sep.c_str();

            ss << strfunc(*it);
        }
    }

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

    template<typename ... values_t>
    string_t join(const string_t & sep, values_t ... values)
    {
        stringstream_t ss;
        if(sizeof ... (values) > 0)
        {
            __join_head(ss, sep, values ...);
        }

        return ss.str();
    }

    ////////// ////////// ////////// ////////// //////////

    template<typename c_t = char_t>
    class cptr_t : public compare_operators_t<cptr_t<c_t>, const c_t *>
    {
        typedef cptr_t<c_t> __self_t;
        cptr_t(const c_t * p, const c_t * p_end)
            : __p(p), __p_end(p_end) { }

    public:
        cptr_t(const c_t * p, size_t length)
            : __p(p), __p_end(p + length) { }

        cptr_t(std::nullptr_t)
            : __p(nullptr), __p_end(nullptr) { }

        c_t operator * () const
        {
            return __p < __p_end? *__p : _T('\0');
        }

        cptr_t & operator ++()
        {
            ++__p;
            return *this;
        }

        cptr_t operator ++(int)
        {
            __self_t self = *this;
            ++__p;
            return self;
        }

        cptr_t & operator --()
        {
            --__p;
            return *this;
        }

        cptr_t operator --(int)
        {
            __self_t self = *this;
            --__p;
            return self;
        }

        size_t operator - (const __self_t & other) const
        {
            return __p - other.__p;
        }

        __self_t operator - (size_t offset) const
        {
            return __self_t(__p - offset, __p_end);
        }

        __self_t operator -= (size_t offset)
        {
            __p -= offset;
            return *this;
        }

        __self_t operator + (size_t offset) const
        {
            return __self_t(__p + offset, __p_end);
        }

        __self_t operator += (size_t offset)
        {
            __p += offset;
            return *this;
        }

        operator const c_t *() const { return __p; }

        __self_t & operator = (const __self_t & other)
        {
            __p = other.__p;
            __p_end = other.__p_end;

            return *this;
        }

        __self_t & operator = (const c_t * p)
        {
            __p = p;
            return *this;
        }

        bool operator == (const char_t * p) const
        {
            return __p == p;
        }

        bool operator == (const __self_t & other) const
        {
            return __p == other.__p;
        }

        bool operator != (const char_t * p) const
        {
            return __p != p;
        }

        bool operator != (const __self_t & other) const
        {
            return __p != other.__p;
        }

    private:
        const c_t * __p, * const __p_end;
    };

    template<typename c_t = char_t>
    cptr_t<c_t> cptr(const c_t * p, size_t length)
    {
        return cptr_t<c_t>(p, length);
    }

    ////////// ////////// ////////// ////////// //////////

    template<typename c_t = char_t>
    struct tsid_t : compare_operators_t<tsid_t<c_t>, const std::basic_string<c_t> *>
    {
        typedef std::basic_string<c_t> __string_t;
        typedef const __string_t * __sid_value_t;

        tsid_t(__sid_value_t value) : value(value) { }
        tsid_t() : tsid_t(nullptr) { }

        __sid_value_t value;

        operator __sid_value_t () const { return value; }

        bool is_null() const  { return value == nullptr; }
        bool is_empty() const { return length() == 0; }

        operator __string_t() const
        {
            return value == nullptr? empty_str : *value;
        }

        const c_t * c_str() const { return value == nullptr? nullptr : value->c_str(); }
        operator void * () const { return (void *)c_str(); }
        size_t length() const { return value == nullptr? 0 : value->length(); }

        static const tsid_t<c_t> null;
    };

    template<typename c_t> const tsid_t<c_t> tsid_t<c_t>::null(nullptr);

    template<typename stream_t, typename c_t>
    stream_t & operator << (stream_t & stream, const tsid_t<c_t> & sid)
    {
        return stream << sid.c_str(), stream;
    }

    //-------- ---------- ---------- ---------- ----------

    template<typename c_t = char_t>
    class tspool_t : public object_t, public no_copy_ctor_t
    {
        typedef std::basic_string<c_t> __string_t;

    public:
        typedef tsid_t<c_t> sid_t;

        tspool_t() { }

        sid_t to_sid(const __string_t & s)
        {
            auto r = __string_set.insert(s);
            return sid_t(&(*r.first));
        }

    private:
        std::set<const __string_t> __string_set;
    };

    typedef tsid_t<char_t>   sid_t;
    typedef tspool_t<char_t> spool_t;

    ////////// ////////// ////////// ////////// //////////
    // regex

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

            template<typename callback_t>
            void match(__string_itor_t begin, __string_itor_t end, callback_t callback)
            {
                for(__regex_itor_t it(begin, end, __regex), it_end; it != it_end; it++)
                {
                    callback(it->str());
                }
            }

            bool is_match(__string_itor_t begin, __string_itor_t end)
            {
                for(__regex_itor_t it(begin, end, __regex), it_end; it != it_end; it++)
                {
                    return true;
                }

                return false;
            }

            string_t match_one(__string_itor_t begin, __string_itor_t end)
            {
                for(__regex_itor_t it(begin, end, __regex), it_end; it != it_end; it++)
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

    template<typename callback_t, typename c_t = char_t>
    void regex_match(const std::basic_string<c_t> & s,
                            const std::basic_regex<c_t> & regex, callback_t callback)
    {
        __regex_operations_t<c_t>(regex).match(s.begin(), s.end(), callback);
    }

    template<typename callback_t, typename c_t = char_t>
    void regex_match(const std::basic_string<c_t> & s,
                            const string_t & regex, callback_t callback)
    {
        typedef std::basic_regex<char_t> regex_t;
        __regex_operations_t<c_t>(regex_t(regex)).match(s.begin(), s.end(), callback);
    }

    template<typename c_t = char_t>
    bool regex_is_match(const std::basic_string<c_t> & s, const std::basic_regex<c_t> & regex)
    {
        return __regex_operations_t<c_t>(regex).is_match(s.begin(), s.end());
    }

    template<typename c_t = char_t>
    bool regex_is_match(const std::basic_string<c_t> & s, const string_t & regex)
    {
        typedef std::basic_regex<char_t> regex_t;
        return __regex_operations_t<c_t>(regex_t(regex)).is_match(s.begin(), s.end());
    }

    template<typename c_t = char_t>
    bool regex_match_one(const std::basic_string<c_t> & s, const std::basic_regex<c_t> & regex)
    {
        return __regex_operations_t<c_t>(regex).match_one(s.begin(), s.end());
    }

    template<typename c_t = char_t>
    bool regex_match_one(const std::basic_string<c_t> & s, const string_t & regex)
    {
        typedef std::basic_regex<char_t> regex_t;
        return __regex_operations_t<c_t>(regex_t(regex)).match_one(s.begin(), s.end());
    }


    ////////// ////////// ////////// ////////// //////////

} }  // X_ROOT_NS::algorithm

#endif  // __ALGORITHM_STRING_H__

