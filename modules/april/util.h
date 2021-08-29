#ifndef __APRIL_UTIL_H__
#define __APRIL_UTIL_H__

namespace X_ROOT_NS { namespace modules { namespace april {

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
            throw _ED(__e_t::file_not_exists, path);
        }
    }

    std::vector<__path_t> all_files(const __path_t & path, bool absolute = false);

    ////////// ////////// ////////// ////////// /////////

} } }  // X_ROOT_NS::modules::april

#endif // __APRIL_UTIL_H__

