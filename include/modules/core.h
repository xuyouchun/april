#ifndef __CORE_H__
#define __CORE_H__

#include <common.h>
#include <algorithm.h>
#include <jc.h>
#include <arch.h>

namespace X_ROOT_NS { namespace modules { namespace core {

    #define UNCERTERN_PARAM_COUNT   -1

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        using namespace al;
        namespace __ns = X_ROOT_NS::modules::core;

        template<typename t, size_t init_size = 2>
        using __vector_t = al::svector_t<t, init_size>;
    }

    class xpool_t;
    class statement_compile_context_t;
    class method_compile_context_t;
    class xilx_write_context_t;

    class statement_region_t;
    class variable_region_t;

    class general_type_t;
    class generic_type_t;

    class cvalue_expression_t;

    typedef int16_t element_value_t;

    // Core assembly name.
    #define CoreAssembly  _T("System")

    // Defines core type names.
    #define __CoreTypeName(name) CoreAssembly _T(".") name

    // Common types.
    #define CoreType_Type         _T("Type")
    #define CoreType_Exception    _T("Exception")
    #define CoreType_Void         _T("Void")
    #define CoreType_Object       _T("Object")
    #define CoreType_Ptr          _T("Ptr")
    #define CoreType_Attribute    _T("Attribute")
    #define CoreType_Array        _T("Array")
    #define CoreType_TArray       _T("Array<>")
    #define CoreType_ITuple       _T("ITuple<...>")
    #define CoreType_Tuple        _T("Tuple<...>")
    #define CoreType_Delegate     _T("Delegate<,...>")
    #define CoreType_MulticastDelegate _T("MulticastDelegate<,...>")
    #define CoreType_Enum         _T("Enum")
    #define CoreType_Int8         _T("Int8")
    #define CoreType_UInt8        _T("UInt8")
    #define CoreType_Int16        _T("Int16")
    #define CoreType_UInt16       _T("UInt16")
    #define CoreType_Int32        _T("Int32")
    #define CoreType_UInt32       _T("UInt32")
    #define CoreType_Int64        _T("Int64")
    #define CoreType_UInt64       _T("UInt64")
    #define CoreType_Boolean      _T("Boolean")
    #define CoreType_Char         _T("Char")
    #define CoreType_String       _T("String")
    #define CoreType_Single       _T("Single")
    #define CoreType_Double       _T("Double")

    // Reflection.
    #define CoreType_ReflectionInfo     _T("Reflection.ReflectionInfo")
    #define CoreType_Assembly           _T("Reflection.Assembly")
    #define CoreType_Member             _T("Reflection.Member")
    #define CoreType_Method             _T("Reflection.Method")
    #define CoreType_Property           _T("Reflection.Property")
    #define CoreType_Event              _T("Reflection.Event")
    #define CoreType_Field              _T("Reflection.Field")
    #define CoreType_Parameter          _T("Reflection.Parameter")
    #define CoreType_GenericParameter   _T("Reflection.GenericParameter")

    // Exceptions.
    #define CoreType_NullReferenceException     _T("NullReferenceException")

    // Attributes.
    #define CoreType_TraceAttribute     _T("Diagnostics.TraceAttribute")

    ////////// ////////// ////////// ////////// //////////

    // Version for assembly.
    struct version_t : compare_operators_t<version_t, int64_t>
    {
        constexpr version_t() : version_t(0, 0, 0, 0) { }
        constexpr version_t(int8_t major, int8_t minor, int16_t build, int16_t revision)
            : major(major), minor(minor), build(build), revision(revision), __reserved__(0)
        { }

        int8_t          major;              // Major version.
        int8_t          minor;              // Minor version.
        int16_t         build;              // Build version.
        int16_t         revision;           // Revision verison.
        int16_t         __reserved__;

        operator int64_t() const { return *(int64_t *)this; }

        // Converts to string.
        operator string_t() const
        {
            return _F(_T("%1%.%2%.%3%.%4%"), major, minor, build, revision);
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // instance_t

    class type_t;
    class runtime_mobject_t { };

    ////////// ////////// ////////// ////////// //////////

    typedef uint16_t msize_t;
    const msize_t unknown_msize = (msize_t)-1;

    ////////// ////////// ////////// ////////// //////////

    // Collection which elements has a owner.
    // Auto set the owner when push elements into it.
    template<typename owner_t, typename t, size_t init_size,
        typename _super_t = __vector_t<t, init_size>
    >
    class owner_collection_t : public _super_t
    {
        typedef _super_t __super_t;

    public:
        owner_collection_t(owner_t owner) : __owner(owner) { }

        // Append a element, auto set owner.
        void push_back(t obj)
        {
            if (obj != nullptr)
                obj->set_owner(__owner);

            __super_t::push_back(obj);
        }

    private:
        owner_t __owner;
    };

    ////////// ////////// ////////// ////////// //////////

    // Code file.
    X_INTERFACE code_file_t 
    {
        // Source code.
        virtual const char_t * get_source_code() const = 0;

        // Name.
        virtual const string_t & get_file_name() const = 0;

        // Path.
        virtual const string_t & get_file_path() const = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    // Code unit structure.
    struct code_unit_t
    {
        code_unit_t() = default;
        code_unit_t(const code_unit_t &) = default;

        code_unit_t(const char_t * s, int32_t length, const code_file_t * file)
            : s(s), length(length), file(file)
        { }

        const char_t *  s;          // Code position
        int32_t         length;     // Code length
        const code_file_t * file;    // Code file

        // Returns current line.
        string_t current_line() const;

        // Returns current line position.
        std::pair<const char_t *, const char_t *> current_line_pos() const;

        // Converts to a string.
        operator string_t() const
        {
            return string_t(s, length);
        }
    };

    // Writes code_unit_t to a stream.
    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const code_unit_t & unit)
    {
        return stream << string_t(unit.s, unit.length);
    }

    ////////// ////////// ////////// ////////// //////////

    // Code element with code unit.
    class code_element_t
    {
    public:
        code_element_t() = default;
        code_element_t(code_unit_t * unit) : code_unit(unit) { }

        code_unit_t * code_unit = nullptr;
    };

    // Combines code elements, its code unit will combined.
    code_element_t * combine(memory_t * memory, code_element_t * from, code_element_t * to);

    ////////// ////////// ////////// ////////// //////////

    // Code element wrapper, append code_unit to an object.
    template<typename obj_t>
    class code_element_wrapper_t : public obj_t, public code_element_t
    {
        typedef obj_t __super_t;

    public:
        using __super_t::__super_t;
    };

    ////////// ////////// ////////// ////////// //////////
    // logger_t

    // Log levels
    X_ENUM(log_level_t)

        // Information
        info,

        // Warning
        warning,

        // Error
        error,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Logger interface for record log of compilers.
    X_INTERFACE logger_t
    {
        // Log inforation / warning / error messages.
        virtual void log(code_element_t * element, log_level_t level,
                                const string_t & name, const string_t & message) = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    // A set of useful functions for logger_t.
    class xlogger_t
    {
    public:
        xlogger_t(logger_t & logger) : logger(logger) { }

        // Inner logger.
        logger_t & logger;

        // Log error
        template<typename code_t>
        void log_error(code_element_t * element, code_t code, const string_t & message)
        {
            logger.log(element, log_level_t::error, _title(code), message);
        }

        // Log error with format.
        template<typename code_t, typename ... args_t>
        void log_errorf(code_element_t * element, code_t code, args_t && ... args)
        {
            log_error(element, code, _F(_desc(code), std::forward<args_t>(args) ...));
        }

        // Log warning
        template<typename code_t>
        void log_warning(code_element_t * element, code_t code, const string_t & message)
        {
            logger.log(element, log_level_t::warning, _title(code), message);
        }

        // Log warning with format.
        template<typename code_t, typename ... args_t>
        void log_warningf(code_element_t * element, code_t code, args_t && ... args)
        {
            log_warning(element, code, _F(_desc(code), std::forward<args_t>(args) ...));
        }

        // Log info
        template<typename code_t>
        void log_info(code_element_t * element, code_t code, const string_t & message)
        {
            logger.log(element, log_level_t::info, _title(code), message);
        }

        // Log info with format.
        template<typename code_t, typename ... args_t>
        void log_infof(code_element_t * element, code_t code, args_t && ... args)
        {
            log_info(element, code, _F(_desc(code), std::forward<args_t>(args) ...));
        }

        // Log message.
        template<typename code_t>
        void log(code_element_t * element, code_t code, const string_t & message)
        {
            __log(element, code, message);
        }

        // Log message.
        template<typename code_t>
        void log(code_element_t * element, log_level_t level, code_t code, const string_t & message)
        {
            logger.log(element, level, _title(code), message);
        }

        // Log message with format.
        template<typename code_t, typename ... args_t>
        void logf(code_element_t * element, code_t code, args_t && ... args)
        {
            __log(element, code, _F(_desc(code), std::forward<args_t>(args) ...));
        }

        // Log message with format.
        template<typename code_t, typename ... args_t>
        void logf(code_element_t * element, log_level_t level, code_t code, args_t && ... args)
        {
            logger.log(element, level, _title(code),
                _F(_desc(code), std::forward<args_t>(args) ...)
            );
        }

    private:
        template<typename code_t>
        void __log(code_element_t * element, code_t code, const string_t & message)
        {
            #define __Code(_code_) ((code_t)_code_)
            typedef code_t  c_t;

            _A(__Code(code) > __Code(c_t::__info__) && __Code(code) < __Code(c_t::__the_end__));

            log_level_t level;
            #define __Between(begin, end) \
                (__Code(code) >= __Code(c_t::begin) && __Code(code) < __Code(c_t::end))

            if (__Between(__info__, __warning__))
                level = log_level_t::info;
            else if (__Between(__warning__, __error__))
                level = log_level_t::warning;
            else if (__Between(__error__, __the_end__))
                level = log_level_t::error;
            else
                throw _ECF(argument_error, _T("argument %1% out of range"), __Code(code));

            logger.log(element, level, _title(code), message);

            #undef __Code
            #undef __Between
        }
    };

    // Compiler log.
    template<typename code_t, typename ... args_t>
    void cpl_logf(logger_t & logger, object_t * object, log_level_t level,
                                            code_t code, args_t && ... args)
    {
        xlogger_t(logger).logf(as<code_element_t *>(object),
            level, code, std::forward<args_t>(args) ...
        );
    }

    // Compiler log_errorf.
    template<typename code_t, typename ... args_t>
    void cpl_log_errorf(logger_t & logger, object_t * object, code_t code, args_t && ... args)
    {
        cpl_logf(logger, object, log_level_t::error, code, std::forward<args_t>(args) ...);
    }

    // Compiler log_warningf.
    template<typename code_t, typename ... args_t>
    void cpl_log_warningf(logger_t & logger, object_t * object, code_t code, args_t && ... args)
    {
        cpl_logf(logger, object, log_level_t::warning, code, std::forward<args_t>(args) ...);
    }

    // Compiler log_infof.
    template<typename code_t, typename ... args_t>
    void cpl_log_infof(logger_t & logger, object_t * object, code_t code, args_t && ... args)
    {
        cpl_logf(logger, object, log_level_t::info, code, std::forward<args_t>(args) ...);
    }

    // Compiler log.
    template<typename code_t, typename ... args_t>
    void cpl_log(logger_t & logger, object_t * object, log_level_t level,
                                        code_t code, const string_t & message)
    {
        xlogger_t(logger).log(as<code_element_t *>(object), level, code, message);
    }

    // Compiler log error.
    template<typename code_t, typename ... args_t>
    void cpl_log_error(logger_t & logger, object_t * object, code_t code, const string_t & message)
    {
        xlogger_t(logger).log_error(as<code_element_t *>(object), code, message);
    }

    //-------- ---------- ---------- ---------- ----------

    // A logger collection.
    class loggers_t : public logger_t
    {
    public:
        loggers_t(std::initializer_list<logger_t *> loggers) : __loggers(loggers) { }

        // Log messages.
        virtual void log(code_element_t * element, log_level_t level,
                                const string_t & name, const string_t & message)
        {
            for (logger_t * logger : __loggers)
            {
                logger->log(element, level, name, message);
            }
        }

    private:
        std::vector<logger_t *> __loggers;
    };

    ////////// ////////// ////////// ////////// //////////

    // A context when commit eobject_t.
    class eobject_commit_context_t : public object_t
    {
    public:
        eobject_commit_context_t(logger_t & logger)
            : logger(logger)
        { }

        logger_t &  logger;

        X_TO_STRING_IMPL(_T("eobject_commit_context_t"))
    };

    //-------- ---------- ---------- ---------- ----------

    // A base class for all codedom nodes.
    class eobject_t : public object_t
    {
    public:
        // Commits
        virtual void commit(eobject_commit_context_t & ctx) { }

        X_TO_STRING_IMPL(_T("eobject_t"))
    };

    //-------- ---------- ---------- ---------- ----------

    // Codedom types
    X_ENUM(efamily_t)

        assembly, module, document, namespace_, type, method, property, field, event, type_def,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    // Struct token_t
    typedef element_value_t token_value_t;
    typedef element_value_t ast_value_t;

    const token_value_t   unknown_token_value = -1;
    const element_value_t document_ast_value  = -2;
    const element_value_t module_ast_value    = -3;

    struct token_data_t;

    // Token in source codes.
    struct token_t : code_element_t
    {
        // Constructors.
        token_t(token_value_t value, const char_t * s, int32_t length,
            token_data_t * data = nullptr, const code_file_t * file = nullptr)
            : code_element_t(&code_unit), code_unit(s, length, file), value(value), data(data)
        { } 

        // Constructors.
        token_t() = default; 
        token_t(const token_t &o) = default;

        // Token data.
        token_data_t * data;

        // Token value.
        token_value_t value;

        // Converts to string.
        operator string_t() const { return (string_t)code_unit; }

        // Code unit.
        code_unit_t code_unit;

        // Converts to code_unit_t *.
        operator code_unit_t * () { return &code_unit; }
        operator const code_unit_t * () const { return &code_unit; }
    };

    ostream_t & operator << (ostream_t & stream, const token_t & w);

    //-------- ---------- ---------- ---------- ----------

    // Token error codes.
    X_ENUM(token_error_code_t)

        // Missing comment end.
        comment_end_missing,

        // Token unknowned.
        unknown_token,

        // Token string format error.
        string_format_error,

        // Token character format error.
        char_format_error,

        // Token numeric format error.
        numeric_format_error,

        // Token numeric overflow.
        numeric_overflow,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Compiler error codes.
    X_ENUM(compile_error_code_t)

        // Duplicated entry points, two main method.
        duplicated_entry_point,

        // Entry point method format error: should no return type.
        entry_point_prototype_mistake__should_no_return_type,

        // Entry point method format error: should not geneirc.
        entry_point_prototype_mistake__should_not_generic,

        // Entry point method format error: should no params.
        entry_point_prototype_mistake__should_no_params,

        // Entry point method format error: should be public and static.
        entry_point_prototype_mistake__should_be_public_static,

        // Entry point method format error.
        entry_point_prototype_mistake__decorate_not_allowed,

        // Extern method format error: shoud be static..
        extern_method_prototype_mistake__should_be_static,

        // Extern method format error: shoud be marked with extern..
        extern_method_prototype_mistake__should_be_extern,

        // Extern method format error: not allowed.
        extern_method_prototype_mistake__not_allowed,

        // Type not found.
        type_not_found,

        // Label not found.
        label_not_found,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Token error
    struct token_error_t
    {
        token_error_t() = default;
        token_error_t(const token_error_t &) = default;

        token_error_t(token_error_code_t code, const char_t * message=nullptr)
            : code(code), message(message? message : _title(code)) { }

        // Error code
        token_error_code_t code;

        // Error message
        const char_t * message;
    };

    //-------- ---------- ---------- ---------- ----------

    #define __TTypeItem(name) name = (int8_t)value_type_t::name

    // Value types
    X_ENUM(vtype_t)

        __TTypeItem(int8_),

        __TTypeItem(uint8_),

        __TTypeItem(int16_),

        __TTypeItem(uint16_),

        __TTypeItem(int32_),

        __TTypeItem(uint32_),

        __TTypeItem(int64_),

        __TTypeItem(uint64_),

        __TTypeItem(float_),

        __TTypeItem(double_),

        __TTypeItem(bool_),

        __TTypeItem(char_),

        ptr_,

        string_,

        void_,

        mobject_,

    X_ENUM_END

    #undef __TTypeItem

    //-------- ---------- ---------- ---------- ----------

    // Returns whether it's a signed int value.
    X_INLINE bool is_signed_integer(vtype_t vtype) _NE
    {
        return al::in(vtype, vtype_t::int8_, vtype_t::int16_, vtype_t::int32_, vtype_t::int64_);
    }

    // Returns whether it's a unsigned int value.
    X_INLINE bool is_unsigned_integer(vtype_t vtype) _NE
    {
        return al::in(vtype, vtype_t::uint8_, vtype_t::uint16_, vtype_t::uint32_, vtype_t::uint64_);
    }

    // Returns whether it's a int (signed or unsigned) value.
    X_INLINE bool is_integer(vtype_t vtype) _NE
    {
        return vtype >= vtype_t::int8_ && vtype <= vtype_t::uint64_;
    }

    // Returns whether it's a float value. (float or double)
    X_INLINE bool is_float(vtype_t vtype) _NE
    {
        return al::in(vtype, vtype_t::float_, vtype_t::double_);
    }

    // Returns max value of vtype_t.
    const void * get_max_value(vtype_t vtype);

    // Returns min value of vtype_t.
    const void * get_min_value(vtype_t vtype);

    //-------- ---------- ---------- ---------- ----------

    // Constant value type

    X_ENUM(cvalue_type_t)

        // Numeric
        number,

        // String
        string,

        // Null
        null,

        // Type
        type,

        // Nan: unknown type
        nan,

    X_ENUM_END


    //-------- ---------- ---------- ---------- ----------

    // Token data
    struct token_data_t
    {
        token_data_t() : error(nullptr), value(nullptr),
            value_type(cvalue_type_t::__unknown__) { }

        token_data_t(const token_data_t &) = default;

        token_error_t  * error;         // Error
        const void     * value;         // Value
        cvalue_type_t    value_type;    // Value type

        // Possible values, a value array, ends with nullptr.
        const token_value_t * possible_values = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////

    typedef uint16_t operator_priority_t;       // Operator priority, small value is higher.
    typedef uint8_t  operator_arity_t;          // Operator arity, 1 or 2.
    typedef uint8_t  operator_adhere_t;         // Operator position in a expression. 0 or 1.

    //-------- ---------- ---------- ---------- ----------

    // Token properties
    struct token_property_t
    {
        bool is_operator        : 1;    // Is a operator
        bool is_expression_box  : 1;    // Is a side of a expression, e.g. '(' ')'
        bool is_invisible       : 1;    // Invisible, e.g. notes
    };

    //-------- ---------- ---------- ---------- ----------

    // Operators
    X_ENUM_(operator_t, sizeof(token_value_t))
    
        add, sub, mul, div, mod,                                            // + - * / %

        add_assign, sub_assign, mul_assign, div_assign, mod_assign,         // += -= *= /= %=

        left_increment, left_decrement, right_increment, right_decrement,   // ++x --x x++ x--

        minus, positive,                                                    // - +

        left_shift, right_shift, left_shift_assign, right_shift_assign,     // << >> <<= >>=

        bit_and, bit_or, bit_not, bit_xor,                                  // & | ~ ^

        bit_and_assign, bit_or_assign, bit_xor_assign,                      // &= |= ^=

        assign,                                                             // =

        greater,        greater_equal,  less,   less_equal,                 // > >= < <=

        equal,          not_equal,                                          // == !=

        logic_and,      logic_or,       logic_not,                          // && || !

        member_point,                                                       // .

        __inner_end__,

        __user_defined__ = 1024,

    X_ENUM_END

    // Returns whether the operator is a system defined operator.
    X_INLINE bool is_system_operator(operator_t op)
    {
        return _int(op) < _int(operator_t::__user_defined__);
    }

    //-------- ---------- ---------- ---------- ----------

    // Operator director
    X_ENUM(operator_direct_t)

        left_to_right = __default__,    // Left to right

        right_to_left,                  // Right to left, e.g. =, +=

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Operator overload model.
    X_ENUM(operator_overload_model_t)

        // Can be overloaded.
        overload = __default__,

        // Cannot be overloaded.
        no_overloaded,

        // Cannot be overloaded, and need no to check. such as "as, is" operators.
        no_check,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Operator prooerties structure.
    struct operator_property_t
    {
        operator_t              op;         // The operator
        operator_direct_t       direct;     // Operator direction
        operator_priority_t     priority;   // Operator priority
        operator_arity_t        arity;      // Operator arity
        operator_adhere_t       adhere;     // Operator position in a expression.
        bool                    is_assign;  // Operator is a assign operator, e.g. =, +=
        operator_overload_model_t overload; // Operator can be overloaded.
        const char_t *          name;       // Operator name.

        // Specified the parent must be this operator, be used for " ? : " operator.
        operator_t              parent_op   = operator_t::__default__;

        // Converts to string.
        operator string_t() const;
    };

    // Gets system operator property, returns nullptr if not a system operator.
    const operator_property_t * get_system_operator_property(operator_t op);

    //-------- ---------- ---------- ---------- ----------

    // Expression box side, left or right.
    // E.g. '(' is left side,  ')' is right side.
    X_ENUM(expression_box_side_t)

        // E.g. '(', ']'
        left        = __default__,

        // E.g. ')', ']'
        right,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Expression box property
    struct expression_box_property_t
    {
        expression_box_side_t   side;           // Left/right side
        token_value_t           pair_to;        // Pair to which token, e.g. '(' pair to ')'
    };

    ////////// ////////// ////////// ////////// //////////

    // Common defined branch values
    X_ENUM_(common_branch_value_t, sizeof(element_value_t))
    
        import,

        using_,

        expression,

        statement,

        type_name,

        cvalue,

        attribute,

        attributes,

        type,

        field,

        property,

        method,

        event,

        __user_defined__ = 1024,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    // eobject ast type, a eobject or a eobject collection.
    X_ENUM(eobject_ast_type_t)

        eobject,        // eobject
        
        eobjects,       // eobject collection.

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    struct code_unit_t;

    // eobject interface.
    X_INTERFACE eobject_ast_base_t
    {
        // Returns eobject type.
        virtual eobject_ast_type_t get_type() = 0;

        // Walks each eobjects, apply to callback function f for each element.
        virtual void each_eobjects(std::function<void (eobject_t *)> f) = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    // Base class for eobject_ast_base_t interface with eobject type.
    template<typename _etype_t>
    X_INTERFACE teobject_ast_base_t : eobject_ast_base_t
    {
        typedef _etype_t etype_t;
    };

    //-------- ---------- ---------- ---------- ----------

    // Base class for eobject_ast_base_t interface with eobject type.
    template<typename _etype_t>
    X_INTERFACE eobject_ast_t : teobject_ast_base_t<_etype_t>
    {
        // Returns this eobject.
        virtual _etype_t to_eobject() = 0;

        // Returns eobject type.
        virtual eobject_ast_type_t get_type() override final
        {
            return eobject_ast_type_t::eobject;
        }

        // Walks each ebojects, apply to callback function f for each element.
        virtual void each_eobjects(std::function<void (eobject_t *)> f) override
        {
            f(to_eobject());
        }
    };

    //-------- ---------- ---------- ---------- ----------

    template<typename _etype_t> X_INTERFACE eobjects_ast_t;

    // eobject iterator.
    template<typename _etype_t>
    struct eobjects_ast_iterator_t
    {
        typedef eobjects_ast_t<_etype_t> container_type;
        typedef _etype_t value_type;
        typedef eobjects_ast_iterator_t<_etype_t> __self_t;

        // Constructor.
        eobjects_ast_iterator_t(const container_type & container, size_t index)
            : __container(container), __index(index)
        { }

        // Returns eobject.
        _etype_t operator *() const { return __container.eobject_at(__index); }

        // Moves to next iterator.
        __self_t & operator ++()
        {
            __index++;
            return *this;
        }

        // Moves to next iterator.
        __self_t operator ++(int)
        {
            __self_t r = *this;
            __index++;
            return r;
        }

        // Moves to previous iterator.
        __self_t & operator --()
        {
            __index--;
            return *this;
        }

        // Moves to previous iterator.
        __self_t operator --(int)
        {
            __self_t r = *this;
            __index--;
            return r;
        }

        // Returns whether two iterator is equals.
        bool operator == (const __self_t & o) const { return __index == o.__index; }

        // Returns whether two iterators is not equals.
        bool operator != (const __self_t & o) const { return __index != o.__index; }

    private:
        const container_type & __container;
        size_t __index;
    };

    //-------- ---------- ---------- ---------- ----------

    // eobject collection interface.
    template<typename _etype_t>
    X_INTERFACE eobjects_ast_t : teobject_ast_base_t<_etype_t>
    {
        typedef eobjects_ast_iterator_t<_etype_t> iterator;
        typedef _etype_t value_type;

        // Returns eobject at the index.
        virtual _etype_t eobject_at(size_t index) const = 0;

        // Returns eobject count.
        virtual size_t   eobject_count() const = 0;

        // Returns the begin iterator.
        iterator begin() const { return iterator(*this, 0); }

        // Returns the end iterator.
        iterator end()   const { return iterator(*this, eobject_count()); }

        // Returns eobject count.
        size_t   size()  const { return eobject_count(); }

        // Returns whether the collection is empty.
        bool     empty() const { return size() == 0; }

        // Returns eobject type.
        virtual eobject_ast_type_t get_type() override final
        {
            return eobject_ast_type_t::eobjects;
        }

        // Walks each eobject, Applies each element for callback function f.
        virtual void each_eobjects(std::function<void (eobject_t *)> f) override
        {
            for (size_t index = 0, count = eobject_count(); index < count; index++)
            {
                f(eobject_at(index));
            }
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Implements for eobject_t with specified etype.
    template<typename _etype_t>
    class eobjects_t : public eobject_t
    {
    public:
        typedef _etype_t etype_t;
        typedef etype_t  value_type;
        typedef eobjects_t<_etype_t> * itype_t;

        typedef __vector_t<etype_t> container_t;
        typedef typename container_t::iterator iterator;

        // Constructors
        eobjects_t() = default;
        eobjects_t(std::initializer_list<etype_t> il)
        {
            std::copy(std::begin(il), std::end(il), std::back_inserter(*this));
        }

        eobjects_t(etype_t eobject)
        {
            push_back(eobject);
        }

        container_t eobjects;

        // Append a new eobject at back.
        void push_back(etype_t eobject)
        {
            eobjects.push_back(eobject);
        }

        // Append a new eobject at front.
        template<typename _eobjects_t>
        void push_front(_eobjects_t & objs)
        {
            eobjects.push_front(objs);
        }

        // Returns the begin iterator.
        auto begin() const { return eobjects.begin(); }

        // Returns the end iterator.
        auto end()   const { return eobjects.end();   }

        // Returns whether the collection is empty.
        bool empty() const { return eobjects.empty(); }

        // Returns count of the collection.
        size_t size()  const { return eobjects.size();  }

        // Returns the eobject at specified index.
        etype_t & operator[](size_t index) { return eobjects[index]; }

        // Returns the eobject at specified index.
        etype_t & at(size_t index) { return eobjects[index]; }

        // Returns the eobject at specified index.
        const etype_t & operator[](size_t index) const { return eobjects[index]; }

        // Returns the eobject at specified index.
        const etype_t & at(size_t index) const { return eobjects[index]; }

        // Converts to string.
        operator string_t() const
        {
            return al::join_str(begin(), end(), _T(", "), [](auto & it) {
                return it? _str(it) : empty_str;
            });
        }

        // Converts to string.
        virtual const string_t to_string() const override { return (string_t)*this; }
    };

    // Returns element count of the collection, returns 0 if nullptr.
    template<typename _etype_t> size_t size_of(eobjects_t<_etype_t> * objs)
    {
        return objs == nullptr? 0 : objs->size();
    }

    // Returns whether the collection is empty of nullptr.
    template<typename _etype_t> size_t is_empty(eobjects_t<_etype_t> * objs)
    {
        return size_of(objs) == 0;
    }

    //-------- ---------- ---------- ---------- ----------

    // A eobject collection with owner.
    template<typename _etype_t, typename _owner_t = eobject_t *>
    class owner_eobjects_t : public eobjects_t<_etype_t>
    {
        typedef eobjects_t<_etype_t> __super_t;

    public:
        using __super_t::__super_t;
        typedef _owner_t owner_t;

        // Sets the owner of eobjects.
        void set_owner(_owner_t owner)
        {
            __owner = owner;

            for (_etype_t eobj : *this)
            {
                eobj->set_owner(owner);
            }
        }

        // Gets the owner.
        _owner_t get_owner() { return __owner; }

    private:
        _owner_t __owner = nullptr;
    };

    //-------- ---------- ---------- ---------- ----------

    // Base class of a eobject with index.
    class with_index_t
    {
    public:
        int index = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    // Operation of indexed eobject when append.
    struct __indexed_eobjects_operator_t
    {
        // Sets the index when append.
        template<typename _eobjects_t, typename _eobject_t = typename _eobjects_t::etype_t>
        static void on_append(_eobjects_t & eobjects, _eobject_t obj)
        {
            if (obj != nullptr)
                obj->index = eobjects.size();
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Operation of indexed eobject when append.
    template<typename operator_t, typename ... operators_t>
    struct __eobjects_operators_t
    {
        // Sets the index when append.
        template<typename _eobjects_t, typename _eobject_t = typename _eobjects_t::etype_t>
        static void on_append(_eobjects_t & eobjects, _eobject_t obj)
        {
            operator_t::on_append(eobjects, obj);
            __eobjects_operators_t<operators_t ...>::on_append(eobjects, obj);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Operation of eobject when append.
    template<typename operator_t> struct __eobjects_operators_t<operator_t>
    {
        // Appends eobject.
        template<typename _eobjects_t, typename _eobject_t = typename _eobjects_t::etype_t>
        static void on_append(_eobjects_t & eobjects, _eobject_t obj)
        {
            operator_t::on_append(eobjects, obj);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // eobject collection with push_back method.
    template<typename eobject_t, typename ... operator_t>
    struct xeobjects_t : eobjects_t<eobject_t>
    {
        typedef eobjects_t<eobject_t> __super_t;

        using __super_t::__super_t;

        void push_back(eobject_t obj)
        {
            __eobjects_operators_t<operator_t ...>::on_append(*this, obj);
            __super_t::push_back(obj);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Indexed eobject collection.
    template<typename eobject_t>
    using indexed_eobjects_t = xeobjects_t<eobject_t, __indexed_eobjects_operator_t>;

    ////////// ////////// ////////// ////////// //////////

    class ast_node_t;
    typedef eobjects_ast_t<ast_node_t *> asts_t;

    ////////// ////////// ////////// ////////// //////////

    // Wrapper of sid_t for various types of name.
    class name_t : public compare_operators_t<name_t, sid_t>
    {
    public:
        constexpr name_t() : sid(sid_t::null) { }
        name_t(const name_t &) = default;
        constexpr explicit name_t(const sid_t & sid) : sid(sid) { }

        operator sid_t () const { return sid; }
        operator string_t () const { return (string_t)sid; }

        bool empty() const { return sid == sid_t::null; }

        sid_t sid;

        const char_t * c_str() const { return sid.c_str(); }

        static const name_t null;
    };

    ////////// ////////// ////////// ////////// //////////

    class mname_t;
    typedef kv_object_t<sid_t, const mname_t *> __mname_cache_t;

    // Multi-name operation context.
    struct mname_operate_context_t
    {
        mname_operate_context_t(al::spool_t & spool, memory_t * memory = nullptr,
                __mname_cache_t * cache = nullptr)
            : spool(spool), memory(memory), cache(cache)
        { }

        memory_t *        memory = nullptr;
        al::spool_t &     spool;
        __mname_cache_t * cache = nullptr;

        // Gets from cache, auto created by load_func it if not found.
        template<typename load_func_t>
        const mname_t * from_cache(const sid_t & sid, load_func_t load_func)
        {
            const mname_t * name;
            if (cache != nullptr && (name = cache->get(sid)) != nullptr)
                return name;

            name = load_func();

            if (cache != nullptr)
                cache->set(sid, name);

            return name;
        }

        // New empty multi-name.
        mname_t * new_mname();

        // New multi-name with sid.
        mname_t * new_mname(const sid_t & sid);

        // New multi-name with mname.
        mname_t * new_mname(const mname_t & mname);
    };

    //-------- ---------- ---------- ---------- ----------

    // Multi-name, e.g. System.Text
    class mname_t : public compare_operators_t<mname_t, sid_t>
    {
    public:
        typedef __vector_t<sid_t> __name_parts_t;

        mname_t() = default;
        mname_t(const sid_t & sid) : sid(sid), parts({ sid }) { }
        mname_t(const mname_t &) = default;

        sid_t sid = 0;
        __name_parts_t parts;

        // Appends a part.
        void append_part(sid_t sid) { parts.push_back(sid); }

        // Commits it.
        void commit(al::spool_t & pool);

        // Returns sid.
        operator sid_t () const { return sid; }

        // Converts to string.
        operator string_t () const { return (string_t)sid; }

        // Returns the begin iterator.
        auto begin() const { return parts.begin(); }

        // Returns the end iterator.
        auto end()   const { return parts.end();   }

        // Returns whether the mname is empty.
        bool empty() const { return sid == sid_t::null; }

        // Returns count of parts.
        size_t part_count() const { return parts.size(); }

        // Returns part at specified index.
        sid_t operator [](size_t index) const { return parts[index]; }

        typedef mname_operate_context_t __context_t;

        // Parse strings to mname
        static const mname_t * parse(__context_t && ctx, const string_t & s);
        static const mname_t * parse(__context_t &  ctx, const string_t & s);

        // Returns part as char_t * at specifed index.
        const char_t * part_of(size_t index) const;

        // Returns last part as char_t *
        const char_t * last_part() const;

        // Combine two mnames.
        static const mname_t * combine(__context_t && ctx, const mname_t * name1,
                                                           const mname_t * name2);
        static const mname_t * combine(__context_t &  ctx, const mname_t * name1,
                                                           const mname_t * name2);

        // Combine two mnames.
        static const mname_t * combine(__context_t && ctx, const mname_t * name1,
                                                           const name_t & name2);
        static const mname_t * combine(__context_t &  ctx, const mname_t * name1,
                                                           const name_t & name2);

        // Returns parent of the mname.
        static const mname_t * get_parent(__context_t && ctx, const mname_t * name);
        static const mname_t * get_parent(__context_t &  ctx, const mname_t * name);

        // Returns child of the mname.
        static const mname_t * get_child(__context_t && ctx, const mname_t * name);
        static const mname_t * get_child(__context_t &  ctx, const mname_t * name);

        // Join parts in the container.
        template<typename container_t>
        static const mname_t * join(__context_t && ctx, container_t & container)
        {
            return join(ctx, container);
        }

        // Join parts in the container.
        template<typename container_t>
        static const mname_t * join(__context_t & ctx, container_t & container)
        {
            return join(ctx, container.begin(), container.end());
        }

        // Join parts in the range [ begin, end ]
        template<typename itor_t>
        static const mname_t * join(__context_t && ctx, itor_t begin, itor_t end)
        {
            return join(ctx, begin, end);
        }

        // Join parts in the range [ begin, end ]
        template<typename itor_t>
        static const mname_t * join(__context_t & ctx, itor_t begin, itor_t end)
        {
            if (begin == end)
                return nullptr;

            string_t s = al::join_str(begin, end, _T("."));
            sid_t sid = ctx.spool.to_sid(s);

            return ctx.from_cache(sid, [&ctx, sid, begin, end]() {
                mname_t * name = ctx.new_mname();
                std::copy(begin, end, std::back_inserter(name->parts));
                name->sid = sid;
                return name;
            });
        }

        // Convert a name to mname.
        static const mname_t * to_mname(__context_t && ctx, const name_t & name);
        static const mname_t * to_mname(__context_t &  ctx, const name_t & name);

        // Put a mname in the pool.
        static const mname_t * to_internal(__context_t && ctx, const mname_t * mname);
        static const mname_t * to_internal(__context_t &  ctx, const mname_t * mname);

        // The empty mname.
        static const mname_t null;
    };

    // eobject of mname.
    class emname_t : public eobject_t
    {
    public:
        emname_t() = default;
        emname_t(const mname_t * mname) : mname(mname) { }

        typedef emname_t * itype_t;

        const mname_t * mname = nullptr;

        X_TO_STRING
    };

    typedef eobject_ast_t<typename emname_t::itype_t> mname_ast_t;

    // Converts emname_t to mname_t
    X_INLINE const mname_t * to_mname(const emname_t * emname)
    {
        if (emname == nullptr)
            return nullptr;

        return emname->mname;
    }

    // Converts mname to sid.
    X_INLINE sid_t to_sid(const mname_t * mname)
    {
        if (mname == nullptr)
            return sid_t::null;

        return mname->sid;
    }

    ////////// ////////// ////////// ////////// //////////

    // Access protect values
    X_ENUM(access_value_t)

        public_, protected_, private_, internal,
        
        protected_or_internal, protected_and_internal,

    X_ENUM_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Decorates for types and members.
    struct decorate_value_t : compare_operators_t<decorate_value_t, uint32_t>
    {
        access_value_t access   : 4;

        bool is_static          : 1;
        bool is_sealed          : 1;
        bool is_override        : 1;
        bool is_virtual         : 1;
        bool is_abstract        : 1;
        bool is_new             : 1;
        bool is_const           : 1;
        bool is_readonly        : 1;
        bool is_extern          : 1;

        uint32_t    ____reserve : 19;

        operator uint32_t() const { return *(uint32_t *)this; }
        operator string_t() const;

        static const decorate_value_t default_value;
    };

    // Writes a decorate value to a stream.
    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const decorate_value_t & d)
    {
        return stream << *(uint32_t *)&d;
    }

    // Reads a decorate value from a stream.
    template<typename stream_t>
    stream_t & operator >> (stream_t & stream, decorate_value_t & d)
    {
        return stream >> *(uint32_t *)&d;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // decorate eobject.
    class decorate_t : public decorate_value_t, public eobject_t
    {
        typedef decorate_value_t __super_t;

    public:
        decorate_t() = default;
        decorate_t(const decorate_value_t & value) : __super_t(value) { }

        typedef decorate_t * itype_t;

        static const decorate_t default_value;

        X_TO_STRING
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<decorate_t *> decorate_ast_t;

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Base class with decorate.
    struct with_decorate_object_t
    {
        decorate_t * decorate = nullptr;

        bool is_static() const   _NE { return decorate && decorate->is_static; }
        bool is_sealed() const   _NE { return decorate && decorate->is_sealed; }
        bool is_override() const _NE { return decorate && decorate->is_override; }
        bool is_virtual() const  _NE { return decorate && decorate->is_virtual; }
        bool is_abstract() const _NE { return decorate && decorate->is_abstract; }
        bool is_new() const      _NE { return decorate && decorate->is_new; }
        bool is_const() const    _NE { return decorate && decorate->is_const; }
        bool is_readonly() const _NE { return decorate && decorate->is_readonly; }
        bool is_extern() const   _NE { return decorate && decorate->is_extern; }

        // Protected model.
        access_value_t access() const _NE
        {
            return decorate? decorate->access : access_value_t::__default__;
        }

        #define __DefineAccessMethod(_name, _value)                             \
            bool is_##_name() const _NE { return access() == access_value_t::_value; }

        __DefineAccessMethod(public, public_)
        __DefineAccessMethod(protected, protected_)
        __DefineAccessMethod(private, private_)
        __DefineAccessMethod(internal, internal)
        __DefineAccessMethod(protected_or_internal, protected_or_internal)
        __DefineAccessMethod(protected_and_internal, protected_and_internal)

        #undef __DefineAccessMethod
    };

    X_INLINE bool is_static_const(const with_decorate_object_t * obj)
    {
        _A(obj != nullptr);
        return obj->is_static() && obj->is_const();
    }

    ////////// ////////// ////////// ////////// //////////

    class expression_t;

    // A constant value with various types.
    class cvalue_t : public eobject_t
    {
    public:
        typedef cvalue_t * itype_t;

        cvalue_t(const char_t * string)
            : value_type(cvalue_type_t::string), string(string) { }

        cvalue_t(const tvalue_t & number)
            : value_type(cvalue_type_t::number), number(number) { }

        cvalue_t(int8_t value)      : cvalue_t(tvalue_t(value)) { }
        cvalue_t(uint8_t value)     : cvalue_t(tvalue_t(value)) { }
        cvalue_t(int16_t value)     : cvalue_t(tvalue_t(value)) { }
        cvalue_t(uint16_t value)    : cvalue_t(tvalue_t(value)) { }
        cvalue_t(int32_t value)     : cvalue_t(tvalue_t(value)) { }
        cvalue_t(uint32_t value)    : cvalue_t(tvalue_t(value)) { }
        cvalue_t(int64_t value)     : cvalue_t(tvalue_t(value)) { }
        cvalue_t(uint64_t value)    : cvalue_t(tvalue_t(value)) { }
        cvalue_t(bool value)        : cvalue_t(tvalue_t(value)) { }
        cvalue_t(char_t value)      : cvalue_t(tvalue_t(value)) { }
        cvalue_t(float value)       : cvalue_t(tvalue_t(value)) { }
        cvalue_t(double value)      : cvalue_t(tvalue_t(value)) { }

        cvalue_t() : cvalue_t(nullptr) { }
        cvalue_t(std::nullptr_t) : value_type(cvalue_type_t::null) { }
        cvalue_t(cvalue_type_t value_type, object_t * mobject = nullptr)
            : value_type(value_type), mobject(mobject) { }

        cvalue_type_t value_type;
        union
        {
            const char_t * string;
            tvalue_t       number;
            object_t     * mobject;
        };

        bool operator == (const cvalue_t & other) const _NE;
        bool operator != (const cvalue_t & other) const _NE;
        bool operator <  (const cvalue_t & other) const _NE;
        bool operator >  (const cvalue_t & other) const _NE;
        bool operator <= (const cvalue_t & other) const _NE;
        bool operator >= (const cvalue_t & other) const _NE;

        bool operator == (bool value) const _NE;
        bool operator != (bool value) const _NE;

        bool operator == (std::nullptr_t) const _NE { return is_null();  }
        bool operator != (std::nullptr_t) const _NE { return !is_null(); }

        static const cvalue_t nan;
        static const cvalue_t null;

        bool is_nan() const _NE  { return value_type == cvalue_type_t::nan; }
        bool is_null() const _NE { return value_type == cvalue_type_t::null; }

        cvalue_t change_type(vtype_t vtype) const _NE;

        #define __X_TYPE_OP(_type)                                                      \
            operator _type##_t() const { return number.get_value<_type##_t>(); }
        __X_EACH_TYPES
        #undef __X_TYPE_OP

        X_TO_STRING
    };

    // Converts a type_t to constant value.
    X_INLINE cvalue_t to_cvalue(type_t * type) _NE
    {
        return cvalue_t(cvalue_type_t::type, (object_t *)type);
    }

    // Returns whether a const value is nan.
    X_INLINE bool is_nan(const cvalue_t & v) _NE
    {
        return v.value_type == cvalue_type_t::nan;
    }

    // Returns whether a const value is a number.
    X_INLINE bool is_number(const cvalue_t & v) _NE
    {
        return v.value_type == cvalue_type_t::number;
    }

    // Returns whether a const value is a string.
    X_INLINE bool is_string(const cvalue_t & v) _NE
    {
        return v.value_type == cvalue_type_t::string;
    }

    // Returns whether a const value is null.
    X_INLINE bool is_null(const cvalue_t & v) _NE
    {
        return v.value_type == cvalue_type_t::null;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<cvalue_t *>     cvalue_ast_t;
    typedef eobject_ast_t<expression_t *> expression_ast_t;
 
    ////////// ////////// ////////// ////////// //////////
    // xil

    // The xil interface. Assembly middle language.
    X_INTERFACE xil_t
    {
        // Empty.
    };

    //-------- ---------- ---------- ---------- ----------

    // Statement exit type.
    X_ENUM_(statement_exit_type_t, sizeof(int16_t))

        // None
        none            = __default__,

        // Pass through
        pass            = 1 << 1,

        // Return.
        return_         = 1 << 2,

        // Break.
        break_          = 1 << 3,

        // Continue.
        continue_       = 1 << 4,

        // Throw.
        throw_          = 1 << 5,

        // Goto.
        goto_           = 1 << 6,

        // Dead cycle, cannot exit.
        dead_cycle      = 1 << 7,

    X_ENUM_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Statement exit type context.
    class statement_exit_type_context_t : public object_t
    {
    public:

        // Constructor.
        statement_exit_type_context_t() { }

        X_TO_STRING_IMPL(_T("statement_exit_type_context_t"))
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Statement eobject.
    X_INTERFACE statement_t : eobject_t
    {
        // Compiles this statement.
        virtual void compile(statement_compile_context_t & ctx) = 0;

        // Returns exit type of this statement.
        virtual statement_exit_type_t exit_type(statement_exit_type_context_t & ctx) = 0;

        // Returns whether the statement is empty.
        virtual bool is_empty() = 0;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<statement_t *> statement_ast_t;
    typedef eobjects_t<statement_t *> statements_t;
    typedef eobject_ast_t<statements_t *> statements_ast_t;

    ////////// ////////// ////////// ////////// //////////

    class assembly_t;
    class generic_arg_t;

    typedef indexed_eobjects_t<generic_arg_t *> generic_args_t;

    //-------- ---------- ---------- ---------- ----------

    // Typename eobject.
    class type_name_t : public eobject_t
    {
    public:
        type_name_t() = default;
        type_name_t(type_t * type) : type(type) { }

        typedef type_name_t * itype_t;

        type_t * type = nullptr;

        X_TO_STRING
    };

    typedef eobject_ast_t<typename type_name_t::itype_t> type_name_ast_t;

    type_t * to_type(type_name_t * type_name);

    ////////// ////////// ////////// ////////// //////////
    // attribute_t

    class argument_t;
    class method_t;
    class type_impl_t;
    typedef indexed_eobjects_t<argument_t *> arguments_t;

    // Attribute eobject.
    class attribute_t : public eobject_t
    {
    public:
        typedef attribute_t * itype_t;

        type_name_t * type_name   = nullptr;
        arguments_t * arguments   = nullptr;
        method_t    * constructor = nullptr;

        // Sets owner of attribute.
        virtual void set_owner(eobject_t * object);

        // Commits it.
        virtual void commit(eobject_commit_context_t & ctx) override;

        runtime_mobject_t * attribute_object = nullptr;
        eobject_t   * bind_object = nullptr;

        X_TO_STRING

    private:
        type_impl_t * __get_impl();
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<attribute_t::itype_t> attribute_ast_t;
    typedef owner_eobjects_t<attribute_t::itype_t> attributes_t;
    typedef eobject_ast_t<attributes_t *> attributes_ast_t;

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Base class with attribute.
    class with_attributes_object_t
    {
    public:
        attributes_t * attributes = nullptr;

        typedef al::svector_t<attribute_t *> output_attributes_t;

        // Returns all attributes with specified attribute type.
        void get_attributes(output_attributes_t & out_attributes,
                general_type_t * attribute_type, bool inherit = false);

        // Returns first attribute with specified attribute type.
        attribute_t * get_attribute(general_type_t * attribute_type, bool inherit = false);

        // Returns whether the attribute is specified.
        bool has_attribute(general_type_t * attribute_type, bool inherit = false)
        {
            return get_attribute(attribute_type, inherit) != nullptr;
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // Argument name type
    X_ENUM(argument_name_type_t)

        param,      // is a param

        field,      // is a field, be used for attribute assign a value to field.

    X_ENUM_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Param type
    X_ENUM(param_type_t)

        default_ = __default__,

        ref,        // e.g. ref int value

        out,        // e.g. out int value   or   out T (for generic params)

        params,     // e.g. params int[] values

        extends,    // e.g. TArgs ... args

    X_ENUM_END

    // Returns it's a ref or out param.
    X_INLINE bool is_addr_param(param_type_t param_type) _NE
    {
        return param_type == param_type_t::ref || param_type == param_type_t::out;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Argument eobject.
    class argument_t : public with_attributes_object_t
                     , public with_index_t
                     , public eobject_t
                     , public runtime_mobject_t
    {
        typedef eobject_t __super_t;

    public:
        typedef argument_t * itype_t;

        name_t               name       = name_t::null;                 // Arguments
        param_type_t         atype      = param_type_t::__default__;    // Argument type.
        argument_name_type_t name_type  = argument_name_type_t::param;  // Name type.
        expression_t *       expression = nullptr;                      // Value.

        // Returns argument type.
        type_t * get_type() const;

        // Converts to string.
        operator string_t() const;
        
        // Converts to string.
        virtual const string_t to_string() const override { return (string_t)*this; }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<argument_t *> argument_ast_t;
    typedef eobject_ast_t<arguments_t *> arguments_ast_t;

    ////////// ////////// ////////// ////////// //////////

    // Param eobject.
    class param_t : public with_attributes_object_t
                  , public with_index_t
                  , public eobject_t, public runtime_mobject_t
    {
    public:
        typedef param_t * itype_t;

        // Constructors
        param_t() = default;
        param_t(type_name_t * type_name, name_t name,
                param_type_t ptype = param_type_t::__default__,
                expression_t * default_value = nullptr)
            : type_name(type_name), name(name), ptype(ptype), default_value(default_value)
        { }

        type_name_t  *  type_name       = nullptr;                      // Param type.
        name_t          name            = name_t::null;                 // Param name.
        param_type_t    ptype           = param_type_t::__default__;    // Param type, ref, out ...
        expression_t *  default_value   = nullptr;                      // Default value.

        // Returns type of the param.
        type_t * get_type() const;

        // Converts to string.
        operator string_t() const;

        // Converts to string.
        virtual const string_t to_string() const override { return (string_t)*this; }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<param_t *> param_ast_t;
    typedef indexed_eobjects_t<param_t *> params_t;
    typedef eobject_ast_t<params_t *> params_ast_t;

    // Converts value_type (defines in common module) to vtype.
    vtype_t to_vtype(value_type_t value_type) _NE;

    // Converts type_t to vtype.
    vtype_t to_vtype(type_t * type);

    // Returns whether a vtype can convert to another type
    // E.g. int can convert to long.
    bool is_vtype_compatible(vtype_t from, vtype_t to) _NE;

    // Returns whether a vtype is a integer type.
    bool is_integer_vtype(vtype_t vtype) _NE;

    // Returns whether a vtype is a float type.
    bool is_float_vtype(vtype_t vtype) _NE;

    // Returns whether a vtype is a numberic.
    bool is_numberic_vtype(vtype_t vtype) _NE;

    // Returns default value of a vtype.
    cvalue_t default_value_of(vtype_t vtype) _NE;

    // Returns whether a vtype is signed.
    bool is_signed_vtype(vtype_t vtype) _NE;

    // Returns whether a vtype is unsigned.
    bool is_unsigned_vtype(vtype_t vtype) _NE;

    // Returns default value of a type.
    template<typename _t> _t default_value_of() _NE
    {
        return (_t)0;
    }

    // Returns size of a vtype.
    constexpr msize_t get_vtype_size(vtype_t vtype) _NE
    {
        switch (vtype)
        {
            case vtype_t::void_:
                return 0;

            case vtype_t::int8_:
            case vtype_t::uint8_:
            case vtype_t::bool_:
                return 1;

            case vtype_t::int16_:
            case vtype_t::uint16_:
                return 2;

            case vtype_t::int32_:
            case vtype_t::uint32_:
            case vtype_t::float_:
            case vtype_t::char_:
                return 4;

            case vtype_t::int64_:
            case vtype_t::uint64_:
            case vtype_t::double_:
                return 8;

            case vtype_t::ptr_:
            case vtype_t::mobject_:
            case vtype_t::string_:
            default:
                return sizeof(void *);
        }
    }

    //-------- ---------- ---------- ---------- ----------

    // Mobject type.
    X_ENUM(mtype_t)

        tuple,          // Tuple

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Category of a type.
    X_ENUM(ttype_t)

        class_,         // Class

        struct_,        // Struct

        interface_,     // Interface

        enum_,          // Enum

    X_ENUM_END

    // Returns whether the ttype is reference type.
    X_INLINE bool is_ref_type(ttype_t ttype)
    {
        return ttype == ttype_t::class_ || ttype == ttype_t::interface_;
    }

    //-------- ---------- ---------- ---------- ----------

    // Kind of a type.
    X_ENUM(gtype_t)

        general,        // General type, defined in the code.

        generic,        // Generic type, implemented by general template and type arguments.

        generic_param,  // Generic params.
        
        array,          // Array type.

        type_def_param, // Param in typedef.
        
        null,           // Null type.

        uncertain,      // Uncertaind type.

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////
    // type_t, member_type_t, ttype_t

    // Member type
    X_ENUM(member_type_t)

        field       =   1 << 0,     // Field member

        method      =   1 << 1,     // Method member
    
        property    =   1 << 2,     // Property member

        event       =   1 << 3,     // Event member

        type        =   1 << 4,     // Nest type member

        type_def    =   1 << 5,     // Typedef defined in a type.

        all         =   -1,         // All member types.

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------
    // member_t

    // Method family.
    X_ENUM(member_family_t)

        // General method, defined in source codes.
        general,

        // Generic method, Dynamic defined when used.
        // Composited with general template and type arguments..
        generic,

        // Implemenation for a generic method.
        impl,

        // Specified member by position, from index 0.
        position,

    X_ENUM_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Base class of member eobject.
    class member_t : public eobject_t
                   , public with_attributes_object_t, public with_decorate_object_t
                   , public runtime_mobject_t
    {
        typedef eobject_t __super_t;

    public:
        type_t * host_type = nullptr;

        // Returns member type
        virtual member_type_t this_type() = 0;

        // Returns name.
        virtual name_t get_name() const = 0;

        // Sets a host type as the member owner.
        void set_owner(type_t * host_type) { this->host_type = host_type; }

        // Returns family.
        virtual member_family_t this_family() const { return member_family_t::general; }

        // Converts to string.
        operator string_t() const { return _T("type member"); }

        // Converts to string.
        virtual const string_t to_string() const override { return (string_t)*this; }
    };

    // Get assembly of a member.
    assembly_t * get_assembly(member_t * member);

    //-------- ---------- ---------- ---------- ----------
    // __named_base_t
    
    // Base class of a named object.
    template<typename _super_t>
    class __named_base_t : public _super_t
    {
        typedef _super_t __super_t;

    public:
        using __super_t::__super_t;

        // Object name.
        name_t name  = name_t::null;

        // Returns name of the object.
        virtual name_t get_name() const override { return name; }
    };

    typedef __named_base_t<member_t> __named_member_t;

    //-------- ---------- ---------- ---------- ----------
    struct typex_t;

    // Argument type structure with a type and it's argument type.
    struct atype_t
    {
        atype_t() = default;
        atype_t(typex_t typex) _NE;
        atype_t(type_t * type, param_type_t atype = param_type_t::__default__) _NE
            : type(type), atype(atype) { }

        type_t * type = nullptr;
        param_type_t atype = param_type_t::__default__;

        // Converts to a string.
        operator string_t() const;
    };

    // Writes to a stream.
    template<typename _stream_t> _stream_t & operator << (_stream_t & stream, atype_t & atype)
    {
        return stream << ((string_t)atype).c_str(), stream;
    }

    typedef al::svector_t<atype_t, 8> atypes_t;
    typedef al::svector_t<type_t *, 8> arg_types_t;

    //-------- ---------- ---------- ---------- ----------
    // method_trait_t

    // Method traits, Be used for searching members in a type.
    X_ENUM(method_trait_t)

        default_ = __default__,

        normal,                 // Normal method.

        constructor,            // Constructor.

        static_constructor,     // Static Constructor.

        destructor,             // Destructor.

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------
    // type_t

    const int __unknown_size = -1;

    // Analyze arguments for searching members in a type.
    class analyze_member_args_t
    {
    public:
        // Constructor.
        analyze_member_args_t(member_type_t member_type, name_t name,
                atypes_t * atypes = nullptr, generic_args_t * generic_args = nullptr,
                type_t * return_type = nullptr)
            : member_type(member_type), name(name)
            , atypes(atypes), generic_args(generic_args), return_type(return_type)
        { }

        member_type_t       member_type;        // Member type.
        name_t              name;               // Member name.
        type_t *            return_type;        // Return type.
        atypes_t *          atypes;             // Argument types.
        generic_args_t *    generic_args;       // Generic arguments.
        method_trait_t      method_trait    = method_trait_t::__default__;  // Method trait.
        arg_types_t         out_arg_types;      // Generic types auto determined
        size_t              generic_args_count = 0; // Generic argument count when fetch nest types.

        bool                exact_match     = false;    // Whether exact match the member.

        // Returns name of member, if name is null, returns name from context
        // (constructor, destructor, static_constructor ...).
        string_t get_name() const;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef __vector_t<member_t *> members_t;

    // Extension analyze arguments for searching members in a type.
    class analyze_members_args_t : public analyze_member_args_t
    {
        typedef analyze_member_args_t __super_t;

    public:
        using __super_t::__super_t; 

        // Output members.
        members_t out_members;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class namespace_t;

    // Base class of types.
    class type_t : public member_t
    {
        typedef member_t __super_t;

    public:
        typedef type_t * itype_t;

        // Returns gtype, ttype, vtype.
        virtual gtype_t this_gtype() = 0;
        virtual ttype_t this_ttype() = 0;
        virtual vtype_t this_vtype() = 0;
        virtual mtype_t this_mtype() = 0;

        // Returns members descripted by args.
        virtual member_t * get_member(analyze_member_args_t & args) = 0;

        // Returns memeber descripted by args.
        virtual void get_members(analyze_members_args_t & args) = 0;

        // Returns all members.
        virtual void get_all_members(members_t & out_members) = 0;

        // Enum all super types.
        typedef std::function<void (type_t *)> each_super_type_callback_t;
        virtual void each_super_type(each_super_type_callback_t callback) = 0;

        // Return this type of member.
        virtual member_type_t this_type() override final
        {
            return member_type_t::type;
        }

        // Returns vsize.
        virtual msize_t get_vsize()   { return unknown_msize; }

        // Returns count of ref type.
        virtual msize_t get_rcount()  { return unknown_msize; }

        // Returns whether the type has the super type.
        virtual bool has_super_type(type_t * super);

        // Returns base type.
        virtual type_t * get_base_type();

        // Converts to string.
        virtual const string_t to_string() const override { return to_full_name(); }

        // Returns the short name.
        virtual const string_t to_short_name() const { return (string_t)*this; }

        // Returns the full name.
        virtual const string_t to_full_name() const { return (string_t)*this; };

        // Returns the identity string of the type, e.g. System.Tuple<int,int>
        virtual const string_t to_identity() const = 0;

        // Converts to string.
        operator string_t() const { return to_identity(); }

        // Commits it.
        virtual void commit(eobject_commit_context_t & ctx) override;

        // This family.
        static const efamily_t __family__ = efamily_t::type;

    private:
        std::set<type_t *> __super_types;
    };

    typedef __named_base_t<type_t> __named_type_t;

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Returns whether a type is a base type of specified type.
    bool is_base_type(type_t * drived, type_t * base);

    // Returns whether a type is a super type of specified type, includes interfaces.
    bool is_super_type(type_t * drived, type_t * super);

    // Returns whether two type are compatiable.
    // Determines whether a type can convert to another type.
    // E.g. int32 -> int64. System.String -> System.Object.
    bool is_type_compatible(type_t * from, type_t * to);

    // Returns whether a type is a void type.
    bool is_void_type(type_t * type);

    // Returns whether a type is a string type.
    bool is_string_type(type_t * type);

    // Returns whether a type is a pointer type.
    bool is_ptr_type(type_t * type);

    // Returns whether a type is a general type, ( defined in source code. )
    X_ALWAYS_INLINE bool is_general(type_t * type)
    {
        return type != nullptr && type->this_gtype() == gtype_t::general;
    }

    // Returns whether a type is a generic type.
    X_ALWAYS_INLINE bool is_generic(type_t * type)
    {
        return type != nullptr && type->this_gtype() == gtype_t::generic;
    }

    // Returns whether a type is a generic param.
    X_ALWAYS_INLINE bool is_generic_param(type_t * type)
    {
        return type != nullptr && type->this_gtype() == gtype_t::generic_param;
    }

    // Returns whether a type is a array type.
    X_ALWAYS_INLINE bool is_array(type_t * type)
    {
        return type != nullptr && type->this_gtype() == gtype_t::array;
    }

    // Returns whether a type is a param of typedef.
    X_ALWAYS_INLINE bool is_type_def_param(type_t * type)
    {
        return type != nullptr && type->this_gtype() == gtype_t::type_def_param;
    }

    // Returns whether a type is a struct type.
    X_ALWAYS_INLINE bool is_struct(type_t * type)
    {
        return type != nullptr && type->this_ttype() == ttype_t::struct_;
    }

    // Returns whether a type is enum type.
    X_ALWAYS_INLINE bool is_enum(type_t * type)
    {
        return type != nullptr && type->this_ttype() == ttype_t::enum_;
    }

    // Returns whether it's a custom defined struct type.
    X_ALWAYS_INLINE bool is_custom_struct(type_t * type)
    {
        return type != nullptr && type->this_ttype() == ttype_t::struct_
                    && type->this_vtype() == vtype_t::mobject_;
    }

    // Returns whether it's a system value type.
    X_ALWAYS_INLINE bool is_system_value_type(vtype_t vtype)
    {
        return vtype >= vtype_t::int8_ && vtype <= vtype_t::ptr_;
    }

    const vtype_t default_enum_underlying_vtype = vtype_t::int32_;

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<type_t *> type_ast_t;

    ////////// ////////// ////////// ////////// //////////
    // Uncertain type.

    class uncertain_type_t : public __named_type_t
    {
        typedef type_t __super_t;

    public:
        // Returns gtype, ttype, vtype.
        virtual gtype_t this_gtype() override final { return gtype_t::uncertain; }
        virtual ttype_t this_ttype() override final { return ttype_t::__unknown__; }
        virtual vtype_t this_vtype() override final { return vtype_t::__unknown__; }
        virtual mtype_t this_mtype() override final { return mtype_t::__unknown__; }

        // Returns member descripted by specified args.
        virtual member_t * get_member(analyze_member_args_t & args) override;

        // Returns members descripted by specified args.
        virtual void get_members(analyze_members_args_t & args) override;

        // Returns all members.
        virtual void get_all_members(members_t & out_members) override;

        // Enum all super types, Applies each item for callback function.
        virtual void each_super_type(each_super_type_callback_t callback) override { }

        // Converts to full name.
        virtual const string_t to_full_name() const override { return (string_t)*this; }

        // Converts to short name.
        virtual const string_t to_short_name() const override { return (string_t)*this; }

        // Converts to identity of this type.
        virtual const string_t to_identity() const override { return _T("<uncertain>"); }

        // Signle instance of uncertain type.
        static uncertain_type_t * instance();

    };

    ////////// ////////// ////////// ////////// //////////

    // Null type. the type of null value, can convert to any ref types.
    class null_type_t : public __named_type_t
    {
        typedef type_t __super_t;

    public:
        // Returns gtype, ttype, vtype.
        virtual gtype_t this_gtype() override final { return gtype_t::null; }
        virtual ttype_t this_ttype() override final { return ttype_t::__unknown__; }
        virtual vtype_t this_vtype() override final { return vtype_t::__unknown__; }
        virtual mtype_t this_mtype() override final { return mtype_t::__unknown__; }

        // Returns member descripted by specified args.
        virtual member_t * get_member(analyze_member_args_t & args) override;

        // Returns members descripted by specified args.
        virtual void get_members(analyze_members_args_t & args) override;

        // Returns all members.
        virtual void get_all_members(members_t & out_members) override;

        // Enum all super types, Applies each item for callback function.
        virtual void each_super_type(each_super_type_callback_t callback) override { }

        // Converts to full name.
        virtual const string_t to_full_name() const override { return (string_t)*this; }

        // Converts to short name.
        virtual const string_t to_short_name() const override { return (string_t)*this; }

        // Converts to identity of this type.
        virtual const string_t to_identity() const override { return _T("<null>"); }

        // Signle instance of null type.
        static null_type_t * instance();
    };

    ////////// ////////// ////////// ////////// //////////
    // generic_param_t

    // Generic param type.
    X_ENUM(generic_param_type_t)

        // Default.
        default_ = __default__,

        // Only for writing.
        in,

        // Only for reading.
        out,

        // Multipy params.
        params,

    X_ENUM_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Generic param.
    class generic_param_t : public __named_type_t, public with_index_t
    {
        typedef type_t __super_t;

    public:
        typedef generic_param_t * itype_t;

        // Generic param type.
        generic_param_type_t param_type = generic_param_type_t::__default__;

        // Returns gtype, ttype, vtype.
        virtual gtype_t this_gtype() override final { return gtype_t::generic_param; }
        virtual ttype_t this_ttype() override final { return ttype_t::__unknown__; }
        virtual vtype_t this_vtype() override final { return vtype_t::__unknown__; }
        virtual mtype_t this_mtype() override final { return mtype_t::__unknown__; }

        // Returns member descripted by specified args.
        virtual member_t * get_member(analyze_member_args_t & args) override;

        // Returns members descripted by specified args.
        virtual void get_members(analyze_members_args_t & args) override;

        // Returns all members.
        virtual void get_all_members(members_t & out_members) override;

        // Enum all super types, Applies each item for callback function.
        virtual void each_super_type(each_super_type_callback_t callback) override { }

        // Converts to full name.
        virtual const string_t to_full_name() const override { return (string_t)*this; }

        // Converts to short name.
        virtual const string_t to_short_name() const override { return (string_t)*this; }

        // Converts to a identity of thiis type.
        // E.g. System.Tuple<int,long>
        virtual const string_t to_identity() const override;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<generic_param_t *> generic_param_ast_t;

    typedef indexed_eobjects_t<generic_param_t *> generic_params_t;
    typedef eobject_ast_t<generic_params_t *>     generic_params_ast_t;

    ////////// ////////// ////////// ////////// //////////

    X_ENUM(generic_arg_type_t)

        ref     = (int)param_type_t::ref,       // ref int

        out     = (int)param_type_t::out,       // out int

        params  = (int)param_type_t::params,    // params int[]

    X_ENUM_END

    // Generic argument of eobject.
    class generic_arg_t : public with_index_t, public eobject_t
    {
        typedef eobject_t __super_t;

    public:
        typedef generic_arg_t * itype_t;

        // Type of the generic argument.
        type_name_t * type_name = nullptr;

        // Argument type.
        generic_arg_type_t atype = generic_arg_type_t::__default__;

        // Returns type of the generic argument.
        type_t * get_type() const { return type_name? type_name->type : nullptr; }

        // Converts to a string.
        operator string_t() const;

        // Converts to a string.
        virtual const string_t to_string() const override { return (string_t)*this; }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<generic_arg_t *> generic_arg_ast_t;
    typedef eobject_ast_t<generic_args_t *> generic_args_ast_t;

    ////////// ////////// ////////// ////////// //////////
    // generic_constraint_t

    // Generic constraint.
    X_ENUM(generic_constraint_ttype_t)

        // Class, ref type
        class_,

        // Struct, value type
        struct_,

    X_ENUM_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Generic constraint.
    class generic_constraint_t : public eobject_t
    {
        typedef eobject_t __super_t;

    public:
        typedef generic_constraint_t * itype_t;

        // Param name, e.g. T1 in type System.Tuple<T1, T2>.
        name_t param_name = name_t::null;

        // Geneirc constraint.
        generic_constraint_ttype_t cttype = generic_constraint_ttype_t::__default__;

        // Types or super types.
        __vector_t<type_name_t *> type_names;

        // Appends a type or super type.
        void append(type_name_t * type_name)
        {
            _A(type_name != nullptr);
            type_names.push_back(type_name);
        }
    };

    typedef eobject_ast_t<generic_constraint_t::itype_t> generic_constraint_ast_t;
    typedef eobjects_t<generic_constraint_t *> generic_constraints_t;
    typedef eobject_ast_t<generic_constraints_t *> generic_constraints_ast_t;

    ////////// ////////// ////////// ////////// //////////

    // The import eobject
    class import_t : public eobject_t
    {
        typedef eobject_t __super_t;

    public:
        typedef import_t * itype_t;

        // Constructors.
        import_t() = default;
        import_t(emname_t * package, const name_t & alias)
            : package(package), alias(alias) { }

        // Package name
        emname_t * package  = nullptr;

        // Assembly name
        emname_t * assembly = nullptr;

        // Alias name
        name_t     alias    = name_t::null;

        // Converts to string.
        operator string_t() const;
        virtual const string_t to_string() const override { return (string_t)*this; }
    };

    typedef eobject_ast_t<import_t *> import_ast_t;

    ////////// ////////// ////////// ////////// //////////

    // The using namespace eobject.
    class using_namespace_t : public eobject_t
    {
        typedef eobject_t __super_t;

    public:
        typedef using_namespace_t * itype_t;

        // Constructors.
        using_namespace_t() = default;
        using_namespace_t(emname_t * ns, const name_t & alias)
            : ns(ns), alias(alias) { }

        // Namespace.
        emname_t * ns    = nullptr;

        // Alias name.
        name_t     alias = name_t::null;

        // Converts to string.
        operator string_t() const;
        virtual const string_t to_string() const override { return (string_t)*this; }
    };

    typedef eobject_ast_t<using_namespace_t *> using_namespace_ast_t;

} } }

#include <modules/core/__xil.h>

namespace X_ROOT_NS { namespace modules { namespace core {

    // The body of a method.
    // Compisited by statements.
    class method_body_t : public eobject_t
    {
        typedef eobject_t __super_t;

    public:
        typedef method_body_t * itype_t;

        // Constructor.
        method_body_t() = default;

        // Constructor.
        method_body_t(variable_region_t * variable_region)
            : variable_region(variable_region)
        { }

        // Constructor.
        method_body_t(std::initializer_list<statement_t *> initializer,
            variable_region_t * variable_region = nullptr)
            : statements(initializer), variable_region(variable_region)
        { }

        // Statements.
        statements_t statements;

        // Current variable region.
        variable_region_t * variable_region = nullptr;

        // Pushes statements at front.
        template<typename _statements_t>
        void push_front(_statements_t & statements)
        {
            this->statements.push_front(statements);
        }

        // Pushes a statement at front.
        void push_front(statement_t * statement)
        {
            _A(statement != nullptr);

            statement_t * statements[] = { statement };
            this->statements.push_front(statements);
        }

        // Pushes statements at back.
        template<typename _statements_t>
        void push_back(_statements_t & statements)
        {
            std::copy(
                statements.begin(), statements.end(), std::back_inserter(this->statements)
            );
        }

        // Pushes a statement at back.
        void push_back(statement_t * statement)
        {
            _A(statement != nullptr);
            this->statements.push_back(statement);
        }
    };

    typedef eobject_ast_t<method_body_t *> method_body_ast_t;

    ////////// ////////// ////////// ////////// //////////

    // Method inline types
    X_ENUM(method_inline_type_t)

        // Default, auto determined.
        default_ = __default__,

        // Always inline.
        always,

        // Never inline.
        never,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    class local_variable_t;
    class xil_buffer_t;
    class xil_pool_t;

    struct default_mt_template_t;
    typedef uint16_t __assembly_layout_version_t;

    // Assembly layout, be used reading information of a assembly.
    template<
        typename spool_t, __assembly_layout_version_t lv, typename mt_template_t
    > class assembly_layout_t;

    typedef class assembly_layout_t<al::spool_t, 0, default_mt_template_t> __assembly_layout_t;

    //-------- ---------- ---------- ---------- ----------

    // Local variable defination in method.
    struct local_variable_defination_t
    {
        // Variable type, ref_t is a type reference in assembly.
        ref_t       type;

        // Variable index defination in method.
        msize_t     index;

        // Write the local variable as xil to buffer.
        void write(__assembly_layout_t & layout, xil_buffer_t & buffer);
    };

    //-------- ---------- ---------- ---------- ----------

    // Method stub type. Components of method.
    X_ENUM(method_stub_type_t)

        // Empty.
        empty           = __default__,

        // Variable definations.
        variable        = 1,

        // Switch-case table.
        switch_table    = 2,

        // Xil blocks.
        xil_block       = 3,

        // commands.
        xil             = 15,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Method stub, the method headers.
    // Description parts of method.
    struct method_stub_t
    {
        method_stub_t() = default;
        method_stub_t(method_stub_type_t type)
            : __type((byte_t)type), __data1(0), __data2(0), __data3(0)
        { }

        byte_t      __type      : 4;
        byte_t      __data1     : 4;
        uint8_t     __data2;
        uint16_t    __data3;

        // Stub type.
        method_stub_type_t type() const { return (method_stub_type_t)__type; }

        // Write the method stub to a buffer.
        void write(xil_buffer_t & buffer);
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    namespace
    {
        // Method stub.
        template<method_stub_type_t _type>
        struct __t_method_stub_t : method_stub_t
        {
            typedef method_stub_t __super_t;

            // Constructor.
            __t_method_stub_t() : __super_t(_type) { }

            static const method_stub_type_t __type__ = _type;
        };

        // Method stub with count.
        template<method_stub_type_t _type>
        struct __t_count_method_stub_t : __t_method_stub_t<_type>
        {
            // Constructor.
            __t_count_method_stub_t(uint16_t count)
            {
                this->__data3 = count;
            }

            // Returns count.
            uint16_t count() const { return this->__data3; }

            // Sets count.
            void set_count(uint16_t count) { this->__data3 = count; }
        };
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Method variable stub.
    struct method_variable_stub_t : __t_count_method_stub_t<method_stub_type_t::variable>
    {
        typedef __t_count_method_stub_t<method_stub_type_t::variable> __super_t;
        using __super_t::__super_t;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Method switch-case table stub.
    struct method_switch_table_stub_t : __t_count_method_stub_t<method_stub_type_t::switch_table>
    {
        typedef __t_count_method_stub_t<method_stub_type_t::switch_table> __super_t;
        using __super_t::__super_t;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef int local_label_t;

    // A switch-case row.
    struct switch_row_t
    {
        switch_row_t() = default;
        switch_row_t(int32_t value, local_label_t label)
            : value(value), label(label) { }

        int32_t     value;
        int32_t     step    = 0;

        local_label_t label;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Switch-case table header.
    struct switch_table_header_t
    {
        // Constructors.
        switch_table_header_t() = default;
        switch_table_header_t(uint16_t count) : count(count), __reserved__(0) { }

        // Switch-case counts.
        uint16_t    count;
        uint16_t    __reserved__;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Switch table.
    struct switch_table_t : switch_table_header_t
    {
        typedef switch_table_header_t __super_t;

        // Constructors.
        switch_table_t() = default;
        switch_table_t(int index, int count) : __super_t(count), index(index) { }

        // Switch-table index
        int             index;

        // Label default.
        local_label_t   default_label = 0;

        // Label default jmp position.
        int32_t         default_step  = 0;

        // Jmp xil. ?
        xil_t *         jmp_xil = nullptr;

        // Switch-case rows.
        switch_row_t    rows[0];

        // Returns the begin row iterator.
        switch_row_t * begin() const { return (switch_row_t *)rows; }

        // Returns the end row iterator.
        switch_row_t * end()   const { return (switch_row_t *)rows + count; }

        // Writes the switch-table to a buffer.
        void write(xil_buffer_t & buffer);
    };

    //-------- ---------- ---------- ---------- ----------

    // Method xil block stub.
    struct method_xil_block_stub_t : __t_count_method_stub_t<method_stub_type_t::xil_block>
    {
        typedef __t_count_method_stub_t<method_stub_type_t::xil_block> __super_t;

        using __super_t::__super_t;
    };

    //-------- ---------- ---------- ---------- ----------

    // Xil block types.
    X_ENUM(method_xil_block_type_t)

        // When specified exception raised.
        catch_,

        // When leaved from protected block.
        finally_,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Method xil block.
    struct method_xil_block_t
    {
        method_xil_block_type_t type = method_xil_block_type_t::__default__;
        uint8_t     __reserved_1__  = 0;
        uint16_t    __reserved_2__  = 0;

        uint32_t    xil_start   = 0;
        uint32_t    xil_end     = 0;
        uint32_t    entry_point = 0;

        ref_t       relation_type = ref_t::null;

        // Converts to string.
        operator string_t() const;
    };

    //-------- ---------- ---------- ---------- ----------

    // Method xil stub.
    struct method_xil_stub_t : __t_method_stub_t<method_stub_type_t::xil>
    {
        typedef __t_method_stub_t<method_stub_type_t::xil> __super_t;
    };

    //-------- ---------- ---------- ---------- ----------

    // Switch-case manager.
    class switch_manager_t : public object_t, public no_copy_ctor_t
    {
    public:

        // Constructors.
        switch_manager_t() : __heap(_T("switch_manager")) { }

        // Appends table with row count.
        switch_table_t * append_table(int row_count);

        // Commits it.
        void commit(statement_compile_context_t & ctx);

        // Write the switch-case to a buffer.
        void write(xil_buffer_t & buffer);

        X_TO_STRING_IMPL(_T("switch_manager_t"))

    private:
        int __tbl_index = 0;
        al::xheap_t __heap;
        al::svector_t<switch_table_t *, 2> __tables;
    };

    //-------- ---------- ---------- ---------- ----------

    typedef chain_node_t<local_variable_t> local_variable_chain_node_t;

    // Local variables.
    class local_variables_t
    {
        typedef local_variable_chain_node_t __node_t;

    public:
        // Write loca variables to a buffer.
        void write(__assembly_layout_t & layout, xil_buffer_t & buffer) const;

        // Appends a variable.
        __node_t ** head_addr() _NE { return &__head; }

        // Enumerate all local variables.
        void each(std::function<void (local_variable_t *)> f) const;

        // Returns count of variables.
        size_t size() const _NE;

        // Returns whether it's empty.
        bool empty() const _NE { return __head == nullptr; }

        // Converts to string.
        operator string_t() const;

    private:
        __node_t * __head = nullptr;
    };

    //-------- ---------- ---------- ---------- ----------

    // Method compiles controller.
    X_INTERFACE method_compile_controller_t
    {
        // Returns whether specified code need optimized.
        virtual bool optimize(int code) = 0;

        // A default controller.
        static method_compile_controller_t * default_();
    };

    //-------- ---------- ---------- ---------- ----------
    // typex_t

    struct typex_t : compare_operators_t<typex_t, void *>
    {
        typedef generic_arg_type_t __gatype_t;

        typex_t() _NE : __v(nullptr) { }
        typex_t(type_t * type) _NE : __v(type) { }
        typex_t(type_t * type, __gatype_t atype) _NE : __v(al::incorp(type, atype)) { }
        typex_t(param_t * param) : __v(al::incorp(param->get_type(), param->ptype)) { }

        type_t *  type() const _NE     { return incorp_p((type_t *)__v);  }
        __gatype_t atype() const _NE   { return incorp_v<__gatype_t>(__v); }
        param_type_t ptype() const _NE { return (param_type_t)atype(); }

        operator type_t * () const _NE     { return type(); }
        operator __gatype_t () const _NE   { return atype(); }
        operator param_type_t () const _NE { return ptype(); }
        operator void *   () const _NE     { return __v; }

        // Converts to string.
        operator string_t() const;

    private:
        void * __v;
    };

    // Type collection.
    typedef __vector_t<typex_t> type_collection_t;

    // Converts generic arguments to a type collection.
    type_collection_t to_type_collection(generic_args_t * args);

    // Fill generic arguments to a type collection.
    void fill_type_collection(type_collection_t & tc, generic_args_t * args);

    //-------- ---------- ---------- ---------- ----------

    // Context for compile a method.
    class method_compile_context_t : public object_t, public no_copy_ctor_t
    {
    public:

        // Constructors.
        method_compile_context_t(__assembly_layout_t & layout,
             xil_buffer_t & buffer, xil_pool_t & xil_pool, logger_t & logger,
             method_compile_controller_t * controller = method_compile_controller_t::default_())
            : layout(layout), xil_pool(xil_pool), buffer(buffer), logger(logger)
            , controller(controller? controller : method_compile_controller_t::default_())
        { }

        __assembly_layout_t &   layout;
        xil_buffer_t &          buffer;
        xil_pool_t &            xil_pool;
        logger_t &              logger;

        method_compile_controller_t * const controller;

        X_TO_STRING_IMPL(_T("method_compile_context_t"))
    };

    //-------- ---------- ---------- ---------- ----------

    // Param type collection.
    X_INTERFACE param_types_t
    {
        // Returns type count.
        virtual size_t param_count() const = 0;

        // Returns type at specified index.
        virtual typex_t param_type_at(size_t index) const = 0;

        // Returns vtype at specified index.
        vtype_t param_vtype_at(size_t index) const;
    };

    //-------- ---------- ---------- ---------- ----------

    // Base class of method.
    X_INTERFACE method_base_t : public param_types_t
    {
        // Returns name.
        virtual name_t get_name() const = 0;

        // Returns this family.
        virtual member_family_t this_family() const = 0;

        // Returns the method of interface defination.
        virtual type_t * get_owner_type() const = 0;

        // Returns method trait.
        virtual method_trait_t get_trait() const = 0;

        // Returns the type of this type.
        virtual type_t * get_type() const = 0;

        // Returns Decorates.
        virtual decorate_t * get_decorate() const = 0;

        // Converts to a string.
        virtual const string_t to_string() const = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    class method_variable_t;

    // General method.
    class method_t : public method_base_t, public __named_member_t
    {
        friend class variable_region_t;
        typedef method_base_t __super_t;

    public:
        typedef method_t * itype_t;

        type_name_t *       owner_type_name = nullptr;      // Interface type defined the method.
        type_name_t *       type_name       = nullptr;      // Returns type of the method.
        generic_params_t *  generic_params  = nullptr;      // Generic parameters.
        params_t *          params          = nullptr;      // Params.
        method_body_t *     body            = nullptr;      // Method body.
        method_variable_t * variable        = nullptr;      // Relation variable.

        // Method trait.
        method_trait_t      trait           = method_trait_t::__default__;

        // Method inline type, inline, noinline or default.
        method_inline_type_t inline_type    = method_inline_type_t::default_;

        // Returns name.
        virtual name_t get_name() const override { return name; }

        // Returns this family.
        virtual member_family_t this_family() const override
        {
            return member_family_t::general;
        }

        // Returns this member type.
        virtual member_type_t this_type() override final
        {
            return member_type_t::method;
        }

        // Returns generic param count.
        size_t generic_param_count() const
        {
            return generic_params? generic_params->size() : 0;
        }

        // Returns whether the method is a generic method.
        bool is_generic() const { return generic_param_count() > 0; }

        // Returns method trait.
        virtual method_trait_t get_trait() const override { return trait; }

        // Returns return type of the method.
        virtual type_t * get_type() const override { return to_type(type_name); }

        // Returns param count.
        virtual size_t param_count() const override
        {
            return params? params->size() : 0;
        }

        // Returns param type at specified index.
        virtual typex_t param_type_at(size_t index) const override;

        // Returns the param at the specified index.
        param_t * param_at(size_t index) const
        {
            _A(index < param_count());
            return (*params)[index];
        }

        // Finds the param by the specified name.
        generic_param_t * find_param(const name_t & name);

        // Returns interface type defined the method.
        virtual type_t * get_owner_type() const override
        {
            return owner_type_name? owner_type_name->type : nullptr;
        }

        // Returns method decorate.
        virtual decorate_t * get_decorate() const override { return decorate; }

        // Compile the method to xil.
        void compile(method_compile_context_t & ctx);

        // Converts to string.
        virtual const string_t to_string() const override { return (string_t)*this; }
        operator string_t() const;

        static const efamily_t __family__ = efamily_t::method;

    private:
        local_variables_t __local_variables;
    };

    // Returns whether the method is a constructor.
    X_INLINE bool is_ctor(method_t * method)
    {
        _A(method != nullptr);

        return method->trait == method_trait_t::constructor;
    }

    //-------- ---------- ---------- ---------- ----------
    // impl_method_t

    // Implemention method of generic method.
    // Converted all generic types with specified types.
    class impl_method_t : public method_t
    {
        typedef method_t __super_t;

    public:
        using __super_t::__super_t;

        // The raw general template method.
        method_t * raw = nullptr;

        // Type arguments.
        type_collection_t args;

        // Returns param type of specified index.
        virtual typex_t param_type_at(size_t index) const override;

        // Builds with generic args.
        void build(method_t * raw, type_collection_t & args);

        // Returns this family.
        virtual member_family_t this_family() const override
        {
            return member_family_t::impl;
        }
    };

    //-------- ---------- ---------- ---------- ----------
    // generic_method_t

    // Generic method.
    class generic_method_t : public method_base_t, public eobject_t
    {
        typedef eobject_t __super_t;

    public:

        // Constructor.
        generic_method_t() = default;

        // Constructor.
        template<typename _type_collection_t>
        generic_method_t(method_t * template_, _type_collection_t && args, type_t * host_type)
            : template_(template_), args(std::forward<_type_collection_t>(args))
            , host_type(host_type)
        {
            _A(host_type != nullptr);
        }

        // Returns name.
        virtual name_t get_name() const override { return __template()->name; }

        // Returns the family.
        virtual member_family_t this_family() const _NE override
        {
            return member_family_t::generic;
        }

        // Returns method trait.
        virtual method_trait_t get_trait() const override
        {
            return __template()->get_trait();
        }

        // Returns interface type that defined the method.
        virtual type_t * get_owner_type() const override
        {
            return __template()->get_owner_type();
        }

        // Returns the return type.
        virtual type_t * get_type() const override
        {
            return __template()->get_type();
        }

        // Returns param count.
        virtual size_t param_count() const override;

        // Returns param type of specified index.
        virtual typex_t param_type_at(size_t index) const override;

        // Returns method decorate.
        virtual decorate_t * get_decorate() const override
        {
            return __template()->get_decorate();
        }

        // Converts to string.
        X_TO_STRING

        // General template.
        method_t * template_ = nullptr;

        // Generic type arguments.
        type_collection_t args;

        // Host type.
        type_t * host_type = nullptr;

    private:

        // Returns general template.
        method_t * __template() const
        {
            _A(template_ != nullptr);
            return template_;
        }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Gets its general method.
    method_t * to_general(method_base_t * method);

    typedef eobject_ast_t<method_t *> method_ast_t;

    ////////// ////////// ////////// ////////// //////////

    class property_variable_t;

    // Property member.
    class property_t : public __named_member_t
    {
        typedef member_t __super_t;

    public:
        typedef property_t * itype_t;

        type_name_t *   owner_type_name = nullptr;  // Interface type defined the property.
        type_name_t *   type_name       = nullptr;  // Property type.
        params_t *      params          = nullptr;  // Index params.

        method_t *      get_method      = nullptr;  // The get method binded.
        method_t *      set_method      = nullptr;  // The set method binded.

        property_variable_t * variable  = nullptr;  // Property variable.

        // Returns this type.
        virtual member_type_t this_type() override final
        {
            return member_type_t::property;
        }

        // Returns this family.
        virtual member_family_t this_family() const override
        {
            return member_family_t::general;
        }

        // Returns whether it's a index. ( name is empty )
        bool is_index() const { return name == name_t::null; }

        // Converts to stirng.
        virtual const string_t to_string() const override { return (string_t)*this; }
        operator string_t() const;

        static const efamily_t __family__ = efamily_t::property;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<property_t *> property_ast_t;

    ////////// ////////// ////////// ////////// //////////

    // Impl property.
    class impl_property_t : public property_t
    {
        typedef property_t __super_t;

    public:
        using __super_t::__super_t;

        property_t * raw = nullptr;

        // Returns this family.
        virtual member_family_t this_family() const override
        {
            return member_family_t::impl;
        }
    };

    ////////// ////////// ////////// ////////// //////////

    class event_variable_t;

    // Event member.
    class event_t : public __named_member_t
    {
        typedef __named_member_t __super_t;

    public:
        typedef event_t * itype_t;

        type_name_t *   owner_type_name = nullptr;  // Interface type defined ths event.
        type_name_t *   type_name       = nullptr;  // Event type.

        method_t *      add_method      = nullptr;  // The add method when add a delegate.
        method_t *      remove_method   = nullptr;  // The rmove method when remove a delegate.

        event_variable_t * variable     = nullptr;  // The event variable.

        // Returns this type.
        virtual member_type_t this_type() override final
        {
            return member_type_t::event;
        }

        // Returns this family.
        virtual member_family_t this_family() const override
        {
            return member_family_t::general;
        }

        // Converts to string.
        virtual const string_t to_string() const override { return (string_t)*this; }
        operator string_t() const;

        static const efamily_t __family__ = efamily_t::event;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<event_t *> event_ast_t;

    ////////// ////////// ////////// ////////// //////////

    // Impl event.
    class impl_event_t : public event_t
    {
        typedef event_t __super_t;

    public:
        using __super_t::__super_t;

        event_t * raw = nullptr;

        // Returns this family.
        virtual member_family_t this_family() const override
        {
            return member_family_t::impl;
        }
    };

    ////////// ////////// ////////// ////////// //////////

    class field_variable_t;

    // Field member.
    class field_t : public __named_member_t
    {
        typedef __named_member_t __super_t;

    public:
        typedef field_t * itype_t;

        type_name_t *   type_name   = nullptr;  // Interface type defined the event.
        expression_t *  init_value  = nullptr;  // Initialize default value.
        msize_t         offset      = unknown_msize;    // Offset in it's type.

        field_variable_t * variable = nullptr;  // Field variable.

        // Returns this type.
        virtual member_type_t this_type() override final
        {
            return member_type_t::field;
        }

        // Returns this family.
        virtual member_family_t this_family() const override
        {
            return member_family_t::general;
        }

        // Returns the size.
        msize_t get_size();

        // Returns type of this field.
        type_t * get_type();

        // Returns type of this field.
        vtype_t get_vtype();

        // Returns whether it's a reference type.
        bool is_ref_type();

        // Converts a string.
        virtual const string_t to_string() const override { return (string_t)*this; }
        operator string_t() const;

        static const efamily_t __family__ = efamily_t::field;

    private:
        msize_t __size = unknown_msize;
        msize_t __get_size();
    };

    typedef eobject_ast_t<field_t *> field_ast_t;
    typedef eobjects_ast_t<field_t *> fields_ast_t;

    ////////// ////////// ////////// ////////// //////////

    // Impl field.
    class impl_field_t : public field_t
    {
        typedef field_t __super_t;

    public:
        using __super_t::__super_t;

        field_t * raw = nullptr;

        // Returns this family.
        virtual member_family_t this_family() const override
        {
            return member_family_t::impl;
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // Position field.
    class position_field_t : public field_t
    {
        typedef field_t __super_t;

    public:
        using __super_t::__super_t;

        int position = 0;

        virtual member_family_t this_family() const override
        {
            return member_family_t::position;
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // Param param of typedef.
    class type_def_param_t : public __named_type_t, public with_index_t
    {
        typedef __named_type_t __super_t;

    public:
        using __super_t::__super_t;

        typedef type_def_param_t * itype_t;

        bool    extends = false;

        // Returns gtype, ttype, vtype.
        virtual gtype_t this_gtype() override final { return gtype_t::type_def_param; }
        virtual ttype_t this_ttype() override final { return ttype_t::__unknown__; }
        virtual vtype_t this_vtype() override final { return vtype_t::__unknown__; }
        virtual mtype_t this_mtype() override final { return mtype_t::__unknown__; }

        // Returns member descripted by the args.
        virtual member_t * get_member(analyze_member_args_t & args) override;

        // Returns members descripted by the args.
        virtual void get_members(analyze_members_args_t & args) override;

        // Returns all members.
        virtual void get_all_members(members_t & out_members) override;

        // Enum all super types, Applies elements by calling callback function.
        virtual void each_super_type(each_super_type_callback_t callback) override { }

        // Converts to string.
        virtual const string_t to_string() const override { return (string_t)*this; }

        // Returns the short name.
        virtual const string_t to_short_name() const override { return (string_t)*this; }

        // Returns the identity.
        virtual const string_t to_identity() const override { return _T("?") + _str(name); }
    };

    typedef eobject_ast_t<type_def_param_t *> type_def_param_ast_t;
    typedef indexed_eobjects_t<type_def_param_t *> type_def_params_t;
    typedef eobject_ast_t<type_def_params_t *> type_def_params_ast_t;

    ////////// ////////// ////////// ////////// //////////

    // typedef member.
    class type_def_t : public __named_member_t
    {
        typedef __named_member_t __super_t;

    public:
        using __super_t::__super_t;

        typedef type_def_t * itype_t;

        type_name_t *       type_name  = nullptr;   // Type defined.
        type_def_params_t * params     = nullptr;   // Generic params.
        namespace_t *       namespace_ = nullptr;   // Its namespace, (When not defined in a type).

        // Returns this type.
        virtual member_type_t this_type() override final
        {
            return member_type_t::type_def;
        }

        // Converts to a string.
        virtual const string_t to_string() const override { return (string_t)*this; }

        // Returns param count.
        size_t param_count() const
        {
            return params? params->size() : 0;
        }

        // Find param by specified name.
        type_def_param_t * find_param(const name_t & name);

        // Returns defined type.
        type_t * get_type() const;

        // Converts to string.
        operator string_t() const;
    };

    typedef eobject_ast_t<typename type_def_t::itype_t> type_def_ast_t;

    ////////// ////////// ////////// ////////// //////////

    // Impl typedef.
    class impl_type_def_t : public type_def_t
    {
        typedef type_def_t __super_t;

    public:
        using __super_t::__super_t;

        type_def_t * raw = nullptr;

        // Returns this family.
        virtual member_family_t this_family() const override
        {
            return member_family_t::impl;
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // Member collection.
    template<typename _member_t, size_t size>
    class __member_vector_t : public al::svector_t<_member_t, size>
    {
        typedef al::svector_t<_member_t, size> __super_t;

    public:
        using __super_t::__super_t;

        // Pushes a member to back.
        _member_t push_back(_member_t member)
        {
            _A(member != nullptr);

            __super_t::push_back(member);
            __member_map[member->get_name()] = member;

            return member;
        }

        // Gets its name.
        _member_t get(name_t name)
        {
            auto it = __member_map.find(name);
            if (it == __member_map.end())
                return nullptr;

            return it->second;
        }

    private:
        std::map<name_t, _member_t> __member_map;
    };

    //-------- ---------- ---------- ---------- ----------

    // Nest type member collection.
    template<size_t size>
    class __member_vector_t<type_t *, size> : public al::svector_t<type_t *, size>
    {
        typedef al::svector_t<type_t *, size> __super_t;
        typedef int __generic_args_count_t;

        typedef std::tuple<name_t, __generic_args_count_t> __key_t;

    public:
        using __super_t::__super_t;

        // Spushes a nest type to back.
        type_t * push_back(type_t * type);

        // Enumerate all nest types with specified name.
        template<typename _f_t>
        void each(name_t name, _f_t f)
        {
            for (auto it = __type_map.lower_bound(name), it_end = __type_map.upper_bound(name);
                it != it_end; it++)
            {
                f(it->second);
            }
        }

        // Enumerate all nest types with specified name and generic param count.
        type_t * get(name_t name, int generic_param_count)
        {
            __key_t key(name, generic_param_count);
            for (auto it = __types_map.lower_bound(key), it_end = __types_map.upper_bound(key);
                it != it_end; it++)
            {
                return it->second;
            }

            return nullptr;
        }

    private:
        std::multimap<name_t, type_t *> __type_map;
        std::multimap<__key_t, type_t *> __types_map;
    };

    //-------- ---------- ---------- ---------- ----------

    // Method member collection.
    template<size_t size>
    class __member_vector_t<method_t *, size> : public al::svector_t<method_t *, size>
    {
        typedef al::svector_t<method_t *, size> __super_t;
        typedef int __generic_args_count_t;
        typedef type_t * __owner_t;

        typedef std::tuple<name_t, __generic_args_count_t,  __owner_t> __key_t;

    public:
        using __super_t::__super_t;

        // Pushes a method to back.
        method_t * push_back(method_t * method)
        {
            _A(method != nullptr);

            __super_t::push_back(method);

            type_t * owner_type = method->get_owner_type();

            __method_map.insert(std::make_pair(
                __key_t(method->name, method->generic_param_count(), owner_type), method
            ));

            __method_map.insert(std::make_pair(
                __key_t(method->name, __unknown_size,  owner_type), method
            ));

            return method;
        }

        // Search methods by name, generic param count and it's owner type.
        // Applies them by calling function f.
        template<typename f_t>
        void each(name_t name, int generic_param_count, type_t * owner_type, f_t f)
        {
            __key_t key(name, generic_param_count, owner_type);
            for (auto it = __method_map.lower_bound(key), it_end = __method_map.upper_bound(key);
                it != it_end; it++)
            {
                f(it->second);
            }
        }

        // Search methods by name, generic param count.
        // Applies them by calling function f.
        template<typename f_t>
        void each(name_t name, int generic_param_count, f_t f)
        {
            each(name, generic_param_count, (type_t *)nullptr, f);
        }

        // Search methods by trait.
        // Applies them calling function f.
        template<typename f_t>
        void each(method_trait_t trait, f_t f)
        {
            __ensure_init_trait_method_map();

            for (auto it = __trait_method_map.lower_bound(trait),
                it_end = __trait_method_map.upper_bound(trait); it != it_end; it++)
            {
                f(it->second);
            }
        }

    private:
        std::multimap<__key_t, method_t *> __method_map;
        std::multimap<method_trait_t, method_t *> __trait_method_map;

        // Ensure initialized the method map.
        void __ensure_init_trait_method_map()
        {
            if (__trait_method_map.empty())
            {
                for (method_t * method : *this)
                {
                    __trait_method_map.insert(std::make_pair(method->trait, method));
                }
            }
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // General like type.
    class __general_type_like_base_t : public type_t
    {
        typedef type_t __super_t;

        template<typename t, size_t size = 2>
        using __list_t = owner_collection_t<type_t *, t, size, __member_vector_t<t, size>>;

    public:
        // Constructors
        __general_type_like_base_t()
            : methods(this), properties(this)
            , fields(this), events(this), nest_types(this), type_defs(this)
        { }

        typedef __vector_t<type_name_t *>     super_type_name_list_t;
        typedef __list_t<method_t *, 5>       method_list_t;
        typedef __list_t<property_t *, 5>     property_list_t;
        typedef __list_t<event_t *>           event_list_t;
        typedef __list_t<field_t *, 5>        field_list_t;
        typedef __list_t<type_t *>            nest_type_list_t;
        typedef __list_t<type_def_t *>        type_def_list_t;

        super_type_name_list_t  super_type_names;       // Super types
        method_list_t           methods;                // Methods
        property_list_t         properties;             // Properties
        field_list_t            fields;                 // Fields.
        event_list_t            events;                 // Events.
        nest_type_list_t        nest_types;             // Nest types.
        type_def_list_t         type_defs;              // Typedefs.

        // Returns member descripted by specified args.
        virtual member_t * get_member(analyze_member_args_t & args) override;

        // Returns members descripted by specified args.
        virtual void get_members(analyze_members_args_t & args) override;

        // Returns all members.
        virtual void get_all_members(members_t & out_members) override;

        // Commits
        virtual void commit(eobject_commit_context_t & ctx) override;

        // Returns whether it is commited.
        bool committed() const { return __committed; }

        // Returns vsize.
        virtual msize_t get_vsize() override;

        // Returns count of reference types.
        virtual msize_t get_rcount() override;

        // Returns base type.
        virtual type_t * get_base_type() override;

        // Enum all super types. Applies each types for calling callback method.
        virtual void each_super_type(each_super_type_callback_t callback) override;

        // Returns underlying type of enum type.
        vtype_t get_underlying_vtype();

    protected:

        msize_t __rcount = unknown_msize, __value_size = unknown_msize;

        class __transform_context_t;
        void __transform(__transform_context_t & tctx);

        // Returns type of specified name.
        type_t * __type_at(general_type_t * template_, const type_collection_t & args,
                                                                name_t name) const;

        // Returns types of specified name. (for extends generic arguments)
        bool __types_at(general_type_t * template_, const type_collection_t & args,
                            name_t type_name, std::function<void (typex_t)> f);

    private:

        // Ensure size is initialized.
        void __ensure_size_initialize();
        bool __committed = false;

        typedef __transform_context_t __tctx_t;

        // Transform member to its implementation with type arguments.
        template<typename _members_t>
        void __transform_members(__tctx_t & tctx, _members_t & members, _members_t & out_members);

        // Transform typename to its implemenation with type arguments.
        type_name_t * __transform_type_name(__tctx_t & tctx, type_name_t * type_name);

        // Transform field to its implemenation with type arguments.
        field_t *     __transform_member(__tctx_t & tctx, field_t * field);

        // Transform method to its implemenation with type arguments.
        method_t *    __transform_member(__tctx_t & tctx, method_t * method);

        // Transform property to its implemenation with type arguments.
        property_t *  __transform_member(__tctx_t & tctx, property_t * property);

        // Transform event to its implemenation with type argumentss.
        event_t *     __transform_member(__tctx_t & tctx, event_t * event);

        // Transform nest type to its implemenation with type arguments.
        general_type_t * __transform_member(__tctx_t & tctx, type_t * nest_type);

        // Transform typedef to its implemenation with type arguments.
        type_def_t *  __transform_member(__tctx_t & tctx, type_def_t * type_def);

        // Transform type_name to its implemenation with type arguments.
        type_name_t * __transform_member(__tctx_t & tctx, type_name_t * type_name);

        // Transform tuple type.
        void __transform_tuple_type(__tctx_t & tctx); 

        // Find type(s) of specified name.
        template<typename _pred_t, typename _f_t>
        bool __find_type(general_type_t * template_, const type_collection_t & args,
                                    _pred_t pred, _f_t f) const;
    };

    ////////// ////////// ////////// ////////// //////////

    // Interface of core type implemention.
    X_INTERFACE type_impl_t : object_t
    {
        // Commit it.
        virtual void commit(eobject_commit_context_t & ctx, eobject_t * bind_object) = 0;

        static type_impl_t * default_();
    };

    ////////// ////////// ////////// ////////// //////////

    // General type.
    class general_type_t : public __general_type_like_base_t
    {
        typedef __general_type_like_base_t __super_t;

    public:
        using __super_t::__super_t;

        // Constructor.
        general_type_t() = default;

        name_t        name       = name_t::null;    // Name.
        assembly_t  * assembly   = nullptr;         // Its assembly.
        namespace_t * namespace_ = nullptr;         // Its namespace.

        generic_params_t * params = nullptr;        // Generic params.

        type_impl_t * impl = type_impl_t::default_();   // Type implemenation for core types.

        vtype_t vtype = vtype_t::mobject_;          // vtype.
        ttype_t ttype = ttype_t::class_;            // ttype.
        mtype_t mtype = mtype_t::__default__;       // mtype.

        // Appends a member.
        void append_member(member_t * member);

        // Nest depth.
        size_t nest_depth() const;

        // Returns gtype, ttype, vtype.
        virtual gtype_t this_gtype() override final { return gtype_t::general; }
        virtual ttype_t this_ttype() override final { return ttype; }
        virtual vtype_t this_vtype() override final;
        virtual mtype_t this_mtype() override final { return mtype; }

        // Returns the name of the type.
        virtual name_t get_name() const override { return name; }

        // Returns full name of the type.
        virtual const string_t to_full_name() const override;

        // Returns short name of the type.
        virtual const string_t to_short_name() const override;

        // Returns identity of the type.
        virtual const string_t to_identity() const override;

        // Returns param count.
        size_t params_count() const { return params? params->size() : 0; }

        // Returns param at specified position.
        generic_param_t * param_at(size_t index) const;

        // Returns if specified param count match this type.
        bool is_match(size_t params_count) const;

        // Returns whether it is has generic params.
        bool is_generic() const { return params_count() > 0; }

        // Returns if it has uncertain count params.
        bool uncertain_params() const;

        // Commits it.
        virtual void commit(eobject_commit_context_t & ctx) override;

        // Find param by name.
        generic_param_t * find_param(const name_t & name);

        // Returns vsize.
        virtual msize_t get_vsize() override;

        // Returns count of reference type.
        virtual msize_t get_rcount() override;

    private:
        bool __ensure_size_initialize();
    };

    //-------- ---------- ---------- ---------- ----------
    // generic_type_t

    // Generic type.
    class generic_type_t : public __general_type_like_base_t, public param_types_t
    {
        typedef __general_type_like_base_t __super_t;

    public:

        // Constructor
        generic_type_t() = default;

        // Constructor.
        template<typename _type_collection_t>
        generic_type_t(general_type_t * template_, _type_collection_t && args,
                                                    type_t * host_type = nullptr)
            : template_(template_), args(std::forward<_type_collection_t>(args))
        {
            this->host_type = host_type;
        }

        // General template type.
        general_type_t * template_ = nullptr;

        // Type arguments.
        type_collection_t args;

        // Returns gtype.
        virtual gtype_t this_gtype() _NE override final { return gtype_t::generic; }

        // Returns ttype.
        virtual ttype_t this_ttype() _NE override final
        {
            return template_? template_->this_ttype() : ttype_t::__unknown__;
        }

        // Returns vtype.
        virtual vtype_t this_vtype() _NE override final
        {
            return template_? template_->this_vtype() : vtype_t::__unknown__;
        }

        virtual mtype_t this_mtype() _NE override final
        {
            return template_? template_->this_mtype() : mtype_t::__unknown__;
        }

        // Gets the name of the method.
        virtual name_t get_name() const override
        {
            return template_? template_->get_name() : name_t::null;
        }

        // Commits it.
        void commit(eobject_commit_context_t & ctx) override;

        // Returns generic argument count.
        size_t argument_count() const _NE { return args.size(); }

        // Returns whether it has any typedef param.
        bool any_type_def_param() const _NE;

        // Returns type at specified index.
        type_t * type_at(size_t index) const _NE;

        // Returns type of specified name.
        type_t * type_at(name_t name) const;

        // Returns param type count. (for delegate type)
        virtual size_t param_count() const override;

        // Returns type at specified index. (for delegate type)
        virtual typex_t param_type_at(size_t index) const override;

        // Returns fullname of the type.
        virtual const string_t to_full_name() const override { return (string_t)*this; }

        // Returns short name of the type.
        virtual const string_t to_short_name() const override;

        // Returns identity of the type.
        virtual const string_t to_identity() const override;

        // Returns whether it has no specified argument type.
        bool is_no_specialization() const { return al::all_of_empty(args); }

        // Returns whether is is partial specialization.
        bool is_partial_specialization() const { return al::any_of_empty(args); }
    };

    //-------- ---------- ---------- ---------- ----------
    // Implements of template<size_t size> __member_vector_t<type_t *, size>

    // Pushes a general nest type to back.
    template<size_t size>
    type_t * __member_vector_t<type_t *, size>::push_back(type_t * type)
    {
        _A(type != nullptr);
        __super_t::push_back(type);

        switch (type->this_gtype())
        {
            case gtype_t::general: {
                general_type_t * general_type = (general_type_t *)type;
                __type_map.insert(std::make_pair(general_type->name, type));
                __types_map.insert(std::make_pair(
                    __key_t(general_type->name, general_type->params_count()), type
                ));
            }   break;

            case gtype_t::generic: {
                generic_type_t * generic_type = (generic_type_t *)type;
                general_type_t * general_type = generic_type->template_;
                __type_map.insert(std::make_pair(general_type->name, type));
                __types_map.insert(std::make_pair(
                    __key_t(general_type->name, general_type->params_count()), type
                ));
            }   break;

            default:
                X_UNEXPECTED();
        }

        return type;
    }

    //-------- ---------- ---------- ---------- ----------
    // array_type_t

    typedef int16_t dimension_t;
    typedef al::svector_t<dimension_t, 2> dimensions_t;

    // Array type.
    class array_type_t : public type_t
    {
    public:

        // Constructor.
        array_type_t(type_t * base_type, type_t * element_type, dimension_t dimension)
            : base_type(base_type), element_type(element_type), dimension(dimension)
        {
            _A(base_type != nullptr);
        }

        type_t *    element_type  = nullptr;        // Element type of the array.
        type_t *    base_type     = nullptr;        // Base type.
        dimension_t dimension     = 1;              // Dimension.

        // Returns gtype, gtype, vtype.
        virtual gtype_t this_gtype() override final { return gtype_t::array;  }
        virtual ttype_t this_ttype() override final { return ttype_t::class_; }
        virtual vtype_t this_vtype() override final { return vtype_t::mobject_; }
        virtual mtype_t this_mtype() override final { return mtype_t::__default__; }

        // Returns name of the type.
        virtual name_t get_name() const override
        {
            return element_type? element_type->get_name() : name_t::null;
        }

        // Returns member descripted by the args.
        virtual member_t * get_member(analyze_member_args_t & args) override;

        // Returns members descripted by the args.
        virtual void get_members(analyze_members_args_t & args) override;

        // Returns all members.
        virtual void get_all_members(members_t & out_members) override;

        // Enum super types by applying each elements for callback function.
        virtual void each_super_type(each_super_type_callback_t callback) override;

        // Commits it.
        virtual void commit(eobject_commit_context_t & ctx) override;

        // Returns vsize.
        virtual msize_t get_vsize() override;

        // Returns count of reference types.
        virtual msize_t get_rcount() override;

        // Returns full name of the array type.
        virtual const string_t to_full_name() const override { return (string_t)*this; }

        // Returns identity of the array type. e.g. System.Int32[] .
        virtual const string_t to_identity() const override;

    private:
        type_t * __base_type;
    };

    ////////// ////////// ////////// ////////// //////////
    // system_types

    // Gets system type by the name.
    general_type_t * get_system_type(const string_t & type_name);

    ////////// ////////// ////////// ////////// //////////
    // namespace

    class general_type_t;

    // Namespace eobject.
    class namespace_t : public with_attributes_object_t
                      , public with_decorate_object_t
                      , public eobject_t
    {
    public:
        typedef namespace_t * itype_t;

        typedef __vector_t<type_t *, 5>   type_list_t;
        typedef __vector_t<namespace_t *> namespace_list_t;
        typedef __vector_t<type_def_t *>  type_def_list_t;

        emname_t *          name        = nullptr;      // Name
        const mname_t *     full_name   = nullptr;      // Fullname
        type_list_t         types;                      // All types.
        namespace_list_t    namespaces;                 // Sub namespaces.
        type_def_list_t     type_defs;                  // All typedefs.

        // Append a type.
        void append_type(general_type_t * type);

        // Converts to string.
        operator string_t() const;
        virtual const string_t to_string() const override { return (string_t)*this; }

        static const efamily_t __family__ = efamily_t::namespace_;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<namespace_t *> namespace_ast_t;

    ////////// ////////// ////////// ////////// //////////
    // document

    // Document eobject.
    class document_t : public eobject_t
    {
    public:

        // Adds a import eobject.
        void add_import(import_t * import)
        {
            _A(import != nullptr);
            __imports.push_back(import);
        }

        // Adds a using eobject.
        void add_using(using_namespace_t * using_namespace)
        {
            _A(using_namespace != nullptr);
            __using_namespaces.push_back(using_namespace);
        }

        // Applies all import eobjects for callback function.
        template<typename callback_t> void all_imports(callback_t callback)
        {
            al::for_each(__imports, callback);
        }

        // Returns all import eobjects.
        auto all_imports() { return _range(__imports); }

        // Applies all using namespace for callback function.
        template<typename callback_t> void all_using_namespaces(callback_t callback)
        {
            al::for_each(__using_namespaces, callback);
        }

        // Returns all using namespaces.
        auto all_using_namespaces() { return _range(__using_namespaces); }

        static const efamily_t __family__ = efamily_t::document;

    private:
        __vector_t<import_t *, 10> __imports;
        __vector_t<using_namespace_t *, 10> __using_namespaces;
    };

    typedef eobject_ast_t<document_t *> document_ast_t;

    ////////// ////////// ////////// ////////// //////////
    // module_t

    // Module eobject.
    class module_t : public eobject_t
    {
    public:

        // Register commit eobject that needed.
        void register_commit(eobject_t * eobj);

        // Commits it.
        virtual void commit(eobject_commit_context_t & ctx) override;

        static const efamily_t __family__ = efamily_t::module;

        // Converts to string.
        X_TO_STRING

    private:
        std::vector<eobject_t *> __need_commit_eobjects;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_collection_cache_t

    typedef multikey_t<type_collection_t> type_collection_key_t;

    // Key of type collection.
    struct type_collection_id_t : compare_operators_t<type_collection_id_t, void *>
    {
        typedef void * __id_t;

        // Constructor.
        type_collection_id_t() : __id(nullptr) { }
        type_collection_id_t(__id_t id) : __id(id) { }

        // Returns the id.
        operator __id_t () const { return __id; }

        // Converts to string.
        operator string_t() const { return _F(_T("%1%"), __id); }

        __id_t __id;

        // Returns whether the id is empty.
        bool empty() const { return __id == nullptr; }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Type collection cache.
    class type_collection_cache_t : public object_t
    {
        typedef type_collection_id_t  __tcid_t;
        typedef type_collection_key_t __key_t;

    public:

        // Returns type collection id.
        template<typename _type_collection_t>
        __tcid_t get_tcid(_type_collection_t && types)
        {
            if (types.empty())
                return __tcid_t(nullptr);

            if (types.size() == 1)
                return __tcid_t((void *)types[0]);

            auto r = __set.insert(__key_t(std::forward<_type_collection_t>(types)));
            return __tcid_t((void *)&(*r.first));
        }

        X_TO_STRING_IMPL(_T("type_collection_cache_t"))

    private:
        std::set<type_collection_key_t> __set;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Generic type cache key.
    typedef std::tuple<general_type_t *, type_collection_id_t, type_t *> generic_type_cache_key_t;
    typedef kv_object_t<generic_type_cache_key_t, generic_type_t *>  __generic_type_cache_t;

    // Array type cache key.
    typedef std::tuple<type_t *, dimension_t> array_type_cache_key_t;
    typedef kv_object_t<array_type_cache_key_t, array_type_t *> __array_type_cache_t;

    // Generic method cache key.
    typedef std::tuple<method_t *, type_collection_id_t, type_t *> generic_method_cache_key_t;
    typedef kv_object_t<generic_method_cache_key_t, generic_method_t *> __generic_method_cache_t;

    // Assembly reference cache key.
    class assembly_reference_t;
    typedef std::tuple<const mname_t *, const mname_t *> assembly_reference_cache_key_t;
    typedef kv_object_t<assembly_reference_cache_key_t, assembly_reference_t *>
                                                                __assembly_reference_cache_t;

    ////////// ////////// ////////// ////////// //////////

    // Extesion type collection.
    struct xtype_collection_t : no_copy_ctor_t
    {
    public:

        // Constructor.
        xtype_collection_t(const type_collection_t & types) : types(types) { }
        xtype_collection_t(const type_collection_t & types, type_collection_id_t tcid)
            : types(types), tcid(tcid)
        { }

        const type_collection_t & types;    // Type collections.
        type_collection_id_t      tcid;     // Type collection id.

        // Returns type collection.
        operator const type_collection_t & () const _NE { return types; }

        // Return size.
        size_t size()  const _NE { return types.size(); }

        // Returns whether the collection is empty.
        bool   empty() const _NE { return size() == 0;  }

        // Returns type collection id.
        type_collection_id_t get_tcid();
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Xpool, including all of the arguments for compiling.
    // Including string pool, memory management etc.
    class xpool_t : public object_t, public no_copy_ctor_t
    {
    public:

        // Constructor
        xpool_t(memory_t * memory);

        // Initialize.
        void initialize();

        al::spool_t         spool;                      // String pool.
        memory_t * const    memory = nullptr;           // Memory management.
        __mname_cache_t *   mname_cache = nullptr;      // Mname cache.

        // Generic type cache.
        __generic_type_cache_t   * generic_type_cache   = nullptr;

        // Array type cache.
        __array_type_cache_t     * array_type_cache     = nullptr; 

        // Generic method cache.
        __generic_method_cache_t * generic_method_cache = nullptr;

        // Assembly reference cache.
        __assembly_reference_cache_t * assembly_reference_cache = nullptr;

        // Type collection.
        type_collection_cache_t type_collection;

        // Creates a new name_t object of specified string.
        name_t to_name(const char_t * s) { return name_t(spool.to_sid(s)); }

        // Creates a new name_t object of specified string.
        name_t to_name(const string_t & s) { return to_name(s.c_str()); }

        // Creates a new generic type.
        generic_type_t * new_generic_type(general_type_t * template_,
                                          xtype_collection_t & types, type_t * host_type);

        // Creates a new generic type.
        generic_type_t * new_generic_type(general_type_t * template_,
                                          type_collection_t & types, type_t * host_type);

        // Creates a new array type.
        array_type_t * new_array_type(type_t * element_type, dimension_t dimension);

        // Creates a new generic method.
        generic_method_t * new_generic_method(method_t * template_, type_collection_t & types,
                                          type_t * host_type);

        // Creates a new generic method.
        generic_method_t * new_generic_method(method_t * template_, xtype_collection_t & types,
                                          type_t * host_type);

        // Creates a new assembly reference.
        assembly_reference_t * new_reference(const mname_t * package_name, assembly_t * assembly);

        // Returns compile time attribute.
        attribute_t    * get_compile_time_attribute();

        // Returns type of the vtype.
        general_type_t * get_internal_type(vtype_t vtype);

        // Returns type of the value type.
        general_type_t * get_internal_type(value_type_t value_type);

        // Returns type of the name.
        general_type_t * get_internal_type(name_t name);

        // Returns System.Object type.
        general_type_t * get_object_type();

        // Returns System.Object type name.
        type_name_t * get_object_type_name();

        // Returns System.Void type.
        general_type_t * get_void_type();

        // Returns System.Array<> type.
        general_type_t * get_tarray_type();

        // Returns System.Array type.
        general_type_t * get_array_type();

        // Returns System.String type.
        general_type_t * get_string_type();

        // Returns System.Type type.
        general_type_t * get_type_type();

        // Returns System.Enum type.
        general_type_t * get_enum_type();

        // Returns System.Ptr type.
        general_type_t * get_ptr_type();

        // Returns System.Delegate type.
        general_type_t * get_delegate_type();

        // Returns System.MulticastDelegate type.
        general_type_t * get_multicast_delegate_type();

        // Returns System.Exception type.
        general_type_t * get_exception_type();

        // Returns System.Tuple<...> type.
        general_type_t * get_tuple_type();

        // Returns System.Diagnostics.TraceAttribute type.
        general_type_t * get_trace_attribute_type();

        // Returns System.Reflection.ReflectionInfo type.
        general_type_t * get_reflection_info_type();

        // Returns System.Reflection.Assembly type.
        general_type_t * get_assembly_type();

        // Returns System.Reflection.Member type.
        general_type_t * get_member_type();

        // Returns System.Reflection.Method type.
        general_type_t * get_method_type();

        // Returns System.Reflection.Property type.
        general_type_t * get_property_type();

        // Returns System.Reflection.Event type.
        general_type_t * get_event_type();

        // Returns System.Reflection.Field type.
        general_type_t * get_field_type();

        // Returns System.Reflection.Parameter type.
        general_type_t * get_parameter_type();

        // Returns System.Reflection.GenericParameter type.
        general_type_t * get_generic_parameter_type();

        // Creates a new object.
        template<typename t, typename ... args_t> t * new_obj(args_t && ... args)
        {
            return memory_t::new_obj<t>(memory, std::forward<args_t>(args) ...);
        }

        // Append a new type.
        void append_new_type(type_t * type);

        // Returns whether it's a core assembly.
        bool is_core_assembly(assembly_t * assembly);

        // Commit types.
        void commit_types(logger_t & logger);

        // Returns the typename of vtype.
        type_name_t * to_type_name(vtype_t vtype);

        // Returns the typename of type.
        type_name_t * to_type_name(type_t * type);

        // Returns cvalue_expression_t of specified cvalue.
        cvalue_expression_t * get_cvalue_expression(cvalue_t cvalue);

        // Returns current xpool.
        static xpool_t & current()
        {
            _A(__current != nullptr);
            return *__current;
        }

        X_TO_STRING_IMPL(_T("xpool_t"))

    private:
        attribute_t * __compile_time_attribute = nullptr;
        std::queue<type_t *> __new_types;
        std::map<sid_t, general_type_t *> __new_system_type_dict;

        // Returns specified type with cache.
        general_type_t * __get_specified_type(const char_t * name, general_type_t * & __cache);

        general_type_t * __object_type      = nullptr;
        general_type_t * __void_type        = nullptr;
        general_type_t * __tarray_type      = nullptr;
        general_type_t * __array_type       = nullptr;
        general_type_t * __string_type      = nullptr;
        general_type_t * __type_type        = nullptr;
        general_type_t * __enum_type        = nullptr;
        general_type_t * __ptr_type         = nullptr;
        general_type_t * __delegate_type    = nullptr;
        general_type_t * __multicast_delegate_type = nullptr;

        general_type_t * __exception_type   = nullptr;
        general_type_t * __tuple_type       = nullptr;
        general_type_t * __trace_attribute_type = nullptr;

        general_type_t * __reflection_info_type = nullptr;
        general_type_t * __assembly_type    = nullptr;
        general_type_t * __member_type      = nullptr;
        general_type_t * __method_type      = nullptr;
        general_type_t * __property_type    = nullptr;
        general_type_t * __event_type       = nullptr;
        general_type_t * __field_type       = nullptr;
        general_type_t * __parameter_type   = nullptr;
        general_type_t * __generic_parameter_type = nullptr;

        type_name_t __object_type_name;

        sid_t __core_assembly_name = sid_t::null;

        // Vtype -> typename cache.
        std::map<vtype_t, type_name_t *> __vtype_type_name_map;

        // type_t * -> typename cache.
        std::map<type_t *, type_name_t *> __type_type_name_map;

        class __cvalue_t : public cvalue_t
        {
        public:
            __cvalue_t(const cvalue_t & value)
            {
                *(cvalue_t *)this = value;
            }

            bool operator < (const __cvalue_t & other) const
            {
                if (value_type == cvalue_type_t::number && other.value_type == cvalue_type_t::number)
                {
                    if (number.type != other.number.type)
                        return number.type < other.number.type;

                    return number < other.number;
                }

                return *(cvalue_t *)this < *(cvalue_t *)&other;
            }
        };

        // cvalue_t -> cvalue_expression_t * cache.
        std::map<__cvalue_t, cvalue_expression_t *> __cvalue_expression_map;

        static xpool_t * __current; 
    };

    #define __XPool     xpool_t::current()
    #define __SPool     __XPool.spool
    #define __Memory    __XPool.memory

    // Returns whether it is a compile time attribute.
    X_INLINE bool is_compile_time_attribute(attribute_t * attr)
    {
        return __XPool.get_compile_time_attribute() == attr;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Converts pool to mname operation context.
    X_INLINE mname_operate_context_t to_mname_operate_context()
    {
        return mname_operate_context_t(__XPool.spool, __XPool.memory, __XPool.mname_cache);
    }

    ////////// ////////// ////////// ////////// //////////
    // variable_t

    X_ENUM_(variable_type_t, 2)

        local           =   1 << 0,             // Local valiable in method.

        param           =   1 << 1,             // Param variable of method.

        field           =   1 << 2,             // Field variable of type.

        property        =   1 << 3,             // Property variable of type.

        event           =   1 << 4,             // Event variable of type.

        method          =   1 << 5,             // Method variable of type.

        array_index     =   1 << 6,             // Array index variable.

        property_index  =   1 << 7,             // Property index.

    X_ENUM_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Variable
    X_INTERFACE variable_t : object_t
    {
        // Returns variable name.
        virtual name_t get_name() const = 0;

        // Returns type of the varible.
        virtual type_t * get_type() = 0;

        // Returns this variable's type.
        virtual variable_type_t this_type() = 0;

        // Returns if it's allowd duplicate in a region, like methods.
        virtual bool allow_duplicate() = 0;

        // Returns whether this will call a method.
        virtual bool is_calling() = 0;

        // Execute the variable.
        virtual cvalue_t execute() = 0;

        // Returns vtype.
        virtual vtype_t get_vtype() = 0;

        // Returns relation member.
        virtual member_t * get_relation_member() { return nullptr; }

        // Returns whether it's a left value.
        virtual bool is_lvalue() = 0;

        // Write count.
        uint16_t write_count = 0;

        // Read count.
        uint16_t read_count = 0;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    namespace
    {
        // Base class of variable.
        template<variable_type_t _type, bool _allow_duplicate = false>
        class __variable_base_t : public variable_t
        {
            typedef variable_t __super_t;

        public:

            // Returns this variable's type.
            virtual variable_type_t this_type() _NE override { return _type; }

            // Returns if it's allowd duplicate in a region, like methods.
            virtual bool allow_duplicate() _NE override { return _allow_duplicate; }

            // Returns whether this will call a method.
            virtual bool is_calling() _NE override { return false; }

            // Execute the variable.
            virtual cvalue_t execute() override
            {
                return cvalue_t::nan;
            }

            static const variable_type_t type = _type;
            static const bool allow_duplicate_ = _allow_duplicate;
        };

        // - - - - - - - - - - - - - - - - - - - - - - - - - -

        // Base class of index variable .
        template<variable_type_t _type>
        class __index_variable_base_t : public __variable_base_t<_type>
        {
            typedef __variable_base_t<_type> __super_t;

        public:
            __index_variable_base_t(arguments_t * arguments) : arguments(arguments)
            {
                _A(arguments != nullptr);
            }

            arguments_t * arguments = nullptr;

            dimension_t dimension() const
            {
                return arguments == nullptr? 0 : arguments->size();
            }

            // Returns whether this will call a method.
            virtual bool is_calling() _NE override { return true; }
        };
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Local variable.
    class local_variable_t : public __variable_base_t<variable_type_t::local>
    {
    public:

        // Constructor.
        local_variable_t(type_name_t * type_name, name_t name)
            : type_name(type_name), name(name)
        {
            _A(type_name != nullptr);
            _A(name != name_t::null);
        }

        type_name_t *  type_name    = nullptr;          // Type of the variable.
        name_t         name         = name_t::null;     // Name of the variable.
        msize_t        identity     = unknown_msize;    // Identity of the variable.
        msize_t        index        = unknown_msize;    // Index of the variable.
        bool           constant     = false;            // A constant variable.
        bool           inline_      = false;            // Cound be inline. ( only write/read once. )
        expression_t * expression   = nullptr;          // Constant initialize value.

        // Returns this variable's type.
        virtual type_t * get_type() override { return to_type(type_name); }

        // Returns vtype.
        virtual vtype_t get_vtype() override;

        // Returns name of this variable.
        virtual name_t get_name() const override { return name; }

        // Returns whether it's a left value.
        virtual bool is_lvalue() override { return true; }

        // Execute the variable.
        cvalue_t execute() override;

        // Converts to a string.
        X_TO_STRING
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Param variable.
    class param_variable_t : public __variable_base_t<variable_type_t::param>
    {
    public:

        // Constructor.
        param_variable_t(param_t * param) : param(param)
        {
            _A(param != nullptr);
        }

        // Param relation.
        param_t * param     = nullptr;

        // Returns type of the param.
        virtual type_t * get_type() override;

        // Returns vtype.
        virtual vtype_t get_vtype() override;

        // Returns name of the param.
        virtual name_t get_name() const override
        {
            return param->name;
        }

        // Returns whether it's a left value.
        virtual bool is_lvalue() override { return true; }

        // Converts to a string.
        X_TO_STRING
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Field variable.
    class field_variable_t : public __variable_base_t<variable_type_t::field>
    {
    public:

        // Constructor.
        field_variable_t(field_t * field) : field(field)
        {
            _A(field != nullptr);
        }

        // Relation field.
        field_t * field = nullptr;

        // Returns type of the field.
        virtual type_t * get_type() override
        {
            return to_type(field->type_name);
        }

        // Returns vtype.
        virtual vtype_t get_vtype() override;

        // Returns relation member.
        virtual member_t * get_relation_member() override { return field; }

        // Retuns name of the field.
        virtual name_t get_name() const override
        {
            return field->name;
        }

        // Returns whether it's a left value.
        virtual bool is_lvalue() override { return true; }

        // Execute the variable.
        virtual cvalue_t execute() override;

        // Converts to a string.
        X_TO_STRING
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Property variable.
    class property_variable_t : public __variable_base_t<variable_type_t::property>
    {
    public:

        // Constructor.
        property_variable_t(property_t * property) : property(property)
        {
            _A(property != nullptr);
        }

        // Relation property.
        property_t * property = nullptr;

        // Returns type of the property.
        virtual type_t * get_type() override
        {
            return to_type(property->type_name);
        }

        // Returns vtype.
        virtual vtype_t get_vtype() override;

        // Returns relation member.
        virtual member_t * get_relation_member() override { return property; }

        // Returns name of the property.
        virtual name_t get_name() const override
        {
            return property->name;
        }

        // Returns whether it's a left value.
        virtual bool is_lvalue() override { return false; }

        // Returns whether this will call a method.
        virtual bool is_calling() _NE override { return true; }

        // Converts to a string.
        X_TO_STRING
    };

    //-------- ---------- ---------- ---------- ----------

    // Property index variable.
    class property_index_variable_t
        : public __index_variable_base_t<variable_type_t::property_index>
    {
        typedef __index_variable_base_t<variable_type_t::property_index> __super_t;

    public:

        // Constructor.
        property_index_variable_t(property_t * property, arguments_t * arguments)
            : __super_t(arguments), property(property)
        {
            _A(property != nullptr);
        }

        // Relation property.
        property_t * property = nullptr;

        // Returns type of the property index.
        virtual type_t * get_type() override
        {
            return to_type(property->type_name);
        }

        // Returns vtype.
        virtual vtype_t get_vtype() override;

        // Returns relation member.
        virtual member_t * get_relation_member() override { return property; }

        // Returns name of the property.
        virtual name_t get_name() const override
        {
            return property->get_name();
        }

        // Returns whether it's a left value.
        virtual bool is_lvalue() override { return false; }

        // Converts to a string.
        X_TO_STRING
    };

    //-------- ---------- ---------- ---------- ----------

    // Array index varible.
    class array_index_variable_t
        : public __index_variable_base_t<variable_type_t::array_index>
    {
        typedef __index_variable_base_t<variable_type_t::array_index> __super_t;

    public:

        // Constructor.
        array_index_variable_t(expression_t * body, type_t * element_type, arguments_t * arguments);

        // Main body of the array.
        expression_t * body = nullptr;

        // Element type of the variable.
        type_t * element_type = nullptr;

        // Returns type of the element in the array.
        virtual type_t * get_type() override;

        // Returns vtype.
        virtual vtype_t get_vtype() override;

        // Returns name of the array index.
        virtual name_t get_name() const override;

        // Returns whether it's a left value.
        virtual bool is_lvalue() override { return true; }

        // Converts to string.
        X_TO_STRING
    };

    //-------- ---------- ---------- ---------- ----------

    // Event variable.
    class event_variable_t : public __variable_base_t<variable_type_t::event>
    {
    public:

        // Constructor.
        event_variable_t(event_t * event) : event(event)
        {
            _A(event != nullptr);
        }

        // Relation event.
        event_t * event = nullptr;

        // Returns type of the event.
        virtual type_t * get_type() override
        {
            return to_type(event->type_name);
        }

        // Returns vtype.
        virtual vtype_t get_vtype() override;

        // Returns relation member.
        virtual member_t * get_relation_member() override { return event; }

        // Returns name of the event.
        virtual name_t get_name() const override
        {
            return event->name;
        }

        // Returns whether it's a left value.
        virtual bool is_lvalue() override { return false; }

        // Converts to string.
        X_TO_STRING
    };

    //-------- ---------- ---------- ---------- ----------

    // Method variable.
    class method_variable_t : public __variable_base_t<variable_type_t::method, true>
    {
    public:

        // Constructor.
        method_variable_t(method_t * method);

        // Relation method.
        method_t * method = nullptr;

        // Returns type of the method.
        virtual type_t * get_type() override;

        // Returns vtype.
        virtual vtype_t get_vtype() override;

        // Returns relation member.
        virtual member_t * get_relation_member() override { return method; }

        // Returns name of the method.
        virtual name_t get_name() const override;

        // Returns whether it's a left value.
        virtual bool is_lvalue() override { return false; }

        // Converters to string.
        X_TO_STRING

    private:
        type_t * __type = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////

    // Ast error codes.
    X_ENUM(ast_error_t)

        // Variable duplicated.
        variable_duplicated,

        // Unexpected variable defination.
        variable_defination_unexpected,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////
    // variable_region_t

    // Variable region.
    class variable_region_t : public object_t
    {
        typedef local_variable_chain_node_t  __local_variable_t;

    public:

        // Constructor.
        variable_region_t(variable_region_t * previous,
                variable_type_t mask = (variable_type_t)0xFF, object_t * owner = nullptr);

        // Constructor.
        variable_region_t(variable_region_t * previous, variable_type_t mask, method_t * owner);

        // Previous variable region.
        variable_region_t * previous = nullptr;

        // Defines a local variable.
        local_variable_t * define_local(type_name_t * type_name, const name_t & name,
                bool constant = false, expression_t * expression = nullptr);

        // Defines a param.
        param_variable_t * define_param(param_t * param);

        // Defines a field.
        field_variable_t * define_field(field_t * field);

        // Defines a property.
        property_variable_t * define_property(property_t * property);

        // Defines an event.
        event_variable_t * define_event(event_t * event);

        // Defines a method.
        method_variable_t * define_method(method_t * method);

        // Gets the variable of the name.
        variable_t * get(const name_t & name);

        // Returns current local index, usually used for temp name of new local variables.
        int current_local_index() { return __current_local_index(); }

        X_TO_STRING_IMPL(_T("variable_region_t"))

    private:
        std::multimap<name_t, variable_t *> __variables;    // Variable map.
        variable_type_t     __mask;                         // Variable mask.

        void * __last_local_variable; // The last local variable node.

        static const msize_t __empty_index = unknown_msize;
        msize_t __local_index = __empty_index, __local_identity = 0;

        // Returns whether the varible type is supported.
        // Determined by __mask.
        bool __is_supported(variable_type_t type) const
        {
            return enum_has_flag(__mask, type);
        }

        // Returns whether the variable type is supported.
        bool __is_local_supported() const
        {
            return __is_supported(variable_type_t::local);
        }

        // Checks whether it has a variable with the same name in the region context.
        void __check_duplicate(variable_t * variable);

        // Returns next local identity.
        msize_t __next_local_identity();

        // Returns current local identity.
        msize_t & __current_local_identity();

        // Returns next local index.
        msize_t __next_local_index();

        // Returns current local index.
        msize_t & __current_local_index();

        // Creates a new variable.
        template<typename _variable_t, typename ... args_t>
        _variable_t * __new_variable(args_t && ... args);

        // Defines a new variable.
        template<typename _variable_t, typename ... args_t>
        _variable_t * __define(args_t && ... args);
    };

    ////////// ////////// ////////// ////////// //////////
    // ast_walk_context_layer_t

    // Ast walk context layer states.
    X_ENUM(ast_walk_context_layer_state_t)

        #define __Ls(name)  name = (int)efamily_t::name

        // Assembly
        __Ls(assembly),

        // Module
        __Ls(module),

        // Document
        __Ls(document),

        // Namespace
        __Ls(namespace_),

        // Type
        __Ls(type),

        // Method
        __Ls(method),

        // Property
        __Ls(property),

        // Event
        __Ls(event),

        // Field
        __Ls(field),

        // Typedef
        __Ls(type_def),

        // Region
        region,

        #undef __Ls

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    class ast_walk_context_t;

    // Ast walk context layer.
    X_INTERFACE ast_walk_context_layer_t
    {
        typedef ast_walk_context_t __context_t;
        typedef ast_walk_context_layer_t __layer_t, __self_t;
        typedef ast_walk_context_layer_state_t __layer_state_t;

        // Constructors.
        ast_walk_context_layer_t(__context_t & context, __self_t * previous,
                                                        object_t * owner = nullptr)
            : context(context), previous(previous)
        { }

        __context_t & context;
        __layer_t * previous = nullptr;     // Previous layer.

        // Gets the eobject of specified layer state.
        virtual void * get(__layer_state_t state) = 0;
    };

    //-------- ---------- ---------- ---------- ----------
    // assembly_loader_t

    // Interface for assembly loader.
    X_INTERFACE assembly_loader_t
    {
        // Load assembly by package name and assembly name.
        virtual assembly_t * load_assembly(
            const mname_t * package_name, const mname_t * assembly_name) = 0;
    };

    //-------- ---------- ---------- ---------- ----------
    // ast_walk_queue_t

    // Ast walk queue.
    template<typename element_t, size_t queue_size>
    class ast_walk_queue_t : public object_t
    {
        typedef object_t __super_t;
        static const size_t __queue_size = queue_size;

    public:

        // Constructor
        ast_walk_queue_t()
        {
            for (size_t index = 0; index < __queue_size; index++)
            {
                __qitems[index] = memory_t::new_obj<__queue_item_t>(&__qitems_pool);
            }
        }

        // Enque item with specified priory.
        void enque(element_t item, int priory)
        {
            _A(priory < __queue_size);

            __qitems[priory]->enque(item);
        }

        // Deque an item.
        bool deque(element_t * out_element)
        {
            for (; __qitem_index < __queue_size; __qitem_index++)
            {
                if (__qitems[__qitem_index]->deque(out_element))
                    return true;
            }

            return false;
        }

        X_TO_STRING_IMPL(_T("ast_walk_queue_t"))

    private:

        // Queue item.
        class __queue_item_t : public object_t
        {
        public:

            // Enque an element.
            void enque(element_t item)
            {
                __queue.push(item);
            }

            // Deque an element.
            bool deque(element_t * out_element)
            {
                if (__queue.empty())
                    return false;

                *out_element = __queue.front();
                __queue.pop();

                return true;
            }

            X_TO_STRING_IMPL(_T("__queue_item_t"))

        private:
            std::queue<element_t> __queue;
        };

        __queue_item_t * __qitems[__queue_size];
        pool_t __qitems_pool;
        size_t __qitem_index = 0;
    };

    //-------- ---------- ---------- ---------- ----------
    // ast_walk_context_t

    class ast_node_t;

    // Ast walk context.
    class ast_walk_context_t : public object_t, public no_copy_ctor_t
    {
        typedef ast_walk_context_layer_t        __layer_t;
        typedef ast_walk_context_layer_state_t  __layer_state_t;

        // Walk nodes.
        template<typename _module_ast_node_t>
        friend void ast_walk(ast_walk_context_t & walk_context,
                                const std::vector<ast_node_t *> & nodes);

    public:

        // Constructor.
        ast_walk_context_t(assembly_t & assembly,
            assembly_loader_t * assembly_loader, logger_t & logger, __layer_t * layer = nullptr
        );

        // Push a namespace.
        void push(namespace_t * namespace_);

        // Push a type.
        void push(type_t * type);

        // Push a method.
        void push(method_t * method);

        // Push a property.
        void push(property_t * property);

        // Push an event.
        void push(event_t * event);

        // Push a field.
        void push(field_t * field);

        // Push a typedef.
        void push(type_def_t * type_def);

        // Push a new region.
        void push_new_region();

        // Push a new document.
        document_t * push_new_document();

        // Push a new module.
        module_t * push_new_module();

        // Pop the top element.
        void pop();

        assembly_t & assembly;
        logger_t &   logger;

        // Creates a new object.
        template<typename t, typename ... args_t> t * new_obj(args_t && ... args)
        {
            return __xheap.new_obj<t>(std::forward<args_t>(args) ...);
        }

        // Returns current module.
        module_t * current_module()
        {
            return (module_t *)__current_layer->get(__layer_state_t::module);
        }

        // Returns current document.
        document_t * current_document()
        {
            return (document_t *)__current_layer->get(__layer_state_t::document);
        }

        // Returns current namespace.
        namespace_t * current_namespace()
        {
            return (namespace_t *)__current_layer->get(__layer_state_t::namespace_);
        }

        // Returns current type.
        type_t * current_type()
        {
            return (type_t *)__current_layer->get(__layer_state_t::type);
        }

        // Returns current method.
        method_t * current_method()
        {
            return (method_t *)__current_layer->get(__layer_state_t::method);
        }

        // Returns current property.
        property_t * current_property()
        {
            return (property_t *)__current_layer->get(__layer_state_t::property);
        }

        // Returns current event.
        event_t * current_event()
        {
            return (event_t *)__current_layer->get(__layer_state_t::event);
        }

        // Returns current field.
        field_t * current_field()
        {
            return (field_t *)__current_layer->get(__layer_state_t::field);
        }

        // Returns current typedef.
        type_def_t * current_type_def()
        {
            return (type_def_t *)__current_layer->get(__layer_state_t::type_def);
        }

        // Returns current region.
        variable_region_t * current_region()
        {
            return (variable_region_t *)__current_layer->get(__layer_state_t::region);
        }

        // Returns current layer.
        __layer_t * current_layer() const { return __current_layer; }

        // Delay walk specified node.
        // Push the node in the queue.
        void delay(ast_node_t * node, int step = 0, void * tag = nullptr)
        {
            __walk_items.enque(__walk_item_t { __current_layer, node, step, tag }, step);
        }

        // Returns the parent mname.
        const mname_t * parent_of(const mname_t * mname);

        // Returns the child of mname.
        const mname_t * child_of(const mname_t * mname);

        // Combines two mnames.
        const mname_t * combine(const mname_t * name1, const mname_t * name2);

        // Combines two mnames.
        const mname_t * combine(const mname_t * name1, const name_t & name2);

        // Load assembly by package name and assembly name.
        assembly_t * load_assembly(const mname_t * package_name, const mname_t * assembly_name);

        // Commit types.
        void commit_types();

        X_TO_STRING_IMPL(_T("ast_walk_context_t"))

    private:

        // Walk item
        struct __walk_item_t
        {
            __layer_t  *    layer;      // Current layer.
            ast_node_t *    node;       // Node
            int             step;       // The step identity.
            void *          tag;        // Argument.
        };

        xheap_t __xheap;                                    // Memory management.
        __layer_t * __current_layer = nullptr;              // Current layer.
        assembly_loader_t * __assembly_loader;              // Assembly loader.
        ast_walk_queue_t<__walk_item_t, 32> __walk_items;   // Items to walk.

        std::map<const mname_t *, const mname_t *> __parent_mnames; // Parent names cache
        std::map<const mname_t *, const mname_t *> __child_mnames;  // Child names cache.

        // Combine names map.
        typedef std::tuple<const mname_t *, const mname_t *> __combine_name_key_t;
        std::map<__combine_name_key_t, const mname_t *> __combine_names;

        // Combine names map.
        typedef std::tuple<const mname_t *, name_t> __combine_name_key2_t;
        std::map<__combine_name_key2_t, const mname_t *> __combine_names2;

        // Assembly cache.
        typedef std::tuple<const mname_t *, const mname_t *> __assembly_cache_key_t;
        std::map<__assembly_cache_key_t, assembly_t *> __assembly_cache;

        // Push a new layer.
        template<typename layer_t, typename ... args_t>
        layer_t * __push_new_layer(args_t && ... args)
        {
            __current_layer = new_obj<layer_t>(*this,
                __current_layer, std::forward<args_t>(args) ...
            );

            return (layer_t *)__current_layer;
        }

        // Begin walk.
        void __walk();
    };

    class module_ast_node_t;

    // Walk ast nodes.
    template<typename _module_ast_node_t = module_ast_node_t>
    void ast_walk(ast_walk_context_t & walk_context, const std::vector<ast_node_t *> & nodes)
    {
        _module_ast_node_t * module_node = __XPool.new_obj<_module_ast_node_t>();

        for (ast_node_t * node : nodes)
        {
            module_node->append(node);
        }

        walk_context.delay(module_node);
        walk_context.__walk();
    }

    // Walk ast node from root.
    template<typename _module_ast_node_t = module_ast_node_t>
    void ast_walk(ast_walk_context_t & walk_context, ast_node_t * root)
    {
        _A(root != nullptr);

        ast_walk<_module_ast_node_t>(walk_context, std::vector<ast_node_t *>({ root }));
    }

    ////////// ////////// ////////// ////////// //////////
    // Class ast_node_t

    // Ast node.
    class ast_node_t : public eobject_t, public code_element_t
    {
    public:

        // Returns Ast value.
        virtual ast_value_t value() const = 0;

        // Returns child count.
        virtual size_t child_count() const = 0;

        // Returns ast node at specified index.
        virtual ast_node_t * child_at(size_t index) const = 0;

        // Sets ast node at specified index.
        virtual void set_child(size_t index, ast_node_t * node) = 0;

        // On walk the ast node.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag);

        // On commit it.
        virtual void on_commit() { }

        // Returns ast node at index.
        ast_node_t * operator[](size_t index) const
        {
            return child_at(index);
        }

        // Converts to a string.
        operator string_t() const
        {
            return this->to_string();
        }

        // Parent node.
        ast_node_t * parent = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////

    // Array ast node.
    template<size_t _child_count>
    class array_ast_node_t : public ast_node_t
    {
    public:
        using ast_node_t::ast_node_t;

        // Returns child count.
        virtual size_t child_count() const override { return _child_count; }

        // Returns ast node at specifed index.
        virtual ast_node_t * child_at(size_t index) const override
        {
            _A(index < _child_count);

            return __childs[index];
        }

        // Sets ast node at specified index.
        virtual void set_child(size_t index, ast_node_t * node) override
        {
            _A(index < _child_count);
            _A(node != nullptr);

            __childs[index] = node;
            node->parent = this;
        }

        // Converts to string.
        virtual const string_t to_string() const override
        {
            return _T("[array ast]");
        }

    private:
        ast_node_t * __childs[_child_count];
    };

    ////////// ////////// ////////// ////////// //////////

    // A simple ast node with no child.
    class single_ast_node_t : public array_ast_node_t<0>
    {
    public:
        using array_ast_node_t<0>::array_ast_node_t;

        // Converts to string.
        virtual const string_t to_string() const override
        {
            return _T("[single ast]");
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // Multipy ast node.
    class multipy_ast_node_t : public ast_node_t
    {
    public:
        using ast_node_t::ast_node_t;

        // Returns child count.
        virtual size_t child_count() const override;

        // Returns ast node at specified index.
        virtual ast_node_t * child_at(size_t index) const override;

        // Sets ast node at specified index.
        virtual void set_child(size_t index, ast_node_t * node) override;

        // Append a ast node.
        void append(ast_node_t * node);

        // Converts to string.
        virtual const string_t to_string() const override
        {
            return _T("[multipy ast]");
        }

    private:
        std::vector<ast_node_t *> __childs;
    };

    //-------- ---------- ---------- ---------- ----------

    // Multipy ast node with element value.
    template<element_value_t _value>
    class tmultipy_ast_node_t : public multipy_ast_node_t
    {
    public:
        using multipy_ast_node_t::multipy_ast_node_t;

        // Returns the element value.
        virtual element_value_t value() const { return _value; }
    };

    //-------- ---------- ---------- ---------- ----------

    // Applies all subnodes for callback function.
    void expand_ast_node(ast_node_t * node, std::function<void (ast_node_t *)> callback);

    ////////// ////////// ////////// ////////// //////////
    // document_ast_node_t

    // Document ast node.
    class document_ast_node_t : public multipy_ast_node_t
    {
        typedef multipy_ast_node_t __super_t;

    public:

        // Returns ast value.
        virtual ast_value_t value() const override final
        {
            return document_ast_value;
        }

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return _T("[document ast]");
        }

        // On commit it.
        virtual void on_commit() override;

        // On walk it.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // module_ast_node_t

    // Module ast node.
    class module_ast_node_t : public multipy_ast_node_t
    {
        typedef multipy_ast_node_t __super_t;

    public:

        // Returns ast value.
        virtual ast_value_t value() const override final
        {
            return module_ast_value;
        }

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return _T("[module ast]");
        }

        // On commit it.
        virtual void on_commit() override;

        // On walk it.
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;

        // Walk it.
        virtual void do_walk(ast_walk_context_t & context, int step, void * tag);

        // Returns module.
        module_t * get_module() { return __module; }

    protected:
        module_t * __module;
    };

    ////////// ////////// ////////// ////////// //////////
    // named_object_t

    // Interface for named object.
    X_INTERFACE named_object_t
    {
        // Constructor
        named_object_t(const mname_t * name) : name(name) { }

        // Name
        const mname_t * name;

        // Converts to string.
        const string_t to_string() const
        {
            if (name == nullptr)
                return empty_str;

            return (string_t)*name;
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // Interface for object with a tag.
    X_INTERFACE tag_object_t
    {
        // Constructors.
        tag_object_t() = default;
        tag_object_t(void * tag) : tag(tag) { }    

        // The tag.
        void * tag = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////
    // Class ast_sulution_t

    class ast_project_t;

    // Ast solution eobject.
    class ast_solution_t : public object_t, public named_object_t, public tag_object_t
    {
        typedef named_object_t __super_t;

    public:

        // Constructor.
        ast_solution_t(const mname_t * name) : named_object_t(name) { }

        // Adds a project.
        void add_project(ast_project_t * cproject);

        // Gets project of specified name.
        ast_project_t * get_project(const mname_t * name);

        // Returns begin iterator.
        auto begin() const { return __projects.begin(); }

        // Returns end iterator.
        auto end()   const { return __projects.end();   }

        // Converts to string.
        X_TO_STRING_IMPL_
        {
            return _T("ast_solution_t");
        }

    private:
        std::map<sid_t, ast_project_t *> __project_map;
        std::vector<ast_project_t *> __projects;
    };

    ////////// ////////// ////////// ////////// //////////
    // Class ast_project_t

    class ast_file_t;

    // Ast project object.
    class ast_project_t : public object_t, public named_object_t, public tag_object_t
    {
        typedef named_object_t __super_t;

    public:

        // Constructor.
        ast_project_t(const mname_t * name) : named_object_t(name) { }

        // Adds a file.
        void add_file(ast_file_t * cfile);

        // Gets a file of specified name.
        ast_file_t * get_file(const mname_t * name);

        // Returns begin iterator.
        auto begin() const { return __files.begin(); }

        // Returns end iterator.
        auto end()   const { return __files.end();   }

        // Its solution.
        ast_solution_t * solution = nullptr;

        // Assembly path.
        string_t         assembly_path;

        X_TO_STRING_IMPL(_T("ast_project_t"))

    private:
        std::map<sid_t, ast_file_t *>   __file_map;
        std::vector<ast_file_t *>       __files;
    };

    ////////// ////////// ////////// ////////// //////////

    // Ast file object.
    class ast_file_t : public object_t, public named_object_t, public tag_object_t
    {
        typedef named_object_t __super_t;

    public:

        // Constructor
        ast_file_t(const mname_t * name, ast_node_t * root)
            : named_object_t(name), root(root)
        {
            _A(root != nullptr);
        }

        // Root node.
        ast_node_t * root;

        // Its project.
        ast_project_t * project = nullptr;

        // Converts to a string.
        X_TO_STRING_IMPL_
        {
            return __super_t::to_string();
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // Expression types.
    X_ENUM(expression_type_t)

        // A simple expression.
        simple,

        // A operation expression.
        op,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Expression execution error code.
    X_ENUM(expression_execute_error_code_t)

        // Excepts a const expression.
        expect_const_expression,

        // Invalid binary operator.
        invalid_binary_operator,

        // Invalid unitary operator.
        invalid_unitary_operator,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Context for compile an expression.
    class expression_compile_context_t : public object_t
    {
    public:

        // Constructor.
        expression_compile_context_t(xilx_write_context_t & ctx);

        // Relation xilx write context.
        xilx_write_context_t & xilx_ctx;

        // Relation statement context.
        statement_compile_context_t & statement_ctx;

        // Variable region.
        variable_region_t * variable_region;

        // Defines a local variable.
        local_variable_t * define_temp_local(type_t * type);

        // Relation statement context.
        operator statement_compile_context_t & () const
        {
            return statement_ctx;
        }

        // Relation method compile context.
        operator method_compile_context_t & () const;

        X_TO_STRING_IMPL(_T("expression_compile_context_t"))

    private:
        byte_t __new_variable_region[sizeof(variable_region_t)] = { 0 };
    };

    //-------- ---------- ---------- ---------- ----------

    // Expression family.
    X_ENUM(expression_family_t)

        name, cvalue, unitary, binary, expressions, function, index,

        new_, new_array, default_value, type_of, type_name, type_cast,

        this_, base, name_unit,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Expression behaviour.
    // Be used for compiling a expression, some type os behaviour will be ignored.
    X_ENUM(expression_behaviour_t)

        default_ = __default__, assign, execute, new_,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Expression.
    class expression_t : public eobject_t
    {
        typedef eobject_t __super_t;

    public:
        typedef expression_t * itype_t;

        // Returns this family.
        virtual expression_family_t this_family() const = 0;

        // Returns expression count.
        virtual size_t expression_count() const = 0;

        // Returns expression at specified index.
        virtual expression_t * expression_at(size_t index) const = 0;

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            auto w = std::bind(al::_wrap_index<size_t>, std::placeholders::_1);
            return al::join_str(w(0), w(expression_count()), _T(", "),
                [this](size_t index) { return _str(expression_at(index)); }
            );
        }

        // Returns expression at specified index.
        expression_t * operator[](size_t index) const
        {
            return expression_at(index);
        }

        // Execute the expression.
        virtual cvalue_t execute() { return cvalue_t::nan; }

        // Compile the expression.
        virtual void compile(expression_compile_context_t & ctx, xil_pool_t & pool,
                                    xil_type_t dtype = xil_type_t::empty)
        {
            throw _unimplemented_error(this, _T("compile"));
        }

        // Returns whether it's do nothing.
        virtual bool is_empty() { return false; }

        // Returns vtype.
        virtual vtype_t get_vtype() const
        {
            throw _unimplemented_error(this, _T("get_vtype"));
        }

        // Returns the type of the expression.
        virtual type_t * get_type() const
        {
            throw _unimplemented_error(this, _T("get_type"));
        }

        // Returns the type_name of expression.
        virtual type_name_t * get_type_name() const
        {
            return __XPool.to_type_name(get_type());
        }

        // Returns the behaviour.
        virtual expression_behaviour_t get_behaviour() const
        {
            throw _unimplemented_error(this, _T("get_behaviour"));
        }

        // Parent expression.
        expression_t * parent = nullptr;

        // Whether it is walked.
        bool walked = false;
    };

    typedef eobject_ast_t<typename expression_t::itype_t> expression_ast_t;

    // Returns whether it is a top expression ( no parent. )
    X_INLINE bool is_top_expression(expression_t * exp)
    {
        return exp != nullptr && exp->parent == nullptr;
    }

    // Returns whether it is a constant expression.
    X_INLINE bool is_constant_expression(expression_t * exp)
    {
        return exp->execute() == cvalue_t::nan;
    }

    // Executes the expression.
    X_INLINE cvalue_t execute_expression(expression_t * exp)
    {
        if (exp == nullptr)
            return cvalue_t::nan;

        return exp->execute();
    }

    ////////// ////////// ////////// ////////// //////////

    // Expression with specified expression count.
    template<size_t _exp_count>
    class texpression_t : public expression_t
    {
        typedef expression_t __super_t;

    public:

        using __super_t::__super_t;

        // Constructor.
        template<typename ... exps_t> texpression_t(exps_t ... exps_)
        {
            static_assert(sizeof ... (exps_) == _exp_count, "expression count not match");

            al::assign(exps, exps_ ...);
            al::each([this](expression_t * exp) { __set_parent(exp); }, exps_ ...);
        }

        // Returns expression count.
        virtual size_t expression_count() const override final
        {
            return _exp_count;
        }

        // Returns the expression at specified index.
        virtual expression_t * expression_at(size_t index) const override final
        {
            return exps[index];
        }

        // Returns behaviour.
        virtual expression_behaviour_t get_behaviour() const override
        {
            return expression_behaviour_t::default_;
        }

        // Expression array.
        expression_t * exps[_exp_count];

    private:

        void __set_parent(expression_t * exp)
        {
            _A(exp != nullptr);
            exp->parent = this;
        }
    };

    //-------- ---------- ---------- ---------- ----------

    namespace
    {
        // Base class of simple expressions.
        class __simple_expression_base_t : public expression_t
        {
            typedef expression_t __super_t;

        public:

            // Returns expression count.
            virtual size_t expression_count() const override final { return 0; }

            // Returns expression at specified index.
            virtual expression_t * expression_at(size_t index) const override final
            {
                return nullptr;
            }

            // Returns behaviour.
            virtual expression_behaviour_t get_behaviour() const override
            {
                return expression_behaviour_t::default_;
            }
        };
    };

    //-------- ---------- ---------- ---------- ----------

    // Interface for varible expression.
    X_INTERFACE variable_expression_t
    {
        // Returns whether it is a variable expression.
        virtual bool is_variable_expression() const = 0;

        // Return the variable of the expression.
        virtual variable_t * get_variable() = 0;

        // Converts to string.
        virtual const string_t to_string() const = 0;
    };

    // Returns the variable of the expression.
    variable_t * to_variable(expression_t * exp);

    //-------- ---------- ---------- ---------- ----------

    // Name expression type.
    X_ENUM(name_expression_type_t)

        variable,               // A variable.
        
        type,                   // A type.
        
        type_def,               // A typedef.

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Name expression.
    class name_expression_t : public __simple_expression_base_t
                            , public variable_expression_t
    {
        typedef __simple_expression_base_t __super_t;

    public:

        // Constructors.
        name_expression_t() = default;
        name_expression_t(const name_t & name) : name(name) { }

        // Constructors.
        template<typename _var_t>   // variable_t, type_t, type_def_t
        name_expression_t(_var_t * var) : name(var->get_name())
        {
            this->set(var);
        }

        // Name
        name_t name = name_t::null;

        // Name expression type.
        name_expression_type_t expression_type = name_expression_type_t::__default__;

        union
        {
            variable_t * variable = nullptr;    // A variable
            type_t *     type;                  // A type
            type_def_t * type_def;              // A typedef
        };

        // Returns whether it's a variable expression.
        virtual bool is_variable_expression() const override
        {
            return expression_type == name_expression_type_t::variable;
        }

        // Returns the variable if it's a variable expression, otherwise returns nullptr.
        virtual variable_t * get_variable() override
        {
            return variable;
        }

        // Returns this family.
        virtual expression_family_t this_family() const override
        {
            return expression_family_t::name;
        }

        // Returns vtype.
        virtual vtype_t get_vtype() const override;

        // Returns type.
        virtual type_t * get_type() const override;

        // Sets variable.
        void set(variable_t * variable);

        // Sets type.
        void set(type_t * type);

        // Sets typedef.
        void set(type_def_t * type_def);

        // Executes the expression.
        virtual cvalue_t execute() override;

        // Returns behaviour.
        virtual expression_behaviour_t get_behaviour() const override;

        // Converts to a string.
        virtual const string_t to_string() const override;
    };

    //-------- ---------- ---------- ---------- ----------

    // Name unit expression.
    class name_unit_expression_t : public name_expression_t
    {
        typedef name_expression_t __super_t;

    public:

        // Constructors.
        using __super_t::__super_t;

        name_unit_expression_t(name_t name, generic_args_t * generic_args)
            : __super_t(name), generic_args(generic_args)
        { }

        generic_args_t * generic_args = nullptr;

        size_t generic_args_count() const
        {
            return generic_args == nullptr? 0 : generic_args->size();
        }

        // Returns this family.
        virtual expression_family_t this_family() const override
        {
            return expression_family_t::name_unit;
        }

        // Converts to a string.
        virtual const string_t to_string() const override;
    };

    // Returns whether it's a name_expression_t or name_unit_expression_t.
    bool is_name_expression(expression_t * exp);

    // Returns whether it's a type expression.
    bool is_type_expression(expression_t * exp);

    // Returns whetner it's a variable expression.
    bool is_field_variable_expression(expression_t * exp,
                        field_variable_t ** out_variable = nullptr);

    //-------- ---------- ---------- ---------- ----------


    // Typename expression.
    class type_name_expression_t : public __simple_expression_base_t
    {
        typedef __simple_expression_base_t __super_t;

    public:

        // Constructors.
        type_name_expression_t(type_name_t * type_name) : type_name(type_name) { }
        type_name_expression_t() { }

        // Type name.
        type_name_t * type_name = nullptr;

        // Returns this family.
        virtual expression_family_t this_family() const override
        {
            return expression_family_t::type_name;
        }

        // Returns vtype.
        virtual vtype_t get_vtype() const override;

        // Returns type.
        virtual type_t * get_type() const override;

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return _str(type_name);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Type cast expression, e.g. (int)value.
    class type_cast_expression_t : public texpression_t<1>
    {
        typedef texpression_t<1> __super_t;

    public:
        using __super_t::__super_t;

        // Constructor.
        type_cast_expression_t(type_name_t * type_name, expression_t * expression) _NE
                : __super_t(expression), type_name(type_name)
        { }

        // Type name.
        type_name_t  * type_name = nullptr;

        // Expression.
        expression_t * expression() const _NE { return exps[0]; }

        // Set expression.
        void set_expression(expression_t * expression);

        // Returns this family.
        virtual expression_family_t this_family() const _NE override
        {
            return expression_family_t::type_cast;
        }

        // Returns vtype.
        virtual vtype_t get_vtype() const override;

        // Returns type.
        virtual type_t * get_type() const override;

        // Executes the expression.
        virtual cvalue_t execute() override;

        // Converts to string.
        virtual const string_t to_string() const override
        {
            return _F(_T("(%1%)%2%"), type_name, expression());
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Constant value expression.
    class cvalue_expression_t : public __simple_expression_base_t
    {
        typedef __simple_expression_base_t __super_t;

    public:

        // Constructor.
        cvalue_expression_t(cvalue_t * value) : value(value)
        {
            _A(value != nullptr);
        }

        // constant value.
        cvalue_t * value = nullptr;

        // Returns this family.
        virtual expression_family_t this_family() const override
        {
            return expression_family_t::cvalue;
        }

        // Converts to string.
        virtual const string_t to_string() const override
        {
            return (string_t)*this;
        }

        // Converts to string.
        operator string_t() const
        {
            return value != nullptr? (string_t)*value : empty_str;
        }

        // Executes the expression.
        virtual cvalue_t execute() override
        {
            return value? *value : cvalue_t();
        }

        // Returns vtype.
        virtual vtype_t get_vtype() const override;

        // Returns type.
        virtual type_t * get_type() const override;
    };

    //-------- ---------- ---------- ---------- ----------

    // Operator expression base.
    class op_expression_base_t
    {
    public:

        // Returns operator property.
        virtual const operator_property_t * get_operator_property() = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    // Expression with operator.
    template<size_t _exp_count>
    class op_expression_t : public texpression_t<_exp_count>
                          , public op_expression_base_t
    {
        typedef texpression_t<_exp_count> __super_t;

    public:

        // Constructor.
        template<typename ... exps_t>
        op_expression_t(const operator_property_t * op_property, exps_t ... exps)
            : texpression_t<_exp_count>(exps ...), op_property(op_property)
        {
            _A(op_property != nullptr);
        }

        // Constructor.
        template<typename ... exps_t>
        op_expression_t(operator_t op, exps_t ... exps)
            : op_expression_t(get_system_operator_property(op), std::forward<exps_t>(exps) ...)
        { }

        // Operator property.
        const operator_property_t * const op_property;

        // Operator.
        operator_t op() const { return op_property->op; }

        // Overload method.
        method_t * overload_method = nullptr;

        // Returns type.
        virtual type_t * get_type() const override
        {
            return overload_method? overload_method->get_type() : nullptr;
        }

        // Returns vtype.
        virtual vtype_t get_vtype() const override
        {
            return overload_method? to_vtype(overload_method->get_type()) : vtype_t::__unknown__;
        }

        // Returns operator property.
        virtual const operator_property_t * get_operator_property() override
        {
            return op_property;
        }

        // Returns behaviour.
        virtual expression_behaviour_t get_behaviour() const override
        {
            if (op_property->is_assign)
                return expression_behaviour_t::assign;

            if (overload_method != nullptr)
                return expression_behaviour_t::execute;

            return expression_behaviour_t::default_;
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Unitary expression.
    class unitary_expression_t : public op_expression_t<1>
    {
        typedef op_expression_t<1> __super_t;

    public:
        using __super_t::__super_t;

        // Returns the expression.
        expression_t * exp() const { return expression_at(0); }

        // Returns the family.
        virtual expression_family_t this_family() const override
        {
            return expression_family_t::unitary;
        }

        // Returns vtype.
        virtual vtype_t get_vtype() const override;

        // Returns type.
        virtual type_t * get_type() const override;

        // Execute the expression.
        cvalue_t execute() override;

        // Converts to string.
        virtual const string_t to_string() const override;
    };

    //-------- ---------- ---------- ---------- ----------

    // Binary expression.
    class binary_expression_t : public op_expression_t<2>
    {
        typedef op_expression_t<2> __super_t;

    public:
        using __super_t::__super_t;

        // Returns the first expression.
        expression_t * exp1() const { return expression_at(0); }

        // Returns the second expression.
        expression_t * exp2() const { return expression_at(1); }

        // Returns this family.
        virtual expression_family_t this_family() const override
        {
            return expression_family_t::binary;
        }

        // Returns behaviour.
        virtual expression_behaviour_t get_behaviour() const override;

        // Returns vtype.
        virtual vtype_t get_vtype() const override;

        // Returns type.
        virtual type_t * get_type() const override;

        // Executes the expression.
        virtual cvalue_t execute() override;

        // Converts to string.
        virtual const string_t to_string() const override;
    };

    //-------- ---------- ---------- ---------- ----------

    // Multipy expression. e.g. ( a = 10, b = 20, a + b )
    class expressions_t : public expression_t
    {
        typedef expression_t __super_t;

    public:
        typedef expression_t * value_type;
        expressions_t() = default;

        // Constructor with range [begin, end]
        template<typename exp_itor_t>
        expressions_t(exp_itor_t begin, exp_itor_t end)
        {
            append(begin, end);
        }

        // Returns this family.
        virtual expression_family_t this_family() const override
        {
            return expression_family_t::expressions;
        }

        // Returns expression count.
        virtual size_t expression_count() const override
        {
            return __expressions.size();
        }

        // Returns expression at specified index.
        virtual expression_t * expression_at(size_t index) const override
        {
            _A(index < __expressions.size());
            return __expressions[index];
        }

        // Returns vtype.
        virtual vtype_t get_vtype() const override;

        // Returns type.
        virtual type_t * get_type() const override;

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            string_t s = al::join_str(__expressions.begin(), __expressions.end(), _T(", "),
                [this](expression_t * exp) { return _str(exp); }
            );

            return sprintf(_T("(%1%)"), s);
        }

        // Append a expression.
        void append(expression_t * expression)
        {
            _A(expression != nullptr);
            __expressions.push_back(expression);
            expression->parent = this;
        }

        // Push a expression back.
        void push_back(expression_t * expression)
        {
            append(expression);
        }

        // Append expressions.
        template<typename exp_itor_t>
        void append(exp_itor_t begin, exp_itor_t end)
        {
            for (exp_itor_t it = begin; it != end; it++)
            {
                append(*it);
            }
        }

        // Execute the expression.
        virtual cvalue_t execute() override
        {
            throw _EC(unimplemented);
        }

        // Gets the behaviour.
        virtual expression_behaviour_t get_behaviour() const override
        {
            return expression_behaviour_t::default_;
        }

        // Returns the begin iterator.
        auto begin() const { return __expressions.begin(); }

        // Returns the end iterator.
        auto end()   const { return __expressions.end();   }

    private:
        std::vector<expression_t *> __expressions;
    };

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(namex_type_t)

        name,

        expression,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    namespace
    {
        // The base class type of expression with argument.
        class __arguments_expression_base_t : public expression_t
        {
            typedef expression_t __super_t;

        public:
            __arguments_expression_base_t() = default;

            // Constructor.
            __arguments_expression_base_t(arguments_t * arguments)
            {
                set_arguments(arguments);
            }

            // Sets arguments.
            void set_arguments(arguments_t * arguments)
            {
                __arguments = arguments;

                if (arguments != nullptr)
                {
                    for (argument_t * arg : *arguments)
                    {
                        arg->expression->parent = this;
                    }
                }
            }

            // Returns arguments.
            arguments_t * arguments() const { return __arguments; }

            // Returns argument count.
            size_t argument_count() const
            {
                return __arguments? __arguments->size() : 0;
            }

            // Returns argument at specified index.
            argument_t * argument_at(size_t index) const
            {
                if (__arguments == nullptr || index >= __arguments->size())
                    return nullptr;

                return (*__arguments)[index];
            }

            // Returns expression count.
            virtual size_t expression_count() const override final
            {
                return argument_count();
            }

            // Returns expression at specified index.
            virtual expression_t * expression_at(size_t index) const override final
            {
                if (__arguments == nullptr)
                    return nullptr;

                return (*__arguments)[index]->expression;
            }

        private:
            arguments_t * __arguments = nullptr;
        };
    }

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(function_expression_type_t)

        method,

        variable,

    X_ENUM_END

    // Function expression.
    class function_expression_t : public __arguments_expression_base_t
    {
        typedef __arguments_expression_base_t   __super_t;
        typedef function_expression_type_t      __ftype_t;
        typedef arch_uint_t                     __uint_t;

    public:

        // Constructor.
        function_expression_t() = default;
        function_expression_t(method_t * method, arguments_t * arguments)
            : __super_t(arguments)
        {
            _A(method != nullptr);
        }

        expression_t    * namex         = nullptr;      // Name
        generic_args_t  * generic_args  = nullptr;      // Generic arguments.

        // Sets namex, namex maybe a name or an expression.
        void set_namex(expression_t * namex) _NE;

        // Sets method.
        void set_method(method_base_t * method) _NE;

        // Gets method.
        method_base_t * get_method() const _NE;

        // Sets variable.
        void set_variable(variable_t * variable) _NE;

        // Gets variable.
        variable_t * get_variable() const _NE;

        // Returns function expression relation object type.
        __ftype_t get_ftype() const _NE { return (__ftype_t)__relation_data; }

        // Returns generic argument count.
        size_t generic_args_count() const
        {
            return generic_args? generic_args->size() : 0;
        }

        // Returns vtype.
        virtual vtype_t get_vtype() const override;

        // Returns this family.
        virtual expression_family_t this_family() const override
        {
            return expression_family_t::function;
        }

        // Returns type.
        virtual type_t * get_type() const override;

        // Gets name.
        name_t get_name() const;

        // Returns behaviour.
        virtual expression_behaviour_t get_behaviour() const override
        {
            return expression_behaviour_t::execute;
        }

        // Converts to string.
        virtual const string_t to_string() const override;

    private:

        incorp_t<void, __ftype_t> __relation_data;
    };

    // Returns delegate return type, (type at index 0).
    type_t * get_delegate_return_type(type_t * type);

    // Returns delegate return type, (type at index 0).
    type_t * get_delegate_return_type(generic_type_t * type);

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Index expression.
    class index_expression_t : public __arguments_expression_base_t
                             , public variable_expression_t
    {
        typedef __arguments_expression_base_t __super_t;

    public:
        variable_t *   variable = nullptr;

        // Sets name.
        void set_namex(expression_t * exp);

        // Gets name.
        expression_t * namex() const;

        // Returns this family.
        virtual expression_family_t this_family() const override
        {
            return expression_family_t::index;
        }

        // Returns vtype.
        virtual vtype_t  get_vtype() const override;

        // Returns type.
        virtual type_t * get_type() const override;

        // Converts to string.
        virtual const string_t to_string() const override;

        // Returns behaviour of the expression.
        virtual expression_behaviour_t get_behaviour() const override
        {
            return expression_behaviour_t::execute;
        }

        // Returns whether it is a variable expression.
        virtual bool is_variable_expression() const override
        {
            return true;
        }

        // Returns variable expression.
        virtual variable_t * get_variable() override
        {
            return variable;
        }

    private:
        expression_t * __namex    = nullptr;
    };

    typedef eobject_ast_t<index_expression_t *> index_expression_ast_t;

    //-------- ---------- ---------- ---------- ----------

    // New expression.
    class new_expression_t : public __arguments_expression_base_t
    {
        typedef __simple_expression_base_t __super_t;

    public:
        type_name_t * type_name   = nullptr;        // Type name
        method_t    * constructor = nullptr;        // Constructor.

        virtual expression_family_t this_family() const override
        {
            return expression_family_t::new_;
        }

        // Returns vtype.
        virtual vtype_t get_vtype() const override;

        // Returns type.
        virtual type_t * get_type() const override;

        // Returns the type_name of expression.
        virtual type_name_t * get_type_name() const override
        {
            return type_name;
        }

        // Converts to string.
        virtual const string_t to_string() const override;

        // Returns behaviour.
        virtual expression_behaviour_t get_behaviour() const override
        {
            return expression_behaviour_t::new_;
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Array initialize element type.
    X_ENUM(array_initialize_element_type_t)

        // A initializer, contains lots elements.
        initializer,

        // Expression.
        expression,

    X_ENUM_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Array initialize checking error code.
    X_ENUM(array_initialize_check_error_code_t)

        // Mixture elements.
        mixture_elements,

        // Unaligned.
        unaligned,

        // Overflow.
        dimension_overflow,

    X_ENUM_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class array_initializer_t;

    // Array initialize element.
    struct array_initialize_element_t
    {
        typedef eobject_t __super_t;
        typedef array_initialize_element_t __self_t;
        typedef array_initialize_element_type_t __type_t;

    public:

        // Constructor.
        array_initialize_element_t(array_initializer_t * initializer)
            : type(__type_t::initializer), initializer(initializer)
        {
            _A(initializer != nullptr);
        }

        // Constructor.
        array_initialize_element_t(expression_t * expression)
            : type(__type_t::expression), expression(expression)
        {
            _A(expression != nullptr);
        }

        // Converts to a string.
        operator string_t() const;

        __type_t type;

        union
        {
            array_initializer_t *   initializer;
            expression_t *          expression;
        };
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Array initializer.
    class array_initializer_t : public eobject_t
    {
        typedef eobject_t __super_t;
        typedef array_initialize_element_type_t __type_t;

    public:
        __vector_t<array_initialize_element_t> elements;

        // Appends expression.
        void append(expression_t * expression);

        // Append initializer.
        void append(array_initializer_t * initializer);

        // Returns size.
        size_t size()  const { return elements.size(); }

        // Returns whether it is empty.
        bool   empty() const { return size() == 0;     }

        // Returns element at the specified index.
        array_initialize_element_t & operator [](size_t index)
        {
            return elements[index];
        }

        // Returns element at the specified index.
        const array_initialize_element_t & operator [](size_t index) const
        {
            return elements[index];
        }

        // The element enumerating callback function.
        typedef std::function<bool(expression_t * exp)> each_element_callback_t;

        // Enumerates all elements.
        bool each_element(each_element_callback_t f);

        // Sets parent for all element expressions.
        void set_parent(expression_t * parent);

        // Checks whether it is corrent.
        // Raise expression when failed.
        void check();

        // Converts to string.
        X_TO_STRING
    };

    typedef eobject_ast_t<array_initializer_t *> array_initializer_ast_t;

    //-------- ---------- ---------- ---------- ----------

    typedef eobjects_t<expression_t *> array_lengths_t;
    typedef eobject_ast_t<array_lengths_t *> array_lengths_ast_t;

    //-------- ---------- ---------- ---------- ----------

    // New array expression.
    class new_array_expression_t : public expression_t
    {
        typedef expression_t __super_t;

    public:
        new_array_expression_t() = default;

        // Constructor.
        new_array_expression_t(array_lengths_t * lengths)
        {
            set_lengths(lengths);
        }

        type_name_t * type_name  = nullptr;             // Array element type name.

        array_initializer_t * initializer()             // Value initializer.
        {
            return __initializer;
        }

        // Sets initializer
        void set_initializer(array_initializer_t * initializer);

        // Returns array type.
        type_t * to_array_type() const;

        // Returns element type.
        type_t * get_element_type() const;

        // Returns array length.
        array_lengths_t * lengths() const { return __lengths; }

        // Returns dimension.
        dimension_t dimension() const
        {
            return __lengths == nullptr? 0 : __lengths->size();
        }

        // Returns this family.
        virtual expression_family_t this_family() const override
        {
            return expression_family_t::new_array;
        }

        // Returns vtype.
        virtual vtype_t get_vtype() const override;

        // Returns type.
        virtual type_t * get_type() const override;

        // Converts to a string.
        virtual const string_t to_string() const override;

        // Returns behaviour.
        virtual expression_behaviour_t get_behaviour() const override
        {
            return expression_behaviour_t::default_;
        }

        // Sets array lengths
        void set_lengths(array_lengths_t * lengths);

        // Gets the length at specified dimension.
        expression_t * length_at(dimension_t dimension) const;

        // Sets length for specified dimension.
        void set_length(dimension_t dimension, expression_t * length);

        // Returns expression count.
        virtual size_t expression_count() const override final
        {
            return __lengths == nullptr? 0 : __lengths->size();
        }

        // Returns expression at specified index.
        virtual expression_t * expression_at(size_t index) const override final
        {
            return length_at(index);
        }

    private:
        array_lengths_t     * __lengths = nullptr;
        array_initializer_t * __initializer = nullptr;
    };

    //-------- ---------- ---------- ---------- ----------

    // Default value expression.
    class default_value_expression_t : public __simple_expression_base_t
    {
        typedef __simple_expression_base_t __super_t;
        
    public:

        // Constructor.
        default_value_expression_t() = default;
        default_value_expression_t(type_name_t * type_name)
            : type_name(type_name) { }

        // Type name
        type_name_t * type_name = nullptr;

        // Returns this family.
        virtual expression_family_t this_family() const override
        {
            return expression_family_t::default_value;
        }

        // Returns this vtype.
        virtual vtype_t get_vtype() const override;

        // Returns this type.
        virtual type_t * get_type() const override;

        // Converts to string.
        virtual const string_t to_string() const override;

        // Execute the expression.
        virtual cvalue_t execute() override;
    };

    //-------- ---------- ---------- ---------- ----------

    // Type of expression.
    class type_of_expression_t : public __simple_expression_base_t
    {
        typedef __simple_expression_base_t __super_t;

    public:
        type_name_t * type_name = nullptr;  // Type name

        // Returns this family.
        virtual expression_family_t this_family() const override
        {
            return expression_family_t::type_of;
        }

        // Returns this vtype.
        virtual vtype_t get_vtype() const override;

        // Returns this type.
        virtual type_t * get_type() const override;

        // Converts to string.
        virtual const string_t to_string() const override;

        // Executes the expression.
        virtual cvalue_t execute() override;
    };

    //-------- ---------- ---------- ---------- ----------

    // This expression, e.g. this
    class this_expression_t : public __simple_expression_base_t
    {
        typedef __simple_expression_base_t __super_t;

    public:

        // Constructor.
        virtual expression_family_t this_family() const override
        {
            return expression_family_t::this_;
        }

        // Returns vtype.
        virtual vtype_t get_vtype() const override;

        // Returns type.
        virtual type_t * get_type() const override;

        // Converts to a string.
        virtual const string_t to_string() const override;

        // Execute the expression.
        virtual cvalue_t execute() override;

        // Set type.
        void set_type(type_t * type) { __type = type; }

    private:
        type_t * __type = nullptr;
    };

    //-------- ---------- ---------- ---------- ----------

    // Base expression. e.g. base
    class base_expression_t : public __simple_expression_base_t
    {
        typedef __simple_expression_base_t __super_t;

    public:

        // Constructor.
        virtual expression_family_t this_family() const override
        {
            return expression_family_t::base;
        }

        // Returns vtype.
        virtual vtype_t get_vtype() const override;

        // Returns type.
        virtual type_t * get_type() const override;

        // Converts to string.
        virtual const string_t to_string() const override;

        // Execute the expression.
        virtual cvalue_t execute() override;

        // Sets type.
        void set_type(type_t * type) { __type = type; }

    private:
        type_t * __type = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////

    // Gets the type of the expression.
    type_t * get_expression_type(expression_t * expression);

    //-------- ---------- ---------- ---------- ----------

    // Creates a new expression.
    template<typename exp_t, typename ... args_t>
    X_INLINE exp_t * __new_exp(memory_t * memory, args_t ... args)
    {
        return memory_t::new_obj<exp_t>(memory, args ...);
    }

    //-------- ---------- ---------- ---------- ----------

    // Cretates a new expression.
    X_INLINE cvalue_expression_t * to_exp(memory_t * memory, cvalue_t * value)
    {
        return memory_t::new_obj<cvalue_expression_t>(memory, value);
    }

    //-------- ---------- ---------- ---------- ----------

    // Cretates a new expression.
    X_INLINE name_expression_t * to_exp(memory_t * memory, const name_t & name)
    {
        return __new_exp<name_expression_t>(memory, name);
    }

    //-------- ---------- ---------- ---------- ----------

    // Cretates a new expression.
    X_INLINE unitary_expression_t * to_exp(memory_t * memory,
                    const operator_property_t * property, expression_t * exp)
    {
        return __new_exp<unitary_expression_t>(memory, property, exp);
    }

    //-------- ---------- ---------- ---------- ----------

    // Cretates a new expression.
    X_INLINE binary_expression_t * to_exp(memory_t * memory,
        const operator_property_t * property, expression_t * exp1, expression_t * exp2)
    {
        return __new_exp<binary_expression_t>(memory, property, exp1, exp2);
    }

    //-------- ---------- ---------- ---------- ----------

    // Cretates a new operator expression.
    template<typename ... exps_t>
    X_INLINE op_expression_t<sizeof ... (exps_t)> * to_exp(memory_t * memory,
                    const operator_property_t * property, exps_t && ... exps)
    {
        return __new_exp<op_expression_t<sizeof ... (exps_t)>>(
            memory, property, std::forward<exps_t>(exps) ...
        );
    }

    //-------- ---------- ---------- ---------- ----------

    // Cretates a new operator expression.
    template<typename ... exps_t>
    X_INLINE texpression_t<sizeof ... (exps_t)> * to_exp(memory_t * memory, exps_t && ... exps)
    {
        return __new_exp<texpression_t<sizeof ... (exps_t)>>(
            memory, std::forward<exps_t>(exps) ...
        );
    }

    //-------- ---------- ---------- ---------- ----------

    // Walk sub expression by applies all sub expression for calling callback function.
    template<typename callback_t>
    X_INLINE bool each_expression(expression_t * expression, callback_t callback,
                                                             bool deep = false)
    {
        if (expression == nullptr)
            return true;

        for (size_t index = 0, count = expression->expression_count(); index < count; index++)
        {
            expression_t * exp = expression->expression_at(index);

            if (!callback(exp))
                return false;

            if (deep && !each_expression(exp, callback, true))
                return false;
        }

        return true;
    }

    ////////// ////////// ////////// ////////// //////////

    // Statement exit point type
    X_ENUM(statement_exit_point_type_t)

        none    = __default__,

        break_,         // When break.

        continue_,      // When continue.

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Xilx block manager.
    class xilx_block_manager_t : public object_t, public no_copy_ctor_t
    {
        typedef method_xil_block_type_t __block_type_t;

    public:

        // Appends block.
        void append_block(__block_type_t type, local_label_t begin, local_label_t end,
                  local_label_t entry_point, type_t * relation_type);

        // Commits it.
        void commit(statement_compile_context_t & ctx);

        // Writes to buffer.
        void write(statement_compile_context_t & ctx, xil_buffer_t & buffer);

        X_TO_STRING_IMPL(_T("xilx_block_manager_t"))

    private:

        // Xilx block.
        struct __xilx_block_t
        {
            __block_type_t  type;
            local_label_t   begin, end, entry_point;
            type_t *        relation_type;
        };

        // Blocks.
        al::svector_t<__xilx_block_t> __blocks;
    };

    //-------- ---------- ---------- ---------- ----------

    // Context for write xilx.
    class xilx_write_context_t : public object_t, public no_copy_ctor_t
    {
        typedef object_t __super_t;

    public:

        // Constructor.
        xilx_write_context_t(statement_compile_context_t & sc_context,
                 xilx_block_manager_t & block_manager, statement_region_t * region);

        // Statement compile context.
        statement_compile_context_t & sc_context;

        // Converts to statement context.
        operator statement_compile_context_t & () const { return sc_context; }

        // Converts to method context.
        operator method_compile_context_t & () const;

        // Standalone regions.
        std::queue<statement_region_t *> regions;

        // Current variable region.
        variable_region_t * variable_region = nullptr;

        // Xilx block manager.
        xilx_block_manager_t & block_manager;

        X_TO_STRING_IMPL(_T("xilx_write_context_t"))
    };

    //-------- ---------- ---------- ---------- ----------

    // Interface for xilx.
    X_INTERFACE xilx_t : object_t
    {
        // Write to a pool.
        virtual void write(xilx_write_context_t & ctx, xil_pool_t & pool) = 0;

        // Converts to a string.
        virtual const string_t to_string() const override { return _T("xilx"); }
    };

    //-------- ---------- ---------- ---------- ----------

    // Interface for statement point.
    X_INTERFACE statement_point_t
    {
        // Returns a local label.
        virtual local_label_t to_position() = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    // Statement region properties.
    X_ENUM(statement_region_property_t)

        // Default property.
        none                = 0,

        // A protected block, such as try block.
        protected_block     = 1 << 0,

        // A protected block, with finally.
        with_finally        = 1 << 1,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Statement region.
    class statement_region_t : public object_t
    {
        typedef statement_exit_point_type_t __point_type_t;

    public:

        // Constructor.
        statement_region_t();

        // Parent region.
        statement_region_t * parent = nullptr;

        // Current variable region.
        variable_region_t * variable_region = nullptr;

        // Sets point with point type.
        virtual void set_point(__point_type_t point_type, statement_point_t * point);

        // Gets point with point type.
        virtual statement_point_t * get_point(__point_type_t point_type);

        // Returns whether the specified point type can be blocked.
        virtual bool can_block(__point_type_t point_type) { return false; }

        // Returns statement region property.
        virtual statement_region_property_t get_property()
        {
            return statement_region_property_t::none;
        }

        // Enum all xilxes.
        template<typename f_t> void each(f_t f)
        {
            al::for_each(__xilxes, f);
        }

        // Append a xilx.
        void append_xilx(xilx_t * xilx)
        {
            _A(xilx != nullptr);
            __xilxes.push_back(xilx);
        }

        // Write xilxes to a pool.
        virtual void write(xilx_write_context_t & ctx, xil_pool_t & pool);

        X_TO_STRING_IMPL(_T("statement_region_t"))

    private:
        statement_point_t * __points[(size_t)__point_type_t::__end__];
        al::svector_t<xilx_t *, 16> __xilxes;
    };

    //-------- ---------- ---------- ---------- ----------

    // Region xilx.
    class region_xilx_t : public xilx_t
    {
    public:

        // Constructor.
        region_xilx_t(statement_region_t * region) : __region(region)
        {
            _A(region != nullptr);
        }

        // Write to a pool.
        virtual void write(xilx_write_context_t & ctx, xil_pool_t & pool) override;

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return _F(_T("region xilx: %1%"), _str(__region));
        }

    private:
        statement_region_t * __region;
    };

    //-------- ---------- ---------- ---------- ----------

    // Xil buffer.
    class xil_buffer_t : public memory_base_t, public object_t
    {
    public:

        // Constructor.
        xil_buffer_t(memory_t * memory = nullptr)
            : memory_base_t(memory) { }

        // Writes to a data structure.
        template<typename t> void write(const t & v)
        {
            write((const byte_t *)&v, sizeof(t));
        }

        // Writes bytes.
        void write(const byte_t * bytes, size_t size);

        // Reads bytes.
        size_t read(byte_t * buffer, size_t size);

        // Returns size.
        size_t size() const { return __p - __buffer; }

        // Returns bytes at begin.
        const byte_t * bytes() const { return __buffer; }

        // Destructor.
        virtual ~xil_buffer_t() override;

        X_TO_STRING_IMPL(_T("xil_buffer_t"))

    private:
        byte_t * __buffer = nullptr, * __p = nullptr, * __p_read = nullptr;
        size_t   __size   = 0;

        void __ensure_size(size_t size);
    };

    //-------- ---------- ---------- ---------- ----------

    typedef uint32_t xil_index_t;

    // Xil pool.
    class xil_pool_t : public object_t, public no_copy_ctor_t
    {
        typedef xil_pool_t  __self_t;
        typedef object_t    __super_t;

        // The wrapper.
        template<typename _xil_t> struct __xil_wrapper_t
        {
            template<typename ... args_t>
            __xil_wrapper_t(args_t && ... args) : xil(std::forward<args_t>(args) ...) { }

            xil_index_t index;
            _xil_t      xil;
        };

        typedef al::svector_t<xil_t *, 128> __xils_t;
        typedef __xil_wrapper_t<byte_t> __v_xw_t;
        typedef arch_int_t __i_t;

        // Index of xil.
        static xil_index_t & __index_of(void * xw)
        {
            return *(xil_index_t *)((byte_t *)xw + (__i_t)(&((__v_xw_t *)nullptr)->index));
        }

        // Index of xil.
        static xil_index_t & __index_of(xil_t * xil)
        {
            return __index_of((void *)((byte_t *)xil - (__i_t)(&((__v_xw_t *)nullptr)->xil)));
        }

        X_TO_STRING_IMPL(_T("xil_pool_t"))

    public:

        xil_pool_t() : __heap(_T("xil_pool")) { }

        // Append a xil.
        template<typename _xil_t, typename ... args_t>
        _xil_t * append(args_t && ... args)
        {
            typedef __xil_wrapper_t<_xil_t> xw_t;
            xw_t * x = memory_t::new_obj<xw_t>(&__heap, std::forward<args_t>(args) ...);

            _xil_t * xil = &x->xil;
            __xils.push_back(xil);

            return xil;
        }

        // Returns the begin iterator.
        auto begin()  const { return __xils.begin(); }

        // Returns the end iterator.
        auto end()    const { return __xils.end();   }

        // Returns whether the xil collection is empty.
        bool empty()  const { return __xils.empty(); }

        // Returns size of the xil collection.
        size_t size() const { return __xils.size();  }

        // Returns xil at specified index.
        xil_t * operator[](size_t index) { return __xils[index]; }

        // Commits it.
        void commit();

        // Returns index of a xil.
        xil_index_t index_of(xil_t * xil);

    private:
        xheap_t     __heap;
        __xils_t    __xils;
    };

    //-------- ---------- ---------- ---------- ----------

    // jmp xil manager.
    class jmp_manager_t : public object_t, public no_copy_ctor_t
    {
    public:

        // Constructor
        jmp_manager_t() { }

        // Appends jmp xil with label.
        void append_jmp(jmp_xil_t * xil, local_label_t label);

        // Appends jmp xil with name.
        void append_jmp(jmp_xil_t * xil, name_t name);

        // Sets jmp point with label.
        void set_point(local_label_t label, xil_t * xil);

        // Sets jmp point with name.
        void set_point(name_t name, xil_t * xil);

        // Returns xil of label, nullptr when not found
        xil_t * label_xil(local_label_t label);

        // Commits.
        void commit(statement_compile_context_t & ctx);

        X_TO_STRING_IMPL(_T("jmp_manager_t"))

    private:
        std::map<jmp_xil_t *, local_label_t> __local_jmp_map;
        std::map<jmp_xil_t *, name_t> __global_jmp_map;

        std::map<local_label_t, xil_t *> __local_point_map;
        std::map<name_t, xil_t *> __global_point_map;
    };

    //-------- ---------- ---------- ---------- ----------

    // Context for compile statements.
    class statement_compile_context_t : public object_t, public no_copy_ctor_t
    {
        typedef statement_exit_point_type_t __point_type_t;

    public:

        // Constructor.
        statement_compile_context_t(method_compile_context_t & mctx, method_t * method);

        method_compile_context_t & mctx;        // Method compile context.

        method_t *              method;                 // Method.
        jmp_manager_t           jmp_manager;            // Jmp manager.
        switch_manager_t        switch_manager;         // Switch-case manager.

        // Begins a region.
        template<typename _region_t, typename ... args_t>
        _region_t * begin_region(args_t && ... args)
        {
            _region_t * region = __new_region<_region_t>(std::forward<args_t>(args) ...);

            if (region->parent != nullptr)
                region->parent->append_xilx(new_obj<region_xilx_t>(region));

            return region;
        }

        // Converts to method compile context.
        operator method_compile_context_t & () const { return mctx; }

        // Completes a region.
        statement_region_t * end_region();

        // Returns assembly layout.
        __assembly_layout_t & assembly_layout() { return mctx.layout; }

        // Returns xil pool.
        xil_pool_t & xil_pool() { return mctx.xil_pool; }

        // Returns index of local label.
        xil_index_t index_of(local_label_t label);

        // Creates a new global object.
        template<typename t, typename ... args_t>
        t * new_global_obj(args_t && ... args)
        {
            return memory_t::new_obj<t>(__XPool.memory, std::forward<args_t>(args) ...);
        }

        // Creates a new object.
        template<typename t, typename ... args_t>
        t * new_obj(args_t && ... args)
        {
            return memory_t::new_obj<t>(&__xheap, std::forward<args_t>(args) ...);
        }

        // Returns current region.
        statement_region_t * current_region() { return __current_region; }

        // Appends xilx.
        template<typename _xilx_t, typename ... args_t>
        _xilx_t * append_xilx(args_t && ... args)
        {
            _xilx_t * xilx = new_obj<_xilx_t>(std::forward<args_t>(args) ...);
            current_region()->append_xilx(xilx);

            return xilx;
        }

        // Converts a string to name_t.
        name_t to_name(const string_t & name)
        {
            return name_t(__XPool.spool.to_sid(name));
        }

        // Returns assembly.
        assembly_t * get_assembly();

        // Returns next local label.
        local_label_t next_local_label()
        {
            return ++__current_local_label;
        }

        // Sets point with point_type.
        void set_point(__point_type_t point_type, statement_point_t * point);

        // Returns memory management.
        memory_t * get_memory() { return &__xheap; }

        X_TO_STRING_IMPL(_T("statement_compile_context_t"))

    private:
        xheap_t                 __xheap;                        // Memory managerment.
        statement_region_t *    __current_region = nullptr;     // Current region.
        assembly_t *            __assembly       = nullptr;     // Assembly.
        local_label_t           __current_local_label = 0;      // Current local label.

        // Creates a new region.
        template<typename t, typename ... args_t>
        t * __new_region(args_t && ... args)
        {
            t * region =  memory_t::new_obj<t>(&__xheap, std::forward<args_t>(args) ...);
            region->parent = __current_region;
            __current_region = region;

            return region;
        }
    };

    ////////// ////////// ////////// ////////// //////////


} } }  // namespace X_ROOT_NS::modules::core

#include <modules/core/__utils.h>
#include <modules/core/__assembly.h>
#include <modules/core/__assembly_layout.h>
#include <modules/core/__vtype_traits.h>

#endif // __CORE_H__
