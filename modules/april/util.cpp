
#include <april.h>
#include "util.h"

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace X_ROOT_NS::modules::april {

    ////////// ////////// ////////// ////////// /////////

    // Returns type of the file.
    compile_file_type_t file_type_of(const __path_t & file)
    {
        string_t ext = lib::fileext(file);

        if (ext == solution_file_ext)
            return compile_file_type_t::solution;

        if (ext == project_file_ext)
            return compile_file_type_t::project;

        if (ext == april_file_ext)
            return compile_file_type_t::assembly;

        return compile_file_type_t::source;
    }

    ////////// ////////// ////////// ////////// /////////

    // Returns all files.
    std::vector<__path_t> all_files(const __path_t & path, bool absolute)
    {
        std::vector<__path_t> paths;
        each_files(path, [&](__path_t path) { paths.push_back(path); }, absolute);

        return std::move(paths);
    }

    ////////// ////////// ////////// ////////// /////////
    // hasher_t

    // Adds bytes.
    void hasher_t::add(const void * data, size_t size)
    {
        const unsigned char * p = (const unsigned char *)data;

        for (size_t k = 0; k < size; k++)
        {
            __h1 = (__h1 ^ p[k]) * 1099511628211ULL;
            __h2 = (__h2 ^ (uint64_t)(p[k] + 1)) * 0x100000001B3ULL * 31 + 17;
        }
    }

    // Adds a string.
    void hasher_t::add(const std::string & s)
    {
        add((uint64_t)s.size());
        add(s.data(), s.size());
    }

    // Adds a number.
    void hasher_t::add(uint64_t value)
    {
        add(&value, sizeof(value));
    }

    // Adds the content of a file.
    bool hasher_t::add_file(const std::string & path)
    {
        add(path);

        std::ifstream in(path, std::ios::binary);
        if (!in)
        {
            add((uint64_t)0);
            return false;
        }

        char buffer[65536];
        uint64_t total = 0;

        while (in.read(buffer, sizeof(buffer)) || in.gcount() > 0)
        {
            add(buffer, (size_t)in.gcount());
            total += (uint64_t)in.gcount();
        }

        add(total);
        return true;
    }

    // Returns the hash.
    std::string hasher_t::hex() const
    {
        char buffer[40];
        std::snprintf(buffer, sizeof(buffer), "%016llx%016llx",
                        (unsigned long long)__h1, (unsigned long long)__h2);

        return buffer;
    }

    ////////// ////////// ////////// ////////// /////////

    // The directory of the builds.
    std::filesystem::path cache_root()
    {
        namespace fs = std::filesystem;

        if (const char * env = std::getenv("APRIL_CACHE"))
        {
            if (*env != 0)
                return fs::path(env);
        }

    #ifdef _WIN32
        if (const char * env = std::getenv("LOCALAPPDATA"))
        {
            if (*env != 0)
                return fs::path(env) / "april" / "cache";
        }
    #else
        if (const char * env = std::getenv("XDG_CACHE_HOME"))
        {
            if (*env != 0)
                return fs::path(env) / "april";
        }

        if (const char * env = std::getenv("HOME"))
        {
            if (*env != 0)
                return fs::path(env) / ".cache" / "april";
        }
    #endif

        std::error_code ec;
        return fs::temp_directory_path(ec) / "april-cache";
    }

    // The path of System.apl.
    std::filesystem::path find_system_assembly()
    {
        namespace fs = std::filesystem;

        std::vector<fs::path> candidates;

        if (const char * env = std::getenv("APRIL_SYSTEM"))
        {
            if (*env != 0)
                candidates.push_back(env);
        }

        std::string exe = native::executable_path();
        if (!exe.empty())
        {
            fs::path dir = fs::path(exe).parent_path();
            candidates.push_back(dir / "System.apl");
            candidates.push_back(dir / ".." / "System.apl");
            candidates.push_back(dir / ".." / "lib" / "System.apl");
        }

        candidates.push_back(fs::path("System.apl"));

        string_t tried;
        for (const fs::path & c : candidates)
        {
            std::error_code ec;
            if (fs::is_regular_file(c, ec))
                return fs::absolute(c, ec).lexically_normal();

            tried += _T("\n    ") + c.lexically_normal().wstring();
        }

        throw _ED(__e_t::system_assembly_not_found, tried);
    }

    ////////// ////////// ////////// ////////// /////////

}   // X_ROOT_NS::modules::april

