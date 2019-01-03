#ifndef __COMPILE_EXPRESSION_H__
#define __COMPILE_EXPRESSION_H__

namespace X_ROOT_NS { namespace modules { namespace compile {

    namespace
    {
        using namespace core;
        using namespace std::placeholders;
    }

    ////////// ////////// ////////// ////////// //////////

    template<typename _expression_t>
    class system_expression_t : public _expression_t
    {
        typedef _expression_t __super_t;

    public:
        using __super_t::__super_t;
    };

    ////////// ////////// ////////// ////////// //////////

    #define __SystemExpressionT(exp_t)                      \
        template<>                                          \
        class system_expression_t<exp_t> : public exp_t     \
        {                                                   \
            typedef exp_t __super_t;                        \
        public:                                             \
            using __super_t::__super_t;

    #define __EndSystemExpressionT                          \
        };

    ////////// ////////// ////////// ////////// //////////

    __SystemExpressionT(binary_expression_t)

        virtual cvalue_t execute(expression_execute_context_t & ctx) override;
        virtual void compile(expression_compile_context_t & ctx, xil_pool_t & pool) override;

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    __SystemExpressionT(unitary_expression_t)

        virtual cvalue_t execute(expression_execute_context_t & ctx) override;
        virtual void compile(expression_compile_context_t & ctx, xil_pool_t & pool) override;

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    __SystemExpressionT(name_expression_t)

        virtual cvalue_t execute(expression_execute_context_t & ctx) override;
        virtual void compile(expression_compile_context_t & ctx, xil_pool_t & pool) override;

    private:
        void __compile_variable(expression_compile_context_t & ctx, xil_pool_t & pool);
        void __compile_local_variable(expression_compile_context_t & ctx, xil_pool_t & pool,
                                                                    local_variable_t * variable);

        void __compile_param_variable(expression_compile_context_t & ctx, xil_pool_t & pool,
                                                                    param_variable_t * variable);

        void __compile_field_variable(expression_compile_context_t & ctx, xil_pool_t & pool,
                                                                    field_variable_t * field);

        void __compile_property_variable(expression_compile_context_t & ctx, xil_pool_t & pool,
                                                                    property_variable_t * property);

        void __compile_type(expression_compile_context_t & ctx, xil_pool_t & pool);
        void __compile_type_def(expression_compile_context_t & ctx, xil_pool_t & pool);

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    __SystemExpressionT(cvalue_expression_t)

        virtual cvalue_t execute(expression_execute_context_t & ctx) override;
        virtual void compile(expression_compile_context_t & ctx, xil_pool_t & pool) override;

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    __SystemExpressionT(function_expression_t)

        virtual cvalue_t execute(expression_execute_context_t & ctx) override;
        virtual void compile(expression_compile_context_t & ctx, xil_pool_t & pool) override;

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    __SystemExpressionT(type_cast_expression_t)

        virtual cvalue_t execute(expression_execute_context_t & ctx) override;
        virtual void compile(expression_compile_context_t & ctx, xil_pool_t & pool) override;

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    __SystemExpressionT(type_name_expression_t)

        virtual cvalue_t execute(expression_execute_context_t & ctx) override;
        virtual void compile(expression_compile_context_t & ctx, xil_pool_t & pool) override;

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    __SystemExpressionT(index_expression_t)

        virtual cvalue_t execute(expression_execute_context_t & ctx) override;
        virtual void compile(expression_compile_context_t & ctx, xil_pool_t & pool) override;

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    __SystemExpressionT(new_expression_t)

        virtual cvalue_t execute(expression_execute_context_t & ctx) override;
        virtual void compile(expression_compile_context_t & ctx, xil_pool_t & pool) override;

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    __SystemExpressionT(new_array_expression_t)

        virtual cvalue_t execute(expression_execute_context_t & ctx) override;
        virtual void compile(expression_compile_context_t & ctx, xil_pool_t & pool) override;

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    __SystemExpressionT(default_value_expression_t)

        virtual cvalue_t execute(expression_execute_context_t & ctx) override;
        virtual void compile(expression_compile_context_t & ctx, xil_pool_t & pool) override;

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    __SystemExpressionT(type_of_expression_t)

        virtual cvalue_t execute(expression_execute_context_t & ctx) override;
        virtual void compile(expression_compile_context_t & ctx, xil_pool_t & pool) override;

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    __SystemExpressionT(this_expression_t)

        virtual cvalue_t execute(expression_execute_context_t & ctx) override;
        virtual void compile(expression_compile_context_t & ctx, xil_pool_t & pool) override;

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    __SystemExpressionT(base_expression_t)

        virtual cvalue_t execute(expression_execute_context_t & ctx) override;
        virtual void compile(expression_compile_context_t & ctx, xil_pool_t & pool) override;

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    bool is_effective(expression_t * exp);

    ////////// ////////// ////////// ////////// //////////

    #undef __SystemExpressionT
    #undef __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    template<typename _expression_t, typename ... args_t>
    expression_t * new_expression(memory_t * memory, args_t && ... args)
    {
        typedef system_expression_t<_expression_t> expression_t;
        return memory_t::new_obj<expression_t>(memory, std::forward<args_t>(args) ...);
    }

    template<typename _expression_t, typename ... args_t>
    expression_t * new_expression(memory_t * memory, operator_property_t * op_property,
                                                                args_t && ... args)
    {
        return new_expression<_expression_t>(
            memory, op_property, std::forward<args_t>(args) ...
        );
    }

    template<typename _expression_t, typename ... args_t>
    expression_t * new_expression(memory_t * memory, operator_t op, args_t && ... args)
    {
        const operator_property_t * op_property = get_system_operator_property(op);
        _A(op_property != nullptr);

        return new_expression<_expression_t>(
            memory, op_property, std::forward<args_t>(args) ...
        );
    }

    ////////// ////////// ////////// ////////// //////////

} } }

#endif // __COMPILE_EXPRESSION_H__
