
#ifndef __COMMON_STREAM_H__
#define __COMMON_STREAM_H__

#include <boost/format.hpp>

namespace X_ROOT_NS {

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        // Value type for writing to a stream.
        enum class __value_output_family_t { normal, object, string, string_like, enum_ };

        // Determined value type forwriting to a stream.
        template<typename t> constexpr __value_output_family_t __value_output_family()
        {
            typedef std::remove_const_t<
                std::remove_reference_t<remove_pointer_const_t<t>>
            > t0;

            // enum
            if (std::is_enum<t0>::value)
                return __value_output_family_t::enum_;

            // string_t
            if (std::is_same<t0, string_t>::value)
                return __value_output_family_t::string;

            // object_t
            if (std::is_convertible<t0, object_t>())
                return __value_output_family_t::object;

            // string_like: can convert to string_t. ( operator_t string_t defined. )
            if (std::is_convertible<t0, string_t>())
                return __value_output_family_t::string_like;

            // Otherwise ...
            return __value_output_family_t::normal;
        }

        //-------- ---------- ---------- ---------- ----------

        // A template base class for value convertion ( to string ).

        template<typename t>
        struct __value_wrapper_base_t
        {
        public:
            __value_wrapper_base_t(const t& value) : __value(value) { }

            const t& __value;

            const t& value() const { return __value; }
        };

        //-------- ---------- ---------- ---------- ----------

        // A template class for writing a value to a stream.

        template<typename t, __value_output_family_t type>
        struct __value_wrapper_t : public __value_wrapper_base_t<t>
        {
            using __value_wrapper_base_t<t>::__value_wrapper_base_t;

            template<typename stream_t> void write_to(stream_t & stream) const
            {
                stream << this->value;
            }
        };

        //-------- ---------- ---------- ---------- ----------

        // Writing a normal value to a stream, like int, long ...

        template<typename t>
        struct __value_wrapper_t<t, __value_output_family_t::normal>
            : public __value_wrapper_base_t<t>
        {
            using __value_wrapper_base_t<t>::__value_wrapper_base_t;

            template<typename stream_t> void write_to(stream_t & stream) const
            {
                stream << this->__value;
            }
        };

        //-------- ---------- ---------- ---------- ----------

        // Writing a string value to a stream

        template<typename t>
        struct __value_wrapper_t<t, __value_output_family_t::string>
            : public __value_wrapper_base_t<t>
        {
            using __value_wrapper_base_t<t>::__value_wrapper_base_t;

            template<typename stream_t> void write_to(stream_t & stream) const
            {
                stream << this->__value.c_str();
            }

            const string_t & value() const
            {
                return this->__value;
            }
        };

        //-------- ---------- ---------- ---------- ----------

        // Writing a string-like ( operator string_t() defined. ) value to a stream.

        template<typename t>
        struct __value_wrapper_t<t, __value_output_family_t::string_like>
            : public __value_wrapper_base_t<t>
        {
            using __value_wrapper_base_t<t>::__value_wrapper_base_t;

            template<typename stream_t> void write_to(stream_t & stream) const
            {
                stream << ((string_t)this->__value).c_str();
            }

            string_t value() const
            {
                return (string_t)this->__value;
            }
        };

        //-------- ---------- ---------- ---------- ----------

        // Writing an object to a stream. converts by calling to_string virtual function.

        template<typename t>
        struct __value_wrapper_t<t, __value_output_family_t::object>
            : public __value_wrapper_base_t<t>
        {
            using __value_wrapper_base_t<t>::__value_wrapper_base_t;

            template<typename stream_t> void write_to(stream_t & stream) const
            {
                stream << _str(this->__value);
            }
        };

        //-------- ---------- ---------- ---------- ----------

        // Writing an enum value to a stream, converts to its title.

        template<typename t>
        struct __value_wrapper_t<t, __value_output_family_t::enum_>
            : public __value_wrapper_base_t<t>
        {
            using __value_wrapper_base_t<t>::__value_wrapper_base_t;

            template<typename stream_t> void write_to(stream_t & stream) const
            {
                stream << _stitle(this->__value).c_str();
            }

            const string_t value() const
            {
                return _stitle(this->__value);
            }
        };

        //-------- ---------- ---------- ---------- ----------

        // Wrap a value to a various wrapper.

        template<typename t, __value_output_family_t type = __value_output_family<t>()>
        __value_wrapper_t<t, type> __wrap(const t & value)
        {
            return __value_wrapper_t<t, type>(value);
        }
    }

    //-------- ---------- ---------- ---------- ----------

    // Writing a value to a stream. with various convertions.

    template<typename stream_t, typename t, __value_output_family_t type>
    X_INLINE stream_t & operator << (stream_t & stream, const __value_wrapper_t<t, type> & value)
    {
        value.write_to(stream);
        return stream;
    }

    //-------- ---------- ---------- ---------- ----------

    namespace
    {
        typedef boost::basic_format<char_t> __formater_t;
        typedef common_error_code_t         __ec_t;

        // Creates an format exception for conversion error.
        template<typename ec_t, typename format_t>
        X_INLINE logic_error_t<ec_t> __exception(ec_t error_code, const format_t & format,
                                                    const std::exception * e = nullptr)
        {
            stringstream_t ss;
            ss << _T("{ ") << format << _T(" }");

            if (e != nullptr)
                ss << _T(":") << string_convert<char, char_t>(e->what()).c_str();

            return _E(error_code, ss.str());
        }

        X_INLINE void __sprintf(__formater_t & formater) { }

        // Generates a string with the given format and value.
        template<typename value_t>
        X_INLINE void __sprintf(__formater_t & formater, value_t && value)
        {
            formater % __wrap(value).value();
        }

        // Generates a string with the given format and arguments.
        template<typename first_t, typename ... args_t>
        X_INLINE void __sprintf(__formater_t & formater, first_t && first, args_t && ... args)
        {
            __sprintf(formater, std::forward<first_t>(first));
            __sprintf(formater, std::forward<args_t>(args) ...);
        }
    }

    //-------- ---------- ---------- ---------- ----------

    // Generates a string with given format and arguments.
    template<typename format_t, typename ... args_t>
    string_t sprintf(format_t && format, args_t && ... args)
    {
        try
        {
            __formater_t f(format);
            __sprintf(f, std::forward<args_t>(args) ...);

            return f.str();
        }
        catch (const boost::io::format_error & e)
        {
            throw __exception(__ec_t::format_error, format, &e);
        }
        catch (const std::exception & e)
        {
            throw __exception(__ec_t::unexpected, format, &e);
        }
        catch (...)
        {
            throw __exception(__ec_t::unexpected, format);
        }
    }

    //-------- ---------- ---------- ---------- ----------

    // Writes a format string to a stream.
    template<typename stream_t, typename format_t, typename ... args_t>
    void tprintf(stream_t & stream, format_t && format, args_t && ... args)
    {
        string_t s = sprintf(std::forward<format_t>(format), std::forward<args_t>(args) ...);
        stream << s.c_str();
    }

    // Writes a format string to standard output stream.
    template<typename format_t, typename ... args_t>
    void printf(format_t && format, args_t && ... args)
    {
        tprintf(cout, std::forward<format_t>(format), std::forward<args_t>(args) ...);
    }

    //-------- ---------- ---------- ---------- ----------

    template<typename stream_t> void tprint(stream_t & stream) { }

    // Writes values to a stream.
    template<typename stream_t, typename value_t>
    void tprint(stream_t & stream, value_t && value)
    {
        stream << __wrap(value);
    }

    // Writes values to a stream.
    template<typename stream_t, typename first_t, typename ... rest_t>
    void tprint(stream_t & stream, first_t && first, rest_t && ... rest)
    {
        tprint(stream, std::forward<first_t>(first));
        tprint(stream, _T(" "));
        tprint(stream, std::forward<rest_t>(rest) ...);
    }

    //-------- ---------- ---------- ---------- ----------

    // Generates a string by specified arguents, splitted by whitespaces.
    template<typename ... args_t>
    string_t sprint(args_t && ... args)
    {
        typedef std::basic_ostringstream<char_t> sstream_t;
        sstream_t ss;
        tprint(ss, std::forward<args_t>(args) ...);
        return std::move(ss.str());
    }

    //-------- ---------- ---------- ---------- ----------

    // Writes values to standard output stream, splitted by whitespaces.
    template<typename ... args_t>
    void print(args_t && ... args)
    {
        tprint(cout, std::forward<args_t>(args) ...);   
    }

    // Writes values to standard output stream, splitted by whitespaces, with \n at the end.
    template<typename ... args_t>
    void println(args_t && ... args)
    {
        print(std::forward<args_t>(args) ...);
        print(_T("\n"));
    }

    ////////// ////////// ////////// ////////// //////////

}  // X_ROOT_NS


#endif // __COMMON_STREAM_H__

