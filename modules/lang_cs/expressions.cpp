
#include "services.h"
#include "utils.h"
#include "expressions.h"

namespace X_ROOT_NS::modules::lang_cs {

    using namespace core;
    using namespace compile;

    ////////// ////////// ////////// ////////// //////////

    // The numeric type that both branches of a condition expression convert to,
    // nullptr when the branches are not numbers of different types.
    static type_t * __numeric_result_type(expression_t * value1, expression_t * value2)
    {
        vtype_t vtype1 = value1->get_vtype(), vtype2 = value2->get_vtype();
        if (vtype1 == vtype2 || !is_numberic_vtype(vtype1) || !is_numberic_vtype(vtype2))
            return nullptr;

        vtype_t vtype = get_binary_vtype(operator_t::add, vtype1, vtype2);
        if (vtype == vtype_t::__unknown__ || vtype == vtype_t::__default__)
            return nullptr;

        return __XPool.get_internal_type(vtype);
    }

    // Returns vtype of cs_condition_expression.
    vtype_t cs_condition_expression_t::get_vtype() const
    {
        if (type_t * type = __numeric_result_type(value1(), value2()); type != nullptr)
            return type->this_vtype();

        return value1()->get_vtype();
    }

    // Returns type of cs_condigion_expression.
    type_t * cs_condition_expression_t::get_type() const
    {
        if (type_t * type = __numeric_result_type(value1(), value2()); type != nullptr)
            return type;

        return value1()->get_type();
    }

    // Compiles this expression.
    void cs_condition_expression_t::compile(__cctx_t & ctx, xil_pool_t & pool,
                                                const __environment_t & env)
    {
        //_PF(_T("__compile: %1%"), this);

        statement_compile_context_t & sctx = ctx.statement_ctx;

        bool value1_effective = is_effective(ctx, value1());
        bool value2_effective = is_effective(ctx, value2());

        cvalue_t condition_value = execute_expression(this->condition());

        // Numbers of different types: both branches are converted to the type of the
        // whole expression.
        type_t * result_type = __numeric_result_type(value1(), value2());
        __environment_t branch_env = result_type != nullptr? __environment_t(result_type)
                                                           : __environment_t::empty;

        if (value1_effective || value2_effective)
        {
            if (condition_value == true)
            {
                value1()->compile(ctx, pool, branch_env);
            }
            else if (condition_value == false)
            {
                value2()->compile(ctx, pool, branch_env);
            }
            else
            {
                condition()->compile(ctx, pool);
                jmp_xil_t * false_jmp_xil = xil::append_jmp_xil(pool, xil_jmp_model_t::false_);

                value1()->compile(ctx, pool, branch_env);
                jmp_xil_t * finally_jmp_xil = xil::append_jmp_xil(pool, xil_jmp_model_t::none);

                if (false_jmp_xil != nullptr)
                    xil::append_label_xil(sctx, pool, false_jmp_xil);

                value2()->compile(ctx, pool, branch_env);

                xil::append_label_xil(sctx, pool, finally_jmp_xil);
            }

            // The value is used as a number of another type.
            type_t * type = this->get_type();
            if (env.dtype != xil_type_t::empty && type != nullptr && is_value_type(type)
                && env.type != nullptr && is_value_type(env.type))
            {
                xil_type_t dtype = to_xil_type(type);
                if (dtype != xil_type_t::empty && dtype != env.dtype)
                    pool.append<x_push_convert_xil_t>(dtype, env.dtype);
            }
        }
    }

    // Executes this expression.
    cvalue_t cs_condition_expression_t::execute()
    {
        cvalue_t condition_value = execute_expression(this->condition());

        if (condition_value == true)
            return execute_expression(this->value1());

        if (condition_value == false)
            return execute_expression(this->value2());

        return cvalue_t::nan;
    }

    ////////// ////////// ////////// ////////// //////////

    // Compiles single expression.
    static void __compile_single_expression(__cctx_t & ctx, xil_pool_t & pool, expression_t * exp)
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
    void cs_expressions_t::compile(__cctx_t & ctx, xil_pool_t & pool, const __environment_t & env)
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
    cvalue_t cs_expressions_t::execute()
    {
        return cvalue_t::nan;
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::lang_cs
