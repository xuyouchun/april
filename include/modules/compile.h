
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

    class code_t;
    class analyze_node_t;

    ////////// ////////// ////////// ////////// //////////

    typedef int32_t token_index_t;

    // Tokens tag.
    // Identity a token position.
    struct token_tag_t : compare_operators_t<token_tag_t, token_index_t>
    {
        // Constructors.
        token_tag_t(const token_tag_t & tag) : index(tag.index) { }
        explicit token_tag_t(int32_t index) : index(index) { }

        const token_index_t index;

        // Returns difference of two tags.
        token_index_t operator - (const token_tag_t & tag) const
        {
            return index - tag.index;
        }

        // Converts to token_index_t.
        operator token_index_t() const { return index; }

        // Converts to a string.
        operator string_t() const { return sprintf(_T("%1%"), index); }
    };

    // Writes a token tag to a string.
    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const token_tag_t & tag)
    {
        return stream << tag.index;
    }

    ////////// ////////// ////////// ////////// //////////

    // Compile error codes.
    X_ENUM(compile_error_code_t)

        // Unknown language.
        unknown_lang,

        // Empty languange.
        empty_lang,

        // Format error.
        format_error,

        // Service not supported.
        service_not_supported,

        // Assembly circular referenced.
        assembly_circular_reference,

        // Assembly not found.
        assembly_not_found,

        // Assigns expression with a error type.
        assign_expression_type_error,

        // Variable cannot determined.
        variable_cannot_determined,

        // Index body missing.
        index_body_missing,

        // Index argument missing.
        index_arguments_missing,

        // Index type undetermined.
        index_type_undetermined,

        // Unexpected jmp operation.
        unexpected_jmp,

        // Jmp point not found.
        jmp_point_not_found,

        // Expression missing.
        expession_missing,

        // Duplicate case in switch case statement.
        duplicate_case,

        // Expected a constant value.
        expected_constant_value,

        // Expected a constant value type.
        expected_constant_value_type,

        // Type missing.
        type_missing,

        // Type error.
        type_error,

        // Unexpected member.
        unexpected_member,

        // Unknown field.
        unknown_field,

        // Unknown field type.
        unknown_field_type,

        // Expected a instance method.
        expected_instance_method,

        // Unknown method.
        unknown_method,

        // Unknown property.
        unknown_property,

        // Property cannot be read.
        property_cannot_be_read,

        // Property cannot be write.
        property_cannot_be_write,

        // Host type missing.
        host_type_missing,

        // Constructor of method missing.
        constructor_missing,

        // Unexpected constructor prototype: should no return type.
        unexpected_constructor_prototype__should_no_return_type,

        // Unexpected constructor prototype: should not a virtual method.
        unexpected_constructor_prototype__should_no_virtual,

        // Unexpected constructor prototype: shoud not a static method.
        unexpected_constructor_prototype__should_no_static,

        // Unexpected host type.
        unexpected_host_type,

        // No default value.
        no_default_value,

        // Unknown array length.
        unknown_array_length,

        // Index main expression missing.
        index_main_expression_missing,

        // Element type of array undeterminded.
        element_type_undeterminded,

        // Index element init expression empty.
        element_init_expression_empty,

        // Cache exception variable undeterminded.
        catch_exception_variable_undeterminded,

        // Operator overload method prototype error: Param count error.
        operator_overload__wrong_param_count,

        // Operator overload method prototype error: Argument type error.
        operator_overload__wrong_argument_type,

        // Operator overload method prototype error: should be static.
        operator_overload__wrong_prototype,

        // Operator overload method prototype error: Should not be a generic type.
        operator_overload__no_generic,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    // Compile optimize codes.
    X_ENUM(compile_optimize_code_t)

        // Remove unreached codes.
        remove_unreached_codes,

        // Converts switch to if statement when case lables is too little.
        convert_switch_to_if_statement,

		// Auto determine constant variables.
		auto_determine_constant_variables,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    // Interface for token enumerator.
    X_INTERFACE token_enumerator_t
    {
        // Reads next token.
        virtual token_t * next() = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    // Interface for token reader.
    X_INTERFACE token_reader_t
    {
        // Returns a token enumerator.
        virtual token_enumerator_t * read(const char_t * code, size_t length,
                                          const code_file_t * file) = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    // Code object metadata keys.
    X_ENUM(code_object_metadata_key_t)

        path,       // Path

        title,      // Title.

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    class global_context_t;
    class compile_context_t;

    // Code object.
    class code_object_t : public object_t
    {
    public:

        // Constructor.
        code_object_t(global_context_t & global_context)
            : global_context(global_context)
        { }

        typedef code_object_metadata_key_t mtkey_t;
        metadata_t<code_object_metadata_key_t, object_t *> metadata;

        // Global context.
        global_context_t & global_context;
    };

    ////////// ////////// ////////// ////////// //////////

    // Code object with name.
    class named_code_object_t : public code_object_t
    {
    public
    :
        // Constructor.
        template<typename name_t>
        named_code_object_t(global_context_t & global_context, name_t && name)
            : code_object_t(global_context), name(name)
        { }

        string_t name;      // Name

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return name;
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // Code section.
    class code_section_t : public code_object_t
    {
    public:

        // Constructor
        code_section_t(global_context_t & global_context,
            const char_t * source_code, size_t length, lang_id_t lang, int depth, code_t * code)
            : code_object_t(global_context), code(code)
            , source_code(source_code), length(length), lang(lang), depth(depth)
        { }

        const char_t * const source_code;   // Source code
        const size_t length;                // Length
        const lang_id_t lang;               // Language.
        const int depth;                    // Depth.

        code_t * code;                      // Code

        // Returns code file.
        const code_file_t * file() const;
    };

    // Writes a code section to a stream.
    X_INLINE ostream_t & operator << (ostream_t & stream, const code_section_t & section)
    {
        if(section.source_code != nullptr)
            stream.write(section.source_code, section.length);

        return stream;
    }

    ////////// ////////// ////////// ////////// //////////

    // Code
    class code_t : public code_object_t
    {
        typedef std::vector<code_section_t *> __sections_t;
        typedef typename __sections_t::iterator __itor_t;

    public:

        // Constructors.
        template<typename _scode_t>
        code_t(global_context_t & global_context, _scode_t code, lang_id_t lang,
                const code_file_t * file)
            : code_object_t(global_context), code(code), lang(lang), file(file)
        { }

        const string_t      code;       // Code
        const lang_id_t     lang;       // Language
        const code_file_t * file;       // Code file.

        // Returns code sections.
        range_t<__itor_t> sections();

        // Returns code section count.
        size_t section_count() const { return __sections.size(); }

        // Build ast tree.
        ast_node_t * build_ast(compile_context_t & context);

        // Returns source code.
        const char_t * source_code() const { return code.c_str(); }

    private:
        __sections_t        __sections;
        pool_t              __pool;
    };

    ////////// ////////// ////////// ////////// //////////

    // File
    class file_t : public named_code_object_t
                 , public code_file_t
    {
    public:

        // Constructor.
        template<typename name_t>
        file_t(global_context_t & global_context, const string_t & path, name_t name)
            : named_code_object_t(global_context, std::forward<name_t>(name))
            , path(path)
        { }

        code_t * code;      // Code
        string_t path;      // Path

        // Loads file from specified path.
        static file_t * load(global_context_t & global_context,
                memory_t * memory, const lib::path_t & path,
                const char_t * lang = nullptr, const char_t * name = nullptr);

        // Returns source code.
        const char_t * source_code() const;

        // Source code.
        virtual const char_t * get_source_code() const override
        {
            return code? code->source_code() : nullptr;
        }

        // Name.
        virtual const string_t & get_file_name() const override { return name; }

        // Path.
        virtual const string_t & get_file_path() const override { return path; }
    };

    ////////// ////////// ////////// ////////// //////////

    // Project
    class project_t : public named_code_object_t
    {
    public:
        using named_code_object_t::named_code_object_t;

        // Append a file.
        void append_file(file_t * file);

        // Gets file by path.
        file_t * get_file(const string_t & path) const;

        // Returns all files.
        auto files() { return _range(__files); }

        // Returns files count.
        size_t file_count() const { return __files.size(); }

        // Returns file at specified index.
        file_t * operator[](size_t index) const { return at(index); }

        // Returns file at specified index.
        file_t * at(size_t index) const;

        // Returns whether a path is exists.
        bool exists(const string_t & path) const;

        // Returns whether file collection is empty.
        bool empty() const { return __files.empty(); }

        assembly_t * assembly = nullptr;
        string_t     assembly_path;

    private:
        std::map<string_t, file_t *> __file_map;
        std::vector<file_t *> __files;
    };

    ////////// ////////// ////////// ////////// //////////

    // A ref of assembly.
    class ref_assembly_t : public named_code_object_t, public xistream_t
    {
        typedef named_code_object_t __super_t;

    public:

        // Constructor.
        template<typename _name_t, typename _package_t>
        ref_assembly_t(global_context_t & global_context, _name_t && name, _package_t && package)
            : __super_t(global_context, std::forward<_name_t>(name))
            , package(package)
        { }

        const string_t package;

        // virtual size_t read(byte_t * buffer, size_t size) override;

        // Convert to string.
        X_TO_STRING
    };

    //-------- ---------- ---------- ---------- ----------

    // Refs of assemblies.
    class ref_assemblies_t : public object_t
    {
        typedef object_t __super_t;

    public:

        // Append ref of assembly.
        void append_ref_assembly(ref_assembly_t * ref_assembly);

        // Returns assembly ref of specified package and name.
        ref_assembly_t * get_ref_assembly(const string_t & package, const string_t & name);

        // Returns all assembly refs.
        auto ref_assemblies() { return _range(__ref_assemblies); }

        // Returns count of assembly refs.
        size_t ref_assembly_count() const { return __ref_assemblies.size(); }

    private:
        typedef std::tuple<string_t, string_t> __ref_assembly_key_t;
        std::map<__ref_assembly_key_t, ref_assembly_t *> __ref_assemblies;
    };

    //-------- ---------- ---------- ---------- ----------

    // Assembly ref of file.
    class file_ref_assembly_t : public ref_assembly_t
    {
        typedef ref_assembly_t __super_t;

    public:

        // Constructor.
        template<typename _name_t, typename _package_t>
        file_ref_assembly_t(global_context_t & global_context, _name_t && name,
                _package_t && package, const lib::path_t & path)
            : __super_t(global_context, std::forward<_name_t>(name),
                                        std::forward<_package_t>(package))
            , __path(path)
        { }

        // Read bytes of assembly.
        virtual size_t read(byte_t * buffer, size_t size) override;

        // Completed notify.
        virtual void completed() override;

    private:
        const lib::path_t __path;
        bool __opened = false;
        file_xistream_t __file_stream;

        // Ensure it's opened.
        void __ensure_open();
    };

    ////////// ////////// ////////// ////////// //////////

    // Solution.
    class solution_t : public named_code_object_t
    {
    public:
        using named_code_object_t::named_code_object_t;

        // Appends a project.
        void append_project(project_t * project);

        // Returns a project of a name.
        project_t * get_project(const string_t & name) const;

        // Returns a project at specified index.
        project_t * operator[](size_t index) const { return at(index); }

        // Returns a project at specified index.
        project_t * at(size_t index) const;

        // Returns all projects.
        auto projects() { return _range(__projects); }

        // Returns project count.
        size_t project_count() const { return __projects.size(); }

        // Returns whether the project collection is empty.
        bool empty() const { return project_count() == 0; }

        // Returns whether the project with speicified name exists.
        bool exists(const string_t & name) const { return get_project(name) != nullptr; }

        // Ref of assemblies.
        ref_assemblies_t ref_assemblies;

    private:
        std::map<string_t, project_t *> __project_map;
        std::vector<project_t *> __projects;
    };

    ////////// ////////// ////////// ////////// //////////

    // Language service types.
    X_ENUM(lang_service_type_t)

        // A service for read tokens.
        token_read,

        // A service for getting token properties.
        token_property,

        // A service for operator properties.
        operator_property,

        // A service for building expression.
        expression_build,

        // A service for analyzing statement.
        statement_analyze,

        // A service for building ast.
        ast_build,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    // Interfaces for language service.
    X_INTERFACE lang_service_t
    {
        // Returns service type.
        virtual lang_service_type_t get_service_type() = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    // Interface for language service with service type.
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

    // Context for reading token.
    class token_reader_context_t
    {
    public:

        // Constructor.
        token_reader_context_t(compile_context_t & context)
            : compile_context(context)
        { }

        // Compile context.
        compile_context_t & compile_context;

        // Creates a new object.
        template<typename t, typename ... args_t>
        t * new_obj(args_t && ... args)
        {
            return memory_t::new_obj<t>(&__heap, std::forward<args_t>(args) ...);
        }

    private:
        al::xheap_t __heap;
    };

    //-------- ---------- ---------- ---------- ----------

    // Interface for read token service.
    X_INTERFACE lang_token_read_service_t
        : public tlang_service_t<lang_service_type_t::token_read>
    {
        virtual token_reader_t * create_token_reader(token_reader_context_t & context) = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    // Interface for read token property.
    X_INTERFACE lang_token_property_service_t
        : public tlang_service_t<lang_service_type_t::token_property>
    {
        virtual const token_property_t * get_token_property(token_value_t token_value) = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    // Interface for read operator property.
    X_INTERFACE lang_operator_property_service_t
        : public tlang_service_t<lang_service_type_t::operator_property>
    {
        virtual const operator_property_t * get_operator_property(token_value_t token_value) = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    // Arguments for building expression.
    class lang_expression_build_args_t
    {
    public:

        // Constructor.
        lang_expression_build_args_t(token_t * token, const operator_property_t * property,
            expression_t ** expressions, size_t count)
            : token(token), expressions(expressions), count(count), property(property)
        { }

        // Expressions
        expression_t ** expressions;

        // Expression count.
        size_t count;

        // Operator property.
        const operator_property_t * property;

        // Tokens.
        token_t * token;
    };

    //-------- ---------- ---------- ---------- ----------

    template<typename _expression_t> class system_expression_t;

    // Context for building an expression.
    class lang_expression_build_context_t
    {
    public:

        // Constructor.
        lang_expression_build_context_t(memory_t * memory)
            : memory(memory)
        { }

        memory_t * const memory;

        // Creates an expression.
        template<typename exp_t, typename ... args_t>
        exp_t * create_expression(args_t ... args)
        {
            return memory_t::new_obj<exp_t>(memory, std::forward<args_t>(args) ...);
        }

        // Creates an system expression.
        template<typename exp_t, typename ... args_t>
        exp_t * create_system_expression(args_t ... args)
        {
            typedef system_expression_t<exp_t> sys_exp_t;
            return create_expression<sys_exp_t>(std::forward<args_t>(args) ...);
        }

        // Creates a default expression.
        virtual expression_t * create_default_expression(
                                        const lang_expression_build_args_t & args) = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    // Interface for building expression.
    X_INTERFACE lang_expression_build_service_t
        : public tlang_service_t<lang_service_type_t::expression_build>
    {
        virtual const expression_box_property_t * get_expression_box_property(
                                                                token_value_t value) = 0;

        virtual expression_t * build_expression(lang_expression_build_context_t & ctx,
                                            const lang_expression_build_args_t & args) = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    // Analyze node types.
    X_ENUM(analyze_node_type_t)

        // Empty node.
        empty,
        
        // Token node.
        token,
        
        // Branch node.
        branch,
        
        // Branch ref node.
        branch_ref,
        
        // End node.
        end,

    X_ENUM_END

    typedef element_value_t analyze_node_value_t;

    //-------- ---------- ---------- ---------- ----------

    // Interface for statement analyze service.
    X_INTERFACE lang_statement_analyze_service_t
        : public tlang_service_t<lang_service_type_t::statement_analyze>
    {
        // Returns analyze pattern.
        virtual const string_t get_analyze_pattern(global_context_t & context) = 0;

        // Returns node value of specified name.
        virtual analyze_node_value_t get_node_value(global_context_t & context,
                         const string_t & name, analyze_node_type_t node_type) = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    struct analyzer_element_t;

    template<typename __tag_t = token_tag_t>
    class lang_ast_build_elements_t;

    //-------- ---------- ---------- ---------- ----------

    // Arguments for building ast.
    class lang_ast_build_args_t
    {
        typedef lang_ast_build_elements_t<> __elements_t;

    public:

        // Constructor.
        lang_ast_build_args_t(analyze_node_value_t node_value, __elements_t & elements)
            : node_value(node_value), elements(elements)
        { }

        const analyze_node_value_t node_value;      // Node value
        const __elements_t & elements;              // Elements.
    };

    //-------- ---------- ---------- ---------- ----------

    class ast_context_t;

    // Interface for build ast.
    X_INTERFACE lang_ast_build_service_t
        : public tlang_service_t<lang_service_type_t::ast_build>
    {
        // Builds ast with specified arguments.
        virtual ast_node_t * build_ast(ast_context_t & context, lang_ast_build_args_t & args) = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    // Interface for language.
    X_INTERFACE lang_t
    {
        // Returns specified service.
        virtual lang_service_t * get_service(lang_service_type_t service_type) = 0;

        // Returns language id.
        virtual lang_id_t get_id() = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    struct lang_create_args_t;

    // Interface for language factory.
    X_INTERFACE lang_factory_t
    {
        // Returns language id of specified name.
        virtual lang_id_t get_lang_id(const string_t & name) = 0;

        // Returns language name of specified id.
        virtual const string_t get_lang_name(lang_id_t lang_id) = 0;

        // Creates a language with specified arguments.
        virtual lang_t * create_lang(memory_t * memory, const lang_create_args_t & args) = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    // Lanaguage cache.
    template<typename _key_t, typename _value_t, int _global_cache_key>
    class lang_cache_t : public object_t, public no_copy_ctor_t
                       , public kv_object_t<_key_t, _value_t>
    {
    public:
        typedef _key_t      key_t;
        typedef _value_t    value_t;

        lang_cache_t() = default;

        // Gets value by specified key, auto creates it with specified creator.
        value_t get(const key_t & key, std::function<value_t ()> load_func)
        {
            return al::map_get(__cache, key, load_func);    // TODO: thread safe
        }

        // Returns value by specified key.
        virtual value_t get(const key_t & key) override
        {
            auto it = __cache.find(key);
            if(it != __cache.end())
                return it->second;

            return _D(value_t);
        }

        // Sets value with specified key and value.
        virtual void set(const key_t & key, value_t & value) override
        {
            __cache[key] = value;
        }

        static const int __cache_key__ = _global_cache_key;

    private:
        std::map<_key_t, _value_t> __cache;
    };

    ////////// ////////// ////////// ////////// //////////

    // Global cache keys.
    enum __global_cache_key_t
    {
        __global_cache_key_mname,
        __global_cache_key_generic_type,
        __global_cache_key_array_type,
        __global_cache_key_generic_method,
        __global_cache_key_assembly_reference,
    };

    // Global generic type cache.
    typedef lang_cache_t<sid_t, const mname_t *, __global_cache_key_mname> mname_cache_t;
    typedef lang_cache_t<
        generic_type_cache_key_t, generic_type_t *, __global_cache_key_generic_type
    > generic_type_cache_t;

    // Global array type cache.
    typedef lang_cache_t<
        array_type_cache_key_t, array_type_t *, __global_cache_key_array_type
    > array_type_cache_t;

    // Global generic method cache.
    typedef lang_cache_t<
        generic_method_cache_key_t, generic_method_t *, __global_cache_key_generic_method
    > generic_method_cache_t;

    // Global assembly reference cache.
    typedef lang_cache_t<
        assembly_reference_cache_key_t, assembly_reference_t *,
        __global_cache_key_assembly_reference
    > assembly_reference_cache_t;

    ////////// ////////// ////////// ////////// //////////

    class analyzer_t;

    // Global context.
    class global_context_t : public object_t, public no_copy_ctor_t
    {
    public:

        // Constructor.
        global_context_t(lang_factory_t * lang_factory);

        // Returns language of specified id.
        lang_t * lang_of(lang_id_t lang_id);

        // Returns language id of specified name.
        lang_id_t lang_id_of(const string_t & lang_name);

        // Returns memory management.
        memory_t * get_memory() { return &__heap; }

        // Creates a new object.
        template<typename t, typename ... args_t>
        t * new_obj(args_t && ... args)
        {
            return memory_t::new_obj<t>(&__heap, std::forward<args_t>(args) ...);
        }

        // Lanaguage factory.
        lang_factory_t * const lang_factory;

        // Gets value by language id and key.
        // Auto creates if not exists.
        void * from_cache(lang_id_t lang_id, int32_t key, std::function<void *()> load_func)
        {
            // TODO: thread safe
            return al::map_get(__cache, __cache_key_t(lang_id, key), load_func);
        }

        // Returns value by language id and key.
        template<typename t>
        t * from_cache(lang_id_t lang_id, std::function<void *()> load_func)
        {
            return (t *)from_cache(lang_id, (int32_t)t::__cache_key__, load_func);
        }

        // Returns value by language id.
        template<typename t>
        t * from_cache(lang_id_t lang_id = 0)
        {
            return from_cache<t>(lang_id, [this]() { return this->new_obj<t>(); });
        }

        // Returns value of key.
        template<
            typename cache_t, typename load_func_t,
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

    // Compile log codes.
    X_ENUM_(compile_log_code_t, sizeof(compile_log_code_value_t))

        // Writes assembly error.
        write_assembly_error,

        // No assembly name.
        no_assembly_name,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Compile logger.
    class compile_log_t : public object_t
    {
        typedef log_level_t __level_t;

    public:

        // Constructor.
        compile_log_t(lang_t * lang, code_element_t * element, __level_t level,
                            const string_t & name, const string_t & message)
            : lang(lang), level(level), name(name), message(message), element(element)
        { }

        lang_t *        lang;           // Language.
        __level_t       level;          // Level
        string_t        name;           // Name
        string_t        message;        // Message
        code_element_t * element;       // Element
    };

    //-------- ---------- ---------- ---------- ----------

    // Compile logger.
    class compile_logs_t : public object_t
    {
    public:

        // Appends a log.
        void append(const compile_log_t * log)
        {
            __logs.push_back(log);
        }

    private:
        std::vector<const compile_log_t *> __logs;
    };

    ////////// ////////// ////////// ////////// //////////

    class assembly_writer_t;

    // Compile context.
    class compile_context_t : public object_t, public no_copy_ctor_t
    {
        typedef log_level_t                 __log_level_t;
        typedef compile_log_code_value_t    __log_code_t;

    public:

        // Constructor.
        compile_context_t(global_context_t & global_context, assembly_writer_t * writer)
            : global_context(global_context), __writer(writer)
        { }

        // Creates a new object.
        template<typename obj_t, typename ... args_t>
        obj_t * new_obj(args_t && ... args)
        {
            return memory_t::new_obj<obj_t>(&__heap, std::forward<args_t>(args) ...);
        }

        // Returns memory management.
        memory_t * get_memory() { return &__heap; }

        global_context_t & global_context;
        compile_logs_t logs;

        // Logs error message.
        template<typename code_t>
        void log_error(lang_t * lang, code_element_t * element, code_t code,
                                                            const string_t & message)
        {
            log(lang, element, __log_level_t::error, _title(code), message);
        }

        // Logs warning message.
        template<typename code_t>
        void log_warning(lang_t * lang, code_element_t * element, code_t code,
                                                            const string_t & message)
        {
            log(lang, element, __log_level_t::warning, _title(code), message);
        }

        // Logs info message.
        template<typename code_t>
        void log_info(lang_t * lang, code_element_t * element, code_t code,
                                                            const string_t & message)
        {
            log(lang, element, __log_level_t::info, _title(code), message);
        }

        // Log message.
        void log(lang_t * lang, code_element_t * element, __log_level_t level,
                                const string_t & name, const string_t & message)
        {
            logs.append(new_obj<compile_log_t>(lang, element, level, name, message));

            if(level == __log_level_t::error)
                __error_count++;
        }

        // Log message.
        template<typename code_t, typename ... args_t>
        void log(__log_level_t level, code_t code, args_t && ... args)
        {
            return log((lang_t *)nullptr, (code_element_t *)nullptr, level,
                _title(code), _F(_desc(code), std::forward<args_t>(args) ...)
            );
        }

        // Returns error counts.
        size_t error_count() const
        {
            return __error_count;
        }

        // Creates output stream of project.
        xostream_t * create_output_stream(ast_project_t * project);

    private:
        lang_factory_t * __lang_factory;
        al::xheap_t __heap;
        size_t __error_count = 0;
        assembly_writer_t * __writer;
    };

    ////////// ////////// ////////// ////////// //////////

    // Compile logger.
    class compile_logger_t : public logger_t, public xlogger_t
    {
        typedef compile_log_code_value_t    __log_code_t;

    public:

        // Constructor.
        compile_logger_t(compile_context_t & compile_context, lang_t * lang = nullptr)
            : xlogger_t(*(logger_t *)this), __compile_context(compile_context), __lang(lang)
        { }

        // Log message.
        virtual void log(code_element_t * element, log_level_t level,
                            const string_t & name, const string_t & message) override
        {
            __compile_context.log(__lang, element, level, name, message);
        }

        // Returns error count.
        size_t error_count() const
        {
            return __compile_context.error_count();
        }

    private:
        compile_context_t & __compile_context;
        lang_t * __lang;
    };

    ////////// ////////// ////////// ////////// //////////

    // A logger for outputing to console.
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

    // A logger collection.
    class multipy_logger_t : public loggers_t
    {
    public:

        // Constructor.
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

        compile_context_t & compile_context;    // Compile context.
        lang_t * const lang;                    // Language.
        logger_t & logger;                      // Logger.

    private:
        multipy_logger_t __logger;
    };

    ////////// ////////// ////////// ////////// //////////

    // Interface for writing a assembly.
    X_INTERFACE assembly_writer_t
    {
        virtual xostream_t * new_stream(compile_context_t & ctx, ast_project_t * cproject) = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    // Assembly writer for writing to a file.
    class file_assembly_writer_t : public object_t, public assembly_writer_t
    {
        typedef assembly_writer_t __super_t;

    public:

        // Constructor.
        file_assembly_writer_t(const string_t & directory)
            : __directory(directory)
        {
            _A(directory.length() > 0);
        }

        // Creates a new stream for specified project.
        virtual xostream_t * new_stream(compile_context_t & ctx, ast_project_t * cproject) override;

    private:
        string_t __directory;
    };

    ////////// ////////// ////////// ////////// //////////

    struct compiler_result_t
    {
        compiler_result_t(assemblies_t * assemblies, size_t error_count)
            : assemblies(assemblies), error_count(error_count)
        { }

        assemblies_t *  assemblies;
        size_t          error_count;
    };

    //-------- ---------- ---------- ---------- ----------

    // Compiler.
    class compiler_t : public object_t
    {
    public:

        // Constructor.
        compiler_t(lang_factory_t * lang_factory) : __global_context(lang_factory)
        {
            _A(lang_factory != nullptr);
        }

        // Builds ast for a solution.
        ast_solution_t * build_ast(compile_context_t & context, solution_t * solution);

        // Compiles a solution.
        compiler_result_t compile(compile_context_t & context, ast_solution_t * csolution,
                                            ref_assemblies_t * ref_assembilies = nullptr);

        // Compiles a solution.
        compiler_result_t compile(memory_t * memory, solution_t * solution,
                                                assembly_writer_t * writer = nullptr);

        // Compiles a solution.
        compiler_result_t compile(memory_t * memory, solution_t * solution,
                                                const string_t & output_path);

        // Returns global context.
        global_context_t & get_context() { return __global_context; }

        // Creates a compile context for specified writer.
        compile_context_t * new_compile_context(memory_t * memory, assembly_writer_t * writer);

    private:
        global_context_t __global_context;              // Global context.
        std::map<void *, void *> __code_relation_map;   // A map for code objects relation.

        typedef pw_t<assemblies_t> __assemblies_t;
        typedef log_level_t         __l_t;
        typedef compile_log_code_t  __c_t;

        class __assembly_loader_t;
        class __assembly_t;
        class __building_context_t;

        // Builds project.
        ast_project_t * __build_project(compile_context_t & context, project_t * project);

        // Builds file.
        ast_file_t    * __build_file(compile_context_t & context, file_t * file);

        // Compiles project.
        __assembly_t * __compile_project(__building_context_t & bctx, compile_context_t & context,
                ast_project_t * cproject, assembly_loader_t * assembly_loader);
    };

    ////////// ////////// ////////// ////////// //////////

    // Arguments for creates a language.
    struct lang_create_args_t
    {
        lang_id_t lang_id;
    };

    typedef creator_t<lang_t, lang_create_args_t> lang_creator_t;

    //-------- ---------- ---------- ---------- ----------

    namespace
    {
        // A simple language creator.
        template<typename _lang_t>
        class __simple_creator_t : public object_t, public lang_creator_t
        {
        public:

            // Creates a language with specified arguments.
            virtual lang_t * create(memory_t * memory, const lang_create_args_t & args) override
            {
                _A(memory != nullptr);
                return memory_t::new_obj<_lang_t>(memory, args);
            }

            // Destories a language.
            virtual void destory(memory_t * memory, lang_t * obj) override { }
        };
    }

    //-------- ---------- ---------- ---------- ----------

    // A simple language factory.
    class simple_lang_factory_t : public object_t, public lang_factory_t
    {
    public:

        // Creates a language with specified arguments.
        virtual lang_t * create_lang(memory_t * memory, const lang_create_args_t & args) override;

        // Gets language id for specified name.
        virtual lang_id_t get_lang_id(const string_t & name) override;

        // Gets language name for specified id.
        virtual const string_t get_lang_name(lang_id_t lang_id) override;

        // Registers a language creator.
        lang_id_t register_lang(const char_t * name, lang_creator_t * creator);

        // Registers a language creator.
        template<typename _lang_t>
        lang_id_t register_lang_t(const char_t * name)
        {
            return register_lang(name, __pool.new_obj<__simple_creator_t<_lang_t>>());
        }

        // Registers a language creator.
        template<typename _lang_t> lang_id_t register_lang_t()
        {
            return register_lang_t<_lang_t>(_lang_t::__default_name__);
        }

    private:
        std::map<lang_id_t, lang_creator_t *> __creators;
        std::map<lang_id_t, string_t> __id_name_map;
        std::map<string_t, lang_id_t> __name_id_map;

        std::mutex  __lock;
        pool_t      __pool;

        lang_id_t __next_lang_id_value = 1;

        // Returns next language id.
        lang_id_t __next_lang_id()
        {
            return __next_lang_id_value++;
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // Gets a service for a language.
    template<typename _lang_service_t>
    _lang_service_t * get_lang_service(lang_t * lang)
    {
        _A(lang != nullptr);
        return (_lang_service_t *)lang->get_service(_lang_service_t::service_type);
    }

    // Requires language service, raise exception it not exists.
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

    // Language service helper.
    class lang_service_helper_t
    {
    public:

        // Constructor.
        lang_service_helper_t(lang_t * lang)
            : lang(lang)
            , __service_map(std::bind(&lang_t::get_service, lang, std::placeholders::_1))
        {
            _A(lang != nullptr);
        }

        lang_t * const lang;

        // Returns language service.
        template<typename _lang_service_t>
        _lang_service_t * get_service()
        {
            lang_service_type_t service_type = _lang_service_t::service_type;
            return (_lang_service_t *)__service_map[service_type];
        }

        // Returns language service, raise exceptions if not exists.
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

        // Token read service
        token_reader_t * create_token_reader(token_reader_context_t & context);

        // Token property service
        const token_property_t * get_token_property(token_value_t value);

        // Operator property service
        const operator_property_t * get_operator_property(token_value_t value);

        // Expression build service
        const expression_box_property_t * get_expression_box_property(token_value_t value);

        // Builds expression with specified arguments.
        expression_t * build_expression(lang_expression_build_context_t & ctx,
                                    const lang_expression_build_args_t & args);

        // Statement analyze service
        const string_t get_analyze_pattern(global_context_t & context);

        // Returns node value for given name and node type.
        analyze_node_value_t get_node_value(global_context_t & context,
                            const string_t & name, analyze_node_type_t node_type);

        // Ast build service
        ast_node_t * build_ast(ast_context_t & context, lang_ast_build_args_t & args);

    private:
        al::auto_cache_t<lang_service_type_t, lang_service_t *> __service_map;
    };

    ////////// ////////// ////////// ////////// //////////

    // Property cache.
    template<typename key_t, typename property_t>
    class property_cache_t : public object_t
    {
        typedef al::auto_cache_t<key_t, const property_t *> __cache_t;

    public:

        // Constructor.
        template<typename loader_t>
        property_cache_t(loader_t loader) : __cache(loader) { }

        // Tries to get a property.
        const property_t * try_get_property(const key_t & key)
        {
            return __cache[key];
        }

        // Returns property for specified key.
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

    // Token property cache.
    template<typename property_t>
    class __token_property_cache_t : public property_cache_t<token_value_t, property_t>
    {
    public:

        // Constructors.
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

    // Defines default token property cache.
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

    // Token property cache.
    __DefaultTokenPropertyCache(token_property_t, get_token_property);

    // Operator property cache.
    __DefaultTokenPropertyCache(operator_property_t, get_operator_property);

    // Expression box property cache.
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
