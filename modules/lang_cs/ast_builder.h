
#include <lang_cs.h>
#include "utils.h"
#include "cs_analyze_tree.h"
#include "ast.h"
#include <algorithm.h>

namespace X_ROOT_NS { namespace modules { namespace lang_cs {

    namespace
    {
        using namespace core;
        using namespace compile;

        typedef cs_token_value_t __token_value_t;
    }

    ////////// ////////// ////////// ////////// //////////

    typedef cs_log_code_t           __c_t;
    typedef common_log_code_t       __cc_t;
    typedef xbranch_value_t         __xc_value_t;

    template<typename _ast_t, typename _self_t,
        typename _base_t = x_ast_builder_base_t<_ast_t, _self_t>
    >
    class __ast_builder_base_t : public _base_t
    {
    public:
        typedef _base_t __base_t;
        using __base_t::__base_t;

        template<typename element_t>
        void __log_logic_error(element_t element, const logic_error_base_t & e)
        {
            __base_t::__log(element, __c_t::__error__, _str(e));
        }

    protected:
        auto __to_mname_ast(const string_t & name, code_unit_t * code_unit = nullptr)
        {
            const ast_value_t value = (ast_value_t)cs_branch_type_t::mname;
            return this->template __to_ast<value, mname_t *>(
                this->__to_mname(name), code_unit
            );
        }

        auto __to_name_unit_ast(const string_t & name)
        {
            const ast_value_t value = (ast_value_t)cs_branch_type_t::type_name_unit;
            return this->template __to_ast<value, type_name_unit_t>(
                type_name_unit_t(this->__to_name(name), nullptr)
            );
        }
    };

    ////////// ////////// ////////// ////////// //////////

    #define AstBuilderName(name)    name##_ast_builder_t
    #define AstName(name)           name##_ast_node_t

    #define AstBuilder(name)                                                    \
        class AstBuilderName(name)                                              \
            : public __ast_builder_base_t<AstName(name), AstBuilderName(name)>  \
        {                                                                       \
            typedef ast_builder_completed_args_t    __completed_args_t;         \
            typedef ast_builder_apply_token_args_t  __apply_token_args_t;       \
            typedef ast_builder_apply_ast_args_t    __apply_ast_args_t;         \
        public:                                                                 \
            typedef __ast_builder_base_t<                                       \
                AstName(name), AstBuilderName(name)                             \
            > __base_t;                                                         \
            using __base_t::__base_t;                                           \
                                                                                \
            virtual void apply_token(token_t * token,                           \
                                __apply_token_args_t & args) override;          \
                                                                                \
            virtual void apply_ast(ast_node_t * node,                           \
                                __apply_ast_args_t & args) override;            \
                                                                                \
            virtual void on_completed(__completed_args_t & args) override;


    #define EndAstBuilder                                                       \
        };

    ////////// ////////// ////////// ////////// //////////

    #define __Builder(name)         AstBuilder(name)
    #define __EndBuilder            EndAstBuilder

    #define __SimpleBuilder(name)                                               \
        __Builder(name)                                                         \
        __EndBuilder

    ////////// ////////// ////////// ////////// //////////
    // expression

    __SimpleBuilder(expression)

    ////////// ////////// ////////// ////////// //////////
    // mname

    __SimpleBuilder(mname)

    ////////// ////////// ////////// ////////// //////////
    // expressions

    __SimpleBuilder(expressions)

    ////////// ////////// ////////// ////////// //////////
    // type_name

    __SimpleBuilder(type_name)

    ////////// ////////// ////////// ////////// //////////
    // type_name_unit

    __Builder(type_name_unit)

    __EndBuilder

    ////////// ////////// ////////// ////////// //////////
    // general_type_name

    __Builder(general_type_name)

        void __set_name(const string_t & name, token_t * token, bool is_root = true);

        global_type_t __global_type = global_type_t::__default__;

    __EndBuilder

    ////////// ////////// ////////// ////////// //////////
    // array_type_name

    __Builder(array_type_name)

        ast_node_t * __last_dimension = nullptr;

    __EndBuilder

    ////////// ////////// ////////// ////////// //////////
    // uncertain_type_name

    __SimpleBuilder(uncertain_type_name)

    ////////// ////////// ////////// ////////// //////////
    // type_def_param

    __SimpleBuilder(type_def_param)

    ////////// ////////// ////////// ////////// //////////
    // type_def_params

    __SimpleBuilder(type_def_params)

    ////////// ////////// ////////// ////////// //////////
    // type_def

    __SimpleBuilder(type_def)

    ////////// ////////// ////////// ////////// //////////
    // type_of

    __SimpleBuilder(type_of)

    ////////// ////////// ////////// ////////// //////////
    // generic_param

    __SimpleBuilder(generic_param)

    ////////// ////////// ////////// ////////// //////////
    // generic_params

    __SimpleBuilder(generic_params)

    ////////// ////////// ////////// ////////// //////////
    // generic_constraint

    __SimpleBuilder(generic_constraint)

    ////////// ////////// ////////// ////////// //////////
    // generic_constraints

    __SimpleBuilder(generic_constraints)

    ////////// ////////// ////////// ////////// //////////
    // generic_arg

    __SimpleBuilder(generic_arg)

    ////////// ////////// ////////// ////////// //////////
    // generic_args

    __Builder(generic_args)

        ast_node_t * __last_arg = nullptr;

    __EndBuilder

    ////////// ////////// ////////// ////////// //////////
    // argument

    __SimpleBuilder(argument)

    ////////// ////////// ////////// ////////// //////////
    // arguments

    __SimpleBuilder(arguments)

    ////////// ////////// ////////// ////////// //////////
    // cvalue

    __Builder(cvalue)

        template<typename value_t>
        void __set_value(token_t * token, value_t && value);

    __EndBuilder

    ////////// ////////// ////////// ////////// //////////
    // _attribute_group

    __SimpleBuilder(_attribute_group)

    ////////// ////////// ////////// ////////// //////////
    // _attribute_group.__item

    __SimpleBuilder(_attribute_group_item)

    ////////// ////////// ////////// ////////// //////////
    // _attribute_group.__assign

    __SimpleBuilder(_attribute_group_assign)

    ////////// ////////// ////////// ////////// //////////
    // _attribute_type_name

    __SimpleBuilder(_attribute_type_name)

    ////////// ////////// ////////// ////////// //////////
    // attributes

    __SimpleBuilder(attributes)

    ////////// ////////// ////////// ////////// //////////
    // _decorate_complex

    __SimpleBuilder(_decorate_complex)

    ////////// ////////// ////////// ////////// //////////
    // statement

    __SimpleBuilder(statement)

    ////////// ////////// ////////// ////////// //////////
    // method_body

    __SimpleBuilder(method_body)

    ////////// ////////// ////////// ////////// //////////
    // property

    __SimpleBuilder(property)

    ////////// ////////// ////////// ////////// //////////
    // event

    __SimpleBuilder(event)

    ////////// ////////// ////////// ////////// //////////
    // _single_expression

    __SimpleBuilder(_single_expression)

    ////////// ////////// ////////// ////////// //////////
    // _function_name_expression

    __SimpleBuilder(_function_name_expression)

    ////////// ////////// ////////// ////////// //////////
    // type_name_exp

    __SimpleBuilder(type_name_exp)

    ////////// ////////// ////////// ////////// //////////
    // type_cast_exp

    __SimpleBuilder(type_cast_exp)

    ////////// ////////// ////////// ////////// //////////
    // function

    __SimpleBuilder(function)

    ////////// ////////// ////////// ////////// //////////
    // index

    __SimpleBuilder(index)

    ////////// ////////// ////////// ////////// //////////
    // new

    __SimpleBuilder(new)

    ////////// ////////// ////////// ////////// //////////
    // new_array

    __SimpleBuilder(new_array)

    ////////// ////////// ////////// ////////// //////////
    // array_initializer

    __SimpleBuilder(array_initializer)

    ////////// ////////// ////////// ////////// //////////
    // array_lengths

    __Builder(array_lengths)

        ast_node_t * __last_exp = nullptr;

    __EndBuilder

    ////////// ////////// ////////// ////////// //////////
    // default_value

    __SimpleBuilder(default_value)

    ////////// ////////// ////////// ////////// //////////
    // statements

    __SimpleBuilder(statements)

    ////////// ////////// ////////// ////////// //////////
    // expresison_st

    __SimpleBuilder(expression_st)

    ////////// ////////// ////////// ////////// //////////
    // type_def_st

    __SimpleBuilder(type_def_st)

    ////////// ////////// ////////// ////////// //////////
    // defination_st

    __SimpleBuilder(defination_st)

    ////////// ////////// ////////// ////////// //////////
    // _defination_st_item

    __SimpleBuilder(_defination_st_item)

    ////////// ////////// ////////// ////////// //////////
    // break_st

    __SimpleBuilder(break_st)

    ////////// ////////// ////////// ////////// //////////
    // continue_st

    __SimpleBuilder(continue_st)

    ////////// ////////// ////////// ////////// //////////
    // throw_st

    __SimpleBuilder(throw_st)

    ////////// ////////// ////////// ////////// //////////
    // goto_st

    __SimpleBuilder(goto_st)

    ////////// ////////// ////////// ////////// //////////
    // return_st

    __SimpleBuilder(return_st)

    ////////// ////////// ////////// ////////// //////////
    // do_while_st

    __SimpleBuilder(do_while_st)

    ////////// ////////// ////////// ////////// //////////
    // while_st

    __SimpleBuilder(while_st)

    ////////// ////////// ////////// ////////// //////////
    // for_st

    __SimpleBuilder(for_st)

    ////////// ////////// ////////// ////////// //////////
    // for_each_st

    __SimpleBuilder(for_each_st)

    ////////// ////////// ////////// ////////// //////////
    // if_st

    __SimpleBuilder(if_st)

    ////////// ////////// ////////// ////////// //////////
    // switch_st

    __SimpleBuilder(switch_st)

    ////////// ////////// ////////// ////////// //////////
    // case

    __SimpleBuilder(case)

    ////////// ////////// ////////// ////////// //////////
    // try_st

    __SimpleBuilder(try_st)

    ////////// ////////// ////////// ////////// //////////
    // catch

    __SimpleBuilder(catch)

    ////////// ////////// ////////// ////////// //////////
    // empty_st

    __SimpleBuilder(empty_st)

    ////////// ////////// ////////// ////////// //////////
    // statement_group_st

    __SimpleBuilder(statement_group_st)

    ////////// ////////// ////////// ////////// //////////
    // _fields

    __SimpleBuilder(_fields)

    ////////// ////////// ////////// ////////// //////////
    // _fields_item

    __SimpleBuilder(_fields_item)

    ////////// ////////// ////////// ////////// //////////
    // _fake_method

    __SimpleBuilder(_fake_method)

    ////////// ////////// ////////// ////////// //////////
    // method

    __SimpleBuilder(method)

    ////////// ////////// ////////// ////////// //////////
    // _enum_fields

    __SimpleBuilder(_enum_fields)

    ////////// ////////// ////////// ////////// //////////
    // _enum_fields_item

    __SimpleBuilder(_enum_fields_item)

    ////////// ////////// ////////// ////////// //////////
    // type

    __SimpleBuilder(type)

    ////////// ////////// ////////// ////////// //////////
    // param

    __SimpleBuilder(param)

    ////////// ////////// ////////// ////////// //////////
    // params

    __SimpleBuilder(params)

    ////////// ////////// ////////// ////////// //////////
    // decorate

    __Builder(decorate)

        template<typename element_t>
        void __set_access(access_value_t access, element_t element);

    __EndBuilder

    ////////// ////////// ////////// ////////// //////////
    // import

    __SimpleBuilder(import)

    ////////// ////////// ////////// ////////// //////////
    // namespace

    __SimpleBuilder(namespace)

    ////////// ////////// ////////// ////////// //////////
    // using

    __SimpleBuilder(using)

    ////////// ////////// ////////// ////////// //////////

    #undef __Builder
    #undef __EndBuilder

    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::lang_cs
