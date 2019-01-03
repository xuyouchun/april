
#include <compile.h>

namespace X_ROOT_NS { namespace modules { namespace compile {

    typedef common_log_code_t __c_t;
    typedef code_element_t    __el_t;

    using namespace core;

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(xbranch_value_t)

        X_C(asts,                       _T("asts"))
        X_C(namespace_,                 _T("namespace"))
        X_C(mname,                      _T("mname"))
        X_C(array_type_name,            _T("array_type_name"))
        X_C(general_type_name,          _T("general_type_name"))
        X_C(type_name_unit,             _T("type_name_unit"))
        X_C(uncertain_type_name,        _T("uncertain_type_name"))
        X_C(type_def,                   _T("type_def"))
        X_C(type_def_param,             _T("type_def_param"))
        X_C(type_def_params,            _T("type_def_params"))
        X_C(type_of,                    _T("type_of"))
        X_C(expressions,                _T("expressions"))
        X_C(argument,                   _T("argument"))
        X_C(arguments,                  _T("arguments"))
        X_C(param,                      _T("param"))
        X_C(params,                     _T("params"))
        X_C(generic_param,              _T("generic_param"))
        X_C(generic_params,             _T("generic_params"))
        X_C(generic_constraint,         _T("generic_constraint"))
        X_C(generic_constraints,        _T("generic_constraints"))
        X_C(generic_arg,                _T("generic_arg"))
        X_C(generic_args,               _T("generic_args"))
        X_C(decorate,                   _T("decorate"))
        X_C(method_body,                _T("method_body"))
        X_C(statements,                 _T("statements"))
        X_C(expression_st,              _T("expression_st"))
        X_C(type_def_st,                _T("type_def_st"))
        X_C(defination_st,              _T("defination_st"))
        X_C(break_st,                   _T("break_st"))
        X_C(continue_st,                _T("continue_st"))
        X_C(throw_st,                   _T("throw_st"))
        X_C(goto_st,                    _T("goto_st"))
        X_C(return_st,                  _T("return_st"))
        X_C(do_while_st,                _T("do_while_st"))
        X_C(while_st,                   _T("while_st"))
        X_C(for_st,                     _T("for_st"))
        X_C(for_each_st,                _T("for_each_st"))
        X_C(if_st,                      _T("if_st"))
        X_C(switch_st,                  _T("switch_st"))
        X_C(case_,                      _T("case_"))
        X_C(try_st,                     _T("try_st"))
        X_C(catch_,                     _T("catch_"))
        X_C(statement_group_st,         _T("statement_group_st"))
        X_C(type_name_exp,              _T("type_name_exp"))
        X_C(type_cast_exp,              _T("type_cast_exp"))
        X_C(function,                   _T("function"))
        X_C(index,                      _T("index"))
        X_C(new_,                       _T("new"))
        X_C(array_initializer,          _T("array_initializer"))
        X_C(array_lengths,              _T("array_lengths"))
        X_C(new_array,                  _T("new_array"))
        X_C(default_value,              _T("default_value"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(ast_error_code_t)

        X_C(mname_empty, _T("mname_empty"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(common_log_code_t)

        X_D(duplicate,  _T("duplicate %1% \"%2%\""))

        X_D(name_empty, _T("%1% name missing"))

        X_D(name_unexpected_first_char,
                _T("unexpected first character \"%1%\" in name \"%2%\""))

        X_D(name_unexpected_char,
                _T("unexpected character \"%1%\" in name \"%2%\""))

        X_D(assembly_name_missing, _T("package name missing"))

        X_D(namespace_name_missing, _T("namespace missing"))

        X_D(type_name_missing,  _T("%1% type name missing"))

        X_D(argument_missing,   _T("argument missing"))

        X_D(condition_missing,  _T("%1% condition missing"))

        X_D(argument_type_error,    _T("%1% argument type must be variable"))

        X_D(unexpected_param_default_value, _T("%1%"))

        X_D(body_missing,  _T("%1% body missing"))

        X_D(iterator_missing, _T("%1% iterator missing"))

        X_D(statement_missing, _T("statement missing"))

        X_D(expression_missing, _T("%1% expression missing"))

        X_D(catch_or_finally_missing, _T("try and finally blocks are both empty"))

        X_D(array_lengths_missing, _T("array '%1%' length missing"))

        X_D(unexpected_params, _T("unexpected %1% params"))

        X_D(function_generic_args_redundance, _T("function generic arguments redundance"))

        X_D(unexpected_variable_defination, _T("variables cannot define here: \"%1%\""))

        X_D(unexpected_field_defination, _T("fields cannot define here: \"%1%\""))

        X_D(unexpected_property_defination, _T("properties cannot define here: \"%1%\""))

        X_D(unexpected_param_defination, _T("parameters cannot define here: \"%1%\""))

        X_D(unexpected_import, _T("unexpected import \"%1%\""))

        X_D(assembly_reference_itself, _T("assembly \"%1%\" reference to itself"))

        X_D(assembly_circular_reference, _T("assembly \"%1%\" circle reference"))

        X_D(assembly_not_found, _T("assembly \"%1%\" not found"))

        X_D(unexpected_using_namespace, _T("unexpected using namespace \"%1%\""))

        X_D(variable_defination_duplicate, _T("variable \"%1%\" duplicate"))

        X_D(variable_undefined, _T("variable \"%1%\" undefined"))

        X_D(type_undefined, _T("type \"%1%\" undefined"))

        X_D(type_def_unsolved, _T("typedef \"%1%\" unsolved"))

        X_D(type_defination_duplicate, _T("type \"%1%\" defination duplicate"))

        X_D(type_def_defination_duplicate, _T("typedef \"%1%\" defination duplicate"))

        X_D(type_undetermind, _T("type undetermind for \"%1%\""))

        X_D(unexpected_symbol, _T("unexpected symbol, \"%1%\""))

        X_D(unexpected_expression, _T("unexpected expression, \"%1%\""))

        X_D(member_not_found, _T("member \"%1%\" undefined in type \"%2%\""))

        X_D(method_not_found, _T("method \"%1%\" undefined in type \"%2%\""))

        X_D(method_not_match, _T("no matching member function for call to \"%1%\" in type \"%2%\""))

        X_D(attribute_constructor_not_match, _T("no matching attribute \"%1%\" constructor: (%2%)"))

        X_D(type_constructor_not_match, _T("no matching type \"%1%\" constructor: (%2%)"))

        X_D(method_conflict, _T("method \"%1%\" conflict in type \"%2%\": %3%"))

        X_D(constructor_method_should_no_return_type,
                                _T("constructor method %1% should no return type"))

        X_D(destructor_method_should_no_return_type,
                                _T("destructor method %1% should no return type"))

        X_D(static_constructor_method_should_no_return_type,
                                _T("static constructor method %1% should no return type"))

        X_D(destructor_method_should_no_params,
                                _T("destructor method %1% should no params"))

        X_D(static_constructor_method_should_no_params,
                                _T("static constructor method %1% should no params"))

        X_D(constructor_method_not_found,
                                _T("constructor method %1%(%2%) not found"))

        X_D(partial_specialization_not_supported,
                                _T("partial specialization of generic type not supported"))

        X_D(no_base_type, _T("type '%1%' has no base type"))

        X_D(expected_array_length, _T("expected array length"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // common functions

    static bool __is_compile_time_attribute(attribute_t * attr)
    {
        general_type_name_t * type_name = as<general_type_name_t *>(attr->type_name);
        return type_name && type_name->equals(JC_COMPILE_ATTRIBUTE_NAME);
    }

    ////////// ////////// ////////// ////////// //////////
    // module

    void __module_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch((walk_step_t)step)
        {
            case walk_step_t::default_:
                __super_t::on_walk(context, step, tag);
                break;

            case walk_step_t::post_confirm: {
                eobject_commit_context_t ctx(context.xpool, context.logger);
                context.xpool.commit_types(context.logger);
                __module->commit(ctx);
            }   break;

            default: break;
        }
    }

    void __module_ast_node_t::do_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::do_walk(context, step, tag);
        context.delay(this, (int)walk_step_t::post_confirm, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // mname

    void mname_ast_node_t::append_part(name_t name, __el_t * el)
    {
        this->__validate_name(name, el);
        __this_mname()->append_part(name);
    }

    void mname_ast_node_t::on_commit()
    {
        __this_mname()->commit(__global_context().xpool.spool);
        const mname_t * mname =  mname_t::to_internal(
            to_mname_operate_context(__global_context().xpool), __this_mname()
        );

        __this_mname()->~mname_t();
        new ((void *)__emname) emname_t(mname);
    }

    emname_t * mname_ast_node_t::to_eobject()
    {
        return __this_emname();
    }

    void mname_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // expressions

    void expressions_ast_node_t::on_commit()
    {
        this->transform_child_to(expressions, __expressions);
    }

    expression_t * expressions_ast_node_t::to_eobject()
    {
        if(__expressions.expression_count() == 1)
        {
            expression_t * exp = __expressions.expression_at(0);
            return exp->parent = nullptr, exp;
        }

        return &__expressions;
    }

    void expressions_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // type_name

    void type_name_ast_node_t::on_commit()
    {
        this->__check_empty(to_eobject(), this, __c_t::type_name_missing, _T(""));
    }

    type_name_t * type_name_ast_node_t::to_eobject()
    {
        return __to_eobject<type_name_t *>(underlying);
    }

    void type_name_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(global_type_t)

        X_C(root,       _T("root"))
        X_C(global,     _T("global"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    type_name_unit_t::operator string_t() const
    {
        if(args == nullptr)
            return _str(name);

        return _F(_T("%1%<%2%>"), _str(name), _str(args));
    }

    X_DEFINE_TO_STRING(general_type_name_t)
    {
        return al::join_str(units.begin(), units.end(), _T("."),
            [](auto it) { return _str(it); }
        );
    }

    X_DEFINE_TO_STRING(array_type_name_t)
    {
        stringstream_t ss;
        ss << _str(element_type_name) << _T("[");

        int index = 0;
        for(expression_t * exp : dimensions)
        {
            if(index++ > 0)
                ss << _T(",");

            if(exp != 0)
                ss << _str(exp);
        }

        ss << _T("]");

        return ss.str();
    }

    ////////// ////////// ////////// ////////// //////////
    // type_name_unit_t

    template<typename args_t>
    bool __is_partial_specialization(const args_t & args)
    {
        enum { unknown, yes, no } is_empty = unknown;

        if(args.size() <= 1)
            return false;

        for(generic_arg_t * arg : args)
        {
            if(is_empty == unknown)
            {
                is_empty = (arg? yes : no);
            }
            else if((arg? yes : no) != is_empty)
            {
                return true;
            }
        }

        return false;
    }

    bool type_name_unit_t::is_partial_specialization() const
    {
        return args != nullptr && __is_partial_specialization(*args);
    }

    ////////// ////////// ////////// ////////// //////////
    // general_type_name

    void general_type_name_ast_node_t::set_global_type(global_type_t global_type)
    {
        __type_name.global_type = global_type;
    }

    void general_type_name_ast_node_t::on_commit()
    {
        this->transform_child_to(units, __type_name.units);
    }

    type_name_t * general_type_name_ast_node_t::to_eobject()
    {
        return &__type_name;
    }

    void general_type_name_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch((walk_step_t)step)
        {
            case walk_step_t::default_:
                __super_t::on_walk(context, step, tag);
                this->__delay(context, walk_step_t::confirm);
                break;

            case walk_step_t::confirm:
                __walk_confirm(context);
                break;

            default: break;
        }
    }

    void general_type_name_ast_node_t::__walk_confirm(ast_walk_context_t & context)
    {
        if(__type_name.type == nullptr)
        {
            __type_name.type = __ascertain_type(context, &__type_name);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // type_name_unit

    void type_name_unit_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__type_name_unit.name, name, el, _T("type"));
    }

    void type_name_unit_ast_node_t::on_commit()
    {
        __type_name_unit.args = __to_eobject<generic_args_t *>(generic_args);
    }

    type_name_unit_t * type_name_unit_ast_node_t::to_eobject()
    {
        return &__type_name_unit;
    }

    void type_name_unit_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__type_name_unit.name, this, __c_t::name_empty, _T("type name unit"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // array_type_name

    void array_type_name_ast_node_t::on_commit()
    {
        this->transform_child_to(dimensions, __type_name.dimensions);

        __type_name.element_type_name = __to_eobject<type_name_t *>(element_type_name);
    }

    type_name_t * array_type_name_ast_node_t::to_eobject()
    {
        return &__type_name;
    }

    void array_type_name_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch((walk_step_t)step)
        {
            case walk_step_t::default_:
                this->__check_empty(__type_name.element_type_name, this,
                                    __c_t::type_name_missing, _T("array"));
                __super_t::on_walk(context, step, tag);
                this->__delay(context, walk_step_t::confirm);
                break;

            case walk_step_t::confirm:
                __walk_confirm(context);
                break;

            default: break;
        }
    }

    void array_type_name_ast_node_t::__walk_confirm(ast_walk_context_t & context)
    {
        array_type_name_t * type_name = &__type_name;

        if(type_name->element_type_name != nullptr && type_name->type == nullptr)
        {
            type_name->type = __ascertain_type(context, type_name);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // uncertain_type_name

    void uncertain_type_name_ast_node_t::on_commit() { }

    type_name_t * uncertain_type_name_ast_node_t::to_eobject()
    {
        return __type_name;
    }

    void uncertain_type_name_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        _P(_T("uncertain"));
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // type_def_param

    void type_def_param_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__type_def_param.name, name, el, _T("typedef param"));
    }

    void type_def_param_ast_node_t::on_commit()
    {

    }

    type_def_param_t * type_def_param_ast_node_t::to_eobject()
    {
        return &__type_def_param;
    }

    void type_def_param_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__type_def_param.name, _T("typedef param"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // type_def_params

    void type_def_params_ast_node_t::on_commit()
    {
        this->transform_child_to(params, __type_def_params);
    }

    type_def_params_t * type_def_params_ast_node_t::to_eobject()
    {
        return &__type_def_params;
    }

    void type_def_params_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // type_def

    void type_def_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__type_def.name, name, el, _T("typedef"));
    }

    void type_def_ast_node_t::on_commit()
    {
        __type_def.type_name  = __to_eobject<type_name_t *>(type_name);
        __type_def.params     = __to_eobject<type_def_params_t *>(params);
    }

    type_def_t * type_def_ast_node_t::to_eobject()
    {
        return &__type_def;
    }

    void type_def_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        context.push(to_eobject());

        __type_def.namespace_ = context.current_namespace();

        this->__check_empty(__type_def.name, this, __c_t::type_name_missing, _T("typedef"));

        const mname_t * ns = ns_to_full_name(context.current_namespace());

        type_def_t * type_def = to_eobject();
        if(!context.assembly.types.append_type_def(to_sid(ns), type_def))
            this->__log(this, __c_t::type_def_defination_duplicate, type_def);

        __super_t::on_walk(context, step, tag);
        context.pop();
    }

    ////////// ////////// ////////// ////////// //////////
    // type_of

    void type_of_ast_node_t::on_commit()
    {
        __type_of.type_name = __to_eobject<type_name_t *>(type_name);
    }

    expression_t * type_of_ast_node_t::to_eobject()
    {
        return &__type_of;
    }

    void type_of_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__type_of.type_name, this, __c_t::type_name_missing, _T("typeof"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // argument

    void argument_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__argument.name, name, el, _T("argument"));
    }

    void argument_ast_node_t::set_argument_type(param_type_t atype, __el_t * el)
    {
        this->__assign(__argument.atype, atype, el, _T("argument type"));
    }

    void argument_ast_node_t::on_commit()
    {
        __argument.expression = __to_eobject<expression_t *>(expression);
        __argument.attributes = __to_eobject_with_owner<attributes_t *>(attributes, &__argument);
    }

    argument_t * argument_ast_node_t::to_eobject()
    {
        return &__argument;
    }

    void argument_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);

        this->__check_empty(__argument.expression, this, __c_t::argument_missing);

        typedef param_type_t t;
        if(__argument.atype == t::ref || __argument.atype == t::out)
        {
            if(__argument.expression != nullptr
                    && __argument.expression->this_family() != expression_family_t::name)
            {
                this->__log(this, __c_t::argument_type_error, _title(__argument.atype));
            }
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // arguments

    void arguments_ast_node_t::on_commit()
    {
        this->transform_child_to(arguments, __arguments);
    }

    void arguments_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    arguments_t * arguments_ast_node_t::to_eobject()
    {
        return &__arguments;
    }

    ////////// ////////// ////////// ////////// //////////
    // cvalue

    void cvalue_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    cvalue_t * cvalue_ast_node_t::to_eobject()
    {
        return &__value;
    }

    void cvalue_ast_node_t::set_value(const cvalue_t & value)
    {
        __value = value;
    }

    void cvalue_ast_node_t::set_value(const token_data_t & data)
    {
        switch(data.value_type)
        {
            case cvalue_type_t::number:
                set_value(cvalue_t(*(tvalue_t *)data.value));
                break;

            case cvalue_type_t::string:
                set_value(cvalue_t((const char_t *)data.value));
                break;

            case cvalue_type_t::null:
                set_value(cvalue_t::null);
                break;
            
            default:
                break;
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // decorate

    template<typename getf_t, typename setf_t>
    struct __bit_field_wrap_t
    {
        __bit_field_wrap_t(getf_t getf, setf_t setf) : __getf(getf), __setf(setf) { }

        getf_t __getf;
        setf_t __setf;

        typedef decltype(__getf()) __value_t;

        operator __value_t() const { return __getf(); }
        __value_t operator = (const __value_t & value) { __setf(value); return value; }
        bool operator == (const __value_t & value) const { return (__value_t)*this == value; }
        bool operator != (const __value_t & value) const { return !operator == (value); }
    };

    template<typename getf_t, typename setf_t>
    auto __bit_field_wrap(getf_t getf, setf_t setf)
    {
        return __bit_field_wrap_t<getf_t, setf_t>(getf, setf);
    }

    #define __BitField(obj, field)                              \
        __bit_field_wrap(                                       \
                [this]() { return obj.field; },                 \
                [this](auto value) { obj.field = value; }       \
        )

    void decorate_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    void decorate_ast_node_t::set_access(access_value_t value, __el_t * el)
    {
        this->__assign(__BitField(__decorate, access), value, el, _T("access"));
    }

    void decorate_ast_node_t::set_static(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_static), true, el, _T("static"));
    }

    void decorate_ast_node_t::set_sealed(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_sealed), true, el, _T("sealed"));
    }

    void decorate_ast_node_t::set_override(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_override), true, el, _T("override"));
    }

    void decorate_ast_node_t::set_virtual(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_virtual), true, el, _T("virtual"));
    }

    void decorate_ast_node_t::set_abstract(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_abstract), true, el, _T("abstract"));
    }

    void decorate_ast_node_t::set_new(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_new), true, el, _T("new"));
    }

    void decorate_ast_node_t::set_readonly(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_readonly), true, el, _T("readonly"));
    }

    void decorate_ast_node_t::set_const(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_const), true, el, _T("const"));
    }

    void decorate_ast_node_t::set_extern(__el_t * el)
    {
        this->__assign(__BitField(__decorate, is_extern), true, el, _T("extern"));
    }

    void decorate_ast_node_t::on_commit()
    {

    }

    decorate_t * decorate_ast_node_t::to_eobject()
    {
        return &__decorate;
    }

    ////////// ////////// ////////// ////////// //////////
    // statements

    void statements_ast_node_t::on_commit()
    {
        this->transform_child_to(statements, __statements);
    }

    statements_t * statements_ast_node_t::to_eobject()
    {
        return &__statements;
    }

    void statements_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // method_body

    void method_body_ast_node_t::on_commit()
    {
        this->transform_child_to(statements, __body.statements);
    }

    method_body_t * method_body_ast_node_t::to_eobject()
    {
        return &__body;
    }

    void method_body_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // expression

    void expression_ast_node_t::append_token(token_t * token)
    {
        _A(token != nullptr);

        __es.push_back(__el_t(token));
    }

    void expression_ast_node_t::append_name(name_t name, token_t * token)
    {
        __es.push_back(__el_t(token, name));
    }

    void expression_ast_node_t::append_expression(ast_node_t * node)
    {
        _A(node != nullptr);

        expression_t * expression = _M(expression_ast_t *, node)->to_eobject();
        __es.push_back(__el_t(expression));

        this->append_child(nodes, node);
    }

    void expression_ast_node_t::append_cvalue(ast_node_t * node)
    {
        _A(node != nullptr);

        cvalue_t * value = _M(cvalue_ast_t *, node)->to_eobject();
        __es.push_back(__el_t(to_sys_exp(__get_memory(), __get_lang(), nullptr, value)));

        this->append_child(nodes, node);
    }

    void expression_ast_node_t::on_commit()
    {
        __expression = __parse_expression(__es);
    }

    expression_t * expression_ast_node_t::to_eobject()
    {
        return __expression;
    }

    void expression_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch((walk_step_t)step)
        {
            case walk_step_t::default_:
                if(is_top_expression(__expression))
                    this->__delay(context, walk_step_t::analysis);
                __super_t::on_walk(context, step, tag);
                break;

            case walk_step_t::analysis: {
                expression_t * exp = this->to_eobject();
                if(!exp->walked)
                    walk_expression(this->__context, context, exp);
            }   break;

            default: break;
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // statement

    void statement_ast_node_t::on_commit()
    {
        if(this->child_at(underlying) == nullptr)
            this->__log(this, __c_t::statement_missing);
    }

    statement_t * statement_ast_node_t::to_eobject()
    {
        return __to_eobject<statement_t *>(underlying);
    }

    void statement_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // import

    void import_ast_node_t::set_alias(name_t alias, __el_t * el)
    {
        this->__assign_name(__import.alias, alias, el, _T("import alias"));
    }

    void import_ast_node_t::on_commit()
    {
        __import.package  = __to_eobject<emname_t *>(package);
        __import.assembly = __to_eobject<emname_t *>(assembly);
    }

    import_t * import_ast_node_t::to_eobject()
    {
        return &__import;
    }

    void import_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        import_t * import = to_eobject();

        if(!__import.assembly)
        {
            this->__log(this, __c_t::assembly_name_missing);
        }
        else
        {
            document_t * document = context.current_document();
            if(document == nullptr)
            {
                this->__log(this, __c_t::unexpected_import, _str(import));
            }
            else
            {
                #define __LogAssembly(code) \
                    this->__log(this, __c_t::code, _str(import->assembly))
                    
                if(!import->package && context.assembly.name == to_mname(import->assembly))
                {
                    __LogAssembly(assembly_reference_itself);
                }
                else
                {
                    document->add_import(import);

                    try
                    {
                        assembly_t * assembly = context.load_assembly(
                            to_mname(import->package), to_mname(import->assembly)
                        );

                        _A(assembly != nullptr);
                        context.assembly.references.add_reference(
                            to_mname(import->package), assembly
                        );
                    }
                    catch(const logic_error_t<compile_error_code_t> & e)
                    {
                        switch(e.code)
                        {
                            case compile_error_code_t::assembly_circular_reference:
                                __LogAssembly(assembly_circular_reference);
                                break;

                            case compile_error_code_t::assembly_not_found:
                                __LogAssembly(assembly_not_found);
                                break;

                            default:
                                break;
                        }
                    }
                }

                #undef __LogAssembly
            }
        }

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // using

    void using_ast_node_t::set_alias(name_t name, __el_t * el)
    {
        this->__validate_name(name, el);
        __using_namespace.alias = name;
    }

    void using_ast_node_t::on_commit()
    {
        __using_namespace.ns = __to_eobject<emname_t *>(ns);
    }

    using_namespace_t * using_ast_node_t::to_eobject()
    {
        return &__using_namespace;
    }

    void using_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if(!__using_namespace.ns)
        {
            this->__log(this, __c_t::namespace_name_missing);
        }
        else
        {
            document_t * document = context.current_document();
            if(document == nullptr)
            {
                this->__log(this, __c_t::unexpected_using_namespace, _str(this->to_eobject()));
            }
            else
            {
                document->add_using(this->to_eobject());
            }
        }

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // namespace

    void namespace_ast_node_t::on_commit()
    {
        __namespace.name = __to_eobject<emname_t *>(name);

        this->transform_child_to(types,      __namespace.types);
        this->transform_child_to(namespaces, __namespace.namespaces);
        this->transform_child_to(type_defs,  __namespace.type_defs);
    }

    namespace_t * namespace_ast_node_t::to_eobject()
    {
        return &__namespace;
    }

    void namespace_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        context.push(this->to_eobject());
        __super_t::on_walk(context, step, tag);
        context.pop();
    }

    ////////// ////////// ////////// ////////// //////////

    class __method_utils_t : public __utils_t
    {
        typedef __utils_t __super_t;

    public:
        __method_utils_t(ast_context_t & ctx, ast_walk_context_t & wctx)
            : __super_t(ctx), __wctx(wctx)
        { }

        void revise_constructor(method_t & method, arguments_t * args, __el_t * node = nullptr)
        {
            method_t * base_ctor = __get_base_contructor(args, node);
            al::small_vector_t<statement_t *, 16> statements;

            // super ctor
            if(base_ctor != nullptr)
            {
                statements.push_back(__new_expression_statement<function_expression_t>(
                    base_ctor, args
                ));
            }

            // fields initialize
            type_t * type = __wctx.current_type();
            _A(type != nullptr);

            analyze_members_args_t members_args(member_type_t::field, name_t::null);
            type->get_members(members_args);

            for(member_t * member : members_args.out_members)
            {
                field_t * field = (field_t *)member;
                expression_t * init_exp = field->init_value;
                if(init_exp == nullptr)
                    init_exp = __new_system_expression<default_value_expression_t>(field->type_name);

                statements.push_back(__new_expression_statement<binary_expression_t>(
                    operator_t::assign, __new_field_expression(field), init_exp
                ));
            }

            if(statements.size() > 0)
            {
                if(method.body == nullptr)
                    method.body = __new_obj<method_body_t>();

                method.body->push_front(statements);
            }
        }

    private:
        ast_walk_context_t & __wctx;

        method_t * __get_base_contructor(arguments_t * args, __el_t * node)
        {
            type_t * type = __wctx.current_type();
            _A(type != nullptr);

            type_t * base_type = type->get_base_type();

            if(base_type == nullptr)
            {
                if(!is_empty(args))
                    this->__log(node, __c_t::constructor_method_not_found, _T(">"), args);

                return nullptr;
            }

            method_t * ctor = find_constructor(this->__context, base_type, args);
            if(ctor == nullptr)
                this->__log(node, __c_t::constructor_method_not_found, base_type, args);

            return ctor;
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // type

    void type_ast_node_t::set_name(const name_t & name, __el_t * el)
    {
        this->__assign_name(__type.name, name, el, _T("type"));
    }

    void type_ast_node_t::set_ttype(ttype_t ttype, __el_t * el)
    {
        __type.ttype = ttype;
    }

    void type_ast_node_t::append_member(member_t * member)
    {
        __type.append_member(member);
    }

    void type_ast_node_t::on_commit()
    {
        __type.params     = __to_eobject<generic_params_t *>(generic_params);
        __type.decorate   = __to_eobject<decorate_t *>(decorate);
        __type.attributes = __to_eobject_with_owner<attributes_t *>(attributes, &__type);

        this->transform_child_to(methods,           __type.methods);
        this->transform_child_to(properties,        __type.properties);
        this->transform_child_to(fields,            __type.fields);
        this->transform_child_to(events,            __type.events);
        this->transform_child_to(nest_types,        __type.nest_types);
        this->transform_child_to(type_defs,         __type.type_defs);
        this->transform_child_to(super_type_names,  __type.super_type_names);
    }

    type_t * type_ast_node_t::to_eobject()
    {
        return &__type;
    }

    void type_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch((walk_step_t)step)
        {
            case walk_step_t::default_:
                context.push(this->to_eobject());
                __walk_default(context, step, tag);
                context.pop();
                break;

            case walk_step_t::analysis:
                __walk_analysis(context, (method_t *)tag);
                break;

            default: break;
        }
    }

    void type_ast_node_t::__walk_default(ast_walk_context_t & context, int step, void * tag)
    {
        general_type_t * type = (general_type_t *)to_eobject();
        mname_operate_context_t mctx = to_mname_operate_context(__global_context().xpool);
        type->namespace_ = context.current_namespace();

        if(!context.assembly.types.append_type(type))
            this->__log(this, __c_t::type_defination_duplicate, type);

        context.xpool.append_new_type(type);

        __super_t::on_walk(context, step, tag);

        bool has_constructor = al::any_of(__type.methods, [](method_t * m) {
            return m->trait == method_trait_t::constructor;
        });

        if(!has_constructor && al::in(type->this_gtype(), gtype_t::general, gtype_t::generic))
        {
            method_t * method = __append_default_constructor();
            this->__delay(context, walk_step_t::analysis, method);
        }
    }

    void type_ast_node_t::__walk_analysis(ast_walk_context_t & context, method_t * method)
    {
        __method_utils_t(__context, context).revise_constructor(*method, nullptr);
    }

    method_t * type_ast_node_t::__append_default_constructor()
    {
        method_t * method = __new_obj<method_t>();

        method->name     = __type.name;
        method->decorate = __new_obj<decorate_t>();
        method->decorate->access = access_value_t::public_;
        method->trait    = method_trait_t::constructor;

        __type.methods.push_back(method);

        return method;
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_param

    void generic_param_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__param.name, name, el, _T("template param"));
    }

    void generic_param_ast_node_t::on_commit()
    {
        __param.attributes = __to_eobject_with_owner<attributes_t *>(attributes, &__param);
    }

    generic_param_t * generic_param_ast_node_t::to_eobject()
    {
        return &__param;
    }

    void generic_param_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__param.name, _T("template param"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_params

    void generic_params_ast_node_t::on_commit()
    {
        this->transform_child_to(params, __generic_params);
    }

    generic_params_t * generic_params_ast_node_t::to_eobject()
    {
        return &__generic_params;
    }

    void generic_params_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_constraint

    void generic_constraint_ast_node_t::set_constraint(generic_constraint_ttype_t cttype,
                                                            __el_t * el)
    {
        this->__assign(__generic_constraint.cttype, cttype, el, _T("constraint type"), true);
    }

    void generic_constraint_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__generic_constraint.param_name, name, el, _T("generic param"));
    }

    void generic_constraint_ast_node_t::on_commit()
    {
        this->transform_child_to(type_names, __generic_constraint.type_names);
    }

    generic_constraint_t * generic_constraint_ast_node_t::to_eobject()
    {
        return &__generic_constraint;
    }

    void generic_constraint_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_constraints

    void generic_constraints_ast_node_t::on_commit()
    {
        this->transform_child_to(constraints, __generic_constraints);
    }

    generic_constraints_t * generic_constraints_ast_node_t::to_eobject()
    {
        return &__generic_constraints;
    }

    void generic_constraints_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_arg

    void generic_arg_ast_node_t::on_commit()
    {
        __arg.type_name = __to_eobject<type_name_t *>(type_name);
    }

    generic_arg_t * generic_arg_ast_node_t::to_eobject()
    {
        return &__arg;
    }

    void generic_arg_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__arg.type_name, this, __c_t::type_name_missing, _T("generic argument"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_args

    void generic_args_ast_node_t::on_commit()
    {
        this->transform_child_to(generic_args, __args);
    }

    generic_args_t * generic_args_ast_node_t::to_eobject()
    {
        return &__args;
    }

    void generic_args_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if(__args.empty())
            this->__log(this, __c_t::type_name_missing, _T("generic arguments"));

        if(__is_partial_specialization(__args))
            this->__log(this, __c_t::partial_specialization_not_supported);

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // param

    void param_ast_node_t::set_name(const name_t & name, __el_t * el)
    {
        this->__assign_name(__param.name, name, el, _T("param"));
    }

    void param_ast_node_t::set_param_type(param_type_t ptype, __el_t * el)
    {
        this->__assign(__param.ptype, ptype, el, _T("param type"));
    }

    void param_ast_node_t::on_commit()
    {
        __param.type_name     = __to_eobject<type_name_t *>(type_name);
        __param.default_value = __to_eobject<expression_t *>(default_value);
        __param.attributes    = __to_eobject_with_owner<attributes_t *>(attributes, &__param);
    }

    param_t * param_ast_node_t::to_eobject()
    {
        return &__param;
    }

    void param_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch((walk_step_t)step)
        {
            case walk_step_t::default_:
                this->__check_name_empty(__param.name, _T("param"));
                this->__check_empty(__param.type_name, this, __c_t::type_name_missing, _T("param"));

                this->__delay(context, walk_step_t::pre_analysis);
                __super_t::on_walk(context, step, tag);
                break;

            case walk_step_t::pre_analysis:
                if(__param.default_value != nullptr)
                {
                    expression_t * exp = __param.default_value;

                    try
                    {
                        expression_execute_context_t ctx(this->__get_xpool());
                        cvalue_t cv = exp->execute(ctx);

                        if(is_nan(cv))
                        {
                            this->__log(exp, __c_t::unexpected_param_default_value,
                                                _T("unexpected constant expression"));
                        }
                    }
                    catch(logic_error_t<expression_execute_error_code_t> & e)
                    {
                        this->__log(exp, __c_t::unexpected_param_default_value, e.get_message());
                    }
                }

                break;

            default: break;
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // params

    void params_ast_node_t::on_commit()
    {
        this->transform_child_to(params, __params);
    }

    params_t * params_ast_node_t::to_eobject()
    {
        return &__params;
    }

    void params_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // attribute

    void attribute_ast_node_t::on_commit()
    {
        __attr.type_name = __to_eobject<type_name_t *>(type_name);
        __attr.arguments = __to_eobject<arguments_t *>(arguments);
    }

    attribute_t * attribute_ast_node_t::to_eobject()
    {
        if(__is_compile_time_attribute(&__attr))
            return __get_compile_time_attribute();

        return &__attr;
    }

    void attribute_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch((walk_step_t)step)
        {
            case walk_step_t::default_:
                context.current_module()->register_commit(this->to_eobject());
                this->__delay(context, walk_step_t::post_confirm);
                __super_t::on_walk(context, step, tag);
                break;

            case walk_step_t::post_confirm:
                if(__is_compile_time_attribute(&__attr))
                    __get_compile_time_attribute()->type_name = __attr.type_name;

                this->__delay(context, walk_step_t::analysis);
                break;

            case walk_step_t::analysis:
                __analyze_attribute_constructor();
                break;

            default: break;
        }
    }

    attribute_t * attribute_ast_node_t::__get_compile_time_attribute()
    {
        return __get_xpool().get_compile_time_attribute();
    }

    void attribute_ast_node_t::__analyze_attribute_constructor()
    {
        type_name_t * type_name = __attr.type_name;
        type_t * type;

        if(type_name == nullptr || (type = type_name->type) == nullptr)
            return;

        method_t * method = find_constructor(this->__context, type, __attr.arguments);
        if(method == nullptr)
        {
            __log(&__attr, __c_t::attribute_constructor_not_match, type,
                to_arg_types_str(this->__context, __attr.arguments)
            );

            return;
        }

        if(__attr.arguments != nullptr)
        {
            expression_execute_context_t ctx(__get_xpool());
            for(argument_t * argument : *__attr.arguments)
            {
                expression_t * exp = argument->expression;
                if(exp != nullptr)
                {
                    try
                    {
                        exp->execute(ctx);
                    }
                    catch(const error_t & e)
                    {
                        _PP(e);
                    }
                }
            }
        }

        this->to_eobject()->constructor = method;
    }

    ////////// ////////// ////////// ////////// //////////
    // attributes

    void attributes_ast_node_t::on_commit()
    {
        this->transform_child_to(attributes, __attributes);
    }

    attributes_t * attributes_ast_node_t::to_eobject()
    {
        return &__attributes;
    }

    void attributes_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // if

    void if_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // for

    void for_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // while

    void while_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // switch

    void switch_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // loop

    void loop_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // break

    void break_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // continue

    void continue_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // return

    void return_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // expression_st

    void expression_st_ast_node_t::on_commit()
    {
        __statement.expression = __to_eobject<expression_t *>(expression);
    }

    statement_t * expression_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    void expression_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__statement.expression, this, __c_t::expression_missing,
                                                          _T("statement"));
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // type_def_st

    void type_def_st_ast_node_t::on_commit()
    {
        __statement.type_def = __to_eobject<type_def_t *>(type_def);
    }

    statement_t * type_def_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    void type_def_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__statement.type_def, this, __c_t::statement_missing);

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // defination_st

    void defination_st_ast_node_t::on_commit()
    {
        __statement.type_name = __to_eobject<type_name_t *>(type_name);
        this->transform_child_to(items, __statement);
    }

    statement_t * defination_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    void defination_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__statement.type_name, this, __c_t::type_name_missing,
                                                        _T("variable defination"));

        variable_region_t * region = context.current_region();
        if(region == nullptr)
        {
            this->__log(&__statement, __c_t::unexpected_variable_defination,
                                                    (string_t)__statement);
        }
        else
        {
            for(defination_statement_item_t * var_item : __statement.items)
            {
                try
                {
                    var_item->variable = region->define_local(
                        __statement.type_name, var_item->name
                    );
                }
                catch(const logic_error_t<ast_error_t> & e)
                {
                    if(e.code == ast_error_t::variable_duplicated)
                    {
                        this->__log(var_item, __c_t::variable_defination_duplicate,
                                                        (string_t)var_item->name);
                    }
                }
            }
        }

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // break_st

    statement_t * break_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    void break_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // continue_st

    statement_t * continue_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    void continue_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // throw_st

    void throw_st_ast_node_t::on_commit()
    {
        __statement.expression = __to_eobject<expression_t *>(expression);
    }

    statement_t * throw_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    void throw_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // goto_st

    void goto_st_ast_node_t::set_label(const name_t & label, __el_t * el)
    {
        this->__assign_name(__statement.label, label, el, _T("label"));
    }

    void goto_st_ast_node_t::on_commit()
    {

    }

    statement_t * goto_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    void goto_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__statement.label, _T("label"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // return_st

    void return_st_ast_node_t::on_commit()
    {
        __statement.expression = __to_eobject<expression_t *>(expression);
    }

    statement_t * return_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    void return_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // do_while_st

    void do_while_st_ast_node_t::on_commit()
    {
        __statement.condition = __to_eobject<expression_t *>(condition);
        __statement.body      = __to_eobject<statement_t *>(body);
    }

    statement_t * do_while_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    void do_while_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__statement.condition, this, __c_t::condition_missing, _T("do while"));
        this->__check_empty(__statement.body, this, __c_t::body_missing, _T("do while"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // while_st

    void while_st_ast_node_t::on_commit()
    {
        __statement.condition = __to_eobject<expression_t *>(condition);
        __statement.body      = __to_eobject<statement_t *>(body);
    }

    statement_t * while_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    void while_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__statement.condition, this, __c_t::condition_missing, _T("while"));
        this->__check_empty(__statement.body, this, __c_t::body_missing, _T("while"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // for_st

    void for_st_ast_node_t::on_commit()
    {
        auto init_statement = __try_to_eobject<statement_t *>(initialize);
        if(init_statement != nullptr)
            __statement.defination_initialize = _M(defination_statement_t *, init_statement);
        else
            __statement.initialize = __to_eobject<expression_t *>(initialize);

        __statement.condition  = __to_eobject<expression_t *>(condition);
        __statement.increase   = __to_eobject<expression_t *>(increase);
        __statement.body       = __to_eobject<statement_t *>(body);
    }

    statement_t * for_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    void for_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__statement.body, this, __c_t::body_missing, _T("while"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // for_each_st

    void for_each_st_ast_node_t::set_variable(name_t name, __el_t * el)
    {
        this->__assign_name(__statement.variable, name, el, _T("var"));
    }

    void for_each_st_ast_node_t::on_commit()
    {
        __statement.iterator   = __to_eobject<expression_t *>(iterator);
        __statement.type_name  = __to_eobject<type_name_t *>(type_name);
        __statement.body       = __to_eobject<statement_t *>(body);
    }

    statement_t * for_each_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    void for_each_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__statement.variable, _T("variable"));
        this->__check_empty(__statement.iterator, this, __c_t::iterator_missing, "for each");
        this->__check_empty(__statement.body, this, __c_t::body_missing, _T("for each"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // if_st

    void if_st_ast_node_t::on_commit()
    {
        __statement.condition   = __to_eobject<expression_t *>(condition);
        __statement.if_body     = __to_eobject<statement_t *>(if_body);
        __statement.else_body   = __to_eobject<statement_t *>(else_body);

        this->__check_empty(__statement.condition, this, __c_t::condition_missing, "if");
        this->__check_empty(__statement.if_body, this, __c_t::body_missing, _T("if"));
    }

    statement_t * if_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    void if_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // switch_st

    void switch_st_ast_node_t::on_commit()
    {
        __statement.expression = __to_eobject<expression_t *>(expression);
        this->transform_child_to(cases, __statement.cases);
    }

    statement_t * switch_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    void switch_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__statement.expression, this, __c_t::expression_missing, _T("switch"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // case

    void case_ast_node_t::append_default(__el_t * el)
    {
        auto * exp_node = to_fake_ast<(__value_t)__cvalue_t::expression>(
            (expression_t *)nullptr, __get_memory(), el->code_unit
        );

        this->append_child(constants, exp_node);
    }

    void case_ast_node_t::on_commit()
    {
        this->transform_child_to(constants, __case.constants);
        __case.statements = __to_eobject<statements_t *>(statements);
    }

    case_t * case_ast_node_t::to_eobject()
    {
        return &__case;
    }

    void case_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__case.constants, this, __c_t::expression_missing, _T("case"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // try_st

    void try_st_ast_node_t::on_commit()
    {
        __statement.try_statement     = __to_eobject<statement_t *>(try_);
        __statement.finally_statement = __to_eobject<statement_t *>(finally);

        this->transform_child_to(catches, __statement.catches);
    }

    statement_t * try_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    void try_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {

        if(__statement.catches.empty() && __statement.finally_statement == nullptr)
        {
            this->__log(this, __c_t::catch_or_finally_missing);
        }

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // catch

    void catch_ast_node_t::set_variable(name_t variable, __el_t * el)
    {
        this->__assign_name(__catch.variable, variable, el, _T("variable"));
        __variable_el = el;
    }

    void catch_ast_node_t::on_commit()
    {
        __catch.type_name = __to_eobject<type_name_t *>(type_name);
        __catch.body      = __to_eobject<statement_t *>(body);
    }

    catch_t * catch_ast_node_t::to_eobject()
    {
        return &__catch;
    }

    void catch_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if(__catch.type_name == nullptr && !__catch.variable.empty())
        {
            this->__log(__variable_el, __c_t::type_name_missing, _T("catch statement"));
        }

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // empty_st

    statement_t * empty_st_ast_node_t::to_eobject()
    {
        return &__statement;
    }

    void empty_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // statement_group_st

    void statement_group_st_ast_node_t::on_commit()
    {
        this->transform_child_to(statements, __statement_group);
    }

    statement_t * statement_group_st_ast_node_t::to_eobject()
    {
        return &__statement_group;
    }

    void statement_group_st_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        context.push_new_region();
        __super_t::on_walk(context, step, tag);
        context.pop();
    }

    ////////// ////////// ////////// ////////// //////////
    // type_name_exp

    void type_name_exp_ast_node_t::on_commit()
    {
        __expression.type_name = __to_eobject<type_name_t *>(type_name);
    }

    expression_t * type_name_exp_ast_node_t::to_eobject()
    {
        return &__expression;
    }

    void type_name_exp_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // type_cast_exp

    void type_cast_exp_ast_node_t::on_commit()
    {
        __expression.type_name  = __to_eobject<type_name_t *>(type_name);
        __expression.expression = __to_eobject<expression_t *>(expression);
    }

    expression_t * type_cast_exp_ast_node_t::to_eobject()
    {
        return &__expression;
    }

    void type_cast_exp_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // function

    void function_ast_node_t::on_commit()
    {
        __expression.namex         = __to_eobject<expression_t *>(namex);
        __expression.generic_args  = __to_eobject<generic_args_t *>(generic_args);

        __expression.set_arguments(__to_eobject<arguments_t *>(arguments));
    }

    expression_t * function_ast_node_t::to_eobject()
    {
        return &__expression;
    }

    void function_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__expression.namex, _T("function"));

        if(__expression.generic_args != nullptr && __expression.get_name() == name_t::null)
        {
            this->__log(this->child_at(generic_args), __c_t::function_generic_args_redundance);
        }

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // index

    void index_ast_node_t::on_commit()
    {
        __expression.namex     = __to_eobject<expression_t *>(namex);

        __expression.set_arguments(__to_eobject<arguments_t *>(arguments));
    }

    expression_t * index_ast_node_t::to_eobject()
    {
        return &__expression;
    }

    void index_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__expression.namex, _T("index"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // new

    void new_ast_node_t::on_commit()
    {
        __expression.type_name = __to_eobject<type_name_t *>(type_name);
        __expression.set_arguments(__to_eobject<arguments_t *>(arguments));
    }

    expression_t * new_ast_node_t::to_eobject()
    {
        return &__expression;
    }

    void new_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__expression.type_name, this, __c_t::type_name_missing,
                                                            _T("object creation"));
        __super_t::on_walk(context, step, tag);

        switch((walk_step_t)step)
        {
            case walk_step_t::default_:
                this->__delay(context, walk_step_t::analysis);
                __super_t::on_walk(context, step, tag);
                break;

            case walk_step_t::analysis: {
                type_t * type = to_type(__expression.type_name);
                if(type != nullptr)
                {
                    __expression.constructor = find_constructor(
                        this->__context, type, __expression.arguments()
                    );

                    if(__expression.constructor == nullptr)
                    {
                        __log(&__expression, __c_t::type_constructor_not_match, type,
                            to_arg_types_str(this->__context, __expression.arguments())
                        );
                    }
                }

            }   break;

            default: break;
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // new_array

    void new_array_ast_node_t::on_commit()
    {
        __expression.type_name = __to_eobject<type_name_t *>(type_name);
        __expression.set_lengths(__to_eobject<array_lengths_t *>(lengths));
        __expression.initializer = __to_eobject<array_initializer_t *>(initializer);
    }

    expression_t * new_array_ast_node_t::to_eobject()
    {
        return &__expression;
    }

    const array_length_t unkown_array_length = -1;

    void new_array_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__expression.type_name, this, __c_t::type_name_missing,
                                                            _T("array creation"));
        this->__check_empty(__expression.lengths(), this, __c_t::array_lengths_missing);

        __super_t::on_walk(context, step, tag);

        // dimension
        array_lengths_t * p_lengths = this->__to_eobject<array_lengths_t *>(lengths);
        if(p_lengths != nullptr)
        {
            array_lengths_t & lengths = *p_lengths;
            for(int dimension = 0, dimensions = lengths.size(); dimension < dimensions;
                                                                  dimension++)
            {
                expression_t * exp = lengths[dimension];
                if(exp == nullptr)
                {
                    array_length_t length = __get_length(dimension);
                    if(length != unkown_array_length)
                        __expression.set_length(dimension, __to_value_expression(length));
                    else
                        this->__log(this, __c_t::expected_array_length);
                }
            }
        }
    }

    array_length_t new_array_ast_node_t::__get_length(dimension_t dimension)
    {
        array_initializer_t * initializer = __expression.initializer;
        _PP((void *)initializer);

        while(dimension-- > 0 && initializer != nullptr)
        {
            array_initialize_element_t element = (*initializer)[0];
            if(element.type != array_initialize_element_type_t::initializer)
                return unkown_array_length;

            initializer = element.initializer;
        }

        if(initializer == nullptr)
            return unkown_array_length;

        return initializer->size();
    }

    ////////// ////////// ////////// ////////// //////////
    // array_initializer

    void array_initializer_ast_node_t::on_commit()
    {
        this->each_child_node<ast_node_t>(elements, [&](ast_node_t * node) {

            expression_ast_t * expression_ast;
            array_initializer_ast_t * initializer_ast;

            if((expression_ast = as<expression_ast_t *>(node)) != nullptr)
            {
                __initializer.append(expression_ast->to_eobject());
            }
            else if((initializer_ast = as<array_initializer_ast_t *>(node)) != nullptr)
            {
                __initializer.append(initializer_ast->to_eobject());
            }
            else
            {
                X_UNEXPECTED();
            }
        });
    }

    array_initializer_t * array_initializer_ast_node_t::to_eobject()
    {
        return &__initializer;
    }

    void array_initializer_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // array_lengths

    void array_lengths_ast_node_t::on_commit()
    {
        this->transform_child_to(lengths, __lengths);
    }

    array_lengths_t * array_lengths_ast_node_t::to_eobject()
    {
        return &__lengths;
    }

    void array_lengths_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // default_value

    void default_value_ast_node_t::on_commit()
    {
        __expression.type_name = __to_eobject<type_name_t *>(type_name);
    }

    expression_t * default_value_ast_node_t::to_eobject()
    {
        return &__expression;
    }

    void default_value_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_empty(__expression.type_name, this, __c_t::type_name_missing,
                                                        _T("default value"));

        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // field

    void field_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__field.name, name, el, _T("field"));
    }

    void field_ast_node_t::on_commit()
    {
        __field.decorate   = __to_eobject<decorate_t *>(decorate);
        __field.type_name  = __to_eobject<type_name_t *>(type_name);
        __field.init_value = __to_eobject<expression_t *>(init_value);
        __field.attributes = __to_eobject_with_owner<attributes_t *>(attributes, &__field);
    }

    field_t * field_ast_node_t::to_eobject()
    {
        return &__field;
    }

    void field_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        context.push(this->to_eobject());
        __super_t::on_walk(context, step, tag);
        context.pop();

        this->__check_empty(__field.type_name, this, __c_t::type_name_missing,
                _F(_T("field %1%"), _str(__field.name)));
        this->__check_empty(__field.name, this, __c_t::name_empty, _T("field"));

        variable_region_t * region = context.current_region();
        field_t * field = to_eobject();

        if(region == nullptr)
            this->__log(field, __c_t::unexpected_field_defination, _str(field));
        else
            region->define_field(field);
    }

    ////////// ////////// ////////// ////////// //////////
    // method

    void method_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__method.name, name, el, _T("method"));
    }

    void method_ast_node_t::set_trait(method_trait_t trait, __el_t * el)
    {
        this->__assign(__method.trait, trait, el, _T("method trait"));
    }

    void method_ast_node_t::on_commit()
    {
        __method.type_name       = __to_eobject<type_name_t *>(type_name);
        __method.decorate        = __to_eobject<decorate_t *>(decorate);
        __method.params          = __to_eobject<params_t *>(params);
        __method.generic_params  = __to_eobject<generic_params_t *>(generic_params);
        __method.owner_type_name = __to_eobject<type_name_t *>(owner_type_name);
        __method.attributes      = __to_eobject_with_owner<attributes_t *>(attributes, &__method);
        __method.body            = __to_eobject<method_body_t *>(body);
    }

    method_t * method_ast_node_t::to_eobject()
    {
        return &__method;
    }

    void method_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch((walk_step_t)step)
        {
            case walk_step_t::default_:
                context.push(this->to_eobject());

                __super_t::on_walk(context, step, tag);
                __walk_default(context);
                this->__delay(context, walk_step_t::analysis);

                context.pop();
                break;

            case walk_step_t::analysis:
                __walk_analysis(context);
                break;

            default: break;
        }
    }

    void method_ast_node_t::__walk_default(ast_walk_context_t & context)
    {
        type_t * type = context.current_type();
        if(type != nullptr && __method.name == type->get_name()
                                && __method.owner_type_name == nullptr)  // constructor
        {
            if(__method.trait == method_trait_t::__default__)
            {
                if(__method.decorate && __method.decorate->is_static)
                    __method.trait = method_trait_t::static_constructor;
                else
                    __method.trait = method_trait_t::constructor;
            }
        }
        else
        {
            if(__method.trait == method_trait_t::__default__)
                __method.trait = method_trait_t::normal;

            this->__check_empty(__method.type_name, this, __c_t::type_name_missing,
                _F(_T("method %1%"), _str(__method.name))
            );
        }

        if(__method.type_name != nullptr)
        {
            switch(__method.trait)
            {
                case method_trait_t::constructor:
                    this->__log(this, __c_t::constructor_method_should_no_return_type,
                                                        _str(__method.name));
                    break;

                case method_trait_t::static_constructor:
                    this->__log(this, __c_t::static_constructor_method_should_no_return_type,
                                                        _str(__method.name));
                    break;

                case method_trait_t::destructor:
                    this->__log(this, __c_t::destructor_method_should_no_return_type,
                                                        _str(__method.name));
                    break;

                default:
                    break;
            }
        }

        this->__check_empty(__method.name, this, __c_t::name_empty, _T("method"));

        if(__method.params && !__method.params->empty())
        {
            variable_region_t * region = context.current_region();
            for(param_t * param : *__method.params)
            {
                if(region == nullptr)
                {
                    this->__log(param, __c_t::unexpected_param_defination, _str(param));
                }
                else
                {
                    try
                    {
                        region->define_param(param);
                    }
                    catch(const logic_error_t<ast_error_t> & e)
                    {
                        if(e.code == ast_error_t::variable_duplicated)
                        {
                            this->__log(param, __c_t::variable_defination_duplicate,
                                                            (string_t)param->name);
                        }
                    }
                }
            }

            switch(__method.trait)
            {
                case method_trait_t::static_constructor:
                    this->__log(this, __c_t::static_constructor_method_should_no_params,
                                                        _str(__method.name));
                    break;

                case method_trait_t::destructor:
                    this->__log(this, __c_t::destructor_method_should_no_params,
                                                        _str(__method.name));
                    break;

                default:
                    break;
            }
        }
    }

    void method_ast_node_t::__walk_analysis(ast_walk_context_t & context)
    {
        switch(__method.trait)
        {
            case method_trait_t::static_constructor:
                break;

            case method_trait_t::constructor:
                __method_utils_t(__context, context).revise_constructor(__method,
                    __to_eobject<arguments_t *>(base_ctor_args), this->child_at(base_ctor_args)
                );
                break;

            case method_trait_t::destructor:
                break;

            default:
                break;
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // property

    void property_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__property.name, name, el, _T("property"));
    }

    void property_ast_node_t::on_commit()
    {
        __property.type_name  = __to_eobject<type_name_t *>(type_name);
        __property.decorate   = __to_eobject<decorate_t *>(decorate);
        __property.get_method = __to_eobject<method_t *>(get_method);
        __property.set_method = __to_eobject<method_t *>(set_method);
        __property.params     = __to_eobject<params_t *>(params);
        __property.owner_type_name = __to_eobject<type_name_t *>(owner_type_name);
        __property.attributes = __to_eobject_with_owner<attributes_t *>(attributes, &__property);
    }

    property_t * property_ast_node_t::to_eobject()
    {
        return &__property;
    }

    bool __append_method(ast_context_t & cctx, ast_walk_context_t & wctx, method_t * method)
    {
        type_t * type = wctx.current_type();
        if(type == nullptr || type->this_gtype() != gtype_t::general)
        {
            ast_log(cctx, method, __c_t::type_undetermind, method);
            return false;
        }

        ((general_type_t *)type)->append_member(method);
        return true;
    }

    void property_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        if(!__property.name.empty())
        {
            if(__property.params != nullptr)
                this->__log(this->child_at(params), __c_t::unexpected_params, "property");
        }
        else
        {
            if(__property.params == nullptr)
                this->__log(this, __c_t::name_empty, _T("property"));
        }

        if(__property.get_method != nullptr)
        {
            __property.get_method->name = __to_name(_F(_T("get_%1%"), __property.name));
            __property.get_method->type_name = __property.type_name;
            __property.get_method->decorate  = __property.decorate;
            __append_method(this->__context, context, __property.get_method);
        }

        if(__property.set_method != nullptr)
        {
            __property.set_method->name = __to_name(_F(_T("set_%1%"), __property.name));
            __property.set_method->type_name = context.to_type_name(vtype_t::void_);
            __property.set_method->decorate  = __property.decorate;

            typedef std::initializer_list<param_t *> il_t;
            __property.set_method->params = __new_obj<params_t>(il_t({
                __new_obj<param_t>(__property.type_name, __to_name(_T("value")))
            }));

            __append_method(this->__context, context, __property.set_method);
        }

        variable_region_t * region = context.current_region();

        if(region == nullptr)
            this->__log(&__property, __c_t::unexpected_property_defination, _str(&__property));
        else
            region->define_property(&__property);

        context.push(this->to_eobject());
        __super_t::on_walk(context, step, tag);
        context.pop();
    }

    ////////// ////////// ////////// ////////// //////////
    // event

    void event_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__event.name, name, el, _T("event"));
    }

    void event_ast_node_t::on_commit()
    {
        __event.type_name       = __to_eobject<type_name_t *>(type_name);
        __event.decorate        = __to_eobject<decorate_t *>(decorate);
        __event.add_method      = __to_eobject<method_t *>(add_method);
        __event.remove_method   = __to_eobject<method_t *>(remove_method);
        __event.owner_type_name = __to_eobject<type_name_t *>(owner_type_name);
        __event.attributes      = __to_eobject_with_owner<attributes_t *>(attributes, &__event);
    }

    event_t * event_ast_node_t::to_eobject()
    {
        return &__event;
    }

    void event_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        this->__check_name_empty(__event.name, _T("event"));

        if(__event.add_method != nullptr)
        {
            __event.add_method->name = __to_name(_F(_T("add_%1%"), __event.name));
            __event.add_method->type_name = __event.type_name;
            __append_method(this->__context, context, __event.add_method);
        }

        if(__event.remove_method != nullptr)
        {
            __event.remove_method->name = __to_name(_F(_T("remove_%1%"), __event.name));
            __event.remove_method->type_name = __event.type_name;
            __append_method(this->__context, context, __event.remove_method);
        }

        context.push(this->to_eobject());
        __super_t::on_walk(context, step, tag);
        context.pop();
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::compile

