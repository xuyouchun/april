
#include <april.h>
#include "util.h"


namespace X_ROOT_NS { namespace modules { namespace april {

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

} } }  // X_ROOT_NS::modules::april

