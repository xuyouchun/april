
#include "cs_analyze_tree.h"

namespace X_ROOT_NS::modules::lang_cs {

    namespace
    {
        using namespace core;
        using namespace compile;
    }

    typedef code_element_t __el_t;

    #define __Ast(name)     Ast(name, cs_branch_type_t::name)
    #define __EndAst(name)  EndAst(name)
    #define __AstName(name) RawAstName(name)

    ////////// ////////// ////////// ////////// //////////

    #define __SimpleAst(name)                                                           \
                                                                                        \
        __Ast(name)                                                                     \
            /* nothing */                                                               \
        __EndAst(name)

    #define __Using(name)   using __AstName(name)::__AstName(name)

    ////////// ////////// ////////// ////////// //////////

    __SimpleAst(_decorate_complex)

    // Decorate complex ast node
    class _decorate_complex_ast_node_t : public __AstName(_decorate_complex)
    {
        typedef __AstName(_decorate_complex) __super_t;

    public:
        __Using(_decorate_complex);

        access_value_t access_value = access_value_t::protected_or_internal;
    };

    ////////// ////////// ////////// ////////// //////////
    // _attribute_group_assign

    // Attribute group assign.
    __Ast(_attribute_group_assign)

        // Expression.
        __expression__,

    __EndAst(_attribute_group_assign)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Attribute group assign ast node.
    class _attribute_group_assign_ast_node_t : public __AstName(_attribute_group_assign)
                                             , public argument_ast_t
    {
        typedef __AstName(_attribute_group_assign) __super_t;

    public:
        using __super_t::__super_t;

        // Sets name.
        void set_name(name_t name, __el_t * el);

        // Commit this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual argument_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        argument_t __argument;
    };

    ////////// ////////// ////////// ////////// //////////
    // _attribute_group

    __Ast(_attribute_group)

        __items__,

    __EndAst(_attribute_group)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Attribute group ast node.
    class _attribute_group_ast_node_t : public __AstName(_attribute_group)
                                      , public eobjects_ast_t<attribute_t *>
    {
        typedef __AstName(_attribute_group) __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns eobject count.
        virtual size_t eobject_count() const override;

        // Returns eobject at specified index.
        virtual attribute_t * eobject_at(size_t index) const override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Attribute group item ast node.
    class _attribute_group_item_ast_node_t : public attribute_ast_node_t
    {
        typedef attribute_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Attribute type name ast node.
    class _attribute_type_name_ast_node_t
        : public type_name_ast_node_base_t<type_name_ast_node_t>
    {
        typedef type_name_ast_node_base_t<type_name_ast_node_t> __super_t;

    public:
        using __super_t::__super_t;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:

        // Walks confirm step.
        bool __walk_confirm(ast_walk_context_t & context);

        // Ascertains type.
        ascertain_type_error_t __ascertain_type(ast_walk_context_t & context,
                                                general_type_name_t * type_name);

        // Try resolve type with postfix 'Attribute'.
        bool __try_resolve_with_postfix(ast_walk_context_t & context,
                                                general_type_name_t * type_name);
    };

    ////////// ////////// ////////// ////////// //////////
    // _defination_st_item

    __Ast(_defination_st_item)

        __expression__,

    __EndAst(_defination_st_item)

    typedef eobject_ast_t<defination_statement_item_t *> defination_statement_item_ast_t;

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Defination statement item ast node.
    class _defination_st_item_ast_node_t : public __AstName(_defination_st_item)
                                         , public defination_statement_item_ast_t
    {
        typedef __AstName(_defination_st_item) __super_t;

    public:
        __Using(_defination_st_item);

        // Sets name.
        void set_name(name_t name, __el_t * el);

        // Commits this node.
        virtual void on_commit() override;

        // Returns this eobject.
        virtual defination_statement_item_t * to_eobject() override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;

    private:
        defination_statement_item_t __item;
    };

    ////////// ////////// ////////// ////////// //////////
    // _fake_method

    // Fake method ast node.
    class _fake_method_ast_node_t : public method_ast_node_t
    {
        typedef method_ast_node_t __super_t;

    public:
        using method_ast_node_t::method_ast_node_t;

        cs_token_value_t token_value = cs_token_value_t::__unknown__;
    };

    ////////// ////////// ////////// ////////// //////////
    // _fields

    __Ast(_fields)

        __type_name__,

        __decorate__,

        __attributes__,

        __items__,

    __EndAst(_fields)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Fields ast node.
    class _fields_ast_node_t : public __AstName(_fields)
                             , public fields_ast_t
    {
        typedef __AstName(_fields) __super_t;

    public:
        using __super_t::__super_t;

        // Commits this node.
        virtual void on_commit() override;

        // Returns eobject count.
        virtual size_t eobject_count() const override;

        // Returns eobject at specified index.
        virtual field_t * eobject_at(size_t index) const override;

        // Walks this node.
        virtual bool on_walk(ast_walk_context_t & context, int step, void * tag) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // _fields_item

    typedef field_ast_node_t _fields_item_ast_node_t;

    ////////// ////////// ////////// ////////// //////////
    // _enum_fields

    // Enum fields ast node.
    class _enum_fields_ast_node_t : public _fields_ast_node_t
    {
        typedef _fields_ast_node_t __super_t;

    public:
        using __super_t::__super_t;
    };

    ////////// ////////// ////////// ////////// //////////
    // _enum_fields_item

    // Enum fields item ast node.
    class _enum_fields_item_ast_node_t : public _fields_item_ast_node_t
    {
        typedef _fields_item_ast_node_t __super_t;

    public:
        using __super_t::__super_t;
    };

    ////////// ////////// ////////// ////////// //////////
    // _single_expression

    // Simple expression ast node.
    class _single_expression_ast_node_t : public expression_ast_node_t
    {
        typedef expression_ast_node_t __super_t;

    public:
        using __super_t::__super_t;
    };

    ////////// ////////// ////////// ////////// //////////
    // _function_name_expression

    // Function name expression ast node.
    class _function_name_expression_ast_node_t : public expression_ast_node_t
    {
        typedef expression_ast_node_t __super_t;

    public:
        using __super_t::__super_t;

        ast_node_t * generic_args_node = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////
    // __default_array_lengths_ast_node_t

    // Default array lengths ast node.
    class __default_array_lengths_ast_node_t : public array_lengths_ast_node_t
    {
        typedef array_lengths_ast_node_t __super_t;

    public:

        // Constructor.
        template<typename ... args_t>
        __default_array_lengths_ast_node_t(args_t && ... args)
            : __super_t(std::forward<args_t>(args) ...)
        {
            this->append_child(__lengths__, nullptr);
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // _operator

    __SimpleAst(_operator)

    // Operator ast node
    class _operator_ast_node_t : public __AstName(_operator)
    {
        typedef __AstName(_operator) __super_t;

    public:
        __Using(_operator);

        const operator_property_t * op_property = nullptr;
    };


    ////////// ////////// ////////// ////////// //////////

    #undef __Using
    #undef __Ast
    #undef __EndAst
    #undef __SimpleAst

}   // namespace X_ROOT_NS::modules::lang_cs


