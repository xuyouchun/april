
/*
    XuYouChun

    The common header file.  Defines common macros, basic data structures and functions.
*/


#ifndef __COMMON_H__
#define __COMMON_H__


// Std libraries

#include <exception>
#include <new>
#include <initializer_list>
#include <iostream>
#include <fstream>
#include <iterator>
#include <valarray>
#include <vector>
#include <list>
#include <string>
#include <sstream>
#include <map>
#include <unordered_map>
#include <stack>
#include <set>
#include <queue>
#include <limits>
#include <codecvt>
#include <type_traits>
#include <thread>
#include <chrono>
#include <functional>
#include <regex>

// Boost Library
#include <boost/filesystem.hpp>

#include <global.h>
#include <__types.h>

////////// ////////// ////////// ////////// //////////

// Definations

// Defines optimize options.

#ifdef CONFIG_OPTIMIZE
    #define X_OPTIMIZE CONFIG_OPTIMIZE
#else
    #define X_OPTIMIZE 0
#endif

#if X_OPTIMIZE == 0
    #define X_DEBUG     1
#else
    #define X_DEBUG     0
#endif

// Defines an inline function.
#ifdef __INLINE
    #define X_INLINE        __INLINE
#else
    #define X_INLINE        inline
#endif

// Defines a noinline function. (if the c++ language version supported).
#ifdef __NO_INLINE
    #define X_NO_INLINE     __NO_INLINE
#else
    #define X_NO_INLINE
#endif


// Defines an function which always inlined.

#if defined __ALWAYS_INLINE && !X_DEBUG
    #define X_ALWAYS_INLINE         __ALWAYS_INLINE X_INLINE
    #define X_ALWAYS_INLINE_METHOD  __ALWAYS_INLINE
#else
    #define X_ALWAYS_INLINE         X_INLINE
    #define X_ALWAYS_INLINE_METHOD  // Empty
#endif

////////// ////////// ////////// ////////// //////////

namespace __root_ns = ::X_ROOT_NS;

// Defines a constant unicode string.
#define _T(s)               X_UNICODE_STR(s)

// Convert a macro value to a string.
#define _S(s)               X_CONVERT_TO_STR(s)

// Format a string with specified arguments.
#define _F(format, args...) __root_ns::sprintf(format, ##args)

// Defines an exception by specified error code, the exception type depended on the error code.
#define _E(code, args...)   X_ERROR(code, ##args)

// Defines a common exception with specified common error code .
#define _EC(code, args...)  X_ERROR(__root_ns::common_error_code_t::code, ##args)

// Defines an exception with specified error code and formated string.
#define _EF(code, format, args...)  X_ERROR(code, __root_ns::sprintf(format, ##args))

// Defines a common exception with specified common error code and formated string.
#define _ECF(code, format, args...) X_ERROR(__root_ns::common_error_code_t::code, \
                                                __root_ns::sprintf(format, ##args))

// Defines an exception with specified error code, its error message depeneded on the description
//    of the error code, which defined by macro X_D.
#define _ED(code, args...)  X_ERROR(code, __root_ns::sprintf(_desc(code), ##args))

// An expression assertion.
#define _A(x, args...)      X_ASSERT(x, ##args)

// Print expressions, multiple expressions splited by spaces.
#define _P                  __root_ns::println

// Print an expression, with the expression as prefix.
#define _PP(s)              _P(_T("") #s ":", s)

// Print an formated string, the first argument is a format string.
#define _PF(s, args...)     _P(sprintf(_T("") s, ##args))

// Default value for the specified type.
#define X_DEFAULT(type)     (__root_ns::def_value<type>())

// Simplification of X_DEFAULT
#define _D(type)            X_DEFAULT(type)

// Assert a expression is an specified type, throw expression if fault to convert.
#define _M(type_t, value)   X_MUST(type_t, value)

// C++ keyword: noexpect
#if X_DEBUG
#   define X_NOEXPECT
#else
#   define X_NOEXPECT noexcept
#endif

#define _NE X_NOEXPECT

// Throw an common unexpected exception.
#define X_UNEXPECTED()      throw _EC(unexpected)

// Throw an common unimplemented exception.
#define X_UNIMPLEMENTED()   throw _EC(unimplemented)

// Defines an interface.
#define X_INTERFACE         struct

// Defines a temporary variable name, composited with line number.
#define X_TEMP_VAR          __temp_var_##__LINE__##__

// Gets a static array elements' count.
#define X_ARRAY_SIZE(arr)   (sizeof(arr) / sizeof((arr)[0]))

// Macros for try ... finally grammar in c++.
#define X_TRY       __root_ns::try_finally([&]() {
#define X_TRY_R     return __root_ns::try_finally_r([&]() {
#define X_FINALLY   }, [&]() {
#define X_TRY_END   });

////////// ////////// ////////// ////////// //////////

namespace X_ROOT_NS {

    namespace __root_ns__ = ::X_ROOT_NS;
    using namespace X_ROOT_NS::arch::types;

    // Types
    typedef int              int_t;
    typedef unsigned int     uint_t;
    typedef short            short_t;
    typedef unsigned short   ushort_t;
    typedef long             long_t;
    typedef unsigned long    ulong_t;
    typedef float            float_t;
    typedef double           double_t;
    typedef long double      ldouble_t;
    typedef bool             bool_t;
    typedef uint8_t          byte_t;

    typedef ::std::basic_string<byte_t>    bytes_t;

    //-------- ---------- ---------- ---------- ----------
    // Time, from std library.

    template<typename rep_t, typename period_t=::std::ratio<1>>
    using duration_t = ::std::chrono::duration<rep_t, period_t>;

    template <class _clock_t, class duration_t>
    using time_point_t = ::std::chrono::time_point<_clock_t, duration_t>;

    typedef ::std::chrono::nanoseconds      nanoseconds_t;
    typedef ::std::chrono::microseconds     microseconds_t;
    typedef ::std::chrono::milliseconds     milliseconds_t;
    typedef ::std::chrono::seconds          seconds_t;
    typedef ::std::chrono::minutes          minutes_t;
    typedef ::std::chrono::hours            hours_t;

    //-------- ---------- ---------- ---------- ----------

    // A static empty string.
    const string_t empty_str = _T("");
    template<typename t> constexpr t * null = (t *)nullptr;

    // Default values
    namespace
    {
        template<typename t>
        using __rr_t = typename std::remove_const<typename std::remove_reference<t>::type>::type;

        // - - - - - - - - - - - - - - - - - - - - - - - - - -

        template<typename t, bool is_pointer, bool is_numeric>
        struct __def_t { constexpr static const __rr_t<t> value = __rr_t<t>(); };

        template<typename t, bool is_pointer, bool is_numeric>
        const __rr_t<t> __def_t<t, is_pointer, is_numeric>::value;

        // - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Pointers

        template<typename t, bool is_numeric>
        struct __def_t<t, true, is_numeric> { constexpr static const __rr_t<t> value = nullptr; };

        template<typename t, bool is_numeric> const __rr_t<t> __def_t<t, true, is_numeric>::value;

        // - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Numerics

        template<typename t>
        struct __def_t<t, false, true> { constexpr static const __rr_t<t> value = (__rr_t<t>)0; };

        template<typename t> const __rr_t<t> __def_t<t, false, true>::value;
    }

    // Gets default value of specified type.
    template<typename t> constexpr __rr_t<t> def_value()
    {
        typedef __rr_t<t> rr_t;
        return __def_t<t, std::is_pointer<rr_t>::value,
                   std::is_integral<rr_t>::value || std::is_floating_point<rr_t>::value
               >::value;
    }

    // Determined whether a value is default value.
    template<typename t> constexpr bool is_default(const t & value)
    {
        return value == def_value<t>();
    }

    //-------- ---------- ---------- ---------- ----------
    // Smart pointer references from std library.

    template<typename t> using sp_t = ::std::shared_ptr<t>;
    template<typename t> using up_t = ::std::unique_ptr<t>;

    class object_t;
    typedef sp_t<object_t> _sp_t;
    typedef up_t<object_t> _up_t;

    // Creates a new object wrapped with smart pointer.
    template<typename t, typename ... args_t>
    X_INLINE sp_t<t> new_sp(args_t && ... args)
    {
        return sp_t<t>(new t(std::forward<args_t>(args) ...));
    }

    //-------- ---------- ---------- ---------- ----------

    // Gets the aligned value of specified size.
    constexpr size_t _align(size_t size, size_t align_size = sizeof(int)) _NE
    {
        return size & ~(align_size - 1);
    }

    // Determinds whether a size is aligned to a specified size.
    constexpr bool __is_aligned(size_t size, size_t align_size = sizeof(int)) _NE
    {
        return (size & (align_size - 1)) == 0;
    }

    // Gets the forward aligned value of specified size.
    constexpr size_t _alignf(size_t size, size_t align_size = sizeof(int)) _NE
    {
        return (size + align_size - 1) & ~(align_size - 1);
    }

    // Gets the min value that large or equals than aligne_size, times by 2.
    constexpr size_t _alignd(size_t size, size_t align_size) _NE
    {
        if (size == 0)
            size = 1;

        while (size < align_size)
        {
            size <<= 1;
        }

        return size;
    }

    //-------- ---------- ---------- ---------- ----------

    namespace
    {
        // Integer type informations.

        template<size_t size, bool sign>
        struct __int_type_info_t { };

        #define __X_INT_VALUE_INFO(type_name)                                           \
            template<> struct __int_type_info_t<sizeof(type_name),                      \
                    std::is_signed<type_name>::value>                                   \
            {                                                                           \
                typedef type_name type;                                                 \
            };

        __X_INT_VALUE_INFO(int8_t)
        __X_INT_VALUE_INFO(uint8_t)
        __X_INT_VALUE_INFO(int16_t)
        __X_INT_VALUE_INFO(uint16_t)
        __X_INT_VALUE_INFO(int32_t)
        __X_INT_VALUE_INFO(uint32_t)
        __X_INT_VALUE_INFO(int64_t)
        __X_INT_VALUE_INFO(uint64_t)

        #undef __X_INT_VALUE_INFO

        // Gets the max integer size that less than specified size.
        template<size_t size>
        X_INLINE constexpr size_t __fix_int_size() _NE
        {
            static_assert(size <= 8, "size must less than 64");

            if (size <= 1)
                return 1;

            if (size <= 2)
                return 2;

            if (size <= 4)
                return 4;

            return 8;
        }
    }

    // Defines an integer type by its size and sign.
    template<size_t size, bool sign=true>
    using int_type_t = typename __int_type_info_t<__fix_int_size<size>(), sign>::type;

    // Defines and unsigned integer type by its size.
    template<size_t size> using uint_type_t = int_type_t<__fix_int_size<size>(), false>;

    // Defines signed integer type of current architecture size.
    typedef int_type_t<sizeof(void *)>  arch_int_t;

    // Defines unsigned integer type of current architecture size.
    typedef uint_type_t<sizeof(void *)> arch_uint_t;
}


#include <common/__enum.h>

namespace X_ROOT_NS {

    //-------- ---------- ---------- ---------- ----------

    // Enum Types
    X_ENUM(value_type_t)

        int8_,

        uint8_,

        int16_,

        uint16_,

        int32_,

        uint32_,

        int64_,

        uint64_,

        float_,

        double_,

        ldouble_,

        bool_,

        char_,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // A value type that include many different types.
    union value_t
    {
        int8_t      int8_value;
        uint8_t     uint8_value;

        int16_t     int16_value;
        uint16_t    uint16_value;

        int32_t     int32_value;
        uint32_t    uint32_value;

        int64_t     int64_value;
        uint64_t    uint64_value;

        float_t     float_value;
        double_t    double_value;
        ldouble_t   ldouble_value;

        bool_t      bool_value;
        char_t      char_value;
    };

    #define __X_EACH_TYPES                                                              \
        __X_TYPE_OP(int8)                                                               \
        __X_TYPE_OP(uint8)                                                              \
        __X_TYPE_OP(int16)                                                              \
        __X_TYPE_OP(uint16)                                                             \
        __X_TYPE_OP(int32)                                                              \
        __X_TYPE_OP(uint32)                                                             \
        __X_TYPE_OP(int64)                                                              \
        __X_TYPE_OP(uint64)                                                             \
        __X_TYPE_OP(float)                                                              \
        __X_TYPE_OP(double)                                                             \
        __X_TYPE_OP(ldouble)                                                            \
        __X_TYPE_OP(bool)                                                               \
        __X_TYPE_OP(char)

    namespace
    {
        // Converts a value type to string.

        template<typename numeric_t> struct __value_t { };
        template<value_type_t type> struct __value_type_t { };

        //-------- ---------- ---------- ---------- ----------

        // Converts numeric value type to string.
        template<typename number_t> struct __value_to_string_t
        {
            static string_t to_string(const number_t & value) _NE
            {
                return std::to_wstring(value);
            }
        };

        //-------- ---------- ---------- ---------- ----------

        // Converts boolean value type to string.
        template<> struct __value_to_string_t<bool>
        {
            static string_t to_string(const bool & value) _NE
            {
                return value? _T("true") : _T("false");
            }
        };

        //-------- ---------- ---------- ---------- ----------

        // Converts character value type to string.
        template<> struct __value_to_string_t<char_t>
        {
            static string_t to_string(const char_t & value) _NE
            {
                const char_t s[] = { _T('\''), value, _T('\''), _T('\0') };
                return string_t(s);
            }
        };

        //-------- ---------- ---------- ---------- ----------

        // Converts numeric type to string.
        template<typename number_t>
        string_t __value_to_string(const number_t & value)
        {
            return __value_to_string_t<number_t>::to_string(value);
        }

        //-------- ---------- ---------- ---------- ----------
        // Defines get/set functions for any numeric types.

        #define __X_VALUE_DEF(type)                                                     \
            template<> struct __value_t<type##_t>                                       \
            {                                                                           \
                typedef type##_t numeric_t;                                             \
                static const value_type_t value_type = value_type_t::type##_;           \
                                                                                        \
                static numeric_t get_value(const value_t & value) _NE                   \
                {                                                                       \
                    return value.type##_value;                                          \
                }                                                                       \
                                                                                        \
                static void set_value(value_t & value, numeric_t v) _NE                 \
                {                                                                       \
                    value.type##_value = v;                                             \
                }                                                                       \
                                                                                        \
                static string_t to_string(const value_t & value)                        \
                {                                                                       \
                    return __value_to_string(value.type##_value);                       \
                }                                                                       \
            };                                                                          \
                                                                                        \
            template<> struct __value_type_t<value_type_t::type##_>                     \
            {                                                                           \
                typedef type##_t numeric_t;                                             \
                static const value_type_t value_type = value_type_t::type##_;           \
            };

        #define __X_TYPE_OP __X_VALUE_DEF
        __X_EACH_TYPES 
        #undef __X_TYPE_OP
    }

    // Get value of specified numeric type.
    template<typename numeric_t> numeric_t get_value(const value_t & value) _NE
    {
        return __value_t<numeric_t>::get_value(value);
    }

    // Set value of specified numeric type.
    template<typename numeric_t> void set_value(value_t & value, numeric_t v) _NE
    {
        __value_t<numeric_t>::set_value(value, v);
    }

    // Set zero value of specifed numeric type.
    template<typename numeric_t> void set_value_zero(value_t & value) _NE
    {
        __value_t<numeric_t>::set_value(value, def_value<numeric_t>());
    }

    // Converts value to value_t type.
    template<typename numeric_t>
    value_t to_value(const numeric_t & numeric) _NE
    {
        value_t value;
        set_value(value, numeric);
        return value;
    }

    // Defines a data structure of numeric_t with its value type.
    struct tvalue_t
    {
        tvalue_t() = default;

        tvalue_t(value_type_t type) _NE : type(type) { }

        template<typename numeric_t>
        tvalue_t(numeric_t v) _NE
            : type(__value_t<numeric_t>::value_type)
            , value(to_value(v)) { }

        value_type_t  type;
        value_t       value;

        template<typename numeric_t>
        tvalue_t & operator = (const tvalue_t & v) _NE
        {
            type = __value_t<numeric_t>::value_type;
            value = to_value(v);
        }

        operator string_t() const _NE
        {
            #define __X_CASE(type)                                                      \
                case value_type_t::type##_:                                             \
                    return __value_t<type##_t>::to_string(value);

            switch (type)
            {
                #define __X_TYPE_OP __X_CASE
                __X_EACH_TYPES 
                #undef __X_TYPE_OP

                default:
                    return _T("");
            }

            #undef __X_CASE
        }

        template<typename numeric_t> numeric_t get_value() const _NE
        {
            if (__value_t<numeric_t>::value_type == type)
                return __root_ns::get_value<numeric_t>(value);

            #define __X_CASE(type)                                                      \
                case value_type_t::type##_:                                             \
                    return static_cast<numeric_t>(__root_ns::get_value<type##_t>(value));

            switch (type)
            {
                #define __X_TYPE_OP __X_CASE
                __X_EACH_TYPES 
                #undef __X_TYPE_OP

                default:
                    return static_cast<numeric_t>(0);
            }

            #undef __X_CASE
        }

        #define __X_TYPE_OP(type) operator type##_t() const { return get_value<type##_t>(); }
        __X_EACH_TYPES 
        #undef __X_TYPE_OP

        bool operator == (const tvalue_t & other) const _NE
        {
            if (type != other.type)
                return false;

            #define __X_CASE(type)                                                      \
                case value_type_t::type##_:                                             \
                    return __root_ns::get_value<type##_t>(value)                        \
                        == __root_ns::get_value<type##_t>(other.value);

            switch (type)
            {
                #define __X_TYPE_OP __X_CASE
                __X_EACH_TYPES 
                #undef __X_TYPE_OP

                default:
                    return true;
            }

            #undef __X_CASE
        }

        bool operator < (const tvalue_t & other) const _NE
        {
            if (type != other.type)
                return type < other.type;

            #define __X_CASE(type)                                                      \
                case value_type_t::type##_:                                             \
                    return __root_ns::get_value<type##_t>(value)                        \
                        < __root_ns::get_value<type##_t>(other.value);

            switch (type)
            {
                #define __X_TYPE_OP __X_CASE
                __X_EACH_TYPES 
                #undef __X_TYPE_OP

                default:
                    return false;
            }

            #undef __X_CASE
        }

        bool operator <= (const tvalue_t & other) const _NE
        {
            if (type != other.type)
                return type <= other.type;

            #define __X_CASE(type)                                                      \
                case value_type_t::type##_:                                             \
                    return __root_ns::get_value<type##_t>(value)                        \
                        <= __root_ns::get_value<type##_t>(other.value);

            switch (type)
            {
                #define __X_TYPE_OP __X_CASE
                __X_EACH_TYPES 
                #undef __X_TYPE_OP

                default:
                    return true;
            }

            #undef __X_CASE
        }

        bool operator != (const tvalue_t & other) const _NE
        {
            return ! operator == (other);
        }

        bool operator > (const tvalue_t & other) const _NE
        {
            return ! operator <= (other);
        }

        bool operator >= (const tvalue_t & other) const _NE
        {
            return ! operator < (other);
        }

        tvalue_t operator - () const _NE
        {
            #define __X_CASE(type)                                                      \
                case value_type_t::type##_:                                             \
                    return tvalue_t(-value.type##_value);

            switch (type)
            {
                #define __X_TYPE_OP __X_CASE
                __X_EACH_TYPES 
                #undef __X_TYPE_OP

                default:
                    return *this;
            }

            #undef __X_CASE
        }

        // Converts to unsigned type.
        tvalue_t to_unsigned() const _NE
        {
            switch (type)
            {
                case value_type_t::int8_:
                    return tvalue_t((uint8_t)value.int8_value);

                case value_type_t::int16_:
                    return tvalue_t((uint16_t)value.int16_value);

                case value_type_t::int32_:
                    return tvalue_t((uint32_t)value.int32_value);

                case value_type_t::int64_:
                    return tvalue_t((uint64_t)value.int64_value);

                default:
                    return *this;
            }
        }

        // Converts to signed type.
        tvalue_t to_signed() const _NE
        {
            switch (type)
            {
                case value_type_t::uint8_:
                    return tvalue_t((int8_t)value.uint8_value);

                case value_type_t::uint16_:
                    return tvalue_t((int16_t)value.uint16_value);

                case value_type_t::uint32_:
                    return tvalue_t((int32_t)value.uint32_value);

                case value_type_t::uint64_:
                    return tvalue_t((int64_t)value.uint64_value);

                default:
                    return *this;
            }
        }
    };

    // Puts the tvalue_t to a stream.
    ostream_t & operator << (ostream_t & stream, const tvalue_t & v);

    //-------- ---------- ---------- ---------- ----------

    // Limits
    #define __X_LIMIT_VALUES(type)                                                      \
        const type##_t type##_max = std::numeric_limits<type##_t>::max();               \
        const type##_t type##_min = std::numeric_limits<type##_t>::min();

    __X_LIMIT_VALUES(int8)
    __X_LIMIT_VALUES(int16)
    __X_LIMIT_VALUES(int32)
    __X_LIMIT_VALUES(int64)
    __X_LIMIT_VALUES(uint8)
    __X_LIMIT_VALUES(uint16)
    __X_LIMIT_VALUES(uint32)
    __X_LIMIT_VALUES(uint64)
    __X_LIMIT_VALUES(char)

    __X_LIMIT_VALUES(int)
    __X_LIMIT_VALUES(uint)
    __X_LIMIT_VALUES(short)
    __X_LIMIT_VALUES(ushort)
    __X_LIMIT_VALUES(long)
    __X_LIMIT_VALUES(ulong)

    __X_LIMIT_VALUES(double)
    __X_LIMIT_VALUES(float)

    __X_LIMIT_VALUES(size)

    #undef __X_LIMIT_VALUES

    //-------- ---------- ---------- ---------- ----------

    namespace
    {
        enum class __value_limit_family_t { unknown, numeric, enum_ };

        template<typename t>
        constexpr __value_limit_family_t __value_limit_family() _NE
        {
            return std::is_enum<t>::value? __value_limit_family_t::enum_
                : std::is_arithmetic<t>::value?  __value_limit_family_t::numeric
                : __value_limit_family_t::unknown;
        }

        template<typename t, __value_limit_family_t family>
        struct __limit_t
        {
            static constexpr t min() _NE { return def_value<t>(); }
            static constexpr t max() _NE { return def_value<t>(); }
        };

        // Limits of numeric type
        template<typename t>
        struct __limit_t<t, __value_limit_family_t::numeric>
        {
            static constexpr t min() _NE { return std::numeric_limits<t>::min(); }
            static constexpr t max() _NE { return std::numeric_limits<t>::max(); }
        };

        // Limits of enumeration types.
        template<typename t>
        struct __limit_t<t, __value_limit_family_t::enum_>
        {
            static constexpr t min() _NE { return (t)0; }
            static constexpr t max() _NE { return t::__end__; }
        };
    }

    // Min value of specified type.
    template<typename t> constexpr t min_value() _NE
    {
        return __limit_t<t, __value_limit_family<t>()>::min();
    }

    // Max value of specified type.
    template<typename t> constexpr t max_value() _NE
    {
        return __limit_t<t, __value_limit_family<t>()>::max();
    }

    ////////// ////////// ////////// ////////// //////////

    // Remove pointer const.
    template<typename t> struct remove_pointer_const { typedef t type; };
    template<typename t> struct remove_pointer_const<const t *> { typedef t * type; };
    template<typename t> using remove_pointer_const_t = typename remove_pointer_const<t>::type;

    // Remove reference & const.
    template<typename t> using remove_rc_t = std::remove_const_t<std::remove_reference_t<t>>;

    ////////// ////////// ////////// ////////// //////////

    // Gets an static array size.
    template<typename t, size_t size>
    constexpr size_t array_size(t (&arr)[size]) _NE { return size; }

    // Gets an static string length.
    template<typename t, size_t size>
    constexpr size_t string_length(t (&arr)[size]) _NE { return size - 1; }

    ////////// ////////// ////////// ////////// //////////

    // Picks a type in the given type list that not void type.
    namespace
    {
        template<typename t, typename ... ts> struct __types_t { typedef t type_t; };
        template<typename ... ts> struct __types_t<void, ts ...> : __types_t<ts ...> { };
        template<> struct __types_t<void> { typedef void type_t; };
    }

    template<typename ... ts>
    using pick_type_t = typename __types_t<ts ...>::type_t;

    ////////// ////////// ////////// ////////// //////////

    // Wrapped with begin iterator and end iterator of a collection.
    template<typename _itor_t>
    struct range_t
    {
        typedef _itor_t itor_t;
        typedef decltype(*(*(itor_t *)nullptr)) value_type;

        range_t() = default;
        range_t(itor_t begin, itor_t end) _NE: __begin(begin), __end(end) { }

        itor_t begin() const _NE { return __begin; }
        itor_t end()   const _NE { return __end; }

        size_t size()  const _NE { return __end - __begin; }

    private:
        itor_t __begin, __end;   
    };

    // Defines begin/end functions for ' c++ for statements' .
    template<typename itor_t> itor_t begin(range_t<itor_t> & r) _NE { return r.begin(); }
    template<typename itor_t> itor_t end(range_t<itor_t> & r)   _NE { return r.end(); }

    // Puts a collection to a stream, splited by spaces.
    template<typename stream_t, typename itor_t>
    stream_t & operator << (stream_t & stream, const range_t<itor_t> & r)
    {
        for (itor_t it_begin = r.begin(), it_end = r.end(), it = it_begin; it != it_end; it++)
        {
            if (it != it_begin)
                stream << _T(" ");
            stream << *it;
        }

        return stream;
    }

    // Gets the range of specified begin/end iterators.
    template<typename itor_t>
    range_t<itor_t> _range(itor_t begin, itor_t end) _NE
    {
        return range_t<itor_t>(begin, end);
    }

    // Gets the range of specified collection.
    template<typename collection_t, typename itor_t=typename collection_t::iterator>
    range_t<itor_t> _range(collection_t & collection)
    {
        return range_t<itor_t>(collection.begin(), collection.end());
    }

    // Gets the range of specified collection.
    template<typename collection_t, typename itor_t=typename collection_t::const_iterator>
    range_t<itor_t> _range(const collection_t & collection)
    {
        return range_t<itor_t>(collection.cbegin(), collection.cend());
    }

    // Gets the reserved range of specified collection.
    template<typename collection_t, typename itor_t=typename collection_t::reverse_iterator>
    range_t<itor_t> _rrange(collection_t & collection)
    {
        return range_t<itor_t>(collection.rbegin(), collection.rend());
    }

    // Gets the reserved range of specified collection.
    template<typename collection_t, typename itor_t=typename collection_t::const_reverse_iterator>
    range_t<itor_t> _rrange(const collection_t & collection)
    {
        return range_t<itor_t>(collection.crbegin(), collection.crend());
    }

    ////////// ////////// ////////// ////////// //////////
    // Variable template args

    namespace
    {
        template<size_t type_count, typename callback_t, typename t, typename ... types_t>
        struct __each_types_t
        {
            static void func(callback_t & callback)
            {
                if (callback.template operator()<t>())
                {
                    __each_types_t<type_count - 1, callback_t, types_t ...>::func(callback);
                }
            }
        };

        template<typename t, typename callback_t>
        struct __each_types_t<1, callback_t, t>
        {
            static void func(callback_t & callback)
            {
                callback.template operator()<t>();
            }
        };
    }

    // Enumerates the type list.
    template<typename ... types_t, typename callback_t>
    void each_types(callback_t & callback)
    {
        __each_types_t<sizeof...(types_t), callback_t, types_t ...>::func(callback);
    }

    //-------- ---------- ---------- ---------- ----------

    namespace
    {
        template<typename ... args_t>
        struct __args_help_t
        {
            typedef std::tuple<args_t ...> __tuple_t;

            template<size_t index>
            using type_t = decltype(std::get<index>(*(__tuple_t *)nullptr));
        };
    }

    // Gets the type of type list by index.
    template<size_t index, typename ... args_t>
    using typeat = typename __args_help_t<args_t ...>::template type_t<index>;

    #define X_TYPEAT(args_t, index) typeat<index, args_t ...>

    ////////// ////////// ////////// ////////// //////////

}  // namespace X_ROOT_NS


////////// ////////// ////////// ////////// //////////

#include <common/__string.h>

namespace X_ROOT_NS {

    // Complex Types
    ////////// ////////// ////////// ////////// //////////

    // The super type that not copyable.
    class no_copy_ctor_t
    {
    public:
        no_copy_ctor_t() { }
        no_copy_ctor_t(const no_copy_ctor_t &) = delete;

        no_copy_ctor_t & operator = (const no_copy_ctor_t &) = delete;
    };

    ////////// ////////// ////////// ////////// //////////

    // The super type that not movable.
    class no_move_ctor_t
    {
        no_move_ctor_t(no_move_ctor_t &&) = delete;
        no_move_ctor_t & operator = (no_move_ctor_t &&) = delete;
    };

    ////////// ////////// ////////// ////////// //////////

    // The super class of all
    class object_t
    {
    public:
        object_t() { }
        virtual ~object_t() { }

    public:
        virtual const string_t to_string() const
        {
            return _T("[object]");
        }
    };

    //-------- ---------- ---------- ---------- ----------
    // Defines to_string() overiable functions.

    #define X_TO_STRING                                                                 \
        operator string_t() const { return this->to_string(); }                         \
        virtual const string_t to_string() const override;

    #define X_TO_STRING_IMPL_                                                           \
        operator string_t() const { return this->to_string(); }                         \
        virtual const string_t to_string() const override

    #define X_DEFINE_TO_STRING(type_t)                                                  \
        const string_t type_t::to_string() const

    #define X_TO_STRING_IMPL(_s)                                                        \
        operator string_t() const { return this->to_string(); }                         \
        virtual const string_t to_string() const override { return _s; }

    //-------- ---------- ---------- ---------- ----------
    // Puts an object to a stream.

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const object_t & obj)
    {
        return stream << obj.to_string().c_str();
    }

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const object_t * obj)
    {
        if (obj != nullptr)
            stream << obj->to_string().c_str();

        return stream;
    }

    ////////// ////////// ////////// ////////// //////////

    // The super type of all exceptions.
    class error_t : public object_t
    {
    public:
        virtual const string_t & get_message() const = 0;
        virtual const string_t to_string() const override;

        operator string_t() const { return this->to_string(); }
    };

    //-------- ---------- ---------- ---------- ----------

    // The super type of exceptions with message, file and line.
    class __error_base_t : public error_t
    {
    public:
        template<typename message_t>
        __error_base_t(message_t && message, const char_t * file=nullptr, size_t line=(size_t)-1)
            : message(std::forward<message_t>(message)), file(file? file : _T("")), line(line) { }

        virtual const string_t & get_message() const override;
        virtual const string_t to_string() const override;

    public:
        const string_t message;
        const string_t file;
        const size_t line;
    };

    //-------- ---------- ---------- ---------- ----------

    // Exception: assert_error_t
    class assert_error_t : public __error_base_t
    {
    public:
        using __error_base_t::__error_base_t;
    };

    //-------- ---------- ---------- ---------- ----------
    // Logic exceptions with error code.

    template<typename error_code_t> class logic_error_t;

    // Super class of logic exception
    class logic_error_base_t : public __error_base_t
    {
    public:
        using __error_base_t::__error_base_t;

    public:
        virtual int get_code() const = 0;

        template<typename code_t> bool equals_code(code_t code) const _NE
        {
            typedef logic_error_t<code_t> error_t;
            const error_t * err = dynamic_cast<const error_t *>(this);
            return err && err->code == code;
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Super class of logic exception with error code
    template<typename error_code_t>
    class logic_error_t : public logic_error_base_t
    {
    public:
        template<typename message_t>
        logic_error_t(error_code_t code, message_t && message,
                const char_t * file=nullptr, size_t line=(size_t)-1) _NE
            : logic_error_base_t(std::forward<message_t>(message), file, line)
            , code(code) { }

    public:
        typedef error_code_t code_t;
        const error_code_t code;

    public:
        virtual int get_code() const _NE override
        {
            return (int)code;
        }
    };

    //-------- ---------- ---------- ---------- ----------
    // Creates a logic exception with error code and message.

    template<typename error_code_t, typename message_t>
    logic_error_t<error_code_t> _error(error_code_t code, message_t && message,
            const char_t * file=nullptr, size_t line=(size_t)-1) _NE
    {
        return logic_error_t<error_code_t>(code, std::forward<message_t>(message), file, line);
    }

    template<typename error_code_t>
    logic_error_t<error_code_t> _error(error_code_t code, 
            const char_t * file=nullptr, size_t line=(size_t)-1) _NE
    {
        const char_t * desc = _desc(code);
        if (desc == nullptr or desc[0] == _T('\0'))
            desc = _title(code);

        return _error(code, desc, file, line);
    }

    //-------- ---------- ---------- ---------- ----------
    // Common error codes.

    X_ENUM(common_error_code_t)

        invalid_operation,

        not_supported,

        conflict,

        no_memory, 

        overflow,

        argument_error,

        format_error,

        convert_error,

        not_found,

        unexpected,

        unimplemented,

    X_ENUM_END

    // Creates an unimplemented exception.
    logic_error_t<common_error_code_t> _unimplemented_error(const object_t * obj,
                                                            const char_t * method) _NE;

    // Creates an unimplemented exception.
    logic_error_t<common_error_code_t> _unimplemented_error(object_t * obj,
                                                     const char_t * method) _NE;

    //-------- ---------- ---------- ---------- ----------
    // A macro of _error function.

    #if X_DEBUG

    #define X_ERROR(code, args...)                                                      \
        __root_ns::_error(code, ##args, _T(__FILE__), __LINE__)

    #else

    #define X_ERROR(code, args...)                                                      \
        __root_ns::_error(code, ##args)

    #endif

    ////////// ////////// ////////// ////////// //////////
    // X_ASSERT

    #if X_DEBUG

    #define X_ASSERT(x, args...)                                                        \
        do {                                                                            \
            if (!(x)) {                                                                 \
                __root_ns::__raise_assert_error(_T("") #x,                              \
                    _T(__FILE__), __LINE__, ##args);                                    \
            }                                                                           \
        } while (0)

    #else

    #define X_ASSERT(x, args...)

    #endif

    void __raise_assert_error(const char_t * exp, const char_t * file, 
                            uint_t line, const char_t * reason = nullptr);

    ////////// ////////// ////////// ////////// //////////

    // Dynamic cast an object to specified type, returns nullptr if failed.
    template<typename t, typename t0> X_INLINE t as(t0 obj) _NE
    {
        return dynamic_cast<t>(obj);
    }

    // Detemined whether an object is specified type.
    template<typename t, typename t0> X_INLINE bool is(t0 obj) _NE
    {
        return as<t>(obj) != nullptr;
    }

    // Cast an object to specified type, raise exception if failed.
    template<typename t, typename t0> X_INLINE t must(t0 obj)
    {
        t obj1 = dynamic_cast<t>(obj);
        if (obj1 == nullptr)
            throw _E(common_error_code_t::convert_error);
        return obj1;
    }

    // Returns a string of specified type name.
    template<typename t> string_t type_str() _NE
    {
        return string_convert<char, char_t>(typeid(t).name());
    }

    // Assert a value must be a specified type.
    #define X_MUST(type_t, value)                                                       \
        (([](auto v) -> type_t {                                                        \
            if (v == nullptr)                                                           \
                return (type_t)nullptr;                                                 \
            type_t v2 = dynamic_cast<type_t>(v);                                        \
            if (v2 == nullptr)                                                          \
                throw _ECF(convert_error, _T("cannot convert from %1% to %2%"),         \
                        type_str<decltype(v)>(), type_str<type_t>());                   \
            return v2;                                                                  \
        })(value))

    ////////// ////////// ////////// ////////// //////////

    class memory_t;

    namespace
    {
        struct __creator_empty_args_t { };
        const __creator_empty_args_t __creator_empty_args;
    }

    // An object creator interface
    // Provide various implements for creating/destorying an object.

    template<typename obj_t, typename args_t = __creator_empty_args_t>
    X_INTERFACE creator_t
    {
        // Creates an object, using the specified memory manager.
        virtual obj_t * create(memory_t * memory, const args_t & args = __creator_empty_args) = 0;

        // Destroys an object, using the specified memory manager.
        virtual void destory(memory_t * memory, obj_t * obj) = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    // An object clonable interface
    // Provide various implements for cloning an object.

    template<typename obj_t>
    X_INTERFACE clonable_t
    {
        // Clones the object, using the spcified memory manager.
        virtual obj_t * clone(memory_t * memory) const = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    // An object equals interface
    // Provide various implements for comparing two objects.

    template<typename obj_t>
    X_INTERFACE equals_t
    {
        // Determines if self object is equals than the specified object.
        virtual bool equals(const obj_t & obj) const = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    // An object less interface
    // Provide various implements for comparing two objects.

    template<typename obj_t>
    X_INTERFACE less_t
    {
        // Determines if self object is less than the specified object.
        virtual bool less(const obj_t & obj) const = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    // An key/value object interface.
    // Provides the ability for searching value by the spcified key.

    template<typename _key_t, typename _value_t>
    X_INTERFACE kv_object_t
    {
        typedef _key_t   key_t;
        typedef _value_t value_t;

        // Gets value of the specified key.
        virtual value_t get(const key_t & key) = 0;

        // Sets value for the specified key.
        virtual void set(const key_t & key, value_t & value) = 0;
    };

    ////////// ////////// ////////// ////////// //////////
    // Enum operations

    // Enum bit-or, no arguments, returns empty.
    template<typename enum_t>
    X_INLINE constexpr enum_t enum_or() _NE
    {
        return (enum_t)0;
    }

    // Enum bit-or, only one arguments, returns it.
    template<typename enum_t>
    X_INLINE constexpr enum_t enum_or(enum_t v) _NE
    {
        return v;
    }

    // Enum bit-or, two arguments, returns the bit-or result.
    template<typename enum_t>
    X_INLINE constexpr enum_t enum_or(enum_t v1, enum_t v2) _NE
    {
        typedef int_type_t<sizeof(enum_t)> t;
        return (enum_t)((t)v1 | (t)v2);
    }

    // Enum bit-or, more than two arguments, returns the bit-or result of all values.
    template<typename enum_t, typename ... args_t>
    X_INLINE constexpr enum_t enum_or(enum_t v1, enum_t v2, args_t ... args) _NE
    {
        return enum_or(enum_or(v1, v2), args ...);
    }

    //-------- ---------- ---------- ---------- ----------

    // Add flags to the specified enum. equals bit-or operation.
    template<typename enum_t, typename ... args_t>
    X_INLINE constexpr enum_t enum_add_flag(enum_t & v, enum_t v1, args_t ... args) _NE
    {
        return (v = enum_or(v, v1, args ...));
    }

    //-------- ---------- ---------- ---------- ----------

    // Enum bit-and, no arguments, returns empty.
    template<typename enum_t>
    X_INLINE constexpr enum_t enum_and() _NE
    {
        return (enum_t)0;
    }

    // Enum bit-and, only one arguments, returns it.
    template<typename enum_t>
    X_INLINE constexpr enum_t enum_and(enum_t v) _NE
    {
        return v;
    }

    // Enum bit-and, two arguments, returns the bit-and result.
    template<typename enum_t>
    X_INLINE constexpr enum_t enum_and(enum_t v1, enum_t v2) _NE
    {
        typedef int_type_t<sizeof(enum_t)> t;
        return (enum_t)((t)v1 & (t)v2);
    }

    // Enum bit-and, more than two arguments, returns the bit-and result of all values.
    template<typename enum_t, typename ... args_t>
    X_INLINE constexpr enum_t enum_and(enum_t v1, enum_t v2, args_t ... args) _NE
    {
        return enum_and(enum_and(v1, v2), args ...);
    }

    //-------- ---------- ---------- ---------- ----------

    // Remove flag from the specified enum.
    template<typename enum_t, typename ... args_t>
    X_INLINE constexpr enum_t enum_remove_flag(enum_t & v, enum_t v1, args_t ... args) _NE
    {
        typedef int_type_t<sizeof(enum_t)> t;
        return (v = enum_and(v, (enum_t)~(t)v1, (enum_t)~(t)args ...));
    }

    //-------- ---------- ---------- ---------- ----------

    // Enum bit-xor, no arguments, returns empty.
    template<typename enum_t>
    X_INLINE constexpr enum_t enum_xor() _NE
    {
        return (enum_t)0;
    }

    // Enum bit-xor, only one arguments, returns it.
    template<typename enum_t>
    X_INLINE constexpr enum_t enum_xor(enum_t v) _NE
    {
        return v;
    }

    // Enum bit-xor, two arguments, returns the bit-xor result.
    template<typename enum_t>
    X_INLINE constexpr enum_t enum_xor(enum_t v1, enum_t v2) _NE
    {
        typedef int_type_t<sizeof(enum_t)> t;
        return (enum_t)((t)v1 ^ (t)v2);
    }

    // Enum bit-xor, more than two arguments, returns the bit-xor result of all values.
    template<typename enum_t, typename ... args_t>
    X_INLINE constexpr enum_t enum_xor(enum_t v1, enum_t v2, args_t ... args) _NE
    {
        return enum_xor(enum_xor(v1, v2), args ...);
    }

    //-------- ---------- ---------- ---------- ----------

    // Enum bit-not
    template<typename enum_t>
    X_INLINE constexpr enum_t enum_not(enum_t v) _NE
    {
        typedef int_type_t<sizeof(enum_t)> t;
        return (enum_t)(~(t)v);
    }

    //-------- ---------- ---------- ---------- ----------

    // Determines if the specivied value has the specified flag.
    template<typename enum_t>
    X_INLINE constexpr bool enum_has_flag(enum_t v, enum_t flag)
    {
        return enum_and(v, flag) != (enum_t)0;
    }

    // Determines if the specivied value matchs the specified flag.
    template<typename enum_t>
    X_INLINE constexpr bool enum_match_flag(enum_t v, enum_t flag) _NE
    {
        return enum_and(v, flag) == flag;
    }

    ////////// ////////// ////////// ////////// //////////

    // An box wrapper for object_t
    template<typename t>
    class box_t : public object_t
    {
        typedef std::remove_reference_t<std::remove_const_t<t>> _t;

    public:
        typedef t value_t;

        box_t(_t && value) _NE : value(value) { }
        box_t(const t & value) _NE : value(value) { }

        t value;

        X_TO_STRING_IMPL(_T("box_t"))
    };

    // Creates an box wrapper for the specified value.
    template<typename t>
    X_INLINE auto _box(t && value) _NE
    {
        return new box_t<std::remove_reference_t<t>>(std::forward<t>(value));
    }

    // Gets the value of the specified boxed object.
    template<typename t>
    X_INLINE t & _unbox(object_t *& obj)
    {
        _A(obj != nullptr);

        typedef std::remove_reference_t<t> _t;
        box_t<_t> * p = dynamic_cast<box_t<_t> *>(obj);
        if (p != nullptr)
            return p->value;

        throw _E(common_error_code_t::convert_error);
    }

    // Gets the value of the specified boxed object.
    template<typename t>
    X_INLINE t & _unbox(box_t<t> * obj) _NE
    {
        _A(obj != nullptr);

        return obj->value;
    }

    // Gets the value of the specified boxed object.
    template<>
    X_INLINE object_t *& _unbox<object_t *>(object_t *& obj)
    {
        return obj;
    }

    ////////// ////////// ////////// ////////// //////////

    // Gives the ability of key/value storage for the object.
    template<typename _key_t=string_t, typename _value_t=object_t *>
    class metadata_t
    {
        typedef std::remove_reference_t<std::remove_const_t<_key_t>> key_t;
        typedef std::remove_reference_t<std::remove_const_t<_value_t>> value_t;

    public:
        metadata_t() = default;

        // Adds a key/value pair metadata.
        void add(key_t && key, value_t && value)
        {
            __data[std::forward<_key_t>(key)] = std::forward<_value_t>(value);
        }

        // Removes a key/value pair metadata.
        void remove(const key_t & key)
        {
            auto it = __data.find(key);
            if (it != __data.end())
                __data.erase(it);
        }

        // Gets the value for the specified key.
        value_t & get(const key_t & key)
        {
            value_t * value;
            if (try_get(key, &value))
                return *value;

            throw _E(common_error_code_t::not_found);
        }

        // Tries to get value for the specified key.
        // Returns true if found, the value is returned by out_value.
        // Returns false if not found.
        bool try_get(const key_t & key, value_t ** out_value=nullptr)
        {
            auto it = __data.find(key);
            if (it != __data.end())
            {
                if (out_value)
                    *out_value = &it->second;
                return true;
            }

            return false;
        }

    private:
        std::map<const key_t, value_t> __data; 
    };

    //-------- ---------- ---------- ---------- ----------

    // Gives the ability of key/value storage for the object.
    // When value is object_t *

    template<typename _key_t>
    class metadata_t<_key_t, object_t *>
    {
        typedef std::remove_reference_t<std::remove_const_t<_key_t>> key_t;
        typedef up_t<object_t> value_t;

    public:

        metadata_t() = default;

        // Adds a key/value pair metadata.
        void add(key_t && key, object_t * value)
        {
            remove(key);
            __data[std::forward<_key_t>(key)] = __value_wrapper_t { value, false };
        }

        // Adds a key/value pair metadata.
        template<typename t>
        t & add(key_t && key, t && value)
        {
            typedef remove_rc_t<t> _t;
            typedef typename decltype(__data)::value_type pair_t;

            auto pair = pair_t(std::forward<key_t>(key), __value_wrapper_t {
                _box(std::forward<t>(value)), true
            });

            auto ret = __data.insert(pair);
            bool success = ret.second;
            if (!success)
            {
                __erase(ret.first);
                ret = __data.insert(pair);
            }

            return _unbox<std::remove_reference_t<t>>(pair.second.value);
        }

        // Removes a key/value pair metadata.
        void remove(const key_t & key)
        {
            auto it = __data.find(key);
            if (it != __data.end())
                __erase(it);
        }

        // Gets the value for the specified key.
        template<typename t>
        t & get(const key_t & key)
        {
            t * value;
            if (try_get(key, &value))
                return *value;

            throw _E(common_error_code_t::not_found);
        }

        // Gets the value for the specified key.
        // Auto create it when not found.
        template<typename t, typename key_t_, typename creator_t>
        t & get(key_t_ && key, creator_t creator)
        {
            t * value;
            if (try_get(key, &value))
                return *value;

            typedef remove_rc_t<key_t_> key_t;
            return add(std::forward<key_t>(key), creator());
        }

        // Gets the value for the specified key.
        // Auto create it when not found.
        template<typename t, typename key_t_>
        t & get(key_t_ && key, bool auto_create)
        {
            typedef remove_rc_t<key_t_> key_t;
            if (auto_create)
                return get<t>(std::forward<key_t>(key), []() { return t(); });
            return get<t>(key);
        }

        // Tries to get value for the specified key.
        // Returns true if found, the value is returned by out_value.
        // Returns false if not found.
        template<typename t>
        bool try_get(const key_t & key, t ** out_value=nullptr)
        {
            auto it = __data.find(key);
            if (it != __data.end())
            {
                if (out_value)
                    *out_value = &_unbox<std::remove_reference_t<t>>(it->second.value);
                return true;
            }

            return false;
        }

        // Deallocator, delete all values. (inherited by object_t)
        ~metadata_t()
        {
            for (auto it = __data.begin(), it_end = __data.end();
                it != it_end; it++)
            {
                if (it->second.boxed)
                    delete it->second.value;
            }
        }

    private:

        struct __value_wrapper_t
        {
            object_t *  value;
            bool        boxed;
        };

        std::map<const key_t, __value_wrapper_t> __data; 
        typedef typename decltype(__data)::iterator itor_t;

        // When remove an key/value pair.
        // Delete the object. (inherited by object_t)
        void __erase(itor_t it)
        {
            if (it->second.boxed)
                delete it->second.value;
            __data.erase(it);
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // Flags when creates objects.
    X_ENUM(memory_flag_t)

        // If inherited by object_t.
        is_object       = 1 << 0,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // An memory manager interface.
    X_INTERFACE memory_t
    {
        // Allocates an object
        virtual void * alloc(size_t size, memory_flag_t flag = memory_flag_t::__default__) = 0;

        // Frees an object
        virtual void   free(void * obj)   = 0;

        // Reallocates an object
        virtual void * realloc(void * obj, size_t size,
                               memory_flag_t flag = memory_flag_t::__default__) = 0;

        // Creates an object with specified arguments.
        template<typename obj_t, typename ... args_t>
        obj_t * new_obj(args_t && ... args)
        {
            memory_flag_t flag = __revise_memory_flag<obj_t>();

            void * buffer = (obj_t *)alloc(sizeof(obj_t), flag);
            return new(buffer) obj_t(std::forward<args_t>(args) ...);
        }

        // Creates an object with specified size and arguments.
        template<typename obj_t, typename ... args_t>
        obj_t * new_obj_with_size(size_t size, args_t && ... args)
        {
            memory_flag_t flag = __revise_memory_flag<obj_t>();

            void * buffer = (obj_t *)alloc(size, flag);
            return new(buffer) obj_t(std::forward<args_t>(args) ...);
        }

        // Static member. 
        // Creates an object with specified arguments.
        template<typename obj_t, typename ... args_t>
        static obj_t * new_obj(memory_t * memory, args_t && ... args)
        {
            if (!memory)
                memory = __default_memory();

            return memory->new_obj<obj_t>(std::forward<args_t>(args) ...);
        }

        // Static member.
        // Creates an object with specified size and arguments.
        template<typename obj_t, typename ... args_t>
        static obj_t * new_obj(size_t size, memory_t * memory, args_t && ... args)
        {
            if (!memory)
                memory = __default_memory();

            return memory->new_obj_with_size<obj_t>(size, std::forward<args_t>(args) ...);
        }

        // Deletes an object
        void delete_obj(object_t * obj)
        {
            _A(obj != nullptr);

            obj->~object_t();
            free((void *)obj);
        }

        // Static member.  Deletes an object.
        static void delete_obj(memory_t * memory, object_t * obj)
        {
            if (!memory)
                memory = __default_memory();

            memory->delete_obj(obj);
        }

        // Static member.
        // Allocates memory of specified size.
        static void * alloc(memory_t * memory, size_t size,
                memory_flag_t flag = memory_flag_t::__default__)
        {
            if (!memory)
                memory = __default_memory();

            return memory->alloc(size, flag);
        }

        // Static member.
        // Creates multiy objects of specified count.
        template<typename obj_t>
        static obj_t * alloc_objs(memory_t * memory, size_t count,
                                  memory_flag_t flag = memory_flag_t::__default__)
        {
            flag = __revise_memory_flag<obj_t>(flag);
            return (obj_t *)alloc(memory, sizeof(obj_t) * count, flag);
        }

        // Static member.
        // Frees specified object.
        static void free(memory_t * memory, void * obj)
        {
            if (!memory)
                memory = __default_memory();

            memory->free(obj);
        }

    private:
        static memory_t * __default_memory();

        // Auto detemined the memory_flag_t.
        template<typename obj_t>
        static memory_flag_t __revise_memory_flag(memory_flag_t flag = memory_flag_t::__default__)
        {
            static_assert(
                std::is_base_of<object_t, obj_t>::value ||
                std::is_trivially_destructible<obj_t>::value,
                "not an object class, it's destructor will not be executed"
            );

            if (std::is_base_of<object_t, obj_t>::value)
                enum_add_flag(flag, memory_flag_t::is_object);
            else
                enum_remove_flag(flag, memory_flag_t::is_object);

            return flag;
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // An interface of reading stream.
    X_INTERFACE xistream_t
    {
        // Read bytes specified size from the stream.
        virtual size_t read(byte_t * buffer, size_t size) = 0;

        // Completed notify.
        virtual void completed() = 0;

        template<typename t> xistream_t & operator >> (t & v)
        {
            read((byte_t *)&v, sizeof(t));
        }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // An implemention of xistream_t for file.
    class file_xistream_t : public object_t, public xistream_t
    {
        typedef std::ios_base __i;
        typedef std::ios_base::openmode __open_mode_t;

    public:
        // Read bytes of specified size from the stream.
        virtual size_t read(byte_t * buffer, size_t size) override;

        // Completed notify.
        virtual void completed() override;

        // Opens the specified file for reading, throw exceptions when failed.
        void open(const string_t & path, __open_mode_t open_mode = __i::in | __i::binary);

        // Dealloctor
        virtual ~file_xistream_t() override;

        X_TO_STRING_IMPL(_T("file_xistream_t"))

    private:
        std::fstream    __stream;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // An interface for writing stream.
    X_INTERFACE xostream_t
    {
        typedef byte_t char_type;

        // Writes bytes to the stream.
        virtual void write(const byte_t * buffer, size_t size) = 0;

        // Completed notify.
        virtual void completed() = 0;

        template<typename t> xostream_t & operator << (const t & v)
        {
            write((const byte_t *)&v, sizeof(t));
        }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // An implemention of xistream_t for file.
    class file_xostream_t : public object_t, public xostream_t
    {
        typedef std::ios_base __i;
        typedef std::ios_base::openmode __open_mode_t;

    public:
        // Writes bytes to the stream.
        virtual void write(const byte_t * buffer, size_t length) override;

        // Completed notify.
        virtual void completed() override;

        // Opens the specified file for writing, throw exceptions when failed.
        void open(const string_t & path,
            __open_mode_t open_mode = __i::out | __i::trunc | __i::binary);

        // Dealloctor
        virtual ~file_xostream_t() override;

        X_TO_STRING_IMPL(_T("file_xostream_t"))

    private:
        std::fstream    __stream;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // An interface for reading/writing stream.
    X_INTERFACE xiostream_t : xistream_t, xostream_t { };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // The xstream implemention base for the specified stream.
    template<typename _stream_t>
    class xstream_impl_base_t
    {
    public:
        xstream_impl_base_t(_stream_t & stream) : stream(stream) { }

        _stream_t stream;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // The xistream implemention for the specified reading stream.

    template<typename _istream_t>
    class xistream_impl_t : virtual public xstream_impl_base_t<_istream_t>
    {
        typedef xstream_impl_base_t<_istream_t> __super_t;

    public:
        using __super_t::__super_t;

        // Read bytes of specified size from the stream.
        virtual size_t read(const byte_t * buffer, size_t size) override
        {
            __super_t::stream.read(buffer, size);
            return __super_t::stream.gcount();
        }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // The xistream implemention for the specified writing stream.

    template<typename _ostream_t>
    class xostream_impl_t : virtual public xstream_impl_base_t<_ostream_t>
    {
        typedef xstream_impl_base_t<_ostream_t> __super_t;

    public:
        using __super_t::__super_t;

        // Writes bytes to the stream.
        virtual void write(const byte_t * buffer, size_t size) override
        {
            __super_t::stream.write(buffer, size);
        }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -
    // The xstream implemention for the specified stream.

    template<typename _stream_t>
    class xstream_impl_t : virtual public xistream_impl_t<_stream_t>
                         , virtual public xostream_impl_t<_stream_t>
    {
    public:
        xstream_impl_t(_stream_t & stream) : xstream_impl_base_t<_stream_t>(stream) { }
    };

    ////////// ////////// ////////// ////////// //////////

    // The default memory management.

    class default_memory_t : public object_t, public memory_t
    {
        // allocates memory of specified size.
        virtual void * alloc(size_t size, memory_flag_t flag = memory_flag_t::__default__) override;

        // Frees memory.
        virtual void   free(void * obj) override;

        // Reallocates memory with specified size.
        virtual void * realloc(void * obj, size_t size,
                               memory_flag_t flag = memory_flag_t::__default__) override;

    public:
        static default_memory_t * instance();

        X_TO_STRING_IMPL(_T("default_memory_t"))
    };

    ////////// ////////// ////////// ////////// //////////

    // Supports the ability of memory management for an object.

    class memory_base_t
    {
    protected:
        // Constructor
        memory_base_t(memory_t * memory = nullptr)
            : __memory(memory) { }

        // Creates an object with specified arguments.
        template<typename obj_t, typename ... args_t>
        obj_t * __new_obj(args_t && ... args)
        {
            return memory_t::new_obj<obj_t>(__memory, std::forward<args_t>(args) ...);
        }

        // Deletes an object.
        void __delete_obj(object_t * obj)
        {
            memory_t::delete_obj(__memory, obj);
        }

        // Allocates memory of specified size.
        void * __alloc(size_t size, memory_flag_t flag = memory_flag_t::__default__)
        {
            return memory_t::alloc(__memory, size, flag);
        }

        // Allocates memory for the specified object with specified size.
        template<typename obj_t>
        obj_t * __alloc_objs(size_t count, memory_flag_t flag = memory_flag_t::__default__)
        {
            return memory_t::alloc_objs<obj_t>(__memory, count, flag);
        }

        // Frees memory.
        void __free(void * obj)
        {
            memory_t::free(__memory, obj);
        }

        memory_t * __memory;
    };

    ////////// ////////// ////////// ////////// //////////

    // A default implemenation of creator_t.

    template<typename obj_t, typename args_t = __creator_empty_args_t>
    class default_creator_t : public object_t, public creator_t<obj_t, args_t>
    {
        typedef default_creator_t<obj_t, args_t> __self_t;

    public:
        // Creates an object with the specified memory manager.
        virtual obj_t * create(memory_t * memory, const args_t & args) override
        {
            _A(memory != nullptr);
            return memory->new_obj<obj_t>();
        }

        // Deletes an object with the specified memory manager.
        virtual void destory(memory_t * memory, obj_t * obj) override
        {
            _A(memory != nullptr);
            memory->delete_obj(obj);
        }

        // An sigleton instance
        static __self_t * instance()
        {
            static __self_t * creator = new __self_t();
            return creator;
        }

        X_TO_STRING_IMPL(_T("default_creator_t"))
    };

    ////////// ////////// ////////// ////////// //////////

    // An object pool, be used for store objects that deallocated at the end of lifecycle.
    class pool_t : public object_t, public memory_t
    {
    public:

        // Returns all objects in the pool.
        auto all() _NE
        {
            return _range(__objects);
        }

        // Allocates memory of specified size.
        virtual void * alloc(size_t size, memory_flag_t flag = memory_flag_t::__default__) override;

        // Free memory of the given object.
        virtual void   free(void * obj) override;

        // Reallocates memory for the given object to the specified size.
        virtual void * realloc(void * obj, size_t size,
                               memory_flag_t flag = memory_flag_t::__default__) override;

        // Deallocator, delete all objects in the pool.
        virtual ~pool_t() override;

        X_TO_STRING_IMPL(_T("pool_t"))

    private:
        std::set<object_t *> __objects;
    };

    //-------- ---------- ---------- ---------- ----------

    namespace
    {
        // The pool wrapper provides the ability of pool management for an object.
        // Objects in the pool will deallocated with the owner object.

        template<typename obj_t, typename _pool_t=pool_t>
        class __pool_wrapper_t : public obj_t
        {
        public:
            using obj_t::obj_t;

            // Creates an object with the given arguments.
            template<typename _obj_t, typename ... args_t>
            _obj_t * new_obj(args_t && ... args)
            {
                return __pool.template new_obj<_obj_t>(std::forward<args_t>(args) ...);
            }

            // Returns the pool.
            _pool_t * get_pool() _NE { return &__pool; }

        private:
            _pool_t __pool;
        };
    }

    // Defines the new type with the ability of pool management.
    template<typename obj_t, typename _pool_t=pool_t>
    using pw_t = __root_ns::__pool_wrapper_t<obj_t, _pool_t>;

    // Creates an object with ablility of pool management.
    template<typename obj_t, typename _pool_t=pool_t, typename ... args_t>
    __pool_wrapper_t<obj_t> * new_wrap(args_t && ... args)
    {
        return new __pool_wrapper_t<obj_t, _pool_t>(std::forward<args_t>(args) ...);
    }

    ////////// ////////// ////////// ////////// //////////

    // The super type of comparable object that defines lots of compare operators.

    template<typename self_t, typename underlying_t>
    struct compare_operators_t
    {
        bool operator == (const self_t & other) const _NE
        {
            return __u(this) == __u(other);
        }

        bool operator != (const self_t & other) const _NE
        {
            return __u(this) != __u(other);
        }

        bool operator < (const self_t & other) const _NE
        {
            return __u(this) < __u(other);
        }

        bool operator <= (const self_t & other) const _NE
        {
            return __u(this) <= __u(other);
        }

        bool operator > (const self_t & other) const _NE
        {
            return __u(this) > __u(other);
        }

        bool operator >= (const self_t & other) const _NE
        {
            return __u(this) >= __u(other);
        }

    private:
        typedef compare_operators_t<self_t, underlying_t> __this_t;

        underlying_t __u(const self_t & o) const _NE
        {
            return (underlying_t)o;
        }

        underlying_t __u(const __this_t * o) const _NE
        {
            return __u(*(const self_t *)o);
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // A callback interface, be used for event handles.

    template<typename args_t>
    X_INTERFACE callback_t
    {
        void operator() (args_t & args)
        {
            on_call(this, args);
        }

        // Called when an event raised.
        virtual void on_call(void * sender, args_t & args) = 0;
    };

    //-------- ---------- ---------- ---------- ---------- 
    // Callback args

    template<typename _action_t>
    struct callback_args_t
    {
        typedef _action_t action_t;

        callback_args_t(_action_t action) _NE : action(action) { }

        const _action_t action;
    };

    //-------- ---------- ---------- ---------- ---------- 

    // Callback args with data of given type.

    template<typename _action_t, typename _data_t>
    struct tcallback_args_t : public callback_args_t<_action_t>
    {
        typedef _data_t data_t;

        // Constructors.
        template<typename __data_t>
        tcallback_args_t(_action_t action, __data_t data) _NE
            : callback_args_t<_action_t>(action), data(std::forward<__data_t>(data)) { }

        // Argument data.
        _data_t data;
    };

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        // Defines operations of convertion a object to string.

        enum class __object_type_t {
            unknown, string, object, string_like, enum_, pointer, numeric,
            char_array,
        };

        // Determines the object types.
        template<typename t>
        constexpr X_INLINE __object_type_t __to_object_type() _NE
        {
            typedef std::remove_const_t<
                std::remove_reference_t<std::remove_pointer_t<t>>
            > _t;

            // An enum value
            if (std::is_enum<_t>::value)
                return __object_type_t::enum_;

            // A string
            if (std::is_same<_t, string_t>::value)
                return __object_type_t::string;

            // An object. ( inherited by object_t. )
            if (std::is_convertible<_t, object_t>())
                return __object_type_t::object;

            // Can converted to string.
            if (std::is_convertible<_t, string_t>())
                return __object_type_t::string_like;

            // Char array
            if (std::is_same<t, wchar_t *>::value || std::is_same<t, const wchar_t *>::value)
                return __object_type_t::char_array;

            // Pointer.
            if (std::is_pointer<t>::value)
                return __object_type_t::pointer;

            // Arithmetic
            if (std::is_arithmetic<_t>::value)
                return __object_type_t::numeric;

            // Unknown
            return __object_type_t::unknown;
        }

        //-------- ---------- ---------- ---------- ---------- 

        // The description string for a null object.
        const string_t __null_str = _T("<NULL>");

        template<typename _obj_t, __object_type_t> struct __to_str_t
        {
            static const string_t to_str(const _obj_t & obj)
            {
                //static_assert(false, "cannot convert to string");
                return _T("?");
            }
        };

        //-------- ---------- ---------- ---------- ---------- 

        // Converts an enum value to string.
        template<typename _obj_t>
        struct __to_str_t<_obj_t, __object_type_t::enum_>
        {
            static const string_t to_str(const _obj_t & obj) _NE
            {
                return _title(obj);
            }
        };

        //-------- ---------- ---------- ---------- ---------- 

        // Converts an enum value (pointer) to string.
        template<typename _obj_t>
        struct __to_str_t<_obj_t *, __object_type_t::enum_>
        {
            static const string_t to_str(const _obj_t * obj)
            {
                if (obj == nullptr)
                    return __null_str;

                return _title(*obj);
            }
        };

        //-------- ---------- ---------- ---------- ---------- 

        // Converts a string like object (operator string_t() defined) to string.
        template<typename _obj_t>
        struct __to_str_t<_obj_t, __object_type_t::string_like>
        {
            static const string_t to_str(const _obj_t & obj)
            {
                return (string_t)obj;
            }
        };

        //-------- ---------- ---------- ---------- ---------- 

        // Converts a string like object (pointer, operator string_t() defined) to string.
        template<typename _obj_t>
        struct __to_str_t<_obj_t *, __object_type_t::string_like>
        {
            static const string_t to_str(const _obj_t * obj)
            {
                if (obj == nullptr)
                    return __null_str;

                return (string_t)*obj;
            }
        };

        //-------- ---------- ---------- ---------- ---------- 

        // Returns the string object.
        template<typename _obj_t>
        struct __to_str_t<_obj_t, __object_type_t::string>
        {
            static const string_t to_str(const _obj_t & obj)
            {
                return obj;
            }
        };

        //-------- ---------- ---------- ---------- ---------- 

        // Returns the string object (pointer).
        template<typename _obj_t>
        struct __to_str_t<_obj_t *, __object_type_t::string>
        {
            static const string_t to_str(const _obj_t * obj)
            {
                if (obj == nullptr)
                    return __null_str;

                return *obj;
            }
        };

        //-------- ---------- ---------- ---------- ---------- 

        // Convert an object to string.
        template<typename _obj_t>
        struct __to_str_t<_obj_t, __object_type_t::object>
        {
            static const string_t to_str(const _obj_t & obj)
            {
                return obj.to_string();
            }
        };

        //-------- ---------- ---------- ---------- ---------- 

        // Convert an object to string (pointer).
        template<typename _obj_t>
        struct __to_str_t<_obj_t *, __object_type_t::object>
        {
            static const string_t to_str(const _obj_t * obj)
            {
                if (obj == nullptr)
                    return __null_str;

                return obj->to_string();
            }
        };

        // Convert a pointer to string.
        template<typename _obj_t>
        struct __to_str_t<_obj_t *, __object_type_t::pointer>
        {
            static const string_t to_str(const _obj_t * obj)
            {
                stringstream_t ss;
                ss << (void *)obj;

                return ss.str();
            }
        };

        // Convert a numeric to string..
        template<typename _obj_t>
        struct __to_str_t<_obj_t, __object_type_t::numeric>
        {
            static const string_t to_str(const _obj_t & obj)
            {
                return std::to_wstring(obj);
            }
        };

        // Convert a char array to string..
        template<typename _obj_t>
        struct __to_str_t<_obj_t, __object_type_t::char_array>
        {
            static const string_t to_str(const char_t * p)
            {
                if (p == nullptr)
                    return __null_str;

                return string_t(p);
            }
        };

    }

    // Converts an object to string.
    template<typename _obj_t>
    X_INLINE const string_t _str(const _obj_t & obj)
    {
        return __to_str_t<_obj_t, __to_object_type<_obj_t>()>::to_str(obj);
    }

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        template<size_t _n, typename ... _args_t>
        struct __tuple_to_string_t
        {
            static void append_string(const std::tuple<_args_t ...> & tuple, stringstream_t & ss)
            {
                if (_n != sizeof ... (_args_t))
                    ss << _T(", ");

                auto v = std::get<sizeof ... (_args_t) - _n>(tuple);
                ss << _str(v).c_str();

                __tuple_to_string_t<_n - 1, _args_t ...>::append_string(tuple, ss);
            }
        };

        template<typename ... _args_t>
        struct __tuple_to_string_t<0, _args_t ...>
        {
            static void append_string(const std::tuple<_args_t ...> & tuple, stringstream_t & ss)
            {
                // Do nothing.
            }
        };
    }

    template<typename ... _args_t>
    string_t _str(const std::tuple<_args_t ...> & tuple)
    {
        stringstream_t ss;
        __tuple_to_string_t<sizeof ... (_args_t), _args_t ...>::append_string(tuple, ss);

        return ss.str();
    }

    ////////// ////////// ////////// ////////// //////////

    // Enumerator wrapper for normal operations.
    template<typename _enum_t> struct enum_t
    {
        typedef enum_t<_enum_t> __self_t;
        typedef int_type_t<sizeof(_enum_t)> __underly_t;

        // Constructors.
        enum_t() = default;
        enum_t(_enum_t value) _NE : value(value) { }
        enum_t(int value) _NE : value( (_enum_t)value ) { }

        // Enum value.
        _enum_t value = (_enum_t)0;

        // Converts to an enum value.
        operator _enum_t() const _NE { return value; }

        // Converts to an enum value.
        _enum_t operator * () const _NE { return value; }

        // Converts to a bool value.
        operator bool() const _NE { return value != (_enum_t)0; }

        // Assigns a value.
        __self_t operator = (_enum_t v) _NE
        {
            this->value = v;
        }

        // Assigns a value.
        __self_t operator = (__self_t v) _NE
        {
            this->value = v.value;
        }

        // Equals.
        bool operator == (_enum_t v) const _NE
        {
            return value == v;
        }

        // Equals to a number.
        bool operator == (int v) const _NE
        {
            return value == (_enum_t)v;
        }

        // Not equals.
        bool operator != (_enum_t v) const _NE
        {
            return value != v;
        }

        // Not equals to a number.
        bool operator != (int v) const _NE
        {
            return value != (_enum_t)v;
        }

        // Bit or & assign.
        __self_t operator |= (_enum_t v) _NE
        {
            return this->value = enum_or(this->value, v), *this;
        }

        // Bit or.
        __self_t operator | (_enum_t v) const _NE
        {
            return __self_t( enum_or(this->value, v) );
        }

        // Bit and & assign.
        __self_t operator &= (_enum_t v) _NE
        {
            return this->value = enum_and(this->value, v), *this;
        }

        // Bit and
        __self_t operator & (_enum_t v) const _NE
        {
            return __self_t( enum_and(this->value, v) );
        }

        // Bit xor && assign.
        __self_t operator ^= (_enum_t v) _NE
        {
            return this->value = enum_xor(this->value, v), *this;
        }

        // Bit xor.
        __self_t operator ^ (_enum_t v) const _NE
        {
            return __self_t( enum_xor(this->value, v) );
        }

        // Bit not.
        __self_t operator ~ () _NE
        {
            return this->value = ~this->value, *this;
        }

        // Returns whether has a flag.
        template<typename ... _enums_t>
        bool has(_enum_t v, _enums_t ... vs) const _NE
        {
            return enum_has_flag(this->value, enum_or(v, vs...));
        }

        // Returns whether it is composed by specified flags.
        template<typename ... _enums_t>
        bool has_only(_enum_t v, _enums_t ... vs) const _NE
        {
            return (__underly_t)this->value != 0
                && ((__underly_t)this->value & ~(__underly_t)enum_or(v, vs...)) == 0;
        }

        // Returns whether it is composed by specified flags.
        bool has_only(_enum_t v) _NE
        {
            return (__underly_t)this->value != 0
                && ((__underly_t)this->value & ~(__underly_t)v) == 0;
        }

        // Removes a flag.
        template<typename ... _enums_t>
        __self_t remove(_enum_t v, _enums_t ... vs) _NE
        {
            return enum_remove_flag(this->value, enum_or(v, vs...));
        }

        // Adds a flag.
        template<typename ... _enums_t>
        __self_t add(_enum_t v, _enums_t ... vs) _NE
        {
            return this->value = enum_or(this->value, v, vs...), *this;
        }

        // Clears flags.
        __self_t clear() _NE
        {
            return this->value = (_enum_t)0, *this;
        }

        // Converts to string.
        operator string_t() const _NE
        {
            return _str(this->value);
        }
    };

    ////////// ////////// ////////// ////////// //////////

    namespace
    {

        // Enum flags.
        template<typename _enum_t>
        struct __enum_flags_t
        {
            typedef __enum_flags_t<_enum_t> __self_t;
            typedef int_type_t<sizeof(_enum_t)> __underly_t;

            // Constructors.
            __enum_flags_t(_enum_t value) _NE : value(value) { }

            // Enum value.
            _enum_t value;

            // Converts to string.
            operator string_t() const _NE
            {
                __underly_t v = (__underly_t)value;
                if (v == 0)
                    return _title(value);

                stringstream_t ss;

                for (int index = 0; v != 0; index++)
                {
                    __underly_t v0 = v & (v - 1);

                    if (index > 0)
                        ss << _T(",");

                    ss << _title((_enum_t)(v - v0));
                    v = v0;
                }

                return ss.str();
            }
        };
    }

    // Returns __enum_flags_t for an enum, be used for convers enum flags to a string.
    template<typename _enum_t> __enum_flags_t<_enum_t> _eflags(_enum_t value) _NE
    {
        return __enum_flags_t<_enum_t>(value);
    }

    ////////// ////////// ////////// ////////// //////////

    // Converts an object to int value with the same size.
    template<typename t> auto _int(const t & value) _NE
    {
        typedef int_type_t<sizeof(t)> int_t;
        return (int_t)value;
    }

    ////////// ////////// ////////// ////////// //////////

    // Output value wrapper, the base class that be used for write a value as specified format.

    template<typename t>
    struct __output_wrapper_t
    {
        explicit __output_wrapper_t(const t & value) _NE : value(value) { }
        const t & value;

        template<typename stream_t> void write_to(stream_t & stream) const
        {
            stream << value;
        }
    };

    //-------- ---------- ---------- ---------- ---------- 

    // Converts a value to hex string, be used for write a value to a stream as hex format.

    template<typename t>
    struct __hex_output_wrapper_t : __output_wrapper_t<t>
    {
        using __output_wrapper_t<t>::__output_wrapper_t;

        template<typename stream_t> void write_to(stream_t & stream) const
        {
            stream << std::hex << this->value << std::dec;
        }
    };

    #define __OutputWrapper(name)                                                       \
        template<typename t>                                                            \
        __##name##_output_wrapper_t<t> _##name(const t & value)                         \
        {                                                                               \
            return __##name##_output_wrapper_t<t>(value);                               \
        }                                                                               \
                                                                                        \
        template<typename stream_t, typename t>                                         \
        X_INLINE stream_t & operator << (stream_t & stream,                             \
                        const __##name##_output_wrapper_t<t> & w)                       \
        {                                                                               \
            w.write_to(stream);                                                         \
            return stream;                                                              \
        }

    __OutputWrapper(hex)

    #undef __OutputWrapper

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        // Convert a char to hex value, returns -1 when converts fault.
        constexpr int __digit_value(char_t c) _NE
        {
            if (c <= _T('9') && c >= _T('0'))
                return c - _T('0');

            if (c <= _T('F') && c >= _T('A'))
                return c - _T('A') + 10;

            if (c <= _T('f') && c >= _T('a'))
                return c - _T('a') + 10;

            return -1;
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Guid structure.
    struct guid_t : compare_operators_t<guid_t, std::tuple<uint64_t, uint64_t>>
    {
        uint8_t __data[16];

        // Equals operator.
        bool operator == (const guid_t & other) _NE
        {
            return *(const int64_t *)this == *(const int64_t *)&other
                    && *((const int64_t *)this + 1) == *((const int64_t *)&other + 1);
        }

        // Converts guid structure to string.
        operator string_t () const _NE
        {
            stringstream_t ss;

            const uint8_t * p = (const uint8_t *)__data;
            ss << std::hex
               << p[0] << p[1] << p[2] << p[3] << _T("-")
               << p[4] << p[5] << _T("-")
               << p[6] << p[7] << _T("-")
               << p[8] << p[9] << _T("-")
               << p[10] << p[11] << p[12] << p[13] << p[14] << p[15];

            return ss.str();
        }

        // An empty guid structure.
        static const guid_t empty;

        // Parses a string to guid_t structure.
        static constexpr guid_t parse(const char_t * s)
        {
            if (s == nullptr)
                return guid_t::empty;

            guid_t g;

            uint8_t * gp = (uint8_t *)&g;
            for (const char_t * p = s, * p_end = p + 36; p < p_end;)
            {
                if (*p == _T('-'))
                {
                    size_t index = p - s;
                    if (index != 8 && index != 13 && index != 18 && index != 23)
                        throw _EC(format_error, _T("guid format error"));
                    p++;
                }
                else
                {
                    int digit1 = __digit_value(*p), digit2 = __digit_value(*(p + 1));
                    if (digit1 < 0 || digit2 < 0)
                    {
                        throw _EC(format_error, _T("guid format error"));
                    }
                    else
                    {
                        *gp++ = (digit1 << 4) | digit2;
                    }
                    p += 2;
                }
            }

            return g;
        }

        // An identity tuple converter, be used for compare to guid objects by two long values.
        // Used by compare_operators_t base class.
        operator std::tuple<uint64_t, uint64_t>() const _NE
        {
            return std::tuple<uint64_t, uint64_t>(
                *(uint64_t *)__data, *(uint64_t *)(__data + 8)
            );
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // A buffer structure contains byte_t * pointer and the size.
    struct buffer_t
    {
        buffer_t() = default;
        buffer_t(const byte_t * bytes, size_t length) _NE
            : bytes(bytes), length(length)
        { }

        const byte_t * bytes;
        size_t length;
    };

    namespace
    {
        // Converts digit number to char.
        X_INLINE char_t __digit_to_char(int value) _NE
        {
            if (value <= 9)
                return _T('0') + value;

            return _T('A') + (value - 10);
        }
    }

    // Write a buffer to the given stream.
    template<typename _stream_t>
    _stream_t & operator << (_stream_t & stream, const buffer_t & buffer) _NE
    {
        if (buffer.length == 0)
            return stream;

        #define __ToChars(byte) __digit_to_char(byte >> 4), __digit_to_char(byte & 0x0F)

        for (const byte_t * p = buffer.bytes, * p_end = p + buffer.length - 1;
                                    p < p_end; p++)
        {
            byte_t byte = *p;
            char_t s[] = { __ToChars(byte), _T(' '), _T('\0') };

            stream << s;
        }

        byte_t byte = buffer.bytes[buffer.length - 1];
        char_t s[] = { __ToChars(byte), _T('\0') };
        stream << s;

        #undef __ToChars

        return stream;
    }

    ////////// ////////// ////////// ////////// //////////

    // A grammar for try ... finally statement.
    template<typename try_function_t, typename finally_function_t>
    void try_finally(try_function_t try_function, finally_function_t finally_function)
    {
        try
        {
            try_function();
        }
        catch (...)
        {
            finally_function();
            throw;
        }

        finally_function();
    }

    // A grama for try ... finally statement, with a return value.
    template<typename try_function_t, typename finally_function_t>
    auto try_finally_r(try_function_t try_function, finally_function_t finally_function)
    {
        typedef decltype(try_function()) ret_t;
        ret_t r;

        try
        {
            r = try_function();
        }
        catch (...)
        {
            finally_function();
            throw;
        }

        finally_function();
        return r;
    }

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        // Validate bits.
        template<typename _t, size_t _bits> void __validate_bits()
        {
            static_assert(_bits <= sizeof(_t) * 8 && _bits >= 1,
                "_size must less/equals than sizeof(_t) * 8 and large than 0");
        }

        // Max/min values of specified bits. for signed values.
        template<typename _t, size_t _bits, bool _signed = std::is_signed<_t>::value>
        struct __limit_of_bits_t
        {
            // Returns the max value of specified bits.
            constexpr static _t max_of_bits() _NE
            {
                __validate_bits<_t, _bits>();
                return max_value<_t>() >> ((sizeof(_t) << 3) - _bits);
            }

            // Returns the min value of specified bits.
            constexpr static _t min_of_bits() _NE
            {
                __validate_bits<_t, _bits>();

                typedef std::make_unsigned_t<_t> ut;
                return _t(ut((_t)-1) << (_bits - 1));
            }
        };

        // Max/min values of specified bits. for unsigned values.
        template<typename _t, size_t _bits>
        struct __limit_of_bits_t<_t, _bits, false>  // unsigned.
        {
            // Returns the max value of specified bits.
            constexpr static _t max_of_bits() _NE
            {
                __validate_bits<_t, _bits>();
                return max_value<_t>() >> ((sizeof(_t) << 3) - _bits);
            }

            // Returns the min value of specified bits.
            constexpr static _t min_of_bits() _NE
            {
                __validate_bits<_t, _bits>();

                return 0;
            }
        };

        template<typename _t, size_t _bits, size_t _offset = 0>
        struct __bits_value_t
        {
            _t  __nouse__ : _offset;
            _t  value     : _bits;
        };

        template<typename _t, size_t _bits>
        struct __bits_value_t<_t, _bits, 0>
        {
            _t  value     : _bits;
        };
    }

    // Returns the max value of specified bits.
    template<typename _t, size_t _bits>
    constexpr _t max_of_bits() _NE
    {
        return __limit_of_bits_t<_t, _bits>::max_of_bits();
    }

    // Returns the min value of specified bits.
    template<typename _t, size_t _bits>
    constexpr _t min_of_bits() _NE
    {
        return __limit_of_bits_t<_t, _bits>::min_of_bits();
    }

    // Returns the value of specified bits.
    template<typename _t, size_t _bits, size_t _offset = 0>
    _t value_of_bits(const void * p) _NE
    {
        __validate_bits<_t, _bits>();

        return ((__bits_value_t<_t, _bits, _offset> *)p)->value;
    }

    // Returns the value of specified bits.
    template<typename _t, size_t _bits, size_t _offset = 0>
    void set_value_of_bits(void * p, _t value) _NE
    {
        __validate_bits<_t, _bits>();

        ((__bits_value_t<_t, _bits, _offset> *)p)->value = value;
    }

    // Returns whether the value is in the range of specified bits.
    template<typename _t, size_t _bits = sizeof(_t) * 8, typename _v_t>
    constexpr bool in_range(_v_t value) _NE
    {
        return value <= max_of_bits<_t, _bits>() && value >= min_of_bits<_t, _bits>();
    }

    ////////// ////////// ////////// ////////// //////////

    // The wrapper of object, used when creates an object that not inherited by object_t.
    template<typename _t>
    class obj_wrap_t : public _t, public object_t
    {
    public:

        using _t::_t;

        X_TO_STRING_IMPL_
        {
            return _str( *(_t *)this );
        }
    };

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS

#include <common/__stream.h>
#include <common/__log.h>

#endif  // __COMMON_H__
