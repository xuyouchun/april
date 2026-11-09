
#include "lq_service.h"

namespace X_ROOT_NS::modules::lang_lq {

    using namespace core;
    using namespace compile;

    typedef lang_cs::cs_token_value_t __cs_token_t;

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        // The contextual keywords of lq: they are names in the code of cs, and keywords of a query.
        // The token is a keyword or a name: the analyzer takes the one that fits.
        struct __keyword_t
        {
            const char_t *  text;
            size_t          length;
            token_value_t   values[3];
        };

        #define __Keyword(kw)                                                           \
            { _T(#kw), sizeof(#kw) - 1,                                                 \
                { (token_value_t)__cs_token_t::k_##kw,                                  \
                  (token_value_t)__cs_token_t::name, unknown_token_value } }

        static const __keyword_t __keywords[] = {
            __Keyword(select),  __Keyword(orderby),  __Keyword(group),     __Keyword(by),
            __Keyword(let),     __Keyword(join),     __Keyword(on),        __Keyword(equals),
            __Keyword(into),    __Keyword(ascending), __Keyword(descending),
        };

        #undef __Keyword

        // Token enumerator of lq: the names that are keywords of lq are marked.
        class __lq_token_enumerator_t : public token_enumerator_t, public object_t
        {
        public:
            __lq_token_enumerator_t(token_reader_context_t & context, token_enumerator_t * base)
                : __context(context), __base(base)
            { }

            // Reads the next token.
            virtual token_t * next() override
            {
                token_t * token = __base->next();

                if (token != nullptr && token->value == (token_value_t)__cs_token_t::name)
                    __mark(token);

                return token;
            }

            X_TO_STRING_IMPL(_T("lq_token_enumerator_t"))

        private:
            token_reader_context_t & __context;
            token_enumerator_t *     __base;

            // Marks the name that is a keyword.
            void __mark(token_t * token)
            {
                const code_unit_t & cu = token->code_unit;

                for (const __keyword_t & keyword : __keywords)
                {
                    if ((size_t)cu.length != keyword.length
                            || !std::equal(keyword.text, keyword.text + keyword.length, cu.s))
                        continue;

                    token_data_t * data = token->data != nullptr?
                        __context.new_obj<token_data_t>(*token->data)
                      : __context.new_obj<token_data_t>();

                    data->possible_values = keyword.values;

                    token->data  = data;
                    token->value = unknown_token_value;
                    return;
                }
            }
        };

        // Token reader of lq.
        class __lq_token_reader_t : public token_reader_t, public object_t
        {
        public:
            __lq_token_reader_t(token_reader_context_t & context, token_reader_t * base)
                : __context(context), __base(base)
            { }

            virtual token_enumerator_t * read(const char_t * code, size_t length,
                                              const code_file_t * file) override
            {
                return __context.new_obj<__lq_token_enumerator_t>(
                    __context, __base->read(code, length, file)
                );
            }

            X_TO_STRING_IMPL(_T("lq_token_reader_t"))

        private:
            token_reader_context_t & __context;
            token_reader_t *         __base;
        };
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns token reader.
    token_reader_t * lq_lang_service_t::create_token_reader(token_reader_context_t & context)
    {
        return context.new_obj<__lq_token_reader_t>(
            context, __super_t::create_token_reader(context)
        );
    }

    // Returns string description of branch.
    const string_t lq_lang_service_t::get_branch_string(analyze_node_value_t value)
    {
        if (is_lq_branch(value))
            return _title((lq_branch_type_t)value);

        return __super_t::get_branch_string(value);
    }

    // Returns analyze pattern.
    const string_t lq_lang_service_t::get_analyze_pattern(global_context_t & context)
    {
        return get_lq_analyze_pattern();
    }

    // Gets node value.
    analyze_node_value_t lq_lang_service_t::get_node_value(global_context_t & context,
                                        const string_t & name, analyze_node_type_t node_type)
    {
        if (node_type == analyze_node_type_t::branch || node_type == analyze_node_type_t::branch_ref)
        {
            analyze_node_value_t value = get_lq_node_value(name);
            if ((long)value != -1)
                return value;
        }

        return __super_t::get_node_value(context, name, node_type);
    }

    // Detect missing element: the keyword select is what a query that goes on lacks.
    detect_missing_element_result lq_lang_service_t::detect_missing_element(
                ast_context_t & ast_context, analyzer_element_reader_t & reader,
                const analyze_node_keys_t & possible_keys, const code_unit_t * cu)
    {
        for (const analyze_node_key_t & key : possible_keys)
        {
            if (key.type == analyze_node_type_t::branch_ref
                    && key.value == (analyze_node_value_t)lq_branch_type_t::lq_select)
            {
                token_t * token = ast_context.compile_context.new_obj<token_t>(
                    (token_value_t)__cs_token_t::k_select, cu
                );

                return detect_missing_element_result(token, _T("select"));
            }
        }

        return __super_t::detect_missing_element(ast_context, reader, possible_keys, cu);
    }

    // Builds ast node.
    ast_node_t * lq_lang_service_t::build_ast(ast_context_t & context, lang_ast_build_args_t & args)
    {
        if (is_lq_branch(args.node_value))
            return build_lq_ast(context, args);

        return __super_t::build_ast(context, args);
    }

    ////////// ////////// ////////// ////////// //////////

}   // X_ROOT_NS::modules::lang_lq
