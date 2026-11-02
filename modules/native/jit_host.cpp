#include "jit_host.h"
#include "jit_api.h"
#include "native_abi.h"
#include "driver.h"

#include <cstdlib>
#include <filesystem>
#include <vector>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

namespace X_ROOT_NS::modules::native {

    namespace fs = std::filesystem;

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
    #ifdef _WIN32
        const char __library_name[] = "april_jit.dll";
    #elif defined(__APPLE__)
        const char __library_name[] = "libapril_jit.dylib";
    #else
        const char __library_name[] = "libapril_jit.so";
    #endif

    #ifdef _WIN32
        // `main` of the programs of MinGW calls __main, that runs the constructors of the C++
        // objects of the program; they have none.
        void __no_constructors() { }
    #endif

        // The functions that the programs call: the runtime of the native programs.
        std::vector<april_jit_symbol_t> __symbols()
        {
            return {
            #ifdef _WIN32
                { "__main",                         (void *)&__no_constructors },
            #endif
                { APRIL_NATIVE_SYM_INIT,            (void *)&april_native_init },
                { APRIL_NATIVE_SYM_SHUTDOWN,        (void *)&april_native_shutdown },
                { APRIL_NATIVE_SYM_FIND_LIBFUNC,    (void *)&april_native_find_libfunc },
                { APRIL_NATIVE_SYM_INVOKE,          (void *)&april_native_invoke },
                { APRIL_NATIVE_SYM_NEW_OBJECT,      (void *)&april_native_new_object },
                { APRIL_NATIVE_SYM_FIND_IFACE,      (void *)&april_native_find_interface_function },
                { APRIL_NATIVE_SYM_IS_INSTANCE,     (void *)&april_native_is_instance },
                { APRIL_NATIVE_SYM_CAST,            (void *)&april_native_cast },
                { APRIL_NATIVE_SYM_UNHANDLED,       (void *)&april_native_unhandled_exception },
                { APRIL_NATIVE_SYM_ADD_ROOT,        (void *)&april_native_add_root },
                { APRIL_NATIVE_SYM_NEW_ARRAY,       (void *)&april_native_new_array },
                { APRIL_NATIVE_SYM_ARRAY_ELEMENT,   (void *)&april_native_array_element },
                { APRIL_NATIVE_SYM_ARRAY_TRY,       (void *)&april_native_array_try_element },
                { APRIL_NATIVE_SYM_ARRAY_LINEAR,    (void *)&april_native_array_linear_element },
                { APRIL_NATIVE_SYM_ARRAY_LENGTH,    (void *)&april_native_array_length },
                { APRIL_NATIVE_SYM_ARRAY_RANK,      (void *)&april_native_array_rank },
                { APRIL_NATIVE_SYM_ARRAY_LENGTH_OF, (void *)&april_native_array_length_of_dimension },
            };
        }

    #ifdef _WIN32
        // The message of the last error of the system.
        std::string __last_error()
        {
            DWORD code = GetLastError();
            char buffer[512];

            DWORD n = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr, code, 0, buffer, (DWORD)sizeof(buffer), nullptr);

            std::string message(buffer, n);
            while (!message.empty() && (message.back() == '\n' || message.back() == '\r'
                                                               || message.back() == ' '))
            {
                message.pop_back();
            }

            return message + " (" + std::to_string((unsigned long)code) + ")";
        }
    #endif
    }

    ////////// ////////// ////////// ////////// //////////

    // The path of the JIT library.
    std::string jit_library_path()
    {
        std::error_code ec;

        if (const char * env = std::getenv("APRIL_JIT"))
        {
            if (*env != 0)
                return fs::is_regular_file(env, ec)? std::string(env) : std::string();
        }

        std::string exe = executable_path();
        if (exe.empty())
            return std::string();

        fs::path dir = fs::path(exe).parent_path();
        for (const fs::path & c : { dir / __library_name, dir / ".." / "lib" / __library_name })
        {
            if (fs::is_regular_file(c, ec))
                return c.lexically_normal().string();
        }

        return std::string();
    }

    ////////// ////////// ////////// ////////// //////////

    // Loads the library.
    jit_backend_t * jit_backend_t::load(std::string & reason)
    {
        static jit_backend_t * backend = nullptr;
        static std::string failure;
        static bool tried = false;

        if (!tried)
        {
            tried = true;

            std::string path = jit_library_path();
            if (path.empty())
            {
                failure = std::string(__library_name) + " not found";
            }
            else
            {
            #ifdef _WIN32
                HMODULE handle = LoadLibraryA(path.c_str());
                if (handle == nullptr)
                {
                    failure = "cannot load '" + path + "': " + __last_error()
                              + " (is the library of LLVM in the PATH?)";
                }

                auto symbol = [&](const char * name) -> void * {
                    return handle? (void *)GetProcAddress(handle, name) : nullptr;
                };
            #else
                void * handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
                if (handle == nullptr)
                {
                    const char * message = dlerror();
                    failure = "cannot load '" + path + "': " + (message? message : "?");
                }

                auto symbol = [&](const char * name) -> void * {
                    return handle? dlsym(handle, name) : nullptr;
                };
            #endif

                if (handle != nullptr)
                {
                    typedef int (*version_t)();
                    typedef const char * (*llvm_version_t)();

                    version_t version = (version_t)symbol("april_jit_version");
                    llvm_version_t llvm_version = (llvm_version_t)symbol("april_jit_llvm_version");

                    jit_backend_t * b = new jit_backend_t();
                    b->__compile = (__compile_t)symbol("april_jit_compile");
                    b->__run = (__run_t)symbol("april_jit_run");

                    if (version == nullptr || version() != APRIL_JIT_API_VERSION
                            || b->__compile == nullptr || b->__run == nullptr)
                    {
                        failure = "'" + path + "' is not the JIT library of this april";
                        delete b;
                    }
                    else
                    {
                        if (llvm_version != nullptr)
                            b->__llvm_version = llvm_version();

                        backend = b;
                    }
                }
            }
        }

        if (backend == nullptr)
            reason = failure;

        return backend;
    }

    // Compiles the IR to an object file.
    bool jit_backend_t::compile(const std::string & ir, int optimize,
                                const std::string & object_path, std::string & error)
    {
        char message[4096] = "";

        if (__compile(ir.data(), ir.size(), optimize, object_path.c_str(),
                      message, sizeof(message)) != 0)
        {
            error = message;
            return false;
        }

        return true;
    }

    // Runs the program of the object file.
    bool jit_backend_t::run(const std::string & object_path, int & exit_code,
                            std::string & error)
    {
        std::vector<april_jit_symbol_t> symbols = __symbols();

        char program[] = "program";
        char * argv[] = { program, nullptr };
        char message[4096] = "";

        if (__run(object_path.c_str(), symbols.data(), symbols.size(), 1, argv, &exit_code,
                  message, sizeof(message)) != 0)
        {
            error = message;
            return false;
        }

        return true;
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::native
