#ifndef __COMPILE_EXPRESSION_H__
#define __COMPILE_EXPRESSION_H__

namespace X_ROOT_NS::modules::compile {

    namespace
    {
        using namespace core;
        using namespace std::placeholders;

        typedef expression_compile_context_t        __cctx_t;
        typedef expression_compile_environment_t    __environment_t;
    }

    ////////// ////////// ////////// ////////// //////////

    // Generic system expression.
    template<typename _expression_t>
    class system_expression_t : public _expression_t
    {
        typedef _expression_t __super_t;

    public:
        using __super_t::__super_t;
    };

    ////////// ////////// ////////// ////////// //////////

    #define __SystemExpressionT(exp_t)                                                  \
        template<>                                                                      \
        class system_expression_t<exp_t> : public exp_t                                 \
        {                                                                               \
            typedef exp_t __super_t;                                                    \
        public:                                                                         \
            using __super_t::__super_t;

    #define __EndSystemExpressionT                                                      \
        };

    ////////// ////////// ////////// ////////// //////////

    // Binary expression.
    __SystemExpressionT(binary_expression_t)

        // Execute the expression.
        virtual cvalue_t execute() override;

        // Compile the expression.
        virtual void compile(__cctx_t & ctx, xil_pool_t & pool, const __environment_t & env)
                                                                                override;

    private:

        // Compile operator overload.
        void __compile_operator_overload(expression_compile_context_t & ctx, xil_pool_t & pool);

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    // Unitary expression.
    __SystemExpressionT(unitary_expression_t)

        // Execute the expression.
        virtual cvalue_t execute() override;

        // Compile the expression.
        virtual void compile(__cctx_t & ctx, xil_pool_t & pool, const __environment_t & env)
                                                                                override;

    private:

        // Compile operator overload.
        void __compile_operator_overload(expression_compile_context_t & ctx, xil_pool_t & pool);

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    // Name expression.
    __SystemExpressionT(name_expression_t)

        // Execute the expression.
        virtual cvalue_t execute() override;

        // Compile the expression.
        virtual void compile(__cctx_t & ctx, xil_pool_t & pool, const __environment_t & env)
                                                                                override;

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    // Name unit expression.
    __SystemExpressionT(name_unit_expression_t)

        // Execute the expression.
        virtual cvalue_t execute() override;

        // Compile the expression.
        virtual void compile(__cctx_t & ctx, xil_pool_t & pool, const __environment_t & env)
                                                                                override;

    __EndSystemExpressionT


    ////////// ////////// ////////// ////////// //////////

    // Constant value expression.
    __SystemExpressionT(cvalue_expression_t)

        // Execute the expression.
        virtual cvalue_t execute() override;

        // Compile the expression.
        virtual void compile(__cctx_t & ctx, xil_pool_t & pool, const __environment_t & env)
                                                                                override;

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    // Function expression.
    __SystemExpressionT(function_expression_t)

        // Execute the expression.
        virtual cvalue_t execute() override;

        // Compile the expression.
        virtual void compile(__cctx_t & ctx, xil_pool_t & pool, const __environment_t & env)
                                                                                override;

    private:

        // Compile method calling expression.
        void __compile_method(expression_compile_context_t & ctx, xil_pool_t & pool,
                                                        const __environment_t & env);

        // Compile delegate calling expression.
        void __compile_delegate(expression_compile_context_t & ctx, xil_pool_t & pool,
                                                        const __environment_t & env);

        // Compile delegate calling expression, for complex namex expressions.
        void __compile_default(expression_compile_context_t & ctx, xil_pool_t & pool,
                                                        const __environment_t & env);

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    // Type cast expression.
    __SystemExpressionT(type_cast_expression_t)

        // Execute the expression.
        virtual cvalue_t execute() override;

        // Compile the expression.
        virtual void compile(__cctx_t & ctx, xil_pool_t & pool, const __environment_t & env)
                                                                                override;

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    // Type name expression.
    __SystemExpressionT(type_name_expression_t)

        // Execute the expression.
        virtual cvalue_t execute() override;

        // Compile the expression.
        virtual void compile(__cctx_t & ctx, xil_pool_t & pool, const __environment_t & env)
                                                                                override;

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    // Index expression.
    __SystemExpressionT(index_expression_t)

        // Execute the expression.
        virtual cvalue_t execute() override;

        // Compile the expression.
        virtual void compile(__cctx_t & ctx, xil_pool_t & pool, const __environment_t & env)
                                                                                override;

        // Compile array index.
        void __compile_array_index(expression_compile_context_t & ctx, xil_pool_t & pool,
                array_index_variable_t * variable, expression_t * namex,
                const __environment_t & env);

        // Compile property index.
        void __compile_property_index(expression_compile_context_t & ctx, xil_pool_t & pool,
                property_index_variable_t * variable, expression_t * namex,
                const __environment_t & env);

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    // New expression.
    __SystemExpressionT(new_expression_t)

        // Execute the expression.
        virtual cvalue_t execute() override;

        // Compile the expression.
        virtual void compile(__cctx_t & ctx, xil_pool_t & pool, const __environment_t & env)
                                                                                override;

    private:

        // Creates instance of class type.
        void __compile_new_class_object(expression_compile_context_t & ctx, xil_pool_t & pool,
                                                    type_t * type);

        // Creates instance of struct type.
        void __compile_new_struct_object(expression_compile_context_t & ctx, xil_pool_t & pool,
                                                    type_t * type);

        // Creates instance of a generic param type.
        void __compile_generic_param(expression_compile_context_t & ctx, xil_pool_t & pool,
                                                    generic_param_t * generic_param);

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    // New array expression.
    __SystemExpressionT(new_array_expression_t)

        // Execute the expression.
        virtual cvalue_t execute() override;

        // Compile the expression.
        virtual void compile(__cctx_t & ctx, xil_pool_t & pool, const __environment_t & env)
                                                                                override;

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    // Default value expression.
    __SystemExpressionT(default_value_expression_t)

        // Execute the expression.
        virtual cvalue_t execute() override;

        // Compile the expression.
        virtual void compile(__cctx_t & ctx, xil_pool_t & pool, const __environment_t & env)
                                                                                override;

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    // Type of expression.
    __SystemExpressionT(type_of_expression_t)

        // Execute the expression.
        virtual cvalue_t execute() override;

        // Compile the expression.
        virtual void compile(__cctx_t & ctx, xil_pool_t & pool, const __environment_t & env)
                                                                                override;

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    // This expression.
    __SystemExpressionT(this_expression_t)

        // Execute the expression.
        virtual cvalue_t execute() override;

        // Compile the expression.
        virtual void compile(__cctx_t & ctx, xil_pool_t & pool, const __environment_t & env)
                                                                                override;

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    // Base expression.
    __SystemExpressionT(base_expression_t)

        // Execute the expression.
        virtual cvalue_t execute() override;

        // Compile the expression.
        virtual void compile(__cctx_t & ctx, xil_pool_t & pool, const __environment_t & env)
                                                                                override;

    __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    // Returns whether the expression is effective.
    // Xil will not generated when it is not effective.
    bool is_effective(expression_t * exp);

    // Returns whether the expression is effective.
    // Xil will not generated when it is not effective.
    // Always returns true when "remove_unnecessary_expressions" switch is on.
    template<typename _ctx_t>
    bool is_effective(_ctx_t & ctx, expression_t * exp)
    {
        if (exp == nullptr)
            return false;

        if (!is_optimize(ctx, compile_optimize_code_t::remove_unnecessary_expressions))
            return true;

        return is_effective(exp);
    }

    ////////// ////////// ////////// ////////// //////////

    #undef __SystemExpressionT
    #undef __EndSystemExpressionT

    ////////// ////////// ////////// ////////// //////////

    // Creates new expression.
    template<typename _expression_t, typename ... args_t>
    expression_t * new_expression(memory_t * memory, args_t && ... args)
    {
        typedef system_expression_t<_expression_t> expression_t;
        return memory_t::new_obj<expression_t>(memory, std::forward<args_t>(args) ...);
    }

    // Creates new expression.
    template<typename _expression_t, typename ... args_t>
    expression_t * new_expression(memory_t * memory, operator_property_t * op_property,
                                                                args_t && ... args)
    {
        return new_expression<_expression_t>(
            memory, op_property, std::forward<args_t>(args) ...
        );
    }

    // Creates new expression.
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

}   // namespace X_ROOT_NS::modules::compile

#endif // __COMPILE_EXPRESSION_H__
