#include <rt.h>

namespace X_ROOT_NS { namespace modules { namespace rt {

    #define __EmptyVTbl ((rt_vtable_t *)0x01)

    typedef rt_error_code_t __e_t;

    ////////// ////////// ////////// ////////// //////////
    
    // Runtime error codes.
    X_ENUM_INFO(rt_error_code_t)

        // Assembly not found.
        X_D(assembly_not_found,         _T("assembly '%1%' not found"))

        // Assembly format error.
        X_D(assembly_format_error,      _T("%1%"))

        // Method not found.
        X_D(method_not_found,           _T("method '%1%' not found"))

        // Field not found.
        X_D(field_not_found,            _T("field '%1%' not found"))

        // Local index out of range.
        X_D(local_index_out_of_range,   _T("local variable index out of range"))

        // Argument index out of range.
        X_D(argument_index_out_of_range, _T("argument index out of range"))

        // Virtual method not found.
        X_D(virtual_method_not_found,   _T("virtual method '%1%' not found"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Storage types.
    X_ENUM_INFO(storage_type_t)

        // Reference types.
        X_C(ref,        _T("ref"))

        // Value types.
        X_C(value,      _T("value"))

        // Mixture of reference and value types.
        X_C(mixture,    _T("mixture"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Runtime type kinds.
    X_ENUM_INFO(rt_type_kind_t)

        // General type
        X_C(general,    _T("general"))

        // Generic type
        X_C(generic,    _T("generic"))

        // Array type
        X_C(array,      _T("array"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Options for searching method.
    X_ENUM_INFO(search_method_options_t)

        // Default.
        X_C(default_,       _T("default"))

        // Only search virtual method.
        X_C(only_virtual,   _T("only_virtual"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Returns whether a method is virtual.
    static bool __is_virtual(decorate_t decorate)
    {
        return decorate.is_virtual || (!decorate.is_override && decorate.is_abstract);
    }

    // Returns whether a method is override.
    static bool __is_override(decorate_t decorate)
    {
        return decorate.is_override;
    }

    ////////// ////////// ////////// ////////// //////////
    // rt_string_t

    // Compare two strings.
    int rt_string_t::compare (const rt_string_t & str) const
    {
        if(__reference_equals(str))
            return true;

        return __compare(str);
    }

    // Returns whether two strings are equals.
    bool rt_string_t::equals(const rt_string_t & str) const
    {
        if(length != str.length)
            return false;

        if(__reference_equals(str))
            return true;

        return __compare(str) == 0;
    }

    // Returns whether two strings are the same reference.
    bool rt_string_t::__reference_equals(const rt_string_t & str) const
    {
        return s == str.s;
    }

    // Compares two strings.
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

    // Converts string to sid.
    rt_sid_t rt_spool_t::to_sid(const rt_string_t & s)
    {
        auto it = __string_set.insert(s);
        return rt_sid_t(&(*it.first));
    }

    // Converts string to sid.
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
    // rt_pool_t

    // Revise key of generic method.
    generic_method_key_t rt_pool_t::revise_key(const generic_method_key_t & key,
                                               rt_type_t ** types, int count)
    {
        return new_key<rpool_key_type_t::generic_method>(
            key.template_(), __find_generic_args_key(key, types, count)
        );
    }

    // Finds generic arguments key.
    __rt_generic_args_key_t rt_pool_t::__find_generic_args_key(
            const generic_method_key_t & key, rt_type_t ** types, int count)
    {
        auto it = __types_map.find(key.args_key());

        if(it != __types_map.end())
            return *it;

        rt_type_t ** new_types = __types_heap.acquire(count);
        std::copy(types, types + count, new_types);

        __rt_generic_args_key_t new_args_key = __rt_generic_args_key_t(
                new_types, new_types + count
         );

        __types_map.insert(new_args_key);
        return new_args_key;
    }

    ////////// ////////// ////////// ////////// //////////
    // rt_type_t

    // Returns size of runtime type.
    msize_t rt_type_t::get_size(analyzer_env_t & env, storage_type_t * out_storage_type)
    {
        this->get_size(env);
        al::assign_value(out_storage_type, __storage_type);

        return __size;
    }

    // Returns size of runtime type.
    msize_t rt_type_t::get_size(analyzer_env_t & env)
    {
        if(__size == unknown_msize)
            __size = this->on_caculate_size(env, &__storage_type);

        return __size;
    }

    // Analyzes runtime type.
    assembly_analyzer_t rt_type_t::__analyzer(analyzer_env_t & env)
    {
        return assembly_analyzer_t(env, this->get_assembly());
    }

    ////////// ////////// ////////// ////////// //////////
    // rt_general_type_t

    // Pre new object.
    void rt_general_type_t::pre_new(analyzer_env_t & env)
    {
        if(__size != unknown_msize)
            return;

        // size
        msize_t size = this->get_size(env);

        // build_vtbl;
        this->__build_vtbl(env);

        // execute static constructor
        this->pre_static_call(env);
    }

    // Pre calling static method.
    void rt_general_type_t::pre_static_call(analyzer_env_t & env)
    {

    }

    // Returns name of general type.
    rt_sid_t rt_general_type_t::get_name(analyzer_env_t & env)
    {
        return assembly->to_sid(mt->name);
    }

    // Returns ttype.
    ttype_t rt_general_type_t::get_ttype(analyzer_env_t & env)
    {
        return (ttype_t)mt->ttype;
    }

    // Enums all fields.
    void rt_general_type_t::each_field(analyzer_env_t & env, each_field_t f)
    {
        for(ref_t field_ref : mt->fields)
        {
            if(!f(field_ref, assembly->get_field(field_ref)))
                break;
        }
    }

    // Enums all methods.
    void rt_general_type_t::each_method(analyzer_env_t & env, each_method_t f)
    {
        for(ref_t method_ref : mt->methods)
        {
            if(!f(method_ref, assembly->get_method(method_ref)))
                break;
        }
    }

    // Gets base type.
    rt_type_t * rt_general_type_t::get_base_type(analyzer_env_t & env)
    {
        ref_t super_types = mt->super_types;
        if(super_types == ref_t::null)
            return nullptr;

        rt_super_type_t * super_type = assembly->get_super_type(super_types[0]);
        _A(super_type != nullptr);

        assembly_analyzer_t analyzer = __analyzer(env);
        rt_type_t * base_type = analyzer.get_type((*super_type)->type);
        _A(base_type != nullptr);

        if(base_type->get_ttype(env) == ttype_t::class_)
            return base_type;

        return nullptr;
    }

    // Gets method offset.
    int rt_general_type_t::get_method_offset(analyzer_env_t & env, ref_t method_ref)
    {
        return method_ref - mt->methods;
    }

    // Compares to methods.
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

    // Searches method.
    ref_t rt_general_type_t::search_method(analyzer_env_t & env,
                    method_prototype_t & prototype, search_method_options_t options)
    {
        bool only_virtual = enum_has_flag(options, search_method_options_t::only_virtual);
        ref_t ret = ref_t::null;

        typedef rt_mt_t<__tidx_t::method> mt_t;

        assembly_analyzer_t analyzer = __analyzer(env);
        assembly->each_methods(mt->methods, [&, this](int idx, mt_t & mt) {

            if(only_virtual && !__is_virtual((decorate_t)mt.decorate))
                return false;

            ref_t method_ref = ref_t(idx);
            rt_method_t * rt_method = analyzer.get_method(method_ref);
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

    // Returns size of structure.
    msize_t __size_of_struct(analyzer_env_t & env, rt_general_type_t * type,
                                                   storage_type_t * out_storage_type)
    {
        vtype_t vtype = (vtype_t)(*type)->vtype;

        if(vtype != vtype_t::mobject)
        {
            al::assign(out_storage_type, storage_type_t::value);
            return get_vtype_size(vtype);
        }

        return type->get_size(env, out_storage_type);
    }

    // Returns size of enum.
    msize_t __size_of_enum(analyzer_env_t & env, rt_general_type_t * type,
                                                 storage_type_t * out_storage_type)
    {
        al::assign(out_storage_type, storage_type_t::value);

        // TODO: size?
        return 0;
    }

    // Returns size of variable.
    msize_t rt_general_type_t::get_variable_size(analyzer_env_t & env,
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
                return __size_of_struct(env, this, out_storage_type);

            case ttype_t::enum_:
                return __size_of_enum(env, this, out_storage_type);

            default:
                X_UNEXPECTED();
        }
    }

    // When caculate size.
    msize_t rt_general_type_t::on_caculate_size(analyzer_env_t & env,
                                                storage_type_t * out_storage_type)
    {
        switch((ttype_t)mt->ttype)
        {
            case ttype_t::interface_:
                return 0;

            case ttype_t::struct_:
                return __size_of_struct(env, this, out_storage_type);

            case ttype_t::enum_:
                return __size_of_enum(env, this, out_storage_type);

            default:
                break;
        }

        rt_type_t * base_type = get_base_type(env);

        msize_t base_size;
        if(base_type == nullptr)
            base_size = 0;
        else
            base_size = base_type->get_size(env);

        assembly_analyzer_t analyzer = __analyzer(env);
        type_layout_t layout(analyzer, base_size, nullptr);
        this->each_field(env, [&](ref_t, rt_field_t * field) {
            return layout.append(field), true;
        });

        layout.commit();

        al::assign(out_storage_type, layout.storage_type());
        return layout.type_size();
    }

    // Gets assembly.
    rt_assembly_t * rt_general_type_t::get_assembly()
    {
        return assembly;
    }

    typedef al::small_vector_t<rt_vfunction_t, 32> rt_vfunctions_t;

    // Builds virtual table.
    static void __build_vtbl(assembly_analyzer_t & analyzer, rt_type_t * type,
                                                             rt_vfunctions_t & vfuncs)
    {
        rt_type_t * base_type = type->get_base_type(analyzer.env);

        if(base_type != nullptr)
            __build_vtbl(analyzer, base_type, vfuncs);

        ref_t ret = ref_t::null;

        typedef rt_mt_t<__tidx_t::method> mt_t;

        rt_assembly_t * assembly = type->get_assembly();
        assembly_analyzer_t base_analyzer(analyzer, assembly);

        type->each_method(base_analyzer.env, [&](ref_t method_ref, rt_method_t * rt_method) {

            decorate_t decorate = (decorate_t)(*rt_method)->decorate;

            if(__is_virtual(decorate))
            {
                vfuncs.push_back(rt_vtable_function_t(assembly->index, method_ref.index));
            }
            else if(__is_override(decorate))
            {
                method_prototype_t prototype;
                analyzer.get_prototype(method_ref, &prototype);

                bool found = false;
                for(rt_vtable_function_t & func : vfuncs)
                {
                    rt_assembly_t * rt_assembly0 = base_analyzer.env.assemblies.at(
                        func.assembly_idx
                    );

                    rt_method_t * rt_method0 = rt_assembly0->get_method(ref_t(func.method_idx));

                    assembly_analyzer_t analyzer0(analyzer, rt_assembly0);
                    if(__compare_method(analyzer0, prototype, rt_method0))
                    {
                        func = rt_vtable_function_t(assembly->index, method_ref.index);

                        found = true;
                        break;
                    }
                }

                if(!found)
                {
                    auto method_name = assembly->get_name(rt_method);
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
                analyzer.env.get_memory(),
                sizeof(rt_vtable_t) + vfuncs.size() * sizeof(rt_vfunction_t)
            );

            al::copy(vfuncs, vtable->functions);
            set_vtable(type, vtable);
        }
    }

    // Builds virtual table.
    void rt_general_type_t::__build_vtbl(analyzer_env_t & env)
    {
        assembly_analyzer_t analyzer = __analyzer(env);

        al::small_vector_t<rt_vfunction_t, 32> vfuncs;
        rt::__build_vtbl(analyzer, this, vfuncs);
    }

    ////////// ////////// ////////// ////////// //////////
    // rt_generic_type_t

    // Pre new object.
    void rt_generic_type_t::pre_new(analyzer_env_t & env)
    {
        
    }

    // Pre calling static method.
    void rt_generic_type_t::pre_static_call(analyzer_env_t & env)
    {

    }

    // Gets ttype.
    ttype_t rt_generic_type_t::get_ttype(analyzer_env_t & env)
    {
        return ttype_t::__unknown__;
    }

    // Gets name.
    rt_sid_t rt_generic_type_t::get_name(analyzer_env_t & env)
    {
        return __analyzer(env).to_sid(_T("GENERIC"));  // TODO: implement it
    }

    // Gets base type.
    rt_type_t * rt_generic_type_t::get_base_type(analyzer_env_t & env)
    {
        return nullptr;
    }

    // Gets method offset.
    int rt_generic_type_t::get_method_offset(analyzer_env_t & env, ref_t method_ref)
    {
        return 0;
    }

    // Searches method.
    ref_t rt_generic_type_t::search_method(analyzer_env_t & env,
                    method_prototype_t & prototype, search_method_options_t options)
    {
        return ref_t::null;
    }

    // Gets variable size.
    msize_t rt_generic_type_t::get_variable_size(analyzer_env_t & env,
                                                 storage_type_t * out_storage_type)
    {
        return 0;
    }

    // Gets assembly.
    rt_assembly_t * rt_generic_type_t::get_assembly()
    {
        _A(template_ != nullptr);

        return template_->get_assembly();
    }

    // When caculate size.
    msize_t rt_generic_type_t::on_caculate_size(analyzer_env_t & env,
                                                storage_type_t * out_storage_type)
    {
        return 0;
    }

    ////////// ////////// ////////// ////////// //////////
    // rt_array_type_t

    rt_sid_t rt_array_type_t::get_name(analyzer_env_t & env)
    {
        assembly_analyzer_t analyzer = __analyzer(env);
        rt_type_t * type = analyzer.get_type(mt->element_type);

        stringstream_t ss;
        if(type == nullptr)
        {
            ss << _T("?");
        }
        else
        {
            string_t name = type->get_name(env);
            ss << name.c_str();
        }

        ss << _T("[");

        for(int k = 1, dimension = mt->dimension; k < dimension; k++)
        {
            ss << _T(", ");
        }

        ss << _T("]");

        return analyzer.to_sid(ss.str());
    }

    // Pre new object.
    void rt_array_type_t::pre_new(analyzer_env_t & env)
    {
        this->pre_static_call(env);
    }

    // Pre calling static method.
    void rt_array_type_t::pre_static_call(analyzer_env_t & env)
    {
        if(element_type == nullptr)
        {
            element_type = __analyzer(env).get_type((*this)->element_type);
            element_type->pre_new(env);
        }
    }

    // Gets ttype.
    ttype_t rt_array_type_t::get_ttype(analyzer_env_t & env)
    {
        return ttype_t::class_;
    }

    // Gets base type.
    rt_type_t * rt_array_type_t::get_base_type(analyzer_env_t & env)
    {
        return nullptr;
    }

    // Gets offset of method.
    int rt_array_type_t::get_method_offset(analyzer_env_t & env, ref_t method_ref)
    {
        return 0;
    }

    // Searches method.
    ref_t rt_array_type_t::search_method(analyzer_env_t & env,
                    method_prototype_t & prototype, search_method_options_t options)
    {
        return ref_t::null;
    }

    // Gets variable size.
    msize_t rt_array_type_t::get_variable_size(analyzer_env_t & env,
                                                        storage_type_t * out_storage_type)
    {
        al::assign(out_storage_type, storage_type_t::ref);
        return rt_ref_size;
    }

    // When caculate size.
    msize_t rt_array_type_t::on_caculate_size(analyzer_env_t & env,
                                                        storage_type_t * out_storage_type)
    {
        al::assign(out_storage_type, storage_type_t::ref);
        return rt_ref_size;
    }

    // Gets assembly.
    rt_assembly_t * rt_array_type_t::get_assembly()
    {
        _A(element_type != nullptr);

        return element_type->get_assembly();
    }

    ////////// ////////// ////////// ////////// //////////
    // rt_method_t

    // Gets assembly.
    rt_assembly_t * rt_method_t::get_assembly()
    {
        _A(host_type != nullptr);

        return host_type->get_assembly();
    }

    // Gets host type.
    rt_type_t * rt_method_t::get_host_type()
    {
        return host_type;
    }

    // Gets name.
    rt_sid_t rt_method_t::get_name()
    {
        return get_assembly()->get_name(this);
    }

    ////////// ////////// ////////// ////////// //////////
    // rt_generic_method_t

    // Gets assembly.
    rt_assembly_t * rt_generic_method_t::get_assembly()
    {
        _A(template_ != nullptr);

        return template_->get_assembly();
    }

    // Gets host type.
    rt_type_t * rt_generic_method_t::get_host_type()
    {
        _A(template_ != nullptr);

        return template_->get_host_type();
    }

    // Gets name.
    rt_sid_t rt_generic_method_t::get_name()
    {
        _A(template_ != nullptr);

        return template_->get_name();
    }

    ////////// ////////// ////////// ////////// //////////

    // Joins assembly name.
    string_t join_assembly_name(const string_t & package, const string_t & name)
    {
        if(package.length() == 0)
            return name;

        return _F(_T("%s.%s"), package, name);
    }

    // Joins method name.
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

    ////////// ////////// ////////// ////////// //////////
    // rt_assemblies

    // Loads main assembly.
    rt_assembly_t * rt_assemblies_t::load_main_assembly()
    {
        if(__main_assembly == nullptr)
        {
            __main_assembly = __provider->load_main_assembly(__ctx);
            __revise_assembly(__main_assembly);
        }

        return __main_assembly;
    }

    // Loads assembly.
    rt_assembly_t * rt_assemblies_t::load_assembly(const string_t & package, const string_t & name)
    {
        __assembly_key_t key(package, name);

        auto it = __assembly_map.find(key);
        if(it != __assembly_map.end())
            return it->second;

        rt_assembly_t * assembly = __provider->load_assembly(__ctx, package, name);
        __revise_assembly(assembly);

        if(assembly == nullptr)
            throw _ED(__e_t::assembly_not_found, join_assembly_name(package, name));

        __assembly_map[key] = assembly;
        return assembly;
    }

    // Returns assembly at specified index.
    rt_assembly_t * rt_assemblies_t::at(int index)
    {
        if(index < 0 || index >= __assembly_vector.size())
            throw _EC(overflow);

        return __assembly_vector[index];
    }

    // Revise assembly, Assigns it's index.
    void rt_assemblies_t::__revise_assembly(rt_assembly_t * assembly)
    {
        if(assembly->index < 0)
        {
            assembly->index = __assembly_vector.size();
            __assembly_vector.push_back(assembly);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // assembly_analyzer_t

    // Gets host type by method ref.
    rt_type_t * assembly_analyzer_t::get_host_type_by_method_ref(ref_t method_ref,
                                                rt_method_ref_t ** out_method_ref)
    {
        switch((mt_member_extra_t)method_ref.extra)
        {
            case mt_member_extra_t::import: {
                rt_method_ref_t * rt_method_ref = current->get_method_ref(method_ref);
                al::assign_value(out_method_ref, rt_method_ref);

                return get_host_type(rt_method_ref);
            }

            case mt_member_extra_t::internal: {
                al::assign_value(out_method_ref, (rt_method_ref_t *)nullptr);
                return current->get_host_by_method_ref(method_ref);
            }

            case mt_member_extra_t::generic: {
                rt_generic_method_t * rt_generic_method = get_generic_method(method_ref);
                _A(rt_generic_method != nullptr);

                return rt_generic_method->get_host_type();
            }

            default:
                X_UNEXPECTED();
        }
    }

    // Gets host type by field ref.
    rt_type_t * assembly_analyzer_t::get_host_type_by_field_ref(ref_t field_ref,
                                                rt_field_ref_t ** out_field_ref)
    {
        switch((mt_member_extra_t)field_ref.extra)
        {
            case mt_member_extra_t::import: {
                rt_field_ref_t * rt_field_ref = current->get_field_ref(field_ref);
                al::assign_value(out_field_ref, rt_field_ref);

                return get_host_type(rt_field_ref);
            }

            case mt_member_extra_t::internal: {
                rt_field_t * rt_field = current->get_field(field_ref);
                return current->get_host_by_field_ref(field_ref);
            }

            default:
                X_UNEXPECTED();
        }
    }

    // Gets host type.
    rt_type_t * assembly_analyzer_t::get_host_type(rt_method_ref_t * rt)
    {
        _A(rt != nullptr);

        return get_host_type(rt->mt);
    }

    // Gets host type.
    rt_type_t * assembly_analyzer_t::get_host_type(rt_field_ref_t * rt)
    {
        _A(rt != nullptr);

        return get_host_type(rt->mt);
    }

    // Gets host type.
    rt_type_t * assembly_analyzer_t::get_host_type(mt_method_ref_t * mt)
    {
        _A(mt != nullptr);

        return get_type(mt->host);
    }

    // Gets host type.
    rt_type_t * assembly_analyzer_t::get_host_type(mt_field_ref_t * mt)
    {
        _A(mt != nullptr);

        return get_type(mt->host);
    }

    // Gets assembly.
    rt_assembly_t * assembly_analyzer_t::get_assembly(mt_type_ref_t * mt)
    {
        _A(mt != nullptr);

        return get_assembly(mt->assembly);
    }

    // Gets assembly.
    rt_assembly_t * assembly_analyzer_t::get_assembly(ref_t assembly_ref)
    {
        rt_assembly_ref_t * rt = current->get_assembly_ref(assembly_ref);
        return get_assembly(rt);
    }

    // Gets assembly.
    rt_assembly_t * assembly_analyzer_t::get_assembly(rt_assembly_ref_t * rt)
    {
        _A(rt != nullptr);

        return get_assembly(rt->mt);
    }

    // Gets assembly.
    rt_assembly_t * assembly_analyzer_t::get_assembly(mt_assembly_ref_t * mt)
    {
        _A(mt != nullptr);

        string_t package = current->to_sid(mt->package);
        string_t name    = current->to_sid(mt->name);

        rt_assembly_t * ref_assembly = env.assemblies.load_assembly(
            package, name
        );

        if(ref_assembly == nullptr)
            throw _ED(rt_error_code_t::assembly_not_found, join_assembly_name(package, name));

        return ref_assembly;
    }

    // Gets type.
    rt_type_t * assembly_analyzer_t::get_type(ref_t type_ref)
    {
        if(type_ref == ref_t::null)
            return nullptr;

        switch((mt_type_extra_t)type_ref.extra)
        {
            case mt_type_extra_t::general: {
                return current->get_type(type_ref);
            }

            case mt_type_extra_t::type_ref: {
                rt_type_ref_t * rt_type_ref = current->get_type_ref(type_ref);
                mt_type_ref_t * mt_type_ref = rt_type_ref->mt;
                rt_assembly_t * rt_assembly = get_assembly(mt_type_ref);

                return rt_assembly->get_type(
                    __to_sid(mt_type_ref->namespace_), __to_sid(mt_type_ref->name),
                    mt_type_ref->generic_param_count
                );
            }

            case mt_type_extra_t::array: {
                rt_array_type_t * arr_type = current->get_array_type(type_ref);
                if(arr_type->element_type == nullptr)
                {
                    rt_assembly_t * element_assembly;
                    arr_type->element_type = get_type((*arr_type)->element_type);
                }
            }

            case mt_type_extra_t::generic: {
                return current->get_generic_type(type_ref);
            }

            case mt_type_extra_t::generic_param:
            default:
                X_UNEXPECTED();
        }
    }

    // Gets method.
    rt_method_t * assembly_analyzer_t::get_method(ref_t method_ref)
    {
        auto key = env.rpool.new_key<rpool_key_type_t::method>(current, method_ref);
        return env.rpool.get(key, [this, method_ref]() {
            return __get_method(method_ref);
        });
    }

    // Gets generic method.
    rt_generic_method_t * assembly_analyzer_t::get_generic_method(ref_t method_ref)
    {
        return current->get_generic_method(method_ref);
    }

    // Gets method.
    rt_method_t * assembly_analyzer_t::__get_method(ref_t method_ref)
    {
        rt_method_ref_t * rt_method_ref;
        rt_type_t * rt_type = get_host_type_by_method_ref(method_ref, &rt_method_ref);
        _A(rt_type != nullptr);

        rt_assembly_t * rt_assembly = current; // rt_type->get_assembly();

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

            rt_type->each_method(env, [&, this](ref_t method_ref, rt_method_t * rt_method0) {
                rt_mt_t<__tidx_t::method> & mt = *rt_method0->mt;
                bool r = __compare_method(rt_assembly, mt, method_name,
                    mt.generic_params.count, return_type, mt.params.count, params
                );

                return r? rt_method = rt_assembly->get_method(method_ref), false : true;
            });

            if(rt_method == nullptr)
            {
                throw _ED(rt_error_code_t::method_not_found,
                    __join_method_name(rt_assembly, rt_type, mt, params)
                );
            }
        }

        return rt_method;
    }

    // Compare method with name and arguments.
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

    // Gets field.
    rt_field_t * assembly_analyzer_t::get_field(ref_t field_ref, rt_type_t ** out_type)
    {
        auto key = env.rpool.new_key<rpool_key_type_t::field>(current, field_ref);
        auto r = env.rpool.get(key, [this, field_ref]() {
            rt_type_t * type;
            rt_field_t * field = __get_field(field_ref, &type);
            return std::make_tuple(field, type);
        });

        al::assign_value(out_type, std::get<rt_type_t *>(r));
        return std::get<rt_field_t *>(r);
    }

    // Gets field.
    rt_field_t * assembly_analyzer_t::__get_field(ref_t field_ref, rt_type_t ** out_type)
    {
        rt_field_ref_t * rt_field_ref;
        rt_type_t * rt_type = get_host_type_by_field_ref(field_ref, &rt_field_ref);
        rt_assembly_t * rt_assembly = rt_type->get_assembly();

        al::assign_value(out_type, rt_type);

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
            rt_type->each_field(env, [&, this](ref_t field_ref, rt_field_t * rt_field) {
                rt_mt_t<__tidx_t::field> & mt = *rt_field->mt;
                bool r = (field_name == rt_assembly->to_sid(mt.name));
                return r? rt_field = rt_assembly->get_field(field_ref), false : true;
            });

            if(rt_field == nullptr)
            {
                throw _ED(rt_error_code_t::field_not_found,
                    __join_field_name(rt_assembly, rt_type, mt)
                );
            }
        }

        return rt_field;
    }

    // Joins method name.
    string_t assembly_analyzer_t::__join_method_name(rt_assembly_t * assembly,
            rt_type_t * type, mt_method_ref_t * mt_method_ref, rt_ptype_t * ptypes)
    {
        return current->to_sid(mt_method_ref->name);
    }

    // Joins field name.
    string_t assembly_analyzer_t::__join_field_name(rt_assembly_t * assembly,
            rt_type_t * type, mt_field_ref_t * mt_field_ref)
    {
        return current->to_sid(mt_field_ref->name);
    }

    // Returns size of type.
    msize_t assembly_analyzer_t::size_of(ref_t type_ref, storage_type_t * out_storage_type)
    {
        rt_type_t * type = this->get_type(type_ref);

        if(type == nullptr)
            return 0;

        return type->get_size(env, out_storage_type);
    }

    // Returns size of type.
    msize_t assembly_analyzer_t::size_of(rt_type_t * type, storage_type_t * out_storage_type)
    {
        if(type == nullptr)
            return 0;

        return type->get_size(env, out_storage_type);
    }

    // Gets size of variable.
    msize_t assembly_analyzer_t::variable_size_of(ref_t type_ref,
                                                        storage_type_t * out_storage_type)
    {
        rt_type_t * type = this->get_type(type_ref);
        if(type == nullptr)
            return 0;

        return type->get_variable_size(env, out_storage_type);
    }

    // Gets size of variable.
    msize_t assembly_analyzer_t::variable_size_of(rt_type_t * type,
                                                        storage_type_t * out_storage_type)
    {
        if(type == nullptr)
            return 0;

        return type->get_variable_size(env, out_storage_type);
    }

    // Gets base type.
    rt_type_t * assembly_analyzer_t::get_base_type(rt_type_t * type)
    {
        _A(type != nullptr);
        return type->get_base_type(env);
    }

    // Gets type size.
    msize_t assembly_analyzer_t::get_type_size(ref_t type_ref)
    {
        rt_type_t * rt_type = get_type(type_ref);
        _A(rt_type != nullptr);

        return rt_type->get_size(env);
    }

    // Gets field offset.
    msize_t assembly_analyzer_t::get_field_offset(ref_t field_ref)
    {
        rt_type_t * rt_type;
        rt_field_t * rt_field = this->get_field(field_ref, &rt_type);

        _A(rt_field != nullptr);
        _A(rt_type != nullptr);

        if(rt_field->offset == unknown_msize)
            rt_type->get_size(env);

        return rt_field->offset;
    }

    // Gets virtual method offset.
    int assembly_analyzer_t::get_virtual_method_offset(assembly_analyzer_t & analyzer,
                                                       ref_t method_ref)
    {
        rt_method_t * method = analyzer.current->get_method(method_ref);
        _A(method != nullptr);

        rt_type_t * rt_type = method->get_host_type();

        decorate_t decorate = (decorate_t)(*method)->decorate;
        if(__is_virtual(decorate))
            return rt_type->get_method_offset(analyzer.env, method_ref);

        if(decorate.is_override)
        {
            method_prototype_t prototype;
            analyzer.get_prototype(method_ref, &prototype);

            do
            {
                rt_type = rt_type->get_base_type(analyzer.env);
                if(rt_type == nullptr)
                    throw _ED(__e_t::virtual_method_not_found, analyzer.current->get_name(method));

                assembly_analyzer_t analyzer0(analyzer, rt_type->get_assembly());
                method_ref = rt_type->search_method(analyzer0.env, prototype);

            } while(method_ref == ref_t::null);

            return get_virtual_method_offset(analyzer, method_ref);
        }

        throw _ED(rt_error_code_t::virtual_method_not_found, analyzer.current->get_name(method));
    }

    // Gets prototype of method.
    void assembly_analyzer_t::get_prototype(ref_t method_ref, method_prototype_t * out_prototype)
    {
        rt_method_t * rt_method = get_method(method_ref);
        _A(rt_method != nullptr);

        rt_assembly_t * rt_assembly = rt_method->get_assembly();
        _A(rt_assembly != nullptr);

        assembly_analyzer_t(*this, rt_assembly).get_prototype(rt_method, out_prototype);
    }

    // Gets prototype of method.
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

    // Returns sid of string.
    rt_sid_t assembly_analyzer_t::to_sid(const string_t & s)
    {
        return current->to_sid(s);
    }

    // Gets name of runtime type.
    rt_sid_t assembly_analyzer_t::get_name(rt_type_t * type)
    {
        _A(type != nullptr);

        return type->get_name(env);
    }

    // Gets name of runtime method.
    rt_sid_t assembly_analyzer_t::get_name(rt_method_t * method)
    {
        _A(method != nullptr);

        return current->get_name(method);
    }

    // Gets name of runtime method.
    rt_sid_t assembly_analyzer_t::get_name(rt_generic_method_t * method)
    {
        _A(method != nullptr);

        return method->get_name();
    }

    // Converts to assembly_analyzer_t.
    assembly_analyzer_t to_analyzer(analyzer_env_t & env, rt_type_t * rt_type)
    {
        _A(rt_type != nullptr);

        return assembly_analyzer_t(env, rt_type->get_assembly());
    }

    ////////// ////////// ////////// ////////// //////////

    // Builds virtual table.
    rt_vtable_t * build_vtable(memory_t * memory, assembly_analyzer_t & analyzer,
                                                  rt_type_t & rt_type)
    {
        return nullptr;
    }

    ////////// ////////// ////////// ////////// //////////

    // Creates a new metadata for generic method.
    rt_generic_method_t * rt_mt_t<__tidx_t::generic_method>::new_entity(rt_context_t & ctx,
                                ref_t ref, __mt_t * mt, rt_assembly_t * assembly)
    {
        analyzer_env_t env = to_env(ctx);
        assembly_analyzer_t analyzer(env, assembly);

        _PP(mt->template_);
        rt_method_t * template_ = analyzer.get_method(mt->template_);
        _A(template_ != nullptr);

        _PF(_T("------ new_generic_method: %1%, %2%"), mt->template_, template_->get_name());

        int atype_count = mt->args.count;
        type_t * atypes[atype_count];

        rt_type_t ** ptype = (rt_type_t **)atypes;

        for(ref_t ga_ref : mt->args)
        {
            rt_generic_argument_t * generic_argument = assembly->get_generic_argument(ga_ref);
            ref_t atype_ref = (*generic_argument)->type;
            rt_type_t * type = analyzer.get_type(atype_ref);
            *ptype++ = type;
        }

        __rt_generic_args_key_t args_key((rt_type_t **)atypes, (rt_type_t **)atypes + atype_count);
        auto key = ctx.rpool.new_key<rpool_key_type_t::generic_method>(template_, args_key);

        return ctx.rpool.get(key, [&]() {
            key = ctx.rpool.revise_key(key, (rt_type_t **)atypes, atype_count);
            return memory_t::new_obj<rt_generic_method_t>(ctx.memory, template_, key.types());
        });
    }

    //-------- ---------- ---------- ---------- ----------

    // Creates a new metadata for general method.
    rt_method_t * rt_mt_t<__tidx_t::method>::new_entity(rt_context_t & ctx,
                                ref_t ref, __mt_t * mt, rt_assembly_t * assembly)
    {
        rt_method_t * method = memory_t::new_obj<rt_method_t>(ctx.memory);

        analyzer_env_t env = to_env(ctx);
        assembly_analyzer_t analyzer(env, assembly);

        method->mt = mt;
        method->host_type = analyzer.get_host_type_by_method_ref(ref);

        return method;
    }

    ////////// ////////// ////////// ////////// //////////

} } }


