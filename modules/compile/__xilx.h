
#ifndef __COMPILE_XILX_H__
#define __COMPILE_XILX_H__

namespace X_ROOT_NS { namespace modules { namespace compile {

    namespace
    {
        using namespace core;

        typedef xilx_write_context_t        __xw_context_t;
        typedef statement_region_t          __region_t;
        typedef statement_exit_point_type_t __exit_point_type_t;

        typedef statement_compile_context_t  __sctx_t;
        typedef expression_compile_context_t __cctx_t;
    }

    ////////// ////////// ////////// ////////// //////////
    // xilx_base_t

    // Base class of xilx.
    class xilx_base_t : public xilx_t
    {
    public:

        // Constructor.
        xilx_base_t(__region_t & region) : __region(region) { }

    protected:
        __region_t & __region;
    };

    ////////// ////////// ////////// ////////// //////////
    // label_xilx_t

    // Label xilx.
    class label_xilx_t : public xilx_base_t
    {
        typedef xilx_base_t __super_t;

    public:

        // Constructor.
        label_xilx_t(__region_t & region) : __super_t(region) { }

        // Converts to string.
        virtual const string_t to_string() const override { return _T("label"); }
    };

    //-------- ---------- ---------- ---------- ----------
    // local_label_xilx_t

    // Local lable xilx.
    class local_label_xilx_t : public label_xilx_t
                             , public statement_point_t
    {
        typedef label_xilx_t __super_t;

    public:

        // Constructor.
        local_label_xilx_t(__region_t & region, local_label_t flag)
            : __super_t(region), flag(flag) { }

        // Writes xil to a pool.
        virtual void write(__xw_context_t & ctx, xil_pool_t & pool) override;

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return _F(_T("local label: %1%"), flag);
        }

        // Returns this position.
        virtual local_label_t to_position() override { return flag; }

        local_label_t flag;
    };

    //-------- ---------- ---------- ---------- ----------
    // global_label_xilx_t

    // Global lable xilx.
    class global_label_xilx_t : public label_xilx_t
    {
        typedef label_xilx_t __super_t;

    public:

        // Constructor.
        global_label_xilx_t(__region_t & region, name_t name)
            : __super_t(region), name(name) { }

        // Writes to a pool.
        virtual void write(__xw_context_t & ctx, xil_pool_t & pool) override;

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return _F(_T("global label: %1%"), name);
        }

        const name_t name;
    };

    ////////// ////////// ////////// ////////// //////////
    // local_assign_xilx_t

    // Local assign xilx.
    class local_assign_xilx_t : public xilx_base_t
    {
        typedef xilx_base_t __super_t;

    public:

        // Constructor.
        local_assign_xilx_t(__region_t & region, local_variable_t * local,
                                expression_t * expression = nullptr)
            : __super_t(region), local(local), expression(expression)
        {
            _A(local != nullptr);
        }

        // Writes xil to a pool.
        virtual void write(__xw_context_t & ctx, xil_pool_t & pool) override;

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return _F(_T("local defination: %1% = %2%"), local, expression);
        }

        // Local variable.
        local_variable_t * local = nullptr;

        // Init expression.
        expression_t * expression = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////
    // return_xilx_t

    // Return xilx.
    class return_xilx_t : public xilx_base_t
    {
        typedef xilx_base_t __super_t;

    public:

        // Constructor.
        return_xilx_t(__region_t & region) : __super_t(region) { }

        // Writes xilx to a pool.
        virtual void write(__xw_context_t & ctx, xil_pool_t & pool) override;

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return _T("ret");
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // throw_xilx_t

    // Throw xilx.
    class throw_xilx_t : public xilx_base_t
    {
        typedef xilx_base_t __super_t;

    public:

        // Consturctor.
        throw_xilx_t(__region_t & region) : __super_t(region) { }

        // Writes xilx to a pool.
        virtual void write(__xw_context_t & ctx, xil_pool_t & pool) override;

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return _T("throw");
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // rethrow_xilx_t

    // Rethrow xilx.
    class rethrow_xilx_t : public xilx_base_t
    {
        typedef xilx_base_t __super_t;

    public:

        // Consturctor.
        rethrow_xilx_t(__region_t & region) : __super_t(region) { }

        // Writes xilx to a pool.
        virtual void write(__xw_context_t & ctx, xil_pool_t & pool) override;

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return _T("rethrow");
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // expression_xilx_t

    // Expression xilx.
    class expression_xilx_t : public xilx_base_t
    {
        typedef xilx_base_t __super_t;

    public:

        // Constructor.
        expression_xilx_t(__region_t & region, expression_t * expression)
            : __super_t(region), __expression(expression)
        {
            _A(expression != nullptr);
        }

        // Writes xils to a pool.
        virtual void write(__xw_context_t & ctx, xil_pool_t & pool) override;

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return _F(_T("expression: %1%"), __expression);
        }

    private:
        expression_t * __expression;
    };

    ////////// ////////// ////////// ////////// //////////
    // pop_variable_xilx_t

    // Pop to variable.
    class pop_variable_xilx_t : public xilx_base_t
    {
        typedef xilx_base_t __super_t;

    public:

        // Constructor.
        pop_variable_xilx_t(__region_t & region, variable_t * variable)
            : __super_t(region), variable(variable)
        {
            _A(variable != nullptr);
        }

        // Writes xils to a pool.
        virtual void write(__xw_context_t & ctx, xil_pool_t & pool) override;

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return _F(_T("pop: %1%"), variable);
        }

        variable_t * variable;
    };

    ////////// ////////// ////////// ////////// //////////
    // jmp_xilx_t

    // Jmp xilx.
    class jmp_xilx_t : public xilx_base_t
    {
        typedef xilx_base_t __super_t;

    public:
        using __super_t::__super_t;

        // Converts to a string.
        virtual const string_t to_string() const override { return _T("jmp"); }
    };

    //-------- ---------- ---------- ---------- ----------

    // Point jmp xilx.
    class point_jmp_xilx_t : public jmp_xilx_t
    {
        typedef jmp_xilx_t __super_t;

    public:

        // Constructor.
        point_jmp_xilx_t(__region_t & region, __exit_point_type_t point_type,
                                xil_jmp_condition_t jmp_condition = xil_jmp_condition_t::none)
            : __super_t(region), point_type(point_type), jmp_condition(jmp_condition) { }

        // Writes xils to a pool.
        virtual void write(__xw_context_t & ctx, xil_pool_t & pool) override;

        __exit_point_type_t point_type;
        xil_jmp_condition_t jmp_condition;

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return _F(_T("point jmp: %1% %2%"), jmp_condition, point_type);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Local label jmp xilx.
    class local_label_jmp_xilx_t : public jmp_xilx_t
    {
        typedef jmp_xilx_t __super_t;

    public:

        // Constructor.
        local_label_jmp_xilx_t(__region_t & region, local_label_t label,
                        xil_jmp_condition_t jmp_condition = xil_jmp_condition_t::none)
            : __super_t(region), label(label), jmp_condition(jmp_condition) { }

        // Writes xils to a pool.
        virtual void write(__xw_context_t & ctx, xil_pool_t & pool) override;

        local_label_t label;
        xil_jmp_condition_t jmp_condition;

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return _F(_T("local jmp: %1% %2%"), jmp_condition, label);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Global label jmp xilx.
    class global_label_jmp_xilx_t : public jmp_xilx_t
    {
        typedef jmp_xilx_t __super_t;

    public:

        // Constructor.
        global_label_jmp_xilx_t(__region_t & region, name_t label,
                        xil_jmp_condition_t jmp_condition = xil_jmp_condition_t::none)
            : __super_t(region), label(label), jmp_condition(jmp_condition) { }

        // Writes xils to a pool.
        virtual void write(__xw_context_t & ctx, xil_pool_t & pool) override;

        name_t label;
        xil_jmp_condition_t jmp_condition;

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return _F(_T("global jmp: %1% %2%"), jmp_condition, label);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Switch jmp xilx.
    class switch_jmp_xilx_t : public jmp_xilx_t
    {
        typedef jmp_xilx_t __super_t;

    public:

        // Constructor.
        switch_jmp_xilx_t(__region_t & region, switch_table_t * table)
            : __super_t(region), table(table)
        {
            _A(table != nullptr);
        }

        // Writes xils to a pool.
        virtual void write(__xw_context_t & ctx, xil_pool_t & pool) override;

        switch_table_t * table;

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return _F(_T("switch jmp: %1%"), table->index);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Block copy xilx.
    class block_copy_xilx_t : public xilx_base_t
    {
        typedef xilx_base_t __super_t;

    public:

        // Constructor.
        block_copy_xilx_t(__region_t & region, res_t res)
            : __super_t(region), res(res)
        { }

        // Writes xils to a pool.
        virtual void write(__xw_context_t & ctx, xil_pool_t & pool) override;

        res_t  res;

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return _F(_T("block copy"));
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Stack copy xilx.
    class stack_copy_xilx_t : public xilx_base_t
    {
        typedef xilx_base_t __super_t;

    public:

        // Constructor.
        stack_copy_xilx_t(__region_t & region, xil_type_t dtype)
            : __super_t(region), dtype(dtype)
        { }

        // Writes xils to a pool.
        virtual void write(__xw_context_t & ctx, xil_pool_t & pool) override;

        xil_type_t dtype;

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return _F(_T("stack copy: %1% %2%"), dtype);
        }
    };

    ////////// ////////// ////////// ////////// //////////

    namespace xil
    {
        // Writes assign xil for local variable.
        void write_assign_xil(__sctx_t & ctx, xil_pool_t & pool,
                                            local_variable_t * local, bool pick = false);

        // Writes assign xil for local variable.
        X_INLINE void write_assign_xil(__cctx_t & ctx, xil_pool_t & pool,
                                            local_variable_t * local, bool pick)
        {
            write_assign_xil(ctx.statement_ctx, pool, local, pick);
        }

        // Writes assign xil for param variable.
        void write_assign_xil(__sctx_t & ctx, xil_pool_t & pool,
                                            param_variable_t * param_var, bool pick = false);

        // Writes assign xil for param variable.
        X_INLINE void write_assign_xil(__cctx_t & ctx, xil_pool_t & pool,
                                            param_variable_t * param_var, bool pick = false)
        {
            write_assign_xil(ctx.statement_ctx, pool, param_var, pick);
        }

        // Writes assign xil for field variable.
        void write_assign_xil(__sctx_t & ctx, xil_pool_t & pool,
                                            field_variable_t * field_var, bool pick = false);

        // Writes assign xilx for field variable.
        X_INLINE void write_assign_xil(__cctx_t & ctx, xil_pool_t & pool,
                                            field_variable_t * field_var, bool pick = false)
        {
            write_assign_xil(ctx.statement_ctx, pool, field_var, pick);
        }

        // Writes label xil.
        smp_xil_t * append_label_xil(xil_pool_t & pool);

        // Writes jmp xil.
        jmp_xil_t * append_jmp_xil(xil_pool_t & pool, xil_jmp_condition_t condition);

        // Writes label xil.
        smp_xil_t * append_label_xil(statement_compile_context_t & ctx,
                                    xil_pool_t & pool, jmp_xil_t * jmp_xil);

        // Writes return xil.
        smp_xil_t * append_ret(xil_pool_t & pool);

        // Writes throw xil.
        smp_xil_t * append_throw(xil_pool_t & pool);

        // Writes rethrow xil.
        smp_xil_t * append_rethrow(xil_pool_t & pool);
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::compile

#endif // __COMPILE_XILX_H__
