#ifndef __RT_H__
#define __RT_H__

#include <core.h>
#include <algorithm.h>

namespace X_ROOT_NS::modules::rt {

    ////////// ////////// ////////// ////////// //////////

    class rt_type_t;
    class rt_array_type_t;
    class rt_method_t;
    class rt_assembly_t;
    class rt_general_type_t;
    class rt_generic_method_t;

    class rt_field_t;
    class rt_field_base_t;
    class rt_tuple_field_t;

    class analyzer_env_t;
    class assembly_analyzer_t;
    class generic_param_manager_t;
    class generic_param_manager_builder_t;

    X_INTERFACE rt_heap_t;

    typedef int32_t rt_length_t;

    //-------- ---------- ---------- ---------- ----------

    namespace
    {
        using namespace core;

        typedef generic_param_manager_t __gp_mgr_t;
    }

    ////////// ////////// ////////// ////////// //////////

    // Runtime error codes.
    X_ENUM(rt_error_code_t)

        // Assembly not found.
        assembly_not_found,

        // Assembly format error.
        assembly_error,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Storage types.
    X_ENUM(storage_type_t)

        // Reference type
        ref = __default__,
        
        // Value type
        value,
        
        // Mixture types.
        mixture,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////
    // rt_assembly_provider_t

    class rt_context_t;

    // Assembly provider.
    X_INTERFACE rt_assembly_provider_t
    {
        // Loads main assembly.
        virtual rt_assembly_t * load_main_assembly(rt_context_t & ctx) = 0;

        // Loads assembly.
        virtual rt_assembly_t * load_assembly(rt_context_t & ctx, const string_t & package,
                                                                  const string_t & name) = 0;
    };

    //-------- ---------- ---------- ---------- ----------
    // rt_assemblies_t

    // Rt assembly collection.
    class rt_assemblies_t : public object_t, public no_copy_ctor_t
    {
    public:

        // Constructor.
        rt_assemblies_t(rt_context_t & ctx, rt_assembly_provider_t * provider)
            : __provider(provider), __ctx(ctx)
        {
            _A(provider != nullptr);
        }

        // Load main assembly.
        rt_assembly_t * load_main_assembly();

        // Load assembly.
        rt_assembly_t * load_assembly(const string_t & package, const string_t & name);

        // Returns assembly at index.
        rt_assembly_t * at(int index);

        X_TO_STRING_IMPL(_T("rt_assemblies_t"))

    private:
        rt_context_t & __ctx;
        typedef std::tuple<string_t, string_t> __assembly_key_t;
        rt_assembly_t * __main_assembly = nullptr;

        rt_assembly_provider_t * __provider;
        std::map<__assembly_key_t, rt_assembly_t *> __assembly_map;
        std::vector<rt_assembly_t *> __assembly_vector;

        void __revise_assembly(rt_assembly_t * assembly);
    };

    ////////// ////////// ////////// ////////// //////////

    struct rt_method_t;

    // Virtual table function.
    struct rt_vtable_function_t
    {
        typedef rt_vtable_function_t __self_t;

        // Constructor.
        rt_vtable_function_t() = default;

        // Constructor.
        rt_vtable_function_t(uint32_t assembly_idx, uint32_t method_idx) _NE
        {
            this->assembly_idx = assembly_idx;
            this->method_idx   = method_idx;

            this->__uninitialized = true;
        }

        // Constructor.
        rt_vtable_function_t(void * method) _NE
        {
            this->method = method;
        }

        // Returns whether it's a method.
        X_ALWAYS_INLINE bool initialized() const _NE
        {
            return !this->__uninitialized;
        }

        union
        {
            struct
            {
                bool        __uninitialized     : 1;
                uint32_t    assembly_idx        : 11;
                uint32_t    method_idx          : 20;
            };

            struct
            {
                void * method;
            };
        };
    };

    typedef rt_vtable_function_t rt_vfunction_t;

    //-------- ---------- ---------- ---------- ----------

    // Virtual table interface item.
    struct rt_vtable_interface_t
    {
        rt_type_t *         interface_type;
        rt_vfunction_t *    functions;
    };

    // Virtual table interfaces.
    struct rt_vtable_interfaces_t
    {
        rt_vtable_interface_t * recently;
        rt_vtable_interface_t   interfaces[0];
    };

    // Virtual table.
    struct rt_vtable_t
    {
        rt_type_t             ** base_types;
        rt_vtable_interfaces_t * interfaces;
        msize_t                  base_type_count;
        msize_t                  interface_count;

        rt_vfunction_t  functions[0];

        // rt_type_t * base_types;
        // rt_vtable_interfaces_t interfaces;
        // rt_vfunction_t interface_functions[...];

        // Search interface of specified type.
        X_ALWAYS_INLINE rt_vtable_interface_t * search_vtable_interface(rt_type_t * interface_type)
        {
            _A(interface_type != nullptr);

            #if X_DEBUG

            rt_vtable_interface_t * interface = __search_vtable_interface<true>(interface_type);
            if (interface != nullptr)
                return interface;

            X_UNEXPECTED(_T("cannot find specified interface type"));

            #else

            return __search_vtable_interface<false>(interface_type);

            #endif
        }

        // Try to search interface of specified type, returns null if not exists.
        X_ALWAYS_INLINE rt_vtable_interface_t * try_search_vtable_interface(
                                                                rt_type_t * interface_type)
        {
            _A(interface_type != nullptr);

            return __search_vtable_interface<true>(interface_type);
        }

        // Search function address of interface methods.
        X_ALWAYS_INLINE rt_vfunction_t search_vtable_interface_function(
                                        rt_type_t * interface_type, int offset)
        {
            _A(interface_type != nullptr);
            _A(offset >= 0);

            return search_vtable_interface(interface_type)->functions[offset];
        }

        // Returns true if specified type is a base type.
        X_ALWAYS_INLINE bool is_base_type(rt_type_t * base_type)
        {
            _A(base_type != nullptr);

            for (rt_type_t ** p = base_types, ** p_end = p + base_type_count; p < p_end; ++p)
            {
                if (base_type == *p)
                    return true;
            }

            return false;
        }

    private:

        // Search interface of specified type.
        template<bool _check_none>
        X_ALWAYS_INLINE rt_vtable_interface_t * __search_vtable_interface(
                                                                rt_type_t * interface_type)
        {
            rt_vtable_interface_t * recently = interfaces->recently;
            if (recently->interface_type == interface_type)
                return recently;

            rt_vtable_interface_t * vtable_interface = interfaces->interfaces;
            rt_vtable_interface_t * vtable_interface_end;

            if constexpr (_check_none)
                vtable_interface_end = vtable_interface + interface_count;

            for ( ; ; )
            {
                if (vtable_interface->interface_type == interface_type)
                {
                    interfaces->recently = vtable_interface;
                    return vtable_interface;
                }

                ++vtable_interface;

                if constexpr (_check_none)
                {
                    if (vtable_interface >= vtable_interface_end)
                        break;
                }
            }

            return nullptr;
        }

    };

    //-------- ---------- ---------- ---------- ----------

    // Rt memory managerment.
    class rt_memory_t : public object_t
    {
    public:

        // Returns new members.
        member_t ** new_members(size_t size)
        {
            return __acquire<member_t>(size);
        }

        X_TO_STRING_IMPL(_T("rt_memory_t"))

    private:
        al::heap_t<void *[]> __heap;

        // Acquire objects of specified size.
        template<typename t> t ** __acquire(size_t size)
        {
            return (t **)__heap.acquire(size);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Rt heap.
    X_INTERFACE rt_heap_t
    {
        // Creates a new object.
        virtual rt_ref_t new_obj(rt_type_t * type) = 0;

        // Creates a new array.
        virtual rt_ref_t new_array(rt_array_type_t * type, const array_length_t * lengths) = 0;

        // allocate a memory space.
        virtual void * alloc(size_t size, void * realloc = nullptr) = 0;

        // free a memory space.
        virtual void free(void * ptr) = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    // Rt object.
    class rt_object_t
    {
        // Empty.
    };
    
    //-------- ---------- ---------- ---------- ----------

    typedef int32_t rt_size_t;

    ////////// ////////// ////////// ////////// //////////

    // Rt string.
    class rt_string_t : public rt_object_t
    {
    public:

        // Constructor.
        rt_string_t(const char_t * s, rt_size_t length)
            : s(s), length(length)
        {
            _A(s != nullptr);
        }

        rt_size_t length = 0;
        const char_t * s = nullptr;

        // Compare operators.

        bool operator <  (const rt_string_t & str) const { return compare(str) <  0; }
        bool operator >  (const rt_string_t & str) const { return compare(str) >  0; }
        bool operator <= (const rt_string_t & str) const { return compare(str) <= 0; }
        bool operator >= (const rt_string_t & str) const { return compare(str) >= 0; }
        bool operator == (const rt_string_t & str) const { return equals(str);  }
        bool operator != (const rt_string_t & str) const { return !equals(str); }

        // Compare two strings.
        int  compare(const rt_string_t & str) const;

        // Return whether two strings are equals.
        bool equals(const rt_string_t & str) const;

        // Converts to string.
        operator string_t() const
        {
            return !s? _T("") : s;
        }

    private:

        // Returns whether two strings are equals.
        bool __reference_equals(const rt_string_t & str) const;

        // Compare two strings.
        int __compare(const rt_string_t & str) const;
    };

    //-------- ---------- ---------- ---------- ----------

    // Sid.
    struct rt_sid_t : compare_operators_t<rt_sid_t, const rt_string_t *>
    {
        typedef const rt_string_t * __sid_value_t;

        // Constructor.
        rt_sid_t() = default;
        rt_sid_t(__sid_value_t value) : value(value) { }

        __sid_value_t value = nullptr;

        // Returns Sid value.
        operator __sid_value_t () const { return value; }

        // Converts to string.
        operator string_t() const { return !value? _T("") : (string_t)*value; }

        // Converts to const char_t *.
        const char_t * c_str() const { return value? value->s : nullptr; }

        // Converts to const char_t *, returns empty string when nullptr.
        const char_t * ce_str() const { return value && value->s? value->s : _T(""); }

        // Returns c_str or default_str when nullptr.
        const char_t * operator | (const char_t * default_str) const
        {
            return empty()? default_str : c_str();
        }

        // Returns whether it's empty.
        bool empty() const { return value == nullptr; }

        static const rt_sid_t null;
    };

    //-------- ---------- ---------- ---------- ----------

    // Rt String Pool.
    class rt_spool_t : public object_t
    {
    public:
        typedef rt_sid_t sid_t;

        // Converts to string id.
        rt_sid_t to_sid(const rt_string_t & s);

        // Converts to string id.
        rt_sid_t to_sid(const string_t & s);

        X_TO_STRING_IMPL(_T("rt_spool_t"))

    private:
        std::set<const rt_string_t> __string_set;
        al::heap_t<char_t[]> __heap;
    };

    ////////// ////////// ////////// ////////// //////////

    class __unknown_entity_t { };

    template<__tidx_t tidx> struct __rt_t
    {
        typedef __unknown_entity_t entity_t;
    };

    template<typename _entity_t> struct __rt_tidx_t
    {
        static const __tidx_t tidx = (__tidx_t)0;
    };

    #define __RtEntity(_tidx, _entity_t)                                                \
        class _entity_t;                                                                \
        template<> struct __rt_t<__tidx_t::_tidx>                                       \
        {                                                                               \
            typedef _entity_t entity_t;                                                 \
        };                                                                              \
                                                                                        \
        template<> struct __rt_tidx_t<_entity_t>                                        \
        {                                                                               \
            static const __tidx_t tidx = __tidx_t::_tidx;                               \
        };

    __RtEntity(assembly,            rt_assembly_t)
    __RtEntity(assembly_ref,        rt_assembly_ref_t)
    __RtEntity(type,                rt_general_type_t)
    __RtEntity(generic_type,        rt_generic_type_t)
    __RtEntity(generic_method,      rt_generic_method_t)
    __RtEntity(generic_field,       rt_generic_field_t)
    __RtEntity(generic_argument,    rt_generic_argument_t)
    __RtEntity(position_field,      rt_position_field_t)
    __RtEntity(array_type,          rt_array_type_t)
    __RtEntity(field,               rt_field_t)
    __RtEntity(method,              rt_method_t)
    __RtEntity(property,            rt_property_t)
    __RtEntity(event,               rt_event_t)
    __RtEntity(super_type,          rt_super_type_t)
    __RtEntity(nest_type,           rt_nest_type_t)
    __RtEntity(field_ref,           rt_field_ref_t)
    __RtEntity(method_ref,          rt_method_ref_t)
    __RtEntity(property_ref,        rt_property_ref_t)
    __RtEntity(event_ref,           rt_event_ref_t)
    __RtEntity(type_ref,            rt_type_ref_t)
    __RtEntity(generic_param,       rt_generic_param_t)
    __RtEntity(param,               rt_param_t)
    __RtEntity(attribute,           rt_attribute_t)
    __RtEntity(attribute_argument,  rt_attribute_argument_t)
    __RtEntity(method_ref_param,    rt_method_ref_param_t)

    template<__tidx_t _tidx>
    using rt_entity_t = typename __rt_t<_tidx>::entity_t;

    template<typename _entity_t> constexpr __tidx_t rt_tidx()
    {
        return __rt_tidx_t<_entity_t>::tidx;
    }

    ////////// ////////// ////////// ////////// //////////

    // Pool key type.
    X_ENUM(rpool_key_type_t)

        // Method.
        method,

        // Field.
        field,

        // Generic method.
        generic_method,

        // Array type.
        array_type,

        // Generic type.
        generic_type,

        // Generic field.
        generic_field,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------


    namespace
    {
        template<rpool_key_type_t _key_type> struct __rpool_key_t { };

        //-------- ---------- ---------- ---------- ----------

        // Rt pool partial.
        template<typename _key_t, typename _value_t>
        class __rt_pool_partial_t
        {
            typedef _key_t      __key_t;
            typedef _value_t    __value_t;
            typedef __rt_pool_partial_t<_key_t, _value_t> __self_t;

        public:

            // Returns value of key.
            __value_t * get(const __key_t & key)
            {
                auto it = __map.find(key);
                if (it == __map.end())
                    return nullptr;

                return &it->second;
            }

            // Returns value of key.
            // Auto creates by specified function.
            template<typename creator_t>
            __value_t get(__key_t & key, creator_t f)
            {
                __value_t * value = get(key);
                if (value != nullptr)
                    return *value;

                __value_t new_value = f();
                __map[key] = new_value;

                return new_value;
            }

            // Sets value of key.
            void set(const __key_t & key, _value_t && value)
            {
                __map[key] = value;
            }

        private:
            std::map<__key_t, __value_t> __map;
        };

        //-------- ---------- ---------- ---------- ----------

        // Base class of rt pool key.
        template<rpool_key_type_t _key_type, typename _value_t, typename ... _args_t>
        struct __rpool_key_base_t : std::tuple<_args_t ...>
        {
            typedef __rpool_key_base_t      __self_t;
            typedef std::tuple<_args_t ...> __super_t;

            using __super_t::__super_t;

            // Constructor.
            __rpool_key_base_t() : __super_t(_D(_args_t) ...) { }

            static const rpool_key_type_t key_type = _key_type;
            typedef __rt_pool_partial_t<__self_t, _value_t> partial_t;

            // Assigns data.
            template<typename ... _keys_t>
            void assign(_keys_t && ... keys)
            {
                __super_t::operator = (__super_t(std::forward<_keys_t>(keys) ...));
            }
        };

        //-------- ---------- ---------- ---------- ----------

        // Base class of rt method pool key.
        typedef __rpool_key_base_t<rpool_key_type_t::method,
            rt_method_t *, rt_assembly_t *, ref_t
        > __rpool_key_method_base_t;

        // Rt pool method key.
        template<>
        struct __rpool_key_t<rpool_key_type_t::method> : __rpool_key_method_base_t
        {
            using __rpool_key_method_base_t::__rpool_key_method_base_t;
        };

        // - - - - - - - - - - - - - - - - - - - - - - - - - -

        // Base class of rt field pool key.
        typedef __rpool_key_base_t<rpool_key_type_t::field,
            std::tuple<rt_field_base_t *, rt_type_t *>, rt_assembly_t *, ref_t
        > __rpool_key_field_base_t;

        // Rt pool field key.
        template<>
        struct __rpool_key_t<rpool_key_type_t::field> : __rpool_key_field_base_t
        {
            using __rpool_key_field_base_t::__rpool_key_field_base_t;
        };

        // - - - - - - - - - - - - - - - - - - - - - - - - - -

        typedef al::multikey_t<range_t<rt_type_t **>> __rt_generic_args_key_t;

        // Base class of rt generic method pool key.
        typedef __rpool_key_base_t<rpool_key_type_t::generic_method,
            rt_generic_method_t *, rt_method_t *, rt_type_t *, __rt_generic_args_key_t
        > __rpool_key_generic_method_base_t;

        // Rt pool generic method key.
        template<>
        struct __rpool_key_t<rpool_key_type_t::generic_method>
            : __rpool_key_generic_method_base_t
        {
            using __rpool_key_generic_method_base_t::__rpool_key_generic_method_base_t;

            __rt_generic_args_key_t args_key() const
            {
                return std::get<__rt_generic_args_key_t>(*this);
            }

            rt_type_t ** types() const { return args_key().begin(); }
            rt_method_t * template_() const { return std::get<rt_method_t *>(*this); }
            rt_type_t * host_type() const { return std::get<rt_type_t *>(*this); }
        };

        // - - - - - - - - - - - - - - - - - - - - - - - - - -

        typedef __rpool_key_base_t<rpool_key_type_t::array_type,
            rt_array_type_t *, rt_type_t *, dimension_t
        > __rpool_key_array_type_base_t;

        // Rt pool array type key.
        template<>
        struct __rpool_key_t<rpool_key_type_t::array_type> : __rpool_key_array_type_base_t
        {
            using __rpool_key_array_type_base_t::__rpool_key_array_type_base_t;
        };

        // - - - - - - - - - - - - - - - - - - - - - - - - - -

        // Base class of rt generic type pool key.
        typedef __rpool_key_base_t<rpool_key_type_t::generic_type,
            rt_generic_type_t *, rt_general_type_t *, rt_type_t *, __rt_generic_args_key_t
        > __rpool_key_generic_type_base_t;

        // Rt pool generic type key.
        template<>
        struct __rpool_key_t<rpool_key_type_t::generic_type> : __rpool_key_generic_type_base_t
        {
            using __rpool_key_generic_type_base_t::__rpool_key_generic_type_base_t;

            __rt_generic_args_key_t args_key() const
            {
                return std::get<__rt_generic_args_key_t>(*this);
            }

            rt_type_t ** types() const { return args_key().begin(); }
            rt_general_type_t * template_() const { return std::get<rt_general_type_t *>(*this); }
            rt_type_t * host_type() const { return std::get<rt_type_t *>(*this); }
        };

        //-------- ---------- ---------- ---------- ----------

        template<rpool_key_type_t _key_type>
        using __partial_t = typename __rpool_key_t<_key_type>::partial_t;

        typedef __rpool_key_t<rpool_key_type_t::generic_method> generic_method_key_t;
        typedef __rpool_key_t<rpool_key_type_t::generic_type>   generic_type_key_t;
    }

    //-------- ---------- ---------- ---------- ----------

    // Rt pool.
    class rt_pool_t : public object_t, public no_copy_ctor_t
                    , __partial_t<rpool_key_type_t::method>
                    , __partial_t<rpool_key_type_t::field>
                    , __partial_t<rpool_key_type_t::generic_method>
                    , __partial_t<rpool_key_type_t::array_type>
                    , __partial_t<rpool_key_type_t::generic_type>
    {
    public:

        // Constructor.
        rt_pool_t(memory_t * memory);

        using __partial_t<rpool_key_type_t::method>::get;
        using __partial_t<rpool_key_type_t::field>::get;
        using __partial_t<rpool_key_type_t::generic_method>::get;
        using __partial_t<rpool_key_type_t::array_type>::get;
        using __partial_t<rpool_key_type_t::generic_type>::get;

        // Creates new key.
        template<rpool_key_type_t _key_type, typename ... _keys_t>
        static __rpool_key_t<_key_type> new_key(_keys_t && ... keys)
        {
            return __rpool_key_t<_key_type>(std::forward<_keys_t>(keys) ...);
        }

        generic_method_key_t revise_key(const generic_method_key_t & key,
                                            rt_type_t ** types, int count);

        generic_type_key_t revise_key(const generic_type_key_t & key,
                                            rt_type_t ** types, int count);

        // Gets generic type.
        rt_generic_type_t * to_generic_type(memory_t * memory, rt_general_type_t * template_,
                        rt_type_t * host_type, rt_type_t ** atypes, int atype_count);

        // Gets array type.
        rt_array_type_t * to_array_type(memory_t * memory, rt_type_t * element_type,
                                        dimension_t dimension);

        // Gets generic method.
        rt_generic_method_t * to_generic_method(memory_t * memory, rt_method_t * template_,
                        rt_type_t * host_type, rt_type_t ** atypes, int atype_count);

        // Creates a msize array.
        msize_t * new_msize_array(size_t count);

        // Creates a rt_tuple_field_t array.
        rt_tuple_field_t * new_tuple_field_array(size_t count);

        X_TO_STRING_IMPL(_T("rt_pool_t"))

    private:
        al::heap_t<rt_type_t *[]> __types_heap;
        std::set<__rt_generic_args_key_t> __types_map;

        memory_t * __memory;

        // Finds generic arguments key.
        __rt_generic_args_key_t __find_generic_args_key(const __rt_generic_args_key_t & args_key,
                                           rt_type_t ** types, int count);

        al::heap_t<msize_t[]> __msize_heap;
        al::heap_t<rt_tuple_field_t[]> * __tuple_fields_heap;

        pool_t __pool;
    };

    ////////// ////////// ////////// ////////// //////////
    // rt_context_t

    class rt_context_t : public object_t
    {
    public:
        rt_context_t(memory_t * memory, rt_heap_t * heap, rt_assembly_provider_t * assembly_provider)
            : memory(memory), heap(heap), assemblies(*this, assembly_provider)
            , rpool(memory)
        {
            _A(heap != nullptr);
        }

        rt_heap_t *     heap;
        memory_t  *     memory;
        rt_memory_t     rmemory;
        rt_pool_t       rpool;
        rt_spool_t      spool;

        rt_assemblies_t assemblies;

        X_TO_STRING_IMPL(_T("rt_context_t"))
    };

    ////////// ////////// ////////// ////////// //////////

    template<__tidx_t tidx> struct rt_mt_t;

    namespace
    {

        // Runtime metadata base.
        template<__tidx_t tidx, typename _self_t = rt_mt_t<tidx>>
        struct __rt_mt_base_t : mt_t<tidx>
        {
            typedef _self_t             __self_t, __mt_t;
            typedef mt_t<tidx>          __super_t;
            typedef rt_entity_t<tidx>   __entity_t;

            using __super_t::__super_t;

            __entity_t * rt_object;

            // Initialize.
            static void init(__self_t & self)
            {
                self.rt_object = nullptr;
            }

            // Returns new entity.
            static __entity_t * new_entity(rt_context_t & ctx, ref_t ref, __mt_t * mt,
                                           rt_assembly_t * assembly)
            {
                __entity_t * entity = memory_t::new_obj<__entity_t>(ctx.memory);
                entity->mt = mt;

                return entity;
            }

            // Commits it.
            static void commit(__self_t & self) { }
        };
    }

    //-------- ---------- ---------- ---------- ----------

    // Runtime metadata.
    template<__tidx_t tidx> struct rt_mt_t : __rt_mt_base_t<tidx>
    {
        typedef __rt_mt_base_t<tidx> __super_t;

        using __super_t::__super_t;
    };

    //-------- ---------- ---------- ---------- ----------

    // Constant runtime metadata.
    template<> struct rt_mt_t<__tidx_t::constant> : mt_t<__tidx_t::constant>
    {
        typedef rt_mt_t<__tidx_t::constant> __self_t;
        typedef mt_t<__tidx_t::constant>    __super_t;

        static void init(__self_t & self) { }
        static void commit(__self_t & self) { }
    };

    //-------- ---------- ---------- ---------- ----------

    // Returns a new entity.
    template<typename _entity_t> _entity_t * __new_entity(memory_t * memory)
    {
        return memory_t::new_obj<_entity_t>(memory);
    }

    ////////// ////////// ////////// ////////// //////////

    // Runtime metadata object.
    template<__tidx_t tidx> class rt_metadata_object_t
    {
    public:
        rt_mt_t<tidx> * mt = nullptr;

        rt_mt_t<tidx> * operator ->() { return mt; }
    };

    //-------- ---------- ---------- ---------- ----------

    // Member type.
    X_ENUM(rt_member_type_t)

        general,

        generic,

        array,          // Array type.

        tuple,          // Tuple field.

        position,       // Position field.

    X_ENUM_END

    // Runtime member.
    class rt_member_t : public rt_object_t
    {

    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime type kinds.
    X_ENUM(rt_type_kind_t)

        // General
        general,
        
        // Generic
        generic,
        
        // Array
        array,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Search method options.
    X_ENUM(search_method_options_t)

        // Default
        default_        = __default__,

        // On search virtual method.
        only_virtual    = 1 << 0,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    struct method_prototype_t;
    typedef std::function<bool (rt_type_t *)> each_super_type_callback_t;

    // Runtime type.
    class rt_type_t : public rt_member_t
    {
    public:

        // Pre new object.
        virtual void pre_new(analyzer_env_t & env) = 0;

        // Pre call static method.
        virtual void pre_static_call(analyzer_env_t & env) = 0;

        // Gets type name.
        virtual rt_sid_t get_name(analyzer_env_t & env) = 0;

        // Gets type kind.
        virtual rt_type_kind_t get_kind() = 0;

        // Gets ttype.
        virtual ttype_t get_ttype(analyzer_env_t & env) { return ttype_t::__unknown__; }

        // Gets base type.
        virtual rt_type_t * get_base_type(analyzer_env_t & env,
                                          const __gp_mgr_t * gp_manager = nullptr) = 0;

        // Enum all super types.
        virtual void each_super_type(analyzer_env_t & env,
                each_super_type_callback_t callback, const __gp_mgr_t * gp_manager = nullptr) = 0;

        // Gets method offset.
        virtual int get_method_offset(analyzer_env_t & env, ref_t method_ref) = 0;

        // Gets field offset.
        virtual msize_t get_field_offset(analyzer_env_t & env, ref_t ref) = 0;

        // Gets filed offset of position.
        virtual msize_t get_field_offset(analyzer_env_t & env, int position) = 0;

        // Enums all fields.
        typedef std::function<bool (ref_t ref, rt_field_base_t * rt_field)> each_field_t;
        virtual void each_field(analyzer_env_t & env, each_field_t f) = 0;

        // Enums all methods.
        typedef std::function<bool (ref_t, rt_method_t *)> each_method_t;
        virtual void each_method(analyzer_env_t & env, each_method_t f) = 0;

        // Searches method.
        virtual ref_t search_method(analyzer_env_t & env,
                    method_prototype_t & prototype,
                    search_method_options_t options = search_method_options_t::default_) = 0;

        // Enumerates all extend types.
        typedef std::function<bool (ref_t, rt_type_t *)> each_type_t;
        virtual void each_extend_types(analyzer_env_t & env, rt_sid_t name, each_type_t f) { }

        // Gets size.
        msize_t get_size(analyzer_env_t & env, storage_type_t * out_storage_type);

        // Gets size.
        msize_t get_size(analyzer_env_t & env);

        // Gets variable size.
        virtual msize_t get_variable_size(analyzer_env_t & env,
                                          storage_type_t * out_storage_type = nullptr) = 0;

        // Gets assembly.
        virtual rt_assembly_t * get_assembly() = 0;

        // Gets size.
        msize_t get_size() { return __size; }

        // Return count of virtual members.
        msize_t get_virtual_count(analyzer_env_t & env);

        // Gets data type.
        virtual vtype_t get_vtype(analyzer_env_t & env) = 0;

        // Returns host type.
        virtual rt_type_t * get_host_type() = 0;

    protected:

        // When caculate size.
        virtual msize_t on_caculate_size(analyzer_env_t & env,
                                        storage_type_t * out_storage_type) = 0;


        // When caculate layout.
        virtual msize_t on_caculate_layout(analyzer_env_t & env, msize_t base_size,
                                        storage_type_t * out_storage_type) = 0;

        // When caculate virtual count.
        virtual msize_t on_caculate_virtual_count(analyzer_env_t & env);

        // Analyze this type.
        assembly_analyzer_t __analyzer(analyzer_env_t & env,
                                       const generic_param_manager_t * gp_manager = nullptr);

        msize_t         __size = unknown_msize;
        msize_t         __virtual_count = unknown_msize;
        storage_type_t  __storage_type;
    };

    // Returns whether it's a generic type.
    bool is_general(rt_type_t * type);

    // Returns whether it's a custom struct.
    bool is_custom_struct(analyzer_env_t & env, rt_type_t * type);

    // Returns whether it's a host type of specified type.
    bool is_host_type_template(rt_type_t * type, rt_type_t * host_type);

    //-------- ---------- ---------- ---------- ----------
    // General like type.

    class __rt_general_like_type_t : public rt_type_t
    {
    public:

    protected:

        // When caculate size.
        virtual msize_t on_caculate_size(analyzer_env_t & env,
                            storage_type_t * out_storage_type) override;

        // Gets variable size.
        virtual msize_t get_variable_size(analyzer_env_t & env,
                                storage_type_t * out_storage_type) override final;
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime general type.
    class rt_general_type_t : public __rt_general_like_type_t
                            , public rt_metadata_object_t<__tidx_t::type>
    {
        typedef __rt_general_like_type_t __super_t;

    public:

        // Constructor.
        rt_general_type_t() = default;
        rt_general_type_t(rt_assembly_t * assembly, rt_general_type_t * host_type)
            : assembly(assembly), host_type(host_type)
        { }

        // Pre new object.
        virtual void pre_new(analyzer_env_t & env) override final;

        // Pre call static method.
        virtual void pre_static_call(analyzer_env_t & env) override final;

        // Gets name.
        virtual rt_sid_t get_name(analyzer_env_t & env) override final;

        // Gets this kind.
        virtual rt_type_kind_t get_kind() override final { return rt_type_kind_t::general; }

        // Gets ttype.
        virtual ttype_t get_ttype(analyzer_env_t & env) override final;

        // Gets base type.
        virtual rt_type_t * get_base_type(analyzer_env_t & env,
                                          const __gp_mgr_t * gp_manager = nullptr) override final;

        // Enum all super types.
        virtual void each_super_type(analyzer_env_t & env,
                each_super_type_callback_t callback, const __gp_mgr_t * gp_manager) override final;

        // Gets method offset.
        virtual int get_method_offset(analyzer_env_t & env, ref_t method_ref)
                                                                    override final;

        // Gets field offset.
        virtual msize_t get_field_offset(analyzer_env_t & env, ref_t ref) override final;

        // Gets filed offset of position.
        virtual msize_t get_field_offset(analyzer_env_t & env, int position) override final;

        // Enumerates all fields.
        virtual void each_field(analyzer_env_t & env, each_field_t f) override final;

        // Enumerates all methods.
        virtual void each_method(analyzer_env_t & env, each_method_t f)
                                                                    override final;
        // Searches methods.
        virtual ref_t search_method(analyzer_env_t & env,
                    method_prototype_t & prototype, search_method_options_t options)
                                                                    override final;

        // Gets data type.
        virtual vtype_t get_vtype(analyzer_env_t & env) override;

        // Gets assembly.
        virtual rt_assembly_t * get_assembly() override final;

        // Owner assembly.
        rt_assembly_t * assembly = nullptr;

        // Host type.
        rt_general_type_t * host_type = nullptr;

        // Returns host type.
        virtual rt_type_t * get_host_type() override { return host_type; }

        // Returns generic param count.
        size_t generic_param_count();

        // Returns if it's has uncertain types.
        bool has_uncertain_types(analyzer_env_t & env);

        // Returns if it's match the specified param count.
        bool is_match(assembly_analyzer_t & analyzer, size_t args_count);

        // Returns whether it's a generic template.
        bool is_generic_template();

    protected:

        // When caculate size.
        virtual msize_t on_caculate_size(analyzer_env_t & env,
                            storage_type_t * out_storage_type) override final;

        // When caculate layout.
        virtual msize_t on_caculate_layout(analyzer_env_t & env, msize_t base_size,
                                        storage_type_t * out_storage_type) override final;

    private:
        void __build_vtbl(analyzer_env_t & env);
    };

    // Returns whether it's a runnable type, general type (except template) or generic type.
    bool is_runnable_type(rt_type_t * type);

    //-------- ---------- ---------- ---------- ----------

    // Runtime type ref.
    class rt_type_ref_t : public rt_object_t
                        , public rt_metadata_object_t<__tidx_t::type_ref>
    {
    public:

    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime generic type.
    class rt_generic_type_t : public __rt_general_like_type_t
    {
        typedef __rt_general_like_type_t __super_t;

    public:

        // Constructor.
        rt_generic_type_t() = default;

        // Constructor.
        rt_generic_type_t(rt_general_type_t * template_, rt_type_t * host_type,
                                            rt_type_t ** atypes, int atype_count);

        // Pre new object.
        virtual void pre_new(analyzer_env_t & env) override final;

        // Pre static call.
        virtual void pre_static_call(analyzer_env_t & env) override final;

        // Gets name.
        virtual rt_sid_t get_name(analyzer_env_t & env) override final;

        // Gets kind.
        virtual rt_type_kind_t get_kind() override final { return rt_type_kind_t::generic; }

        // Gets ttype.
        virtual ttype_t get_ttype(analyzer_env_t & env) override final;

        // Gets data type.
        virtual vtype_t get_vtype(analyzer_env_t & env) override;

        // Gets base type.
        virtual rt_type_t * get_base_type(analyzer_env_t & env,
                                          const __gp_mgr_t * gp_manager = nullptr) override final;

        // Enum all super types.
        virtual void each_super_type(analyzer_env_t & env,
                each_super_type_callback_t callback, const __gp_mgr_t * gp_manager) override final;

        // Gets method offset.
        virtual int get_method_offset(analyzer_env_t & env, ref_t method_ref) override final;

        // Gets field offset.
        virtual msize_t get_field_offset(analyzer_env_t & env, ref_t ref) override final;

        // Gets filed offset of position.
        virtual msize_t get_field_offset(analyzer_env_t & env, int position) override final;

        // Enumerates all fields.
        virtual void each_field(analyzer_env_t & env, each_field_t f) override final;

        // Enumerates all methods.
        virtual void each_method(analyzer_env_t & env, each_method_t f) override final;

        // Searches method.
        virtual ref_t search_method(analyzer_env_t & env,
                method_prototype_t & prototype, search_method_options_t options) override final;

        // Enumerates all extend types.
        virtual void each_extend_types(analyzer_env_t & env, rt_sid_t name, each_type_t f)
                                                                                override;

        // Gets assembly.
        virtual rt_assembly_t * get_assembly() override final;

        // Generic type template.
        rt_general_type_t * template_ = nullptr;

        // Generic arguments.
        rt_type_t ** atypes = nullptr;

        // Generic argument count.
        int atype_count() const { return __atype_count; }

        // Host type.
        rt_type_t * host_type = nullptr;

        // Returns host type.
        virtual rt_type_t * get_host_type() override { return host_type; }

        // It's a tuple type?
        bool is_tuple() { return __is_tuple(); }

    protected:

        // When caculate size.
        virtual msize_t on_caculate_size(analyzer_env_t & env,
                            storage_type_t * out_storage_type) override final;

        // When caculate layout.
        virtual msize_t on_caculate_layout(analyzer_env_t & env, msize_t base_size,
                                        storage_type_t * out_storage_type) override final;

    private:

        // Build virtual table.
        void __build_vtbl(analyzer_env_t & env, generic_param_manager_t * gp_mgr);

        // It's a tuple type?
        bool __is_tuple();

        // Ensure field offset initialzed.
        void __ensure_field_offset_initialized(analyzer_env_t & env);

        // Returns tuple fields.
        rt_tuple_field_t * __get_tuple_fields(analyzer_env_t & env);

        // Field offset array.
        msize_t * __field_offsets = nullptr;
        rt_tuple_field_t * __tuple_fields = nullptr;
        int __atype_count = 0;

        // int __b = 0;        // TODO: why it's set valuel 1?
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime array time.
    class rt_array_type_t : public rt_type_t
    {
    public:

        // Constructor.
        rt_array_type_t() = default;

        // Constructor.
        rt_array_type_t(rt_type_t * element_type, dimension_t dimension);

        // Pre new object.
        virtual void pre_new(analyzer_env_t & env) override final;

        // Pre call static member.
        virtual void pre_static_call(analyzer_env_t & env) override final;

        // Gets name.
        virtual rt_sid_t get_name(analyzer_env_t & env) override final;

        // Gets kind.
        virtual rt_type_kind_t get_kind() override final { return rt_type_kind_t::array; }

        // Gets ttype.
        virtual ttype_t get_ttype(analyzer_env_t & env) override final;

        // Gets base type.
        virtual rt_type_t * get_base_type(analyzer_env_t & env,
                              const __gp_mgr_t * gp_manager = nullptr) override final;

        // Enum all super types.
        virtual void each_super_type(analyzer_env_t & env,
                each_super_type_callback_t callback, const __gp_mgr_t * gp_manager) override final;

        // Gets method offset.
        virtual int get_method_offset(analyzer_env_t & env, ref_t method_ref) override final;

        // Gets field offset.
        virtual msize_t get_field_offset(analyzer_env_t & env, ref_t ref) override final;

        // Gets filed offset of position.
        virtual msize_t get_field_offset(analyzer_env_t & env, int position) override final;

        // Enums fields.
        virtual void each_field(analyzer_env_t & env, each_field_t f) override final { }

        // Enums methods.
        virtual void each_method(analyzer_env_t & env, each_method_t f) override final { }

        // Searches method.
        virtual ref_t search_method(analyzer_env_t & env, method_prototype_t & prototype,
                search_method_options_t options = search_method_options_t::default_) override final;

        // Gets variable size.
        virtual msize_t get_variable_size(analyzer_env_t & env,
                                          storage_type_t * out_storage_type) override final;

        // Element type.
        rt_type_t * element_type = nullptr;

        // Dimension
        dimension_t dimension = 0;

        // Gets assembly.
        virtual rt_assembly_t * get_assembly() override final;

        // Gets data type.
        virtual vtype_t get_vtype(analyzer_env_t & env) override { return vtype_t::mobject_; }

        // Returns host type.
        virtual rt_type_t * get_host_type() override { return nullptr; }

    protected:

        // When caculate size.
        virtual msize_t on_caculate_size(analyzer_env_t & env,
                                storage_type_t * out_storage_type) override final;

        // When caculate layout.
        virtual msize_t on_caculate_layout(analyzer_env_t & env, msize_t base_size,
                                        storage_type_t * out_storage_type) override final;
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime bytes.
    struct rt_bytes_t
    {
        const byte_t * bytes    = nullptr;
        size_t         length   = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime method base.
    class rt_method_base_t
    {
    public:

        // Returns type of this member.
        virtual rt_member_type_t this_type() = 0;

        // Returns method name.
        virtual rt_sid_t get_name() = 0;

        // Gets host type.
        virtual rt_type_t * get_host_type() = 0;

        // Returns a description of this method.
        virtual string_t to_string(analyzer_env_t & env, bool include_host_type = false) = 0;
    };

    //-------- ---------- ---------- ---------- ----------
    // rt_method_t

    // Runtime method.
    class rt_method_t : public rt_member_t
                      , public rt_metadata_object_t<__tidx_t::method>
                      , public rt_method_base_t
    {
    public:

        // Returns type of this object.
        virtual rt_member_type_t this_type() override { return rt_member_type_t::general; }

        // Host type.
        rt_type_t * host_type = nullptr;

        // Gets assembly.
        rt_assembly_t * get_assembly();

        // Gets host type.
        rt_type_t * get_host_type() override;

        // Gets owner type (an interface type).
        rt_type_t * get_owner_type(analyzer_env_t & env);

        // Gets name.
        virtual rt_sid_t get_name() override;

        // Returns generic param count.
        int generic_param_count();

        // Returns a description of this method.
        virtual string_t to_string(analyzer_env_t & env, bool include_host_type) override;
    };

    //-------- ---------- ---------- ---------- ----------
    // rt_generic_method_t

    // Runtime generic method.
    class rt_generic_method_t : public rt_object_t
                              , public rt_method_base_t
    {
    public:

        // Constructor.
        rt_generic_method_t() = default;
        rt_generic_method_t(rt_method_t * template_, rt_type_t * host_type, rt_type_t ** atypes,
                                                                        int atype_count = -1);

        // Returns type of this object.
        virtual rt_member_type_t this_type() override { return rt_member_type_t::generic; }

        // Method template.
        rt_method_t *  template_ = nullptr;

        // Arguments.
        rt_type_t  ** atypes = nullptr;

        // Argument count.
        int atype_count() { return __atype_count; /* template_->generic_param_count(); */ }

        // Host Type.
        rt_type_t * host_type = nullptr;

        // Returns host type.
        rt_type_t * get_host_type() override;

        // Gets assembly.
        rt_assembly_t * get_assembly();

        // Gets name.
        virtual rt_sid_t get_name() override;

        // Returns a description of this method.
        virtual string_t to_string(analyzer_env_t & env, bool include_host_type) override;

    private:
        int __atype_count;
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime method ref.
    class rt_method_ref_t : public rt_object_t
                          , public rt_metadata_object_t<__tidx_t::method_ref>
    {
    public:

        // Offset.
        msize_t offset = unknown_msize;
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime field base.

    class rt_field_base_t
    {
    public:

        // Offset.
        msize_t offset = unknown_msize;

        // Sets offset.
        void set_offset(msize_t offset)
        {
            this->offset = offset;
        }

        virtual rt_type_t * get_field_type(assembly_analyzer_t & analyzer,
                                rt_generic_type_t * owner = nullptr) = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime general field.
    class rt_field_t : public rt_member_t
                     , public rt_field_base_t
                     , public rt_metadata_object_t<__tidx_t::field>
    {
    public:
        virtual rt_type_t * get_field_type(assembly_analyzer_t & analyzer,
                                rt_generic_type_t * owner) override;
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime tuple field.
    class rt_tuple_field_t : public rt_member_t
                           , public rt_field_base_t
    {
    public:
        rt_type_t * type = nullptr;
        int position = 0;

        virtual rt_type_t * get_field_type(assembly_analyzer_t & analyzer,
                                rt_generic_type_t * owner) override { return type; }
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime generic field.
    class rt_generic_field_t : public rt_object_t
                             , public rt_field_base_t
    {
    public:
        rt_generic_field_t(rt_type_t * type) : type(type) { }

        rt_type_t * type;

        virtual rt_type_t * get_field_type(assembly_analyzer_t & analyzer,
                                rt_generic_type_t * owner) override { return type; }
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime position field.
    class rt_position_field_t : public rt_object_t
    {
        // Empty.
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime field ref.
    class rt_field_ref_t : public rt_object_t
                         , public rt_metadata_object_t<__tidx_t::field_ref>
    {
        // Empty.
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime event.
    class rt_event_t : public rt_member_t
                     , public rt_metadata_object_t<__tidx_t::event>
    {
        // Empty.
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime event ref.
    class rt_event_ref_t : public rt_object_t
                         , public rt_metadata_object_t<__tidx_t::event_ref>
    {
        // Empty.
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime property.
    class rt_property_t : public rt_member_t
                        , public rt_metadata_object_t<__tidx_t::property>
    {
        // Empty.
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime super type.
    class rt_super_type_t : public rt_object_t
                          , public rt_metadata_object_t<__tidx_t::super_type>
    {
        // Empty.
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime nest type.
    class rt_nest_type_t : public rt_object_t
                         , public rt_metadata_object_t<__tidx_t::nest_type>
    {
        // Empty.
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime property ref.
    class rt_property_ref_t : public rt_object_t
                            , public rt_metadata_object_t<__tidx_t::property_ref>
    {
        // Empty.
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime generic param.
    class rt_generic_param_t : public rt_object_t
                             , public rt_metadata_object_t<__tidx_t::generic_param>
    {
        // Empty.
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime param.
    class rt_param_t : public rt_object_t
                     , public rt_metadata_object_t<__tidx_t::param>
    {
        // Empty.
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime generic argument.
    class rt_generic_argument_t : public rt_object_t
                                , public rt_metadata_object_t<__tidx_t::generic_argument>
    {
        // Empty.
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime attribute.
    class rt_attribute_t : public rt_object_t
                         , public rt_metadata_object_t<__tidx_t::attribute>
    {
        // Empty.
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime attribute arguments.
    class rt_attribute_argument_t : public rt_object_t
                                  , public rt_metadata_object_t<__tidx_t::attribute_argument>
    {
        // Empty.
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime method ref param.
    class rt_method_ref_param_t : public rt_object_t
                                , public rt_metadata_object_t<__tidx_t::method_ref_param>
    {
        // Empty.
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime assembly ref.
    class rt_assembly_ref_t : public rt_object_t
                            , public rt_metadata_object_t<__tidx_t::assembly_ref>
    {
        // Empty.
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime param type.
    struct rt_ptype_t
    {
        // Constructor.
        rt_ptype_t() = default;
        rt_ptype_t(rt_type_t * type, param_type_t param_type)
            : type(type), param_type(param_type) { }

        // Type.
        rt_type_t *     type = nullptr;

        // Param type.
        param_type_t    param_type = param_type_t::__default__;

        // Converts to string.
        string_t to_string(analyzer_env_t & env) const;
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime assembly.
    class rt_assembly_t : public rt_object_t
                        , public rt_metadata_object_t<__tidx_t::assembly>
                        , public object_t
    {
    public:

        // Converts res_t to sid.
        virtual rt_sid_t to_sid(res_t s) = 0;

        // Converts string to sid.
        virtual rt_sid_t to_sid(const string_t & s) = 0;

        // Returns name.
        virtual rt_sid_t get_name() = 0;

        // Gets entity point.
        virtual rt_method_t * get_entry_point() = 0;

        // Gets method body.
        virtual rt_bytes_t get_method_body(rt_method_t * method) = 0;

        // Gets field name.
        virtual rt_sid_t get_name(rt_field_t * field) = 0;

        // Gets method name.
        virtual rt_sid_t get_name(rt_method_t * method) = 0;

        // Gets method ref.
        virtual rt_method_ref_t * get_method_ref(ref_t method_ref) = 0;

        // Gets field ref.
        virtual rt_field_ref_t * get_field_ref(ref_t field_ref) = 0;

        // Gets type ref.
        virtual rt_type_ref_t * get_type_ref(ref_t type_ref) = 0;

        // Gets assembly ref.
        virtual rt_assembly_ref_t * get_assembly_ref(ref_t assembly_ref) = 0;

        // Gets name.
        virtual rt_sid_t get_name(rt_assembly_ref_t * assembly_ref) = 0;

        // Gets package.
        virtual rt_sid_t get_package(rt_assembly_ref_t * assembly_ref) = 0;

        // Gets general type.
        virtual rt_general_type_t * get_type(ref_t ref) = 0;

        // Gets general type by specified namespace, name and generic param count.
        virtual rt_general_type_t * get_type(rt_sid_t ns, rt_sid_t name,
                                            int generic_param_count) = 0;

        // Gets general type by specified namespace, name and generic param count.
        virtual rt_general_type_t * get_type(const string_t & ns, const string_t & name,
                                            int generic_param_count) = 0;

        // Gets name of general type.
        virtual rt_sid_t get_name(rt_general_type_t * type) = 0;

        // Gets array type metadata.
        virtual rt_mt_t<__tidx_t::array_type> * mt_of_array(ref_t ref) = 0;

        // Gets generic type metadata.
        virtual rt_mt_t<__tidx_t::generic_type> * mt_of_generic_type(ref_t ref) = 0;

        // Gets generic field metadata.
        virtual rt_mt_t<__tidx_t::generic_field> * mt_of_generic_field(ref_t ref) = 0;

        // Gets position field metadata.
        virtual rt_mt_t<__tidx_t::position_field> * mt_of_position_field(ref_t ref) = 0;

        // Gets generic method metadata.
        virtual rt_mt_t<__tidx_t::generic_method> * mt_of_generic_method(ref_t ref) = 0;

        // Gets method.
        virtual rt_method_t * get_method(ref_t ref) = 0;

        // Gets param.
        virtual rt_field_t * get_field(ref_t ref) = 0;

        // Gets generic method.
        virtual rt_param_t  * get_param(ref_t param_ref) = 0;

        // Gets generic method.
        virtual rt_generic_method_t * get_generic_method(ref_t ref,
                                    const generic_param_manager_t * gp_mgr) = 0;

        // Gets generic argument.
        virtual rt_generic_argument_t * get_generic_argument(ref_t ref) = 0;

        // Gets generic param.
        virtual rt_generic_param_t * get_generic_param(ref_t ref) = 0;

        // Gets super type.
        virtual rt_super_type_t * get_super_type(ref_t ref) = 0;

        // Gets nest ype.
        virtual rt_nest_type_t  * get_nest_type(ref_t ref) = 0;

        // Callback function for enums members.
        template<__tidx_t tidx>
        using each_func_t = std::function<bool(int index, rt_mt_t<tidx> & mt)>;

        // Enums all methods.
        typedef each_func_t<__tidx_t::method> __each_method_func_t;
        virtual bool each_methods(ref_t methods, __each_method_func_t f) = 0;

        // Enums all fields.
        typedef each_func_t<__tidx_t::field> __each_field_func_t;
        virtual bool each_fields(ref_t field, __each_field_func_t f) = 0;

        // Enums all params.
        typedef each_func_t<__tidx_t::param> __each_param_func_t;
        virtual bool each_params(ref_t params, __each_param_func_t f) = 0;

        // Gets host type by method ref.
        virtual rt_general_type_t * get_host_by_method_ref(ref_t method_ref) = 0;

        // Gets host type by field ref.
        virtual rt_general_type_t * get_host_by_field_ref(ref_t field_ref) = 0;

        // Gets host by nest type ref.
        virtual rt_general_type_t * get_host_by_nest_type_ref(ref_t type_ref) = 0;

        // Gets method ref param.
        virtual rt_method_ref_param_t * get_method_ref_param(ref_t ref) = 0;

        int index = -1;

        X_TO_STRING_IMPL(_T("rt_assembly_t"))
    };

    ////////// ////////// ////////// ////////// //////////

    // Gets virtual table.
    template<typename _type_t>
    rt_vtable_t * get_vtable(_type_t * type)
    {
        _A(type != nullptr);

        return *((rt_vtable_t **)type - 1);
    }

    // Sets virtual table.
    template<typename _type_t>
    void set_vtable(_type_t * type, rt_vtable_t * vtbl)
    {
        _A(type != nullptr);

        *((rt_vtable_t **)type - 1) = vtbl;
    }

    //-------- ---------- ---------- ---------- ----------
    // method_prototype_t

    // Method prototype.
    struct method_prototype_t
    {
        typedef al::svector_t<rt_ptype_t, 8> __ptypes;

        rt_sid_t        name;                   // Method property.
        rt_type_t *     return_type;            // Return type.
        __ptypes        arg_types;              // Argument types.
        int             generic_param_count;    // Generic param count.

        // Returns param count.
        int param_count() const { return arg_types.size(); }

        // Converts to string.
        string_t to_string(analyzer_env_t & env) const;
    };

    //-------- ---------- ---------- ---------- ----------
    // analyzer_env_t

    // Analyze environment.
    class analyzer_env_t : public object_t
    {
        typedef analyzer_env_t  __self_t;
        typedef object_t        __super_t;

    public:

        // Constructor.
        analyzer_env_t(memory_t * memory, rt_pool_t & rpool, rt_assemblies_t & assemblies)
            : rpool(rpool), assemblies(assemblies), memory(memory)
        {
            _A(memory  != nullptr);
        }

        rt_pool_t &         rpool;          // Runtime pool.
        rt_assemblies_t &   assemblies;     // Assemblies.
        memory_t *          memory;         // Memory managerment.

        // Returns memory managerment.
        memory_t * get_memory() { return this->memory; }

        // Returns core assembly.
        rt_assembly_t * get_core_assembly();
        // Returns core type of specified name.
        rt_general_type_t * get_core_type(const string_t & ns, const string_t & name,
                                          int generic_param_count = 0);

        // Returns array type.
        rt_general_type_t * get_tarray_type();

        // Returns tuple type.
        rt_general_type_t * get_tuple_type();

        X_TO_STRING_IMPL(_T("analyzer_env_t"))

    private:
        rt_assembly_t * __core_assembly = nullptr;
        rt_general_type_t * __t_array_type = nullptr;
        rt_general_type_t * __t_tuple_type = nullptr;
    };

    // Converts rt_context_t to analyze_env_t.
    X_INLINE analyzer_env_t to_env(rt_context_t & ctx)
    {
        return analyzer_env_t(ctx.memory, ctx.rpool, ctx.assemblies);
    }

    //-------- ---------- ---------- ---------- ----------
    // assembly_analyzer_t

    class generic_param_manager_t;

    // Assembly analyzer.
    class assembly_analyzer_t : public object_t
    {
        typedef assembly_analyzer_t __self_t;

    public:

        // Constructor.
        assembly_analyzer_t(analyzer_env_t & env, rt_assembly_t * current,
                            const generic_param_manager_t * gp_manager = nullptr);

        // Constructor.
        assembly_analyzer_t(assembly_analyzer_t & prototype, rt_assembly_t * current)
            : assembly_analyzer_t(prototype.env, current, prototype.gp_manager)
        { }

        // Constructor.
        assembly_analyzer_t(assembly_analyzer_t & prototype, const __gp_mgr_t * gp_mgr)
            : assembly_analyzer_t(prototype.env, prototype.current, gp_mgr)
        { }

        analyzer_env_t & env;                   // Analyze environment.
        rt_assembly_t  * current;               // Current assembly.
        const generic_param_manager_t * gp_manager;   // Generic param manager.

        // Gets sid by res.
        rt_sid_t to_sid(res_t res) { return __to_sid(res); }

        // Gets sid by string.
        rt_sid_t to_sid(const string_t & s);

        // Gets name of runtime type.
        rt_sid_t get_name(rt_type_t * type);

        // Gets name of runtime method.
        rt_sid_t get_name(rt_method_t * method);

        // Gets name of runtime generic method.
        rt_sid_t get_name(rt_generic_method_t * method);

        // Gets host type by method ref.
        rt_type_t * get_host_type_by_method_ref(ref_t method_ref,
                                rt_method_ref_t ** out_method_ref = nullptr);

        // Gets host type field ref.
        rt_type_t * get_host_type_by_field_ref(ref_t field_ref,
                                rt_field_ref_t ** out_field_ref = nullptr);

        // Gets host type by method ref.
        rt_type_t * get_host_type(rt_method_ref_t * rt);

        // Gets host type by field ref.
        rt_type_t * get_host_type(rt_field_ref_t * rt);

        // Gets host type by method metadata.
        rt_type_t * get_host_type(mt_method_ref_t * mt);

        // Gets host type by field metadata.
        rt_type_t * get_host_type(mt_field_ref_t * mt);

        // Gets assembly by type ref.
        rt_assembly_t * get_assembly(mt_type_ref_t * mt);

        // Gets assembly by assembly ref.
        rt_assembly_t * get_assembly(ref_t assembly_ref);

        // Gets assembly by runtime assembly ref.
        rt_assembly_t * get_assembly(rt_assembly_ref_t * rt);

        // Gets assembly by assembly ref metadata.
        rt_assembly_t * get_assembly(mt_assembly_ref_t * mt);

        // Gets type by type ref.
        rt_type_t * get_type(ref_t type_ref);

        // Gets array type.
        rt_array_type_t * to_array_type(rt_type_t * element_type, dimension_t dimension);

        // Gets generic type.
        rt_generic_type_t * to_generic_type(rt_general_type_t * template_,
                            rt_type_t * host_type, rt_type_t ** atypes, int atype_count);

        // Converts template to generic type, used by gp_manager.
        rt_type_t * to_generic_type(rt_general_type_t * template_);

        // Gets generic method.
        rt_generic_method_t * to_generic_method(rt_method_t * template_,
                    rt_type_t * host_type, rt_type_t ** atypes = nullptr, int atype_count = 0);

        // Gets method by method ref.
        rt_method_t * get_method(ref_t method_ref);

        // Gets generic method by method ref.
        rt_generic_method_t * get_generic_method(ref_t method_ref);

        // Gets method or generic method by method ref.
        rt_method_base_t * get_method_base(ref_t method_ref);

        // Gets field by field ref.
        rt_field_base_t * get_field(ref_t field_ref, rt_type_t ** out_type = nullptr);

        // Gets generic param.
        rt_generic_param_t * get_generic_param(ref_t ref);

        // Callback function for enums members.
        using each_extern_params_func_t = std::function<bool(rt_type_t * type)>;

        // Each extends types.
        void each_extend_params(ref_t extern_param_ref, each_extern_params_func_t f);

        // Returns char_t * by resource.
        const char_t * to_cstr(res_t res)
        {
            return __to_sid(res).c_str();
        }

        // Returns runtime pool.
        rt_pool_t & get_pool() { return env.rpool; }

        // Returns size of type.
        msize_t size_of(ref_t type, storage_type_t * out_storage_type = nullptr);

        // Returns size of type.
        msize_t size_of(rt_type_t * type, storage_type_t * out_storage_type = nullptr);

        // Returns variable size.
        msize_t variable_size_of(ref_t type_ref, storage_type_t * out_storage_type = nullptr);

        // Returns variable size.
        msize_t variable_size_of(rt_type_t * type, storage_type_t * out_storage_type = nullptr);

        // Gets base type.
        rt_type_t * get_base_type(rt_type_t * type);

        // Enum all super types.
        void each_super_type(rt_type_t * type, analyzer_env_t & env,
                                               each_super_type_callback_t callback);

        // Gets type size.
        msize_t get_type_size(ref_t type_ref);

        // Gets field offset.
        msize_t get_field_offset(ref_t field_ref, rt_type_t * host_type = nullptr);

        // Gets virtual method offset.
        int get_virtual_method_offset(ref_t method_ref);

        // Gets prototype of method ref.
        void get_prototype(ref_t method_ref, method_prototype_t * out_prototype);

        // Gets prototype of runtime method.
        void get_prototype(rt_method_t * rt_method, method_prototype_t * out_prototype);

        X_TO_STRING_IMPL(_T("assembly_analyzer_t"))

    private:

        // Returns whether it's current method.
        bool __is_current(rt_assembly_t * assembly) { return assembly == this->current; }

        // Returns sid of specified res.
        rt_sid_t __to_sid(res_t res) { return current->to_sid(res); }

        // Compare method.
        bool __compare_method(rt_assembly_t * rt_assembly, rt_mt_t<__tidx_t::method> & mt,
            rt_sid_t name, int generic_param_count, rt_type_t * return_type,
            int param_count, rt_ptype_t * params);

        // Binds to specified assembly.
        __self_t __bind(rt_assembly_t * current)
        {
            return __self_t((analyzer_env_t &)*this, current, gp_manager);
        }

        // Gets method by method ref.
        rt_method_t * __get_method(ref_t method_ref);

        // Gets field by field ref.
        rt_field_base_t  * __get_field(ref_t field_ref, rt_type_t ** out_type);

        // Creates an runtime array type.
        rt_array_type_t * __to_array_type(rt_type_t * element_type, dimension_t dimension);

        // Converts to generic type.
        rt_type_t * __to_generic(rt_general_type_t * template_);

        // Joins method name.
        string_t __join_method_name(rt_assembly_t * assembly,
            rt_type_t * type, mt_method_ref_t * mt_method, rt_ptype_t * ptypes);

        // Joins field name.
        string_t __join_field_name(rt_assembly_t * assembly,
            rt_type_t * type, mt_field_ref_t * mt_field_ref);
    };

    // Converts to assembly analyzer.
    assembly_analyzer_t to_analyzer(analyzer_env_t & env, rt_type_t * rt_type,
                                const generic_param_manager_t * gp_manager = nullptr);

    ////////// ////////// ////////// ////////// //////////

    // Joins assembly name.
    string_t join_assembly_name(const string_t & package, const string_t & name);

    // Joins type name.
    string_t join_type_name(const string_t & ns, const string_t & name, int generic_param_count);

    // Builds virtual table.
    rt_vtable_t * build_vtable(memory_t * memory, assembly_analyzer_t & analyzer,
                                                  rt_type_t & rt_type);

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        // Type wrapper for creating type entity.
        template<__tidx_t tidx>
        struct __type_wrap_t
        {
            typedef rt_entity_t<tidx> __entity_t;

            template<typename ... args_t>
            __type_wrap_t(args_t && ... args) : type(std::forward<args_t>(args) ...) { }

            rt_vtable_t *  vtbl = nullptr;
            __entity_t     type;
        };

        //-------- ---------- ---------- ---------- ----------

        // Base class for creating type entity.
        template<__tidx_t _tidx> struct __t_type_rt_mt_base_t : __rt_mt_base_t<_tidx>
        {
            typedef __rt_mt_base_t<_tidx>   __super_t;
            typedef rt_entity_t<_tidx>      __entity_t;
            typedef __type_wrap_t<_tidx>    __wrap_t;

            using __super_t::__super_t;
        };

        //-------- ---------- ---------- ---------- ----------

        // Base class for creating type entity.
        template<__tidx_t _tidx> struct __type_rt_mt_base_t : __t_type_rt_mt_base_t<_tidx>
        {
            typedef __type_rt_mt_base_t<_tidx> __self_t;
            typedef rt_mt_t<_tidx>             __mt_t;

            typedef __t_type_rt_mt_base_t<_tidx> __super_t;
            using __super_t::__super_t;

            // Creates a new entity.
            static typename __super_t::__entity_t * new_entity(
                    rt_context_t & ctx, ref_t ref, __mt_t * mt, rt_assembly_t * rt_assembly)
            {
                typedef typename __super_t::__wrap_t wrap_t;
                auto entity = &memory_t::new_obj<wrap_t>(ctx.memory, rt_assembly)->type;
                entity->mt = mt;

                return entity;
            }
        };

        //-------- ---------- ---------- ---------- ----------
        // general type

        // Base class for creating general type entity.
        template<> struct __type_rt_mt_base_t<__tidx_t::type>
            : __t_type_rt_mt_base_t<__tidx_t::type>
        {
            typedef __type_rt_mt_base_t<__tidx_t::type> __self_t;
            typedef rt_mt_t<__tidx_t::type>             __mt_t;

            typedef __t_type_rt_mt_base_t<__tidx_t::type> __super_t;
            using __super_t::__super_t;

            static typename __super_t::__entity_t * new_entity(
                    rt_context_t & ctx, ref_t ref, __mt_t * mt, rt_assembly_t * assembly)
            {
                typedef typename __super_t::__wrap_t wrap_t;
                rt_general_type_t * host_type = assembly->get_host_by_nest_type_ref(ref);

                auto entity = &memory_t::new_obj<wrap_t>(ctx.memory, assembly, host_type)->type;
                entity->mt = mt;

                return entity;
            }
        };

        //-------- ---------- ---------- ---------- ----------
        // array type

        // Base class for creating array type.
        template<> struct __type_rt_mt_base_t<__tidx_t::array_type>
            : __t_type_rt_mt_base_t<__tidx_t::array_type>
        {
            typedef __type_rt_mt_base_t<__tidx_t::array_type> __self_t;
            typedef rt_mt_t<__tidx_t::array_type>             __mt_t;

            typedef __t_type_rt_mt_base_t<__tidx_t::array_type> __super_t;
            using __super_t::__super_t;

            static typename __super_t::__entity_t * new_entity(
                    rt_context_t & ctx, ref_t ref, __mt_t * mt, rt_assembly_t * rt_assembly)
            {
                typedef typename __super_t::__wrap_t wrap_t;
                auto entity = &memory_t::new_obj<wrap_t>(ctx.memory)->type;
                // entity->mt = mt;

                return entity;
            }
        };

        //-------- ---------- ---------- ---------- ----------
        // generic type

        // Base class for creating generic type.
        template<> struct __type_rt_mt_base_t<__tidx_t::generic_type>
            : __t_type_rt_mt_base_t<__tidx_t::generic_type>
        {
            typedef __type_rt_mt_base_t<__tidx_t::generic_type> __self_t;
            typedef rt_mt_t<__tidx_t::generic_type>             __mt_t;

            typedef __t_type_rt_mt_base_t<__tidx_t::generic_type> __super_t;
            using __super_t::__super_t;

            static typename __super_t::__entity_t * new_entity(rt_context_t & ctx,
                ref_t ref, __mt_t * mt, rt_assembly_t * rt_assembly)
            {
                auto entity = &memory_t::new_obj<typename __super_t::__wrap_t>(ctx.memory)->type;
                // entity->mt = mt;

                return entity;
            }
        };

        //-------- ---------- ---------- ---------- ----------
    }

    #define __DefineTypeRtMt(_type)                \
        template<> struct rt_mt_t<__tidx_t::_type> : __type_rt_mt_base_t<__tidx_t::_type> { };

    __DefineTypeRtMt(type)
    __DefineTypeRtMt(array_type)
    __DefineTypeRtMt(generic_type)

    #undef __DefineTypeRtMt

    //-------- ---------- ---------- ---------- ----------

    // Runtime metadata for generic method.
    template<> struct rt_mt_t<__tidx_t::generic_method> : __rt_mt_base_t<__tidx_t::generic_method>
    {
        typedef rt_mt_t<__tidx_t::generic_method> __self_t, __mt_t;
        typedef __rt_mt_base_t<__tidx_t::generic_method> __super_t;

        using __super_t::__super_t;

        static rt_generic_method_t * new_entity(rt_context_t & ctx,
            ref_t ref, __mt_t * mt, rt_assembly_t * assembly, const __gp_mgr_t * gp_mgr = nullptr);
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime metadata for general method.
    template<> struct rt_mt_t<__tidx_t::method> : __rt_mt_base_t<__tidx_t::method>
    {
        typedef rt_mt_t<__tidx_t::method> __self_t, __mt_t;
        typedef __rt_mt_base_t<__tidx_t::method> __super_t;

        using __super_t::__super_t;

        static rt_method_t * new_entity(rt_context_t & ctx,
                            ref_t ref, __mt_t * mt, rt_assembly_t * assembly);
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime metadata for generic method.
    template<> struct rt_mt_t<__tidx_t::generic_field> : __rt_mt_base_t<__tidx_t::generic_field>
    {
        typedef rt_mt_t<__tidx_t::generic_field> __self_t, __mt_t;
        typedef __rt_mt_base_t<__tidx_t::generic_field> __super_t;

        using __super_t::__super_t;

        static rt_generic_field_t * new_entity(rt_context_t & ctx,
            ref_t ref, __mt_t * mt, rt_assembly_t * assembly, const __gp_mgr_t * gp_mgr = nullptr);
    };

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::rt

#include <modules/rt/__assembly_loader.h>
#include <modules/rt/__layout.h>

#endif // __RT_H__

