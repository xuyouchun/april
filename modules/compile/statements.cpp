
#include <compile.h>
#include "utils.h"

namespace X_ROOT_NS { namespace modules { namespace compile {

    using namespace core;
    using namespace xilx;

    typedef statement_compile_context_t __context_t;
    typedef statement_exit_point_type_t __exit_point_type_t;
    typedef compile_error_code_t        __e_t;

    //-------- ---------- ---------- ---------- ----------

    namespace xilx
    {
        point_jmp_xilx_t * append_jmp(__context_t & ctx, __exit_point_type_t type,
                                xil_jmp_condition_t jmp_condition)
        {
            return append_xilx<point_jmp_xilx_t>(ctx, type, jmp_condition);
        }

        local_label_jmp_xilx_t * append_jmp(__context_t & ctx, local_label_t label,
                                xil_jmp_condition_t jmp_condition)
        {
            return append_xilx<local_label_jmp_xilx_t>(ctx, label, jmp_condition);
        }

        global_label_jmp_xilx_t * append_jmp(__context_t & ctx, name_t name,
                                xil_jmp_condition_t jmp_condition)
        {
            return append_xilx<global_label_jmp_xilx_t>(ctx, name, jmp_condition);
        }

        global_label_jmp_xilx_t * append_jmp(__context_t & ctx, const string_t & name,
                                xil_jmp_condition_t jmp_condition)
        {
            return append_jmp(ctx, ctx.to_name(name), jmp_condition);
        }

        switch_jmp_xilx_t * __append_switch(__context_t & ctx, switch_table_t * tbl)
        {
            return append_xilx<switch_jmp_xilx_t>(ctx, tbl);
        }

        //-------- ---------- ---------- ---------- ----------

        global_label_xilx_t * append_global_label(__context_t & ctx, name_t name)
        {
            return append_xilx<global_label_xilx_t>(ctx, name);
        }

        global_label_xilx_t * append_global_label(__context_t & ctx, const string_t & name)
        {
            return append_global_label(ctx, ctx.to_name(name));
        }

        local_label_xilx_t * append_local_label(__context_t & ctx, local_label_t flag)
        {
            return append_xilx<local_label_xilx_t>(ctx, flag);
        }

        //-------- ---------- ---------- ---------- ----------

        local_label_xilx_t * set_point(__context_t & ctx, __exit_point_type_t point_type)
        {
            local_label_xilx_t * label = append_xilx<local_label_xilx_t>(
                ctx, ctx.next_local_label()
            );

            ctx.set_point(point_type, label);

            return label;
        }

        //-------- ---------- ---------- ---------- ----------

        void append_condition(__context_t & ctx, expression_t * condition)
        {
            if(condition != nullptr)
            {
                set_assign_parent(condition);
                append_xilx<expression_xilx_t>(ctx, condition);
            }
        }
    }

    void compile_statement(__context_t & ctx, statement_t * statement)
    {
        if(statement != nullptr)
            statement->compile(ctx);
    }

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

    ////////// ////////// ////////// ////////// //////////

    X_ENUM(__exit_point_mask_t)

        #define __EMask(v)  (1 << (int)v)

        self        = __EMask(__exit_point_type_t::self),
        break_      = __EMask(__exit_point_type_t::break_),
        continue_   = __EMask(__exit_point_type_t::continue_),

    X_ENUM_END

    X_ENUM_INFO(__exit_point_mask_t)

        X_C(self,       _T("self"))
        X_C(break_,     _T("break"))
        X_C(continue_,  _T("continue"))

    X_ENUM_INFO_END

    typedef __exit_point_mask_t __e_mask_t;

    __exit_point_mask_t __e_mask(__exit_point_type_t point_type)
    {
        return (__exit_point_mask_t)__EMask(point_type);
    }

    //-------- ---------- ---------- ---------- ----------

    template<__exit_point_mask_t ... _masks>
    class __statement_region_t : public statement_region_t
    {
        typedef statement_region_t __super_t;
        static const __exit_point_mask_t mask = enum_xor<__exit_point_mask_t>(_masks ...);

    public:
        using __super_t::__super_t;

        virtual void set_point(__exit_point_type_t type, statement_point_t * label) override
        {
            if(!__is_supported(type))
                throw _ED(__e_t::unexpected_jmp, type);

            __super_t::set_point(type, label);
        }

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
        bool __is_supported(__exit_point_type_t type)
        {
            return enum_has_flag(mask, __e_mask(type));
        }
    };

    ////////// ////////// ////////// ////////// //////////

    cvalue_t execute_expression(statement_compile_context_t & ctx, expression_t * exp)
    {
        if(exp == nullptr)
            return cvalue_t::nan;

        expression_execute_context_t ectx(ctx.xpool());
        return exp->execute(ectx);
    }

    cvalue_t execute_expression(expression_compile_context_t & ctx, expression_t * exp)
    {
        return execute_expression(ctx.statement_ctx, exp);
    }

    cvalue_t execute_expression(expression_execute_context_t & ctx, expression_t * exp)
    {
        if(exp == nullptr)
            return cvalue_t::nan;

        return exp->execute(ctx);
    }

    ////////// ////////// ////////// ////////// //////////

    void statement_base_t::compile(statement_compile_context_t & ctx)
    {
        on_compile(ctx);
    }

    ////////// ////////// ////////// ////////// //////////

    void statement_group_t::push_back(statement_t * statement)
    {
        _A(statement != nullptr);

        __statements.push_back(statement);
    }

    void statement_group_t::on_compile(statement_compile_context_t & ctx)
    {
        for(statement_t * statement : __statements)
        {
            compile_statement(ctx, statement);
        }
    }

    ////////// ////////// ////////// ////////// //////////

    void expression_statement_t::on_compile(statement_compile_context_t & ctx)
    {
        if(expression != nullptr)
            append_xilx<expression_xilx_t>(ctx, expression);
    }

    ////////// ////////// ////////// ////////// //////////

    void type_def_statement_t::on_compile(statement_compile_context_t & ctx)
    {

    }

    ////////// ////////// ////////// ////////// //////////

    X_DEFINE_TO_STRING(defination_statement_item_t)
    {
        if(expression == nullptr)
            return _str(name);

        return _F(_T("%1% = %2%"), _str(name), _str(expression));
    }

    X_DEFINE_TO_STRING(defination_statement_t)
    {
        string_t names = al::join_str(items.begin(), items.end(), _T(", "),
            [](auto & it) { return _str(it); }
        );

        return _F(_T("%1% %2%"), _str(type_name), names);
    }

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

    void set_assign_parent(expression_t * exp)
    {
        if(exp != nullptr)
            exp->parent = &__assign_expression_instance;
    }

    void defination_statement_t::on_compile(statement_compile_context_t & ctx)
    {
        for(defination_statement_item_t * item : items)
        {
            _A(item->variable != nullptr);

            if(item->expression != nullptr)
            {
                set_assign_parent(item->expression);

                append_xilx<expression_xilx_t>(ctx, item->expression);
                append_xilx<local_assign_xilx_t>(ctx, item->variable);
            }
        }
    }

    ////////// ////////// ////////// ////////// //////////

    void break_statement_t::on_compile(statement_compile_context_t & ctx) 
    {
        append_jmp(ctx, __exit_point_type_t::break_);
    }

    ////////// ////////// ////////// ////////// //////////

    void continue_statement_t::on_compile(statement_compile_context_t & ctx) 
    {
        append_jmp(ctx, __exit_point_type_t::continue_);
    }

    ////////// ////////// ////////// ////////// //////////

    void throw_statement_t::on_compile(statement_compile_context_t & ctx) 
    {
        
    }

    ////////// ////////// ////////// ////////// //////////

    void goto_statement_t::on_compile(statement_compile_context_t & ctx) 
    {
        append_jmp(ctx, label);
    }

    ////////// ////////// ////////// ////////// //////////

    void return_statement_t::on_compile(statement_compile_context_t & ctx) 
    {
        if(expression != nullptr)
        {
            set_assign_parent(expression);
            append_xilx<expression_xilx_t>(ctx, expression);
        }

        append_xilx<return_xilx_t>(ctx);
    }

    ////////// ////////// ////////// ////////// //////////

    typedef __statement_region_t<
        __e_mask_t::continue_, __e_mask_t::break_
    > __do_while_statement_region_t;

    //-------- ---------- ---------- ---------- ----------

    void do_while_statement_t::on_compile(statement_compile_context_t & ctx) 
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
            // do nothing
        }
        else
        {
            append_condition(ctx, condition);
            append_jmp(ctx, __exit_point_type_t::continue_, xil_jmp_condition_t::true_);
        }

        set_point(ctx, __exit_point_type_t::break_);
        ctx.end_region();
    }

    ////////// ////////// ////////// ////////// //////////

    typedef __statement_region_t<
        __e_mask_t::continue_, __e_mask_t::break_
    > __while_statement_region_t;

    //-------- ---------- ---------- ---------- ----------

    void while_statement_t::on_compile(statement_compile_context_t & ctx)
    {
        cvalue_t condition_value = execute_expression(ctx, condition);
        if(condition_value == false)
            return;

        ctx.begin_region<__while_statement_region_t>();

        set_point(ctx, __exit_point_type_t::continue_);

        if(!(condition_value == true || condition == nullptr))
        {
            append_condition(ctx, condition);
            append_jmp(ctx, __exit_point_type_t::break_, xil_jmp_condition_t::false_);
        }

        compile_statement(ctx, body);
        append_jmp(ctx, __exit_point_type_t::continue_);

        set_point(ctx, __exit_point_type_t::break_);

        ctx.end_region();
    }

    ////////// ////////// ////////// ////////// //////////

    typedef __statement_region_t<
        __e_mask_t::continue_, __e_mask_t::break_
    > __for_statement_region_t;

    //-------- ---------- ---------- ---------- ----------

    void for_statement_t::on_compile(statement_compile_context_t & ctx)
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
            // do nothing   
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
            if(!(condition_value == true || condition == nullptr))
            {
                append_condition(ctx, condition);
                append_jmp(ctx, __exit_point_type_t::break_, xil_jmp_condition_t::false_);
            }

            compile_statement(ctx, body);
            append_jmp(ctx, __exit_point_type_t::continue_);
            set_point(ctx, __exit_point_type_t::break_);
        }

        ctx.end_region();
    }

    ////////// ////////// ////////// ////////// //////////

    void for_each_statement_t::on_compile(statement_compile_context_t & ctx)
    {

    }

    ////////// ////////// ////////// ////////// //////////

    typedef __statement_region_t< > __if_statement_region_t, __else_statement_region_t;

    //-------- ---------- ---------- ---------- ----------

    void if_statement_t::on_compile(statement_compile_context_t & ctx)
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
                append_condition(ctx, condition);
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

    ////////// ////////// ////////// ////////// //////////

    typedef __statement_region_t<__e_mask_t::break_> __switch_statement_region_t;

    void switch_statement_t::on_compile(statement_compile_context_t & ctx)
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

    void switch_statement_t::__compile_as_switch(statement_compile_context_t & ctx, int row_count)
    {
        ctx.begin_region<__switch_statement_region_t>();
        append_condition(ctx, expression);

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

                append_condition(ctx, condition);
                append_jmp(ctx, label_else, xil_jmp_condition_t::false_);

                compile_statements(ctx, if_statements);

                append_local_label(ctx, label_else);
                compile_statements(ctx, else_statements);
            }
        }

        set_point(ctx, __exit_point_type_t::break_);
        ctx.end_region();
    }

    ////////// ////////// ////////// ////////// //////////

    void try_statement_t::on_compile(statement_compile_context_t & ctx)
    {

    }

    ////////// ////////// ////////// ////////// //////////

    void empty_statement_t::on_compile(statement_compile_context_t & ctx)
    {
        // do nothing
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::compile

