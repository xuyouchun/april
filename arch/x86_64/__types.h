#ifndef __ARCH_TYPES_H__
#define __ARCH_TYPES_H__

#define X_UNICODE_STR(s)        L##s
#define X_CONVERT_TO_STR(s)     X_UNICODE_STR("") #s

namespace X_ROOT_NS { namespace arch { namespace types {

    // Common Types
    typedef signed char          int8_t;
    typedef signed short         int16_t;
    typedef signed int           int32_t;
    typedef signed long long     int64_t;
    typedef unsigned char        uint8_t;
    typedef unsigned short       uint16_t;
    typedef unsigned int         uint32_t;
    typedef unsigned long long   uint64_t;
    typedef unsigned long        size_t;

    // Strings
    typedef wchar_t              char_t;
    typedef ::std::basic_string<char_t>        string_t;
    typedef ::std::basic_ostringstream<char_t> stringstream_t;

    // Streams
    typedef decltype(::std::wcout)             ostream_t;
    typedef decltype(::std::wcin)              istream_t;

    // Streams
    extern istream_t & cin;
    extern ostream_t & cout;
    extern ostream_t & cerr;
    extern ostream_t & clog;

} } }  // namespace X_ROOT_NS::arch::types


#endif  // __ARCH_TYPES_H__

