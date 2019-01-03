
#ifndef __COMMON_STRING_H__
#define __COMMON_STRING_H__

////////// ////////// ////////// ////////// //////////
// convert functions

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

        template<> struct __string_info_t<__u8string_size>
        {
            typedef std::string string_type;
            typedef typename string_type::value_type char_type;
        };

        template<> struct __string_info_t<__u16string_size>
        {
            typedef std::u16string string_type;
            typedef typename string_type::value_type char_type;
        };

        template<> struct __string_info_t<__u32string_size>
        {
            typedef std::u32string string_type;
            typedef typename string_type::value_type char_type;
        };

        typedef typename __string_info_t<sizeof(wchar_t)>::char_type __wchar_t;

        ////////// ////////// ////////// ////////// //////////

        const std::codecvt_mode __codecvt_mode = (std::codecvt_mode)std::little_endian;
        const unsigned long __max_code = 0x10ffff;

        template<typename c_t>
        using __codecvt_utf8 = std::codecvt_utf8<c_t, __max_code, __codecvt_mode>;

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
        // __string_convert_base_t

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
                for(const from_ch_t * p = bytes; ; p++)
                {
                    if(!*p)
                        return p - bytes;
                }
            }
        };

        ////////// ////////// ////////// ////////// //////////
        // __string_convert_t

        template<typename from_ch_t, typename to_ch_t>
        class __string_convert_t { };

        //-------- ---------- ---------- ---------- ----------

        // from/to char
        template<typename from_ch_t>
        class __string_convert_t<from_ch_t, char>
            : public __string_convert_base_t<from_ch_t, char, \
                    __string_convert_to_bytes_t<from_ch_t, char, __codecvt_utf8<from_ch_t>>>
        { };

        //-------- ---------- ---------- ---------- ----------

        template<typename to_ch_t>
        class __string_convert_t<char, to_ch_t>
            : public __string_convert_base_t<char, to_ch_t, \
                    __string_convert_from_bytes_t<char, to_ch_t, __codecvt_utf8<to_ch_t>>>
        { };

        //-------- ---------- ---------- ---------- ----------

        template<>
        class __string_convert_t<char, char>
            : public __string_convert_base_t<char, char, __string_convert_empty_function_t<char>>
        { };

        //-------- ---------- ---------- ---------- ----------

        // from/to char16_t
        template<>
        class __string_convert_t<char32_t, char16_t>
            : public __string_convert_base_t<char32_t, char16_t, \
                    __string_convert_to_bytes_t<char32_t, char16_t, __codecvt_utf16<char32_t>>>
        { };

        //-------- ---------- ---------- ---------- ----------

        template<>
        class __string_convert_t<char16_t, char32_t>
            : public __string_convert_base_ex_t<char16_t, char32_t, \
                    __string_convert_from_bytes_t<char16_t, char32_t, __codecvt_utf16<char32_t>>>
        { };

        //-------- ---------- ---------- ---------- ----------

        template<>
        class __string_convert_t<char16_t, wchar_t>
            : public __string_convert_base_ex_t<char16_t, wchar_t, \
                    __string_convert_from_bytes_t<char16_t, wchar_t, __codecvt_utf16<wchar_t>>>
        { };

        //-------- ---------- ---------- ---------- ----------

        template<>
        class __string_convert_t<char16_t, char16_t>
            : public __string_convert_base_t<char16_t, char16_t,
                    __string_convert_empty_function_t<char16_t>>
        { };

        //-------- ---------- ---------- ---------- ----------

        template<>
        class __string_convert_t<char32_t, char32_t>
            : public __string_convert_base_t<char32_t, char32_t,
                    __string_convert_empty_function_t<char32_t>>
        { };

        //-------- ---------- ---------- ---------- ----------

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

    template<typename from_ch_t, typename to_ch_t, typename ... args_t>
    std::basic_string<to_ch_t> string_convert(args_t && ... args)
    {
        __string_convert_call_t<from_ch_t, to_ch_t> f;
        return f(std::forward<args_t>(args) ...);
    }


    ////////// ////////// ////////// ////////// //////////

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

    template<typename _string_t>
    using char_type_t = typename __string_char_type_t<_string_t>::char_t;
}

#endif  // __COMMON_STRING_H__
