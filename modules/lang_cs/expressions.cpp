
#include "services.h"
#include "utils.h"
#include "expressions.h"

namespace X_ROOT_NS { namespace modules { namespace lang_cs {

    using namespace core;
    using namespace compile;

    ////////// ////////// ////////// ////////// //////////

    // Returns vtype of cs_condition_expression.
    vtype_t cs_condition_expression_t::get_vtype() const
    {
        return value1()->get_vtype();
    }

    // Returns type of cs_condigion_expression.
    type_t * cs_condition_expression_t::get_type() const
    {
        return value1()->get_type();
    }

    // Compiles this expression.
    void cs_condition_expression_t::compile(expression_compile_context_t & ctx,
										xil_pool_t & pool, xil_type_t dtype)
    {
        //_PF(_T("__compile: %1%"), this);

        statement_compile_context_t & sctx = ctx.statement_ctx;

        bool value1_effective = is_effective(value1());
        bool value2_effective = is_effective(value2());

        cvalue_t condition_value = execute_expression(ctx, this->condition());

        if (value1_effective || value2_effective)
        {
            if (condition_value == true)
            {
                value1()->compile(ctx, pool);
            }
            else if (condition_value == false)
            {
                value2()->compile(ctx, pool);
            }
            else
            {
                condition()->compile(ctx, pool);
                jmp_xil_t * false_jmp_xil = xil::append_jmp_xil(pool, xil_jmp_model_t::false_);

                value1()->compile(ctx, pool);
                jmp_xil_t * finally_jmp_xil = xil::append_jmp_xil(pool, xil_jmp_model_t::none);

                if (false_jmp_xil != nullptr)
                    xil::append_label_xil(sctx, pool, false_jmp_xil);

                value2()->compile(ctx, pool);

                xil::append_label_xil(sctx, pool, finally_jmp_xil);
            }
        }
    }

    // Executes this expression.
    cvalue_t cs_condition_expression_t::execute(expression_execute_context_t & ctx)
    {
        cvalue_t condition_value = execute_expression(ctx, this->condition());

        if (condition_value == true)
            return execute_expression(ctx, this->value1());

        if (condition_value == false)
            return execute_expression(ctx, this->value2());

        return cvalue_t::nan;
    }

    ////////// ////////// ////////// ////////// //////////

    // Compiles single expression.
    static void __compile_single_expression(expression_compile_context_t & ctx,
                                            xil_pool_t & pool, expression_t * exp)
    {
        if (exp != nullptr)
        {
            expression_t * parent_exp = exp->parent;
            exp->parent = nullptr;

            exp->compile(ctx, pool);

            exp->parent = parent_exp;
        }
    }

    // Compiles this expression.
    void cs_expressions_t::compile(expression_compile_context_t & ctx, xil_pool_t & pool,
											xil_type_t dtype)
    {
        _A(this->expression_count() > 0);

        for (auto it = this->begin(), it_end = this->end() - 1; it < it_end; it++)
        {
            __compile_single_expression(ctx, pool, *it);
        }

        // last expression
        expression_t * last_exp = *(this->end() - 1);
        last_exp->compile(ctx, pool);
    }

    // Executes this expression.
    cvalue_t cs_expressions_t::execute(expression_execute_context_t & ctx)
    {
        return cvalue_t::nan;
    }

    ////////// ////////// ////////// ////////// //////////

} } }
