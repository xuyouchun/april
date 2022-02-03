
#ifndef __APRIL_H__
#define __APRIL_H__

#include <common.h>
#include <arch.h>
#include <lib.h>
#include <algorithm.h>
#include <modules/compile.h>
#include <modules/rt.h>
#include <modules/mm.h>
#include <modules/lang_cs.h>
#include <modules/lang_lq.h>
#include <modules/exec.h>

#include <external/cmdline/cmdline.h>

namespace X_ROOT_NS::modules::april {

    typedef cmdline::parser __cmdline_parser_t;

    ////////// ////////// ////////// ////////// /////////
    
    // Constants.
    const char_t april_name[]           = _T("april");
    const char_t solution_file_ext[]    = _T("solution");
    const char_t project_file_ext[]     = _T("project");
    const char_t april_file_ext[]       = JC_ASSEMBLY_EXT;

    ////////// ////////// ////////// ////////// /////////
    
    // April commands.
    X_ENUM(april_command_t)

        // Compile.
        compile,

        // Run.
        run,

        // Version.
        version,

        // Help.
        help,

    X_ENUM_END

    ////////// ////////// ////////// ////////// /////////

    // April error codes.
    X_ENUM(april_error_code_t)

        // No files.
        no_files,

        // Project not committed.
        project_not_committed,

        // No project need commit.
        no_project_need_commit,

        // No current project.
        no_current_project,

        // File not exists.
        file_not_exists,

        // No projects.
        no_projects,

        // Mixture file types.
        mixture_file_types,

        // Open file failed.
        open_file_failed,

        // Unknown file type.
        unknown_file_type,

        // Configurate format error.
        config_format_error,

        // Project has no files.
        project_no_files,

        // Assembly not found.
        assembly_not_found,

        // Assembly cannot executable.
        assembly_cannot_executable,

        // Duplicated executable assembly.
        duplicated_executable_assembly_,

        // Duplicated executable assembly.
        duplicated_executable_assembly,

        // No executable assembly.
        no_executable_assembly,

        // Optimize argument error.
        optimize_argument_error,

    X_ENUM_END

    ////////// ////////// ////////// ////////// /////////

    // Compile file types.
    X_ENUM(compile_file_type_t)

        none = __default__,

        // Source.
        source,

        // Project.
        project,

        // Solution.
        solution,

        // Assebly.
        assembly,

    X_ENUM_END

    ////////// ////////// ////////// ////////// /////////

    namespace
    {
        template<bool _is_object = true> struct __unions_init_t
        {
            // Initialize buffer with specified arguments.
            template<typename _obj_t, typename ... _args_t>
            static void init(void * buffer, object_t * & out_obj, _args_t && ... args)
            {
                out_obj = new (buffer) _obj_t(std::forward<_args_t>(args) ...);
            }
        };

        template<> struct __unions_init_t<false>
        {
            // Initialize buffer with specified arguments.
            template<typename _obj_t, typename ... _args_t>
            static void init(void * buffer, object_t * & out_obj, _args_t && ... args)
            {
                static_assert(
                    std::is_trivially_destructible<_obj_t>::value,
                    "not an object class, it's destructor will not be executed"
                );

                new (buffer) _obj_t(std::forward<_args_t>(args) ...);
            }
        };
    }

    // Unions
    template<typename ... _objs_t> class unions_t
    {
    public:
        static const size_t size = al::max(sizeof(_objs_t) ...);

        // Initialize.
        template<typename _obj_t, typename ... _args_t>
        _obj_t * init(_args_t && ... args)
        {
            __destroy_obj();

            typedef __unions_init_t<
                std::is_base_of<object_t, _obj_t>::value
            > init_t;

            init_t::template init<_obj_t>((void *)__buffer, obj, std::forward<_args_t>(args) ...);
            return get<_obj_t>();
        }

        // Gets object of specified type.
        template<typename _obj_t> _obj_t * get() const
        {
            return (_obj_t *)obj;
        }

        // Destructor.
        ~unions_t()
        {
            __destroy_obj();
        }

    private:
        byte_t __buffer[size];
        object_t * obj = nullptr;

        // Destory the object.
        void __destroy_obj()
        {
            if (obj != nullptr)
            {
                obj->~object_t();
                obj = nullptr;
            }
        }
    };
    
    ////////// ////////// ////////// ////////// /////////

    // April options.
    class april_options_t : public object_t
    {
    public:

        // Constructor.
        april_options_t();

        // Parses commands.
        bool parse(int argc, char ** argv);

        // Shows help.
        void show_help(april_command_t command);

        // Shows help.
        void show_help();

        april_command_t command;

        //-------- ---------- ---------- ---------- ----------

        // Compiling options.
        struct compile_options_t : object_t
        {
            std::vector<string_t>   files;
            string_t                output_path;
            int                     optimize;
        };

        //-------- ---------- ---------- ---------- ----------

        // Running options.
        struct run_options_t : compile_options_t
        {
            string_t execute_assembly;
        };

        //-------- ---------- ---------- ---------- ----------

        // Help options.
        struct help_options_t : object_t
        {
            string_t sub_command;
        };

        //-------- ---------- ---------- ---------- ----------

        // Initialize options.
        template<typename _options_t> _options_t & init_options()
        {
            return *__unions.init<_options_t>();
        }

        // Gets options.
        template<typename _options_t> _options_t & get() const
        {
            return *__unions.get<_options_t>();
        }

        X_TO_STRING_IMPL(_T("april_options_t"))

    private:
        unions_t<compile_options_t, run_options_t> __unions;

        // Parses running options.
        bool __parse_run(__cmdline_parser_t & parser, int argc, char ** argv);

        // Parses compling options.
        bool __parse_compile(__cmdline_parser_t & parser, int argc, char ** argv);

        // Parses helping options.
        bool __parse_help(__cmdline_parser_t & parser, int argc, char ** argv);

        // Parses version options.
        bool __parse_version(__cmdline_parser_t & parser, int argc, char ** argv);

        // Initializes compile parser.
        void __init_compile_parser(__cmdline_parser_t & parser);

        // Reads compile options.
        bool __read_compile_options(__cmdline_parser_t & parser, compile_options_t & options);

        // Parses command.
        bool __parse(april_command_t commad, __cmdline_parser_t & parser, int argc, char ** argv);

        // copies string.
        void __copy_strings(const std::vector<std::string> & src, std::vector<string_t> & dst);
    };

    ////////// ////////// ////////// ////////// /////////

    // The main april executing class.
    class april_t : public object_t
    {
    public:

        // Constructor.
        april_t(april_options_t & options);

        // Executes.
        void execute();

        X_TO_STRING_IMPL(_T("april_t"))

    private:
        april_options_t & __options;

        typedef april_options_t __t;

        // Executes compiling command.
        void __execute_compile(const __t::compile_options_t & options);

        // Executes running command.
        void __execute_run(const __t::run_options_t & options);

        // Executes helping command.
        void __execute_help(const __t::help_options_t & options);
    };

    ////////// ////////// ////////// ////////// /////////

}   // X_ROOT_NS::modules::april


#include <modules/april/__parser.h>
#include <modules/april/__compiler.h>

#endif  // __APRIL_H__

