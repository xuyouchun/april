#ifndef __APRIL_UTIL_H__
#define __APRIL_UTIL_H__

#include <filesystem>

namespace X_ROOT_NS::modules::april {

    namespace
    {
        using namespace core;
        using namespace compile;

        using namespace rt;
        using namespace exec;

    }

    namespace __bf = ::boost::filesystem;
    typedef lib::path_t __path_t;

    typedef april_error_code_t __e_t;

    ////////// ////////// ////////// ////////// /////////

    compile_file_type_t file_type_of(const __path_t & file);

    ////////// ////////// ////////// ////////// /////////

    namespace
    {
        X_INLINE __path_t __revise(const __path_t & path, bool absolute)
        {
            if (absolute)
                return lib::absolute(path);

            return path;
        }
    }

    X_INLINE bool is_hidden_file(const __path_t & path)
    {
        return path.string()[0] == '.';
    }

    template<typename f_t> void each_files(const __path_t & path, f_t f, bool absolute = false)
    {
        __path_t directory;
        string_t filter;

        if (lib::split_path_filter(path, directory, filter))
        {
            for (__path_t file : lib::all_files(directory, filter.c_str()))
            {
                f(__revise(file, absolute));
            }
        }
        else if (__bf::is_directory(path))
        {
            for (__path_t file : lib::all_files(directory, _T("*.*")))
            {
                if (!is_hidden_file(file))
                    f(__revise(file, absolute));
            }
        }
        else if (__bf::exists(path))
        {
            f(__revise(path, absolute));
        }
        else
        {
            throw _ED(__e_t::file_not_exists, lib::to_str(path));
        }
    }

    std::vector<__path_t> all_files(const __path_t & path, bool absolute = false);

    ////////// ////////// ////////// ////////// /////////

    // The hash of the inputs of a build, to tell whether a build in the cache is the one
    // that is needed: 128 bits, two FNV-1a lanes.
    class hasher_t
    {
    public:

        // Adds bytes.
        void add(const void * data, size_t size);

        // Adds a string, with its length.
        void add(const std::string & s);

        // Adds a number.
        void add(uint64_t value);

        // Adds the content of a file, with its path: false if it cannot be read.
        bool add_file(const std::string & path);

        // Returns the hash, 32 hexadecimal digits.
        std::string hex() const;

    private:
        uint64_t __h1 = 14695981039346656037ULL;
        uint64_t __h2 = 0x9E3779B97F4A7C15ULL;
    };

    // The directory of the builds of the programs that were run: $APRIL_CACHE, or
    // %LOCALAPPDATA%pril\cache, or $XDG_CACHE_HOME/april, or ~/.cache/april.
    std::filesystem::path cache_root();

    // The path of System.apl, that every program uses: it is not given to april. The first of
    // $APRIL_SYSTEM, <april dir>/System.apl, <april dir>/../System.apl, <april dir>/../lib/System.apl,
    // ./System.apl. Throws if there is none.
    std::filesystem::path find_system_assembly();

    ////////// ////////// ////////// ////////// /////////

}   // X_ROOT_NS::modules::april

#endif // __APRIL_UTIL_H__

