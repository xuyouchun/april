
#include <arch.h>
#include <execinfo.h>

namespace X_ROOT_NS { namespace arch {

    ////////// ////////// ////////// ////////// //////////

    // Prints current execute stack.
    X_NO_INLINE void print_stack()
    {
        void *buffer[1024];
        char **strings;
        int nptrs = backtrace(buffer, sizeof(buffer));
        backtrace_symbols_fd(buffer + 1, nptrs - 1, STDOUT_FILENO);
    }

    ////////// ////////// ////////// ////////// //////////

} }  // namespace X_ROOT_NS::arch
