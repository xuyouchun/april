
#ifndef __COMPILE_XILX_H__
#define __COMPILE_XILX_H__

namespace X_ROOT_NS { namespace modules { namespace compile {

    namespace
    {
        using namespace core;

        typedef statement_compile_context_t __context_t;
        typedef statement_region_t          __region_t;
        typedef statement_exit_point_type_t __exit_point_type_t;

        typedef statement_compile_context_t  __sctx_t;
        typedef expression_compile_context_t __cctx_t;
    }

    ////////// ////////// ////////// ////////// //////////
    // xilx_base_t

    class xilx_base_t : public xilx_t
    {
    public:
        xilx_base_t(__region_t & region) : __region(region) { }

    protected:
        __region_t & __region;
    };

    ////////// ////////// ////////// ////////// //////////
    // label_xilx_t

    class label_xilx_t : public xilx_base_t
    {
        typedef xilx_base_t __super_t;

    public:
        label_xilx_t(__region_t & region) : __super_t(region) { }
        virtual const string_t to_string() const override { return _T("label"); }
    };

    //-------- ---------- ---------- ---------- ----------
    // local_label_xilx_t

    class local_label_xilx_t : public label_xilx_t
                             , public statement_point_t
    {
        typedef label_xilx_t __super_t;

    public:
        local_label_xilx_t(__region_t & region, local_label_t flag)
            : __super_t(region), flag(flag) { }

        virtual void write(__context_t & ctx, xil_pool_t & pool) override;
        virtual const string_t to_string() const override
        {
            return _F(_T("local label: %1%"), flag);
        }

        virtual local_label_t to_position() override { return flag; }

        local_label_t flag;
    };

    //-------- ---------- ---------- ---------- ----------
    // global_label_xilx_t

    class global_label_xilx_t : public label_xilx_t
    {
        typedef label_xilx_t __super_t;

    public:
        global_label_xilx_t(__region_t & region, name_t name)
            : __super_t(region), name(name) { }

        virtual void write(__context_t & ctx, xil_pool_t & pool) override;
        virtual const string_t to_string() const override
        {
            return _F(_T("global label: %1%"), name);
        }

        const name_t name;
    };

    ////////// ////////// ////////// ////////// //////////
    // local_assign_xilx_t

    class local_assign_xilx_t : public xilx_base_t
    {
        typedef xilx_base_t __super_t;

    public:
        local_assign_xilx_t(__region_t & region, local_variable_t * local)
            : __super_t(region), local(local)
        {
            _A(local != nullptr);
        }

        virtual void write(__context_t & ctx, xil_pool_t & pool) override;
        virtual const string_t to_string() const override
        {
            return _F(_T("local assign: %1%"), local);
        }

        local_variable_t * local;
    };

    ////////// ////////// ////////// ////////// //////////
    // return_xilx_t

    class return_xilx_t : public xilx_base_t
    {
        typedef xilx_base_t __super_t;

    public:
        return_xilx_t(__region_t & region) : __super_t(region) { }

        virtual void write(__context_t & ctx, xil_pool_t & pool) override;
        virtual const string_t to_string() const override
        {
            return _T("ret");
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // expression_xilx_t

    class expression_xilx_t : public xilx_base_t
    {
        typedef xilx_base_t __super_t;

    public:
        expression_xilx_t(__region_t & region, expression_t * expression)
            : __super_t(region), __expression(expression) { }

        virtual void write(__context_t & ctx, xil_pool_t & pool) override;
        virtual const string_t to_string() const override
        {
            return _F(_T("expression: %1%"), __expression);
        }

    private:
        expression_t * __expression;

        void __compile(__context_t & ctx, xil_pool_t & pool, expression_t * expression);
    };

    ////////// ////////// ////////// ////////// //////////
    // jmp_xilx_t

    class jmp_xilx_t : public xilx_base_t
    {
        typedef xilx_base_t __super_t;

    public:
        using __super_t::__super_t;

        virtual const string_t to_string() const override { return _T("jmp"); }
    };

    //-------- ---------- ---------- ---------- ----------

    class point_jmp_xilx_t : public jmp_xilx_t
    {
        typedef jmp_xilx_t __super_t;

    public:
        point_jmp_xilx_t(__region_t & region, __exit_point_type_t point_type,
                                xil_jmp_condition_t jmp_condition = xil_jmp_condition_t::none)
            : __super_t(region), point_type(point_type), jmp_condition(jmp_condition) { }

        virtual void write(__context_t & ctx, xil_pool_t & pool) override;

        __exit_point_type_t point_type;
        xil_jmp_condition_t jmp_condition;

        virtual const string_t to_string() const override
        {
            return _F(_T("point jmp: %1% %2%"), jmp_condition, point_type);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    class local_label_jmp_xilx_t : public jmp_xilx_t
    {
        typedef jmp_xilx_t __super_t;

    public:
        local_label_jmp_xilx_t(__region_t & region, local_label_t label,
                        xil_jmp_condition_t jmp_condition = xil_jmp_condition_t::none)
            : __super_t(region), label(label), jmp_condition(jmp_condition) { }

        virtual void write(__context_t & ctx, xil_pool_t & pool) override;

        local_label_t label;
        xil_jmp_condition_t jmp_condition;

        virtual const string_t to_string() const override
        {
            return _F(_T("local jmp: %1% %2%"), jmp_condition, label);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    class global_label_jmp_xilx_t : public jmp_xilx_t
    {
        typedef jmp_xilx_t __super_t;

    public:
        global_label_jmp_xilx_t(__region_t & region, name_t label,
                        xil_jmp_condition_t jmp_condition = xil_jmp_condition_t::none)
            : __super_t(region), label(label), jmp_condition(jmp_condition) { }

        virtual void write(__context_t & ctx, xil_pool_t & pool) override;

        name_t label;
        xil_jmp_condition_t jmp_condition;

        virtual const string_t to_string() const override
        {
            return _F(_T("global jmp: %1% %2%"), jmp_condition, label);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    class switch_jmp_xilx_t : public jmp_xilx_t
    {
        typedef jmp_xilx_t __super_t;

    public:
        switch_jmp_xilx_t(__region_t & region, switch_table_t * table)
            : __super_t(region), table(table)
        {
            _A(table != nullptr);
        }

        virtual void write(__context_t & ctx, xil_pool_t & pool) override;

        switch_table_t * table;

        virtual const string_t to_string() const override
        {
            return _F(_T("switch jmp: %1%"), table->index);
        }
    };

    ////////// ////////// ////////// ////////// //////////

    namespace xil
    {
        void write_assign_xil(__sctx_t & ctx, xil_pool_t & pool,
                                            local_variable_t * local, bool pick = false);

        X_INLINE void write_assign_xil(__cctx_t & ctx, xil_pool_t & pool,
                                            local_variable_t * local, bool pick)
        {
            write_assign_xil(ctx.statement_ctx, pool, local, pick);
        }

        void write_assign_xil(__sctx_t & ctx, xil_pool_t & pool,
                                            param_variable_t * param_var, bool pick = false);

        X_INLINE void write_assign_xil(__cctx_t & ctx, xil_pool_t & pool,
                                            param_variable_t * param_var, bool pick = false)
        {
            write_assign_xil(ctx.statement_ctx, pool, param_var, pick);
        }

        void write_assign_xil(__sctx_t & ctx, xil_pool_t & pool,
                                            field_variable_t * field_var, bool pick = false);

        X_INLINE void write_assign_xil(__cctx_t & ctx, xil_pool_t & pool,
                                            field_variable_t * field_var, bool pick = false)
        {
            write_assign_xil(ctx.statement_ctx, pool, field_var, pick);
        }

        smp_xil_t * append_label_xil(xil_pool_t & pool);
        jmp_xil_t * append_jmp_xil(xil_pool_t & pool, xil_jmp_condition_t condition);

        smp_xil_t * append_label_xil(statement_compile_context_t & ctx,
                                    xil_pool_t & pool, jmp_xil_t * jmp_xil);
        smp_xil_t * append_ret(xil_pool_t & pool);
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::compile

#endif // __COMPILE_XILX_H__
