
#ifndef __COMPILE_H__
#define __COMPILE_H__

#include <core.h>
#include <algorithm.h>
#include <lib.h>


namespace X_ROOT_NS { namespace modules { namespace compile {

    namespace
    {
        using namespace core;
        using namespace std::placeholders;
    }

    typedef uint32_t lang_id_t;
    const lang_id_t unknown_lang_id = 0;

    ////////// ////////// ////////// ////////// //////////

    class analyze_node_t;

    typedef int32_t token_index_t;

    struct token_tag_t : compare_operators_t<token_tag_t, token_index_t>
    {
        token_tag_t(const token_tag_t & tag) : index(tag.index) { }
        explicit token_tag_t(int32_t index) : index(index) { }

        const token_index_t index;

        token_index_t operator - (const token_tag_t & tag) const
        {
            return index - tag.index;
        }

        operator token_index_t() const { return index; }
        operator string_t() const { return sprintf(_T("%1%"), index); }
    };

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const token_tag_t & tag)
    {
        return stream << tag.index;
    }

    ////////// ////////// ////////// ////////// //////////

    X_ENUM(compile_error_code_t)

        unknown_lang,

        empty_lang,

        format_error,

        service_not_supported,

        assembly_circular_reference,

        assembly_not_found,

        assign_expression_type_error,

        unexpected_jmp,

        jmp_point_not_found,

        expession_missing,

        duplicate_case,

        expected_constant_value,

        expected_constant_value_type,

        type_missing,

        unexpected_member,

        unknown_field,

        unknown_field_type,

        expected_instance_method,

        unknown_method,

        unknown_property,

        property_cannot_be_read,

        property_cannot_be_write,

        host_type_missing,

        constructor_missing,

        unexpected_constructor_prototype__should_no_return_type,

        unexpected_constructor_prototype__should_no_virtual,

        unexpected_constructor_prototype__should_no_static,

        unexpected_host_type,

        no_default_value,

        unknown_array_length,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    X_INTERFACE token_enumerator_t
    {
        virtual token_t * next() = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    X_INTERFACE token_reader_t
    {
        virtual token_enumerator_t * read(const char_t * code, size_t length) = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    X_ENUM(code_object_metadata_key_t)

        path,

        title,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    class global_context_t;
    class compile_context_t;

    class code_object_t : public object_t
    {
    public:
        code_object_t(global_context_t & global_context)
            : global_context(global_context)
        { }

        typedef code_object_metadata_key_t mtkey_t;
        metadata_t<code_object_metadata_key_t, object_t *> metadata;

        global_context_t & global_context;
    };

    ////////// ////////// ////////// ////////// //////////

    class named_code_object_t : public code_object_t
    {
    public:
        template<typename name_t>
        named_code_object_t(global_context_t & global_context, name_t && name)
            : code_object_t(global_context), name(name)
        { }

        const string_t name;

        virtual const string_t to_string() const override
        {
            return name;
        }
    };

    ////////// ////////// ////////// ////////// //////////

    class code_section_t : public code_object_t
    {
    public:
        code_section_t(global_context_t & global_context,
            const char_t * code, size_t length, lang_id_t lang, int depth)
            : code_object_t(global_context)
            , code(code), length(length), lang(lang), depth(depth)
        { }

        const char_t * const code;
        const size_t length;
        const lang_id_t lang;
        const int depth;
    };

    X_INLINE ostream_t & operator << (ostream_t & stream, const code_section_t & section)
    {
        return stream.write(section.code, section.length), stream;
    }

    ////////// ////////// ////////// ////////// //////////

    class code_t : public code_object_t
    {
        typedef std::vector<code_section_t *> __sections_t;
        typedef typename __sections_t::iterator __itor_t;

    public:
        template<typename scode_t>
        code_t(global_context_t & global_context, scode_t code, lang_id_t lang)
            : code_object_t(global_context), code(code), lang(lang)
        { }

        const string_t code;
        const lang_id_t lang;

        range_t<__itor_t> sections();
        size_t section_count() const { return __sections.size(); }

        ast_node_t * build_ast(compile_context_t & context);

    private:
        __sections_t        __sections;
        pool_t              __pool;
    };

    ////////// ////////// ////////// ////////// //////////

    class file_t : public named_code_object_t
    {
    public:
        template<typename name_t>
        file_t(global_context_t & global_context, name_t name)
            : named_code_object_t(global_context, std::forward<name_t>(name))
        { }

        code_t * code;

        static file_t * load(global_context_t & global_context,
                memory_t * memory, const lib::path_t & path,
                const char_t * lang=nullptr, const char_t * name=nullptr);
    };

    ////////// ////////// ////////// ////////// //////////

    class project_t : public named_code_object_t
    {
    public:
        using named_code_object_t::named_code_object_t;

        void append_file(file_t * file);
        file_t * get_file(const string_t & name);

        auto files() { return _range(__files); }
        size_t file_count() const { return __files.size(); }

    private:
        std::map<string_t, file_t *> __files;
    };

    ////////// ////////// ////////// ////////// //////////

    class ref_assembly_t : public named_code_object_t, public xistream_t
    {
        typedef named_code_object_t __super_t;

    public:
        template<typename _name_t, typename _package_t>
        ref_assembly_t(global_context_t & global_context, _name_t && name, _package_t && package)
            : __super_t(global_context, std::forward<_name_t>(name))
            , package(package)
        { }

        const string_t package;

        // virtual size_t read(byte_t * buffer, size_t size) override;

        X_TO_STRING
    };

    //-------- ---------- ---------- ---------- ----------

    class ref_assemblies_t : public object_t
    {
        typedef object_t __super_t;

    public:
        void append_ref_assembly(ref_assembly_t * ref_assembly);
        ref_assembly_t * get_ref_assembly(const string_t & package, const string_t & name);

        auto ref_assemblies() { return _range(__ref_assemblies); }
        size_t ref_assembly_count() const { return __ref_assemblies.size(); }

    private:
        typedef std::tuple<string_t, string_t> __ref_assembly_key_t;
        std::map<__ref_assembly_key_t, ref_assembly_t *> __ref_assemblies;
    };

    //-------- ---------- ---------- ---------- ----------

    class file_ref_assembly_t : public ref_assembly_t
    {
        typedef ref_assembly_t __super_t;

    public:
        template<typename _name_t, typename _package_t>
        file_ref_assembly_t(global_context_t & global_context, _name_t && name,
                _package_t && package, const lib::path_t & path)
            : __super_t(global_context, std::forward<_name_t>(name),
                                        std::forward<_package_t>(package))
            , __path(path)
        { }

        virtual size_t read(byte_t * buffer, size_t size) override;
        virtual void completed() override;

    private:
        const lib::path_t __path;
        bool __opened = false;
        file_xistream_t __file_stream;

        void __ensure_open();
    };

    ////////// ////////// ////////// ////////// //////////

    class solution_t : public named_code_object_t
    {
    public:
        using named_code_object_t::named_code_object_t;

        void append_project(project_t * project);
        project_t * get_project(const string_t & name);

        auto projects() { return _range(__projects); }
        size_t project_count() const { return __projects.size(); }

        ref_assemblies_t ref_assemblies;

    private:
        std::map<string_t, project_t *> __projects;
    };

    ////////// ////////// ////////// ////////// //////////

    X_ENUM(lang_service_type_t)

        token_read,

        token_property,

        operator_property,

        expression_build,

        statement_analyze,

        ast_build,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    X_INTERFACE lang_service_t
    {
        virtual lang_service_type_t get_service_type() = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    template<lang_service_type_t _service_type>
    X_INTERFACE tlang_service_t : public lang_service_t
    {
        virtual lang_service_type_t get_service_type() override
        {
            return _service_type;
        }

        static const lang_service_type_t service_type;
    };

    template<lang_service_type_t _service_type>
    const lang_service_type_t tlang_service_t<_service_type>::service_type = _service_type;

    ////////// ////////// ////////// ////////// //////////

    class token_reader_context_t
    {
    public:
        token_reader_context_t(compile_context_t & context)
            : compile_context(context)
        { }

        compile_context_t & compile_context;

        template<typename t, typename ... args_t>
        t * new_obj(args_t && ... args)
        {
            return memory_t::new_obj<t>(&__heap, std::forward<args_t>(args) ...);
        }

    private:
        al::xheap_t __heap;
    };

    //-------- ---------- ---------- ---------- ----------

    X_INTERFACE lang_token_read_service_t
        : public tlang_service_t<lang_service_type_t::token_read>
    {
        virtual token_reader_t * create_token_reader(token_reader_context_t & context) = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    X_INTERFACE lang_token_property_service_t
        : public tlang_service_t<lang_service_type_t::token_property>
    {
        virtual const token_property_t * get_token_property(token_value_t token_value) = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    X_INTERFACE lang_operator_property_service_t
        : public tlang_service_t<lang_service_type_t::operator_property>
    {
        virtual const operator_property_t * get_operator_property(token_value_t token_value) = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    class lang_expression_build_args_t
    {
    public:
        lang_expression_build_args_t(token_t * token, const operator_property_t * property,
            expression_t ** expressions, size_t count)
            : token(token), expressions(expressions), count(count), property(property)
        { }

        expression_t ** expressions;
        size_t count;

        const operator_property_t * property;
        token_t * token;
    };

    //-------- ---------- ---------- ---------- ----------

    template<typename _expression_t> class system_expression_t;

    class lang_expression_build_context_t
    {
    public:
        lang_expression_build_context_t(memory_t * memory)
            : memory(memory)
        { }

        memory_t * const memory;

        template<typename exp_t, typename ... args_t>
        exp_t * create_expression(args_t ... args)
        {
            return memory_t::new_obj<exp_t>(memory, std::forward<args_t>(args) ...);
        }

        template<typename exp_t, typename ... args_t>
        exp_t * create_system_expression(args_t ... args)
        {
            typedef system_expression_t<exp_t> sys_exp_t;
            return create_expression<sys_exp_t>(std::forward<args_t>(args) ...);
        }

        virtual expression_t * create_default_expression(
                                        const lang_expression_build_args_t & args) = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    X_INTERFACE lang_expression_build_service_t
        : public tlang_service_t<lang_service_type_t::expression_build>
    {
        virtual const expression_box_property_t * get_expression_box_property(
                                                                token_value_t value) = 0;

        virtual expression_t * build_expression(lang_expression_build_context_t & ctx,
                                            const lang_expression_build_args_t & args) = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    X_ENUM(analyze_node_type_t)
        
        empty, token, branch, branch_ref, end,

    X_ENUM_END

    typedef element_value_t analyze_node_value_t;

    //-------- ---------- ---------- ---------- ----------

    X_INTERFACE lang_statement_analyze_service_t
        : public tlang_service_t<lang_service_type_t::statement_analyze>
    {
        virtual const string_t get_analyze_pattern(global_context_t & context) = 0;

        virtual analyze_node_value_t get_node_value(global_context_t & context,
                         const string_t & name, analyze_node_type_t node_type) = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    struct analyzer_element_t;

    template<typename __tag_t = token_tag_t>
    class lang_ast_build_elements_t;

    //-------- ---------- ---------- ---------- ----------

    class lang_ast_build_args_t
    {
        typedef lang_ast_build_elements_t<> __elements_t;

    public:
        lang_ast_build_args_t(analyze_node_value_t node_value, __elements_t & elements)
            : node_value(node_value), elements(elements)
        { }

        const analyze_node_value_t node_value;
        const __elements_t & elements;
    };

    //-------- ---------- ---------- ---------- ----------

    class ast_context_t;

    X_INTERFACE lang_ast_build_service_t
        : public tlang_service_t<lang_service_type_t::ast_build>
    {
        virtual ast_node_t * build_ast(ast_context_t & context, lang_ast_build_args_t & args) = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    X_INTERFACE lang_t
    {
        virtual lang_service_t * get_service(lang_service_type_t service_type) = 0;
        virtual lang_id_t get_id() = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    struct lang_create_args_t;

    X_INTERFACE lang_factory_t
    {
        virtual lang_id_t get_lang_id(const string_t & name) = 0;
        virtual const string_t get_lang_name(lang_id_t lang_id) = 0;
        virtual lang_t * create_lang(memory_t * memory, const lang_create_args_t & args) = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    template<typename _key_t, typename _value_t, int _global_cache_key>
    class lang_cache_t : public object_t, public no_copy_ctor_t
                       , public kv_object_t<_key_t, _value_t>
    {
    public:
        typedef _key_t      key_t;
        typedef _value_t    value_t;

        lang_cache_t() = default;

        value_t get(const key_t & key, std::function<value_t ()> load_func)
        {
            return al::map_get(__cache, key, load_func);    // TODO: thread safe
        }

        virtual value_t get(const key_t & key) override
        {
            auto it = __cache.find(key);
            if(it != __cache.end())
                return it->second;

            return _D(value_t);
        }

        virtual void set(const key_t & key, value_t & value) override
        {
            __cache[key] = value;
        }

        static const int __cache_key__ = _global_cache_key;

    private:
        std::map<_key_t, _value_t> __cache;
    };

    ////////// ////////// ////////// ////////// //////////

    enum __global_cache_key_t
    {
        __global_cache_key_mname,
        __global_cache_key_generic_type,
        __global_cache_key_array_type,
        __global_cache_key_assembly_reference,
    };

    typedef lang_cache_t<sid_t, const mname_t *, __global_cache_key_mname> mname_cache_t;
    typedef lang_cache_t<
        generic_type_cache_key_t, generic_type_t *, __global_cache_key_generic_type
    > generic_type_cache_t;

    typedef lang_cache_t<
        array_type_cache_key_t, array_type_t *, __global_cache_key_array_type
    > array_type_cache_t;

    typedef lang_cache_t<
        assembly_reference_cache_key_t, assembly_reference_t *,
        __global_cache_key_assembly_reference
    > assembly_reference_cache_t;

    ////////// ////////// ////////// ////////// //////////

    class analyzer_t;

    class global_context_t : public object_t, public no_copy_ctor_t
    {
    public:
        global_context_t(lang_factory_t * lang_factory);

        lang_t * lang_of(lang_id_t lang_id);
        lang_id_t lang_id_of(const string_t & lang_name);

        memory_t * get_memory() { return &__heap; }

        template<typename t, typename ... args_t>
        t * new_obj(args_t && ... args)
        {
            return memory_t::new_obj<t>(&__heap, std::forward<args_t>(args) ...);
        }

        lang_factory_t * const lang_factory;

        void * from_cache(lang_id_t lang_id, int32_t key, std::function<void *()> load_func)
        {
            // TODO: thread safe
            return al::map_get(__cache, __cache_key_t(lang_id, key), load_func);
        }

        template<typename t>
        t * from_cache(lang_id_t lang_id, std::function<void *()> load_func)
        {
            return (t *)from_cache(lang_id, (int32_t)t::__cache_key__, load_func);
        }

        template<typename t>
        t * from_cache(lang_id_t lang_id = 0)
        {
            return from_cache<t>(lang_id, [this]() { return this->new_obj<t>(); });
        }

        template<typename cache_t, typename load_func_t,
            typename key_t   = typename cache_t::key_t,
            typename value_t = typename cache_t::value_t
        >
        value_t cache_of(const key_t & key, load_func_t load_func)
        {
            return from_cache<cache_t>().get(key, std::function<value_t ()>(load_func));
        }

        xpool_t xpool;

    private:
        std::map<lang_id_t, lang_t *> __lang_map;

        typedef std::tuple<lang_id_t, int32_t> __cache_key_t;
        std::map<__cache_key_t, void *> __cache;

        al::xheap_t __heap;
    };

    ////////// ////////// ////////// ////////// //////////

    typedef uint16_t compile_log_code_value_t;

    X_ENUM_(compile_log_code_t, sizeof(compile_log_code_value_t))

        write_assembly_error,

        no_assembly_name,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    class compile_log_t : public object_t
    {
        typedef log_level_t __level_t;

    public:
        compile_log_t(lang_t * lang, code_element_t * element, __level_t level,
                            const string_t & name, const string_t & message)
            : lang(lang), level(level), name(name), message(message), element(element)
        { }

        lang_t *        lang;
        __level_t       level;
        string_t        name;
        string_t        message;
        code_element_t * element;
    };

    //-------- ---------- ---------- ---------- ----------

    class compile_logs_t : public object_t
    {
    public:
        void append(const compile_log_t * log)
        {
            __logs.push_back(log);
        }

    private:
        std::vector<const compile_log_t *> __logs;
    };

    ////////// ////////// ////////// ////////// //////////

    class assembly_writer_t;

    class compile_context_t : public object_t, public no_copy_ctor_t
    {
        typedef log_level_t                 __log_level_t;
        typedef compile_log_code_value_t    __log_code_t;

    public:
        compile_context_t(global_context_t & global_context, assembly_writer_t * writer)
            : global_context(global_context), __writer(writer)
        { }

        template<typename obj_t, typename ... args_t>
        obj_t * new_obj(args_t && ... args)
        {
            return memory_t::new_obj<obj_t>(&__heap, std::forward<args_t>(args) ...);
        }

        memory_t * get_memory() { return &__heap; }

        global_context_t & global_context;
        compile_logs_t logs;

        template<typename code_t>
        void log_error(lang_t * lang, code_element_t * element, code_t code,
                                                            const string_t & message)
        {
            log(lang, element, __log_level_t::error, _title(code), message);
        }

        template<typename code_t>
        void log_warning(lang_t * lang, code_element_t * element, code_t code,
                                                            const string_t & message)
        {
            log(lang, element, __log_level_t::warning, _title(code), message);
        }

        template<typename code_t>
        void log_info(lang_t * lang, code_element_t * element, code_t code,
                                                            const string_t & message)
        {
            log(lang, element, __log_level_t::info, _title(code), message);
        }

        void log(lang_t * lang, code_element_t * element, __log_level_t level,
                                const string_t & name, const string_t & message)
        {
            logs.append(new_obj<compile_log_t>(lang, element, level, name, message));

            if(level == __log_level_t::error)
                __error_count++;
        }

        template<typename code_t, typename ... args_t>
        void log(__log_level_t level, code_t code, args_t && ... args)
        {
            return log((lang_t *)nullptr, (code_element_t *)nullptr, level,
                _title(code), _F(_desc(code), std::forward<args_t>(args) ...)
            );
        }

        size_t error_count() const
        {
            return __error_count;
        }

        xostream_t * create_output_stream(ast_project_t * project);

    private:
        lang_factory_t * __lang_factory;
        al::xheap_t __heap;
        size_t __error_count = 0;
        assembly_writer_t * __writer;
    };

    ////////// ////////// ////////// ////////// //////////

    class compile_logger_t : public logger_t, public xlogger_t
    {
        typedef compile_log_code_value_t    __log_code_t;

    public:
        compile_logger_t(compile_context_t & compile_context, lang_t * lang = nullptr)
            : xlogger_t(*(logger_t *)this), __compile_context(compile_context), __lang(lang)
        { }

        virtual void log(code_element_t * element, log_level_t level,
                            const string_t & name, const string_t & message) override
        {
            __compile_context.log(__lang, element, level, name, message);
        }

        size_t error_count() const
        {
            return __compile_context.error_count();
        }

    private:
        compile_context_t & __compile_context;
        lang_t * __lang;
    };

    ////////// ////////// ////////// ////////// //////////

    class console_logger_t : public logger_t
    {
    public:
        virtual void log(code_element_t * element, log_level_t level,
                            const string_t & name, const string_t & message) override
        {
            _PF(_T("[%1%] %2%:%3%"), level, name, message);
        }
    };

    ////////// ////////// ////////// ////////// //////////

    class multipy_logger_t : public loggers_t
    {
    public:
        multipy_logger_t(compile_context_t & compile_context, lang_t * lang = nullptr)
            : __compile_logger(compile_context, lang)
            , loggers_t({ &__compile_logger, &__console_logger })
        { }

    private:
        compile_logger_t __compile_logger;
        console_logger_t __console_logger;
    };

    ////////// ////////// ////////// ////////// //////////

    class ast_context_t : public object_t, public no_copy_ctor_t
    {
    public:
        ast_context_t(compile_context_t & compile_context, lang_t * lang)
            : compile_context(compile_context), lang(lang)
            , __logger(compile_context, lang), logger(__logger)
        {
            _A(lang != nullptr);
        }

        compile_context_t & compile_context;
        lang_t * const lang;
        logger_t & logger;

    private:
        multipy_logger_t __logger;
    };

    ////////// ////////// ////////// ////////// //////////

    X_INTERFACE assembly_writer_t
    {
        virtual xostream_t * new_stream(compile_context_t & ctx, ast_project_t * cproject) = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    class file_assembly_writer_t : public object_t, public assembly_writer_t
    {
        typedef assembly_writer_t __super_t;

    public:
        file_assembly_writer_t(const string_t & directory)
            : __directory(directory)
        {
            _A(directory.length() > 0);
        }

        virtual xostream_t * new_stream(compile_context_t & ctx, ast_project_t * cproject) override;

    private:
        string_t __directory;
    };

    ////////// ////////// ////////// ////////// //////////

    class compiler_t : public object_t
    {
    public:
        compiler_t(lang_factory_t * lang_factory) : __global_context(lang_factory)
        {
            _A(lang_factory != nullptr);
        }

        ast_solution_t * build_ast(compile_context_t & context, solution_t * solution);
        assemblies_t * compile(compile_context_t & context, ast_solution_t * csolution,
                                            ref_assemblies_t * ref_assembilies = nullptr);
        assemblies_t * compile(memory_t * memory, solution_t * solution,
                                                assembly_writer_t * writer = nullptr);
        assemblies_t * compile(memory_t * memory, solution_t * solution,
                                                const string_t & output_path);

        global_context_t & get_context() { return __global_context; }
        compile_context_t * new_compile_context(memory_t * memory, assembly_writer_t * writer);

    private:
        global_context_t __global_context;
        typedef pw_t<assemblies_t> __assemblies_t;
        typedef log_level_t         __l_t;
        typedef compile_log_code_t  __c_t;

        class __assembly_loader_t;
        class __assembly_t;
        class __building_context_t;

        ast_project_t * __build_project(compile_context_t & context, project_t * project);
        ast_file_t    * __build_file(compile_context_t & context, file_t * file);

        __assembly_t * __compile_project(__building_context_t & bctx, compile_context_t & context,
                ast_project_t * cproject, assembly_loader_t * assembly_loader);
    };

    ////////// ////////// ////////// ////////// //////////

    struct lang_create_args_t
    {
        lang_id_t lang_id;
    };

    typedef creator_t<lang_t, lang_create_args_t> lang_creator_t;

    //-------- ---------- ---------- ---------- ----------

    namespace
    {
        template<typename _lang_t>
        class __simple_creator_t : public lang_creator_t
        {
        public:
            virtual lang_t * create(memory_t * memory, const lang_create_args_t & args) override
            {
                _A(memory != nullptr);
                return memory_t::new_obj<_lang_t>(memory, args);
            }

            virtual void destory(memory_t * memory, lang_t * obj) override { }
        };
    }

    //-------- ---------- ---------- ---------- ----------

    class simple_lang_factory_t : public object_t, public lang_factory_t
    {
    public:
        virtual lang_t * create_lang(memory_t * memory, const lang_create_args_t & args) override;
        virtual lang_id_t get_lang_id(const string_t & name) override;
        virtual const string_t get_lang_name(lang_id_t lang_id) override;

        lang_id_t register_lang(const char_t * name, lang_creator_t * creator);

        template<typename _lang_t>
        lang_id_t register_lang_t(const char_t * name)
        {
            return register_lang(name, __pool.new_obj<__simple_creator_t<_lang_t>>());
        }

        template<typename _lang_t> lang_id_t register_lang_t()
        {
            return register_lang_t<_lang_t>(_lang_t::__default_name__);
        }

    private:
        std::map<lang_id_t, lang_creator_t *> __creators;
        std::map<lang_id_t, string_t> __id_name_map;
        std::map<string_t, lang_id_t> __name_id_map;

        std::mutex __lock;
        pool_t __pool;

        lang_id_t __next_lang_id_value = 1;

        lang_id_t __next_lang_id()
        {
            return __next_lang_id_value++;
        }
    };

    ////////// ////////// ////////// ////////// //////////

    template<typename _lang_service_t>
    _lang_service_t * get_lang_service(lang_t * lang)
    {
        _A(lang != nullptr);
        return (_lang_service_t *)lang->get_service(_lang_service_t::service_type);
    }

    template<typename _lang_service_t>
    _lang_service_t * require_lang_service(lang_t * lang)
    {
        _lang_service_t * service = get_lang_service<_lang_service_t>(lang);
        if(service == nullptr)
        {
            throw _E(compile_error_code_t::service_not_supported,
                    sprintf(_T("service \"%1%\" not supported"), _lang_service_t::service_type));
        }

        return service;
    }

    ////////// ////////// ////////// ////////// //////////

    class lang_service_helper_t
    {
    public:
        lang_service_helper_t(lang_t * lang)
            : lang(lang)
            , __service_map(std::bind(&lang_t::get_service, lang, std::placeholders::_1))
        {
            _A(lang != nullptr);
        }

        lang_t * const lang;

        template<typename _lang_service_t>
        _lang_service_t * get_service()
        {
            lang_service_type_t service_type = _lang_service_t::service_type;
            return (_lang_service_t *)__service_map[service_type];
        }

        template<typename _lang_service_t>
        _lang_service_t * require_service()
        {
            _lang_service_t * service = get_service<_lang_service_t>();
            if(service == nullptr)
            {
                throw _E(compile_error_code_t::service_not_supported,
                    sprintf(_T("service \"%1%\" not supported"), _lang_service_t::service_type)
                );
            }

            return service;
        }

        // token read service
        token_reader_t * create_token_reader(token_reader_context_t & context);

        // token property service
        const token_property_t * get_token_property(token_value_t value);

        // operator property service
        const operator_property_t * get_operator_property(token_value_t value);

        // expression build service
        const expression_box_property_t * get_expression_box_property(token_value_t value);
        expression_t * build_expression(lang_expression_build_context_t & ctx,
                                    const lang_expression_build_args_t & args);

        // statement analyze service
        const string_t get_analyze_pattern(global_context_t & context);

        analyze_node_value_t get_node_value(global_context_t & context,
                            const string_t & name, analyze_node_type_t node_type);

        // ast build service
        ast_node_t * build_ast(ast_context_t & context,
                                                    lang_ast_build_args_t & args);

    private:
        al::auto_cache_t<lang_service_type_t, lang_service_t *> __service_map;
    };

    ////////// ////////// ////////// ////////// //////////

    template<typename key_t, typename property_t>
    class property_cache_t : public object_t
    {
        typedef al::auto_cache_t<key_t, const property_t *> __cache_t;

    public:
        template<typename loader_t>
        property_cache_t(loader_t loader) : __cache(loader) { }

        const property_t * try_get_property(const key_t & key)
        {
            return __cache[key];
        }

        const property_t * get_property(const key_t & key)
        {
            const property_t * property = try_get_property(key);
            if(property == nullptr)
                throw _ECF(not_found, _T("connot find property of %1%"), key);

            return property;
        }

    private:
        __cache_t __cache;
    };

    //-------- ---------- ---------- ---------- ----------

    template<typename property_t>
    class __token_property_cache_t : public property_cache_t<token_value_t, property_t>
    {
    public:
        template<typename loader_t>
        __token_property_cache_t(lang_service_helper_t * service_helper, loader_t loader)
            : property_cache_t<token_value_t, property_t>(std::bind(loader, service_helper, _1))
            , __service_helper(service_helper)
        {
            _A(service_helper != nullptr);
        }

    private:
        lang_service_helper_t * __service_helper;
    };

    //-------- ---------- ---------- ---------- ----------

    template<typename property_t> class token_property_cache_t { };

    //-------- ---------- ---------- ---------- ----------

    #define __DefaultTokenPropertyCache(property_t, loader)                                 \
                                                                                            \
        template<>                                                                          \
        class token_property_cache_t<property_t>                                            \
            : public __token_property_cache_t<property_t>                                   \
        {                                                                                   \
        public:                                                                             \
            token_property_cache_t(lang_service_helper_t * service_helper)                  \
                : __token_property_cache_t<property_t>(                                     \
                    service_helper, &lang_service_helper_t::loader)                         \
            { }                                                                             \
        };

    __DefaultTokenPropertyCache(token_property_t, get_token_property);
    __DefaultTokenPropertyCache(operator_property_t, get_operator_property);
    __DefaultTokenPropertyCache(expression_box_property_t, get_expression_box_property);

    #undef __DefaultTokenPropertyCache

    ////////// ////////// ////////// ////////// //////////

} } }  // X_ROOT_NS::modules::compile

#include <modules/compile/__analyze_utils.h>
#include <modules/compile/__expression.h>
#include <modules/compile/__analyze_tree.h>
#include <modules/compile/__analyze_stack.h>
#include <modules/compile/__token_reader.h>
#include <modules/compile/__xilx.h>
#include <modules/compile/__statements.h>
#include <modules/compile/__ast_utils.h>
#include <modules/compile/__ast.h>
#include <modules/compile/__helper.h>

#endif  // __COMPILE_H__
