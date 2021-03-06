#ifndef __COMPILE_UTILS_H__
#define __COMPILE_UTILS_H__

#include <compile.h>
#include <algorithm.h>

namespace X_ROOT_NS { namespace modules { namespace compile {

    namespace
    {
        using namespace core;
        typedef expression_compile_context_t __cctx_t;
        typedef statement_compile_context_t  __sctx_t;
    }

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
        return get_assembly(member) != ctx.get_assembly();
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

    X_INLINE ref_t __search_field_ref(__cctx_t & ctx, field_t * field)
    {
        return __search_field_ref(ctx.statement_ctx, field);
    }

    ////////// ////////// ////////// ////////// //////////

} } }

#endif // __COMPILE_UTILS_H__
