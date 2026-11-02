#ifndef __NATIVE_JIT_HOST_H__
#define __NATIVE_JIT_HOST_H__

// The JIT back end, as april sees it: the shared library april_jit (see jit_api.h), that is
// loaded when it is there, with LLVM; if it is not, the programs are built with clang, or
// run with the interpreter. Standard library and the system only (no april headers).

#include <string>

#ifndef X_ROOT_NS
#define X_ROOT_NS april
#endif

namespace X_ROOT_NS::modules::native {

    ////////// ////////// ////////// ////////// //////////

    // Returns the path of the JIT library: $APRIL_JIT, or next to the april executable
    // (april_jit.dll, libapril_jit.so), or <april dir>/../lib. Empty if there is none.
    std::string jit_library_path();

    // The JIT back end.
    class jit_backend_t
    {
    public:

        // Loads the library, once. Returns null if there is none, or it cannot be loaded; the
        // reason is told.
        static jit_backend_t * load(std::string & reason);

        // The version of LLVM of the library.
        const std::string & llvm_version() const { return __llvm_version; }

        // Compiles the IR to an object file. Returns false, with the message, if it failed.
        bool compile(const std::string & ir, int optimize, const std::string & object_path,
                                                            std::string & error);

        // Runs the program of the object file in this process, with the runtime of the native
        // programs of april. Returns false, with the message, if it could not be run, else the
        // exit code is that of the program.
        bool run(const std::string & object_path, int & exit_code, std::string & error);

    private:
        jit_backend_t() = default;

        std::string __llvm_version;

        typedef int (*__compile_t)(const char *, size_t, int, const char *, char *, size_t);
        typedef int (*__run_t)(const char *, const void *, size_t, int, char **, int *,
                                                                    char *, size_t);

        __compile_t __compile = nullptr;
        __run_t     __run = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::native

#endif  // __NATIVE_JIT_HOST_H__
