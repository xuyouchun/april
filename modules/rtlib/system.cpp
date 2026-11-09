#include <rtlib.h>
#include <charconv>

namespace X_ROOT_NS::modules::rtlib {

    ////////// ////////// ////////// ////////// //////////

    // Formats a floating point number like .NET: the shortest text that reads back to the
    // same number, in scientific notation when the exponent is 15 or more or below -5.
    template<typename _float_t> static std::wstring __format_floating(_float_t value)
    {
        if (value != value)
            return L"NaN";

        if (value == std::numeric_limits<_float_t>::infinity())
            return L"Infinity";

        if (value == -std::numeric_limits<_float_t>::infinity())
            return L"-Infinity";

        bool negative = std::signbit(value);
        if (negative)
            value = -value;

        if (value == 0)
            return negative? L"-0" : L"0";

        // The shortest scientific form: d.ddde+XX
        char buffer[64];
        auto r = std::to_chars(buffer, buffer + sizeof(buffer), value,
                                                    std::chars_format::scientific);
        std::string text(buffer, r.ptr);

        size_t e_pos = text.find('e');
        std::string mantissa = text.substr(0, e_pos);
        int exponent = std::atoi(text.c_str() + e_pos + 1);

        std::string digits;
        for (char c : mantissa)
        {
            if (c != '.')
                digits += c;
        }

        std::string result;
        if (exponent >= 15 || exponent < -5)
        {
            result = digits.substr(0, 1);
            if (digits.size() > 1)
                result += "." + digits.substr(1);

            char exp_text[16];
            std::snprintf(exp_text, sizeof(exp_text), "E%c%02d",
                                        exponent < 0? '-' : '+', std::abs(exponent));
            result += exp_text;
        }
        else if (exponent >= 0)
        {
            size_t int_count = (size_t)exponent + 1;
            if (digits.size() <= int_count)
            {
                result = digits + std::string(int_count - digits.size(), '0');
            }
            else
            {
                result = digits.substr(0, int_count) + "." + digits.substr(int_count);
            }
        }
        else
        {
            result = "0." + std::string((size_t)(-exponent - 1), '0') + digits;
        }

        return std::wstring(negative? L"-" : L"") + std::wstring(result.begin(), result.end());
    }

    std::wstring format_floating(double value) { return __format_floating(value); }
    std::wstring format_floating(float value)  { return __format_floating(value); }

    // Write.
    template<typename _t> struct __write_args_t
    {
        _t  value;
    };

    // Writes a value: floating point numbers are formatted like .NET.
    template<typename _t, typename _v_t> static void __write_value(_v_t v)
    {
        if constexpr (std::is_floating_point<_v_t>::value)
            std::wcout << __format_floating(v);
        else if constexpr (std::is_same<_v_t, rt_char_t>::value)
            std::wcout << to_host_string(&v, 1);
        else
            std::wcout << v;
    }

    template<typename _t> void __console_write(rtlib_context_t & ctx)
    {
        typedef __write_args_t<_t> args_t;

        args_t * arg = ctx.args<args_t>();
        __write_value<_t>(*arg->value);
    }

    template<typename _t> void __console_write_line(rtlib_context_t & ctx)
    {
        typedef __write_args_t<_t> args_t;

        args_t * arg = ctx.args<args_t>();
        __write_value<_t>(*arg->value);
        std::wcout << std::endl;
    }


    ////////// ////////// ////////// ////////// //////////

    void console_write_string(rtlib_context_t & ctx)
    {
        typedef __write_args_t<rstring_t> args_t;

        args_t * arg = ctx.args<args_t>();
        const rt_char_t * s = *arg->value;

        if (s != nullptr)
            std::wcout << to_host_string(s, rt_string_length(s));
    }

    void console_write_string_line(rtlib_context_t & ctx)
    {
        typedef __write_args_t<rstring_t> args_t;

        args_t * arg = ctx.args<args_t>();
        const rt_char_t * s = *arg->value;

        if (s != nullptr)
            std::wcout << to_host_string(s, rt_string_length(s)) << std::endl;
    }

    void console_write_char(rtlib_context_t & ctx)
    {
        __console_write<rchar_t>(ctx);
    }

    void console_write_char_line(rtlib_context_t & ctx)
    {
        __console_write_line<rchar_t>(ctx);
    }

    void console_write_int8(rtlib_context_t & ctx)
    {
        __console_write<rint8_t>(ctx);
    }

    void console_write_int8_line(rtlib_context_t & ctx)
    {
        __console_write_line<rint8_t>(ctx);
    }

    void console_write_uint8(rtlib_context_t & ctx)
    {
        __console_write<ruint8_t>(ctx);
    }

    void console_write_uint8_line(rtlib_context_t & ctx)
    {
        __console_write_line<ruint8_t>(ctx);
    }

    void console_write_int16(rtlib_context_t & ctx)
    {
        __console_write<rint16_t>(ctx);
    }

    void console_write_int16_line(rtlib_context_t & ctx)
    {
        __console_write_line<rint16_t>(ctx);
    }

    void console_write_uint16(rtlib_context_t & ctx)
    {
        __console_write<ruint16_t>(ctx);
    }

    void console_write_uint16_line(rtlib_context_t & ctx)
    {
        __console_write_line<ruint16_t>(ctx);
    }

    void console_write_int32(rtlib_context_t & ctx)
    {
        __console_write<rint32_t>(ctx);
    }

    void console_write_int32_line(rtlib_context_t & ctx)
    {
        __console_write_line<rint32_t>(ctx);
    }

    void console_write_uint32(rtlib_context_t & ctx)
    {
        __console_write<ruint32_t>(ctx);
    }

    void console_write_uint32_line(rtlib_context_t & ctx)
    {
        __console_write_line<ruint32_t>(ctx);
    }

    void console_write_int64(rtlib_context_t & ctx)
    {
        __console_write<rint64_t>(ctx);
    }

    void console_write_int64_line(rtlib_context_t & ctx)
    {
        __console_write_line<rint64_t>(ctx);
    }

    void console_write_uint64(rtlib_context_t & ctx)
    {
        __console_write<ruint64_t>(ctx);
    }

    void console_write_uint64_line(rtlib_context_t & ctx)
    {
        __console_write<ruint64_t>(ctx);
    }

    void console_write_float(rtlib_context_t & ctx)
    {
        __console_write<rfloat_t>(ctx);
    }

    void console_write_float_line(rtlib_context_t & ctx)
    {
        __console_write_line<rfloat_t>(ctx);
    }

    void console_write_double(rtlib_context_t & ctx)
    {
        __console_write<rdouble_t>(ctx);
    }

    void console_write_double_line(rtlib_context_t & ctx)
    {
        __console_write_line<rdouble_t>(ctx);
    }

    ////////// ////////// ////////// ////////// //////////
    // Resources (System.Runtime.Resource): the bytes that are embedded in the assemblies.

    static resource_finder_t __resource_finder = nullptr;

    // Sets the function that finds the resources of a program that has no assemblies.
    void set_resource_finder(resource_finder_t finder)
    {
        __resource_finder = finder;
    }

    // Finds the resource of the name in the assemblies that are loaded.
    static bool __find_resource(rtlib_context_t & ctx, const rt_char_t * name, const byte_t ** bytes, size_t * length)
    {
        string_t host_name = name == nullptr? string_t() : to_host_string(name, rt_string_length(name));

        if (__resource_finder != nullptr)
        {
            uint64_t length64 = 0;
            bool found = __resource_finder(host_name, bytes, &length64);

            *length = (size_t)length64;
            return found;
        }

        analyzer_env_t & env = ctx.exec_env != nullptr? *ctx.exec_env : ctx.analyzer.env;
        return env.assemblies.find_resource(host_name, bytes, length);
    }

    // Resource_GetSize: the number of bytes of the resource, -1 when there is none of the name.
    void resource_get_size(rtlib_context_t & ctx)
    {
        struct args_t { rstring_t name; };
        args_t * args = ctx.args<args_t>();

        const byte_t * bytes;
        size_t length;

        ctx.return_<int32_t>(__find_resource(ctx, *args->name, &bytes, &length)? (int32_t)length : -1);
    }

    // Resource_GetPtr: the address of the bytes of the resource, null when there is none of the name.
    void resource_get_ptr(rtlib_context_t & ctx)
    {
        struct args_t { rstring_t name; };
        args_t * args = ctx.args<args_t>();

        const byte_t * bytes = nullptr;
        size_t length = 0;

        ctx.return_<void *>(__find_resource(ctx, *args->name, &bytes, &length)? (void *)bytes : nullptr);
    }

    // Resource_Read: copies the bytes of the resource to the array (that has the size of them, the caller made it).
    void resource_read(rtlib_context_t & ctx)
    {
        struct args_t { rstring_t name; robject_t destination; };
        args_t * args = ctx.args<args_t>();

        const byte_t * bytes;
        size_t length;

        uint8_t * destination = (uint8_t *)(void *)(rt_ref_t)args->destination;
        if (destination != nullptr && __find_resource(ctx, *args->name, &bytes, &length) && length > 0)
            std::memcpy(destination, bytes, length);
    }

    ////////// ////////// ////////// ////////// //////////

    // Ptr_FromInt64, PtrT_FromInt64: the pointer of an address.
    void ptr_from_int64(rtlib_context_t & ctx)
    {
        struct args_t { rint64_t value; };

        ctx.return_<void *>((void *)(intptr_t)*ctx.args<args_t>()->value);
    }

    // Memory_Alloc: bytes of the memory of the program (not collected), null when there is not enough memory.
    void memory_alloc(rtlib_context_t & ctx)
    {
        struct args_t { rint64_t bytes; };

        int64_t bytes = *ctx.args<args_t>()->bytes;
        ctx.return_<void *>(bytes < 0? nullptr : std::malloc(bytes == 0? 1 : (size_t)bytes));
    }

    // Memory_Free: gives the memory back.
    void memory_free(rtlib_context_t & ctx)
    {
        struct args_t { rptr_t ptr; };

        std::free(*ctx.args<args_t>()->ptr);
    }

    // Memory_Copy: the bytes of the source are copied to the destination (the ranges may overlap).
    void memory_copy(rtlib_context_t & ctx)
    {
        struct args_t { rptr_t destination; rptr_t source; rint64_t bytes; };
        args_t * args = ctx.args<args_t>();

        int64_t bytes = *args->bytes;
        if (bytes > 0)
            std::memmove(*args->destination, *args->source, (size_t)bytes);
    }

    // Memory_Fill: the bytes are set to the value.
    void memory_fill(rtlib_context_t & ctx)
    {
        struct args_t { rptr_t destination; ruint8_t value; rint64_t bytes; };
        args_t * args = ctx.args<args_t>();

        int64_t bytes = *args->bytes;
        if (bytes > 0)
            std::memset(*args->destination, (int)*args->value, (size_t)bytes);
    }

    // String_AddressOf: the address of the character of the index; the index is checked by the caller.
    void string_address_of(rtlib_context_t & ctx)
    {
        struct args_t { rstring_t s; rint32_t index; };
        args_t * args = ctx.args<args_t>();

        const rt_char_t * s = *args->s;
        ctx.return_<void *>(s == nullptr? nullptr : (void *)(s + *args->index));
    }

    // Ptr_ToInt64, PtrT_ToInt64: the address of a pointer.
    void ptr_to_int64(rtlib_context_t & ctx)
    {
        struct args_t { rptr_t value; };

        ctx.return_<int64_t>((int64_t)(intptr_t)*ctx.args<args_t>()->value);
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::rtlib


