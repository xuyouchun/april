#ifndef __APRIL_COMPILER_H__
#define __APRIL_COMPILER_H__

namespace X_ROOT_NS { namespace modules { namespace april {

    namespace
    {
        using namespace core;
        using namespace compile;

        using namespace rt;
        using namespace exec;
    }

    ////////// ////////// ////////// ////////// /////////

    // April compiler.
    class april_compiler_t : public object_t, public no_copy_ctor_t
    {
    public:

        // Constructor.
        april_compiler_t(const string_t & solution_name = _T("solution1"));

        // Begins a project.
        project_t * begin_project(const string_t & name = _T(""));

        // Adds file to current project.
        void add_file(const lib::path_t & file, const string_t & name = _T(""));

        // Commits project.
        void commit_project();

        // Adds assembly.
        void add_assembly(file_ref_assembly_t * ref_assembly);

        // Compiles.
        bool compile(const string_t & output_path);

        // Sets solution name.
        void set_solution_name(const string_t & name);

        // Returns global context.
        global_context_t & global_context() { return __global_context(); }

    private:
        xheap_t                 __xheap;            // Memory management.
        compiler_t *            __compiler;         // Compiler.
        simple_lang_factory_t   __lang_factory;     // Language factory.
        solution_t *            __solution;         // Solution.
        project_t *             __current_project = nullptr;    // Current project.

        // Returns global context.
        global_context_t & __global_context() { return __compiler->get_context(); }

        // Returns memory.
        memory_t * __get_memory() { return &__xheap; }

        // Creates new object.
        template<typename t, typename ... args_t> t * __new_obj(args_t && ... args)
        {
            return memory_t::new_obj<t>(&__xheap, std::forward<args_t>(args) ...);
        }

        // Returns default project name.
        string_t __get_default_project_name(project_t * project);

        // Returns default output path.
        string_t __get_default_output_path();
    };

    ////////// ////////// ////////// ////////// /////////
    // Class april_xcompiler_t

    typedef april_options_t::compile_options_t __compile_options_t;

    // April compiler with multipy sub copilers.
    class april_xcompiler_t : public object_t
    {
    public:

        // Constructor.
        april_xcompiler_t(const __compile_options_t & options);

        // Compiles, returns true if succeed.
        bool compile();

        // Run the specified assembly.
        bool run(const string_t & execute_assembly = _T(""));

    private:
        class __cassembly_t;
        class __raw_cassembly_t;
        class __project_cassembly_t;

        const __compile_options_t & __options;
        std::vector<lib::path_t> __assemblies;

        // Determines the compile type.
        compile_file_type_t __determine_compile_type();

        // Enums all files.
        template<typename f_t> void __each_files(f_t f);

        // Compile solutions.
        bool __compile_solutions();

        // Compile specified solution.
        bool __compile_solution(solution_config_t * solution_config);

        // Compile projects.
        bool __compile_projects();

        // Adds a project.
        __project_cassembly_t * __add_project(april_compiler_t & compiler,
                                project_config_t * project_config);

        // Compiles sources.
        bool __compile_sources();

        // Initialize compiler.
        void __init_compiler(april_compiler_t & compiler);

        // Records an assembly.
        __raw_cassembly_t * __record_assembly(const lib::path_t & path);

        // Records a project.
        __project_cassembly_t * __record_project(project_t * project);

        // Runs the specified assembly.
        void __run(const string_t & execute_assembly);

        // Memory manager.
        xheap_t __xheap;

        // Creates a new object.
        template<typename _t, typename ... _args_t>
        _t * __new_obj(_args_t && ... args)
        {
            return memory_t::new_obj<_t>(&__xheap, std::forward<_args_t>(args) ...);
        }

        enum __cassembly_from_t { __from_project, __from_assembly };
        enum __cassembly_executable_t { __executable_yes, __executable_no, __executable_unknown };

        //-------- ---------- ---------- ---------- ----------

        // Base class for assembly information.
        class __cassembly_t : public object_t
        {
        public:

            // Constructor.
            __cassembly_t(const string_t & name, __cassembly_from_t from)
                : name(name), from(from)
           { }

            string_t            name;
            __cassembly_from_t  from;

            // Returns whether it's exeutable.
            bool executable();

            // Returns assembly path.
            virtual lib::path_t get_assembly_path() const = 0;

            // Commits it.
            virtual void commit() = 0;

            virtual const string_t to_string() const override
            {
                return _T("cassembly");
            }

        protected:
            virtual __cassembly_executable_t is_executable() = 0;

        private:
            __cassembly_executable_t __executable = __executable_unknown;
        };

        //-------- ---------- ---------- ---------- ----------

        // Information for raw assembly.
        class __raw_cassembly_t : public __cassembly_t
        {
            typedef __cassembly_t __super_t;

        public:

            // Constructor.
            __raw_cassembly_t(const string_t & name, const lib::path_t & path)
                : __super_t(name, __from_assembly), path(path)
            { }

            lib::path_t path;

            // Gets assembly path.
            virtual lib::path_t get_assembly_path() const override { return path; }

            // Commits it.
            virtual void commit() override { }

            virtual const string_t to_string() const override
            {
                return _F(_T("raw assembly: %1%"), get_assembly_path());
            }

        protected:

            // Returns whether it's executable.
            virtual __cassembly_executable_t is_executable() override;
        };

        //-------- ---------- ---------- ---------- ----------

        // Information for project.
        class __project_cassembly_t : public __cassembly_t
        {
            typedef __cassembly_t __super_t;

        public:

            // Constructor.
            __project_cassembly_t(project_t * project)
                : __super_t(project->name, __from_project), project(project)
            { }

            project_t * project;

            // Gets assembly path.
            virtual lib::path_t get_assembly_path() const override;

            // Commits it.
            virtual void commit() override;

            virtual const string_t to_string() const override
            {
                return _F(_T("project assembly: %1%"), get_assembly_path());
            }

        protected:

            // Returns whether it's executable.
            virtual __cassembly_executable_t is_executable() override;

        private:
            __cassembly_executable_t __executable = __executable_unknown;
            lib::path_t __assembly_path;
        };

        //-------- ---------- ---------- ---------- ----------

        // Assembly informations.
        class __cassemblies_t : public std::vector<__cassembly_t *>
        {
        public:

            // Commits it.
            void commit();
        };

        //-------- ---------- ---------- ---------- ----------

        std::vector<__cassembly_t *> __cassemblies;

        // Gets default execute assembly.
        __cassembly_t * __pick_default_execute_assembly();

        // Gets assembly of speicified name.
        __cassembly_t * __get_assembly(const string_t & name);
    };

    ////////// ////////// ////////// ////////// /////////

} } }  // X_ROOT_NS::modules::april

#endif // __APRIL_COMPILER_H__
