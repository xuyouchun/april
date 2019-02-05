
#include "compile.h"


namespace X_ROOT_NS { namespace modules { namespace compile {

    using namespace core;

    typedef ast_build_error_code_t e_t;
    typedef token_tag_t __tag_t;
    typedef lang_ast_build_elements_t<__tag_t> __elements_t;

    ////////// ////////// ////////// ////////// //////////

    // Root ast node.
    class __root_ast_node_t : public document_ast_node_t
    {
        typedef document_ast_node_t __super_t;

    public:
        using __super_t::__super_t;
        __root_ast_node_t(ast_context_t & context) : __context(context) { }

    private:
        ast_context_t & __context;
    };

    //-------- ---------- ---------- ---------- ----------

    // Root ast builder.
    class __root_ast_builder_t : public tast_builder_t<__root_ast_node_t>
    {
        typedef tast_builder_t<__root_ast_node_t> __super_t;

    public:
        using __super_t::__super_t;

        // Applies token.
        virtual void apply_token(token_t * token, __apply_token_args_t & args) override { }

        // Applies ast node.
        virtual void apply_ast(ast_node_t * node, __apply_ast_args_t & args) override
        {
            (*this)->append(node);
        }

        // Completed.
        virtual void completed(__completed_args_t & args) override
        {
            (*this)->on_commit();
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // When branch matched.
    void ast_factory_t::on_branch_matched(const analyze_node_t * node,
            __node_value_t value, __tag_t * from_tag, __tag_t * end_tag)
    {
        __elements_t elements(__reader, from_tag, end_tag);
        lang_ast_build_args_t args(value, elements);

        ast_node_t * ast_node = __build_ast(args);

        if(ast_node != nullptr)
        {
            __reader.append_ast(from_tag, end_tag, ast_node, node);
        }
    }

    // Analyze end notifiy.
    void ast_factory_t::on_end()
    {

    }

    // Builds ast nodes.
    ast_node_t * ast_factory_t::__build_ast(lang_ast_build_args_t & args)
    {
        if((__node_value_t)args.node_value == __root_node_value)
            return __root_ast_builder_t(__ast_context, args).build();

        return __build_service->build_ast(__ast_context, args);
    }

    // Returns the analyze result.
    ast_node_t * ast_factory_t::get_result()
    {
        return __reader.get_root_ast();
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns node flag.
    static const char_t * __get_node_flag(analyzer_element_t & element)
    {
        const analyze_node_t * node = element.matched_node;
        if(node == nullptr)
            return _T("");

        return node->flag.c_str();
    }

    // Builds ast node.
    ast_node_t * ast_builder_t::build()
    {
        ast_node_t * node = this->create();
        if(node == nullptr)
            return nullptr;

        for(analyzer_element_t & element : __args.elements)
        {
            switch(element.type)
            {
                case analyzer_element_type_t::token: {
                    ast_builder_apply_token_args_t args;
                    this->apply_token(element.token, args);
                }   break;

                case analyzer_element_type_t::ast_node: {
                    ast_builder_apply_ast_args_t args { __get_node_flag(element) };
                    this->apply_ast(element.ast_node, args);
                } break;

                default:
                    break;
            }
        }

        ast_builder_completed_args_t args { nullptr };
        this->completed(args);

        return node;
    }

    ////////// ////////// ////////// ////////// //////////

} } }


