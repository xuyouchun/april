
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

    // Base class of statement.
    class statement_base_t : public statement_t
    {
    public:
        // Compiles this statement.
        virtual void compile(statement_compile_context_t & ctx) override final;

    protected:
        // Compiles this statememt.
        virtual void on_compile(statement_compile_context_t & ctx) = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    // Statement group.
    class statement_group_t : public statement_base_t
    {
        typedef al::small_vector_t<statement_t *, 2> __statements_t;

    public:
        typedef statement_t * value_type;

        // Appends a statement.
        void push_back(statement_t * statement);

    protected:

        // Compiles this statement.
        virtual void on_compile(statement_compile_context_t & ctx) override;

    private:
        __statements_t __statements;
    };

    ////////// ////////// ////////// ////////// //////////

    // Expression statement.
    class expression_statement_t : public statement_base_t
    {
    public:

        // Constructors.
        expression_statement_t() = default;
        expression_statement_t(expression_t * expression)
            : expression(expression)
        { }

        expression_t * expression = nullptr;

    protected:

        // Compiles this statement.
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // Typedef statement.
    class type_def_statement_t : public statement_base_t
    {
    public:
        type_def_t * type_def = nullptr;

    protected:

        // Compiles this statement.
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // Defination statement item.
    class defination_statement_item_t : public eobject_t
    {
    public:
        name_t         name       = name_t::null;   // Name.
        expression_t * expression = nullptr;        // Expression.

        local_variable_t * variable;                // Local variable.

        // Converts to string.
        X_TO_STRING
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Defination statement.
    class defination_statement_t : public statement_base_t
    {
        typedef al::small_vector_t<defination_statement_item_t *> items_t;

    public:
        typedef defination_statement_item_t * etype_t;
        typedef etype_t value_type;

        type_name_t * type_name = nullptr;
        items_t       items;

        // Appends an item.
        void push_back(defination_statement_item_t * item)
        {
            items.push_back(item);
        }

        // Converts to string.
        X_TO_STRING

    protected:

        // Compiles this statement.
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // Break statement.
    class break_statement_t : public statement_base_t
    {
    protected:

        // Compiles this statement.
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // Continue statement.
    class continue_statement_t : public statement_base_t
    {
    protected:

        // Compiles this statement.
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // Throw statement.
    class throw_statement_t : public statement_base_t
    {
    public:
        expression_t * expression = nullptr;

    protected:

        // Compiles this statement.
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // Goto statement.
    class goto_statement_t : public statement_base_t
    {
    public:
        name_t label = name_t::null;

    protected:

        // Compiles this statement.
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // Returns statement.
    class return_statement_t : public statement_base_t
    {
    public:
        expression_t * expression = nullptr;

    protected:

        // Compiles this statement.
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // Do while statement.
    class do_while_statement_t : public statement_base_t
    {
    public:
        expression_t * condition = nullptr;
        statement_t  * body      = nullptr;

    protected:

        // Compiles this statement.
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // Loop until statement.
    class loop_until_statement_t : public statement_base_t
    {
    public:
        expression_t * condition = nullptr;
        statement_t  * body      = nullptr;

    protected:

        // Compiles this statement.
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // While statement.
    class while_statement_t : public statement_base_t
    {
    public:
        expression_t * condition = nullptr;
        statement_t  * body      = nullptr;

    protected:

        // Compiles this statement.
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // For statement.
    class for_statement_t : public statement_base_t
    {
    public:
        defination_statement_t * defination_initialize = nullptr;
        expression_t * initialize = nullptr;

        expression_t * condition  = nullptr;
        expression_t * increase   = nullptr;
        statement_t  * body       = nullptr;

    protected:

        // Compiles this statement.
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // For...each statement.
    class for_each_statement_t : public statement_base_t
    {
    public:
        expression_t * iterator  = nullptr; 
        type_name_t  * type_name = nullptr;
        name_t         variable  = name_t::null;
        statement_t  * body      = nullptr;

    protected:

        // Compiles this statement.
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // If statement.
    class if_statement_t : public statement_base_t
    {
    public:
        expression_t * condition    = nullptr;
        statement_t  * if_body      = nullptr;
        statement_t  * else_body    = nullptr;

    protected:

        // Compiles statement.
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // Case
    struct case_t : public eobject_t
    {
        typedef al::small_vector_t<expression_t *, 2> __constants_t;

        __constants_t   constants;
        statements_t  * statements   = nullptr;
    };

    typedef eobject_ast_t<case_t *> case_ast_t;

    //-------- ---------- ---------- ---------- ----------

    // Switch statement.
    class switch_statement_t : public statement_base_t
    {
        typedef al::small_vector_t<case_t *, 5> __cases_t;

    public:
        expression_t *  expression = nullptr;
        __cases_t       cases;

    protected:

        // Compile statement.
        virtual void on_compile(statement_compile_context_t & ctx) override;

        // Reads a int constant value.
        int32_t __read_const_int(statement_compile_context_t & ctx, expression_t * exp);

        // Returns case counts.
        int     __get_rows();

        // Compiles as a statement.
        void __compile_as_statement(statement_compile_context_t & ctx);

        // Compiles as if statement.
        void __compile_as_if(statement_compile_context_t & ctx);

        // Compiles as switch statement.
        void __compile_as_switch(statement_compile_context_t & ctx, int row_count);
    };

    ////////// ////////// ////////// ////////// //////////

    // Catch.
    struct catch_t : public eobject_t
    {
        type_name_t *   type_name   =   nullptr;
        name_t          variable    =   name_t::null;
        statement_t *   body        =   nullptr;
    };

    typedef eobject_ast_t<catch_t *> catch_ast_t;

    //-------- ---------- ---------- ---------- ----------

    // Try statement.
    class try_statement_t : public statement_base_t
    {
        typedef al::small_vector_t<catch_t *> __catches_t;

    public:
        statement_t * try_statement     =   nullptr;
        __catches_t   catches           =   nullptr;
        statement_t * finally_statement =   nullptr;

    protected:

        // Compile this statement.
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // empty_statement

    // Empty statement.
    class empty_statement_t : public statement_base_t
    {
    protected:

        // Compile this statement.
        virtual void on_compile(statement_compile_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // tools

    // Executes the expression.
    cvalue_t execute_expression(statement_compile_context_t & ctx, expression_t * exp);

    // Executes the expression.
    cvalue_t execute_expression(expression_compile_context_t & ctx, expression_t * exp);

    // Executes the expression.
    cvalue_t execute_expression(expression_execute_context_t & ctx, expression_t * exp);

    namespace xilx
    {
        // Appends xilx.
        template<typename _xilx_t, typename ... args_t>
        _xilx_t * append_xilx(__context_t & ctx, args_t && ... args)
        {
            return ctx.append_xilx<_xilx_t>(
                *ctx.current_region(), std::forward<args_t>(args) ...
            );
        }

        // Appends jmp xilx.
        point_jmp_xilx_t * append_jmp(__context_t & ctx, __exit_point_type_t type,
                            xil_jmp_condition_t jmp_condition = xil_jmp_condition_t::none);

        // Appends jmp xilx.
        local_label_jmp_xilx_t * append_jmp(__context_t & ctx, local_label_t label,
                                xil_jmp_condition_t jmp_condition = xil_jmp_condition_t::none);

        // Appends jmp xilx.
        global_label_jmp_xilx_t * append_jmp(__context_t & ctx, name_t name,
                                xil_jmp_condition_t jmp_condition = xil_jmp_condition_t::none);

        // Appends jmp xilx.
        global_label_jmp_xilx_t * append_jmp(__context_t & ctx, const string_t & name,
                                xil_jmp_condition_t jmp_condition = xil_jmp_condition_t::none);

        // Appends global label.
        global_label_xilx_t * append_global_label(__context_t & ctx, name_t name);

        // Appends global label.
        global_label_xilx_t * append_global_label(__context_t & ctx, const string_t & name);

        // Appends local label.
        local_label_xilx_t * append_local_label(__context_t & ctx, local_label_t flag);

        // Sets point.
        local_label_xilx_t * set_point(__context_t & ctx, __exit_point_type_t point_type);

        // Appends condition.
        void append_condition(__context_t & ctx, expression_t * condition);
    }

    // Compiles statement.
    void compile_statement(__context_t & ctx, statement_t * statement);

    // Compiles statements.
    void compile_statements(__context_t & ctx, statements_t * statements);

    // Compiles with region.
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
