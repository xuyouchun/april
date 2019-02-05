
/*

    A numeric parser to parse char array to a numberic.

    Supports:

        123                 // Integer
        123.456             // Double
        0123                // Octonary
        0xF0                // Hexadecimal
        123e+10             // Scientific notation

        010101B             // Binary
        10F0H               // Hexadecimal
        123L                // Long integer
        123.456F            // Float
        123.456L            // Long double
        123U                // unsigned int
        123UL               // unsigned long

        123'456'789         // Numeric separator
*/

#include <algorithm.h>
#include <cmath>

namespace X_ROOT_NS { namespace algorithm {

    #define __AlwaysInline  X_ALWAYS_INLINE

    ////////// ////////// ////////// ////////// //////////

    // Numberic parseing error codes.
    X_ENUM_INFO(parse_numeric_error_code_t)

        // Success
        X_C(success,            _T("success"))

        // Format error
        X_C(format_error,       _T("format error"))

        // Overflow
        X_C(overflow,           _T("overflow"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    typedef parse_numeric_error_code_t __e_t;
    typedef cptr_t<char_t> __cptr_t;

    namespace
    {
        // Base class for parsing operations for various types.
        template<typename _numeric_t, bool _continued_parse_supported = true>
        struct __parser_op_base_t
        {
            __parser_op_base_t(int radix, bool positive) { }

            // Whether parsing operation can continue to larger numberic.
            // e.g. int16 -> int32 -> int64.
            static const bool continued_parse_supported = _continued_parse_supported;
        };

        //-------- ---------- ---------- ---------- ----------

        // Parsing operations for various types.
        template<typename _numeric_t, bool _is_signed, bool _is_floating>
        struct __parser_op_t : public __parser_op_base_t<_numeric_t>
        {
            using __parser_op_base_t<_numeric_t>::__parser_op_base_t;
        };

        //-------- ---------- ---------- ---------- ----------

        // Parsing operation for interger types.    ( int16, int32, int64 )
        template<typename _numeric_t, bool _is_signed>
        struct __parser_op_t<_numeric_t, _is_signed, false>
            : public __parser_op_base_t<_numeric_t, true>
        {
            typedef typename std::make_unsigned<_numeric_t>::type unumeric_t;

            // Constructor
            __parser_op_t(int radix, bool positive, _numeric_t & numeric)
                : __parser_op_base_t<_numeric_t, true>(radix, positive)
                , __radix(radix), __positive(positive)
                , __numeric(numeric), __max(__max_value(positive))
                , __prelimit(__max / radix), __prelimit_mod(__max % radix) { }

            // Returns the max value of specified type.
            __AlwaysInline static unumeric_t __max_value(bool positive)
            {
                if(_is_signed)
                {
                    if(positive)
                        return (unumeric_t)max_value<_numeric_t>();
                    else
                        return (unumeric_t)max_value<_numeric_t>() + 1;
                }
                else
                {
                    return max_value<unumeric_t>();
                }
            }

            // Pushes a char to continue parsing operation.
            __AlwaysInline __e_t push(char_t c)
            {
                int n = digit_value(c);
                if(__numeric > __prelimit || (__numeric == __prelimit && n > __prelimit_mod))
                    return __e_t::overflow;

                __numeric = __numeric * __radix + n;
                return __e_t::success;
            }

            // Commits to completed the paring operation.
            __AlwaysInline __e_t commit()
            {
                return __e_t::success;
            }

        private:
            const unumeric_t __max, __prelimit, __prelimit_mod;
            const int __radix;
            const bool __positive;
            _numeric_t & __numeric;
        };

        //-------- ---------- ---------- ---------- ----------

        // Parsing operation for float types.    ( float, double, ldouble )
        template<typename _numeric_t, bool _is_signed>
        struct __parser_op_t<_numeric_t, _is_signed, true>
            : public __parser_op_base_t<_numeric_t, false>
        {
            __parser_op_t(int radix, bool positive, _numeric_t & numeric)
                : __parser_op_base_t<_numeric_t, false>(radix, positive)
                , __numeric(numeric)
            {
                _A(radix == 10);
            }

            // Pushes a char to continue parse operation.
            __AlwaysInline __e_t push(char_t c)
            {
                switch(c)
                {
                    case _T('.'):
                        __point = true;
                        if(__integer_zero)
                            __point_zero_count++;
                        return __e_t::success;

                    case _T('E'): case _T('e'):
                        __e = true;
                        return __e_t::success;

                    case _T('+'): case _T('-'):
                        __ec = c;
                        return __e_t::success;
                }

                int n = digit_value(c);

                if(!__e)
                {
                    if(!__point)
                    {
                        __numeric = __numeric * 10 + n;
                        if(n > 0)
                            __integer_zero = false;
                    }
                    else
                    {
                        __numeric = __numeric + n * __point_mod;
                        __point_mod /= 10;

                        if(!__point_zero_end && n == 0 && __integer_zero)
                            __point_zero_count++;
                        else
                            __point_zero_end = true;
                    }

                    if(std::isinf(__numeric))
                        return __e_t::overflow;
                }
                else
                {
                    if(__pow > __pow_limit)
                        return __e_t::overflow;

                    __pow = __pow * 10 + n;
                }

                return __e_t::success;
            }

            // Commits to completed the paring operation.
            __AlwaysInline __e_t commit()
            {
                if(__pow != 0)
                {
                    if(__point_zero_count > 0)
                    {
                        __numeric *= std::pow((_numeric_t)10.0, __point_zero_count);
                        __pow -= __point_zero_count;
                    }

                    _numeric_t ev = std::pow((_numeric_t)10.0, __pow);
                    if(std::isinf(ev))
                        return __e_t::overflow;

                    if(__ec == _T('+'))
                        __numeric *= ev;
                    else
                        __numeric /= ev;

                    if(std::isinf(__numeric))
                        return __e_t::overflow;
                }

                return __e_t::success;
            }

        private:
            _numeric_t & __numeric;
            bool         __point = false;
            _numeric_t   __point_mod = (_numeric_t)0.1;
            bool         __e = false;
            char_t       __ec = _T('\0');
            int32_t      __pow = 0;
            int32_t      __point_zero_count = 0;
            bool         __point_zero_end = false;
            bool         __integer_zero = true;

            static const int32_t __pow_limit = max_value<int32_t>() / 100;
        };

        //-------- ---------- ---------- ---------- ----------

        // Numeric parser
        template<typename _numeric_t>
        struct __parser_t
        {
            static const bool is_signed = std::is_signed<_numeric_t>::value;
            static const bool is_floating = std::is_floating_point<_numeric_t>::value;
            typedef __parser_op_t<_numeric_t, is_signed, is_floating> __op_t;
            static const bool continued_parse_supported = __op_t::continued_parse_supported;

        public:
            __parser_t(int radix, bool positive) : __radix(radix), __positive(positive) { }

            // Parses specified char array to numeric.
            __AlwaysInline __e_t parse(const char_t * & p, const char_t * p_end,
                                                           _numeric_t & numeric)
            {
                __op_t op(__radix, __positive, numeric);
                for(; p < p_end; p++)
                {
                    if(*p == _T('\''))
                        continue;

                    __e_t r = op.push(*p);
                    if(r != __e_t::success)
                        return r;
                }

                return op.commit();
            }

        private:
            const int __radix;
            const bool __positive;
        };

        //-------- ---------- ---------- ---------- ----------

        // Multipy numeric parser
        // Be used for continue supported parse operation, like int16->int32->int64.
        struct __multi_parser_t
        {
            __multi_parser_t(int radix, bool positive, const char_t * &p, const char_t * p_end)
                : __radix(radix), __positive(positive), __p(p), __p0(p), __p_end(p_end) { }

            // Parse numerics for each type.
            template<typename ... types_t>
            __AlwaysInline tvalue_t parse()
            {
                typedef X_TYPEAT(types_t, 0) first_t;
                __value = def_value<first_t>();

                each_types<types_t ...>(*this);
                if(__error_code != __e_t::success)
                    throw _E(__error_code);
                return __value;
            }

            // Calls the parsing method.
            template<typename _numeric_t> __AlwaysInline bool operator() ()
            {
                __error_code = __try_parse<_numeric_t>();
                return __error_code == __e_t::overflow;
            }

        private:
            const int  __radix;
            const bool __positive;
            tvalue_t __value;
            __e_t __error_code = __e_t::success;
            const char_t * &__p, * __p0;
            const char_t * __p_end;

            // Try to parse, Raise exception when parsing error.
            template<typename _numeric_t>
            __AlwaysInline __e_t __try_parse()
            {
                __parser_t<_numeric_t> parser(__radix, __positive);
                _numeric_t numeric;

                if(parser.continued_parse_supported)
                {
                    numeric = __value.get_value<_numeric_t>();
                }
                else
                {
                    numeric = def_value<_numeric_t>();
                    __p = __p0;
                }

                __e_t ret = parser.parse(__p, __p_end, numeric);
                __value = numeric;

                return ret;
            }
        };

        // Parse numeric with radix, positive.
        template<typename ... _types_t>
        tvalue_t __parse_numeric(int radix, bool positive,
                             const char_t * & p_start, const char_t * p_end)
        {
            return __multi_parser_t(radix, positive, p_start, p_end).parse<_types_t ...>();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Numeric parser.
    class __numeric_parser_t
    {
        // Enums to record properies of the numeric.

        // Signed / Unsigned
        enum __ts_t { ts_unknown, ts_signed, ts_unsigned };

        // Positive / Negative
        enum __tp_t { tp_unknown, tp_positive, tp_negative };

        // Long ( long int or long double ) / Normal
        enum __tl_t { tl_unknown, tl_default, tl_long };

        // Int / Float / Double.
        enum __tf_t { tf_unknown, tf_int, tf_float, tf_double };

    public:
        __numeric_parser_t(__cptr_t &p) : __p(p) { }

        // The paring operation.
        __AlwaysInline tvalue_t parse()
        {
            const char_t * p_start = __read_prefix();
            const char_t * p_end = __continue_read();

            if(__error == __e_t::success)
            {
                tvalue_t value = __parse(p_start, p_end);
                return __revise(value);
            }

            throw _E(__error);
        }

    private:
        __ts_t __ts = ts_unknown;
        __tp_t __tp = tp_unknown;
        __tl_t __tl = tl_unknown;
        __tf_t __tf = tf_unknown;

        int             __radix = 0;
        const char_t *  __e = nullptr;
        const char_t *  __point = nullptr;
        __cptr_t &      __p;

        __e_t __error = __e_t::success;

        // Reads the prefix of numberic. ( -, +, 0, 0x etc. )
        __AlwaysInline const char_t * __read_prefix()
        {
            const char_t * p_start = __do_read_prefix();

            if(__radix == 0)
                __radix = 10;

            if(__tp == tp_unknown)
                __tp = tp_positive;

            return p_start;
        }

        // Reads the prefix of numberic. ( -, +, 0, 0x etc. )
        __AlwaysInline const char_t * __do_read_prefix()
        {
            for(char_t c = *__p; ; c = *(++__p))
            {
                switch(c)
                {
                    case _T('-'):
                        __set_positive(tp_negative);
                        break;

                    case _T('+'):
                        __set_positive(tp_positive);
                        break;

                    case _T('0'):
                        switch(*(__p + 1))
                        {
                            case _T('X'):   case _T('x'):
                                __set_radix(16);
                                __p += 2;
                                return __p;

                            case _T('B'):   case _T('b'):
                                __set_radix(2);
                                __p += 2;
                                return __p;

                            case _T('.'):
                                return __p;

                            case _T('0'):
                                break;

                            default:
                                __set_radix(8);
                                return ++__p;
                        }
                        break;

                    case _T('\0'):
                        __set_error(__e_t::format_error);
                        return __p;

                    default:
                        return __p;
                }
            }
        }

        // Continues to parse after __read_prefix.
        __AlwaysInline const char_t * __continue_read()
        {
            const char_t * p_end = __do_continue_read();

            if(__ts == ts_unknown)
                __ts = ts_signed;

            if(__tl == tl_unknown)
                __tl = tl_default;

            if(__tf == tf_unknown && (__point || __e))
                __tf = tf_double;

            if((__tf == tf_double || __tf == tf_float) && __radix == 8)
                __radix = 10;

            return p_end;
        }

        // Continues to parse after __read_prefix.
        __AlwaysInline const char_t * __do_continue_read()
        {
            const char_t * p_end = nullptr;

            for(char_t c = *__p; ; c = *(++__p))
            {
                switch(c)
                {
                    case _T('.'):
                        __set_point(__p);
                        break;

                    case _T('E'): case _T('e'):
                        if(__radix == 10)
                        {
                            __set_e(__p);
                            if(*++__p != _T('+') && *__p != _T('-'))
                            {
                                __set_error(__e_t::format_error);
                                goto label_default;
                            }

                            if(!is_digit(*++__p))
                            {
                                __set_error(__e_t::format_error);
                                goto label_default;
                            }
                        }
                        else
                        {
                            goto label_default;
                        }
                        break;

                    case _T('F'): case _T('f'):
                        if(__radix == 10)
                        {
                            __set_float(tf_float);
                            if(!p_end)
                                p_end = __p;
                        }
                        else
                        {
                            goto label_default;
                        }
                        break;

                    case _T('L'): case _T('l'):
                        __set_long(tl_long);
                        if(!p_end)
                            p_end = __p;
                        break;

                    case _T('U'): case _T('u'):
                        __set_sign(ts_unsigned);
                        if(!p_end)
                            p_end = __p;
                        break;

                    case _T('\''):
                        if(!al::is_digit(*(__p - 1)))
                            __set_error(__e_t::format_error);
                        break;

                    default:
                    label_default:
                        if(!is_word(c))
                        {
                            if(!p_end)
                                p_end = __p;

                            if(*(p_end - 1) == _T('\''))
                                __set_error(__e_t::format_error);

                            return p_end;
                        }

                        if(p_end)
                            __set_error(__e_t::format_error);

                        if(!is_digit(c, __radix))
                            __set_error(__e_t::format_error);

                        break;
                }
            }
        }

        // Parses with specified char array.
        __AlwaysInline tvalue_t __parse(const char_t * p, const char_t * p_end)
        {
            if(__tf == tf_double)  // double
            {
                if(__radix != 10)
                    throw _E(__e_t::format_error);

                if(__ts == ts_unsigned)
                    throw _E(__e_t::format_error);

                if(__tl == tl_long)  // long double
                    return __do_parse<ldouble_t>(p, p_end);
                else
                    return __do_parse<double_t, ldouble_t>(p, p_end);
            }
            else if(__tf == tf_float)  // float
            {
                if(__radix != 10)
                    throw _E(__e_t::format_error);

                if(__ts == ts_unsigned)
                    throw _E(__e_t::format_error);

                if(__tl == tl_long)
                    throw _E(__e_t::format_error);

                return __do_parse<float_t>(p, p_end);
            }
            else  // integer
            {
                if(__tl == tl_long)
                {
                    if(__ts == ts_unsigned)
                        return __do_parse<uint64_t>(p, p_end);
                    else
                        return __do_parse<int64_t, uint64_t>(p, p_end);
                }
                else
                {
                    if(__ts == ts_unsigned)
                    {
                        return __do_parse<uint32_t, uint64_t>(p, p_end);
                    }
                    else
                    {
                        if(__radix == 10)
                            return __do_parse<int32_t, int64_t, uint64_t>(p, p_end);
                        else
                            return __do_parse<int32_t, uint32_t, int64_t, uint64_t>(p, p_end);
                    }
                }
            }
        }

        // Revises value, return negative value if negative.
        __AlwaysInline tvalue_t __revise(tvalue_t value)
        {
            if(__tp == tp_negative)
                value = -value;

            return value;
        }

        // Parses with specified char array.
        template<typename ... types_t>
        __AlwaysInline tvalue_t __do_parse(const char_t * p, const char_t * p_end)
        {
            return __parse_numeric<types_t ...>(__radix, __tp != tp_negative, p, p_end);
        }

        // Sets radix: 2, 8, 10, 16.
        __AlwaysInline void __set_radix(int radix)
        {
            if(__radix > 0)
                __set_error(__e_t::format_error);
            else
                __radix = radix;
        }

        // Sets point position.
        __AlwaysInline void __set_point(const char_t * point)
        {
            if(__point)
                __set_error(__e_t::format_error);
            else
                __point = point;
        }

        // Sets sign.
        __AlwaysInline void __set_sign(__ts_t ts)
        {
            if(__ts != ts_unknown || __tf != tf_unknown)
                __set_error(__e_t::format_error);
            else
                __ts = ts;
        }

        // Sets positive.
        __AlwaysInline void __set_positive(__tp_t tp)
        {
            if(__tp != tp_unknown)
                __set_error(__e_t::format_error);
            else
                __tp = tp;
        }

        // Sets long type.
        __AlwaysInline void __set_long(__tl_t tl)
        {
            if(__tl != tl_unknown)
                __set_error(__e_t::format_error);
            else
                __tl = tl;
        }

        // Sets float type.
        __AlwaysInline void __set_float(__tf_t tf)
        {
            if(__tf != tf_unknown || __ts != ts_unknown)
                __set_error(__e_t::format_error);
            else
                __tf = tf;
        }

        // Sets 'E' / 'e' position.
        __AlwaysInline void __set_e(const char_t * e)
        {
            if(__e)
                __set_error(__e_t::format_error);
            else
                __e = e;
        }

        // Sets error.
        __AlwaysInline void __set_error(__e_t error)
        {
            if(__error == __e_t::success)
                __error = error;
        }
    };

    // Parses the give char array to numeric.
    // Raise exception when parse fault.
    tvalue_t parse_numeric(__cptr_t &p)
    {
        _A(p != nullptr);
        _A(p[0] != _T('\0'));

        return __numeric_parser_t(p).parse();
    }

    // Parses the give char array to numeric.
    // Returns the error code when parse fault.
    __e_t try_parse_numeric(__cptr_t &p, tvalue_t * out_value)
    {
        tvalue_t value;
        try
        {
            value = parse_numeric(p);
            if(out_value)
                *out_value = value;

            return __e_t::success;
        }
        catch(logic_error_t<__e_t> & e)
        {
            if(out_value)
                *out_value = tvalue_t(value_type_t::__default__);

            return e.code;
        }
    }

    ////////// ////////// ////////// ////////// //////////

    #undef __AlwaysInline

} }  // X_ROOT_NS::algorithm
