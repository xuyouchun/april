
#ifndef __COMMON_STRING_H__
#define __COMMON_STRING_H__

////////// ////////// ////////// ////////// //////////
// String convert functions

namespace X_ROOT_NS
{
    namespace
    {
        ////////// ////////// ////////// ////////// //////////
        // __string_info_t

        template<typename __string_t> constexpr size_t __size_of_char()
        {
            return sizeof(typename __string_t::value_type);
        }

        const size_t __u8string_size  = __size_of_char<std::string>();
        const size_t __u16string_size = __size_of_char<std::u16string>();
        const size_t __u32string_size = __size_of_char<std::u32string>();

        template<size_t char_size> struct __string_info_t { };

        // Defines character type of u8string.
        template<> struct __string_info_t<__u8string_size>
        {
            typedef std::string string_type;
            typedef typename string_type::value_type char_type;
        };

        // Defines character type of u16string.
        template<> struct __string_info_t<__u16string_size>
        {
            typedef std::u16string string_type;
            typedef typename string_type::value_type char_type;
        };

        // Defines character type of u32string.
        template<> struct __string_info_t<__u32string_size>
        {
            typedef std::u32string string_type;
            typedef typename string_type::value_type char_type;
        };

        typedef typename __string_info_t<sizeof(wchar_t)>::char_type __wchar_t;

        ////////// ////////// ////////// ////////// //////////

        const std::codecvt_mode __codecvt_mode = (std::codecvt_mode)std::little_endian;
        const unsigned long __max_code = 0x10ffff;

        // String converter of utf8
        template<typename c_t>
        using __codecvt_utf8 = std::codecvt_utf8<c_t, __max_code, __codecvt_mode>;

        // String converter of utf16
        template<typename c_t>
        using __codecvt_utf16 = std::codecvt_utf16<c_t, __max_code, __codecvt_mode>;

        // __string_convert_function_t
        template<typename ch_t, typename codecvt_t>
        class __string_convert_function_t
        {
        protected:
            std::wstring_convert<codecvt_t, ch_t> _converter;
        };

        //-------- ---------- ---------- ---------- ----------

        // Converts bytes to string.
        template<typename from_ch_t, typename to_ch_t, typename codecvt_t>
        class __string_convert_from_bytes_t
            : public __string_convert_function_t<to_ch_t, codecvt_t>
        {
        public:
            template<typename ... args_t>
            X_INLINE std::basic_string<to_ch_t> convert (args_t && ... args)
            {
                return this->_converter.from_bytes(std::forward<args_t>(args) ...);
            }
        };

        //-------- ---------- ---------- ---------- ----------

        // Converts string to bytes.
        template<typename from_ch_t, typename to_ch_t, typename codecvt_t>
        class __string_convert_to_bytes_t
            : public __string_convert_function_t<from_ch_t, codecvt_t>
        {
        public:
            template<typename ... args_t>
            X_INLINE std::basic_string<to_ch_t> convert(args_t && ... args)
            {
                std::string s = this->_converter.to_bytes(std::forward<args_t>(args) ...);
                return std::basic_string<to_ch_t>((const to_ch_t *)s.c_str(),
                        s.length() / sizeof(to_ch_t));
            }
        };

        // Conters string to bytes (char).
        template<typename from_ch_t, typename codecvt_t>
        class __string_convert_to_bytes_t<from_ch_t, char, codecvt_t>
            : public __string_convert_function_t<from_ch_t, codecvt_t>
        {
        public:
            template<typename ... args_t>
            X_INLINE std::basic_string<char> convert(args_t && ... args)
            {
                return this->_converter.to_bytes(std::forward<args_t>(args) ...);
            }
        };

        //-------- ---------- ---------- ---------- ----------

        // An empty convert function.
        template<typename ch_t>
        class __string_convert_empty_function_t
        {
        public:
            template<typename ... args_t>
            X_INLINE std::basic_string<ch_t> convert(args_t && ... args)
            {
                return std::basic_string<ch_t>(std::forward<args_t>(args) ...);
            }
        };

        ////////// ////////// ////////// ////////// //////////
        // The base converter.

        template<typename from_ch_t, typename to_ch_t, typename function_t>
        class __string_convert_base_t : public function_t
        {
        public:
            template<typename ... args_t>
            X_INLINE std::basic_string<to_ch_t> operator() (args_t && ... args)
            {
                return this->convert(std::forward<args_t>(args) ...);
            }
        };

        //-------- ---------- ---------- ---------- ----------
        // The ex-base converter. with more functions to convert bytes to string.

        template<typename from_ch_t, typename to_ch_t, typename function_t>
        class __string_convert_base_ex_t : public function_t
        {
        public:
            X_INLINE std::basic_string<to_ch_t> operator() 
                    (const from_ch_t * first, const from_ch_t * last)
            {
                return this->convert((const char *)first, (const char *)last);
            }

            X_INLINE std::basic_string<to_ch_t> operator() (const from_ch_t * bytes)
            {
                return this->operator()(bytes, bytes + __len(bytes));
            }

            X_INLINE std::basic_string<to_ch_t> operator()
                    (const std::basic_string<from_ch_t> & wstr)
            {
                const from_ch_t * s = wstr.c_str();
                return this->operator()(s, s + wstr.length());
            }

        private:
            static size_t __len(const from_ch_t * bytes)
            {
                for (const from_ch_t * p = bytes; ; p++)
                {
                    if (!*p)
                        return p - bytes;
                }
            }
        };

        ////////// ////////// ////////// ////////// //////////
        // __string_convert_t

        template<typename from_ch_t, typename to_ch_t> class __string_convert_t { };

        //-------- ---------- ---------- ---------- ----------

        // Converts to char
        template<typename from_ch_t> class __string_convert_t<from_ch_t, char>
            : public __string_convert_base_t<from_ch_t, char,
                    __string_convert_to_bytes_t<from_ch_t, char, __codecvt_utf8<from_ch_t>>>
        { };

        //-------- ---------- ---------- ---------- ----------

        // Converts from char
        template<typename to_ch_t>
        class __string_convert_t<char, to_ch_t>
            : public __string_convert_base_t<char, to_ch_t,
                    __string_convert_from_bytes_t<char, to_ch_t, __codecvt_utf8<to_ch_t>>>
        { };

        //-------- ---------- ---------- ---------- ----------

        // From char to char. (no convertion)
        template<>
        class __string_convert_t<char, char>
            : public __string_convert_base_t<char, char, __string_convert_empty_function_t<char>>
        { };

        //-------- ---------- ---------- ---------- ----------

        // Converts char32 to char16
        template<>
        class __string_convert_t<char32_t, char16_t>
            : public __string_convert_base_t<char32_t, char16_t, \
                    __string_convert_to_bytes_t<char32_t, char16_t, __codecvt_utf16<char32_t>>>
        { };

        //-------- ---------- ---------- ---------- ----------

        // Converts char16 to char32
        template<>
        class __string_convert_t<char16_t, char32_t>
            : public __string_convert_base_ex_t<char16_t, char32_t, \
                    __string_convert_from_bytes_t<char16_t, char32_t, __codecvt_utf16<char32_t>>>
        { };

        //-------- ---------- ---------- ---------- ----------
        #if WCHAR_MAX > 0xFFFF  // wchar_t == char16_t on Windows: covered by __wchar_t below.


        // Converts char16 to wchar_t
        template<>
        class __string_convert_t<char16_t, wchar_t>
            : public __string_convert_base_ex_t<char16_t, wchar_t, \
                    __string_convert_from_bytes_t<char16_t, wchar_t, __codecvt_utf16<wchar_t>>>
        { };

        #endif

        //-------- ---------- ---------- ---------- ----------

        // Converts char16 to char16. (no convertion)
        template<>
        class __string_convert_t<char16_t, char16_t>
            : public __string_convert_base_t<char16_t, char16_t,
                    __string_convert_empty_function_t<char16_t>>
        { };

        //-------- ---------- ---------- ---------- ----------

        // Converts char32 to char32. (no convertion)

        template<>
        class __string_convert_t<char32_t, char32_t>
            : public __string_convert_base_t<char32_t, char32_t,
                    __string_convert_empty_function_t<char32_t>>
        { };

        //-------- ---------- ---------- ---------- ----------

        // Converts wchar_t to __wchar_t (determines by the platform).

        template<>
        class __string_convert_t<wchar_t, __wchar_t>
        {
        public:
            X_INLINE std::basic_string<__wchar_t> operator() (const wchar_t * p)
            {
                return std::basic_string<__wchar_t>((const __wchar_t *)p);
            }

            X_INLINE std::basic_string<__wchar_t> operator()
                    (const wchar_t * first, const wchar_t * last)
            {
                return std::basic_string<__wchar_t>( \
                        (const __wchar_t *)first, (const __wchar_t *)last);
            }

            X_INLINE std::basic_string<__wchar_t> operator()
                    (const std::basic_string<wchar_t> & wstr)
            {
                const __wchar_t * s = (const __wchar_t *)wstr.c_str();
                return std::basic_string<__wchar_t>(s, s + wstr.length());
            }
        };

        //-------- ---------- ---------- ---------- ----------

        // Converts __wchar_t (determines by the platform) to wchar_t.
        template<>
        class __string_convert_t<__wchar_t, wchar_t>
        {
        public:
            X_INLINE std::basic_string<wchar_t> operator() (const __wchar_t * p)
            {
                return std::basic_string<wchar_t>((const wchar_t *)p);
            }

            X_INLINE std::basic_string<wchar_t> operator()
                    (const __wchar_t * first, const __wchar_t * last)
            {
                return std::basic_string<wchar_t>( \
                        (const wchar_t *)first, (const wchar_t *)last);
            }

            X_INLINE std::basic_string<wchar_t> operator()
                    (const std::basic_string<__wchar_t> & wstr)
            {
                const wchar_t * s = (const wchar_t *)wstr.c_str();
                return std::basic_string<wchar_t>(s, s + wstr.length());
            }
        };

        ////////// ////////// ////////// ////////// //////////

        // A function wrapper for convertion.
        template<typename from_ch_t, typename to_ch_t>
        class __string_convert_call_t
        {
        public:
            template<typename ... args_t>
            X_INLINE std::basic_string<to_ch_t> operator() (args_t && ... args)
            {
                __string_convert_t<from_ch_t, to_ch_t> f;
                return f(std::forward<args_t>(args) ...);
            }
        };

        //-------- ---------- ---------- ---------- ----------

        // A function wrapper for convertion. ( for wchar_t )
        template<typename to_ch_t>
        class __string_convert_call_t<wchar_t, to_ch_t>
        {
        public:
            template<typename ... args_t>
            X_INLINE std::basic_string<to_ch_t> operator() (const wchar_t * p)
            {
                __string_convert_t<__wchar_t, to_ch_t> f;
                return f((const __wchar_t *)p);
            }

            template<typename ... args_t>
            X_INLINE std::basic_string<to_ch_t> operator()
                    (const wchar_t * first, const wchar_t * last)
            {
                __string_convert_t<__wchar_t, to_ch_t> f;
                return f((const __wchar_t *)first, (const __wchar_t *)last);
            }

            template<typename ... args_t>
            X_INLINE std::basic_string<to_ch_t> operator() (const std::basic_string<wchar_t> & wstr)
            {
                __string_convert_t<__wchar_t, to_ch_t> f;
                const __wchar_t * s = (const __wchar_t *)wstr.c_str();
                return f(s, s + wstr.length());
            }
        };

        //-------- ---------- ---------- ---------- ----------

        // A function wrapper for convertion. ( From wchar_t to wchar_t. no convertion. )
        template<>
        class __string_convert_call_t<wchar_t, wchar_t>
        {
        public:
            template<typename ... args_t>
            std::basic_string<wchar_t> operator() (args_t && ... args)
            {
                return std::basic_string<wchar_t>(std::forward<args_t>(args) ...);
            }
        };

    }

    // Converts string from a char type to another char type.
    template<typename from_ch_t, typename to_ch_t, typename ... args_t>
    std::basic_string<to_ch_t> string_convert(args_t && ... args)
    {
        __string_convert_call_t<from_ch_t, to_ch_t> f;
        return f(std::forward<args_t>(args) ...);
    }

    ////////// ////////// ////////// ////////// //////////
    // Picks a char type for a string. 
    // (string type defined in std library, or a simple char pointer. )

    namespace
    {
        template<typename _string_t> struct __string_char_type_t
        {
            typedef typename std::remove_reference_t<_string_t>::value_type char_t;
        };

        template<typename _c_t> struct __string_char_type_t<const _c_t *>
        {
            typedef _c_t char_t;
        };

        template<typename _c_t> struct __string_char_type_t<_c_t *>
        {
            typedef _c_t char_t;
        };
    }

    // Picks a char type for a string.
    // ( string type defined in std library, or a simple char pointer. )
    template<typename _string_t>
    using char_type_t = typename __string_char_type_t<_string_t>::char_t;

    ////////// ////////// ////////// ////////// //////////
    // The strings of the virtual machine: UTF-16 on every platform (rt_char_t is 16 bits). The strings of
    // the host are those of char_t: UTF-16 on Windows, UTF-32 on Linux.

    // Converts a string of the host to a string of the virtual machine: the characters that are not in
    // the basic plane are a pair of surrogates.
    inline ustring_t to_ustring(const char_t * s, size_t length)
    {
        ustring_t result;
        if (s == nullptr)
            return result;

        result.reserve(length);

        for (size_t k = 0; k < length; k++)
        {
            uint32_t c = (uint32_t)s[k];

            if constexpr (sizeof(char_t) > 2)
            {
                if (c >= 0x10000 && c <= 0x10FFFF)
                {
                    c -= 0x10000;
                    result.push_back((rt_char_t)(0xD800 + (c >> 10)));
                    result.push_back((rt_char_t)(0xDC00 + (c & 0x3FF)));
                    continue;
                }
            }

            result.push_back((rt_char_t)c);
        }

        return result;
    }

    inline ustring_t to_ustring(const char_t * s)
    {
        return s == nullptr? ustring_t() : to_ustring(s, std::char_traits<char_t>::length(s));
    }

    inline ustring_t to_ustring(const string_t & s)
    {
        return to_ustring(s.c_str(), s.length());
    }

    // Converts a string of the virtual machine to a string of the host: a pair of surrogates is a
    // character of the host when it is wide.
    inline string_t to_host_string(const rt_char_t * s, size_t length)
    {
        string_t result;
        if (s == nullptr)
            return result;

        result.reserve(length);

        for (size_t k = 0; k < length; k++)
        {
            uint32_t c = (uint32_t)s[k];

            if constexpr (sizeof(char_t) > 2)
            {
                if (c >= 0xD800 && c <= 0xDBFF && k + 1 < length
                        && (uint32_t)s[k + 1] >= 0xDC00 && (uint32_t)s[k + 1] <= 0xDFFF)
                {
                    c = 0x10000 + ((c - 0xD800) << 10) + ((uint32_t)s[k + 1] - 0xDC00);
                    k++;
                }
            }

            result.push_back((char_t)c);
        }

        return result;
    }

    inline string_t to_host_string(const rt_char_t * s)
    {
        return s == nullptr? string_t() : to_host_string(s, std::char_traits<rt_char_t>::length(s));
    }

    inline string_t to_host_string(const ustring_t & s)
    {
        return to_host_string(s.c_str(), s.length());
    }

    // Converts UTF-8 to a string of the virtual machine (UTF-16). A sequence that is not valid is the
    // replacement character (U+FFFD).
    inline ustring_t utf8_to_ustring(const char * s, size_t length)
    {
        ustring_t result;
        if (s == nullptr)
            return result;

        result.reserve(length);

        for (size_t k = 0; k < length; )
        {
            uint32_t c = (uint8_t)s[k];
            size_t extra = 0;

            if (c < 0x80)           { extra = 0; }
            else if (c >= 0xF0 && c < 0xF8) { c &= 0x07; extra = 3; }
            else if (c >= 0xE0)     { c &= 0x0F; extra = 2; }
            else if (c >= 0xC0)     { c &= 0x1F; extra = 1; }
            else                    { result.push_back((rt_char_t)0xFFFD); k++; continue; }

            if (extra > 0 && k + extra >= length)
            {
                result.push_back((rt_char_t)0xFFFD);
                break;
            }

            bool valid = true;
            for (size_t e = 1; e <= extra; e++)
            {
                uint32_t cc = (uint8_t)s[k + e];
                if ((cc & 0xC0) != 0x80)
                {
                    valid = false;
                    break;
                }

                c = (c << 6) | (cc & 0x3F);
            }

            if (!valid || c > 0x10FFFF)
            {
                result.push_back((rt_char_t)0xFFFD);
                k++;
                continue;
            }

            k += extra + 1;

            if (c >= 0x10000)
            {
                c -= 0x10000;
                result.push_back((rt_char_t)(0xD800 + (c >> 10)));
                result.push_back((rt_char_t)(0xDC00 + (c & 0x3FF)));
            }
            else
            {
                result.push_back((rt_char_t)c);
            }
        }

        return result;
    }

    inline ustring_t utf8_to_ustring(const char * s)
    {
        return s == nullptr? ustring_t() : utf8_to_ustring(s, std::char_traits<char>::length(s));
    }

    // Converts a string of the virtual machine (UTF-16) to UTF-8. A surrogate that has no pair is the
    // replacement character.
    inline std::string ustring_to_utf8(const rt_char_t * s, size_t length)
    {
        std::string result;
        if (s == nullptr)
            return result;

        result.reserve(length);

        for (size_t k = 0; k < length; k++)
        {
            uint32_t c = (uint32_t)s[k];

            if (c >= 0xD800 && c <= 0xDBFF && k + 1 < length
                    && (uint32_t)s[k + 1] >= 0xDC00 && (uint32_t)s[k + 1] <= 0xDFFF)
            {
                c = 0x10000 + ((c - 0xD800) << 10) + ((uint32_t)s[k + 1] - 0xDC00);
                k++;
            }
            else if (c >= 0xD800 && c <= 0xDFFF)
            {
                c = 0xFFFD;
            }

            if (c < 0x80)
            {
                result.push_back((char)c);
            }
            else if (c < 0x800)
            {
                result.push_back((char)(0xC0 | (c >> 6)));
                result.push_back((char)(0x80 | (c & 0x3F)));
            }
            else if (c < 0x10000)
            {
                result.push_back((char)(0xE0 | (c >> 12)));
                result.push_back((char)(0x80 | ((c >> 6) & 0x3F)));
                result.push_back((char)(0x80 | (c & 0x3F)));
            }
            else
            {
                result.push_back((char)(0xF0 | (c >> 18)));
                result.push_back((char)(0x80 | ((c >> 12) & 0x3F)));
                result.push_back((char)(0x80 | ((c >> 6) & 0x3F)));
                result.push_back((char)(0x80 | (c & 0x3F)));
            }
        }

        return result;
    }

    inline std::string ustring_to_utf8(const ustring_t & s)
    {
        return ustring_to_utf8(s.c_str(), s.length());
    }

    ////////// ////////// ////////// ////////// //////////
    // UTF-8 for the wide streams. std::codecvt_utf8<wchar_t> is UCS-2 when wchar_t is 16 bits (Windows):
    // a character that is not in the basic plane (a pair of surrogates) stops the stream. This facet
    // converts the pairs.

    class utf8_wchar_codecvt_t : public std::codecvt<wchar_t, char, std::mbstate_t>
    {
    public:
        typedef std::codecvt<wchar_t, char, std::mbstate_t> __super_t;

        explicit utf8_wchar_codecvt_t(size_t refs = 0) : __super_t(refs) { }

    private:
        // Writes a code point as UTF-8, returns false when there is no room.
        static bool __put(uint32_t c, char *& to, char * to_end)
        {
            size_t need = c < 0x80? 1 : c < 0x800? 2 : c < 0x10000? 3 : 4;
            if ((size_t)(to_end - to) < need)
                return false;

            if (need == 1)
            {
                *to++ = (char)c;
            }
            else if (need == 2)
            {
                *to++ = (char)(0xC0 | (c >> 6));
                *to++ = (char)(0x80 | (c & 0x3F));
            }
            else if (need == 3)
            {
                *to++ = (char)(0xE0 | (c >> 12));
                *to++ = (char)(0x80 | ((c >> 6) & 0x3F));
                *to++ = (char)(0x80 | (c & 0x3F));
            }
            else
            {
                *to++ = (char)(0xF0 | (c >> 18));
                *to++ = (char)(0x80 | ((c >> 12) & 0x3F));
                *to++ = (char)(0x80 | ((c >> 6) & 0x3F));
                *to++ = (char)(0x80 | (c & 0x3F));
            }

            return true;
        }

    protected:
        virtual bool do_always_noconv() const noexcept override { return false; }
        virtual int do_max_length() const noexcept override { return 4; }
        virtual int do_encoding() const noexcept override { return 0; }

        // wchar_t (UTF-16 units) to UTF-8. A high surrogate waits for its low one in the state (the streams
        // of stdio convert one character at a time).
        virtual result do_out(state_type & state, const wchar_t * from, const wchar_t * from_end,
                const wchar_t *& from_next, char * to, char * to_end, char *& to_next) const override
        {
            static_assert(sizeof(state_type) >= sizeof(uint32_t), "the state keeps a surrogate");
            uint32_t & pending = *(uint32_t *)&state;

            from_next = from;
            to_next = to;

            while (from_next < from_end)
            {
                uint32_t c = (uint32_t)(uint16_t)*from_next;
                uint32_t out = c;

                if (pending != 0)
                {
                    if (c >= 0xDC00 && c <= 0xDFFF)
                        out = 0x10000 + ((pending - 0xD800) << 10) + (c - 0xDC00);
                    else
                        out = 0xFFFD;       // The high surrogate had no low one: c is written next.

                    if (!__put(out, to_next, to_end))
                        return partial;

                    bool paired = out != 0xFFFD || (c >= 0xDC00 && c <= 0xDFFF);
                    pending = 0;

                    if (paired)
                        from_next++;

                    continue;
                }

                if (c >= 0xD800 && c <= 0xDBFF)
                {
                    pending = c;
                    from_next++;
                    continue;
                }

                if (c >= 0xDC00 && c <= 0xDFFF)
                    out = 0xFFFD;

                if (!__put(out, to_next, to_end))
                    return partial;

                from_next++;
            }

            return ok;
        }

        // UTF-8 to wchar_t (UTF-16 units). A sequence that is cut at the end is not consumed (partial).
        virtual result do_in(state_type &, const char * from, const char * from_end,
                const char *& from_next, wchar_t * to, wchar_t * to_end, wchar_t *& to_next) const override
        {
            from_next = from;
            to_next = to;

            while (from_next < from_end)
            {
                uint32_t c = (uint8_t)*from_next;
                size_t extra = 0;

                if (c < 0x80)                   { extra = 0; }
                else if (c >= 0xF0 && c < 0xF8) { c &= 0x07; extra = 3; }
                else if (c >= 0xE0 && c < 0xF0) { c &= 0x0F; extra = 2; }
                else if (c >= 0xC0 && c < 0xE0) { c &= 0x1F; extra = 1; }
                else                            { c = 0xFFFD; }

                if (extra > 0 && (size_t)(from_end - from_next) <= extra)
                    return partial;

                bool valid = true;
                for (size_t e = 1; e <= extra; e++)
                {
                    uint32_t cc = (uint8_t)from_next[e];
                    if ((cc & 0xC0) != 0x80)
                    {
                        valid = false;
                        break;
                    }

                    c = (c << 6) | (cc & 0x3F);
                }

                size_t length = extra + 1;
                if (!valid || c > 0x10FFFF)
                {
                    c = 0xFFFD;
                    length = 1;
                }

                size_t units = c >= 0x10000? 2 : 1;
                if ((size_t)(to_end - to_next) < units)
                    return partial;

                if (units == 2)
                {
                    c -= 0x10000;
                    *to_next++ = (wchar_t)(0xD800 + (c >> 10));
                    *to_next++ = (wchar_t)(0xDC00 + (c & 0x3FF));
                }
                else
                {
                    *to_next++ = (wchar_t)c;
                }

                from_next += length;
            }

            return ok;
        }

        // A high surrogate that waits for its low one at the end is the replacement character.
        virtual result do_unshift(state_type & state, char * to, char * to_end, char *& to_next) const override
        {
            uint32_t & pending = *(uint32_t *)&state;
            to_next = to;

            if (pending != 0)
            {
                if (!__put(0xFFFD, to_next, to_end))
                    return partial;

                pending = 0;
            }

            return ok;
        }

        virtual int do_length(state_type & state, const char * from, const char * end, std::size_t max) const override
        {
            const char * from_next = from;
            wchar_t buffer[2];
            std::size_t count = 0;

            while (from_next < end && count < max)
            {
                wchar_t * to_next = buffer;
                const char * next = from_next;

                if (do_in(state, from_next, end, next, buffer, buffer + 2, to_next) == partial
                        && next == from_next)
                    break;

                if (next == from_next)
                    break;

                from_next = next;
                count += (size_t)(to_next - buffer);
            }

            return (int)(from_next - from);
        }
    };

    // The locale of the wide streams that write and read UTF-8, whatever the platform is.
    inline std::locale utf8_locale()
    {
        if constexpr (sizeof(wchar_t) > 2)
            return std::locale(std::locale(), new std::codecvt_utf8<wchar_t>());
        else
            return std::locale(std::locale(), new utf8_wchar_codecvt_t());
    }
}

#endif  // __COMMON_STRING_H__
