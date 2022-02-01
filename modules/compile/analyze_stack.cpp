
#include <compile.h>
#include "utils.h"

namespace X_ROOT_NS { namespace modules { namespace compile {

    using namespace core;

    typedef core::expression_t      __expression_t;
    typedef analyze_stack_error_t   e_t;

    const operator_priority_t __lowest_priority = max_value<operator_priority_t>();

    #define _Ef(error_code, args...)                                    \
        _E(e_t::error_code, sprintf(_title(e_t::error_code), ##args))

    ////////// ////////// ////////// ////////// //////////

    // Analyze stack error codes.
    X_ENUM_INFO(analyze_stack_error_t)

        // Format error.
        X_C(format_error,                _T("format error, at %1%"))

        // Unknown element type.
        X_C(unknown_element_type,        _T("unknown element type"))

        // Unknown token property.
        X_C(unknown_token_property,      _T("unknown token property: token = %1%"))

        // Unknown operator property.
        X_C(unknown_operator_property,   _T("unknown operator property: token = %1%"))

        // Unknown expression property.
        X_C(unknown_expression_property, _T("unknown expression property: token = %1%"))

        // Adhere overflow.
        X_C(adhere_overflow,             _T("adhere overflow: token = %1%"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Analyze stack element types.
    X_ENUM_INFO(analyze_stack_element_type_t)

        // Token.
        X_C(token)

        // Name.
        X_C(expression)

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Returns whether two elements are equals.
    bool analyze_stack_element_t::operator == (const analyze_stack_element_t & other) const
    {
        if (type != other.type)
            return false;

        switch (type)
        {
            case analyze_stack_element_type_t::token:
                return token == other.token;

            case analyze_stack_element_type_t::name:
                return name == other.name;

            case analyze_stack_element_type_t::expression:
                return expression == other.expression;

            default:
                return true;
        }
    }

    // Returns whether two elements are not equals.
    bool analyze_stack_element_t::operator != (const analyze_stack_element_t & other) const
    {
        return !operator == (other);
    }

    // Converts to a string.
    analyze_stack_element_t::operator string_t() const
    {
        switch (type)
        {
            case analyze_stack_element_type_t::token:
                return (string_t)*token;

            case analyze_stack_element_type_t::name:
                return (string_t)name;

            case analyze_stack_element_type_t::expression:
                return _str(expression);

            default:
                return _T("");
        }
    }

    // The end element.
    const analyze_stack_element_t analyze_stack_element_t::end(
            analyze_stack_element_type_t::__unknown__
    );

    ////////// ////////// ////////// ////////// //////////

    // Builds expression with context and arguments.
    __expression_t * analyze_stack_context_t::build_expression(
            lang_expression_build_context_t & ctx, const lang_expression_build_args_t & args)
    {
        return __service_helper.build_expression(ctx, args);
    }

    // Returns operator property.
    const operator_property_t * analyze_stack_context_t::__get_operator_property(
                                                            token_value_t value)
    {
        const operator_property_t * property = __service_helper.get_operator_property(value);
        if (property != nullptr && is_system_operator(property->op))
            property = get_system_operator_property(property->op);

        int arity = property->arity, adhere = property->adhere;
        _A(arity >= 1 && arity <= 2);
        _A(adhere >= 0 && adhere <= arity);

        return property;
    }

    ////////// ////////// ////////// ////////// //////////

    // Creates default expression.
    __expression_t * __lang_expression_build_context_t::
                        create_default_expression(const lang_expression_build_args_t & args)
    {
        token_t * op_token = args.token;
        const operator_property_t * property = args.property;

        if (property == nullptr)
            throw _Ef(unknown_token_property, _str(op_token));

        __expression_t ** expressions = args.expressions;

        switch (property->arity)
        {
            case 1:
                return __context->to_exp(
                    op_token, property, expressions[0]
                );

            case 2:
                return __context->to_exp(
                    op_token, property, expressions[0], expressions[1]
                );
                    
            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Analyze.
    __expression_t * analyze_stack_t::analyze()
    {
        __stack_element_t element;
        while ((element = __elements->read_next()) != __stack_element_t::end)
        {
            switch (element.type)
            {
                case analyze_stack_element_type_t::token:
                    if (element.token->value == __end_token_value)
                        goto __end;
                    __push_token(element.token);
                    break;

                case analyze_stack_element_type_t::name:
                    __push_name(element.token, element.name);
                    break;

                case analyze_stack_element_type_t::expression:
                    __push_expression(element.expression);
                    break;

                case analyze_stack_element_type_t::operator_:
                    __push_operator(element.operator_);
                    break;

                default:
                    throw _E(analyze_stack_error_t::unknown_element_type,
                        sprintf(_T("unknown element type: %1%"), element.type)
                    );
            }
        }

    __end:
        if (__end_token_value != __no_end_token_value && element == __stack_element_t::end)
        {
            token_value_t value = __context->
                    get_expression_box_property(__end_token_value)->pair_to;

            throw _E(e_t::format_error, sprintf(_T("\"%1%\" missing"), value));
        }

        return __combine_all();
    }

    // Pushes a token.
    void analyze_stack_t::__push_token(token_t * token)
    {
        _A(token != nullptr);

        token_value_t token_value = token->value;

        if (__context->is_operator(token_value))
        {
            __push_operator(token);
        }
        else if (__context->is_expression_box(token_value))
        {
            __push_expression_box(token);
        }
        else if (!__context->is_invisible(token_value))
        {
            __lang_expression_build_context_t ctx(__context);
            lang_expression_build_args_t args(token, nullptr, nullptr, 0);
            __expression_t * expression = __context->build_expression(ctx, args);

            if (expression == nullptr)
                throw _E(e_t::unknown_token, _F(_T("unknown token \"%1%\""), _str(token)));

            __push_expression(expression);
        }
    }

    // Pushes an operator.
    void analyze_stack_t::__push_operator(token_t * token)
    {
        //_PF(_T("push operator: %1%"), (string_t)*token);
        const operator_property_t * property = __context->get_operator_property(token->value);
        __push_operator(property, token);
    }

    // Pushes an operator.
    void analyze_stack_t::__push_operator(operator_t op)
    {
        //_PF(_T("push operator: %1%"), op);
        const operator_property_t * property = __context->get_operator_property((token_value_t)op);
        __push_operator(property, nullptr);
    }

    // Pushes an operator.
    void analyze_stack_t::__push_operator(const operator_property_t * property, token_t * token)
    {
        if (property->adhere > property->arity)
            throw _Ef(adhere_overflow, (string_t)*token);

        int left_placeholder = property->adhere;
        if (left_placeholder > __exp_stack.size())
            throw _Ef(format_error, (string_t)*token);

        int right_placeholder = property->arity - left_placeholder;

        if (__right_placeholder > 0 && left_placeholder > 0)
            throw _Ef(format_error, (string_t)*token);

        __combine_expressions(property->priority);

        __op_stack.push(__op_t(property, token));
        __right_placeholder = right_placeholder;
    }

    // Combines expressions.
    void analyze_stack_t::__combine_expressions(operator_priority_t until_priority)
    {
        while (!__op_stack.empty() && __low_privilege(until_priority,
                __context->get_priority(__op_stack.top().token->value)))
        {
            __op_t top_op = __op_stack.pop_back();
            token_t * op_token = top_op.token;
            const operator_property_t * property = __context->get_operator_property(
                                                                op_token->value);
            __push_expression(
                __combine_expressions(property, op_token)
            );
        }
    }

    // Returns top operator property.
    const operator_property_t * analyze_stack_t::__top_op_property()
    {
        if (__op_stack.empty())
            return nullptr;

        __op_t op = __op_stack.top();
        if (op.op_property != nullptr)
            return op.op_property;

        return __context->get_operator_property(op.token->value);
    }

    // Combines expressions.
    expression_t * analyze_stack_t::__combine_expressions(const operator_property_t * property,
                                                                token_t * token)
    {
        operator_priority_t priority = property->priority;
        int arity = property->arity, adhere = property->adhere;

        expression_t * exps[2];
        expression_t ** p_exp = (expression_t **)exps + arity - 1;

        if (arity > adhere)
            *p_exp-- = __pop_expression();

        if (p_exp < exps)
            return __combine_expression(property, token, (expression_t **)exps);

        const operator_property_t * top_property = __top_op_property();

        if (top_property != nullptr)
        {
            if (property->parent_op == top_property->op)
            {
                *p_exp = __pop_expression();
                __push_expression(
                    __combine_expression(property, token, (expression_t **)exps)
                );

                return __combine_expressions(top_property, __op_stack.pop_back().token);
            }

            if (property->parent_op != operator_t::__default__)
            {
                for (; ; __push_expression(*p_exp))
                {
                    token_t * token = __op_stack.pop_back().token;
                    *p_exp = __combine_expressions(top_property, token);

                    top_property = __top_op_property();
                    if (top_property == nullptr)
                        throw _Ef(format_error, (string_t)*token);

                    if (top_property->op == property->parent_op)
                        break;
                }

                return __combine_expression(property, token, (expression_t **)exps);
            }

            if (!__high_privilege(priority, top_property->priority) && 
                top_property->direct != operator_direct_t::right_to_left)
            {
                *p_exp = __combine_expressions(top_property, __op_stack.pop_back().token);
                return __combine_expression(property, token, (expression_t **)exps);
            }
        }

        *p_exp = __pop_expression();
        return __combine_expression(property, token, (expression_t **)exps);
    }

    // Combines expressions.
    __expression_t * analyze_stack_t::__combine_expression(
            const operator_property_t * property, token_t * token, expression_t ** expressions)
    {
        __lang_expression_build_context_t ctx(__context);
        lang_expression_build_args_t args(token, property, expressions, property->arity);
        __expression_t * expression = __context->build_expression(ctx, args);

        if (expression == nullptr)
            expression = ctx.create_default_expression(args);

        if (!__op_stack.empty())
            __right_placeholder = __get_right_placeholder(__op_stack.top().token);

        return expression;
    }

    // Returns right placeholder.
    int analyze_stack_t::__get_right_placeholder(token_t * token)
    {
        const operator_property_t * property = __context->get_operator_property(token->value);
        int right_placeholder = property->arity - property->adhere;

        if (right_placeholder < 0)
            throw _Ef(adhere_overflow, (string_t)*token);

        return right_placeholder;
    }

    // Pushes a name.
    void analyze_stack_t::__push_name(token_t * token, const name_t & name)
    {
        expression_t * exp = __context->to_exp(nullptr, name);
        __push_expression(exp);

        if (token != nullptr)
            exp->code_unit = (const code_unit_t *)*token;
    }

    // Pushes an expression box.
    void analyze_stack_t::__push_expression_box(token_t * token)
    {
        const expression_box_property_t * property
                                    = __context->get_expression_box_property(token->value);

        if (property->side != expression_box_side_t::left)
            throw _Ef(format_error, (string_t)*token);

        analyze_stack_t stack(__context, __elements, property->pair_to, __deep + 1);
        __expression_t * expression = stack.analyze();
        __push_expression(expression);
    }

    // Pushes an expression.
    void analyze_stack_t::__push_expression(__expression_t * expression)
    {
        //_PF(_T("push expression: %1%"), expression);
        _A(expression != nullptr);

        if (!__op_stack.empty() && __right_placeholder <= 0)
            throw _Ef(format_error, expression->to_string());

        __right_placeholder--;
        __exp_stack.push(__exp_t(expression));
    }

    // Pop expressions.
    expression_t * analyze_stack_t::__pop_expression()
    {
        if (__exp_stack.empty())
            throw _E(e_t::format_error, _T("format error"));

        return __exp_stack.pop_back().exp;
    }

    // Combine all expressions.
    expression_t * analyze_stack_t::__combine_all()
    {
        __combine_expressions(__lowest_priority);
        return __pop_expression();
    }

    ////////// ////////// ////////// ////////// //////////


} } }  // X_ROOT_NS::modules::compile
