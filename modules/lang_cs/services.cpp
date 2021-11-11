
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

    // Ast build service
    ast_node_t * cs_lang_service_t::build_ast(ast_context_t & context,
                                                        lang_ast_build_args_t & args)
    {
        #define __Build(name)   AstBuilderName(name)(context, args).build()
        #define __Case_(name, builder_name)  case type_t::name: return __Build(builder_name);
        #define __Case(name)    __Case_(name, name)

        typedef cs_branch_type_t type_t;

        switch ((cs_branch_type_t)args.node_value)
        {
            __Case(_fields)
            __Case(_fields_item)
            __Case(_enum_fields)
            __Case(_enum_fields_item)

            __Case(method)
            __Case(cvalue)
            __Case(type_name)
            __Case(type_name_unit)
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
            __Case(function_name)
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
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::lang_cs

