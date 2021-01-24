#include <rtlib.h>

namespace X_ROOT_NS { namespace modules { namespace rtlib {

    ////////// ////////// ////////// ////////// //////////

    // Write.
	template<typename _t> struct __write_args_t
    {
        _t	value;
    };

	template<typename _t> void __console_write(rtlib_context_t & ctx)
	{
		typedef __write_args_t<_t> args_t;

        args_t * arg = ctx.args<args_t>();
		std::wcout << *arg->value;
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

    void console_write_char(rtlib_context_t & ctx)
    {
		__console_write<rchar_t>(ctx);
    }

    void console_write_int8(rtlib_context_t & ctx)
    {
		__console_write<rint8_t>(ctx);
    }

    void console_write_uint8(rtlib_context_t & ctx)
    {
		__console_write<ruint8_t>(ctx);
    }

    void console_write_int16(rtlib_context_t & ctx)
    {
		__console_write<rint16_t>(ctx);
    }

    void console_write_uint16(rtlib_context_t & ctx)
    {
		__console_write<ruint16_t>(ctx);
    }

    void console_write_int32(rtlib_context_t & ctx)
    {
		__console_write<rint32_t>(ctx);
    }

    void console_write_uint32(rtlib_context_t & ctx)
    {
		__console_write<ruint32_t>(ctx);
    }

    void console_write_int64(rtlib_context_t & ctx)
    {
		__console_write<rint64_t>(ctx);
    }

    void console_write_uint64(rtlib_context_t & ctx)
    {
		__console_write<ruint64_t>(ctx);
    }

    void console_write_float(rtlib_context_t & ctx)
    {
		__console_write<rfloat_t>(ctx);
    }

    void console_write_double(rtlib_context_t & ctx)
    {
		__console_write<rdouble_t>(ctx);
    }

    ////////// ////////// ////////// ////////// //////////

} } }


