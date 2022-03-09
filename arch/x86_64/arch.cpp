
#include <arch.h>
#include <execinfo.h>
#include <common.h>
#include <algorithm.h>

namespace X_ROOT_NS::arch {

    ////////// ////////// ////////// ////////// //////////

    // Prints current execute stack.
    X_NO_INLINE void print_call_stack(int trim)
    {
        void * buffer[1024];
        int nptrs = backtrace(buffer, sizeof(buffer));

        backtrace_symbols_fd(buffer + trim, nptrs - trim, STDOUT_FILENO);
    }

    // Returns a string of current stack.
    X_NO_INLINE string_t call_stack(int trim)
    {
        _A(trim >= 0);

        void * buffer[1024];
        int nptrs = backtrace(buffer, sizeof(buffer));

        if (nptrs <= trim)
            return _T("");

        char ** strings = backtrace_symbols(buffer + trim, nptrs - trim);

        if (strings != nullptr)
        {
            std::basic_ostringstream<char> ss;

            for (int k = 0; k < nptrs - trim; k++)
            {
                if (k > 0)
                    ss << std::endl;

                ss << strings[k];
            }

            free(strings);

            return string_convert<char, char_t>(ss.str());
        }

        return _T("");
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::arch

