
#include <compile.h>

namespace X_ROOT_NS { namespace modules { namespace compile {

    typedef common_log_code_t __c_t;

    // Ascertains type.
    type_t * __ascertain_type(ast_context_t & cctx, ast_walk_context_t & wctx, name_t name);

    // Ascertains type.
    type_t * __ascertain_type(ast_context_t & cctx, ast_walk_context_t & wctx,
                                                general_type_name_t * type_name);

    ////////// ////////// ////////// ////////// //////////

    // Returns xpool of ast context.
    static xpool_t & __xpool(ast_context_t & cctx)
    {
        return cctx.compile_context.global_context.xpool;
    }

    // Converts multi-name to sid.
    sid_t mname_to_sid(const mname_t * mname)
    {
        if(mname == nullptr)
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
        return get_expression_type(__xpool(cctx), expression);
    }

    // Fills argument types.
    void __fill_atypes(ast_context_t & cctx, atypes_t & atypes, arguments_t * arguments)
    {
        if(arguments == nullptr)
            return;

        for(argument_t * argument : *arguments)
        {
            type_t * type = get_expression_type(cctx, argument->expression);
            atypes.push_back(atype_t(type, argument->atype));
        }
    }

    // Convets to mname_t *.
    static const mname_t * __to_mname(const emname_t * ns)
    {
        if(ns == nullptr)
            return nullptr;

        return ns->mname;
    }

    // Converts to mname_t *.
    static const mname_t * __to_mname(const using_namespace_t * ns)
    {
        if(ns == nullptr)
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

                if(__in_mname())
                    new ((void *)unit) type_name_unit_t(name_t((*__units.__mname)[index]));
                else
                    punit = *(__units.__begin + (index - __units.__mname_part_count()));
            }

            union
            {
                type_name_unit_t * punit;
                byte_t             unit[sizeof(type_name_unit_t)];
            };

            // Returns typename unit.
            type_name_unit_t * get()
            {
                if(__in_mname())
                    return (type_name_unit_t *)&unit;

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
            for(__itor_t it = this->begin(), it_end = this->end(); it != it_end; it++)
            {
                if(index++ > 0)
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
        for(auto it : units)
        {
            if(index++ > 0)
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
            if(type != nullptr)
                return type;

            return __ascertain_general_type();
        }

        // Ascertains param types.
        generic_param_t * __ascertain_param_type()
        {
            if(__type_name->units.size() != 1)
                return nullptr;

            type_name_unit_t * unit = __type_name->units[0];
            if(unit == nullptr || unit->is_generic())
                return nullptr;

            name_t name = unit->name;
            
            if(__current_method != nullptr)
            {
                generic_param_t * param = __current_method->find_param(name);
                if(param != nullptr)
                    return param;
            }

            if(__current_type == nullptr)
                return nullptr;

            for(general_type_t * type = __current_type; type != nullptr;
                                    type = as<general_type_t *>(type->host_type))
            {
                generic_param_t * param = type->find_param(name);
                if(param != nullptr)
                    return param;
            }

            return nullptr;
        }

        // Ascertains general type.
        type_t * __ascertain_general_type()
        {
            const mname_t * current_ns = ns_to_full_name(__wctx.current_namespace());
            mname_operate_context_t op_ctx = to_mname_operate_context(__wctx.xpool);

            if(__type_name->global_type == global_type_t::global)
                return __search_global_type(__type_name->begin(), __type_name->end()).type;

            if(__type_name->global_type == global_type_t::root)
            {
                __find_ret_t r = __find_type(
                    &__wctx.assembly, nullptr, __type_name->begin(), __type_name->end()
                );

                if(r.success)
                    return r.type;

                return __search_global_type(__type_name->begin(), __type_name->end()).type;
            }

            // nest type
            if(__current_type != nullptr)
            {
                __find_ret_t r = __find_nest_type(&__wctx.assembly, current_ns,
                    __current_type, __type_name->begin(), __type_name->end()
                );

                if(r.success)
                    return r.type;
            }

            // from current & parent namespace
            for(const mname_t * ns = current_ns; ; ns = __wctx.parent_of(ns))
            {
                __find_ret_t r = __find_type(&__wctx.assembly, ns, __type_name->begin(),
                                                                   __type_name->end());
                if(r.success)
                    return r.type;

                if(ns == nullptr)
                    break;
            }

            __find_ret_t r = __search_global_type(__type_name->begin(), __type_name->end());

            if(r.success)
                return r.type;

            // from using namespace
            document_t * document = __wctx.current_document();
            if(document != nullptr)
            {
                for(using_namespace_t * using_ns : document->all_using_namespaces())
                {
                    if(using_ns->alias.empty())
                    {
                        __multi_units_t units(__to_mname(using_ns), __type_name);
                        r = __search_global_type(units.begin(), units.end());
                    }
                    else
                    {
                        if(__type_name->units.size() >= 2
                                && *__type_name->units[0] == using_ns->alias)
                        {
                            __multi_units_t units(to_mname(using_ns->ns),
                                    __type_name->begin() + 1, __type_name->end()
                            );
                            r = __search_global_type(units.begin(), units.end());
                        }
                    }

                    if(r.success)
                        return r.type;
                }
            }

            return nullptr;
        }

        // Transforms generic argument.
        type_t * __transform_generic_arg(generic_arg_t * arg)
        {
            if(arg == nullptr || arg->type_name == nullptr)
                return nullptr;

            type_t * type = arg->type_name->type;
            if(type == nullptr)
                return nullptr;

            if(type->this_gtype() == gtype_t::type_def_param)
            {
                if(__get_current_type_def() != nullptr)
                    return type;
            }

            return type;
        }

        // Gets current typedef.
        type_def_t * __get_current_type_def()
        {
            if(__current_type_def == __EmptyTypeDef)
                __current_type_def = __wctx.current_type_def();

            return __current_type_def;
        }

        // Gets type by namespace and name.
        type_t * __get_type(assembly_t * assembly, sid_t ns, sid_t name,
                size_t generic_param_count, type_t * host_type, type_name_unit_t * unit = nullptr)
        {
            type_t * raw_host_type = host_type;
            if(host_type != nullptr && host_type->this_gtype() == gtype_t::generic)
            {
                raw_host_type = ((generic_type_t *)host_type)->template_;
                assembly = ((general_type_t *)raw_host_type)->assembly;
            }

            // from type_def params
            type_def_t * current_type_def;
            if(!generic_param_count && (current_type_def = __get_current_type_def()))
            {
                type_def_param_t * param = current_type_def->find_param((name_t)name);
                if(param != nullptr)
                    return param;
            }

            // from general_type
            type_t * type = assembly->types.get_general_type(
                ns, name, generic_param_count, raw_host_type
            );

            if(type != nullptr)
            {
                if(unit != nullptr && unit->generic_args_count() > 0)
                    type = __new_generic_type((general_type_t *)type, unit, host_type);
                return type;
            }

            // from type_def
            type_def_t * type_def = assembly->types.get_type_def(
                ns, name, generic_param_count, raw_host_type
            );

            if(type_def && type_def->type_name)
            {
                if(!type_def->type_name->type)
                    throw _E(ascertain_type_error_t::type_def_unsolved);

                type_t * type = type_def->type_name->type;
                return __assign_type_def_param(type, unit, host_type);
            }

            return nullptr;
        }

        // Finds nest type.
        template<typename itor_t>
        __find_ret_t __find_nest_type(assembly_t * assembly, const mname_t * ns,
                                                type_t * type, itor_t begin, itor_t end)
        {
            if(begin == end)
                return __find_ret_t { type, type != nullptr };

            while(type != nullptr)
            {
                type_t * nest_type = type;

                bool found = false;

                for(type_name_unit_t * unit : _range(begin, end))
                {
                    generic_type_t * generic_type = nullptr;
                    type_t * host_type = nest_type;

                    if(nest_type->this_gtype() == gtype_t::generic)
                    {
                        generic_type = (generic_type_t *)nest_type;
                        nest_type = generic_type->template_;
                    }

                    type_t * nest_type0 = __get_type(assembly, to_sid(ns),
                            (sid_t)unit->name, unit->generic_args_count(), host_type, unit);

                    if(nest_type0 != nullptr)
                    {
                        found = true;

                        if((generic_type != nullptr || unit->generic_args_count() > 0)
                            && nest_type0->host_type == nest_type
                            && nest_type0->this_gtype() == gtype_t::general)
                        {
                            general_type_t * template_ = (general_type_t *)nest_type0;
                            _A(template_->params_count() == unit->generic_args_count());

                            nest_type0 = __new_generic_type(template_, unit, host_type);
                        }

                        nest_type = nest_type0;
                    }
                    else if(found)
                    {
                        return __find_ret_t { nullptr, false };
                    }
                    else
                    {
                        break;
                    }
                }

                if(found)
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
            if(document == nullptr)
                return __find_ret_t { nullptr, false };

            for(import_t * import : document->all_imports())
            {
                int prefix_count = __import_starts_with(import, begin, end);
                if(prefix_count > 0)
                {
                    itor_t it = begin + prefix_count;
                    assembly_t * assembly = __wctx.load_assembly(
                        to_mname(import->package), to_mname(import->assembly)
                    );

                    if(assembly != nullptr)
                    {
                        __find_ret_t r = __search_type(assembly, nullptr, it, end);
                        if(r.success)
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
            if(begin == end)
                return 0;

            if(!import->alias.empty())
            {
                if(**begin == import->alias)
                    return 1;
            }
            else if(__starts_with(to_mname(import->assembly), begin, end))
            {
                return to_mname(import->assembly)->part_count();
            }

            return 0;
        }

        // Determines whether the assembly name starts with specified prefix.
        template<typename itor_t>
        bool __starts_with(const mname_t * assembly_name, itor_t begin, itor_t end)
        {
            if(assembly_name == nullptr)
                return false;

            itor_t it = begin;
            for(sid_t sid : *assembly_name)
            {
                if(it == end || **it++ != sid)
                    return false;
            }

            return it != end;
        }

        // Searches types from specified assembly.
        template<typename itor_t>
        __find_ret_t __search_type(assembly_t * assembly, const mname_t * ns,
                                                    itor_t begin, itor_t end)
        {
            if(begin == end)
                return __find_ret_t { nullptr, false };

            itor_t it = begin;

            do
            {
                __find_ret_t r = __find_type(assembly, ns, it, end);
                if(r.success)
                    return r;

            } while((it + 1) != end && !(*it)->is_generic()
                && (ns = __wctx.combine(ns, (*it++)->name), true));

            return __find_ret_t { nullptr, false };
        }

        // Finds type from specified assembly.
        template<typename itor_t>
        __find_ret_t __find_type(assembly_t * assembly, const mname_t * ns,
                                                    itor_t type_begin, itor_t type_end)
        {
            for(itor_t it = type_begin; it != type_end; it++)
            {
                type_name_unit_t * unit = *it;
                type_t * type = __get_type(assembly, mname_to_sid(ns),
                        (sid_t)unit->name, unit->generic_args_count(), nullptr, unit
                );

                __find_ret_t r = __find_nest_type(assembly, ns, type, it + 1, type_end);
                if(r.success)
                    return r;

                if(unit->generic_args_count() == 0)
                    ns = __wctx.combine(ns, unit->name);
                else
                    break;
            }

            return __find_ret_t { nullptr, false };
        }

        // Assigns type def param.
        type_t * __assign_type_def_param(type_t * type, type_name_unit_t * unit,
                                                    type_t * host_type = nullptr)
        {
            if(type == nullptr)
                return type;

            gtype_t gtype = type->this_gtype();
            if(gtype == gtype_t::type_def_param)
            {
                type_def_param_t * param = (type_def_param_t *)type;
                _A(unit != nullptr && param->index < unit->generic_args_count());
                return __transform_generic_arg((*unit)[param->index]);
            }
            else if(gtype == gtype_t::array)
            {
                array_type_t * array_type = (array_type_t *)type;
                type_t * element_type1 = __assign_type_def_param(array_type->element_type,
                                                                    unit, host_type);
                if(element_type1 != array_type->element_type)
                {
                    return __new_array_type(element_type1, array_type->dimension);
                }

                return type;
            }
            else if(gtype == gtype_t::generic_param)
            {
                generic_param_t * param = (generic_param_t *)type;
                for(type_t * t = host_type; t != nullptr; t = t->host_type)
                {
                    if(t->this_gtype() == gtype_t::generic)
                    {
                        generic_type_t * generic_type = (generic_type_t *)t;
                        generic_param_t * p = generic_type->template_->find_param(param->name);
                        if(p != nullptr)
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

            for(; current_type; current_type = current_type->host_type)
            {
                type_stack.push(current_type);
            }

            bool break_away = false;
            for(; !type_stack.empty(); type_stack.pop())
            {
                type_t * t = type_stack.top();
                gtype_t gtype = t->this_gtype();

                if(break_away || gtype == gtype_t::generic)
                {
                    generic_type_t * generic_type = (generic_type_t *)t;
                    type_collection_t types;

                    al::transform(generic_type->args, std::back_inserter(types),
                        [this, unit, &break_away, host_type](type_t * t0) {
                            type_t * t1 = __assign_type_def_param(t0, unit, host_type);
                            return t0 == t1? t0 : (break_away = true, t1);
                        }
                    );

                    if(break_away)
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
        generic_type_t * __new_generic_type(general_type_t * template_, type_name_unit_t * unit,
                                                                        type_t * host_type)
        {
            type_collection_t types;
            xtype_collection_t xtypes(types);

            if(unit->args != nullptr)
            {
                al::transform(*unit->args, std::back_inserter(types),
                    [this](generic_arg_t * arg) { return __transform_generic_arg(arg); }
                );
            }

            return __new_generic_type(template_, xtypes, host_type);
        }

        // Creates a new generic type.
        generic_type_t * __new_generic_type(general_type_t * template_, xtype_collection_t & xtypes,
                                                                        type_t * host_type)
        {
            return __wctx.assembly.types.new_generic_type(template_, xtypes, host_type);
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

        if(type == nullptr)
            throw _ED(ascertain_type_error_t::unknown_type, type_name);

        return type;
    }

    // Ascertains type.
    type_t * __ascertain_type(ast_context_t & cctx, ast_walk_context_t & wctx, name_t name)
    {
        general_type_name_t type_name;
        type_name_unit_t unit(name);
        type_name.units.push_back(&unit);

        return __ascertain_type(cctx, wctx, &type_name);
    }

    // Ascertains type.
    type_t * ascertain_type(ast_context_t & cctx, ast_walk_context_t & wctx,
                                                name_t name)
    {
        type_t * type = __ascertain_type(cctx, wctx, name);

        if(type == nullptr)
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

        if((general_type_name = as<general_type_name_t *>(element_type_name)) != nullptr)
            element_type = ascertain_type(cctx, wctx, general_type_name);
        else if((array_type_name = as<array_type_name_t *>(element_type_name)) != nullptr)
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
            __walk(exp);
        }

    private:
        ast_context_t       & __cctx;
        ast_walk_context_t  & __wctx;
        variable_region_t   * __region;

        // Checks walk expression
        bool __check_walk(expression_t * exp)
        {
            if(exp->walked)
                return false;

            exp->walked = true;
            return true;
        }

        #define __CheckWalk(exp)                    \
            do                                      \
            {                                       \
                if(!__check_walk(exp))  return;     \
            } while(false)

        // Walks expression.
        void __walk(expression_t * exp)
        {
            __CheckWalk(exp);

            switch(exp->this_family())
            {
                case expression_family_t::name:
                    __walk_name((name_expression_t *)exp);
                    return;

                case expression_family_t::function:
                    __walk_function((function_expression_t *)exp);
                    return;

                case expression_family_t::binary:
                    switch(((binary_expression_t *)exp)->op())
                    {
                        case operator_t::member_point:
                            __walk_member((binary_expression_t *)exp);
                            return;

                        default:
                            break;
                    }

                case expression_family_t::new_:
                    __walk_new((new_expression_t *)exp);
                    break;

                case expression_family_t::this_:
                    ((this_expression_t *)exp)->set_type(__wctx.current_type());
                    break;

                case expression_family_t::base: {
                    type_t * base_type = __wctx.current_type()->get_base_type();
                    if(base_type == nullptr)
                        ast_log(__cctx, exp, __c_t::no_base_type, __wctx.current_type());
                    else
                        ((base_expression_t *)exp)->set_type(base_type);

                }   break;

                case expression_family_t::index: {
                    __walk_index((index_expression_t *)exp);
                }   break;

                default:
                    break;
            }

            __each(exp);
        }

        // Avoid to walk expression.
        void __avoid_walk(expression_t * exp)
        {
            if(exp != nullptr)
                exp->walked = true;
        }

        // Enums expression.
        void __each(expression_t * exp)
        {
            each_expression(exp, [this](expression_t * exp0) { __walk(exp0); });
        }

        // Walks name expression.
        void __walk_name(name_expression_t * name_exp, type_t * parent_type = nullptr)
        {
            if(name_exp->is_variable_expression() && name_exp->variable != nullptr)
                return;

            name_t name = name_exp->name;

            variable_t * variable;
            if(__region == nullptr || (variable = __region->get(name)) == nullptr)
            {
                if(!__walk_type(name_exp))
                {
                    ast_log(__cctx, name_exp, __c_t::variable_undefined, name);
                }
            }
            else
            {
                name_exp->set(variable);
            }
        }

        // Walks index expression.
        void __walk_index(index_expression_t * index_exp)
        {
            expression_t * body_exp = index_exp->namex();
            __walk(body_exp);

            type_t * body_type = body_exp->get_type(__xpool(__cctx));
            _A(body_type != nullptr);

            arguments_t * arguments = index_exp->arguments();
            __walk_arguments(arguments);

            if(is_array(body_type)) // array index
            {
                index_exp->variable = __new_obj<array_index_variable_t>(
                    body_exp, ((array_type_t *)body_type)->element_type, arguments
                );
            }
            else   // property index
            {
                atypes_t atypes;
                __fill_atypes(__cctx, atypes, arguments);

                analyze_member_args_t args(member_type_t::property, name_t::null, &atypes);
                member_t * member = body_type->get_member(args);
                if(member == nullptr)
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
            type_t * type = __ascertain_type(__cctx, __wctx, name_exp->name);

            if(type != nullptr)
            {
                name_exp->set(type);
                return true;
            }

            return false;
        }

        enum class __exp2_walked_t { yes, no };

        // Walks Binary expression member.
        void __walk_member(binary_expression_t * exp)
        {
            expression_t * exp1 = exp->exp1(), * exp2 = exp->exp2();

            __walk(exp1);

            if(__walk_member(exp1, exp2) != __exp2_walked_t::yes)
                __walk(exp2);
        }

        // Walks member.
        __exp2_walked_t __walk_member(expression_t * exp1, expression_t * exp2)
        {
            type_t * type = get_expression_type(__cctx, exp1);

            if(type == nullptr)
            {
                ast_log(__cctx, exp1, __c_t::type_undetermind, exp1);
                return __exp2_walked_t::no;
            }

            // field & property
            switch(exp2->this_family())
            {
                case expression_family_t::name:
                    return __walk_fieldx(type, (name_expression_t *)exp2);

                case expression_family_t::function:
                    __walk_function((function_expression_t *)exp2, type);
                    return __exp2_walked_t::yes;

                default:
                    break;
            }

            ast_log(__cctx, exp2, __c_t::unexpected_symbol, exp2);
            return __exp2_walked_t::no;
        }

        // Walks field.
        __exp2_walked_t __walk_fieldx(type_t * type, name_expression_t * name_exp)
        {
            name_t name = name_exp->name;
            analyze_member_args_t args(member_type_t::all, name);
            member_t * member = type->get_member(args);

            if(member == nullptr)
            {
                ast_log(__cctx, name_exp, __c_t::member_not_found, name, type);
                return __exp2_walked_t::no;
            }

            //_PP(member->this_type());
            switch(member->this_type())
            {
                case member_type_t::property:
                    name_exp->set(((property_t *)member)->variable);
                    break;

                case member_type_t::field:
                    name_exp->set(((field_t *)member)->variable);
                    break;

                case member_type_t::event:
                    name_exp->set(((event_t *)member)->variable);
                    break;

                case member_type_t::type:
                    name_exp->set((type_t *)member);
                    break;

                case member_type_t::type_def:
                    name_exp->set((type_def_t *)member);
                    break;

                default:
                    throw _EC(unexpected);
            }

            return __exp2_walked_t::no;
        }

        // Walks function.
        void __walk_function(function_expression_t * function_exp, type_t * type = nullptr)
        {
            expression_t * namex_exp = function_exp->namex;
            if(namex_exp == nullptr)
                return;

            //_PP(function_exp);
            __avoid_walk(namex_exp);

            switch(namex_exp->this_family())
            {
                case expression_family_t::name:
                    __walk_method(type, ((name_expression_t *)namex_exp)->name, function_exp);
                    return;

                case expression_family_t::binary: {
                    binary_expression_t * binary_exp = (binary_expression_t *)namex_exp;

                    if(binary_exp->op() == operator_t::member_point)
                    {
                        expression_t * exp1 = binary_exp->exp1();
                        expression_t * exp2 = binary_exp->exp2();

                        __walk(exp1);
                        __avoid_walk(exp2);

                        switch(exp2->this_family())
                        {
                            case expression_family_t::name: {
                                auto name_exp = (name_expression_t *)exp2;
                                type_t * type = get_expression_type(__cctx, exp1);
                                __walk_method(type, name_exp->name, function_exp);
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

                default:
                    break;
            }
        }

        // Tries to get method.
        common_error_code_t __try_get_method(type_t * type, analyze_member_args_t & args,
                                                                method_t ** out_method)
        {
            try
            {
                *out_method = (method_t *)type->get_member(args);
            }
            catch(const logic_error_t<common_error_code_t> & e)
            {
                return e.code;
            }

            return common_error_code_t::__default__;
        }

        // Walks arguments.
        void __walk_arguments(arguments_t * arguments)
        {
            if(arguments == nullptr)
                return;

            for(argument_t * argument : *arguments)
                __walk(argument->expression);
        }

        // Walks methods.
        void __walk_method(type_t * type, name_t name, function_expression_t * function_exp)
        {
            //_PF(_T("type: %1%, name: %2%, function_exp: %3%"), type, name, function_exp);

            if(type == nullptr)
            {
                type = __wctx.current_type();
                if(type == nullptr)
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

            if(__try_get_method(type, am_args, &method) == common_error_code_t::conflict)
            {
                analyze_members_args_t ams_args(
                    member_type_t::method, name, &atypes, function_exp->generic_args
                );

                type->get_members(ams_args);

                members_t & members = ams_args.out_members;
                string_t s = al::join_str(members.begin(), members.end(), _T(", "));
                ast_log(__cctx, function_exp, __c_t::method_conflict, name, type, s);

                return;
            }

            if(method == nullptr)
            {
                analyze_members_args_t ams_args(
                    member_type_t::method, name, &atypes, function_exp->generic_args
                );

                type->get_members(ams_args);

                members_t & members = ams_args.out_members;
                if(members.empty())
                    ast_log(__cctx, function_exp, __c_t::method_not_found, name, type);
                else
                    ast_log(__cctx, function_exp, __c_t::method_not_match, name, type);

                return;
            }
            else
            {
                type_collection_t tc;

                if(function_exp->generic_args == nullptr)
                    al::copy(am_args.out_arg_types, std::back_inserter(tc));
                else
                    fill_type_collection(tc, function_exp->generic_args);

                if(tc.empty())
                    function_exp->method = method;
                else
                    function_exp->method = __xpool(__cctx).new_generic_method(method, tc);
            }

            if(!am_args.out_arg_types.empty() && function_exp->generic_args == nullptr)
            {
                function_exp->generic_args = __new_obj<generic_args_t>();
                for(type_t * arg_type : am_args.out_arg_types)
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
            if(type_name == nullptr)
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

    // Walks expression.
    void walk_expression(ast_context_t & cctx, ast_walk_context_t & wctx,
                                                expression_t * expression)
    {
        //_P(_T("walk_expression: "), expression);

        if(expression != nullptr)
            __walk_expression_t(cctx, wctx).walk(expression);
    }

    ////////// ////////// ////////// ////////// //////////
    // find_method

    // Finds method.
    method_t * find_method(ast_context_t & cctx, type_t * type, method_trait_t trait, name_t name,
                        generic_args_t * generic_args, atypes_t * atypes)
    {
        _A(type != nullptr);

        analyze_member_args_t args(member_type_t::method, name, atypes, generic_args);
        args.method_trait = trait;

        return (method_t *)type->get_member(args);
    }

    // Finds method.
    method_t * find_method(ast_context_t & cctx, type_t * type, method_trait_t trait, name_t name,
                        generic_args_t * generic_args, arguments_t * arguments)
    {
        atypes_t atypes;
        __fill_atypes(cctx, atypes, arguments);

        return find_method(cctx, type, trait, name, generic_args, &atypes);
    }

    // Finds a constructor.
    method_t * find_constructor(ast_context_t & cctx, type_t * type, atypes_t * atypes)
    {
        return find_method(cctx, type, method_trait_t::constructor, name_t::null,
                                                                nullptr, atypes);
    }

    // Finds a constructor.
    method_t * find_constructor(ast_context_t & cctx, type_t * type, arguments_t * arguments)
    {
        return find_method(cctx, type, method_trait_t::constructor, name_t::null,
                                                                nullptr, arguments);
    }

    // Finds static constructor.
    method_t * find_static_constructor(ast_context_t & cctx, type_t * type)
    {
        return find_method(cctx, type, method_trait_t::static_constructor, name_t::null,
                                                                nullptr, (atypes_t *)nullptr);
    }

    // Finds desctructor.
    method_t * find_destructor(ast_context_t & cctx, type_t * type, arguments_t * arguments)
    {
        return find_method(cctx, type, method_trait_t::destructor, name_t::null,
                                                                nullptr, (atypes_t *)nullptr);
    }

    // Finds methods.
    void find_methods(ast_context_t & cctx, type_t * type, method_trait_t trait, name_t name,
                        generic_args_t * generic_args, arguments_t * arguments)
    {

    }

    // Converts to argument types string.
    string_t to_arg_types_str(ast_context_t & cctx, arguments_t * arguments)
    {
        if(arguments == nullptr || arguments->empty())
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

        if(__region == nullptr)
        {
            __log(__c_t::unexpected_variable_defination, _str(name));
            return nullptr;
        }

        try
        {
            return __region->define_local(type_name, name, constant, expression);
        }
        catch(const logic_error_t<ast_error_t> & e)
        {
            __deal_error(e, name, element);
            return nullptr;
        }
    }

    // Defines param variable.
    param_variable_t * variable_defination_t::define_param(param_t * param)
    {
        _A(param != nullptr);

        if(__region == nullptr)
        {
            __log(__c_t::unexpected_param_defination, _str(param));
            return nullptr;
        }

        try
        {
            return __region->define_param(param);
        }
        catch(const logic_error_t<ast_error_t> & e)
        {
            __deal_error(e, param->name, param);
            return nullptr;
        }
    }

    // Defines field variable.
    field_variable_t * variable_defination_t::define_field(field_t * field)
    {
        _A(field != nullptr);

        if(__region == nullptr)
        {
            __log(__c_t::unexpected_field_defination, _str(field));
            return nullptr;
        }

        try
        {
            return __region->define_field(field);
        }
        catch(const logic_error_t<ast_error_t> & e)
        {
            __deal_error(e, field->name, field);
            return nullptr;
        }
    }

    // Defines property variable.
    property_variable_t * variable_defination_t::define_property(property_t * property)
    {
        _A(property != nullptr);

        if(__region == nullptr)
        {
            __log(__c_t::unexpected_property_defination, _str(property));
            return nullptr;
        }

        try
        {
            return __region->define_property(property);
        }
        catch(const logic_error_t<ast_error_t> & e)
        {
            __deal_error(e, property->name, property);
            return nullptr;
        }
    }

    // Deals defination error.
    void variable_defination_t::__deal_error(const logic_error_t<ast_error_t> & e,
                                            name_t name, __code_element_t element)
    {
        switch(e.code)
        {
            case ast_error_t::variable_duplicated:
                __log(__c_t::variable_defination_duplicate, (string_t)name, element);
                break;

            default:
                X_UNEXPECTED();
        }
    }


    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::compile

