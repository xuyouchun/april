
#include <core.h>

namespace X_ROOT_NS { namespace modules { namespace core {

    typedef assembly_error_code_t __e_t;

    ////////// ////////// ////////// ////////// //////////
    // assembly_references_t

    // Add a reference.
    assembly_reference_t * assembly_references_t::add_reference(
                            __package_name_t package_name, assembly_t * assembly)
    {
        _A(assembly != nullptr);

        __assembly_reference_key_t key(package_name, assembly->name);

        auto it = __assembly_references.find(key);
        if(it != __assembly_references.end())
            return it->second;

        assembly_reference_t * ref = __xpool.new_reference(package_name, assembly);
        __assembly_references[key] = ref;

        if(!al::map_insert(__assembly_reference_relation, assembly, ref))
            throw _ED(__e_t::assembly_duplicated, assembly->name);

        return ref;
    }

    // Get assembly reference for a assembly.
    assembly_reference_t * assembly_references_t::get_reference(assembly_t * assembly)
    {
        auto it = __assembly_reference_relation.find(assembly);
        if(it == __assembly_reference_relation.end())
            return nullptr;

        return it->second;
    }

    ////////// ////////// ////////// ////////// //////////
    // types_t

    // Returns value from a map.
    template<typename _map_t, typename _key_t = typename _map_t::key_type,
                              typename _value_t = typename _map_t::mapped_type>
    _value_t __get_type(const _map_t & map, const _key_t & key)
    {
        auto it = map.find(key);

        if(it != map.end())
            return it->second;

        return nullptr;
    }

    // Append a value to a map.
    template<typename _map_t, typename _key_t = typename _map_t::key_type,
                              typename _value_t = typename _map_t::mapped_type>
    bool __append(_map_t & map, const _key_t & key, _value_t && value)
    {
        typedef typename _map_t::value_type pair_t;
        return map.insert(pair_t(key, value)).second;
    }

    // Converts mname to a sid.
    static sid_t __to_sid(const mname_t * mname)
    {
        return mname? mname->sid : sid_t::null;
    }

    // Converts namespace to a sid.
    static sid_t __to_sid(namespace_t * ns)
    {
        return ns? __to_sid(ns->full_name) : sid_t::null;
    }

    // Converts namespace to a sid.
    static sid_t __ns_to_sid(type_t * type, sid_t default_ = sid_t::null)
    {
        if(type == nullptr)
            return default_;

        while(type->host_type != nullptr)
        {
            type = type->host_type;
        }

        return __to_sid(_M(general_type_t *, type)->namespace_);
    }

    // Appends a type.
    bool types_t::append_type(general_type_t * type)
    {
        _A(type != nullptr);

        __genaral_type_key_t key(__ns_to_sid(type), (sid_t)type->name,
            type->params_count(), type->host_type
        );

        type->assembly = __assembly;
        return __append(__general_types, key, type);
    }

    // Appends typedef.
    bool types_t::append_type_def(sid_t ns, type_def_t * type_def)
    {
        _A(type_def != nullptr);

        __type_def_key_t key(ns, (sid_t)type_def->name,
            type_def->param_count(), type_def->host_type
        );

        return __append(__type_defs, key, type_def);
    }

    // Gets general type by namespace, name, etc.
    general_type_t * types_t::get_general_type(sid_t ns, sid_t name,
            size_t template_param_count, type_t * host_type)
    {
        ns = __ns_to_sid(host_type, ns);
        if(ns.is_empty())
            ns = sid_t::null;

        return __get_type(__general_types,
            __genaral_type_key_t(ns, name, template_param_count, host_type)
        );
    }

    // Gets typedef by namespace, name, etc.
    type_def_t * types_t::get_type_def(sid_t ns, sid_t name,
                    size_t template_param_count, type_t * host_type)
    {
        return __get_type(__type_defs, __type_def_key_t(
            ns, name, template_param_count, host_type
        ));
    }

    // Gets generic type by template, types and host type.
    generic_type_t * types_t::get_generic_type(general_type_t * template_,
                                        xtype_collection_t & types, type_t * host_type)
    {
        _A(template_ != nullptr);

        return __get_type(__generic_types, __generic_type_key_t(
            template_, types.get_tcid(__xpool), host_type
        ));
    }

    // Creates a generic type.
    generic_type_t * types_t::new_generic_type(general_type_t * template_,
                                        xtype_collection_t & types, type_t * host_type)
    {
        _A(template_ != nullptr);

        generic_type_t * generic_type = get_generic_type(template_, types, host_type);
        if(generic_type == nullptr)
        {
            generic_type = __xpool.new_generic_type(template_, types, host_type);
            __append(__generic_types,
                __generic_type_key_t(template_, types.get_tcid(__xpool), host_type),
                generic_type
            );
        }

        return generic_type;
    }

    // Creates a array type.
    array_type_t * types_t::new_array_type(type_t * type, size_t dimension)
    {
        if(type == nullptr)
            type = null_type_t::instance();

        array_type_t * array_type = __get_type(__array_types, __array_type_key_t(type, dimension));
        if(array_type == nullptr)
        {
            array_type = __xpool.new_array_type(type, dimension);
            __append(__array_types, __array_type_key_t(type, dimension), array_type);
        }

        return array_type;
    }

    ////////// ////////// ////////// ////////// //////////
    // assembly_t

    // Writes the assembly to a stream.
    void assembly_t::write(xostream_t & stream, logger_t & logger)
    {
        assembly_write(stream, *this, __current_lv, logger);
    }

    // Loads assembly from a stream.
    void assembly_t::load(xistream_t & stream, assembly_loader_t * loader)
    {
        assembly_read(stream, *this, loader);
    }

    ///////// ////////// ////////// ////////// //////////
    // assemblies_t

    // Append a assembly.
    void assemblies_t::append(assembly_t * assembly)
    {
        _A(assembly != nullptr);

        __assembly_sid_map[to_sid(assembly->name)] = assembly;
        __assembly_string_map[(string_t)*assembly->name] = assembly;
        __assemblies.push_back(assembly);
    }

    // Returns assembly by its name.
    assembly_t * assemblies_t::get(const mname_t * name)
    {
        auto it = __assembly_sid_map.find(to_sid(name));
        if(it != __assembly_sid_map.end())
            return it->second;

        return nullptr;
    }

    // Returns assembly by its name.
    assembly_t * assemblies_t::get(const string_t & name)
    {
        auto it = __assembly_string_map.find(name);
        if(it != __assembly_string_map.end())
            return it->second;

        return nullptr;
    }

    ////////// ////////// ////////// ////////// //////////

    #undef __S
    #undef __SearchRet
    #undef __CheckEmpty
    #undef __CheckEmptyV
    #undef __Unexpected

    #undef __M
    #undef __EntityMember

} } }

