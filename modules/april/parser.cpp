
#include <april.h>
#include "util.h"

namespace X_ROOT_NS { namespace modules { namespace april {

    using namespace boost::property_tree;
    typedef april_error_code_t __e_t;

    ////////// ////////// ////////// ////////// //////////
    // parser_base_t

    // Config parser.
    void parser_t::parse(__ptree_stream_t & stream)
    {
        this->on_parse(stream, __ptree);
    }

    // Parse the config of specified path.
    void parser_t::parse(const lib::path_t & path)
    {
        typedef std::basic_ifstream<char_t> __fstream_t;
        __fstream_t fs;

        fs.open(path.string(), std::ios_base::in);
        if(!fs.is_open())
            throw _ED(__e_t::open_file_failed, path);

        try
        {
            this->parse(fs);
            fs.close();
        }
        catch(...)
        {
            fs.close();
            throw;
        }
    }

    // Returns the root.
    __ptree_t & parser_t::root()
    {
        if(__ptree.empty())
            throw _ED(__e_t::config_format_error, "[root]");

        return __ptree.begin()->second;
    }

    ////////// ////////// ////////// ////////// //////////
    // xml_parser_t

    // Parses the config.
    void xml_parser_t::on_parse(__ptree_stream_t & stream, __ptree_t & ptree)
    {
        read_xml(stream, ptree);
    }

    ////////// ////////// ////////// ////////// //////////

    // Reads the config.
    void config_t::read(memory_t * memory, __ptree_t & root, const lib::path_t & relative_path)
    {
        this->on_read(memory, root, relative_path);
    }

    ////////// ////////// ////////// ////////// //////////
    // solution_config_t

    // Reads the config.
    __ptree_t & __read_child(__ptree_t & node, const char_t * path)
    {
        try
        {
            return node.get_child(path);
        }
        catch(...)
        {
            throw _ED(__e_t::config_format_error, path);
        }
    }

    // Reads the string.
    string_t __read_string(__ptree_t & node, const char_t * path)
    {
        try
        {
            return node.get<string_t>(path);
        }
        catch(...)
        {
            throw _ED(__e_t::config_format_error, path);
        }
    }

    // Reads the solution.
    void solution_config_t::on_read(memory_t * memory, __ptree_t & root,
                                                    const lib::path_t & relative_path)
    {
        this->name = __read_string(root, _T("name"));
        __ptree_t & project_nodes = __read_child(root, _T("projects"));

        for(auto && it : project_nodes)
        {
            __ptree_t & project_node = it.second;

            string_t project_path = project_node.get<string_t>(_T("path"));
            project_config_t * project_config;

            if(project_path.empty())
            {
                project_config = memory_t::new_obj<project_config_t>(memory);
                project_config->read(memory, project_node, relative_path);
            }
            else
            {
                lib::path_t path = lib::absolute(lib::path_t(project_path), relative_path);
                project_config = parse_project(memory, path);
            }

            this->projects.push_back(project_config);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // project_config_t

    // Reads the project.
    void project_config_t::on_read(memory_t * memory, __ptree_t & root,
                                                    const lib::path_t & relative_path)
    {
        this->name = __read_string(root, _T("name"));

        __ptree_t & file_nodes = __read_child(root, _T("files"));
        for(auto && it : file_nodes)
        {
            __ptree_t & file_node = it.second;
            file_config_t * file_config = memory_t::new_obj<file_config_t>(memory);
            file_config->read(memory, file_node, relative_path);

            this->files.push_back(file_config);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // file_config_t

    // Reads the config.
    void file_config_t::on_read(memory_t * memory, __ptree_t & root,
                                                    const lib::path_t & relative_path)
    {
        this->path = lib::absolute(__read_string(root, _T("path")), relative_path);
    }

    ////////// ////////// ////////// ////////// //////////

    // Parses the config.
    config_t * parse_config(memory_t * memory, const lib::path_t & path)
    {
        switch(file_type_of(path))
        {
            case compile_file_type_t::solution:
                return parse_solution(memory, path);

            case compile_file_type_t::project:
                return parse_project(memory, path);

            case compile_file_type_t::source:
            case compile_file_type_t::assembly:
            default:
                return memory_t::new_obj<file_config_t>(memory, path);
        }
    }

    // Parses the config.
    template<typename _config_t>
    _config_t * __parse_config(memory_t * memory, const lib::path_t & path)
    {
        xml_parser_t parser;
        parser.parse(path);

        _config_t * config;
        config = memory_t::new_obj<_config_t>(memory);
        config->read(memory, parser.root(), path.parent_path());

        return config;
    }

    // Parses solution.
    solution_config_t * parse_solution(memory_t * memory, const lib::path_t & path)
    {
        return __parse_config<solution_config_t>(memory, path);
    }

    // Parges project.
    project_config_t * parse_project(memory_t * memory, const lib::path_t & path)
    {
        return __parse_config<project_config_t>(memory, path);
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // X_ROOT_NS::modules::compile


