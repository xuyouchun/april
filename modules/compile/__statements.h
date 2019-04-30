
#ifndef __COMPILE_STATEMENTS_H__
#define __COMPILE_STATEMENTS_H__

namespace X_ROOT_NS { namespace modules { namespace compile {

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        using namespace core;

        template<typename t, size_t init_size = 2>
        using __vector_t = al::svector_t<t, init_size>;

        typedef statement_compile_context_t __context_t;
        typedef statement_exit_point_type_t __exit_point_type_t;
    }

    ////////// ////////// ////////// ////////// //////////

    // Base class of statement.
    class statement_base_t : public statement_t
    {
        // Returns whether the statement is empty.
        virtual bool is_empty(xpool_t & xpool) override { return false; }
    };

    ////////// ////////// ////////// ////////// //////////

    // Statement group.
    class statement_group_t : public statement_base_t
    {
        typedef al::svector_t<statement_t *, 2> __statements_t;

    public:
        typedef statement_t * value_type;

        // Appends a statement.
        void push_back(statement_t * statement);

        // Compiles this statement.
        virtual void compile(statement_compile_context_t & ctx) override;

        // Returns exit type.
        virtual statement_exit_type_t exit_type(statement_exit_type_context_t & ctx) override;

        // Returns whether the statement is empty.
        virtual bool is_empty(xpool_t & xpool) override;

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

        // Compiles this statement.
        virtual void compile(statement_compile_context_t & ctx) override;

        // Returns exit type.
        virtual statement_exit_type_t exit_type(statement_exit_type_context_t & ctx) override;

        // Returns whether the statement is empty.
        virtual bool is_empty(xpool_t & xpool) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // Typedef statement.
    class type_def_statement_t : public statement_base_t
    {
    public:

        // Constructors.
        type_def_statement_t() = default;
        type_def_statement_t(type_def_t * type_def) : type_def(type_def) { }

        type_def_t * type_def = nullptr;

        // Compiles this statement.
        virtual void compile(statement_compile_context_t & ctx) override;

        // Returns exit type.
        virtual statement_exit_type_t exit_type(statement_exit_type_context_t & ctx) override;

        // Returns whether the statement is empty.
        virtual bool is_empty(xpool_t & xpool) override { return true; }
    };

    ////////// ////////// ////////// ////////// //////////

    // Defination statement item.
    class defination_statement_item_t : public eobject_t
    {
    public:

        // Constructors.
        defination_statement_item_t() = default;
        defination_statement_item_t(name_t name, expression_t * expression,
                                    local_variable_t * variable)
            : name(name), expression(expression), variable(variable)
        { }

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
        typedef al::svector_t<defination_statement_item_t *> items_t;

    public:

        typedef defination_statement_item_t * etype_t;
        typedef etype_t value_type;

        // Constructors.
        defination_statement_t() = default;
        defination_statement_t(type_name_t * type_name) : type_name(type_name) { }

        type_name_t * type_name = nullptr;
        items_t       items;

        // Appends an item.
        void push_back(defination_statement_item_t * item)
        {
            items.push_back(item);
        }

        // Converts to string.
        X_TO_STRING

        // Compiles this statement.
        virtual void compile(statement_compile_context_t & ctx) override;

        // Returns exit type.
        virtual statement_exit_type_t exit_type(statement_exit_type_context_t & ctx) override;

        // Returns whether the statement is empty.
        virtual bool is_empty(xpool_t & xpool) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // Break statement.
    class break_statement_t : public statement_base_t
    {
    public:

        // Compiles this statement.
        virtual void compile(statement_compile_context_t & ctx) override;

        // Returns exit type.
        virtual statement_exit_type_t exit_type(statement_exit_type_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // Continue statement.
    class continue_statement_t : public statement_base_t
    {
    public:

        // Compiles this statement.
        virtual void compile(statement_compile_context_t & ctx) override;

        // Returns exit type.
        virtual statement_exit_type_t exit_type(statement_exit_type_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // Throw statement.
    class throw_statement_t : public statement_base_t
    {
    public:

        // Constructors.
        throw_statement_t() = default;
        throw_statement_t(expression_t * expression) : expression(expression) { }

        expression_t * expression = nullptr;

        // Compiles this statement.
        virtual void compile(statement_compile_context_t & ctx) override;

        // Returns exit type.
        virtual statement_exit_type_t exit_type(statement_exit_type_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // Goto statement.
    class goto_statement_t : public statement_base_t
    {
    public:

        // Constructors.
        goto_statement_t() = default;
        goto_statement_t(name_t label) : label(label) { }

        name_t label = name_t::null;        // Label.

        // Compiles this statement.
        virtual void compile(statement_compile_context_t & ctx) override;

        // Returns exit type.
        virtual statement_exit_type_t exit_type(statement_exit_type_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // Returns statement.
    class return_statement_t : public statement_base_t
    {
    public:

        // Constructors.
        return_statement_t() = default;
        return_statement_t(expression_t * expression) : expression(expression) { }

        expression_t * expression = nullptr;    // Return value.

        // Compiles this statement.
        virtual void compile(statement_compile_context_t & ctx) override;

        // Returns exit type.
        virtual statement_exit_type_t exit_type(statement_exit_type_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // Do while statement.
    class do_while_statement_t : public statement_base_t
    {
    public:

        // Constructors.
        do_while_statement_t() = default;
        do_while_statement_t(expression_t * condition, statement_t * body)
            : condition(condition), body(body)
        { }

        expression_t * condition = nullptr;     // Condition
        statement_t  * body      = nullptr;     // Body

        // Compiles this statement.
        virtual void compile(statement_compile_context_t & ctx) override;

        // Returns exit type.
        virtual statement_exit_type_t exit_type(statement_exit_type_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // Loop until statement.
    class loop_until_statement_t : public statement_base_t
    {
    public:

        // Constructors.
        loop_until_statement_t() = default;
        loop_until_statement_t(expression_t * condition, statement_t * body)
            : condition(condition), body(body) { }

        expression_t * condition = nullptr;     // Condition
        statement_t  * body      = nullptr;     // Body

        // Compiles this statement.
        virtual void compile(statement_compile_context_t & ctx) override;

        // Returns exit type.
        virtual statement_exit_type_t exit_type(statement_exit_type_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // While statement.
    class while_statement_t : public statement_base_t
    {
    public:

        // Constructors.
        while_statement_t() = default;
        while_statement_t(expression_t * condition, statement_t * body)
            : condition(condition), body(body) { }

        expression_t * condition = nullptr;     // Condition expression
        statement_t  * body      = nullptr;     // Body

        // Compiles this statement.
        virtual void compile(statement_compile_context_t & ctx) override;

        // Returns exit type.
        virtual statement_exit_type_t exit_type(statement_exit_type_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // For statement.
    class for_statement_t : public statement_base_t
    {
    public:

        // Constructors.
        for_statement_t() = default;
        for_statement_t(defination_statement_t * defination_initialize,
            expression_t * initialize, expression_t * condition, expression_t * increase,
            statement_t * body)
            : defination_initialize(defination_initialize)
            , initialize(initialize), condition(condition), increase(increase), body(body)
        { }

        // Defination initialize expression.
        defination_statement_t * defination_initialize = nullptr;
        expression_t * initialize = nullptr;    // Initialize expression.

        expression_t * condition  = nullptr;    // Condition expression.
        expression_t * increase   = nullptr;    // Increase expression.
        statement_t  * body       = nullptr;    // Body.

        // Compiles this statement.
        virtual void compile(statement_compile_context_t & ctx) override;

        // Returns exit type.
        virtual statement_exit_type_t exit_type(statement_exit_type_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // For...each statement.
    class for_each_statement_t : public statement_base_t
    {
    public:

        // Constructors.
        for_each_statement_t() = default;
        for_each_statement_t(expression_t * iterator, type_name_t * type_name,
            name_t variable, statement_t * body)
            : iterator(iterator), type_name(type_name), variable(variable), body(body)
        { }

        expression_t * iterator  = nullptr;         // Iterator.
        type_name_t  * type_name = nullptr;         // Type name.
        name_t         variable  = name_t::null;    // Variable name.
        statement_t  * body      = nullptr;         // Body.

        // Compiles this statement.
        virtual void compile(statement_compile_context_t & ctx) override;

        // Returns exit type.
        virtual statement_exit_type_t exit_type(statement_exit_type_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // If statement.
    class if_statement_t : public statement_base_t
    {
    public:

        // Constructors.
        if_statement_t() = default;
        if_statement_t(expression_t * condition, statement_t * if_body, statement_t * else_body)
            : condition(condition), if_body(if_body), else_body(else_body)
        { }

        expression_t * condition    = nullptr;      // Condition.
        statement_t  * if_body      = nullptr;      // If body.
        statement_t  * else_body    = nullptr;      // Else body.

        // Compiles statement.
        virtual void compile(statement_compile_context_t & ctx) override;

        // Returns exit type.
        virtual statement_exit_type_t exit_type(statement_exit_type_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

    // Case
    struct case_t : public eobject_t
    {
        typedef al::svector_t<expression_t *, 2> __constants_t;

        __constants_t   constants;                  // Constant values.
        statements_t  * statements   = nullptr;     // Statements.
    };

    typedef eobject_ast_t<case_t *> case_ast_t;

    //-------- ---------- ---------- ---------- ----------

    // Switch statement.
    class switch_statement_t : public statement_base_t
    {
        typedef al::svector_t<case_t *, 5> __cases_t;

    public:

        // Constructors.
        switch_statement_t() = default;
        switch_statement_t(expression_t *  expression) : expression(expression) { }

        expression_t *  expression = nullptr;       // Switch value.
        __cases_t       cases;                      // Cases.

        // Compile statement.
        virtual void compile(statement_compile_context_t & ctx) override;

        // Returns exit type.
        virtual statement_exit_type_t exit_type(statement_exit_type_context_t & ctx) override;

    private:

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
        // Constructors.
        catch_t() = default;
        catch_t(type_name_t * type_name, name_t name, statement_t * body)
            : type_name(type_name), name(name), body(body)
        { }

        type_name_t *   type_name   =   nullptr;        // Type name.
        name_t          name        =   name_t::null;   // Variable name.
        statement_t *   body        =   nullptr;        // Body.

        variable_t  *   variable    =   nullptr;        // Variable object.
    };

    typedef eobject_ast_t<catch_t *> catch_ast_t;

    //-------- ---------- ---------- ---------- ----------

    // Try statement.
    class try_statement_t : public statement_base_t
    {
        typedef al::svector_t<catch_t *> __catches_t;

    public:

        // Constructors.
        try_statement_t() = default;
        try_statement_t(statement_t * try_statement, statement_t * finally_statement)
            : try_statement(try_statement), finally_statement(finally_statement)
        { }

        statement_t * try_statement     =   nullptr;    // Try statement.
        __catches_t   catches           =   nullptr;    // Catch statements.
        statement_t * finally_statement =   nullptr;    // Finally statement.

        // Compile this statement.
        virtual void compile(statement_compile_context_t & ctx) override;

        // Returns exit type.
        virtual statement_exit_type_t exit_type(statement_exit_type_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////
    // empty_statement

    // Empty statement.
    class empty_statement_t : public statement_base_t
    {
    public:

        // Compile this statement.
        virtual void compile(statement_compile_context_t & ctx) override;

        // Returns exit type.
        virtual statement_exit_type_t exit_type(statement_exit_type_context_t & ctx) override;

        // Returns whether the statement is empty.
        virtual bool is_empty(xpool_t & xpool) override;
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

    // Returns exit type of statements.
    template<typename _statements_t>
    statement_exit_type_t exit_type_of(statement_exit_type_context_t & ctx,
                                       _statements_t * statements)
    {
        typedef statement_exit_type_t       __exit_type_t;
        typedef enum_t<__exit_type_t>       __e_exit_type_t;

        if(statements == nullptr)
            return statement_exit_type_t::none;

        __e_exit_type_t type;

        for(statement_t * statement : *statements)
        {
            __e_exit_type_t et = statement->exit_type(ctx);
            type |= et;

            if(et.has_only(__exit_type_t::return_, __exit_type_t::throw_, __exit_type_t::dead_cycle))
            {
                type.remove(__exit_type_t::pass);
                break;
            }
        }

        return *type;
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::compile

#endif // __COMPILE_STATEMENTS_H__
