#include <rtlib.h>

namespace X_ROOT_NS { namespace modules { namespace rtlib {

    ////////// ////////// ////////// ////////// //////////

    // Write.
    struct __write_args_t
    {
        rstring_t s;
    };

    void console_write(rtlib_context_t & ctx)
    {
        __write_args_t * arg = ctx.args<__write_args_t>();

		if (*arg->s != nullptr)
	        std::wcout << *arg->s;
    }

    ////////// ////////// ////////// ////////// //////////

} } }


