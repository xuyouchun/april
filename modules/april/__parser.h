#ifndef __APRIL_PARSER_H__
#define __APRIL_PARSER_H__

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/typeof/typeof.hpp> 

namespace X_ROOT_NS { namespace modules { namespace april {

    namespace
    {
        using namespace boost::property_tree;
    }

    typedef wptree __ptree_t;
    typedef std::basic_istream<typename __ptree_t::key_type::value_type> __ptree_stream_t;

    ////////// ////////// ////////// ////////// /////////

    class parser_t : public object_t
    {
    public:
        void parse(__ptree_stream_t & stream);
        void parse(const lib::path_t & path);

        __ptree_t & root();

    protected:
        virtual void on_parse(__ptree_stream_t & stream, __ptree_t & ptree) = 0;

    private:
        __ptree_t   __ptree;
    };

    ////////// ////////// ////////// ////////// /////////

    class xml_parser_t : public parser_t
    {
        typedef parser_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void on_parse(__ptree_stream_t & stream, __ptree_t & root) override;
    };

    ////////// ////////// ////////// ////////// /////////

    class config_t : public object_t
    {
    public:
        void read(memory_t * memory, __ptree_t & root, const lib::path_t & relative_path);

        string_t name;

    protected:
        virtual void on_read(memory_t * memory, __ptree_t & root,
                                                const lib::path_t & relative_path) = 0;
    };

    ////////// ////////// ////////// ////////// /////////
    // solution_config_t

    class project_config_t;

    class solution_config_t : public config_t
    {
        typedef config_t __super_t;

    public:
        using __super_t::__super_t;

        std::vector<project_config_t *> projects;

    protected:
        virtual void on_read(memory_t * memory, __ptree_t & root,
                                                const lib::path_t & relative_path) override;
    };

    ////////// ////////// ////////// ////////// /////////
    // project_config_t

    class file_config_t;

    class project_config_t : public config_t
    {
        typedef config_t __super_t;

    public:
        using __super_t::__super_t;

        std::vector<file_config_t *> files;

    protected:
        virtual void on_read(memory_t * memory, __ptree_t & root,
                                                const lib::path_t & relative_path) override;
    };

    ////////// ////////// ////////// ////////// /////////
    // file_config_t

    class file_config_t : public config_t
    {
        typedef config_t __super_t;

    public:
        using __super_t::__super_t;

        file_config_t(const lib::path_t & path) : path(path) { }

        lib::path_t path;

    protected:
        virtual void on_read(memory_t * memory, __ptree_t & root,
                                                const lib::path_t & relative_path) override;
    };

    ////////// ////////// ////////// ////////// /////////

    config_t * parse_config(memory_t * memory, const lib::path_t & path);
    solution_config_t * parse_solution(memory_t * memory, const lib::path_t & path);
    project_config_t * parse_project(memory_t * memory, const lib::path_t & path);

    ////////// ////////// ////////// ////////// /////////

} } }  // X_ROOT_NS::modules::april

#endif  // __APRIL_PARSER_H__
