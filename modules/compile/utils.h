#ifndef __COMPILE_UTILS_H__
#define __COMPILE_UTILS_H__

#include <compile.h>
#include <algorithm.h>

namespace X_ROOT_NS::modules::compile {

    namespace
    {
        using namespace core;
        typedef expression_compile_context_t __cctx_t;
        typedef statement_compile_context_t  __sctx_t;
    }

    #define __CompileFailed(_format, _s ...)                                            \
        _E(__e_t::compile_failed, _F(_format, ##_s))

    #define __Failed(_format, _s ...)                                                   \
        throw __CompileFailed(_T("") _format, ##_s)

    #define __FailedWhen(_condition, _format, _s ...)                                   \
        if (_condition) __Failed(_format, ##_s)

    #define __FailedWhenNull(_exp, _format, _s ...)                                     \
        __FailedWhen( ((_exp) == nullptr), _format, ##_s)

    ////////// ////////// ////////// ////////// //////////

    class code_section_builder_t
    {
    public:
        code_section_builder_t(global_context_t & global_context, code_t * code,
            lang_id_t default_lang, std::vector<code_section_t *> & sections, pool_t & pool);

        void build();

    private:
        global_context_t &      __global_context;
        code_t *                __code;
        const char_t * const    __source_code;
        const char_t *          __p;
        const lang_id_t         __default_lang;
        std::vector<code_section_t *> & __sections;
        pool_t &                __pool;
        std::stack<lang_id_t>   __lang_stack;

        template<typename callback_t> void __build(callback_t callback);
        bool __skip_char(char_t c);
        string_t __read_lang();
    };


    ////////// ////////// ////////// ////////// //////////

    enum class __char_flag_t
    {
        analyze_tree_token_end = (1 << 0),
    };

    struct char_property_t : public al::flag_property_t<char_property_t, char, __char_flag_t>
    {
        static void init(enumerator_t & e);
    };

    typedef typename char_property_t::properties_t<127> __char_properties_t;
    extern __char_properties_t __char_properties;

    X_INLINE bool __char_has_flag(char_t c, __char_flag_t flag, bool default_)
    {
        return __char_properties.overlimit(c)? default_
                : __char_properties[(char)c].has_flag(flag);
    }

    X_INLINE bool is_analyze_tree_token_end(char_t c)
    {
        return __char_has_flag(c, __char_flag_t::analyze_tree_token_end, true);
    }

    ////////// ////////// ////////// ////////// //////////

    void set_assign_parent(expression_t * exp);

    ////////// ////////// ////////// ////////// //////////

    template<mt_table_index_t tidx>
    ref_t __ref_of(__sctx_t & ctx, const typename mt_t<tidx>::entity_t & entity)
    {
        return ctx.assembly_layout().ref_of<tidx>(entity);
    }

    template<mt_table_index_t tidx>
    ref_t __ref_of(__cctx_t & ctx, const typename mt_t<tidx>::entity_t & entity)
    {
        return __ref_of<tidx>(ctx.statement_ctx, entity);
    }

    X_INLINE ref_t __ref_of(__sctx_t & ctx, type_t * type)
    {
        return ctx.mctx.layout.ref_of(type);
    }

    X_INLINE ref_t __ref_of(__cctx_t & ctx, type_t * type)
    {
        return __ref_of(ctx.statement_ctx, type);
    }

    X_INLINE bool __is_extern(__sctx_t & ctx, member_t * member)
    {
        _A(member != nullptr);
        return member->get_assembly() != ctx.get_assembly();
    }

    X_INLINE bool __is_extern(__cctx_t & ctx, member_t * member)
    {
        return __is_extern(ctx.statement_ctx, member);
    }

    ref_t __search_method_ref(__sctx_t & ctx, method_base_t * method);

    X_INLINE ref_t __search_method_ref(__cctx_t & ctx, method_base_t * method)
    {
        return __search_method_ref(ctx.statement_ctx, method);
    }

    ref_t __search_field_ref(__sctx_t & ctx, field_t * field);

    xil_call_type_t __get_constructor_calltype(type_t * host_type, method_t * constructor);

    X_INLINE ref_t __search_field_ref(__cctx_t & ctx, field_t * field)
    {
        return __search_field_ref(ctx.statement_ctx, field);
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns whether it's ref or out argument.
    X_INLINE bool __is_addr(param_type_t param_type)
    {
        return is_addr_param(param_type);
    }

    X_INLINE bool __is_addr(param_variable_t * var)
    {
        _A(var != nullptr);
        return __is_addr(var->param->ptype);
    }

    X_INLINE bool __is_addr(variable_t * var)
    {
        _A(var != nullptr);
        return var->this_type() == variable_type_t::param && __is_addr((param_variable_t *)var);
    }

    ////////// ////////// ////////// ////////// //////////

    // Pre append custom struct for assign.
    // If expression is new_expression_t, only put address of this variable.
    // Otherwise, append object_copy xil.
    // See also: __compile_new_struct_object() in expression.cpp.
    void __pre_custom_struct_assign(expression_compile_context_t & ctx, xil_pool_t & pool,
        variable_t * variable, expression_t * expression, expression_t * this_ = nullptr);

    // Pre append custom struct for return.
    void __do_custom_struct_return(expression_compile_context_t & ctx, xil_pool_t & pool,
        expression_t * expression);

    // Push address of variable.
    void __push_variable_address(expression_compile_context_t & ctx, xil_pool_t & pool,
        variable_t * variable);

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::compile

#endif // __COMPILE_UTILS_H__
