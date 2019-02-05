
#include <lib.h>
#include <boost/filesystem/fstream.hpp>
#include <algorithm.h>

namespace X_ROOT_NS { namespace lib {

    ////////// ////////// ////////// ////////// //////////

    // File error codes
    X_ENUM_INFO(file_error_code_t)

        // File / directory not exists
        X_C(not_exists,   _T("file/directory not exists"))

        // Open file error
        X_C(open_error,   _T("file open error"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    typedef boost::filesystem::basic_ifstream<char_t> __cifstream_t;
    typedef boost::filesystem::basic_ofstream<char_t> __cofstream_t;

    typedef boost::filesystem::basic_ifstream<char> __bifstream_t;
    typedef boost::filesystem::basic_ofstream<char> __bofstream_t;

    typedef std::ios_base::openmode __openmode_t;

    //-------- ---------- ---------- ---------- ----------

    // Converts path to string type.
    string_t to_str(const path_t & path)
    {
        return string_convert<char, char_t>(path.string());
    }

    //-------- ---------- ---------- ---------- ----------

    // Returns length of the string.
    template<typename c_t>
    size_t __len(const c_t * s)
    {
        for(const c_t * p = s; ; p++)
        {
            if(!*p)
                return p - s;
        }
    }

    //-------- ---------- ---------- ---------- ----------

    // Read text from a file.
    template<typename istream_t, typename c_t=typename istream_t::char_type>
    size_t __read_stream(istream_t & f, c_t * buffer, size_t size)
    {
        f.read(buffer, size);
        return f.gcount();
    }

    // Write text to a file.
    template<typename ostream_t, typename c_t=typename ostream_t::char_type>
    void __write_all(ostream_t & stream, const c_t * s, size_t length)
    {
        typedef typename ostream_t::char_type _c_t;
        stream.write((_c_t *)s, length == __empty_length? __len(s) : length);
    }

    // Write text to a file.
    template<typename ostream_t, typename c_t=typename ostream_t::char_type>
    void __write_all(const path_t & file, const c_t * s, size_t length, __openmode_t mode)
    {
        ostream_t f;
        f.open(file, mode | std::ios_base::out);
        if(!f.is_open())
            throw _E(file_error_code_t::open_error);

        __write_all(f, s, length);

        f.close();
    }
 
    //-------- ---------- ---------- ---------- ----------

    // Read all text from a text file.
    template<
        typename istream_t, typename c_t = typename istream_t::char_type,
        typename s_t = std::basic_string<c_t>
    >
    s_t __read_all(istream_t & stream)
    {
        c_t buffer[1024];
        size_t size;
        s_t ret_s;

        typedef typename istream_t::char_type _c_t;
        while((size = __read_stream(stream, (_c_t *)buffer, array_size(buffer))) > 0)
        {
            ret_s.append(buffer, size);
        }

        return std::move(ret_s);
    }

    // Read all text from a text file.
    template<
        typename istream_t, typename c_t = typename istream_t::char_type,
        typename s_t = std::basic_string<c_t>
    >
    s_t __read_all(const path_t & file)
    {
        istream_t f;
        f.open(file, std::ios_base::in);
        if(!f.is_open())
            throw _E(file_error_code_t::open_error);

        s_t ret = __read_all<istream_t, c_t, s_t>(f);
        f.close();

        return std::move(ret);
    }

    //-------- ---------- ---------- ---------- ----------
    // text file

    // Reads all text from a text file.
    string_t read_all_text(const path_t & file)
    {
        return __read_all<__cifstream_t>(file);
    }

    // Writes text to a file.
    void write_all_text(const path_t & file, const char_t * s, size_t length)
    {
        _A(s != nullptr);
        __write_all<__cofstream_t>(file, s, length, std::ios_base::trunc);
    }

    // Writes text to a file.
    void write_all_text(const path_t & file, const string_t & s)
    {
        write_all_text(file, s.c_str(), s.length());
    }

    // Appends text to the end of the file.
    void append_all_text(const path_t & file, const char_t * s, size_t length)
    {
        _A(s != nullptr);
        __write_all<__cofstream_t>(file, s, length, std::ios_base::app);
    }

    // Appends text to the end of the file.
    void append_all_text(const path_t & file, const string_t & s)
    {
        append_all_text(file, s.c_str(), s.length());
    }

    //-------- ---------- ---------- ---------- ----------
    // binary file

    // Reads all bytes from a file.
    bytes_t read_all_bytes(const path_t & file)
    {
        return __read_all<__bifstream_t, byte_t>(file);
    }

    // Writes bytes to a file.
    void write_all_bytes(const path_t & file, const byte_t * bytes, size_t length)
    {
        _A(bytes != nullptr);
        __write_all<__bofstream_t>(file, bytes, length, std::ios_base::trunc);
    }

    // Writes bytes to a file.
    void write_all_bytes(const path_t & file, const bytes_t & bytes)
    {
        write_all_bytes(file, bytes.c_str(), bytes.length());
    }

    // Appends bytes to the end of the file.
    void append_all_bytes(const path_t & file, const byte_t * bytes, size_t length)
    {
        _A(bytes != nullptr);
        __write_all<__cofstream_t>(file, bytes, length, std::ios_base::app);
    }

    // Append bytes to the end of the file.
    void append_all_bytes(const path_t & file, const bytes_t & bytes)
    {
        append_all_bytes(file, bytes.c_str(), bytes.length());
    }

    ////////// ////////// ////////// ////////// //////////

    // Converts char string to wchar_t string.
    X_INLINE string_t __to_string_t(const std::basic_string<char> & s)
    {
        return string_convert<char, char_t>(s);
    }

    // Returns file name of the path.
    string_t filename(const path_t & path)
    {
        return path.filename().wstring();
    }

    // Returns file extension of the path.
    string_t fileext(const path_t & path)
    {
        auto s = path.extension().wstring();
        if(s.length() == 0)
            return _T("");
        
        const char_t * start = s.c_str(), * end = start + s.length();
        if(s[0] == '.')
            start++;

        return string_t(start, end);
    }

    // Returns parent path of the path.
    string_t parent(const path_t & path)
    {
        return path.parent_path().wstring();
    }

    // Returns path that deleted file name.
    string_t truncate_fileext(const path_t & path)
    {
        auto s = path.wstring();
        auto ext = path.extension().wstring();

        size_t len = s.length();
        if(s.length() > 0)
            len -= ext.length();

        const char_t * start = s.c_str(), * end = start + len;
        return string_t(start, end);
    }

    // Returns true if the p_start has the specified prefix.
    template<size_t _size>
    bool __pre_with(const char_t * p_start, const char_t * p, const char_t (&prefix)[_size])
    {
        const size_t size = _size - 1;
        if(p - p_start + 1 < size)
            return false;

        const char_t * s = p - size + 1;
        if(s <= p_start || *(s - 1) == _T('/'))
            return al::starts_with(s, prefix);

        return false;
    }

    bool __jmp_relative(const char_t * p_start, const char_t * & p)
    {
        if(__pre_with(p_start, p - 1, _T(".")))             // ./
        {
            p -= 2;
            if(p >= p_start && *p == _T('/'))
                __jmp_relative(p_start, p);

            return true;
        }

        if(__pre_with(p_start, p - 1, _T("..")))
        {
            p -= 3;
            if(p >= p_start)
            {
                if(*p == _T('/'))
                    __jmp_relative(p_start, p);
            }
            else
                throw _EC(format_error);

            for(p--; p >= p_start; p--)
            {
                if(*p == _T('/'))
                    break;
            }

            return true;
        }

        return false;
    }

    // Make absolute for the path, Deleted . / .. sections.
    void __make_absolute(const char_t * p_start, const char_t * & p, char_t * & out)
    {
        *out-- = _T('\0');
        __jmp_relative(p_start, p);

        if(*p == _T('\0') || *p == _T('/'))
            p--;

        while(p >= p_start)
        {
            if(*p == _T('/'))
            {
                while(__jmp_relative(p_start, p))
                {
                    if(p < p_start || *p != _T('/'))
                        break;
                }

                if(p >= p_start)
                    *out-- = *p--;
            }
            else
            {
                *out-- = *p--;
            }
        }
    }

    // Make absolute for the path, Deleted . / .. sections.
    path_t make_absolute(const path_t & path)
    {
        string_t s = path.wstring();
        size_t length = s.length();

        char_t ss[length + 1];
        char_t * out = ss + length;

        const char_t * p_start = s.c_str(), * p = p_start + length;
        __make_absolute(p_start, p, out);

        return path_t(string_t(out + 1));
    }

    // Returns absolution for the relatived by relative
    path_t absolute(const path_t & path, const path_t & relative)
    {
        return make_absolute(__bf::absolute(path, relative));
    }

    ////////// ////////// ////////// ////////// //////////

    // Options for walk files / directories.
    X_ENUM_INFO(each_path_option_t)

        // Walk all files
        X_C(files,          _T("files"))

        // Walk all directories
        X_C(directories,    _T("directories"))

        // Walk directories first.
        X_C(recurve,        _T("recurve"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Returns all file / directories in the path.
    std::vector<path_t> __all_paths(const path_t & path, const char_t * filter,
                                                    each_path_option_t options)
    {
        typedef each_path_option_t option_t;

        std::vector<path_t> paths;
        each(path, 
            [&paths](auto & it) { return paths.push_back(it.path()), true; },
            filter, options
        );

        return std::move(paths);
    }

    // Returns all directories.
    std::vector<path_t> all_directories(const path_t & path, const char_t * filter)
    {
        return __all_paths(path, filter, each_path_option_t::directories);
    }

    // Returns all files.
    std::vector<path_t> all_files(const path_t & path, const char_t * filter)
    {
        return __all_paths(path, filter, each_path_option_t::files);
    }

    // Split path to directory and filter.
    // Returns true if the path contains filters.
    bool split_path_filter(const path_t & path, path_t & out_dir, string_t & out_filter)
    {
        string_t spath = to_str(path);
        size_t index = spath.find_last_of(_T("/"));

        auto npos = string_t::npos;
        if(index == npos || spath.find_first_of(_T("?*"), index + 1) == npos)
            return false;

        out_dir = path_t(spath.substr(0, index));
        out_filter = spath.substr(index + 1);        

        return true;
    }

    ////////// ////////// ////////// ////////// //////////

} }
