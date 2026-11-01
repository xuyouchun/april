
#include <april.h>
#include "util.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace X_ROOT_NS::modules::april {

    using namespace core;
    using namespace compile;

    using namespace rt;
    using namespace exec;
    using namespace mm;

    ////////// ////////// ////////// ////////// /////////
    // april_compiler_t

    // Constructor.
    april_compiler_t::april_compiler_t(const string_t & solution_name,
                                       method_compile_controller_t * controller)
        : __xheap(_T("april_compiler")), __controller(controller)
    {
        __lang_factory.register_lang_t<lang_cs::cs_lang_t>();
        __lang_factory.register_lang_t<lang_lq::lq_lang_t>();

        __compiler = __new_obj<compiler_t>(&__lang_factory, __controller);
        __solution = __new_obj<solution_t>(__global_context(), solution_name);
    }

    // Begins a project.
    project_t * april_compiler_t::begin_project(const string_t & name)
    {
        if (__current_project != nullptr)
            throw _ED(__e_t::project_not_committed, __current_project);

        __current_project = __new_obj<project_t>(__global_context(), name);
        return __current_project;
    }

    // Adds file to current project.
    void april_compiler_t::add_file(const lib::path_t & file, const string_t & name)
    {
        if (__current_project == nullptr)
            throw _ED(__e_t::no_current_project);

        std::vector<__path_t> files;
        each_files(file, [&](const __path_t & file_path) {
            files.push_back(file_path);
        }, true);

        std::sort(files.begin(), files.end());

        for (__path_t & file_path : files)
        {
            compile_file_type_t file_type = file_type_of(file_path);
            if (file_type != compile_file_type_t::source)
                return;

            if (!__current_project->exists(lib::to_str(file_path)))
            {
                file_t * file = file_t::load(__global_context(), __get_memory(),
                    file_path, name.c_str()
                );

                __current_project->append_file(file);
            }
        }
    }

    // Commits project.
    void april_compiler_t::commit_project()
    {
        if (__current_project == nullptr)
            throw _ED(__e_t::no_project_need_commit);

        if (__current_project->empty())
            throw _ED(__e_t::project_no_files, __current_project->name);

        if (__current_project->name == _T(""))
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
        if (__current_project != nullptr)
            throw _ED(__e_t::project_not_committed, __current_project);

        if (__solution->empty())
            throw _ED(__e_t::no_projects);

        string_t opath = output_path;
        if (opath.length() == 0)
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

        if (!__solution->exists(dir_name))
            return dir_name;

        for (int index = 1; ; index++)
        {
            string_t dir_name0 = _F(_T("%1%%2%"), dir_name, index);
            if (!__solution->exists(dir_name0))
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
            if (it != __map.end())
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

    // Gets assembly name.
    static string_t __get_assembly_name(const lib::path_t & path);

    // Constructor.
    april_xcompiler_t::april_xcompiler_t(const __compile_options_t & options)
        : __options(options), __xheap(_T("april_xcompiler")), __controller(__options)
    { }

    // Compiles.
    bool april_xcompiler_t::compile()
    {
        return __compile_type(__prepare());
    }

    // Compiles the files of the type.
    bool april_xcompiler_t::__compile_type(compile_file_type_t type)
    {
        switch (type)
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

    // Determines the compile type, and adds System.apl.
    compile_file_type_t april_xcompiler_t::__prepare()
    {
        compile_file_type_t type = __determine_compile_type();

        if (!__is_system_project())
            __record_assembly(lib::path_t(find_system_assembly().string()));

        return type;
    }

    // Returns whether it is the project System that is compiled.
    bool april_xcompiler_t::__is_system_project()
    {
        bool found = false;

        __each_files([&](const __path_t & path) {
            compile_file_type_t type = file_type_of(path);

            if (type == compile_file_type_t::project)
            {
                project_config_t * config = (project_config_t *)parse_config(&__xheap, path);
                found = found || config->name == _T("System");
            }
            else if (type == compile_file_type_t::solution)
            {
                solution_config_t * config = (solution_config_t *)parse_config(&__xheap, path);
                for (project_config_t * project_config : config->projects)
                    found = found || project_config->name == _T("System");
            }
        });

        return found;
    }

    ////////// ////////// ////////// ////////// /////////
    // The builds of the programs that are run, or that are built native.

    namespace fs = std::filesystem;

    // The time of a file as a number.
    static uint64_t __stamp_of(const fs::path & path)
    {
        std::error_code ec;
        auto time = fs::last_write_time(path, ec);

        return ec? 0 : (uint64_t)time.time_since_epoch().count();
    }

    // Adds the size and the time of a file.
    static void __add_stamp(hasher_t & hasher, const fs::path & path)
    {
        std::error_code ec;
        hasher.add(path.filename().string());
        hasher.add((uint64_t)fs::file_size(path, ec));
        hasher.add(__stamp_of(path));
    }

    // Returns the key of the build.
    std::string april_xcompiler_t::__cache_key(compile_file_type_t type)
    {
        hasher_t hasher;
        hasher.add(std::string("april-build-1"));

        // The compiler: a program that is built by another one is built again.
        std::string exe = native::executable_path();
        if (!exe.empty())
            __add_stamp(hasher, exe);

        hasher.add((uint64_t)__options.optimize);
        hasher.add((uint64_t)__options.native_optimize);
        hasher.add(string_convert<char_t, char>(__options.execute_assembly));
        hasher.add((uint64_t)__options.native_no_pie);
        hasher.add(string_convert<char_t, char>(__options.native_clang));
        hasher.add(string_convert<char_t, char>(__options.native_libs));

        // The JIT library: the object files are those of its LLVM.
        std::string jit = native::jit_library_path();
        if (!jit.empty())
            __add_stamp(hasher, jit);

        // The libraries of the executables.
        try
        {
            native::build_options_t bo;
            bo.libs_dir = string_convert<char_t, char>(__options.native_libs);

            for (const std::string & lib : native::list_static_libs(native::find_libs_dir(bo)))
                __add_stamp(hasher, lib);
        }
        catch (const native::driver_error_t &)
        {
            // There is no native build then.
        }

        // The assemblies that are given, and System.apl.
        for (const lib::path_t & path : __assemblies)
            hasher.add_file(path.string());

        // The sources: the files, and the files of the projects and of the solutions.
        __each_files([&](const __path_t & path) {
            compile_file_type_t file_type = file_type_of(path);
            if (file_type == compile_file_type_t::assembly)
                return;

            hasher.add_file(path.string());

            if (file_type == compile_file_type_t::source)
                return;

            std::vector<std::string> sources;
            std::error_code ec;

            for (fs::recursive_directory_iterator it(fs::path(path.string()).parent_path(), ec), end;
                        !ec && it != end; it.increment(ec))
            {
                std::string ext = it->path().extension().string();
                if (it->is_regular_file(ec) && (ext == ".cs" || ext == ".lq"))
                    sources.push_back(it->path().string());
            }

            std::sort(sources.begin(), sources.end());
            for (const std::string & source : sources)
                hasher.add_file(source);
        });

        return hasher.hex();
    }

    // Compiles into the directory of the builds.
    bool april_xcompiler_t::__prepare_build(__build_t & build)
    {
        compile_file_type_t type = __prepare();

        fs::path dir = cache_root() / __cache_key(type);
        fs::path manifest = dir / "manifest.txt";
        std::error_code ec;

        if (!fs::is_regular_file(manifest, ec))
        {
            fs::remove_all(dir, ec);
            fs::create_directories(dir, ec);

            if (ec)
                throw _ED(__e_t::cache_directory_failed, dir.wstring());

            if (type != compile_file_type_t::none)
            {
                __options.output_path = dir.wstring();

                if (!__compile_type(type))
                {
                    fs::remove_all(dir, ec);
                    return false;
                }
            }

            // The assembly that is run, and the assemblies that are made.
            __cassembly_t * main_cassembly;
            if (__options.execute_assembly.empty())
                main_cassembly = __pick_default_execute_assembly();
            else
                main_cassembly = __get_assembly(__options.execute_assembly);

            std::ofstream out(manifest.string(), std::ios::binary);
            out << "main=" << main_cassembly->get_assembly_path().string() << "\n";

            for (__cassembly_t * ca : __cassemblies)
            {
                if (ca->from == __from_project)
                {
                    out << "asm=" << string_convert<char_t, char>(ca->name) << "\t"
                        << ca->get_assembly_path().string() << "\n";
                }
            }
        }

        // The build: the assemblies of the manifest, and those that were given.
        build.dir = lib::path_t(dir.string());

        std::ifstream in(manifest.string(), std::ios::binary);
        std::string line;

        while (std::getline(in, line))
        {
            if (line.compare(0, 5, "main=") == 0)
            {
                build.main = lib::path_t(line.substr(5));
            }
            else if (line.compare(0, 4, "asm=") == 0)
            {
                size_t tab = line.find('\t');
                if (tab != std::string::npos)
                {
                    build.assemblies.push_back(std::make_pair(
                        string_convert<char, char_t>(line.substr(4, tab - 4)),
                        lib::path_t(line.substr(tab + 1))
                    ));
                }
            }
        }

        for (__cassembly_t * ca : __cassemblies)
        {
            if (ca->from == __from_assembly)
                build.assemblies.push_back(std::make_pair(ca->name, ca->get_assembly_path()));
        }

        return true;
    }

    // Returns the LLVM IR of the build, and writes it next to it.
    std::string april_xcompiler_t::__generate_ir(const __build_t & build)
    {
        __rt_assembly_provider_t provider(build.main);
        for (auto & assembly : build.assemblies)
            provider.append(assembly.first, assembly.second);

        // Only reads metadata: no managed objects are created, any heap will do.
        rt_context_t rt_ctx(&__xheap, create_heap(&__xheap), &provider);
        std::string ir = native::generate_ir(rt_ctx);

        fs::path ir_path = fs::path(build.dir.string()) / "program.ll";

        std::ofstream out(ir_path.string(), std::ios::binary);
        if (!out)
            throw _ED(__e_t::open_file_failed, ir_path.wstring());

        out << ir;
        return ir;
    }

    // Returns the object file of the build, for the JIT.
    std::string april_xcompiler_t::__native_object(const __build_t & build,
                                                   native::jit_backend_t * jit)
    {
        fs::path object = fs::path(build.dir.string()) / "program.o";
        std::error_code ec;

        if (fs::is_regular_file(object, ec))
            return object.string();

        std::string ir = __generate_ir(build);

        // The file appears when it is complete.
        fs::path partial = object;
        partial += ".tmp";

        std::string error;
        if (!jit->compile(ir, __options.native_optimize, partial.string(), error))
            throw native::driver_error_t(error);

        fs::rename(partial, object, ec);
        if (ec)
            throw native::driver_error_t("cannot make '" + object.string() + "': " + ec.message());

        return object.string();
    }

    // Returns the executable of the build.
    std::string april_xcompiler_t::__native_executable(const __build_t & build)
    {
        fs::path dir(build.dir.string());
        fs::path marker = dir / "native.txt";
        std::error_code ec;

        // It was built.
        {
            std::ifstream in(marker.string(), std::ios::binary);
            std::string exe;

            if (std::getline(in, exe) && fs::is_regular_file(exe, ec))
                return exe;
        }

        __generate_ir(build);
        fs::path ir_path = dir / "program.ll";

        native::build_options_t bo;
        bo.ir_path  = ir_path.string();

        // The object file of the JIT library: no clang is needed to compile.
        {
            std::string reason;
            if (native::jit_backend_t * jit = native::jit_backend_t::load(reason))
                bo.object_path = __native_object(build, jit);
        }

        bo.optimize = __options.native_optimize;
        bo.no_pie   = __options.native_no_pie;
        bo.clang    = string_convert<char_t, char>(__options.native_clang);
        bo.libs_dir = string_convert<char_t, char>(__options.native_libs);

        if (__options.verbose)
            bo.log = [](const std::string & cmd) { std::cerr << "native: " << cmd << std::endl; };

        std::string exe = native::build_executable(bo);

        std::ofstream marker_out(marker.string(), std::ios::binary);
        marker_out << exe << "\n";

        return exe;
    }

    // Runs the program.
    bool april_xcompiler_t::run(bool interpret)
    {
        __build_t build;
        if (!__prepare_build(build))
            return false;

        if (!interpret)
        {
            std::string exe;

            try
            {
                // The JIT: the program is run in this process.
                std::string reason;
                native::jit_backend_t * jit = native::jit_backend_t::load(reason);

                if (jit != nullptr)
                {
                    std::string object = __native_object(build, jit);

                    int code = 0;
                    std::string error;

                    if (jit->run(object, code, error))
                    {
                        if (code != 0)
                            std::exit(code);

                        return true;
                    }

                    throw native::driver_error_t(error);
                }

                if (__options.verbose)
                    std::cerr << "native: no JIT: " << reason << std::endl;

                exe = __native_executable(build);
            }
            catch (const logic_error_t<native::native_error_code_t> & e)
            {
                // The output of the program is its own: tell why only when it is asked.
                if (__options.verbose)
                {
                    std::wcerr << _T("native: ") << e.get_message()
                               << _T("; the program is run with the interpreter") << std::endl;
                }

                interpret = true;
            }
            catch (const native::driver_error_t & e)
            {
                if (__options.verbose)
                {
                    std::cerr << "native: " << e.what()
                              << "; the program is run with the interpreter" << std::endl;
                }

                interpret = true;
            }

            if (!interpret)
            {
                int code = native::run_executable(exe);
                if (code != 0)
                    std::exit(code);

                return true;
            }
        }

        return __run_interpreter(build);
    }

    // Compiles, and builds a native executable.
    bool april_xcompiler_t::compile_native()
    {
        string_t output = __options.output_path;

        __build_t build;
        if (!__prepare_build(build))
            return false;

        std::string exe;
        try
        {
            exe = __native_executable(build);
        }
        catch (const native::driver_error_t & e)
        {
            std::cerr << "native: error: " << e.what() << std::endl;
            return false;
        }

        // -o is the executable, or the directory of it.
        fs::path target = string_convert<char_t, char>(output);
        std::error_code ec;

        // By default: next to the first source file or project file, with its name.
        fs::path first;
        __each_files([&](const __path_t & path) {
            if (first.empty() && file_type_of(path) != compile_file_type_t::assembly)
                first = fs::path(path.string());
        });

        std::string name = (first.empty()? fs::path(build.main.string()) : first).stem().string();
        std::string ext = fs::path(exe).extension().string();
        if (ext != ".exe")
            ext.clear();

        if (target.empty())
        {
            fs::path dir = fs::absolute(first, ec).parent_path();
            target = (first.empty()? fs::current_path() : dir) / (name + ext);
        }
        else if (fs::is_directory(target, ec))
        {
            target = target / (name + ext);
        }

        // No "." or ".." in the path.
        target = fs::absolute(target, ec).lexically_normal();

        fs::copy_file(exe, target, fs::copy_options::overwrite_existing, ec);
        if (ec)
        {
            std::cerr << "native: cannot write '" << target.string() << "': "
                      << ec.message() << std::endl;
            return false;
        }

        std::cout << "native: built " << target.string() << std::endl;
        return true;
    }

    // Runs the main assembly of the build with the interpreter.
    bool april_xcompiler_t::__run_interpreter(const __build_t & build)
    {
        __rt_assembly_provider_t provider(build.main);

        for (auto & assembly : build.assemblies)
            provider.append(assembly.first, assembly.second);

        // Uses the garbage collected heap by default;
        // APRIL_GC=0 switches back to the never-freeing default heap.
        const char * gc_env = std::getenv("APRIL_GC");
        rt_heap_t * heap = (gc_env != nullptr && *gc_env == '0')
                ? create_heap(&__xheap)
                : create_gc_heap(&__xheap);

        rt_context_t rt_ctx(&__xheap, heap, &provider);
        return exec::execute(rt_ctx);
    }

    // Determines the compile type.
    compile_file_type_t april_xcompiler_t::__determine_compile_type()
    {
        compile_file_type_t file_type = compile_file_type_t::none;

        __each_files([&](const __path_t & path) {

            compile_file_type_t file_type0 = file_type_of(path);
            if (file_type0 == compile_file_type_t::assembly)
            {
                // System.apl is used by every program, it is added by the compiler.
                if (__get_assembly_name(path) == _T("System"))
                    throw _ED(__e_t::system_assembly_specified, path);

                __record_assembly(path);
            }
            else
            {
                if (file_type == compile_file_type_t::none)
                    file_type = file_type0;
                else if (file_type != file_type0)
                    throw _ED(__e_t::mixture_file_types, _F(_T("%1%, %2%"), file_type, file_type0));
            }
        });

        return file_type;
    }

    // Enums all files.
    template<typename f_t> void april_xcompiler_t::__each_files(f_t f)
    {
        for (const string_t & file : __options.files)
        {
            each_files(file, f, true);
        }
    }

    // Compile solutions.
    bool april_xcompiler_t::__compile_solutions()
    {
        bool result = true;

        __each_files([&](const __path_t & path) {
            if (file_type_of(path) == compile_file_type_t::solution)
            {
                if (!__compile_solution((solution_config_t *)parse_config(&__xheap, path)))
                    result = false;
            }
        });

        return result;
    }

    // Compiles solution.
    bool april_xcompiler_t::__compile_solution(solution_config_t * solution_config)
    {
        april_compiler_t compiler(solution_config->name, &__controller);
        __init_compiler(compiler);

        __cassemblies_t cas;

        for (project_config_t * project_config : solution_config->projects)
        {
            project_t * project = compiler.begin_project(project_config->name);

            for (file_config_t * file_config : project_config->files)
            {
                compiler.add_file(file_config->path, file_config->name);
            }

            compiler.commit_project();
            cas.push_back(__record_project(project));
        }

        bool r = compiler.compile(__output_path());
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
            if (file_type_of(path) == compile_file_type_t::project)
                cas.push_back(
                    __add_project(compiler, (project_config_t *)parse_config(&__xheap, path))
                );
        });

        bool r = compiler.compile(__output_path());
        cas.commit();

        return r;
    }

    // Adds project.
    april_xcompiler_t::__project_cassembly_t *
        april_xcompiler_t::__add_project(april_compiler_t & compiler,
                                          project_config_t * project_config)
    {
        project_t * project = compiler.begin_project();

        for (file_config_t * file_config : project_config->files)
        {
            compiler.add_file(file_config->path, file_config->name);
        }

        compiler.commit_project();
        return __record_project(project);
    }

    // The first file that is compiled (not an assembly).
    lib::path_t april_xcompiler_t::__first_file()
    {
        __path_t first;

        __each_files([&](const __path_t & path) {
            if (first.empty() && file_type_of(path) != compile_file_type_t::assembly)
                first = path;
        });

        return first;
    }

    // The directory of the output: -o, or the directory of the first file.
    string_t april_xcompiler_t::__output_path()
    {
        if (!__options.output_path.empty())
            return __options.output_path;

        __path_t first = __first_file();
        if (first.empty())
            return string_t();

        std::error_code ec;
        return string_convert<char, char_t>(
            fs::absolute(fs::path(first.string()), ec).parent_path().string()
        );
    }

    // Compiles sources.
    bool april_xcompiler_t::__compile_sources()
    {
        april_compiler_t compiler;
        __init_compiler(compiler);

        // The assembly is named after the first file.
        project_t * project = compiler.begin_project(
            string_convert<char, char_t>(__first_file().stem().string())
        );

        for (const string_t & file : __options.files)
        {
            compiler.add_file(file);
        }

        compiler.commit_project();
        __project_cassembly_t * ca = __record_project(project);

        bool r = compiler.compile(__output_path());
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
        for (const lib::path_t & path : __assemblies)
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
        if (__executable == __executable_unknown)
            __executable = this->is_executable();

        return __executable == __executable_yes;
    }

    // Returns whether the assembly is executable.
    april_xcompiler_t::__cassembly_executable_t
        april_xcompiler_t::__raw_cassembly_t::is_executable()
    {
        if (get_assembly_type(path.wstring()) == assembly_type_t::executable)
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
        if (project->assembly->has_entry_point())
            __executable = __executable_yes;
        else
            __executable = __executable_no;

        __assembly_path = project->assembly_path;
    }

    // Commits it.
    void april_xcompiler_t::__cassemblies_t::commit()
    {
        for (__cassembly_t * ca : *this)
        {
            ca->commit();
        }
    }

    // Gets the default executable assembly.
    april_xcompiler_t::__cassembly_t * april_xcompiler_t::__pick_default_execute_assembly()
    {
        std::vector<__cassembly_t *> cassemblies;

        for (__cassembly_t * cassembly : __cassemblies)
        {
            if (cassembly->executable())
                cassemblies.push_back(cassembly);
        }

        if (cassemblies.empty())
            throw _ED(__e_t::no_executable_assembly);

        if (cassemblies.size() == 1)
            return cassemblies[0];

        al::sort(cassemblies, [](__cassembly_t * c1, __cassembly_t * c2) {
            return c1->from < c2->from;
        });

        __cassembly_t * c1 = cassemblies[0], * c2 = cassemblies[1];
        if (c1->from == c2->from)
            throw _ED(__e_t::duplicated_executable_assembly);

        return c1;
    }

    // Gets the apssembly by specified name.
    april_xcompiler_t::__cassembly_t * april_xcompiler_t::__get_assembly(const string_t & name)
    {
        std::vector<__cassembly_t *> cassemblies;
        bool found = false;

        for (__cassembly_t * cassembly : __cassemblies)
        {
            if (cassembly->name != name)
                continue;

            found = true;
            if (cassembly->executable())
                cassemblies.push_back(cassembly);
        }

        if (cassemblies.empty())
        {
            if (!found)
                throw _ED(__e_t::assembly_not_found, name);

            throw _ED(__e_t::assembly_cannot_executable, name);
        }

        if (cassemblies.size() == 1)
            return cassemblies[0];

        al::sort(cassemblies, [](__cassembly_t * c1, __cassembly_t * c2) {
            return c1->from < c2->from;
        });

        __cassembly_t * c1 = cassemblies[0], * c2 = cassemblies[1];
        if (c1->from == c2->from)
            throw _ED(__e_t::duplicated_executable_assembly_, name);

        return c1;
    }

    // Returns whether specified code need optimized.
    bool april_xcompiler_t::__method_compile_controller_t::optimize(int code)
    {
        int optimize_level = __options.optimize;

        #define __Case(_code, _level)                                                   \
            case compile_optimize_code_t::_code:                                        \
                return optimize_level >= _level;

        switch ((compile_optimize_code_t)code)
        {
            __Case(remove_unreached_codes,              2)
            __Case(convert_switch_to_if_statement,      2)
            __Case(auto_determine_constant_variables,   2)
            __Case(compute_constant_values,             1)
            __Case(optimize_basic_algorighm,            2)
            __Case(remove_unnecessary_expressions,      1)

            default:
                return optimize_level >= 1;
        }
    }

    ////////// ////////// ////////// ////////// /////////

}   // X_ROOT_NS::modules::april


