
#include <core.h>

namespace X_ROOT_NS::modules::core {

    #define __Unexpected() X_UNEXPECTED()

    // Check ref limit.
    #define __CheckRefLimit(ref, limit, name)                                           \
        do {                                                                            \
            if (ref.count > 0 && ref.index + ref.count - 1 >= limit)                    \
                throw _ED(__e_t::overlimit, name);                                      \
        } while (false)

    #define __FakeEntryPoint    ((method_t *)0x01)

    typedef assembly_error_code_t __e_t;

    ////////// ////////// ////////// ////////// //////////

    // Returns a format error.
    static logic_error_t<__e_t> __format_error(const string_t & s)
    {
        return _E(__e_t::format_error, s);
    }

    ////////// ////////// ////////// ////////// //////////

    // Base class of assembly reader.
    template<typename reader_t, __lv_t lv, __lv_t rlv = lv>
    class __assembly_reader_super_t { };

    //-------- ---------- ---------- ---------- ----------

    // Metadata.
    template<__tidx_t tidx> struct __mt_t : mt_t<tidx>
    {
        typedef __mt_t<tidx> __self_t;
        typedef mt_t<tidx>   __super_t;

        using __super_t::__super_t;

        static void init(__self_t & self) { }
        static void commit(__self_t & self) { }
    };

    //-------- ---------- ---------- ---------- ----------

    // Metadata reader template.
    struct __mt_reader_template_t
    {
        template<__tidx_t tidx> using mt_t = __mt_t<tidx>;
    };

    ////////// ////////// ////////// ////////// //////////
    // __assembly_reader_super_t

    // Base class of a assembly reader.
    template<typename reader_t, __lv_t rlv>
    class __assembly_reader_super_t<reader_t, 0, rlv>
            : public compile_assembly_layout_t<al::spool_t, rlv, __mt_reader_template_t>
    {
        typedef __assembly_reader_super_t<reader_t, 0, rlv> __self_t;
        typedef compile_assembly_layout_t<al::spool_t, rlv, __mt_reader_template_t> __super_t;

        template<__tidx_t tidx>
        using __mt_t = typename __mt_reader_template_t::template mt_t<tidx>;

    public:

        // Constructors.
        __assembly_reader_super_t(xistream_t & stream, assembly_t & assembly,
                assembly_loader_t * loader)
            : __assembly(assembly), __stream(stream)
            , __assembly_loader(loader), __attributes(this)
            , __res_reader(this->__heap, __XPool.spool)
        { }

        // Read metadatas.
        void read(bool brief)
        {
            if (!brief)
            {
                __read_metadata<>(&this->__heap);
                __build_entities();
            }
            else
            {
                __read_metadata<__tidx_t::assembly>(nullptr);
                __load_assembly_brief();
            }
        }

    protected:
        assembly_t        & __assembly;
        assembly_loader_t * __assembly_loader;
        xistream_t        & __stream;

    private:

        // Returns table count.
        static constexpr int __table_count()
        {
            return __self_t::value_of(__ln_t::table_count);
        }

        // Read metadata.
        template<__tidx_t _read_to = (__tidx_t)(__table_count() - 1)>
        void __read_metadata(mt_heap_t * heap)
        {
            read_metadata<rlv, __table_count(), __mt_reader_template_t, _read_to>(
                __stream, this->__mt_objects, heap
            );
        }

        template<typename entity_t> using __list_t = std::vector<entity_t>;

        // Attribue list
        class __attribute_list_t : public __list_t<attribute_t *>
        {
            typedef __list_t<attribute_t *> __super_t;
            typedef __self_t                __owner_t;
            typedef size_type               __size_t;

        public:

            // Constructor.
            __attribute_list_t(__owner_t * owner) : __owner(owner) { }

            // Returns attribute at specified pos.
            attribute_t * operator [] (__size_t pos)
            {
                attribute_t * attr = __super_t::operator[](pos);
                if (__owner->__is_compile_time_attribute(attr))
                    return __XPool.get_compile_time_attribute();

                return attr;
            }

        private:
            __owner_t * __owner;
        };

        //-------- ---------- ---------- ---------- ----------

        // Logger.
        class __logger_t : public logger_t
        {
        public:
            virtual void log(code_element_t * element, log_level_t level,
                            const string_t & name, int code, const string_t & message) override
            {
                if (level == log_level_t::error)
                    throw _E(__e_t::format_error, message);
            }
        };

        //-------- ---------- ---------- ---------- ----------

        // Generic param placeholder type.
        class __generlc_param_placeholder_type_t : public type_t
        {
            typedef type_t __super_t;

        public:
            virtual gtype_t this_gtype() override final { return gtype_t::__unknown__; }
            virtual ttype_t this_ttype() override final { return ttype_t::__unknown__; }
            virtual vtype_t this_vtype() override final { return vtype_t::__unknown__; }
            virtual mtype_t this_mtype() override final { return mtype_t::__unknown__; }

            virtual member_t * get_member(analyze_member_args_t & args) override
                                                                    { return nullptr; }
            virtual void get_members(analyze_members_args_t & args) override { }
            virtual void get_all_members(members_t & out_members) override { }
            virtual member_t * check_duplicate(member_t * member) override
                                                                    { return nullptr; }

            virtual const string_t to_full_name() const override { return (string_t)*this; }
            virtual const string_t to_short_name() const override { return (string_t)*this; }
            virtual name_t get_name() const override { return name_t::null; }
            virtual const string_t to_identity() const override { return _T("<placeholder>"); }

            operator string_t() const { return _T("__generlc_param_placeholder_type_t"); }
        };

        //-------- ---------- ---------- ---------- ----------

        std::map<name_t, namespace_t *>             __ns_map;
        std::map<type_t *, type_name_t *>           __type_name_map;
        std::map<decorate_value_t, decorate_t *>    __decorate_map;

        __list_t<assembly_t *>          __assembly_refs;        // Assembly refs
        __list_t<general_type_t *>      __type_refs;            // Type refs.
        __list_t<method_t *>            __method_refs;          // Method refs.
        __list_t<field_t *>             __field_refs;           // Field refs.
        __list_t<property_t *>          __property_refs;        // Property refs.
        __list_t<event_t *>             __event_refs;           // Event refs.

        __list_t<general_type_t *>      __general_types;        // General types.
        __list_t<generic_type_t *>      __generic_types;        // Generic types.
        __list_t<array_type_t *>        __array_types;          // Array types.
        __list_t<generic_method_t *>    __generic_methods;      // Generic methods.

        __list_t<type_t *>              __super_types;          // Super types.
        __list_t<general_type_t *>      __nest_types;           // Nest types.
        __list_t<method_t *>            __methods;              // Methods.
        __list_t<field_t *>             __fields;               // Fields.
        __list_t<property_t *>          __properties;           // Properties.
        __list_t<event_t *>             __events;               // Events.

        __list_t<type_def_t *>          __type_defs;            // Typedefs.
        __list_t<type_def_param_t *>    __type_def_params;      // Typedef params.

        __list_t<param_t *>             __params;               // Params.
        __list_t<generic_param_t *>     __generic_params;       // Generic params.
        __attribute_list_t              __attributes;           // Attributes.
        __list_t<argument_t *>          __attribute_arguments;  // Attribute arguments.

        __list_t<expression_t *>        __constants;            // Constants.

        std::set<attribute_t *>         __compile_time_attributes; 
                                                                // Compile time attributes.

        std::vector<generic_args_t *>   __generic_args_array;   // Generic argument array.
        std::vector<type_t *>           __generlc_param_placeholder_types; 
                                                                // Generic param placehoder.

        pool_t                          __pool;                 // Pool.

        __logger_t                      __logger;               // Logger.

        res_reader_t<al::spool_t>       __res_reader;           // Resourc reader.

        //-------- ---------- ---------- ---------- ----------

        // Returns metadata manager.
        template<__tidx_t tidx> mt_manager_t<tidx, __mt_t<tidx>> & __mt_manager()
        {
            return __super_t::template __mt_manager<tidx>();
        }

        // Creates a new object.
        template<typename t, typename ... args_t>
        t * __new_obj(args_t && ... args)
        {
            return __super_t::template __new_obj<t>(std::forward<args_t>(args) ...);
        }

        // Returns metadata of ref.
        template<__tidx_t tidx> mt_t<tidx> & __mt(ref_t ref)
        {
            return __mt_manager<tidx>().get(ref);
        }

        // Returns whether it's null or empty.
        bool __is_null_or_empty(sid_t sid)
        {
            return sid.is_null() || sid.is_empty();
        }

        // Gets sid_t of res_t.
        sid_t __to_sid(res_t res)
        {
            return __res_reader.read_sid(res);
        }

        // Gets name_t of res_t.
        name_t __to_name(res_t res)
        {
            return name_t(__to_sid(res));
        }

        // Returns mname of sid.
        const mname_t * __to_mname(sid_t sid)
        {
            if (__is_null_or_empty(sid))
                return nullptr;

            mname_operate_context_t mctx = to_mname_operate_context();
            return mname_t::parse(mctx, (string_t)sid);
        }

        // Returns mname of res.
        const mname_t * __to_mname(res_t res)
        {
            sid_t sid = __to_sid(res);
            return __to_mname(sid);
        }

        // Returns namespace_t of ns.
        namespace_t * __to_namespace(name_t name)
        {
            if (name == name_t::null)
                return nullptr;

            mname_operate_context_t ctx = to_mname_operate_context();
            const mname_t * mname = mname_t::to_mname(ctx, name);

            namespace_t * ns = __new_obj<namespace_t>();
            ns->full_name = mname;
            ns->name      = __new_obj<emname_t>(mname);

            const mname_t * parent_name = mname_t::get_parent(ctx, mname);
            if (parent_name != nullptr)
            {
                namespace_t * parent_ns = __to_namespace(name_t((sid_t)*parent_name));
                parent_ns->namespaces.push_back(ns);
            }

            return ns;
        }

        // Returns namespace_t of res.
        namespace_t * __to_namespace(res_t ns_res)
        {
            if (ns_res == res_t::null)
                return nullptr;

            name_t name = __to_name(ns_res);

            auto it = __ns_map.find(name);
            if (it != __ns_map.end())
                return it->second;

            namespace_t * ns = __to_namespace(name);
            __ns_map[name] = ns;

            return ns;
        }

        // Returns typename of type.
        type_name_t * __to_type_name(type_t * type)
        {
            if (type == nullptr)
                return nullptr;

            auto it = __type_name_map.find(type);
            if (it != __type_name_map.end())
                return it->second;

            type_name_t * type_name = __new_obj<type_name_t>(type);
            __type_name_map[type] = type_name;

            return type_name;
        }

        // Returns typename of ref.
        type_name_t * __to_type_name(ref_t ref)
        {
            return __to_type_name(__get_type(ref));
        }

        // Returns type of ref.
        type_t * __get_type(ref_t ref)
        {
            if (ref == ref_t::null)
                return nullptr;

            switch ((mt_type_extra_t)ref.extra)
            {
                case mt_type_extra_t::general:
                    __CheckRefLimit(ref, __general_types.size(), _T("general type"));
                    return __general_types[ref.index];

                case mt_type_extra_t::type_ref:
                    __CheckRefLimit(ref, __type_refs.size(), _T("type ref"));
                    return __type_refs[ref.index];

                case mt_type_extra_t::generic:
                    return __get_generic_type(ref);

                case mt_type_extra_t::array:
                    return __get_array_type(ref);

                case mt_type_extra_t::generic_param:
                    return __get_generic_param(ref);

                case mt_type_extra_t::type_def_param:
                    return __get_type_def_param(ref);

                default:
                    __Unexpected();
            }
        }

        // Returns generic type of ref.
        generic_type_t * __get_generic_type(ref_t ref)
        {
            __CheckRefLimit(ref, __generic_types.size(), _T("generic type"));

            generic_type_t * generic_type = __generic_types[ref.index];
            if (generic_type == nullptr)
            {
                auto & mgr = __mt_manager<__tidx_t::generic_type>();

                mt_generic_type_t & mt = mgr[ref];
                type_t * template_ = __get_type(mt.template_);

                _A(template_ != nullptr);
                _A(template_->this_gtype() == gtype_t::general);

                type_collection_t args;
                auto & arg_mgr = __mt_manager<__tidx_t::generic_argument>();

                ref_t arg_ref = mt.args;
                __CheckRefLimit(arg_ref, arg_mgr.count(), _T("generic argument"));

                arg_mgr.each(arg_ref, [this, &args](int index, mt_generic_argument_t & mt) {
                    return args.push_back(__get_type(mt.type)), true;
                });

                type_t * host_type = __get_type(mt.host);
                generic_type = __XPool.new_generic_type(
                            (general_type_t *)template_, args, host_type);

                __generic_types[ref.index] = generic_type;
            }

            return generic_type;
        }

        // Returns array type of ref.
        array_type_t * __get_array_type(ref_t ref)
        {
            __CheckRefLimit(ref, __array_types.size(), _T("array type"));

            array_type_t * array_type = __array_types[ref.index];
            if (array_type == nullptr)
            {
                auto & mgr = __mt_manager<__tidx_t::array_type>();

                mt_array_type_t & mt = mgr[ref];
                type_t * element_type = __get_type(mt.element_type);
                _A(element_type != nullptr);

                array_type = __XPool.new_array_type(element_type, mt.dimension);
                __array_types[ref.index] = array_type;
            }

            return array_type;
        }

        // Returns whether it's a general type.
        bool __is_general_type(ref_t ref)
        {
            return (mt_type_extra_t)ref.extra == mt_type_extra_t::general;
        }

        // Returns whether it's a compile time attribute.
        bool __is_compile_time_attribute(attribute_t * attribute)
        {
            return __compile_time_attributes.find(attribute) != __compile_time_attributes.end();
        }

        // Returns a constant type.
        cvalue_t __read_const_type(const mt_constant_t & mt)
        {
            ref_t ref = *(ref_t *)mt.data2;
            type_t * type = __get_type(ref);

            return to_cvalue(type);
        }

        // Returns a constant value.
        cvalue_t __new_cvalue(const mt_constant_t & mt)
        {
            cvalue_type_t cvalue_type = to_cvalue_type(mt.constant_type);
            if (cvalue_type == cvalue_type_t::type)
                return __read_const_type(mt);

            return __res_reader.read_const(mt);
        }

        //-------- ---------- ---------- ---------- ----------

        // Bind entities.
        void __build_entities()
        {
            #define __Bind(func)                                                        \
                std::bind(&__self_t::func, this, std::placeholders::_1, std::placeholders::_2)

            __set_placeholders_null<__tidx_t::generic_type>(__generic_types);
            __set_placeholders_null<__tidx_t::array_type>(__array_types);
            __set_placeholders_null<__tidx_t::generic_method>(__generic_methods);
            __set_placeholders<__tidx_t::type>(__general_types);

            __set_placeholders<__tidx_t::method>(__methods);
            __set_placeholders<__tidx_t::field>(__fields);
            __set_placeholders<__tidx_t::property>(__properties);
            __set_placeholders<__tidx_t::event>(__events);

            __set_placeholders<__tidx_t::type_def_param>(__type_def_params);
            __set_placeholders<__tidx_t::type_def>(__type_defs);

            __set_placeholders<__tidx_t::param>(__params);
            __set_placeholders<__tidx_t::generic_param>(__generic_params);
            __set_placeholders<__tidx_t::attribute>(__attributes);
            __set_placeholders<__tidx_t::attribute_argument>(__attribute_arguments);

            __import_assembly_refs();
            __import_type_refs();
            __import_field_refs();
            __import_method_refs();
            __import_property_refs();
            __import_event_refs();

            __build<__tidx_t::type>(__general_types, __Bind(__build_general_type));

            __load_constants();
            __load_super_types();
            __load_nest_types();

            __build<__tidx_t::type>(__general_types, __Bind(__fill_general_type_nest_types));
            __build<__tidx_t::attribute>(__attributes, __Bind(__build_attribute));
            __build<__tidx_t::method>(__methods, __Bind(__build_method));
            __build<__tidx_t::field>(__fields, __Bind(__build_field));
            __build<__tidx_t::property>(__properties, __Bind(__build_property));
            __build<__tidx_t::event>(__events, __Bind(__build_event));

            __build<__tidx_t::type_def>(__type_defs, __Bind(__build_type_def));
            __build<__tidx_t::type_def_param>(__type_def_params, __Bind(__build_type_def_param));

            __build<__tidx_t::param>(__params, __Bind(__build_param));
            __build<__tidx_t::generic_param>(__generic_params, __Bind(__build_generic_param));

            __build<__tidx_t::attribute_argument>(__attribute_arguments,
                                                __Bind(__build_attribute_argument));

            __build<__tidx_t::type>(__general_types, __Bind(__fill_general_type_members));

            __load_assembly();

            #undef __Bind

            __commit<__tidx_t::attribute>(__attributes);
            __commit_general_types();
            __XPool.commit_types(__logger);

            __commit_type_defs();
        }

        //-------- ---------- ---------- ---------- ----------
        // __get

        // Returns entity at specified ref.
        template<__tidx_t tidx, typename entity_t = typename mt_t<tidx>::entity_t>
        auto __get(__list_t<entity_t> & entities, ref_t ref, const char_t * name)
        {
            if (ref == ref_t::null)
                return (entity_t)nullptr;

            __CheckRefLimit(ref, entities.size(), name);
            return entities[ref.index];
        }

        // Returns method of specified ref.
        method_t * __get_method(ref_t ref)
        {
            return __get<__tidx_t::method>(__methods, ref, _T("method"));
        }

        // Returns generic param of specified ref.
        generic_param_t * __get_generic_param(ref_t ref)
        {
            return __get<__tidx_t::generic_param>(__generic_params, ref, _T("generic param"));
        }

        // Returns type def param of specified ref.
        type_def_param_t * __get_type_def_param(ref_t ref)
        {
            return __get<__tidx_t::type_def_param>(__type_def_params, ref, _T("typedef param"));
        }

        // Returns constant of specified ref.
        expression_t * __get_constant(ref_t ref)
        {
            return __get<__tidx_t::constant>(__constants, ref, _T("constant"));
        }

        // Returns assembly of specified ref.
        assembly_t * __get_assembly_ref(ref_t ref)
        {
            return __get<__tidx_t::assembly_ref>(__assembly_refs, ref, _T("assembly"));
        }

        //-------- ---------- ---------- ---------- ----------
        // __to_entities

        // Converts to entity collection.
        template<typename _entities_t, __tidx_t tidx, typename _list_t,
            typename entity_t = typename mt_t<tidx>::entity_t
        >
        _entities_t * __to_entities(_list_t & all_entities, ref_t refs, const char_t * name)
        {
            if (refs == ref_t::null)
                return nullptr;

            __CheckRefLimit(refs, all_entities.size(), name);
            _entities_t * entities = __new_obj<_entities_t>();

            auto & mgr = __mt_manager<tidx>();
            for (ref_t ref : refs)
            {
                entities->push_back(all_entities[ref.index]);
            }

            return entities;
        }

        // Convert to entity collection with owner.
        template<typename _entities_t, __tidx_t tidx, typename _list_t,
            typename entity_t = typename mt_t<tidx>::entity_t
        >
        _entities_t * __to_entities_with_owner(_list_t & all_entities, ref_t refs,
                                    const char_t * name, eobject_t * owner)
        {
            _entities_t * entities = __to_entities<_entities_t, tidx, _list_t, entity_t>(
                all_entities, refs, name
            );

            if (entities != nullptr)
                entities->set_owner(owner);

            return entities;
        }

        // Converts to generic params.
        generic_params_t * __to_generic_params(ref_t refs)
        {
            return __to_entities<generic_params_t, __tidx_t::generic_param>(
                __generic_params, refs, _T("generic param")
            );
        }

        // Converts to attribute arguments.
        arguments_t * __to_attribute_arguments(ref_t refs)
        {
            return __to_entities<arguments_t, __tidx_t::attribute_argument>(
                __attribute_arguments, refs, _T("attribute argument")
            );
        }

        // Converts to params.
        params_t * __to_params(ref_t refs)
        {
            return __to_entities<params_t, __tidx_t::param>(
                __params, refs, _T("param")
            );
        }

        // Converts to typedef params.
        type_def_params_t * __to_type_def_params(ref_t refs)
        {
            return __to_entities<type_def_params_t, __tidx_t::type_def_param>(
                __type_def_params, refs, _T("type def param")
            );
        }

        // Converts to attributes.
        attributes_t * __to_attributes(ref_t refs, eobject_t * owner)
        {
            return __to_entities_with_owner<attributes_t, __tidx_t::attribute>(
                __attributes, refs, _T("attribute"), owner
            );
        }

        // Converts to decorate.
        decorate_t * __to_decorate(__decorate_t decorate)
        {
            decorate_value_t value = (decorate_value_t)decorate;
            if (value == decorate_value_t::default_value)
                return nullptr;

            auto it = __decorate_map.find(value);
            if (it != __decorate_map.end())
                return it->second;

            decorate_t * d = __new_obj<decorate_t>(value);
            __decorate_map[value] = d;

            return d;
        }

        //-------- ---------- ---------- ---------- ----------
        // __import ...

        // Import assembly refs.
        void __import_assembly_refs()
        {
            for (mt_assembly_ref_t * mt : __mt_manager<__tidx_t::assembly_ref>())
            {
                const mname_t * package = __to_mname(mt->package);
                const mname_t * name    = __to_mname(mt->name);

                assembly_t * assembly;
                if (__assembly_loader == nullptr
                    || (assembly = __assembly_loader->load_assembly(package, name)) == nullptr)
                {
                    if (package != nullptr)
                        throw _ED(__e_t::assembly_load_error_in_package, name, package);
                    else
                        throw _ED(__e_t::assembly_load_error, name);
                }

                __assembly_refs.push_back(assembly);
            }
        }

        // Import type refs.
        void __import_type_refs()
        {
            __set_placeholders_null<__tidx_t::type_ref>(__type_refs);

            for (size_t index = 0, count = __type_refs.size(); index < count; index++)
            {
                __import_type_ref(index);
            }
        }

        // Import type ref.
        general_type_t * __import_type_ref(size_t index)
        {
            general_type_t * type = __type_refs[index];

            if (type == nullptr)
            {
                auto & mgr = __mt_manager<__tidx_t::type_ref>();
                mt_type_ref_t & mt = mgr[index];

                assembly_t * assembly = __get_assembly_ref(mt.assembly);
                if (assembly == nullptr)
                    throw _ED(__e_t::assembly_empty, mt.assembly);

                sid_t namespace_   = __to_sid(mt.namespace_);
                sid_t name         = __to_sid(mt.name);

                general_type_t * host_type = nullptr;
                if (mt.host != ref_t::null)
                {
                    if ((mt_type_extra_t)mt.host.extra != mt_type_extra_t::type_ref
                                                                || mt.host.count != 1)
                        throw _ED(__e_t::invalid_ref, _T("host type"));

                    host_type = __import_type_ref(mt.host.index);
                    if (host_type->assembly != assembly)
                        throw _ED(__e_t::invalid_host_type, host_type);
                }

                type = assembly->types.get_general_type(
                    namespace_, name, mt.generic_param_count, host_type
                );

                if (type == nullptr)
                {
                    if (!__is_null_or_empty(namespace_))
                        throw _ED(__e_t::type_load_error, _F(_T("%1%.%2%"), namespace_, name),
                                                                            assembly);
                    else
                        throw _ED(__e_t::type_load_error, name, assembly);
                }

                __type_refs[index] = type;
            }

            return type;
        }

        // Import field refs.
        void __import_field_refs()
        {
            for (mt_field_ref_t * mt : __mt_manager<__tidx_t::field_ref>())
            {
                
            }
        }

        // Gets generic param placeholder type.
        type_t * __get_generlc_param_placeholder_type(int index)
        {
            while (__generlc_param_placeholder_types.size() < index + 1)
            {
                __generlc_param_placeholder_types.push_back(nullptr);
            }

            if (__generlc_param_placeholder_types[index] == nullptr)
            {
                __generlc_param_placeholder_types[index] = 
                    __pool.template new_obj<__generlc_param_placeholder_type_t>();
            }

            return __generlc_param_placeholder_types[index];
        }

        #define __GenericParamType(index) (__get_generlc_param_placeholder_type(index))

        // Gets import method param type.
        type_t * __get_import_method_param_type(ref_t ref)
        {
            switch ((mt_type_extra_t)ref.extra)
            {
                case mt_type_extra_t::generic_param_index:
                    return __GenericParamType(ref.index);

                default:
                    return __get_type(ref);
            }
        }

        // Imports method refs.
        void __import_method_refs()
        {
            for (mt_method_ref_t * mt : __mt_manager<__tidx_t::method_ref>())
            {
                name_t name = __to_name(mt->name);

                type_t * host_type = __get_type(mt->host);
                if (host_type == nullptr)
                    throw _ED(__e_t::method_host_type_empty, name);

                atypes_t atypes;
                auto & m = __mt_manager<__tidx_t::method_ref_param>();

                for (ref_t ref : mt->params)
                {
                    mt_method_ref_param_t & p = m[ref];
                    type_t * arg_type = __get_import_method_param_type(p.type);
                    atypes.push_back(atype_t { arg_type, p.ptype });
                }

                generic_args_t * generic_args = __to_generic_args(mt->generic_param_count);
                analyze_members_args_t args(
                    member_type_t::method, name, &atypes, generic_args
                );

                args.exact_match = true;
                eobject_commit_context_t ctx(__logger);
                host_type->commit(ctx);

                method_t * method = (method_t *)host_type->get_member(args);
                if (method == nullptr)
                    throw _ED(__e_t::method_not_found, name);

                type_t * ret_type = __get_import_method_param_type(mt->type);
                if (to_type(method->type_name) != ret_type)
                {
                    throw _ED(__e_t::method_return_type_not_matched, name);
                }
                else
                {
                    if (ret_type == nullptr && method->trait == method_trait_t::normal)
                        throw _ED(__e_t::method_return_type_empty, name);
                }

                __method_refs.push_back(method);
            }
        }

        // Creates a generic arguments of specified count.
        generic_args_t * __to_generic_args(int args_count)
        {
            if (args_count == 0)
                return nullptr;

            // _A(args_count < );  TODO

            while (__generic_args_array.size() < args_count)
            {
                __generic_args_array.push_back(nullptr);
            }

            if (__generic_args_array[args_count - 1] == nullptr)
                __generic_args_array[args_count - 1] = __build_generic_args(args_count);

            return __generic_args_array[args_count - 1];
        }

        // Builds generic arguments of specified count.
        generic_args_t * __build_generic_args(int args_count)
        {
            generic_args_t * args = __pool.template new_obj<generic_args_t>();
            for (int index = 0; index < args_count; index++)
            {
                generic_arg_t * arg = __pool.template new_obj<generic_arg_t>();
                arg->type_name = __to_type_name(__GenericParamType(index));
                args->push_back(arg);
            }

            return args;
        }

        // Import property refs.
        void __import_property_refs()
        {
            for (mt_property_ref_t * mt : __mt_manager<__tidx_t::property_ref>())
            {
                
            }
        }

        // Import event refs.
        void __import_event_refs()
        {
            for (mt_event_ref_t * mt : __mt_manager<__tidx_t::event_ref>())
            {
                
            }
        }

        //-------- ---------- ---------- ---------- ----------
        // __load ...

        // Load super types.
        void __load_super_types()
        {
            auto & mgr = __mt_manager<__tidx_t::super_type>();
            for (mt_super_type_t * mt : mgr)
            {
                __super_types.push_back(__get_type(mt->type));
            }
        }

        // Load nest types.
        void __load_nest_types()
        {
            auto & mgr = __mt_manager<__tidx_t::nest_type>();
            for (mt_nest_type_t * mt : mgr)
            {
                if (!__is_general_type(mt->type))
                    throw _ED(__e_t::read_unexpected_nest_type);

                type_t * type = __get_type(mt->type);
                if (!is_general(type))
                    throw _ED(__e_t::read_unexpected_nest_type);

                __nest_types.push_back((general_type_t *)type);
            }
        }

        // Load constants.
        void __load_constants()
        {
            std::map<cvalue_t, expression_t *> map;

            auto & mgr = __mt_manager<__tidx_t::constant>();
            for (mt_constant_t * mt : mgr)
            {
                cvalue_t cvalue = __new_cvalue(*mt);
                auto it = map.find(cvalue);
                if (it != map.end())
                {
                    __constants.push_back(it->second);
                }
                else
                {
                    expression_t * exp = __new_obj<cvalue_expression_t>(
                        __new_obj<cvalue_t>(cvalue)
                    );

                    map[cvalue] = exp;
                    __constants.push_back(exp);
                }
            }
        }

        // Returns assembly metadata.
        mt_assembly_t & __mt_assembly()
        {
            auto & mgr = __mt_manager<__tidx_t::assembly>();

            if (mgr.count() == 0)
                throw __format_error(_T("no assembly information"));

            if (mgr.count() >= 2)
                throw __format_error(_T("multiply assembly information"));

            mt_assembly_t & mt_assembly = mgr[0];
            return mt_assembly;
        }

        // Loads assembly.
        void __load_assembly()
        {
            mt_assembly_t & mt_assembly = __mt_assembly();
            const mname_t * mname0 = __to_mname(mt_assembly.name);

            if (__assembly.name != nullptr && __assembly.name != __to_mname(mt_assembly.name))
                throw __format_error(_T("assembly name not matched"));

            __assembly.name        = __to_mname(mt_assembly.name);
            __assembly.version     = mt_assembly.version;
            __assembly.information = __to_sid(mt_assembly.info);

            __assembly.entry_point = __get_method(mt_assembly.entry_point);
        }

        // Load assembly brief.
        void __load_assembly_brief()
        {
            mt_assembly_t & mt_assembly = __mt_assembly();

            if (mt_assembly.entry_point != ref_t::null)
                __assembly.entry_point = __FakeEntryPoint;
        }

        //-------- ---------- ---------- ---------- ----------
        // __set_placeholders && __fill ...

        // Sets placeholders.
        template<__tidx_t tidx, typename entity_t = typename mt_t<tidx>::entity_t>
        void __set_placeholders(std::vector<entity_t> & entities)
        {
            for (int k = 0, count = __mt_manager<tidx>().row_count(); k < count; k++)
            {
                typedef std::remove_pointer_t<entity_t> _entity_t;
                entities.push_back(__new_obj<_entity_t>());
            }
        }

        // Sets placeholders to nullptr.
        template<__tidx_t tidx, typename entity_t = typename mt_t<tidx>::entity_t>
        void __set_placeholders_null(std::vector<entity_t> & entities)
        {
            for (int k = 0, count = __mt_manager<tidx>().row_count(); k < count; k++)
            {
                entities.push_back(nullptr);
            }
        }

        // Fills entities.
        template<__tidx_t tidx, typename _list_t, typename entity_t = typename mt_t<tidx>::entity_t>
        void __fill_entities(_list_t & entities, __list_t<entity_t> & all_entities,
                                                    ref_t refs, const char_t * name)
        {
            if (refs == ref_t::null)
                return;

            __CheckRefLimit(refs, all_entities.size(), name);
            for (ref_t ref : refs)
            {
                entities.push_back(all_entities[ref.index]);
            }
        }

        // Fills members.
        template<__tidx_t tidx, typename _list_t, typename entity_t = typename mt_t<tidx>::entity_t>
        void __fill_members(type_t * host_type, _list_t & members,
                __list_t<entity_t> & all_members, ref_t refs, const char_t * name)
        {
            __fill_entities<tidx>(members, all_members, refs, name);
            for (auto && member : members)
            {
                member->host_type = host_type;
            }
        }

        // Fills type names.
        template<__tidx_t tidx, typename _list_t>
        void __fill_type_names(__list_t<type_t *> & all_types, _list_t & type_names, ref_t refs,
                                                                        const char_t * name)
        {
            if (refs == ref_t::null)
                return;

            __CheckRefLimit(refs, all_types.size(), name);

            auto & ref_array = __mt_manager<tidx>();
            for (ref_t ref : refs)
            {
                type_names.push_back(
                    __to_type_name(all_types[ref.index])
                );
            }
        }

        //-------- ---------- ---------- ---------- ----------
        // __build ...

        // Build entities.
        template<__tidx_t tidx, typename f_t, typename entity_t = typename mt_t<tidx>::entity_t>
        void __build(__list_t<entity_t> & all_entities, f_t f)
        {
            auto & mgr = __mt_manager<tidx>();
            for (auto && it : al::zip(mgr, all_entities))
            {
                mt_t<tidx> * mt;
                entity_t entity;
                al::tie(mt, entity) = it;

                f(*mt, entity);
            }
        }

        // Build general type.
        void __build_general_type(mt_type_t & mt_type, general_type_t * type)
        {
            type->name       = __to_name(mt_type.name);
            type->decorate   = __to_decorate(mt_type.decorate);
            type->ttype      = (ttype_t)mt_type.ttype;
            type->vtype      = (vtype_t)mt_type.vtype;
            type->params     = __to_generic_params(mt_type.generic_params);
            type->assembly   = &__assembly;
            type->namespace_ = __to_namespace(mt_type.namespace_);
        }

        // Fill nest types of general type.
        void __fill_general_type_nest_types(mt_type_t & mt_type, general_type_t * type)
        {
            __fill_members<__tidx_t::type>(type, type->nest_types, __nest_types,
                                                mt_type.nest_types, _T("nest type"));
        }

        // Fill members of general type.
        void __fill_general_type_members(mt_type_t & mt_type, general_type_t * type)
        {
            __fill_members<__tidx_t::method>(type, type->methods, __methods,
                                                mt_type.methods, _T("method"));

            __fill_members<__tidx_t::property>(type, type->properties, __properties,
                                                mt_type.properties, _T("property"));

            __fill_members<__tidx_t::field>(type, type->fields, __fields,
                                                mt_type.fields, _T("field"));

            __fill_members<__tidx_t::event>(type, type->events, __events,
                                                mt_type.events, _T("event"));

            __fill_members<__tidx_t::type_def>(type, type->type_defs, __type_defs,
                                                mt_type.type_defs, _T("typedef"));

            __fill_type_names<__tidx_t::super_type>(__super_types, type->super_type_names,
                                                mt_type.super_types, _T("super type"));

            type->attributes = __to_attributes(mt_type.attributes, type);
        }

        // Builds array type.
        void __build_array_type(mt_array_type_t & mt, array_type_t * type)
        {
            type->element_type = __get_type(mt.element_type);
            type->dimension    = mt.dimension;
        }

        // Builds filed.
        void __build_field(mt_field_t & mt, field_t * field)
        {
            field->name             = __to_name(mt.name);
            field->decorate         = __to_decorate(mt.decorate);

            field->type_name        = __to_type_name(mt.type);
            field->attributes       = __to_attributes(mt.attributes, field);
            field->init_value       = __get_constant(mt.init_value);

            field->variable         = __new_obj<field_variable_t>(field);
        }

        // Builds method.
        void __build_method(mt_method_t & mt, method_t * method)
        {
            method->name            = __to_name(mt.name);
            method->decorate        = __to_decorate(mt.decorate);

            method->type_name       = __to_type_name(mt.type);
            method->generic_params  = __to_generic_params(mt.generic_params);
            method->params          = __to_params(mt.params);
            method->trait           = mt.trait;

            method->variable        = __new_obj<method_variable_t>(method);
        }

        // Builds property.
        void __build_property(mt_property_t & mt, property_t * property)
        {
            property->name          = __to_name(mt.name);
            property->decorate      = __to_decorate(mt.decorate);

            property->type_name     = __to_type_name(mt.type);
            property->attributes    = __to_attributes(mt.attributes, property);
            property->params        = __to_params(mt.params);

            property->get_method    = __get_method(mt.get_method);
            property->set_method    = __get_method(mt.set_method);

            property->variable      = __new_obj<property_variable_t>(property);
        }

        // Builds event.
        void __build_event(mt_event_t & mt, event_t * event)
        {
            event->name             = __to_name(mt.name);
            event->decorate         = __to_decorate(mt.decorate);

            event->type_name        = __to_type_name(mt.type);
            event->attributes       = __to_attributes(mt.attributes, event);

            event->add_method       = __get_method(mt.add_method);
            event->remove_method    = __get_method(mt.remove_method);

            event->variable         = __new_obj<event_variable_t>(event);
        }

        // Builds typedef.
        void __build_type_def(mt_type_def_t & mt, type_def_t * type_def)
        {
            type_def->name          = __to_name(mt.name);
            type_def->decorate      = __to_decorate(mt.decorate);

            type_def->type_name     = __to_type_name(mt.type);
            type_def->params        = __to_type_def_params(mt.params);
            type_def->namespace_    = __to_namespace(mt.namespace_);
        }

        // Builds typedef param.
        void __build_type_def_param(mt_type_def_param_t & mt, type_def_param_t * type_def_param)
        {
            type_def_param->name        = __to_name(mt.name);
            type_def_param->attributes  = __to_attributes(mt.attributes, type_def_param);
        }

        // Builds param.
        void __build_param(mt_param_t & mt, param_t * param)
        {
            param->name             = __to_name(mt.name);
            param->attributes       = __to_attributes(mt.attributes, param);
            param->type_name        = __to_type_name(mt.type);
            param->ptype            = mt.param_type;
            param->default_value    = __get_constant(mt.default_value);
        }

        // Builds attribute.
        void __build_attribute(mt_attribute_t & mt, attribute_t * attribute)
        {
            attribute->type_name   = __to_type_name(mt.type);
            attribute->constructor = __get_method(mt.constructor);
            attribute->arguments   = __to_attribute_arguments(mt.arguments);

            if (mt.flag.compile_time)
                __compile_time_attributes.insert(attribute);
        }

        // Builds attribute argument.
        void __build_attribute_argument(mt_attribute_argument_t & mt, argument_t * argument)
        {
            argument->name          = __to_name(mt.name);
            argument->expression    = __get_constant(mt.value);
            argument->name_type     = mt.name_type;
        }

        // Builds generic param.
        void __build_generic_param(mt_generic_param_t & mt, generic_param_t * generic_param)
        {
            generic_param->name       = __to_name(mt.name);
            generic_param->attributes = __to_attributes(mt.attributes, generic_param);
            generic_param->param_type = mt.param_type;
        }

        //-------- ---------- ---------- ---------- ----------
        // commit

        // Commits entities.
        template<__tidx_t tidx, typename entity_t = typename mt_t<tidx>::entity_t>
        void __commit(__list_t<entity_t> & entities)
        {
            eobject_commit_context_t ctx(__logger);
            for (entity_t entity : entities)
            {
                entity->commit(ctx);
            }
        }

        // Commits general types.
        void __commit_general_types()
        {
            for (general_type_t * type : __general_types)
            {
                __XPool.append_new_type(type);
            }

            eobject_commit_context_t ctx(__logger);

            for (general_type_t * type : __general_types)
            {
                if (type->namespace_ != nullptr)
                    type->namespace_->append_type(type);

                this->__assembly.types.append_type(type);
                type->commit(ctx);
            }
        }

        // Converts namespace to a sid.
        sid_t __ns_to_sid(namespace_t * ns)
        {
            if (ns != nullptr)
            {
                const mname_t * full_name = ns->full_name;
                if (full_name != nullptr)
                {
                    return full_name->sid;
                }
            }

            return sid_t::null;
        }

        // Commits type defs.
        void __commit_type_defs()
        {
            for (type_def_t * type_def : __type_defs)
            {
                if (type_def->namespace_ != nullptr)
                    type_def->namespace_->type_defs.push_back(type_def);

                this->__assembly.types.append_type_def(
                    __ns_to_sid(type_def->namespace_), type_def
                );
            }
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // layout 1

    // Base class of assembly reader.
    template<typename reader_t, __lv_t rlv>
    class __assembly_reader_super_t<reader_t, 1, rlv>
            : public __assembly_reader_super_t<reader_t, 0, rlv>
    {
        typedef __assembly_reader_super_t<reader_t, 0, rlv> __super_t;

    public:
        using __super_t::__super_t;
    };

    ////////// ////////// ////////// ////////// //////////
    // __assembly_reader_t

    // Assembly reader.
    template<__lv_t lv>
    class __assembly_reader_t : public __assembly_reader_super_t<__assembly_reader_t<lv>, lv>
    {
        typedef __assembly_reader_super_t<__assembly_reader_t<lv>, lv> __super_t;

    public:
        using __super_t::__super_t;
    };

    ////////// ////////// ////////// ////////// //////////

    // Read guid of assembly.
    void assembly_read_guide(xistream_t & stream, mt_guide_t * guide)
    {
        __entity_operation_t<mt_guide_t>::read(stream, *guide);

        if (guide->flag != guid_t::parse(JC_ASSEMBLY_FLAG))
            throw __format_error(_T("unexpected assembly flag"));
    }

    // Read assembly.
    void assembly_read(xistream_t & stream, assembly_t & assembly,
                assembly_loader_t * loader, __lv_t * out_lv, bool brief)
    {
        mt_guide_t guide;
        assembly_read_guide(stream, &guide);

        switch (guide.layout)
        {
            case 1:
                __assembly_reader_t<1>(stream, assembly, loader).read(brief);
                break;

            default:
                throw _ED(__e_t::unsupported_layout, guide.layout);
        }
    }

    // Gets assembly type.
    assembly_type_t get_assembly_type(xistream_t & stream)
    {
        assembly_t assembly(nullptr);

        try
        {
            assembly_read(stream, assembly, nullptr, nullptr, true);
            if (assembly.entry_point != nullptr)
                return assembly_type_t::executable;

            return assembly_type_t::dynamic_library;
        }
        catch (logic_error_t<__e_t> & e)
        {
            if (e.code == __e_t::unsupported_layout)
                return assembly_type_t::unknown;

            throw;
        }
    }

    // Gets assembly type.
    assembly_type_t get_assembly_type(const string_t & path)
    {
        file_xistream_t stream;
        stream.open(path);

        X_TRY_R
            return get_assembly_type(stream);
        X_FINALLY
            stream.completed();
        X_TRY_END
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::core
