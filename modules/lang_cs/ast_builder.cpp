
#include "utils.h"
#include "cs_analyze_tree.h"
#include "ast_builder.h"

namespace X_ROOT_NS::modules::lang_cs {

    using namespace core;
    using namespace compile;

    ////////// ////////// ////////// ////////// //////////

    #define __B(_name)      AstBuilderName(_name)
    #define __C(_name)      AstName(_name)
    #define __This          (*this)
    #define __Type_(_node)  std::remove_reference_t<decltype(*_node)>
    #define __Type          __Type_(__This)

    #define __ChildCount(_node, _child_name)                                            \
        (_node->child_count(__Type_(_node)::_child_name))

    #define __ApplyToken(_name, _token, _args)                                          \
        void __B(_name)::apply_token(token_t * _token, __apply_token_args_t & _args)

    #define __ApplyAst(_name, _node, _args)                                             \
        void __B(_name)::apply_ast(ast_node_t * _node, __apply_ast_args_t & _args)

    #define __OnCompleted(_name, _args)                                                 \
                    void __B(_name)::on_completed(ast_builder_completed_args_t & _args)

    #define __AstValue(_name)       ((element_value_t)cs_branch_type_t::_name)

    #define __TokenValue(_name)     ((element_value_t)cs_token_value_t::_name)

    #define __NewFakeAst(_type_t, _value)                                               \
        this->__new_fake_ast<_type_t, (ast_value_t)__AstValue(_value)>()

    #define __EachChild(_ast_t, _index, _func)                                          \
        ((_ast_t *)node)->each_child_node<ast_node_t>(ast_t::_index, _func)


    ////////// ////////// ////////// ////////// //////////
    // expression

    __ApplyToken(expression, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->append_name(this->__to_name(token), token);
                break;

            default:
                __This->append_token(token);
                break;
        }
    }

    __ApplyAst(expression, node, args)
    {
        switch (node->value())
        {
            case __AstValue(cvalue):
                __This->append_cvalue(node);
                break;

            case __AstValue(name_unit):
                __This->append_name_unit((name_unit_ast_node_t *)node);
                break;

            default:
                if (is<expression_ast_t *>(node))
                    __This->append_expression(node);
                break;
        }
    }

    __OnCompleted(expression, args) { }

    ////////// ////////// ////////// ////////// //////////
    // mname

    __ApplyToken(mname, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __node->append_part(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(mname, node, args) { }
    __OnCompleted(mname, args) { }

    ////////// ////////// ////////// ////////// //////////
    // expressions

    __ApplyToken(expressions, token, args) { }

    __ApplyAst(expressions, node, args)
    {
        if (is<expression_ast_t *>(node))
        {
            __This->append_child(__Type::__expressions__, node);
        }
    }

    __OnCompleted(expressions, args) { }

    ////////// ////////// ////////// ////////// //////////
    // type_name

    __ApplyToken(type_name, token, args) { }

    __ApplyAst(type_name, node, args)
    {
        if (as<type_name_ast_t *>(node) != nullptr)
        {
            __This->set_child(__Type::__underlying__, node);
        }
    }

    __OnCompleted(type_name, args) { }

    ////////// ////////// ////////// ////////// //////////
    // name_unit

    __ApplyToken(name_unit, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(name_unit, node, args)
    {
        switch (node->value())
        {
            case __AstValue(generic_args):
                __This->set_child(__Type::__generic_args__, node);
                break;
        }
    }

    __OnCompleted(name_unit, args) { }

    ////////// ////////// ////////// ////////// //////////
    // general_type_name

    __ApplyToken(general_type_name, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(k_sbyte):
                __set_name(_T("Int8"), token);
                break;

            case __TokenValue(k_byte):
                __set_name(_T("UInt8"), token);
                break;

            case __TokenValue(k_short):
                __set_name(_T("Int16"), token);
                break;

            case __TokenValue(k_ushort):
                __set_name(_T("UInt16"), token);
                break;

            case __TokenValue(k_int):
                __set_name(_T("Int32"), token);
                break;

            case __TokenValue(k_uint):
                __set_name(_T("UInt32"), token);
                break;

            case __TokenValue(k_long):
                __set_name(_T("Int64"), token);
                break;

            case __TokenValue(k_ulong):
                __set_name(_T("UInt64"), token);
                break;

            case __TokenValue(k_char):
                __set_name(_T("Char"), token);
                break;

            case __TokenValue(k_string):
                __set_name(_T("String"), token);
                break;

            case __TokenValue(k_float):
                __set_name(_T("Single"), token);
                break;

            case __TokenValue(k_double):
                __set_name(_T("Double"), token);
                break;

            case __TokenValue(k_decimal):
                __set_name(_T("Decimal"), token);
                break;

            case __TokenValue(k_void):
                __set_name(_T("Void"), token);
                break;

            case __TokenValue(k_bool):
                __set_name(_T("Boolean"), token);
                break;

            case __TokenValue(k_object):
                __set_name(_T("Object"), token);
                break;

            case __TokenValue(k_global):
                __global_type = global_type_t::global;
                break;

            case __TokenValue(question_mark):
                __This->set_nullable();
                break;

            case __TokenValue(double_colon):
                if (__global_type == global_type_t::__default__)
                    __global_type = global_type_t::root;
                break;
        }
    }

    void __B(general_type_name)::__set_name(const string_t & name, token_t * token, bool is_root)
    {
        __This->append_child(__Type::__units__, this->__to_name_unit_ast(_T("System")));
        __This->append_child(__Type::__units__, this->__to_name_unit_ast(name));
        __This->set_global_type(global_type_t::global);
    }

    __ApplyAst(general_type_name, node, args)
    {
        switch (node->value())
        {
            case __AstValue(name_unit):
                __This->append_child(__Type::__units__, node);
                break;

            // (int Number, string Str) is System.Tuple<Int32, String>, the names are kept by the arguments.
            case __AstValue(generic_args): {
                auto * unit = this->template __new_ast<name_unit_ast_node_t>();
                unit->name = this->__to_name(_T("Tuple"));
                unit->set_child(name_unit_ast_node_t::__generic_args__, node);
                unit->on_commit();

                __This->append_child(__Type::__units__, this->__to_name_unit_ast(_T("System")));
                __This->append_child(__Type::__units__, unit);
                __This->set_global_type(global_type_t::global);
            }   break;
        }
    }

    __OnCompleted(general_type_name, args)
    {
        if (__global_type != global_type_t::__default__)
            __This->set_global_type(__global_type);
    }

    ////////// ////////// ////////// ////////// //////////
    // array_type_name

    __ApplyToken(array_type_name, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(comma):
            case __TokenValue(right_square):
                __This->append_child(__Type::__dimensions__, __last_dimension);
                __last_dimension = nullptr;
                break;

            // int[]? is an array that can be null, as the others.
            case __TokenValue(question_mark):
                __This->nullable = true;
                break;
        }
    }

    __ApplyAst(array_type_name, node, args)
    {
        switch (node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::__element_type_name__, node);
                break;

            case __AstValue(expression):
                __last_dimension = node;
                break;
        }
    }

    __OnCompleted(array_type_name, args) { }

    ////////// ////////// ////////// ////////// //////////
    // uncertain_type_name

    __ApplyToken(uncertain_type_name, token, args) { }

    __ApplyAst(uncertain_type_name, node, args) { }

    __OnCompleted(uncertain_type_name, args) { }

    ////////// ////////// ////////// ////////// //////////
    // type_def_param

    __ApplyToken(type_def_param, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
                break;

            case __TokenValue(three_dots):
                __This->set_extends(true);
                break;
        }
    }

    __ApplyAst(type_def_param, node, args) { }

    __OnCompleted(type_def_param, args) { }

    ////////// ////////// ////////// ////////// //////////
    // type_def_params

    __ApplyToken(type_def_params, token, args) { }

    __ApplyAst(type_def_params, node, args)
    {
        switch (node->value())
        {
            case __AstValue(type_def_param):
                __This->append_child(__Type::__params__, node);
                break;
        }
    }

    __OnCompleted(type_def_params, args) { }


    ////////// ////////// ////////// ////////// //////////
    // type_def

    __ApplyToken(type_def, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(type_def, node, args)
    {
        switch (node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::__type_name__, node);
                break;

            case __AstValue(attributes):
                __This->set_child(__Type::__attributes__, node);
                break;

            case __AstValue(decorate):
                __This->set_child(__Type::__decorate__, node);
                break;

            case __AstValue(type_def_params):
                __This->set_child(__Type::__params__, node);
                break;
        }
    }

    __OnCompleted(type_def, args) { }

    ////////// ////////// ////////// ////////// //////////
    // type_of

    __ApplyToken(type_of, token, args) { }

    __ApplyAst(type_of, node, args)
    {
        switch (node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::__type_name__, node);
                break;
        }
    }

    __OnCompleted(type_of, args) { }

    ////////// ////////// ////////// ////////// //////////
    // generic_param

    __ApplyToken(generic_param, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
                break;

            case __TokenValue(k_in):
                __This->set_param_type(generic_param_type_t::in, token);
                break;

            case __TokenValue(k_out):
                __This->set_param_type(generic_param_type_t::out, token);
                break;

            case __TokenValue(three_dots):
                __This->set_param_type(generic_param_type_t::params, token);
                break;
        }
    }

    __ApplyAst(generic_param, node, args)
    {
        switch (node->value())
        {
            case __AstValue(attributes):
                __This->set_child(__Type::__attributes__, node);
                break;
        }
    }

    __OnCompleted(generic_param, args) { }

    ////////// ////////// ////////// ////////// //////////
    // generic_params

    __ApplyToken(generic_params, token, args) { }

    __ApplyAst(generic_params, node, args)
    {
        switch (node->value())
        {
            case __AstValue(generic_param):
                __This->append_child(__Type::__params__, node);
                break;
        }
    }

    __OnCompleted(generic_params, args) { }

    ////////// ////////// ////////// ////////// //////////
    // generic_constraint

    __ApplyToken(generic_constraint, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
                break;

            case __TokenValue(k_class):
                __This->set_constraint(generic_constraint_ttype_t::class_, token);
                break;

            case __TokenValue(k_struct):
                __This->set_constraint(generic_constraint_ttype_t::struct_, token);
                break;

            case __TokenValue(k_new):
                __This->set_constraint(generic_constraint_ttype_t::new_, token);
                break;

            // class?
            case __TokenValue(question_mark):
                __This->set_constraint(generic_constraint_ttype_t::nullable_, token);
                break;
        }
    }

    __ApplyAst(generic_constraint, node, args)
    {
        switch (node->value())
        {
            case __AstValue(type_name):
                __This->append_child(__Type::__type_names__, node);
                break;
        }
    }

    __OnCompleted(generic_constraint, args) { }

    ////////// ////////// ////////// ////////// //////////
    // generic_constraints

    __ApplyToken(generic_constraints, token, args) { }

    __ApplyAst(generic_constraints, node, args)
    {
        switch (node->value())
        {
            case __AstValue(generic_constraint):
                __This->append_child(__Type::__constraints__, node);
                break;
        }
    }

    __OnCompleted(generic_constraints, args) { }

    ////////// ////////// ////////// ////////// //////////
    // generic_arg

    __ApplyToken(generic_arg, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(k_ref):
                __This->set_atype(generic_arg_type_t::ref);
                break;

            case __TokenValue(k_out):
                __This->set_atype(generic_arg_type_t::out);
                break;

            case __TokenValue(k_params):
                __This->set_atype(generic_arg_type_t::params);
                break;

            // The name of the item of a tuple type.
            case __TokenValue(name):
                __This->name = this->__to_name(token);
                break;
        }
    }

    __ApplyAst(generic_arg, node, args)
    {
        switch (node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::__type_name__, node);
                break;
        }
    }

    __OnCompleted(generic_arg, args) { }

    ////////// ////////// ////////// ////////// //////////
    // deconstruct_st

    // The number of the hidden variables that hold the tuples.
    static int __deconstruct_counter = 0;

    __ApplyToken(deconstruct_st, token, args)
    {
        if (__first == nullptr)
            __first = token;
    }

    __ApplyAst(deconstruct_st, node, args)
    {
        if (node->value() == __AstValue(generic_args))
            __args = node;
        else if (is<expression_ast_t *>(node))
            __value = node;
    }

    // (T1 a, T2 b) = value; is: var __tupleN = value; and the variables, of the items of the tuple.
    __OnCompleted(deconstruct_st, args)
    {
        _A(__args != nullptr && __value != nullptr);

        name_t tuple_name = this->__to_name(_F(_T("__tuple%1%"), ++__deconstruct_counter));

        // The tuple: var __tupleN = value;
        auto * uncertain = this->template __new_ast<uncertain_type_name_ast_node_t>();
        uncertain->on_commit();

        auto * type_name = this->template __new_ast<type_name_ast_node_t>();
        type_name->set_child(type_name_ast_node_t::__underlying__, uncertain);
        type_name->on_commit();
        __This->set_child(__Type::__type_name__, type_name);

        auto * tuple_item = this->template __new_ast<_defination_st_item_ast_node_t>();
        tuple_item->set_name(tuple_name, __first);
        tuple_item->set_child(_defination_st_item_ast_node_t::__expression__, __value);
        tuple_item->on_commit();
        __This->append_child(__Type::__items__, tuple_item);

        // The variables.
        int index = 0;
        auto * arg_nodes = (generic_args_ast_node_t *)__args;
        for (size_t k = 0, count = arg_nodes->child_count(generic_args_ast_node_t::__generic_args__);
                                                                                    k < count; k++)
        {
            auto * arg = (generic_arg_ast_node_t *)arg_nodes->child_at(
                                            generic_args_ast_node_t::__generic_args__, k);

            // _ is no variable.
            if (arg->name == this->__to_name(_T("_")))
            {
                index++;
                continue;
            }

            auto * item = this->template __new_ast<_defination_st_item_ast_node_t>();
            item->code_unit = arg->code_unit != nullptr? arg->code_unit : __args->code_unit;
            if (arg->name != name_t::null)
                item->set_name(arg->name, __first);

            item->set_child(_defination_st_item_ast_node_t::__type_name__,
                            arg->child_at(generic_arg_ast_node_t::__type_name__));
            item->set_tuple_source(tuple_name, index++);
            item->on_commit();

            __This->append_child(__Type::__items__, item);
        }
    }


    // The variables of a deconstruction of the names, that are all var: var __tupleN = value; and the variable of each name
    // that is not _, that has the item of the tuple that its index is. It is a part of the code of the builders that make
    // them.
    #define __VarDeconstruct(define, names, value, first_token, tuple_name)                                           \
        do {                                                                                                    \
            auto var_type_name = [&]() {                                                                        \
                auto * uncertain = this->template __new_ast<uncertain_type_name_ast_node_t>();                  \
                uncertain->on_commit();                                                                         \
                auto * type_name = this->template __new_ast<type_name_ast_node_t>();                            \
                type_name->set_child(type_name_ast_node_t::__underlying__, uncertain);                          \
                type_name->on_commit();                                                                         \
                return type_name;                                                                               \
            };                                                                                                  \
                                                                                                                \
            (define)->set_child(defination_st_ast_node_t::__type_name__, var_type_name());                      \
                                                                                                                \
            auto * tuple_item = this->template __new_ast<_defination_st_item_ast_node_t>();                     \
            tuple_item->set_name(tuple_name, first_token);                                                            \
            tuple_item->set_child(_defination_st_item_ast_node_t::__expression__, value);                       \
            tuple_item->on_commit();                                                                            \
            (define)->append_child(defination_st_ast_node_t::__items__, tuple_item);                            \
                                                                                                                \
            int item_index = 0;                                                                                 \
            for (auto & target : names)                                                                         \
            {                                                                                                   \
                int this_index = item_index++;                                                                  \
                if (target.first == this->__to_name(_T("_")))                                                   \
                    continue;                                                                                   \
                                                                                                                \
                auto * item = this->template __new_ast<_defination_st_item_ast_node_t>();                       \
                item->code_unit = (define)->code_unit;                                                          \
                item->set_name(target.first, target.second);                                                    \
                item->set_child(_defination_st_item_ast_node_t::__type_name__, var_type_name());                \
                item->set_tuple_source(tuple_name, this_index);                                                 \
                item->on_commit();                                                                              \
                                                                                                                \
                (define)->append_child(defination_st_ast_node_t::__items__, item);                              \
            }                                                                                                   \
        } while (false)

    ////////// ////////// ////////// ////////// //////////
    // deconstruct_var_st

    __ApplyToken(deconstruct_var_st, token, args)
    {
        if (__first == nullptr)
            __first = token;

        if (token->value == __TokenValue(name))
            __names.push_back(std::make_pair(this->__to_name(token), token));
    }

    __ApplyAst(deconstruct_var_st, node, args)
    {
        if (is<expression_ast_t *>(node))
            __value = node;
    }

    // var (a, b) = value; is: var __tupleN = value; and the variables, of the items of the tuple.
    __OnCompleted(deconstruct_var_st, args)
    {
        _A(!__names.empty() && __value != nullptr);

        name_t tuple_name = this->__to_name(_F(_T("__tuple%1%"), ++__deconstruct_counter));
        __VarDeconstruct(&*__This, __names, __value, __first, tuple_name);
    }

    ////////// ////////// ////////// ////////// //////////
    // deconstruct_assign_st

    __ApplyToken(deconstruct_assign_st, token, args)
    {
        if (__first == nullptr)
            __first = token;

        if (token->value == __TokenValue(name))
            __names.push_back(std::make_pair(this->__to_name(token), token));
    }

    __ApplyAst(deconstruct_assign_st, node, args)
    {
        if (is<expression_ast_t *>(node))
            __value = node;
    }

    // (a, b) = value; is { var __tupleN = value; a = __tupleN.Item1; b = __tupleN.Item2; }
    __OnCompleted(deconstruct_assign_st, args)
    {
        _A(__value != nullptr);

        typedef _single_expression_ast_node_t __single_t;

        name_t tuple_name = this->__to_name(_F(_T("__tuple%1%"), ++__deconstruct_counter));

        auto token_of = [&](cs_token_value_t value) {
            return this->template __new_obj<token_t>((token_value_t)value,
                                                     __first != nullptr? (const code_unit_t *)*__first : nullptr);
        };

        // The expression that has the tokens.
        auto expression_of = [&](ast_node_t * single) {
            auto * exp = this->template __new_ast<expression_ast_node_t>();
            exp->append_expression(single);
            exp->code_unit = __This->code_unit;
            exp->on_commit();
            return exp;
        };

        // var __tupleN = value;
        auto * uncertain = this->template __new_ast<uncertain_type_name_ast_node_t>();
        uncertain->on_commit();

        auto * type_name = this->template __new_ast<type_name_ast_node_t>();
        type_name->set_child(type_name_ast_node_t::__underlying__, uncertain);
        type_name->on_commit();

        auto * define = this->template __new_ast<defination_st_ast_node_t>();
        define->set_child(defination_st_ast_node_t::__type_name__, type_name);

        auto * tuple_item = this->template __new_ast<_defination_st_item_ast_node_t>();
        tuple_item->set_name(tuple_name, __first);
        tuple_item->set_child(_defination_st_item_ast_node_t::__expression__, __value);
        tuple_item->on_commit();
        define->append_child(defination_st_ast_node_t::__items__, tuple_item);
        define->on_commit();

        __This->append_child(__Type::__statements__, define);

        // a = __tupleN.Item1;
        int index = 0;
        for (auto & target : __names)
        {
            // (a, _) = value: the item is not assigned.
            if (target.first == this->__to_name(_T("_")))
            {
                ++index;
                continue;
            }

            auto * single = this->template __new_ast<__single_t>();
            single->append_name(target.first, target.second);
            single->append_token(token_of(cs_token_value_t::assign));
            single->append_name(tuple_name, target.second);
            single->append_token(token_of(cs_token_value_t::member_point));
            single->append_name(this->__to_name(_F(_T("Item%1%"), ++index)), target.second);
            single->code_unit = __This->code_unit;
            single->on_commit();

            auto * statement = this->template __new_ast<expression_st_ast_node_t>();
            statement->set_child(expression_st_ast_node_t::__expression__, expression_of(single));
            statement->on_commit();

            __This->append_child(__Type::__statements__, statement);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_args

    __ApplyToken(generic_args, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(comma):
            case __TokenValue(right_angle):
            case __TokenValue(right_bracket):
                __This->append_child(__Type::__generic_args__, __last_arg);
                __last_arg = nullptr;
                break;
        }
    }

    __ApplyAst(generic_args, node, args)
    {
        switch (node->value())
        {
            case __AstValue(generic_arg):
                __last_arg = node;
                break;
        }
    }

    __OnCompleted(generic_args, args) { }

    ////////// ////////// ////////// ////////// //////////
    // argument

    __ApplyToken(argument, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
                break;

            case __TokenValue(k_ref):
                __This->set_argument_type(param_type_t::ref, token);
                break;

            case __TokenValue(k_out):
                __This->set_argument_type(param_type_t::out, token);
                break;

            // The items of a params generic parameter: f(items ...).
            case __TokenValue(three_dots):
                __This->set_argument_type(param_type_t::extends, token);
                break;
        }
    }

    __ApplyAst(argument, node, args)
    {
        switch (node->value())
        {
            case __AstValue(expression):
            case __AstValue(declaration_exp):
                __This->set_child(__Type::__expression__, node);
                break;

            case __AstValue(attributes):
                __This->set_child(__Type::__attributes__, node);
                break;
        }
    }

    __OnCompleted(argument, args) { }

    ////////// ////////// ////////// ////////// //////////
    // arguments

    __ApplyToken(arguments, token, args) { }

    __ApplyAst(arguments, node, args)
    {
        switch (node->value())
        {
            case __AstValue(argument):
                __This->append_child(__Type::__arguments__, node);
                break;
        }
    }

    __OnCompleted(arguments, args) { }

    ////////// ////////// ////////// ////////// //////////
    // cvalue

    __ApplyToken(cvalue, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(k_true):
                __set_value(token, cvalue_t(tvalue_t(true)));
                break;

            case __TokenValue(k_false):
                __set_value(token, cvalue_t(tvalue_t(false)));
                break;

            case __TokenValue(k_null):
                __set_value(token, cvalue_t());
                break;

            case __TokenValue(cvalue): {
                token_data_t * data = token->data;
                _A(data != nullptr);
                __set_value(token, *data);
            }   break;
        }
    }

    template<typename value_t>
    void __B(cvalue)::__set_value(token_t * token, value_t && value)
    {
        __This->set_value(std::forward<value_t>(value));
    }

    __ApplyAst(cvalue, node, args) { }
    __OnCompleted(cvalue, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _attribute_group

    __ApplyToken(_attribute_group, token, args) { }

    __ApplyAst(_attribute_group, node, args)
    {
        switch (node->value())
        {
            case __AstValue(attribute):
                __This->append_child(__Type::__items__, node);
                break;
        }
    }

    __OnCompleted(_attribute_group, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _attribute_group_item

    __ApplyToken(_attribute_group_item, token, args) { }

    __ApplyAst(_attribute_group_item, node, args)
    {
        switch (node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::__type_name__, node);
                break;

            case __AstValue(arguments):
                __This->set_child(__Type::__arguments__, node);
                break;

            case __AstValue(_attribute_group_assign):
                __This->append_child(__Type::__assigns__, node);
                break;
        }
    }

    __OnCompleted(_attribute_group_item, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _attribute_group.__assign

    __ApplyToken(_attribute_group_assign, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->set_name(name_t(this->__to_sid(token)), token);
                break;
        }
    }

    __ApplyAst(_attribute_group_assign, node, args)
    {
        switch (node->value())
        {
            case __AstValue(expression):
                __This->set_child(__Type::__expression__, node);
                break;
        }
    }

    __OnCompleted(_attribute_group_assign, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _assembly_group

    __ApplyToken(_assembly_group, token, args) { }

    __ApplyAst(_assembly_group, node, args)
    {
        switch (node->value())
        {
            case __AstValue(attribute):
                __This->append_child(__Type::__items__, node);
                break;
        }
    }

    __OnCompleted(_assembly_group, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _assembly_group_item

    __ApplyToken(_assembly_group_item, token, args) { }

    __ApplyAst(_assembly_group_item, node, args)
    {
        switch (node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::__type_name__, node);
                break;

            case __AstValue(arguments):
                __This->set_child(__Type::__arguments__, node);
                break;

            case __AstValue(_assembly_group_assign):
                __This->append_child(__Type::__assigns__, node);
                break;
        }
    }

    __OnCompleted(_assembly_group_item, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _assembly_group.__assign

    __ApplyToken(_assembly_group_assign, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->set_name(name_t(this->__to_sid(token)), token);
                break;
        }
    }

    __ApplyAst(_assembly_group_assign, node, args)
    {
        switch (node->value())
        {
            case __AstValue(expression):
                __This->set_child(__Type::__expression__, node);
                break;
        }
    }

    __OnCompleted(_assembly_group_assign, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _attribute_type_name

    __ApplyToken(_attribute_type_name, token, args) { }

    __ApplyAst(_attribute_type_name, node, args)
    {
        if (as<type_name_ast_t *>(node) != nullptr)
        {
            __This->set_child(__Type::__underlying__, node);
        }
    }

    __OnCompleted(_attribute_type_name, args) { }

    ////////// ////////// ////////// ////////// //////////
    // attributes

    __ApplyToken(attributes, token, args) { }

    __ApplyAst(attributes, node, args)
    {
        switch (node->value())
        {
            case __AstValue(attribute):
            case __AstValue(_attribute_group):
                __This->append_child(__Type::__attributes__, node);
                break;
        }
    }

    __OnCompleted(attributes, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _decorate_complex

    __ApplyToken(_decorate_complex, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(k_internal):      // internal protected
                __This->access_value = access_value_t::protected_or_internal;
                break;

            case __TokenValue(k_private):       // private protected
                __This->access_value = access_value_t::protected_and_internal;
                break;
        }
    }

    __ApplyAst(_decorate_complex, node, args) { }

    __OnCompleted(_decorate_complex, args) { }

    ////////// ////////// ////////// ////////// //////////
    // statement

    __ApplyToken(statement, token, args) { }

    __ApplyAst(statement, node, args)
    {
        if (as<statement_ast_t *>(node) != nullptr)
        {
            __This->set_child(__Type::__underlying__, node);
        }
    }

    __OnCompleted(statement, args) { }

    ////////// ////////// ////////// ////////// //////////
    // The statements that follow a using declaration (using T obj = ...;) are its body.

    // Returns the last statement of a block if it is a using declaration.
    template<typename node_t, typename index_t>
    static using_st_ast_node_t * __open_using(node_t * block, index_t index)
    {
        size_t count = block->child_count(index);
        if (count == 0)
            return nullptr;

        // The statement is the child of a statement node.
        ast_node_t * last = block->child_at(index, count - 1);
        if (auto * wrapper = dynamic_cast<statement_ast_node_t *>(last))
            last = wrapper->child_at(statement_ast_node_t::__underlying__);

        using_st_ast_node_t * node = dynamic_cast<using_st_ast_node_t *>(last);

        return node != nullptr && node->declaration_group() != nullptr? node : nullptr;
    }

    // Appends a statement to the group of a using declaration.
    static void __append_to_using_group(using_st_ast_node_t * node, ast_node_t * statement)
    {
        typedef statement_group_st_ast_node_t __group_t;
        auto * group = (__group_t *)node->declaration_group();

        if (group->code_unit == nullptr)
            group->code_unit = statement->code_unit;

        using_st_ast_node_t * inner = __open_using(group, __group_t::__statements__);
        if (inner != nullptr)
            __append_to_using_group(inner, statement);
        else
            group->append_child(__group_t::__statements__, statement);
    }

    // Appends a statement to a block.
    template<typename node_t, typename index_t>
    static void __append_statement(node_t * block, index_t index, ast_node_t * statement)
    {
        using_st_ast_node_t * node = __open_using(block, index);
        if (node != nullptr)
            __append_to_using_group(node, statement);
        else
            block->append_child(index, statement);
    }

    ////////// ////////// ////////// ////////// //////////
    // method_body

    __ApplyToken(method_body, token, args) { }

    __ApplyAst(method_body, node, args)
    {
        switch (node->value())
        {
            case __AstValue(statement):
                __append_statement(&*__This, __Type::__statements__, node);
                break;

            case __AstValue(expression): {
                // => expression; is the body of "return expression;".
                auto * st = this->template __new_ast<return_st_ast_node_t>();
                st->is_arrow = true;
                st->set_child(return_st_ast_node_t::__expression__, node);
                st->on_commit();

                __This->append_child(__Type::__statements__, st);
                break;
            }
        }
    }

    __OnCompleted(method_body, args) { }

    ////////// ////////// ////////// ////////// //////////
    // property

    __ApplyToken(property, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(property, node, args)
    {
        switch (node->value())
        {
            case __AstValue(attributes):
                __This->set_child(__Type::__attributes__, node);
                break;

            case __AstValue(decorate):
                __This->set_child(__Type::__decorate__, node);
                break;

            case __AstValue(type_name):
                if (al::starts_with(args.flag, _T("r")))         // ret
                    __This->set_child(__Type::__type_name__, node);
                else if (al::starts_with(args.flag, _T("o")))    // owner
                    __This->set_child(__Type::__owner_type_name__, node);
                break;

            case __AstValue(params):
                __This->set_child(__Type::__params__, node);
                break;

            case __AstValue(expression): {
                // Type Name { get; set; } = expression; : the initial value.
                if (al::starts_with(args.flag, _T("init")))
                {
                    __This->set_child(__Type::__initializer__, node);
                    break;
                }

                // Type Name => expression; is Type Name { get => expression; }.
                auto * body = this->template __new_ast<method_body_ast_node_t>();
                auto * st = this->template __new_ast<return_st_ast_node_t>();
                st->is_arrow = true;
                st->set_child(return_st_ast_node_t::__expression__, node);
                st->on_commit();
                body->append_child(method_body_ast_node_t::__statements__, st);
                body->on_commit();

                auto * getter = this->template __new_ast<_fake_method_ast_node_t>();
                getter->token_value = cs_token_value_t::k_get;
                getter->set_name(this->__to_name(_T("get")), nullptr);
                getter->set_child(_fake_method_ast_node_t::__body__, body);
                getter->on_commit();

                __This->set_child(__Type::__get_method__, getter);
                break;
            }

            case __AstValue(method):
                switch (((_fake_method_ast_node_t *)node)->token_value)
                {
                    case cs_token_value_t::k_get:
                        __This->set_child_with_check(__Type::__get_method__, node,
                                __cc_t::duplicate, _T("property method"), _T("get"));
                        break;

                    case cs_token_value_t::k_set:
                        __This->set_child_with_check(__Type::__set_method__, node,
                                __cc_t::duplicate, _T("property method"), _T("set"));
                        break;

                    // init is a set that is called only when the object is made.
                    case cs_token_value_t::k_init:
                        __This->set_child_with_check(__Type::__set_method__, node,
                                __cc_t::duplicate, _T("property method"), _T("set or init"));
                        __This->is_init_only = true;
                        break;
 
                    default:
                        _A(false, _T("unknown fake method in property"));
                        break;
                }

                break;
        }
    }

    __OnCompleted(property, args) { }

    ////////// ////////// ////////// ////////// //////////
    // event

    __ApplyToken(event, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(event, node, args)
    {
        switch (node->value())
        {
            case __AstValue(attributes):
                __This->set_child(__Type::__attributes__, node);
                break;

            case __AstValue(decorate):
                __This->set_child(__Type::__decorate__, node);
                break;

            case __AstValue(type_name):
                if (al::starts_with(args.flag, _T("r")))                 // ret
                    __This->set_child(__Type::__type_name__, node);
                else if (al::starts_with(args.flag, _T("o")))
                    __This->set_child(__Type::__owner_type_name__, node);   // owner
                break;

            case __AstValue(method):
                switch (((_fake_method_ast_node_t *)node)->token_value)
                {
                    case cs_token_value_t::k_add:
                        __This->set_child_with_check(__Type::__add_method__, node,
                                __cc_t::duplicate, _T("event method"), _T("add"));
                        break;

                    case cs_token_value_t::k_remove:
                        __This->set_child_with_check(__Type::__remove_method__, node,
                                __cc_t::duplicate, _T("event method"), _T("remove"));
                        break;

                    case cs_token_value_t::k_raise:
                        __This->set_child_with_check(__Type::__raise_method__, node,
                                __cc_t::duplicate, _T("event method"), _T("raise"));
                        break;
 
                    default:
                        _A(false, _T("unknown fake method in event"));
                        break;
                }

                break;
        }
    }

    __OnCompleted(event, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _single_expression

    __ApplyToken(_single_expression, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->append_name(this->__to_name(token), token);
                break;

            // The condition a ? b : c has a colon for each ?. A colon that is not one of them is that of the name
            // of an item of a tuple: (Number: 1, Str: "2"); the name and the value are one element.
            case __TokenValue(question_mark):
                __questions++;
                __This->append_token(token);
                break;

            case __TokenValue(question_colon):
                if (__questions > 0)
                {
                    __questions--;
                    __This->append_token(token);
                }
                else
                {
                    __This->begin_named_item();
                }
                break;

            default:
                __This->append_token(token);
                break;
        }
    }

    __ApplyAst(_single_expression, node, args)
    {
        switch (node->value())
        {
            case __AstValue(cvalue):
                __This->append_cvalue(node);
                break;

            case __AstValue(name_unit):
                __This->append_name_unit((name_unit_ast_node_t *)node);
                break;

            default:
                if (is<expression_ast_t *>(node))
                    __This->append_expression(node);
                break;
        }
    }

    __OnCompleted(_single_expression, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _function_name_expression

    __ApplyToken(_function_name_expression, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->append_name(this->__to_name(token), token);
                break;

            default:
                __This->append_token(token);
                break;
        }
    }

    __ApplyAst(_function_name_expression, node, args)
    {
        switch (node->value())
        {
            case __AstValue(cvalue):
                __This->append_cvalue(node);
                break;

            case __AstValue(generic_args):
                __This->generic_args_node = node;
                break;

            default:
                if (is<expression_ast_t *>(node))
                    __This->append_expression(node);
                break;
        }
    }

    __OnCompleted(_function_name_expression, args) { }

    ////////// ////////// ////////// ////////// //////////
    // type_name_exp

    __ApplyToken(type_name_exp, token, args) { }

    __ApplyAst(type_name_exp, node, args)
    {
        switch (node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::__type_name__, node);
                break;
        }
    }

    __OnCompleted(type_name_exp, args) { }

    ////////// ////////// ////////// ////////// //////////
    // declaration_exp

    __ApplyToken(declaration_exp, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->name = this->__to_name(token);
                break;
        }
    }

    __ApplyAst(declaration_exp, node, args)
    {
        switch (node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::__type_name__, node);
                break;
        }
    }

    __OnCompleted(declaration_exp, args) { }

    ////////// ////////// ////////// ////////// //////////
    // type_cast_exp

    __ApplyToken(type_cast_exp, token, args) { }

    __ApplyAst(type_cast_exp, node, args)
    {
        switch (node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::__type_name__, node);
                break;

            case __AstValue(expression):
            case __AstValue(type_cast_exp):     // (Int64)(Color)c
                __This->set_child(__Type::__expression__, node);
                break;
        }
    }

    __OnCompleted(type_cast_exp, args) { }

    ////////// ////////// ////////// ////////// //////////
    // function

    __ApplyToken(function, token, args) { }

    __ApplyAst(function, node, args)
    {
        switch (node->value())
        {
            case __AstValue(expression): {
                __This->set_child(__Type::__namex__, node);
                auto n = (_function_name_expression_ast_node_t *)(node);
                if (n->generic_args_node != nullptr)
                    __This->set_child(__Type::__generic_args__, n->generic_args_node);
            }   break;

            case __AstValue(arguments):
                __This->set_child(__Type::__arguments__, node);
                break;
        }
    }

    __OnCompleted(function, args) { }

    ////////// ////////// ////////// ////////// //////////
    // index

    __ApplyToken(index, token, args)
    {
        // a?[0]
        if (token->value == __TokenValue(question_square))
            __This->null_conditional = true;
    }

    __ApplyAst(index, node, args)
    {
        switch (node->value())
        {
            case __AstValue(expression):
                __This->set_child(__Type::__namex__, node);
                break;

            case __AstValue(arguments):
                __This->set_child(__Type::__arguments__, node);
                break;
        }
    }

    __OnCompleted(index, args) { }

    ////////// ////////// ////////// ////////// //////////
    // new

    __ApplyToken(new, token, args) { }

    __ApplyAst(new, node, args)
    {
        switch (node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::__type_name__, node);
                break;

            case __AstValue(arguments):
                __This->set_child(__Type::__arguments__, node);
                break;

            case __AstValue(new_assign):
                __This->append_child(__Type::__inits__, node);
                break;
        }
    }

    __OnCompleted(new, args) { }

    ////////// ////////// ////////// ////////// //////////
    // new.__assign: Name = value of an object initializer.

    __ApplyToken(new_assign, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->set_name(name_t(this->__to_sid(token)), token);
                break;
        }
    }

    __ApplyAst(new_assign, node, args)
    {
        switch (node->value())
        {
            case __AstValue(expression):
                __This->set_child(__Type::__expression__, node);
                break;
        }
    }

    __OnCompleted(new_assign, args) { }

    ////////// ////////// ////////// ////////// //////////
    // new_array

    __ApplyToken(new_array, token, args)
    {
        // The brackets of the lengths belong to the first group, the others are
        // rank specifiers of the array type of the elements.
        switch (token->value)
        {
            // new String?[2]: the elements can be null.
            case __TokenValue(question_square):
                __This->element_nullable = true;
                break;

            case __TokenValue(left_square):
                if (__closed > 0)
                    __rank = 1;
                break;

            case __TokenValue(comma):
                if (__closed > 0)
                    __rank++;
                break;

            case __TokenValue(right_square):
                if (__closed > 0)
                    __ranks.push_back(__rank);

                __closed++;
                break;
        }
    }

    __ApplyAst(new_array, node, args)
    {
        switch (node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::__type_name__, node);
                break;

            case __AstValue(array_lengths):
                __This->set_child(__Type::__lengths__, node);
                break;

            case __AstValue(array_initializer):
                __This->set_child(__Type::__initializer__, node);
                break;
        }
    }

    __OnCompleted(new_array, args)
    {
        // new T[n][][,]: the elements are arrays, the first rank specifier is the outermost.
        if (!__ranks.empty())
        {
            ast_node_t * element = __This->child_at(__Type::__type_name__);

            for (int k = (int)__ranks.size() - 1; k >= 0; k--)
            {
                auto * array_type = this->template __new_ast<array_type_name_ast_node_t>();
                array_type->set_child(array_type_name_ast_node_t::__element_type_name__, element);

                for (int d = 0; d < __ranks[k]; d++)
                    array_type->append_child(array_type_name_ast_node_t::__dimensions__, nullptr);

                array_type->on_commit();
                element = array_type;
            }

            __This->set_child(__Type::__type_name__, element);
        }

        if (!__This->has_child(__Type::__lengths__))
        {
            __This->set_child(__Type::__lengths__,
                this->__new_ast_with_commit<__default_array_lengths_ast_node_t>()
            );
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // array_initializer

    __ApplyToken(array_initializer, token, args) { }

    __ApplyAst(array_initializer, node, args)
    {
        switch (node->value())
        {
            case __AstValue(array_initializer):
            case __AstValue(expression):
                __This->append_child(__Type::__elements__, node);
                break;
        }
    }

    __OnCompleted(array_initializer, args) { }

    ////////// ////////// ////////// ////////// //////////
    // array_lengths

    __ApplyToken(array_lengths, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(comma):
                __This->append_child(__Type::__lengths__, __last_exp);
                __last_exp = nullptr;
                break;
        }
    }

    __ApplyAst(array_lengths, node, args)
    {
        switch (node->value())
        {
            case __AstValue(expression):
                __last_exp = node;
                break;
        }
    }

    __OnCompleted(array_lengths, args)
    {
        __This->append_child(__Type::__lengths__, __last_exp);
    }

    ////////// ////////// ////////// ////////// //////////
    // lambda

    __ApplyToken(lambda, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->add_param_name(this->__to_name(token));
                break;
        }
    }

    __ApplyAst(lambda, node, args)
    {
        switch (node->value())
        {
            case __AstValue(params):
                __This->set_child(__Type::__params__, node);
                break;

            case __AstValue(method_body):
                __This->set_child(__Type::__body__, node);
                break;

            case __AstValue(expression):
                __This->set_child(__Type::__expression__, node);
                break;
        }
    }

    __OnCompleted(lambda, args) { }

    ////////// ////////// ////////// ////////// //////////
    // default_value

    __ApplyToken(default_value, token, args) { }

    __ApplyAst(default_value, node, args)
    {
        switch (node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::__type_name__, node);
                break;
        }
    }

    __OnCompleted(default_value, args) { }

    ////////// ////////// ////////// ////////// //////////
    // checked_exp

    __ApplyToken(checked_exp, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(k_checked):
                __This->checked = true;
                break;

            case __TokenValue(k_unchecked):
                __This->checked = false;
                break;
        }
    }

    __ApplyAst(checked_exp, node, args)
    {
        switch (node->value())
        {
            case __AstValue(expression):
                __This->set_child(__Type::__expression__, node);
                break;
        }
    }

    __OnCompleted(checked_exp, args) { }

    ////////// ////////// ////////// ////////// //////////
    // await_exp

    __ApplyToken(await_exp, token, args) { }

    __ApplyAst(await_exp, node, args)
    {
        switch (node->value())
        {
            case __AstValue(_single_expression):
            case __AstValue(expression):
                __This->set_child(__Type::__expression__, node);
                break;
        }
    }

    __OnCompleted(await_exp, args) { }

    ////////// ////////// ////////// ////////// //////////
    // throw_exp

    __ApplyToken(throw_exp, token, args) { }

    __ApplyAst(throw_exp, node, args)
    {
        switch (node->value())
        {
            case __AstValue(expression):
                __This->set_child(__Type::__expression__, node);
                break;
        }
    }

    __OnCompleted(throw_exp, args) { }

    ////////// ////////// ////////// ////////// //////////
    // statements

    __ApplyToken(statements, token, args) { }

    __ApplyAst(statements, node, args)
    {
        switch (node->value())
        {
            case __AstValue(statement):
                __append_statement(&*__This, __Type::__statements__, node);
                break;
        }
    }

    __OnCompleted(statements, args) { }

    ////////// ////////// ////////// ////////// //////////
    // expression_st

    __ApplyToken(expression_st, token, args) { }

    __ApplyAst(expression_st, node, args)
    {
        switch (node->value())
        {
            case __AstValue(expression):
                __This->set_child(__Type::__expression__, node);
                break;
        }
    }

    __OnCompleted(expression_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // type_def_st

    __ApplyToken(type_def_st, token, args) { }

    __ApplyAst(type_def_st, node, args)
    {
        switch (node->value())
        {
            case __AstValue(type_def):
                __This->set_child(__Type::__type_def__, node);
                break;
        }
    }

    __OnCompleted(type_def_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // defination_st

    __ApplyToken(defination_st, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(k_const):
                __This->set_constant(true);
                break;
        }

    }

    __ApplyAst(defination_st, node, args)
    {
        switch (node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::__type_name__, node);
                break;

            case __AstValue(_defination_st_item):
                __This->append_child(__Type::__items__, node); 
                break;
        }
    }

    __OnCompleted(defination_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _defination_st_item

    __ApplyToken(_defination_st_item, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(_defination_st_item, node, args)
    {
        switch (node->value())
        {
            default:
                if (is<expression_ast_t *>(node))
                    __This->set_child(__Type::__expression__, node);
                break;
        }
    }

    __OnCompleted(_defination_st_item, args) { }

    ////////// ////////// ////////// ////////// //////////
    // break_st

    __ApplyToken(break_st, token, args) { }
    __ApplyAst(break_st, node, args) { }
    __OnCompleted(break_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // continue_st

    __ApplyToken(continue_st, token, args) { }
    __ApplyAst(continue_st, node, args) { }
    __OnCompleted(continue_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // throw_st

    __ApplyToken(throw_st, token, args) { }

    __ApplyAst(throw_st, node, args)
    {
        switch (node->value())
        {
            case __AstValue(expression):
                __This->set_child(__Type::__expression__, node);
                break;
        }
    }

    __OnCompleted(throw_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // goto_st

    __ApplyToken(goto_st, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->set_label(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(goto_st, node, args) { }
    __OnCompleted(goto_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // return_st

    __ApplyToken(return_st, token, args) { }

    __ApplyAst(return_st, node, args)
    {
        switch (node->value())
        {
            case __AstValue(expression):
                __This->set_child(__Type::__expression__, node);
                break;
        }
    }

    __OnCompleted(return_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // yield_st

    __ApplyToken(yield_st, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(k_break):
                __This->set_break();
                break;
        }
    }

    __ApplyAst(yield_st, node, args)
    {
        switch (node->value())
        {
            case __AstValue(expression):
                __This->set_child(__Type::__expression__, node);
                break;
        }
    }

    __OnCompleted(yield_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // do_while_st

    __ApplyToken(do_while_st, token, args) { }

    __ApplyAst(do_while_st, node, args)
    {
        switch (node->value())
        {
            case __AstValue(expression):
            case __AstValue(expressions):
                __This->set_child(__Type::__condition__, node);
                break;

            case __AstValue(statement):
                __This->set_child(__Type::__body__, node);
                break;
        }
    }

    __OnCompleted(do_while_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // while_st

    __ApplyToken(while_st, token, args) { }

    __ApplyAst(while_st, node, args)
    {
        switch (node->value())
        {
            case __AstValue(expression):
            case __AstValue(expressions):
                __This->set_child(__Type::__condition__, node);
                break;

            case __AstValue(statement):
                __This->set_child(__Type::__body__, node);
                break;
        }
    }

    __OnCompleted(while_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // for_st

    __ApplyToken(for_st, token, args) { }

    __ApplyAst(for_st, node, args)
    {
        switch (node->value())
        {
            case __AstValue(defination_st):
                __This->set_child(__Type::__initialize__, node);
                break;

            case __AstValue(expression):
            case __AstValue(expressions):
                if (al::starts_with(args.flag, _T("ini")))       // initialize
                {
                    __This->set_child(__Type::__initialize__, node);
                }
                else if (al::starts_with(args.flag, _T("c")))    // condition
                {
                    __This->set_child(__Type::__condition__, node);
                }
                else if (al::starts_with(args.flag, _T("inc")))  // increase
                {
                    __This->set_child(__Type::__increase__, node);
                }
                break;

            case __AstValue(statement):
                __This->set_child(__Type::__body__, node);
                break;
        }
    }

    __OnCompleted(for_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // for_each_st

    __ApplyToken(for_each_st, token, args)
    {
        if (__first == nullptr && token->value == __TokenValue(name))
            __first = token;

        switch (token->value)
        {
            case __TokenValue(name):
                if (__deconstruct)
                    __names.push_back(std::make_pair(this->__to_name(token), token));
                else
                    __This->set_variable(this->__to_name(token), token);
                break;

            case __TokenValue(k_await):
                __This->is_await = true;
                break;

            // foreach (var (a, b) in items): var is not the type of a name, it is the items.
            case __TokenValue(k_var):
                __deconstruct = true;
                break;
        }
    }

    __ApplyAst(for_each_st, node, args)
    {
        switch (node->value())
        {
            case __AstValue(expression):
                __This->set_child(__Type::__iterator__, node);
                break;

            case __AstValue(type_name):
                __This->set_child(__Type::__type_name__, node);
                break;

            case __AstValue(statement):
                if (__deconstruct)
                    __body = node;
                else
                    __This->set_child(__Type::__body__, node);
                break;
        }
    }

    // foreach (var (a, b) in items) body is: foreach (var __item in items) { var (a, b) = __item; body }
    __OnCompleted(for_each_st, args)
    {
        if (!__deconstruct)
            return;

        _A(!__names.empty() && __body != nullptr);

        typedef _single_expression_ast_node_t __single_t;

        static int __foreach_counter = 0;
        name_t item_name = this->__to_name(_F(_T("__foreach_item%1%"), ++__foreach_counter));
        name_t tuple_name = this->__to_name(_F(_T("__tuple%1%"), ++__deconstruct_counter));

        // The variable of the item: var __item.
        auto * uncertain = this->template __new_ast<uncertain_type_name_ast_node_t>();
        uncertain->on_commit();

        auto * type_name = this->template __new_ast<type_name_ast_node_t>();
        type_name->set_child(type_name_ast_node_t::__underlying__, uncertain);
        type_name->on_commit();

        __This->set_child(__Type::__type_name__, type_name);
        __This->set_variable(item_name, __first);

        // var (a, b) = __item;
        auto * single = this->template __new_ast<__single_t>();
        single->append_name(item_name, __first);
        single->code_unit = __This->code_unit;
        single->on_commit();

        auto * value = this->template __new_ast<expression_ast_node_t>();
        value->append_expression(single);
        value->code_unit = __This->code_unit;
        value->on_commit();

        auto * define = this->template __new_ast<defination_st_ast_node_t>();
        define->code_unit = __This->code_unit;
        __VarDeconstruct(define, __names, value, __first, tuple_name);
        define->on_commit();

        // { var (a, b) = __item; body }
        auto * group = this->template __new_ast<statement_group_st_ast_node_t>();
        group->append_child(statement_group_st_ast_node_t::__statements__, define);
        group->append_child(statement_group_st_ast_node_t::__statements__, __body);
        group->code_unit = __This->code_unit;
        group->on_commit();

        __This->set_child(__Type::__body__, group);
    }

    ////////// ////////// ////////// ////////// //////////
    // using_st

    __ApplyToken(using_st, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(k_lock):
                __This->set_lock();
                break;

            case __TokenValue(name):
                __This->set_variable(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(using_st, node, args)
    {
        switch (node->value())
        {
            case __AstValue(expression):
                __This->set_child(__Type::__initializer__, node);
                break;

            case __AstValue(type_name):
                __This->set_child(__Type::__type_name__, node);
                break;

            case __AstValue(statement):
                __This->set_child(__Type::__body__, node);
                break;
        }
    }

    __OnCompleted(using_st, args)
    {
        // using T obj = ...; : the statements that follow it are the body.
        if (!__This->has_child(__Type::__body__))
            __This->set_declaration(__new_obj<statement_group_st_ast_node_t>(this->__context));
    }

    ////////// ////////// ////////// ////////// //////////
    // if_st

    __ApplyToken(if_st, token, args) { }

    __ApplyAst(if_st, node, args)
    {
        switch (node->value())
        {
            case __AstValue(expression):
            case __AstValue(expressions):
                __This->set_child(__Type::__condition__, node);
                break;

            case __AstValue(statement):
                if (al::starts_with(args.flag, _T("if")))        // if_body
                    __This->set_child(__Type::__if_body__, node);
                else if (al::starts_with(args.flag, _T("else"))) // else_body
                    __This->set_child(__Type::__else_body__, node);

                break;
        }
    }

    __OnCompleted(if_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // switch_st

    __ApplyToken(switch_st, token, args) { }

    __ApplyAst(switch_st, node, args)
    {
        switch (node->value())
        {
            case __AstValue(expression):
            case __AstValue(expressions):
                __This->set_child(__Type::__expression__, node);
                break;

            case __AstValue(case_):
                __This->append_child(__Type::__cases__, node);
                break;
        }
    }

    __OnCompleted(switch_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // case

    __ApplyToken(case, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(k_default):
                __This->append_default(token);
                break;
        }
    }

    __ApplyAst(case, node, args)
    {
        switch (node->value())
        {
            case __AstValue(expression):
            case __AstValue(declaration_exp):
                if (al::starts_with(args.flag, _T("when")))     // The condition of the label.
                    __This->append_guard(node);
                else
                    __This->append_constant(node);

                break;

            case __AstValue(statements):
                __This->set_child(__Type::__statements__, node);
                break;
        }
    }

    __OnCompleted(case, args) { }

    ////////// ////////// ////////// ////////// //////////
    // try_st

    __ApplyToken(try_st, token, args) { }

    __ApplyAst(try_st, node, args)
    {
        switch (node->value())
        {
            case __AstValue(statement):
                if (al::starts_with(args.flag, _T("t")))         // try
                {
                    __This->set_child(__Type::__try__, node);
                }
                else if (al::starts_with(args.flag, _T("f")))    // finally
                {
                    __This->set_child(__Type::__finally__, node);
                }

                break;

            case __AstValue(catch_):
                __This->append_child(__Type::__catches__, node);
                break;
        }
    }

    __OnCompleted(try_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // catch

    __ApplyToken(catch, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->set_variable(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(catch, node, args)
    {
        switch (node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::__type_name__, node);
                break;

            case __AstValue(statement):
                __This->set_child(__Type::__body__, node);
                break;
        }
    }

    __OnCompleted(catch, args) { }

    ////////// ////////// ////////// ////////// //////////
    // empty_st

    __ApplyToken(empty_st, token, args) { }
    __ApplyAst(empty_st, node, args) { }
    __OnCompleted(empty_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // statement_group_st

    __ApplyToken(statement_group_st, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(k_checked):
                __This->has_checked = true;
                __This->checked = true;
                break;

            case __TokenValue(k_unchecked):
                __This->has_checked = true;
                __This->checked = false;
                break;
        }
    }

    __ApplyAst(statement_group_st, node, args)
    {
        switch (node->value())
        {
            case __AstValue(statement):
                __append_statement(&*__This, __Type::__statements__, node);
                break;
        }
    }

    __OnCompleted(statement_group_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _fields

    __ApplyToken(_fields, token, args) { }

    __ApplyAst(_fields, node, args)
    {
        switch (node->value())
        {
            case __AstValue(attributes):
                __This->set_child(__Type::__attributes__, node);
                break;

            case __AstValue(decorate):
                __This->set_child(__Type::__decorate__, node);
                break;

            case __AstValue(type_name):
                __This->set_child(__Type::__type_name__, node);
                break;

            case __AstValue(field):
                __This->append_child(__Type::__items__, node);
                break;
        }
    }

    __OnCompleted(_fields, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _fields_item

    __ApplyToken(_fields_item, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(_fields_item, node, args)
    {
        switch (node->value())
        {
            case __AstValue(attributes):
                __This->set_child(__Type::__attributes__, node);
                break;

            case __AstValue(expression):
                __This->set_child(__Type::__init_value__, node);
                break;
        }
    }

    __OnCompleted(_fields_item, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _fake_method

    __ApplyToken(_fake_method, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(k_add):
                __This->token_value = cs_token_value_t::k_add;
                __This->set_name(this->__to_name(_T("add")), token);
                break;

            case __TokenValue(k_remove):
                __This->token_value = cs_token_value_t::k_remove;
                __This->set_name(this->__to_name(_T("remove")), token);
                break;

            case __TokenValue(k_raise):
                __This->token_value = cs_token_value_t::k_raise;
                __This->set_name(this->__to_name(_T("raise")), token);
                break;

            case __TokenValue(k_set):
                __This->token_value = cs_token_value_t::k_set;
                __This->set_name(this->__to_name(_T("set")), token);
                break;

            case __TokenValue(k_init):
                __This->token_value = cs_token_value_t::k_init;
                __This->set_name(this->__to_name(_T("init")), token);
                break;

            case __TokenValue(k_get):
                __This->token_value = cs_token_value_t::k_get;
                __This->set_name(this->__to_name(_T("get")), token);
                break;
        }
    }

    __ApplyAst(_fake_method, node, args)
    {
        switch (node->value())
        {
            case __AstValue(attributes):
                __This->set_child(__Type::__attributes__, node);
                break;

            case __AstValue(decorate):
                __This->set_child(__Type::__decorate__, node);
                break;

            case __AstValue(params):
                __This->set_child(__Type::__params__, node);
                break;

            case __AstValue(method_body):
                __This->set_child(__Type::__body__, node);
                break;
        }
    }

    __OnCompleted(_fake_method, args) { }

    ////////// ////////// ////////// ////////// //////////
    // method

    __ApplyToken(method, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
                break;

            case __TokenValue(bit_not):
                __This->set_trait(method_trait_t::destructor, token);
                break;

            case __TokenValue(k_this):      // A constructor that calls another: : this(...)
                __This->set_ctor_this();
                break;

            case __TokenValue(k_checked):   // operator checked +
                __This->set_operator_checked();
                break;

            case __TokenValue(k_implicit):  // implicit operator T(S s)
                __This->set_conversion_kind(true);
                break;

            case __TokenValue(k_explicit):  // explicit operator T(S s)
                __This->set_conversion_kind(false);
                break;
        }
    }

    __ApplyAst(method, node, args)
    {
        switch (node->value())
        {
            case __AstValue(decorate):
                __This->set_child(__Type::__decorate__, node);
                break;

            case __AstValue(type_name):
                if (al::starts_with(args.flag, _T("r")))         // ret
                    __This->set_child(__Type::__type_name__, node);
                else if (al::starts_with(args.flag, _T("o")))    // owner
                    __This->set_child(__Type::__owner_type_name__, node);
                else if (al::starts_with(args.flag, _T("c")))    // conversion: the type that it makes is the type
                {                                                //   that the method returns
                    __This->set_child(__Type::__type_name__, node);
                    __This->set_conversion(node);
                }
                break;

            case __AstValue(_operator):
                __operator_ast_node = (_operator_ast_node_t *)node;
                break;

            case __AstValue(attributes):
                __This->set_child(__Type::__attributes__, node);
                break;

            case __AstValue(generic_params):
                __This->set_child(__Type::__generic_params__, node);
                break;

            case __AstValue(params):
                __This->set_child(__Type::__params__, node);
                break;

            case __AstValue(arguments):
                __This->set_child(__Type::__base_ctor_args__, node);
                break;

            case __AstValue(method_body):
                __This->set_child(__Type::__body__, node);
                break;

            case __AstValue(generic_constraints):
                __This->set_child(__Type::__generic_constraints__, node);
                break;
        }
    }

    __OnCompleted(method, args)
    {
        if (__operator_ast_node == nullptr)
            return;

        const operator_property_t * op_property = __operator_ast_node->op_property;
        params_ast_node_t * params_node = (params_ast_node_t *)__This->child_at(__Type::__params__);

        if (params_node != nullptr)
        {
            #define __CheckReassign(_op_count, _op)                                         \
                do {                                                                        \
                    if (__ChildCount(params_node, __params__) == _op_count)                 \
                        op_property = get_system_operator_property(operator_t::_op);        \
                } while (0)

            switch (op_property->op)
            {
                case operator_t::add:
                    __CheckReassign(1, positive);
                    break;

                case operator_t::positive:
                    __CheckReassign(2, add);
                    break;

                case operator_t::sub:
                    __CheckReassign(1, minus);
                    break;

                case operator_t::minus:
                    __CheckReassign(2, sub);
                    break;

                default: break;
            }

            #undef __CheckReassign
        }

        __This->set_operator(op_property, __operator_ast_node);
    }

    ////////// ////////// ////////// ////////// //////////
    // _operator

    __ApplyToken(_operator, token, args)
    {
        // The token that follows >> (the second angle of two right angles) is not a part of the operator.
        if ((cs_token_value_t)token->value == cs_token_value_t::empty)
            return;

        const operator_property_t * property = get_operator_property(
            (cs_token_value_t)token->value
        );

        _A(property != nullptr);
        __This->op_property = property;
    }

    __ApplyAst(_operator, node, args) { }

    __OnCompleted(_operator, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _enum_fields

    __ApplyToken(_enum_fields, token, args) { }

    __ApplyAst(_enum_fields, node, args)
    {
        switch (node->value())
        {
            case __AstValue(field):
                __This->append_child(__Type::__items__, node);
                break;
        }
    }

    __OnCompleted(_enum_fields, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _enum_fields_item

    __ApplyToken(_enum_fields_item, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(_enum_fields_item, node, args)
    {
        switch (node->value())
        {
            case __AstValue(attributes):
                __This->set_child(__Type::__attributes__, node);
                break;

            case __AstValue(expression):
                __This->set_child(__Type::__init_value__, node);
                break;
        }
    }

    __OnCompleted(_enum_fields_item, args) { }

    ////////// ////////// ////////// ////////// //////////
    // type

    __ApplyToken(type, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
                break;

            case __TokenValue(k_class):
                __This->set_ttype(ttype_t::class_, token);
                break;

            case __TokenValue(k_struct):
                __This->set_ttype(ttype_t::struct_, token);
                break;

            case __TokenValue(k_interface):
                __This->set_ttype(ttype_t::interface_, token);
                break;

            case __TokenValue(k_enum):
                __This->set_ttype(ttype_t::enum_, token);
                break;
        }
    }

    __ApplyAst(type, node, args)
    {
        switch (node->value())
        {
            case __AstValue(generic_params):
                __This->set_child(__Type::__generic_params__, node);
                break;

            case __AstValue(type_name):
                __This->append_child(__Type::__super_type_names__, node);
                break;

            case __AstValue(decorate):
                __This->set_child(__Type::__decorate__, node);
                break;

            case __AstValue(attributes):
                __This->set_child(__Type::__attributes__, node);
                break;

            case __AstValue(field):
                __This->append_child(__Type::__fields__, node);
                break;

            case __AstValue(_fields):
                __This->append_child(__Type::__fields__, node);
                break;

            case __AstValue(property):
                __This->append_child(__Type::__properties__, node);
                break;

            case __AstValue(method):
                __This->append_child(__Type::__methods__, node);
                break;

            case __AstValue(event):
                __This->append_child(__Type::__events__, node);
                break;

            case __AstValue(type):
                __This->append_child(__Type::__nest_types__, node);
                break;

            case __AstValue(type_def):
                __This->append_child(__Type::__type_defs__, node);
                break;

            case __AstValue(generic_constraints):
                __This->set_child(__Type::__generic_constraints__, node);
                break;
        }
    }

    __OnCompleted(type, args) { }

    ////////// ////////// ////////// ////////// //////////
    // param

    __ApplyToken(param, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
                break;

            case __TokenValue(k_ref):
                __This->set_param_type(param_type_t::ref, token);
                break;

            case __TokenValue(k_out):
                __This->set_param_type(param_type_t::out, token);
                break;

            case __TokenValue(k_params):
                __This->set_param_type(param_type_t::params, token);
                break;

            case __TokenValue(k_this):
                __This->set_this(token);
                break;

            case __TokenValue(three_dots):
                __This->set_param_type(param_type_t::extends, token);
                break;
        }
    }

    __ApplyAst(param, node, args)
    {
        switch (node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::__type_name__, node);
                break;

            case __AstValue(attributes):
                __This->set_child(__Type::__attributes__, node);
                break;

            case __AstValue(expression):
                __This->set_child(__Type::__default_value__, node);
                break;
        }
    }

    __OnCompleted(param, args) { }

    ////////// ////////// ////////// ////////// //////////
    // params

    __ApplyToken(params, token, args) { }

    __ApplyAst(params, node, args)
    {
        switch (node->value())
        {
            case __AstValue(param):
                __This->append_child(__Type::__params__, node);
                break;
        }
    }

    __OnCompleted(params, args) { }

    ////////// ////////// ////////// ////////// //////////
    // decorate

    __ApplyToken(decorate, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(k_public):
                __set_access(access_value_t::public_, token);
                break;

            case __TokenValue(k_private):
                __set_access(access_value_t::private_, token);
                break;

            case __TokenValue(k_protected):
                __set_access(access_value_t::protected_, token);
                break;

            case __TokenValue(k_internal):
                __set_access(access_value_t::internal, token);
                break;

            case __TokenValue(k_static):
                __This->set_static(token);
                break;

            case __TokenValue(k_sealed):
                __This->set_sealed(token);
                break;

            case __TokenValue(k_override):
                __This->set_override(token);
                break;

            case __TokenValue(k_virtual):
                __This->set_virtual(token);
                break;

            case __TokenValue(k_abstract):
                __This->set_abstract(token);
                break;

            case __TokenValue(k_new):
                __This->set_new(token);
                break;

            case __TokenValue(k_readonly):
                __This->set_readonly(token);
                break;

            case __TokenValue(k_const):
                __This->set_const(token);
                break;

            case __TokenValue(k_extern):
                __This->set_extern(token);
                break;

            case __TokenValue(k_partial):
                __This->set_partial(token);
                break;

            // Accepted, it has no effect: one managed thread runs at a time.
            case __TokenValue(k_volatile):
                break;
        }
    }

    __ApplyAst(decorate, node, args)
    {
        switch (node->value())
        {
            case __AstValue(_decorate_complex):
                __set_access(
                    _M(_decorate_complex_ast_node_t *, node)->access_value, node
                );
                break;
        }
    }

    template<typename element_t>
    void __B(decorate)::__set_access(access_value_t access, element_t element)
    {
        __This->set_access(access, element);
    }

    __OnCompleted(decorate, args) { }

    ////////// ////////// ////////// ////////// //////////
    // import

    __ApplyToken(import, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->set_alias(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(import, node, args)
    {
        switch (node->value())
        {
            case __AstValue(mname):
                if (al::starts_with(args.flag, _T("f")))         // from
                    __This->set_child(__Type::__package__, node);
                else if (al::starts_with(args.flag, _T("i")))    // import
                    __This->set_child(__Type::__assembly__, node);
                break;
        }
    }

    __OnCompleted(import, args) { }

    ////////// ////////// ////////// ////////// //////////
    // namespace

    __ApplyToken(namespace, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(semicolon):
                __This->set_file_scoped();
                break;
        }
    }

    __ApplyAst(namespace, node, args)
    {
        switch (node->value())
        {
            case __AstValue(mname):
                __This->set_child(__Type::__name__, node);
                break;

            case __AstValue(type):
                __This->append_child(__Type::__types__, node);
                break;

            case __AstValue(namespace_):
                __This->append_child(__Type::__namespaces__, node);
                break;

            case __AstValue(type_def):
                __This->append_child(__Type::__type_defs__, node);
                break;
        }
    }

    __OnCompleted(namespace, args) { }

    ////////// ////////// ////////// ////////// //////////
    // using

    __ApplyToken(using, token, args)
    {
        switch (token->value)
        {
            case __TokenValue(name):
                __This->set_alias(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(using, node, args)
    {
        switch (node->value())
        {
            case __AstValue(mname):
                __This->set_child(__Type::__ns__, node);
                break;
        }
    }

    __OnCompleted(using, args) { }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::lang_cs

