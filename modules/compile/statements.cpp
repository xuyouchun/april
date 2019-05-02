
#include <compile.h>
#include "utils.h"

namespace X_ROOT_NS { namespace modules { namespace compile {

    using namespace core;
    using namespace xilx;

    typedef statement_compile_context_t     __context_t;
    typedef statement_exit_point_type_t     __exit_point_type_t;
    typedef compile_error_code_t            __e_t;
    typedef statement_exit_type_t           __exit_type_t;
    typedef enum_t<__exit_type_t>           __e_exit_type_t;
    typedef statement_exit_type_context_t   __exit_type_context_t;
    typedef method_xil_block_type_t         __block_type_t;

    #define __FreeExitFlags     enum_or(__exit_type_t::break_,                  \
            __exit_type_t::return_, __exit_type_t::goto_, __exit_type_t::throw_ \
        )

    //-------- ---------- ---------- ---------- ----------

    namespace xilx
    {
        // Appends jmp xilx.
        point_jmp_xilx_t * append_jmp(__context_t & ctx, __exit_point_type_t type,
                                xil_jmp_condition_t jmp_condition)
        {
            return append_xilx<point_jmp_xilx_t>(ctx, type, jmp_condition);
        }

        // Appends jmp xilx.
        local_label_jmp_xilx_t * append_jmp(__context_t & ctx, local_label_t label,
                                xil_jmp_condition_t jmp_condition)
        {
            return append_xilx<local_label_jmp_xilx_t>(ctx, label, jmp_condition);
        }

        // Appends jmp xilx.
        global_label_jmp_xilx_t * append_jmp(__context_t & ctx, name_t name,
                                xil_jmp_condition_t jmp_condition)
        {
            return append_xilx<global_label_jmp_xilx_t>(ctx, name, jmp_condition);
        }

        // Appends jmp xilx.
        global_label_jmp_xilx_t * append_jmp(__context_t & ctx, const string_t & name,
                                xil_jmp_condition_t jmp_condition)
        {
            return append_jmp(ctx, ctx.to_name(name), jmp_condition);
        }

        // Appends switch jmp xilx.
        switch_jmp_xilx_t * __append_switch(__context_t & ctx, switch_table_t * tbl)
        {
            return append_xilx<switch_jmp_xilx_t>(ctx, tbl);
        }

        //-------- ---------- ---------- ---------- ----------

        // Appends global label.
        global_label_xilx_t * append_global_label(__context_t & ctx, name_t name)
        {
            return append_xilx<global_label_xilx_t>(ctx, name);
        }

        // Appends global label.
        global_label_xilx_t * append_global_label(__context_t & ctx, const string_t & name)
        {
            return append_global_label(ctx, ctx.to_name(name));
        }

        // Appends local lable.
        local_label_xilx_t * append_local_label(__context_t & ctx, local_label_t flag)
        {
            return append_xilx<local_label_xilx_t>(ctx, flag);
        }

        //-------- ---------- ---------- ---------- ----------

        // Sets point.
        local_label_xilx_t * set_point(__context_t & ctx, __exit_point_type_t point_type)
        {
            local_label_xilx_t * label = append_xilx<local_label_xilx_t>(
                ctx, ctx.next_local_label()
            );

            ctx.set_point(point_type, label);

            return label;
        }

        //-------- ---------- ---------- ---------- ----------

        // Appends expression.
        void append_expression(__context_t & ctx, expression_t * expression)
        {
            if(expression != nullptr)
            {
                set_assign_parent(expression);
                append_xilx<expression_xilx_t>(ctx, expression);
            }
        }
    }

    // Returns whether the statement is empty.
    X_ALWAYS_INLINE bool __is_empty_statement(__context_t & ctx, statement_t * statement)
    {
        if(statement == nullptr)
            return true;

        return statement->is_empty(ctx.xpool());
    }

    // Compiles statement.
    void compile_statement(__context_t & ctx, statement_t * statement)
    {
        if(statement != nullptr)
            statement->compile(ctx);
    }

    // Compiles statements.
    void compile_statements(__context_t & ctx, statements_t * statements)
    {
        if(statements != nullptr)
        {
            for(statement_t * statement : *statements)
            {
                compile_statement(ctx, statement);
            }
        }
    }

    // Compiles statement with region.
    template<typename _region_t>
    void compile_statement_with_region(__context_t & ctx, statement_t * statement)
    {
        if(statement == nullptr)
            return;

        ctx.begin_region<_region_t>();
        statement->compile(ctx);
        ctx.end_region();
    }

    // Returns exit type of a statement.
    __e_exit_type_t __exit_type(__exit_type_context_t & ctx, statement_t * statement)
    {
        if(statement == nullptr)
            return __exit_type_t::none;

        return statement->exit_type(ctx);
    }

    ////////// ////////// ////////// ////////// //////////

    // Exit point mask.
    X_ENUM(__exit_point_mask_t)

        #define __EMask(v)  (1 << (int)v)

        self        = __EMask(__exit_point_type_t::self),
        break_      = __EMask(__exit_point_type_t::break_),
        continue_   = __EMask(__exit_point_type_t::continue_),

    X_ENUM_END

    // Exit point mask.
    X_ENUM_INFO(__exit_point_mask_t)

        X_C(self,       _T("self"))
        X_C(break_,     _T("break"))
        X_C(continue_,  _T("continue"))

    X_ENUM_INFO_END

    typedef __exit_point_mask_t __e_mask_t;

    // Returns exit point mask of point type.
    __exit_point_mask_t __e_mask(__exit_point_type_t point_type)
    {
        return (__exit_point_mask_t)__EMask(point_type);
    }

    //-------- ---------- ---------- ---------- ----------

    // Statement region.
    template<__exit_point_mask_t ... _masks>
    class __statement_region_t : public statement_region_t
    {
        typedef statement_region_t __super_t;
        static const __exit_point_mask_t mask = enum_xor<__exit_point_mask_t>(_masks ...);

    public:
        using __super_t::__super_t;

        // Sets point.
        virtual void set_point(__exit_point_type_t type, statement_point_t * label) override
        {
            if(!__is_supported(type))
                throw _ED(__e_t::unexpected_jmp, type);

            __super_t::set_point(type, label);
        }

        // Gets point.
        virtual statement_point_t * get_point(__exit_point_type_t type) override
        {
            statement_point_t * point;

            if(!__is_supported(type))
            {
                statement_region_t * parent = this->parent;
                if(parent == nullptr || (point = parent->get_point(type)) == nullptr)
                    throw _ED(__e_t::jmp_point_not_found, type);

            }
            else
            {
                point = __super_t::get_point(type);
                if(point == nullptr)
                    throw _ED(__e_t::jmp_point_not_found, type);
            }

            return point;
        }

    private:

        // Returns whether specified type is supported.
        bool __is_supported(__exit_point_type_t type)
        {
            return enum_has_flag(mask, __e_mask(type));
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // Executes the expression.
    cvalue_t execute_expression(statement_compile_context_t & ctx, expression_t * exp)
    {
        if(exp == nullptr)
            return cvalue_t::nan;

        expression_execute_context_t ectx(ctx.xpool());
        return exp->execute(ectx);
    }

    // Executes the expression.
    cvalue_t execute_expression(expression_compile_context_t & ctx, expression_t * exp)
    {
        return execute_expression(ctx.statement_ctx, exp);
    }

    // Executes the expression.
    cvalue_t execute_expression(expression_execute_context_t & ctx, expression_t * exp)
    {
        if(exp == nullptr)
            return cvalue_t::nan;

        return exp->execute(ctx);
    }

    cvalue_t execute_expression(__exit_type_context_t & ctx, expression_t * exp)
    {
        if(exp == nullptr)
            return cvalue_t::nan;

        expression_execute_context_t ectx(ctx.xpool);
        return exp->execute(ectx);
    }

    ////////// ////////// ////////// ////////// //////////

    // Pushes a statement.
    void statement_group_t::push_back(statement_t * statement)
    {
        _A(statement != nullptr);

        __statements.push_back(statement);
    }

    // Compiles this statement group.
    void statement_group_t::compile(statement_compile_context_t & ctx)
    {
        for(statement_t * statement : __statements)
        {
            compile_statement(ctx, statement);
        }
    }

    // Returns exit type.
    __exit_type_t statement_group_t::exit_type(__exit_type_context_t & ctx)
    {
        return exit_type_of(ctx, &__statements);
    }

    // Returns whether the statement is empty.
    bool statement_group_t::is_empty(xpool_t & xpool)
    {
        for(statement_t * statement : __statements)
        {
            if(!statement->is_empty(xpool))
                return false;
        }

        return true;
    }

    ////////// ////////// ////////// ////////// //////////

    // Compiles this expression statement.
    void expression_statement_t::compile(statement_compile_context_t & ctx)
    {
        if(expression != nullptr)
            append_xilx<expression_xilx_t>(ctx, expression);
    }

    // Returns exit type.
    __exit_type_t expression_statement_t::exit_type(__exit_type_context_t & ctx)
    {
        return __exit_type_t::pass;
    }

    // Returns whether the statement is empty.
    bool expression_statement_t::is_empty(xpool_t & xpool)
    {
        return expression == nullptr || expression->is_empty(xpool);
    }

    ////////// ////////// ////////// ////////// //////////

    // Compiles this statement.
    void type_def_statement_t::compile(statement_compile_context_t & ctx)
    {
        // Do nothing.
    }

    // Returns exit type.
    __exit_type_t type_def_statement_t::exit_type(__exit_type_context_t & ctx)
    {
        return __exit_type_t::none;
    }

    ////////// ////////// ////////// ////////// //////////

    // Converts to string.
    X_DEFINE_TO_STRING(defination_statement_item_t)
    {
        if(expression == nullptr)
            return _str(name);

        return _F(_T("%1% = %2%"), _str(name), _str(expression));
    }

    // Converts to string.
    X_DEFINE_TO_STRING(defination_statement_t)
    {
        string_t names = al::join_str(items.begin(), items.end(), _T(", "),
            [](auto & it) { return _str(it); }
        );

        return _F(_T("%1% %2%"), _str(type_name), names);
    }

    // Assign expression.
    class __assign_expression_t : public expression_t
    {
    public:
        virtual expression_family_t this_family() const override
        {
            return expression_family_t::__unknown__;
        }

        virtual size_t expression_count() const override { return 0; }

        virtual expression_t * expression_at(size_t index) const override
        {
            return nullptr;
        }

        virtual expression_behaviour_t get_behaviour() const override
        {
            return expression_behaviour_t::assign;
        }

    } __assign_expression_instance;

    // Sets parent of expression.
    void set_assign_parent(expression_t * exp)
    {
        if(exp != nullptr)
            exp->parent = &__assign_expression_instance;
    }

    // Compiles this statement.
    void defination_statement_t::compile(statement_compile_context_t & ctx)
    {
        for(defination_statement_item_t * item : items)
        {
            _A(item->variable != nullptr);

            if(!item->variable->constant && item->expression != nullptr)
            {
                set_assign_parent(item->expression);

                append_xilx<expression_xilx_t>(ctx, item->expression);
                append_xilx<local_assign_xilx_t>(ctx, item->variable);
            }
        }
    }

    // Returns exit type.
    __exit_type_t defination_statement_t::exit_type(__exit_type_context_t & ctx)
    {
        if(is_empty(ctx.xpool))
            return __exit_type_t::none;

        return __exit_type_t::pass;
    }

    // Returns whether the statement is empty.
    bool defination_statement_t::is_empty(xpool_t & xpool)
    {
        for(defination_statement_item_t * item : items)
        {
            if(item->expression != nullptr)
                return false;
        }

        return true;
    }

    ////////// ////////// ////////// ////////// //////////

    // Compiles this statement.
    void break_statement_t::compile(statement_compile_context_t & ctx) 
    {
        append_jmp(ctx, __exit_point_type_t::break_);
    }

    // Returns exit type.
    __exit_type_t break_statement_t::exit_type(__exit_type_context_t & ctx)
    {
        return __exit_type_t::break_;
    }

    ////////// ////////// ////////// ////////// //////////

    // Compiles this statement.
    void continue_statement_t::compile(statement_compile_context_t & ctx) 
    {
        append_jmp(ctx, __exit_point_type_t::continue_);
    }

    // Returns exit type.
    __exit_type_t continue_statement_t::exit_type(__exit_type_context_t & ctx)
    {
        return __exit_type_t::continue_;
    }

    ////////// ////////// ////////// ////////// //////////

    // Compiles this statement.
    void throw_statement_t::compile(statement_compile_context_t & ctx) 
    {
        if(expression != nullptr)
        {
            append_expression(ctx, expression);
            append_xilx<throw_xilx_t>(ctx);
        }
        else
        {
            append_xilx<rethrow_xilx_t>(ctx);
        }
    }

    // Returns exit type.
    __exit_type_t throw_statement_t::exit_type(__exit_type_context_t & ctx)
    {
        return __exit_type_t::throw_;
    }

    ////////// ////////// ////////// ////////// //////////

    // Compiles this statement.
    void goto_statement_t::compile(statement_compile_context_t & ctx) 
    {
        append_jmp(ctx, label);
    }

    // Returns exit type.
    __exit_type_t goto_statement_t::exit_type(__exit_type_context_t & ctx)
    {
        return __exit_type_t::goto_;
    }

    ////////// ////////// ////////// ////////// //////////

    // Compiles this statement.
    void return_statement_t::compile(statement_compile_context_t & ctx) 
    {
        if(expression != nullptr)
            append_expression(ctx, expression);

        append_xilx<return_xilx_t>(ctx);
    }

    // Returns exit type.
    __exit_type_t return_statement_t::exit_type(__exit_type_context_t & ctx)
    {
        return __exit_type_t::return_;
    }

    ////////// ////////// ////////// ////////// //////////

    typedef __statement_region_t<
        __e_mask_t::continue_, __e_mask_t::break_
    > __do_while_statement_region_t;

    //-------- ---------- ---------- ---------- ----------

    // Compiles this statement.
    void do_while_statement_t::compile(statement_compile_context_t & ctx) 
    {
        ctx.begin_region<__do_while_statement_region_t>();

        set_point(ctx, __exit_point_type_t::continue_);
        compile_statement(ctx, body);

        cvalue_t condition_value = execute_expression(ctx, condition);
        if(condition_value == true || condition == nullptr)
        {
            append_jmp(ctx, __exit_point_type_t::continue_);
        }
        else if(condition_value == false)
        {
            // Do nothing
        }
        else
        {
            append_expression(ctx, condition);
            append_jmp(ctx, __exit_point_type_t::continue_, xil_jmp_condition_t::true_);
        }

        set_point(ctx, __exit_point_type_t::break_);
        ctx.end_region();
    }

    // Returns exit type.
    __exit_type_t do_while_statement_t::exit_type(__exit_type_context_t & ctx)
    {
        cvalue_t condition_value = execute_expression(ctx, condition);

        // When condition always true.
        if(condition_value == true || condition == nullptr)
        {
            __e_exit_type_t et_body = __exit_type(ctx, body);
            if(!et_body.has(__FreeExitFlags))
                return __exit_type_t::dead_cycle;

            return et_body.remove(__exit_type_t::break_, __exit_type_t::continue_,
                                  __exit_type_t::pass);
        }

        __e_exit_type_t et_body = __exit_type(ctx, body);
        return et_body.remove(__exit_type_t::break_, __exit_type_t::continue_);
    }

    ////////// ////////// ////////// ////////// //////////

    typedef __statement_region_t<
        __e_mask_t::continue_, __e_mask_t::break_
    > __loop_until_statement_region_t;

    //-------- ---------- ---------- ---------- ----------

    // Compiles this statement.
    void loop_until_statement_t::compile(statement_compile_context_t & ctx) 
    {
        ctx.begin_region<__loop_until_statement_region_t>();

        set_point(ctx, __exit_point_type_t::continue_);
        compile_statement(ctx, body);

        cvalue_t condition_value = execute_expression(ctx, condition);
        if(condition_value == false || condition == nullptr)
        {
            append_jmp(ctx, __exit_point_type_t::continue_);
        }
        else if(condition_value == true)
        {
            // Do nothing
        }
        else
        {
            append_expression(ctx, condition);
            append_jmp(ctx, __exit_point_type_t::continue_, xil_jmp_condition_t::false_);
        }

        set_point(ctx, __exit_point_type_t::break_);
        ctx.end_region();
    }

    // Returns exit type.
    __exit_type_t loop_until_statement_t::exit_type(__exit_type_context_t & ctx)
    {
        cvalue_t condition_value = execute_expression(ctx, condition);

        // When condition always false.
        if(condition_value == false || condition == nullptr)
        {
            __e_exit_type_t et_body = __exit_type(ctx, body);
            if(!et_body.has(__FreeExitFlags))
                return __exit_type_t::dead_cycle;

            return et_body.remove(__exit_type_t::break_, __exit_type_t::continue_,
                                  __exit_type_t::pass);
        }

        // When condition always true.
        if(condition_value == true)
            return __exit_type_t::none;

        __e_exit_type_t et_body = __exit_type(ctx, body);
        return et_body.remove(__exit_type_t::break_, __exit_type_t::continue_);
    }

    ////////// ////////// ////////// ////////// //////////

    typedef __statement_region_t<
        __e_mask_t::continue_, __e_mask_t::break_
    > __while_statement_region_t;

    //-------- ---------- ---------- ---------- ----------

    // Compiles this statement.
    void while_statement_t::compile(statement_compile_context_t & ctx)
    {
        cvalue_t condition_value = execute_expression(ctx, condition);
        if(condition_value == false)
            return;

        ctx.begin_region<__while_statement_region_t>();

        set_point(ctx, __exit_point_type_t::continue_);

        if(!(condition_value == true || condition == nullptr))
        {
            append_expression(ctx, condition);
            append_jmp(ctx, __exit_point_type_t::break_, xil_jmp_condition_t::false_);
        }

        compile_statement(ctx, body);
        append_jmp(ctx, __exit_point_type_t::continue_);

        set_point(ctx, __exit_point_type_t::break_);

        ctx.end_region();
    }

    // Returns exit type.
    __exit_type_t while_statement_t::exit_type(__exit_type_context_t & ctx)
    {
        cvalue_t condition_value = execute_expression(ctx, condition);

        // When condition always false.
        if(condition_value == false)
            return __exit_type_t::none;

        // When condition always true.
        if(condition_value == true || condition == nullptr)
        {
            __e_exit_type_t et_body = __exit_type(ctx, body);
            if(!et_body.has(__FreeExitFlags))
                return __exit_type_t::dead_cycle;

            return et_body.remove(__exit_type_t::break_, __exit_type_t::continue_,
                                  __exit_type_t::pass);
        }

        __e_exit_type_t et_body = __exit_type(ctx, body);
        return et_body.remove(__exit_type_t::break_, __exit_type_t::continue_);
    }

    ////////// ////////// ////////// ////////// //////////

    typedef __statement_region_t<
        __e_mask_t::continue_, __e_mask_t::break_
    > __for_statement_region_t;

    //-------- ---------- ---------- ---------- ----------

    // Compiles this statement.
    void for_statement_t::compile(statement_compile_context_t & ctx)
    {
        ctx.begin_region<__for_statement_region_t>();

        // initialize expression
        if(defination_initialize != nullptr)
        {
            compile_statement(ctx, defination_initialize);
        }
        else if(initialize != nullptr)
        {
            append_xilx<expression_xilx_t>(ctx, initialize);
        }

        cvalue_t condition_value = execute_expression(ctx, condition);
        if(condition_value == false)
        {
            // Do nothing   
        }
        else
        {
            local_label_t label_condition = ctx.next_local_label();

            if(increase != nullptr)
                append_jmp(ctx, label_condition);

            set_point(ctx, __exit_point_type_t::continue_);

            // increase expression
            if(increase != nullptr)
                append_xilx<expression_xilx_t>(ctx, increase);

            append_local_label(ctx, label_condition);
            if(!(condition_value == true || condition != nullptr))
            {
                append_expression(ctx, condition);
                append_jmp(ctx, __exit_point_type_t::break_, xil_jmp_condition_t::false_);
            }

            compile_statement(ctx, body);
            append_jmp(ctx, __exit_point_type_t::continue_);
            set_point(ctx, __exit_point_type_t::break_);
        }

        ctx.end_region();
    }

    // Returns exit type.
    __exit_type_t for_statement_t::exit_type(__exit_type_context_t & ctx)
    {
        cvalue_t condition_value = execute_expression(ctx, condition);

        // When condition always false.
        if(condition_value == false)
            return __exit_type_t::none;

        // When condition always true.
        if(condition_value == true || condition == nullptr)
        {
            __e_exit_type_t et_body = __exit_type(ctx, body);
            if(!et_body.has(__FreeExitFlags))
                return __exit_type_t::dead_cycle;

            return et_body.remove(__exit_type_t::break_, __exit_type_t::continue_,
                                  __exit_type_t::pass);
        }

        __e_exit_type_t et_body = __exit_type(ctx, body);
        return et_body.remove(__exit_type_t::break_, __exit_type_t::continue_);
    }

    ////////// ////////// ////////// ////////// //////////

    // Compiles this statement.
    void for_each_statement_t::compile(statement_compile_context_t & ctx)
    {
        // TODO
        X_UNEXPECTED();
    }

    // Returns exit type.
    __exit_type_t for_each_statement_t::exit_type(__exit_type_context_t & ctx)
    {
        // TODO
        X_UNEXPECTED();
        return __exit_type_t::none;
    }

    ////////// ////////// ////////// ////////// //////////

    typedef __statement_region_t< > __if_statement_region_t, __else_statement_region_t;

    //-------- ---------- ---------- ---------- ----------

    // Compiles this statement.
    void if_statement_t::compile(statement_compile_context_t & ctx)
    {
        cvalue_t condition_value = execute_expression(ctx, condition);
        if(condition_value == true)
        {
            compile_with_region<__if_statement_region_t>(ctx, if_body);
        }
        else if(condition_value == false)
        {
            compile_with_region<__else_statement_region_t>(ctx, else_body);
        }
        else
        {
            // if statement
            ctx.begin_region<__if_statement_region_t>();

            local_label_t label_else = ctx.next_local_label();
            local_label_t label_end  = ctx.next_local_label();

            if(condition != nullptr)
            {
                append_expression(ctx, condition);
                append_jmp(ctx, label_else, xil_jmp_condition_t::false_);
            }

            compile_statement(ctx, if_body);

            append_jmp(ctx, label_end);
            append_local_label(ctx, label_else);

            ctx.end_region();

            // else statement
            if(else_body != nullptr)
            {
                compile_with_region<__else_statement_region_t>(ctx, else_body);
            }

            append_local_label(ctx, label_end);
        }
    }

    // Returns exit type.
    __exit_type_t if_statement_t::exit_type(__exit_type_context_t & ctx)
    {
        cvalue_t condition_value = execute_expression(ctx, condition);

        // When condition always true.
        if(condition_value == true)
            return __exit_type(ctx, if_body);

        // When condition always false.
        if(condition_value == false)
            return __exit_type(ctx, else_body);

        __e_exit_type_t et_if   = __exit_type(ctx, if_body);
        __e_exit_type_t et_else = __exit_type(ctx, else_body);

        __e_exit_type_t et = et_if | et_else;
        if((!et_if || !et_else) && et)
            et.add(__exit_type_t::pass);

        return et;
    }

    ////////// ////////// ////////// ////////// //////////

    typedef __statement_region_t<__e_mask_t::break_> __switch_statement_region_t;

    // Compiles this statement.
    void switch_statement_t::compile(statement_compile_context_t & ctx)
    {
        if(expression == nullptr)
            throw _ED(__e_t::expession_missing, _T("switch"));

        int row_count = __get_rows();
        switch(row_count)
        {
            case 0:
                __compile_as_statement(ctx);
                break;

            case 1:
                __compile_as_if(ctx);
                break;

            default:
                __compile_as_switch(ctx, row_count);
                break;
        }
    }

    // Compiles as switch statement.
    void switch_statement_t::__compile_as_switch(statement_compile_context_t & ctx, int row_count)
    {
        ctx.begin_region<__switch_statement_region_t>();
        append_expression(ctx, expression);

        switch_table_t * tbl = ctx.switch_manager.append_table(row_count);
        __append_switch(ctx, tbl);

        switch_row_t * rows = (switch_row_t *)tbl->rows;

        local_label_t default_case_label = 0;
        std::set<int> case_values;

        for(case_t * c : cases)
        {
            local_label_t case_label = ctx.next_local_label();

            for(expression_t * exp : c->constants)
            {
                if(exp == nullptr)      // default case
                {
                    if(default_case_label != 0)
                        throw _ED(__e_t::duplicate_case, _T("default"));
                    default_case_label = case_label;
                }
                else
                {
                    int32_t value = __read_const_int(ctx, exp);
                    if(!case_values.insert(value).second)
                        throw _ED(__e_t::duplicate_case, execute_expression(ctx, exp));

                    *rows++ = switch_row_t(value, case_label);
                }
            }

            append_local_label(ctx, case_label);
            compile_statements(ctx, c->statements);
        }

        set_point(ctx, __exit_point_type_t::break_);

        if(default_case_label == 0)
        {
            default_case_label = ctx.next_local_label();
            append_local_label(ctx, default_case_label);
        }

        tbl->default_label = default_case_label;

        std::sort(tbl->rows, tbl->rows + tbl->count, [](switch_row_t & r1, switch_row_t & r2) {
            return r1.value < r2.value;
        });

        ctx.end_region();
    }

    // Reads a constant int value.
    int32_t switch_statement_t::__read_const_int(statement_compile_context_t & ctx,
                                                 expression_t * exp)
    {
        if(exp == nullptr)
            throw _ED(__e_t::expession_missing, _T("case"));

        cvalue_t const_value = execute_expression(ctx, exp);
        if(is_nan(const_value))
            throw _ED(__e_t::expected_constant_value);

        if(const_value.value_type != cvalue_type_t::number)
            throw _ED(__e_t::expected_constant_value_type, _T("int"));

        tvalue_t tvalue = const_value.number;
        value_t  value  = tvalue.value;

        switch(tvalue.type)
        {
            case value_type_t::int8_:
                return value.int8_value;

            case value_type_t::uint8_:
                return value.uint8_value;

            case value_type_t::int16_:
                return value.int16_value;

            case value_type_t::uint16_:
                return value.uint16_value;

            case value_type_t::int32_:
                return value.int32_value;

            case value_type_t::uint32_:
                return *(int32_t *)&value.uint32_value;

            case value_type_t::char_:
                return value.char_value;

            case value_type_t::bool_:
                return value.bool_value;

            default:
                throw _ED(__e_t::expected_constant_value_type, _T("int"));
        }
    }

    // Returns rows of cases.
    int switch_statement_t::__get_rows()
    {
        int count = 0;

        for(case_t * c : cases)
        {
            for(expression_t * exp : c->constants)
            {
                if(exp != nullptr)
                    count++;
            }
        }

        return count;
    }

    // Compiles as statement.
    void switch_statement_t::__compile_as_statement(statement_compile_context_t & ctx)
    {
        append_xilx<expression_xilx_t>(ctx, expression);

        if(cases.size() == 0)
            return;

        statements_t * statements = cases[0]->statements;
        if(statements == nullptr)
            return;

        ctx.begin_region<__switch_statement_region_t>();

        compile_statements(ctx, statements);

        set_point(ctx, __exit_point_type_t::break_);
        ctx.end_region();
    }

    // Compiles as if statement.
    void switch_statement_t::__compile_as_if(statement_compile_context_t & ctx)
    {
        statements_t * if_statements = nullptr, * else_statements = nullptr;
        expression_t * if_condition = nullptr;

        for(case_t * c : cases)
        {
            for(expression_t * exp : c->constants)
            {
                if(exp != nullptr)
                {
                    if_condition = exp;
                    if_statements = c->statements;
                }
                else
                {
                    else_statements = c->statements;
                }
            }
        }

        _A(if_condition != nullptr);

        ctx.begin_region<__switch_statement_region_t>();

        if(if_statements == else_statements)
        {
            append_xilx<expression_xilx_t>(ctx, expression);
            compile_statements(ctx, if_statements);
        }
        else
        {
            auto condition = new_expression<binary_expression_t>(ctx.get_memory(),
                operator_t::equal, expression, if_condition
            );

            cvalue_t condition_cvalue = execute_expression(ctx, condition);

            if(condition_cvalue == true)
            {
                compile_statements(ctx, if_statements);
            }
            else if(condition_cvalue == false)
            {
                compile_statements(ctx, else_statements);
            }
            else
            {
                local_label_t label_else = ctx.next_local_label();

                append_expression(ctx, condition);
                append_jmp(ctx, label_else, xil_jmp_condition_t::false_);

                compile_statements(ctx, if_statements);

                append_local_label(ctx, label_else);
                compile_statements(ctx, else_statements);
            }
        }

        set_point(ctx, __exit_point_type_t::break_);
        ctx.end_region();
    }

    // Returns exit type.
    __exit_type_t switch_statement_t::exit_type(__exit_type_context_t & ctx)
    {
        // TODO: how?
        X_UNEXPECTED();
        return __exit_type_t::none;
    }

    ////////// ////////// ////////// ////////// //////////

    template<__block_type_t _block_type>
    class __standalone_statement_region_t : public __statement_region_t< >
    {
        typedef __statement_region_t< > __super_t;

    public:

        // Constructor.
        __standalone_statement_region_t(local_label_t begin, local_label_t end,
                    local_label_t entry_point, type_t * relation_type = nullptr)
            : __begin(begin), __end(end), __entry_point(entry_point)
            , __relation_type(relation_type)
        { }

        // Write xilxes to a pool.
        virtual void write(xilx_write_context_t & ctx, xil_pool_t & pool) override
        {
            ctx.block_manager.append_block(
                _block_type, __begin, __end, __entry_point, __relation_type
            );

            ctx.regions.push(this);
        }

    private:
        local_label_t __begin, __end, __entry_point;
        type_t * __relation_type;
    };

    typedef __statement_region_t< > __try_statement_region_t;
    typedef __standalone_statement_region_t<__block_type_t::catch_>   __catch_statement_region_t;
    typedef __standalone_statement_region_t<__block_type_t::finally_> __finally_statement_region_t;

    // Compiles this statement.
    void try_statement_t::compile(statement_compile_context_t & ctx)
    {
        if(__is_empty_statement(ctx, try_statement))
        {
            compile_statement(ctx, finally_statement);
            return;
        }

        if(__is_empty_statement(ctx, finally_statement) && catches.empty())
        {
            compile_statement(ctx, try_statement);
            return;
        }

        local_label_t label_try     = ctx.next_local_label();
        local_label_t label_try_end = ctx.next_local_label();

        // Compiles try statement.
        ctx.begin_region<__try_statement_region_t>();

        append_local_label(ctx, label_try);
        compile_statement(ctx, try_statement);
        append_local_label(ctx, label_try_end);

        ctx.end_region();

        // Compiles catch statements.
        for(catch_t * c : catches)
        {
            local_label_t label_catch = ctx.next_local_label();

            if(c->variable == nullptr)
                throw _ED(__e_t::catch_exception_variable_undeterminded);

            ctx.begin_region<__catch_statement_region_t>(label_try,
                            label_try_end, label_catch, to_type(c->type_name));

            append_local_label(ctx, label_catch);
            xilx::append_xilx<pop_variable_xilx_t>(ctx, c->variable);
            compile_statement(ctx, c->body);

            ctx.end_region();
        }

        // Compiles finally statement.
        if(!__is_empty_statement(ctx, finally_statement))
        {
            local_label_t label_finally = ctx.next_local_label();

            ctx.begin_region<__finally_statement_region_t>(label_try, label_try_end, label_finally);

            append_local_label(ctx, label_finally);
            compile_statement(ctx, finally_statement);

            ctx.end_region();
        }
    }

    // Returns exit type.
    __exit_type_t try_statement_t::exit_type(__exit_type_context_t & ctx)
    {
        //TODO: X_UNEXPECTED();
        return __exit_type_t::none;
    }

    ////////// ////////// ////////// ////////// //////////

    // Compiles this statement.
    void empty_statement_t::compile(statement_compile_context_t & ctx)
    {
        // Do nothing
    }

    // Returns exit type.
    __exit_type_t empty_statement_t::exit_type(__exit_type_context_t & ctx)
    {
        return __exit_type_t::none;
    }

    // Returns whether the statement is empty.
    bool empty_statement_t::is_empty(xpool_t & xpool)
    {
        return true;
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::compile

