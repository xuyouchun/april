#ifndef __CORE_ASSEMBLY_H__
#define __CORE_ASSEMBLY_H__

namespace X_ROOT_NS { namespace modules { namespace core {

    ////////// ////////// ////////// ////////// //////////

    // Assembly layout version.
    typedef __assembly_layout_version_t __lv_t;

    // Current layout version.
    const __lv_t __current_lv = 1;

    ////////// ////////// ////////// ////////// //////////
    // assembly_type_t

    // Assembly types
    X_ENUM(assembly_type_t)

        // Unknown, error assembly type.
        unknown,

        // Dynamic library.
        dynamic_library,

        // Executable assembly.
        executable,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////
    // assembly_reference_t

    // Assembly reference.
    class assembly_t;
    class assembly_reference_t : public object_t
    {
    public:

        // Constructor
        assembly_reference_t(const mname_t * package_name, assembly_t * assembly)
            : package_name(package_name), assembly(assembly)
        { }

        const mname_t * package_name  = nullptr;    // Package name.
        assembly_t *    assembly      = nullptr;    // Assembly.
    };

    ////////// ////////// ////////// ////////// //////////
    // assembly_references_t

    // Assembly references.
    class assembly_references_t : public object_t
    {
        typedef const mname_t * __package_name_t, * __assembly_name_t;

    public:

        // Constructors.
        assembly_references_t(assembly_t * assembly, xpool_t & xpool)
            : __assembly(assembly), __xpool(xpool) { }

        // Add a reference.
        assembly_reference_t * add_reference(__package_name_t package_name, assembly_t * assembly);

        // Get a reference of a assembly.
        assembly_reference_t * get_reference(assembly_t * assembly);
        
    private:
        typedef std::tuple<__package_name_t, __assembly_name_t> __assembly_reference_key_t;
        std::map<__assembly_reference_key_t, assembly_reference_t *> __assembly_references;
        std::map<assembly_t *, assembly_reference_t *> __assembly_reference_relation;

        assembly_t * __assembly;
        xpool_t &    __xpool;
    };

    ////////// ////////// ////////// ////////// //////////
    // types_t

    // Type collection.
    class types_t : public object_t
    {
    public:

        // Constructor.
        types_t(assembly_t * assembly, xpool_t & xpool) : __assembly(assembly), __xpool(xpool) { }

        // Appends a type.
        bool append_type(general_type_t * type);

        // Appends a typedef.
        bool append_type_def(sid_t ns, type_def_t * type_def);

        // Gets type of specified namespace and name.
        general_type_t * get_general_type(sid_t ns, sid_t name,
            size_t template_param_count = 0, type_t * host_type = nullptr
        );

        // Enums all general types.
        template<typename callback_t> void each_general_types(callback_t callback)
        {
            __each_types(__general_types, callback);
        }

        // Gets typedef of specified namespace and name.
        type_def_t * get_type_def(sid_t ns, sid_t name,
            size_t template_param_count = 0, type_t * host_type = nullptr);

        // Enums all typedefs.
        template<typename callback_t> void each_type_defs(callback_t callback)
        {
            __each_types(__type_defs, callback);
        }

        // Gets the generic type of specified template and types and host type.
        generic_type_t * get_generic_type(general_type_t * template_,
                        xtype_collection_t & types, type_t * host_type = nullptr);

        // Enums all generic types.
        template<typename callback_t> void each_generic_types(callback_t callback)
        {
            __each_types(__generic_types, callback);
        }

        // Creates a new generic type.
        generic_type_t * new_generic_type(general_type_t * template_,
                        xtype_collection_t & types,  type_t * host_type = nullptr);

        // Creates a new array type.
        array_type_t * new_array_type(type_t * type, size_t dimension);

        // Enums all array types.
        template<typename callback_t> void each_array_types(callback_t callback)
        {
            __each_types(__array_types, callback);
        }

    private:
        xpool_t &    __xpool;       // Xpool.
        assembly_t * __assembly;    // Assembly.

        typedef sid_t __full_class_name_t, __namespace_t, __name_t;
        typedef size_t __generic_argument_count_t;
        typedef type_t * __host_type_t;

        typedef std::tuple<
            __full_class_name_t, __namespace_t, __generic_argument_count_t, __host_type_t
        > __general_type_key_t;

        typedef std::tuple<
			type_t *, type_collection_id_t, __generic_argument_count_t,  __host_type_t
		> __generic_type_key_t;

        typedef std::tuple<type_t *, size_t> __array_type_key_t;

        typedef std::tuple<
            __namespace_t, __name_t, __generic_argument_count_t, __host_type_t
        > __type_def_key_t;

        std::map<__general_type_key_t, general_type_t *> __general_types;
        std::map<__generic_type_key_t, generic_type_t *> __generic_types;
        std::map<__array_type_key_t,   array_type_t *>   __array_types;
        std::map<__type_def_key_t,     type_def_t *>     __type_defs;

        // Enums types in the map.
        template<typename _key_t, typename _type_t, typename callback_t>
        void __each_types(std::map<_key_t, _type_t *> & map, callback_t callback)
        {
            al::for_each(map, [&callback](auto & it) { callback(it.second); });
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // assembly_t

    // Assembly.
    class assembly_t : public object_t
                     , public named_object_t, public no_copy_ctor_t
    {
    public:

        // Constructor.
        assembly_t(xpool_t & xpool, const mname_t * name)
            : named_object_t(name), types(this, xpool), references(this, xpool)
            , __xpool(xpool), version(1, 0, 0, 0)
        { }

        // Write assembly buffer to a stream.
        void write(xostream_t & stream, logger_t & logger, method_compile_controller_t * controller);

        // Loads assembly from a stream.
        void load(xistream_t & stream, assembly_loader_t * loader);

        // Assembly references.
        assembly_references_t   references;

        // Types.
        types_t                 types;

        version_t   version;                        // Assembly version.
        method_t *  entry_point     = nullptr;      // Entry point.
        sid_t       information;                    // Information.

        // Converts to a string.
        operator string_t () const { return (string_t)*name; }

        // Convets to a string.
        virtual const string_t to_string() const override 
        {
            return (string_t)*this;
        }

        // Gets xpool.
        xpool_t & get_xpool() const { return __xpool; }

        // Returns whether the assembly has a entry point.
        bool has_entry_point() const { return entry_point != nullptr; }

        static const efamily_t __family__ = efamily_t::assembly;

    private:
        xpool_t & __xpool;      // Xpool.
    };

    ////////// ////////// ////////// ////////// //////////
    // assemblies_t

    // Assemblies.
    class assemblies_t : public object_t, public no_copy_ctor_t
    {
    public:

        // Appends an assembly.
        void append(assembly_t * assembly);

        // Gets assembly by a name.
        assembly_t * get(const mname_t * name);

        // Gets assembly by a name.
        assembly_t * get(const string_t & name);

        // Returns count of assembly.
        const size_t size() const { return __assemblies.size(); }

        // Returns whether the assembly collection is empty.
        bool empty() const { return __assemblies.empty(); }

        // Returns the begin iterator.
        auto begin() const { return __assemblies.begin(); }

        // Returns the end iterator.
        auto end()   const { return __assemblies.end();   }

    private:
        std::map<sid_t, assembly_t *> __assembly_sid_map;
        std::map<string_t, assembly_t *> __assembly_string_map;
        std::vector<assembly_t *> __assemblies;
    };

    ////////// ////////// ////////// ////////// //////////

} } }

#endif  // __CORE_ASSEMBLY_H__ 

