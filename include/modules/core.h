#ifndef __CORE_H__
#define __CORE_H__

#include <common.h>
#include <algorithm.h>
#include <jc.h>
#include <arch.h>

namespace X_ROOT_NS { namespace modules { namespace core {

    namespace
    {
        using namespace al;
        namespace __ns = X_ROOT_NS::modules::core;

        template<typename t, size_t init_size = 2>
        using __vector_t = al::small_vector_t<t, init_size>;
    }

    typedef int16_t element_value_t;

    ////////// ////////// ////////// ////////// //////////

    struct version_t : compare_operators_t<version_t, int64_t>
    {
        constexpr version_t() : version_t(0, 0, 0, 0) { }
        constexpr version_t(int8_t major, int8_t minor, int16_t build, int16_t revision)
            : major(major), minor(minor), build(build), revision(revision), __reserved__(0)
        { }

        int8_t          major;
        int8_t          minor;
        int16_t         build;
        int16_t         revision;
        int16_t         __reserved__;

        operator int64_t() const { return *(int64_t *)this; }

        operator string_t() const
        {
            return _F(_T("%1%.%2%.%3%.%4%"), major, minor, build, revision);
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // instance_t

    class type_t;
    class mobject_t { };

    ////////// ////////// ////////// ////////// //////////

    typedef uint16_t msize_t;
    const msize_t unknown_msize = (msize_t)-1;

    //-------- ---------- ---------- ---------- ----------

    ////////// ////////// ////////// ////////// //////////

    template<typename owner_t, typename t, size_t init_size,
        typename _super_t = __vector_t<t, init_size>
    >
    class owner_collection_t : public _super_t
    {
        typedef _super_t __super_t;

    public:
        owner_collection_t(owner_t owner) : __owner(owner) { }

        void push_back(t obj)
        {
            if(obj != nullptr)
                obj->set_owner(__owner);

            __super_t::push_back(obj);
        }

    private:
        owner_t __owner;
    };

    ////////// ////////// ////////// ////////// //////////

    class ast_file_t;

    struct code_unit_t
    {
        code_unit_t() = default;
        code_unit_t(const code_unit_t &o) = default;

        code_unit_t(const char_t * s, int32_t length, ast_file_t * file = nullptr)
            : s(s), length(length), file(file)
        { }

        const char_t * s;
        int32_t length;
        ast_file_t * file;

        operator string_t() const
        {
            return string_t(s, length);
        }
    };

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const code_unit_t & unit)
    {
        return stream << string_t(unit.s, unit.length);
    }

    ////////// ////////// ////////// ////////// //////////

    class code_element_t
    {
    public:
        code_element_t() = default;
        code_element_t(code_unit_t * unit) : code_unit(unit) { }

        code_unit_t * code_unit = nullptr;
    };

    code_element_t * combine(memory_t * memory, code_element_t * from, code_element_t * to);

    ////////// ////////// ////////// ////////// //////////

    template<typename obj_t>
    class code_element_wrapper_t : public obj_t, public code_element_t
    {
        typedef obj_t __super_t;

    public:
        using __super_t::__super_t;
    };

    ////////// ////////// ////////// ////////// //////////
    // logger_t

    X_ENUM(log_level_t)

        info,

        warning,

        error,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    X_INTERFACE logger_t
    {
        virtual void log(code_element_t * element, log_level_t level,
                                const string_t & name, const string_t & message) = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    class xlogger_t
    {
    public:
        xlogger_t(logger_t & logger) : logger(logger) { }

        logger_t & logger;

        template<typename code_t>
        void log_error(code_element_t * element, code_t code, const string_t & message)
        {
            logger.log(element, log_level_t::error, _title(code), message);
        }

        template<typename code_t, typename ... args_t>
        void log_errorf(code_element_t * element, code_t code, args_t && ... args)
        {
            log_error(element, code, _F(_desc(code), std::forward<args_t>(args) ...));
        }

        template<typename code_t>
        void log_warning(code_element_t * element, code_t code, const string_t & message)
        {
            logger.log(element, log_level_t::warning, _title(code), message);
        }

        template<typename code_t, typename ... args_t>
        void log_warningf(code_element_t * element, code_t code, args_t && ... args)
        {
            log_warning(element, code, _F(_desc(code), std::forward<args_t>(args) ...));
        }

        template<typename code_t>
        void log_info(code_element_t * element, code_t code, const string_t & message)
        {
            logger.log(element, log_level_t::info, _title(code), message);
        }

        template<typename code_t, typename ... args_t>
        void log_infof(code_element_t * element, code_t code, args_t && ... args)
        {
            log_info(element, code, _F(_desc(code), std::forward<args_t>(args) ...));
        }

        template<typename code_t>
        void log(code_element_t * element, code_t code, const string_t & message)
        {
            __log(element, code, message);
        }

        template<typename code_t>
        void log(code_element_t * element, log_level_t level, code_t code, const string_t & message)
        {
            logger.log(element, level, _title(code), message);
        }

        template<typename code_t, typename ... args_t>
        void logf(code_element_t * element, code_t code, args_t && ... args)
        {
            __log(element, code, _F(_desc(code), std::forward<args_t>(args) ...));
        }

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

            if(__Between(__info__, __warning__))
                level = log_level_t::info;
            else if(__Between(__warning__, __error__))
                level = log_level_t::warning;
            else if(__Between(__error__, __the_end__))
                level = log_level_t::error;
            else
                throw _ECF(argument_error, _T("argument %1% out of range"), __Code(code));

            logger.log(element, level, _title(code), message);

            #undef __Code
            #undef __Between
        }
    };

    template<typename code_t, typename ... args_t>
    void cpl_logf(logger_t & logger, object_t * object, log_level_t level,
                                            code_t code, args_t && ... args)
    {
        xlogger_t(logger).logf(as<code_element_t *>(object),
            level, code, std::forward<args_t>(args) ...
        );
    }

    template<typename code_t, typename ... args_t>
    void cpl_log_errorf(logger_t & logger, object_t * object, code_t code, args_t && ... args)
    {
        cpl_logf(logger, object, log_level_t::error, code, std::forward<args_t>(args) ...);
    }

    template<typename code_t, typename ... args_t>
    void cpl_log_warningf(logger_t & logger, object_t * object, code_t code, args_t && ... args)
    {
        cpl_logf(logger, object, log_level_t::warning, code, std::forward<args_t>(args) ...);
    }

    template<typename code_t, typename ... args_t>
    void cpl_log_infof(logger_t & logger, object_t * object, code_t code, args_t && ... args)
    {
        cpl_logf(logger, object, log_level_t::info, code, std::forward<args_t>(args) ...);
    }

    template<typename code_t, typename ... args_t>
    void cpl_log(logger_t & logger, object_t * object, log_level_t level,
                                        code_t code, const string_t & message)
    {
        xlogger_t(logger).log(as<code_element_t *>(object), level, code, message);
    }

    template<typename code_t, typename ... args_t>
    void cpl_log_error(logger_t & logger, object_t * object, code_t code, const string_t & message)
    {
        xlogger_t(logger).log_error(as<code_element_t *>(object), code, message);
    }

    //-------- ---------- ---------- ---------- ----------

    class loggers_t : public logger_t
    {
    public:
        loggers_t(std::initializer_list<logger_t *> loggers) : __loggers(loggers) { }

        virtual void log(code_element_t * element, log_level_t level,
                                const string_t & name, const string_t & message)
        {
            for(logger_t * logger : __loggers)
            {
                logger->log(element, level, name, message);
            }
        }

    private:
        std::vector<logger_t *> __loggers;
    };

    ////////// ////////// ////////// ////////// //////////

    class xpool_t;

    class eobject_commit_context_t : public object_t
    {
    public:
        eobject_commit_context_t(xpool_t & xpool, logger_t & logger)
            : xpool(xpool), logger(logger)
        { }

        xpool_t &   xpool;
        logger_t &  logger;
    };

    //-------- ---------- ---------- ---------- ----------

    class eobject_t : public object_t
    {
    public:
        virtual void commit(eobject_commit_context_t & ctx) { }
    };

    //-------- ---------- ---------- ---------- ----------

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

    struct token_t : code_element_t
    {
        token_t(token_value_t value, const char_t * s, int32_t length,
            token_data_t * data = nullptr, ast_file_t * file = nullptr)
            : code_element_t(&code_unit), code_unit(s, length, file), value(value), data(data)
        { } 

        token_t() = default; 
        token_t(const token_t &o) = default;

        token_data_t * data;
        token_value_t value;

        operator string_t() const { return (string_t)code_unit; }

        code_unit_t code_unit;

        operator code_unit_t * () { return &code_unit; }
        operator const code_unit_t * () const { return &code_unit; }
    };

    ostream_t & operator << (ostream_t & stream, const token_t & w);

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(token_error_code_t)

        comment_end_missing,

        unknown_token,

        string_format_error,

        char_format_error,

        numeric_format_error,

        numeric_overflow,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(compile_error_code_t)

        duplicated_entry_point,

        entry_point_prototype_mistake__should_no_return_type,

        entry_point_prototype_mistake__should_not_generic,

        entry_point_prototype_mistake__should_no_params,

        entry_point_prototype_mistake__should_be_public_static,

        entry_point_prototype_mistake__decorate_not_allowed,

        extern_method_prototype_mistake__should_be_static,

        extern_method_prototype_mistake__should_be_extern,

        extern_method_prototype_mistake__now_allowed,

        type_not_found,

        label_not_found,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    struct token_error_t
    {
        token_error_t() = default;
        token_error_t(const token_error_t &) = default;

        token_error_t(token_error_code_t code, const char_t * message=nullptr)
            : code(code), message(message? message : _title(code)) { }

        token_error_code_t code;
        const char_t * message;
    };

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(cvalue_type_t)

        number,

        string,

        null,

        type,

        nan,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    struct token_data_t
    {
        token_data_t() : error(nullptr), value(nullptr),
            value_type(cvalue_type_t::__unknown__) { }

        token_data_t(const token_data_t &) = default;

        token_error_t  * error;
        const void     * value;
        cvalue_type_t    value_type;
        const token_value_t * possible_values = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////

    typedef uint16_t operator_priority_t;
    typedef uint8_t  operator_arity_t;
    typedef uint8_t  operator_adhere_t;

    //-------- ---------- ---------- ---------- ----------

    struct token_property_t
    {
        bool is_operator        : 1;
        bool is_expression_box  : 1;
        bool is_invisible       : 1;
    };

    //-------- ---------- ---------- ---------- ----------

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

    X_INLINE bool is_system_operator(operator_t op)
    {
        return _int(op) < _int(operator_t::__user_defined__);
    }

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(operator_direct_t)

        left_to_right = __default__,

        right_to_left,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    struct operator_property_t
    {
        operator_t              op;
        operator_t              parent_op   = operator_t::__default__;
        operator_direct_t       direct;
        operator_priority_t     priority;
        operator_arity_t        arity;
        operator_adhere_t       adhere;
        bool                    is_assign;

        operator string_t() const;
    };

    const operator_property_t * get_system_operator_property(operator_t op);

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(expression_box_side_t)

        left        = __default__,

        right,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    struct expression_box_property_t
    {
        expression_box_side_t   side;
        token_value_t           pair_to;
    };

    ////////// ////////// ////////// ////////// //////////

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

    X_ENUM(eobject_ast_type_t)

        eobject, eobjects,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    struct code_unit_t;

    X_INTERFACE eobject_ast_base_t
    {
        virtual eobject_ast_type_t get_type() = 0;

        virtual void each_eobjects(std::function<void (eobject_t *)> f) = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    template<typename _etype_t>
    X_INTERFACE teobject_ast_base_t : eobject_ast_base_t
    {
        typedef _etype_t etype_t;
    };

    //-------- ---------- ---------- ---------- ----------

    template<typename _etype_t>
    X_INTERFACE eobject_ast_t : teobject_ast_base_t<_etype_t>
    {
        virtual _etype_t to_eobject() = 0;
        virtual eobject_ast_type_t get_type() override final
        {
            return eobject_ast_type_t::eobject;
        }

        virtual void each_eobjects(std::function<void (eobject_t *)> f) override
        {
            f(to_eobject());
        }
    };

    //-------- ---------- ---------- ---------- ----------

    template<typename _etype_t> X_INTERFACE eobjects_ast_t;

    template<typename _etype_t>
    struct eobjects_ast_iterator_t
    {
        typedef eobjects_ast_t<_etype_t> container_type;
        typedef _etype_t value_type;
        typedef eobjects_ast_iterator_t<_etype_t> __self_t;

        eobjects_ast_iterator_t(const container_type & container, size_t index)
            : __container(container), __index(index)
        { }

        _etype_t operator *() const { return __container.eobject_at(__index); }

        __self_t & operator ++()
        {
            __index++;
            return *this;
        }

        __self_t operator ++(int)
        {
            __self_t r = *this;
            __index++;
            return r;
        }

        __self_t & operator --()
        {
            __index--;
            return *this;
        }

        __self_t operator --(int)
        {
            __self_t r = *this;
            __index--;
            return r;
        }

        bool operator == (const __self_t & o) const { return __index == o.__index; }
        bool operator != (const __self_t & o) const { return __index != o.__index; }

    private:
        const container_type & __container;
        size_t __index;
    };

    //-------- ---------- ---------- ---------- ----------

    template<typename _etype_t>
    X_INTERFACE eobjects_ast_t : teobject_ast_base_t<_etype_t>
    {
        typedef eobjects_ast_iterator_t<_etype_t> iterator;
        typedef _etype_t value_type;

        virtual _etype_t eobject_at(size_t index) const = 0;
        virtual size_t   eobject_count() const = 0;

        iterator begin() const { return iterator(*this, 0); }
        iterator end()   const { return iterator(*this, eobject_count()); }
        size_t   size()  const { return eobject_count(); }
        bool     empty() const { return size() == 0; }

        virtual eobject_ast_type_t get_type() override final
        {
            return eobject_ast_type_t::eobjects;
        }

        virtual void each_eobjects(std::function<void (eobject_t *)> f) override
        {
            for(size_t index = 0, count = eobject_count(); index < count; index++)
            {
                f(eobject_at(index));
            }
        }
    };

    //-------- ---------- ---------- ---------- ----------

    template<typename _etype_t>
    class eobjects_t : public eobject_t
    {
    public:
        typedef _etype_t etype_t;
        typedef etype_t  value_type;
        typedef eobjects_t<_etype_t> * itype_t;

        typedef __vector_t<etype_t> container_t;
        typedef typename container_t::iterator iterator;

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

        void push_back(etype_t eobject)
        {
            eobjects.push_back(eobject);
        }

        template<typename _eobjects_t>
        void push_front(_eobjects_t & objs)
        {
            eobjects.push_front(objs);
        }

        auto begin() const { return eobjects.begin(); }
        auto end()   const { return eobjects.end();   }

        bool   empty() const { return eobjects.empty(); }
        size_t size()  const { return eobjects.size();  }

        etype_t & operator[](size_t index)
        {
            return eobjects[index];
        }

        const etype_t & operator[](size_t index) const
        {
            return eobjects[index];
        }

        operator string_t() const
        {
            return al::join_str(begin(), end(), _T(", "), [](auto & it) {
                return it? _str(it) : empty_str;
            });
        }

        virtual const string_t to_string() const override { return (string_t)*this; }
    };

    template<typename _etype_t> size_t size_of(eobjects_t<_etype_t> * objs)
    {
        return objs == nullptr? 0 : objs->size();
    }

    template<typename _etype_t> size_t is_empty(eobjects_t<_etype_t> * objs)
    {
        return size_of(objs) == 0;
    }

    //-------- ---------- ---------- ---------- ----------

    template<typename _etype_t, typename _owner_t = eobject_t *>
    class owner_eobjects_t : public eobjects_t<_etype_t>
    {
        typedef eobjects_t<_etype_t> __super_t;

    public:
        using __super_t::__super_t;
        typedef _owner_t owner_t;

        void set_owner(_owner_t owner)
        {
            __owner = owner;

            for(_etype_t eobj : *this)
            {
                eobj->set_owner(owner);
            }
        }

        _owner_t get_owner() { return __owner; }

    private:
        _owner_t __owner = nullptr;
    };

    //-------- ---------- ---------- ---------- ----------

    class with_index_t
    {
    public:
        int index = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    struct __indexed_eobjects_operator_t
    {
        template<typename _eobjects_t, typename _eobject_t = typename _eobjects_t::etype_t>
        static void on_append(_eobjects_t & eobjects, _eobject_t obj)
        {
            if(obj != nullptr)
                obj->index = eobjects.size();
        }
    };

    //-------- ---------- ---------- ---------- ----------

    template<typename operator_t, typename ... operators_t>
    struct __eobjects_operators_t
    {
        template<typename _eobjects_t, typename _eobject_t = typename _eobjects_t::etype_t>
        static void on_append(_eobjects_t & eobjects, _eobject_t obj)
        {
            operator_t::on_append(eobjects, obj);
            __eobjects_operators_t<operators_t ...>::on_append(eobjects, obj);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    template<typename operator_t> struct __eobjects_operators_t<operator_t>
    {
        template<typename _eobjects_t, typename _eobject_t = typename _eobjects_t::etype_t>
        static void on_append(_eobjects_t & eobjects, _eobject_t obj)
        {
            operator_t::on_append(eobjects, obj);
        }
    };

    //-------- ---------- ---------- ---------- ----------

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

    template<typename eobject_t>
    using indexed_eobjects_t = xeobjects_t<eobject_t, __indexed_eobjects_operator_t>;

    ////////// ////////// ////////// ////////// //////////

    class ast_node_t;
    typedef eobjects_ast_t<ast_node_t *> asts_t;

    ////////// ////////// ////////// ////////// //////////

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

        static const name_t null;
    };

    ////////// ////////// ////////// ////////// //////////

    class mname_t;
    typedef kv_object_t<sid_t, const mname_t *> __mname_cache_t;

    struct mname_operate_context_t
    {
        mname_operate_context_t(al::spool_t & spool, memory_t * memory = nullptr,
                __mname_cache_t * cache = nullptr)
            : spool(spool), memory(memory), cache(cache)
        { }

        memory_t *        memory = nullptr;
        al::spool_t &     spool;
        __mname_cache_t * cache = nullptr;

        template<typename load_func_t>
        const mname_t * from_cache(const sid_t & sid, load_func_t load_func)
        {
            const mname_t * name;
            if(cache != nullptr && (name = cache->get(sid)) != nullptr)
                return name;

            name = load_func();

            if(cache != nullptr)
                cache->set(sid, name);

            return name;
        }

        mname_t * new_mname();
        mname_t * new_mname(const sid_t & sid);
        mname_t * new_mname(const mname_t & mname);
    };

    //-------- ---------- ---------- ---------- ----------

    class mname_t : public compare_operators_t<mname_t, sid_t>
    {
    public:
        typedef __vector_t<sid_t> __name_parts_t;

        mname_t() = default;
        mname_t(const sid_t & sid) : sid(sid), parts({ sid }) { }
        mname_t(const mname_t &) = default;

        sid_t sid = 0;
        __name_parts_t parts;

        void append_part(sid_t sid) { parts.push_back(sid); }
        void commit(al::spool_t & pool);

        operator sid_t () const { return sid; }
        operator string_t () const { return (string_t)sid; }

        auto begin() const { return parts.begin(); }
        auto end()   const { return parts.end();   }

        bool empty() const { return sid == sid_t::null; }
        size_t part_count() const { return parts.size(); }

        sid_t operator [](size_t index) const { return parts[index]; }

        typedef mname_operate_context_t __context_t;

        static const mname_t * parse(__context_t && ctx, const string_t & s);
        static const mname_t * parse(__context_t &  ctx, const string_t & s);

        const char_t * part_of(size_t index) const;
        const char_t * last_part() const;

        static const mname_t * combine(__context_t && ctx, const mname_t * name1,
                                                           const mname_t * name2);
        static const mname_t * combine(__context_t &  ctx, const mname_t * name1,
                                                           const mname_t * name2);

        static const mname_t * combine(__context_t && ctx, const mname_t * name1,
                                                           const name_t & name2);
        static const mname_t * combine(__context_t &  ctx, const mname_t * name1,
                                                           const name_t & name2);

        static const mname_t * get_parent(__context_t && ctx, const mname_t * name);
        static const mname_t * get_parent(__context_t &  ctx, const mname_t * name);

        static const mname_t * get_child(__context_t && ctx, const mname_t * name);
        static const mname_t * get_child(__context_t &  ctx, const mname_t * name);

        template<typename container_t>
        static const mname_t * join(__context_t && ctx, container_t & container)
        {
            return join(ctx, container);
        }

        template<typename container_t>
        static const mname_t * join(__context_t & ctx, container_t & container)
        {
            return join(ctx, container.begin(), container.end());
        }

        template<typename itor_t>
        static const mname_t * join(__context_t && ctx, itor_t begin, itor_t end)
        {
            return join(ctx, begin, end);
        }

        template<typename itor_t>
        static const mname_t * join(__context_t & ctx, itor_t begin, itor_t end)
        {
            if(begin == end)
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

        static const mname_t * to_mname(__context_t && ctx, const name_t & name);
        static const mname_t * to_mname(__context_t &  ctx, const name_t & name);

        static const mname_t * to_internal(__context_t && ctx, const mname_t * mname);
        static const mname_t * to_internal(__context_t &  ctx, const mname_t * mname);

        static const mname_t null;
    };

    class emname_t : public eobject_t
    {
    public:
        emname_t() = default;
        emname_t(const mname_t * mname) : mname(mname) { }

        typedef emname_t * itype_t;

        const mname_t * mname = nullptr;
    };

    typedef eobject_ast_t<typename emname_t::itype_t> mname_ast_t;

    X_INLINE const mname_t * to_mname(const emname_t * emname)
    {
        if(emname == nullptr)
            return nullptr;

        return emname->mname;
    }

    X_INLINE sid_t to_sid(const mname_t * mname)
    {
        if(mname == nullptr)
            return sid_t::null;

        return mname->sid;
    }

    ////////// ////////// ////////// ////////// //////////

    X_ENUM(access_value_t)

        public_, protected_, private_, internal,
        
        protected_or_internal, protected_and_internal,

    X_ENUM_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

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

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const decorate_value_t & d)
    {
        return stream << *(uint32_t *)&d;
    }

    template<typename stream_t>
    stream_t & operator >> (stream_t & stream, decorate_value_t & d)
    {
        return stream >> *(uint32_t *)&d;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class decorate_t : public decorate_value_t, public eobject_t
    {
        typedef decorate_value_t __super_t;

    public:
        decorate_t() = default;
        decorate_t(const decorate_value_t & value) : __super_t(value) { }

        typedef decorate_t * itype_t;

        static const decorate_t default_value;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<decorate_t *> decorate_ast_t;

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    struct with_decorate_object_t
    {
        decorate_t * decorate = nullptr;

        bool is_static() const      { return decorate && decorate->is_static; }
        bool is_sealed() const      { return decorate && decorate->is_sealed; }
        bool is_override() const    { return decorate && decorate->is_override; }
        bool is_virtual() const     { return decorate && decorate->is_virtual; }
        bool is_abstract() const    { return decorate && decorate->is_abstract; }
        bool is_new() const         { return decorate && decorate->is_new; }
        bool is_const() const       { return decorate && decorate->is_const; }
        bool is_readonly() const    { return decorate && decorate->is_readonly; }
        bool is_extern() const      { return decorate && decorate->is_extern; }
    };

    ////////// ////////// ////////// ////////// //////////

    class expression_t;

    class cvalue_t : public eobject_t
    {
    public:
        typedef cvalue_t * itype_t;

        cvalue_t(const char_t * string) : value_type(cvalue_type_t::string), string(string) { }
        cvalue_t(const tvalue_t & number) : value_type(cvalue_type_t::number), number(number) { }

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

        bool operator == (const cvalue_t & other) const;
        bool operator != (const cvalue_t & other) const;
        bool operator <  (const cvalue_t & other) const;
        bool operator >  (const cvalue_t & other) const;
        bool operator <= (const cvalue_t & other) const;
        bool operator >= (const cvalue_t & other) const;

        bool operator == (bool value) const;
        bool operator != (bool value) const;

        bool operator == (std::nullptr_t) const { return is_null();  }
        bool operator != (std::nullptr_t) const { return !is_null(); }

        static const cvalue_t nan;
        static const cvalue_t null;

        bool is_nan() const { return value_type == cvalue_type_t::nan; }
        bool is_null() const { return value_type == cvalue_type_t::null; }

        X_TO_STRING
    };

    X_INLINE cvalue_t to_cvalue(type_t * type)
    {
        return cvalue_t(cvalue_type_t::type, (object_t *)type);
    }

    X_INLINE bool is_nan(const cvalue_t & v)
    {
        return v.value_type == cvalue_type_t::nan;
    }

    X_INLINE bool is_number(const cvalue_t & v)
    {
        return v.value_type == cvalue_type_t::number;
    }

    X_INLINE bool is_string(const cvalue_t & v)
    {
        return v.value_type == cvalue_type_t::string;
    }

    X_INLINE bool is_null(const cvalue_t & v)
    {
        return v.value_type == cvalue_type_t::null;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<cvalue_t *>     cvalue_ast_t;
    typedef eobject_ast_t<expression_t *> expression_ast_t;
 
    ////////// ////////// ////////// ////////// //////////
    // xil

    X_INTERFACE xil_t
    {

    };

    //-------- ---------- ---------- ---------- ----------

    class statement_compile_context_t;

    X_INTERFACE statement_t : eobject_t
    {
        virtual void compile(statement_compile_context_t & ctx) = 0;
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
    typedef indexed_eobjects_t<argument_t *> arguments_t;

    class attribute_t : public eobject_t
    {
    public:
        typedef attribute_t * itype_t;

        type_name_t * type_name   = nullptr;
        arguments_t * arguments   = nullptr;
        method_t    * constructor = nullptr;

        virtual void set_owner(eobject_t * object);
        virtual void commit(eobject_commit_context_t & ctx) override;

        mobject_t * attribute_object = nullptr;
        eobject_t * bind_object = nullptr;

        X_TO_STRING
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<attribute_t::itype_t> attribute_ast_t;
    typedef owner_eobjects_t<attribute_t::itype_t> attributes_t;
    typedef eobject_ast_t<attributes_t *> attributes_ast_t;

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class with_attributes_object_t
    {
    public:
        attributes_t * attributes = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////

    X_ENUM(argument_name_type_t)

        param,

        field,

    X_ENUM_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    X_ENUM(param_type_t)

        ref,

        out,

        params,

    X_ENUM_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class argument_t : public with_attributes_object_t
                     , public with_index_t
                     , public eobject_t
                     , public mobject_t
    {
        typedef eobject_t __super_t;

    public:
        typedef argument_t * itype_t;

        name_t               name       = name_t::null;
        param_type_t         atype      = param_type_t::__default__;
        argument_name_type_t name_type  = argument_name_type_t::param;
        expression_t *       expression = nullptr;

        type_t * get_type(xpool_t & xpool) const;

        operator string_t() const;
        
        virtual const string_t to_string() const override { return (string_t)*this; }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<argument_t *> argument_ast_t;
    typedef eobject_ast_t<arguments_t *> arguments_ast_t;

    ////////// ////////// ////////// ////////// //////////

    class param_t : public with_attributes_object_t
                  , public with_index_t
                  , public eobject_t, public mobject_t
    {
    public:
        typedef param_t * itype_t;

        param_t() = default;
        param_t(type_name_t * type_name, name_t name,
                param_type_t ptype = param_type_t::__default__,
                expression_t * default_value = nullptr)
            : type_name(type_name), name(name), ptype(ptype), default_value(default_value)
        { }

        type_name_t  *  type_name       = nullptr;
        name_t          name            = name_t::null;
        param_type_t    ptype           = param_type_t::__default__;
        expression_t *  default_value   = nullptr;

        type_t * get_type() const;

        operator string_t() const;
        virtual const string_t to_string() const override { return (string_t)*this; }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<param_t *> param_ast_t;
    typedef indexed_eobjects_t<param_t *> params_t;
    typedef eobject_ast_t<params_t *> params_ast_t;

    ////////// ////////// ////////// ////////// //////////
    // type_t, member_type_t, vtype_t, ttype_t

    #define __TTypeItem(name) name = (int8_t)value_type_t::name

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

        string,

        void_,

        ptr,

        mobject,

    X_ENUM_END

    #undef __TTypeItem

    vtype_t to_vtype(value_type_t value_type);
    bool is_vtype_compatible(vtype_t from, vtype_t to);
    bool is_integer_vtype(vtype_t vtype);
    bool is_float_vtype(vtype_t vtype);
    bool is_numberic_type(vtype_t vtype);

    cvalue_t default_value_of(vtype_t vtype);

    constexpr msize_t get_vtype_size(vtype_t vtype)
    {
        switch(vtype)
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

            case vtype_t::ptr:
            case vtype_t::mobject:
            case vtype_t::string:
            default:
                return sizeof(void *);
        }
    }

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(ttype_t)

        class_,

        struct_,

        interface_,

        enum_,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(gtype_t)

        general, generic, generic_param, array, type_def_param, null,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(member_type_t)

        field       =   1 << 0,

        method      =   1 << 1,
    
        property    =   1 << 2,

        event       =   1 << 3,

        type        =   1 << 4, 

        type_def    =   1 << 5,

        all         =   -1,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------
    // member_t

    class member_t : public eobject_t
                   , public with_attributes_object_t, public with_decorate_object_t
                   , public mobject_t
    {
        typedef eobject_t __super_t;

    public:
        type_t * host_type = nullptr;

        virtual member_type_t this_type() = 0;
        virtual name_t get_name() const = 0;
        void set_owner(type_t * host_type) { this->host_type = host_type; }

        operator string_t() const { return _T("type member"); }
        virtual const string_t to_string() const override { return (string_t)*this; }
    };

    assembly_t * get_assembly(member_t * member);

    //-------- ---------- ---------- ---------- ----------
    // __named_base_t
    
    template<typename _super_t>
    class __named_base_t : public _super_t
    {
        typedef _super_t __super_t;

    public:
        using __super_t::__super_t;

        name_t name  = name_t::null;

        virtual name_t get_name() const override { return name; }
    };

    typedef __named_base_t<member_t> __named_member_t;

    //-------- ---------- ---------- ---------- ----------

    struct atype_t
    {
        atype_t() = default;
        atype_t(type_t * type, param_type_t atype = param_type_t::__default__)
            : type(type), atype(atype) { }

        type_t * type = nullptr;
        param_type_t atype = param_type_t::__default__;

        operator string_t() const;
    };

    typedef al::small_vector_t<atype_t, 8> atypes_t;
    typedef al::small_vector_t<type_t *, 8> arg_types_t;

    //-------- ---------- ---------- ---------- ----------
    // method_trait_t

    X_ENUM(method_trait_t)

        normal,

        constructor,

        static_constructor,

        destructor,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------
    // type_t

    const int __unknown_size = -1;

    class analyze_member_args_t
    {
    public:
        analyze_member_args_t(member_type_t member_type, name_t name,
                atypes_t * atypes = nullptr, generic_args_t * generic_args = nullptr)
            : member_type(member_type), name(name), atypes(atypes), generic_args(generic_args)
        { }

        member_type_t       member_type;
        name_t              name;
        atypes_t *          atypes;
        generic_args_t *    generic_args;
        method_trait_t      method_trait    = method_trait_t::__default__;
        arg_types_t         out_arg_types;

        bool                exact_match     = false;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef __vector_t<member_t *> members_t;

    class analyze_members_args_t : public analyze_member_args_t
    {
        typedef analyze_member_args_t __super_t;

    public:
        using __super_t::__super_t; 

        members_t out_members;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class namespace_t;

    class type_t : public member_t
    {
        typedef member_t __super_t;

    public:
        typedef type_t * itype_t;

        virtual gtype_t this_gtype() const = 0;
        virtual ttype_t this_ttype() const = 0;
        virtual vtype_t this_vtype() const = 0;

        virtual member_t * get_member(analyze_member_args_t & args) = 0;
        virtual void get_members(analyze_members_args_t & args) = 0;
        virtual void get_all_members(members_t & out_members) = 0;

        typedef std::function<void (type_t *)> each_super_type_callback_t;
        virtual void each_super_type(each_super_type_callback_t callback) = 0;

        virtual member_type_t this_type() override final
        {
            return member_type_t::type;
        }

        virtual msize_t get_vsize()   { return unknown_msize; }
        virtual msize_t get_rcount()  { return unknown_msize; }

        virtual bool has_super_type(type_t * super);
        virtual type_t * get_base_type() { return nullptr; }

        virtual const string_t to_string() const override { return to_full_name(); }
        virtual const string_t to_short_name() const { return (string_t)*this; }
        virtual const string_t to_full_name() const { return (string_t)*this; };

        operator string_t() const { return _T("type"); }

        virtual void commit(eobject_commit_context_t & ctx) override;

        static const efamily_t __family__ = efamily_t::type;

    private:
        std::set<type_t *> __super_types;
    };

    typedef __named_base_t<type_t> __named_type_t;

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    bool is_base_type(type_t * drived, type_t * base);
    bool is_super_type(type_t * drived, type_t * super);
    bool is_type_compatible(type_t * from, type_t * to);

    bool is_void_type(type_t * type);
    bool is_string_type(type_t * type);
    bool is_ptr_type(type_t * type);

    X_INLINE bool is_general(type_t * type)
    {
        return type != nullptr && type->this_gtype() == gtype_t::general;
    }

    X_INLINE bool is_generic(type_t * type)
    {
        return type != nullptr && type->this_gtype() == gtype_t::generic;
    }

    X_INLINE bool is_generic_param(type_t * type)
    {
        return type != nullptr && type->this_gtype() == gtype_t::generic_param;
    }

    X_INLINE bool is_array(type_t * type)
    {
        return type != nullptr && type->this_gtype() == gtype_t::array;
    }

    X_INLINE bool is_type_def_param(type_t * type)
    {
        return type != nullptr && type->this_gtype() == gtype_t::type_def_param;
    }

    X_INLINE bool is_struct(type_t * type)
    {
        return type != nullptr && type->this_ttype() == ttype_t::struct_;
    }

    msize_t get_type_size(type_t * type);

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<type_t *> type_ast_t;

    ////////// ////////// ////////// ////////// //////////

    class null_type_t : public __named_type_t
    {
        typedef type_t __super_t;

    public:
        virtual gtype_t this_gtype() const override final { return gtype_t::null; }
        virtual ttype_t this_ttype() const override final { return ttype_t::__unknown__; }
        virtual vtype_t this_vtype() const override final { return vtype_t::__unknown__; }

        virtual member_t * get_member(analyze_member_args_t & args) override;
        virtual void get_members(analyze_members_args_t & args) override;
        virtual void get_all_members(members_t & out_members) override;

        virtual void each_super_type(each_super_type_callback_t callback) override { }

        virtual const string_t to_full_name() const override { return (string_t)*this; }
        virtual const string_t to_short_name() const override { return (string_t)*this; }

        operator string_t() const { return _T("?"); }

        static null_type_t * instance();
    };

    ////////// ////////// ////////// ////////// //////////
    // generic_param_t

    class generic_param_t : public __named_type_t, public with_index_t
    {
        typedef type_t __super_t;

    public:
        typedef generic_param_t * itype_t;

        virtual gtype_t this_gtype() const override final { return gtype_t::generic_param; }
        virtual ttype_t this_ttype() const override final { return ttype_t::__unknown__; }
        virtual vtype_t this_vtype() const override final { return vtype_t::__unknown__; }

        virtual member_t * get_member(analyze_member_args_t & args) override;
        virtual void get_members(analyze_members_args_t & args) override;
        virtual void get_all_members(members_t & out_members) override;

        virtual void each_super_type(each_super_type_callback_t callback) override { }

        virtual const string_t to_full_name() const override { return (string_t)*this; }
        virtual const string_t to_short_name() const override { return (string_t)*this; }

        operator string_t() const { return _T("?") + _str(name); }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<generic_param_t *> generic_param_ast_t;

    typedef indexed_eobjects_t<generic_param_t *> generic_params_t;
    typedef eobject_ast_t<generic_params_t *>     generic_params_ast_t;

    ////////// ////////// ////////// ////////// //////////

    class generic_arg_t : public with_index_t, public eobject_t
    {
        typedef eobject_t __super_t;

    public:
        typedef generic_arg_t * itype_t;

        type_name_t * type_name = nullptr;

        type_t * get_type() const { return type_name? type_name->type : nullptr; }

        operator string_t() const { return _str(type_name); }
        virtual const string_t to_string() const override { return (string_t)*this; }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<generic_arg_t *> generic_arg_ast_t;
    typedef eobject_ast_t<generic_args_t *> generic_args_ast_t;

    ////////// ////////// ////////// ////////// //////////
    // generic_constraint_t

    X_ENUM(generic_constraint_ttype_t)

        class_, struct_,

    X_ENUM_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class generic_constraint_t : public eobject_t
    {
        typedef eobject_t __super_t;

    public:
        typedef generic_constraint_t * itype_t;

        name_t param_name = name_t::null;
        generic_constraint_ttype_t cttype = generic_constraint_ttype_t::__default__;

        __vector_t<type_name_t *> type_names;

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

    class import_t : public eobject_t
    {
        typedef eobject_t __super_t;

    public:
        typedef import_t * itype_t;

        import_t() = default;
        import_t(emname_t * package, const name_t & alias)
            : package(package), alias(alias) { }

        emname_t * package  = nullptr;
        emname_t * assembly = nullptr;
        name_t     alias    = name_t::null;

        operator string_t() const;
        virtual const string_t to_string() const override { return (string_t)*this; }
    };

    typedef eobject_ast_t<import_t *> import_ast_t;

    ////////// ////////// ////////// ////////// //////////

    class using_namespace_t : public eobject_t
    {
        typedef eobject_t __super_t;

    public:
        typedef using_namespace_t * itype_t;

        using_namespace_t() = default;
        using_namespace_t(emname_t * ns, const name_t & alias)
            : ns(ns), alias(alias) { }

        emname_t * ns    = nullptr;
        name_t     alias = name_t::null;

        operator string_t() const;
        virtual const string_t to_string() const override { return (string_t)*this; }
    };

    typedef eobject_ast_t<using_namespace_t *> using_namespace_ast_t;

} } }

#include <modules/core/__xil.h>

namespace X_ROOT_NS { namespace modules { namespace core {

    class method_body_t : public eobject_t
    {
        typedef eobject_t __super_t;

    public:
        typedef method_body_t * itype_t;

        statements_t statements;

        template<typename _statements_t>
        void push_front(_statements_t & statements)
        {
            this->statements.push_front(statements);
        }
    };

    typedef eobject_ast_t<method_body_t *> method_body_ast_t;

    ////////// ////////// ////////// ////////// //////////

    X_ENUM(method_inline_type_t)

        default_ = __default__,

        always,

        never,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    class local_variable_t;
    class xil_buffer_t;
    class xil_pool_t;

    struct default_mt_template_t;
    typedef uint16_t __assembly_layout_version_t;

    template<
        typename spool_t, __assembly_layout_version_t lv, typename mt_template_t
    > class assembly_layout_t;

    typedef class assembly_layout_t<al::spool_t, 0, default_mt_template_t> __assembly_layout_t;

    //-------- ---------- ---------- ---------- ----------

    struct local_variable_defination_t
    {
        ref_t       type;
        msize_t     index;

        void write(xpool_t & xpool, __assembly_layout_t & layout, xil_buffer_t & buffer);
    };

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(method_stub_type_t)

        empty           = __default__,

        variable        = 1,

        switch_table    = 2,

        xil             = 15,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

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

        method_stub_type_t type() const { return (method_stub_type_t)__type; }

        void write(xil_buffer_t & buffer);
    };

    namespace
    {
        template<method_stub_type_t _type>
        struct __t_method_stub_t : method_stub_t
        {
            typedef method_stub_t __super_t;

            __t_method_stub_t() : __super_t(_type) { }

            static const method_stub_type_t __type__ = _type;
        };

        template<method_stub_type_t _type>
        struct __t_count_method_stub_t : __t_method_stub_t<_type>
        {
            __t_count_method_stub_t(uint16_t count)
            {
                this->__data3 = count;
            }

            uint16_t count() const { return this->__data3; }
            void set_count(uint16_t count) { this->__data3 = count; }
        };
    }

    struct method_variable_stub_t : __t_count_method_stub_t<method_stub_type_t::variable>
    {
        typedef __t_count_method_stub_t<method_stub_type_t::variable> __super_t;
        using __super_t::__super_t;
    };

    struct method_switch_table_stub_t : __t_count_method_stub_t<method_stub_type_t::switch_table>
    {
        typedef __t_count_method_stub_t<method_stub_type_t::switch_table> __super_t;
        using __super_t::__super_t;
    };

    typedef int local_label_t;

    struct switch_row_t
    {
        switch_row_t() = default;
        switch_row_t(int32_t value, local_label_t label)
            : value(value), label(label) { }

        int32_t     value;
        int32_t     step    = 0;

        local_label_t label;
    };

    struct switch_table_header_t
    {
        switch_table_header_t() = default;
        switch_table_header_t(uint16_t count) : count(count), __reserved__(0) { }

        uint16_t    count;
        uint16_t    __reserved__;
    };

    struct switch_table_t : switch_table_header_t
    {
        typedef switch_table_header_t __super_t;

        switch_table_t() = default;
        switch_table_t(int index, int count) : __super_t(count), index(index) { }

        int             index;

        local_label_t   default_label = 0;
        int32_t         default_step  = 0;
        xil_t *         jmp_xil = nullptr;

        switch_row_t    rows[0];

        switch_row_t * begin() const { return (switch_row_t *)rows; }
        switch_row_t * end()   const { return (switch_row_t *)rows + count; }

        void write(xil_buffer_t & buffer);
    };

    struct method_xil_stub_t : __t_method_stub_t<method_stub_type_t::xil>
    {
        typedef __t_method_stub_t<method_stub_type_t::xil> __super_t;
    };

    //-------- ---------- ---------- ---------- ----------

    class method_compile_context_t;

    class switch_manager_t : public object_t, public no_copy_ctor_t
    {
    public:
        switch_manager_t() { }

        switch_table_t * append_table(int row_count);
        void commit(statement_compile_context_t & ctx);
        void write(xil_buffer_t & buffer);

    private:
        int __tbl_index = 0;
        al::xheap_t __heap;
        al::small_vector_t<switch_table_t *, 2> __tables;
    };

    //-------- ---------- ---------- ---------- ----------

    class local_variables_t
    {
    public:
        void write(xpool_t & xpool, __assembly_layout_t & layout, xil_buffer_t & buffer);
        void append(local_variable_t * variable);

    private:
        al::small_vector_t<local_variable_t *, 5>  __variables;
    };

    //-------- ---------- ---------- ---------- ----------

    class method_compile_context_t : public object_t
    {
    public:
        method_compile_context_t(xpool_t & xpool, __assembly_layout_t & layout,
                 xil_pool_t & xil_pool, xil_buffer_t & buffer, logger_t & logger)
            : xil_pool(xil_pool), layout(layout), xpool(xpool), buffer(buffer), logger(logger)
        { }

        xpool_t &               xpool;
        __assembly_layout_t &   layout;
        xil_pool_t &            xil_pool;
        xil_buffer_t &          buffer;
        logger_t &              logger;
    };

    //-------- ---------- ---------- ---------- ----------

    class method_t : public __named_member_t
    {
        typedef member_t __super_t;

    public:
        typedef method_t * itype_t;

        type_name_t *       owner_type_name = nullptr;
        type_name_t *       type_name       = nullptr;
        generic_params_t *  generic_params  = nullptr;
        params_t *          params          = nullptr;
        method_body_t *     body            = nullptr;

        /*
        arguments_t *       base_ctor_args  = nullptr;
        method_t *          base_ctor       = nullptr;
        */

        method_trait_t      trait           = method_trait_t::__default__;

        method_inline_type_t inline_type    = method_inline_type_t::default_;

        virtual member_type_t this_type() override final
        {
            return member_type_t::method;
        }

        size_t generic_param_count() const
        {
            return generic_params? generic_params->size() : 0;
        }

        bool is_generic() const { return generic_param_count() > 0; }

        size_t param_count() const
        {
            return params? params->size() : 0;
        }

        param_t * param_at(size_t index) const
        {
            if(index >= param_count())
                return nullptr;

            return (*params)[index];
        }

        generic_param_t * find_param(const name_t & name);

        type_t * get_owner_type()
        {
            return owner_type_name? owner_type_name->type : nullptr;
        }

        void append_local(local_variable_t * variable);

        void compile(method_compile_context_t & ctx);

        virtual const string_t to_string() const override { return (string_t)*this; }
        operator string_t() const;

        static const efamily_t __family__ = efamily_t::method;

    private:
        local_variables_t __local_variables;
    };

    X_INLINE bool is_ctor(method_t * method)
    {
        return method->trait == method_trait_t::constructor;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<method_t *> method_ast_t;

    ////////// ////////// ////////// ////////// //////////

    class property_variable_t;

    class property_t : public __named_member_t
    {
        typedef member_t __super_t;

    public:
        typedef property_t * itype_t;

        type_name_t *   owner_type_name = nullptr;
        type_name_t *   type_name       = nullptr;
        params_t *      params          = nullptr;

        method_t *      get_method      = nullptr;
        method_t *      set_method      = nullptr;

        property_variable_t * variable  = nullptr;

        virtual member_type_t this_type() override final
        {
            return member_type_t::property;
        }

        virtual const string_t to_string() const override { return (string_t)*this; }
        operator string_t() const;

        static const efamily_t __family__ = efamily_t::property;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<property_t *> property_ast_t;

    ////////// ////////// ////////// ////////// //////////

    class event_variable_t;

    class event_t : public __named_member_t
    {
        typedef member_t __super_t;

    public:
        typedef event_t * itype_t;

        type_name_t *   owner_type_name = nullptr;
        type_name_t *   type_name       = nullptr;

        method_t *      add_method      = nullptr;
        method_t *      remove_method   = nullptr;

        event_variable_t * variable     = nullptr;

        virtual member_type_t this_type() override final
        {
            return member_type_t::event;
        }

        virtual const string_t to_string() const override { return (string_t)*this; }
        operator string_t() const;

        static const efamily_t __family__ = efamily_t::event;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<event_t *> event_ast_t;

    ////////// ////////// ////////// ////////// //////////

    class field_variable_t;

    class field_t : public __named_member_t
    {
        typedef member_t __super_t;

    public:
        typedef field_t * itype_t;

        type_name_t *   type_name   = nullptr;
        expression_t *  init_value  = nullptr;
        msize_t         offset      = unknown_msize;

        field_variable_t * variable = nullptr;

        virtual member_type_t this_type() override final
        {
            return member_type_t::field;
        }

        msize_t get_size();

        type_t * get_type() const;
        bool is_ref_type() const;

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

    class type_def_param_t : public __named_type_t, public with_index_t
    {
        typedef __named_type_t __super_t;

    public:
        using __super_t::__super_t;

        typedef type_def_param_t * itype_t;

        virtual gtype_t this_gtype() const override final { return gtype_t::type_def_param; }
        virtual ttype_t this_ttype() const override final { return ttype_t::__unknown__; }
        virtual vtype_t this_vtype() const override final { return vtype_t::__unknown__; }

        virtual member_t * get_member(analyze_member_args_t & args) override;
        virtual void get_members(analyze_members_args_t & args) override;

        virtual void get_all_members(members_t & out_members) override;
        virtual void each_super_type(each_super_type_callback_t callback) override { }

        virtual const string_t to_string() const override { return (string_t)*this; }
        virtual const string_t to_short_name() const override { return (string_t)*this; }

        operator string_t() const { return _T("?") + _str(name); }
    };

    typedef eobject_ast_t<type_def_param_t *> type_def_param_ast_t;
    typedef indexed_eobjects_t<type_def_param_t *> type_def_params_t;
    typedef eobject_ast_t<type_def_params_t *> type_def_params_ast_t;

    ////////// ////////// ////////// ////////// //////////

    class type_def_t : public __named_member_t
    {
        typedef __named_member_t __super_t;

    public:
        using __super_t::__super_t;

        typedef type_def_t * itype_t;

        type_name_t *       type_name  = nullptr;
        type_def_params_t * params     = nullptr;
        namespace_t *       namespace_ = nullptr;

        virtual member_type_t this_type() override final
        {
            return member_type_t::type_def;
        }

        virtual const string_t to_string() const override { return (string_t)*this; }

        size_t param_count() const
        {
            return params? params->size() : 0;
        }

        type_def_param_t * find_param(const name_t & name);
        type_t * get_type() const;

        operator string_t() const;
    };

    typedef eobject_ast_t<typename type_def_t::itype_t> type_def_ast_t;

    ////////// ////////// ////////// ////////// //////////

    template<typename _member_t, size_t size>
    class __member_vector_t : public al::small_vector_t<_member_t, size>
    {
        typedef al::small_vector_t<_member_t, size> __super_t;

    public:
        using __super_t::__super_t;

        _member_t push_back(_member_t member)
        {
            _A(member != nullptr);

            __super_t::push_back(member);
            __member_map[member->get_name()] = member;

            return member;
        }

        _member_t get(name_t name)
        {
            auto it = __member_map.find(name);
            if(it == __member_map.end())
                return nullptr;

            return it->second;
        }

    private:
        std::map<name_t, _member_t> __member_map;
    };

    //-------- ---------- ---------- ---------- ----------

    template<size_t size>
    class __member_vector_t<method_t *, size> : public al::small_vector_t<method_t *, size>
    {
        typedef al::small_vector_t<method_t *, size> __super_t;
        typedef int __generic_args_count_t;
        typedef type_t * __owner_t;

        typedef std::tuple<name_t, __generic_args_count_t,  __owner_t> __key_t;

    public:
        using __super_t::__super_t;

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

        template<typename f_t>
        void each(name_t name, int generic_param_count, type_t * owner_type, f_t f)
        {
            __key_t key(name, generic_param_count, owner_type);
            for(auto it = __method_map.lower_bound(key), it_end = __method_map.upper_bound(key);
                it != it_end; it++)
            {
                f(it->second);
            }
        }

        template<typename f_t>
        void each(name_t name, int generic_param_count, f_t f)
        {
            each(name, generic_param_count, (type_t *)nullptr, f);
        }

        template<typename f_t>
        void each(method_trait_t trait, f_t f)
        {
            __ensure_init_trait_method_map();

            for(auto it = __trait_method_map.lower_bound(trait),
                it_end = __trait_method_map.upper_bound(trait); it != it_end; it++)
            {
                f(it->second);
            }
        }

    private:
        std::multimap<__key_t, method_t *> __method_map;
        std::multimap<method_trait_t, method_t *> __trait_method_map;

        void __ensure_init_trait_method_map()
        {
            if(__trait_method_map.empty())
            {
                for(method_t * method : *this)
                {
                    __trait_method_map.insert(std::make_pair(method->trait, method));
                }
            }
        }
    };

    ////////// ////////// ////////// ////////// //////////

    class __general_type_like_base_t : public type_t
    {
        typedef type_t __super_t;

        template<typename t, size_t size = 2>
        using __list_t = owner_collection_t<type_t *, t, size, __member_vector_t<t, size>>;

    public:
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

        super_type_name_list_t  super_type_names;
        method_list_t           methods;
        property_list_t         properties;
        field_list_t            fields;
        event_list_t            events;
        nest_type_list_t        nest_types;
        type_def_list_t         type_defs;

        virtual member_t * get_member(analyze_member_args_t & args) override;
        virtual void get_members(analyze_members_args_t & args) override;
        virtual void get_all_members(members_t & out_members) override;

        virtual void commit(eobject_commit_context_t & ctx) override;
        bool committed() const { return __committed; }

        virtual msize_t get_vsize() override;
        virtual msize_t get_rcount() override;

        virtual type_t * get_base_type() override;

        virtual void each_super_type(each_super_type_callback_t callback) override;

    protected:
        msize_t __rcount = unknown_msize, __value_size = unknown_msize;

    private:
        void __ensure_size_initialize();
        bool __committed = false;
    };

    ////////// ////////// ////////// ////////// //////////

    X_INTERFACE type_impl_t : object_t
    {
        virtual void commit(eobject_commit_context_t & ctx, eobject_t * bind_object) = 0;

        static type_impl_t * default_();
    };

    ////////// ////////// ////////// ////////// //////////

    class general_type_t : public __general_type_like_base_t
    {
        typedef __general_type_like_base_t __super_t;

    public:
        using __super_t::__super_t;

        general_type_t() = default;

        name_t        name       = name_t::null;
        assembly_t  * assembly   = nullptr;
        namespace_t * namespace_ = nullptr;

        generic_params_t * params = nullptr;

        type_impl_t * impl = type_impl_t::default_();

        vtype_t vtype = vtype_t::mobject;
        ttype_t ttype = ttype_t::class_;

        void append_member(member_t * member);

        size_t nest_depth() const;

        virtual gtype_t this_gtype() const override final { return gtype_t::general; }
        virtual ttype_t this_ttype() const override final { return ttype; }
        virtual vtype_t this_vtype() const override final { return vtype; }

        virtual name_t get_name() const override { return name; }

        virtual const string_t to_full_name() const override { return (string_t)*this; }
        virtual const string_t to_short_name() const override;

        size_t params_count() const { return params? params->size() : 0; }
        bool   is_generic() const { return params_count() > 0; }

        virtual void commit(eobject_commit_context_t & ctx) override;

        generic_param_t * find_param(const name_t & name);

        operator string_t() const;

        virtual msize_t get_vsize() override;
        virtual msize_t get_rcount() override;

    private:
        bool __ensure_size_initialize();
    };

    //-------- ---------- ---------- ---------- ----------
    // generic_type_t

    typedef __vector_t<type_t *> type_collection_t;

    class generic_type_t : public __general_type_like_base_t
    {
        typedef __general_type_like_base_t __super_t;

    public:
        generic_type_t() = default;

        template<typename _type_collection_t>
        generic_type_t(general_type_t * template_, _type_collection_t && args,
                                                    type_t * host_type = nullptr)
            : template_(template_), args(std::forward<_type_collection_t>(args))
        {
            this->host_type = host_type;
        }

        general_type_t * template_ = nullptr;
        type_collection_t args;

        virtual gtype_t this_gtype() const override final { return gtype_t::generic; }

        virtual ttype_t this_ttype() const override final
        {
            return template_? template_->this_ttype() : ttype_t::__unknown__;
        }

        virtual vtype_t this_vtype() const override final
        {
            return template_? template_->this_vtype() : vtype_t::__unknown__;
        }

        virtual name_t get_name() const override
        {
            return template_? template_->get_name() : name_t::null;
        }

        void commit(eobject_commit_context_t & ctx) override;

        size_t argument_count() const { return args.size(); }
        bool any_type_def_param() const;
        type_t * type_at(size_t index) const;
        type_t * type_at(name_t name) const;

        virtual const string_t to_full_name() const override { return (string_t)*this; }
        virtual const string_t to_short_name() const override;

        bool is_no_specialization() const { return al::all_of_empty(args); }
        bool is_partial_specialization() const { return al::any_of_empty(args); }

        operator string_t() const;

    private:

        class __transform_context_t;
        typedef __transform_context_t __ctx_t;

        template<typename _members_t>
        void __transform_members(__ctx_t & ctx, _members_t & members, _members_t & out_members);

        type_t *      __transform_type(xpool_t & xpool, type_t * type);
        type_name_t * __transform_type_name(xpool_t & xpool, type_name_t * type_name);
        type_t *      __transform_generic_type(xpool_t & xpool, generic_type_t * type);

        field_t *     __transform_member(__ctx_t & ctx, field_t * field);
        method_t *    __transform_member(__ctx_t & ctx, method_t * method);
        property_t *  __transform_member(__ctx_t & ctx, property_t * property);
        event_t *     __transform_member(__ctx_t & ctx, event_t * event);
        type_t *      __transform_member(__ctx_t & ctx, type_t * nest_type);
        type_def_t *  __transform_member(__ctx_t & ctx, type_def_t * type_def);
        type_name_t * __transform_member(__ctx_t & ctx, type_name_t * type_name);
    };

    //-------- ---------- ---------- ---------- ----------
    // array_type_t

    typedef int16_t dimension_t;
    typedef al::small_vector_t<dimension_t, 2> dimensions_t;

    class array_type_t : public type_t
    {
    public:
        array_type_t() = default;
        array_type_t(type_t * element_type, dimension_t dimension)
            : element_type(element_type), dimension(dimension)
        { }

        type_t *    element_type  = nullptr;
        dimension_t dimension     = 1;

        virtual gtype_t this_gtype() const override final { return gtype_t::array;  }
        virtual ttype_t this_ttype() const override final { return ttype_t::class_; }
        virtual vtype_t this_vtype() const override final { return vtype_t::mobject; }

        virtual name_t  get_name() const override
        {
            return element_type? element_type->get_name() : name_t::null;
        }

        virtual member_t * get_member(analyze_member_args_t & args) override;
        virtual void get_members(analyze_members_args_t & args) override;
        virtual void get_all_members(members_t & out_members) override;

        virtual void each_super_type(each_super_type_callback_t callback) override { }

        virtual void commit(eobject_commit_context_t & ctx) override;

        virtual msize_t get_vsize() override;
        virtual msize_t get_rcount() override;

        virtual const string_t to_full_name() const override { return (string_t)*this; }
        operator string_t() const;
    };

    ////////// ////////// ////////// ////////// //////////
    // system_types

    general_type_t * get_system_type(const string_t & type_name);

    ////////// ////////// ////////// ////////// //////////
    // namespace

    class general_type_t;

    class namespace_t : public with_attributes_object_t
                      , public with_decorate_object_t
                      , public eobject_t
    {
    public:
        typedef namespace_t * itype_t;

        typedef __vector_t<type_t *, 5>   type_list_t;
        typedef __vector_t<namespace_t *> namespace_list_t;
        typedef __vector_t<type_def_t *>  type_def_list_t;

        emname_t *          name        = nullptr;
        const mname_t *     full_name   = nullptr;
        type_list_t         types;
        namespace_list_t    namespaces;
        type_def_list_t     type_defs;

        void append_type(general_type_t * type);

        operator string_t() const;
        virtual const string_t to_string() const override { return (string_t)*this; }

        static const efamily_t __family__ = efamily_t::namespace_;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef eobject_ast_t<namespace_t *> namespace_ast_t;

    ////////// ////////// ////////// ////////// //////////
    // document

    class document_t : public eobject_t
    {
    public:
        void add_import(import_t * import)
        {
            _A(import != nullptr);
            __imports.push_back(import);
        }

        void add_using(using_namespace_t * using_namespace)
        {
            _A(using_namespace != nullptr);
            __using_namespaces.push_back(using_namespace);
        }

        template<typename callback_t> void all_imports(callback_t callback)
        {
            al::for_each(__imports, callback);
        }

        auto all_imports() { return _range(__imports); }

        template<typename callback_t> void all_using_namespaces(callback_t callback)
        {
            al::for_each(__using_namespaces, callback);
        }

        auto all_using_namespaces() { return _range(__using_namespaces); }

        static const efamily_t __family__ = efamily_t::document;

    private:
        __vector_t<import_t *, 10> __imports;
        __vector_t<using_namespace_t *, 10> __using_namespaces;
    };

    typedef eobject_ast_t<document_t *> document_ast_t;

    ////////// ////////// ////////// ////////// //////////
    // module_t

    class module_t : public eobject_t
    {
    public:

        void register_commit(eobject_t * eobj);
        void commit(eobject_commit_context_t & ctx);

        static const efamily_t __family__ = efamily_t::module;

    private:
        std::vector<eobject_t *> __need_commit_eobjects;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_collection_cache_t

    typedef multikey_t<type_collection_t> type_collection_key_t;
    struct type_collection_id_t : compare_operators_t<type_collection_id_t, void *>
    {
        typedef void * __id_t;

        type_collection_id_t() : __id(nullptr) { }
        type_collection_id_t(__id_t id) : __id(id) { }

        operator __id_t () const { return __id; }
        operator string_t() const { return _F(_T("%1%"), __id); }

        __id_t __id;

        bool empty() const { return __id == nullptr; }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class type_collection_cache_t : public object_t
    {
        typedef type_collection_id_t  __tcid_t;
        typedef type_collection_key_t __key_t;

    public:
        template<typename _type_collection_t>
        __tcid_t get_tcid(_type_collection_t && types)
        {
            if(types.empty())
                return __tcid_t(nullptr);

            if(types.size() == 1)
                return __tcid_t((void *)types[0]);

            auto r = __set.insert(__key_t(std::forward<_type_collection_t>(types)));
            return __tcid_t((void *)&(*r.first));
        }

    private:
        std::set<type_collection_key_t> __set;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    typedef std::tuple<general_type_t *, type_collection_id_t, type_t *> generic_type_cache_key_t;
    typedef kv_object_t<generic_type_cache_key_t, generic_type_t *>  __generic_type_cache_t;

    typedef std::tuple<type_t *, dimension_t> array_type_cache_key_t;
    typedef kv_object_t<array_type_cache_key_t, array_type_t *> __array_type_cache_t;

    class assembly_reference_t;
    typedef std::tuple<const mname_t *, const mname_t *> assembly_reference_cache_key_t;
    typedef kv_object_t<assembly_reference_cache_key_t, assembly_reference_t *>
                                            __assembly_reference_cache_t;

    ////////// ////////// ////////// ////////// //////////
    // xpool_t

    class xpool_t;

    struct xtype_collection_t : no_copy_ctor_t
    {
    public:
        xtype_collection_t(const type_collection_t & types) : types(types) { }
        xtype_collection_t(const type_collection_t & types, type_collection_id_t tcid)
            : types(types), tcid(tcid)
        { }

        const type_collection_t & types;
        type_collection_id_t      tcid;

        operator const type_collection_t & () const { return types; }
        size_t size()  const { return types.size(); }
        bool   empty() const { return size() == 0;  }

        type_collection_id_t get_tcid(xpool_t & pool);

    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class xpool_t : public object_t, public no_copy_ctor_t
    {
    public:
        xpool_t(memory_t * memory) : memory(memory) { }

        void initialize();

        al::spool_t         spool;
        memory_t * const    memory = nullptr;
        __mname_cache_t *   mname_cache = nullptr;

        __generic_type_cache_t * generic_type_cache = nullptr;
        __array_type_cache_t   * array_type_cache   = nullptr; 

        __assembly_reference_cache_t * assembly_reference_cache = nullptr;

        type_collection_cache_t type_collection;

        generic_type_t * new_generic_type(general_type_t * template_,
                                          xtype_collection_t & types, type_t * host_type);
        generic_type_t * new_generic_type(general_type_t * template_,
                                          type_collection_t & types, type_t * host_type);

        array_type_t * new_array_type(type_t * element_type, dimension_t dimension);

        assembly_reference_t * new_reference(const mname_t * package_name, assembly_t * assembly);

        attribute_t    * get_compile_time_attribute();
        general_type_t * get_internal_type(vtype_t vtype);
        general_type_t * get_internal_type(value_type_t value_type);
        general_type_t * get_internal_type(name_t name);

        template<typename t, typename ... args_t> t * new_obj(args_t && ... args)
        {
            return memory_t::new_obj<t>(memory, std::forward<args_t>(args) ...);
        }

        void append_new_type(type_t * type);
        void commit_types(logger_t & logger);

    private:
        attribute_t * __compile_time_attribute = nullptr;
        std::queue<type_t *> __new_types;
    };

    X_INLINE bool is_compile_time_attribute(xpool_t & xpool, attribute_t * attr)
    {
        return xpool.get_compile_time_attribute() == attr;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    X_INLINE mname_operate_context_t to_mname_operate_context(xpool_t & pool)
    {
        return mname_operate_context_t(pool.spool, pool.memory, pool.mname_cache);
    }

    ////////// ////////// ////////// ////////// //////////
    // variable_t

    X_ENUM(variable_type_t)

        local       = 1 << 0,

        param       = 1 << 1,

        field       = 1 << 2,

        property    = 1 << 3,

        event       = 1 << 4,

    X_ENUM_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    X_INTERFACE variable_t : object_t
    {
        virtual name_t get_name() = 0;
        virtual type_name_t * get_type_name() = 0;
        virtual variable_type_t this_type() = 0;

        vtype_t  get_vtype();
        type_t * get_type();
    };

    class variable_base_t : public variable_t
    {

    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class local_variable_t : public variable_base_t
    {
    public:
        local_variable_t(type_name_t * type_name, name_t name)
            : type_name(type_name), name(name)
        {
            _A(type_name != nullptr);
            _A(name != name_t::null);
        }

        type_name_t * type_name = nullptr;
        name_t        name      = name_t::null;
        msize_t       identity  = unknown_msize;
        msize_t       index     = unknown_msize;

        static const variable_type_t type = variable_type_t::local;

        virtual type_name_t * get_type_name() override { return type_name; }
        virtual name_t get_name() override { return name; }
        virtual variable_type_t this_type() override { return variable_type_t::local; }

        X_TO_STRING
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class param_variable_t : public variable_base_t
    {
    public:
        param_variable_t(param_t * param) : param(param)
        {
            _A(param != nullptr);
        }

        param_t * param     = nullptr;

        virtual type_name_t * get_type_name() override
        {
            return param->type_name;
        }

        virtual name_t get_name() override
        {
            return param->name;
        }

        virtual variable_type_t this_type() override
        {
            return variable_type_t::param;
        }

        X_TO_STRING

        static const variable_type_t type = variable_type_t::param;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class field_variable_t : public variable_base_t
    {
    public:
        field_variable_t(field_t * field) : field(field)
        {
            _A(field != nullptr);
        }

        field_t * field = nullptr;

        virtual type_name_t * get_type_name() override
        {
            return field->type_name;
        }

        virtual name_t get_name() override
        {
            return field->name;
        }

        virtual variable_type_t this_type() override
        {
            return variable_type_t::field;
        }

        X_TO_STRING

        static const variable_type_t type = variable_type_t::field;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class property_variable_t : public variable_base_t
    {
    public:
        property_variable_t(property_t * property) : property(property)
        {
            _A(property != nullptr);
        }

        property_t * property = nullptr;

        virtual type_name_t * get_type_name() override
        {
            return property->type_name;
        }

        virtual name_t get_name() override
        {
            return property->name;
        }

        virtual variable_type_t this_type() override
        {
            return variable_type_t::property;
        }

        X_TO_STRING

        static const variable_type_t type = variable_type_t::property;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class event_variable_t : public variable_base_t
    {
    public:
        event_variable_t(event_t * event) : event(event)
        {
            _A(event != nullptr);
        }

        event_t * event = nullptr;

        virtual type_name_t * get_type_name() override
        {
            return event->type_name;
        }

        virtual name_t get_name() override
        {
            return event->name;
        }

        virtual variable_type_t this_type() override
        {
            return variable_type_t::event;
        }

        X_TO_STRING

        static const variable_type_t type = variable_type_t::event;
    };

    ////////// ////////// ////////// ////////// //////////

    X_ENUM(ast_error_t)

        variable_duplicated,

        variable_defination_unexpected,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////
    // variable_region_t

    class variable_region_t : public object_t
    {
    public:
        variable_region_t(memory_t * memory, variable_region_t * previous,
                variable_type_t mask = (variable_type_t)0xFF, object_t * owner = nullptr)
            : __memory(memory), previous(previous), __mask(mask)
            , __owner(owner? owner: previous? previous->__owner : nullptr) { }

        variable_region_t * previous = nullptr;

        local_variable_t * define_local(type_name_t * type_name, const name_t & name);
        param_variable_t * define_param(param_t * param);
        field_variable_t * define_field(field_t * field);
        property_variable_t * define_property(property_t * property);
        event_variable_t * define_event(event_t * event);

        variable_t * get(const name_t & name);

    private:
        memory_t * __memory;
        std::map<name_t, variable_t *> __variables;
        variable_type_t __mask;
        object_t * __owner;

        static const msize_t __empty_index = unknown_msize;
        msize_t __local_index = __empty_index, __local_identity = 0;

        bool __is_supported(variable_type_t type) const
        {
            return enum_has_flag(__mask, type);
        }

        bool __is_local_supported() const
        {
            return __is_supported(variable_type_t::local);
        }

        void __check_duplicate(const name_t & name);

        msize_t __next_local_identity();
        msize_t & __current_local_identity();

        msize_t __next_local_index();
        msize_t & __current_local_index();

        template<typename _variable_t, typename ... args_t>
        _variable_t * __new_variable(args_t && ... args);

        template<typename _variable_t, typename ... args_t>
        _variable_t * __define(args_t && ... args);
    };

    //-------- ---------- ---------- ---------- ----------

    

    ////////// ////////// ////////// ////////// //////////
    // ast_walk_context_layer_t

    X_ENUM(ast_walk_context_layer_state_t)

        #define __Ls(name)  name = (int)efamily_t::name

        __Ls(assembly),

        __Ls(module),
        
        __Ls(document),
        
        __Ls(namespace_),
        
        __Ls(type),
        
        __Ls(method),
        
        __Ls(property),
        
        __Ls(event),
        
        __Ls(field),
        
        __Ls(type_def),
        
        region,

        #undef __Ls

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    class ast_walk_context_t;

    X_INTERFACE ast_walk_context_layer_t
    {
        typedef ast_walk_context_t __context_t;
        typedef ast_walk_context_layer_t __layer_t, __self_t;
        typedef ast_walk_context_layer_state_t __layer_state_t;

        ast_walk_context_layer_t(__context_t & context, __self_t * previous,
                                                object_t * owner = nullptr)
            : context(context), previous(previous)
        { }

        __context_t & context;
        __layer_t * previous = nullptr;

        virtual void * get(__layer_state_t state) = 0;
    };

    //-------- ---------- ---------- ---------- ----------
    // assembly_loader_t

    X_INTERFACE assembly_loader_t
    {
        virtual assembly_t * load_assembly(
            const mname_t * package_name, const mname_t * assembly_name) = 0;
    };

    //-------- ---------- ---------- ---------- ----------
    // ast_walk_queue_t

    template<typename element_t, size_t queue_size>
    class ast_walk_queue_t : public object_t
    {
        typedef object_t __super_t;
        static const size_t __queue_size = queue_size;

    public:
        ast_walk_queue_t()
        {
            for(size_t index = 0; index < __queue_size; index++)
            {
                __qitems[index] = memory_t::new_obj<__queue_item_t>(&__qitems_pool);
            }
        }

        void enque(element_t item, int priory)
        {
            _A(priory < __queue_size);

            __qitems[priory]->enque(item);
        }

        bool deque(element_t * out_element)
        {
            for(; __qitem_index < __queue_size; __qitem_index++)
            {
                if(__qitems[__qitem_index]->deque(out_element))
                    return true;
            }

            return false;
        }

    private:

        class __queue_item_t : public object_t
        {
        public:
            void enque(element_t item)
            {
                __queue.push(item);
            }

            bool deque(element_t * out_element)
            {
                if(__queue.empty())
                    return false;

                *out_element = __queue.front();
                __queue.pop();

                return true;
            }

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

    class ast_walk_context_t : public object_t, public no_copy_ctor_t
    {
        typedef ast_walk_context_layer_t        __layer_t;
        typedef ast_walk_context_layer_state_t  __layer_state_t;

        template<typename _module_ast_node_t>
        friend void ast_walk(ast_walk_context_t & walk_context,
                                const std::vector<ast_node_t *> & nodes);

    public:
        ast_walk_context_t(assembly_t & assembly, xpool_t & xpool,
            assembly_loader_t * assembly_loader, logger_t & logger, __layer_t * layer = nullptr
        );

        void push(namespace_t * namespace_);
        void push(type_t * type);
        void push(method_t * method);
        void push(property_t * property);
        void push(event_t * event);
        void push(field_t * field);
        void push(type_def_t * type_def);
        void push_new_region();
        document_t * push_new_document();
        module_t * push_new_module();

        void pop();

        assembly_t & assembly;
        logger_t &   logger;

        template<typename t, typename ... args_t> t * new_obj(args_t && ... args)
        {
            return __xheap.new_obj<t>(std::forward<args_t>(args) ...);
        }

        module_t * current_module()
        {
            return (module_t *)__current_layer->get(__layer_state_t::module);
        }

        document_t * current_document()
        {
            return (document_t *)__current_layer->get(__layer_state_t::document);
        }

        namespace_t * current_namespace()
        {
            return (namespace_t *)__current_layer->get(__layer_state_t::namespace_);
        }

        type_t * current_type()
        {
            return (type_t *)__current_layer->get(__layer_state_t::type);
        }

        method_t * current_method()
        {
            return (method_t *)__current_layer->get(__layer_state_t::method);
        }

        property_t * current_property()
        {
            return (property_t *)__current_layer->get(__layer_state_t::property);
        }

        event_t * current_event()
        {
            return (event_t *)__current_layer->get(__layer_state_t::event);
        }

        field_t * current_field()
        {
            return (field_t *)__current_layer->get(__layer_state_t::field);
        }

        type_def_t * current_type_def()
        {
            return (type_def_t *)__current_layer->get(__layer_state_t::type_def);
        }

        variable_region_t * current_region()
        {
            return (variable_region_t *)__current_layer->get(__layer_state_t::region);
        }

        __layer_t * current_layer() const { return __current_layer; }

        void delay(ast_node_t * node, int step = 0, void * tag = nullptr)
        {
            __walk_items.enque(__walk_item_t { __current_layer, node, step, tag }, step);
        }

        xpool_t & xpool;

        const mname_t * parent_of(const mname_t * mname);
        const mname_t * child_of(const mname_t * mname);
        const mname_t * combine(const mname_t * name1, const mname_t * name2);
        const mname_t * combine(const mname_t * name1, const name_t & name2);

        type_name_t * to_type_name(vtype_t vtype);

        assembly_t * load_assembly(const mname_t * package_name, const mname_t * assembly_name);

    private:
        struct __walk_item_t
        {
            __layer_t  *    layer;
            ast_node_t *    node;
            int             step;
            void *          tag;
        };

        xheap_t __xheap;
        __layer_t * __current_layer = nullptr;
        assembly_loader_t * __assembly_loader;
        ast_walk_queue_t<__walk_item_t, 32> __walk_items;

        std::map<const mname_t *, const mname_t *> __parent_mnames;
        std::map<const mname_t *, const mname_t *> __child_mnames;

        typedef std::tuple<const mname_t *, const mname_t *> __combine_name_key_t;
        std::map<__combine_name_key_t, const mname_t *> __combine_names;

        typedef std::tuple<const mname_t *, name_t> __combine_name_key2_t;
        std::map<__combine_name_key2_t, const mname_t *> __combine_names2;

        typedef std::tuple<const mname_t *, const mname_t *> __assembly_cache_key_t;
        std::map<__assembly_cache_key_t, assembly_t *> __assembly_cache;

        std::map<vtype_t, type_name_t *> __vtype_type_name_map;

        template<typename layer_t, typename ... args_t>
        layer_t * __push_new_layer(args_t && ... args)
        {
            __current_layer = new_obj<layer_t>(*this,
                __current_layer, std::forward<args_t>(args) ...
            );

            return (layer_t *)__current_layer;
        }

        void __walk();
    };

    class module_ast_node_t;

    template<typename _module_ast_node_t = module_ast_node_t>
    void ast_walk(ast_walk_context_t & walk_context, const std::vector<ast_node_t *> & nodes)
    {
        _module_ast_node_t * module_node = walk_context.xpool.new_obj<_module_ast_node_t>();

        for(ast_node_t * node : nodes)
        {
            module_node->append(node);
        }

        walk_context.delay(module_node);
        walk_context.__walk();
    }

    template<typename _module_ast_node_t = module_ast_node_t>
    void ast_walk(ast_walk_context_t & walk_context, ast_node_t * root)
    {
        _A(root != nullptr);

        ast_walk<_module_ast_node_t>(walk_context, std::vector<ast_node_t *>({ root }));
    }

    ////////// ////////// ////////// ////////// //////////
    // Class ast_node_t

    class ast_node_t : public eobject_t, public code_element_t
	{
    public:
        virtual ast_value_t value() const = 0;
        virtual size_t child_count() const = 0;
        virtual ast_node_t * child_at(size_t index) const = 0;
        virtual void set_child(size_t index, ast_node_t * node) = 0;

        virtual void on_walk(ast_walk_context_t & context, int step, void * tag);
        virtual void on_commit() { }

        ast_node_t * operator[](size_t index) const
        {
            return child_at(index);
        }

        operator string_t() const
        {
            return this->to_string();
        }
    };

    ////////// ////////// ////////// ////////// //////////

    template<size_t _child_count>
    class array_ast_node_t : public ast_node_t
    {
    public:
        using ast_node_t::ast_node_t;

        virtual size_t child_count() const override { return _child_count; }

        virtual ast_node_t * child_at(size_t index) const override
        {
            _A(index < _child_count);

            return __childs[index];
        }

        virtual void set_child(size_t index, ast_node_t * node) override
        {
            _A(index < _child_count);
            _A(node != nullptr);

            __childs[index] = node;
        }

        virtual const string_t to_string() const override
        {
            return _T("[array ast]");
        }

    private:
        ast_node_t * __childs[_child_count];
    };

    ////////// ////////// ////////// ////////// //////////

    class single_ast_node_t : public array_ast_node_t<0>
    {
    public:
        using array_ast_node_t<0>::array_ast_node_t;

        virtual const string_t to_string() const override
        {
            return _T("[single ast]");
        }
    };

    ////////// ////////// ////////// ////////// //////////

    class multipy_ast_node_t : public ast_node_t
    {
    public:
        using ast_node_t::ast_node_t;

        virtual size_t child_count() const override;
        virtual ast_node_t * child_at(size_t index) const override;
        virtual void set_child(size_t index, ast_node_t * node) override;
        void append(ast_node_t * node);

        virtual const string_t to_string() const override
        {
            return _T("[multipy ast]");
        }

    private:
        std::vector<ast_node_t *> __childs;
    };

    //-------- ---------- ---------- ---------- ----------

    template<element_value_t _value>
    class tmultipy_ast_node_t : public multipy_ast_node_t
    {
    public:
        using multipy_ast_node_t::multipy_ast_node_t;

        virtual element_value_t value() const { return _value; }
    };

    //-------- ---------- ---------- ---------- ----------

    void expand_ast_node(ast_node_t * node, std::function<void (ast_node_t *)> callback);

    ////////// ////////// ////////// ////////// //////////
    // document_ast_node_t

    class document_ast_node_t : public multipy_ast_node_t
    {
        typedef multipy_ast_node_t __super_t;

    public:
        virtual ast_value_t value() const override final
        {
            return document_ast_value;
        }

        virtual const string_t to_string() const override
        {
            return _T("[document ast]");
        }

        virtual void on_commit() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // module_ast_node_t

    class module_ast_node_t : public multipy_ast_node_t
    {
        typedef multipy_ast_node_t __super_t;

    public:
        virtual ast_value_t value() const override final
        {
            return module_ast_value;
        }

        virtual const string_t to_string() const override
        {
            return _T("[module ast]");
        }

        virtual void on_commit() override;
        virtual void on_walk(ast_walk_context_t & context, int step, void * tag) override;
        virtual void do_walk(ast_walk_context_t & context, int step, void * tag);

        module_t * get_module() { return __module; }

    protected:
        module_t * __module;
    };

    ////////// ////////// ////////// ////////// //////////
    // named_object_t

    X_INTERFACE named_object_t
    {
        named_object_t(const mname_t * name) : name(name) { }

        const mname_t * name;

        const string_t to_string() const
        {
            if(name == nullptr)
                return empty_str;

            return (string_t)*name;
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // Class ast_sulution_t

    class ast_project_t;

    class ast_solution_t : public object_t, public named_object_t
    {
        typedef named_object_t __super_t;

    public:
        ast_solution_t(const mname_t * name) : named_object_t(name) { }

        void add_project(ast_project_t * cproject);
        ast_project_t * get_project(const mname_t * name);

        auto begin() const { return __projects.begin(); }
        auto end()   const { return __projects.end();   }

        virtual const string_t to_string() const override
        {
            return __super_t::to_string();
        }

    private:
        std::map<sid_t, ast_project_t *> __project_map;
        std::vector<ast_project_t *> __projects;
    };

    ////////// ////////// ////////// ////////// //////////
    // Class ast_project_t

    class ast_file_t;

    class ast_project_t : public object_t, public named_object_t
    {
        typedef named_object_t __super_t;

    public:
        ast_project_t(const mname_t * name) : named_object_t(name) { }

        void add_file(ast_file_t * cfile);
        ast_file_t * get_file(const mname_t * name);

        auto begin() const { return __files.begin(); }
        auto end()   const { return __files.end();   }

        virtual const string_t to_string() const override
        {
            return __super_t::to_string();
        }

        ast_solution_t * solution = nullptr;

    private:
        std::map<sid_t, ast_file_t *> __file_map;
        std::vector<ast_file_t *> __files;
    };

    ////////// ////////// ////////// ////////// //////////

    // Class ast_file
    class ast_file_t : public object_t, public named_object_t
    {
        typedef named_object_t __super_t;

    public:
        ast_file_t(const mname_t * name, ast_node_t * root)
            : named_object_t(name), root(root)
        {
            _A(root != nullptr);
        }

        ast_node_t * root;
        ast_project_t * project = nullptr;

        virtual const string_t to_string() const override
        {
            return __super_t::to_string();
        }
    };

    ////////// ////////// ////////// ////////// //////////

    X_ENUM(expression_type_t)

        simple,

        op,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(expression_execute_error_code_t)

        expect_const_expression,

        invalid_binary_operator,

        invalid_unitary_operator,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    class expression_execute_context_t
    {
    public:
        expression_execute_context_t(xpool_t & xpool) : xpool(xpool) { }

        xpool_t & xpool;
    };

    //-------- ---------- ---------- ---------- ----------

    class statement_compile_context_t;

    class expression_compile_context_t : public object_t
    {
    public:
        expression_compile_context_t(statement_compile_context_t & statement_ctx)
            : statement_ctx(statement_ctx) { }

        statement_compile_context_t & statement_ctx;
    };

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(expression_family_t)

        name, cvalue, unitary, binary, expressions, function, index,

        new_, new_array, default_value, type_of, type_name, type_cast,

        this_, base,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(expression_behaviour_t)

        default_, assign, execute, new_,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    class expression_t : public eobject_t
    {
        typedef eobject_t __super_t;

    public:
        typedef expression_t * itype_t;

        virtual expression_family_t this_family() const = 0;

        virtual size_t expression_count() const = 0;
        virtual expression_t * expression_at(size_t index) const = 0;

        virtual const string_t to_string() const override
        {
            auto w = std::bind(al::_wrap_index<size_t>, std::placeholders::_1);
            return al::join_str(w(0), w(expression_count()), _T(", "),
                [this](size_t index) { return _str(expression_at(index)); }
            );
        }

        expression_t * operator[](size_t index) const
        {
            return expression_at(index);
        }

        virtual cvalue_t execute(expression_execute_context_t & ctx) { return cvalue_t::nan; }

        virtual void compile(expression_compile_context_t & ctx, xil_pool_t & pool)
        {
            throw _unimplemented_error(this, _T("compile"));
        }

        virtual vtype_t get_vtype() const
        {
            throw _unimplemented_error(this, _T("get_vtype"));
        }

        virtual type_t * get_type(xpool_t & xpool) const
        {
            throw _unimplemented_error(this, _T("get_type"));
        }

        virtual expression_behaviour_t get_behaviour() const
        {
            throw _unimplemented_error(this, _T("get_behaviour"));
        }

        expression_t * parent = nullptr;
        bool walked = false;
    };

    typedef eobject_ast_t<typename expression_t::itype_t> expression_ast_t;

    X_INLINE bool is_top_expression(expression_t * exp)
    {
        return exp != nullptr && exp->parent == nullptr;
    }

    //-------- ---------- ---------- ---------- ----------

    template<size_t exp_count>
    class texpression_t : public expression_t
    {
        typedef expression_t __super_t;

    public:
        template<typename ... exps_t> texpression_t(exps_t ... exps_)
        {
            static_assert(sizeof ... (exps_) == exp_count, "expression count error");

            al::assign(exps, exps_ ...);
            al::each([this](expression_t * exp) { exp->parent = this; }, exps_ ...);
        }

        virtual size_t expression_count() const override final
        {
            return exp_count;
        }

        virtual expression_t * expression_at(size_t index) const override final
        {
            return exps[index];
        }

        virtual expression_behaviour_t get_behaviour() const override
        {
            return expression_behaviour_t::default_;
        }

        expression_t * exps[exp_count];
    };

    //-------- ---------- ---------- ---------- ----------

    namespace
    {
        class __simple_expression_base_t : public expression_t
        {
            typedef expression_t __super_t;

        public:
            virtual size_t expression_count() const override final { return 0; }
            virtual expression_t * expression_at(size_t index) const override final
            {
                return nullptr;
            }

            virtual expression_behaviour_t get_behaviour() const override
            {
                return expression_behaviour_t::default_;
            }
        };
    };

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(name_expression_type_t)

        variable, type, type_def,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    class name_expression_t : public __simple_expression_base_t
    {
        typedef __simple_expression_base_t __super_t;

    public:
        name_expression_t() = default;
        name_expression_t(const name_t & name) : name(name) { }

        template<typename _var_t>   // variable_t, type_t, type_def_t
        name_expression_t(_var_t * var) : name(var->get_name())
        {
            this->set(var);
        }

        name_t name = name_t::null;
        name_expression_type_t expression_type = name_expression_type_t::__default__;

        union
        {
            variable_t * variable = nullptr;
            type_t *     type;
            type_def_t * type_def;
        };

        virtual expression_family_t this_family() const override
        {
            return expression_family_t::name;
        }

        virtual vtype_t get_vtype() const override;
        virtual type_t * get_type(xpool_t & xpool) const override;

        void set(variable_t * variable);
        void set(type_t * type);
        void set(type_def_t * type_def);

        virtual const string_t to_string() const override;
    };

    //-------- ---------- ---------- ---------- ----------

    class type_name_expression_t : public __simple_expression_base_t
    {
        typedef __simple_expression_base_t __super_t;

    public:
        type_name_expression_t(type_name_t * type_name) : type_name(type_name) { }
        type_name_expression_t() { }

        type_name_t * type_name = nullptr;

        virtual expression_family_t this_family() const override
        {
            return expression_family_t::type_name;
        }

        virtual vtype_t get_vtype() const override;
        virtual type_t * get_type(xpool_t & xpool) const override;

        virtual const string_t to_string() const override
        {
            return _str(type_name);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    class type_cast_expression_t : public __simple_expression_base_t
    {
        typedef __simple_expression_base_t __super_t;

    public:
        using __super_t::__super_t;

        type_name_t  * type_name = nullptr;
        expression_t * expression = nullptr;

        virtual expression_family_t this_family() const override
        {
            return expression_family_t::type_cast;
        }

        virtual vtype_t get_vtype() const override;
        virtual type_t * get_type(xpool_t & xpool) const override;

        virtual const string_t to_string() const override
        {
            return _F(_T("(%1%)%2%"), type_name, expression);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    class cvalue_expression_t : public __simple_expression_base_t
    {
        typedef __simple_expression_base_t __super_t;

    public:
        cvalue_expression_t(cvalue_t * value) : value(value)
        {
            _A(value != nullptr);
        }

        cvalue_t * value = nullptr;

        virtual expression_family_t this_family() const override
        {
            return expression_family_t::cvalue;
        }

        virtual const string_t to_string() const override
        {
            return (string_t)*this;
        }

        operator string_t() const
        {
            return value != nullptr? (string_t)*value : empty_str;
        }

        virtual cvalue_t execute(expression_execute_context_t & ctx) override
        {
            return value? *value : cvalue_t();
        }

        virtual vtype_t get_vtype() const override;
        virtual type_t * get_type(xpool_t & xpool) const override;
    };

    //-------- ---------- ---------- ---------- ----------

    template<size_t exp_count>
    class op_expression_t : public texpression_t<exp_count>
    {
        typedef texpression_t<exp_count> __super_t;

    public:
        template<typename ... exps_t>
        op_expression_t(const operator_property_t * op_property, exps_t ... exps)
            : texpression_t<exp_count>(exps ...), op_property(op_property)
        {
            _A(op_property != nullptr);
        }

        template<typename ... exps_t>
        op_expression_t(operator_t op, exps_t ... exps)
            : op_expression_t(get_system_operator_property(op), std::forward<exps_t>(exps) ...)
        { }


        const operator_property_t * const op_property;
        operator_t op() const { return op_property->op; }

        virtual expression_behaviour_t get_behaviour() const override
        {
            if(op_property->is_assign)
                return expression_behaviour_t::assign;

            return expression_behaviour_t::default_;
        }
    };

    //-------- ---------- ---------- ---------- ----------

    class unitary_expression_t : public op_expression_t<1>
    {
        typedef op_expression_t<1> __super_t;

    public:
        using __super_t::__super_t;

        expression_t * exp() const { return expression_at(0); }

        virtual expression_family_t this_family() const override
        {
            return expression_family_t::unitary;
        }

        virtual vtype_t get_vtype() const override;
        virtual type_t * get_type(xpool_t & xpool) const override;
        cvalue_t execute(expression_execute_context_t & ctx) override;

        virtual const string_t to_string() const override;
    };

    //-------- ---------- ---------- ---------- ----------

    class binary_expression_t : public op_expression_t<2>
    {
        typedef op_expression_t<2> __super_t;

    public:
        using __super_t::__super_t;

        expression_t * exp1() const { return expression_at(0); }
        expression_t * exp2() const { return expression_at(1); }

        virtual expression_family_t this_family() const override
        {
            return expression_family_t::binary;
        }

        virtual vtype_t get_vtype() const override;
        virtual type_t * get_type(xpool_t & xpool) const override;

        virtual cvalue_t execute(expression_execute_context_t & ctx) override;

        virtual const string_t to_string() const override;
    };

    //-------- ---------- ---------- ---------- ----------

    class expressions_t : public expression_t
    {
        typedef expression_t __super_t;

    public:
        typedef expression_t * value_type;
        expressions_t() = default;

        virtual expression_family_t this_family() const override
        {
            return expression_family_t::expressions;
        }

        template<typename exp_itor_t>
        expressions_t(exp_itor_t begin, exp_itor_t end)
        {
            append(begin, end);
        }

        virtual size_t expression_count() const override
        {
            return __expressions.size();
        }

        virtual expression_t * expression_at(size_t index) const override
        {
            _A(index < __expressions.size());
            return __expressions[index];
        }

        virtual vtype_t get_vtype() const override;
        virtual type_t * get_type(xpool_t & xpool) const override;

        virtual const string_t to_string() const override
        {
            string_t s = al::join_str(__expressions.begin(), __expressions.end(), _T(", "),
                [this](expression_t * exp) { return _str(exp); }
            );

            return sprintf(_T("(%1%)"), s);
        }

        void append(expression_t * expression)
        {
            _A(expression != nullptr);
            __expressions.push_back(expression);
            expression->parent = this;
        }

        void push_back(expression_t * expression)
        {
            append(expression);
        }

        template<typename exp_itor_t>
        void append(exp_itor_t begin, exp_itor_t end)
        {
            for(exp_itor_t it = begin; it != end; it++)
            {
                append(*it);
            }
        }

        virtual cvalue_t execute(expression_execute_context_t & ctx) override
        {
            throw _EC(unimplemented);
        }

        virtual expression_behaviour_t get_behaviour() const override
        {
            return expression_behaviour_t::default_;
        }

        auto begin() const { return __expressions.begin(); }
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
        class __arguments_expression_base_t : public expression_t
        {
            typedef expression_t __super_t;

        public:
            __arguments_expression_base_t() = default;

            __arguments_expression_base_t(arguments_t * arguments)
            {
                set_arguments(arguments);
            }

            void set_arguments(arguments_t * arguments)
            {
                __arguments = arguments;

                if(arguments != nullptr)
                {
                    for(argument_t * arg : *arguments)
                    {
                        arg->expression->parent = this;
                    }
                }
            }

            arguments_t * arguments() const { return __arguments; }

            size_t argument_count() const
            {
                return __arguments? __arguments->size() : 0;
            }

            argument_t * argument_at(size_t index) const
            {
                if(__arguments == nullptr || index >= __arguments->size())
                    return nullptr;

                return (*__arguments)[index];
            }

            virtual size_t expression_count() const override final
            {
                return argument_count();
            }

            virtual expression_t * expression_at(size_t index) const override final
            {
                if(__arguments == nullptr)
                    return nullptr;

                return (*__arguments)[index]->expression;
            }

        private:
            arguments_t * __arguments = nullptr;
        };
    }

    //-------- ---------- ---------- ---------- ----------

    class function_expression_t : public __arguments_expression_base_t
    {
        typedef __arguments_expression_base_t __super_t;

    public:
        function_expression_t() = default;
        function_expression_t(method_t * method, arguments_t * arguments)
            : __super_t(arguments), method(method)
        { }

        expression_t    * namex  = nullptr;
        method_t        * method = nullptr;
        generic_args_t  * generic_args = nullptr;

        size_t generic_args_count() const
        {
            return generic_args? generic_args->size() : 0;
        }

        virtual vtype_t get_vtype() const override;
        virtual expression_family_t this_family() const override
        {
            return expression_family_t::function;
        }

        name_t get_name() const;

        virtual expression_behaviour_t get_behaviour() const override
        {
            return expression_behaviour_t::execute;
        }

        virtual const string_t to_string() const override;
    };

    //-------- ---------- ---------- ---------- ----------

    class index_expression_t : public __arguments_expression_base_t
    {
        typedef __arguments_expression_base_t __super_t;

    public:
        expression_t * namex    = nullptr;
        property_t   * property = nullptr;

        virtual expression_family_t this_family() const override
        {
            return expression_family_t::index;
        }

        virtual vtype_t get_vtype() const override;

        virtual const string_t to_string() const override;

        virtual expression_behaviour_t get_behaviour() const override
        {
            return expression_behaviour_t::execute;
        }
    };

    typedef eobject_ast_t<index_expression_t *> index_expression_ast_t;

    //-------- ---------- ---------- ---------- ----------
    
    class new_expression_t : public __arguments_expression_base_t
    {
        typedef __simple_expression_base_t __super_t;

    public:
        type_name_t * type_name   = nullptr;
        method_t    * constructor = nullptr;

        virtual expression_family_t this_family() const override
        {
            return expression_family_t::new_;
        }

        virtual vtype_t get_vtype() const override;
        virtual const string_t to_string() const override;

        virtual expression_behaviour_t get_behaviour() const override
        {
            return expression_behaviour_t::new_;
        }
    };

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(array_initialize_element_type_t)

        initializer, expression,

    X_ENUM_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class array_initializer_t;

    struct array_initialize_element_t
    {
        typedef eobject_t __super_t;
        typedef array_initialize_element_t __self_t;
        typedef array_initialize_element_type_t __type_t;

    public:
        array_initialize_element_t(array_initializer_t * initializer)
            : type(__type_t::initializer), initializer(initializer)
        {
            _A(initializer != nullptr);
        }

        array_initialize_element_t(expression_t * expression)
            : type(__type_t::expression), expression(expression)
        {
            _A(expression != nullptr);
        }

        operator string_t() const;

        __type_t type;

        union
        {
            array_initializer_t *   initializer;
            expression_t *          expression;
        };
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class array_initializer_t : public eobject_t
    {
        typedef eobject_t __super_t;

    public:
        __vector_t<array_initialize_element_t> elements;

        void append(expression_t * expression);
        void append(array_initializer_t * initializer);

        size_t size()  const { return elements.size(); }
        bool   empty() const { return size() == 0;     }

        array_initialize_element_t & operator [](size_t index)
        {
            return elements[index];
        }

        const array_initialize_element_t & operator [](size_t index) const
        {
            return elements[index];
        }

        X_TO_STRING
    };

    typedef eobject_ast_t<array_initializer_t *> array_initializer_ast_t;

    //-------- ---------- ---------- ---------- ----------

    typedef eobjects_t<expression_t *> array_lengths_t;
    typedef eobject_ast_t<array_lengths_t *> array_lengths_ast_t;

    //-------- ---------- ---------- ---------- ----------

    class new_array_expression_t : public expression_t
    {
        typedef expression_t __super_t;

    public:
        new_array_expression_t() = default;

        new_array_expression_t(array_lengths_t * lengths)
        {
            set_lengths(lengths);
        }

        type_name_t * type_name  = nullptr;
        array_initializer_t * initializer = nullptr;

        type_t * to_array_type(xpool_t & pool);

        array_lengths_t * lengths() const { return __lengths; }

        dimension_t dimension() const
        {
            return __lengths == nullptr? 0 : __lengths->size();
        }

        virtual expression_family_t this_family() const override
        {
            return expression_family_t::new_array;
        }

        virtual vtype_t get_vtype() const override;
        virtual const string_t to_string() const override;

        virtual expression_behaviour_t get_behaviour() const override
        {
            return expression_behaviour_t::new_;
        }

        void set_lengths(array_lengths_t * lengths);
        expression_t * length_at(dimension_t dimension) const;
        void set_length(dimension_t dimension, expression_t * length);

        virtual size_t expression_count() const override final
        {
            return __lengths == nullptr? 0 : __lengths->size();
        }

        virtual expression_t * expression_at(size_t index) const override final
        {
            return length_at(index);
        }

    private:
        array_lengths_t * __lengths = nullptr;
    };

    //-------- ---------- ---------- ---------- ----------

    class default_value_expression_t : public __simple_expression_base_t
    {
        typedef __simple_expression_base_t __super_t;
        
    public:
        default_value_expression_t() = default;
        default_value_expression_t(type_name_t * type_name)
            : type_name(type_name) { }

        type_name_t * type_name = nullptr;

        virtual expression_family_t this_family() const override
        {
            return expression_family_t::default_value;
        }

        virtual vtype_t get_vtype() const override;
        virtual type_t * get_type(xpool_t & xpool) const override;
        virtual const string_t to_string() const override;

        virtual cvalue_t execute(expression_execute_context_t & ctx) override;
    };

    //-------- ---------- ---------- ---------- ----------

    class type_of_expression_t : public __simple_expression_base_t
    {
        typedef __simple_expression_base_t __super_t;

    public:
        type_name_t * type_name = nullptr;

        virtual expression_family_t this_family() const override
        {
            return expression_family_t::type_of;
        }

        virtual vtype_t get_vtype() const override;
        virtual type_t * get_type(xpool_t & xpool) const override;
        virtual const string_t to_string() const override;

        virtual cvalue_t execute(expression_execute_context_t & ctx) override;
    };

    //-------- ---------- ---------- ---------- ----------

    class this_expression_t : public __simple_expression_base_t
    {
        typedef __simple_expression_base_t __super_t;

    public:
        virtual expression_family_t this_family() const override
        {
            return expression_family_t::this_;
        }

        virtual vtype_t get_vtype() const override;
        virtual type_t * get_type(xpool_t & xpool) const override;
        virtual const string_t to_string() const override;

        virtual cvalue_t execute(expression_execute_context_t & ctx) override;

        void set_type(type_t * type) { __type = type; }

    private:
        type_t * __type = nullptr;
    };

    //-------- ---------- ---------- ---------- ----------

    class base_expression_t : public __simple_expression_base_t
    {
        typedef __simple_expression_base_t __super_t;

    public:
        virtual expression_family_t this_family() const override
        {
            return expression_family_t::base;
        }

        virtual vtype_t get_vtype() const override;
        virtual type_t * get_type(xpool_t & xpool) const override;
        virtual const string_t to_string() const override;

        virtual cvalue_t execute(expression_execute_context_t & ctx) override;
        void set_type(type_t * type) { __type = type; }

    private:
        type_t * __type = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////

    type_t * get_expression_type(xpool_t & xpool, expression_t * expression);

    //-------- ---------- ---------- ---------- ----------

    template<typename exp_t, typename ... args_t>
    X_INLINE exp_t * __new_exp(memory_t * memory, args_t ... args)
    {
        return memory_t::new_obj<exp_t>(memory, args ...);
    }

    //-------- ---------- ---------- ---------- ----------

    X_INLINE cvalue_expression_t * to_exp(memory_t * memory, cvalue_t * value)
    {
        return memory_t::new_obj<cvalue_expression_t>(memory, value);
    }

    //-------- ---------- ---------- ---------- ----------

    X_INLINE name_expression_t * to_exp(memory_t * memory, const name_t & name)
    {
        return __new_exp<name_expression_t>(memory, name);
    }

    //-------- ---------- ---------- ---------- ----------

    X_INLINE unitary_expression_t * to_exp(memory_t * memory,
                    const operator_property_t * property, expression_t * exp)
    {
        return __new_exp<unitary_expression_t>(memory, property, exp);
    }

    //-------- ---------- ---------- ---------- ----------

    X_INLINE binary_expression_t * to_exp(memory_t * memory,
        const operator_property_t * property, expression_t * exp1, expression_t * exp2)
    {
        return __new_exp<binary_expression_t>(memory, property, exp1, exp2);
    }

    //-------- ---------- ---------- ---------- ----------

    template<typename ... exps_t>
    X_INLINE op_expression_t<sizeof ... (exps_t)> * to_exp(memory_t * memory,
                    const operator_property_t * property, exps_t && ... exps)
    {
        return __new_exp<op_expression_t<sizeof ... (exps_t)>>(
            memory, property, std::forward<exps_t>(exps) ...
        );
    }

    //-------- ---------- ---------- ---------- ----------

    template<typename ... exps_t>
    X_INLINE texpression_t<sizeof ... (exps_t)> * to_exp(memory_t * memory, exps_t && ... exps)
    {
        return __new_exp<texpression_t<sizeof ... (exps_t)>>(
            memory, std::forward<exps_t>(exps) ...
        );
    }

    //-------- ---------- ---------- ---------- ----------

    template<typename callback_t>
    X_INLINE void each_expression(expression_t * expression, callback_t callback)
    {
        if(expression == nullptr)
            return;

        for(size_t index = 0, count = expression->expression_count(); index < count; index++)
        {
            callback(expression->expression_at(index));
        }
    }

    ////////// ////////// ////////// ////////// //////////

    X_ENUM(statement_exit_point_type_t)

        self,

        break_,

        continue_,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    class statement_compile_context_t;

    X_INTERFACE xilx_t : object_t
    {
        virtual void write(statement_compile_context_t & ctx, xil_pool_t & pool) = 0;

        virtual const string_t to_string() const override { return _T("xil"); }
    };

    //-------- ---------- ---------- ---------- ----------

    X_INTERFACE statement_point_t
    {
        virtual local_label_t to_position() = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    class statement_region_t : public object_t
    {
        typedef statement_exit_point_type_t __point_type_t;

    public:
        statement_region_t();
        statement_region_t * parent = nullptr;

        void append_xilx(xilx_t * xilx);

        virtual void set_point(__point_type_t point_type, statement_point_t * point);
        virtual statement_point_t * get_point(__point_type_t point_type);

        template<typename f_t> void each(f_t f)
        {
            al::for_each(__xilxes, f);
        }

    private:
        al::small_vector_t<xilx_t *, 10> __xilxes;
        statement_point_t * __points[(size_t)__point_type_t::__end__];
    };

    //-------- ---------- ---------- ---------- ----------

    class region_xilx_t : public xilx_t
    {
    public:
        region_xilx_t(statement_region_t * region) : __region(region)
        {
            _A(region != nullptr);
        }

        virtual void write(statement_compile_context_t & ctx, xil_pool_t & pool) override;

        virtual const string_t to_string() const override
        {
            return _F(_T("region xilx: %1%"), _str(__region));
        }

    private:
        statement_region_t * __region;
    };

    //-------- ---------- ---------- ---------- ----------

    class xil_buffer_t : public memory_base_t, public object_t
    {
    public:
        xil_buffer_t(memory_t * memory = nullptr)
            : memory_base_t(memory) { }

        template<typename t> void write(const t & v)
        {
            write((const byte_t *)&v, sizeof(t));
        }

        void write(const byte_t * bytes, size_t size);

        virtual ~xil_buffer_t() override;

        size_t read(byte_t * buffer, size_t size);

        size_t size() const { return __p - __buffer; }
        const byte_t * bytes() const { return __buffer; }

    private:
        byte_t * __buffer = nullptr, * __p = nullptr, * __p_read = nullptr;
        size_t   __size   = 0;

        void __ensure_size(size_t size);
    };

    //-------- ---------- ---------- ---------- ----------

    class xil_pool_t : public object_t, public no_copy_ctor_t
    {
        typedef xil_pool_t  __self_t;
        typedef object_t    __super_t;

        template<typename _xil_t> struct __xil_wrapper_t
        {
            template<typename ... args_t>
            __xil_wrapper_t(args_t && ... args) : xil(std::forward<args_t>(args) ...) { }

            int     index;
            _xil_t  xil;
        };

        typedef al::small_vector_t<xil_t *, 128> __xils_t;
        typedef __xil_wrapper_t<byte_t> __v_xw_t;
        typedef arch_int_t __i_t;

        static int & __index_of(void * xw)
        {
            return *(int *)((byte_t *)xw + (__i_t)(&((__v_xw_t *)nullptr)->index));
        }

        static int & __index_of(xil_t * xil)
        {
            return __index_of((void *)((byte_t *)xil - (__i_t)(&((__v_xw_t *)nullptr)->xil)));
        }

    public:
        template<typename _xil_t, typename ... args_t>
        _xil_t * append(args_t && ... args)
        {
            typedef __xil_wrapper_t<_xil_t> xw_t;
            xw_t * x = memory_t::new_obj<xw_t>(&__heap, std::forward<args_t>(args) ...);

            _xil_t * xil = &x->xil;
            __xils.push_back(xil);

            return xil;
        }

        auto begin()  const { return __xils.begin(); }
        auto end()    const { return __xils.end();   }

        bool empty()  const { return __xils.empty(); }
        size_t size() const { return __xils.size();  }

        xil_t * operator[](size_t index) { return __xils[index]; }

        void commit();

        int index_of(xil_t * xil);

    private:
        xheap_t     __heap;
        __xils_t    __xils;
    };

    //-------- ---------- ---------- ---------- ----------

    class jmp_manager_t : public object_t, public no_copy_ctor_t
    {
    public:
        jmp_manager_t() { }

        void append_jmp(jmp_xil_t * xil, local_label_t label);
        void append_jmp(jmp_xil_t * xil, name_t name);

        void set_point(local_label_t label, xil_t * xil);
        void set_point(name_t name, xil_t * xil);

        // Return xil of label, nullptr when not found
        xil_t * label_xil(local_label_t label);

        void commit(statement_compile_context_t & ctx);

    private:
        std::map<jmp_xil_t *, local_label_t> __local_jmp_map;
        std::map<jmp_xil_t *, name_t> __global_jmp_map;

        std::map<local_label_t, xil_t *> __local_point_map;
        std::map<name_t, xil_t *> __global_point_map;
    };

    //-------- ---------- ---------- ---------- ----------

    class statement_compile_context_t : public object_t
    {
        typedef statement_exit_point_type_t __point_type_t;

    public:
        statement_compile_context_t(method_compile_context_t & mctx, method_t * method);

        method_compile_context_t & mctx;

        method_t *          method;
        jmp_manager_t       jmp_manager;
        switch_manager_t    switch_manager;

        template<typename _region_t, typename ... args_t>
        _region_t * begin_region(args_t && ... args)
        {
            _region_t * region = __new_region<_region_t>(std::forward<args_t>(args) ...);

            if(region->parent != nullptr)
                region->parent->append_xilx(new_obj<region_xilx_t>(region));

            return region;
        }

        statement_region_t * end_region();

        xpool_t & xpool() { return mctx.xpool; }
        __assembly_layout_t & assembly_layout() { return mctx.layout; }
        xil_pool_t & xil_pool() { return mctx.xil_pool; }

        template<typename t, typename ... args_t>
        t * new_global_obj(args_t && ... args)
        {
            return memory_t::new_obj<t>(xpool().memory, std::forward<args_t>(args) ...);
        }

        template<typename t, typename ... args_t>
        t * new_obj(args_t && ... args)
        {
            return memory_t::new_obj<t>(&__xheap, std::forward<args_t>(args) ...);
        }

        statement_region_t * current_region() { return __current_region; }

        template<typename _xilx_t, typename ... args_t>
        _xilx_t * append_xilx(args_t && ... args)
        {
            _xilx_t * xilx = new_obj<_xilx_t>(std::forward<args_t>(args) ...);
            current_region()->append_xilx(xilx);

            return xilx;
        }

        name_t to_name(const string_t & name)
        {
            return name_t(xpool().spool.to_sid(name));
        }

        assembly_t * get_assembly();

        local_label_t next_local_label()
        {
            return ++__current_local_label;
        }

        void set_point(__point_type_t point_type, statement_point_t * point);

        memory_t * get_memory() { return &__xheap; }

    private:
        xheap_t                 __xheap;
        statement_region_t *    __current_region = nullptr;
        assembly_t *            __assembly       = nullptr;
        local_label_t           __current_local_label = 0;

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
