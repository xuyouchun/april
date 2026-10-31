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
        const char_t * s = *arg->value;

        if (s != nullptr)
            std::wcout << s;
    }

    void console_write_string_line(rtlib_context_t & ctx)
    {
        typedef __write_args_t<rstring_t> args_t;

        args_t * arg = ctx.args<args_t>();
        const char_t * s = *arg->value;

        if (s != nullptr)
            std::wcout << s << std::endl;
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

}   // namespace X_ROOT_NS::modules::rtlib


