
#include <compile.h>
#include <lib.h>
#include <algorithm.h>
#include "utils.h"
#include "controller.h"

namespace X_ROOT_NS { namespace modules { namespace compile {

    using namespace core;

    ////////// ////////// ////////// ////////// //////////

    // Compile error codes.
    X_ENUM_INFO(compile_error_code_t)

        // Unknown language.
        X_C(unknown_lang,                   _T("unknown language name"))

        // Empty language.
        X_C(empty_lang,                     _T("language name empty"))

        // Format error.
        X_C(format_error,                   _T("format error"))

        // Service not supported.
        X_C(service_not_supported,          _T("service not supported"))

        // Assembly curcular referenced.
        X_C(assembly_circular_reference,    _T("assembly circular reference"))

        // Assembly not found.
        X_C(assembly_not_found,             _T("assembly_not_found"))

        // Assign expression type error.
        X_D(assign_expression_type_error,
                    _T("assign left expression should be a variable/index: found '%1%'"))

        // Variable cannot determinded.
        X_D(variable_cannot_determined,     _T("variable '%1%' connot determined"))

        // Index body missing.
        X_D(index_body_missing,             _T("index '%1%' body missing"))

        // Index arguent missing.
        X_D(index_arguments_missing,        _T("index '%1%' arguments missing"))

        // Index type undeterminded.
        X_D(index_type_undetermined,        _T("index '%1%' type undetermined"))

        // Unexpected jmp operation.
        X_D(unexpected_jmp,                 _T("unexpected jmp '%1%'"))

        // Jmp point no found.
        X_D(jmp_point_not_found,            _T("jmp point '%1%' not found"))

        // Expression missing.
        X_D(expession_missing,              _T("%1% expression missing"))

        // Duplicate switch case.
        X_D(duplicate_case,                 _T("duplicate case '%1%'"))

        // Expected constant value.
        X_D(expected_constant_value,        _T("expected constant value"))

        // Expected constant value type.
        X_D(expected_constant_value_type,   _T("expected constant value type: '%1%'"))

        // Type missing.
        X_D(type_missing,                   _T("type missing"))

        // Type error.
        X_D(type_error,                     _T("type error, expected %1%"))

        // Unexpected member.
        X_D(unexpected_member,              _T("unexpected member '%1%'"))

        // Unknown field.
        X_D(unknown_field,                  _T("unknown field '%1%'"))

        // Unknown field type.
        X_D(unknown_field_type,             _T("unknown field '%1%' type"))

        // Expected a instance method.
        X_D(expected_instance_method,       _T("'%1%' is not an instance method"))

        // Unknown method.
        X_D(unknown_method,                 _T("unknown method '%1%'"))

        // Unknown property.
        X_D(unknown_property,               _T("unknown property '%1%'"))

        // Property cannot be read.
        X_D(property_cannot_be_read,        _T("property '%1%' cannot be read"))

        // Property cannot be write.
        X_D(property_cannot_be_write,       _T("property '%1%' cannot be write"))

        // Host type missing.
        X_D(host_type_missing,              _T("member '%1%' host type missing"))

        // Constructor missing.
        X_D(constructor_missing,            _T("constructor of type '%1%' missing"))

        // Default constructor missing.
        X_D(default_constructor_missing,    _T("default constructor of type '%1%' missing"))

        // Unexpected constructor prototype: should no return type.
        X_D(unexpected_constructor_prototype__should_no_return_type,
                                            _T("constructor '%1%' should no return type"))

        // Unexpected constructor prototype: should not a virtual method.
        X_D(unexpected_constructor_prototype__should_no_virtual,
                                            _T("constructor '%1%' should no vitual"))

        // Unexpected constructor prototype: shoud not a static method.
        X_D(unexpected_constructor_prototype__should_no_virtual,
                                            _T("constructor '%1%' should no static"))

        // Unexpected host type.
        X_D(unexpected_host_type,           _T("member '%1%' has unexpected host type '%2%'"))

        // No default value.
        X_D(no_default_value,               _T("type '%1%' has no default value"))

        // Unknown array length.
        X_D(unknown_array_length,           _T("unknown array length: '%1%'"))

        // Index man expression missing.
        X_D(index_main_expression_missing,  _T("index main expression missing"))

        // Element type of array undeterminded.
        X_D(element_type_undeterminded,     _T("index element type underterminded"))

        // Index element init expression empty.
        X_D(element_init_expression_empty,  _T("index element init expression empty"))

        // Cache exception variable undeterminded.
        X_D(catch_exception_variable_undeterminded, _T("Cache exception variable undeterminded"))

        // Operator overload method prototype error: Param count error.
        X_D(operator_overload__wrong_param_count,
                _T("Operator overload method '%1%' prototype error: Param count error"))

        // Operator overload method prototype error: Argument type error.
        X_D(operator_overload__wrong_argument_type,
            _T("Operator overload method '%1%' argument type error: cannot assign %1% to param %2%"))

        // Operator overload method prototype error: should be static.
        X_D(operator_overload__wrong_prototype,
                _T("Operator overload method '%1%' prototype error: should be static"))

        // Operator overload method prototype error: Should not be a generic type.
        X_D(operator_overload__no_generic,
                _T("Operator overload method '%1%' prototype error: should not be a generic type"))

		X_D(delegate_type_error,
				_T("Delegate type error, should be System.Deletate<...>, but not %1%"))

		X_D(create_instance_type_error,
				_T("Cannot create instance of type '%1%', because it's a '%2%' type"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Compile optimize codes.
    X_ENUM_INFO(compile_optimize_code_t)

        // Remove unreached codes.
        X_D(remove_unreached_codes, _T("Remove unreached codes."))

        // Converts switch to if statement when case lables is too little.
        X_D(convert_switch_to_if_statement,
                _T("Converts switch to if statement when case lables is too little."))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Analyze node types
    X_ENUM_INFO(analyze_node_type_t)

        // Empty node.
        X_C(empty,          _T("empty"))

        // Token node.
        X_C(token,          _T("token"))

        // Branch node.
        X_C(branch,         _T("branch"))

        // Branch ref node.
        X_C(branch_ref,     _T("branch_ref"))

        // End node.
        X_C(end,            _T("end"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    typedef code_object_metadata_key_t __mt_key_t;

    // Returns entity of code_object.
    template<typename mt_t>
    mt_t & __mt_obj(code_object_t * code_object)
    {
        __mt_key_t key = mt_t::key;
        return code_object->metadata.get<mt_t>(key, true);
    }

    //-------- ---------- ---------- ---------- ----------

    // Compile file.
    struct __mt_compile_file_t
    {
        static const __mt_key_t key = (__mt_key_t)1979;

        ast_file_t * ast_file;
    };

    ////////// ////////// ////////// ////////// //////////

    // Code object metadata key.
    X_ENUM_INFO(code_object_metadata_key_t)

        // Path
        X_C(path,   _T("path"))

        // Title
        X_C(title,  _T("title"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Load source file by path and name.
    file_t * file_t::load(global_context_t & global_context,
            memory_t * memory, const lib::path_t & path, const char_t * lang, const char_t * name)
    {
        _A(memory != nullptr);

        string_t code = lib::read_all_text(path);
        string_t s_name = !al::is_empty(name)? string_t(name) : lib::filename(path);
        string_t s_lang = !al::is_empty(lang)? string_t(lang) : lib::fileext(path);

        file_t * file = memory_t::new_obj<file_t>(memory, global_context, lib::to_str(path), s_name);
        lang_id_t lang_id = global_context.lang_id_of(s_lang);
        file->code = memory_t::new_obj<code_t>(memory, global_context, std::move(code),
                                                                        lang_id, file);

        return file;
    }

    // Returns source code.
    const char_t * file_t::source_code() const
    {
        if (code == nullptr)
            return nullptr;

        return code->code.c_str();
    }

    ////////// ////////// ////////// ////////// //////////
    // code_section_t

    // Returns code file.
    const code_file_t * code_section_t::file() const
    {
        return code? code->file : nullptr;
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns code sections.
    range_t<code_t::__itor_t> code_t::sections()
    {
        if (!__sections.size())
        {
            code_section_builder_t(global_context, this, lang, __sections, __pool).build();
        }

        return _range(__sections);
    }

    // Builds ast nodes.
    ast_node_t * code_t::build_ast(compile_context_t & context)
    {
        return analyze_ast(context, this);
    }

    ////////// ////////// ////////// ////////// //////////

    // Appends a file.
    void project_t::append_file(file_t * file)
    {
        _A(file != nullptr);

        __file_map[file->path] = file;
        __files.push_back(file);
    }

    // Gets a file.
    file_t * project_t::get_file(const string_t & path) const
    {
        auto it = __file_map.find(path);
        if (it != __file_map.end())
            return it->second;

        return nullptr;
    }

    // Gets a file at index.
    file_t * project_t::at(size_t index) const
    {
        _A(index < file_count());

        return __files[index];
    }

    // Returns whether a path exists in the project.
    bool project_t::exists(const string_t & path) const
    {
        return get_file(path) != nullptr;
    }

    ////////// ////////// ////////// ////////// //////////
    // ref_assembly_t

    // Converts ref_assembly_t to a stirng.
    X_DEFINE_TO_STRING(ref_assembly_t)
    {
        if (package.empty())
            return name;

        return _F(_T("%1%.%2%"), package, name);
    }

    ////////// ////////// ////////// ////////// //////////
    // ref_assemblies_t

    // Appends a ref of assembly.
    void ref_assemblies_t::append_ref_assembly(ref_assembly_t * ref_assembly)
    {
        _A(ref_assembly != nullptr);

        __ref_assembly_key_t key(ref_assembly->package, ref_assembly->name);
        if (__ref_assemblies.find(key) != __ref_assemblies.end())
        {
            if (ref_assembly->package.empty())
                throw _ECF(invalid_operation, _T("duplicate assembly '%1%'"), ref_assembly);
        }

        __ref_assemblies[key] = ref_assembly;
    }

    // Gets a ref of assembly.
    ref_assembly_t * ref_assemblies_t::get_ref_assembly(const string_t & package,
                                                        const string_t & name)
    {
        __ref_assembly_key_t key(package, name);

        auto it = __ref_assemblies.find(key);
        if (it != __ref_assemblies.end())
            return it->second;

        return nullptr;
    }

    ////////// ////////// ////////// ////////// //////////
    // file_ref_assembly_t

    // Read assembly bytes.
    size_t file_ref_assembly_t::read(byte_t * buffer, size_t size)
    {
        __ensure_open();

        return __file_stream.read(buffer, size);
    }

    // Completed notify.
    void file_ref_assembly_t::completed()
    {
        __file_stream.completed();
    }

    // Ensures file is opened.
    void file_ref_assembly_t::__ensure_open()
    {
        if (!__opened)
        {
            typedef std::ios_base __i;
            typedef std::ios_base::openmode __open_mode_t;

            __open_mode_t mode = __i::in | __i::binary;
            __file_stream.open(lib::to_str(__path), mode);

            __opened = true;
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Appends a project.
    void solution_t::append_project(project_t * project)
    {
        _A(project != nullptr);

        auto name = project->name;
        __project_map[name] = project;
        __projects.push_back(project);
    }

    // Gets a project by a name.
    project_t * solution_t::get_project(const string_t & name) const
    {
        auto it = __project_map.find(name);
        if (it != __project_map.end())
            return it->second;

        return nullptr;
    }

    // Returns project at specified index.
    project_t * solution_t::at(size_t index) const
    {
        _A(index < project_count());

        return __projects[index];
    }

    ////////// ////////// ////////// ////////// //////////

    // Language service types.
    X_ENUM_INFO(lang_service_type_t)

        // Token reader service.
        X_C(token_read)

        // Token property service.
        X_C(token_property)

        // Operator property service.
        X_C(operator_property)

        // Expression build service.
        X_C(expression_build)

        // Statement analyze service.
        X_C(statement_analyze)

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Constructor.
    global_context_t::global_context_t(lang_factory_t * lang_factory)
        : lang_factory(lang_factory), xpool(&__heap)
    {
        _A(lang_factory != nullptr);

        xpool.mname_cache = from_cache<mname_cache_t>();
        xpool.generic_type_cache = from_cache<generic_type_cache_t>();
        xpool.array_type_cache = from_cache<array_type_cache_t>();
        xpool.generic_method_cache = from_cache<generic_method_cache_t>();
        xpool.assembly_reference_cache = from_cache<assembly_reference_cache_t>();

        xpool.initialize();
    }

    // Returns language of specified id.
    lang_t * global_context_t::lang_of(lang_id_t lang_id)
    {
        return al::map_get(__lang_map, lang_id, [lang_id, this] {  // TODO: thread safe
            return lang_factory->create_lang(&__heap, lang_create_args_t { lang_id });
        });
    }

    // Returns language id of specified name.
    lang_id_t global_context_t::lang_id_of(const string_t & lang_name)
    {
        lang_id_t lang_id = lang_factory->get_lang_id(lang_name);
        if (lang_id == unknown_lang_id)
        {
            throw _E(compile_error_code_t::unknown_lang,
                sprintf(_T("unknown lang \"%1%\""), lang_name)
            );
        }

        return lang_id;
    }

    ////////// ////////// ////////// ////////// //////////

    enum class __assembly_status_t { init, loading, completed };

    // Assembly
    class compiler_t::__assembly_t : public assembly_t
    {
    public:
        using assembly_t::assembly_t;

        __assembly_status_t status = __assembly_status_t::init;
    };

    ////////// ////////// ////////// ////////// //////////

    // Context for building asts.
    class compiler_t::__building_context_t
    {
    public:

        // Constructor.
        __building_context_t(assemblies_t * assemblies) : assemblies(assemblies) { }

        // Assemblies
        assemblies_t * assemblies;

        // Begin compile.
        bool begin_compile(ast_project_t * cproject)
        {
            _A(cproject != nullptr);

            return __compiling_assemblies.insert(to_sid(cproject->name)).second;
        }

        // End compile.
        void end_compile(__assembly_t * assembly)
        {
            _A(assembly != nullptr);

            __compiling_assemblies.erase(to_sid(assembly->name));
            assemblies->append(assembly);
        }

        // Returns whether the project is compiling.
        bool is_compiling(const mname_t * name)
        {
            return __compiling_assemblies.find(to_sid(name)) != __compiling_assemblies.end();
        }

        // Returns whether the project is compiled.
        bool is_compiled(const mname_t * name)
        {
            return assemblies->get(name) != nullptr;
        }

        // Returns assembly of specified package and name.
        __assembly_t * get_ref_assembly(const mname_t * package, const mname_t * name)
        {
            __ref_assembly_key_t key(to_sid(package), to_sid(name));

            auto it = __ref_assemblies.find(key);
            if (it == __ref_assemblies.end())
                return nullptr;

            return it->second;
        }

        // Add reference of assembly.
        void add_ref_assembly(const mname_t * package, const mname_t * name, __assembly_t * assembly)
        {
            _A(assembly != nullptr);

            __ref_assembly_key_t key(to_sid(package), to_sid(name));
            __ref_assemblies[key] = assembly;
        }

    private:
        std::set<sid_t> __compiling_assemblies;

        typedef std::tuple<sid_t, sid_t> __ref_assembly_key_t;
        std::map<__ref_assembly_key_t, __assembly_t *> __ref_assemblies;
    };

    ////////// ////////// ////////// ////////// //////////

    // Assembly loader.
    class compiler_t::__assembly_loader_t : public assembly_loader_t
    {
    public:

        // Constructor.
        __assembly_loader_t(__building_context_t & bctx, compiler_t * compiler,
                compile_context_t & context, ast_solution_t * csolution,
                ref_assemblies_t * ref_assemblies)
            : __bctx(bctx), __compiler(compiler), __context(context), __csolution(csolution)
            , __ref_assemblies(ref_assemblies)
        { }

        // Load assembly.
        virtual assembly_t * load_assembly(const mname_t * package_name,
                                           const mname_t * assembly_name) override
        {
            _A(assembly_name != nullptr);

            __assembly_key_t key(to_sid(package_name), to_sid(assembly_name));
            assembly_t * assembly = al::map_get(__assemblies, key,
                [this, package_name, assembly_name] {
                    return __load_assembly(package_name, assembly_name);
                }
            );

            if (assembly == nullptr)
                throw _E(compile_error_code_t::assembly_not_found);

            return assembly;
        }

    private:
        __building_context_t & __bctx;
        compiler_t           * __compiler;
        compile_context_t    & __context;
        ast_solution_t       * __csolution;
        ref_assemblies_t     * __ref_assemblies;

        typedef sid_t __package_sid_t, __assembly_sid_t;
        typedef std::tuple<__package_sid_t, __assembly_sid_t> __assembly_key_t;
        std::map<__assembly_key_t, __assembly_t *> __assemblies;

        xpool_t & __xpool() { return __context.global_context.xpool; }

        // Loads assembly.
        __assembly_t * __load_assembly(const mname_t * package_name, const mname_t * assembly_name)
        {
            if (package_name == nullptr)
                return __load_assembly(assembly_name);

            return __load_assembly_from_package(package_name, assembly_name);
        }

        // Loads assembly.
        __assembly_t * __load_assembly(const mname_t * assembly_name)
        {
            assembly_t * assembly = __bctx.assemblies->get(assembly_name);
            if (assembly != nullptr)
                return (__assembly_t *)assembly;

            ast_project_t * cproject = __csolution->get_project(assembly_name);
            if (cproject != nullptr)
                return __build_project(cproject);

            return __load_assembly_from_package(nullptr, assembly_name);
        }

        // Loads assembly from package.
        __assembly_t * __load_assembly_from_package(const mname_t * package_name,
                                                    const mname_t * assembly_name)
        {
            __assembly_t * assembly = __bctx.get_ref_assembly(package_name, assembly_name);
            if (assembly != nullptr)
                return assembly;

            if (__ref_assemblies == nullptr)
                return nullptr;

            ref_assembly_t * ref_assembly = __ref_assemblies->get_ref_assembly(
                package_name? (string_t)*package_name : empty_str,
                assembly_name? (string_t)*assembly_name : empty_str
            );

            if (ref_assembly == nullptr)
                return nullptr;

            try
            {
                assembly = __context.new_obj<__assembly_t>(__xpool(), assembly_name);
                assembly->load(*ref_assembly, this);
            }
            catch (error_t & e)
            {
                // TODO:
                throw;
            }

            ref_assembly->completed();
            return assembly;
        }

        // Builds project.
        __assembly_t * __build_project(ast_project_t * cproject)
        {
            return __compiler->__compile_project(__bctx, __context, cproject, this);
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // Creates an assembly output stream.
    xostream_t * file_assembly_writer_t::new_stream(
                            compile_context_t & ctx, ast_project_t * cproject)
    {
        if (cproject->name == nullptr)
            throw _EC(unexpected, _T("project name empty"));

        string_t name = (string_t)*cproject->name;
        string_t path = lib::path_combine(__directory, name);
        path = lib::path_add_extension(path, JC_ASSEMBLY_EXT);

        file_xostream_t * fs = ctx.new_obj<file_xostream_t>();
        typedef std::ios_base __i;

        fs->open(path, __i::out | __i::trunc | __i::binary);
        cproject->assembly_path = path;

        return fs;
    }   

    ////////// ////////// ////////// ////////// //////////

    // Creates an assembly output stream for a project.
    xostream_t * compile_context_t::create_output_stream(ast_project_t * project)
    {
        if (__writer == nullptr)
            return nullptr;

        return __writer->new_stream(*this, project);
    }

    // An empty assembly output stream.
    class __empty_xostream_t : public xostream_t
    {
    public:
        virtual void write(const byte_t * buffer, size_t size) override { }
        virtual void completed() override { }
    } __empty_xostream;

    ////////// ////////// ////////// ////////// //////////

    // Converts name to a mname.
    const mname_t * __to_mname(global_context_t & gctx, const string_t & name)
    {
        return mname_t::parse(
            to_mname_operate_context(gctx.xpool), name
        );
    }

    // Builds ast nodes.
    ast_solution_t * compiler_t::build_ast(compile_context_t & context, solution_t * solution)
    {
        _A(solution != nullptr);

        ast_solution_t * csolution = context.new_obj<ast_solution_t>(
            __to_mname(__global_context, solution->name)
        );

        __code_relation_map[csolution] = solution;

        for (project_t * project : solution->projects())
        {
            ast_project_t * cproject = __build_project(context, project);
            __code_relation_map[cproject] = project;
            csolution->add_project(cproject);
        }

        return csolution;
    }

    // Builds project.
    ast_project_t * compiler_t::__build_project(compile_context_t & context, project_t * project)
    {
        ast_project_t * cproject = context.new_obj<ast_project_t>(
            __to_mname(__global_context, project->name)
        );

        for (file_t * file : project->files())
        {
            ast_file_t * cfile = __build_file(context, file);
            cproject->add_file(cfile);
            __code_relation_map[cfile] = file;
        }

        return cproject;
    }

    // Builds file.
    ast_file_t * compiler_t::__build_file(compile_context_t & context, file_t * file)
    {
        _A(file->code != nullptr);

        return context.new_obj<ast_file_t>(
            __to_mname(__global_context, file->name), file->code->build_ast(context)
        );
    }

    // Compiles a solution.
    compiler_result_t compiler_t::compile(compile_context_t & context, ast_solution_t * csolution,
                                                        ref_assemblies_t * ref_assemblies)
    {
        _A(csolution != nullptr);

        assemblies_t * assemblies = context.new_obj<assemblies_t>();
        __building_context_t bctx(assemblies);
        __assembly_loader_t assembly_loader(bctx, this, context, csolution, ref_assemblies);

        for (ast_project_t * cproject : *csolution)
        {
            if (!bctx.is_compiling(cproject->name) && !bctx.is_compiled(cproject->name))
            {
                assembly_t * assembly = __compile_project(bctx, context, cproject, &assembly_loader);
            }
        }

        return compiler_result_t(assemblies, context.error_count());
    }

    // Compiles solution.
    compiler_result_t compiler_t::compile(memory_t * memory, solution_t * solution,
                                                        const string_t & output_path)
    {
        file_assembly_writer_t * writer = memory_t::new_obj<file_assembly_writer_t>(
            memory, output_path
        );

        return compile(memory, solution, writer);
    }

    // Compiles project.
    compiler_t::__assembly_t * compiler_t::__compile_project(__building_context_t & bctx,
        compile_context_t & context, ast_project_t * cproject, assembly_loader_t * assembly_loader)
    {
        if (!bctx.begin_compile(cproject))
            throw _E(compile_error_code_t::assembly_circular_reference);

        __assembly_t * assembly = context.new_obj<__assembly_t>(
            __global_context.xpool, cproject->name
        );

        multipy_logger_t logger(context);
        ast_walk_context_t walk_context(
            *assembly, context.global_context.xpool, assembly_loader, logger
        );

        std::vector<ast_node_t *> nodes;
        for (ast_file_t * cfile : *cproject)
        {
            ast_node_t * root = cfile->root;
            _A(root != nullptr);

            nodes.push_back(root);
        }

        ast_walk<__module_ast_node_t>(walk_context, nodes);
        if (context.error_count() == 0)
        {
            xostream_t * stream = nullptr;
            if (assembly->name == nullptr || assembly->name->sid == sid_t::null)
                context.log(__l_t::warning, __c_t::no_assembly_name);
            else
                stream = context.create_output_stream(cproject);

            if (stream == nullptr)
                stream = &__empty_xostream;

            assembly->write(*stream, logger, default_controller());
            stream->completed();
        }

        bctx.end_compile(assembly);

        project_t * project = (project_t *)__code_relation_map[cproject];
        _A(project != nullptr);

        project->assembly      = assembly;
        project->assembly_path = cproject->assembly_path;

        return assembly;
    }

    // Compiles solution.
    compiler_result_t compiler_t::compile(memory_t * memory, solution_t * solution,
                                                    assembly_writer_t * writer)
    {
        _A(solution != nullptr);

        compile_context_t * context = new_compile_context(memory, writer);
        ast_solution_t * csolution = build_ast(*context, solution);

        return compile(*context, csolution, &solution->ref_assemblies);
    }

    // Creates a new compile context.
    compile_context_t * compiler_t::new_compile_context(memory_t * memory,
                                                    assembly_writer_t * writer)
    {
        return memory_t::new_obj<compile_context_t>(memory, __global_context, writer);
    }

    ////////// ////////// ////////// ////////// //////////
    // Compile log
    
    // Compile log codes.
    X_ENUM_INFO(compile_log_code_t)

        // Writes assembly error.
        X_D(write_assembly_error, _T("write assembly %1% fail: %2%"))

        // No assembly name.
        X_D(no_assembly_name, _T("write assembly fail: no assembly name"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Creates a language with specified arguments.
    lang_t * simple_lang_factory_t::create_lang(memory_t * memory, const lang_create_args_t & args)
    {
        _A(memory != nullptr);

        if (args.lang_id == unknown_lang_id)
            throw _E(compile_error_code_t::unknown_lang);

        auto creator_it = __creators.find(args.lang_id);
        if (creator_it == __creators.end())
        {
            throw _E(compile_error_code_t::unknown_lang, 
                sprintf(_T("unknown language \"%1%\""), get_lang_name(args.lang_id))
            );
        }

        lang_creator_t * creator = creator_it->second;
        lang_t * lang = creator->create(memory, args);

        _A(lang != nullptr);
        return lang;
    }

    // Registes a language with name and creator.
    lang_id_t simple_lang_factory_t::register_lang(const char_t * name, lang_creator_t * creator)
    {
        _A(!al::is_empty(name));
        _A(creator != nullptr);

        _L(__lock);

        lang_id_t lang_id = __next_lang_id();
        __creators[lang_id] = creator;
        __id_name_map[lang_id] = name;
        __name_id_map[name] = lang_id;

        return lang_id;
    }

    // Returns language id of name.
    lang_id_t simple_lang_factory_t::get_lang_id(const string_t & name)
    {
        auto it = __name_id_map.find(name);
        return it != __name_id_map.end()? it->second : unknown_lang_id;
    }

    // Returns language name of language id.
    const string_t simple_lang_factory_t::get_lang_name(lang_id_t lang_id)
    {
        auto it = __id_name_map.find(lang_id);
        return it != __id_name_map.end()? it->second : string_t(_T(""));
    }

    ////////// ////////// ////////// ////////// //////////
    // lang_service_helper_t

    #define __RequireService(service) (require_service<lang_##service##_service_t>())

    // Token read service
    token_reader_t * lang_service_helper_t::create_token_reader(token_reader_context_t & context)
    {
        return __RequireService(token_read)->create_token_reader(context);
    }

    // Token property service
    const token_property_t * lang_service_helper_t::get_token_property(token_value_t value)
    {
        return __RequireService(token_property)->get_token_property(value);
    }

    // Operator property service
    const operator_property_t * lang_service_helper_t::get_operator_property(token_value_t value)
    {
        return __RequireService(operator_property)->get_operator_property(value);
    }

    // Expression build service
    const expression_box_property_t * lang_service_helper_t::get_expression_box_property(
                                                                        token_value_t value)
    {
        return __RequireService(expression_build)->get_expression_box_property(value);
    }

    // Building expressions.
    expression_t * lang_service_helper_t::build_expression(
        lang_expression_build_context_t & ctx, const lang_expression_build_args_t & args)
    {
        return __RequireService(expression_build)->build_expression(ctx, args);
    }

    // Statement analyze service
    const string_t lang_service_helper_t::get_analyze_pattern(global_context_t & context)
    {
        return __RequireService(statement_analyze)->get_analyze_pattern(context);
    }

    // Returns node value.
    analyze_node_value_t lang_service_helper_t::get_node_value(global_context_t & context,
                                const string_t & name, analyze_node_type_t node_type)
    {
        return __RequireService(statement_analyze)->get_node_value(context, name, node_type);
    }

    // Ast build service
    ast_node_t * lang_service_helper_t::build_ast(ast_context_t & context, 
                                                            lang_ast_build_args_t & args)
    {
        return __RequireService(ast_build)->build_ast(context, args);
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // X_ROOT_NS::modules::compile

