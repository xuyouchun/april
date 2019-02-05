
#include <arch.h>
#include <iostream>

namespace X_ROOT_NS { namespace arch { namespace types {

    istream_t & cin  = std::wcin;       // Data input stream
    ostream_t & cout = std::wcout;      // Data output stream
    ostream_t & cerr = std::wcerr;      // Error output stream
    ostream_t & clog = std::wclog;      // Log output stream

} } }  // namespace X_ROOT_NS::arch::types

