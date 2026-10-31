#include <rtlib.h>

namespace X_ROOT_NS::modules::rtlib {

    ////////// ////////// ////////// ////////// //////////
    // Memory of the strings made by programs.

    static string_allocator_t __string_allocator = nullptr;

    // Sets the allocator of the strings.
    void set_string_allocator(string_allocator_t allocator)
    {
        __string_allocator = allocator;
    }

    // Allocates the memory of a string, it is never freed unless the allocator is managed.
    void * allocate_string_memory(size_t bytes)
    {
        if (__string_allocator != nullptr)
            return __string_allocator(bytes);

        return std::malloc(bytes);
    }

    ////////// ////////// ////////// ////////// //////////

    // Length of a string: a null string is empty.
    static size_t __length_of(const char_t * s)
    {
        return s == nullptr? 0 : std::char_traits<char_t>::length(s);
    }

    // Creates a string from the text.
    static const char_t * __new_string(const char_t * s, size_t length)
    {
        char_t * result = (char_t *)allocate_string_memory((length + 1) * sizeof(char_t));
        if (length > 0)
            std::memcpy(result, s, length * sizeof(char_t));

        result[length] = 0;
        return result;
    }

    // Creates a string from a wide string.
    static const char_t * __new_string(const std::wstring & s)
    {
        std::basic_string<char_t> text(s.begin(), s.end());
        return __new_string(text.c_str(), text.size());
    }

    ////////// ////////// ////////// ////////// //////////

    struct __string2_args_t
    {
        rstring_t   a;
        rstring_t   b;
    };

    // String_Concat
    void string_concat(rtlib_context_t & ctx)
    {
        __string2_args_t * args = ctx.args<__string2_args_t>();
        const char_t * a = *args->a, * b = *args->b;
        size_t length_a = __length_of(a), length_b = __length_of(b);

        char_t * result = (char_t *)allocate_string_memory(
                                            (length_a + length_b + 1) * sizeof(char_t));
        if (length_a > 0)
            std::memcpy(result, a, length_a * sizeof(char_t));

        if (length_b > 0)
            std::memcpy(result + length_a, b, length_b * sizeof(char_t));

        result[length_a + length_b] = 0;
        ctx.return_<const char_t *>(result);
    }

    // Compares two strings, a null string is less than the others.
    static int __compare(const char_t * a, const char_t * b)
    {
        if (a == b)
            return 0;

        if (a == nullptr)
            return -1;

        if (b == nullptr)
            return 1;

        size_t length_a = __length_of(a), length_b = __length_of(b);

        int r = std::char_traits<char_t>::compare(a, b, std::min(length_a, length_b));
        if (r != 0)
            return r < 0? -1 : 1;

        return length_a == length_b? 0 : (length_a < length_b? -1 : 1);
    }

    // String_Equals
    void string_equals(rtlib_context_t & ctx)
    {
        __string2_args_t * args = ctx.args<__string2_args_t>();
        ctx.return_<bool>(__compare(*args->a, *args->b) == 0);
    }

    // String_Compare
    void string_compare(rtlib_context_t & ctx)
    {
        __string2_args_t * args = ctx.args<__string2_args_t>();
        ctx.return_<int32_t>(__compare(*args->a, *args->b));
    }

    struct __string1_args_t
    {
        rstring_t   s;
    };

    // String_GetHashCode: FNV-1a of the characters.
    void string_get_hash_code(rtlib_context_t & ctx)
    {
        __string1_args_t * args = ctx.args<__string1_args_t>();
        const char_t * s = *args->s;

        uint32_t hash = 2166136261u;
        for (size_t k = 0, length = __length_of(s); k < length; k++)
        {
            hash = (hash ^ (uint32_t)s[k]) * 16777619u;
        }

        ctx.return_<int32_t>((int32_t)(hash & 0x7fffffff));
    }

    // String_GetLength
    void string_get_length(rtlib_context_t & ctx)
    {
        __string1_args_t * args = ctx.args<__string1_args_t>();
        ctx.return_<int32_t>((int32_t)__length_of(*args->s));
    }

    ////////// ////////// ////////// ////////// //////////
    // Numbers to strings.

    template<typename _rt_t> struct __number_args_t
    {
        _rt_t   value;
    };

    // Int64_ToString
    void int64_to_string(rtlib_context_t & ctx)
    {
        ctx.return_<const char_t *>(__new_string(
            std::to_wstring(*ctx.args<__number_args_t<rint64_t>>()->value)
        ));
    }

    // UInt64_ToString
    void uint64_to_string(rtlib_context_t & ctx)
    {
        ctx.return_<const char_t *>(__new_string(
            std::to_wstring(*ctx.args<__number_args_t<ruint64_t>>()->value)
        ));
    }

    // Double_ToString
    void double_to_string(rtlib_context_t & ctx)
    {
        ctx.return_<const char_t *>(__new_string(
            format_floating(*ctx.args<__number_args_t<rdouble_t>>()->value)
        ));
    }

    // Single_ToString
    void float_to_string(rtlib_context_t & ctx)
    {
        ctx.return_<const char_t *>(__new_string(
            format_floating(*ctx.args<__number_args_t<rfloat_t>>()->value)
        ));
    }

    // Char_ToString
    void char_to_string(rtlib_context_t & ctx)
    {
        char_t c = *ctx.args<__number_args_t<rchar_t>>()->value;
        ctx.return_<const char_t *>(__new_string(&c, 1));
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::rtlib
