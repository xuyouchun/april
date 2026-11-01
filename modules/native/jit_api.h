#ifndef __NATIVE_JIT_API_H__
#define __NATIVE_JIT_API_H__

// The C interface between april and its JIT back end: a shared library (april_jit.dll,
// libapril_jit.so) that is linked with LLVM, and that april loads when it is there. The
// program that is run is the LLVM IR of the native back end: it is compiled to an object
// file (kept in the directory of the builds), the object is run in the process of april by
// the JIT of LLVM, the symbols of the runtime of the native programs are those of april.
//
// Only C types cross the boundary: the library and april do not share a C++ runtime.

#include <stddef.h>

#ifdef _WIN32
    #define APRIL_JIT_EXPORT    __declspec(dllexport)
#else
    #define APRIL_JIT_EXPORT    __attribute__((visibility("default")))
#endif

#define APRIL_JIT_API_VERSION   1

#ifdef __cplusplus
extern "C" {
#endif

    // A symbol of april that the programs use.
    typedef struct april_jit_symbol_t
    {
        const char *    name;
        void *          address;
    } april_jit_symbol_t;

    // The version of this interface.
    APRIL_JIT_EXPORT int april_jit_version(void);

    // The version of LLVM, e.g. "23.1.4".
    APRIL_JIT_EXPORT const char * april_jit_llvm_version(void);

    // Compiles the IR (text, `length` bytes) to an object file of the host, optimized at
    // level 0 ~ 3. Returns 0 if it succeeded; else the message is written to `error`.
    APRIL_JIT_EXPORT int april_jit_compile(const char * ir, size_t length, int optimize,
                    const char * object_path, char * error, size_t error_size);

    // Runs the `main` of the object file, with the symbols of april. Returns 0 if it was
    // run, and then *exit_code is the value that `main` returned; else the message is
    // written to `error`.
    APRIL_JIT_EXPORT int april_jit_run(const char * object_path,
                    const april_jit_symbol_t * symbols, size_t symbol_count,
                    int argc, char ** argv, int * exit_code, char * error, size_t error_size);

#ifdef __cplusplus
}
#endif

#endif  // __NATIVE_JIT_API_H__
