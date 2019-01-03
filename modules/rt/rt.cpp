#include <rt.h>

namespace X_ROOT_NS { namespace modules { namespace rt {

    #define __EmptyVTbl ((rt_vtable_t *)0x01)

    typedef rt_error_code_t __e_t;

    ////////// ////////// ////////// ////////// //////////
    
    X_ENUM_INFO(rt_error_code_t)

        X_D(assembly_not_found,         _T("assembly '%1%' not found"))

        X_D(assembly_format_error,      _T("%1%"))

        X_D(method_not_found,           _T("method '%1%' not found"))

        X_D(field_not_found,            _T("field '%1%' not found"))

        X_D(local_index_out_of_range,   _T("local variable index out of range"))

        X_D(argument_index_out_of_range, _T("argument index out of range"))

        X_D(virtual_method_not_found,   _T("virtual method '%1%' not found"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(storage_type_t)

        X_C(ref,        _T("ref"))

        X_C(value,      _T("value"))

        X_C(mixture,    _T("mixture"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(rt_type_kind_t)

        X_C(general,    _T("general"))

        X_C(generic,    _T("generic"))

        X_C(array,      _T("array"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(search_method_options_t)

        X_C(default_,       _T("default"))

        X_C(only_virtual,   _T("only_virtual"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    static bool __is_virtual(decorate_t decorate)
    {
         return decorate.is_virtual || (!decorate.is_override && decorate.is_abstract);
    }

    static bool __is_override(decorate_t decorate)
    {
        return decorate.is_override;
    }

    ////////// ////////// ////////// ////////// //////////

    // rt_string_t

    int rt_string_t::compare (const rt_string_t & str) const
    {
        if(__reference_equals(str))
            return true;

        return __compare(str);
    }

    bool rt_string_t::equals(const rt_string_t & str) const
    {
        if(length != str.length)
            return false;

        if(__reference_equals(str))
            return true;

        return __compare(str) == 0;
    }

    bool rt_string_t::__reference_equals(const rt_string_t & str) const
    {
        return s == str.s;
    }

    int rt_string_t::__compare(const rt_string_t & str) const
    {
        rt_size_t min_length = al::min(length, str.length);
        for(const char_t * s0 = s, * s0_end = s0 + min_length, * s1 = str.s;
                                            s0 < s0_end; s0++, s1++)
        {
            if(*s0 != *s1)
                return *s0 - *s1;
        }

        return length - str.length;
    }

    //-------- ---------- ---------- ---------- ----------

    // rt_spool_t

    rt_sid_t rt_spool_t::to_sid(const rt_string_t & s)
    {
        auto it = __string_set.insert(s);
        return rt_sid_t(&(*it.first));
    }

    rt_sid_t rt_spool_t::to_sid(const string_t & s)
    {
        rt_string_t str(s.c_str(), s.length());
        auto it = __string_set.find(str);
        if(it == __string_set.end())
        {
            char_t * dst = __heap.acquire(s.length() + 1);
            al::strcpy(dst, s.c_str());
            it = __string_set.insert(rt_string_t(dst, s.length())).first;
        }

        return rt_sid_t(&(*it));
    }

    ////////// ////////// ////////// ////////// //////////
    // rt_type_t

    msize_t rt_type_t::get_size(assembly_analyzer_t & analyzer, storage_type_t * out_storage_type)
    {
        this->get_size(analyzer);
        al::assign_value(out_storage_type, __storage_type);

        return __size;
    }

    msize_t rt_type_t::get_size(assembly_analyzer_t & analyzer)
    {
        if(__size == unknown_msize)
            __size = this->on_caculate_size(analyzer, &__storage_type);

        return __size;
    }

    ////////// ////////// ////////// ////////// //////////
    // rt_general_type_t

    void rt_general_type_t::pre_new(assembly_analyzer_t & analyzer)
    {
        if(__size != unknown_msize)
            return;

        // size
        msize_t size = this->get_size(analyzer);

        // build_vtbl;
        this->__build_vtbl(analyzer);

        // execute static constructor
    }

    rt_sid_t rt_general_type_t::get_name(rt_assembly_t * assembly)
    {
        _A(assembly != nullptr);

        return assembly->to_sid(mt->name);
    }

    ttype_t rt_general_type_t::get_ttype(assembly_analyzer_t & analyzer)
    {
        return (ttype_t)mt->ttype;
    }

    void rt_general_type_t::each_field(assembly_analyzer_t & analyzer, each_field_t f)
    {
        for(ref_t field_ref : mt->fields)
        {
            if(!f(field_ref, analyzer.current->get_field(field_ref)))
                break;
        }
    }

    void rt_general_type_t::each_method(assembly_analyzer_t & analyzer, each_method_t f)
    {
        for(ref_t method_ref : mt->methods)
        {
            if(!f(method_ref, analyzer.current->get_method(method_ref)))
                break;
        }
    }

    rt_type_t * rt_general_type_t::get_base_type(assembly_analyzer_t & analyzer,
                                              rt_assembly_t ** out_assembly)
    {
        ref_t super_types = mt->super_types;
        if(super_types == ref_t::null)
            return nullptr;

        rt_super_type_t * super_type = analyzer.current->get_super_type(super_types[0]);
        _A(super_type != nullptr);

        rt_type_t * base_type = analyzer.get_type((*super_type)->type, out_assembly);
        _A(base_type != nullptr);

        if(base_type->get_ttype(analyzer) == ttype_t::class_)
            return base_type;

        return nullptr;
    }

    int rt_general_type_t::get_method_offset(assembly_analyzer_t & analyzer, ref_t method_ref)
    {
        return method_ref - mt->methods;
    }

    static bool __compare_method(assembly_analyzer_t & analyzer, method_prototype_t & prototype,
                                                                 rt_method_t * rt_method)
    {
        rt_assembly_t * rt_assembly = analyzer.current;
        mt_method_t & mt = *rt_method->mt;

        if(rt_assembly->get_name(rt_method) != prototype.name)
            return false;

        if(prototype.generic_param_count != mt.generic_params.count)
            return false;

        if(prototype.param_count() != mt.params.count)
            return false;

        if(prototype.return_type != analyzer.get_type(mt.type))
            return false;

        if(prototype.param_count() > 0)
        {
            auto p = prototype.arg_types.begin();
            typedef rt_mt_t<__tidx_t::param> mt_param_t;

            bool r = rt_assembly->each_params(mt.params, [&](int, mt_param_t & param) {
                if(param.param_type != p->param_type)
                    return false;

                if(analyzer.get_type(param.type) != p->type)
                    return false;

                p++;
                return true;
            });

            if(!r)
                return false;
        }

        return true;
    }

    ref_t rt_general_type_t::search_method(assembly_analyzer_t & analyzer,
                    method_prototype_t & prototype, search_method_options_t options)
    {
        bool only_virtual = enum_has_flag(options, search_method_options_t::only_virtual);
        ref_t ret = ref_t::null;

        typedef rt_mt_t<__tidx_t::method> mt_t;

        analyzer.current->each_methods(mt->methods, [&, this](int idx, mt_t & mt) {

            if(only_virtual && !__is_virtual((decorate_t)mt.decorate))
                return false;

            rt_general_type_t * rt_type;
            rt_assembly_t * rt_assembly;

            ref_t method_ref = ref_t(idx);
            rt_method_t * rt_method = analyzer.get_method(method_ref, &rt_type, &rt_assembly);
            _A(rt_method != nullptr);

            if(__compare_method(analyzer, prototype, rt_method))
            {
                ret = method_ref;
                return false;
            }

            return true;
        });

        return ret;
    }

    msize_t __size_of_struct(assembly_analyzer_t & analyzer, rt_general_type_t * type,
                                                    storage_type_t * out_storage_type)
    {
        vtype_t vtype = (vtype_t)(*type)->vtype;

        if(vtype != vtype_t::mobject)
        {
            al::assign(out_storage_type, storage_type_t::value);
            return get_vtype_size(vtype);
        }

        return type->get_size(analyzer, out_storage_type);
    }

    msize_t __size_of_enum(assembly_analyzer_t & analyzer, rt_general_type_t * type,
                                                    storage_type_t * out_storage_type)
    {
        al::assign(out_storage_type, storage_type_t::value);

        // TODO: size?
        return 0;
    }

    msize_t rt_general_type_t::get_variable_size(assembly_analyzer_t & analyzer,
                                                    storage_type_t * out_storage_type)
    {
        ttype_t ttype = (ttype_t)mt->ttype;

        switch(ttype)
        {
            case ttype_t::class_:
            case ttype_t::interface_:
                al::assign(out_storage_type, storage_type_t::ref);
                return rt_ref_size;

            case ttype_t::struct_:
                return __size_of_struct(analyzer, this, out_storage_type);

            case ttype_t::enum_:
                return __size_of_enum(analyzer, this, out_storage_type);

            default:
                X_UNEXPECTED();
        }
    }

    msize_t rt_general_type_t::on_caculate_size(assembly_analyzer_t & analyzer,
                                                        storage_type_t * out_storage_type)
    {
        switch((ttype_t)mt->ttype)
        {
            case ttype_t::interface_:
                return 0;

            case ttype_t::struct_:
                return __size_of_struct(analyzer, this, out_storage_type);

            case ttype_t::enum_:
                return __size_of_enum(analyzer, this, out_storage_type);

            default:
                break;
        }

        rt_assembly_t * rt_assembly;
        rt_type_t * base_type = get_base_type(analyzer, &rt_assembly);

        msize_t base_size;
        if(base_type == nullptr)
        {
            base_size = 0;
        }
        else
        {
            assembly_analyzer_t analyzer0(analyzer, rt_assembly);
            base_size = base_type->get_size(analyzer0);
        }

        type_layout_t layout(analyzer, base_size);
        this->each_field(analyzer, [&](ref_t, rt_field_t * field) {
            return layout.append(field), true;
        });

        layout.commit();

        al::assign(out_storage_type, layout.storage_type());
        return layout.type_size();
    }

    typedef al::small_vector_t<rt_vfunction_t, 32> rt_vfunctions_t;

    void __build_vtbl(assembly_analyzer_t & analyzer, rt_type_t * type, rt_vfunctions_t & vfuncs)
    {
        rt_assembly_t * rt_base_assembly;
        rt_type_t * base_type = type->get_base_type(analyzer, &rt_base_assembly);

        if(base_type != nullptr)
        {
            assembly_analyzer_t analyzer0(analyzer, rt_base_assembly);
            __build_vtbl(analyzer0, base_type, vfuncs);
        }

        ref_t ret = ref_t::null;

        typedef rt_mt_t<__tidx_t::method> mt_t;

        type->each_method(analyzer, [&](ref_t method_ref, rt_method_t * rt_method) {

            decorate_t decorate = (decorate_t)(*rt_method)->decorate;

            if(__is_virtual(decorate))
            {
                vfuncs.push_back(rt_vtable_function_t(analyzer.current->index, method_ref.index));
            }
            else if(__is_override(decorate))
            {
                method_prototype_t prototype;
                analyzer.get_prototype(method_ref, &prototype);

                bool found = false;
                for(rt_vtable_function_t & func : vfuncs)
                {
                    rt_assembly_t * rt_assembly0 = analyzer.assembly_provider->at(func.assembly_idx);
                    rt_method_t * rt_method0 = rt_assembly0->get_method(ref_t(func.method_idx));

                    assembly_analyzer_t analyzer0(analyzer, rt_assembly0);
                    if(__compare_method(analyzer0, prototype, rt_method0))
                    {
                        func = rt_vtable_function_t(analyzer.current->index, method_ref.index);

                        found = true;
                        break;
                    }
                }

                if(!found)
                {
                    auto method_name = analyzer.current->get_name(rt_method);
                    throw _ED(__e_t::virtual_method_not_found, method_name);
                }
            }
            else
            {
                return false;
            }

            return true;
        });

        if(vfuncs.size() == 0)
        {
            set_vtable(type, __EmptyVTbl);
        }
        else
        {
            rt_vtable_t * vtable = (rt_vtable_t *)memory_t::alloc(
                analyzer.memory, sizeof(rt_vtable_t) + vfuncs.size() * sizeof(rt_vfunction_t)
            );

            al::copy(vfuncs, vtable->functions);
            set_vtable(type, vtable);
        }
    }

    void rt_general_type_t::__build_vtbl(assembly_analyzer_t & analyzer)
    {
        al::small_vector_t<rt_vfunction_t, 32> vfuncs;
        rt::__build_vtbl(analyzer, this, vfuncs);
    }

    ////////// ////////// ////////// ////////// //////////
    // rt_generic_type_t

    ttype_t rt_generic_type_t::get_ttype(assembly_analyzer_t & analyzer)
    {
        return ttype_t::__unknown__;
    }

    rt_type_t * rt_generic_type_t::get_base_type(assembly_analyzer_t & analyzer,
                                                rt_assembly_t ** out_assembly)
    {
        return nullptr;
    }

    int rt_generic_type_t::get_method_offset(assembly_analyzer_t & analyzer, ref_t method_ref)
    {
        return 0;
    }

    ref_t rt_generic_type_t::search_method(assembly_analyzer_t & analyzer,
                    method_prototype_t & prototype, search_method_options_t options)
    {
        return ref_t::null;
    }

    msize_t rt_generic_type_t::get_variable_size(assembly_analyzer_t & analyzer,
                                                        storage_type_t * out_storage_type)
    {
        return 0;
    }

    msize_t rt_generic_type_t::on_caculate_size(assembly_analyzer_t & analyzer,
                                                        storage_type_t * out_storage_type)
    {
        return 0;
    }

    ////////// ////////// ////////// ////////// //////////
    // rt_array_type_t

    void rt_array_type_t::pre_new(assembly_analyzer_t & analyzer)
    {
        if(__element_type == nullptr)
        {
            rt_assembly_t * rt_assembly;
            __element_type = analyzer.get_type((*this)->element_type, &rt_assembly);

            assembly_analyzer_t analyzer0(analyzer, rt_assembly);
            __element_type->pre_new(analyzer0);
        }
    }

    ttype_t rt_array_type_t::get_ttype(assembly_analyzer_t & analyzer)
    {
        return ttype_t::class_;
    }

    rt_type_t * rt_array_type_t::get_base_type(assembly_analyzer_t & analyzer,
                                                rt_assembly_t ** out_assembly)
    {
        return nullptr;
    }

    int rt_array_type_t::get_method_offset(assembly_analyzer_t & analyzer, ref_t method_ref)
    {
        return 0;
    }

    ref_t rt_array_type_t::search_method(assembly_analyzer_t & analyzer,
                    method_prototype_t & prototype, search_method_options_t options)
    {
        return ref_t::null;
    }

    msize_t rt_array_type_t::get_variable_size(assembly_analyzer_t & analyzer,
                                                        storage_type_t * out_storage_type)
    {
        al::assign(out_storage_type, storage_type_t::ref);
        return rt_ref_size;
    }

    msize_t rt_array_type_t::on_caculate_size(assembly_analyzer_t & analyzer,
                                                        storage_type_t * out_storage_type)
    {
        al::assign(out_storage_type, storage_type_t::ref);
        return rt_ref_size;
    }

    ////////// ////////// ////////// ////////// //////////

    string_t join_assembly_name(const string_t & package, const string_t & name)
    {
        if(package.length() == 0)
            return name;

        return _F(_T("%s.%s"), package, name);
    }

    string_t __join_method_name(const string_t & assembly,
        const string_t & type_name, int type_generic_param_count,
        const string_t & method_name, int generic_param_count,
        const string_t & return_type, int param_count, const string_t * params)
    {
        stringstream_t ss;

        if(!return_type.empty())
            ss << return_type << _T(" ");

        if(!assembly.empty())
            ss << assembly;

        ss << _T(".") << type_name;
        if(type_generic_param_count > 0)
            ss << _T("<") << string_t(type_generic_param_count, _T(',')) << _T(">");

        ss << _T(".") << method_name;

        if(generic_param_count > 0)
            ss << _T("<") << string_t(generic_param_count, _T(',')) << _T(">");

        ss << _T("(");

        if(param_count > 0)
            ss << al::join_str(params, params + param_count, _T(", "));

        ss << _T(")");

        return ss.str();
    }

    rt_assembly_t * assembly_analyzer_t::get_assembly_by_method_ref(ref_t method_ref,
            rt_general_type_t ** out_type, rt_method_ref_t ** out_method_ref)
    {
        switch((mt_member_extra_t)method_ref.extra)
        {
            case mt_member_extra_t::import: {
                rt_method_ref_t * rt_method_ref = current->get_method_ref(method_ref);
                al::assign_value(out_method_ref, rt_method_ref);
                return get_assembly(rt_method_ref, out_type);
            }

            case mt_member_extra_t::internal:
                if(out_type != nullptr)
                {
                    *out_type = current->get_host_by_method_ref(method_ref);
                    _A(*out_type != nullptr);
                }

                return current;

            default:
                X_UNEXPECTED();
        }
    }

    rt_assembly_t * assembly_analyzer_t::get_assembly_by_field_ref(ref_t field_ref,
            rt_general_type_t ** out_type, rt_field_ref_t ** out_field_ref)
    {
        switch((mt_member_extra_t)field_ref.extra)
        {
            case mt_member_extra_t::import: {
                rt_field_ref_t * rt_field_ref = current->get_field_ref(field_ref);
                al::assign_value(out_field_ref, rt_field_ref);
                return get_assembly(rt_field_ref, out_type);
            }

            case mt_member_extra_t::internal:
                if(out_type != nullptr)
                    *out_type = current->get_host_by_field_ref(field_ref);
                return current;

            default:
                X_UNEXPECTED();
        }
    }


    rt_assembly_t * assembly_analyzer_t::get_assembly(rt_method_ref_t * rt,
            rt_general_type_t ** out_type)
    {
        _A(rt != nullptr);

        return get_assembly(rt->mt, out_type);
    }

    rt_assembly_t * assembly_analyzer_t::get_assembly(rt_field_ref_t * rt,
            rt_general_type_t ** out_type)
    {
        _A(rt != nullptr);

        return get_assembly(rt->mt, out_type);
    }

    rt_assembly_t * assembly_analyzer_t::get_assembly(mt_method_ref_t * mt,
            rt_general_type_t ** out_type)
    {
        _A(mt != nullptr);

        return get_assembly_by_type_ref(mt->host, out_type);
    }

    rt_assembly_t * assembly_analyzer_t::get_assembly(mt_field_ref_t * mt,
            rt_general_type_t ** out_type)
    {
        _A(mt != nullptr);

        return get_assembly_by_type_ref(mt->host, out_type);
    }

    rt_assembly_t * assembly_analyzer_t::get_assembly_by_type_ref(ref_t type_ref,
            rt_general_type_t ** out_type)
    {
        rt_type_t * rt_type;
        rt_assembly_t * rt_assembly;

        switch((mt_type_extra_t)type_ref.extra)
        {
            case mt_type_extra_t::general:
            case mt_type_extra_t::type_ref:
                rt_type = get_type(type_ref, &rt_assembly);
                al::assign_value(out_type, (rt_general_type_t *)rt_type);
                break;

            case mt_type_extra_t::array:
            case mt_type_extra_t::generic:
            case mt_type_extra_t::generic_param:
            default:
                X_UNEXPECTED();
        }

        return rt_assembly;
    }

    rt_assembly_t * assembly_analyzer_t::get_assembly(rt_type_ref_t * rt,
            rt_general_type_t ** out_type)
    {
        _A(rt != nullptr);

        rt_assembly_t * rt_assembly = get_assembly(rt->mt);

        if(out_type != nullptr)
        {
            *out_type = rt_assembly->get_type(
                __to_sid(rt->mt->namespace_), __to_sid(rt->mt->name),
                rt->mt->generic_param_count
            );
        }

        return rt_assembly;
    }

    rt_assembly_t * assembly_analyzer_t::get_assembly(mt_type_ref_t * mt)
    {
        _A(mt != nullptr);

        return get_assembly_by_assembly_ref(mt->assembly);
    }

    rt_assembly_t * assembly_analyzer_t::get_assembly_by_assembly_ref(ref_t assembly_ref)
    {
        rt_assembly_ref_t * rt = current->get_assembly_ref(assembly_ref);
        return get_assembly(rt);
    }

    rt_assembly_t * assembly_analyzer_t::get_assembly(rt_assembly_ref_t * rt)
    {
        _A(rt != nullptr);

        return get_assembly(rt->mt);
    }

    rt_assembly_t * assembly_analyzer_t::get_assembly(mt_assembly_ref_t * mt)
    {
        _A(mt != nullptr);

        string_t package = current->to_sid(mt->package);
        string_t name    = current->to_sid(mt->name);

        rt_assembly_t * ref_assembly = assembly_provider->load_assembly(
            package, name
        );

        if(ref_assembly == nullptr)
            throw _ED(rt_error_code_t::assembly_not_found, join_assembly_name(package, name));

        return ref_assembly;
    }

    rt_type_t * assembly_analyzer_t::get_type(ref_t type_ref, rt_assembly_t ** out_assembly)
    {
        if(type_ref == ref_t::null)
            return nullptr;

        switch((mt_type_extra_t)type_ref.extra)
        {
            case mt_type_extra_t::general: {
                al::assign_value(out_assembly, current);
                return current->get_type(type_ref);
            }

            case mt_type_extra_t::type_ref: {
                rt_general_type_t * type;
                rt_assembly_t * rt_assembly = get_assembly(
                    current->get_type_ref(type_ref), &type
                );
                al::assign_value(out_assembly, rt_assembly);
                return type;
            }

            case mt_type_extra_t::array: {
                al::assign_value(out_assembly, current);
                return current->get_array_type(type_ref);
            };

            case mt_type_extra_t::generic: {
                al::assign_value(out_assembly, current);
                return current->get_generic_type(type_ref);
            }

            default:
                X_UNEXPECTED();
        }
    }

    rt_method_t * assembly_analyzer_t::get_method(ref_t method_ref,
        rt_general_type_t ** out_type, rt_assembly_t ** out_assembly)
    {
        auto key = std::make_tuple((rt_method_t *)nullptr, current, method_ref);
        auto r = __pool.get(key, [this, method_ref]() {
            rt_general_type_t * type;
            rt_assembly_t * assembly;
            rt_method_t * method = __get_method(method_ref, &type, &assembly);
            return std::make_tuple(method, type, assembly);
        });

        al::assign_value(out_type, std::get<rt_general_type_t *>(r));
        al::assign_value(out_assembly, std::get<rt_assembly_t *>(r));

        return std::get<rt_method_t *>(r);
    }

    rt_method_t * assembly_analyzer_t::__get_method(ref_t method_ref,
            rt_general_type_t ** out_type, rt_assembly_t ** out_assembly)
    {
        rt_method_ref_t * rt_method_ref;
        rt_general_type_t * rt_general_type;

        rt_assembly_t * rt_assembly = get_assembly_by_method_ref(
            method_ref, &rt_general_type, &rt_method_ref
        );

        al::assign_value(out_type, rt_general_type);
        al::assign_value(out_assembly, rt_assembly);

        rt_method_t * rt_method;
        if(__is_current(rt_assembly))
        {
            rt_method = rt_assembly->get_method(method_ref);
        }
        else
        {
            mt_method_ref_t * mt = rt_method_ref->mt;
            ref_t params_ref = mt->params;
            int param_count = params_ref.count;
            rt_ptype_t params[param_count];

            ref_t param_ref = params_ref[0];
            for(rt_ptype_t * p = params, * p_end = p + param_count; p < p_end; p++)
            {
                rt_method_ref_param_t * param = current->get_method_ref_param(param_ref++);
                rt_type_t * type = get_type(param->mt->type);
                *p = rt_ptype_t(type, param->mt->ptype);
            }

            rt_sid_t method_name = __to_sid(rt_method_ref->mt->name);
            rt_type_t * return_type = get_type(mt->type);
            rt_method = nullptr;

            rt_assembly->each_methods(rt_general_type->mt->methods, [&, this](int index,
                                                    rt_mt_t<__tidx_t::method> & mt) {
                bool r = __compare_method(rt_assembly, mt, method_name,
                    mt.generic_params.count, return_type, mt.params.count, params
                );

                return r? rt_method = rt_assembly->get_method(ref_t(index)), false : true;
            });

            if(rt_method == nullptr)
            {
                throw _ED(rt_error_code_t::method_not_found,
                    __join_method_name(rt_assembly, rt_general_type, mt, params)
                );
            }
        }

        return rt_method;
    }

    X_ALWAYS_INLINE bool assembly_analyzer_t::__compare_method(
        rt_assembly_t * rt_assembly, rt_mt_t<__tidx_t::method> & mt, rt_sid_t name,
        int generic_param_count, rt_type_t * return_type, int param_count, rt_ptype_t * params)
    {
        if(rt_assembly->to_sid(mt.name) != name)
            return false;

        if(generic_param_count != mt.generic_params.count)
            return false;

        if(param_count != mt.params.count)
            return false;

        __self_t analyzer = __bind(rt_assembly);
        if(return_type != analyzer.get_type(mt.type))
            return false;

        if(param_count > 0)
        {
            rt_ptype_t * p = params;
            typedef rt_mt_t<__tidx_t::param> mt_param_t;

            bool r = rt_assembly->each_params(mt.params, [&p, &analyzer](int, mt_param_t & param) {
                if(param.param_type != p->param_type)
                    return false;

                if(analyzer.get_type(param.type) != p->type)
                    return false;

                p++;
                return true;
            });

            if(!r)
                return false;
        }

        return true;
    }

    rt_field_t * assembly_analyzer_t::get_field(ref_t field_ref,
        rt_general_type_t ** out_type, rt_assembly_t ** out_assembly)
    {
        auto key = std::make_tuple((rt_field_t *)nullptr, current, field_ref);
        auto r = __pool.get(key, [this, field_ref]() {
            rt_general_type_t * type;
            rt_assembly_t * assembly;
            rt_field_t * field = __get_field(field_ref, &type, &assembly);
            return std::make_tuple(field, type, assembly);
        });

        al::assign_value(out_type, std::get<rt_general_type_t *>(r));
        al::assign_value(out_assembly, std::get<rt_assembly_t *>(r));

        return std::get<rt_field_t *>(r);
    }

    rt_field_t * assembly_analyzer_t::__get_field(ref_t field_ref,
            rt_general_type_t ** out_type, rt_assembly_t ** out_assembly)
    {
        rt_field_ref_t * rt_field_ref;
        rt_general_type_t * rt_general_type;

        rt_assembly_t * rt_assembly = get_assembly_by_field_ref(
            field_ref, &rt_general_type, &rt_field_ref
        );

        al::assign_value(out_type, rt_general_type);
        al::assign_value(out_assembly, rt_assembly);

        rt_field_t * rt_field;
        if(__is_current(rt_assembly))
        {
            rt_field = rt_assembly->get_field(field_ref);
        }
        else
        {
            mt_field_ref_t * mt = rt_field_ref->mt;
            rt_sid_t field_name = __to_sid(mt->name);

            rt_field = nullptr;
            rt_assembly->each_fields(rt_general_type->mt->fields, [&, this](int index,
                                                    rt_mt_t<__tidx_t::field> & mt) {
                bool r = (field_name == rt_assembly->to_sid(mt.name));
                return r? rt_field = rt_assembly->get_field(ref_t(index)), false : true;
            });

            if(rt_field == nullptr)
            {
                throw _ED(rt_error_code_t::field_not_found,
                    __join_field_name(rt_assembly, rt_general_type, mt)
                );
            }
        }

        return rt_field;
    }

    string_t assembly_analyzer_t::__join_method_name(rt_assembly_t * assembly,
            rt_general_type_t * type, mt_method_ref_t * mt_method_ref, rt_ptype_t * ptypes)
    {
        return current->to_sid(mt_method_ref->name);
    }

    string_t assembly_analyzer_t::__join_field_name(rt_assembly_t * assembly,
            rt_general_type_t * type, mt_field_ref_t * mt_field_ref)
    {
        return current->to_sid(mt_field_ref->name);
    }

    msize_t assembly_analyzer_t::size_of(ref_t type_ref, storage_type_t * out_storage_type)
    {
        rt_type_t * type = this->get_type(type_ref);

        if(type == nullptr)
            return 0;

        return type->get_size(*this, out_storage_type);
    }

    msize_t assembly_analyzer_t::size_of(rt_type_t * type, storage_type_t * out_storage_type)
    {
        if(type == nullptr)
            return 0;

        return type->get_size(*this, out_storage_type);
    }

    msize_t assembly_analyzer_t::variable_size_of(ref_t type_ref,
                                                        storage_type_t * out_storage_type)
    {
        rt_type_t * type = this->get_type(type_ref);
        if(type == nullptr)
            return 0;

        return type->get_variable_size(*this, out_storage_type);
    }

    msize_t assembly_analyzer_t::variable_size_of(rt_type_t * type,
                                                        storage_type_t * out_storage_type)
    {
        if(type == nullptr)
            return 0;

        return type->get_variable_size(*this, out_storage_type);
    }

    rt_type_t * assembly_analyzer_t::get_base_type(rt_type_t * type,
                                                   rt_assembly_t ** out_assembly)
    {
        _A(type != nullptr);
        return type->get_base_type(*this, out_assembly);
    }

    msize_t assembly_analyzer_t::get_type_size(ref_t type_ref)
    {
        rt_assembly_t * rt_assembly;
        rt_type_t * rt_type = get_type(type_ref, &rt_assembly);

        _A(rt_assembly != nullptr);
        _A(rt_type != nullptr);

        return rt_type->get_size(*this);
    }

    msize_t assembly_analyzer_t::get_field_offset(ref_t field_ref)
    {
        rt_assembly_t * rt_assembly;
        rt_general_type_t * rt_type;

        rt_field_t * rt_field = this->get_field(field_ref, &rt_type, &rt_assembly);

        _A(rt_field != nullptr);
        _A(rt_type != nullptr);

        if(rt_field->offset == unknown_msize)
            rt_type->get_size(*this);

        return rt_field->offset;
    }

    int assembly_analyzer_t::get_virtual_method_offset(assembly_analyzer_t & analyzer,
                                                     rt_type_t * rt_type, ref_t method_ref)
    {
        _A(rt_type != nullptr);

        rt_method_t * method = analyzer.current->get_method(method_ref);
        _A(method != nullptr);

        decorate_t decorate = (decorate_t)(*method)->decorate;
        if(__is_virtual(decorate))
            return rt_type->get_method_offset(analyzer, method_ref);

        if(decorate.is_override)
        {
            rt_assembly_t * assembly;
            method_prototype_t prototype;
            analyzer.get_prototype(method_ref, &prototype);

            do
            {
                rt_type = rt_type->get_base_type(analyzer, &assembly);
                if(rt_type == nullptr)
                    throw _ED(__e_t::virtual_method_not_found, analyzer.current->get_name(method));

                assembly_analyzer_t analyzer0(analyzer, assembly);
                method_ref = rt_type->search_method(analyzer0, prototype);

            } while(method_ref == ref_t::null);

            return get_virtual_method_offset(analyzer, rt_type, method_ref);
        }

        throw _ED(rt_error_code_t::virtual_method_not_found, analyzer.current->get_name(method));
    }

    void assembly_analyzer_t::get_prototype(ref_t method_ref, method_prototype_t * out_prototype)
    {
        rt_assembly_t * rt_assembly;
        rt_general_type_t * rt_type;

        rt_method_t * rt_method = get_method(method_ref, &rt_type, &rt_assembly);
        _A(rt_method != nullptr);

        assembly_analyzer_t(*this, rt_assembly).get_prototype(rt_method, out_prototype);
    }

    void assembly_analyzer_t::get_prototype(rt_method_t * rt_method,
                                                method_prototype_t * out_prototype)
    {
        method_prototype_t * mp = out_prototype;

        mt_method_t * mt = rt_method->mt;

        for(ref_t param_ref : mt->params)
        {
            rt_param_t * rt_param = current->get_param(param_ref);
            _A(rt_param != nullptr);

            rt_type_t * param_type = get_type((*rt_param)->type);
            mp->arg_types.push_back(rt_ptype_t(param_type, (*rt_param)->param_type));
        }

        mp->name = current->get_name(rt_method);
        mp->return_type = get_type(mt->type);
        mp->generic_param_count = mt->generic_params.count;
    }

    ////////// ////////// ////////// ////////// //////////

    rt_vtable_t * build_vtable(memory_t * memory, assembly_analyzer_t & analyzer,
                                                  rt_type_t & rt_type)
    {
        return nullptr;
    }

    ////////// ////////// ////////// ////////// //////////

} } }


