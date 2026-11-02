#include <compile.h>
#include "utils.h"

namespace X_ROOT_NS::modules::compile {

    using namespace core;

    typedef common_log_code_t __c_t;

    ////////// ////////// ////////// ////////// //////////

    // The analysis of the nullable references: the states (maybe null, or not null) of the
    // variables are known through the statements of a method, the warnings are the uses of a
    // value that can be null where it cannot be (a dereference, a variable, an argument or a
    // return value that is not annotated with ?). It is enabled by #nullable enable in the
    // source, until #nullable disable.

    namespace
    {
        // The directives #nullable of a file: the offsets from which it is enabled, or not.
        struct __file_directives_t
        {
            const char_t * begin = nullptr;                     // The source code.
            const char_t * end   = nullptr;
            std::vector<std::pair<const char_t *, bool>> items;
        };

        static std::map<const code_file_t *, __file_directives_t> __directives_cache;

        // Returns the directives of a file.
        static const __file_directives_t & __directives_of(const code_file_t * file)
        {
            auto it = __directives_cache.find(file);
            if (it != __directives_cache.end())
                return it->second;

            __file_directives_t & d = __directives_cache[file];

            const char_t * text = file->get_source_code();
            if (text == nullptr)
                return d;

            d.begin = text;
            d.end   = text + al::strlen(text);

            const char_t * p = text;
            while (*p)
            {
                // The beginning of a line.
                const char_t * line = p;
                while (*p && !al::is_lineend(*p))
                    p++;

                const char_t * q = line;
                while (q < p && al::is_whitespace(*q))
                    q++;

                if (q < p && *q == _T('#'))
                {
                    string_t s(q + 1, p);

                    // #nullable enable, #nullable disable
                    size_t i = 0;
                    while (i < s.size() && al::is_whitespace(s[i]))
                        i++;

                    if (s.compare(i, 8, _T("nullable")) == 0)
                    {
                        i += 8;
                        while (i < s.size() && al::is_whitespace(s[i]))
                            i++;

                        if (s.compare(i, 6, _T("enable")) == 0)
                            d.items.push_back({ line, true });
                        else if (s.compare(i, 7, _T("disable")) == 0)
                            d.items.push_back({ line, false });
                    }
                }

                while (*p && al::is_lineend(*p))
                    p++;
            }

            return d;
        }

        // Returns whether the position of the code is in the source code of its file.
        static bool __is_located(const code_unit_t * unit)
        {
            if (unit == nullptr || unit->file == nullptr || unit->s == nullptr)
                return false;

            const __file_directives_t & d = __directives_of(unit->file);
            return d.begin != nullptr && unit->s >= d.begin && unit->s <= d.end;
        }

        // Returns whether the nullable analysis is enabled at the position of the code.
        static bool __enabled_at(const code_unit_t * unit)
        {
            if (!__is_located(unit))
                return false;

            const __file_directives_t & d = __directives_of(unit->file);

            bool enabled = is_nullable_enabled_by_default(unit->file);
            for (auto & item : d.items)
            {
                if (item.first > unit->s)
                    break;

                enabled = item.second;
            }

            return enabled;
        }

        ////////// ////////// ////////// ////////// //////////

        // The state of the variables: whether they can be null.
        struct __state_t
        {
            std::map<variable_t *, bool>   maybe;               // The variables with a state.
            bool                            reachable = true;   // Whether the code is reached.
        };

        // The unwrapped method template (a method of a generic type, a generic method).
        static method_t * __raw_method_of(method_base_t * method)
        {
            while (method != nullptr)
            {
                switch (method->this_family())
                {
                    case member_family_t::general:
                        return (method_t *)method;

                    case member_family_t::impl:
                        method = ((impl_method_t *)method)->raw;
                        break;

                    case member_family_t::generic:
                        method = ((generic_method_t *)method)->template_;
                        break;

                    default:
                        return nullptr;
                }
            }

            return nullptr;
        }

        // Returns whether the type name is annotated: it can be null.
        static bool __is_annotated(type_name_t * type_name)
        {
            if (type_name == nullptr || !type_name->nullable)
                return false;

            return true;
        }

        // Returns whether a value of the type name can be null by its declaration: T? can be
        // null, whatever T is.
        static bool __declared_maybe(type_name_t * type_name)
        {
            if (type_name == nullptr)
                return false;

            type_t * type = to_type(type_name);
            if (type == nullptr)
                return false;

            if (type->this_gtype() == gtype_t::generic_param)
                return __is_annotated(type_name);

            if (!can_be_null(type))
                return false;

            return __is_annotated(type_name);
        }

        // Returns whether the param is annotated with [MaybeNullWhen(b)]: -1 when it is not,
        // else b (0, 1).
        static int __maybe_null_when(param_t * param)
        {
            if (param == nullptr || param->attributes == nullptr)
                return -1;

            for (attribute_t * attribute : *param->attributes)
            {
                type_t * type = to_type(attribute->type_name);
                if (type == nullptr)
                    continue;

                string_t name = type->to_string();
                const string_t suffix = _T("MaybeNullWhenAttribute");
                if (name.size() < suffix.size()
                        || name.compare(name.size() - suffix.size(), suffix.size(), suffix) != 0)
                {
                    continue;
                }

                if (attribute->arguments != nullptr && attribute->arguments->size() > 0)
                {
                    expression_t * exp = (*attribute->arguments)[0]->expression;
                    cvalue_t value = exp->execute();

                    if (value == true)
                        return 1;

                    if (value == false)
                        return 0;
                }
            }

            return -1;
        }

        // Returns whether the type name can be null, and it is not annotated.
        static bool __declared_not_null(type_name_t * type_name)
        {
            if (type_name == nullptr)
                return false;

            type_t * type = to_type(type_name);
            if (type == nullptr || !can_be_null(type))
                return false;

            return !__is_annotated(type_name);
        }

        // The nullability of a type: whether a value can be null, and the nullability of the
        // generic arguments (List<String?>), and of the elements of an array (String?[]).
        struct __ntype_t
        {
            bool                    known = false;      // Whether it is known.
            bool                    maybe = false;      // Whether a value can be null.
            std::vector<__ntype_t>  args;               // The generic arguments, the element.
        };

        // The analysis of a method.
        class __nullable_analyzer_t
        {
        public:
            __nullable_analyzer_t(method_t * method, logger_t & logger)
                : __method(method), __logger(logger)
            { }

            // Analyzes the body of the method.
            void analyze()
            {
                __begin_constructor();

                if (__method->body != nullptr)
                {
                    for (statement_t * statement : __method->body->statements)
                        __exec(statement);
                }

                // The end of a constructor.
                __check_fields();
            }

        private:
            method_t *      __method;
            logger_t &      __logger;
            __state_t       __s;                    // The current state.
            int             __silent = 0;           // The warnings are not logged when it is not 0.

            struct __loop_t
            {
                __loop_t()
                {
                    breaks.reachable = false;
                    continues.reachable = false;
                }

                __state_t   breaks;                 // The state of the breaks.
                __state_t   continues;              // The state of the continues.
            };

            std::vector<__loop_t *> __loops;
            std::set<std::pair<void *, int>> __logged;

            // The fields that cannot be null, and are not initialized when a constructor begins.
            std::vector<field_t *> __fields;

            // The object of the member that is evaluated: list in list.Get(0).
            expression_t * __receiver = nullptr;

            // The last expression that has a position: the expressions that are made by the
            // compiler (the foreach statement) have none.
            expression_t * __anchor = nullptr;

            // The nullability of the variables of var: the arguments (List<String?>) are known.
            std::map<variable_t *, __ntype_t> __var_ntypes;

            // The nullability of the generic params of the method that is called (First<T>):
            // they are given (First<String?>(...)) or inferred from the arguments.
            std::map<generic_param_t *, __ntype_t> __method_generics;

            // The states of the out arguments of the last call: the variable, whether it can be
            // null when the call returns true, and when it returns false.
            struct __out_t { variable_t * variable; bool when_true; bool when_false; };
            std::vector<__out_t> __outs;

            // Returns whether the method is a constructor that calls another one: this(...).
            bool __calls_this_constructor()
            {
                if (__method->body == nullptr)
                    return false;

                for (statement_t * statement : __method->body->statements)
                {
                    auto * st = dynamic_cast<expression_statement_t *>(statement);
                    if (st == nullptr || st->expression == nullptr
                            || st->expression->this_family() != expression_family_t::function)
                        continue;

                    function_expression_t * function = (function_expression_t *)st->expression;
                    if (function->get_ftype() != function_expression_type_t::method)
                        continue;

                    method_t * callee = __raw_method_of(function->get_method());
                    if (callee != nullptr && callee != __method
                            && callee->trait == method_trait_t::constructor
                            && callee->host_type == __method->host_type)
                    {
                        return true;
                    }
                }

                return false;
            }

            // Begins a constructor: the fields that cannot be null have no value.
            void __begin_constructor()
            {
                if (__method->trait != method_trait_t::constructor || __method->is_static())
                    return;

                type_t * host = __method->host_type;
                if (host == nullptr || host->this_ttype() != ttype_t::class_)
                    return;

                // this(...) does the construction.
                if (__calls_this_constructor())
                    return;

                analyze_members_args_t args(member_type_t::field, name_t::null);
                host->get_members(args);

                for (member_t * member : args.out_members)
                {
                    field_t * field = (field_t *)member;
                    if (field->is_static() || field->is_const() || field->variable == nullptr)
                        continue;

                    if (!__declared_not_null(field->type_name))
                        continue;

                    __fields.push_back(field);
                    __s.maybe[__key(field->variable)] = true;
                }
            }

            // Checks the fields when a constructor exits.
            void __check_fields()
            {
                if (__fields.empty() || !__s.reachable)
                    return;

                static std::set<std::pair<field_t *, int>> logged;

                for (field_t * field : __fields)
                {
                    if (!__maybe_of(__s, field->variable))
                        continue;

                    code_element_t * ce = as<code_element_t *>(field);
                    if (ce == nullptr || !__enabled_at(ce->get_code_unit()))
                        continue;

                    if (!logged.insert({ field, (int)__c_t::nullable_field_uninitialized }).second)
                        continue;

                    ast_log(__logger, field, __c_t::nullable_field_uninitialized, field->name);
                }
            }

            ////////// ////////// ////////// ////////// //////////
            // states

            static __state_t __unreachable()
            {
                __state_t s;
                s.reachable = false;
                return s;
            }

            // Returns the variable that has the state: a field has many variable objects.
            static variable_t * __key(variable_t * variable)
            {
                if (variable != nullptr && variable->this_type() == variable_type_t::field)
                {
                    field_t * field = ((field_variable_t *)variable)->field;
                    if (field != nullptr && field->variable != nullptr)
                        return field->variable;
                }

                return variable;
            }

            // Returns the state of a variable in a state.
            static bool __maybe_of(const __state_t & s, variable_t * variable)
            {
                auto it = s.maybe.find(__key(variable));
                if (it != s.maybe.end())
                    return it->second;

                return __declared_maybe_of(variable);
            }

            // Returns whether the variable can be null by its declaration.
            static bool __declared_maybe_of(variable_t * variable)
            {
                if (variable == nullptr)
                    return false;

                switch (variable->this_type())
                {
                    case variable_type_t::local:
                        return __declared_maybe(((local_variable_t *)variable)->type_name);

                    case variable_type_t::param:
                        return __declared_maybe(((param_variable_t *)variable)->param->type_name);

                    case variable_type_t::field:
                        return __declared_maybe(((field_variable_t *)variable)->field->type_name);

                    case variable_type_t::property:
                        return __declared_maybe(
                            ((property_variable_t *)variable)->property->type_name
                        );

                    default:
                        return false;
                }
            }

            // Returns the type name of a variable.
            static type_name_t * __type_name_of(variable_t * variable)
            {
                if (variable == nullptr)
                    return nullptr;

                switch (variable->this_type())
                {
                    case variable_type_t::local:
                        return ((local_variable_t *)variable)->type_name;

                    case variable_type_t::param:
                        return ((param_variable_t *)variable)->param->type_name;

                    case variable_type_t::field:
                        return ((field_variable_t *)variable)->field->type_name;

                    case variable_type_t::property:
                        return ((property_variable_t *)variable)->property->type_name;

                    default:
                        return nullptr;
                }
            }

            // Returns whether the variable has a state in the analysis.
            static bool __is_tracked(variable_t * variable)
            {
                if (variable == nullptr)
                    return false;

                return al::in(variable->this_type(), variable_type_t::local,
                              variable_type_t::param, variable_type_t::field);
            }

            // Joins two states.
            static __state_t __join(const __state_t & a, const __state_t & b)
            {
                if (!a.reachable)
                    return b;

                if (!b.reachable)
                    return a;

                __state_t r;
                r.reachable = true;

                for (auto & item : a.maybe)
                    r.maybe[__key(item.first)] = item.second || __maybe_of(b, item.first);

                for (auto & item : b.maybe)
                {
                    if (r.maybe.find(item.first) == r.maybe.end())
                        r.maybe[__key(item.first)] = item.second || __maybe_of(a, item.first);
                }

                return r;
            }

            ////////// ////////// ////////// ////////// //////////
            // warnings

            template<typename ... _args_t>
            void __warn(expression_t * exp, __c_t code, _args_t && ... args)
            {
                if (__silent > 0 || !__s.reachable)
                    return;

                // The position is that of the expression, or of the one that has it.
                expression_t * at = exp;
                while (at != nullptr)
                {
                    code_element_t * ce = as<code_element_t *>(at);
                    if (ce != nullptr && __is_located(ce->get_code_unit()))
                        break;

                    at = at->parent;
                }

                // The position of what was evaluated before.
                if (at == nullptr && __anchor != nullptr)
                {
                    code_element_t * ce = as<code_element_t *>(__anchor);
                    if (ce != nullptr && __is_located(ce->get_code_unit()))
                        at = __anchor;
                }

                if (at == nullptr
                        || !__enabled_at(as<code_element_t *>(at)->get_code_unit()))
                    return;

                if (!__logged.insert({ exp, (int)code }).second)
                    return;

                ast_log(__logger, at, code, std::forward<_args_t>(args) ...);
            }

            // Whether the null states are checked at the expression.
            bool __checked_at(expression_t * exp)
            {
                code_element_t * ce = as<code_element_t *>(exp);
                return ce != nullptr && __enabled_at(ce->get_code_unit());
            }

            // A value is used where it cannot be null.
            void __check_assign(expression_t * value, bool maybe, type_name_t * to,
                                                                    __c_t * out_code = nullptr)
            {
                if (!maybe || !__declared_not_null(to))
                    return;

                type_t * type = to_type(to);
                type_t * from = value->get_type();

                if (from != nullptr && is_null(from))
                    __warn(value, __c_t::nullable_null_literal, type->to_string());
                else
                    __warn(value, __c_t::nullable_convert, type->to_string());
            }

            ////////// ////////// ////////// ////////// //////////
            // expressions

            // Returns the variable of an expression, that has a state: a name, this.name.
            variable_t * __variable_of(expression_t * exp)
            {
                if (exp == nullptr)
                    return nullptr;

                switch (exp->this_family())
                {
                    case expression_family_t::name:
                    case expression_family_t::name_unit: {
                        name_expression_t * name_exp = (name_expression_t *)exp;
                        if (name_exp->expression_type == name_expression_type_t::variable)
                            return name_exp->variable;

                        return nullptr;
                    }

                    case expression_family_t::binary: {
                        binary_expression_t * binary = (binary_expression_t *)exp;
                        if (binary->op() == operator_t::member_point
                                && !binary->null_conditional
                                && binary->exp1() != nullptr
                                && binary->exp1()->this_family() == expression_family_t::this_)
                        {
                            return __variable_of(binary->exp2());
                        }

                        return nullptr;
                    }

                    case expression_family_t::expressions: {
                        expressions_t * exps = (expressions_t *)exp;
                        if (exps->expression_count() == 1)
                            return __variable_of(exps->expression_at(0));

                        return nullptr;
                    }

                    default:
                        return nullptr;
                }
            }

            // Returns the type name of the value of a member: the field, the property, the
            // method that is returned.
            type_name_t * __member_type_name_of(expression_t * exp)
            {
                if (exp == nullptr)
                    return nullptr;

                switch (exp->this_family())
                {
                    case expression_family_t::name:
                    case expression_family_t::name_unit: {
                        name_expression_t * name_exp = (name_expression_t *)exp;
                        if (name_exp->expression_type != name_expression_type_t::variable)
                            return nullptr;

                        variable_t * variable = name_exp->variable;
                        if (variable == nullptr)
                            return nullptr;

                        if (variable->this_type() == variable_type_t::field)
                        {
                            field_t * field = ((field_variable_t *)variable)->field;
                            while (field->this_family() == member_family_t::impl)
                                field = ((impl_field_t *)field)->raw;

                            return field->type_name;
                        }

                        if (variable->this_type() == variable_type_t::property)
                        {
                            property_t * property = ((property_variable_t *)variable)->property;
                            while (property->this_family() == member_family_t::impl)
                                property = ((impl_property_t *)property)->raw;

                            return property->type_name;
                        }

                        return __type_name_of(variable);
                    }

                    case expression_family_t::function: {
                        function_expression_t * function = (function_expression_t *)exp;
                        if (function->get_ftype() != function_expression_type_t::method)
                            return nullptr;

                        method_t * raw = __raw_method_of(function->get_method());
                        return raw != nullptr? raw->type_name : nullptr;
                    }

                    default:
                        return nullptr;
                }
            }

            // The nullability of a declared type name, with its arguments.
            static __ntype_t __ntype_from(type_name_t * type_name)
            {
                __ntype_t r;
                if (type_name == nullptr)
                    return r;

                r.known = true;
                r.maybe = type_name->nullable;

                if (auto * general = dynamic_cast<general_type_name_t *>(type_name))
                {
                    if (!general->units.empty())
                    {
                        name_unit_t * unit = general->units[general->units.size() - 1];
                        if (unit != nullptr && unit->args != nullptr)
                        {
                            for (generic_arg_t * arg : *unit->args)
                            {
                                r.args.push_back(arg != nullptr && arg->type_name != nullptr?
                                    __ntype_from(arg->type_name) : __ntype_t());
                            }
                        }
                    }
                }
                else if (auto * array = dynamic_cast<array_type_name_t *>(type_name))
                {
                    r.args.push_back(__ntype_from(array->element_type_name));
                }

                return r;
            }

            // Returns the nullability of a declared type, when it is accessed through an object:
            // T of a List<T> is the argument of the type of the object (List<String?>).
            __ntype_t __ntype_subst(type_t * type, type_name_t * type_name, const __ntype_t & object,
                                    method_t * raw)
            {
                __ntype_t r;
                if (type == nullptr)
                    return r;

                if (type->this_gtype() == gtype_t::generic_param)
                {
                    generic_param_t * gp = (generic_param_t *)type;

                    // A param of the method is not one of the type.
                    bool own = false;
                    if (raw != nullptr && raw->generic_params != nullptr)
                    {
                        for (generic_param_t * mp : *raw->generic_params)
                            own = own || mp == gp;
                    }

                    // The nullability of a param of the method is given, or inferred.
                    if (own)
                    {
                        auto it = __method_generics.find(gp);
                        if (it != __method_generics.end())
                        {
                            r = it->second;
                            if (type_name != nullptr && type_name->nullable)
                                r.maybe = true;

                            return r;
                        }
                    }

                    if (!own && object.known && gp->index >= 0
                            && (size_t)gp->index < object.args.size()
                            && object.args[gp->index].known)
                    {
                        r = object.args[gp->index];

                        // T? can be null.
                        if (type_name != nullptr && type_name->nullable)
                            r.maybe = true;

                        return r;
                    }

                    r.maybe = type_name != nullptr && type_name->nullable;
                    return r;
                }

                r = type_name != nullptr? __ntype_from(type_name) : __ntype_t();
                r.known = true;

                // The arguments: List<T> of the type of the object.
                std::vector<type_name_t *> arg_type_names;
                if (auto * general = dynamic_cast<general_type_name_t *>(type_name))
                {
                    if (!general->units.empty())
                    {
                        name_unit_t * unit = general->units[general->units.size() - 1];
                        if (unit != nullptr && unit->args != nullptr)
                        {
                            for (generic_arg_t * arg : *unit->args)
                                arg_type_names.push_back(arg != nullptr? arg->type_name : nullptr);
                        }
                    }
                }
                else if (auto * array = dynamic_cast<array_type_name_t *>(type_name))
                {
                    arg_type_names.push_back(array->element_type_name);
                }

                std::vector<type_t *> arg_types;
                if (type->this_gtype() == gtype_t::generic)
                {
                    for (const typex_t & arg : ((generic_type_t *)type)->args)
                        arg_types.push_back(arg.type());
                }
                else if (type->this_gtype() == gtype_t::array)
                {
                    arg_types.push_back(((array_type_t *)type)->element_type);
                }

                if (!arg_types.empty())
                {
                    r.args.clear();
                    for (size_t k = 0; k < arg_types.size(); k++)
                    {
                        r.args.push_back(__ntype_subst(arg_types[k],
                            k < arg_type_names.size()? arg_type_names[k] : nullptr, object, raw));
                    }
                }

                return r;
            }

            // The nullability of a member of an object: the field, the property, the function.
            __ntype_t __ntype_of_member(expression_t * member, const __ntype_t & object)
            {
                if (member == nullptr)
                    return __ntype_t();

                type_name_t * type_name = __member_type_name_of(member);
                method_t * raw = nullptr;

                if (member->this_family() == expression_family_t::function)
                {
                    function_expression_t * function = (function_expression_t *)member;
                    if (function->get_ftype() == function_expression_type_t::method)
                        raw = __raw_method_of(function->get_method());
                }

                if (type_name == nullptr)
                    return __ntype_t();

                return __ntype_subst(to_type(type_name), type_name, object, raw);
            }

            // The nullability of the value of an expression.
            __ntype_t __ntype_of(expression_t * exp)
            {
                if (exp == nullptr)
                    return __ntype_t();

                switch (exp->this_family())
                {
                    case expression_family_t::name:
                    case expression_family_t::name_unit: {
                        name_expression_t * name_exp = (name_expression_t *)exp;
                        if (name_exp->expression_type != name_expression_type_t::variable
                                || name_exp->variable == nullptr)
                            return __ntype_t();

                        variable_t * variable = name_exp->variable;

                        // A variable of var.
                        auto vit = __var_ntypes.find(variable);
                        __ntype_t r = vit != __var_ntypes.end()? vit->second
                                                              : __ntype_of_member(exp, __ntype_t());

                        if (__is_tracked(variable))
                            r.maybe = __maybe_of(__s, variable);

                        return r;
                    }

                    case expression_family_t::function:
                        return __ntype_of_member(exp, __ntype_t());

                    case expression_family_t::new_:
                        return __ntype_from(((new_expression_t *)exp)->type_name);

                    case expression_family_t::type_cast:
                        return __ntype_from(((type_cast_expression_t *)exp)->type_name);

                    case expression_family_t::new_array: {
                        // The elements: new String?[2].
                        __ntype_t r;
                        r.known = true;
                        r.args.push_back(__ntype_from(((new_array_expression_t *)exp)->type_name));
                        return r;
                    }

                    case expression_family_t::expressions:
                        if (exp->expression_count() == 1)
                            return __ntype_of(exp->expression_at(0));

                        return __ntype_t();

                    case expression_family_t::index: {
                        index_expression_t * index = (index_expression_t *)exp;
                        __ntype_t body = __ntype_of(index->namex());

                        if (index->variable != nullptr
                                && index->variable->this_type() == variable_type_t::array_index)
                        {
                            // An element of an array: String?[].
                            if (body.known && body.args.size() == 1)
                                return body.args[0];

                            return __ntype_t();
                        }

                        if (index->variable != nullptr
                                && index->variable->this_type() == variable_type_t::property_index)
                        {
                            property_t * property =
                                ((property_index_variable_t *)index->variable)->property;

                            while (property != nullptr
                                    && property->this_family() == member_family_t::impl)
                                property = ((impl_property_t *)property)->raw;

                            if (property != nullptr)
                            {
                                return __ntype_subst(to_type(property->type_name),
                                                     property->type_name, body, nullptr);
                            }
                        }

                        return __ntype_t();
                    }

                    case expression_family_t::binary: {
                        binary_expression_t * binary = (binary_expression_t *)exp;
                        if (binary->op() != operator_t::member_point)
                            return __ntype_t();

                        // this.name: a variable that has a state.
                        if (binary->exp1() != nullptr
                                && binary->exp1()->this_family() == expression_family_t::this_)
                            return __ntype_of(binary->exp2());

                        __ntype_t r = __ntype_of_member(binary->exp2(), __ntype_of(binary->exp1()));
                        return r;
                    }

                    default:
                        return __ntype_t();
                }
            }

            // Infers the nullability of the generic params of a method from an argument: the param
            // is T, List<T> or T[], the argument is a value (a List<String?>).
            void __unify(type_t * param_type, type_name_t * param_type_name, const __ntype_t & value,
                         method_t * raw)
            {
                if (param_type == nullptr || !value.known)
                    return;

                if (param_type->this_gtype() == gtype_t::generic_param)
                {
                    generic_param_t * gp = (generic_param_t *)param_type;

                    bool own = false;
                    if (raw != nullptr && raw->generic_params != nullptr)
                    {
                        for (generic_param_t * mp : *raw->generic_params)
                            own = own || mp == gp;
                    }

                    if (!own)
                        return;

                    // Given by the call: the arguments of the method are not inferred.
                    auto it = __method_generics.find(gp);
                    if (it == __method_generics.end())
                        __method_generics[gp] = value;
                    else if (value.maybe)
                        it->second.maybe = true;

                    return;
                }

                std::vector<type_t *> param_args;
                if (param_type->this_gtype() == gtype_t::generic)
                {
                    for (const typex_t & arg : ((generic_type_t *)param_type)->args)
                        param_args.push_back(arg.type());
                }
                else if (param_type->this_gtype() == gtype_t::array)
                {
                    param_args.push_back(((array_type_t *)param_type)->element_type);
                }

                for (size_t k = 0; k < param_args.size() && k < value.args.size(); k++)
                    __unify(param_args[k], nullptr, value.args[k], raw);
            }

            // Evaluates the arguments of a function, with the params that are not annotated.
            void __eval_arguments(function_expression_t * function,
                                  expression_t * receiver = nullptr)
            {
                method_t * raw = function->get_ftype() == function_expression_type_t::method?
                                    __raw_method_of(function->get_method()) : nullptr;

                // The arguments of the type of the object: List<String?>.
                __ntype_t object = receiver != nullptr? __ntype_of(receiver) : __ntype_t();

                std::vector<__out_t> outs;

                // The generic params of the method: First<String?>(...).
                __method_generics.clear();
                if (raw != nullptr && raw->generic_params != nullptr && function->generic_args != nullptr)
                {
                    for (size_t k = 0; k < function->generic_args->size()
                                    && k < raw->generic_params->size(); k++)
                    {
                        // The arguments that the compiler infers have no written type name.
                        generic_arg_t * given = (*function->generic_args)[k];
                        if (given != nullptr && (dynamic_cast<general_type_name_t *>(given->type_name)
                                || dynamic_cast<array_type_name_t *>(given->type_name)))
                        {
                            __method_generics[(*raw->generic_params)[k]] = __ntype_from(given->type_name);
                        }
                    }
                }

                // The nested calls (the arguments) set their own params: this call has its own.
                std::map<generic_param_t *, __ntype_t> given_generics = __method_generics;

                for (size_t k = 0, count = function->argument_count(); k < count; k++)
                {
                    argument_t * arg = function->argument_at(k);
                    if (arg == nullptr || arg->expression == nullptr)
                        continue;

                    bool maybe = __eval(arg->expression);

                    param_t * param = (raw != nullptr && raw->params != nullptr
                                       && k < raw->params->size())? (*raw->params)[k] : nullptr;

                    if (param == nullptr)
                        continue;

                    // The params of the method that the argument tells.
                    if (raw->generic_params != nullptr && raw->generic_params->size() > 0
                            && arg->atype == param_type_t::__default__)
                    {
                        std::map<generic_param_t *, __ntype_t> saved = __method_generics;
                        __method_generics = given_generics;

                        __ntype_t value = __ntype_of(arg->expression);
                        if (!value.known)
                            value.known = arg->expression->get_type() != nullptr;

                        value.maybe = value.maybe || maybe;

                        __unify(to_type(param->type_name), param->type_name, value, raw);

                        given_generics = __method_generics;
                        __method_generics = saved;
                    }

                    // The nullability of the param: through the type of the object.
                    __ntype_t param_ntype = __ntype_subst(to_type(param->type_name),
                                                          param->type_name, object, raw);

                    // An out (or ref) argument gets the state of the param.
                    if (arg->atype != param_type_t::__default__)
                    {
                        variable_t * variable = __variable_of(arg->expression);
                        if (__is_tracked(variable))
                        {
                            bool param_maybe = param_ntype.known && param_ntype.maybe;
                            bool when_true = param_maybe, when_false = param_maybe;

                            // [MaybeNullWhen(false)]: it is null when the call returns false.
                            // The argument of the type that can be null keeps it.
                            int when = __maybe_null_when(param);
                            if (when >= 0)
                            {
                                when_true  = when == 1 || param_maybe;
                                when_false = when == 0 || param_maybe;
                            }

                            __s.maybe[__key(variable)] = when_true || when_false;
                            outs.push_back({ variable, when_true, when_false });
                        }

                        continue;
                    }

                    // A param that cannot be null: not annotated, or the argument of the object.
                    type_t * arg_type = arg->expression->get_type();
                    if (param->ptype == param_type_t::__default__ && maybe
                            && param_ntype.known && !param_ntype.maybe
                            && arg_type != nullptr && (is_null(arg_type) || can_be_null(arg_type)))
                    {
                        if (is_null(arg_type))
                        {
                            // The name of a generic param is ?T.
                            string_t type_string = to_type(param->type_name)->to_string();
                            if (!type_string.empty() && type_string[0] == _T('?'))
                                type_string.erase(0, 1);

                            __warn(arg->expression, __c_t::nullable_null_literal, type_string);
                        }
                        else
                        {
                            __warn(arg->expression, __c_t::nullable_argument,
                                param->name, raw->name);
                        }
                    }
                }

                __outs = outs;

                // The result of this call is that of its own params.
                __method_generics = given_generics;
            }

            // Evaluates an expression, returns whether the value can be null.
            bool __eval(expression_t * exp)
            {
                if (exp == nullptr)
                    return false;

                if (code_element_t * ce = as<code_element_t *>(exp))
                {
                    const code_unit_t * unit = ce->get_code_unit();
                    if (unit != nullptr && unit->s != nullptr)
                        __anchor = exp;
                }

                switch (exp->this_family())
                {
                    case expression_family_t::cvalue: {
                        type_t * type = exp->get_type();
                        return type != nullptr && is_null(type);
                    }

                    case expression_family_t::name:
                    case expression_family_t::name_unit: {
                        name_expression_t * name_exp = (name_expression_t *)exp;
                        if (name_exp->expression_type != name_expression_type_t::variable
                                || name_exp->variable == nullptr)
                            return false;

                        variable_t * variable = name_exp->variable;
                        if (__is_tracked(variable))
                            return __maybe_of(__s, variable);

                        // A property: by its declaration.
                        type_name_t * type_name = __member_type_name_of(exp);
                        return __declared_maybe(type_name);
                    }

                    case expression_family_t::default_value: {
                        type_t * type = exp->get_type();
                        return type != nullptr && can_be_null(type);
                    }

                    case expression_family_t::type_cast:
                        return __eval(((type_cast_expression_t *)exp)->expression());

                    case expression_family_t::checked:
                        return __eval(exp->expression_at(0));

                    case expression_family_t::function: {
                        function_expression_t * function = (function_expression_t *)exp;

                        // The object of the member, that has the function.
                        expression_t * receiver = __receiver;
                        __receiver = nullptr;

                        // The function through a variable (a delegate).
                        if (function->get_ftype() == function_expression_type_t::variable)
                        {
                            __eval_arguments(function);
                            return false;
                        }

                        __eval_arguments(function, receiver);

                        // The nullability of the result, through the type of the object.
                        __ntype_t result = __ntype_of_member(exp,
                            receiver != nullptr? __ntype_of(receiver) : __ntype_t());

                        return result.known && result.maybe;
                    }

                    case expression_family_t::index: {
                        index_expression_t * index = (index_expression_t *)exp;

                        expression_t * body = index->namex();
                        if (body != nullptr)
                        {
                            bool maybe_body = __eval(body);

                            // The object of ?[ and of the members that follow it are not checked.
                            if (!index->null_conditional && !in_null_conditional_chain(body))
                                __check_dereference(body, maybe_body);
                        }

                        for (size_t k = 0, count = index->argument_count(); k < count; k++)
                        {
                            argument_t * arg = index->argument_at(k);
                            if (arg != nullptr)
                                __eval(arg->expression);
                        }

                        // The chain that has ?[ is null when its object is.
                        if (index->is_null_conditional_chain_top())
                        {
                            type_t * type = index->get_type();
                            return type != nullptr && can_be_null(type);
                        }

                        // An element of an array (String?[]), the indexer of a type with an argument
                        // that can be null (list[0]).
                        __ntype_t element = __ntype_of(exp);
                        return element.known && element.maybe;
                    }

                    case expression_family_t::unitary: {
                        unitary_expression_t * unitary = (unitary_expression_t *)exp;

                        if (unitary->op() == operator_t::null_forgiving)
                        {
                            // The state of x! is not null: x is not null after it too.
                            __eval(unitary->exp());

                            variable_t * variable = __variable_of(unitary->exp());
                            if (__is_tracked(variable))
                                __s.maybe[__key(variable)] = false;

                            return false;
                        }

                        __eval(unitary->exp());
                        return false;
                    }

                    case expression_family_t::binary:
                        return __eval_binary((binary_expression_t *)exp);

                    case expression_family_t::expressions: {
                        bool maybe = false;

                        for (size_t k = 0, count = exp->expression_count(); k < count; k++)
                            maybe = __eval(exp->expression_at(k));

                        return maybe;
                    }

                    case expression_family_t::__unknown__: {
                        // A condition: c? a : b.
                        if (exp->expression_count() == 3)
                        {
                            __state_t t, f;
                            __eval_condition(exp->expression_at(0), &t, &f);

                            __state_t saved = __s;

                            __s = t;
                            bool maybe1 = __eval(exp->expression_at(1));
                            __state_t s1 = __s;

                            __s = f;
                            bool maybe2 = __eval(exp->expression_at(2));
                            __state_t s2 = __s;

                            __s = __join(s1, s2);
                            return maybe1 || maybe2;
                        }

                        for (size_t k = 0, count = exp->expression_count(); k < count; k++)
                            __eval(exp->expression_at(k));

                        return false;
                    }

                    default:
                        // The sub expressions are evaluated, whatever they are.
                        for (size_t k = 0, count = exp->expression_count(); k < count; k++)
                            __eval(exp->expression_at(k));

                        return false;
                }
            }

            // A dereference of a value that can be null.
            void __check_dereference(expression_t * exp, bool maybe)
            {
                if (!maybe)
                    return;

                type_t * type = exp->get_type();
                if (type == nullptr || !can_be_null(type))
                    return;

                __warn(exp, __c_t::nullable_dereference, exp->to_string());
            }

            // Evaluates a binary expression.
            bool __eval_binary(binary_expression_t * binary)
            {
                expression_t * exp1 = binary->exp1(), * exp2 = binary->exp2();

                switch (binary->op())
                {
                    case operator_t::member_point: {

                        // The object of a member of ?. is not checked: the chain is null when
                        // it is, and the members that follow it are not reached.
                        bool is_chain = binary->in_null_conditional_chain();

                        // A type name has no state.
                        bool maybe1 = __eval(exp1);

                        if (!binary->null_conditional)
                        {
                            bool object_is_chain = in_null_conditional_chain(exp1);

                            if (!object_is_chain)
                                __check_dereference(exp1, maybe1);
                        }

                        // After the member, the object is not null.
                        variable_t * object_variable = __variable_of(exp1);
                        if (__is_tracked(object_variable) && !is_chain)
                            __s.maybe[__key(object_variable)] = false;

                        // The member.
                        bool maybe2;
                        if (exp1 != nullptr && exp1->this_family() == expression_family_t::this_)
                            maybe2 = __eval(exp2);
                        else if (exp2->this_family() == expression_family_t::function)
                        {
                            __receiver = exp1;
                            maybe2 = __eval(exp2);
                            __receiver = nullptr;
                        }
                        else
                        {
                            // The member: the nullability of its type, through the object.
                            __ntype_t member = __ntype_of_member(exp2, __ntype_of(exp1));
                            maybe2 = member.known && member.maybe;
                        }

                        // The chain that has ?. is null when its object is null.
                        if (binary->is_null_conditional_chain_top())
                        {
                            type_t * type = binary->get_type();
                            return type != nullptr && can_be_null(type);
                        }

                        return maybe2;
                    }

                    case operator_t::assign:
                        return __eval_assign(binary);

                    case operator_t::null_coalescing: {
                        bool maybe1 = __eval(exp1);
                        __state_t saved = __s;
                        bool maybe2 = __eval(exp2);

                        // The state after: the second is evaluated only when the first is null.
                        __s = __join(saved, __s);

                        return maybe1 && maybe2;
                    }

                    case operator_t::null_coalescing_assign: {
                        bool maybe1 = __eval(exp1);
                        __state_t saved = __s;
                        bool maybe2 = __eval(exp2);

                        __s = __join(saved, __s);

                        variable_t * variable = __variable_of(exp1);
                        if (__is_tracked(variable))
                            __s.maybe[__key(variable)] = maybe1 && maybe2;

                        return maybe1 && maybe2;
                    }

                    case operator_t::logic_and:
                    case operator_t::logic_or: {
                        __state_t t, f;
                        __eval_condition(binary, &t, &f);
                        __s = __join(t, f);
                        return false;
                    }

                    case operator_t::as:
                        __eval(exp1);
                        return true;

                    case operator_t::is:
                        __eval(exp1);
                        return false;

                    default:
                        __eval(exp1);
                        __eval(exp2);
                        return false;
                }
            }

            // Evaluates an assignment.
            bool __eval_assign(binary_expression_t * binary)
            {
                expression_t * target = binary->exp1(), * value = binary->exp2();

                // The target is evaluated for its sub expressions (a.b = ...: a is dereferenced).
                variable_t * variable = __variable_of(target);
                if (variable == nullptr)
                {
                    if (target->this_family() == expression_family_t::binary)
                    {
                        binary_expression_t * member = (binary_expression_t *)target;
                        if (member->op() == operator_t::member_point)
                        {
                            bool maybe_object = __eval(member->exp1());
                            if (!member->null_conditional)
                                __check_dereference(member->exp1(), maybe_object);
                        }
                    }
                    else if (target->this_family() == expression_family_t::index)
                    {
                        __eval(target);
                    }
                }

                bool maybe = __eval(value);

                // The declared type of the target.
                type_name_t * type_name = nullptr;
                if (variable != nullptr)
                    type_name = __type_name_of(variable);
                else if (target->this_family() == expression_family_t::binary)
                    type_name = __member_type_name_of(((binary_expression_t *)target)->exp2());
                else
                    type_name = __member_type_name_of(target);

                if (variable != nullptr && variable->this_type() == variable_type_t::property)
                    type_name = ((property_variable_t *)variable)->property->type_name;

                __check_assign(value, maybe, type_name);

                // A variable that has no type name with arguments (var, the variable of a
                // foreach): its nullability is that of the value.
                if (__is_tracked(variable) && variable->this_type() == variable_type_t::local
                        && dynamic_cast<general_type_name_t *>(type_name) == nullptr
                        && dynamic_cast<array_type_name_t *>(type_name) == nullptr)
                {
                    __var_ntypes[variable] = __ntype_of(value);
                }

                if (__is_tracked(variable))
                {
                    bool declared = __declared_maybe(type_name);
                    __s.maybe[__key(variable)] = declared? maybe : false;
                    return __s.maybe[__key(variable)];
                }

                return maybe;
            }

            // Evaluates a condition: the states when it is true, and when it is false.
            void __eval_condition(expression_t * exp, __state_t * out_true, __state_t * out_false)
            {
                if (exp != nullptr && exp->this_family() == expression_family_t::expressions
                        && exp->expression_count() == 1)
                {
                    return __eval_condition(exp->expression_at(0), out_true, out_false);
                }

                if (exp != nullptr && exp->this_family() == expression_family_t::unitary)
                {
                    unitary_expression_t * unitary = (unitary_expression_t *)exp;
                    if (unitary->op() == operator_t::logic_not)
                    {
                        __eval_condition(unitary->exp(), out_false, out_true);
                        return;
                    }
                }

                if (exp != nullptr && exp->this_family() == expression_family_t::binary)
                {
                    binary_expression_t * binary = (binary_expression_t *)exp;
                    expression_t * exp1 = binary->exp1(), * exp2 = binary->exp2();

                    switch (binary->op())
                    {
                        case operator_t::logic_and: {
                            __state_t ta, fa;
                            __eval_condition(exp1, &ta, &fa);

                            __s = ta;
                            __state_t tb, fb;
                            __eval_condition(exp2, &tb, &fb);

                            *out_true = tb;
                            *out_false = __join(fa, fb);
                            __s = __join(*out_true, *out_false);
                            return;
                        }

                        case operator_t::logic_or: {
                            __state_t ta, fa;
                            __eval_condition(exp1, &ta, &fa);

                            __s = fa;
                            __state_t tb, fb;
                            __eval_condition(exp2, &tb, &fb);

                            *out_true = __join(ta, tb);
                            *out_false = fb;
                            __s = __join(*out_true, *out_false);
                            return;
                        }

                        case operator_t::equal:
                        case operator_t::not_equal: {
                            bool is_equal = binary->op() == operator_t::equal;

                            // x == null, null == x
                            expression_t * value = nullptr;
                            if (__is_null_literal(exp2))
                                value = exp1;
                            else if (__is_null_literal(exp1))
                                value = exp2;

                            if (value != nullptr)
                            {
                                __eval(value);

                                __state_t t = __s, f = __s;
                                variable_t * variable = __variable_of(value);

                                if (__is_tracked(variable))
                                {
                                    // x == null: x is null in the true branch, else it is not.
                                    (is_equal? t : f).maybe[__key(variable)] = true;
                                    (is_equal? f : t).maybe[__key(variable)] = false;
                                }

                                *out_true = t;
                                *out_false = f;
                                return;
                            }

                            break;
                        }

                        case operator_t::is: {
                            __eval(exp1);

                            __state_t t = __s, f = __s;
                            variable_t * variable = __variable_of(exp1);
                            if (__is_tracked(variable))
                                t.maybe[__key(variable)] = false;

                            *out_true = t;
                            *out_false = f;
                            return;
                        }

                        default:
                            break;
                    }
                }

                __outs.clear();
                __eval(exp);

                *out_true = __s;
                *out_false = __s;

                // A call with an out param that can be null when it returns false:
                // if (d.TryGetValue(k, out v)) ...
                if (!__outs.empty())
                {
                    expression_t * call = exp;
                    if (call->this_family() == expression_family_t::binary
                            && ((binary_expression_t *)call)->op() == operator_t::member_point)
                    {
                        call = ((binary_expression_t *)call)->exp2();
                    }

                    if (call->this_family() == expression_family_t::function)
                    {
                        for (const __out_t & out : __outs)
                        {
                            out_true->maybe[__key(out.variable)] = out.when_true;
                            out_false->maybe[__key(out.variable)] = out.when_false;
                        }
                    }

                    __outs.clear();
                }
            }

            // Returns whether the expression is null.
            static bool __is_null_literal(expression_t * exp)
            {
                if (exp == nullptr || exp->this_family() != expression_family_t::cvalue)
                    return false;

                type_t * type = exp->get_type();
                return type != nullptr && is_null(type);
            }

            ////////// ////////// ////////// ////////// //////////
            // statements

            // Executes a loop: the body is analyzed twice, the first time without warnings, to
            // know the state at the beginning of it (the entry, and the end of the body).
            void __exec_loop(expression_t * condition, statement_t * body, expression_t * increase,
                             bool do_while)
            {
                __state_t entry = __s;
                __state_t after;

                // The first pass without warnings, the second with them.
                __state_t head = entry;

                for (int pass = 0; pass < 2; pass++)
                {
                    if (pass == 0)
                        __silent++;

                    __loop_t loop;
                    __loops.push_back(&loop);

                    __s = head;

                    __state_t exit_state;

                    if (do_while)
                    {
                        __exec(body);
                        __s = __join(__s, loop.continues);

                        __state_t t, f;
                        if (condition != nullptr)
                            __eval_condition(condition, &t, &f);
                        else
                            t = f = __s;

                        exit_state = f;
                        __s = t;
                    }
                    else
                    {
                        __state_t t, f;
                        if (condition != nullptr)
                            __eval_condition(condition, &t, &f);
                        else
                        {
                            t = __s;
                            f = __unreachable();
                        }

                        exit_state = f;
                        __s = t;

                        __exec(body);
                        __s = __join(__s, loop.continues);

                        if (increase != nullptr && __s.reachable)
                            __eval(increase);
                    }

                    __loops.pop_back();

                    if (pass == 0)
                    {
                        __silent--;
                        head = __join(entry, __s);
                    }
                    else
                    {
                        after = __join(exit_state, loop.breaks);
                    }
                }

                __s = after;
            }

            // Executes a statement.
            void __exec(statement_t * statement)
            {
                if (statement == nullptr)
                    return;

                if (auto * group = dynamic_cast<statement_group_t *>(statement))
                {
                    for (statement_t * st : *group)
                        __exec(st);
                }
                else if (auto * st = dynamic_cast<expression_statement_t *>(statement))
                {
                    __eval(st->expression);
                }
                else if (auto * st = dynamic_cast<defination_statement_t *>(statement))
                {
                    for (defination_statement_item_t * item : st->items)
                    {
                        type_name_t * type_name = item->variable->type_name;

                        if (item->expression != nullptr)
                        {
                            bool maybe = __eval(item->expression);
                            __check_assign(item->expression, maybe, type_name);

                            // var: the nullability is that of the value.
                            if (dynamic_cast<general_type_name_t *>(type_name) == nullptr
                                    && dynamic_cast<array_type_name_t *>(type_name) == nullptr)
                            {
                                __var_ntypes[item->variable] = __ntype_of(item->expression);
                            }

                            __s.maybe[__key(item->variable)] = __declared_maybe(type_name)? maybe : false;
                        }
                        else
                        {
                            __s.maybe[__key(item->variable)] = __declared_maybe(type_name);
                        }
                    }
                }
                else if (auto * st = dynamic_cast<if_statement_t *>(statement))
                {
                    __state_t t, f;
                    __eval_condition(st->condition, &t, &f);

                    __s = t;
                    __exec(st->if_body);
                    __state_t s1 = __s;

                    __s = f;
                    __exec(st->else_body);

                    __s = __join(s1, __s);
                }
                else if (auto * st = dynamic_cast<while_statement_t *>(statement))
                {
                    __exec_loop(st->condition, st->body, nullptr, false);
                }
                else if (auto * st = dynamic_cast<do_while_statement_t *>(statement))
                {
                    __exec_loop(st->condition, st->body, nullptr, true);
                }
                else if (auto * st = dynamic_cast<for_statement_t *>(statement))
                {
                    if (st->defination_initialize != nullptr)
                        __exec(st->defination_initialize);

                    if (st->initialize != nullptr)
                        __eval(st->initialize);

                    __exec_loop(st->condition, st->body, st->increase, false);
                }
                else if (auto * st = dynamic_cast<for_each_statement_t *>(statement))
                {
                    if (st->lowered != nullptr)
                        __exec(st->lowered);
                    else
                    {
                        __eval(st->iterator);
                        __exec_loop(nullptr, st->body, nullptr, false);
                    }
                }
                else if (auto * st = dynamic_cast<using_statement_t *>(statement))
                {
                    if (st->lowered != nullptr)
                        __exec(st->lowered);
                    else
                    {
                        __eval(st->initializer);
                        __exec(st->body);
                    }
                }
                else if (auto * st = dynamic_cast<return_statement_t *>(statement))
                {
                    if (st->expression != nullptr)
                    {
                        bool maybe = __eval(st->expression);

                        if (maybe && __declared_not_null(__method->type_name))
                        {
                            type_t * from = st->expression->get_type();
                            if (from != nullptr && is_null(from))
                            {
                                __warn(st->expression, __c_t::nullable_null_literal,
                                    to_type(__method->type_name)->to_string());
                            }
                            else
                            {
                                __warn(st->expression, __c_t::nullable_return);
                            }
                        }
                    }

                    // A constructor that returns.
                    __check_fields();

                    __s = __unreachable();
                }
                else if (auto * st = dynamic_cast<throw_statement_t *>(statement))
                {
                    __eval(st->expression);
                    __s = __unreachable();
                }
                else if (dynamic_cast<break_statement_t *>(statement))
                {
                    if (!__loops.empty())
                        __loops.back()->breaks = __join(__loops.back()->breaks, __s);

                    __s = __unreachable();
                }
                else if (dynamic_cast<continue_statement_t *>(statement))
                {
                    if (!__loops.empty())
                        __loops.back()->continues = __join(__loops.back()->continues, __s);

                    __s = __unreachable();
                }
                else if (dynamic_cast<goto_statement_t *>(statement))
                {
                    __s = __unreachable();
                }
                else if (auto * st = dynamic_cast<yield_statement_t *>(statement))
                {
                    if (st->expression != nullptr)
                        __eval(st->expression);
                }
                else if (auto * st = dynamic_cast<try_statement_t *>(statement))
                {
                    __state_t entry = __s;

                    __exec(st->try_statement);
                    __state_t after = __s;

                    // A catch starts from any state of the try.
                    __state_t catch_entry = __join(entry, after);

                    for (catch_t * c : st->catches)
                    {
                        __s = catch_entry;
                        if (c->variable != nullptr)
                            __s.maybe[__key(c->variable)] = false;

                        __exec(c->body);
                        after = __join(after, __s);
                    }

                    __s = after;

                    if (st->finally_statement != nullptr)
                    {
                        __state_t saved = __s;
                        __s = __join(catch_entry, saved);
                        __exec(st->finally_statement);

                        // The state of the end of the finally is for the code after it.
                        __s = saved.reachable? __s : saved;
                    }
                }
                else if (auto * st = dynamic_cast<switch_statement_t *>(statement))
                {
                    __eval(st->expression);

                    __state_t entry = __s;
                    __state_t after = __unreachable();
                    bool has_default = false;

                    __loop_t loop;
                    __loops.push_back(&loop);

                    for (case_t * c : st->cases)
                    {
                        if (c->contains_default())
                            has_default = true;

                        __s = entry;

                        if (c->statements != nullptr)
                        {
                            for (statement_t * st2 : *c->statements)
                                __exec(st2);
                        }

                        after = __join(after, __s);
                    }

                    __loops.pop_back();

                    after = __join(after, loop.breaks);

                    if (!has_default)
                        after = __join(after, entry);

                    __s = after;
                }
                // Other statements (empty, typedef, ...) have nothing to analyze.
            }
        };
    }

}   // namespace X_ROOT_NS::modules::compile

namespace X_ROOT_NS::modules::core {

    // Checks the arguments of a generic type name.
    void check_nullable_constraints(type_name_t * type_name, code_element_t * element,
                                    logger_t & logger)
    {
        auto * general = dynamic_cast<compile::general_type_name_t *>(type_name);
        if (general == nullptr || general->units.empty() || element == nullptr
                || !compile::__enabled_at(element->get_code_unit()))
            return;

        type_t * type = to_type(type_name);
        if (type == nullptr || type->this_gtype() != gtype_t::generic)
            return;

        general_type_t * template_ = ((generic_type_t *)type)->template_;
        if (template_ == nullptr || template_->params == nullptr)
            return;

        compile::name_unit_t * unit = general->units[general->units.size() - 1];
        if (unit == nullptr || unit->args == nullptr)
            return;

        for (size_t k = 0; k < unit->args->size() && k < template_->params->size(); k++)
        {
            generic_arg_t * arg = (*unit->args)[k];
            if (arg == nullptr || arg->type_name == nullptr || !arg->type_name->nullable)
                continue;

            generic_param_t * param = (*template_->params)[k];
            if (template_->constraints == nullptr)
                continue;

            for (generic_constraint_t * constraint : *template_->constraints)
            {
                if (constraint->param_name != param->name)
                    continue;

                int ttype = (int)constraint->cttype;

                // notnull, or class (that is not class?).
                bool not_null = (ttype & (int)generic_constraint_ttype_t::notnull_) != 0
                    || ((ttype & (int)generic_constraint_ttype_t::class_) != 0
                        && (ttype & (int)generic_constraint_ttype_t::nullable_) == 0);

                // A type that is not annotated (where T : Base) cannot be null, Base? can.
                for (type_name_t * constraint_type : constraint->type_names)
                {
                    if (constraint_type != nullptr && !constraint_type->nullable
                            && to_type(constraint_type) != nullptr
                            && to_type(constraint_type)->this_gtype() != gtype_t::generic_param)
                    {
                        not_null = true;
                    }
                }

                if (not_null)
                {
                    compile::ast_log(logger, element, compile::common_log_code_t::nullable_constraint,
                        to_type(arg->type_name)->to_string(), param->name, template_->name);
                    break;
                }
            }
        }
    }

    // Analyzes the constructors that have no body.
    void analyze_nullable_constructors(assembly_t * assembly, logger_t & logger)
    {
        assembly->types.each_general_types([&](general_type_t * type) {

            for (method_t * method : type->methods)
            {
                if (method->trait == method_trait_t::constructor && method->body == nullptr
                        && !method->is_static())
                {
                    compile::__nullable_analyzer_t(method, logger).analyze();
                }
            }
        });
    }

    // Analyzes the null states of a method, and warns.
    void analyze_nullable(method_t * method, logger_t & logger)
    {
        if (method == nullptr || method->body == nullptr)
            return;

        compile::__nullable_analyzer_t(method, logger).analyze();
    }

}   // namespace X_ROOT_NS::modules::core
