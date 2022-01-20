
#include "services.h"
#include "utils.h"
#include "cs_analyze_tree.h"
#include "ast_builder.h"
#include "expressions.h"

namespace X_ROOT_NS { namespace modules { namespace lang_cs {

    using namespace core;
    using namespace compile;

    ////////// ////////// ////////// ////////// //////////

    // Returns token property.
    X_INLINE const __cs_token_property_t * __get_token_property(token_value_t value)
    {
        return get_token_property((cs_token_value_t)value);
    }

    // Returns a string description of a token.
    X_INLINE const string_t __get_token_string(token_value_t value)
    {
        return get_token_string((cs_token_value_t)value);
    }

    // Returns node value.
    X_INLINE analyze_node_value_t __get_node_value(
                                const string_t & name, analyze_node_type_t node_type)
    {
        return get_node_value(name, node_type);
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns token reader.
    token_reader_t * cs_lang_service_t::create_token_reader(token_reader_context_t & context)
    {
        return context.new_obj<cs_token_reader_t>(context, __lang);
    }

    // Token property service
    const token_property_t * cs_lang_service_t::get_token_property(token_value_t value)
    {
        return __get_token_property(value);
    }

    // Returns string description of a token.
    const string_t cs_lang_service_t::get_token_string(token_value_t value)
    {
        return __get_token_string(value);
    }

    // Returns string description of branch.
    const string_t cs_lang_service_t::get_branch_string(analyze_node_value_t value)
    {
        return _title((cs_branch_type_t)value);
    }

    // Operator property service
    const operator_property_t * cs_lang_service_t::get_operator_property(token_value_t value)
    {
        return __get_token_property(value);
    }

    // Expression build service
    const expression_box_property_t * cs_lang_service_t::get_expression_box_property(
                                                                    token_value_t value)
    {
        return __get_token_property(value);
    }

    // Builds expression.
    expression_t * cs_lang_service_t::build_expression(lang_expression_build_context_t & ctx,
                                                   const lang_expression_build_args_t & args)
    {
        switch ((cs_token_value_t)args.token->value)
        {
            case cs_token_value_t::k_this:
                return ctx.create_system_expression<this_expression_t>();

            case cs_token_value_t::k_base:
                return ctx.create_system_expression<base_expression_t>();

            case cs_token_value_t::question_mark:  {

                _A(args.expressions[1]->this_family() == expression_family_t::binary);
                binary_expression_t * colon_exp = (binary_expression_t *)(args.expressions[1]);

                return ctx.create_expression<cs_condition_expression_t>(
                    args.expressions[0], colon_exp->exp1(), colon_exp->exp2()
                );
            }

            case cs_token_value_t::comma: {
                cs_expressions_t * expressions = as<cs_expressions_t *>(args.expressions[0]);
                if (expressions != nullptr)
                {
                    expressions->append(args.expressions + 1, args.expressions + args.count);
                }
                else
                {
                    expressions = ctx.create_expression<cs_expressions_t>(
                        args.expressions, args.expressions + args.count
                    );
                }

                return expressions;
            }

            default:
                return ctx.create_default_expression(args);
        }
    }

    // Returns analyze pattern.
    const string_t cs_lang_service_t::get_analyze_pattern(global_context_t & context)
    {
        return get_analyze_tree_pattern();
    }

    // Gets node value.
    analyze_node_value_t cs_lang_service_t::get_node_value(global_context_t & context,
                                        const string_t & name, analyze_node_type_t node_type)
    {
        return __get_node_value(name, node_type);
    }

    #define __Builder(name)   AstBuilderName(name)(context, args).build()
    #define __BuilderCase_(name, builder_name)                                          \
                            case cs_branch_type_t::name: return __Builder(builder_name);
    #define __BuilderCase(name) __BuilderCase_(name, name)

    // Ast build service
    ast_node_t * cs_lang_service_t::build_ast(ast_context_t & context,
                                                        lang_ast_build_args_t & args)
    {
        #define __Case      __BuilderCase
        #define __Case_     __BuilderCase_

        switch ((cs_branch_type_t)args.node_value)
        {
            __Case(_fields)
            __Case(_fields_item)
            __Case(_enum_fields)
            __Case(_enum_fields_item)

            __Case(method)
            __Case(cvalue)
            __Case(type_name)
            __Case(name_unit)
            __Case(general_type_name)
            __Case(array_type_name)
            __Case(uncertain_type_name)
            __Case(type_def_param)
            __Case(type_def_params)
            __Case(type_def)
            __Case(type_of)
            __Case(expression)
            __Case(_single_expression)
            __Case(_function_name_expression)
            __Case(statement)
            __Case(method_body)
            __Case(_fake_method)
            __Case(_operator)
            __Case(property)
            __Case(event)
            __Case(type)

            __Case(function)
            __Case(index)
            __Case_(new_, new)
            __Case(new_array)
            __Case(array_initializer)
            __Case(array_lengths)
            __Case(default_value)
            __Case(type_name_exp)
            __Case(type_cast_exp)

            __Case(import)
            __Case_(using_, using)
            __Case_(namespace_, namespace)

            __Case(decorate)
            __Case(_decorate_complex)

            __Case(mname)
            __Case(generic_param)
            __Case(generic_params)
            __Case(generic_constraint)
            __Case(generic_constraints)
            __Case(generic_arg)
            __Case(generic_args)

            __Case(expressions)
            __Case(param)
            __Case(params)
            __Case(argument)
            __Case(arguments)

            __Case(_attribute_group)
            __Case(_attribute_group_item)
            __Case(_attribute_group_assign)
            __Case(_attribute_type_name)
            __Case(attributes)

            __Case(expression_st)
            __Case(type_def_st)
            __Case(defination_st)
            __Case(_defination_st_item)

            __Case(break_st)
            __Case(continue_st)
            __Case(goto_st)
            __Case(throw_st)
            __Case(return_st)

            __Case(statements)
            __Case(do_while_st)
            __Case(while_st)
            __Case(for_st)
            __Case(for_each_st)
            __Case(if_st)
            __Case(switch_st)
            __Case_(case_, case)
            __Case(try_st)
            __Case_(catch_, catch)
            __Case(empty_st)
            __Case(statement_group_st)

            default:
                return nullptr;
        }

        #undef __Case_
        #undef __Case
    }

    // Detect missing element when compile format error.
    detect_missing_element_result cs_lang_service_t::detect_missing_element(
                ast_context_t & ast_context, analyzer_element_reader_t & reader,
                const analyze_node_keys_t & possible_keys, const code_unit_t * cu)
    {
        std::set<analyze_node_key_t> keys;
        al::copy(possible_keys, al::inserter(keys));

        // Nodes.
        {
            typedef cs_branch_type_t t;
            static t prefer_branches[] = { t::_expression, t::_single_expression };

            for (t branch : prefer_branches)
            {
                analyze_node_key_t key(analyze_node_type_t::branch_ref, (analyze_node_value_t)branch);
                if (al::contains(keys, key))
                    return __build_ast_node(ast_context, branch);
            }
        }

        // Tokens.
        {
            typedef cs_token_value_t t;
            static t prefer_tokens[] = { t::semicolon, t::comma };

            for (t token : prefer_tokens)
            {
                analyze_node_key_t key(analyze_node_type_t::token, (analyze_node_value_t)token);

                if (al::contains(keys, key))
                {
                    token_value_t token_value = (token_value_t)key.value;
                    token_t * token = ast_context.compile_context.new_obj<token_t>(
                        token_value, cu
                    );

                    return detect_missing_element_result(token, get_token_string(token_value));
                }
            }
        }

        return detect_missing_element_result::empty;
    }

    // A fake expression ast_node.
    class __fake_expression_ast_node_t
        : public tsingle_ast_node_t<(element_value_t)cs_branch_type_t::_expression>
        , public expression_ast_t
    {
        typedef tsingle_ast_node_t<(element_value_t)cs_branch_type_t::_expression> __super_t;

    public:
        __fake_expression_ast_node_t() : __value(0), __expression(&__value) { }

        virtual expression_t * to_eobject() override
        {
            return &__expression;
        }

    private:
        cvalue_t __value;
        cvalue_expression_t __expression;
    };

    // Creates a new ast node of specified branch value.
    detect_missing_element_result cs_lang_service_t::__build_ast_node(
                                    ast_context_t & context, cs_branch_type_t branch)
    { 
        array_ast_build_elements_t elements;
        lang_ast_build_args_t args((analyze_node_value_t)branch, elements);

        switch ((cs_branch_type_t)branch)
        {
            case cs_branch_type_t::_expression: {
                static __fake_expression_ast_node_t node;
                return detect_missing_element_result(&node, _T("expression"));
            }   break;

            default: {
                ast_node_t * node = this->build_ast(context, args);
                _A(node != nullptr);
                return detect_missing_element_result(
                    node, get_branch_string((analyze_node_value_t)branch)
                );
            }   break;
        }
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::lang_cs

