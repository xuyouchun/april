#include <rt.h>
#include "utils.h"

namespace X_ROOT_NS::modules::rt {

    #define __EmptyVTbl ((rt_vtable_t *)0x01)

    typedef rt_error_code_t __e_t;
    typedef generic_param_manager_t __gp_mgr_t;

    ////////// ////////// ////////// ////////// //////////
    
    // Runtime error codes.
    X_ENUM_INFO(rt_error_code_t)

        // Assembly not found.
        X_D(assembly_not_found,         _T("assembly '%1%' not found"))

        // Assembly for mat error.
        X_D(assembly_error,             _T("%1%"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Member type.
    X_ENUM_INFO(rt_member_type_t)

        X_C(general,    _T("general"))

        X_C(generic,    _T("generic"))

        X_C(array,      _T("array"))

        X_C(tuple,      _T("tuple"))

        X_C(position,   _T("position"))

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

    // Options for  searching method.
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

    static bool __is_tuple_type(rt_type_t * type)
    {
        _A(type != nullptr);

        switch (type->get_kind())
        {
            case rt_type_kind_t::generic:
                return __is_tuple_type(((rt_generic_type_t *)type)->template_);

            case rt_type_kind_t::general:
                return  (mtype_t)(*(rt_general_type_t *)type)->mtype == mtype_t::tuple;

            default:
                return false;
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // rt_string_t

    // Compare two strings.
    int rt_string_t::compare(const rt_string_t & str) const
    {
        if (__reference_equals(str))
            return true;

        return __compare(str);
    }

    // Returns whether two strings are equals.
    bool rt_string_t::equals(const rt_string_t & str) const
    {
        if (length != str.length)
            return false;

        if (__reference_equals(str))
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
        for (const char_t * s0 = s, * s0_end = s0 + min_length, * s1 = str.s;
                                            s0 < s0_end; s0++, s1++)
        {
            if (*s0 != *s1)
                return *s0 - *s1;
        }

        return length - str.length;
    }

    //-------- ---------- ---------- ---------- ----------
    // rt_sid_t

    const rt_sid_t rt_sid_t::null;

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
        if (it == __string_set.end())
        {
            char_t * dst = __heap.acquire(s.length() + 1);
            al::strcpy(dst, s.c_str());
            it = __string_set.insert(rt_string_t(dst, s.length())).first;
        }

        return rt_sid_t(&(*it));
    }

    ////////// ////////// ////////// ////////// //////////
    // rt_pool_t

    rt_pool_t::rt_pool_t(memory_t * memory) : __memory(memory), __msize_heap(memory)
    {
        __tuple_fields_heap = __pool.new_obj<al::heap_t<rt_tuple_field_t[]>>();
    }

    // Revise key of generic method.
    generic_method_key_t rt_pool_t::revise_key(const generic_method_key_t & key,
                                               rt_type_t ** types, int count)
    {
        return new_key<rpool_key_type_t::generic_method>(
            key.template_(), key.host_type(),
            __find_generic_args_key(key.args_key(), types, count)
        );
    }

    generic_type_key_t rt_pool_t::revise_key(const generic_type_key_t & key,
                                                rt_type_t ** types, int count)
    {
        return new_key<rpool_key_type_t::generic_type>(
            key.template_(), key.host_type(),
            __find_generic_args_key(key.args_key(), types, count)
        );
    }

    // Finds generic arguments key.
    __rt_generic_args_key_t rt_pool_t::__find_generic_args_key(
            const __rt_generic_args_key_t & args_key, rt_type_t ** types, int count)
    {
        auto it = __types_map.find(args_key);

        if (it != __types_map.end())
            return *it;

        rt_type_t ** new_types = __types_heap.acquire(count);
        std::copy(types, types + count, new_types);

        __rt_generic_args_key_t new_args_key = __rt_generic_args_key_t(
                new_types, new_types + count
        );

        __types_map.insert(new_args_key);
        return new_args_key;
    }

    // Gets generic type.
    rt_generic_type_t * rt_pool_t::to_generic_type(memory_t * memory,
            rt_general_type_t * template_, rt_type_t * host_type, rt_type_t ** atypes,
            int atype_count)
    {
        _A(template_ != nullptr);

        // size_t atype_count = template_->generic_param_count();
        __rt_generic_args_key_t args_key(atypes, atypes + atype_count);
        auto key = new_key<rpool_key_type_t::generic_type>(template_, host_type, args_key);

        return get(key, [&]() {

            key = revise_key(key, (rt_type_t **)atypes, atype_count);
            return memory_t::new_obj<rt_generic_type_t>(memory,
                template_, host_type, key.types(), atype_count
            );

        });
    }


    // Gets array type.
    rt_array_type_t * rt_pool_t::to_array_type(memory_t * memory, rt_type_t * element_type,
                                               dimension_t dimension)
    {
        _A(element_type != nullptr);

        auto key = new_key<rpool_key_type_t::array_type>(element_type, dimension);
        return get(key, [memory, element_type, dimension]() {
            return memory_t::new_obj<rt_array_type_t>(memory, element_type, dimension);
        });
    }

    // Creates a msize array.
    msize_t * rt_pool_t::new_msize_array(size_t count)
    {
        return __msize_heap.acquire(count);
    }

    // Creates a rt_tuple_field_t array.
    rt_tuple_field_t * rt_pool_t::new_tuple_field_array(size_t count)
    {
        return __tuple_fields_heap->acquire(count);
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
        if (__size == unknown_msize)
            __size = this->on_caculate_size(env, &__storage_type);

        _A(__size != unknown_msize);
        return __size;
    }

    // Return count of virtual members.
    msize_t rt_type_t::get_virtual_count(analyzer_env_t & env)
    {
        if (__virtual_count == unknown_msize)
        {
            msize_t count = on_caculate_virtual_count(env);
            rt_type_t * base_type = this->get_base_type(env);

            if (base_type != nullptr)
                count += base_type->get_virtual_count(env);

            __virtual_count = count;
        }

        return __virtual_count;
    }

    // Analyzes runtime type.
    assembly_analyzer_t rt_type_t::__analyzer(analyzer_env_t & env,
                                              const generic_param_manager_t * gp_manager)
    {
        return assembly_analyzer_t(env, this->get_assembly(), gp_manager);
    }

    // When caculate virtual count.
    msize_t rt_type_t::on_caculate_virtual_count(analyzer_env_t & env)
    {
        ttype_t ttype = get_ttype(env);

        if (ttype != ttype_t::class_)
            return 0;

        msize_t count = 0;
        this->each_method(env, [&count](ref_t, rt_method_t * method) {
            return __is_virtual((decorate_t)(*method)->decorate)? (++count, true) : false;
        });

        return count;
    }

    // Returns whether it's a generic type.
    bool is_general(rt_type_t * type)
    {
        _A(type != nullptr);

        return type->get_kind() == rt_type_kind_t::general;
    }

    // Returns whether it's a custom struct.
    bool is_custom_struct(analyzer_env_t & env, rt_type_t * type)
    {
        if (type == nullptr)    // Maybe void
            return false;

        return type->get_vtype(env) == vtype_t::mobject_ &&
            type->get_ttype(env) == ttype_t::struct_;
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns size of structure.
    msize_t __size_of_struct(analyzer_env_t & env, rt_type_t * type,
                                                   storage_type_t * out_storage_type)
    {
        _A(type != nullptr);

        vtype_t vtype = type->get_vtype(env);

        if (vtype != vtype_t::mobject_)
        {
            al::assign(out_storage_type, storage_type_t::value);
            return get_vtype_size(vtype);
        }

        return unknown_msize;
    }

    // Returns size of enum.
    msize_t __size_of_enum(analyzer_env_t & env, rt_type_t * type,
                                                 storage_type_t * out_storage_type)
    {
        al::assign(out_storage_type, storage_type_t::value);

        vtype_t vtype = type->get_vtype(env);
        return get_vtype_size(vtype);
    }

    ////////// ////////// ////////// ////////// //////////
    // General like type.

    // When caculate size.
    msize_t __rt_general_like_type_t::on_caculate_size(analyzer_env_t & env,
                                                       storage_type_t * out_storage_type)
    {
        msize_t msize;

        switch (this->get_ttype(env))
        {
            case ttype_t::interface_:
                X_UNEXPECTED();

            case ttype_t::struct_:
                msize = __size_of_struct(env, this, out_storage_type);
                break;

            case ttype_t::enum_:
                msize = __size_of_enum(env, this, out_storage_type);
                break;

            default:
                msize = unknown_msize;
                break;
        }

        if (msize != unknown_msize)
            return msize;

        rt_type_t * base_type = get_base_type(env);

        msize_t base_size;
        if (base_type == nullptr)
            base_size = 0;
        else
            base_size = base_type->get_size(env);

        return this->on_caculate_layout(env, base_size, out_storage_type);
    }

    // Returns size of variable.
    msize_t __rt_general_like_type_t::get_variable_size(analyzer_env_t & env,
                                                 storage_type_t * out_storage_type)
    {
        ttype_t ttype = get_ttype(env);

        switch (ttype)
        {
            case ttype_t::class_:
            case ttype_t::interface_:
                al::assign(out_storage_type, storage_type_t::ref);
                return rt_ref_size;

            case ttype_t::struct_:
            case ttype_t::enum_:
                return this->get_size(env, out_storage_type);

            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // rt_general_type_t

    // Pre new object.
    void rt_general_type_t::pre_new(analyzer_env_t & env)
    {
        if (__size != unknown_msize)
            return;

        rt_type_t * base_type = get_base_type(env);
        if (base_type != nullptr)
            base_type->pre_new(env);

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
        // Empty.
    }

    // Returns name of general type.
    rt_sid_t rt_general_type_t::get_name(analyzer_env_t & env)
    {
        ref_t generic_params = (*this)->generic_params;
        if (generic_params.count == 0 && host_type == nullptr)
            return assembly->to_sid(mt->name);

        stringstream_t ss;

        if (host_type != nullptr)
            ss << host_type->get_name(env).c_str() << _T(".");

        assembly_analyzer_t analyzer = __analyzer(env);
        if (generic_params.count > 0)
        {
            ss << assembly->to_sid(mt->name).c_str();
            ss << _T("<");

            for  (ref_t gp : generic_params)
            {
                if (gp.index > generic_params.index)
                    ss << _T(",");

                rt_generic_param_t * rt_gp = analyzer.get_generic_param(gp);
                _A(rt_gp != nullptr);

                ss << analyzer.to_sid((*rt_gp)->name).c_str();

                if ((*rt_gp)->param_type == generic_param_type_t::params)
                    ss << _T("...");
            }

            ss << _T(">");
        }

        return analyzer.to_sid(ss.str());
    }

    // Returns ttype.
    ttype_t rt_general_type_t::get_ttype(analyzer_env_t & env)
    {
        return (ttype_t)mt->ttype;
    }

    // Enumerates all fields.
    void rt_general_type_t::each_field(analyzer_env_t & env, each_field_t f)
    {
        for  (ref_t field_ref : mt->fields)
        {
            if (!f(field_ref, assembly->get_field(field_ref)))
                break;
        }
    }

    // Enumerates all methods.
    void rt_general_type_t::each_method(analyzer_env_t & env, each_method_t f)
    {
        for (ref_t method_ref : mt->methods)
        {
            if (!f(method_ref, assembly->get_method(method_ref)))
                break;
        }
    }

    // Gets base type.
    rt_type_t * rt_general_type_t::get_base_type(analyzer_env_t & env,
                                                 const generic_param_manager_t * gp_manager)
    {
        ref_t super_types = mt->super_types;
        if (super_types == ref_t::null)
            return nullptr;

        rt_super_type_t * super_type = assembly->get_super_type(super_types[0]);
        _A(super_type != nullptr);

        assembly_analyzer_t analyzer = __analyzer(env, gp_manager);
        rt_type_t * base_type = analyzer.get_type((*super_type)->type);
        _A(base_type != nullptr);

        if (base_type->get_ttype(env) == ttype_t::class_)
            return base_type;

        return nullptr;
    }

    // Enum all super types.
    void rt_general_type_t::each_super_type(analyzer_env_t & env,
                    each_super_type_callback_t callback, const __gp_mgr_t * gp_manager)
    {
        ref_t super_types = mt->super_types;
        if (super_types == ref_t::null)
            return;

        assembly_analyzer_t analyzer = __analyzer(env, gp_manager);

        for (ref_t ref : super_types)
        {
            rt_super_type_t * super_type = assembly->get_super_type(ref);
            _A(super_type != nullptr);

            rt_type_t * type = analyzer.get_type((*super_type)->type);
            _A(type != nullptr);

            if (!callback(type))
                break;
        }
    }

    // Gets method offset.
    int rt_general_type_t::get_method_offset(analyzer_env_t & env, ref_t method_ref)
    {
        return method_ref - mt->methods;
    }

    // Gets field offset.
    msize_t rt_general_type_t::get_field_offset(analyzer_env_t & env, ref_t ref)
    {
        rt_field_base_t * field = __analyzer(env).get_field(ref);
        _A(field != nullptr);

        if (field->offset == unknown_msize)
            this->get_size(env);

        _A(field->offset != unknown_msize);

        return field->offset;
    }

    // Gets field offset.
    msize_t rt_general_type_t::get_field_offset(analyzer_env_t & env, int position)
    {
        X_UNIMPLEMENTED();
    }

    // Compares to methods.
    static bool __compare_method(analyzer_env_t & env, method_prototype_t & prototype,
                                                                 rt_method_t * rt_method)
    {
        rt_assembly_t * rt_assembly = rt_method->get_assembly();
        assembly_analyzer_t analyzer(env, rt_assembly);
        mt_method_t & mt = *rt_method->mt;

        if (rt_method->get_name() != prototype.name)
            return false;

        if (prototype.generic_param_count != mt.generic_params.count)
            return false;

        if (prototype.param_count() != mt.params.count)
            return false;

        if (prototype.return_type != analyzer.get_type(mt.type))
            return false;

        if (prototype.param_count() > 0)
        {
            auto p = prototype.arg_types.begin();
            typedef rt_mt_t<__tidx_t::param> mt_param_t;

            bool r = rt_assembly->each_params(mt.params, [&](int, mt_param_t & param) {
                if (param.param_type != p->param_type)
                    return false;

                if (analyzer.get_type(param.type) != p->type)
                    return false;

                p++;
                return true;
            });

            if (!r)
                return false;
        }

        return true;
    }

    // Searches method.
    ref_t rt_general_type_t::search_method(analyzer_env_t & env,
                    method_prototype_t & prototype, search_method_options_t options)
    {
        bool only_virtual = bit_has_flag(options, search_method_options_t::only_virtual);
        ref_t ret = ref_t::null;

        typedef rt_mt_t<__tidx_t::method> mt_t;

        assembly_analyzer_t analyzer = __analyzer(env);
        assembly->each_methods(mt->methods, [&, this](int idx, mt_t & mt) {

            if (only_virtual && !__is_virtual((decorate_t)mt.decorate))
                return false;

            ref_t method_ref = ref_t(idx);
            rt_method_t * rt_method = analyzer.get_method(method_ref);
            _A(rt_method != nullptr);

            if (__compare_method(env, prototype, rt_method))
            {
                ret = method_ref;
                return false;
            }

            return true;
        });

        return ret;
    }

    // Gets data type.
    vtype_t rt_general_type_t::get_vtype(analyzer_env_t & env)
    {
        return (vtype_t)(*this)->vtype;
    }

    // Gets assembly.
    rt_assembly_t * rt_general_type_t::get_assembly()
    {
        return assembly;
    }

    // When caculate size.
    msize_t rt_general_type_t::on_caculate_size(analyzer_env_t & env,
                                                storage_type_t * out_storage_type)
    {
        _A((*this)->generic_params.count == 0);

        return __super_t::on_caculate_size(env, out_storage_type);
    }

    // When caculate layout.
    msize_t rt_general_type_t::on_caculate_layout(analyzer_env_t & env, msize_t base_size,
                                                storage_type_t * out_storage_type)
    {
        assembly_analyzer_t analyzer = __analyzer(env);
        type_layout_t<rt_field_base_t *> layout(analyzer, base_size);

        this->each_field(env, [&, this](ref_t, rt_field_base_t * field) {

            rt_type_t * field_type = field->get_field_type(analyzer);
            _A(field_type != nullptr);

            return layout.append(field, field_type), true;
        });

        layout.commit();

        al::assign(out_storage_type, layout.storage_type());
        return layout.type_size();
    }

    // Returns generic param count.
    size_t rt_general_type_t::generic_param_count()
    {
        return mt->generic_params.count;
    }

    // Returns if it's has uncertain types.
    bool rt_general_type_t::has_uncertain_types(analyzer_env_t & env)
    {
        size_t count = this->generic_param_count();
        if (count == 0)
            return false;

        ref_t gp_ref = mt->generic_params[count - 1];
        rt_generic_param_t * gp = __analyzer(env).get_generic_param(gp_ref);
        _A(gp != nullptr);

        return (*gp)->param_type == generic_param_type_t::params;
    }

    // Returns if it's match the specified param count.
    bool rt_general_type_t::is_match(assembly_analyzer_t & analyzer, size_t args_count)
    {
        if (!this->has_uncertain_types(analyzer.env))
            return this->generic_param_count() == args_count;

        return args_count >= this->generic_param_count() - 1;
    }

    // Returns whether it's a generic template.
    bool rt_general_type_t::is_generic_template()
    {
        if (generic_param_count() > 0)
            return true;

        if (host_type == nullptr)
            return false;

        return host_type->is_generic_template();
    }

    struct __rt_interface_vfunction_t
    {
        rt_method_t * method;

        uint32_t    assembly_index;
        uint32_t    method_index;

        operator rt_vtable_function_t() const
        {
            return rt_vtable_function_t(assembly_index, method_index);
        }
    };

    typedef al::svector_t<rt_vfunction_t, 32> __rt_vfunctions_t;
    typedef al::svector_t<__rt_interface_vfunction_t, 32> __rt_interface_vfunctions_t;
    typedef std::map<rt_type_t *, __rt_interface_vfunctions_t> __rt_interfaces_t;

    // Builds virtual functions of virtual table.
    static void __build_vfunctions(analyzer_env_t & env, rt_type_t * type,
                                                        __rt_vfunctions_t & vfuncs)
    {
        rt_type_t * base_type = type->get_base_type(env);
        if (base_type != nullptr)
            __build_vfunctions(env, base_type, vfuncs);

        ref_t ret = ref_t::null;

        typedef rt_mt_t<__tidx_t::method> mt_t;

        rt_assembly_t * assembly = type->get_assembly();
        assembly_analyzer_t analyzer(env, assembly);

        type->each_method(env, [&](ref_t method_ref, rt_method_t * rt_method) {

            decorate_t decorate = (decorate_t)(*rt_method)->decorate;

            if (__is_virtual(decorate))
            {
                vfuncs.push_back(rt_vtable_function_t(assembly->index, method_ref.index));
            }
            else if (__is_override(decorate))
            {
                method_prototype_t prototype;
                analyzer.get_prototype(method_ref, &prototype);

                bool found = false;
                for (rt_vtable_function_t & func : vfuncs)
                {
                    rt_assembly_t * rt_assembly0 = env.assemblies.at(func.assembly_idx);
                    rt_method_t * rt_method0 = rt_assembly0->get_method(ref_t(func.method_idx));
                    assembly_analyzer_t analyzer0(env, rt_assembly0);

                    if (__compare_method(env, prototype, rt_method0))
                    {
                        new (&func) rt_vtable_function_t(assembly->index, method_ref.index);

                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    auto method_name = assembly->get_name(rt_method);
                    throw __RtAssemblyErrorF("virtual method '%1%' not found", method_name);
                }
            }
            else
            {
                return false;   // All virtual methods should be in front of the method list.
            }

            return true;
        });
    }

    // Builds virtual functions for an interface method.
    static bool __build_interface(assembly_analyzer_t & analyzer, rt_type_t * type,
        __rt_interface_vfunctions_t & vfs, int index, rt_type_t * interface_type,
        method_prototype_t & prototype)
    {
        analyzer_env_t & env = analyzer.env;
        rt_method_t * matched_method = nullptr;

        rt_type_t * base_type = type->get_base_type(env);
        if (base_type != nullptr)
            __build_interface(analyzer, base_type, vfs, index, interface_type, prototype);

        if (index < vfs.size())
            matched_method = vfs[index].method;

        bool found_new = false;
        ref_t method_ref;

        type->each_method(env, [&](ref_t method_ref0, rt_method_t * method) {

            if (!__compare_method(env, prototype, method))
                return true;

            rt_type_t * owner_type = method->get_owner_type(env);

            if (owner_type != nullptr)
            {
                if (owner_type != interface_type)
                    return true;
            }

            matched_method = method;
            method_ref = method_ref0;
            found_new = true;

            return owner_type == nullptr;   // if owner_type is not nullptr, stop search.

        });

        if (matched_method == nullptr)
            return false;

        if (found_new)
        {
            _A(index <= vfs.size());

            uint32_t assembly_index = type->get_assembly()->index;
            uint32_t method_index = method_ref.index;

            #define __RtInterfaceVFunction() __rt_interface_vfunction_t {               \
                matched_method, assembly_index, method_index }

            if (index == vfs.size())
                vfs.push_back(__RtInterfaceVFunction());
            else
                vfs[index] = __RtInterfaceVFunction();

            #undef __RtInterfaceVFunction
        }

        return true;
    }

    // Builds virtual functions for interfaces.
    static void __build_interfaces(analyzer_env_t & env, rt_type_t * type,
                            __rt_interfaces_t & interfaces, rt_type_t * interface_type)
    {
        interface_type->each_super_type(env, [&](rt_type_t * super_type) {

            _A(super_type->get_ttype(env) == ttype_t::interface_);
            __build_interfaces(env, type, interfaces, super_type);

            return true;

        });

        __rt_interface_vfunctions_t & vfs = al::map_get(interfaces, interface_type);

        assembly_analyzer_t analyzer(env, interface_type->get_assembly());
        int index = 0;

        interface_type->each_method(env, [&](ref_t method_ref, rt_method_t * method) {

            method_prototype_t prototype;
            analyzer.get_prototype(method_ref, &prototype);

            if (!__build_interface(analyzer, type, vfs, index++, interface_type, prototype))
            {
                throw __RtAssemblyErrorF("cannot find member '%1%.%2%' in type '%3%'",
                    interface_type->get_name(env), method->get_name(), type->get_name(env)
                );
            }

            return true;

        });
    }

    // Builds virtual functions for interfaces.
    static void __build_interfaces(analyzer_env_t & env, rt_type_t * type,
                                                __rt_interfaces_t & interfaces)
    {
        rt_type_t * base_type = type->get_base_type(env);

        if (base_type != nullptr)
            __build_interfaces(env, base_type, interfaces);

        type->each_super_type(env, [&](rt_type_t * super_type) {

            if (super_type->get_ttype(env) == ttype_t::interface_)
                __build_interfaces(env, type, interfaces, super_type);

            return true;
        });
    }

    // Builds virtual table.
    void __build_vtbl(analyzer_env_t & env, rt_type_t * type)
    {
        __rt_vfunctions_t vfuncs;
        rt::__build_vfunctions(env, type, vfuncs);

        __rt_interfaces_t interfaces;
        rt::__build_interfaces(env, type, interfaces);

        // No virtual function & interfaces.
        if (vfuncs.empty() && interfaces.empty())
        {
            set_vtable(type, __EmptyVTbl);
            return;
        }

        // Builds virtual table.
        size_t interface_vfunction_count = 0;
        for (auto && it : interfaces)
        {
            __rt_interface_vfunctions_t & funcs = it.second;
            interface_vfunction_count += funcs.size();
        }

        // Alloc vtable object.
        size_t vfunction_arr_pos = sizeof(rt_vtable_t);
        size_t vtbl_interfaces_pos = vfunction_arr_pos + vfuncs.size() * sizeof(rt_vfunction_t);
        size_t vtbl_interface_arr_pos = vtbl_interfaces_pos + sizeof(rt_vtable_interfaces_t);
        size_t interface_function_arr_pos = vtbl_interface_arr_pos
                                        + interfaces.size() * sizeof(rt_vtable_interface_t);
        size_t size = interface_function_arr_pos + interface_vfunction_count
                                        * sizeof(rt_vfunction_t);
        rt_vtable_t * vtable = memory_t::new_obj<rt_vtable_t>(size, env.get_memory());
        byte_t * ptr0 = (byte_t *)vtable;

        // Copy vfunctions.
        al::copy(vfuncs, vtable->functions);

        // Copy interfaces.
        {
            vtable->interface_count = interfaces.size();
            vtable->interfaces = (rt_vtable_interfaces_t *)(ptr0 + vtbl_interfaces_pos);

            rt_vtable_interfaces_t * interfaces_ptr = vtable->interfaces;
            interfaces_ptr->recently = interfaces_ptr->interfaces;

            rt_vtable_interface_t * interface_arr = (rt_vtable_interface_t *)
                                                        (ptr0 + vtbl_interface_arr_pos);
            rt_vtable_interface_t * interface_arr0 = interface_arr;
            rt_vfunction_t * function_arr = (rt_vfunction_t *)(ptr0 + interface_function_arr_pos);

            for (auto && it : interfaces)
            {
                interface_arr->interface_type = it.first;
                interface_arr->functions = function_arr;

                al::copy(it.second, function_arr);

                function_arr += it.second.size();
                interface_arr++;
            }

            // Sort by interface type for faster lookup.
            std::sort(interface_arr0, interface_arr,
                [](auto & x, auto & y) { return x.interface_type < y.interface_type; }
            );
        }

        set_vtable(type, vtable);
    }

    // Builds virtual table.
    void rt_general_type_t::__build_vtbl(analyzer_env_t & env)
    {
        rt::__build_vtbl(env, this);
    }

    ////////// ////////// ////////// ////////// //////////
    // rt_generic_type_t

    // Constructor.
    rt_generic_type_t::rt_generic_type_t(rt_general_type_t * template_,
                rt_type_t * host_type, rt_type_t ** atypes, int atype_count)
        : template_(template_), host_type(host_type), atypes(atypes), __atype_count(atype_count)
    {
        _A(template_ != nullptr);
    }

    // Pre new object.
    void rt_generic_type_t::pre_new(analyzer_env_t & env)
    {
        if (__size != unknown_msize)
            return;

        assembly_analyzer_t analyzer = __analyzer(env);
        generic_param_manager_t gp_mgr(analyzer, this);

        rt_type_t * base_type = get_base_type(env, &gp_mgr);
        if (base_type != nullptr)
            base_type->pre_new(env);

        // size
        msize_t size = this->get_size(env);

        // build_vtbl;
        this->__build_vtbl(env);

        // execute static constructor
        this->pre_static_call(env);
    }

    // Pre calling static method.
    void rt_generic_type_t::pre_static_call(analyzer_env_t & env)
    {
        // Do nothing.
    }

    // Builds virtual table.
    void rt_generic_type_t::__build_vtbl(analyzer_env_t & env)
    {
        rt::__build_vtbl(env, this);
    }

    // It's a tuple type?
    bool rt_generic_type_t::__is_tuple()
    {
        return  (mtype_t)(*template_)->mtype == mtype_t::tuple;
    }

    // Returns tuple fields.
    rt_tuple_field_t * rt_generic_type_t::__get_tuple_fields(analyzer_env_t & env)
    {
        _A(__is_tuple());

        if (__tuple_fields == nullptr)
        {
            int atype_count = this->atype_count();
            __tuple_fields = env.rpool.new_tuple_field_array(atype_count);

            for  (int k = 0, count = atype_count; k < count; k++)
            {
                rt_type_t * field_type = atypes[k];
                rt_tuple_field_t & field = __tuple_fields[k];

                new ((void *)&field) rt_tuple_field_t();

                field.position = k;
                field.type = field_type;
            }
        }

        return __tuple_fields;
    }

    // Gets ttype.
    ttype_t rt_generic_type_t::get_ttype(analyzer_env_t & env)
    {
        _A(template_ != nullptr);

        return template_->get_ttype(env);
    }

    // Gets data type.
    vtype_t rt_generic_type_t::get_vtype(analyzer_env_t & env)
    {
        ttype_t ttype = get_ttype(env);

        if (is_ref_type(get_ttype(env)) || ttype == ttype_t::struct_)
            return vtype_t::mobject_;

        return vtype_t::ptr_;
    }

    // Enums all methods.
    void rt_generic_type_t::each_method(analyzer_env_t & env, each_method_t f)
    {
        _A(template_ != nullptr);

        template_->each_method(env, f);
    }

    // Gets name.
    rt_sid_t rt_generic_type_t::get_name(analyzer_env_t & env)
    {
        assembly_analyzer_t analyzer = __analyzer(env);

        stringstream_t ss;

        if (host_type != nullptr)
            ss << host_type->get_name(env).c_str() << _T(".");

        if (template_ == nullptr)
            ss << _T("?");
        else
            ss << analyzer.to_sid(((*template_)->name)).c_str();

        ss << _T("<");

        if (template_ == nullptr)
        {
            ss << _T("??");
        }
        else
        {
            for (size_t k = 0, count = atype_count(); k < count; k++)
            {
                if (k > 0)
                    ss << _T(",");

                rt_type_t * atype = atypes[k];
                const char_t * s;

                if (atype == nullptr || (s = atype->get_name(env).c_str()) == nullptr)
                    ss << _T("?");
                else
                    ss << s;
            }
        }

        ss << _T(">");

        return analyzer.to_sid(ss.str());
    }

    // Gets base type.
    rt_type_t * rt_generic_type_t::get_base_type(analyzer_env_t & env,
                                                 const generic_param_manager_t * gp_manager)
    {
        _A(template_ != nullptr);

        assembly_analyzer_t analyzer = __analyzer(env);
        generic_param_manager_t gp_mgr(analyzer, this);

        rt_type_t * base_type = template_->get_base_type(env, &gp_mgr);
        if (base_type == nullptr)
            return nullptr;

        if (base_type->get_kind() != rt_type_kind_t::general)
            return base_type;

        rt_general_type_t * general_type = (rt_general_type_t *)base_type;
        if (!general_type->is_generic_template())
            return general_type;

        X_UNEXPECTED();
    }

    // Enum all super types.
    void rt_generic_type_t::each_super_type(analyzer_env_t & env,
                    each_super_type_callback_t callback, const __gp_mgr_t * gp_manager)
    {
        _A(template_ != nullptr);

        assembly_analyzer_t analyzer = __analyzer(env);
        generic_param_manager_t gp_mgr(analyzer, this);

        template_->each_super_type(env, callback, &gp_mgr);
    }

    // Gets method offset.
    int rt_generic_type_t::get_method_offset(analyzer_env_t & env, ref_t method_ref)
    {
        return 0;
    }

    // Ensure field offset initialzed.
    void rt_generic_type_t::__ensure_field_offset_initialized(analyzer_env_t & env)
    {
        if (__field_offsets == nullptr)
            this->get_size(env);

        _A(__field_offsets != nullptr);
    }

    // Gets field offset.
    msize_t rt_generic_type_t::get_field_offset(analyzer_env_t & env, ref_t ref)
    {
        __ensure_field_offset_initialized(env);

        _A(template_ != nullptr);
        _A(!__is_tuple());

        int position = ref - (*template_)->fields;
        return __field_offsets[position];
    }

    // Gets field offset.
    msize_t rt_generic_type_t::get_field_offset(analyzer_env_t & env, int position)
    {
        __ensure_field_offset_initialized(env);

        _A(__is_tuple());
        _A(position < atype_count());

        return __field_offsets[position];
    }

    // Enums all fields.
    void rt_generic_type_t::each_field(analyzer_env_t & env, each_field_t f)
    {
        _A(template_ != nullptr);

        if (__is_tuple())
        {
            rt_tuple_field_t * fields = __get_tuple_fields(env);
            for  (int k = 0, count = atype_count(); k < count; k++)
            {
                f(ref_t::null, fields + k);
            }
        }
        else
        {
            template_->each_field(env, [&f, this](ref_t ref, rt_field_base_t * rt_field) {
                return f(ref, rt_field);
            });
        }
    }

    // Searches method.
    ref_t rt_generic_type_t::search_method(analyzer_env_t & env,
                    method_prototype_t & prototype, search_method_options_t options)
    {
        return ref_t::null;
    }

    // Enumerates all extend types.
    void rt_generic_type_t::each_extend_types(analyzer_env_t & env, rt_sid_t name, each_type_t f)
    {
        // Empty.
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
        return __super_t::on_caculate_size(env, out_storage_type);
    }

    // Finds type by generic param name.
    rt_type_t * __find_type_by_generic_param_name(assembly_analyzer_t & analyzer,
                                                  rt_generic_type_t * host_type, rt_sid_t name)
    {
        rt_general_type_t * template_ = host_type->template_;
        _A(template_ != nullptr);

        ref_t params = (*template_)->generic_params;
        for (ref_t gp_ref : params)
        {
            rt_generic_param_t * gp = analyzer.get_generic_param(gp_ref);
            _A(gp != nullptr);

            if (analyzer.to_sid((*gp)->name) == name)
            {
                int index = gp_ref - params;
                _A(index < host_type->atype_count());

                return host_type->atypes[index];
            }
        }

        rt_type_t * parent_type = host_type->host_type;
        if (parent_type == nullptr || parent_type->get_kind() != rt_type_kind_t::generic)
            return nullptr;

        return __find_type_by_generic_param_name(analyzer, (rt_generic_type_t *)parent_type, name);
    }

    // Fetch type by ref.
    rt_type_t * __fetch_type(assembly_analyzer_t & analyzer, rt_generic_type_t * this_, ref_t type)
    {
        if ((mt_type_extra_t)type.extra == mt_type_extra_t::generic_param)
        {
            _A(this_ != nullptr);

            rt_generic_param_t * gp = analyzer.get_generic_param(type);
            _A(gp != nullptr);

            rt_sid_t name = analyzer.to_sid((*gp)->name);

            rt_type_t * param_type = __find_type_by_generic_param_name(
                analyzer, this_, name
            );

            _A(param_type != nullptr);
            return param_type;
        }

        return analyzer.get_type(type);
    }

    // A generic field wrapper.
    struct __generic_field_wrapper_t
    {
        msize_t * p_offset;

        void set_offset(msize_t offset)
        {
            *p_offset = offset;
        }

        bool operator == (std::nullptr_t) const
        {
            return p_offset == nullptr;
        }

        bool operator != (std::nullptr_t) const
        {
            return p_offset != nullptr;
        }

        __generic_field_wrapper_t * operator -> () { return this; }
    };

    // Wraps a offset pointer.
    __generic_field_wrapper_t __wrap_generic_field(msize_t * p_offset)
    {
        return __generic_field_wrapper_t { p_offset };
    }

    // When caculate layout.
    msize_t rt_generic_type_t::on_caculate_layout(analyzer_env_t & env, msize_t base_size,
                                                storage_type_t * out_storage_type)
    {
        _A(__field_offsets == nullptr);
        _A(template_ != nullptr);

        size_t field_count;
        if (__is_tuple())
        {
            field_count = atype_count();
        }
        else
        {
            ref_t field_refs = (*template_)->fields;
            field_count = field_refs.count;
        }

        if (field_count == 0)
        {
            al::assign(out_storage_type, storage_type_t::value);
            return base_size;
        }

        msize_t * msize_array = env.rpool.new_msize_array(field_count);
        msize_t * p = msize_array;

        assembly_analyzer_t analyzer0 = __analyzer(env);
        generic_param_manager_t gp_mgr(analyzer0, this);
        assembly_analyzer_t analyzer = __analyzer(env, &gp_mgr);

        type_layout_t<__generic_field_wrapper_t> layout(analyzer, base_size);

        this->each_field(env, [&, this](ref_t, rt_field_base_t * field) {

            rt_type_t * field_type = field->get_field_type(analyzer, this);
            _A(field_type != nullptr);

            return layout.append(__wrap_generic_field(p++), field_type), true;
        });

        layout.commit();

        __field_offsets = msize_array;
        al::assign(out_storage_type, layout.storage_type());

        return layout.type_size();
    }

    ////////// ////////// ////////// ////////// //////////
    // rt_array_type_t

    // Constructor.
    rt_array_type_t::rt_array_type_t(rt_type_t * element_type, dimension_t dimension)
        : element_type(element_type), dimension(dimension)
    {
        _A(element_type != nullptr);
        _A(dimension >= 1);
    }

    // Returns name of array type.
    rt_sid_t rt_array_type_t::get_name(analyzer_env_t & env)
    {
        assembly_analyzer_t analyzer = __analyzer(env);
        rt_type_t * type = this->element_type;

        stringstream_t ss;
        if (type == nullptr)
        {
            ss << _T("?");
        }
        else
        {
            string_t name = type->get_name(env);
            ss << name.c_str();
        }

        ss << _T("[");

        for (int k = 1, dimension = this->dimension; k < dimension; k++)
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
        _A(element_type != nullptr);

        element_type->pre_new(env);
    }

    // Gets ttype.
    ttype_t rt_array_type_t::get_ttype(analyzer_env_t & env)
    {
        return ttype_t::class_;
    }

    // Gets base type.
    rt_type_t * rt_array_type_t::get_base_type(analyzer_env_t & env,
                                               const generic_param_manager_t * gp_manager)
    {
        rt_general_type_t * tarray_type = env.get_tarray_type();

        // TODO: reutrn ?
        X_UNEXPECTED();

        return nullptr;
    }

    // Enum all super types.
    void rt_array_type_t::each_super_type(analyzer_env_t & env,
                    each_super_type_callback_t callback, const __gp_mgr_t * gp_manager)
    {
        // TODO: return ?

        X_UNEXPECTED();
    }

    // Gets offset of method.
    int rt_array_type_t::get_method_offset(analyzer_env_t & env, ref_t method_ref)
    {
        return 0;
    }

    // Gets field offset.
    msize_t rt_array_type_t::get_field_offset(analyzer_env_t & env, ref_t ref)
    {
        X_UNEXPECTED();
    }

    // Gets field offset.
    msize_t rt_array_type_t::get_field_offset(analyzer_env_t & env, int position)
    {
        X_UNEXPECTED();
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

    // When caculate layout.
    msize_t rt_array_type_t::on_caculate_layout(analyzer_env_t & env, msize_t base_size,
                                                        storage_type_t * out_storage_type)
    {
        al::assign(out_storage_type, storage_type_t::__unknown__);
        return 0;
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

    // Gets owner type (an interface type).
    rt_type_t * rt_method_t::get_owner_type(analyzer_env_t & env)
    {
        ref_t owner_type_ref = (*this)->owner;
        if (owner_type_ref == ref_t::null)
            return nullptr;

        assembly_analyzer_t analyzer(env, get_assembly());
        return analyzer.get_type(owner_type_ref);;
    }

    // Gets name.
    rt_sid_t rt_method_t::get_name()
    {
        return get_assembly()->get_name(this);
    }

    // Returns generic param count.
    int rt_method_t::generic_param_count()
    {
        return (*this)->generic_params.count;
    }

    ////////// ////////// ////////// ////////// //////////
    // rt_generic_method_t

    // Constructor.
    rt_generic_method_t::rt_generic_method_t(rt_method_t * template_, rt_type_t * host_type,
                                             rt_type_t ** atypes)
        : template_(template_), host_type(host_type), atypes(atypes)
    {
        _A(template_ != nullptr);
        _A(host_type != nullptr);
    }

    // Returns host type.
    rt_type_t * rt_generic_method_t::get_host_type()
    {
        return host_type;
    }

    // Gets assembly.
    rt_assembly_t * rt_generic_method_t::get_assembly()
    {
        _A(template_ != nullptr);

        return template_->get_assembly();
    }

    // Gets name.
    rt_sid_t rt_generic_method_t::get_name()
    {
        _A(template_ != nullptr);

        return template_->get_name();
    }

    ////////// ////////// ////////// ////////// //////////

    rt_type_t * rt_field_t::get_field_type(assembly_analyzer_t & analyzer,
                                                rt_generic_type_t * owner)
    {
        ref_t type_ref = (*this)->type;
        _A((mt_type_extra_t)type_ref.extra != mt_type_extra_t::generic_param);

        rt_type_t * field_type = __fetch_type(analyzer, owner, type_ref);
        _A(field_type != nullptr);

        return field_type;
    }

    ////////// ////////// ////////// ////////// //////////

    // Joins assembly name.
    string_t join_assembly_name(const string_t & package, const string_t & name)
    {
        if (package.length() == 0)
            return name;

        return _F(_T("%s.%s"), package, name);
    }

    // Joins type name.
    string_t join_type_name(const string_t & ns, const string_t & name, int generic_param_count)
    {
        stringstream_t ss;

        if (!ns.empty())
            ss << ns.c_str() << _T(".");

        ss << name.c_str();

        if (generic_param_count > 0)
        {
            ss << _T("<");

            for (int k = 0; k < generic_param_count; k++)
            {
                ss << _T(",");
            }

            ss << _T(">");
        }

        return ss.str();
    }

    // Joins method name.
    string_t __join_method_name(const string_t & assembly,
        const string_t & type_name, int type_generic_param_count,
        const string_t & method_name, int generic_param_count,
        const string_t & return_type, int param_count, const string_t * params)
    {
        stringstream_t ss;

        if (!return_type.empty())
            ss << return_type << _T(" ");

        if (!assembly.empty())
            ss << assembly;

        ss << _T(".") << type_name;
        if (type_generic_param_count > 0)
            ss << _T("<") << string_t(type_generic_param_count, _T(',')) << _T(">");

        ss << _T(".") << method_name;

        if (generic_param_count > 0)
            ss << _T("<") << string_t(generic_param_count, _T(',')) << _T(">");

        ss << _T("(");

        if (param_count > 0)
            ss << al::join_str(params, params + param_count, _T(", "));

        ss << _T(")");

        return ss.str();
    }

    ////////// ////////// ////////// ////////// //////////
    // rt_assemblies

    // Loads main assembly.
    rt_assembly_t * rt_assemblies_t::load_main_assembly()
    {
        if (__main_assembly == nullptr)
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
        if (it != __assembly_map.end())
            return it->second;

        rt_assembly_t * assembly = __provider->load_assembly(__ctx, package, name);
        __revise_assembly(assembly);

        if (assembly == nullptr)
            throw _ED(__e_t::assembly_not_found, join_assembly_name(package, name));

        __assembly_map[key] = assembly;
        return assembly;
    }

    // Returns assembly at specified index.
    rt_assembly_t * rt_assemblies_t::at(int index)
    {
        if (index < 0 || index >= __assembly_vector.size())
            throw __RtAssemblyError(_T("index overflow when reading rt_assembly_t object"));

        return __assembly_vector[index];
    }

    // Revise assembly, Assigns it's index.
    void rt_assemblies_t::__revise_assembly(rt_assembly_t * assembly)
    {
        if (assembly->index < 0)
        {
            assembly->index = __assembly_vector.size();
            __assembly_vector.push_back(assembly);
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns core assembly.
    rt_assembly_t * analyzer_env_t::get_core_assembly()
    {
        if (__core_assembly == nullptr)
            __core_assembly = assemblies.load_assembly(_T(""), CoreAssembly);

        return __core_assembly;
    }

    // Returns core type of specified name.
    rt_general_type_t * analyzer_env_t::get_core_type(const string_t & ns, const string_t & name,
                                                      int generic_param_count)
    {
        rt_assembly_t * core_assembly = get_core_assembly();
        rt_general_type_t * type = core_assembly->get_type(ns, name, generic_param_count);

        if (type == nullptr)
        {
            throw __RtAssemblyErrorF("type '%1%' not found",
                    join_type_name(ns, name, generic_param_count)
            );
        }

        return type;
    }

    // Returns array type.
    rt_general_type_t * analyzer_env_t::get_tarray_type()
    {
        if (__t_array_type == nullptr)
            __t_array_type = get_core_type(_T(""), _T("Array"), 1);

        return __t_array_type;
    }

    // Returns tuple type.
    rt_general_type_t * analyzer_env_t::get_tuple_type()
    {
        if (__t_tuple_type == nullptr)
            __t_tuple_type = get_core_type(_T(""), _T("Tuple"), 1);

        return __t_tuple_type;
    }

    ////////// ////////// ////////// ////////// //////////
    // assembly_analyzer_t

    assembly_analyzer_t::assembly_analyzer_t(analyzer_env_t & env, rt_assembly_t * current,
            const generic_param_manager_t * gp_manager)
        : env(env), current(current)
        , gp_manager(gp_manager? gp_manager : generic_param_manager_t::empty_instance())
    { }

    // Gets host type by method ref.
    rt_type_t * assembly_analyzer_t::get_host_type_by_method_ref(ref_t method_ref,
                                                rt_method_ref_t ** out_method_ref)
    {
        switch ((mt_member_extra_t)method_ref.extra)
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
        switch ((mt_member_extra_t)field_ref.extra)
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

            case mt_member_extra_t::generic: {
                mt_generic_field_t * mt = current->mt_of_generic_field(field_ref);
                _A(!mt->host.empty());

                return this->get_type(mt->host);
            }

            case mt_member_extra_t::position: {
                mt_position_field_t * mt = current->mt_of_position_field(field_ref);
                _A(!mt->host.empty());

                return this->get_type(mt->host);
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

        if (ref_assembly == nullptr)
            throw _ED(__e_t::assembly_not_found, join_assembly_name(package, name));

        return ref_assembly;
    }

    // Gets type.
    rt_type_t * assembly_analyzer_t::get_type(ref_t type_ref)
    {
        if (type_ref == ref_t::null)
            return nullptr;

        switch ((mt_type_extra_t)type_ref.extra)
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
                rt_mt_t<__tidx_t::array_type> * mt = current->mt_of_array(type_ref);
                rt_type_t * element_type = get_type(mt->element_type);
                dimension_t dimension = mt->dimension;

                return to_array_type(element_type, dimension);
            }

            case mt_type_extra_t::generic: {
                rt_mt_t<__tidx_t::generic_type> * mt = current->mt_of_generic(type_ref);
                rt_general_type_t * template_ = _M(rt_general_type_t *, get_type(mt->template_));
                rt_type_t * host_type = get_type(mt->host);

                size_t atype_count = mt->args.count;
                rt_type_t * atypes[atype_count];

                _A(template_->is_match(*this, atype_count));

                rt_type_t ** ptype = (rt_type_t **)atypes;
                for (ref_t ga_ref : mt->args)
                {
                    rt_generic_argument_t * ga = current->get_generic_argument(ga_ref);
                    ref_t atype_ref = (*ga)->type;

                    rt_type_t * atype = get_type(atype_ref);
                    _A(atype != nullptr);

                    *ptype++ = atype;
                }

                return to_generic_type(template_, host_type, (rt_type_t **)atypes, atype_count);
            }

            case mt_type_extra_t::generic_param: {
                rt_generic_param_t * gp = get_generic_param(type_ref);
                _A(gp != nullptr);

                rt_sid_t name = to_sid((*gp)->name);
                return gp_manager->type_at(name);
            }

            case mt_type_extra_t::generic_param_index: {
                int index = type_ref.index;
                return gp_manager->type_at(index);
            }

            default:
                X_UNEXPECTED();
        }
    }

    // Gets array type.
    rt_array_type_t * assembly_analyzer_t::to_array_type(rt_type_t * element_type,
                                                         dimension_t dimension)
    {
        return env.rpool.to_array_type(env.memory, element_type, dimension);
    }

    // Gets generic type.
    rt_generic_type_t * assembly_analyzer_t::to_generic_type(rt_general_type_t * template_,
            rt_type_t * host_type, rt_type_t ** atypes, int atype_count)
    {
        return env.rpool.to_generic_type(env.memory, template_, host_type, atypes, atype_count);
    }

    // Converts template to generic type, used by gp_manager.
    rt_type_t * assembly_analyzer_t::to_generic_type(rt_general_type_t * template_)
    {
        _A(template_ != nullptr);

        return __to_generic(template_);
    }

    rt_type_t * assembly_analyzer_t::__to_generic(rt_general_type_t * template_)
    {
        if (template_ == nullptr)
            return nullptr;

        rt_type_t * host_type = __to_generic(template_->host_type);

        if ((host_type != nullptr && host_type->get_kind() == rt_type_kind_t::generic)
            || template_->generic_param_count() > 0)
        {
            al::svector_t<rt_type_t *> atypes;

            for (ref_t gp_ref : (*template_)->generic_params)
            {
                rt_generic_param_t * gp = get_generic_param(gp_ref);
                _A(gp != nullptr);

                rt_sid_t name = to_sid((*gp)->name);

                if ((generic_param_type_t)(*gp)->param_type == generic_param_type_t::params)
                {
                    int index, count;
                    if (gp_manager->types_of(name, &index, &count))
                    {
                        for (int end = index + count; index < end; index++)
                        {
                            rt_type_t * atype = gp_manager->type_at(index);
                            atypes.push_back(atype);

                            // _PP(atype->get_name(env));
                        }
                    }
                }
                else
                {
                    rt_type_t * atype = gp_manager->type_at(name);
                    _A(atype != nullptr);

                    atypes.push_back(atype);
                }
            }

            return to_generic_type(template_, host_type, std::begin(atypes), atypes.size());
        }

        return template_;
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
        return current->get_generic_method(method_ref, gp_manager);
    }

    // Gets method.
    rt_method_t * assembly_analyzer_t::__get_method(ref_t method_ref)
    {
        rt_method_ref_t * rt_method_ref;
        rt_type_t * rt_type = get_host_type_by_method_ref(method_ref, &rt_method_ref);
        _A(rt_type != nullptr);

        rt_assembly_t * rt_assembly = rt_type->get_assembly();
        rt_method_t * rt_method;

        if (__is_current(rt_assembly))
        {
            rt_method = rt_assembly->get_method(method_ref);
        }
        else
        {
            assembly_analyzer_t analyzer0(*this, rt_type->get_assembly());
            generic_param_manager_t gp_mgr(analyzer0, rt_type);
            assembly_analyzer_t analyzer(*this, &gp_mgr);

            mt_method_ref_t * mt = rt_method_ref->mt;
            ref_t params_ref = mt->params;
            int param_count = params_ref.count;
            rt_ptype_t params[param_count];

            ref_t param_ref = params_ref[0];
            for (rt_ptype_t * p = params, * p_end = p + param_count; p < p_end; p++)
            {
                rt_method_ref_param_t * param = current->get_method_ref_param(param_ref++);
                rt_type_t * type = analyzer.get_type(param->mt->type);
                *p = rt_ptype_t(type, param->mt->ptype);
            }

            rt_sid_t method_name = __to_sid(rt_method_ref->mt->name);
            rt_type_t * return_type = analyzer.get_type(mt->type);
            rt_method = nullptr;

            rt_type->each_method(env, [&, this](ref_t method_ref, rt_method_t * rt_method0) {
                rt_mt_t<__tidx_t::method> & mt = *rt_method0->mt;

                bool r = analyzer.__compare_method(rt_assembly, mt, method_name,
                    mt.generic_params.count, return_type, param_count, params
                );

                return r? rt_method = rt_assembly->get_method(method_ref), false : true;
            });

            if (rt_method == nullptr)
            {
                throw __RtAssemblyErrorF("method '%1%' not found",
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
        if (rt_assembly->to_sid(mt.name) != name)
            return false;

        if (generic_param_count != mt.generic_params.count)
            return false;

        if (param_count != mt.params.count)
            return false;

        __self_t analyzer = __bind(rt_assembly);

        if (return_type != analyzer.get_type(mt.type))
            return false;

        if (param_count > 0)
        {
            rt_ptype_t * p = params;
            typedef rt_mt_t<__tidx_t::param> mt_param_t;

            bool r = rt_assembly->each_params(mt.params, [&p, &analyzer](int, mt_param_t & param) {
                
                if (param.param_type != p->param_type)
                    return false;

                if (analyzer.get_type(param.type) != p->type)
                    return false;

                p++;
                return true;
            });

            if (!r)
                return false;
        }

        return true;
    }

    // Gets field.
    rt_field_base_t * assembly_analyzer_t::get_field(ref_t field_ref, rt_type_t ** out_type)
    {
        auto key = env.rpool.new_key<rpool_key_type_t::field>(current, field_ref);
        auto r = env.rpool.get(key, [this, field_ref]() {
            rt_type_t * type;
            rt_field_base_t * field = __get_field(field_ref, &type);
            return std::make_tuple(field, type);
        });

        al::assign_value(out_type, std::get<rt_type_t *>(r));
        return std::get<rt_field_base_t *>(r);
    }

    // Gets field.
    rt_field_base_t * assembly_analyzer_t::__get_field(ref_t field_ref, rt_type_t ** out_type)
    {
        _A((mt_member_extra_t)field_ref.extra != mt_member_extra_t::generic);

        rt_field_ref_t * rt_field_ref;
        rt_type_t * rt_type = get_host_type_by_field_ref(field_ref, &rt_field_ref);

        rt_assembly_t * rt_assembly = rt_type->get_assembly();

        al::assign_value(out_type, rt_type);

        rt_field_base_t * rt_field;
        if (__is_current(rt_assembly))
        {
            rt_field = rt_assembly->get_field(field_ref);
        }
        else if (__is_tuple_type(rt_type))
        {
            rt_field = nullptr;
        }
        else
        {
            mt_field_ref_t * mt = rt_field_ref->mt;
            rt_sid_t field_name = __to_sid(mt->name);

            rt_field = nullptr;
            rt_type->each_field(env, [&, this](ref_t field_ref, rt_field_base_t * field_base) {
                rt_field_t * field = (rt_field_t *)field_base;
                res_t name = (*field)->name;
                bool r = (field_name == rt_assembly->to_sid(name));
                return r? rt_field = rt_assembly->get_field(field_ref), false : true;
            });

            if (rt_field == nullptr)
            {
                throw __RtAssemblyErrorF("field '%1%' not found",
                    __join_field_name(rt_assembly, rt_type, mt)
                );
            }
        }

        return rt_field;
    }

    // Gets generic param.
    rt_generic_param_t * assembly_analyzer_t::get_generic_param(ref_t ref)
    {
        return current->get_generic_param(ref);
    }

    // Each extends types.
    void assembly_analyzer_t::each_extend_params(ref_t param_ref, each_extern_params_func_t f)
    {
        rt_param_t * rt_param = current->get_param(param_ref);
        _A((*rt_param)->param_type == param_type_t::extends);

        ref_t type_ref = (*rt_param)->type;
        rt_generic_param_t * gp = get_generic_param(type_ref);
        _A(gp != nullptr);

        rt_sid_t name = to_sid((*gp)->name);

        int index, count;
        if (gp_manager->types_of(name, &index, &count))
        {
            for (int end = index + count; index < end; index++)
            {
                rt_type_t * type = gp_manager->type_at(index);

                if (!f(type))
                    break;
            }
        }
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

        if (type == nullptr)
            return 0;

        return type->get_size(env, out_storage_type);
    }

    // Returns size of type.
    msize_t assembly_analyzer_t::size_of(rt_type_t * type, storage_type_t * out_storage_type)
    {
        if (type == nullptr)
            return 0;

        return type->get_size(env, out_storage_type);
    }

    // Gets size of variable.
    msize_t assembly_analyzer_t::variable_size_of(ref_t type_ref,
                                                        storage_type_t * out_storage_type)
    {
        rt_type_t * type = this->get_type(type_ref);
        if (type == nullptr)
            return 0;

        return type->get_variable_size(env, out_storage_type);
    }

    // Gets size of variable.
    msize_t assembly_analyzer_t::variable_size_of(rt_type_t * type,
                                                        storage_type_t * out_storage_type)
    {
        if (type == nullptr)
            return 0;

        return type->get_variable_size(env, out_storage_type);
    }

    // Gets base type.
    rt_type_t * assembly_analyzer_t::get_base_type(rt_type_t * type)
    {
        _A(type != nullptr);
        return type->get_base_type(env, gp_manager);
    }

    // Enum all super types.
    void assembly_analyzer_t::each_super_type(rt_type_t * type, analyzer_env_t & env,
                                                        each_super_type_callback_t callback)
    {
        _A(type != nullptr);
        type->each_super_type(env, callback, gp_manager);
    }

    // Gets type size.
    msize_t assembly_analyzer_t::get_type_size(ref_t type_ref)
    {
        rt_type_t * rt_type = get_type(type_ref);
        _A(rt_type != nullptr);

        return rt_type->get_size(env);
    }

    // Gets field offset.
    msize_t assembly_analyzer_t::get_field_offset(ref_t field_ref, rt_type_t * host_type)
    {
        _A(host_type != nullptr);

        switch ((mt_member_extra_t)field_ref.extra)
        {
            case mt_member_extra_t::internal:
                return host_type->get_field_offset(env, field_ref);

            case mt_member_extra_t::generic: {
                X_UNEXPECTED();
            }

            default:
                X_UNEXPECTED();
        }
    }

    // Gets virtual method offset.
    int assembly_analyzer_t::get_virtual_method_offset(ref_t method_ref)
    {
        rt_method_t * method = current->get_method(method_ref);
        _A(method != nullptr);

        rt_type_t * rt_type = method->get_host_type();

        // All methods in interface types are all virtual members.
        if (rt_type->get_ttype(env) == ttype_t::interface_)
            return rt_type->get_method_offset(env, method_ref);

        decorate_t decorate = (decorate_t)(*method)->decorate;
        if (__is_virtual(decorate))
        {
            int offset = rt_type->get_method_offset(env, method_ref);

            rt_type_t * base_type = rt_type->get_base_type(env);
            if (base_type != nullptr)
                offset += base_type->get_virtual_count(env);

            return offset;
        }

        if (decorate.is_override)
        {
            method_prototype_t prototype;
            get_prototype(method_ref, &prototype);

            do
            {
                rt_type = rt_type->get_base_type(env);
                if (rt_type == nullptr)
                {
                    throw __RtAssemblyErrorF("virtual method '%1%' not found",
                            current->get_name(method)
                    );
                }

                assembly_analyzer_t analyzer0(*this, rt_type->get_assembly());
                method_ref = rt_type->search_method(analyzer0.env, prototype);

            } while (method_ref == ref_t::null);

            return get_virtual_method_offset( method_ref);
        }

        throw __RtAssemblyErrorF("virtual method '%1%' not found", current->get_name(method));
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

        for (ref_t param_ref : mt->params)
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
    assembly_analyzer_t to_analyzer(analyzer_env_t & env, rt_type_t * rt_type,
                                    const generic_param_manager_t * gp_manager)
    {
        _A(rt_type != nullptr);

        return assembly_analyzer_t(env, rt_type->get_assembly(), gp_manager);
    }

    ////////// ////////// ////////// ////////// //////////

    // Builds virtual table.
    rt_vtable_t * build_vtable(memory_t * memory, assembly_analyzer_t & analyzer,
                                                  rt_type_t & rt_type)
    {
        return nullptr;
    }

    ////////// ////////// ////////// ////////// //////////

    // Creates a new metadata for  generic method.
    rt_generic_method_t * rt_mt_t<__tidx_t::generic_method>::new_entity(rt_context_t & ctx,
            ref_t ref, __mt_t * mt, rt_assembly_t * assembly, const __gp_mgr_t * gp_mgr)
    {
        analyzer_env_t env = to_env(ctx);
        assembly_analyzer_t analyzer(env, assembly, gp_mgr);

        rt_type_t * host_type;
        rt_method_t * template_;

        switch ((mt_member_extra_t)mt->template_.extra)
        {
            case mt_member_extra_t::internal: {
                template_ = analyzer.get_method(mt->template_);

                if (mt->host.empty())   // it's general host type.
                    host_type = assembly->get_host_by_method_ref(mt->template_);
                else                    // the generic host type, with generic arguments.
                    host_type = analyzer.get_type(mt->host);

            }   break;

            case mt_member_extra_t::generic: {
                rt_generic_method_t * g_template = assembly->get_generic_method(
                    mt->template_, gp_mgr
                );

                _A(g_template != nullptr);

                template_ = g_template->template_;
                host_type = g_template->host_type;

            }  break;

            case mt_member_extra_t::import: {
                template_ = analyzer.get_method(mt->template_);

                if (mt->host.empty())   // it's general host type.
                    host_type = template_->get_host_type();
                else                    // the generic host type, with generic arguments.
                    host_type = analyzer.get_type(mt->host);

            }  break;

            default:
                X_UNEXPECTED();
        }

        _A(template_ != nullptr);
        _A(host_type != nullptr);

        int atype_count = mt->args.count;
        type_t * atypes[atype_count];

        rt_type_t ** ptype = (rt_type_t **)atypes;
        for (ref_t ga_ref : mt->args)
        {
            rt_generic_argument_t * generic_argument = assembly->get_generic_argument(ga_ref);
            ref_t atype_ref = (*generic_argument)->type;
            rt_type_t * atype = analyzer.get_type(atype_ref);
            _A(atype != nullptr);

            *ptype++ = atype;
        }

        __rt_generic_args_key_t args_key((rt_type_t **)atypes, (rt_type_t **)atypes + atype_count);
        auto key = ctx.rpool.new_key<rpool_key_type_t::generic_method>(
            template_, host_type, args_key
        );

        return ctx.rpool.get(key, [&]() {

            key = ctx.rpool.revise_key(key, (rt_type_t **)atypes, atype_count);
            return memory_t::new_obj<rt_generic_method_t>(
                ctx.memory, template_, host_type, key.types()
            );

        });
    }

    //-------- ---------- ---------- ---------- ----------

    // Creates a new metadata for  general method.
    rt_method_t * rt_mt_t<__tidx_t::method>::new_entity(rt_context_t & ctx,
                                ref_t ref, __mt_t * mt, rt_assembly_t * assembly)
    {
        rt_method_t * method = memory_t::new_obj<rt_method_t>(ctx.memory);

        analyzer_env_t env = to_env(ctx);
        assembly_analyzer_t analyzer(env, assembly);

        method->mt = mt;
        method->host_type = analyzer.get_host_type_by_method_ref(ref);

        _A(method->host_type != nullptr);

        return method;
    }

    ////////// ////////// ////////// ////////// //////////

    #undef __EmptyVTbl

}   // namespace X_ROOT_NS::modules::rt


