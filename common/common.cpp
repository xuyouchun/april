
/*
    XuYouChun

    The common source file.
*/


#include <common.h>
#include <arch.h>

namespace X_ROOT_NS {

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(xbool_t)

        X_C(no,         _T("no"))

        X_C(yes,        _T("yes"))

        X_C(none,       _T("none"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // All value_type_t descriptions

    #define __X_TYPE_OP(type) X_C(type##_, _S(type))

    X_ENUM_INFO(value_type_t)
        __X_EACH_TYPES 
    X_ENUM_INFO_END

    #undef __X_TYPE_OP

    ////////// ////////// ////////// ////////// //////////

    // An empty value of guid_t
    const guid_t guid_t::empty { };

    ////////// ////////// ////////// ////////// //////////

    tvalue_t::operator string_t() const _NE
    {
        #define __X_CASE(type)                                                          \
            case value_type_t::type##_:                                                 \
                return __value_t<type##_t>::to_string(value);

        switch (type)
        {
            #define __X_TYPE_OP __X_CASE
            __X_EACH_TYPES 
            #undef __X_TYPE_OP

            default:
                return _T("");
        }

        #undef __X_CASE
    }

    template<typename _compare_func_t>
    bool __compare_tvalue(const tvalue_t & v1, const tvalue_t & v2, _compare_func_t f)
    {
        #define __Compare(_t1, _t2) f(v1.get_value<_t1>(), v2.get_value<_t2>())

        value_type_t t1 = v1.type, t2 = v2.type;

        if (t1 == t2)
        {
            #define __X_CASE(_type)                                                     \
                case value_type_t::_type##_:                                            \
                    return __Compare(_type##_t, _type##_t);

            switch (t1)
            {
                #define __X_TYPE_OP __X_CASE
                __X_EACH_TYPES 
                #undef __X_TYPE_OP

                default:
                    X_UNEXPECTED();
            }

            #undef __X_CASE
        }

        if (is_float(t1) || is_float(t2))
            return __Compare(ldouble_t, ldouble_t);

        if (t1 == value_type_t::uint64_)
            return __Compare(uint64_t, int64_t);

        if (t2 == value_type_t::uint64_)
            return __Compare(int64_t, uint64_t);

        return __Compare(int64_t, int64_t);

        #undef __Compare
    }

    bool tvalue_t::operator == (const tvalue_t & other) const _NE
    {
        return __compare_tvalue(*this, other, [](auto v1, auto v2) { return v1 == v2; });
    }

    bool tvalue_t::operator < (const tvalue_t & other) const _NE
    {
        return __compare_tvalue(*this, other, [](auto v1, auto v2) { return v1 < v2; });
    }

    bool tvalue_t::operator <= (const tvalue_t & other) const _NE
    {
        return __compare_tvalue(*this, other, [](auto v1, auto v2) { return v1 <= v2; });
    }

    bool tvalue_t::operator != (const tvalue_t & other) const _NE
    {
        return __compare_tvalue(*this, other, [](auto v1, auto v2) { return v1 != v2; });
    }

    bool tvalue_t::operator > (const tvalue_t & other) const _NE
    {
        return __compare_tvalue(*this, other, [](auto v1, auto v2) { return v1 > v2; });
    }

    bool tvalue_t::operator >= (const tvalue_t & other) const _NE
    {
        return __compare_tvalue(*this, other, [](auto v1, auto v2) { return v1 >= v2; });
    }

    tvalue_t tvalue_t::operator - () const _NE
    {
        #define __X_CASE(type)                                                          \
            case value_type_t::type##_:                                                 \
                return tvalue_t(-value.type##_value);

        switch (type)
        {
            #define __X_TYPE_OP __X_CASE
            __X_EACH_TYPES 
            #undef __X_TYPE_OP

            default:
                return *this;
        }

        #undef __X_CASE
    }

    // Converts to unsigned type.
    tvalue_t tvalue_t::to_unsigned() const _NE
    {
        switch (type)
        {
            case value_type_t::int8_:
                return tvalue_t((uint8_t)value.int8_value);

            case value_type_t::int16_:
                return tvalue_t((uint16_t)value.int16_value);

            case value_type_t::int32_:
                return tvalue_t((uint32_t)value.int32_value);

            case value_type_t::int64_:
                return tvalue_t((uint64_t)value.int64_value);

            default:
                return *this;
        }
    }

    // Converts to signed type.
    tvalue_t tvalue_t::to_signed() const _NE
    {
        switch (type)
        {
            case value_type_t::uint8_:
                return tvalue_t((int8_t)value.uint8_value);

            case value_type_t::uint16_:
                return tvalue_t((int16_t)value.uint16_value);

            case value_type_t::uint32_:
                return tvalue_t((int32_t)value.uint32_value);

            case value_type_t::uint64_:
                return tvalue_t((int64_t)value.uint64_value);

            default:
                return *this;
        }
    }

    // Write a tvalue_t object into output stream
    ostream_t & operator << (ostream_t & stream, const tvalue_t & v)
    {
        #define __X_CASE(type)                                                          \
            case value_type_t::type##_:                                                 \
                stream << get_value<type##_t>(v.value);                                 \
                break;

        switch (v.type)
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

    // Converts error_t to string
    const string_t error_t::to_string() const
    {
        return this->get_message();
    }

    ////////// ////////// ////////// ////////// //////////
    // Class __error_base_t

    // Converts __error_base_t to string, with the poistion in which file and which line.
    const string_t __error_base_t::to_string() const
    {
#if X_DEBUG
        stringstream_t ss;

        ss << this->get_message().c_str();

        if (file.length() > 0)
        {
            ss << _T(" [") << file.c_str();

            if (line != (size_t)-1)
                ss << _T(":") << line;

            ss << _T("]");
        }

        ss << std::endl << std::endl << _T("call stack:") << std::endl << call_stack.c_str();
        return ss.str();
#else
        return this->get_message();
#endif
    }

    // Gets the error message.
    const string_t & __error_base_t::get_message() const
    {
        return this->message;
    }

    ////////// ////////// ////////// ////////// //////////

    // All descriptions of common_error_code_t
    X_ENUM_INFO(common_error_code_t)

        X_C(invalid_operation,  _T("invalid operation"))

        X_C(not_supported,      _T("not supported"))

        X_C(no_memory,          _T("no enough memory"))

        X_C(overflow,           _T("overflow"))

        X_C(argument_error,     _T("argument error"))

        X_C(format_error,       _T("format error"))

        X_C(convert_error,      _T("convert error"))

        X_C(not_found,          _T("not found"))

        X_C(unexpected,         _T("unexpected"))

        X_C(unimplemented,      _T("unimplemented"))

    X_ENUM_INFO_END

    // Creates an instance of unimplemented error. ( for constant objects. )
    logic_error_t<common_error_code_t> _unimplemented_error(const object_t * obj,
                                                            const char_t * method) _NE
    {
        string_t class_name = string_convert<char, char_t>(
            typeid(*const_cast<object_t *>(obj)).name()
        );

        return _E(common_error_code_t::unimplemented,
            _F(_T("%1%.%2% const not implemented"), class_name, method)
        );
    }

    // Creates an instance of unimplemented error.
    logic_error_t<common_error_code_t> _unimplemented_error(object_t * obj,
                                                     const char_t * method) _NE
    {
        string_t class_name = string_convert<char, char_t>(typeid(*obj).name());

        return _E(common_error_code_t::unimplemented,
            _F(_T("%1%.%2% not implemented"), class_name, method)
        );
    }

    // Returns current call stack.
    X_NO_INLINE string_t to_call_stack_string()
    {
        return arch::call_stack(2);
    }

    ////////// ////////// ////////// ////////// //////////
    // X_ASSERT

    // Raise assert error if the specified expression is false.
    void __raise_assert_error(const char_t * exp, const char_t * file, uint_t line,
                                    string_t && call_stack, const char_t * reason)
    {
        string_t msg;

        if (reason && reason[0])
            msg = sprintf(_T("Assert error: %1%, %2%"), exp, reason);
        else
            msg = sprintf(_T("Assert error: %1%"), exp);

        throw assert_error_t(std::move(msg)
#if X_DEBUG
            , file, line, std::move(call_stack)
#endif
        );
    }

    // Raise assert error if the specified expression is false.
    void __raise_assert_error(const char_t * exp, const char_t * file, uint_t line,
                                    string_t && call_stack, const string_t & reason)
    {
        return __raise_assert_error(exp, file, line, std::move(call_stack), reason.c_str());
    }

    ////////// ////////// ////////// ////////// //////////
    // memory_t

    // Descriptions of enum memory_flag_t

    X_ENUM_INFO(memory_flag_t)

        X_C(is_object)

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Returns the single instance of default memory manager.
    memory_t * memory_t::__default_memory()
    {
        return default_memory_t::instance();
    }

    ////////// ////////// ////////// ////////// //////////
    // default_memory_t

    // Allocates memory with the specified size, throws no_memory exception if failed.
    void * default_memory_t::alloc(size_t size, memory_flag_t flag)
    {
        void * bytes = ::operator new(size);
        if (!bytes)
            throw _E(common_error_code_t::no_memory);

        return bytes;
    }

    // Frees the specified memory.
    void default_memory_t::free(void * obj)
    {
        ::operator delete(obj);
    }

    // Reallocates memory with the specified size, throws no_memory expception if failed.
    void * default_memory_t::realloc(void * obj, size_t size, memory_flag_t flag)
    {
        free(obj);
        return alloc(size, flag);
    }

    // The single instance of default memory manager.
    default_memory_t * default_memory_t::instance()
    {
        static default_memory_t * memory = new default_memory_t();
        return memory;
    }

    ////////// ////////// ////////// ////////// //////////
    // pool_t

    // Allocates memory with the specified size, throws no_memory exception if failed.
    // If succeed, put the object into a set, then failed it in the decallocate function.
    void * pool_t::alloc(size_t size, memory_flag_t flag)
    {
        void * bytes = ::operator new(size);
        if (!bytes)
            throw _E(common_error_code_t::no_memory);

        if (bit_has_flag(flag, memory_flag_t::is_object))
            __objects.insert((object_t *)bytes);

        return bytes;
    }

    // Frees the specified memory.
    // Then remove the object from the pool.
    void pool_t::free(void * obj)
    {
        auto it = __objects.find((object_t *)obj);
        if (it != __objects.end())
        {
            (*it)->~object_t();
            __objects.erase(it);
        }

        ::operator delete(*it);
    }

    // Reallocates the specified memory. ( not supported )
    void * pool_t::realloc(void * obj, size_t size, memory_flag_t flag)
    {
        throw _E(common_error_code_t::not_supported);
    }

    // Deallocator of pool_t. 
    // Executes the deallocators for objects in the pool.
    pool_t::~pool_t()
    {
        for (object_t * obj : __objects)
        {
            obj->~object_t();

            ::operator delete(obj);
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Opens the specified file for reading, throw exceptions when failed.
    void file_xistream_t::open(const string_t & path, __open_mode_t open_mode)
    {
        __open_mode_t mode = (open_mode | __i::in) & ~(__i::out | __i::trunc);
        __stream.open(string_convert<char_t, char>(path), mode);

        if (!__stream.is_open())
            throw _ECF(invalid_operation, _T("cannot open file '%1%'"), path);
    }

    // Read bytes of specified size from the stream.
    size_t file_xistream_t::read(byte_t * buffer, size_t length)
    {
        if (buffer == nullptr)
            throw _EC(argument_error, _T("buffer is null"));

        return __stream.read((char *)buffer, length), __stream.gcount();
    }

    // Completed notify.
    void file_xistream_t::completed()
    {
        __stream.close();
    }

    // Deallocator
    file_xistream_t::~file_xistream_t()
    {
        __stream.close();
    }

    ////////// ////////// ////////// ////////// //////////

    // Opens the specified file for writing, throw exceptions when failed.
    void file_xostream_t::open(const string_t & path, __open_mode_t open_mode)
    {
        __open_mode_t mode = (open_mode | __i::out) & ~__i::in;
        __stream.open(string_convert<char_t, char>(path), mode);

        if (!__stream.is_open())
            throw _ECF(invalid_operation, _T("cannot open file '%1%'"), path);
    }

    // Writes bytes to the stream.
    void file_xostream_t::write(const byte_t * buffer, size_t length)
    {
        if (buffer == nullptr)
            throw _EC(argument_error, _T("buffer is null"));

        __stream.write((const char *)buffer, length);
    }

    // Completed notify.
    void file_xostream_t::completed()
    {
        __stream.close();
    }

    // Dealloctor
    file_xostream_t::~file_xostream_t()
    {
        __stream.close();
    }

    ////////// ////////// ////////// ////////// //////////

}    // namespace X_ROOT_NS
