#include <rt.h>
#include "utils.h"

namespace X_ROOT_NS::modules::rt {

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

        X_C(dynamic,    _T("dynamic"))

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

    // Returns whether a method is virtual.
    static bool __is_virtual(rt_method_t * method)
    {
        _A(method != nullptr);
        return __is_virtual((decorate_t)(*method)->decorate);
    }

    // Returns whether a method is override.
    static bool __is_override(decorate_t decorate)
    {
        return decorate.is_override;
    }

    // Returns whether a method is override.
    static bool __is_override(rt_method_t * method)
    {
        _A(method != nullptr);
        return __is_override((decorate_t)(*method)->decorate);
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
        for (const rt_char_t * s0 = s, * s0_end = s0 + min_length, * s1 = str.s;
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
        auto it = __string_set.find(s);
        if (it != __string_set.end())
            return rt_sid_t(&(*it));

        // The characters are copied: the strings of the pool have a head.
        return to_sid(ustring_t(s.s, s.length));
    }

    // Converts string to sid.
    rt_sid_t rt_spool_t::to_sid(const string_t & s)
    {
        return to_sid(to_ustring(s));
    }

    // Converts string to sid.
    rt_sid_t rt_spool_t::to_sid(const ustring_t & s)
    {
        rt_string_t str(s.c_str(), s.length());
        auto it = __string_set.find(str);
        if (it == __string_set.end())
        {
            // The head of the string (the length) is before the characters.
            rt_char_t * memory = __heap.acquire(rt_string_bytes(s.length()) / sizeof(rt_char_t));
            rt_char_t * dst = rt_string_init(memory, s.length());
            std::char_traits<rt_char_t>::copy(dst, s.c_str(), s.length() + 1);
            it = __string_set.insert(rt_string_t(dst, s.length())).first;
        }

        return rt_sid_t(&(*it));
    }

    ////////// ////////// ////////// ////////// //////////
    // rt_pool_t

    rt_pool_t::rt_pool_t(memory_t * memory) : __memory(memory), __msize_heap(memory)
    {
        __tuple_fields_heap = __pool.new_obj<al::heap_t<rt_tuple_field_t[]>>();
        __dynamic_methods_heap = __pool.new_obj<al::heap_t<rt_dynamic_method_t[]>>();
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
            // The virtual table is stored just before the type, see set_vtable.
            struct __wrap_t
            {
                __wrap_t(rt_general_type_t * template_, rt_type_t * host_type,
                                            rt_type_t ** atypes, int atype_count)
                    : type(template_, host_type, atypes, atype_count) { }

                rt_vtable_t *       vtbl = nullptr;
                rt_generic_type_t   type;
            };

            return &memory_t::new_obj<__wrap_t>(memory,
                template_, host_type, key.types(), atype_count
            )->type;

        });
    }
    
    // Gets array type.
    rt_array_type_t * rt_pool_t::to_array_type(memory_t * memory, rt_type_t * element_type,
                                               dimension_t dimension)
    {
        _A(element_type != nullptr);

        auto key = new_key<rpool_key_type_t::array_type>(element_type, dimension);
        return get(key, [memory, element_type, dimension]() {

            // The virtual table is stored just before the type, see set_vtable.
            struct __wrap_t
            {
                __wrap_t(rt_type_t * element_type, dimension_t dimension)
                    : type(element_type, dimension) { }

                rt_vtable_t *   vtbl = nullptr;
                rt_array_type_t type;
            };

            return &memory_t::new_obj<__wrap_t>(memory, element_type, dimension)->type;
        });
    }

    // Gets generic method.
    rt_generic_method_t * rt_pool_t::to_generic_method(memory_t * memory,
                rt_method_t * template_, rt_type_t * host_type,
                rt_type_t ** atypes, int atype_count)
    {
        _A(template_ != nullptr);

        __rt_generic_args_key_t args_key(atypes, atypes + atype_count);
        auto key = new_key<rpool_key_type_t::generic_method>(template_, host_type, args_key);

        return get(key, [&]() {

            key = revise_key(key, (rt_type_t **)atypes, atype_count);
            return memory_t::new_obj<rt_generic_method_t>(memory,
                template_, host_type, key.types(), atype_count
            );

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

    // Creates a rt_dynamic_method_t array.
    rt_dynamic_method_t * rt_pool_t::new_dynamic_method_array(size_t count)
    {
        return __dynamic_methods_heap->acquire(count);
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
        // The virtual methods and the overrides are in front of the others; the overrides do not
        // have slots of their own.
        this->each_method(env, [&count](ref_t, rt_method_base_t * method) {
            if (method->is_virtual())
                return ++count, true;

            return method->is_override();
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

    // Returns whether it's a host type of specified type.
    bool is_host_type_template(rt_type_t * type, rt_type_t * host_type)
    {
        _A(type != nullptr);
        _A(host_type != nullptr);

        for (; type != nullptr; type = type->get_host_type())
        {
            switch (type->get_kind())
            {
                case rt_type_kind_t::generic:
                    if (host_type == (rt_type_t *)((generic_type_t *)type)->template_)
                        return true;
                    break;

                default:
                    if (host_type == type)
                        return true;
                    break;
            }
        }

        return false;
    }

    // Returns whether it's a place holder.
    // Returns the place holder of the index (layout.cpp).
    rt_type_t * __get_type_place_holder(int index);

    // Returns whether the type is the place holder of the types of a params generic parameter
    // (layout.cpp).
    bool is_pack_place_holder(rt_type_t * type);

    bool is_place_holder(rt_type_t * type)
    {
        _A(type != nullptr);

        return type->get_kind() == rt_type_kind_t::place_holder;
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
            al::assign_value(out_storage_type, storage_type_t::value);
            return get_vtype_size(vtype);
        }

        return unknown_msize;
    }

    // Returns size of enum.
    msize_t __size_of_enum(analyzer_env_t & env, rt_type_t * type,
                                                 storage_type_t * out_storage_type)
    {
        al::assign_value(out_storage_type, storage_type_t::value);

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
                // A value of an interface is a reference (the items of an array of them).
                al::assign_value(out_storage_type, storage_type_t::ref);
                msize = rt_ref_size;
                break;

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
                al::assign_value(out_storage_type, storage_type_t::ref);
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
        rt_vtable_t * vtbl = get_vtable(this);
        if (__size != unknown_msize && vtbl != nullptr)
            return;

        rt_type_t * base_type = get_base_type(env);
        if (base_type != nullptr)
            base_type->pre_new(env);

        // size
        if (__size == unknown_msize)
            this->get_size(env);

        // build_vtbl;
        if (vtbl == nullptr)
            this->__build_vtbl(env);

        // execute static constructor
        this->pre_static_call(env);
    }

    // Pre calling static method.
    void rt_general_type_t::pre_static_call(analyzer_env_t & env)
    {
        // Empty.
    }

    // Returns the full name of general type: the namespace and the host types, like
    // System.Collections.List<T>.
    rt_sid_t rt_general_type_t::get_full_name(analyzer_env_t & env)
    {
        string_t name = get_name(env);
        stringstream_t ss;

        if (host_type != nullptr)
        {
            // The name has the name of the host type before its own.
            size_t prefix = string_t(host_type->get_name(env)).length() + 1;
            ss << host_type->get_full_name(env) << _T(".") << name.substr(prefix);
        }
        else
        {
            string_t ns = assembly->to_sid(mt->namespace_);
            if (!ns.empty())
                ss << ns << _T(".");

            ss << name;
        }

        return __analyzer(env).to_sid(ss.str());
    }

    // Returns name of general type.
    rt_sid_t rt_general_type_t::get_name(analyzer_env_t & env)
    {
        ref_t generic_params = (*this)->generic_params;
        if (generic_params.count == 0 && host_type == nullptr)
            return assembly->to_sid(mt->name);

        stringstream_t ss;

        if (host_type != nullptr)
            ss << host_type->get_name(env) << _T(".");

        assembly_analyzer_t analyzer = __analyzer(env);
        if (generic_params.count == 0)
        {
            ss << assembly->to_sid(mt->name);
        }
        else
        {
            ss << assembly->to_sid(mt->name);
            ss << _T("<");

            for  (ref_t gp : generic_params)
            {
                if (gp.index > generic_params.index)
                    ss << _T(",");

                rt_generic_param_t * rt_gp = analyzer.get_generic_param(gp);
                _A(rt_gp != nullptr);

                ss << analyzer.to_sid((*rt_gp)->name);

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
        // The base type of an enum is System.Enum (its super type is the underlying type).
        if (get_ttype(env) == ttype_t::enum_)
            return this == env.get_enum_type()? nullptr : env.get_enum_type();

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
        int offset = method_ref - mt->methods;
        _A(offset >= 0);

        // The static methods of an interface are not virtual, they have no slot in the virtual
        // table: the offset is the index among the others.
        if (get_ttype(env) == ttype_t::interface_)
        {
            assembly_analyzer_t analyzer = __analyzer(env);

            int index = 0;
            for (int k = 0; k < offset; k++)
            {
                rt_method_base_t * method = analyzer.get_method(mt->methods[k]);
                if (!method->is_static())
                    index++;
            }

            return index;
        }

        // A virtual method takes the slot after those of the virtual methods that are before it:
        // an override takes the slot of the method that it overrides (it has none of its own).
        assembly_analyzer_t analyzer = __analyzer(env);

        int index = 0;
        for (int k = 0; k < offset; k++)
        {
            if (analyzer.get_method(mt->methods[k])->is_virtual())
                index++;
        }

        return index;
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
                 rt_method_base_t * rt_method, const generic_param_manager_t * gp_mgr = nullptr)
    {
        _A(rt_method != nullptr);

        return rt_method->compare_to(env, prototype, gp_mgr);
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
            rt_method_base_t * rt_method = analyzer.get_method(method_ref);
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

        al::assign_value(out_storage_type, layout.storage_type());
        return layout.type_size();
    }

    // Searches the interface of specified type, or an interface that can be used as it.
    rt_vtable_interface_t * rt_vtable_t::search_vtable_interface_variant(
                                    analyzer_env_t & env, rt_type_t * interface_type)
    {
        if (rt_vtable_interface_t * exact = try_search_vtable_interface(interface_type))
            return exact;

        if (interface_type->get_kind() != rt_type_kind_t::generic)
            return nullptr;

        rt_vtable_interface_t * item = interfaces->interfaces;
        for (msize_t index = 0; index < interface_count; index++, item++)
        {
            rt_type_t * type = item->interface_type;
            if (type->get_kind() == rt_type_kind_t::generic
                    && ((rt_generic_type_t *)type)->is_variant_of(env, interface_type))
            {
                return item;
            }
        }

        return nullptr;
    }

    // Searches function address of interface methods, with the variance.
    rt_vfunction_t rt_vtable_t::search_vtable_interface_function(
                            analyzer_env_t & env, rt_type_t * interface_type, int offset)
    {
        rt_vtable_interface_t * item = search_vtable_interface_variant(env, interface_type);
        _A(item != nullptr);

        return item->functions[offset];
    }

    // Returns whether a type can be used as another by the variance, the types are
    // references.
    static bool __is_ref_compatible(analyzer_env_t & env, rt_type_t * from, rt_type_t * to)
    {
        if (from == to)
            return true;

        ttype_t from_ttype = from->get_ttype(env), to_ttype = to->get_ttype(env);
        if ((from_ttype != ttype_t::class_ && from_ttype != ttype_t::interface_)
                || (to_ttype != ttype_t::class_ && to_ttype != ttype_t::interface_))
        {
            return false;
        }

        if (from->get_kind() == rt_type_kind_t::generic
                && ((rt_generic_type_t *)from)->is_variant_of(env, to))
        {
            return true;
        }

        bool found = false;
        from->each_super_type(env, [&](rt_type_t * super_type) {
            found = __is_ref_compatible(env, super_type, to);
            return !found;
        });

        return found;
    }

    // Returns whether a value of this generic type can be used as the other type.
    bool rt_generic_type_t::is_variant_of(analyzer_env_t & env, rt_type_t * other)
    {
        if (other == nullptr || other->get_kind() != rt_type_kind_t::generic)
            return false;

        rt_generic_type_t * other_generic = (rt_generic_type_t *)other;
        if (other_generic->template_ != this->template_
                || other_generic->atype_count() != this->atype_count())
        {
            return false;
        }

        assembly_analyzer_t analyzer(env, this->template_->get_assembly());

        bool variant = false;
        for (int index = 0, count = this->atype_count(); index < count; index++)
        {
            rt_type_t * from = this->atypes[index], * to = other_generic->atypes[index];
            if (from == to)
                continue;

            rt_generic_param_t * gp = analyzer.get_generic_param(
                this->template_->mt->generic_params[index]
            );
            _A(gp != nullptr);

            switch ((generic_param_type_t)(*gp)->param_type)
            {
                case generic_param_type_t::out:
                    if (!__is_ref_compatible(env, from, to))
                        return false;
                    break;

                case generic_param_type_t::in:
                    if (!__is_ref_compatible(env, to, from))
                        return false;
                    break;

                default:
                    return false;
            }

            variant = true;
        }

        return variant;
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
        rt_method_base_t * method;

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

    // Builds virtual functions of virtual table. The owners are the types that declare the
    // functions, one for each: the types of a function are those of the generic params of its
    // owner (Base<Int32> for the T of Base<T>), when it is compared to an override.
    static void __build_vfunctions(analyzer_env_t & env, rt_type_t * type,
                            __rt_vfunctions_t & vfuncs, al::svector_t<rt_type_t *, 32> & owners,
                            const generic_param_manager_t * gp_mgr)
    {
        rt_type_t * base_type = type->get_base_type(env);
        if (base_type != nullptr)
            __build_vfunctions(env, base_type, vfuncs, owners, gp_mgr);

        generic_param_manager_t type_gp(env, type);

        ref_t ret = ref_t::null;

        typedef rt_mt_t<__tidx_t::method> mt_t;

        rt_assembly_t * assembly = type->get_assembly();
        assembly_analyzer_t analyzer(env, assembly, gp_mgr);
        assembly_analyzer_t type_analyzer(env, assembly, &type_gp);

        type->each_method(env, [&](ref_t method_ref, rt_method_base_t * rt_method) {

            if (rt_method->is_virtual())
            {
                vfuncs.push_back(rt_vtable_function_t(assembly->index, method_ref.index));
                owners.push_back(type);
            }
            else if (rt_method->is_override())
            {
                method_prototype_t prototype;
                (type->get_kind() == rt_type_kind_t::generic? type_analyzer : analyzer)
                                                .get_prototype(method_ref, &prototype);

                bool found = false;
                for (size_t k = 0; k < vfuncs.size(); k++)
                {
                    rt_vtable_function_t & func = vfuncs[k];
                    rt_assembly_t * rt_assembly0 = env.assemblies.at(func.assembly_idx);
                    rt_method_t * rt_method0 = rt_assembly0->get_method(ref_t(func.method_idx));

                    generic_param_manager_t owner_gp(env, owners[k]);
                    if (__compare_method(env, prototype, rt_method0, &owner_gp))
                    {
                        new (&func) rt_vtable_function_t(assembly->index, method_ref.index);
                        owners[k] = type;

                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    auto method_name = rt_method->to_string(env, true);
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

    static rt_type_t * __owner_type_of(analyzer_env_t & env, rt_method_base_t * method,
                                       const generic_param_manager_t * gp_mgr);

    // Builds virtual functions for an interface method.
    static bool __build_interface(assembly_analyzer_t & analyzer, rt_type_t * type,
        __rt_interface_vfunctions_t & vfs, int index, rt_type_t * interface_type,
        method_prototype_t & prototype)
    {
        analyzer_env_t & env = analyzer.env;
        rt_method_base_t * matched_method = nullptr;

        rt_type_t * base_type = type->get_base_type(env);
        if (base_type != nullptr)
            __build_interface(analyzer, base_type, vfs, index, interface_type, prototype);

        if (index < vfs.size())
            matched_method = vfs[index].method;

        bool found_new = false;
        ref_t method_ref;

        // The types of the methods of the type are those of its own generic params, the manager of
        // the interface has other ones (W<T> : IEnumerator<List<T>>: the T of the interface is
        // List<Int32>, the one of W<Int32> is Int32).
        generic_param_manager_t type_gp_mgr(env, type);

        type->each_method(env, [&](ref_t method_ref0, rt_method_base_t * method) {

            if (!__compare_method(env, prototype, method, &type_gp_mgr))
                return true;

            rt_type_t * owner_type = __owner_type_of(env, method, &type_gp_mgr);

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

    // The type of the interface that a method implements explicitly (IReadOnlyCollection<T>.Count).
    // It has the generic params of the type of the method: W<Int32> has the interface of Int32.
    static rt_type_t * __owner_type_of(analyzer_env_t & env, rt_method_base_t * method,
                                       const generic_param_manager_t * gp_mgr)
    {
        if (method->this_type() != rt_member_type_t::general)
            return method->get_owner_type(env);

        rt_method_t * rt_method = (rt_method_t *)method;

        ref_t owner_type_ref = (*rt_method)->owner;
        if (owner_type_ref == ref_t::null)
            return nullptr;

        assembly_analyzer_t analyzer(env, rt_method->get_assembly(), gp_mgr);
        return analyzer.get_type(owner_type_ref);
    }

    // Builds virtual functions for an interface method.
    // Returns stop means stop build.
    static bool __build_interface_override(assembly_analyzer_t & analyzer, rt_type_t * type,
        __rt_interface_vfunctions_t & vfs, int index, rt_type_t * interface_type,
        method_prototype_t & prototype)
    {
        _A(index < vfs.size());

        analyzer_env_t & env = analyzer.env;
        bool stop_build = false;

        generic_param_manager_t type_gp_mgr(env, type);

        type->each_method(env, [&](ref_t ref, rt_method_base_t * method) {

            if (__owner_type_of(env, method, &type_gp_mgr) != nullptr)
                return true;

            if (!__compare_method(env, prototype, method, &type_gp_mgr))
                return true;

            if (method->is_override())
            {
                vfs[index] = __rt_interface_vfunction_t {
                    method, (uint32_t)type->get_assembly()->index, ref.index
                };
            }
            else
            {
                stop_build = true;
            }

            return false;
        });

        return stop_build;
    }

    // Builds virtual functions for interfaces.
    static void __build_interfaces(analyzer_env_t & env, rt_type_t * type,
            __rt_interfaces_t & interfaces, rt_type_t * interface_type,
            rt_type_t * raw_type, const generic_param_manager_t * gp_mgr)
    {
        interface_type->each_super_type(env, [&](rt_type_t * super_type) {

            _A(super_type->get_ttype(env) == ttype_t::interface_);
            __build_interfaces(env, type, interfaces, super_type, raw_type, gp_mgr);

            return true;

        });

        __rt_interface_vfunctions_t & vfs = al::map_get(interfaces, interface_type);

        generic_param_manager_t gp_mgr1(env, interface_type, gp_mgr);
        assembly_analyzer_t analyzer(env, interface_type->get_assembly(), &gp_mgr1);
        int index = 0;

        interface_type->each_method(env, [&](ref_t method_ref, rt_method_base_t * method) {

            if (method_ref == ref_t::null)
                return true;

            // The static methods of the interfaces are not virtual: they have no slot.
            if (method->is_static())
                return true;

            method_prototype_t prototype;
            analyzer.get_prototype(method_ref, &prototype);

            if (!__build_interface(analyzer, type, vfs, index, interface_type, prototype))
            {
                throw __RtAssemblyErrorF("cannot find member '%1%.%2%' in type '%3%'",
                    interface_type->get_name(env), method->get_name(), type->get_name(env)
                );
            }

            rt_method_base_t * method1 = vfs[index].method;
            if (method1->is_virtual() && method1->get_owner_type(env) == nullptr)
            {
                rt_type_t * type1 = raw_type;
                while (type1 != type)
                {
                    if (!__build_interface_override(analyzer, type1, vfs, index,
                                                            interface_type, prototype))
                        break;

                    type1 = type1->get_base_type(env);
                }
            }

            index++;
            return true;

        });
    }

    // Builds virtual functions for interfaces.
    static void __build_interfaces(analyzer_env_t & env, rt_type_t * type,
                    __rt_interfaces_t & interfaces, rt_type_t * raw_type,
                    const generic_param_manager_t * gp_mgr)
    {
        rt_type_t * base_type = type->get_base_type(env);

        if (base_type != nullptr)
            __build_interfaces(env, base_type, interfaces, raw_type, gp_mgr);

        type->each_super_type(env, [&](rt_type_t * super_type) {

            if (super_type->get_ttype(env) == ttype_t::interface_)
                __build_interfaces(env, type, interfaces, super_type, raw_type, gp_mgr);

            return true;
        });
    }

    // Builds virtual table.
    void __build_vtbl(analyzer_env_t & env, rt_type_t * type,
                                const generic_param_manager_t * gp_mgr = nullptr)
    {
        static rt_vtable_t * simple_vtbl = nullptr;

        __rt_vfunctions_t vfuncs;
        al::svector_t<rt_type_t *, 32> vfunc_owners;
        rt::__build_vfunctions(env, type, vfuncs, vfunc_owners, gp_mgr);

        __rt_interfaces_t interfaces;
        rt::__build_interfaces(env, type, interfaces, type, gp_mgr);

        // Builds virtual table.
        size_t interface_vfunction_count = 0;
        for (auto && it : interfaces)
        {
            __rt_interface_vfunctions_t & funcs = it.second;
            interface_vfunction_count += funcs.size();
        }

        // Builds base types.
        al::svector_t<rt_type_t *, 16> base_types;

        {
            rt_type_t * type1 = type;
            while ((type1 = type1->get_base_type(env)) != nullptr)
            {
                base_types.push_back(type1);
            }
        }

        #define __IsSimpleVtbl() (vfuncs.empty() && interfaces.empty() && base_types.size() == 1)

        if (simple_vtbl != nullptr && __IsSimpleVtbl())
        {
            _A(base_types[0]->get_name(env) == _T("System.Object"));
            set_vtable(type, simple_vtbl);
            return;
        }

        // Alloc vtable object.
        size_t vfunction_arr_pos = sizeof(rt_vtable_t);
        size_t base_types_pos = vfunction_arr_pos + vfuncs.size() * sizeof(rt_vfunction_t);
        size_t vtbl_interfaces_pos = base_types_pos + base_types.size() * sizeof(rt_type_t *);
        size_t vtbl_interface_arr_pos = vtbl_interfaces_pos + sizeof(rt_vtable_interfaces_t);
        size_t interface_function_arr_pos = vtbl_interface_arr_pos
                                        + interfaces.size() * sizeof(rt_vtable_interface_t);
        size_t size = interface_function_arr_pos + interface_vfunction_count
                                        * sizeof(rt_vfunction_t);
        rt_vtable_t * vtable = memory_t::new_obj<rt_vtable_t>(size, env.get_memory());
        byte_t * ptr0 = (byte_t *)vtable;

        // Copy vfunctions.
        al::copy(vfuncs, vtable->functions);

        // Copy base types.
        vtable->base_types = (rt_type_t **)(ptr0 + base_types_pos);
        vtable->base_type_count = base_types.size();
        al::copy(base_types, vtable->base_types);

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

        if (simple_vtbl != nullptr && __IsSimpleVtbl())
            simple_vtbl = vtable;
    }

    // Builds virtual table.
    void rt_general_type_t::__build_vtbl(analyzer_env_t & env)
    {
        rt::__build_vtbl(env, this);
    }

    ////////// ////////// ////////// ////////// //////////

    // Gets assembly.
    rt_assembly_t * rt_dynamic_method_t::get_assembly()
    {
        _A(host_type != nullptr);

        return host_type->get_assembly();
    }

    // Compares to prototype, returns true if equals.
    bool rt_dynamic_method_t::compare_to(analyzer_env_t & env, method_prototype_t & prototype,
                                        const generic_param_manager_t * gp_mgr)
    {
        if (prototype.name != this->name)
            return false;

        if (prototype.generic_param_count != 0)
            return false;

        if (prototype.return_type != this->return_type)
            return false;

        if (prototype.arg_types.size() != this->param_count)
            return false;

        if (this->param_count > 0)
        {
            if (this->params == nullptr)
                return false;

            for (int k = 0, count = this->param_count; k < count; k++)
            {
                rt_dynamic_param_t & param = this->params[k];
                rt_ptype_t ptype = prototype.arg_types[k];

                if (param.param_type != ptype.param_type)
                    return false;

                if (param.type != ptype.type)
                    return false;
            }
        }

        return true;
    }

    // Returns a description of this method.
    string_t rt_dynamic_method_t::to_string(analyzer_env_t & env, bool include_host_type)
    {
        stringstream_t ss;

        // Return type.
        if (return_type != nullptr)
            ss << (return_type->get_name(env) | _T("?")) << _T(" ");

        // Host type.
        if (include_host_type && host_type != nullptr)
            ss << (host_type->get_name(env) | _T("?")) << _T(".");

        // Name.
        ss << (name | _T("?"));

        // Parameters.
        ss << _T("(");
        if (params != nullptr)
        {
            for (rt_dynamic_param_t * param = params, * param_end = param + param_count;
                param < param_end; param++)
            {
                if (param > params)
                    ss << _T(", ");

                if (param->param_type != param_type_t::__default__)
                    ss << _str(param->param_type).c_str() << _T(" ");

                if (param->type != nullptr)
                    ss << param->type->get_name(env);
                else
                    ss << _T("?");

                ss << _T(" ") << (param->name | _T("?"));
            }
        }
        ss << _T(")");

        return ss.str();
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
        // The size is known when a struct is made in a variable, before any object of the type.
        rt_vtable_t * vtbl = get_vtable(this);
        if (__size != unknown_msize && vtbl != nullptr)
            return;

        assembly_analyzer_t analyzer = __analyzer(env);
        generic_param_manager_t gp_mgr(analyzer, this);

        rt_type_t * base_type = get_base_type(env, &gp_mgr);
        if (base_type != nullptr)
            base_type->pre_new(env);

        // size
        msize_t size = this->get_size(env);

        // build_vtbl;
        if (vtbl == nullptr)
            this->__build_vtbl(env, &gp_mgr);

        // execute static constructor
        this->pre_static_call(env);
    }

    // Pre calling static method.
    void rt_generic_type_t::pre_static_call(analyzer_env_t & env)
    {
        // Do nothing.
    }

    // Builds virtual table.
    void rt_generic_type_t::__build_vtbl(analyzer_env_t & env, generic_param_manager_t * gp_mgr)
    {
        rt::__build_vtbl(env, this, gp_mgr);
    }

    // It's a tuple type?
    bool rt_generic_type_t::__is_tuple()
    {
        return  (mtype_t)(*template_)->mtype == mtype_t::tuple;
    }

    // It's the interface of the tuples?
    bool rt_generic_type_t::__is_tuple_interface()
    {
        return  (mtype_t)(*template_)->mtype == mtype_t::tuple_interface;
    }

    // Returns the dynamic methods of the tuple that has the same types.
    rt_dynamic_method_t * rt_generic_type_t::__get_interface_dynamic_methods(analyzer_env_t & env)
    {
        _A(__is_tuple_interface());

        assembly_analyzer_t analyzer = __analyzer(env);
        rt_generic_type_t * tuple_type = analyzer.to_generic_type(
            env.get_tuple_type(), nullptr, atypes, atype_count()
        );

        _A(tuple_type != nullptr && tuple_type->__is_tuple());
        return tuple_type->__get_dynamic_methods(env);
    }

    // Returns tuple fields.
    rt_tuple_field_t * rt_generic_type_t::__get_tuple_fields(analyzer_env_t & env)
    {
        return __get_tuple_members(env)->fields;
    }

    // Returns tuple dynamic methods.
    rt_dynamic_method_t * rt_generic_type_t::__get_dynamic_methods(analyzer_env_t & env)
    {
        return __get_tuple_members(env)->methods;
    }

    // Initialize tuple members and return them.
    rt_generic_type_t::__tuple_members_t * rt_generic_type_t::__get_tuple_members(
                                                                analyzer_env_t & env)
    {
        _A(__is_tuple());

        if (__tuple_members != nullptr)
            return __tuple_members;

        __tuple_members = memory_t::new_obj<__tuple_members_t>(env.memory);

        int atype_count = this->atype_count();

        rt_tuple_field_t * tuple_fields = env.rpool.new_tuple_field_array(atype_count);
        rt_dynamic_method_t * dynamic_methods = env.rpool.new_dynamic_method_array(atype_count);
        rt_assembly_t * assembly = this->get_assembly();

        for (int position = 0; position < atype_count; position++)
        {
            rt_type_t * field_type = atypes[position];

            // Field.
            rt_tuple_field_t & field = tuple_fields[position];
            new ((void *)&field) rt_tuple_field_t();

            field.position = position;
            field.type = field_type;

            // Method.
            rt_dynamic_method_t & method = dynamic_methods[position];
            new ((void *)&method) rt_dynamic_method_t();

            method.host_type = this;
            method.name = assembly->to_sid(_FT("get_Item%1%", position + 1));
            method.return_type = field_type;

            rt_dynamic_method_body_builder_t builder;
            if (is_custom_struct(env, field_type))
            {
                // A struct is written at the address that is below the arguments.
                builder.append<rt_push_argument_command_t>(0);
                builder.append<rt_push_field_addr_command_t>(&field);
                builder.append<rt_push_argument_command_t>(XIL_CALLING_BOTTOM_IDENTITY);
                builder.append<rt_copy_command_t>(field_type);
            }
            else
            {
                builder.append<rt_push_argument_command_t>(0);
                builder.append<rt_push_field_command_t>(&field);
            }

            builder.append<rt_ret_command_t>();

            method.body = builder.to_method_body(env.memory);
        }

        __tuple_members->fields  = tuple_fields;
        __tuple_members->methods = dynamic_methods;

        return __tuple_members;
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
        {
            // A struct of the core that is a value (Ptr<T>) has the data type of its template.
            if (ttype == ttype_t::struct_)
            {
                vtype_t template_vtype = template_->get_vtype(env);
                if (template_vtype != vtype_t::mobject_)
                    return template_vtype;
            }

            return vtype_t::mobject_;
        }

        return vtype_t::ptr_;
    }

    // Enums all methods.
    void rt_generic_type_t::each_method(analyzer_env_t & env, each_method_t f)
    {
        _A(template_ != nullptr);

        if (__is_tuple())
        {
            rt_dynamic_method_t * methods = __get_dynamic_methods(env);
            for (int k = 0, count = atype_count(); k < count; k++)
            {
                f(ref_t::null, methods + k);
            }
        }

        // The items of a tuple are read through its interface by the methods of the tuple (the
        // interface of a tuple is always implemented by a tuple), that are not virtual.
        if (__is_tuple_interface())
        {
            rt_dynamic_method_t * methods = __get_interface_dynamic_methods(env);
            for (int k = 0, count = atype_count(); k < count; k++)
            {
                f(ref_t::null, methods + k);
            }
        }

        template_->each_method(env, f);
    }

    // Gets name.
    rt_sid_t rt_generic_type_t::get_name(analyzer_env_t & env)
    {
        assembly_analyzer_t analyzer = __analyzer(env);

        stringstream_t ss;

        if (host_type != nullptr)
            ss << host_type->get_name(env) << _T(".");

        if (template_ == nullptr)
            ss << _T("?");
        else
            ss << analyzer.to_sid(((*template_)->name));

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
                if (atype == nullptr || atype->get_name(env).empty())
                    ss << _T("?");
                else
                    ss << atype->get_name(env);
            }
        }

        ss << _T(">");

        return analyzer.to_sid(ss.str());
    }

    // Gets full name: with the namespace, like System.Collections.List<System.Int32>.
    rt_sid_t rt_generic_type_t::get_full_name(analyzer_env_t & env)
    {
        assembly_analyzer_t analyzer = __analyzer(env);

        stringstream_t ss;

        if (host_type != nullptr)
        {
            ss << host_type->get_full_name(env) << _T(".");
        }
        else if (template_ != nullptr)
        {
            string_t ns = template_->assembly->to_sid((*template_)->namespace_);
            if (!ns.empty())
                ss << ns << _T(".");
        }

        ss << (template_ == nullptr? string_t(_T("?")) : (string_t)analyzer.to_sid(((*template_)->name)));
        ss << _T("<");

        for (size_t k = 0, count = template_ == nullptr? 0 : atype_count(); k < count; k++)
        {
            if (k > 0)
                ss << _T(",");

            rt_type_t * atype = atypes[k];
            ss << (atype == nullptr? string_t(_T("?")) : (string_t)atype->get_full_name(env));
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
            for (int k = 0, count = atype_count(); k < count; k++)
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

    // Compares to prototype, returns true if equals.
    bool rt_generic_method_t::compare_to(analyzer_env_t & env, method_prototype_t & prototype,
                                        const generic_param_manager_t * gp_mgr)
    {
        _A(template_ != nullptr);

        if (gp_mgr == nullptr)
        {
            assembly_analyzer_t analyzer0(env, this->get_assembly());
            generic_param_manager_t gp_mgr(analyzer0, this);

            return template_->compare_to(env, prototype, &gp_mgr);
        }

        return template_->compare_to(env, prototype, gp_mgr);
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
        mt_type_extra_t extra = (mt_type_extra_t)type.extra;

        if (extra == mt_type_extra_t::generic_param)
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
        msize_t *           p_offset;
        rt_field_base_t *   rt_field;

        void set_offset(msize_t offset)
        {
            *p_offset = offset;
            rt_field->offset = offset;
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
    __generic_field_wrapper_t __wrap_generic_field(msize_t * p_offset, rt_field_base_t * rt_field)
    {
        return __generic_field_wrapper_t {
            .p_offset = p_offset,
            .rt_field = rt_field,
        };
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
            al::assign_value(out_storage_type, storage_type_t::value);
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

            return layout.append(__wrap_generic_field(p++, field), field_type), true;
        });

        layout.commit();

        __field_offsets = msize_array;
        al::assign_value(out_storage_type, layout.storage_type());

        return layout.type_size();
    }

    // Returns whether it's a tuple type.
    bool is_tuple_type(rt_type_t * type)
    {
        _A(type != nullptr);

        if (type->get_kind() != rt_type_kind_t::generic)
            return false;

        rt_generic_type_t * generic_type = (rt_generic_type_t *)type;
        return generic_type->is_tuple();
    }

    // Returns whether it's a runnable type, general type (except template) or generic type.
    bool is_runnable_type(rt_type_t * type)
    {
        if (type == nullptr)
            return false;

        rt_type_kind_t kind = type->get_kind();
        return kind == rt_type_kind_t::generic ||
            (kind == rt_type_kind_t::general && !((rt_general_type_t *)type)->is_generic_template());
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

    // Returns Array<T>.
    rt_generic_type_t * rt_array_type_t::as_generic(analyzer_env_t & env)
    {
        if (__generic == nullptr)
        {
            rt_type_t * atypes[1] = { element_type };
            __generic = env.rpool.to_generic_type(
                env.memory, env.get_tarray_type(), nullptr, atypes, 1
            );
        }

        return __generic;
    }

    // Pre new object.
    void rt_array_type_t::pre_new(analyzer_env_t & env)
    {
        this->pre_static_call(env);

        // The array is derived from Array<T>: it has its virtual table, with the base types
        // Array<T>, Array and Object, and the interfaces of Array<T> (IReadOnlyList<T>).
        if (get_vtable(this) == nullptr)
        {
            this->as_generic(env)->pre_new(env);
            rt::__build_vtbl(env, this);
        }
    }

    // Pre calling static method.
    void rt_array_type_t::pre_static_call(analyzer_env_t & env)
    {
        _A(element_type != nullptr);

        // The items of an interface are references: there is no object of the type itself.
        if (element_type->get_ttype(env) != ttype_t::interface_)
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
        return this->as_generic(env);
    }

    // Enum all super types.
    void rt_array_type_t::each_super_type(analyzer_env_t & env,
                    each_super_type_callback_t callback, const __gp_mgr_t * gp_manager)
    {
        // The base type: its own super types are the ones of it.
        callback(this->as_generic(env));
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
        al::assign_value(out_storage_type, storage_type_t::ref);
        return rt_ref_size;
    }

    // When caculate size.
    msize_t rt_array_type_t::on_caculate_size(analyzer_env_t & env,
                                                        storage_type_t * out_storage_type)
    {
        al::assign_value(out_storage_type, storage_type_t::ref);
        return rt_ref_size;
    }

    // When caculate layout.
    msize_t rt_array_type_t::on_caculate_layout(analyzer_env_t & env, msize_t base_size,
                                                        storage_type_t * out_storage_type)
    {
        al::assign_value(out_storage_type, storage_type_t::__unknown__);
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

    // Gets return type.
    rt_type_t * rt_method_t::get_return_type(analyzer_env_t & env)
    {
        ref_t ret_type_ref = (*this)->type;

        assembly_analyzer_t analyzer(env, get_assembly());
        return analyzer.get_type(ret_type_ref);
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

    // Returns whether it's a virtual method.
    bool rt_method_t::is_virtual()
    {
        return __is_virtual((decorate_t)(*this)->decorate);
    }

    // Returns whether it's a override method.
    bool rt_method_t::is_override()
    {
        return __is_override((decorate_t)(*this)->decorate);
    }

    // Returns whether it's a static method.
    bool rt_method_t::is_static()
    {
        return ((decorate_t)(*this)->decorate).is_static;
    }

    // Returns whether the return types are the same: when the types of the arguments of a generic
    // type are params (ITuple<TItems ...>), the prototype has a place holder instead of them, it
    // is the same type if the templates are the same.
    static bool __is_same_return_type(rt_type_t * prototype_type, rt_type_t * type)
    {
        if (prototype_type == type)
            return true;

        if (prototype_type == nullptr || type == nullptr
                || prototype_type->get_kind() != rt_type_kind_t::generic
                || type->get_kind() != rt_type_kind_t::generic)
            return false;

        rt_generic_type_t * g1 = (rt_generic_type_t *)prototype_type;
        rt_generic_type_t * g2 = (rt_generic_type_t *)type;

        if (g1->template_ != g2->template_)
            return false;


        // The types of a params generic parameter of a method are not known.
        if (g1->atype_count() != g2->atype_count())
            return true;

        for (int k = 0, count = g1->atype_count(); k < count; k++)
        {
            if (is_place_holder(g1->atypes[k]))
                return true;
        }

        return false;
    }

    // Returns whether the type of a param of the prototype is the type of the param of a method:
    // the types that are made of the place holders (the generic params of a generic type that is
    // not made of arguments) are those that the place holders can be, and the place holder of a
    // params generic parameter is all of its types.
    static bool __is_same_param_type(rt_type_t * prototype_type, rt_type_t * type)
    {
        if (prototype_type == type)
            return true;

        if (prototype_type == nullptr || type == nullptr
                || prototype_type->get_kind() != rt_type_kind_t::generic
                || type->get_kind() != rt_type_kind_t::generic)
            return false;

        rt_generic_type_t * g1 = (rt_generic_type_t *)prototype_type;
        rt_generic_type_t * g2 = (rt_generic_type_t *)type;

        if (g1->template_ != g2->template_)
            return false;

        int count1 = g1->atype_count(), count2 = g2->atype_count();

        // The types of the params generic parameter that are collapsed are the last ones.
        if (count1 != count2)
        {
            if (count2 == 0 || !is_pack_place_holder(g2->atypes[count2 - 1]) || count1 < count2 - 1)
                return false;

            count2 = count2 - 1;
        }

        for (int k = 0; k < count2; k++)
        {
            rt_type_t * t1 = g1->atypes[k], * t2 = g2->atypes[k];
            if (t1 != t2 && !is_place_holder(t1) && !is_place_holder(t2)
                    && !__is_same_param_type(t1, t2))
            {
                return false;
            }
        }

        return true;
    }

    // Compares to prototype, returns true if equals.
    bool rt_method_t::compare_to(analyzer_env_t & env, method_prototype_t & prototype,
                                        const generic_param_manager_t * gp_mgr)
    {
        rt_assembly_t * rt_assembly = get_assembly();
        mt_method_t & mt = *this->mt;

        if (this->get_name() != prototype.name)
            return false;

        if (prototype.generic_param_count != mt.generic_params.count)
            return false;

        if (prototype.param_count() != mt.params.count)
            return false;

        // The generic params of a method that is looked for are place holders in the prototype,
        // they are place holders in the types of this method too (T of IEnumerable<T> is the
        // place holder of its index).
        generic_param_manager_t method_gp_mgr;
        const generic_param_manager_t * types_gp_mgr = gp_mgr;

        if (mt.generic_params.count > 0)
        {
            assembly_analyzer_t analyzer0(env, rt_assembly, gp_mgr);

            method_gp_mgr.parent = gp_mgr;

            int index = 0;
            for (ref_t gp_ref : mt.generic_params)
            {
                rt_generic_param_t * gp = analyzer0.get_generic_param(gp_ref);
                rt_sid_t gp_name = analyzer0.to_sid((*gp)->name);
                method_gp_mgr.append(gp_name, __get_type_place_holder(index));

                // The types of a params generic parameter that is in a generic type
                // (Delegate<R, TArgs ...>) are its place holder.
                if ((*gp)->param_type == generic_param_type_t::params)
                    method_gp_mgr.append(gp_name, index, 1);

                index++;
            }

            types_gp_mgr = &method_gp_mgr;
        }

        assembly_analyzer_t analyzer(env, rt_assembly, types_gp_mgr);

        if (!__is_same_return_type(prototype.return_type, analyzer.get_type(mt.type)))
            return false;

        if (prototype.param_count() > 0)
        {
            rt_ptype_t * p = (rt_ptype_t *)prototype.arg_types;
            typedef rt_mt_t<__tidx_t::param> mt_param_t;

            bool r = true;
            rt_assembly->each_params(mt.params, [&](int, mt_param_t & param) {

                if (param.param_type != p->param_type)
                    return (r = false, false);

                rt_generic_param_t * gp;
                if (analyzer.is_generic_params(param.type, &gp))
                    return (r = is_place_holder(p->type), false);

                // A type that is made of a params generic parameter (MulticastDelegate<R, TArgs ...>)
                // is compared like the return type.
                if (!__is_same_param_type(p->type, analyzer.get_type(param.type)))
                {
                    return (r = false, false);
                }

                p++;
                return true;
            });

            if (!r)
                return false;
        }

        return true;
    }

    // Returns a description of this method.
    string_t rt_method_t::to_string(analyzer_env_t & env, bool include_host_type)
    {
        generic_param_manager_t gp_mgr(env, this);
        rt_assembly_t * assembly = get_assembly();
        assembly_analyzer_t analyzer(env, assembly, &gp_mgr);

        stringstream_t ss;

        // Return type.
        {
            rt_type_t * return_type = analyzer.get_type((*this)->type);
            if (return_type != nullptr)
            {
                ss << return_type->get_name(env) << _T(" ");
            }
        }

        // Host type.
        if (include_host_type)
        {
            rt_type_t * host_type = this->get_host_type();
            if (host_type != nullptr)
            {
                ss << host_type->get_name(env) << _T(".");
            }
        }

        // Method name.
        ss << get_name();

        // Generic params.
        if (generic_param_count() > 0)
        {
            ss << _T("<");

            int index = 0;
            for (ref_t ref : (*this)->generic_params)
            {
                rt_generic_param_t * gp = analyzer.get_generic_param(ref);
                _A(gp != nullptr);

                if (index++ > 0)
                    ss << _T(", ");

                ss << analyzer.to_sid((*gp)->name);
            }

            ss << _T(">");
        }

        // Params
        {
            ss << _T("(");

            int index = 0;
            for (ref_t ref : (*this)->params)
            {
                rt_param_t * rt_param = assembly->get_param(ref);
                _A(rt_param != nullptr);

                if (index++ > 0)
                    ss << _T(", ");

                // A pack of types (T ... items): the types are not known here.
                rt_generic_param_t * pack_gp;
                if (analyzer.is_generic_params((*rt_param)->type, &pack_gp))
                {
                    ss << _T("???");
                    continue;
                }

                rt_type_t * param_type = analyzer.get_type((*rt_param)->type);
                _A(param_type != nullptr);

                rt_sid_t name = param_type->get_name(env);
                ss << (name.empty() ? string_t(_T("?")) : (string_t)name) << _T(" ");
                ss << analyzer.to_sid((*rt_param)->name);
            }

            ss << _T(")");
        }

        return ss.str();
    }

    ////////// ////////// ////////// ////////// //////////
    // rt_generic_method_t

    // Constructor.
    rt_generic_method_t::rt_generic_method_t(rt_method_t * template_, rt_type_t * host_type,
                                             rt_type_t ** atypes, int atype_count)
        : template_(template_), host_type(host_type), atypes(atypes)
        , __atype_count(atype_count < 0? template_->generic_param_count() : atype_count)
    {
        _A(template_ != nullptr);
        _A(host_type != nullptr);

        // A params generic parameter takes any count of types, none too.
        _A(__atype_count + 1 >= template_->generic_param_count());
    }

    // Returns host type.
    rt_type_t * rt_generic_method_t::get_host_type()
    {
        return host_type;
    }

    // Gets owner type (an interface type).
    rt_type_t * rt_generic_method_t::get_owner_type(analyzer_env_t & env)
    {
        _A(template_ != nullptr);

        return template_->get_owner_type(env);
    }

    // Gets return type.
    rt_type_t * rt_generic_method_t::get_return_type(analyzer_env_t & env)
    {
        _A(template_ != nullptr);

        ref_t ret_type_ref = (*template_)->type;
        generic_param_manager_t gp(env, this);

        assembly_analyzer_t analyzer(env, get_assembly(), &gp);
        rt_type_t * type = analyzer.get_type(ret_type_ref);

        return type;
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

    // Returns whether it's a virtual method.
    bool rt_generic_method_t::is_virtual()
    {
        _A(template_ != nullptr);

        return template_->is_virtual();
    }

    // Returns whether it's a override method.
    bool rt_generic_method_t::is_override()
    {
        _A(template_ != nullptr);

        return template_->is_override();
    }

    // Returns whether it's a static method.
    bool rt_generic_method_t::is_static()
    {
        _A(template_ != nullptr);

        return template_->is_static();
    }

    // Returns a description of this method.
    string_t rt_generic_method_t::to_string(analyzer_env_t & env, bool include_host_type)
    {
        generic_param_manager_t gp_mgr(env, this);
        rt_assembly_t * assembly = get_assembly();
        assembly_analyzer_t analyzer(env, assembly, &gp_mgr);

        stringstream_t ss;
        _A(template_ != nullptr);

        // Return type.
        {
            rt_type_t * return_type = analyzer.get_type((*template_)->type);
            if (return_type != nullptr)
            {
                ss << return_type->get_name(env) << _T(" ");
            }
        }

        // Host type.
        rt_type_t * host_type = this->get_host_type();
        if (include_host_type)
        {
            if (host_type != nullptr)
            {
                ss << host_type->get_name(env) << _T(".");
            }
        }

        // Method name.
        ss << get_name();

        // Generic params.
        if (atypes != nullptr && atype_count() > 0)
        {
            ss << _T("<");

            int index = 0;
            for (rt_type_t * type : _range(atypes, atypes + atype_count()))
            {
                if (index++ > 0)
                    ss << _T(", ");

                ss << type->get_name(env);
            }

            ss << _T(">");
        }

        // Params.
        {
            ss << _T("(");

            int index = 0;
            for (ref_t ref : (*template_)->params)
            {
                rt_param_t * rt_param = assembly->get_param(ref);
                _A(rt_param != nullptr);

                if (index++ > 0)
                    ss << _T(", ");

                rt_generic_param_t * gp;
                if (analyzer.is_generic_params((*rt_param)->type, &gp))
                {
                    rt_sid_t name = analyzer.to_sid((*rt_param)->name);
                    if (__is_tuple_type(host_type))
                    {
                        int index = 0;
                        for (rt_type_t * type : _range(atypes, atypes + atype_count()))
                        {
                            ss << type->get_name(env) << _T(" item");
                        }
                    }
                    else
                    {
                        ss << _T("???");
                    }
                }
                else
                {
                    rt_type_t * param_type = analyzer.get_type((*rt_param)->type);
                    rt_sid_t name = param_type->get_name(env);
                    ss << name << _T(" ");
                    ss << analyzer.to_sid((*rt_param)->name);
                }
            }

            ss << _T(")");
        }

        return ss.str();
    }

    ////////// ////////// ////////// ////////// //////////

    rt_type_t * rt_field_t::get_field_type(assembly_analyzer_t & analyzer,
                                                rt_generic_type_t * owner)
    {
        ref_t type_ref = (*this)->type;
        return __fetch_type(analyzer, owner, type_ref);
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

        ss << name;

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

    // Finds a resource of the loaded assemblies.
    bool rt_assemblies_t::find_resource(const string_t & name, const byte_t ** out_bytes, size_t * out_length)
    {
        for (rt_assembly_t * assembly : __assembly_vector)
        {
            for (int index = 0, count = assembly->get_resource_count(); index < count; index++)
            {
                if (assembly->get_resource_name(index) == name)
                {
                    rt_bytes_t bytes = assembly->get_resource_bytes(index);
                    *out_bytes  = bytes.bytes;
                    *out_length = bytes.length;

                    return true;
                }
            }
        }

        return false;
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
    // method_prototype_t

    string_t rt_ptype_t::to_string(analyzer_env_t & env) const
    {
        stringstream_t ss;

        if (param_type != param_type_t::__default__)
            ss << _str(param_type).c_str() << _T(" ");

        ss << type->get_name(env);
        return ss.str();
    }

    string_t method_prototype_t::to_string(analyzer_env_t & env) const
    {
        stringstream_t ss;

        if (return_type != nullptr)
            ss << return_type->get_name(env) << _T(" ");

        ss << name;

        if (generic_param_count > 0)
        {
            ss << _T("<");
            for (int k = 0; k < generic_param_count - 1; k++)
            {
                ss << _T(",");
            }
            ss << _T(">");
        }

        ss << _T("(");

        {
            int index = 0;
            for (rt_ptype_t & ptype : arg_types)
            {
                if (index++ == 0)
                    ss << _T(", ");

                ss << _str(ptype).c_str();
            }
        }

        ss << _T(")");

        return ss.str();
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

    // Returns the type System.Enum.
    rt_general_type_t * analyzer_env_t::get_enum_type()
    {
        if (__enum_type == nullptr)
            __enum_type = get_core_type(_T(""), _T("Enum"), 0);

        return __enum_type;
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
                rt_generic_method_t * rt_generic_method = _M(rt_generic_method_t *,
                                                            get_method(method_ref));
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
            case mt_type_extra_t::general:
                return current->get_type(type_ref);

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
                rt_mt_t<__tidx_t::generic_type> * mt = current->mt_of_generic_type(type_ref);
                rt_general_type_t * template_ = _M(rt_general_type_t *, get_type(mt->template_));
                rt_type_t * host_type = get_type(mt->host);

                size_t atype_count = mt->args.count;
                _A(template_->is_match(*this, atype_count));

                al::svector_t<rt_type_t *> atypes;

                for (ref_t ga_ref : mt->args)
                {
                    rt_generic_argument_t * ga = current->get_generic_argument(ga_ref);
                    ref_t atype_ref = (*ga)->type;

                    rt_generic_param_t * gp;
                    int pack_count;

                    if (is_generic_params(atype_ref, &gp))
                    {
                        rt_sid_t name = to_sid((*gp)->name);
                        _A(gp_manager != nullptr);
                        gp_manager->pick_types(name, std::back_inserter(atypes));
                    }
                    else if ((mt_type_extra_t)atype_ref.extra == mt_type_extra_t::generic_param_index
                            && gp_manager->pack_at_index(atype_ref.index, &pack_count))
                    {
                        // A params generic parameter that is written by its index.
                        for (int k = 0; k < pack_count; k++)
                            atypes.push_back(gp_manager->type_at(atype_ref.index + k));
                    }
                    else
                    {
                        rt_type_t * atype = get_type(atype_ref);
                        _A(atype != nullptr);

                        atypes.push_back(atype);
                    }
                }

                return to_generic_type(template_, host_type, (rt_type_t **)atypes, atypes.size());
            }

            case mt_type_extra_t::generic_param: {
                _A(!is_generic_params(type_ref));

                rt_generic_param_t * gp = get_generic_param(type_ref);
                rt_sid_t name = to_sid((*gp)->name);

                return gp_manager->type_at(name);
            }

            case mt_type_extra_t::generic_param_index: {
                int index = type_ref.index;

                // The generic params of a generic method that is to be found are not known: the
                // type is a place holder.
                if (index >= (int)gp_manager->size())
                    return __get_type_place_holder(index);

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

    // Gets generic method.
    rt_generic_method_t * assembly_analyzer_t::to_generic_method(rt_method_t * template_,
                        rt_type_t * host_type, rt_type_t ** atypes, int atype_count)
    {
        return env.rpool.to_generic_method(env.memory, template_, host_type, atypes, atype_count);
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

                if ((*gp)->param_type == generic_param_type_t::params)
                {
                    _A(gp_manager != nullptr);
                    gp_manager->pick_types(name, std::back_inserter(atypes));
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

    // Gets method or generic method by method ref.
    rt_method_base_t * assembly_analyzer_t::get_method(ref_t method_ref, ref_t * out_method_ref)
    {
        switch ((mt_member_extra_t)method_ref.extra)
        {
            case mt_member_extra_t::internal:
            case mt_member_extra_t::import: {
                auto key = env.rpool.new_key<rpool_key_type_t::method>(current, method_ref);
                if (__rpool_method_t * cached = env.rpool.get(key))
                {
                    al::assign_value(out_method_ref, cached->ref);
                    return cached->method;
                }

                // A method whose types are those of the caller (a generic param of the method that calls) is
                // not the same for the callers that have other arguments: it is not kept.
                ref_t method_ref1;
                bool depends_on_caller = false;
                rt_method_base_t * method = __get_method(method_ref, &method_ref1, &depends_on_caller);

                if (!depends_on_caller)
                    env.rpool.set(key, __rpool_method_t { method, method_ref1 });

                al::assign_value(out_method_ref, method_ref1);
                return method;
            }

            case mt_member_extra_t::generic:
                return current->get_generic_method(method_ref, gp_manager);

            default:
                X_UNEXPECTED(_T("unexpected method type"));
        }
    }

    // Gets method.
    rt_method_base_t * assembly_analyzer_t::__get_method(ref_t method_ref,
                                                         ref_t * out_method_ref,
                                                         bool * out_depends_on_caller)
    {
        rt_method_ref_t * rt_method_ref;
        rt_type_t * rt_type = get_host_type_by_method_ref(method_ref, &rt_method_ref);
        _A(rt_type != nullptr);

        rt_assembly_t * rt_assembly = rt_type->get_assembly();
        rt_method_base_t * rt_method;

        al::assign_value(out_method_ref, method_ref);

        // Current assembly.
        if (__is_current(rt_assembly))
        {
            switch ((mt_member_extra_t)method_ref.extra)
            {
                case mt_member_extra_t::generic:
                    return rt_assembly->get_generic_method(method_ref, gp_manager)->template_;

                default:
                    return rt_assembly->get_method(method_ref);
            }
        }
        else    // External assemblies.
        {
            assembly_analyzer_t analyzer0(*this, rt_type->get_assembly());
            generic_param_manager_t gp_mgr(analyzer0, rt_type);
            assembly_analyzer_t analyzer(*this, &gp_mgr);

            mt_method_ref_t * mt = rt_method_ref->mt;

            // The types of the reference are those of the method as the caller sees them: a generic param of the
            // caller is not one of the host type (t.Item2 of a Tuple<T, T> in a generic method, that is T).
            auto type_of = [&](ref_t type_ref) {
                rt_type_t * type = analyzer.get_type(type_ref);
                if (type != nullptr)
                    return type;

                al::assign_value(out_depends_on_caller, true);
                return this->get_type(type_ref);
            };

            method_prototype_t prototype = {
                .name = __to_sid(rt_method_ref->mt->name),
                .return_type = type_of(mt->type),
                .generic_param_count = mt->generic_param_count,
            };

            ref_t params_ref = mt->params;
            int param_count = params_ref.count;
            rt_ptype_t params[param_count];

            ref_t param_ref = params_ref[0];
            for (rt_ptype_t * p = params, * p_end = p + param_count; p < p_end; p++)
            {
                rt_method_ref_param_t * param = current->get_method_ref_param(param_ref++);
                rt_type_t * type = type_of(param->mt->type);
                prototype.arg_types.push_back(rt_ptype_t(type, param->mt->ptype));
            }

            rt_method = nullptr;
            rt_type->each_method(env, [&, this](ref_t ref, rt_method_base_t * rt_method_) {
                bool r = rt_method_->compare_to(env, prototype, &gp_mgr);
                return r? rt_method = rt_method_, al::assign_value(out_method_ref, ref),
                    false : true;
            });
        }

        _A(rt_method != nullptr);
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

            bool r = true;
            rt_assembly->each_params(mt.params, [&p, &analyzer, &r](int, mt_param_t & param) {
                
                if (param.param_type != p->param_type)
                    return (r = false, false);

                rt_generic_param_t * gp;
                if (analyzer.is_generic_params(param.type, &gp))
                    return (r = is_place_holder(p->type), false);

                if (analyzer.get_type(param.type) != p->type)
                    return (r = false, false);

                p++;

                return true;
            });

            return r;
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
    rt_field_base_t * assembly_analyzer_t::__get_field(ref_t field_ref,
                                                        rt_type_t ** out_type = nullptr)
    {
        rt_field_ref_t * rt_field_ref;
        rt_type_t * rt_type = get_host_type_by_field_ref(field_ref, &rt_field_ref);

        rt_assembly_t * rt_assembly = rt_type->get_assembly();

        al::assign_value(out_type, rt_type);

        rt_field_base_t * rt_field;

        mt_member_extra_t extra = (mt_member_extra_t)field_ref.extra;
        switch (extra)
        {
            case mt_member_extra_t::internal:
                rt_field = rt_assembly->get_field(field_ref);
                break;

            case mt_member_extra_t::generic: {
                mt_generic_field_t * mt = current->mt_of_generic_field(field_ref);

                rt_field_t * template_field = as<rt_field_t *>(__get_field(mt->template_));
                _A(template_field != nullptr);
                
                generic_param_manager_t gp(env, rt_type);
                assembly_analyzer_t analyzer(env, rt_type->get_assembly(), &gp);

                ref_t field_type_ref = (*template_field)->type;
                rt_type_t * field_type = analyzer.get_type(field_type_ref);
                rt_field = memory_t::new_obj<rt_generic_field_t>(env.memory, field_type);

            }   break;

            case mt_member_extra_t::position:
                rt_field = nullptr;
                break;

            case mt_member_extra_t::import: {
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
            }   break;

            default:
                X_UNEXPECTED_F("unexpected member type '%1%'", extra);
        }

        return rt_field;
    }

    // Gets generic param.
    rt_generic_param_t * assembly_analyzer_t::get_generic_param(ref_t ref)
    {
        return current->get_generic_param(ref);
    }

    // Returns whether it's a generic params.
    bool assembly_analyzer_t::is_generic_params(ref_t ref, rt_generic_param_t ** out_gp)
    {
        if ((mt_type_extra_t)ref.extra == mt_type_extra_t::generic_param)
        {
            rt_generic_param_t * gp = get_generic_param(ref);
            if ((*gp)->param_type == generic_param_type_t::params)
            {
                al::assign_value(out_gp, gp);
                return true;
            }
        }

        return false;
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
    int assembly_analyzer_t::get_virtual_method_offset(ref_t method_ref, rt_type_t * host_type)
    {
        rt_method_t * method;

        // The method of a generic type that has generic params of its own (IB<Int32>.Make<TKey>)
        // is a generic method of a generic method: the template is found by the two of them.
        while ((mt_member_extra_t)method_ref.extra == mt_member_extra_t::generic)
        {
            rt_mt_t<__tidx_t::generic_method> * mt = current->mt_of_generic_method(method_ref);
            method_ref = mt->template_;
        }

        method = _M(rt_method_t *, this->get_method(method_ref, &method_ref));
        _A(method != nullptr);

        rt_type_t * rt_type = method->get_host_type();

        // All methods in interface types are all virtual members.
        if (rt_type->get_ttype(env) == ttype_t::interface_)
            return rt_type->get_method_offset(env, method_ref);

        // The base type of a generic template (Base<T> of Derived<T>) has place holders as its
        // arguments: the number of its virtual members does not depend on them.
        auto base_of = [&](rt_type_t * type) -> rt_type_t * {
            if (type->get_kind() != rt_type_kind_t::general
                    || ((rt_general_type_t *)type)->generic_param_count() == 0)
                return type->get_base_type(env);

            rt_general_type_t * general = (rt_general_type_t *)type;
            assembly_analyzer_t type_analyzer(env, general->get_assembly());

            generic_param_manager_t holders;
            int index = 0;
            for (ref_t gp_ref : (*general)->generic_params)
            {
                rt_generic_param_t * gp = type_analyzer.get_generic_param(gp_ref);
                holders.append(type_analyzer.to_sid((*gp)->name), __get_type_place_holder(index++));
            }

            return general->get_base_type(env, &holders);
        };

        decorate_t decorate = (decorate_t)(*method)->decorate;
        if (__is_virtual(decorate))
        {
            int offset = rt_type->get_method_offset(env, method_ref);

            rt_type_t * base_type = base_of(rt_type);
            if (base_type != nullptr)
                offset += base_type->get_virtual_count(env);

            return offset;
        }

        if (decorate.is_override)
        {
            // An override in an instance of a generic type: the base types are those of the
            // instance, the method that it overrides is found by the prototype in their terms.
            if (host_type != nullptr && host_type->get_kind() == rt_type_kind_t::generic)
            {
                generic_param_manager_t owner_gp(env, host_type);
                assembly_analyzer_t owner_analyzer(env, method->get_assembly(), &owner_gp);

                method_prototype_t prototype;
                owner_analyzer.get_prototype(method, &prototype);

                for (rt_type_t * type = host_type->get_base_type(env); type != nullptr;
                                                            type = type->get_base_type(env))
                {
                    generic_param_manager_t type_gp(env, type);

                    rt_method_base_t * root = nullptr;
                    ref_t root_ref = ref_t::null;

                    type->each_method(env, [&](ref_t ref, rt_method_base_t * m) {
                        if (!m->is_static() && m->is_virtual() && !m->is_override()
                                && m->compare_to(env, prototype, &type_gp))
                        {
                            root = m;
                            root_ref = ref;
                            return false;
                        }

                        return true;
                    });

                    if (root != nullptr)
                    {
                        return assembly_analyzer_t(env, root->get_assembly())
                                                    .get_virtual_method_offset(root_ref);
                    }
                }

                throw __RtAssemblyErrorF("virtual method '%1%' not found", method->get_name());
            }

            // method_ref is a ref of the assembly of the method (it is changed by get_method), that may not
            // be the current one: the method of an imported type that is called by the type itself.
            method_prototype_t prototype;
            assembly_analyzer_t(*this, method->get_assembly()).get_prototype(method, &prototype);

            rt_assembly_t * base_assembly;

            do
            {
                rt_type = base_of(rt_type);
                if (rt_type == nullptr)
                    throw __RtAssemblyErrorF("virtual method '%1%' not found", method->get_name());

                base_assembly = rt_type->get_assembly();

                assembly_analyzer_t analyzer0(*this, base_assembly);
                method_ref = rt_type->search_method(analyzer0.env, prototype);

            } while (method_ref == ref_t::null);

            // The ref is a ref of the assembly of the base type.
            return assembly_analyzer_t(*this, base_assembly).get_virtual_method_offset(method_ref);
        }

        throw __RtAssemblyErrorF("virtual method '%1%' not found", method->get_name());
    }

    // Gets prototype of method.
    void assembly_analyzer_t::get_prototype(ref_t method_ref, method_prototype_t * out_prototype)
    {
        rt_method_t * rt_method = _M(rt_method_t *, get_method(method_ref));
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

        // The generic params of the method are place holders (T of IEnumerable<T> in
        // Make<T>(IEnumerable<T> items)): the methods that are compared to the prototype
        // have the same ones.
        generic_param_manager_t method_gp_mgr;
        const generic_param_manager_t * types_gp_mgr = gp_manager;

        if (mt->generic_params.count > 0)
        {
            method_gp_mgr.parent = gp_manager;

            int index = 0;
            for (ref_t gp_ref : mt->generic_params)
            {
                rt_generic_param_t * gp = get_generic_param(gp_ref);
                method_gp_mgr.append(to_sid((*gp)->name), __get_type_place_holder(index++));
            }

            types_gp_mgr = &method_gp_mgr;
        }

        assembly_analyzer_t analyzer(env, current, types_gp_mgr);

        for (ref_t param_ref : mt->params)
        {
            rt_param_t * rt_param = current->get_param(param_ref);
            _A(rt_param != nullptr);

            rt_type_t * param_type = analyzer.get_type((*rt_param)->type);
            mp->arg_types.push_back(rt_ptype_t(param_type, (*rt_param)->param_type));
        }

        mp->name = current->get_name(rt_method);
        mp->return_type = analyzer.get_type(mt->type);
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
                template_ = _M(rt_method_t *, analyzer.get_method(mt->template_));

                if (mt->host.empty())   // it's general host type.
                    host_type = assembly->get_host_by_method_ref(mt->template_);
                else                    // the generic host type, with generic arguments.
                    host_type = analyzer.get_type(mt->host);

            }   break;

            case mt_member_extra_t::generic: {
                rt_generic_method_t * g_template = assembly->get_generic_method(
                                                            mt->template_, gp_mgr);
                _A(g_template != nullptr);

                template_ = g_template->template_;
                host_type = g_template->host_type;

            }  break;

            case mt_member_extra_t::import: {
                template_ = _M(rt_method_t *, analyzer.get_method(mt->template_));

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
        // The host is the generic type itself (a method of Box<T> that calls another generic method of Box<T>): it has the
        // arguments that the method that calls has.
        if (gp_mgr != nullptr && host_type->get_kind() == rt_type_kind_t::general
                && ((rt_general_type_t *)host_type)->is_generic_template())
        {
            rt_type_t * instance = analyzer.to_generic_type((rt_general_type_t *)host_type);
            if (instance != nullptr)
                host_type = instance;
        }

        // The argument may be the types of a params generic parameter of the method that calls
        // (Build<R, TArgs ...> calls Build2<R, TArgs ...>): they are many arguments.
        std::vector<rt_type_t *> atype_list;
        for (ref_t ga_ref : mt->args)
        {
            rt_generic_argument_t * generic_argument = assembly->get_generic_argument(ga_ref);
            ref_t atype_ref = (*generic_argument)->type;

            rt_generic_param_t * pack_gp;
            if (analyzer.is_generic_params(atype_ref, &pack_gp))
            {
                _A(gp_mgr != nullptr);
                gp_mgr->pick_types(analyzer.to_sid((*pack_gp)->name), std::back_inserter(atype_list));
                continue;
            }

            rt_type_t * atype = analyzer.get_type(atype_ref);
            _A(atype != nullptr);

            atype_list.push_back(atype);
        }

        int atype_count = (int)atype_list.size();
        rt_type_t ** atypes = atype_list.data();

        __rt_generic_args_key_t args_key(atypes, atypes + atype_count);
        auto key = ctx.rpool.new_key<rpool_key_type_t::generic_method>(
            template_, host_type, args_key
        );

        return ctx.rpool.get(key, [&]() {

            key = ctx.rpool.revise_key(key, atypes, atype_count);
            return memory_t::new_obj<rt_generic_method_t>(
                ctx.memory, template_, host_type, key.types(), atype_count
            );

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

        _A(method->host_type != nullptr);

        return method;
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns whether it's a rt_generic_param_t and the type is generic_param_type_t::params.
    bool is_generic_params(rt_generic_param_t * gp)
    {
        _A(gp != nullptr);

        return (*gp)->param_type == generic_param_type_t::params;
    }

    ////////// ////////// ////////// ////////// //////////

    // Finds, among the types that the receiver is (its base types, its interfaces), the one with
    // the template: with its generic arguments.
    static rt_type_t * __find_type_of_template(analyzer_env_t & env, rt_type_t * receiver,
                                               rt_type_t * template_type,
                                               rt_type_t ** atypes = nullptr, int atype_count = 0)
    {
        rt_type_t * found = nullptr;

        std::function<bool (rt_type_t *)> find = [&](rt_type_t * type) -> bool {

            rt_type_t * type_template = type->get_kind() == rt_type_kind_t::generic?
                    (rt_type_t *)((rt_generic_type_t *)type)->template_ : type;

            if (type_template == template_type)
            {
                // With the arguments that the interface of the call has.
                bool match = true;

                if (atype_count > 0)
                {
                    match = type->get_kind() == rt_type_kind_t::generic
                        && ((rt_generic_type_t *)type)->atype_count() == atype_count;

                    for (int k = 0; match && k < atype_count; k++)
                        match = ((rt_generic_type_t *)type)->atypes[k] == atypes[k];
                }

                if (match)
                {
                    found = type;
                    return false;
                }
            }

            type->each_super_type(env, [&](rt_type_t * super_type) {
                return find(super_type);
            });

            return found == nullptr;
        };

        for (rt_type_t * type = receiver; type != nullptr && found == nullptr;
                                                            type = type->get_base_type(env))
        {
            find(type);
        }

        return found;
    }

    // A static abstract (or virtual) method of an interface that is called through a generic
    // param (T.MaxValue): the method has as its host the type that the param is. Returns the
    // method of that type that implements it; or the default method of the interface, if it
    // has one (static virtual). Returns null if the call is not of this kind.
    rt_method_base_t * resolve_constrained_static(analyzer_env_t & env,
                                                  rt_generic_method_t * method)
    {
        rt_method_t * template_ = method->template_;

        if (template_ == nullptr || !template_->is_static())
            return nullptr;

        decorate_t decorate = (decorate_t)(*template_)->decorate;
        if (!decorate.is_abstract && !decorate.is_virtual)
            return nullptr;

        rt_type_t * interface_template = template_->get_host_type();
        if (interface_template == nullptr || interface_template->get_ttype(env) != ttype_t::interface_)
            return nullptr;

        // The receiver is the interface itself, or an interface of it: it is not the type that a
        // param was.
        rt_type_t * receiver = method->get_host_type();
        if (receiver == nullptr || receiver->get_ttype(env) == ttype_t::interface_)
            return nullptr;

        // The arguments of a generic method are its own (Create<TOther>): they are not those of the interface.
        int own_count = template_->generic_param_count();

        // The interface, with its generic arguments, that the type implements.
        rt_type_t * interface_type = own_count > 0?
            __find_type_of_template(env, receiver, interface_template)
          : __find_type_of_template(env, receiver, interface_template,
                                    method->atypes, method->atype_count());

        if (interface_type == nullptr)
        {
            throw __RtAssemblyErrorF("the type '%1%' does not implement the interface '%2%'",
                receiver->get_name(env), interface_template->get_name(env));
        }

        // The prototype of the method, in the terms of the type that implements.
        generic_param_manager_t interface_gp(env, interface_type);
        assembly_analyzer_t analyzer(env, template_->get_assembly(), &interface_gp);

        method_prototype_t prototype;
        analyzer.get_prototype(template_, &prototype);

        rt_method_base_t * found = nullptr;
        for (rt_type_t * type = receiver; type != nullptr && found == nullptr;
                                                            type = type->get_base_type(env))
        {
            generic_param_manager_t type_gp(env, type);

            type->each_method(env, [&](ref_t, rt_method_base_t * m) {
                if (m->is_static() && m->compare_to(env, prototype, &type_gp))
                {
                    found = m;
                    return false;
                }

                return true;
            });
        }

        // A generic method of the type is made for the arguments of the call.
        if (found != nullptr && own_count > 0)
        {
            assembly_analyzer_t found_analyzer(env, found->get_assembly());

            if (found->this_type() == rt_member_type_t::general)
            {
                return found_analyzer.to_generic_method(static_cast<rt_method_t *>(found),
                                    found->get_host_type(), method->atypes, method->atype_count());
            }

            rt_generic_method_t * generic_found = static_cast<rt_generic_method_t *>(found);
            return found_analyzer.to_generic_method(generic_found->template_, generic_found->get_host_type(),
                                                    method->atypes, method->atype_count());
        }

        if (found != nullptr)
            return found;

        // The method that the interface has (static virtual), with the types of the interface.
        if (decorate.is_virtual && (*template_)->body != res_t::null)
        {
            assembly_analyzer_t default_analyzer(env, template_->get_assembly());
            return own_count > 0?
                default_analyzer.to_generic_method(template_, interface_type, method->atypes, method->atype_count())
              : default_analyzer.to_generic_method(template_, interface_type);
        }

        throw __RtAssemblyErrorF("the type '%1%' does not implement the static member '%2%' of '%3%'",
            receiver->get_name(env), template_->get_name(), interface_type->get_name(env));
    }

    // A call of a member of a generic param (t.Get(), where T : IFoo): the method of the type that
    // the param is, found by the method of the constraint; a class takes the virtual table.
    constrained_target_t resolve_constrained_instance(analyzer_env_t & env,
                                                      rt_generic_method_t * method)
    {
        rt_method_t * template_ = method->template_;
        rt_type_t * receiver = method->get_host_type();

        rt_type_t * owner_type = __find_type_of_template(env, receiver, template_->get_host_type(),
                                                         method->atypes, method->atype_count());
        if (owner_type == nullptr)
        {
            throw __RtAssemblyErrorF("the type '%1%' does not have the method '%2%'",
                receiver->get_name(env), template_->get_name());
        }

        // The prototype of the method, in the terms of the type that has it.
        generic_param_manager_t owner_gp(env, owner_type);
        assembly_analyzer_t analyzer(env, template_->get_assembly(), &owner_gp);

        method_prototype_t prototype;
        analyzer.get_prototype(template_, &prototype);

        rt_method_base_t * found = nullptr;
        rt_type_t * found_type = nullptr;
        ref_t found_ref = ref_t::null;

        for (rt_type_t * type = receiver; type != nullptr && found == nullptr;
                                                            type = type->get_base_type(env))
        {
            generic_param_manager_t type_gp(env, type);

            type->each_method(env, [&](ref_t ref, rt_method_base_t * m) {
                if (!m->is_static() && m->compare_to(env, prototype, &type_gp))
                {
                    found = m;
                    found_type = type;
                    found_ref = ref;
                    return false;
                }

                return true;
            });
        }

        if (found == nullptr)
        {
            throw __RtAssemblyErrorF("the type '%1%' does not have the method '%2%'",
                receiver->get_name(env), template_->get_name());
        }

        // The method of a generic type is the one of its instance.
        rt_method_base_t * target = found;
        if (found_type->get_kind() == rt_type_kind_t::generic
                && found->this_type() == rt_member_type_t::general)
        {
            assembly_analyzer_t found_analyzer(env, found->get_assembly());
            target = found_analyzer.to_generic_method(static_cast<rt_method_t *>(found), found_type);
        }

        // A class: the virtual table, as the object can be of a type that derives.
        bool is_virtual = receiver->get_ttype(env) == ttype_t::class_ && found_ref != ref_t::null
                && (found->is_virtual() || found->is_override());

        // The slot of the virtual table is the one of the method that declares it: an override
        // takes it from a base type (that may be a generic type).
        int virtual_offset = -1;
        if (is_virtual)
        {
            rt_method_base_t * root = found;
            ref_t root_ref = found_ref;

            if (found->is_override())
            {
                bool done = false;

                for (rt_type_t * type = found_type->get_base_type(env);
                                    type != nullptr && !done; type = type->get_base_type(env))
                {
                    generic_param_manager_t type_gp(env, type);

                    type->each_method(env, [&](ref_t ref, rt_method_base_t * m) {
                        if (!m->is_static() && m->is_virtual() && !m->is_override()
                                && m->compare_to(env, prototype, &type_gp))
                        {
                            root = m;
                            root_ref = ref;
                            done = true;
                            return false;
                        }

                        return true;
                    });
                }
            }

            assembly_analyzer_t root_analyzer(env, root->get_assembly());
            virtual_offset = root_analyzer.get_virtual_method_offset(root_ref);
        }

        return constrained_target_t { target, found, found_ref, is_virtual, virtual_offset };
    }


}   // namespace X_ROOT_NS::modules::rt


