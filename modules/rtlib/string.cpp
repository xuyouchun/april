#include <rtlib.h>
#include <charconv>

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

    // Length of a string: a null string is empty. It is kept in the head of the string.
    static size_t __length_of(const rt_char_t * s)
    {
        return (size_t)rt_string_length(s);
    }

    // Creates a string from the text.
    static const rt_char_t * __new_string(const rt_char_t * s, size_t length)
    {
        rt_char_t * result = rt_string_init(allocate_string_memory(rt_string_bytes(length)), length);
        if (length > 0)
            std::memcpy(result, s, length * sizeof(rt_char_t));

        return result;
    }

    // Creates a string from a wide string.
    static const rt_char_t * __new_string(const std::wstring & s)
    {
        ustring_t text = to_ustring(s);
        return __new_string(text.c_str(), text.size());
    }

    // Creates a string of the runtime from a text (for the other files of the library).
    const rt_char_t * new_rt_string(const std::wstring & text)
    {
        return __new_string(text);
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
        const rt_char_t * a = *args->a, * b = *args->b;
        size_t length_a = __length_of(a), length_b = __length_of(b);

        rt_char_t * result = rt_string_init(
                        allocate_string_memory(rt_string_bytes(length_a + length_b)), length_a + length_b);
        if (length_a > 0)
            std::memcpy(result, a, length_a * sizeof(rt_char_t));

        if (length_b > 0)
            std::memcpy(result + length_a, b, length_b * sizeof(rt_char_t));

        result[length_a + length_b] = 0;
        ctx.return_<const rt_char_t *>(result);
    }

    // Compares two strings, a null string is less than the others.
    static int __compare(const rt_char_t * a, const rt_char_t * b)
    {
        if (a == b)
            return 0;

        if (a == nullptr)
            return -1;

        if (b == nullptr)
            return 1;

        size_t length_a = __length_of(a), length_b = __length_of(b);

        int r = std::char_traits<rt_char_t>::compare(a, b, std::min(length_a, length_b));
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
        const rt_char_t * s = *args->s;

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

    struct __substring_args_t
    {
        rstring_t   s;
        rint32_t    start;
        rint32_t    length;
    };

    // String_Substring: the range is checked by the caller, it is cut to the string here.
    void string_substring(rtlib_context_t & ctx)
    {
        __substring_args_t * args = ctx.args<__substring_args_t>();
        const rt_char_t * s = *args->s;

        size_t size = __length_of(s);
        size_t start = (size_t)std::max<int32_t>(*args->start, 0);
        size_t length = (size_t)std::max<int32_t>(*args->length, 0);

        if (start > size)
            start = size;

        if (length > size - start)
            length = size - start;

        ctx.return_<const rt_char_t *>(__new_string(s + start, length));
    }

    // String_GetChar: the index is checked by the caller.
    void string_get_char(rtlib_context_t & ctx)
    {
        struct args_t { rstring_t s; rint32_t index; };
        args_t * args = ctx.args<args_t>();
        const rt_char_t * s = *args->s;
        int32_t index = *args->index;

        ctx.return_<rt_char_t>(index >= 0 && (size_t)index < __length_of(s)? s[index] : 0);
    }

    // String_FromChars: a string of the characters (the range is checked by the caller).
    void string_from_chars(rtlib_context_t & ctx)
    {
        struct args_t { robject_t chars; rint32_t start; rint32_t count; };
        args_t * args = ctx.args<args_t>();
        const rt_char_t * chars = (const rt_char_t *)(void *)(rt_ref_t)args->chars;
        int32_t start = *args->start, count = *args->count;

        if (chars == nullptr || start < 0 || count <= 0)
            count = 0, start = 0;

        ctx.return_<const rt_char_t *>(__new_string(chars + start, (size_t)count));
    }

    // String_CopyTo: copies the characters of the string to the array (the ranges are checked by the
    // caller).
    void string_copy_to(rtlib_context_t & ctx)
    {
        struct args_t { rstring_t s; rint32_t source; robject_t chars; rint32_t start; rint32_t count; };
        args_t * args = ctx.args<args_t>();
        const rt_char_t * s = *args->s;
        rt_char_t * chars = (rt_char_t *)(void *)(rt_ref_t)args->chars;
        int32_t source = *args->source, start = *args->start, count = *args->count;

        if (s != nullptr && chars != nullptr && source >= 0 && start >= 0 && count > 0)
            std::memmove(chars + start, s + source, (size_t)count * sizeof(rt_char_t));
    }

    // String_IndexOf: the first position of the value at or after the start, or -1. An empty value is
    // found at the start (if the start is in the string).
    void string_index_of(rtlib_context_t & ctx)
    {
        struct args_t { rstring_t s; rstring_t value; rint32_t start; };
        args_t * args = ctx.args<args_t>();
        const rt_char_t * s = *args->s, * value = *args->value;
        size_t size = __length_of(s), count = __length_of(value);
        int32_t start = *args->start;

        if (start < 0 || (size_t)start > size)
        {
            ctx.return_<int32_t>(-1);
            return;
        }

        for (size_t k = (size_t)start; k + count <= size; k++)
        {
            if (count == 0 || std::memcmp(s + k, value, count * sizeof(rt_char_t)) == 0)
            {
                ctx.return_<int32_t>((int32_t)k);
                return;
            }
        }

        ctx.return_<int32_t>(-1);
    }

    // String_LastIndexOf: the last position of the value that ends at or before the end (the end is
    // the length of the searched part from the beginning), or -1.
    void string_last_index_of(rtlib_context_t & ctx)
    {
        struct args_t { rstring_t s; rstring_t value; rint32_t end; };
        args_t * args = ctx.args<args_t>();
        const rt_char_t * s = *args->s, * value = *args->value;
        size_t size = __length_of(s), count = __length_of(value);
        int32_t end = *args->end;

        if (end < 0 || (size_t)end > size || count > (size_t)end)
        {
            ctx.return_<int32_t>(-1);
            return;
        }

        for (size_t k = (size_t)end - count + 1; k > 0; k--)
        {
            if (count == 0 || std::memcmp(s + k - 1, value, count * sizeof(rt_char_t)) == 0)
            {
                ctx.return_<int32_t>((int32_t)(k - 1));
                return;
            }
        }

        ctx.return_<int32_t>(-1);
    }

    ////////// ////////// ////////// ////////// //////////
    // Characters. The classes and the cases are the same on every platform (they do not depend on the
    // locale or on the C library): ASCII, Latin-1, Latin Extended-A, Greek, Cyrillic, and the common
    // blocks of letters (CJK, kana, hangul) are known, a surrogate is none of them.

    static bool __is_letter(uint32_t c)
    {
        if (c < 0x80)
            return (c | 0x20) >= 'a' && (c | 0x20) <= 'z';

        if (c < 0x100)
            return c == 0xAA || c == 0xB5 || c == 0xBA || (c >= 0xC0 && c != 0xD7 && c != 0xF7);

        return (c >= 0x100 && c <= 0x2AF)
            || (c >= 0x370 && c <= 0x3FF && c != 0x375 && c != 0x37E && c != 0x384 && c != 0x385 && c != 0x387)
            || (c >= 0x400 && c <= 0x481) || (c >= 0x48A && c <= 0x52F)
            || (c >= 0x531 && c <= 0x556) || (c >= 0x561 && c <= 0x587)
            || (c >= 0x5D0 && c <= 0x5EA) || (c >= 0x620 && c <= 0x64A)
            || (c >= 0x1E00 && c <= 0x1FFF)
            || (c >= 0x3041 && c <= 0x3096) || (c >= 0x30A1 && c <= 0x30FA)
            || (c >= 0x3400 && c <= 0x4DBF) || (c >= 0x4E00 && c <= 0x9FFF)
            || (c >= 0xAC00 && c <= 0xD7A3) || (c >= 0xFF21 && c <= 0xFF3A) || (c >= 0xFF41 && c <= 0xFF5A);
    }

    static bool __is_digit(uint32_t c)
    {
        return (c >= '0' && c <= '9') || (c >= 0x660 && c <= 0x669) || (c >= 0xFF10 && c <= 0xFF19);
    }

    static bool __is_white_space(uint32_t c)
    {
        return (c >= 9 && c <= 13) || c == 32 || c == 0x85 || c == 0xA0 || c == 0x1680
            || (c >= 0x2000 && c <= 0x200A) || c == 0x2028 || c == 0x2029 || c == 0x202F
            || c == 0x205F || c == 0x3000;
    }

    static rt_char_t __to_upper(rt_char_t c)
    {
        if (c < 0x80)
            return c >= 'a' && c <= 'z'? c - 0x20 : c;

        if (c < 0x100)
        {
            if (c == 0xB5)  return 0x39C;
            if (c == 0xFF)  return 0x178;
            return c >= 0xE0 && c != 0xF7? c - 0x20 : c;
        }

        if ((c >= 0x100 && c <= 0x137) || (c >= 0x14A && c <= 0x177))
            return (c & 1) && c != 0x131? c - 1 : c;                // pairs, the lower one is odd

        if ((c >= 0x139 && c <= 0x148) || (c >= 0x179 && c <= 0x17E))
            return !(c & 1)? c - 1 : c;                             // pairs, the lower one is even

        if (c >= 0x3B1 && c <= 0x3C9 && c != 0x3C2)    return c - 0x20;
        if (c == 0x3C2)                                return 0x3A3;
        if (c >= 0x3AC && c <= 0x3AF)                  return c == 0x3AC? 0x386 : c - 0x25;
        if (c >= 0x430 && c <= 0x44F)                  return c - 0x20;
        if (c >= 0x450 && c <= 0x45F)                  return c - 0x50;
        if (c >= 0x460 && c <= 0x481)                  return (c & 1)? c - 1 : c;
        if (c >= 0x48A && c <= 0x4BF)                  return (c & 1)? c - 1 : c;
        if (c >= 0x561 && c <= 0x586)                  return c - 0x30;
        if (c >= 0x1E00 && c <= 0x1EFF)                return (c & 1)? c - 1 : c;
        if (c >= 0xFF41 && c <= 0xFF5A)                return c - 0x20;

        return c;
    }

    static rt_char_t __to_lower(rt_char_t c)
    {
        if (c < 0x80)
            return c >= 'A' && c <= 'Z'? c + 0x20 : c;

        if (c < 0x100)
            return c >= 0xC0 && c <= 0xDE && c != 0xD7? c + 0x20 : c;

        if (c == 0x178)                                 return 0xFF;
        if ((c >= 0x100 && c <= 0x137) || (c >= 0x14A && c <= 0x177))
            return !(c & 1) && c != 0x130? c + 1 : c;

        if ((c >= 0x139 && c <= 0x148) || (c >= 0x179 && c <= 0x17E))
            return (c & 1)? c + 1 : c;

        if (c >= 0x391 && c <= 0x3A9 && c != 0x3A2)    return c + 0x20;
        if (c == 0x386)                                return 0x3AC;
        if (c >= 0x388 && c <= 0x38A)                  return c + 0x25;
        if (c >= 0x410 && c <= 0x42F)                  return c + 0x20;
        if (c >= 0x400 && c <= 0x40F)                  return c + 0x50;
        if (c >= 0x460 && c <= 0x481)                  return (c & 1)? c : c + 1;
        if (c >= 0x48A && c <= 0x4BF)                  return (c & 1)? c : c + 1;
        if (c >= 0x531 && c <= 0x556)                  return c + 0x30;
        if (c >= 0x1E00 && c <= 0x1EFF)                return (c & 1)? c : c + 1;
        if (c >= 0xFF21 && c <= 0xFF3A)                return c + 0x20;

        return c;
    }

    struct __char_args_t
    {
        rchar_t value;
    };

    // Char_IsLetter, Char_IsDigit, Char_IsWhiteSpace
    void char_is_letter(rtlib_context_t & ctx)
    {
        ctx.return_<bool>(__is_letter(*ctx.args<__char_args_t>()->value));
    }

    void char_is_digit(rtlib_context_t & ctx)
    {
        ctx.return_<bool>(__is_digit(*ctx.args<__char_args_t>()->value));
    }

    void char_is_white_space(rtlib_context_t & ctx)
    {
        ctx.return_<bool>(__is_white_space(*ctx.args<__char_args_t>()->value));
    }

    // Char_ToUpper, Char_ToLower
    void char_to_upper(rtlib_context_t & ctx)
    {
        ctx.return_<rt_char_t>(__to_upper(*ctx.args<__char_args_t>()->value));
    }

    void char_to_lower(rtlib_context_t & ctx)
    {
        ctx.return_<rt_char_t>(__to_lower(*ctx.args<__char_args_t>()->value));
    }

    // String_IndexOfChar: the first position of the character at or after the start (the start is
    // checked by the caller), or -1.
    void string_index_of_char(rtlib_context_t & ctx)
    {
        struct args_t { rstring_t s; rchar_t value; rint32_t start; };
        args_t * args = ctx.args<args_t>();
        const rt_char_t * s = *args->s;
        rt_char_t value = *args->value;
        int32_t start = *args->start;

        size_t size = __length_of(s);
        for (size_t k = start < 0? 0 : (size_t)start; k < size; k++)
        {
            if (s[k] == value)
            {
                ctx.return_<int32_t>((int32_t)k);
                return;
            }
        }

        ctx.return_<int32_t>(-1);
    }

    // String_LastIndexOfChar: the last position of the character, or -1.
    void string_last_index_of_char(rtlib_context_t & ctx)
    {
        struct args_t { rstring_t s; rchar_t value; };
        args_t * args = ctx.args<args_t>();
        const rt_char_t * s = *args->s;
        rt_char_t value = *args->value;

        for (size_t k = __length_of(s); k > 0; k--)
        {
            if (s[k - 1] == value)
            {
                ctx.return_<int32_t>((int32_t)(k - 1));
                return;
            }
        }

        ctx.return_<int32_t>(-1);
    }

    // String_Trim: the string without the white spaces at the start (sides & 1) and at the end
    // (sides & 2). A string that has none of them is returned as it is.
    void string_trim(rtlib_context_t & ctx)
    {
        struct args_t { rstring_t s; rint32_t sides; };
        args_t * args = ctx.args<args_t>();
        const rt_char_t * s = *args->s;
        int32_t sides = *args->sides;

        size_t begin = 0, end = __length_of(s);

        if (sides & 1)
        {
            while (begin < end && __is_white_space(s[begin]))
                begin++;
        }

        if (sides & 2)
        {
            while (end > begin && __is_white_space(s[end - 1]))
                end--;
        }

        if (begin == 0 && end == __length_of(s))
            ctx.return_<const rt_char_t *>(s);
        else
            ctx.return_<const rt_char_t *>(__new_string(s + begin, end - begin));
    }

    // String_IsBlank: whether the string has no characters, or only white spaces.
    void string_is_blank(rtlib_context_t & ctx)
    {
        const rt_char_t * s = *ctx.args<__string1_args_t>()->s;

        for (size_t k = 0, size = __length_of(s); k < size; k++)
        {
            if (!__is_white_space(s[k]))
            {
                ctx.return_<bool>(false);
                return;
            }
        }

        ctx.return_<bool>(true);
    }

    ////////// ////////// ////////// ////////// //////////
    // UTF-8 (System.Text.UTF8Encoding). A character that cannot be coded (a surrogate that has no pair) is
    // U+FFFD, and so are the bytes that are not UTF-8: a sequence that is too long, a code point that is
    // larger than U+10FFFF or a surrogate, and a sequence that is not complete.

    // The number of bytes of a character: its units are s[k] (and s[k + 1] if it is a pair).
    static size_t __utf8_size_of(const rt_char_t * s, size_t length, size_t k, size_t * units)
    {
        uint32_t c = (uint32_t)s[k];
        *units = 1;

        if (c < 0x80)
            return 1;

        if (c < 0x800)
            return 2;

        if (c >= 0xD800 && c <= 0xDBFF && k + 1 < length && (uint32_t)s[k + 1] >= 0xDC00 && (uint32_t)s[k + 1] <= 0xDFFF)
        {
            *units = 2;
            return 4;
        }

        return 3;       // The others, and U+FFFD for a surrogate that has no pair.
    }

    // Utf8_GetByteCount: the number of bytes of the string.
    void utf8_get_byte_count(rtlib_context_t & ctx)
    {
        const rt_char_t * s = *ctx.args<__string1_args_t>()->s;
        size_t length = __length_of(s), total = 0;

        for (size_t k = 0, units; k < length; k += units)
            total += __utf8_size_of(s, length, k, &units);

        ctx.return_<int32_t>((int32_t)total);
    }

    // Utf8_GetBytes: writes the bytes of the string at the index of the array (the room is checked by the
    // caller), returns the number of bytes.
    void utf8_get_bytes(rtlib_context_t & ctx)
    {
        struct args_t { rstring_t s; robject_t bytes; rint32_t index; };
        args_t * args = ctx.args<args_t>();
        const rt_char_t * s = *args->s;
        uint8_t * out = (uint8_t *)(void *)(rt_ref_t)args->bytes + *args->index;
        size_t length = __length_of(s), written = 0;

        for (size_t k = 0, units; k < length; k += units)
        {
            size_t size = __utf8_size_of(s, length, k, &units);
            uint32_t c = (uint32_t)s[k];

            if (size == 4)
                c = 0x10000 + ((c - 0xD800) << 10) + ((uint32_t)s[k + 1] - 0xDC00);
            else if (size == 3 && c >= 0xD800 && c <= 0xDFFF)
                c = 0xFFFD;

            switch (size)
            {
                case 1:
                    out[written++] = (uint8_t)c;
                    break;

                case 2:
                    out[written++] = (uint8_t)(0xC0 | (c >> 6));
                    out[written++] = (uint8_t)(0x80 | (c & 0x3F));
                    break;

                case 3:
                    out[written++] = (uint8_t)(0xE0 | (c >> 12));
                    out[written++] = (uint8_t)(0x80 | ((c >> 6) & 0x3F));
                    out[written++] = (uint8_t)(0x80 | (c & 0x3F));
                    break;

                default:
                    out[written++] = (uint8_t)(0xF0 | (c >> 18));
                    out[written++] = (uint8_t)(0x80 | ((c >> 12) & 0x3F));
                    out[written++] = (uint8_t)(0x80 | ((c >> 6) & 0x3F));
                    out[written++] = (uint8_t)(0x80 | (c & 0x3F));
                    break;
            }
        }

        ctx.return_<int32_t>((int32_t)written);
    }

    // Utf8_GetString: the string of the bytes of the array (the range is checked by the caller).
    void utf8_get_string(rtlib_context_t & ctx)
    {
        struct args_t { robject_t bytes; rint32_t index; rint32_t count; };
        args_t * args = ctx.args<args_t>();
        const uint8_t * p = (const uint8_t *)(void *)(rt_ref_t)args->bytes;
        int32_t index = *args->index, count = *args->count;

        std::basic_string<rt_char_t> text;
        if (p != nullptr && index >= 0 && count > 0)
        {
            p += index;
            text.reserve((size_t)count);

            for (size_t k = 0, n = (size_t)count; k < n; )
            {
                uint32_t b = p[k], c, min;
                size_t need;

                if (b < 0x80)               { text.push_back((rt_char_t)b); k++; continue; }
                else if (b >= 0xC2 && b <= 0xDF)    { need = 1; c = b & 0x1F; min = 0x80; }
                else if (b >= 0xE0 && b <= 0xEF)    { need = 2; c = b & 0x0F; min = 0x800; }
                else if (b >= 0xF0 && b <= 0xF4)    { need = 3; c = b & 0x07; min = 0x10000; }
                else                        { text.push_back((rt_char_t)0xFFFD); k++; continue; }

                size_t used = 1;
                while (used <= need && k + used < n && (p[k + used] & 0xC0) == 0x80)
                {
                    c = (c << 6) | (p[k + used] & 0x3F);
                    used++;
                }

                if (used <= need)                   // Not complete: the bytes that are good are one error.
                {
                    text.push_back((rt_char_t)0xFFFD);
                    k += used;
                    continue;
                }

                if (c < min || c > 0x10FFFF || (c >= 0xD800 && c <= 0xDFFF))
                {
                    text.push_back((rt_char_t)0xFFFD);
                    k++;
                    continue;
                }

                k += used;

                if (c >= 0x10000)
                {
                    c -= 0x10000;
                    text.push_back((rt_char_t)(0xD800 + (c >> 10)));
                    text.push_back((rt_char_t)(0xDC00 + (c & 0x3FF)));
                }
                else
                {
                    text.push_back((rt_char_t)c);
                }
            }
        }

        ctx.return_<const rt_char_t *>(__new_string(text.c_str(), text.size()));
    }

    // String_ToUpper, String_ToLower
    void string_to_upper(rtlib_context_t & ctx)
    {
        const rt_char_t * s = *ctx.args<__string1_args_t>()->s;
        size_t length = __length_of(s);
        ustring_t text(s == nullptr? u"" : s, length);

        for (rt_char_t & c : text)
            c = __to_upper(c);

        ctx.return_<const rt_char_t *>(__new_string(text.c_str(), length));
    }

    void string_to_lower(rtlib_context_t & ctx)
    {
        const rt_char_t * s = *ctx.args<__string1_args_t>()->s;
        size_t length = __length_of(s);
        ustring_t text(s == nullptr? u"" : s, length);

        for (rt_char_t & c : text)
            c = __to_lower(c);

        ctx.return_<const rt_char_t *>(__new_string(text.c_str(), length));
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
        ctx.return_<const rt_char_t *>(__new_string(
            std::to_wstring(*ctx.args<__number_args_t<rint64_t>>()->value)
        ));
    }

    // UInt64_ToString
    void uint64_to_string(rtlib_context_t & ctx)
    {
        ctx.return_<const rt_char_t *>(__new_string(
            std::to_wstring(*ctx.args<__number_args_t<ruint64_t>>()->value)
        ));
    }

    // Double_ToString
    void double_to_string(rtlib_context_t & ctx)
    {
        ctx.return_<const rt_char_t *>(__new_string(
            format_floating(*ctx.args<__number_args_t<rdouble_t>>()->value)
        ));
    }

    // Single_ToString
    void float_to_string(rtlib_context_t & ctx)
    {
        ctx.return_<const rt_char_t *>(__new_string(
            format_floating(*ctx.args<__number_args_t<rfloat_t>>()->value)
        ));
    }

    // The text of a floating point number with a number of digits (std::to_chars: the exact value is rounded, whatever the
    // locale is).
    static std::wstring __floating_text(double value, std::chars_format format, int digits)
    {
        char buffer[512];

        if (digits < 0)
            digits = 0;

        if (digits > 99)
            digits = 99;

        auto result = std::to_chars(buffer, buffer + sizeof(buffer), value, format, digits);
        return std::wstring(buffer, result.ptr);
    }

    struct __floating_digits_args_t
    {
        rdouble_t   value;
        rint32_t    digits;
    };

    // Double_ToFixed: 1234.5 with 2 digits is 1234.50.
    void double_to_fixed(rtlib_context_t & ctx)
    {
        __floating_digits_args_t * args = ctx.args<__floating_digits_args_t>();
        ctx.return_<const rt_char_t *>(__new_string(__floating_text(*args->value, std::chars_format::fixed, *args->digits)));
    }

    // Double_ToScientific: 1234.5 with 3 digits is 1.234e+03 (the exponent has at least 2 digits).
    void double_to_scientific(rtlib_context_t & ctx)
    {
        __floating_digits_args_t * args = ctx.args<__floating_digits_args_t>();
        ctx.return_<const rt_char_t *>(__new_string(__floating_text(*args->value, std::chars_format::scientific, *args->digits)));
    }

    // Char_ToString
    void char_to_string(rtlib_context_t & ctx)
    {
        rt_char_t c = *ctx.args<__number_args_t<rchar_t>>()->value;
        ctx.return_<const rt_char_t *>(__new_string(&c, 1));
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::rtlib
