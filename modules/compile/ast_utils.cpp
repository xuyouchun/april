
#include <compile.h>

namespace X_ROOT_NS::modules::compile {

    typedef common_log_code_t __c_t;

    // Ascertains type.
    type_t * __ascertain_type(ast_context_t & cctx, ast_walk_context_t & wctx, name_t name,
                                                generic_args_t * args = nullptr);

    // Ascertains type.
    type_t * __ascertain_type(ast_context_t & cctx, ast_walk_context_t & wctx,
                                                general_type_name_t * type_name);

    ////////// ////////// ////////// ////////// //////////

    // Converts to name.
    static name_t __to_name(const string_t & s)
    {
        return name_t(__SPool.to_sid(s));
    }

    // Returns vtype of expression.
    static vtype_t __vtype_of(expression_t * exp)
    {
        _A(exp != nullptr);

        type_t * type = exp->get_type();
        if (type != nullptr)
            return type->this_vtype();

        return exp->get_vtype();
    }

    // Converts multi-name to sid.
    sid_t mname_to_sid(const mname_t * mname)
    {
        if (mname == nullptr)
            return sid_t::null;

        return mname->sid;
    }

    // Converts namespace to multi-name.
    const mname_t * ns_to_full_name(namespace_t * ns)
    {
        return ns? ns->full_name : nullptr;
    }

    // Returns expression type.
    type_t * get_expression_type(ast_context_t & cctx, expression_t * expression)
    {
        return get_expression_type(expression);
    }

    // Pick type from current context, includes.
    // int a = [exp];       => exp type is int
    // func([exp]);         => exp type is argument type
    type_t * pick_type_from_current_context(expression_t * exp)
    {
        _A(exp != nullptr);

        expression_t * parent_exp = exp->parent;

        if (parent_exp == nullptr)
            return nullptr;

        expression_family_t family = parent_exp->this_family();

        // An item of a tuple: the tuple tells the type of it.
        if (family == expression_family_t::tuple)
            return nullptr;

        // An item of the initializer of an array (new Tuple<Int32, String>[] { (1, "2") }): the type of the elements.
        if (family == expression_family_t::new_array)
            return ((new_array_expression_t *)parent_exp)->get_element_type();

        // An argument of a call or of a new expression (Take(Method), new Thread(Method)): the type that it is converted to
        // is that of the parameter, which the overload tells later, not the one of what the call is assigned to.
        if (family == expression_family_t::function || family == expression_family_t::new_)
        {
            arguments_t * arguments = ((__arguments_expression_base_t *)parent_exp)->arguments();

            if (arguments != nullptr)
            {
                for (argument_t * argument : *arguments)
                {
                    if (argument->expression == exp)
                        return nullptr;
                }
            }
        }

        if (family == expression_family_t::binary)
        {
            binary_expression_t * binary_exp = (binary_expression_t *)parent_exp;
            if (binary_exp->op() == operator_t::assign)
            {
                expression_t * exp1 = binary_exp->exp1();
                return exp1->get_type();
            }

            return pick_type_from_current_context(parent_exp);
        }
        else if (family == expression_family_t::type_cast)
        {
            type_cast_expression_t * type_cast_exp = (type_cast_expression_t *)parent_exp;
            _A(type_cast_exp->type_name != nullptr);
            return type_cast_exp->type_name->type;
        }

        return pick_type_from_current_context(parent_exp);
    }

    // Fills argument types.
    void __fill_atypes(ast_context_t & cctx, atypes_t & atypes, arguments_t * arguments)
    {
        if (arguments == nullptr)
            return;

        for (argument_t * argument : *arguments)
        {
            type_t * type = get_expression_type(cctx, argument->expression);

            // The items of a params generic parameter (items ...): their type is the generic
            // parameter, as it is written (TArgs ... items), not the address that they are.
            if (argument->atype == param_type_t::extends && argument->expression != nullptr
                    && argument->expression->this_family() == expression_family_t::name)
            {
                variable_t * variable = ((name_expression_t *)argument->expression)->variable;
                if (variable != nullptr && variable->this_type() == variable_type_t::param)
                {
                    param_t * param = ((param_variable_t *)variable)->param;
                    if (param->ptype == param_type_t::extends && param->get_type() != nullptr)
                        type = param->get_type();
                }
            }

            // The items of a tuple of the items of a params generic parameter (tuple ...): their types
            // are those of the parameter.
            if (argument->atype == param_type_t::extends)
            {
                if (generic_param_t * pack = get_tuple_pack_param(type))
                    type = pack;
            }

            atype_t atype(type, argument->atype);
            atype.lambda = get_lambda_method(argument->expression);

            atypes.push_back(atype);
        }
    }

    // Convets to mname_t *.
    static const mname_t * __to_mname(const emname_t * ns)
    {
        if (ns == nullptr)
            return nullptr;

        return ns->mname;
    }

    // Converts to mname_t *.
    static const mname_t * __to_mname(const using_namespace_t * ns)
    {
        if (ns == nullptr)
            return nullptr;

        return __to_mname(ns->ns);
    }

    ////////// ////////// ////////// ////////// //////////
    // __multi_units_t

    // Multi-units container.
    struct __multi_units_t
    {
        typedef general_type_name_t::unit_iterator_t __unit_iterator_t;

        // Constructor.
        __multi_units_t(const mname_t * mname, __unit_iterator_t begin, __unit_iterator_t end)
            : __mname(mname), __begin(begin), __end(end)
        { }

        // Constructor.
        __multi_units_t(const mname_t * mname, general_type_name_t * type_name)
            : __multi_units_t(mname, type_name->begin(), type_name->end())
        { }

        const mname_t * __mname;
        __unit_iterator_t __begin, __end;

        // Returns part count.
        size_t __mname_part_count() const
        {
            return __mname? __mname->part_count() : 0;
        }

        // Iterator of units.
        struct __itor_t : iterator_base_t<__itor_t>
        {
            // Constructor.
            __itor_t(const __multi_units_t & units, int index) : __units(units)
            {
                __assign_index(index);
            }

            // Returns whether the index is in the units.
            bool __in_mname() { return __index < __units.__mname_part_count(); }

            const __multi_units_t & __units;
            int __index;

            // Assigns index.
            void __assign_index(int index)
            {
                __index = index;

                if (__in_mname())
                    new ((void *)unit) name_unit_t(name_t((*__units.__mname)[index]));
                else
                    punit = *(__units.__begin + (index - __units.__mname_part_count()));
            }

            union
            {
                name_unit_t * punit;
                byte_t        unit[sizeof(name_unit_t)];
            };

            // Returns typename unit.
            name_unit_t * get()
            {
                if (__in_mname())
                    return (name_unit_t *)&unit;

                return punit;
            }

            // Moves to next iterator.
            void increase(int inc)
            {
                __assign_index(__index + inc);
            }

            // Returns whether two iterators are equals.
            bool equals(const __itor_t & it) const
            {
                return __index == it.__index;
            }
        };

        // Returns the begin iterator.
        __itor_t begin() const { return __itor_t(*this, 0); }

        // Returns the end iterator.
        __itor_t end() const
        {
            return __itor_t(*this, __mname_part_count() + (__end - __begin));
        }

        // Converts to string.
        operator string_t() const
        {
            stringstream_t ss;

            int index = 0;
            for (__itor_t it = this->begin(), it_end = this->end(); it != it_end; it++)
            {
                if (index++ > 0)
                    ss << _T(".");

                ss << *it;
            }

            return ss.str();
        }
    };

    // Writes units to a stream.
    template<typename stream_t>
    stream_t & operator << (stream_t & stream, __multi_units_t & units)
    {
        int index = 0;
        for (auto it : units)
        {
            if (index++ > 0)
                stream << _T(".");

            stream << _str(*it);
        }

        return stream;
    }

    ////////// ////////// ////////// ////////// //////////
    // type resolver

    // Ascertain type error codes.
    X_ENUM_INFO(ascertain_type_error_t)

        // Unknown type.
        X_D(unknown_type,           _T("unknown type: %1%"))

        // Element type empty.
        X_C(element_type_empty,     _T("array element type empty"))

        // Element type not supported.
        X_C(element_type_not_supported,  _T("array element type not supported"))

        // Typedef unsolved.
        X_C(type_def_unsolved,      _T("type def unsolved"))

        // The alias is not accessible.
        X_C(type_def_inaccessible,  _T("type def inaccessible"))

    X_ENUM_INFO_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Type resolver.
    class __type_resolver_t
    {
    public:

        // Constructor.
        __type_resolver_t(ast_context_t & cctx, ast_walk_context_t & wctx,
                                            general_type_name_t * type_name)
            : __cctx(cctx), __wctx(wctx), __type_name(type_name)
        { }

        // Ascertains type.
        type_t * ascertain_type()
        {
            return __ascertain_type();
        }

    private:

        #define __EmptyTypeDef  ((type_def_t *)-1)

        ast_context_t & __cctx;
        ast_walk_context_t & __wctx;
        general_type_t * __current_type;
        method_t * __current_method;
        general_type_name_t * __type_name;
        type_def_t * __current_type_def = __EmptyTypeDef;
        module_t * __current_module;

        struct __find_ret_t { type_t * type;  bool success; };

        // Ascertains type.
        type_t * __ascertain_type()
        {
            __current_type = as<general_type_t *>(__wctx.current_type());
            __current_method = __wctx.current_method();
            __current_module = __wctx.current_module();

            type_t * type = __ascertain_param_type();
            if (type != nullptr)
                return type;

            return __ascertain_general_type();
        }

        // Ascertains param types.
        generic_param_t * __ascertain_param_type()
        {
            if (__type_name->units.size() != 1)
                return nullptr;

            name_unit_t * unit = __type_name->units[0];
            if (unit == nullptr || unit->is_generic())
                return nullptr;

            name_t name = unit->name;
            
            if (__current_method != nullptr)
            {
                generic_param_t * param = __current_method->find_param(name);
                if (param != nullptr)
                    return param;
            }

            if (__current_type == nullptr)
                return nullptr;

            for (general_type_t * type = __current_type; type != nullptr;
                                    type = as<general_type_t *>(type->host_type))
            {
                generic_param_t * param = type->find_param(name);
                if (param != nullptr)
                    return param;
            }

            return nullptr;
        }

        // Ascertains general type.
        type_t * __ascertain_general_type()
        {
            const mname_t * current_ns = ns_to_full_name(__wctx.current_namespace());
            mname_operate_context_t op_ctx = to_mname_operate_context();

            if (__type_name->global_type == global_type_t::global)
                return __search_global_type(__type_name->begin(), __type_name->end()).type;

            if (__type_name->global_type == global_type_t::root)
            {
                __find_ret_t r = __find_type(
                    &__wctx.assembly, nullptr, __type_name->begin(), __type_name->end()
                );

                if (r.success)
                    return r.type;

                return __search_global_type(__type_name->begin(), __type_name->end()).type;
            }

            // nest type
            if (__current_type != nullptr)
            {
                __find_ret_t r = __find_nest_type(&__wctx.assembly, current_ns,
                    __current_type, __type_name->begin(), __type_name->end()
                );

                if (r.success)
                    return r.type;
            }

            // from the typedefs of the blocks (typedef Name = Type; in a method)
            variable_region_t * region = __wctx.current_region();

            if (region != nullptr && !__type_name->units.empty())
            {
                name_unit_t * first = __type_name->units[0];
                type_def_t * type_def = region->find_type_def(first->name, first->generic_args_count());

                if (type_def != nullptr)
                {
                    type_t * type = __expand_type_def(type_def, first, nullptr);

                    if (__type_name->units.size() == 1)
                        return type;

                    __find_ret_t r = __find_nest_type(&__wctx.assembly, current_ns, type,
                                                      __type_name->begin() + 1, __type_name->end());
                    if (r.success)
                        return r.type;
                }
            }

            // from current & parent namespace
            for (const mname_t * ns = current_ns; ; ns = __wctx.parent_of(ns))
            {
                __find_ret_t r = __find_type(&__wctx.assembly, ns, __type_name->begin(),
                                                                   __type_name->end());
                if (r.success)
                    return r.type;

                if (ns == nullptr)
                    break;
            }

            __find_ret_t r = __search_global_type(__type_name->begin(), __type_name->end());

            if (r.success)
                return r.type;

            // from using namespace
            document_t * document = __wctx.current_document();
            if (document != nullptr)
            {
                for (using_namespace_t * using_ns : document->all_using_namespaces())
                {
                    if (using_ns->alias.empty())
                    {
                        __multi_units_t units(__to_mname(using_ns), __type_name);
                        r = __search_global_type(units.begin(), units.end());
                    }
                    else
                    {
                        if (__type_name->units.size() >= 2
                                && *__type_name->units[0] == using_ns->alias)
                        {
                            __multi_units_t units(to_mname(using_ns->ns),
                                    __type_name->begin() + 1, __type_name->end()
                            );
                            r = __search_global_type(units.begin(), units.end());
                        }
                    }

                    if (r.success)
                        return r.type;
                }
            }

            return nullptr;
        }

        // Transforms generic argument.
        type_t * __transform_generic_arg(generic_arg_t * arg)
        {
            if (arg == nullptr || arg->type_name == nullptr)
                return nullptr;

            type_t * type = arg->type_name->type;
            if (type == nullptr)
                return nullptr;

            if (type->this_gtype() == gtype_t::type_def_param)
            {
                if (__get_current_type_def() != nullptr)
                    return type;
            }

            return type;
        }

        // Gets current typedef.
        type_def_t * __get_current_type_def()
        {
            if (__current_type_def == __EmptyTypeDef)
                __current_type_def = __wctx.current_type_def();

            return __current_type_def;
        }

        // Gets type by namespace and name.
        type_t * __get_type(assembly_t * assembly, sid_t ns, sid_t name,
                size_t generic_param_count, type_t * host_type, name_unit_t * unit = nullptr)
        {
            type_t * raw_host_type = host_type;
            if (host_type != nullptr && host_type->this_gtype() == gtype_t::generic)
            {
                raw_host_type = ((generic_type_t *)host_type)->template_;
                assembly = ((general_type_t *)raw_host_type)->assembly;
            }

            // from type_def params
            type_def_t * current_type_def;
            if (!generic_param_count && (current_type_def = __get_current_type_def()))
            {
                type_def_param_t * param = current_type_def->find_param((name_t)name);
                if (param != nullptr)
                    return param;
            }

            // from general_type
            type_t * type = assembly->types.get_general_type(
                ns, name, generic_param_count, raw_host_type
            );

            if (type != nullptr)
            {
                if (unit != nullptr && unit->generic_args_count() > 0)
                    type = __new_generic_type((general_type_t *)type, unit, host_type);

                return type;
            }

            // from type_def
            type_def_t * type_def = assembly->types.get_type_def(
                ns, name, generic_param_count, raw_host_type
            );

            // The aliases of another assembly are those that are public.
            if (type_def && assembly != &__wctx.assembly && !type_def->is_public())
                type_def = nullptr;

            if (type_def && type_def->type_name)
                return __expand_type_def(type_def, unit, host_type);

            return nullptr;
        }

        // The type that an alias names, with the args of the name (M<String> of M<T> = List<T>).
        type_t * __expand_type_def(type_def_t * type_def, name_unit_t * unit, type_t * host_type)
        {
            // An alias that is a member of a type has its access.
            if (type_def->host_type != nullptr)
            {
                type_t * current_type = __wctx.current_type();
                bool accessible = current_type != nullptr? check_access(current_type, (member_t *)type_def)
                                                         : check_access(&__wctx.assembly, (member_t *)type_def);
                if (!accessible)
                    throw _E(ascertain_type_error_t::type_def_inaccessible);
            }

            // The alias may not be walked yet: its type is made now.
            type_t * type = type_def->ensure_type(__wctx);
            if (type == nullptr)
                throw _E(ascertain_type_error_t::type_def_unsolved);

            // The names of the items of a tuple that the typedef names, and those in its generic arguments.
            if (type_def->type_name != nullptr
                    && !__type_name->units.empty() && __type_name->units[__type_name->units.size() - 1] == unit)
            {
                __type_name->item_names = type_def->type_name->item_names;

                // The arguments of the name are not those of the type that it names: a param of the typedef is the
                // argument that the name is given.
                __type_name->arg_type_names.clear();
                __type_name->args_known = true;

                for (size_t index = 0, count = type_def->type_name->arg_name_count(); index < count; index++)
                {
                    type_name_t * arg = type_def->type_name->arg_name_at(index);

                    general_type_name_t * general = dynamic_cast<general_type_name_t *>(arg);
                    if (general != nullptr && type_def->params != nullptr && unit->args != nullptr
                            && general->units.size() == 1 && general->units[0] != nullptr
                            && general->units[0]->args == nullptr)
                    {
                        for (size_t k = 0; k < type_def->params->size(); k++)
                        {
                            if ((*type_def->params)[k]->name == general->units[0]->name)
                            {
                                arg = k < unit->args->size() && (*unit->args)[k] != nullptr?
                                                                    (*unit->args)[k]->type_name : nullptr;
                                break;
                            }
                        }
                    }

                    __type_name->arg_type_names.push_back(arg);
                }
            }

            return __assign_type_def_param(type, unit, host_type);
        }

        // Finds nest type.
        template<typename itor_t>
        __find_ret_t __find_nest_type(assembly_t * assembly, const mname_t * ns,
                                                type_t * type, itor_t begin, itor_t end)
        {
            if (begin == end)
                return __find_ret_t { type, type != nullptr };

            while (type != nullptr)
            {
                type_t * nest_type = type;

                bool found = false;

                for (name_unit_t * unit : _range(begin, end))
                {
                    generic_type_t * generic_type = nullptr;
                    type_t * host_type = nest_type;

                    if (nest_type->this_gtype() == gtype_t::generic)
                    {
                        generic_type = (generic_type_t *)nest_type;
                        nest_type = generic_type->template_;
                    }

                    type_t * nest_type0 = __get_type(assembly, to_sid(ns),
                            (sid_t)unit->name, unit->generic_args_count(), host_type, unit);

                    if (nest_type0 != nullptr)
                    {
                        found = true;

                        if ((generic_type != nullptr || unit->generic_args_count() > 0)
                            && nest_type0->host_type == nest_type
                            && nest_type0->this_gtype() == gtype_t::general)
                        {
                            general_type_t * template_ = (general_type_t *)nest_type0;
                            _A(template_->params_count() == unit->generic_args_count());

                            nest_type0 = __new_generic_type(template_, unit, host_type);
                        }

                        nest_type = nest_type0;
                    }
                    else if (found)
                    {
                        return __find_ret_t { nullptr, false };
                    }
                    else
                    {
                        break;
                    }
                }

                if (found)
                    return __find_ret_t { nest_type, true };

                type = as<general_type_t *>(type->host_type);
            }

            return __find_ret_t { nullptr, false };
        }

        // Search global types.
        template<typename itor_t>
        __find_ret_t __search_global_type(itor_t begin, itor_t end)
        {
            document_t * document = __wctx.current_document();
            if (document == nullptr)
                return __find_ret_t { nullptr, false };

            // From current assembly.
            __find_ret_t r = __search_type(&__wctx.assembly, nullptr, begin, end);
            if (r.success)
                return r;

            // From all imports.
            for (import_t * import : document->all_imports())
            {
                int prefix_count = __import_starts_with(import, begin, end);
                if (prefix_count > 0)
                {
                    itor_t it = begin + prefix_count;
                    assembly_t * assembly = __wctx.load_assembly(
                        to_mname(import->package), to_mname(import->assembly)
                    );

                    if (assembly != nullptr)
                    {
                        __find_ret_t r = __search_type(assembly, nullptr, it, end);
                        if (r.success)
                            return r;
                    }
                }
            }

            return __find_ret_t { nullptr, false };
        }

        // Determines whether imports starts with specified prefix.
        template<typename itor_t>
        int __import_starts_with(import_t * import, itor_t begin, itor_t end)
        {
            if (begin == end)
                return 0;

            if (!import->alias.empty())
            {
                if (**begin == import->alias)
                    return 1;
            }
            else if (__starts_with(to_mname(import->assembly), begin, end))
            {
                return to_mname(import->assembly)->part_count();
            }

            return 0;
        }

        // Determines whether the assembly name starts with specified prefix.
        template<typename itor_t>
        bool __starts_with(const mname_t * assembly_name, itor_t begin, itor_t end)
        {
            if (assembly_name == nullptr)
                return false;

            itor_t it = begin;
            for (sid_t sid : *assembly_name)
            {
                if (it == end || **it++ != sid)
                    return false;
            }

            return it != end;
        }

        // Searches types from specified assembly.
        template<typename itor_t>
        __find_ret_t __search_type(assembly_t * assembly, const mname_t * ns,
                                                    itor_t begin, itor_t end)
        {
            if (begin == end)
                return __find_ret_t { nullptr, false };

            itor_t it = begin;

            do
            {
                __find_ret_t r = __find_type(assembly, ns, it, end);
                if (r.success)
                    return r;

            } while ((it + 1) != end && !(*it)->is_generic()
                && (ns = __wctx.combine(ns, (*it++)->name), true));

            return __find_ret_t { nullptr, false };
        }

        // Finds type from specified assembly.
        template<typename itor_t>
        __find_ret_t __find_type(assembly_t * assembly, const mname_t * ns,
                                                    itor_t type_begin, itor_t type_end)
        {
            for (itor_t it = type_begin; it != type_end; it++)
            {
                name_unit_t * unit = *it;
                type_t * type = __get_type(assembly, mname_to_sid(ns),
                    (sid_t)unit->name, unit->generic_args_count(), nullptr, unit
                );

                __find_ret_t r = __find_nest_type(assembly, ns, type, it + 1, type_end);
                if (r.success)
                    return r;

                if (unit->generic_args_count() == 0)
                    ns = __wctx.combine(ns, unit->name);
                else
                    break;
            }

            return __find_ret_t { nullptr, false };
        }

        // Assigns type def param.
        type_t * __assign_type_def_param(type_t * type, name_unit_t * unit,
                                                type_t * host_type = nullptr)
        {
            if (type == nullptr)
                return type;

            gtype_t gtype = type->this_gtype();
            if (gtype == gtype_t::type_def_param)
            {
                type_def_param_t * param = (type_def_param_t *)type;
                _A(unit != nullptr && param->index < unit->generic_args_count());
                return __transform_generic_arg((*unit)[param->index]);
            }
            else if (gtype == gtype_t::array)
            {
                array_type_t * array_type = (array_type_t *)type;
                type_t * element_type1 = __assign_type_def_param(array_type->element_type,
                                                                    unit, host_type);
                if (element_type1 != array_type->element_type)
                {
                    return __new_array_type(element_type1, array_type->dimension);
                }

                return type;
            }
            else if (gtype == gtype_t::generic_param)
            {
                generic_param_t * param = (generic_param_t *)type;
                for (type_t * t = host_type; t != nullptr; t = t->host_type)
                {
                    if (t->this_gtype() == gtype_t::generic)
                    {
                        generic_type_t * generic_type = (generic_type_t *)t;
                        generic_param_t * p = generic_type->template_->find_param(param->name);
                        if (p != nullptr)
                        {
                            _A(p->index < generic_type->args.size());
                            return generic_type->args[p->index];
                        }
                    }
                }

                return type;
            }

            std::stack<type_t *> type_stack;
            type_t * current_type = type;

            for (; current_type; current_type = current_type->host_type)
            {
                type_stack.push(current_type);
            }

            bool break_away = false;
            for (; !type_stack.empty(); type_stack.pop())
            {
                type_t * t = type_stack.top();
                gtype_t gtype = t->this_gtype();

                if (break_away || gtype == gtype_t::generic)
                {
                    generic_type_t * generic_type = (generic_type_t *)t;
                    type_collection_t types;

                    for (type_t * t0 : generic_type->args)
                    {
                        // The pack of the alias is all the arguments that follow the ones before it.
                        if (t0 != nullptr && t0->this_gtype() == gtype_t::type_def_param
                                && ((type_def_param_t *)t0)->extends)
                        {
                            for (size_t k = ((type_def_param_t *)t0)->index; k < unit->generic_args_count(); k++)
                                types.push_back(__transform_generic_arg((*unit)[k]));

                            break_away = true;
                            continue;
                        }

                        type_t * t1 = __assign_type_def_param(t0, unit, host_type);
                        types.push_back(t0 == t1? t0 : (break_away = true, t1));
                    }

                    if (break_away)
                    {
                        xtype_collection_t xtypes(types);
                        current_type = __new_generic_type(
                            generic_type->template_, xtypes, current_type
                        );

                        continue;
                    }
                }

                current_type = t;
            }

            return current_type;
        }

        // Creates a new generic type.
        generic_type_t * __new_generic_type(general_type_t * template_, name_unit_t * unit,
                                                                        type_t * host_type)
        {
            type_collection_t types;
            xtype_collection_t xtypes(types);

            if (unit->args != nullptr)
            {
                al::transform(*unit->args, std::back_inserter(types), [this](generic_arg_t * arg) {
                    return typex_t(__transform_generic_arg(arg), arg->atype);
                });
            }

            return __new_generic_type(template_, xtypes, host_type);
        }

        // Creates a new generic type.
        generic_type_t * __new_generic_type(general_type_t * template_, xtype_collection_t & xtypes,
                                                                        type_t * host_type)
        {
            generic_type_t * type = __wctx.assembly.types.new_generic_type(
                template_, xtypes, host_type
            );

            return type;
        }

        // Creates a new array type.
        array_type_t * __new_array_type(type_t * element_type, dimension_t dimension)
        {
            return __wctx.assembly.types.new_array_type(element_type, dimension);
        }

        #undef __EmptyTypeDef
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Ascertains type.
    type_t * __ascertain_type(ast_context_t & cctx, ast_walk_context_t & wctx,
                                                general_type_name_t * type_name)
    {
        return __type_resolver_t(cctx, wctx, type_name).ascertain_type();
    }

    // Ascertains type.
    type_t * ascertain_type(ast_context_t & cctx, ast_walk_context_t & wctx,
                                                general_type_name_t * type_name)
    {
        type_t * type = __ascertain_type(cctx, wctx, type_name);

        if (type == nullptr)
            throw _ED(ascertain_type_error_t::unknown_type, type_name);

        return type;
    }

    // Ascertains type.
    type_t * __ascertain_type(ast_context_t & cctx, ast_walk_context_t & wctx, name_t name,
                                generic_args_t * args)
    {
        general_type_name_t type_name;
        name_unit_t unit(name, args);
        type_name.units.push_back(&unit);

        return __ascertain_type(cctx, wctx, &type_name);
    }

    // Ascertains type.
    type_t * ascertain_type(ast_context_t & cctx, ast_walk_context_t & wctx, name_t name)
    {
        type_t * type = __ascertain_type(cctx, wctx, name);

        if (type == nullptr)
            throw _ED(ascertain_type_error_t::unknown_type, name);

        return type;
    }

    // Ascertains type.
    type_t * ascertain_type(ast_context_t & cctx, ast_walk_context_t & wctx,
                                                array_type_name_t * type_name)
    {
        _A(type_name != nullptr);
        _A(type_name->element_type_name != nullptr);

        type_name_t * element_type_name = type_name->element_type_name;
        type_t * element_type;

        general_type_name_t * general_type_name;
        array_type_name_t   * array_type_name;

        if ((general_type_name = as<general_type_name_t *>(element_type_name)) != nullptr)
            element_type = ascertain_type(cctx, wctx, general_type_name);
        else if ((array_type_name = as<array_type_name_t *>(element_type_name)) != nullptr)
            element_type = ascertain_type(cctx, wctx, array_type_name);
        else
            throw _E(ascertain_type_error_t::element_type_not_supported);

        return wctx.assembly.types.new_array_type(element_type, type_name->dimensions.size());
    }

    ////////// ////////// ////////// ////////// //////////

    // Type for walks expression.
    class __walk_expression_t
    {
    public:

        // Constructor.
        __walk_expression_t(ast_context_t & cctx, ast_walk_context_t & wctx)
            : __cctx(cctx), __wctx(wctx), __region(wctx.current_region())
        { }

        // Walks expression.
        void walk(expression_t * exp)
        {
            _A(exp != nullptr);
            __walk(exp);
        }

        // The tuple literal is made for the type that the context wants: (1, "2") that is given to
        // an ITuple<Int32, String> is made in the heap, to a Tuple<Int32, String> is a struct.
        void retarget(expression_t * exp, type_t * type)
        {
            if (exp != nullptr && exp->this_family() == expression_family_t::tuple
                    && exp->walked && type != nullptr)
                __lower_tuple((tuple_expression_t *)exp, type);
        }

    private:
        ast_context_t       & __cctx;
        ast_walk_context_t  & __wctx;
        variable_region_t   * __region;
        type_t              * __current_type = nullptr;

        // Checks walk expression
        bool __check_walk(expression_t * exp)
        {
            if (exp->walked)
                return false;

            exp->walked = true;
            return true;
        }

        #define __CheckWalk(exp)                                                        \
            do                                                                          \
            {                                                                           \
                if (!__check_walk(exp))  return;                                        \
            } while (false)

        // Walks expression.
        void __walk(expression_t * exp)
        {
            __CheckWalk(exp);

            switch (exp->this_family())
            {
                case expression_family_t::name:
                    __walk_name((name_expression_t *)exp);
                    return;

                case expression_family_t::name_unit:
                    __walk_name_unit((name_unit_expression_t *)exp);
                    return;

                case expression_family_t::function:
                    __walk_function((function_expression_t *)exp);
                    return;

                case expression_family_t::binary:
                    switch (((binary_expression_t *)exp)->op())
                    {
                        case operator_t::member_point:
                            __walk_member((binary_expression_t *)exp);
                            return;

                        default:
                            __each(exp);
                            __walk_op_expression((binary_expression_t *)exp);
                            break;
                    }   break;

                case expression_family_t::unitary:
                    __each(exp);
                    __walk_op_expression((unitary_expression_t *)exp);
                    break;

                case expression_family_t::new_:
                    __walk_new((new_expression_t *)exp);
                    __each(exp);
                    break;

                case expression_family_t::this_:
                    // The target of an object initializer has the type of the new expression.
                    if (!((this_expression_t *)exp)->is_new_target())
                        ((this_expression_t *)exp)->set_type(__wctx.current_type());

                    __each(exp);
                    break;

                case expression_family_t::base: {
                    type_t * base_type = __wctx.current_type()->get_base_type();
                    if (base_type == nullptr)
                        ast_log(__cctx, exp, __c_t::no_base_type, __wctx.current_type());
                    else
                        ((base_expression_t *)exp)->set_type(base_type);

                    __each(exp);
                }   break;

                case expression_family_t::index: {
                    __walk_index((index_expression_t *)exp);
                    __each(exp);
                }   break;

                case expression_family_t::tuple:
                    __walk_tuple((tuple_expression_t *)exp);
                    break;

                default:
                    __each(exp);
                    break;
            }
        }

        // Walks a tuple literal: its items first, then the expression that makes it, for the type
        // that the context wants (the declared type of the variable that it is assigned to, the type of
        // a cast). A call, a return and the others tell it later (retarget), the tuple is
        // made of the types of its items until then.
        void __walk_tuple(tuple_expression_t * exp)
        {
            for (expression_t * item : *exp)
                __walk(item);

            type_t * target = pick_type_from_current_context(exp);
            if (target != nullptr && target->this_gtype() == gtype_t::uncertain)
                target = nullptr;

            __lower_tuple(exp, target);
        }

        // Returns the type System.ITuple, that has the method Create.
        type_t * __ituple_class()
        {
            general_type_name_t type_name;
            name_unit_t system_unit(__to_name_of(_T("System")), nullptr);
            name_unit_t ituple_unit(__to_name_of(_T("ITuple")), nullptr);
            type_name.units.push_back(&system_unit);
            type_name.units.push_back(&ituple_unit);

            return ::april::modules::compile::__ascertain_type(__cctx, __wctx, &type_name);
        }

        // Makes the name of the string.
        name_t __to_name_of(const char_t * s)
        {
            return name_t(__SPool.to_sid(s));
        }

        // Makes the expression that makes the tuple: new Tuple<..>(items) for a struct,
        // ITuple.Create<..>(items) for a reference type.
        void __lower_tuple(tuple_expression_t * exp, type_t * target)
        {
            xpool_t & xpool = __XPool;
            size_t count = exp->expression_count();

            type_t * type = target;
            if (type != nullptr)
            {
                if (type_t * underlying = get_nullable_underlying(type); underlying != nullptr)
                    type = underlying;
            }

            // The target tells the kind of the tuple, and the types of the items.
            bool by_ref = false, has_item_types = false;
            type_collection_t types;

            if (type != nullptr && type->this_gtype() == gtype_t::generic)
            {
                generic_type_t * generic_type = (generic_type_t *)type;
                general_type_t * template_ = generic_type->template_;

                if (template_ == xpool.get_tuple_type() || template_ == xpool.get_ituple_type())
                {
                    by_ref = template_ == xpool.get_ituple_type();

                    if (generic_type->args.size() != count)
                    {
                        ast_log(__cctx, exp, __c_t::tuple_item_count_mismatch, count, target);
                        return;
                    }

                    for (type_t * arg : generic_type->args) types.push_back(arg);
                    has_item_types = true;
                }
            }

            // A reference that is not a tuple (Object, ITuple): the tuple is made in the heap.
            if (!has_item_types && type != nullptr && type->this_gtype() != gtype_t::uncertain
                    && can_be_null(type))
                by_ref = true;

            for (size_t index = 0; index < count; index++)
            {
                expression_t * item = exp->expression_at(index);

                if (has_item_types)
                {
                    // A tuple that is an item is made for the type of the item.
                    if (item->this_family() == expression_family_t::tuple)
                        __lower_tuple((tuple_expression_t *)item, types[index]);
                }
                else
                {
                    type_t * item_type = get_expression_type(__cctx, item);
                    if (item_type == nullptr || is_null(item_type))
                    {
                        ast_log(__cctx, item, __c_t::tuple_item_undetermined, index + 1);
                        return;
                    }

                    types.push_back(item_type);
                }
            }

            arguments_t * arguments = __new_obj<arguments_t>();
            for (expression_t * item : *exp)
            {
                argument_t * argument = __new_obj<argument_t>();
                argument->expression = item;
                arguments->push_back(argument);
            }

            exp->target = target;

            if (!by_ref)
            {
                type_t * tuple_type = xpool.new_generic_type(xpool.get_tuple_type(), types, nullptr);
                __wctx.commit_types();

                auto * new_exp = __new_obj<system_expression_t<new_expression_t>>();
                new_exp->type_name = __new_obj<type_name_t>(tuple_type);
                new_exp->set_arguments(arguments);
                new_exp->parent = exp;
                exp->lowered = new_exp;

                __walk(new_exp);

                new_exp->constructor = find_constructor(__cctx, tuple_type, arguments);
                if (new_exp->constructor == nullptr)
                {
                    ast_log(__cctx, exp, __c_t::type_constructor_not_match, tuple_type,
                                                        to_arg_types_str(__cctx, arguments));
                    return;
                }

                convert_arguments(__cctx, __wctx, arguments, new_exp->constructor);
                return;
            }

            type_t * ituple = __ituple_class();
            if (ituple == nullptr)
            {
                ast_log(__cctx, exp, __c_t::type_undefined, _T("System.ITuple"));
                return;
            }

            auto * type_exp = __new_obj<system_expression_t<name_expression_t>>(ituple);
            auto * name_exp = __new_obj<system_expression_t<name_expression_t>>(__to_name_of(_T("Create")));
            auto * member = __new_obj<system_expression_t<binary_expression_t>>(
                operator_t::member_point, type_exp, name_exp
            );

            auto * call = __new_obj<system_expression_t<function_expression_t>>();
            call->set_namex(member);
            call->set_arguments(arguments);
            call->generic_args = __new_obj<generic_args_t>();

            for (type_t * item_type : types)
            {
                generic_arg_t * generic_arg = __new_obj<generic_arg_t>();
                generic_arg->type_name = __new_obj<type_name_t>(item_type);
                call->generic_args->push_back(generic_arg);
            }

            call->parent = exp;
            exp->lowered = call;

            __walk(call);
        }

        // Avoid to walk expression.
        void __avoid_walk(expression_t * exp)
        {
            if (exp != nullptr)
                exp->walked = true;
        }

        // Enums expression.
        void __each(expression_t * exp)
        {
            each_expression(exp, [this](expression_t * exp0) { __walk(exp0); return true; });
        }

        // Walks name expression.
        void __walk_name(name_expression_t * name_exp)
        {
            if (name_exp->expression_type != name_expression_type_t::__default__)
                return;

            // A name that declares a variable: out int value, o is string s.
            if (name_exp->declare_type_name != nullptr)
            {
                __walk_declaration(name_exp);
                return;
            }

            name_t name = name_exp->name;

            variable_t * variable;
            if (__region != nullptr && (variable = __region->get(name)) != nullptr)
            {
                if (variable->this_type() == variable_type_t::method
                        && ((method_variable_t *)variable)->method->is_lambda)
                {
                    // The name of a lambda is that of its method, nothing else has it.
                    name_exp->set(variable);
                }
                else if (variable->this_type() == variable_type_t::method)
                {
                    type_t * context_type;

                    type_t * host_type = ((method_variable_t *)variable)->method->host_type;
                    method_t * method = __pick_method_from_current_context(
                        host_type, name_exp, &context_type
                    );

                    if (method != nullptr)
                    {
                        _A(method->variable != nullptr);
                        name_exp->set(method->variable);
                    }
                    else
                    {
                        if (context_type != nullptr)
                        {
                            ast_log(__cctx, name_exp, __c_t::cannot_find_method_with_prototype,
                                                    __to_method_prototype(context_type));
                        }
                        else
                        {
                            // If context_type is not nullptr, maybe it's defined by var.
                            // TODO: need check duplicated methods.
                            name_exp->set(variable);
                        }
                    }
                }
                else
                {
                    name_exp->set(variable);

                    // The names of the local variables of the method that does the work of an
                    // iterator are the fields of the state object.
                    method_t * current = __wctx.current_method();
                    if (variable->this_type() == variable_type_t::local
                            && current != nullptr && current->iterator_info != nullptr)
                    {
                        ((iterator_info_t *)current->iterator_info)->refs.push_back(name_exp);
                    }
                }
            }
            else
            {
                if (!__walk_inherited_field(name_exp) && !__walk_type(name_exp))
                    ast_log(__cctx, name_exp, __c_t::variable_undefined, name);
            }
        }

        // A name that is a field of a base class: the field of this.
        bool __walk_inherited_field(name_expression_t * name_exp)
        {
            type_t * current = __wctx.current_type();
            if (current == nullptr)
                return false;

            method_t * method = __wctx.current_method();

            for (type_t * type = current->get_base_type(); type != nullptr;
                                                          type = type->get_base_type())
            {
                // A base type that is generic (Base<T> of Derived<T>): its field has the types of
                // the arguments (the T of Derived<T>), the type finds it like this.Value does.
                if (type->this_gtype() == gtype_t::generic)
                {
                    member_t * member = nullptr;

                    try
                    {
                        analyze_member_args_t args(member_type_t::all, name_exp->name);
                        member = type->get_member(args, true);
                    }
                    catch (const logic_error_t<common_error_code_t> &)
                    {
                    }

                    if (member == nullptr)
                        continue;

                    if (member->this_type() == member_type_t::property)
                    {
                        property_t * property = (property_t *)member;
                        if (property->variable == nullptr
                                || to_access_value(property->decorate) == access_value_t::private_
                                || (method != nullptr && method->is_static()
                                                                && !property->is_static()))
                            continue;

                        name_exp->set(property->variable);
                        return true;
                    }

                    if (member->this_type() != member_type_t::field)
                        continue;

                    field_t * field = (field_t *)member;
                    if (field->variable == nullptr
                            || to_access_value(field->decorate) == access_value_t::private_
                            || (method != nullptr && method->is_static() && !field->is_static()))
                        continue;

                    name_exp->set(field->variable);
                    return true;
                }

                if (type->this_gtype() != gtype_t::general)
                    continue;

                for (field_t * field : ((general_type_t *)type)->fields)
                {
                    if (field->name != name_exp->name || field->variable == nullptr)
                        continue;

                    if (to_access_value(field->decorate) == access_value_t::private_)
                        continue;

                    if (method != nullptr && method->is_static() && !field->is_static())
                        continue;

                    name_exp->set(field->variable);
                    return true;
                }

                // A property of the base type (static or not).
                for (property_t * property : ((general_type_t *)type)->properties)
                {
                    if (property->name != name_exp->name || property->variable == nullptr)
                        continue;

                    if (to_access_value(property->decorate) == access_value_t::private_)
                        continue;

                    if (method != nullptr && method->is_static() && !property->is_static())
                        continue;

                    name_exp->set(property->variable);
                    return true;
                }
            }

            return false;
        }

        // Walks name_unit expression.
        void __walk_name_unit(name_unit_expression_t * name_unit_exp)
        {
            if (!__walk_type(name_unit_exp))
            {
                ast_log(__cctx, name_unit_exp, __c_t::type_undefined, name_unit_exp);
            }
        }

        // Walks a name that declares a variable: it is defined in the region of the expression,
        // the names that follow it are those of the variable.
        void __walk_declaration(name_expression_t * name_exp)
        {
            variable_defination_t vd(__cctx, __wctx, (object_t *)name_exp);
            vd.use_own_slot();

            local_variable_t * variable = vd.define_local(
                name_exp->declare_type_name, name_exp->name, false, nullptr, (object_t *)name_exp
            );

            if (variable == nullptr)
                return;

            name_exp->set(variable);

            // The local variables of the method that does the work of an iterator are the fields
            // of the state object.
            method_t * current = __wctx.current_method();
            if (current != nullptr && current->iterator_info != nullptr)
                ((iterator_info_t *)current->iterator_info)->refs.push_back(name_exp);
        }

        // Walks index expression.
        void __walk_index(index_expression_t * index_exp)
        {
            expression_t * body_exp = index_exp->namex();
            __walk(body_exp);

            type_t * body_type = body_exp->get_type();
            _A(body_type != nullptr);

            // The object of ?[ is a reference.
            if (index_exp->null_conditional && !can_be_null(body_type))
            {
                ast_log(__cctx, index_exp, __c_t::null_conditional_wrong_type,
                                                                    body_type->to_prototype());
                return;
            }

            arguments_t * arguments = index_exp->arguments();
            __walk_arguments(arguments);

            if (is_array(body_type)) // array index
            {
                // An array of any dimension is read as an array of one dimension (the items are
                // in the order of the rows, it is its base type Array<T>), or by one index for each
                // dimension. It is not read as an array of more dimensions than it has.
                array_type_t * array_type = (array_type_t *)body_type;
                size_t index_count = arguments == nullptr? 0 : arguments->size();

                if (index_count != 1 && index_count != (size_t)array_type->dimension)
                {
                    ast_log(__cctx, index_exp, __c_t::array_index_count_mismatch,
                        array_type->to_prototype(), index_count,
                        array_type->dimension == 1? string_t(_T("1"))
                            : _F(_T("1 or %1%"), (int)array_type->dimension));

                    // The expression has its type, the errors that follow are not told.
                }

                index_exp->variable = __new_obj<array_index_variable_t>(
                    body_exp, ((array_type_t *)body_type)->element_type, arguments
                );
            }
            else   // property index
            {
                atypes_t atypes;
                __fill_atypes(__cctx, atypes, arguments);

                analyze_member_args_t args(member_type_t::property, name_t::null, &atypes);

                member_t * member = body_type->get_member(args, true);
                if (member == nullptr)
                {
                    ast_log(__cctx, index_exp, __c_t::property_index_undetermind, index_exp);
                    return;
                }

                _A(member->this_type() == member_type_t::property);
                property_t * property = (property_t *)member;

                index_exp->variable = __new_obj<property_index_variable_t>(
                    property, arguments
                );
            }
        }

        // Walks type.
        bool __walk_type(name_expression_t * name_exp)
        {
            type_t * type = this->__ascertain_type(name_exp->name, nullptr);

            if (type != nullptr)
            {
                __current_type = type;

                name_exp->set(type);
                return true;
            }

            return false;
        }

        // Walks type.
        bool __walk_type(name_unit_expression_t * name_unit_exp)
        {
            type_t * type = __ascertain_type(name_unit_exp->name, name_unit_exp->generic_args);
            if (type != nullptr)
            {
                name_unit_exp->set(type);
                return true;
            }

            return false;
        }

        // Asscertain type with specified name and generic arguments.
        type_t * __ascertain_type(name_t name, generic_args_t * generic_args)
        {
            type_t * type = ::april::modules::compile::__ascertain_type(
                __cctx, __wctx, name, generic_args
            );

            __wctx.commit_types();

            return type;
        }

        enum class __exp2_walked_t { yes, no };

        // Collects the names of a path: A.B.C . Returns false if it has other expressions.
        bool __collect_path_names(expression_t * exp, std::vector<name_t> & names)
        {
            if (exp->this_family() == expression_family_t::name)
            {
                name_expression_t * name_exp = (name_expression_t *)exp;
                if (name_exp->expression_type != name_expression_type_t::__default__
                        || name_exp->declare_type_name != nullptr)
                    return false;

                names.push_back(name_exp->name);
                return true;
            }

            if (exp->this_family() == expression_family_t::binary)
            {
                binary_expression_t * binary = (binary_expression_t *)exp;
                if (binary->op() != operator_t::member_point || binary->null_conditional
                        || binary->exp2()->this_family() != expression_family_t::name)
                    return false;

                return __collect_path_names(binary->exp1(), names)
                    && __collect_path_names(binary->exp2(), names);
            }

            return false;
        }

        // The object of a member is a type that has a namespace: Shapes.Circle.Count, the
        // object is the type.
        void __walk_qualified_type(binary_expression_t * exp)
        {
            expression_t * object = exp->exp1();
            if (object->this_family() != expression_family_t::binary)
                return;

            std::vector<name_t> names;
            if (!__collect_path_names(object, names) || names.size() < 2)
                return;

            // The first name is a variable or a type: the path is not a namespace.
            if (__region != nullptr && __region->get(names[0]) != nullptr)
                return;

            if (__ascertain_type(names[0], nullptr) != nullptr)
                return;

            general_type_name_t type_name;
            std::vector<name_unit_t> units;
            units.reserve(names.size());

            for (name_t name : names)
            {
                units.push_back(name_unit_t(name, nullptr));
                type_name.units.push_back(&units.back());
            }

            type_t * type = ::april::modules::compile::__ascertain_type(__cctx, __wctx, &type_name);
            __wctx.commit_types();

            if (type == nullptr)
                return;

            name_expression_t * type_exp = __new_obj<system_expression_t<name_expression_t>>(type);
            type_exp->parent = exp;
            exp->exps[0] = type_exp;
        }

        // Walks Binary expression member.
        void __walk_member(binary_expression_t * exp)
        {
            __walk_qualified_type(exp);

            // A method has no members: the name of a method that is the name of a type too is the
            // type (Match.Empty, in a class that has the method Match).
            if (expression_t * object = exp->exp1(); object->this_family() == expression_family_t::name)
            {
                name_expression_t * object_name = (name_expression_t *)object;
                variable_t * variable;

                if (object_name->expression_type == name_expression_type_t::__default__
                        && object_name->declare_type_name == nullptr
                        && __region != nullptr
                        && (variable = __region->get(object_name->name)) != nullptr
                        && variable->this_type() == variable_type_t::method)
                {
                    __walk_type(object_name);
                }
            }

            expression_t * exp1 = exp->exp1(), * exp2 = exp->exp2();

            __walk(exp1);

            // The object of ?. is a reference.
            if (exp->null_conditional)
            {
                type_t * type1 = exp1->get_type();
                if (type1 != nullptr && !can_be_null(type1))
                {
                    ast_log(__cctx, exp, __c_t::null_conditional_wrong_type, type1->to_prototype());
                    return;
                }
            }

            if (__walk_member(exp1, exp2) != __exp2_walked_t::yes)
                __walk(exp2);
        }

        // Returns whether the type is string: its operators are methods of the type.
        static bool __is_string(type_t * type)
        {
            return type != nullptr && type->this_vtype() == vtype_t::string_;
        }

        // Walks binary expression.
        template<size_t _exp_count>
        void __walk_op_expression(op_expression_t<_exp_count> * exp)
        {
            // Check operator.
            const operator_property_t * op_property = exp->op_property;
            _A(op_property != nullptr);

            // The member of ?. is not assigned.
            if (_exp_count == 2 && op_property->is_assign)
            {
                expression_t * left = exp->expression_at(0);
                if (in_null_conditional_chain(left))
                {
                    ast_log(__cctx, exp, __c_t::null_conditional_assign);
                    return;
                }
            }

            // A lambda that is assigned to a delegate gets the return type of it.
            if (_exp_count == 2 && op_property->op == operator_t::assign)
            {
                bind_lambda_to_type(exp->expression_at(1 % _exp_count),
                                    exp->expression_at(0)->get_type());

                // An implicit operator of the types converts the value that is assigned.
                if (expression_t * converted = insert_user_conversion(exp->expression_at(1),
                                                                      exp->expression_at(0)->get_type());
                        converted != exp->expression_at(1))
                {
                    exp->exps[1] = converted;
                    converted->parent = exp;
                }
            }

            if (op_property->overload == operator_overload_model_t::no_check)
                return;

            operator_t op = op_property->op;

            // E += handler, E -= handler: the add method, or the remove method, of the event is
            // called with the handler: it is a delegate of the type of the event.
            if (_exp_count == 2 && (op == operator_t::add_assign || op == operator_t::sub_assign))
            {
                event_variable_t * event_var = __event_of(exp->expression_at(0));
                if (event_var != nullptr)
                {
                    event_t * event = event_var->event;
                    type_t * handler_type = to_type(event->type_name);

                    method_t * accessor = op == operator_t::add_assign? event->add_method
                                                                      : event->remove_method;
                    if (accessor == nullptr)
                    {
                        ast_log(__cctx, exp, __c_t::event_accessor_missing, event->name,
                                op == operator_t::add_assign? _T("add") : _T("remove"));
                        return;
                    }

                    bind_lambda_to_type(exp->expression_at(1), handler_type);
                    type_t * type2 = exp->expression_at(1)->get_type();

                    if (handler_type != nullptr && type2 != nullptr
                            && !is_type_compatible(type2, handler_type)
                            && !is_constant_compatible(exp->expression_at(1), handler_type))
                    {
                        ast_log(__cctx, exp, __c_t::invalid_type_cast, type2->to_prototype(),
                                                                       handler_type->to_prototype());
                    }

                    return;
                }
            }

            if (op == operator_t::assign)
            {
                _A(_exp_count == 2);

                type_t * type1 = exp->expression_at(0)->get_type();
                bind_lambda_to_type(exp->expression_at(1), type1);
                type_t * type2 = exp->expression_at(1)->get_type();

                // An operand of undefined type has been reported.
                if (type1 == nullptr || type2 == nullptr)
                    return;

                if (!is_type_compatible(type2, type1)
                        && !is_constant_compatible(exp->expression_at(1), type1))
                {
                    ast_log(__cctx, exp, __c_t::invalid_type_cast, type2->to_prototype(),
                                                                   type1->to_prototype());
                }

                return;
            }

            // Collect argument types.
            atypes_t atypes;
            bool exists_custom_type = false;
            bool exists_null = false;
            bool exists_string = false;

            for (expression_t * exp : exp->exps)
            {
                type_t * arg_type = exp->get_type();

                // An operand of undefined type has been reported: the operator is not analyzed.
                if (arg_type == nullptr)
                    return;

                if (is_null(arg_type))
                    exists_null = true;
                else if (__is_string(arg_type))
                    exists_custom_type = exists_string = true;
                else if (is_object(arg_type) || is_generic_param(arg_type) || is_ptr_type(arg_type))
                    exists_custom_type = true;       // A pointer (Ptr, Ptr<T>) has the operators of its type.

                atypes.push_back(arg_type);
            }

            if (!exists_custom_type)
                return;

            // null types only support equal or not_equal operators. (The operators of strings
            // take null, as a null string.)
            if (exists_null && !exists_string
                    && (op == operator_t::equal || op == operator_t::not_equal))
                return;

            if (op_property->overload == operator_overload_model_t::no_overloaded)
            {
                ast_log(__cctx, exp, __c_t::operator_cannot_be_overloaded, op_property->op);
                return;
            }

            // Finds method. x += y is x = x + y: the method is that of the operator +.
            const operator_property_t * name_property = exp->op_property;
            switch (op)
            {
                case operator_t::add_assign:
                    name_property = get_system_operator_property(operator_t::add);
                    break;

                case operator_t::sub_assign:
                    name_property = get_system_operator_property(operator_t::sub);
                    break;

                case operator_t::mul_assign:
                    name_property = get_system_operator_property(operator_t::mul);
                    break;

                case operator_t::div_assign:
                    name_property = get_system_operator_property(operator_t::div);
                    break;

                case operator_t::mod_assign:
                    name_property = get_system_operator_property(operator_t::mod);
                    break;


                default:
                    break;
            }

            const char_t * op_name = name_property->name;
            _A(op_name != nullptr);

            name_t name = __to_name(_F(_T("op_%1%"), op_name));

            // One operator method does x++ and ++x: it is declared as operator ++.
            name_t other_name = name;
            switch (op)
            {
                case operator_t::left_increment:
                    other_name = __to_name(_F(_T("op_%1%"),
                        get_system_operator_property(operator_t::right_increment)->name));
                    break;

                case operator_t::right_increment:
                    other_name = __to_name(_F(_T("op_%1%"),
                        get_system_operator_property(operator_t::left_increment)->name));
                    break;

                case operator_t::left_decrement:
                    other_name = __to_name(_F(_T("op_%1%"),
                        get_system_operator_property(operator_t::right_decrement)->name));
                    break;

                case operator_t::right_decrement:
                    other_name = __to_name(_F(_T("op_%1%"),
                        get_system_operator_property(operator_t::left_decrement)->name));
                    break;

                default:
                    break;
            }

            al::svector_t<method_t *> methods;
            for (atype_t atype : atypes)
            {
                if (!is_object(atype.type) && !__is_string(atype.type)
                        && !is_generic_param(atype.type) && !is_ptr_type(atype.type))
                    continue;

                method_t * method = nullptr;

                try
                {
                    method = find_method(
                        atype.type, method_trait_t::default_, name, nullptr, &atypes
                    );

                    if (method == nullptr && other_name != name)
                    {
                        method = find_method(
                            atype.type, method_trait_t::default_, other_name, nullptr, &atypes
                        );
                    }
                }
                catch (const method_compare_error_t & e)
                {
                    string_t s = al::join_str(atypes.begin(), atypes.end(), _T(", "));
                    ast_log(__cctx, exp, __c_t::operator_overload_ambiguous, op_property->op, s);

                    return;
                }

                if (method == nullptr)
                    continue;

                // The operator method of an operand type: static, takes all the operands.
                methods.push_back(method);
                if (method->is_static())
                    break;
            }

            if (methods.empty())
            {
                // The operands that an implicit operator converts to a primitive type are operands of the
                // operator that the language has for it: t + 1 of a Tagged that converts to an Int32.
                if (__convert_operands(exp))
                    return;

                string_t s = al::join_str(atypes.begin(), atypes.end(), _T(", "));
                ast_log(__cctx, exp, __c_t::operator_overload_not_defined, op_property->op, s);

                return;
            }

            if (methods.size() > 1)
            {
                // TODO:
            }

            exp->overload_method = methods[0];
        }

        // Converts the operands of custom types to primitive types, by their implicit operators. It is done only
        // if no operand is of a custom type after that (the operator is that of the language).
        template<size_t _exp_count>
        bool __convert_operands(op_expression_t<_exp_count> * exp)
        {
            expression_t * original[_exp_count];
            bool converted = false;

            for (size_t k = 0; k < _exp_count; k++)
            {
                original[k] = exp->exps[k];

                type_t * type = original[k]->get_type();
                if (type == nullptr || !is_object(type) || __is_string(type) || is_generic_param(type))
                    continue;

                // The type of the other operand: the conversion that is to it, when there is.
                type_t * other = nullptr;
                if (_exp_count == 2)
                {
                    other = exp->exps[1 - k]->get_type();
                    if (other != nullptr && is_object(other) && !__is_string(other))
                        other = nullptr;
                }

                type_t * target = find_user_conversion_target(type, other);
                if (target == nullptr)
                    continue;

                expression_t * cast = insert_user_conversion(original[k], target);
                if (cast != original[k])
                {
                    exp->exps[k] = cast;
                    cast->parent = exp;
                    converted = true;
                }
            }

            if (!converted)
                return false;

            // Nothing of a custom type must be left.
            bool left = false;
            for (size_t k = 0; k < _exp_count; k++)
            {
                type_t * type = exp->exps[k]->get_type();
                if (type != nullptr && is_object(type) && !__is_string(type))
                    left = true;
            }

            if (left)
            {
                for (size_t k = 0; k < _exp_count; k++)
                {
                    exp->exps[k] = original[k];
                    original[k]->parent = exp;
                }

                return false;
            }

            return true;
        }

        // Returns whether the type is a tuple.
        static bool __is_tuple_type(type_t * type)
        {
            if (type == nullptr || type->this_gtype() != gtype_t::generic)
                return false;

            general_type_t * template_ = ((generic_type_t *)type)->template_;
            return template_ == __XPool.get_tuple_type() || template_ == __XPool.get_ituple_type();
        }

        // The type name that the member of a generic type declares: the instance of a member has the types that
        // replace the params, the raw member has the params (T this[Int32 index]).
        static type_name_t * __raw_type_name(field_t * field)
        {
            impl_field_t * impl = dynamic_cast<impl_field_t *>(field);
            return impl != nullptr && impl->raw != nullptr? impl->raw->type_name : field->type_name;
        }

        static type_name_t * __raw_type_name(property_t * property)
        {
            impl_property_t * impl = dynamic_cast<impl_property_t *>(property);
            return impl != nullptr && impl->raw != nullptr? impl->raw->type_name : property->type_name;
        }

        static type_name_t * __raw_type_name(method_t * method)
        {
            impl_method_t * impl = dynamic_cast<impl_method_t *>(method);
            return impl != nullptr && impl->raw != nullptr? impl->raw->type_name : method->type_name;
        }

        // The declared type name of the expression, that has the names of the items of a tuple: of a
        // variable, a field, a property, a method that is called, a member of them, a cast.
        static type_name_t * __declared_type_name(expression_t * exp)
        {
            if (exp == nullptr)
                return nullptr;

            switch (exp->this_family())
            {
                case expression_family_t::name: {
                    variable_t * variable = ((name_expression_t *)exp)->variable;
                    if (variable == nullptr)
                        return nullptr;

                    switch (variable->this_type())
                    {
                        case variable_type_t::local:
                            return ((local_variable_t *)variable)->type_name;

                        case variable_type_t::param:
                            return ((param_variable_t *)variable)->param->type_name;

                        case variable_type_t::field:
                            return __raw_type_name(((field_variable_t *)variable)->field);

                        case variable_type_t::property:
                            return __raw_type_name(((property_variable_t *)variable)->property);

                        default:
                            return nullptr;
                    }
                }

                case expression_family_t::binary:
                    if (((binary_expression_t *)exp)->op() == operator_t::member_point)
                        return __declared_type_name(((binary_expression_t *)exp)->exp2());

                    return nullptr;

                case expression_family_t::function: {
                    function_expression_t * function_exp = (function_expression_t *)exp;
                    if (function_exp->get_ftype() != function_expression_type_t::method)
                        return nullptr;

                    method_base_t * method = function_exp->get_method();
                    if (method == nullptr || dynamic_cast<method_t *>(method) == nullptr)
                        return nullptr;

                    return __raw_type_name((method_t *)method);
                }

                case expression_family_t::type_cast:
                    return ((type_cast_expression_t *)exp)->type_name;

                default:
                    return nullptr;
            }
        }

        // The index of the param in the params of the generic type, -1 if it is not one of them.
        static int __param_index_of(type_t * owner_type, type_t * param)
        {
            if (owner_type == nullptr || param == nullptr || owner_type->this_gtype() != gtype_t::generic)
                return -1;

            general_type_t * template_ = ((generic_type_t *)owner_type)->template_;
            if (template_ == nullptr || template_->params == nullptr)
                return -1;

            for (size_t index = 0, count = template_->params->size(); index < count; index++)
            {
                if ((*template_->params)[index] == param)
                    return (int)index;
            }

            return -1;
        }

        // The names of a member of a generic type: the type of the member is declared with the params of the type
        // (T this[Int32 index], KeyValuePair<K, V> Current), the arguments of the object give the names that they have.
        type_name_t * __substitute_names(type_name_t * declared, type_name_t * owner, type_t * owner_type,
                                                                                        int depth = 0)
        {
            if (declared == nullptr || owner == nullptr || depth > 8)
                return declared;

            int index = __param_index_of(owner_type, declared->type);
            if (index >= 0)
                return owner->arg_name_at(index);

            size_t count = declared->arg_name_count();
            if (count == 0)
                return declared;

            std::vector<type_name_t *> args;
            bool changed = false;

            for (size_t k = 0; k < count; k++)
            {
                type_name_t * arg = declared->arg_name_at(k);
                type_name_t * substituted = __substitute_names(arg, owner, owner_type, depth + 1);

                changed = changed || substituted != arg;
                args.push_back(substituted);
            }

            if (!changed)
                return declared;

            type_name_t * result = __new_obj<type_name_t>(declared->type);
            result->item_names = declared->item_names;
            result->nullable = declared->nullable;
            result->arg_type_names = args;
            result->args_known = true;

            return result;
        }

    public:

        // The type name that has the names of the items of the tuples that the expression is, or has in its generic
        // arguments: of a variable, a field, a property, an indexer, a method that is called, a member of them, a cast.
        type_name_t * names_of(expression_t * exp)
        {
            if (exp == nullptr)
                return nullptr;

            switch (exp->this_family())
            {
                case expression_family_t::name:
                case expression_family_t::function:
                case expression_family_t::type_cast:
                    return __declared_type_name(exp);

                case expression_family_t::binary: {
                    binary_expression_t * binary = (binary_expression_t *)exp;
                    if (binary->op() != operator_t::member_point)
                        return nullptr;

                    expression_t * exp1 = binary->exp1(), * exp2 = binary->exp2();
                    type_name_t * owner = names_of(exp1);
                    type_t * owner_type = exp1->get_type();

                    // Item2 of a tuple is its second argument.
                    if (owner != nullptr && exp2->this_family() == expression_family_t::name && __is_tuple_type(owner_type))
                    {
                        string_t text = (string_t)(sid_t)((name_expression_t *)exp2)->name;
                        if (text.size() > 4 && text.compare(0, 4, _T("Item")) == 0
                                && text.find_first_not_of(_T("0123456789"), 4) == string_t::npos)
                            {
                            size_t number = 0;
                            for (size_t k = 4; k < text.size(); k++)
                                number = number * 10 + (size_t)(text[k] - _T('0'));

                            return number > 0? owner->arg_name_at(number - 1) : nullptr;
                        }
                    }

                    return __substitute_names(__declared_type_name(exp2), owner, owner_type);
                }

                case expression_family_t::index: {
                    index_expression_t * index_exp = (index_expression_t *)exp;
                    expression_t * namex = index_exp->namex();
                    if (namex == nullptr)
                        return nullptr;

                    type_name_t * owner = names_of(namex);
                    type_t * owner_type = namex->get_type();

                    variable_t * variable = index_exp->variable;
                    if (variable != nullptr && variable->this_type() == variable_type_t::property_index)
                    {
                        return __substitute_names(__raw_type_name(((property_index_variable_t *)variable)->property),
                                                                                            owner, owner_type);
                    }

                    // The element of an array.
                    if (owner != nullptr && owner_type != nullptr && owner_type->this_gtype() == gtype_t::array)
                        return owner->arg_name_at(0);

                    return nullptr;
                }

                default:
                    return nullptr;
            }
        }

        // The names of the element that a foreach gets from the iterator: the element is an argument of the type of the
        // iterator (List<T>), or has them (KeyValuePair<K, V> of Dictionary<K, V>).
        type_name_t * element_names(expression_t * iterator, type_t * element_type)
        {
            type_name_t * source = names_of(iterator);
            type_t * type = iterator != nullptr? iterator->get_type() : nullptr;

            if (source == nullptr || type == nullptr || element_type == nullptr)
                return nullptr;

            if (type->this_gtype() == gtype_t::array)
                return source->arg_name_at(0);

            if (type->this_gtype() != gtype_t::generic)
                return nullptr;

            generic_type_t * generic = (generic_type_t *)type;
            for (size_t k = 0; k < generic->argument_count(); k++)
            {
                if (generic->type_at(k) == element_type)
                    return source->arg_name_at(k);
            }

            if (element_type->this_gtype() != gtype_t::generic)
                return nullptr;

            generic_type_t * element = (generic_type_t *)element_type;
            type_name_t * result = __new_obj<type_name_t>(element_type);
            result->args_known = true;

            for (size_t j = 0; j < element->argument_count(); j++)
            {
                type_name_t * arg = nullptr;
                for (size_t k = 0; k < generic->argument_count() && arg == nullptr; k++)
                {
                    if (generic->type_at(k) == element->type_at(j))
                        arg = source->arg_name_at(k);
                }

                result->arg_type_names.push_back(arg);
            }

            return result->has_item_names()? result : nullptr;
        }

    private:

        // The name of an item of a tuple is that of its declaration: tuple.Number is tuple.Item1. When
        // the item has a name, Item1 is not its name. Returns whether the lookup goes on.
        bool __use_item_name(expression_t * object, name_expression_t * name_exp, type_t * type)
        {
            type_name_t * type_name = names_of(object);
            if (type_name == nullptr || type_name->item_names.empty())
                return true;

            const std::vector<name_t> & names = type_name->item_names;
            const string_t & text = (string_t)(sid_t)name_exp->name;

            for (size_t index = 0; index < names.size(); index++)
            {
                if (names[index] != name_t::null && names[index] == name_exp->name)
                {
                    name_exp->name = __to_name_of(_F(_T("Item%1%"), index + 1).c_str());
                    return true;
                }
            }

            // ItemN of an item that has a name.
            for (size_t index = 0; index < names.size(); index++)
            {
                if (names[index] != name_t::null && text == _F(_T("Item%1%"), index + 1))
                {
                    ast_log(__cctx, name_exp, __c_t::member_not_found, name_exp->name, type);
                    return false;
                }
            }

            return true;
        }

        // Walks member.
        __exp2_walked_t __walk_member(expression_t * exp1, expression_t * exp2)
        {
            type_t * type = get_expression_type(__cctx, exp1);

            if (type == nullptr)
            {
                // The type of the object is not known because of an error that is told: the
                // member of it cannot be found, and it is not an error of its own.
                if (__cctx.compile_context.error_count() == 0)
                    ast_log(__cctx, exp1, __c_t::type_undetermind, exp1);

                return __exp2_walked_t::yes;
            }

            // The items of a tuple that is written with names have them: tuple.Number.
            if (exp2->this_family() == expression_family_t::name && __is_tuple_type(type))
            {
                if (!__use_item_name(exp1, (name_expression_t *)exp2, type))
                    return __exp2_walked_t::yes;
            }

            // field & property
            switch (exp2->this_family())
            {
                case expression_family_t::name:
                    return __walk_fieldx(type, (name_expression_t *)exp2);

                case expression_family_t::name_unit:
                    return __walk_fieldx(type, (name_unit_expression_t *)exp2);

                case expression_family_t::function:
                    __walk_function((function_expression_t *)exp2, type);
                    return __exp2_walked_t::yes;

                default:
                    break;
            }

            ast_log(__cctx, exp2, __c_t::unexpected_symbol, exp2);
            return __exp2_walked_t::no;
        }

        // Try convert to a delegate type.
        generic_type_t * __try_fetch_delegate_type(type_t * type)
        {
            if (type == nullptr)
                return nullptr;

            // A nullable delegate (Delegate<..>? d = F;): the method or the lambda is converted to the delegate.
            if (type_t * underlying = get_nullable_underlying(type); underlying != nullptr)
                type = underlying;

            if (type->this_gtype() == gtype_t::generic)
            {
                xpool_t & xpool = __XPool;

                generic_type_t * generic_type = (generic_type_t *)type;
                general_type_t * general_type = generic_type->template_;

                if (general_type == xpool.get_delegate_type()
                    || general_type == xpool.get_multicast_delegate_type())
                {
                    return generic_type;
                }
            }

            return nullptr;
        }

        bool __pick_argument_types_from_context_type(type_t * context_type, atypes_t & atypes,
            type_t ** out_return_type)
        {
            generic_type_t * delegate_type = __try_fetch_delegate_type(context_type);
            if (delegate_type != nullptr)
            {
                auto & args = delegate_type->args;
                _A(args.size() > 0);

                al::assign(out_return_type, get_delegate_return_type(delegate_type));

                if (args.size() >= 2)   // The first is return type.
                    al::copy(args, std::back_inserter(atypes), 1);

                return true;
            }

            return false;
        }

        // Returns a method prototype from a generic type.
        string_t __to_method_prototype(type_t * type)
        {
            generic_type_t * delegate_type = __try_fetch_delegate_type(type);

            if (delegate_type != nullptr)
            {
                auto & args = delegate_type->args;
                _A(args.size() > 0);

                stringstream_t ss;
                ss << get_delegate_return_type(delegate_type) << _T(" (");

                for (size_t index = 1, count = args.size(); index < count; index++)
                {
                    if (index > 1)
                        ss << _T(", ");

                    ss << args[index];
                }

                ss << _T(")");

                return ss.str();
            }

            return _T("?");
        }

        // Pick a method from current context type.
        method_t * __pick_method_from_current_context(type_t * type, name_expression_t * name_exp,
            type_t ** out_context_type = nullptr)
        {
            _A(name_exp != nullptr);

            type_t * context_type = pick_type_from_current_context(name_exp);
            al::assign(out_context_type, context_type);

            if (context_type == nullptr)
                return nullptr;

            atypes_t atypes;
            type_t * return_type;

            __pick_argument_types_from_context_type(context_type, atypes, &return_type);

            analyze_member_args_t args1(
                member_type_t::method, name_exp->name, &atypes, nullptr, return_type
            );

            return (method_t *)type->get_member(args1, true);
        }

        bool __try_get_member(type_t * type, name_expression_t * name_exp, member_t ** out_member)
        {
            name_t name = name_exp->name;
            analyze_member_args_t args(member_type_t::all, name);

            name_unit_expression_t * name_unit_exp =
                                        dynamic_cast<name_unit_expression_t *>(name_exp);

            if (name_unit_exp != nullptr)
            {
                args.generic_args = name_unit_exp->generic_args;
                args.generic_args_count = name_unit_exp->generic_args_count();
            }

            try
            {
                *out_member = type->get_member(args, true);

                if (*out_member == nullptr)
                {
                    ast_log(__cctx, name_exp, __c_t::member_not_found, name, type);
                    return false;
                }

                member_t * & member = *out_member;
                if (member->this_type() == member_type_t::type && name_unit_exp != nullptr
                        && name_unit_exp->generic_args_count() > 0)
                {
                    general_type_t * template_ = _M(general_type_t *, member);
                    type_collection_t args;

                    al::transform(*name_unit_exp->generic_args, std::back_inserter(args),
                        [](generic_arg_t * arg) { return arg->get_type(); }
                    );

                    member = __XPool.new_generic_type(template_, args, type);
                    __wctx.commit_types();
                }

                return true;
            }
            catch (const logic_error_t<common_error_code_t> & e)
            {
                if (e.code == common_error_code_t::conflict)
                {
                    try
                    {
                        *out_member = __pick_method_from_current_context(type, name_exp);

                        if (*out_member == nullptr)
                        {
                            ast_log(__cctx, name_exp, __c_t::member_not_found, name, type);
                            return false;
                        }

                        return true;
                    }
                    catch (const logic_error_t<common_error_code_t> & e)
                    {
                        ast_log(__cctx, name_exp, __c_t::method_conflict, name, type, _T(""));
                        return false;
                    }
                }
                else
                {
                    ast_log(__cctx, name_exp, __c_t::member_not_found, name, type);
                    return false;
                }
            }
        }

        // Walks field, property, event, type, type_def, method.
        __exp2_walked_t __walk_fieldx(type_t * type, name_expression_t * name_exp)
        {
            member_t * member;
            if (!__try_get_member(type, name_exp, &member))
                return __exp2_walked_t::yes;    // The error is told, the name is not looked up again.

            // _PP(member->this_type());
            switch (member->this_type())
            {
                case member_type_t::property:
                    name_exp->set(((property_t *)member)->variable);
                    break;

                case member_type_t::field:
                    name_exp->set(((field_t *)member)->variable);
                    break;

                case member_type_t::event: {
                    event_t * event = (event_t *)member;
                    name_exp->set(event->variable);

                    // Outside of its type, an event is only added to and removed from.
                    if (!__is_event_accessor_target(name_exp)
                            && !__is_inside_event_type(event))
                    {
                        ast_log(__cctx, name_exp, __c_t::event_cannot_be_used, event->name);
                    }

                }   break;

                case member_type_t::method:
                    name_exp->set(((method_t *)member)->variable);
                    break;

                case member_type_t::type:
                    name_exp->set((type_t *)member);
                    break;

                case member_type_t::type_def:
                    name_exp->set((type_def_t *)member);
                    break;

                default:
                    X_UNEXPECTED_F("unexpect member type '%1%'", member->this_type());
            }

            if (!check_access(__wctx.current_type(), member))
                ast_log(__cctx, name_exp, __c_t::inaccessible_protection_level, name_exp);

            return __exp2_walked_t::yes;
        }

        // Walks method.
        __exp2_walked_t __walk_fieldx(type_t * type, name_unit_expression_t * name_unit_exp)
        {
            member_t * member;
            if (!__try_get_member(type, name_unit_exp, &member))
                return __exp2_walked_t::yes;    // The error is told, the name is not looked up again.

            switch (member->this_type())
            {
                case member_type_t::method: {
                    method_t * method = (method_t *)member;

                    impl_method_t * impl_method = __XPool.new_obj<impl_method_t>();
                    type_collection_t types;
                    fill_type_collection(types, name_unit_exp->generic_args);
                    impl_method->build(method, types);

                    name_unit_exp->set(impl_method->variable);

                }   break;

                case member_type_t::type:
                    name_unit_exp->set((type_t *)member);
                    break;

                default:
                    _PP(member->this_type());
                    throw _EC(unexpected);
            }

            return __exp2_walked_t::yes;
        }

        // Returns the name of the method that is called by E(args), when E is an event: its raise
        // method.
        name_t __event_raise_name(event_t * event)
        {
            return __to_name(_F(_T("raise_%1%"), event->name));
        }

        // Returns the event of a type that has the name, or null.
        event_t * __event_in(type_t * type, name_t name)
        {
            if (type == nullptr)
                type = __wctx.current_type();

            if (type == nullptr)
                return nullptr;

            analyze_member_args_t args(member_type_t::all, name);
            member_t * member = nullptr;

            try
            {
                member = type->get_member(args, true);
            }
            catch (const method_compare_error_t &)
            {
                return nullptr;
            }

            if (member == nullptr || member->this_type() != member_type_t::event)
                return nullptr;

            return (event_t *)member;
        }

        // Walks E(args): the raise method of the event is called.
        void __walk_raise(type_t * type, event_t * event, function_expression_t * function_exp)
        {
            if (event->raise_method == nullptr)
            {
                ast_log(__cctx, function_exp, __c_t::event_raise_not_valid, event->name,
                                                    _T("it has no raise accessor"));
                return;
            }

            __walk_method(type, __event_raise_name(event), function_exp);
        }

        // Walks obj.Name(args): Name is a method, or an event that is raised.
        void __walk_call(type_t * type, name_t name, function_expression_t * function_exp)
        {
            event_t * event = __event_in(type, name);

            if (event != nullptr)
                __walk_raise(type, event, function_exp);
            else
                __walk_method(type, name, function_exp);
        }

        // Returns whether the expression is the left operand of += or -= (E, or obj.E).
        static bool __is_event_accessor_target(expression_t * exp)
        {
            expression_t * e = exp;

            if (e->parent != nullptr && e->parent->this_family() == expression_family_t::binary
                    && ((binary_expression_t *)e->parent)->op() == operator_t::member_point
                    && ((binary_expression_t *)e->parent)->exp2() == e)
            {
                e = e->parent;
            }

            if (e->parent == nullptr || e->parent->this_family() != expression_family_t::binary)
                return false;

            binary_expression_t * parent = (binary_expression_t *)e->parent;
            operator_t op = parent->op();

            return (op == operator_t::add_assign || op == operator_t::sub_assign)
                && parent->exp1() == e;
        }

        // Returns whether the code that is walked is in the type that has the event.
        bool __is_inside_event_type(event_t * event)
        {
            type_t * current = __wctx.current_type();
            type_t * host = event->host_type;

            if (current == nullptr || host == nullptr)
                return false;

            return to_general(current) == to_general(host);
        }

        // Returns the event that an expression is (E, or obj.E), null if it is not an event.
        event_variable_t * __event_of(expression_t * exp)
        {
            if (exp == nullptr)
                return nullptr;

            if (exp->this_family() == expression_family_t::binary
                    && ((binary_expression_t *)exp)->op() == operator_t::member_point)
            {
                exp = ((binary_expression_t *)exp)->exp2();
            }

            if (exp->this_family() != expression_family_t::name)
                return nullptr;

            variable_t * variable = ((name_expression_t *)exp)->variable;
            if (variable == nullptr || variable->this_type() != variable_type_t::event)
                return nullptr;

            return (event_variable_t *)variable;
        }

        // Returns whether the type is a delegate (Delegate<..>, MulticastDelegate<..>).
        static bool __is_delegate_type(type_t * type)
        {
            if (type->this_gtype() != gtype_t::generic)
                return false;

            type_t * template_ = ((generic_type_t *)type)->template_;
            return template_ == __XPool.get_delegate_type()
                || template_ == __XPool.get_multicast_delegate_type();
        }

        // Walks function.
        void __walk_function(function_expression_t * function_exp, type_t * type = nullptr)
        {
            expression_t * namex_exp = function_exp->namex;
            if (namex_exp == nullptr)
                return;

            switch (namex_exp->this_family())
            {
                case expression_family_t::name: {
                    __avoid_walk(namex_exp);
                    name_expression_t * name_exp = (name_expression_t *)namex_exp;
                    variable_t * variable;

                    if (__region != nullptr && (variable = __region->get(name_exp->name)) != nullptr
                                && variable->this_type() == variable_type_t::event)
                    {
                        // E(args) calls the raise method of the event.
                        __walk_raise(type, ((event_variable_t *)variable)->event, function_exp);
                    }
                    else if (__region != nullptr && (variable = __region->get(name_exp->name)) != nullptr
                                && variable->this_type() != variable_type_t::method)
                    {
                        function_exp->set_variable(variable);
                        __walk_arguments(function_exp->arguments());

                        // Only a delegate is called: x(1) of a number or of a nullable delegate is an error.
                        if (type_t * variable_type = variable->get_type();
                                variable_type != nullptr && !__is_delegate_type(variable_type))
                        {
                            ast_log(__cctx, namex_exp, __c_t::variable_not_callable,
                                                                    name_exp->name, variable_type);
                        }

                        // The call of a delegate takes the items of a params generic parameter as
                        // arguments: they are written with ...
                        if (function_exp->arguments() != nullptr)
                        {
                            for (argument_t * argument : *function_exp->arguments())
                            {
                                if (argument->atype != param_type_t::extends
                                        && __is_items(argument->expression))
                                {
                                    ast_log(__cctx, argument->expression, __c_t::items_without_dots,
                                        ((param_variable_t *)((name_expression_t *)
                                            argument->expression)->variable)->param->name);
                                }
                            }
                        }

                        // The local variables of the method that does the work of an iterator
                        // are the fields of the state object, the delegates that are called too.
                        method_t * current = __wctx.current_method();
                        if (variable->this_type() == variable_type_t::local
                                && current != nullptr && current->iterator_info != nullptr)
                        {
                            ((iterator_info_t *)current->iterator_info)->calls.push_back(function_exp);
                        }
                    }
                    else if (type == nullptr && variable != nullptr
                                && ((method_variable_t *)variable)->method->is_lambda)
                    {
                        __walk_lambda_call(function_exp, ((method_variable_t *)variable)->method);
                    }
                    else
                    {
                        name_t member_name = ((name_expression_t *)namex_exp)->name;

                        // obj.A(args), A holds a delegate; else obj.E(args): the raise method, when the member is an event.
                        if (!__walk_delegate_member_call(type, member_name, function_exp))
                            __walk_call(type, member_name, function_exp);
                    }
                }   return;

                case expression_family_t::binary: {
                    __avoid_walk(namex_exp);
                    binary_expression_t * binary_exp = (binary_expression_t *)namex_exp;

                    if (binary_exp->op() == operator_t::member_point)
                    {
                        expression_t * exp1 = binary_exp->exp1();
                        expression_t * exp2 = binary_exp->exp2();

                        __walk(exp1);
                        __avoid_walk(exp2);

                        switch (exp2->this_family())
                        {
                            case expression_family_t::name: {
                                auto name_exp = (name_expression_t *)exp2;
                                type_t * type = get_expression_type(__cctx, exp1);

                                __walk_call(type, name_exp->name, function_exp);
                            }   break;

                            default:
                                break;
                        }
                    }
                    else
                    {
                        ast_log(__cctx, namex_exp, __c_t::unexpected_expression, namex_exp);
                        return;
                    }
                }   break;

                case expression_family_t::function:
                    // f(1)(2), Type.Method(1)(2): the method is a member of the type, if there is one,
                    // and the delegate that it returns is called.
                    __avoid_walk(namex_exp);
                    __walk_function((function_expression_t *)namex_exp, type);
                    __walk_arguments(function_exp->arguments());
                    break;

                default:
                    __walk(namex_exp);
                    __walk_arguments(function_exp->arguments());
                    break;
            }
        }

        // obj.Field(args): the field of the type holds a delegate, that is called. Returns whether the
        // name is such a member.
        bool __walk_delegate_member_call(type_t * type, name_t name, function_expression_t * function_exp)
        {
            if (type == nullptr)
                return false;

            member_t * member = nullptr;

            try
            {
                analyze_member_args_t args(member_type_t::all, name);
                member = type->get_member(args, true);
            }
            catch (const logic_error_t<common_error_code_t> &)
            {
                return false;
            }
            catch (const method_compare_error_t &)
            {
                return false;
            }

            variable_t * variable = nullptr;

            if (member != nullptr && member->this_type() == member_type_t::field
                    && __is_delegate_type(((field_t *)member)->get_type()))
                variable = ((field_t *)member)->variable;
            else if (member != nullptr && member->this_type() == member_type_t::property
                    && __is_delegate_type(((property_t *)member)->get_type()))
            {
                // The getter returns the delegate: a call needs it in a variable.
                ast_log(__cctx, function_exp, __c_t::property_not_callable, name);
                __walk_arguments(function_exp->arguments());
                return true;
            }

            if (variable == nullptr)
                return false;

            function_exp->set_variable(variable);
            __walk_arguments(function_exp->arguments());

            return true;
        }

        // Walks a lambda that is called at once: ((Int32 x) => x + 1)(2). It has no delegate that
        // gives its return type, it is the type of its body, if it is an expression. The types of
        // its params, if they have none, are those of the arguments.
        void __walk_lambda_call(function_expression_t * function_exp, method_t * method)
        {
            arguments_t * arguments = function_exp->arguments();
            __walk_arguments(arguments);

            size_t count = arguments == nullptr? 0 : arguments->size();
            if (count != method->param_count())
            {
                ast_log(__cctx, function_exp, __c_t::method_not_match, method->get_name(),
                                                                       method->host_type);
                return;
            }

            for (size_t index = 0; index < count; index++)
            {
                param_t * param = (*method->params)[index];
                type_t * arg_type = arguments->at(index)->expression->get_type();

                if (method->lambda_implicit && param->type_name != nullptr
                        && param->type_name->type == uncertain_type_t::instance()
                        && arg_type != nullptr && !is_null(arg_type))
                {
                    param->type_name->type = arg_type;
                }
            }

            __resolve_lambda_return_type(function_exp, method);

            function_exp->set_method(method);
        }

        // The return type of a lambda that no delegate gives is the type of its body: the body is
        // analyzed, with the types of the params of the lambda that are known.
        void __resolve_lambda_return_type(expression_t * exp, method_t * method)
        {
            // The body is analyzed now: its type is the return type.
            if (method->type_name != nullptr
                    && method->type_name->type == uncertain_type_t::instance())
            {
                __wctx.run_pending((ast_walk_context_t::pending_t *)method->lambda_pending);
            }

            // A block that returns only null: its type is object.
            if (method->type_name != nullptr && method->lambda_return_inferred
                    && is_null(method->type_name->type))
            {
                method->type_name->type = __XPool.get_object_type();
                if (method->variable != nullptr)
                    method->variable->reset_type();
            }

            // A block that returns no value.
            if (method->type_name != nullptr
                    && method->type_name->type == uncertain_type_t::instance()
                    && method->lambda_no_value)
            {
                method->type_name->type = __XPool.get_internal_type(vtype_t::void_);
                if (method->variable != nullptr)
                    method->variable->reset_type();
            }

            // A block that returns a value: nothing tells its type.
            if (method->type_name != nullptr
                    && method->type_name->type == uncertain_type_t::instance())
            {
                ast_log(__cctx, exp, __c_t::lambda_type_undetermined);
                method->type_name->type = __XPool.get_object_type();
            }
        }

        // Returns the index of the params generic parameter of the method that the type is, -1 if
        // it is not one.
        static size_t __pack_index(method_t * method, type_t * type)
        {
            if (!is_generic_param(type)
                    || ((generic_param_t *)type)->param_type != generic_param_type_t::params)
                return (size_t)-1;

            for (size_t k = 0, count = method->generic_param_count(); k < count; k++)
            {
                if ((*method->generic_params)[k]->name == ((generic_param_t *)type)->name)
                    return k;
            }

            return (size_t)-1;
        }

        // The type of a type that has generic params of the method, with the types that are
        // known: nullptr if one of them is not known.
        type_t * __substitute(method_t * method, const std::vector<type_t *> & types, type_t * type)
        {
            if (type == nullptr)
                return nullptr;

            if (is_generic_param(type))
            {
                generic_param_t * gp = (generic_param_t *)type;
                for (size_t k = 0, count = method->generic_param_count(); k < count; k++)
                {
                    if ((*method->generic_params)[k]->name == gp->name)
                        return k < types.size()? types[k] : nullptr;
                }

                return type;    // A param of the type of the method.
            }

            if (!has_generic_params(type))
                return type;

            generic_type_t * generic_type = (generic_type_t *)type;

            type_collection_t args;
            for (typex_t arg : generic_type->args)
            {
                // The types of a params generic parameter are all that are left.
                size_t pack_index = __pack_index(method, (type_t *)arg);
                if (pack_index != (size_t)-1)
                {
                    for (size_t k = pack_index; k < types.size(); k++)
                    {
                        if (types[k] == nullptr)
                            return nullptr;

                        args.push_back(types[k]);
                    }

                    continue;
                }

                type_t * arg_type = __substitute(method, types, (type_t *)arg);
                if (arg_type == nullptr)
                    return nullptr;

                args.push_back(arg_type);
            }

            return __XPool.new_generic_type(generic_type->template_, args, generic_type->host_type);
        }

        // Tells a generic param of the method from a type: Delegate<R, T> and the type of the body.
        void __unify(method_t * method, std::vector<type_t *> & types, type_t * param_type,
                                                                      type_t * type)
        {
            if (param_type == nullptr || type == nullptr)
                return;

            if (__pack_index(method, param_type) != (size_t)-1)
                return;

            if (is_generic_param(param_type))
            {
                generic_param_t * gp = (generic_param_t *)param_type;
                for (size_t k = 0, count = method->generic_param_count(); k < count; k++)
                {
                    if ((*method->generic_params)[k]->name == gp->name && k < types.size()
                            && types[k] == nullptr)
                    {
                        types[k] = type;
                    }
                }

                return;
            }

            if (!is_generic(param_type))
                return;

            // The type is the one of the param, or one of its super types: a List<Int32> is an
            // IEnumerable<Int32> that tells TResult of an IEnumerable<TResult>.
            generic_type_t * pt = (generic_type_t *)param_type;
            std::set<type_t *> visited;

            std::function<generic_type_t *(type_t *)> find = [&](type_t * t) -> generic_type_t * {
                if (t == nullptr || !visited.insert(t).second)
                    return nullptr;

                if (is_generic(t) && ((generic_type_t *)t)->template_ == pt->template_)
                    return (generic_type_t *)t;

                generic_type_t * found = nullptr;
                t->each_super_type([&](type_t * super_type) {
                    found = find(super_type);
                    return found == nullptr;
                });

                return found;
            };

            generic_type_t * at = find(type);
            if (at == nullptr)
                return;

            for (size_t k = 0, count = std::min(pt->args.size(), at->args.size()); k < count; k++)
                __unify(method, types, (type_t *)pt->args[k], (type_t *)at->args[k]);
        }

        // The generic arguments that the lambdas tell: the types of the params of a lambda are
        // those of the delegate (they are known now), the type of its body is the return type.
        // Returns false if a generic argument is not known.
        bool __infer_by_lambdas(method_t * method, function_expression_t * function_exp,
                                arguments_t * arguments, std::vector<type_t *> & types)
        {
            if (arguments == nullptr)
                return false;

            for (size_t index = 0, count = std::min((size_t)arguments->size(),
                                        (size_t)method->param_count()); index < count; index++)
            {
                method_t * lambda = get_lambda_method(arguments->at(index)->expression);
                type_t * param_type = method->param_at(index)->get_type();

                if (lambda == nullptr || param_type == nullptr || !has_generic_params(param_type)
                        || ((generic_type_t *)param_type)->template_ != __XPool.get_delegate_type())
                {
                    continue;
                }

                generic_type_t * delegate_type = (generic_type_t *)param_type;
                size_t lambda_count = lambda->params == nullptr? 0 : lambda->params->size();

                // The last argument of the delegate may be a params generic parameter: the types
                // of the params that are left.
                size_t arg_count = delegate_type->args.size();
                size_t pack_index = arg_count > 1?
                            __pack_index(method, (type_t *)delegate_type->args[arg_count - 1])
                          : (size_t)-1;
                bool has_pack = pack_index != (size_t)-1;
                size_t fixed_count = has_pack? arg_count - 2 : arg_count - 1;

                if (has_pack? lambda_count < fixed_count : lambda_count != fixed_count)
                    return false;

                // The types of the params.
                for (size_t k = 0; k < lambda_count; k++)
                {
                    param_t * lambda_param = (*lambda->params)[k];
                    if (lambda_param->type_name == nullptr
                            || lambda_param->type_name->type != uncertain_type_t::instance())
                    {
                        continue;
                    }

                    type_t * arg_type;
                    if (k < fixed_count)
                    {
                        arg_type = __substitute(method, types, (type_t *)delegate_type->args[k + 1]);
                    }
                    else
                    {
                        size_t pack_item = pack_index + (k - fixed_count);
                        arg_type = pack_item < types.size()? types[pack_item] : nullptr;
                    }

                    if (arg_type == nullptr)
                        return false;

                    lambda_param->type_name->type = arg_type;
                }

                // The return type of the lambda is the type of its body.
                if (lambda->type_name != nullptr
                        && lambda->type_name->type == uncertain_type_t::instance())
                {
                    __resolve_lambda_return_type(function_exp, lambda);
                }

                if (lambda->variable != nullptr)
                    lambda->variable->reset_type();

                __unify(method, types, (type_t *)delegate_type->args[0],
                        lambda->type_name != nullptr? lambda->type_name->type : nullptr);
            }

            if (std::find(types.begin(), types.end(), nullptr) != types.end())
                return false;

            // The return type of a lambda is the return type of its delegate, when the type of its
            // body is converted to it (a List<Int32> is an IEnumerable<Int32>).
            for (size_t index = 0, count = std::min((size_t)arguments->size(),
                                        (size_t)method->param_count()); index < count; index++)
            {
                method_t * lambda = get_lambda_method(arguments->at(index)->expression);
                type_t * param_type = method->param_at(index)->get_type();

                if (lambda == nullptr || param_type == nullptr || !has_generic_params(param_type)
                        || ((generic_type_t *)param_type)->template_ != __XPool.get_delegate_type()
                        || lambda->type_name == nullptr || lambda->type_name->type == nullptr)
                {
                    continue;
                }

                type_t * declared = __substitute(method, types,
                                        (type_t *)((generic_type_t *)param_type)->args[0]);

                if (declared != nullptr && declared != lambda->type_name->type
                        && is_type_compatible(lambda->type_name->type, declared))
                {
                    lambda->type_name->type = declared;
                    if (lambda->variable != nullptr)
                        lambda->variable->reset_type();
                }
            }

            return true;
        }

        // Tries to get method.
        // Walks arguments.
        void __walk_arguments(arguments_t * arguments)
        {
            if (arguments == nullptr)
                return;

            for (argument_t * argument : *arguments)
            {
                __walk(argument->expression);

                // Only an out argument declares a variable.
                if (argument->expression != nullptr
                        && argument->expression->this_family() == expression_family_t::name
                        && ((name_expression_t *)argument->expression)->declare_type_name != nullptr
                        && argument->atype != param_type_t::out)
                {
                    ast_log(__cctx, argument->expression, __c_t::unexpected_expression,
                                                                        argument->expression);
                }

                // The items of a params generic parameter are the argument that is written with ...,
                // or a tuple of them (a name, or a member: this.items ...): the type tells it later.
                if (argument->atype == param_type_t::extends && !__is_items(argument->expression)
                        && !__may_be_tuple(argument->expression))
                    ast_log(__cctx, argument->expression, __c_t::items_expected);
            }
        }

        // Returns whether the expression can be a tuple that is a value: a name or a member.
        static bool __may_be_tuple(expression_t * exp)
        {
            return exp != nullptr && (exp->this_family() == expression_family_t::name
                                   || exp->this_family() == expression_family_t::binary);
        }

        // Returns whether the expression is the items of a params generic parameter (TArgs ... args).
        static bool __is_items(expression_t * exp)
        {
            if (exp == nullptr || exp->this_family() != expression_family_t::name)
                return false;

            variable_t * variable = ((name_expression_t *)exp)->variable;
            return variable != nullptr && variable->this_type() == variable_type_t::param
                && ((param_variable_t *)variable)->param->ptype == param_type_t::extends;
        }

        // Returns the text of a name of a namespace or an assembly, empty for null.
        static string_t __text_of(const mname_t * name)
        {
            return name == nullptr? string_t() : (string_t)*name;
        }

        // Returns whether the method is an extension method: the methods of the assembly that is
        // compiled have the param (this Type name), the others have ExtensionAttribute.
        static bool __is_extension_method(method_t * method)
        {
            method_ast_node_t * node = dynamic_cast<method_ast_node_t *>(method);
            if (node != nullptr)
                return node->has_this_param();

            general_type_t * extension_type = __XPool.get_extension_attribute_type();
            return extension_type != nullptr && method->has_attribute(extension_type);
        }

        // Returns the object of a call, in obj.Method(args): the member expression that has it
        // is returned by out_member.
        expression_t * __receiver_of(function_expression_t * function_exp,
                                                    binary_expression_t ** out_member)
        {
            binary_expression_t * member = nullptr;

            expression_t * parent = function_exp->parent;
            expression_t * namex = function_exp->namex;

            if (parent != nullptr && parent->this_family() == expression_family_t::binary
                    && ((binary_expression_t *)parent)->op() == operator_t::member_point
                    && ((binary_expression_t *)parent)->exp2() == function_exp)
            {
                member = (binary_expression_t *)parent;
            }
            else if (namex != nullptr && namex->this_family() == expression_family_t::binary
                    && ((binary_expression_t *)namex)->op() == operator_t::member_point)
            {
                member = (binary_expression_t *)namex;
            }

            *out_member = member;
            return member == nullptr? nullptr : member->exp1();
        }

        // Collects the namespaces where the extension methods are searched: the current
        // namespace and its parents, and the namespaces of the usings.
        void __collect_extension_namespaces(std::vector<const mname_t *> & namespaces)
        {
            for (const mname_t * ns = ns_to_full_name(__wctx.current_namespace()); ;
                                                            ns = __wctx.parent_of(ns))
            {
                namespaces.push_back(ns);
                if (ns == nullptr)
                    break;
            }

            document_t * document = __wctx.current_document();
            if (document == nullptr)
                return;

            for (using_namespace_t * using_ns : document->all_using_namespaces())
            {
                if (using_ns->alias.empty())
                    namespaces.push_back(to_mname(using_ns->ns));
            }
        }

        // Finds the extension method that is called as a member of an object: obj.Name(args) is
        // Type.Name(obj, args), the Type is a static class of the namespaces that are used. The
        // call is changed to it, and walked again, when there is one.
        bool __walk_extension_method(name_t name, function_expression_t * function_exp)
        {
            binary_expression_t * member;
            expression_t * receiver = __receiver_of(function_exp, &member);

            if (receiver == nullptr)
                return false;

            // Type.Method(args) is not the call of an extension method.
            if (receiver->this_family() == expression_family_t::name
                    && ((name_expression_t *)receiver)->expression_type
                                                        != name_expression_type_t::variable
                    && ((name_expression_t *)receiver)->expression_type
                                                        != name_expression_type_t::__default__)
            {
                return false;
            }

            type_t * receiver_type = get_expression_type(__cctx, receiver);
            if (receiver_type == nullptr)
                return false;

            arguments_t * arguments = function_exp->arguments();

            atypes_t atypes;
            atypes.push_back(receiver_type);
            if (arguments != nullptr)
                __fill_atypes(__cctx, atypes, arguments);   // Appends them after the object.

            std::vector<const mname_t *> namespaces;
            __collect_extension_namespaces(namespaces);

            std::vector<assembly_t *> assemblies { &__wctx.assembly };
            if (document_t * document = __wctx.current_document())
            {
                for (import_t * import : document->all_imports())
                {
                    assembly_t * assembly = __wctx.load_assembly(
                        to_mname(import->package), to_mname(import->assembly)
                    );

                    if (assembly != nullptr && std::find(assemblies.begin(), assemblies.end(),
                                                                assembly) == assemblies.end())
                        assemblies.push_back(assembly);
                }
            }

            std::set<string_t> used_namespaces;
            for (const mname_t * ns : namespaces)
                used_namespaces.insert(__text_of(ns));

            general_type_t * found_type = nullptr;
            bool ambiguous = false;

            // The assemblies and the classes that have the extension methods are marked with
            // ExtensionAttribute, the others are not looked at. (The marks of the assembly that
            // is compiled are written when its methods are walked: all its classes are looked at.)
            general_type_t * extension_type = __XPool.get_extension_attribute_type();

            for (assembly_t * assembly : assemblies)
            {
                bool compiling = assembly == &__wctx.assembly;

                if (!compiling && (extension_type == nullptr
                                        || !assembly->has_attribute(extension_type)))
                    continue;

                assembly->types.each_general_types([&](general_type_t * t) {
                    if (!t->is_static() || t->host_type != nullptr || t->is_generic())
                        return;

                    if (!compiling && !t->has_attribute(extension_type))
                        return;

                    // The namespace of the type of another assembly begins with the name of the
                    // assembly: using System; is the namespace of the assembly.
                    string_t type_ns = t->namespace_ == nullptr? string_t()
                                                : __text_of(t->namespace_->full_name);
                    if (!compiling)
                    {
                        string_t assembly_name = __text_of(assembly->name);
                        type_ns = type_ns.empty()? assembly_name : assembly_name + _T(".") + type_ns;
                    }

                    if (used_namespaces.find(type_ns) == used_namespaces.end())
                        return;

                    analyze_member_args_t args(
                        member_type_t::method, name, &atypes, function_exp->generic_args
                    );
                    args.throw_on_not_found = true;

                    try
                    {
                        method_t * method = (method_t *)t->get_member(args, false);
                        if (method == nullptr || !__is_extension_method(method))
                            return;

                        if (found_type != nullptr && found_type != t)
                            ambiguous = true;

                        found_type = t;
                    }
                    catch (const method_compare_error_t &) { }
                    catch (const logic_error_t<common_error_code_t> &) { }
                });
            }

            if (found_type == nullptr || ambiguous)
                return false;

            // The object is the first argument, the member is a member of the static class.
            arguments_t * new_arguments = __new_obj<arguments_t>();

            argument_t * first = __new_obj<argument_t>();
            first->expression = receiver;
            new_arguments->push_back(first);

            if (arguments != nullptr)
            {
                for (argument_t * argument : *arguments)
                    new_arguments->push_back(argument);
            }

            function_exp->set_arguments(new_arguments);

            name_expression_t * type_exp =
                        __new_obj<system_expression_t<name_expression_t>>((type_t *)found_type);
            type_exp->parent = member;
            member->exps[0] = type_exp;

            __walk_method(found_type, name, function_exp);
            return true;
        }

        // Walks methods.
        void __walk_method(type_t * type, name_t name, function_expression_t * function_exp)
        {
            // _PF(_T("type: %1%, name: %2%, function_exp: %3%"), type, name, function_exp);

            if (type == nullptr)
            {
                type = __wctx.current_type();
                if (type == nullptr)
                {
                    ast_log(__cctx, function_exp, __c_t::type_undetermind, function_exp);
                    return;
                }
            }

            arguments_t * arguments = function_exp->arguments();
            __walk_arguments(arguments);

            atypes_t atypes;
            __fill_atypes(__cctx, atypes, arguments);

            method_t * method;
            analyze_member_args_t am_args(
                member_type_t::method, name, &atypes, function_exp->generic_args
            );
            am_args.throw_on_not_found = true;

            try
            {
                method = (method_t *)type->get_member(am_args, true);
            }
            catch (const method_compare_error_t & e)
            {
                auto & methods = e.related_methods;
                string_t s = al::join_str(methods.begin(), methods.end(), _T(", "));

                switch (e.error_code)
                {
                    case method_compare_error_code_t::conflict:
                        ast_log(__cctx, function_exp, __c_t::method_conflict, name, type, s);
                        break;

                    case method_compare_error_code_t::not_match:
                        // The method is an extension method, if the type has none.
                        if (__walk_extension_method(name, function_exp))
                            return;

                        // An argument has no type because of an error that is told: the call
                        // cannot match for that reason, and it is not an error of its own.
                        if (__cctx.compile_context.error_count() > 0
                            && al::any_of(atypes, [](const atype_t & a) { return a.type == nullptr; }))
                            return;

                        if (methods.empty())
                            ast_log(__cctx, function_exp, __c_t::method_not_found, name, type);
                        else
                            ast_log(__cctx, function_exp, __c_t::method_not_match, name, type);
                        break;

                    case method_compare_error_code_t::auto_determined_failed:
                        ast_log(__cctx, function_exp, __c_t::type_arguments_cannot_be_inferred,
                                name, type);
                        break;
                                        
                    default:
                        break;
                }

                return;
            }

            type_collection_t tc;

            // The generic arguments that are inferred: the types that the lambdas tell (the return
            // types of them) are not known yet, when the method is chosen.
            std::vector<type_t *> inferred(am_args.out_arg_types.begin(),
                                           am_args.out_arg_types.end());

            if (function_exp->generic_args == nullptr
                    && std::find(inferred.begin(), inferred.end(), nullptr) != inferred.end()
                    && !__infer_by_lambdas(method, function_exp, arguments, inferred))
            {
                ast_log(__cctx, function_exp, __c_t::type_arguments_cannot_be_inferred,
                        name, type);
                return;
            }

            if (function_exp->generic_args == nullptr)
                al::copy(inferred, std::back_inserter(tc));
            else
                fill_type_collection(tc, function_exp->generic_args);

            // The lambdas that are arguments are converted to the types of the params.
            if (arguments != nullptr && tc.empty() && method->generic_params == nullptr)
            {
                for (size_t index = 0, count = std::min((size_t)arguments->size(),
                                                (size_t)method->param_count()); index < count; index++)
                {
                    bind_lambda_to_type(arguments->at(index)->expression,
                                        method->param_at(index)->get_type());
                }
            }

            if (tc.empty())
            {
                function_exp->set_method(method);
            }
            else
            {
                generic_method_t * generic_method = __XPool.new_generic_method(method, tc, type);
                function_exp->set_method(generic_method);

                // The types of the params and the return type that are made from the arguments.
                generic_method->get_type();
                for (size_t k = 0, count = generic_method->param_count(); k < count; k++)
                    generic_method->param_type_at(k);

                __wctx.commit_types();

                // The lambdas are converted to the delegates that the generic arguments tell.
                if (arguments != nullptr)
                {
                    for (size_t index = 0, count = std::min((size_t)arguments->size(),
                                (size_t)generic_method->param_count()); index < count; index++)
                    {
                        bind_lambda_to_type(arguments->at(index)->expression,
                                            generic_method->param_type_at(index).type());
                    }
                }
            }

            // The items of a params generic parameter that are given to a parameter that takes
            // them: they are written with ...
            if (arguments != nullptr)
            {
                method_base_t * chosen = function_exp->get_method();
                for (size_t index = 0, count = std::min((size_t)arguments->size(),
                                (size_t)chosen->param_count()); index < count; index++)
                {
                    argument_t * argument = arguments->at(index);
                    type_t * param_type = chosen->param_type_at(index).type();

                    if (argument->atype != param_type_t::extends && __is_items(argument->expression)
                            && param_type != nullptr && is_generic_param(param_type)
                            && ((generic_param_t *)param_type)->param_type
                                                        == generic_param_type_t::params)
                    {
                        ast_log(__cctx, argument->expression, __c_t::items_without_dots,
                                    ((param_variable_t *)((name_expression_t *)
                                        argument->expression)->variable)->param->name);
                    }
                }
            }

            // The arguments that an implicit operator converts to the types of the params.
            convert_arguments(__cctx, __wctx, arguments, function_exp->get_method());

            // out var value: the type is that of the param.
            if (arguments != nullptr)
            {
                method_base_t * chosen = function_exp->get_method();
                for (size_t index = 0, count = std::min((size_t)arguments->size(),
                                (size_t)chosen->param_count()); index < count; index++)
                {
                    expression_t * arg_exp = arguments->at(index)->expression;
                    if (arg_exp == nullptr || arg_exp->this_family() != expression_family_t::name)
                        continue;

                    name_expression_t * declared = (name_expression_t *)arg_exp;
                    if (declared->declare_type_name == nullptr
                            || declared->declare_type_name->type == nullptr
                            || declared->declare_type_name->type->this_gtype() != gtype_t::uncertain)
                        continue;

                    type_t * param_type = chosen->param_type_at(index).type();
                    if (param_type != nullptr)
                    {
                        declared->declare_type_name->type = param_type;
                        declared->declare_type_name->nullable = true;
                    }
                }
            }

            if (!inferred.empty() && function_exp->generic_args == nullptr)
            {
                function_exp->generic_args = __new_obj<generic_args_t>();
                for (type_t * arg_type : inferred)
                {
                    generic_arg_t * generic_arg = __new_obj<generic_arg_t>();
                    generic_arg->type_name = __new_obj<type_name_t>(arg_type);

                    function_exp->generic_args->push_back(generic_arg);
                }
            }
        }

        // Walks method expression.
        void __walk_new(new_expression_t * exp)
        {
            type_name_t * type_name = exp->type_name;
            if (type_name == nullptr)
            {
                ast_log(__cctx, exp, __c_t::type_name_missing, _T("object creation"));
                return;
            }
        }

        // Creates a new object.
        template<typename t, typename ... args_t>
        t * __new_obj(args_t && ... args)
        {
            return __cctx.compile_context.new_obj<t>(std::forward<args_t>(args) ...);
        }

        #undef __CheckWalk
    };

    // Walks the items of a lambda that were kept apart.
    void analyze_lambda_now(ast_walk_context_t & context, expression_t * expression)
    {
        if (expression == nullptr || expression->this_family() != expression_family_t::name)
            return;

        name_expression_t * name_exp = (name_expression_t *)expression;
        if (name_exp->expression_type != name_expression_type_t::variable
                || name_exp->variable == nullptr
                || name_exp->variable->this_type() != variable_type_t::method)
        {
            return;
        }

        method_t * method = ((method_variable_t *)name_exp->variable)->method;
        if (method != nullptr && method->is_lambda && method->lambda_pending != nullptr)
            context.run_pending((ast_walk_context_t::pending_t *)method->lambda_pending);
    }

    // Walks expression.
    void walk_expression(ast_context_t & cctx, ast_walk_context_t & wctx,
                                                expression_t * expression)
    {
        // _P(_T("walk_expression: "), expression);

        if (expression != nullptr)
            __walk_expression_t(cctx, wctx).walk(expression);
    }

    ////////// ////////// ////////// ////////// //////////
    // find_method

    // The expression in a cast that calls the implicit operator of the types, if there is one.
    expression_t * insert_user_conversion(expression_t * expression, type_t * to)
    {
        if (expression == nullptr || to == nullptr)
            return expression;

        type_t * from = expression->get_type();
        if (from == nullptr || is_type_compatible(from, to) || is_constant_compatible(expression, to)
                || find_user_conversion(from, to, false, false) == nullptr)
        {
            return expression;
        }

        expression_t * parent = expression->parent;

        auto * cast = __XPool.new_obj<system_expression_t<type_cast_expression_t>>(
            __XPool.to_type_name(to), expression
        );

        cast->is_implicit = true;
        cast->parent = parent;
        return cast;
    }

    // Converts the arguments of a call to the types of the params; a tuple literal is made for the
    // type of the param.
    void convert_arguments(ast_context_t & cctx, ast_walk_context_t & wctx,
                                        arguments_t * arguments, method_base_t * method)
    {
        if (arguments == nullptr || method == nullptr)
            return;

        for (size_t index = 0, count = std::min((size_t)arguments->size(), (size_t)method->param_count());
                                                                                    index < count; index++)
        {
            argument_t * argument = arguments->at(index);
            if (argument->atype != param_type_t::__default__
                    || argument->expression->this_family() != expression_family_t::tuple)
                continue;

            type_t * param_type = method->param_type_at(index).type();
            if (param_type != nullptr && !is_generic_param(param_type) && !has_generic_params(param_type))
                retarget_tuple(cctx, wctx, argument->expression, param_type);
        }

        convert_arguments(arguments, method);
    }

    // The tuple literal is made for the type: an ITuple is made in the heap, a Tuple is a struct.
    void retarget_tuple(ast_context_t & cctx, ast_walk_context_t & wctx,
                                                    expression_t * expression, type_t * type)
    {
        __walk_expression_t(cctx, wctx).retarget(expression, type);
    }

    // The type name takes the names of the items of the tuples that the expression is, or has in its generic arguments.
    void take_item_names(ast_context_t & cctx, ast_walk_context_t & wctx, type_name_t * type_name,
                                                                        expression_t * expression)
    {
        if (type_name != nullptr)
            type_name->take_item_names(__walk_expression_t(cctx, wctx).names_of(expression));
    }

    // The type name takes the names of the items of the element that a foreach gets from the iterator.
    void take_element_item_names(ast_context_t & cctx, ast_walk_context_t & wctx, type_name_t * type_name,
                                                        expression_t * iterator, type_t * element_type)
    {
        if (type_name != nullptr)
            type_name->take_item_names(__walk_expression_t(cctx, wctx).element_names(iterator, element_type));
    }

    // Converts the arguments of a call to the types of the params.
    void convert_arguments(arguments_t * arguments, method_base_t * method)
    {
        if (arguments == nullptr || method == nullptr)
            return;

        for (size_t index = 0, count = std::min((size_t)arguments->size(), (size_t)method->param_count());
                                                                                    index < count; index++)
        {
            argument_t * argument = arguments->at(index);
            if (argument->atype != param_type_t::__default__)
                continue;

            type_t * param_type = method->param_type_at(index).type();
            if (param_type == nullptr || is_generic_param(param_type) || has_generic_params(param_type))
                continue;

            argument->expression = insert_user_conversion(argument->expression, param_type);
        }
    }

    // Finds method.
    method_t * find_method(type_t * type, method_trait_t trait, name_t name,
                        generic_args_t * generic_args, atypes_t * atypes)
    {
        _A(type != nullptr);

        analyze_member_args_t args(member_type_t::method, name, atypes, generic_args);
        args.method_trait = trait;

        return (method_t *)type->get_member(args, true);
    }

    // Finds method.
    method_t * find_method(ast_context_t & cctx, type_t * type, method_trait_t trait, name_t name,
                        generic_args_t * generic_args, arguments_t * arguments)
    {
        atypes_t atypes;
        __fill_atypes(cctx, atypes, arguments);

        return find_method(type, trait, name, generic_args, &atypes);
    }

    // Finds a constructor.
    method_t * find_constructor(type_t * type, atypes_t * atypes)
    {
        return find_method(type, method_trait_t::constructor, name_t::null,
                                                                nullptr, atypes);
    }

    // Finds a constructor.
    method_t * find_constructor(ast_context_t & cctx, type_t * type, arguments_t * arguments)
    {
        return find_method(cctx, type, method_trait_t::constructor, name_t::null,
                                                                nullptr, arguments);
    }

    // Finds default constructor: the one that is called without arguments. (Not the one that has no condition on the
    // arguments: a type that has two constructors would conflict.)
    method_t * find_default_constructor(type_t * type)
    {
        atypes_t atypes;

        try
        {
            return find_constructor(type, &atypes);
        }
        catch (const method_compare_error_t &)
        {
            // Many constructors that can be called without arguments (all their params have default values).
            return nullptr;
        }
    }

    // Returns whether the type has default constructor.
    bool has_default_constructor(type_t * type)
    {
        return find_default_constructor(type) != nullptr;
    }

    // Returns whether the type has public default constructor.
    bool has_public_default_constructor(type_t * type)
    {
        method_t * method = find_default_constructor(type);
        return method != nullptr && method->get_access_value() == access_value_t::public_;
    }

    // Finds static constructor.
    method_t * find_static_constructor(type_t * type)
    {
        return find_method(type, method_trait_t::static_constructor, name_t::null,
                                                    nullptr, (atypes_t *)nullptr);
    }

    // Finds desctructor.
    method_t * find_destructor(type_t * type, arguments_t * arguments)
    {
        return find_method(type, method_trait_t::destructor, name_t::null,
                                                    nullptr, (atypes_t *)nullptr);
    }

    // Converts to argument types string.
    string_t to_arg_types_str(ast_context_t & cctx, arguments_t * arguments)
    {
        if (arguments == nullptr || arguments->empty())
            return empty_str;

        atypes_t atypes;
        __fill_atypes(cctx, atypes, arguments);

        return al::join_str(atypes.begin(), atypes.end(), _T(", "), [](const atype_t & atype) {
            return _str(atype);
        });
    }

    ////////// ////////// ////////// ////////// //////////

    // Defines local variable.
    local_variable_t * variable_defination_t::define_local(type_name_t * type_name,
            name_t name, bool constant, expression_t * expression, __code_element_t element)
    {
        _A(type_name != nullptr);

        if (__region == nullptr)
        {
            __log(__c_t::unexpected_variable_defination, _str(name));
            return nullptr;
        }

        try
        {
            local_variable_t * variable = __region->define_local(type_name, name, constant,
                                                                      expression, __own_slot);

            // The local variables of the method that does the work of an iterator are fields.
            method_t * method = __wctx.current_method();
            if (method != nullptr && method->iterator_info != nullptr)
                ((iterator_info_t *)method->iterator_info)->locals.push_back(variable);

            return variable;
        }
        catch (const logic_error_t<ast_error_t> & e)
        {
            __deal_error(e, name, element);
            return nullptr;
        }
    }

    // Defines param variable.
    param_variable_t * variable_defination_t::define_param(param_t * param)
    {
        _A(param != nullptr);

        if (__region == nullptr)
        {
            __log(__c_t::unexpected_param_defination, _str(param));
            return nullptr;
        }

        try
        {
            return __region->define_param(param);
        }
        catch (const logic_error_t<ast_error_t> & e)
        {
            __deal_error(e, param->name, param);
            return nullptr;
        }
    }

    // Defines field variable.
    field_variable_t * variable_defination_t::define_field(field_t * field)
    {
        _A(field != nullptr);

        if (__region == nullptr)
        {
            __log(__c_t::unexpected_field_defination, _str(field));
            return nullptr;
        }

        try
        {
            return __region->define_field(field);
        }
        catch (const logic_error_t<ast_error_t> & e)
        {
            __deal_error(e, field->name, field);
            return nullptr;
        }
    }

    // Defines property variable.
    property_variable_t * variable_defination_t::define_property(property_t * property)
    {
        _A(property != nullptr);

        if (__region == nullptr)
        {
            __log(__c_t::unexpected_property_defination, _str(property));
            return nullptr;
        }

        try
        {
            return __region->define_property(property);
        }
        catch (const logic_error_t<ast_error_t> & e)
        {
            __deal_error(e, property->name, property);
            return nullptr;
        }
    }

    // Defines event variable.
    event_variable_t * variable_defination_t::define_event(event_t * event)
    {
        _A(event != nullptr);

        if (__region == nullptr)
            return nullptr;

        try
        {
            return __region->define_event(event);
        }
        catch (const logic_error_t<ast_error_t> & e)
        {
            __deal_error(e, event->name, event);
            return nullptr;
        }
    }

    // Defines method variable.
    method_variable_t * variable_defination_t::define_method(method_t * method)
    {
        _A(method != nullptr);

        if (__region == nullptr)
        {
            __log(__c_t::unexpected_method_defination, _str(method));
            return nullptr;
        }

        try
        {
            return __region->define_method(method);
        }
        catch (const logic_error_t<ast_error_t> & e)
        {
            __deal_error(e, method->get_name(), method);
            return nullptr;
        }
    }

    // Deals defination error.
    void variable_defination_t::__deal_error(const logic_error_t<ast_error_t> & e,
                                            name_t name, __code_element_t element)
    {
        switch (e.code)
        {
            case ast_error_t::variable_duplicated:
                __log(__c_t::variable_defination_duplicate, (string_t)name, element);
                break;

            default:
                X_UNEXPECTED();
        }
    }


    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::compile


