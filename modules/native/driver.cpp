
#include "driver.h"

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <filesystem>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#else
#include <fcntl.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>

extern char ** environ;
#endif

namespace X_ROOT_NS::modules::native {

    namespace fs = std::filesystem;

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        // The minimum LLVM version: the generated IR uses opaque pointers.
        const int __min_llvm_version = 15;

        // The libraries the executables link with, besides april's own.
    #ifdef _WIN32
        // MSYS2 clang64: april is built with libc++ and the static boost libraries.
        const char * const __system_libs[] = {
            "-stdlib=libc++", "-Wl,-Bstatic",
            "-lboost_log-mt", "-lboost_thread-mt", "-lboost_date_time-mt",
            "-lboost_container-mt", "-lboost_chrono-mt", "-lboost_filesystem-mt",
            "-lboost_atomic-mt", "-Wl,-Bdynamic",
            "-lole32", "-loleaut32", "-lpsapi", "-lsecur32", "-lws2_32", "-lmswsock",
            "-ladvapi32", "-lsynchronization", "-lbcrypt", "-lpthread",
        };
    #else
        const char * const __system_libs[] = {
            "-lboost_log", "-lboost_system", "-lboost_filesystem", "-lpthread",
        };
    #endif

    #ifndef _WIN32
        // Returns the exit code of a wait status.
        int __exit_code_of(int status)
        {
            if (WIFEXITED(status))
                return WEXITSTATUS(status);

            if (WIFSIGNALED(status))
                return 128 + WTERMSIG(status);

            return 1;
        }

        // Converts to a argv array, valid while `argv` lives.
        std::vector<char *> __to_c_argv(const std::vector<std::string> & argv)
        {
            std::vector<char *> r;
            for (const std::string & s : argv)
                r.push_back(const_cast<char *>(s.c_str()));

            r.push_back(nullptr);
            return r;
        }
    #endif

        // Returns the directory of the running april executable.
        std::string __april_dir(const build_options_t & options)
        {
            std::string path = options.april_path;

            if (path.empty())
            {
                char buffer[4096];

            #ifdef _WIN32
                DWORD n = GetModuleFileNameA(nullptr, buffer, (DWORD)sizeof(buffer));
                if (n > 0 && n < sizeof(buffer))
                    path.assign(buffer, (size_t)n);
            #else
                ssize_t n = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
                if (n > 0)
                    path.assign(buffer, (size_t)n);
            #endif
            }

            if (path.empty())
                return std::string();

            return fs::path(path).parent_path().string();
        }

        bool __has_libnative(const fs::path & dir)
        {
            std::error_code ec;
            return fs::is_regular_file(dir / "libnative.a", ec);
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Quotes an argument for a shell command line.
    std::string shell_quote(const std::string & s)
    {
        if (!s.empty() && s.find_first_not_of(
                "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-+=.,:/@%")
                == std::string::npos)
        {
            return s;
        }

        std::string r = "'";
        for (char c : s)
        {
            if (c == '\'')
                r += "'\\''";
            else
                r += c;
        }

        return r + "'";
    }

    // Joins a command line for display.
    std::string command_line_of(const std::vector<std::string> & argv)
    {
        std::string r;
        for (size_t k = 0; k < argv.size(); k++)
            r += (k > 0? " " : "") + shell_quote(argv[k]);

        return r;
    }

    // Parses the major version of `clang --version`.
    int parse_clang_major_version(const std::string & version_output)
    {
        const std::string key = "clang version ";

        size_t pos = version_output.find(key);
        if (pos == std::string::npos)
            return -1;

        pos += key.size();

        int major = 0;
        size_t digits = 0;
        for (; pos < version_output.size() && std::isdigit((unsigned char)version_output[pos]);
                                                                            pos++, digits++)
        {
            major = major * 10 + (version_output[pos] - '0');
        }

        return digits > 0? major : -1;
    }

    // "dir/name.ll" -> "dir/name.native".
    std::string default_output_path(const std::string & ir_path)
    {
        fs::path p(ir_path);
    #ifdef _WIN32
        p.replace_extension(".exe");
    #else
        p.replace_extension(".native");
    #endif
        return p.string();
    }

    // Finds the directory of the static libraries.
    std::string find_libs_dir(const build_options_t & options)
    {
        std::vector<fs::path> candidates;

        if (!options.libs_dir.empty())
        {
            // Explicit: no guessing.
            if (!__has_libnative(options.libs_dir))
            {
                throw driver_error_t("libnative.a not found in '" + options.libs_dir
                                     + "': build april first");
            }

            return options.libs_dir;
        }

        if (const char * env = std::getenv("APRIL_LIBS"))
        {
            if (*env != 0)
                candidates.push_back(env);
        }

        std::string dir = __april_dir(options);
        if (!dir.empty())
        {
            candidates.push_back(fs::path(dir) / ".." / "libs");
            candidates.push_back(fs::path(dir) / ".." / "build");
        }

        candidates.push_back("libs");
        candidates.push_back("build");

        std::string tried;
        for (const fs::path & c : candidates)
        {
            if (__has_libnative(c))
                return c.lexically_normal().string();

            tried += "\n    " + c.lexically_normal().string();
        }

        throw driver_error_t("libnative.a not found; build april first, or set APRIL_LIBS "
                             "/ pass --libs. Searched:" + tried);
    }

    // Lists the static libraries of the directory, sorted by name.
    std::vector<std::string> list_static_libs(const std::string & libs_dir)
    {
        std::vector<std::string> libs;

        std::error_code ec;
        for (fs::directory_iterator it(libs_dir, ec), end; !ec && it != end; it.increment(ec))
        {
            std::string name = it->path().filename().string();

            if (name.size() > 5 && name.compare(0, 3, "lib") == 0
                    && name.compare(name.size() - 2, 2, ".a") == 0
                    && it->is_regular_file(ec))
            {
                libs.push_back(it->path().string());
            }
        }

        std::sort(libs.begin(), libs.end());
        return libs;
    }

    // The command line of compiling the IR to an object file.
    std::vector<std::string> compile_command(const std::string & clang,
                const std::string & ir_path, const std::string & object_path, int optimize)
    {
        return {
            clang, "-c", "-x", "ir", ir_path,
            "-O" + std::to_string(std::min(3, std::max(0, optimize))),
            "-o", object_path,
        };
    }

    // The command line of linking.
    std::vector<std::string> link_command(const std::string & clang,
                const std::string & object_path, const std::string & output_path,
                const std::vector<std::string> & libs, bool no_pie)
    {
        std::vector<std::string> argv = { clang, object_path, "-o", output_path };

        if (no_pie)
            argv.push_back("-no-pie");

        // The libraries reference each other: link them as a group.
        argv.push_back("-Wl,--start-group");
        argv.insert(argv.end(), libs.begin(), libs.end());
        argv.push_back("-Wl,--end-group");

        for (const char * lib : __system_libs)
            argv.push_back(lib);

        return argv;
    }

    ////////// ////////// ////////// ////////// //////////

#ifdef _WIN32

    namespace
    {
        // Quotes an argument of a Windows command line (the rules of CommandLineToArgvW).
        std::string __win_quote(const std::string & s)
        {
            if (!s.empty() && s.find_first_of(" \t\n\v\"") == std::string::npos)
                return s;

            std::string r = "\"";
            for (size_t k = 0; ; k++)
            {
                size_t slashes = 0;
                while (k < s.size() && s[k] == '\\')
                    k++, slashes++;

                if (k == s.size())
                {
                    r.append(slashes * 2, '\\');
                    break;
                }

                if (s[k] == '"')
                {
                    r.append(slashes * 2 + 1, '\\');
                    r += '"';
                }
                else
                {
                    r.append(slashes, '\\');
                    r += s[k];
                }
            }

            return r + "\"";
        }

        // Starts a process and waits for it. The write end of a pipe (if not null) is its
        // standard output, the read end is drained into `output`.
        int __win_run(const std::vector<std::string> & argv, HANDLE out_write,
                      HANDLE out_read, std::string * output)
        {
            std::string command;
            for (size_t k = 0; k < argv.size(); k++)
                command += (k > 0? " " : "") + __win_quote(argv[k]);

            STARTUPINFOA si = { };
            si.cb = sizeof(si);
            si.dwFlags = STARTF_USESTDHANDLES;
            si.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
            si.hStdOutput = out_write? out_write : GetStdHandle(STD_OUTPUT_HANDLE);
            si.hStdError  = GetStdHandle(STD_ERROR_HANDLE);

            PROCESS_INFORMATION pi = { };
            std::vector<char> buffer(command.begin(), command.end());
            buffer.push_back(0);

            // No application name: the executable is searched in PATH (".exe" appended).
            BOOL ok = CreateProcessA(nullptr, buffer.data(), nullptr, nullptr, TRUE, 0,
                                     nullptr, nullptr, &si, &pi);
            DWORD err = ok? 0 : GetLastError();

            if (out_write)
                CloseHandle(out_write);

            if (!ok)
            {
                if (out_read)
                    CloseHandle(out_read);

                throw driver_error_t("cannot run '" + argv[0] + "': error "
                    + std::to_string(err)
                    + (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND?
                                    " (is it installed and in PATH?)" : ""));
            }

            if (out_read)
            {
                char chunk[4096];
                DWORD n;
                while (ReadFile(out_read, chunk, sizeof(chunk), &n, nullptr) && n > 0)
                    output->append(chunk, (size_t)n);

                CloseHandle(out_read);
            }

            WaitForSingleObject(pi.hProcess, INFINITE);

            DWORD code = 1;
            GetExitCodeProcess(pi.hProcess, &code);

            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);

            return (int)code;
        }
    }

    // Runs a process and waits for it.
    int run_process(const std::vector<std::string> & argv)
    {
        if (argv.empty())
            throw driver_error_t("empty command");

        return __win_run(argv, nullptr, nullptr, nullptr);
    }

    // Runs a process and captures its standard output.
    int run_process_capture(const std::vector<std::string> & argv, std::string & output)
    {
        if (argv.empty())
            throw driver_error_t("empty command");

        output.clear();

        SECURITY_ATTRIBUTES sa = { sizeof(sa), nullptr, TRUE };
        HANDLE read_end = nullptr, write_end = nullptr;
        if (!CreatePipe(&read_end, &write_end, &sa, 0))
            throw driver_error_t("cannot create a pipe");

        // The child must not inherit the read end.
        SetHandleInformation(read_end, HANDLE_FLAG_INHERIT, 0);

        return __win_run(argv, write_end, read_end, &output);
    }

#else   // POSIX

    // Runs a process and waits for it.
    int run_process(const std::vector<std::string> & argv)
    {
        if (argv.empty())
            throw driver_error_t("empty command");

        std::vector<char *> c_argv = __to_c_argv(argv);

        pid_t pid;
        int err = posix_spawnp(&pid, argv[0].c_str(), nullptr, nullptr, c_argv.data(), environ);
        if (err != 0)
        {
            throw driver_error_t("cannot run '" + argv[0] + "': " + std::strerror(err)
                                 + (err == ENOENT? " (is it installed and in PATH?)" : ""));
        }

        int status = 0;
        while (waitpid(pid, &status, 0) < 0)
        {
            if (errno != EINTR)
                throw driver_error_t("waiting for '" + argv[0] + "' failed");
        }

        return __exit_code_of(status);
    }

    // Runs a process and captures its standard output.
    int run_process_capture(const std::vector<std::string> & argv, std::string & output)
    {
        if (argv.empty())
            throw driver_error_t("empty command");

        output.clear();

        int fds[2];
        if (pipe(fds) != 0)
            throw driver_error_t("cannot create a pipe");

        posix_spawn_file_actions_t actions;
        posix_spawn_file_actions_init(&actions);
        posix_spawn_file_actions_adddup2(&actions, fds[1], STDOUT_FILENO);
        posix_spawn_file_actions_addclose(&actions, fds[0]);
        posix_spawn_file_actions_addclose(&actions, fds[1]);

        std::vector<char *> c_argv = __to_c_argv(argv);

        pid_t pid;
        int err = posix_spawnp(&pid, argv[0].c_str(), &actions, nullptr, c_argv.data(), environ);

        posix_spawn_file_actions_destroy(&actions);
        close(fds[1]);

        if (err != 0)
        {
            close(fds[0]);
            throw driver_error_t("cannot run '" + argv[0] + "': " + std::strerror(err)
                                 + (err == ENOENT? " (is it installed and in PATH?)" : ""));
        }

        char buffer[4096];
        for (;;)
        {
            ssize_t n = read(fds[0], buffer, sizeof(buffer));
            if (n > 0)
                output.append(buffer, (size_t)n);
            else if (n == 0 || errno != EINTR)
                break;
        }

        close(fds[0]);

        int status = 0;
        while (waitpid(pid, &status, 0) < 0)
        {
            if (errno != EINTR)
                throw driver_error_t("waiting for '" + argv[0] + "' failed");
        }

        return __exit_code_of(status);
    }

#endif  // _WIN32

    ////////// ////////// ////////// ////////// //////////

    // Builds the executable from the IR.
    std::string build_executable(const build_options_t & options)
    {
        auto log = [&](const std::vector<std::string> & argv) {
            if (options.log)
                options.log(command_line_of(argv));
        };

        std::error_code ec;
        if (!fs::is_regular_file(options.ir_path, ec))
            throw driver_error_t("IR file '" + options.ir_path + "' not found");

        // The clang driver.
        std::string clang = options.clang;
        if (clang.empty())
        {
            const char * env = std::getenv("CLANG");
            clang = (env != nullptr && *env != 0)? env : "clang++";
        }

        std::string version_output;
        int code = run_process_capture({ clang, "--version" }, version_output);

        int major = parse_clang_major_version(version_output);
        if (code != 0 || major < 0)
        {
            throw driver_error_t("'" + clang + "' does not look like clang (its --version "
                                 "failed or has no 'clang version'); set CLANG to a clang++");
        }

        if (major < __min_llvm_version)
        {
            throw driver_error_t("clang " + std::to_string(major) + " is too old: LLVM "
                                 + std::to_string(__min_llvm_version)
                                 + "+ is required (the IR uses opaque pointers)");
        }

        // The libraries.
        std::string libs_dir = find_libs_dir(options);
        std::vector<std::string> libs = list_static_libs(libs_dir);

        std::string output_path = options.output_path.empty()?
                            default_output_path(options.ir_path) : options.output_path;

        fs::path object_path = fs::path(output_path);
        object_path += ".o";

        // Compile.
        {
            std::vector<std::string> argv = compile_command(
                clang, options.ir_path, object_path.string(), options.optimize
            );

            log(argv);
            code = run_process(argv);

            if (code != 0)
            {
                throw driver_error_t("compiling '" + options.ir_path + "' failed (exit code "
                                     + std::to_string(code) + "); the diagnostics of clang "
                                     "above point to lines of the IR");
            }
        }

        // Link.
        {
            std::vector<std::string> argv = link_command(
                clang, object_path.string(), output_path, libs, options.no_pie
            );

            log(argv);
            code = run_process(argv);

            if (code != 0)
            {
                throw driver_error_t("linking '" + output_path + "' failed (exit code "
                                     + std::to_string(code) + "); if the errors mention "
                                     "relocations, try --no-pie");
            }
        }

        if (!options.keep_object)
            fs::remove(object_path, ec);

        return output_path;
    }

    // Runs an executable with the arguments.
    int run_executable(const std::string & path, const std::vector<std::string> & args)
    {
        // A path without a slash would be searched in PATH: make it explicit.
        std::string exe = path;
    #ifdef _WIN32
        if (exe.find_first_of("/\\:") == std::string::npos)
            exe = ".\\" + exe;
    #else
        if (exe.find('/') == std::string::npos)
            exe = "./" + exe;
    #endif

        std::vector<std::string> argv = { exe };
        argv.insert(argv.end(), args.begin(), args.end());

        return run_process(argv);
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::native
