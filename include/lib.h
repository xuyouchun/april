
#ifndef __LIB_H__
#define __LIB_H__

#include <common.h>
#include <algorithm.h>

namespace X_ROOT_NS { namespace lib {

    namespace
    {
        namespace __bf = ::boost::filesystem;
    }

    ////////// ////////// ////////// ////////// //////////

    typedef ::boost::filesystem::path path_t;

    const size_t __empty_length = (size_t)-1;

    // File error codes
    X_ENUM(file_error_code_t)

        // File / directory not exists
        not_exists,

        // Open file error
        open_error,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////
    // text file

    // Reads all text from a text file.
    string_t read_all_text(const path_t & file);

    // Writes text to a file.
    void write_all_text(const path_t & file, const char_t * s, size_t length=__empty_length);

    // Writes text to a file.
    void write_all_text(const path_t & file, const string_t & s);

    // Appends text to the end of the file.
    void append_all_text(const path_t & file, const char_t * s, size_t length=__empty_length);

    // Appends text to the end of the file.
    void append_all_text(const path_t & file, const string_t & s);

    //-------- ---------- ---------- ---------- ----------

    // binary file

    // Reads all bytes from a file.
    bytes_t read_all_bytes(const path_t & file);

    // Writes bytes to a file.
    void write_all_bytes(const path_t & file, const byte_t * bytes, size_t length=__empty_length);

    // Writes bytes to a file.
    void write_all_bytes(const path_t & file, const bytes_t & bytes);

    // Appends bytes to the end of the file.
    void append_all_bytes(const path_t & file, const bytes_t * bytes, size_t length=__empty_length);

    // Append bytes to the end of the file.
    void append_all_bytes(const path_t & file, const bytes_t & bytes);

    ////////// ////////// ////////// ////////// //////////

    // Returns file name of the path.
    string_t filename(const path_t & path);

    // Returns file extension of the path.
    string_t fileext(const path_t & path);

    // Returns parent path of the path.
    string_t parent(const path_t & path);

    // Returns path that deleted file name.
    string_t truncate_fileext(const path_t & path);

    // Converts path_t to a string.
    string_t to_str(const path_t & path);

    // Make absolute for the path, Deleted . / .. sections.
    path_t make_absolute(const path_t & path);

    // Returns absolution for the relatived by relative
    path_t absolute(const path_t & path, const path_t & relative = __bf::current_path());

    namespace
    {
        // Returns length of the string.
        X_INLINE size_t __string_length(const string_t & s)
        {
            return s.length();
        }

        // Returns character pointer of the string.
        X_INLINE const char_t * __c_str(const string_t & s)
        {
            return s.c_str();
        }

        // Returns length of the string.
        X_INLINE size_t __string_length(const char_t * s)
        {
            return s == nullptr? 0 : al::strlen(s);
        }

        // Returns character pointer of the string.
        X_INLINE const char_t * __c_str(const char_t * s)
        {
            return s;
        }

        // Returns length of the string.
        template<size_t size>
        X_INLINE size_t __string_length(const char_t (&s)[size])
        {
            return size - 1;
        }

        // Appends path section.
        void __path_combine(stringstream_t & ss, bool first) { }

        // Appends path section.
        template<typename path_t, typename ... paths_t>
        void __path_combine(stringstream_t & ss, bool first, path_t && path, paths_t && ... paths)
        {
            size_t len = __string_length(path);
            if (len > 0 || (len == 1 && path[0] != _T('/')))
            {
                if (!first && path[0] == _T('/'))
                {
                    ss << __c_str(path) + 1;
                }
                else
                {
                    ss << __c_str(path);
                }

                if (sizeof ... (paths) > 0 && __c_str(path)[len - 1] != _T('/'))
                {
                    ss << _T('/');
                }
            }

            __path_combine(ss, false, std::forward<paths_t>(paths) ...);
        }
    }

    // Combine path sections.
    template<typename ... paths_t> string_t path_combine(paths_t && ... paths)
    {
        stringstream_t ss;
        __path_combine(ss, true, std::forward<paths_t>(paths) ...);

        return ss.str();
    }

    // Append path extension.
    template<typename path_t, typename ext_t>
    string_t path_add_extension(path_t && path, ext_t && ext)
    {
        if (__string_length(ext) == 0)
            return path;

        stringstream_t ss;
        ss << __c_str(path);

        if (ext[0] != _T('.'))
            ss << _T('.');

        ss << __c_str(ext);
        return ss.str();
    }

    // Open a file stream.
    template<typename fstream_t, typename path_t>
    bool fstream_open(fstream_t & fstream, path_t && path, std::ios_base::openmode openmode)
    {
        fstream.open(string_convert<char_type_t<path_t>, char>(path), openmode);
        return fstream.is_open();
    }

    ////////// ////////// ////////// ////////// //////////

    // Options for walk files / directories.
    X_ENUM(each_path_option_t)

        // Walk all files
        files           =   1 << 0,

        // Walk all directories
        directories     =   1 << 1,

        // Walk directories first.
        recurve         =   1 << 2,

        // Default options.
        default_        =   enum_or(files, directories),

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    namespace
    {
        // Walks files / directories in the path.
        template<typename callback_t>
        bool __each(const path_t & path, callback_t callback, std::basic_regex<char_t> * filter,
                                                              each_path_option_t options)
        {
            typedef each_path_option_t option_t;

            if (!__bf::exists(path))
                return false;

            for (__bf::directory_iterator it(path), it_end; it != it_end; it++)
            {
                auto p = *it;
                if (__bf::is_directory(p))
                {
                    if (enum_has_flag(options, option_t::directories) && !callback(p))
                        return false;

                    if (enum_has_flag(options, option_t::recurve)
                                            && !__each(p, callback, filter, options))
                        return false;
                }
                else
                {
                    if (enum_has_flag(options, option_t::files)
                        && (!filter || al::regex_is_match(filename(p.path()), *filter))
                        && !callback(p))
                        return false;
                }
            }

            return true;
        }

        // Convert *, ? filters to a regex.
        template<typename c_t = char_t>
        std::basic_string<c_t> __wildcard_to_regex(const c_t * s)
        {
            _A(s != nullptr);

            std::basic_ostringstream<c_t> ss;
            ss << _T('^');

            c_t c;
            while ((c = *s++) != _T('\0'))
            {
                switch (c)
                {
                    case _T('\\'): case _T('.'): case _T('['): case _T(']'):
                    case _T(':'):  case _T('('): case _T(')'):
                    case _T('{'):  case _T('}'):
                        ss << _T('\\') << c;
                        break;

                    case _T('*'):
                        ss << _T(".*?");
                        break;

                    case _T('?'):
                        ss << _T(".");
                        break;

                    default:
                        ss << c;
                        break;
                }
            }

            ss << _T('$');
            return ss.str();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Walks files / directories in the path.
    template<typename callback_t>
    void each(const path_t & path, callback_t callback, const char_t * filter = nullptr,
              each_path_option_t options = each_path_option_t::default_)
    {
        if (filter == nullptr)
        {
            __each(path, callback, nullptr, options);
        }
        else
        {
            std::basic_regex<char_t> regex(__wildcard_to_regex(filter));
            __each(path, callback, &regex, options);
        }
    }

    // Returns all directories.
    std::vector<path_t> all_directories(const path_t & path, const char_t * filter = nullptr);

    // Returns all files.
    std::vector<path_t> all_files(const path_t & path, const char_t * filter = nullptr);

    // Split path to directory and filter.
    // Returns true if the path contains filters.
    bool split_path_filter(const path_t & path, path_t & out_dir, string_t & out_filter);

    ////////// ////////// ////////// ////////// //////////

} } // namespace X_ROOT_NS::lib

#endif  // __LIB_H__

