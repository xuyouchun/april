#include <compile.h>
#include "utils.h"

namespace X_ROOT_NS { namespace modules { namespace compile {

    using namespace core;
    using namespace xil;

    typedef statement_compile_context_t __sctx_t;
    typedef xilx_write_context_t        __xw_context_t;

    ////////// ////////// ////////// ////////// //////////

    // Returns xil type of the expression.
    X_INLINE xil_type_t __xil_type(expression_t * exp)
    {
        vtype_t vtype = exp->get_vtype();
        return to_xil_type(vtype);
    }

    ////////// ////////// ////////// ////////// //////////

    // Pop local xil.
    struct __pop_local_xil_t : xil_extra_t<pop_xil_t>
    {
        typedef xil_extra_t<pop_xil_t> __super_t;

        __pop_local_xil_t(xil_type_t xtype, msize_t identity)
            : __super_t(xil_storage_type_t::local, xtype)
        {
            this->set_identity(identity);
        }
    };

    // Pick local xil.
    struct __pick_local_xil_t : xil_extra_t<pick_xil_t>
    {
        typedef xil_extra_t<pick_xil_t> __super_t;

        __pick_local_xil_t(xil_type_t xtype, msize_t identity)
            : __super_t(xil_storage_type_t::local, xtype)
        {
            this->set_identity(identity);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Pop argument xil.
    struct __pop_argument_xil_t : xil_extra_t<pop_xil_t>
    {
        typedef xil_extra_t<pop_xil_t> __super_t;

        __pop_argument_xil_t(xil_type_t xtype, msize_t identity)
            : __super_t(xil_storage_type_t::argument, xtype)
        {
            this->set_identity(identity);
        }
    };

    // Pick argument xil.
    struct __pick_argument_xil_t : xil_extra_t<pick_xil_t>
    {
        typedef xil_extra_t<pick_xil_t> __super_t;

        __pick_argument_xil_t(xil_type_t xtype, msize_t identity)
            : __super_t(xil_storage_type_t::argument, xtype)
        {
            this->set_identity(identity);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Pop field xil.
    struct __pop_field_xil_t : xil_extra_t<pop_xil_t>
    {
        typedef xil_extra_t<pop_xil_t> __super_t;

        __pop_field_xil_t(xil_type_t xtype, ref_t field_ref)
            : __super_t(xil_storage_type_t::field, xtype)
        {
            this->set_field_ref(field_ref);
        }
    };

    // Pick field xil.
    struct __pick_field_xil_t : xil_extra_t<pick_xil_t>
    {
        typedef xil_extra_t<pick_xil_t> __super_t;

        __pick_field_xil_t(xil_type_t xtype, ref_t field_ref)
            : __super_t(xil_storage_type_t::field, xtype)
        {
            this->set_field_ref(field_ref);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Jmp xil.
    struct __jmp_xil_t : xil_extra_t<jmp_xil_t>
    {
        typedef xil_extra_t<jmp_xil_t> __super_t;

        using __super_t::__super_t;
    };

    // Appends jmp xil.
    jmp_xil_t * xil::append_jmp_xil(xil_pool_t & pool, xil_jmp_model_t condition)
    {
        return pool.append<__jmp_xil_t>(condition);
    }

    // Switch xil.
    struct __switch_xil_t : xil_extra_t<jmp_xil_t>
    {
        typedef xil_extra_t<jmp_xil_t> __super_t;

        __switch_xil_t(int tbl_idx) : __super_t(xil_jmp_model_t::switch_)
        {
            this->set_tbl(tbl_idx);
        }
    };

    // Appends label xil.
    smp_xil_t * xil::append_label_xil(xil_pool_t & pool)
    {
        return pool.append<smp_xil_t>(xil_smp_t::label);
    }

    // Appends label xil.
    smp_xil_t * xil::append_label_xil(statement_compile_context_t & ctx,
                                    xil_pool_t & pool, jmp_xil_t * jmp_xil)
    {
        local_label_t label = ctx.next_local_label();
        smp_xil_t * label_xil = xil::append_label_xil(pool);

        jmp_manager_t & jm = ctx.jmp_manager;
        jm.append_jmp(jmp_xil, label);
        jm.set_point(label, label_xil);

        return label_xil;
    }

    // Appends ret xil.
    smp_xil_t * xil::append_ret(xil_pool_t & pool)
    {
        return pool.append<smp_xil_t>(xil_smp_t::ret);
    }

    // Appends throw xil.
    smp_xil_t * xil::append_throw(xil_pool_t & pool)
    {
        return pool.append<smp_xil_t>(xil_smp_t::throw_);
    }

    // Appends rethrow xil.
    smp_xil_t * xil::append_rethrow(xil_pool_t & pool)
    {
        return pool.append<smp_xil_t>(xil_smp_t::rethrow);
    }

    // Writes end_finally xil.
    smp_xil_t * xil::append_end_block(xil_pool_t & pool)
    {
        return pool.append<smp_xil_t>(xil_smp_t::end_finally);
    }

    // Compiles xilx.
    static void __compile_expression(__xw_context_t & ctx, xil_pool_t & pool,
                                     expression_t * expression)
    {
        expression_compile_context_t exp_ctx(ctx);
        expression->compile(exp_ctx, pool);
    }

    ////////// ////////// ////////// ////////// //////////
    // local_assign_xilx_t

    // Writes assign xil for local variable.
    void xil::write_assign_xil(__sctx_t & ctx, xil_pool_t & pool, local_variable_t * local,
									  xil_type_t dtype, bool pick)
    {
        type_t * type = to_type(local->type_name);
        _A(type != nullptr);

        #define __Append(name, xil_type)                                    \
            pool.append<__##name##_local_xil_t>(                            \
                xil_type, local->identity									\
            )

        #define __Write(xil_type)											\
			do {															\
				if (pick)                                                   \
	                __Append(pick, xil_type);                               \
		        else                                                        \
			        __Append(pop, xil_type);								\
			} while (false)
            

        switch(type->this_gtype())
        {
            case gtype_t::general: {

				if (dtype == xil_type_t::empty)
					dtype = to_xil_type(((general_type_t *)type)->vtype);

				__Write(dtype);

			}	break;

            case gtype_t::array:
                __Write(xil_type_t::object);
                break;

            case gtype_t::generic_param:
                __Write(xil_type_t::empty);
                break;

            case gtype_t::generic:
                __Write(xil_type_t::object);        // TODO: struct?
                break;

            default:
                X_UNEXPECTED();
        }

        #undef __Case
        #undef __Write
        #undef __Append
    }

    // Writes assign xil for param variable.
    void xil::write_assign_xil(__sctx_t & sctx, xil_pool_t & pool, param_variable_t * param_var,
							xil_type_t dtype, bool pick)
    { 
        _A(param_var != nullptr);
        _A(param_var->param != nullptr);

        type_t * type = to_type(param_var->param->type_name);
        _A(type != nullptr);

        msize_t index = param_var->param->index;
        if(call_type_of_method(sctx.method) != xil_call_type_t::static_)
            index++;

        #define __Append(name, xil_type)                                    \
            pool.append<__##name##_argument_xil_t>(                         \
                xil_type_t::xil_type, index                                 \
            )

        #define __Write(xil_type)                                           \
            if(pick)                                                        \
                __Append(pick, xil_type);                                   \
            else                                                            \
                __Append(pop, xil_type);

        #define __Case(type, xil_type)                                      \
            case vtype_t::type:                                             \
                __Write(xil_type)                                           \
                break;

        switch(type->this_gtype())
        {
            case gtype_t::general: 
                switch(((general_type_t *)type)->vtype)
                {
                    __Case(int8_, int8)
                    __Case(uint8_, uint8)

                    __Case(int16_, int16)
                    __Case(uint16_, uint16)

                    __Case(int32_, int32)
                    __Case(uint32_, uint32)

                    __Case(int64_, int64)
                    __Case(uint64_, uint64)

                    __Case(float_, float_)
                    __Case(double_, double_)

                    __Case(bool_, bool_)
                    __Case(char_, char_)
                    
                    __Case(string_, string)
                    __Case(mobject_, object)

                    __Case(ptr_, ptr)

                    default:
                        X_UNEXPECTED();

                }   break;

            case gtype_t::array:
                __Write(object)
                break;

            case gtype_t::generic_param:
                __Write(empty);
                break;

            case gtype_t::generic:
                __Write(object);     // TODO: struct
                break;

            default:
                X_UNEXPECTED();
        }

        #undef __Case
        #undef __Write
        #undef __Append
    }

    // Writes assign xil for field variable.
    void xil::write_assign_xil(__sctx_t & ctx, xil_pool_t & pool, field_variable_t * field_var,
								xil_type_t dtype, bool pick)
    {
        _A(field_var != nullptr);
        _A(field_var->field != nullptr);

        field_t * field = field_var->field;
        type_t * type = to_type(field->type_name);
        _A(type != nullptr);

        #define __Append(name, xil_type)                                    \
            pool.append<__##name##_field_xil_t>(                            \
                xil_type_t::xil_type, __search_field_ref(ctx, field)        \
            )

        #define __Write(xil_type)                                           \
                if(pick)                                                    \
                    __Append(pick, xil_type);                               \
                else                                                        \
                    __Append(pop, xil_type);

        #define __Case(type, xil_type)                                      \
            case vtype_t::type:                                             \
                __Write(xil_type)                                           \
                break;

        switch(type->this_gtype())
        {
            case gtype_t::general:
                switch(((general_type_t *)type)->vtype)
                {
                    __Case(int8_, int8)
                    __Case(uint8_, uint8)

                    __Case(int16_, int16)
                    __Case(uint16_, uint16)

                    __Case(int32_, int32)
                    __Case(uint32_, uint32)

                    __Case(int64_, int64)
                    __Case(uint64_, uint64)

                    __Case(float_, float_)
                    __Case(double_, double_)

                    __Case(bool_, bool_)
                    __Case(char_, char_)
                    
                    __Case(string_, string)
                    __Case(mobject_, object)

                    __Case(ptr_, ptr)

                    default:
                        X_UNEXPECTED();

                }   break;

            case gtype_t::array:
                __Write(object)
                break;

            case gtype_t::generic_param:
                __Write(empty);
                break;

            case gtype_t::generic:
                __Write(object);     // TODO: struct
                break;

            default:
                X_UNEXPECTED();
        }

        #undef __Case
        #undef __Write
        #undef __Append

    }

    // Writes assign xils for variable.
    void __write_assign_xil(__sctx_t & ctx, xil_pool_t & pool, variable_t * var,
							xil_type_t dtype, bool pick = false)
    {
        _A(var != nullptr);

        switch(var->this_type())
        {
            case variable_type_t::local:
                write_assign_xil(ctx, pool, (local_variable_t *)var, dtype, pick);
                break;

            case variable_type_t::param:
                write_assign_xil(ctx, pool, (param_variable_t *)var, dtype, pick);
                break;

            case variable_type_t::field:
                write_assign_xil(ctx, pool, (field_variable_t *)var, dtype, pick);
                break;

            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Local assign xilx.
    void local_assign_xilx_t::write(__xw_context_t & ctx, xil_pool_t & pool)
    {
        if (expression == nullptr)
        {
            write_assign_xil(ctx, pool, local, xil_type_t::empty);
            return;
        }

        if (local->write_count == 1)
        {
            if (is_constant_expression(ctx, expression))
            {
				if (is_optimize(ctx, compile_optimize_code_t::auto_determine_constant_variables))
				{
					local->constant = true;
					local->expression = expression;

					return;
				}
            }

            /*
            if(local->read_count == 1)
            {
                local->inline_ = true;
                local->expression = expression;
                return;
            }
            */
        }

        __compile_expression(ctx, pool, expression);
        write_assign_xil(ctx, pool, local, __xil_type(expression));
    };

    ////////// ////////// ////////// ////////// //////////

    // Writes ret xil.
    void return_xilx_t::write(__xw_context_t & ctx, xil_pool_t & pool)
    {
        xil::append_ret(pool);
    }

    ////////// ////////// ////////// ////////// //////////

    // Writes throw xil.
    void throw_xilx_t::write(__xw_context_t & ctx, xil_pool_t & pool)
    {
        xil::append_throw(pool);
    }

    ////////// ////////// ////////// ////////// //////////

    // Writes rethrow xil.
    void rethrow_xilx_t::write(__xw_context_t & ctx, xil_pool_t & pool)
    {
        xil::append_rethrow(pool);
    }

    ////////// ////////// ////////// ////////// //////////

    // Leave ret xilx.
    void leave_ret_xilx_t::write(__xw_context_t & ctx, xil_pool_t & pool)
    {
        xil::append_jmp_xil(pool, xil_jmp_model_t::leave);
    }

    ////////// ////////// ////////// ////////// //////////

    // Writes end_block xil.
    void end_block_xilx_t::write(__xw_context_t & ctx, xil_pool_t & pool)
    {
        xil::append_end_block(pool);
    }

    ////////// ////////// ////////// ////////// //////////
    // expression_xilx_t

    // Writes xils to pool.
    void expression_xilx_t::write(__xw_context_t & ctx, xil_pool_t & pool)
    {
        __compile_expression(ctx, pool, __expression);
    }

    ////////// ////////// ////////// ////////// //////////
    // pop_variable_xilx_t

    // Writes xils to pool.
    void pop_variable_xilx_t::write(__xw_context_t & ctx, xil_pool_t & pool)
    {
        __write_assign_xil(ctx.sc_context, pool, variable, xil_type_t::empty);
    }

    ////////// ////////// ////////// ////////// //////////
    // label_xilx_t

    // Writes xils to pool.
    void global_label_xilx_t::write(__xw_context_t & ctx, xil_pool_t & pool)
    {

    }

    //-------- ---------- ---------- ---------- ----------

    // Writes local label xils to pool.
    void local_label_xilx_t::write(__xw_context_t & ctx, xil_pool_t & pool)
    {
        xil_t * xil = append_label_xil(pool);
        ((__sctx_t &)ctx).jmp_manager.set_point(flag, xil);
    }

    ////////// ////////// ////////// ////////// //////////
    // jmp_xilx_t

    void point_jmp_xilx_t::write(__xw_context_t & ctx, xil_pool_t & pool)
    {
        statement_point_t * point = __region.get_point(point_type);
        _A(point != nullptr);

        __jmp_xil_t * xil = pool.append<__jmp_xil_t>(jmp_model);
        ((__sctx_t &)ctx).jmp_manager.append_jmp(xil, point->to_position());
    }

    //-------- ---------- ---------- ---------- ----------

    // Writes xils to pool.
    void local_label_jmp_xilx_t::write(__xw_context_t & ctx, xil_pool_t & pool)
    {
        __jmp_xil_t * xil = pool.append<__jmp_xil_t>(jmp_model);
        ((__sctx_t &)ctx).jmp_manager.append_jmp(xil, label);
    }

    //-------- ---------- ---------- ---------- ----------

    // Writes xils to pool.
    void global_label_jmp_xilx_t::write(__xw_context_t & ctx, xil_pool_t & pool)
    {

    }

    //-------- ---------- ---------- ---------- ----------

    // Writes xils to pool.
    void switch_jmp_xilx_t::write(__xw_context_t & ctx, xil_pool_t & pool)
    {
        __switch_xil_t * xil = pool.append<__switch_xil_t>(table->index);
        table->jmp_xil = xil;
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::compile

