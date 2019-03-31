
#include <april.h>
#include "util.h"

namespace X_ROOT_NS { namespace modules { namespace april {

    using namespace core;
    using namespace compile;

    using namespace rt;
    using namespace exec;
    using namespace mm;

    ////////// ////////// ////////// ////////// /////////
    // april_compiler_t

    // Constructor.
    april_compiler_t::april_compiler_t(const string_t & solution_name)
    {
        __lang_factory.register_lang_t<lang_cs::cs_lang_t>();
        __lang_factory.register_lang_t<lang_lq::lq_lang_t>();

        __compiler = __new_obj<compiler_t>(&__lang_factory);
        __solution = __new_obj<solution_t>(__global_context(), solution_name);
    }

    // Begins a project.
    project_t * april_compiler_t::begin_project(const string_t & name)
    {
        if(__current_project != nullptr)
            throw _ED(__e_t::project_not_committed, __current_project);

        __current_project = __new_obj<project_t>(__global_context(), name);
        return __current_project;
    }

    // Adds file to current project.
    void april_compiler_t::add_file(const lib::path_t & file, const string_t & name)
    {
        if(__current_project == nullptr)
            throw _ED(__e_t::no_current_project);

        each_files(file, [&](const __path_t & file_path) {

            compile_file_type_t file_type = file_type_of(file_path);
            if(file_type != compile_file_type_t::source)
                return;

            if(!__current_project->exists(lib::to_str(file_path)))
            {
                file_t * file = file_t::load(__global_context(), __get_memory(),
                    file_path, name.c_str()
                );

                __current_project->append_file(file);
            }

        }, true);
    }

    // Commits project.
    void april_compiler_t::commit_project()
    {
        if(__current_project == nullptr)
            throw _ED(__e_t::no_project_need_commit);

        if(__current_project->empty())
            throw _ED(__e_t::project_no_files, __current_project->name);

        if(__current_project->name == _T(""))
            __current_project->name = __get_default_project_name(__current_project);

        __solution->append_project(__current_project);
        __current_project = nullptr;
    }

    // Adds assembly.
    void april_compiler_t::add_assembly(file_ref_assembly_t * ref_assembly)
    {
        _A(ref_assembly != nullptr);

        __solution->ref_assemblies.append_ref_assembly(ref_assembly);
    }

    // Compiles, returns true if succeed.
    bool april_compiler_t::compile(const string_t & output_path)
    {
        if(__current_project != nullptr)
            throw _ED(__e_t::project_not_committed, __current_project);

        if(__solution->empty())
            throw _ED(__e_t::no_projects);

        string_t opath = output_path;
        if(opath.length() == 0)
            opath = __get_default_output_path();

        opath = lib::to_str(__bf::absolute(opath));
        compiler_result_t r = __compiler->compile(__get_memory(), __solution, opath);

        return r.error_count == 0;
    }

    // Sets solution name.
    void april_compiler_t::set_solution_name(const string_t & name)
    {
        __solution->name = name;
    }

    // Gets default project name.
    string_t april_compiler_t::__get_default_project_name(project_t * project)
    {
        _A(project != nullptr);
        _A(!project->empty());

        file_t * file = project->at(0);
        string_t parent_path = lib::parent(file->path);
        string_t dir_name = lib::filename(parent_path);

        if(!__solution->exists(dir_name))
            return dir_name;

        for(int index = 1; ; index++)
        {
            string_t dir_name0 = _F(_T("%1%%2%"), dir_name, index);
            if(!__solution->exists(dir_name0))
                return dir_name;
        }
    }

    // Gets default output path.
    string_t april_compiler_t::__get_default_output_path()
    {
        return lib::to_str(__bf::current_path());
    }

    ////////// ////////// ////////// ////////// /////////

    // Runtime assembly provider.
    class __rt_assembly_provider_t : public rt_assembly_provider_t
    {
        typedef std::tuple<string_t, string_t> __assembly_key_t;

    public:

        // Constructor.
        __rt_assembly_provider_t(const lib::path_t & main_assembly_path)
            : __main_assembly_path(main_assembly_path)
        { }

        // Loads main assembly.
        virtual rt_assembly_t * load_main_assembly(rt_context_t & ctx) override
        {
            return __load_assembly(ctx, __main_assembly_path);
        }

        // Loads assembly.
        virtual rt_assembly_t * load_assembly(rt_context_t & ctx, const string_t & package,
                                              const string_t & name) override
        {
            auto it = __map.find(__assembly_key_t(package, name));
            if(it != __map.end())
            {
                lib::path_t path = it->second;
                return __load_assembly(ctx, path);
            }

            return __load_assembly(ctx, package, name);
        }

        // Appends a assembly with specified package and name.
        void append(const string_t & package, const string_t & name, const lib::path_t & path)
        {
            __map[__assembly_key_t(package, name)] = path;
        }

        // Appends a assembly with specified path.
        void append(const string_t & name, const lib::path_t & path)
        {
            append(_T(""), name, path);
        }

    private:
        lib::path_t     __main_assembly_path;
        std::map<__assembly_key_t, lib::path_t> __map;

        // Loads assembly by specified path.
        rt_assembly_t * __load_assembly(rt_context_t & ctx, const lib::path_t & path)
        {
            return rt::load_assembly(ctx, path.wstring());
        }

        // Loads assembly by package and name.
        rt_assembly_t * __load_assembly(rt_context_t & ctx, const string_t & package,
                                                            const string_t & name)
        {
            return rt::load_assembly(ctx,
                __main_assembly_path.parent_path().wstring(), package, name
            );
        }
    };

    ////////// ////////// ////////// ////////// /////////
    // april_xcompiler_t

    // Constructor.
    april_xcompiler_t::april_xcompiler_t(const __compile_options_t & options)
        : __options(options)
    { }

    // Compiles.
    bool april_xcompiler_t::compile()
    {
        switch(__determine_compile_type())
        {
            case compile_file_type_t::solution:
                return __compile_solutions();

            case compile_file_type_t::project:
                return __compile_projects();

            case compile_file_type_t::source:
                return __compile_sources();

            case compile_file_type_t::none:
            default:
                throw _ED(__e_t::no_files);
        }
    }

    // Runs specified assembly.
    bool april_xcompiler_t::run(const string_t & execute_assembly)
    {
        try
        {
            if(!this->compile())
                return false;
        }
        catch(const logic_error_t<__e_t> & e)
        {
            if(e.code != __e_t::no_files)
                throw e;
        }

        this->__run(execute_assembly);
        return true;
    }

    // Runs specified assembly.
    void april_xcompiler_t::__run(const string_t & execute_assembly)
    {
        __cassembly_t * main_cassembly;

        if(execute_assembly.empty())
            main_cassembly = __pick_default_execute_assembly();
        else
            main_cassembly = __get_assembly(execute_assembly);

        __rt_assembly_provider_t provider(main_cassembly->get_assembly_path());

        for(__cassembly_t * ca : __cassemblies)
        {
            provider.append(ca->name, ca->get_assembly_path());
        }

        rt_context_t rt_ctx(&__xheap, create_heap(&__xheap), &provider);
        exec::execute(rt_ctx);
    }

    // Determines the compile type.
    compile_file_type_t april_xcompiler_t::__determine_compile_type()
    {
        compile_file_type_t file_type = compile_file_type_t::none;

        __each_files([&](const __path_t & path) {

            compile_file_type_t file_type0 = file_type_of(path);
            if(file_type0 == compile_file_type_t::assembly)
            {
                __record_assembly(path);
            }
            else
            {
                if(file_type == compile_file_type_t::none)
                    file_type = file_type0;
                else if(file_type != file_type0)
                    throw _ED(__e_t::mixture_file_types, _F(_T("%1%, %2%"), file_type, file_type0));
            }
        });

        return file_type;
    }

    // Enums all files.
    template<typename f_t> void april_xcompiler_t::__each_files(f_t f)
    {
        for(const string_t & file : __options.files)
        {
            each_files(file, f, true);
        }
    }

    // Compile solutions.
    bool april_xcompiler_t::__compile_solutions()
    {
        bool result = true;

        __each_files([&](const __path_t & path) {
            if(file_type_of(path) == compile_file_type_t::solution)
            {
                if(!__compile_solution((solution_config_t *)parse_config(&__xheap, path)))
                    result = false;
            }
        });

        return result;
    }

    // Compiles solution.
    bool april_xcompiler_t::__compile_solution(solution_config_t * solution_config)
    {
        april_compiler_t compiler(solution_config->name);
        __init_compiler(compiler);

        __cassemblies_t cas;

        for(project_config_t * project_config : solution_config->projects)
        {
            project_t * project = compiler.begin_project(project_config->name);

            for(file_config_t * file_config : project_config->files)
            {
                compiler.add_file(file_config->path, file_config->name);
            }

            compiler.commit_project();
            cas.push_back(__record_project(project));
        }

        bool r = compiler.compile(__options.output_path);
        cas.commit();

        return r;
    }

    // Compiles projects.
    bool april_xcompiler_t::__compile_projects()
    {
        april_compiler_t compiler;
        __init_compiler(compiler);

        __cassemblies_t cas;

        __each_files([&](const __path_t & path) {
            if(file_type_of(path) == compile_file_type_t::project)
                cas.push_back(
                    __add_project(compiler, (project_config_t *)parse_config(&__xheap, path))
                );
        });

        bool r = compiler.compile(__options.output_path);
        cas.commit();

        return r;
    }

    // Adds project.
    april_xcompiler_t::__project_cassembly_t *
        april_xcompiler_t::__add_project(april_compiler_t & compiler,
                                          project_config_t * project_config)
    {
        project_t * project = compiler.begin_project();

        for(file_config_t * file_config : project_config->files)
        {
            compiler.add_file(file_config->path, file_config->name);
        }

        compiler.commit_project();
        return __record_project(project);
    }

    // Compiles sources.
    bool april_xcompiler_t::__compile_sources()
    {
        april_compiler_t compiler;
        __init_compiler(compiler);

        project_t * project = compiler.begin_project();

        for(const string_t & file : __options.files)
        {
            compiler.add_file(file);
        }

        compiler.commit_project();
        __project_cassembly_t * ca = __record_project(project);

        bool r = compiler.compile(__options.output_path);
        ca->commit();

        return r;
    }

    // Gets assembly name.
    static string_t __get_assembly_name(const lib::path_t & path)
    {
        string_t file_name = lib::filename(path);
        return lib::truncate_fileext(file_name);
    }

    // Initializer the compiler.
    void april_xcompiler_t::__init_compiler(april_compiler_t & compiler)
    {
        // Append assembly references
        for(const lib::path_t & path : __assemblies)
        {
            string_t name = __get_assembly_name(path);

            file_ref_assembly_t * ref_assembly = memory_t::new_obj<file_ref_assembly_t>(
                &__xheap, compiler.global_context(), name, _T(""), path
            );

            compiler.add_assembly(ref_assembly);
        }
    }

    // Records the assembly.
    april_xcompiler_t::__raw_cassembly_t *
        april_xcompiler_t::__record_assembly(const lib::path_t & path)
    {
        __assemblies.push_back(path);

        string_t name = __get_assembly_name(path);
        __raw_cassembly_t * cassembly = __new_obj<__raw_cassembly_t>(name, path);
        __cassemblies.push_back(cassembly);

        return cassembly;
    }

    // Records the project.
    april_xcompiler_t::__project_cassembly_t *
        april_xcompiler_t::__record_project(project_t * project)
    {
        __project_cassembly_t * cassembly = __new_obj<__project_cassembly_t>(project);
        __cassemblies.push_back(cassembly);

        return cassembly;
    }

    // Returns whether the assembly is executable.
    bool april_xcompiler_t::__cassembly_t::executable()
    {
        if(__executable == __executable_unknown)
            __executable = this->is_executable();

        return __executable == __executable_yes;
    }

    // Returns whether the assembly is executable.
    april_xcompiler_t::__cassembly_executable_t
        april_xcompiler_t::__raw_cassembly_t::is_executable()
    {
        if(get_assembly_type(path.wstring()) == assembly_type_t::executable)
            return __executable_yes;

        return __executable_no;
    }

    // Returns whether the assembly is executable.
    april_xcompiler_t::__cassembly_executable_t
        april_xcompiler_t::__project_cassembly_t::is_executable()
    {
        return __executable;
    }

    // Gets assembly path.
    lib::path_t april_xcompiler_t::__project_cassembly_t::get_assembly_path() const
    {
        return __assembly_path;
    }

    // Commits it.
    void april_xcompiler_t::__project_cassembly_t::commit()
    {
        if(project->assembly->has_entry_point())
            __executable = __executable_yes;
        else
            __executable = __executable_no;

        __assembly_path = project->assembly_path;
    }

    // Commits it.
    void april_xcompiler_t::__cassemblies_t::commit()
    {
        for(__cassembly_t * ca : *this)
        {
            ca->commit();
        }
    }

    // Gets the default executable assembly.
    april_xcompiler_t::__cassembly_t * april_xcompiler_t::__pick_default_execute_assembly()
    {
        std::vector<__cassembly_t *> cassemblies;

        for(__cassembly_t * cassembly : __cassemblies)
        {
            if(cassembly->executable())
                cassemblies.push_back(cassembly);
        }

        if(cassemblies.empty())
            throw _ED(__e_t::no_executable_assembly);

        if(cassemblies.size() == 1)
            return cassemblies[0];

        al::sort(cassemblies, [](__cassembly_t * c1, __cassembly_t * c2) {
            return c1->from < c2->from;
        });

        __cassembly_t * c1 = cassemblies[0], * c2 = cassemblies[1];
        if(c1->from == c2->from)
            throw _ED(__e_t::duplicated_executable_assembly);

        return c1;
    }

    // Gets the apssembly by specified name.
    april_xcompiler_t::__cassembly_t * april_xcompiler_t::__get_assembly(const string_t & name)
    {
        std::vector<__cassembly_t *> cassemblies;
        bool found = false;

        for(__cassembly_t * cassembly : __cassemblies)
        {
            if(cassembly->name != name)
                continue;

            found = true;
            if(cassembly->executable())
                cassemblies.push_back(cassembly);
        }

        if(cassemblies.empty())
        {
            if(!found)
                throw _ED(__e_t::assembly_not_found, name);

            throw _ED(__e_t::assembly_cannot_executable, name);
        }

        if(cassemblies.size() == 1)
            return cassemblies[0];

        al::sort(cassemblies, [](__cassembly_t * c1, __cassembly_t * c2) {
            return c1->from < c2->from;
        });

        __cassembly_t * c1 = cassemblies[0], * c2 = cassemblies[1];
        if(c1->from == c2->from)
            throw _ED(__e_t::duplicated_executable_assembly_, name);

        return c1;
    }

    ////////// ////////// ////////// ////////// /////////

} } }  // X_ROOT_NS::modules::april


