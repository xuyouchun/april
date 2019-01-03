
#include <lib.h>
#include <boost/filesystem/fstream.hpp>
#include <algorithm.h>

namespace X_ROOT_NS { namespace lib {

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(file_error_code_t)
        X_C(not_exists,   _T("file/directory not exists"))
        X_C(open_error,   _T("file open error"))
    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    typedef boost::filesystem::basic_ifstream<char_t> __cifstream_t;
    typedef boost::filesystem::basic_ofstream<char_t> __cofstream_t;

    typedef boost::filesystem::basic_ifstream<char> __bifstream_t;
    typedef boost::filesystem::basic_ofstream<char> __bofstream_t;

    typedef std::ios_base::openmode __openmode_t;

    //-------- ---------- ---------- ---------- ----------

    string_t to_str(const path_t & path)
    {
        return string_convert<char, char_t>(path.string());
    }

    //-------- ---------- ---------- ---------- ----------

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

    template<typename istream_t, typename c_t=typename istream_t::char_type>
    size_t __read_stream(istream_t & f, c_t * buffer, size_t size)
    {
        f.read(buffer, size);
        return f.gcount();
    }

    template<typename ostream_t, typename c_t=typename ostream_t::char_type>
    void __write_all(ostream_t & stream, const c_t * s, size_t length)
    {
        typedef typename ostream_t::char_type _c_t;
        stream.write((_c_t *)s, length == __empty_length? __len(s) : length);
    }

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

    template<typename istream_t, typename c_t=typename istream_t::char_type,
             typename s_t=std::basic_string<c_t>>
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

    template<typename istream_t, typename c_t=typename istream_t::char_type,
             typename s_t=std::basic_string<c_t>>
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

    string_t read_all_text(const path_t & file)
    {
        return __read_all<__cifstream_t>(file);
    }

    void write_all_text(const path_t & file, const char_t * s, size_t length)
    {
        _A(s != nullptr);
        __write_all<__cofstream_t>(file, s, length, std::ios_base::trunc);
    }

    void write_all_text(const path_t & file, const string_t & s)
    {
        write_all_text(file, s.c_str(), s.length());
    }

    void append_all_text(const path_t & file, const char_t * s, size_t length)
    {
        _A(s != nullptr);
        __write_all<__cofstream_t>(file, s, length, std::ios_base::app);
    }

    void append_all_text(const path_t & file, const string_t & s)
    {
        append_all_text(file, s.c_str(), s.length());
    }

    //-------- ---------- ---------- ---------- ----------
    // binary file

    bytes_t read_all_bytes(const path_t & file)
    {
        return __read_all<__bifstream_t, byte_t>(file);
    }

    void write_all_bytes(const path_t & file, const byte_t * bytes, size_t length)
    {
        _A(bytes != nullptr);
        __write_all<__bofstream_t>(file, bytes, length, std::ios_base::trunc);
    }

    void write_all_bytes(const path_t & file, const bytes_t & bytes)
    {
        write_all_bytes(file, bytes.c_str(), bytes.length());
    }

    void append_all_bytes(const path_t & file, const byte_t * bytes, size_t length)
    {
        _A(bytes != nullptr);
        __write_all<__cofstream_t>(file, bytes, length, std::ios_base::app);
    }

    void append_all_bytes(const path_t & file, const bytes_t & bytes)
    {
        append_all_bytes(file, bytes.c_str(), bytes.length());
    }

    ////////// ////////// ////////// ////////// //////////

    X_INLINE string_t __to_string_t(const std::basic_string<char> & s)
    {
        return string_convert<char, char_t>(s);
    }

    string_t filename(const path_t & path)
    {
        return __to_string_t(path.filename().string());
    }

    string_t fileext(const path_t & path)
    {
        auto s = path.extension().string();
        if(s.length() == 0)
            return _T("");
        
        const char * start = s.c_str(), * end = start + s.length();
        if(s[0] == '.')
            start++;

        return string_convert<char, char_t>(start, end);
    }

    string_t truncate_fileext(const path_t & path)
    {
        auto s = path.string();
        auto ext = path.extension().string();

        size_t len = s.length();
        if(s.length() > 0)
            len -= ext.length();

        const char * start = s.c_str(), * end = start + len;
        return string_convert<char, char_t>(start, end);
    }

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(each_path_option_t)

        X_C(files,          _T("files"))
        X_C(directories,    _T("directories"))
        X_C(recurve,        _T("recurve"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

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

    std::vector<path_t> all_directories(const path_t & path, const char_t * filter)
    {
        return __all_paths(path, filter, each_path_option_t::directories);
    }

    std::vector<path_t> all_files(const path_t & path, const char_t * filter)
    {
        return __all_paths(path, filter, each_path_option_t::files);
    }

    ////////// ////////// ////////// ////////// //////////

} }
