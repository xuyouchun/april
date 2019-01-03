
#include <common.h>

namespace X_ROOT_NS {

    ////////// ////////// ////////// ////////// //////////

    #define __X_TYPE_OP(type) X_C(type##_, _S(type))

    X_ENUM_INFO(value_type_t)
        __X_EACH_TYPES 
    X_ENUM_INFO_END

    #undef __X_TYPE_OP

    ////////// ////////// ////////// ////////// //////////

    const guid_t guid_t::empty { };

    ////////// ////////// ////////// ////////// //////////

    ostream_t & operator << (ostream_t & stream, const tvalue_t & v)
    {
        #define __X_CASE(type)                              \
            case value_type_t::type##_:                     \
                stream << get_value<type##_t>(v.value);     \
                break;

        switch(v.type)
        {
            #define __X_TYPE_OP __X_CASE
            __X_EACH_TYPES 
            #undef __X_TYPE_OP

            default:
                break;
        }

        #undef __X_CASE

        return stream;
    }

    ////////// ////////// ////////// ////////// //////////
    // Class error_t

    const string_t error_t::to_string() const
    {
        return this->get_message();
    }

    ////////// ////////// ////////// ////////// //////////
    // Class __error_base_t

    const string_t __error_base_t::to_string() const
    {
        string_t s = this->get_message();

        if(file.length() > 0)
        {
            std::basic_ostringstream<char_t> os;
            os << s.c_str() << _T(", file: ") << file.c_str();

            if(line != (size_t)-1)
            {
                os << _T(", line: ") << line;
            }

            return os.str();
        }
        else
        {
            return s;
        }
    }

    const string_t & __error_base_t::get_message() const
    {
        return this->message;
    }

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(common_error_code_t)
        X_C(invalid_operation,  _T("invalid operation"))
        X_C(not_supported,      _T("not supported"))
        X_C(no_memory,          _T("no enough memory"))
        X_C(argument_error,     _T("argument error"))
        X_C(format_error,       _T("format error"))
        X_C(convert_error,      _T("convert error"))
		X_C(not_found,		    _T("not found"))
        X_C(unexpected,         _T("unexpected"))
        X_C(unimplemented,      _T("unimplemented"))
    X_ENUM_INFO_END

    logic_error_t<common_error_code_t> _unimplemented_error(const object_t * obj,
                                                            const char_t * method)
    {
        string_t class_name = string_convert<char, char_t>(
            typeid(*const_cast<object_t *>(obj)).name()
        );

        return _error(common_error_code_t::unimplemented,
            _F(_T("%1%.%2% const not implemented"), class_name, method)
        );
    }

    logic_error_t<common_error_code_t> _unimplemented_error(object_t * obj,
                                                     const char_t * method)
    {
        string_t class_name = string_convert<char, char_t>(typeid(*obj).name());

        return _error(common_error_code_t::unimplemented,
            _F(_T("%1%.%2% not implemented"), class_name, method)
        );
    }

    ////////// ////////// ////////// ////////// //////////
    // X_ASSERT

    void __raise_assert_error(const char_t * exp, const char_t * file, uint_t line,
                                                            const char_t * reason)
    {
        string_t msg;

        if(reason && reason[0])
            msg = sprintf(_T("Assert error: %1%, %2%"), exp, reason);
        else
            msg = sprintf(_T("Assert error: %1%"), exp);

        throw assert_error_t(std::move(msg), file, line);
    }

    ////////// ////////// ////////// ////////// //////////
    // memory_t

    X_ENUM_INFO(memory_flag_t)

        X_C(is_object)

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    memory_t * memory_t::__default_memory()
    {
        return default_memory_t::instance();
    }

    ////////// ////////// ////////// ////////// //////////
    // default_memory_t

    void * default_memory_t::alloc(size_t size, memory_flag_t flag)
    {
        void * bytes = ::operator new(size);
        if(!bytes)
            throw _E(common_error_code_t::no_memory);

        return bytes;
    }

    void default_memory_t::free(void * obj)
    {
        ::operator delete(obj);
    }

    void * default_memory_t::realloc(void * obj, size_t size, memory_flag_t flag)
    {
        free(obj);
        return alloc(size, flag);
    }

    default_memory_t * default_memory_t::instance()
    {
        static default_memory_t * memory = new default_memory_t();
        return memory;
    }

    ////////// ////////// ////////// ////////// //////////
    // pool_t

    void * pool_t::alloc(size_t size, memory_flag_t flag)
    {
        void * bytes = ::operator new(size);
        if(!bytes)
            throw _E(common_error_code_t::no_memory);

        __objects[(unsigned long)bytes] = (object_t *)bytes;
        return bytes;
    }

    void pool_t::free(void * obj)
    {
        auto it = __objects.find((unsigned long)obj);
        if(it == __objects.end())
            throw _E(common_error_code_t::invalid_operation, _T("object not in this pool"));

        __objects.erase(it);
        ::operator delete(it->second);
    }

    void * pool_t::realloc(void * obj, size_t size, memory_flag_t flag)
    {
        throw _E(common_error_code_t::not_supported);
    }

    pool_t::~pool_t()
    {
        for(auto it : __objects)
        {
            object_t * obj = it.second;
            obj->~object_t();
            ::operator delete(obj);
        }
    }

    ////////// ////////// ////////// ////////// //////////

    void file_xistream_t::open(const string_t & path, __open_mode_t open_mode)
    {
        __open_mode_t mode = (open_mode | __i::in) & ~(__i::out | __i::trunc);
        __stream.open(string_convert<char_t, char>(path), mode);

        if(!__stream.is_open())
            throw _ECF(invalid_operation, _T("cannot open file '%1%'"), path);
    }

    size_t file_xistream_t::read(byte_t * buffer, size_t length)
    {
        if(buffer == nullptr)
            throw _EC(argument_error, _T("buffer is null"));

        return __stream.read((char *)buffer, length), __stream.gcount();
    }

    void file_xistream_t::completed()
    {
        __stream.close();
    }

    file_xistream_t::~file_xistream_t()
    {
        __stream.close();
    }

    ////////// ////////// ////////// ////////// //////////

    void file_xostream_t::open(const string_t & path, __open_mode_t open_mode)
    {
        __open_mode_t mode = (open_mode | __i::out) & ~__i::in;
        __stream.open(string_convert<char_t, char>(path), mode);

        if(!__stream.is_open())
            throw _ECF(invalid_operation, _T("cannot open file '%1%'"), path);
    }

    void file_xostream_t::write(const byte_t * buffer, size_t length)
    {
        if(buffer == nullptr)
            throw _EC(argument_error, _T("buffer is null"));

        __stream.write((const char *)buffer, length);
    }

    void file_xostream_t::completed()
    {
        __stream.close();
    }

    file_xostream_t::~file_xostream_t()
    {
        __stream.close();
    }

    ////////// ////////// ////////// ////////// //////////

}	 // namespace X_ROOT_NS
