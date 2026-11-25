
#include "services.h"
#include "utils.h"
#include "cs_analyze_tree.h"
#include "ast_builder.h"
#include "expressions.h"

namespace X_ROOT_NS::modules::lang_cs {

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
        // The branches of the parts of an expression are told as an expression.
        switch ((cs_branch_type_t)value)
        {
            case cs_branch_type_t::_expression:
            case cs_branch_type_t::_single_expression:
                return _T("expression");

            case cs_branch_type_t::_function_name_expression:
                return _T("function name");

            default:
                return _title((cs_branch_type_t)value);
        }
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

            // x is null, x is not null, x is not T: x == null, x != null, !(x is T).
            case cs_token_value_t::k_is: {
                expression_t * object = args.expressions[0], * pattern = args.expressions[1];

                bool negate = false;
                if (pattern->this_family() == expression_family_t::unitary
                        && ((unitary_expression_t *)pattern)->op() == operator_t::logic_not)
                {
                    negate = true;
                    pattern = ((unitary_expression_t *)pattern)->exp();
                }

                expression_t * exp;
                if (pattern->this_family() == expression_family_t::cvalue)
                {
                    exp = ctx.create_system_expression<binary_expression_t>(
                        negate? operator_t::not_equal : operator_t::equal, object, pattern
                    );

                    exp->code_unit = combine_all_code_units(ctx.memory, args.token, object, pattern);
                    return exp;
                }

                exp = ctx.create_system_expression<binary_expression_t>(
                    operator_t::is, object, pattern
                );
                exp->code_unit = combine_all_code_units(ctx.memory, args.token, object, pattern);

                if (negate)
                {
                    exp = ctx.create_system_expression<unitary_expression_t>(
                        operator_t::logic_not, exp
                    );
                    exp->code_unit = combine_all_code_units(ctx.memory, args.token, object, pattern);
                }

                return exp;
            }

            case cs_token_value_t::question_mark:  {

                _A(args.expressions[1]->this_family() == expression_family_t::binary);
                binary_expression_t * colon_exp = (binary_expression_t *)(args.expressions[1]);

                return ctx.create_expression<cs_condition_expression_t>(
                    args.expressions[0], colon_exp->exp1(), colon_exp->exp2()
                );
            }

            case cs_token_value_t::comma: {
                // (a, b, c) is a tuple. A tuple that is in parentheses is an item of the
                // next one: ((a, b), c).
                tuple_expression_t * tuple = as<tuple_expression_t *>(args.expressions[0]);

                // An item that has a name is the binary expression of the name and the value, that the node of the
                // expression makes: Number: 1.
                auto add_item = [](tuple_expression_t * tuple, expression_t * item) {
                    name_t name = name_t::null;

                    if (item->this_family() == expression_family_t::binary)
                    {
                        binary_expression_t * binary = (binary_expression_t *)item;
                        if (binary->named_item)
                        {
                            name = ((name_expression_t *)binary->exp1())->name;
                            item = binary->exp2();
                        }
                    }

                    tuple->append(item);
                    tuple->item_names.push_back(name);
                };

                if (tuple != nullptr && !tuple->closed)
                {
                    for (size_t index = 1; index < (size_t)args.count; index++)
                        add_item(tuple, args.expressions[index]);
                }
                else
                {
                    tuple = ctx.create_system_expression<tuple_expression_t>();
                    for (size_t index = 0; index < (size_t)args.count; index++)
                        add_item(tuple, args.expressions[index]);
                }

                tuple->code_unit = combine_all_code_units(
                    ctx.memory, args.token, args.expressions[0], args.expressions[args.count - 1]
                );

                return tuple;
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
            __Case_(with_exp, new)
            __Case_(with_assign, new_assign)
            __Case(new_array)
            __Case(array_initializer)
            __Case(array_lengths)
            __Case(default_value)
            __Case(checked_exp)
            __Case(await_exp)
            __Case(throw_exp)
            __Case(index_range)
            __Case(lambda)
            __Case(type_name_exp)
            __Case(declaration_exp)
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
            __Case_(tuple_args, generic_args)
            __Case(deconstruct_st)
            __Case(deconstruct_var_st)
            __Case(deconstruct_assign_st)
            __Case_(label_st, goto_st)

            __Case(expressions)
            __Case(param)
            __Case(params)
            __Case(argument)
            __Case(arguments)

            __Case(_attribute_group)
            __Case(_attribute_group_item)
            __Case(_attribute_group_assign)
            __Case(_assembly_group)
            __Case(_assembly_group_item)
            __Case(_assembly_group_assign)
            __Case(new_assign)
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
            __Case(yield_st)

            __Case(statements)
            __Case(do_while_st)
            __Case(while_st)
            __Case(for_st)
            __Case(for_each_st)
            __Case(using_st)
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

    // Returns the position after a line comment: // ...
    static size_t __skip_line_comment(const char_t * code, size_t length, size_t i)
    {
        while (i < length && code[i] != _T('\r') && code[i] != _T('\n'))
            i++;

        return i;
    }

    // Returns the position after a block comment: /* ... */
    static size_t __skip_block_comment(const char_t * code, size_t length, size_t i)
    {
        i += 2;
        while (i + 1 < length && !(code[i] == _T('*') && code[i + 1] == _T('/')))
            i++;

        return i + 2 <= length? i + 2 : length;
    }

    // Returns the position after a verbatim string: @"..." , "" is a quote.
    static size_t __skip_verbatim_string(const char_t * code, size_t length, size_t i)
    {
        i += 2;
        while (i < length)
        {
            if (code[i] == _T('"'))
            {
                if (i + 1 < length && code[i + 1] == _T('"'))
                {
                    i += 2;
                    continue;
                }

                return i + 1;
            }

            i++;
        }

        return length;
    }

    // Returns the position after a string or a char: "..." and 'x', a backslash escapes the next char,
    // a char ends at the end of the line.
    static size_t __skip_quoted(const char_t * code, size_t length, size_t i)
    {
        char_t quote = code[i];

        size_t j = i + 1;
        while (j < length && code[j] != quote
                && !(quote == _T('\'') && (code[j] == _T('\r') || code[j] == _T('\n'))))
        {
            if (code[j] == _T('\\') && j + 1 < length)
                j++;

            j++;
        }

        return j < length && code[j] == quote? j + 1 : j;
    }

    // Returns the position after the comment, the string or the char that starts at the position
    // (the same ones that the token reader reads), the position itself if there is no one.
    size_t cs_lang_service_t::skip_non_code(const char_t * code, size_t length, size_t i)
    {
        char_t next = i + 1 < length? code[i + 1] : _T('\0');

        switch (code[i])
        {
            case _T('/'):
                switch (next)
                {
                    case _T('/'):
                        return __skip_line_comment(code, length, i);

                    case _T('*'):
                        return __skip_block_comment(code, length, i);

                    default:
                        return i;
                }

            case _T('@'):
                return next == _T('"')? __skip_verbatim_string(code, length, i) : i;

            case _T('"'):
            case _T('\''):
                return __skip_quoted(code, length, i);

            default:
                return i;
        }
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::lang_cs

