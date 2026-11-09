#include "c_service.h"

namespace X_ROOT_NS::modules::lang_c {

    using namespace core;
    using namespace compile;

    typedef lang_cs::cs_token_value_t __cs_token_t;

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        // Token enumerator of c: the tokens of cs, and the token -> .
        class __c_token_enumerator_t : public token_enumerator_t, public object_t
        {
        public:
            __c_token_enumerator_t(token_reader_context_t & context, token_enumerator_t * base)
                : __context(context), __base(base)
            { }

            // Reads the next token: the characters - and > that are together are the token ->.
            virtual token_t * next() override
            {
                token_t * token = __pending != nullptr? __pending : __base->next();
                __pending = nullptr;

                if (token != nullptr && __is_char(token, _T('-')))
                {
                    token_t * following = __base->next();

                    if (following != nullptr && __is_char(following, _T('>'))
                            && following->code_unit.s == token->code_unit.s + token->code_unit.length)
                    {
                        const code_unit_t & cu = token->code_unit;
                        token = __context.new_obj<token_t>(
                            c_ptr_member_token_value, cu.s, (int32_t)(cu.length + 1), cu.file
                        );
                    }
                    else
                    {
                        __pending = following;
                    }
                }

                return token;
            }

            X_TO_STRING_IMPL(_T("c_token_enumerator_t"))

        private:
            token_reader_context_t & __context;
            token_enumerator_t *     __base;
            token_t *                __pending = nullptr;

            // Returns whether the token is the character.
            static bool __is_char(token_t * token, char_t c)
            {
                return token->code_unit.length == 1 && token->code_unit.s[0] == c;
            }
        };

        // Token reader of c.
        class __c_token_reader_t : public token_reader_t, public object_t
        {
        public:
            __c_token_reader_t(token_reader_context_t & context, token_reader_t * base)
                : __context(context), __base(base)
            { }

            virtual token_enumerator_t * read(const char_t * code, size_t length,
                                              const code_file_t * file) override
            {
                return __context.new_obj<__c_token_enumerator_t>(
                    __context, __base->read(code, length, file)
                );
            }

            X_TO_STRING_IMPL(_T("c_token_reader_t"))

        private:
            token_reader_context_t & __context;
            token_reader_t *         __base;
        };
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns token reader.
    token_reader_t * c_lang_service_t::create_token_reader(token_reader_context_t & context)
    {
        return context.new_obj<__c_token_reader_t>(
            context, __super_t::create_token_reader(context)
        );
    }

    // The property of the token ->: it is not an operator, and it is not seen.
    static const token_property_t __ptr_member_property = { false, false, false };

    // Returns token property.
    const token_property_t * c_lang_service_t::get_token_property(token_value_t value)
    {
        if (value == c_ptr_member_token_value)
            return &__ptr_member_property;

        return __super_t::get_token_property(value);
    }

    // Returns string description of a token.
    const string_t c_lang_service_t::get_token_string(token_value_t value)
    {
        if (value == c_ptr_member_token_value)
            return _T("->");

        return __super_t::get_token_string(value);
    }

    // Returns string description of branch.
    const string_t c_lang_service_t::get_branch_string(analyze_node_value_t value)
    {
        if (is_c_branch(value))
            return _title((c_branch_type_t)value);

        return __super_t::get_branch_string(value);
    }

    // Returns analyze pattern.
    const string_t c_lang_service_t::get_analyze_pattern(global_context_t & context)
    {
        return get_c_analyze_pattern();
    }

    // Gets node value.
    analyze_node_value_t c_lang_service_t::get_node_value(global_context_t & context,
                                        const string_t & name, analyze_node_type_t node_type)
    {
        if (node_type == analyze_node_type_t::branch || node_type == analyze_node_type_t::branch_ref)
        {
            analyze_node_value_t value = get_c_node_value(name);
            if ((long)value != -1)
                return value;
        }

        // The token -> .
        if (node_type == analyze_node_type_t::token && name == _T("->"))
            return (analyze_node_value_t)c_ptr_member_token_value;

        return __super_t::get_node_value(context, name, node_type);
    }

    // Builds ast node.
    ast_node_t * c_lang_service_t::build_ast(ast_context_t & context, lang_ast_build_args_t & args)
    {
        if (is_c_branch(args.node_value))
            return build_c_ast(context, args);

        // A simple expression has the token -> .
        if (args.node_value == (analyze_node_value_t)lang_cs::cs_branch_type_t::_single_expression)
            return build_c_single_expression(context, args);

        return __super_t::build_ast(context, args);
    }

    ////////// ////////// ////////// ////////// //////////

}   // X_ROOT_NS::modules::lang_c
