
#ifndef __NATIVE_H__
#define __NATIVE_H__

#include <core.h>
#include <rt.h>

// The clang driver (build / run the executable); standard library only.
#include "../../modules/native/driver.h"

// The native back end: generates LLVM IR (textual .ll) from the XIL of a program, which
// clang / llc turn into a native executable, linked with the native runtime support
// (modules/native/native_rt.cpp) and the runtime libraries.
//
// Milestone M1 supports a small subset (see docs/aot-llvm-feasibility.md): static
// methods, primitive and string values, locals and arguments, static calls and calls
// of the runtime library functions (Console, Math ...). Everything else is rejected
// with a clear error that names what is not supported yet.

namespace X_ROOT_NS::modules::native {

    namespace
    {
        using namespace core;
        using namespace rt;
    }

    ////////// ////////// ////////// ////////// //////////

    // Native back end error codes.
    X_ENUM(native_error_code_t)

        // The xil is not supported yet.
        unsupported_xil,

        // The type is not supported yet.
        unsupported_type,

        // The method is not supported yet.
        unsupported_method,

        // The program has no entry point.
        no_entry_point,

        // The runtime library function is not available to native programs.
        unsupported_libfunc,

        // Internal error: the evaluation stack is inconsistent.
        bad_stack,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    // Generates the LLVM IR of the program: the main assembly of the context
    // and everything reachable from its entry point.
    // Throws logic_error_t<native_error_code_t> for what is not supported yet.
    std::string generate_ir(rt_context_t & ctx);

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::native

#endif  // __NATIVE_H__
