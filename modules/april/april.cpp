
#include <april.h>
#include <lib.h>

namespace X_ROOT_NS { namespace modules { namespace april {

    typedef april_error_code_t __e_t;

    ////////// ////////// ////////// ////////// /////////

    // April commands.
    X_ENUM_INFO(april_command_t)

        // Compile command.
        X_C(compile,        _T("compile"))

        // Run command.
        X_C(run,            _T("run"))

        // Help command.
        X_C(help,           _T("help"))

        // Version command.
        X_C(version,        _T("version"))

    X_ENUM_INFO_END

    // Returns all commands.
    static string_t __all_commands()
    {
        std::vector<string_t> commands;
        each(april_command_t(), [&](auto & it) {
            if (it.value != april_command_t::__default__ && it.value != april_command_t::__unknown__)
                commands.push_back(it.title);
        });

        al::sort(commands);
        return al::join_str(commands.begin(), commands.end(), _T(" | "));
    }

    ////////// ////////// ////////// ////////// /////////

    // Aprils error codes.
    X_ENUM_INFO(april_error_code_t)

        // No files.
        X_D(no_files,               _T("no file specified"))

        // Projects not committed.
        X_D(project_not_committed,  _T("project '%1%' not committed"))

        // No project need committed.
        X_D(no_project_need_commit, _T("no project need commit"))

        // No current project.
        X_D(no_current_project,     _T("no current project"))

        // File not exists.
        X_D(file_not_exists,        _T("file '%1%' not exists"))

        // No projects.
        X_D(no_projects,            _T("no project specified"))

        // Mixture file types.
        X_D(mixture_file_types,     _T("muxture file types: %1%"))

        // Opens file failed.
        X_D(open_file_failed,       _T("cannot open file %2%"))

        // Unknown file type.
        X_D(unknown_file_type,      _T("unknown file type: %1%"))

        // Configuration format error.
        X_D(config_format_error,    _T("config file format error: %1%"))

        // Project no files.
        X_D(project_no_files,       _T("project '%1%' has no files"))

        // Assembly not found.
        X_D(assembly_not_found,     _T("assembly '%1%' not found"))

        // Assembly cannot executable.
        X_D(assembly_cannot_executable,         _T("assembly '%1%' cannot executable"))

        // Duplicated executable assembly.
        X_D(duplicated_executable_assembly_,    _T("duplicated assemblies '%1%'"))

        // Duplicated executable assembly.
        X_D(duplicated_executable_assembly,     _T("duplicated assemblies"))

        // No executable assembly.
        X_D(no_executable_assembly, _T("no executable assembly"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// /////////

    // Compile file types.
    X_ENUM_INFO(compile_file_type_t)

        X_C(none,       _T("none"))

        // Source.
        X_C(source,     _T("source"))

        // Project.
        X_C(project,    _T("project"))

        // Solution.
        X_C(solution,   _T("solution"))

        // Assembly.
        X_C(assembly,   _T("assembly"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// /////////

    // Reads string.
    static string_t __read_string(__cmdline_parser_t & parser, const char * name)
    {
        return string_convert<char, char_t>(parser.get<std::string>(name));
    }

    ////////// ////////// ////////// ////////// /////////

    // Constructor.
    april_options_t::april_options_t()
    {
        
    }

    const string_t __help_message = _T("                                    \n\
April is a tool for managing April source code.                             \n\
                                                                            \n\
Usage:                                                                      \n\
                                                                            \n\
        april command [arguments]                                           \n\
                                                                            \n\
The commands are:                                                           \n\
                                                                            \n\
        build       compile packages and dependencies                       \n\
        run         compile and run April program                           \n\
        version     print April version                                     \n\
                                                                            \n\
Use \"april help [command]\" for more information about a command.          \n\
");

    // Converts to april command.
    static april_command_t __to_april_command(const char_t * cmd)
    {
        if (al::equals(cmd, _T("compile")))
            return april_command_t::compile;

        if (al::equals(cmd, _T("run")))
            return april_command_t::run;

        if (al::equals(cmd, _T("version")))
            return april_command_t::version;

        if (al::equals(cmd, _T("help")))
            return april_command_t::help;

        return april_command_t::__unknown__;
    }

    // Converts to april command.
    static april_command_t __to_april_command(const char * cmd)
    {
        return __to_april_command(string_convert<char, char_t>(cmd).c_str());
    }

    // Parses arguments.
    bool april_options_t::parse(int argc, char ** argv)
    {
        if (argc == 1)
        {
            show_help();
            return false;
        }

        _A(argc >= 2);

        char * commandStr = argv[1];
        this->command = __to_april_command(commandStr);

        __cmdline_parser_t parser;

        argc -= 1;
        argv += 1;

        if (!__parse(this->command, parser, argc, argv))
        {
            std::cout << parser.usage().c_str() << std::endl;
            return false;
        }

        return true;
    }

    // Shows help.
    void april_options_t::show_help(april_command_t command)
    {
        __cmdline_parser_t parser;
        if (__parse(command, parser, 0, nullptr))
        {
            std::cout << parser.usage().c_str() << std::endl;
        }
    }

    // Shows help.
    void april_options_t::show_help()
    {
        std::wcout << __help_message << std::endl;
    }

    // Parses options.
    bool april_options_t::__parse(april_command_t command, __cmdline_parser_t & parser,
                                                            int argc, char ** argv)
    {
        string_t program_name = _F(_T("april %1%"), command);
        parser.set_program_name(string_convert<char_t, char>(program_name));

        switch (command)
        {
            case april_command_t::run:
                return __parse_run(parser, argc, argv);

            case april_command_t::compile:
                return __parse_compile(parser, argc, argv);

            case april_command_t::help:
                return __parse_help(parser, argc, argv);

            case april_command_t::version:
                return __parse_version(parser, argc, argv);

            default:
                _PF(_T("unknown command '%1%'"), argv[1]);
                show_help();
                return false;
        }
    }

    // Parses running command.
    bool april_options_t::__parse_run(__cmdline_parser_t & parser, int argc, char ** argv)
    {
        __init_compile_parser(parser);
        parser.add<std::string>("execute", 'e', "execute assembly name", false, "");

        if (argc == 0)
            return true;

        parser.parse_check(argc, argv);

        run_options_t & options = init_options<run_options_t>();
        options.execute_assembly = __read_string(parser, "execute");

        return __read_compile_options(parser, options);
    }

    // Parses compiling command.
    bool april_options_t::__parse_compile(__cmdline_parser_t & parser, int argc, char ** argv)
    {
        __init_compile_parser(parser);

        if (argc == 0)
            return true;

        parser.parse_check(argc, argv);

        compile_options_t & options = init_options<compile_options_t>();
        return __read_compile_options(parser, options);
    }

    // Parses helping command.
    bool april_options_t::__parse_help(__cmdline_parser_t & parser, int argc, char ** argv)
    {
        parser.footer(string_convert<char_t, char>(
            _F(_T("<command: %1%>"), __all_commands())
        ));

        if (argc == 0)
            return true;

        parser.parse_check(argc, argv);

        if (parser.rest().empty())
        {
            std::wcout << _T("command missing") << std::endl;
            return false;
        }

        if (parser.rest().size() > 1)
        {
            std::wcout << _T("multipy commands") << std::endl;
            return false;
        }

        help_options_t & options = init_options<help_options_t>();
        options.sub_command = string_convert<char, char_t>(parser.rest()[0]);

        return true;
    }

    // Parses version command.
    bool april_options_t::__parse_version(__cmdline_parser_t & parser, int argc, char ** argv)
    {
        return true;
    }

    // Initialize compile parser.
    void april_options_t::__init_compile_parser(__cmdline_parser_t & parser)
    {
        parser.footer("files ...");
        parser.add<std::string>("output", 'o', "output file", false, "");
    }

    // Reads compile options.
    bool april_options_t::__read_compile_options(__cmdline_parser_t & parser,
                                                compile_options_t & options)
    {
        __copy_strings(parser.rest(), options.files);
        options.output_path = __read_string(parser, "output");

        if (options.files.empty())
        {
            _P(_T("source files missing"));
            return false;
        }

        return true;
    }

    // Copies strings.
    void april_options_t::__copy_strings(const std::vector<std::string> & src,
                                         std::vector<string_t> & dst)
    {
        al::transform(src, std::back_inserter(dst), [](const std::string & s) {
            return string_convert<char, char_t>(s);
        });
    }

    ////////// ////////// ////////// ////////// /////////

    // Constructor.
    april_t::april_t(april_options_t & options) : __options(options)
    {

    }

    // Executes.
    void april_t::execute()
    {
        switch (__options.command)
        {
            case april_command_t::compile:
                __execute_compile(__options.get<__t::compile_options_t>());
                break;

            case april_command_t::run:
                __execute_run(__options.get<__t::run_options_t>());
                break;

            case april_command_t::help:
                __execute_help(__options.get<__t::help_options_t>());
                break;

            case april_command_t::version:
                std::wcout << _T("April version: ") << JC_VERSION << std::endl;
                break;

            default:
                X_UNEXPECTED();
        }
    }

    // Executes compile command.
    void april_t::__execute_compile(const __t::compile_options_t & options)
    {
        april_xcompiler_t(options).compile();
    }

    // Executes run command.
    void april_t::__execute_run(const __t::run_options_t & options)
    {
        april_xcompiler_t(options).run(options.execute_assembly);
    }

    // Executes help command.
    void april_t::__execute_help(const __t::help_options_t & options)
    {
        april_command_t command = __to_april_command(options.sub_command.c_str());
        if (command == april_command_t::__unknown__)
        {
            _PF(_T("unknown command %1%"), options.sub_command);
            __options.show_help();
            return;
        }

        __options.show_help(command);
    }

    ////////// ////////// ////////// ////////// /////////

    // Main method.
    int __main(int argc, char ** argv)
    {
        try
        {
            april_options_t options;
            if (options.parse(argc, argv))
            {
                april_t(options).execute();
                return 0;
            }
        }
        catch (const error_t & err)
        {
            _P(_str(err));
        }

        return 1;
    }

    ////////// ////////// ////////// ////////// /////////

} } }  // X_ROOT_NS::modules::april


////////// ////////// ////////// ////////// /////////

namespace __april = __root_ns::modules::april;

// Main function
int main(int argc, char ** argv)
{
    return __april::__main(argc, argv);
}

