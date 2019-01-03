
#ifndef __COMPILE_STATEMENTS_H__
#define __COMPILE_STATEMENTS_H__

namespace X_ROOT_NS { namespace modules { namespace compile {

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        using namespace core;

        template<typename t, size_t init_size = 2>
        using __vector_t = al::small_vector_t<t, init_size>;

        typedef statement_compile_context_t __context_t;
        typedef statement_exit_point_type_t __exit_point_type_t;
    }

    ////////// ////////// ////////// ////////// //////////

    class statement_base_t : public statement_t
    {
    public:
        virtual void compile(statement_compile_context_t & ctx) override final;

    protected:
        virtual void on_compile(statement_compile_context_t & ctx) = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    class statement_group_t : public statement_base_t
    {
        typedef al::small_vector_t<statement_t *, 2> __statements_t;

    public:
        typedef statement_t * value_type;
        void push_back(statement_t * statement);

    protected:
        virtual void on_compile(statement_compile_context_t & ctx) override;

    private:
        __statements_t __statements;
    };

    ////////// ////////// ////////// ////////// //////////

    class expression_statement_t : public statement_base_t
    {
    public:
        expression_statement_t() = default;
        expression_statement_t(expression_t * expression)
            : expression(expression)
        { }

        expression_t * expression = nullptr;

    protected:
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    class type_def_statement_t : public statement_base_t
    {
    public:
        type_def_t * type_def = nullptr;

    protected:
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    class defination_statement_item_t : public eobject_t
    {
    public:
        name_t         name       = name_t::null;
        expression_t * expression = nullptr;

        local_variable_t * variable;

        X_TO_STRING
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    class defination_statement_t : public statement_base_t
    {
        typedef al::small_vector_t<defination_statement_item_t *> items_t;

    public:
        typedef defination_statement_item_t * etype_t;
        typedef etype_t value_type;

        type_name_t * type_name = nullptr;
        items_t       items;

        void push_back(defination_statement_item_t * item)
        {
            items.push_back(item);
        }

        X_TO_STRING

    protected:
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    class break_statement_t : public statement_base_t
    {
    protected:
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    class continue_statement_t : public statement_base_t
    {
    protected:
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    class throw_statement_t : public statement_base_t
    {
    public:
        expression_t * expression = nullptr;

    protected:
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    class goto_statement_t : public statement_base_t
    {
    public:
        name_t label = name_t::null;

    protected:
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    class return_statement_t : public statement_base_t
    {
    public:
        expression_t * expression = nullptr;

    protected:
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    class do_while_statement_t : public statement_base_t
    {
    public:
        expression_t * condition = nullptr;
        statement_t  * body      = nullptr;

    protected:
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    class while_statement_t : public statement_base_t
    {
    public:
        expression_t * condition = nullptr;
        statement_t  * body      = nullptr;

    protected:
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    class for_statement_t : public statement_base_t
    {
    public:
        defination_statement_t * defination_initialize = nullptr;
        expression_t * initialize = nullptr;

        expression_t * condition  = nullptr;
        expression_t * increase   = nullptr;
        statement_t  * body       = nullptr;

    protected:
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    class for_each_statement_t : public statement_base_t
    {
    public:
        expression_t * iterator  = nullptr; 
        type_name_t  * type_name = nullptr;
        name_t         variable  = name_t::null;
        statement_t  * body      = nullptr;

    protected:
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    class if_statement_t : public statement_base_t
    {
    public:
        expression_t * condition    = nullptr;
        statement_t  * if_body      = nullptr;
        statement_t  * else_body    = nullptr;

    protected:
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    struct case_t : public eobject_t
    {
        typedef al::small_vector_t<expression_t *, 2> __constants_t;

        __constants_t   constants;
        statements_t  * statements   = nullptr;
    };

    typedef eobject_ast_t<case_t *> case_ast_t;

    //-------- ---------- ---------- ---------- ----------

    class switch_statement_t : public statement_base_t
    {
        typedef al::small_vector_t<case_t *, 5> __cases_t;

    public:
        expression_t *  expression = nullptr;
        __cases_t       cases;

    protected:
        virtual void on_compile(statement_compile_context_t & ctx) override;
        int32_t __read_const_int(statement_compile_context_t & ctx, expression_t * exp);
        int     __get_rows();

        void __compile_as_statement(statement_compile_context_t & ctx);
        void __compile_as_if(statement_compile_context_t & ctx);
        void __compile_as_switch(statement_compile_context_t & ctx, int row_count);
    };

    ////////// ////////// ////////// ////////// //////////

    struct catch_t : public eobject_t
    {
        type_name_t *   type_name   =   nullptr;
        name_t          variable    =   name_t::null;
        statement_t *   body        =   nullptr;
    };

    typedef eobject_ast_t<catch_t *> catch_ast_t;

    //-------- ---------- ---------- ---------- ----------

    class try_statement_t : public statement_base_t
    {
        typedef al::small_vector_t<catch_t *> __catches_t;

    public:
        statement_t * try_statement     =   nullptr;
        __catches_t   catches           =   nullptr;
        statement_t * finally_statement =   nullptr;

    protected:
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // empty_statement

    class empty_statement_t : public statement_base_t
    {
    protected:
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // tools

    cvalue_t execute_expression(statement_compile_context_t & ctx, expression_t * exp);
    cvalue_t execute_expression(expression_compile_context_t & ctx, expression_t * exp);
    cvalue_t execute_expression(expression_execute_context_t & ctx, expression_t * exp);

    namespace xilx
    {
        template<typename _xilx_t, typename ... args_t>
        _xilx_t * append_xilx(__context_t & ctx, args_t && ... args)
        {
            return ctx.append_xilx<_xilx_t>(
                *ctx.current_region(), std::forward<args_t>(args) ...
            );
        }

        point_jmp_xilx_t * append_jmp(__context_t & ctx, __exit_point_type_t type,
                            xil_jmp_condition_t jmp_condition = xil_jmp_condition_t::none);

        local_label_jmp_xilx_t * append_jmp(__context_t & ctx, local_label_t label,
                                xil_jmp_condition_t jmp_condition = xil_jmp_condition_t::none);

        global_label_jmp_xilx_t * append_jmp(__context_t & ctx, name_t name,
                                xil_jmp_condition_t jmp_condition = xil_jmp_condition_t::none);

        global_label_jmp_xilx_t * append_jmp(__context_t & ctx, const string_t & name,
                                xil_jmp_condition_t jmp_condition = xil_jmp_condition_t::none);

        global_label_xilx_t * append_global_label(__context_t & ctx, name_t name);
        global_label_xilx_t * append_global_label(__context_t & ctx, const string_t & name);

        local_label_xilx_t * append_local_label(__context_t & ctx, local_label_t flag);
        local_label_xilx_t * set_point(__context_t & ctx, __exit_point_type_t point_type);

        void append_condition(__context_t & ctx, expression_t * condition);
    }

    void compile_statement(__context_t & ctx, statement_t * statement);
    void compile_statements(__context_t & ctx, statements_t * statements);

    template<typename _region_t>
    void compile_with_region(__context_t & ctx, statement_t * statement)
    {
        if(statement != nullptr)
        {
            ctx.begin_region<_region_t>();
            compile_statement(ctx, statement);
            ctx.end_region();
        }
    }


    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::compile

#endif // __COMPILE_STATEMENTS_H__
