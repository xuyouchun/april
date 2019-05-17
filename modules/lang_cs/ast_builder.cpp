
#include "utils.h"
#include "cs_analyze_tree.h"
#include "ast_builder.h"

namespace X_ROOT_NS { namespace modules { namespace lang_cs {

    using namespace core;
    using namespace compile;

    ////////// ////////// ////////// ////////// //////////

    #define __B(name)     AstBuilderName(name)
    #define __C(name)     AstName(name)
    #define __This        (*this)
    #define __Type        std::remove_reference_t<decltype(*__This)>

    #define __ApplyToken(name, token, args)   \
        void __B(name)::apply_token(token_t * token, __apply_token_args_t & args)

    #define __ApplyAst(name, node, args)  \
        void __B(name)::apply_ast(ast_node_t * node, __apply_ast_args_t & args)

    #define __OnCompleted(name, args)   \
                    void __B(name)::on_completed(ast_builder_completed_args_t & args)

    #define __AstValue(name)        ((element_value_t)cs_branch_type_t::name)

    #define __TokenValue(name)      ((element_value_t)cs_token_value_t::name)

    #define __NewFakeAst(type_t, value) \
        this->__new_fake_ast<type_t, (ast_value_t)__AstValue(value)>()

    #define __EachChild(ast_t, index, func)                                 \
        ((ast_t *)node)->each_child_node<ast_node_t>(ast_t::index, func)    \


    ////////// ////////// ////////// ////////// //////////
    // expression

    __ApplyToken(expression, token, args)
    {
        switch(token->value)
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
        switch(node->value())
        {
            case __AstValue(cvalue):
                __This->append_cvalue(node);
                break;

            default:
                if(is<expression_ast_t *>(node))
                    __This->append_expression(node);
                break;
        }
    }

    __OnCompleted(expression, args) { }

    ////////// ////////// ////////// ////////// //////////
    // mname

    __ApplyToken(mname, token, args)
    {
        switch(token->value)
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
        if(is<expression_ast_t *>(node))
        {
            __This->append_child(__Type::expressions, node);
        }
    }

    __OnCompleted(expressions, args) { }

    ////////// ////////// ////////// ////////// //////////
    // type_name

    __ApplyToken(type_name, token, args) { }

    __ApplyAst(type_name, node, args)
    {
        if(as<type_name_ast_t *>(node) != nullptr)
        {
            __This->set_child(__Type::underlying, node);
        }
    }

    __OnCompleted(type_name, args) { }

    ////////// ////////// ////////// ////////// //////////
    // type_name_unit

    __ApplyToken(type_name_unit, token, args)
    {
        switch(token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(type_name_unit, node, args)
    {
        switch(node->value())
        {
            case __AstValue(generic_args):
                __This->set_child(__Type::generic_args, node);
                break;
        }
    }

    __OnCompleted(type_name_unit, args) { }

    ////////// ////////// ////////// ////////// //////////
    // general_type_name

    __ApplyToken(general_type_name, token, args)
    {
        switch(token->value)
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

            case __TokenValue(double_colon):
                if(__global_type == global_type_t::__default__)
                    __global_type = global_type_t::root;
                break;
        }
    }

    void __B(general_type_name)::__set_name(const string_t & name, token_t * token, bool is_root)
    {
        __This->append_child(__Type::units, this->__to_name_unit_ast(_T("System")));
        __This->append_child(__Type::units, this->__to_name_unit_ast(name));
        __This->set_global_type(global_type_t::global);
    }

    __ApplyAst(general_type_name, node, args)
	{
        switch(node->value())
        {
            case __AstValue(type_name_unit):
                __This->append_child(__Type::units, node);
                break;
        }
	}

    __OnCompleted(general_type_name, args)
    {
        if(__global_type != global_type_t::__default__)
        {
            __This->set_global_type(__global_type);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // array_type_name

    __ApplyToken(array_type_name, token, args)
    {
        switch(token->value)
        {
            case __TokenValue(comma):
            case __TokenValue(right_square):
                __This->append_child(__Type::dimensions, __last_dimension);
                __last_dimension = nullptr;
                break;
        }
    }

    __ApplyAst(array_type_name, node, args)
    {
        switch(node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::element_type_name, node);
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
        switch(token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
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
        switch(node->value())
        {
            case __AstValue(type_def_param):
                __This->append_child(__Type::params, node);
                break;
        }
    }

    __OnCompleted(type_def_params, args) { }


    ////////// ////////// ////////// ////////// //////////
    // type_def

    __ApplyToken(type_def, token, args)
    {
        switch(token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(type_def, node, args)
    {
        switch(node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::type_name, node);
                break;

            case __AstValue(type_def_params):
                __This->set_child(__Type::params, node);
                break;
        }
    }

    __OnCompleted(type_def, args) { }

    ////////// ////////// ////////// ////////// //////////
    // type_of

    __ApplyToken(type_of, token, args) { }

    __ApplyAst(type_of, node, args)
    {
        switch(node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::type_name, node);
                break;
        }
    }

    __OnCompleted(type_of, args) { }

    ////////// ////////// ////////// ////////// //////////
    // generic_param

    __ApplyToken(generic_param, token, args)
    {
        switch(token->value)
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

            case __TokenValue(k_params):
                __This->set_param_type(generic_param_type_t::params, token);
                break;
        }
    }

    __ApplyAst(generic_param, node, args)
    {
        switch(node->value())
        {
            case __AstValue(attributes):
                __This->set_child(__Type::attributes, node);
                break;
        }
    }

    __OnCompleted(generic_param, args) { }

    ////////// ////////// ////////// ////////// //////////
    // generic_params

    __ApplyToken(generic_params, token, args) { }

    __ApplyAst(generic_params, node, args)
    {
        switch(node->value())
        {
            case __AstValue(generic_param):
                __This->append_child(__Type::params, node);
                break;
        }
    }

    __OnCompleted(generic_params, args) { }

    ////////// ////////// ////////// ////////// //////////
    // generic_constraint

    __ApplyToken(generic_constraint, token, args)
    {
        switch(token->value)
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
        }
    }

    __ApplyAst(generic_constraint, node, args)
    {
        switch(node->value())
        {
            case __AstValue(type_name):
                __This->append_child(__Type::type_names, node);
                break;
        }
    }

    __OnCompleted(generic_constraint, args) { }

    ////////// ////////// ////////// ////////// //////////
    // generic_constraints

    __ApplyToken(generic_constraints, token, args) { }

    __ApplyAst(generic_constraints, node, args)
    {
        switch(node->value())
        {
            case __AstValue(generic_constraint):
                __This->append_child(__Type::constraints, node);
                break;
        }
    }

    __OnCompleted(generic_constraints, args) { }

    ////////// ////////// ////////// ////////// //////////
    // generic_arg

    __ApplyToken(generic_arg, token, args) { }

    __ApplyAst(generic_arg, node, args)
    {
        switch(node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::type_name, node);
                break;
        }
    }

    __OnCompleted(generic_arg, args) { }

    ////////// ////////// ////////// ////////// //////////
    // generic_args

    __ApplyToken(generic_args, token, args)
    {
        switch(token->value)
        {
            case __TokenValue(comma):
            case __TokenValue(right_angle):
                __This->append_child(__Type::generic_args, __last_arg);
                __last_arg = nullptr;
                break;
        }
    }

    __ApplyAst(generic_args, node, args)
    {
        switch(node->value())
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
        switch(token->value)
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
        }
    }

    __ApplyAst(argument, node, args)
    {
        switch(node->value())
        {
            case __AstValue(expression):
                __This->set_child(__Type::expression, node);
                break;

            case __AstValue(attributes):
                __This->set_child(__Type::attributes, node);
                break;
        }
    }

    __OnCompleted(argument, args) { }

    ////////// ////////// ////////// ////////// //////////
    // arguments

    __ApplyToken(arguments, token, args) { }

    __ApplyAst(arguments, node, args)
    {
        switch(node->value())
        {
            case __AstValue(argument):
                __This->append_child(__Type::arguments, node);
                break;
        }
    }

    __OnCompleted(arguments, args) { }

    ////////// ////////// ////////// ////////// //////////
    // cvalue

    __ApplyToken(cvalue, token, args)
    {
        switch(token->value)
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
        switch(node->value())
        {
            case __AstValue(attribute):
                __This->append_child(__Type::items, node);
                break;
        }
    }

    __OnCompleted(_attribute_group, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _attribute_group_item

    __ApplyToken(_attribute_group_item, token, args) { }

    __ApplyAst(_attribute_group_item, node, args)
    {
        switch(node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::type_name, node);
                break;

            case __AstValue(arguments):
            case __AstValue(_attribute_group_assign):
                __This->set_child(__Type::arguments, node);
                break;
        }
    }

    __OnCompleted(_attribute_group_item, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _attribute_group.__assign

    __ApplyToken(_attribute_group_assign, token, args)
    {
        switch(token->value)
        {
            case __TokenValue(name):
                __This->set_name(name_t(this->__to_sid(token)), token);
                break;
        }
    }

    __ApplyAst(_attribute_group_assign, node, args)
    {
        switch(node->value())
        {
            case __AstValue(expression):
                __This->set_child(__Type::expression, node);
                break;
        }
    }

    __OnCompleted(_attribute_group_assign, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _attribute_type_name

    __ApplyToken(_attribute_type_name, token, args) { }

    __ApplyAst(_attribute_type_name, node, args)
    {
        if(as<type_name_ast_t *>(node) != nullptr)
        {
            __This->set_child(__Type::underlying, node);
        }
    }

    __OnCompleted(_attribute_type_name, args) { }

    ////////// ////////// ////////// ////////// //////////
    // attributes

    __ApplyToken(attributes, token, args) { }

    __ApplyAst(attributes, node, args)
    {
        switch(node->value())
        {
            case __AstValue(attribute):
            case __AstValue(_attribute_group):
                __This->append_child(__Type::attributes, node);
                break;
        }
    }

    __OnCompleted(attributes, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _decorate_complex

    __ApplyToken(_decorate_complex, token, args)
    {
        switch(token->value)
        {
            case __TokenValue(bit_and):
                __This->access_value = access_value_t::protected_and_internal;
                break;

            case __TokenValue(bit_or):
                __This->access_value = access_value_t::protected_or_internal;
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
        if(as<statement_ast_t *>(node) != nullptr)
        {
            __This->set_child(__Type::underlying, node);
        }
    }

    __OnCompleted(statement, args) { }

    ////////// ////////// ////////// ////////// //////////
    // method_body

    __ApplyToken(method_body, token, args) { }

    __ApplyAst(method_body, node, args)
    {
        switch(node->value())
        {
            case __AstValue(statement):
                __This->append_child(__Type::statements, node);
                break;
        }
    }

    __OnCompleted(method_body, args) { }

    ////////// ////////// ////////// ////////// //////////
    // property

    __ApplyToken(property, token, args)
    {
        switch(token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(property, node, args)
    {
        switch(node->value())
        {
            case __AstValue(attributes):
                __This->set_child(__Type::attributes, node);
                break;

            case __AstValue(decorate):
                __This->set_child(__Type::decorate, node);
                break;

            case __AstValue(type_name):
                if(al::starts_with(args.flag, _T("r")))         // ret
                    __This->set_child(__Type::type_name, node);
                else if(al::starts_with(args.flag, _T("o")))    // owner
                    __This->set_child(__Type::owner_type_name, node);
                break;

            case __AstValue(params):
                __This->set_child(__Type::params, node);
                break;

            case __AstValue(method):
                switch(((_fake_method_ast_node_t *)node)->token_value)
                {
                    case cs_token_value_t::k_get:
                        __This->set_child_with_check(__Type::get_method, node,
                                __cc_t::duplicate, _T("property method"), _T("get"));
                        break;

                    case cs_token_value_t::k_set:
                        __This->set_child_with_check(__Type::set_method, node,
                                __cc_t::duplicate, _T("property method"), _T("set"));
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
        switch(token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(event, node, args)
    {
        switch(node->value())
        {
            case __AstValue(attributes):
                __This->set_child(__Type::attributes, node);
                break;

            case __AstValue(decorate):
                __This->set_child(__Type::decorate, node);
                break;

            case __AstValue(type_name):
                if(al::starts_with(args.flag, _T("r")))                 // ret
                    __This->set_child(__Type::type_name, node);
                else if(al::starts_with(args.flag, _T("o")))
                    __This->set_child(__Type::owner_type_name, node);   // owner
                break;

            case __AstValue(method):
                switch(((_fake_method_ast_node_t *)node)->token_value)
                {
                    case cs_token_value_t::k_add:
                        __This->set_child_with_check(__Type::add_method, node,
                                __cc_t::duplicate, _T("event method"), _T("add"));
                        break;

                    case cs_token_value_t::k_remove:
                        __This->set_child_with_check(__Type::remove_method, node,
                                __cc_t::duplicate, _T("event method"), _T("remove"));
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
        switch(token->value)
        {
            case __TokenValue(name):
                __This->append_name(this->__to_name(token), token);
                break;

            default:
                __This->append_token(token);
                break;
        }
    }

    __ApplyAst(_single_expression, node, args)
    {
        switch(node->value())
        {
            case __AstValue(cvalue):
                __This->append_cvalue(node);
                break;

            default:
                if(is<expression_ast_t *>(node))
                    __This->append_expression(node);
                break;
        }
    }

    __OnCompleted(_single_expression, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _function_name_expression

    __ApplyToken(_function_name_expression, token, args)
    {
        switch(token->value)
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
        switch(node->value())
        {
            case __AstValue(cvalue):
                __This->append_cvalue(node);
                break;

            case __AstValue(generic_args):
                __This->generic_args_node = node;
                break;

            default:
                if(is<expression_ast_t *>(node))
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
        switch(node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::type_name, node);
                break;
        }
    }

    __OnCompleted(type_name_exp, args) { }

    ////////// ////////// ////////// ////////// //////////
    // type_cast_exp

    __ApplyToken(type_cast_exp, token, args) { }

    __ApplyAst(type_cast_exp, node, args)
    {
        switch(node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::type_name, node);
                break;

            case __AstValue(expression):
                __This->set_child(__Type::expression, node);
                break;
        }
    }

    __OnCompleted(type_cast_exp, args) { }

    ////////// ////////// ////////// ////////// //////////
    // function

    __ApplyToken(function, token, args) { }

    __ApplyAst(function, node, args)
    {
        switch(node->value())
        {
            case __AstValue(expression): {
                __This->set_child(__Type::namex, node);
                auto n = (_function_name_expression_ast_node_t *)(node);
                if(n->generic_args_node != nullptr)
                    __This->set_child(__Type::generic_args, n->generic_args_node);
            }   break;

            case __AstValue(arguments):
                __This->set_child(__Type::arguments, node);
                break;
        }
    }

    __OnCompleted(function, args) { }

    ////////// ////////// ////////// ////////// //////////
    // index

    __ApplyToken(index, token, args) { }

    __ApplyAst(index, node, args)
    {
        switch(node->value())
        {
            case __AstValue(expression):
                __This->set_child(__Type::namex, node);
                break;

            case __AstValue(arguments):
                __This->set_child(__Type::arguments, node);
                break;
        }
    }

    __OnCompleted(index, args) { }

    ////////// ////////// ////////// ////////// //////////
    // new

    __ApplyToken(new, token, args) { }

    __ApplyAst(new, node, args)
    {
        switch(node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::type_name, node);
                break;

            case __AstValue(arguments):
                __This->set_child(__Type::arguments, node);
                break;
        }
    }

    __OnCompleted(new, args) { }

    ////////// ////////// ////////// ////////// //////////
    // new_array

    __ApplyToken(new_array, token, args) { }

    __ApplyAst(new_array, node, args)
    {
        switch(node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::type_name, node);
                break;

            case __AstValue(array_lengths):
                __This->set_child(__Type::lengths, node);
                break;

            case __AstValue(array_initializer):
                __This->set_child(__Type::initializer, node);
                break;
        }
    }

    __OnCompleted(new_array, args)
    {
        if(!__This->has_child(__Type::lengths))
        {
            __This->set_child(__Type::lengths,
                this->__new_ast_with_commit<__default_array_lengths_ast_node_t>()
            );
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // array_initializer

    __ApplyToken(array_initializer, token, args) { }

    __ApplyAst(array_initializer, node, args)
    {
        switch(node->value())
        {
            case __AstValue(array_initializer):
            case __AstValue(expression):
                __This->append_child(__Type::elements, node);
                break;
        }
    }

    __OnCompleted(array_initializer, args) { }

    ////////// ////////// ////////// ////////// //////////
    // array_lengths

    __ApplyToken(array_lengths, token, args)
    {
        switch(token->value)
        {
            case __TokenValue(comma):
                __This->append_child(__Type::lengths, __last_exp);
                __last_exp = nullptr;
                break;
        }
    }

    __ApplyAst(array_lengths, node, args)
    {
        switch(node->value())
        {
            case __AstValue(expression):
                __last_exp = node;
                break;
        }
    }

    __OnCompleted(array_lengths, args)
    {
        __This->append_child(__Type::lengths, __last_exp);
    }

    ////////// ////////// ////////// ////////// //////////
    // default_value

    __ApplyToken(default_value, token, args) { }

    __ApplyAst(default_value, node, args)
    {
        switch(node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::type_name, node);
                break;
        }
    }

    __OnCompleted(default_value, args) { }

    ////////// ////////// ////////// ////////// //////////
    // statements

    __ApplyToken(statements, token, args) { }

    __ApplyAst(statements, node, args)
    {
        switch(node->value())
        {
            case __AstValue(statement):
                __This->append_child(__Type::statements, node);
                break;
        }
    }

    __OnCompleted(statements, args) { }

    ////////// ////////// ////////// ////////// //////////
    // expression_st

    __ApplyToken(expression_st, token, args) { }

    __ApplyAst(expression_st, node, args)
    {
        switch(node->value())
        {
            case __AstValue(expression):
                __This->set_child(__Type::expression, node);
                break;
        }
    }

    __OnCompleted(expression_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // type_def_st

    __ApplyToken(type_def_st, token, args) { }

    __ApplyAst(type_def_st, node, args)
    {
        switch(node->value())
        {
            case __AstValue(type_def):
                __This->set_child(__Type::type_def, node);
                break;
        }
    }

    __OnCompleted(type_def_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // defination_st

    __ApplyToken(defination_st, token, args)
    {
        switch(token->value)
        {
            case __TokenValue(k_const):
                __This->set_constant(true);
                break;
        }

    }

    __ApplyAst(defination_st, node, args)
    {
        switch(node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::type_name, node);
                break;

            case __AstValue(_defination_st_item):
                __This->append_child(__Type::items, node); 
                break;
        }
    }

    __OnCompleted(defination_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _defination_st_item

    __ApplyToken(_defination_st_item, token, args)
    {
        switch(token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(_defination_st_item, node, args)
    {
        switch(node->value())
        {
            default:
                if(is<expression_ast_t *>(node))
                    __This->set_child(__Type::expression, node);
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
        switch(node->value())
        {
            case __AstValue(expression):
                __This->set_child(__Type::expression, node);
                break;
        }
    }

    __OnCompleted(throw_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // goto_st

    __ApplyToken(goto_st, token, args)
    {
        switch(token->value)
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
        switch(node->value())
        {
            case __AstValue(expression):
                __This->set_child(__Type::expression, node);
                break;
        }
    }

    __OnCompleted(return_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // do_while_st

    __ApplyToken(do_while_st, token, args) { }

    __ApplyAst(do_while_st, node, args)
    {
        switch(node->value())
        {
            case __AstValue(expression):
            case __AstValue(expressions):
                __This->set_child(__Type::condition, node);
                break;

            case __AstValue(statement):
                __This->set_child(__Type::body, node);
                break;
        }
    }

    __OnCompleted(do_while_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // while_st

    __ApplyToken(while_st, token, args) { }

    __ApplyAst(while_st, node, args)
    {
        switch(node->value())
        {
            case __AstValue(expression):
            case __AstValue(expressions):
                __This->set_child(__Type::condition, node);
                break;

            case __AstValue(statement):
                __This->set_child(__Type::body, node);
                break;
        }
    }

    __OnCompleted(while_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // for_st

    __ApplyToken(for_st, token, args) { }

    __ApplyAst(for_st, node, args)
    {
        switch(node->value())
        {
            case __AstValue(defination_st):
                __This->set_child(__Type::initialize, node);
                break;

            case __AstValue(expression):
            case __AstValue(expressions):
                if(al::starts_with(args.flag, _T("ini")))       // initialize
                {
                    __This->set_child(__Type::initialize, node);
                }
                else if(al::starts_with(args.flag, _T("c")))    // condition
                {
                    __This->set_child(__Type::condition, node);
                }
                else if(al::starts_with(args.flag, _T("inc")))  // increase
                {
                    __This->set_child(__Type::increase, node);
                }
                break;

            case __AstValue(statement):
                __This->set_child(__Type::body, node);
                break;
        }
    }

    __OnCompleted(for_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // for_each_st

    __ApplyToken(for_each_st, token, args)
    {
        switch(token->value)
        {
            case __TokenValue(name):
                __This->set_variable(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(for_each_st, node, args)
    {
        switch(node->value())
        {
            case __AstValue(expression):
                __This->set_child(__Type::iterator, node);
                break;

            case __AstValue(type_name):
                __This->set_child(__Type::type_name, node);
                break;

            case __AstValue(statement):
                __This->set_child(__Type::body, node);
                break;
        }
    }

    __OnCompleted(for_each_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // if_st

    __ApplyToken(if_st, token, args) { }

    __ApplyAst(if_st, node, args)
    {
        switch(node->value())
        {
            case __AstValue(expression):
            case __AstValue(expressions):
                __This->set_child(__Type::condition, node);
                break;

            case __AstValue(statement):
                if(al::starts_with(args.flag, _T("if")))        // if_body
                    __This->set_child(__Type::if_body, node);
                else if(al::starts_with(args.flag, _T("else"))) // else_body
                    __This->set_child(__Type::else_body, node);

                break;
        }
    }

    __OnCompleted(if_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // switch_st

    __ApplyToken(switch_st, token, args) { }

    __ApplyAst(switch_st, node, args)
    {
        switch(node->value())
        {
            case __AstValue(expression):
            case __AstValue(expressions):
                __This->set_child(__Type::expression, node);
                break;

            case __AstValue(case_):
                __This->append_child(__Type::cases, node);
                break;
        }
    }

    __OnCompleted(switch_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // case

    __ApplyToken(case, token, args)
    {
        switch(token->value)
        {
            case __TokenValue(k_default):
                __This->append_default(token);
                break;
        }
    }

    __ApplyAst(case, node, args)
    {
        switch(node->value())
        {
            case __AstValue(expression):
                __This->append_child(__Type::constants, node);
                break;

            case __AstValue(statements):
                __This->set_child(__Type::statements, node);
                break;
        }
    }

    __OnCompleted(case, args) { }

    ////////// ////////// ////////// ////////// //////////
    // try_st

    __ApplyToken(try_st, token, args) { }

    __ApplyAst(try_st, node, args)
    {
        switch(node->value())
        {
            case __AstValue(statement):
                if(al::starts_with(args.flag, _T("t")))         // try
                {
                    __This->set_child(__Type::try_, node);
                }
                else if(al::starts_with(args.flag, _T("f")))    // finally
                {
                    __This->set_child(__Type::finally, node);
                }

                break;

            case __AstValue(catch_):
                __This->append_child(__Type::catches, node);
                break;
        }
    }

    __OnCompleted(try_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // catch

    __ApplyToken(catch, token, args)
    {
        switch(token->value)
        {
            case __TokenValue(name):
                __This->set_variable(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(catch, node, args)
    {
        switch(node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::type_name, node);
                break;

            case __AstValue(statement):
                __This->set_child(__Type::body, node);
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

    __ApplyToken(statement_group_st, token, args) { }

    __ApplyAst(statement_group_st, node, args)
    {
        switch(node->value())
        {
            case __AstValue(statement):
                __This->append_child(__Type::statements, node);
                break;
        }
    }

    __OnCompleted(statement_group_st, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _fields

    __ApplyToken(_fields, token, args) { }

    __ApplyAst(_fields, node, args)
    {
        switch(node->value())
        {
            case __AstValue(attributes):
                __This->set_child(__Type::attributes, node);
                break;

            case __AstValue(decorate):
                __This->set_child(__Type::decorate, node);
                break;

            case __AstValue(type_name):
                __This->set_child(__Type::type_name, node);
                break;

            case __AstValue(field):
                __This->append_child(__Type::items, node);
                break;
        }
    }

    __OnCompleted(_fields, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _fields_item

    __ApplyToken(_fields_item, token, args)
    {
        switch(token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(_fields_item, node, args)
    {
        switch(node->value())
        {
            case __AstValue(attributes):
                __This->set_child(__Type::attributes, node);
                break;

            case __AstValue(expression):
                __This->set_child(__Type::init_value, node);
                break;
        }
    }

    __OnCompleted(_fields_item, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _fake_method

    __ApplyToken(_fake_method, token, args)
    {
        switch(token->value)
        {
            case __TokenValue(k_add):
                __This->token_value = cs_token_value_t::k_add;
                __This->set_name(this->__to_name(_T("add")), token);
                break;

            case __TokenValue(k_remove):
                __This->token_value = cs_token_value_t::k_remove;
                __This->set_name(this->__to_name(_T("remove")), token);
                break;

            case __TokenValue(k_set):
                __This->token_value = cs_token_value_t::k_set;
                __This->set_name(this->__to_name(_T("set")), token);
                break;

            case __TokenValue(k_get):
                __This->token_value = cs_token_value_t::k_get;
                __This->set_name(this->__to_name(_T("get")), token);
                break;
        }
    }

    __ApplyAst(_fake_method, node, args)
    {
        switch(node->value())
        {
            case __AstValue(attributes):
                __This->set_child(__Type::attributes, node);
                break;

            case __AstValue(decorate):
                __This->set_child(__Type::decorate, node);
                break;

            case __AstValue(method_body):
                __This->set_child(__Type::body, node);
                break;
        }
    }

    __OnCompleted(_fake_method, args) { }

    ////////// ////////// ////////// ////////// //////////
    // method

    __ApplyToken(method, token, args)
    {
        switch(token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
                break;

            case __TokenValue(bit_not):
                __This->set_trait(method_trait_t::destructor, token);
                break;
        }
    }

    __ApplyAst(method, node, args)
    {
        switch(node->value())
        {
            case __AstValue(decorate):
                __This->set_child(__Type::decorate, node);
                break;

            case __AstValue(type_name):
                if(al::starts_with(args.flag, _T("r")))         // ret
                    __This->set_child(__Type::type_name, node);
                else if(al::starts_with(args.flag, _T("o")))    // owner
                    __This->set_child(__Type::owner_type_name, node);
                else if(al::starts_with(args.flag, _T("c")))    // conversion
                    __This->set_child(__Type::conversion_type_name, node);
                break;

            case __AstValue(_operator):
                __This->set_operator(
                    ((_operator_ast_node_t *)node)->op_property, node
                );
                break;

            case __AstValue(attributes):
                __This->set_child(__Type::attributes, node);
                break;

            case __AstValue(generic_params):
                __This->set_child(__Type::generic_params, node);
                break;

            case __AstValue(params):
                __This->set_child(__Type::params, node);
                break;

            case __AstValue(arguments):
                __This->set_child(__Type::base_ctor_args, node);
                break;

            case __AstValue(method_body):
                __This->set_child(__Type::body, node);
                break;
        }
    }

    __OnCompleted(method, args) { }

    ////////// ////////// ////////// ////////// //////////
    // _operator

    __ApplyToken(_operator, token, args)
    {
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
        switch(node->value())
        {
            case __AstValue(field):
                __This->append_child(__Type::items, node);
                break;
        }
    }

    __OnCompleted(_enum_fields, args)
    {
        __This->set_child(__Type::type_name, __NewFakeAst(general_type_name_t, type_name));

        decorate_t decorate;
        decorate.is_static = true;
        decorate.is_const  = true;

        __This->set_child(__Type::decorate,
            this->__to_ast<__AstValue(decorate)>(decorate)
        );
    }

    ////////// ////////// ////////// ////////// //////////
    // _enum_fields_item

    __ApplyToken(_enum_fields_item, token, args)
    {
        switch(token->value)
        {
            case __TokenValue(name):
                __This->set_name(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(_enum_fields_item, node, args)
    {
        switch(node->value())
        {
            case __AstValue(attributes):
                __This->set_child(__Type::attributes, node);
                break;

            case __AstValue(expression):
                __This->set_child(__Type::init_value, node);
                break;
        }
    }

    __OnCompleted(_enum_fields_item, args) { }

    ////////// ////////// ////////// ////////// //////////
    // type

    __ApplyToken(type, token, args)
    {
        switch(token->value)
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
        switch(node->value())
        {
            case __AstValue(generic_params):
                __This->set_child(__Type::generic_params, node);
                break;

            case __AstValue(type_name):
                __This->append_child(__Type::super_type_names, node);
                break;

            case __AstValue(decorate):
                __This->set_child(__Type::decorate, node);
                break;

            case __AstValue(attributes):
                __This->set_child(__Type::attributes, node);
                break;

            case __AstValue(field):
                __This->append_child(__Type::fields, node);
                break;

            case __AstValue(_fields):
                __This->append_child(__Type::fields, node);
                break;

            case __AstValue(property):
                __This->append_child(__Type::properties, node);
                break;

            case __AstValue(method):
                __This->append_child(__Type::methods, node);
                break;

            case __AstValue(event):
                __This->append_child(__Type::events, node);
                break;

            case __AstValue(type):
                __This->append_child(__Type::nest_types, node);
                break;

            case __AstValue(type_def):
                __This->append_child(__Type::type_defs, node);
                break;
        }
    }

    __OnCompleted(type, args) { }

    ////////// ////////// ////////// ////////// //////////
    // param

    __ApplyToken(param, token, args)
    {
        switch(token->value)
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
        }
    }

    __ApplyAst(param, node, args)
    {
        switch(node->value())
        {
            case __AstValue(type_name):
                __This->set_child(__Type::type_name, node);
                break;

            case __AstValue(attributes):
                __This->set_child(__Type::attributes, node);
                break;

            case __AstValue(expression):
                __This->set_child(__Type::default_value, node);
                break;
        }
    }

    __OnCompleted(param, args) { }

    ////////// ////////// ////////// ////////// //////////
    // params

    __ApplyToken(params, token, args) { }

    __ApplyAst(params, node, args)
    {
        switch(node->value())
        {
            case __AstValue(param):
                __This->append_child(__Type::params, node);
                break;
        }
    }

    __OnCompleted(params, args) { }

    ////////// ////////// ////////// ////////// //////////
    // decorate

    __ApplyToken(decorate, token, args)
    {
        switch(token->value)
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
        }
    }

    __ApplyAst(decorate, node, args)
    {
        switch(node->value())
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
        switch(token->value)
        {
            case __TokenValue(name):
                __This->set_alias(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(import, node, args)
    {
        switch(node->value())
        {
            case __AstValue(mname):
                if(al::starts_with(args.flag, _T("f")))         // from
                    __This->set_child(__Type::package, node);
                else if(al::starts_with(args.flag, _T("i")))    // import
                    __This->set_child(__Type::assembly, node);
                break;
        }
    }

    __OnCompleted(import, args) { }

    ////////// ////////// ////////// ////////// //////////
    // namespace

    __ApplyToken(namespace, token, args) { }

    __ApplyAst(namespace, node, args)
    {
        switch(node->value())
        {
            case __AstValue(mname):
                __This->set_child(__Type::name, node);
                break;

            case __AstValue(type):
                __This->append_child(__Type::types, node);
                break;

            case __AstValue(namespace_):
                __This->append_child(__Type::namespaces, node);
                break;

            case __AstValue(type_def):
                __This->append_child(__Type::type_defs, node);
                break;
        }
    }

    __OnCompleted(namespace, args) { }

    ////////// ////////// ////////// ////////// //////////
    // using

    __ApplyToken(using, token, args)
    {
        switch(token->value)
        {
            case __TokenValue(name):
                __This->set_alias(this->__to_name(token), token);
                break;
        }
    }

    __ApplyAst(using, node, args)
    {
        switch(node->value())
        {
            case __AstValue(mname):
                __This->set_child(__Type::ns, node);
                break;
        }
    }

    __OnCompleted(using, args) { }

    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::lang_cs
