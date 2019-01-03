#ifndef __RT_H__
#define __RT_H__

#include <core.h>
#include <algorithm.h>

namespace X_ROOT_NS { namespace modules { namespace rt {

    class rt_type_t;
    class rt_array_type_t;
    class assembly_analyzer_t;

    X_INTERFACE rt_heap_t;

    typedef int32_t rt_length_t;

    //-------- ---------- ---------- ---------- ----------

    namespace
    {
        using namespace core;
    }

    ////////// ////////// ////////// ////////// //////////

    X_ENUM(rt_error_code_t)

        assembly_not_found,

        assembly_format_error,

        method_not_found,

        field_not_found,

        local_index_out_of_range,

        argument_index_out_of_range,

        virtual_method_not_found,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(storage_type_t)

        ref, value, mixture,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    struct rt_method_t;

    struct rt_vtable_function_t
    {
        typedef rt_vtable_function_t __self_t;

        rt_vtable_function_t() = default;

        rt_vtable_function_t(uint32_t assembly_idx, uint32_t method_idx)
        {
            this->assembly_idx = assembly_idx;
            this->method_idx   = method_idx;

            this->__no_use = 0x03;

            if(sizeof(__self_t) > (arch_uint_t)&((__self_t *)nullptr)->__placeholder)
            {
                *((byte_t *)this + sizeof(__self_t) - 1) = 0xFF;
            }
        }

        rt_vtable_function_t(void * method)
        {
            this->method = method;
        }

        bool is_method() const { return ((*(byte_t *)this + sizeof(__self_t) - 1) & 0x01) == 0; }

        union
        {
            struct
            {
                uint32_t    assembly_idx    : 10;
                uint32_t    method_idx      : 20;
                uint32_t    __no_use        : 2;
                byte_t      __placeholder[0];
            };

            struct
            {
                void * method;
            };
        };
    };

    typedef rt_vtable_function_t rt_vfunction_t;

    //-------- ---------- ---------- ---------- ----------

    struct rt_vtable_t
    {
        rt_vfunction_t  functions[0];
    };

    //-------- ---------- ---------- ---------- ----------

    class rt_memory_t : public object_t
    {
    public:
        member_t ** new_members(size_t size)
        {
            return __acquire<member_t>(size);
        }

    private:
        al::heap_t<void *[]> __heap;

        template<typename t> t ** __acquire(size_t size)
        {
            return (t **)__heap.acquire(size);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    X_INTERFACE rt_heap_t
    {
        virtual rt_ref_t new_obj(rt_type_t * type) = 0;
        virtual rt_ref_t new_array(rt_array_type_t * type, dimension_t dimension,
                                   const array_length_t * lengths) = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    class rt_object_t
    {
    public:
        rt_object_t() : __value__(0) { }

        rt_vtable_t * vtable ;

        union
        {
            struct
            {
                bool        external : 1;
                bool        fixed    : 1;
                bool        static_  : 1;

                byte_t      gc_version;
                byte_t      sync_lock;
                byte_t      no_use;
            };

            uint32_t __value__;
        };
    };
    
    //-------- ---------- ---------- ---------- ----------

    typedef int32_t rt_size_t;

    ////////// ////////// ////////// ////////// //////////

    class rt_string_t : public rt_object_t
    {
    public:
        rt_string_t(const char_t * s, rt_size_t length)
            : s(s), length(length)
        {
            _A(s != nullptr);
            this->external = true;
        }

        rt_size_t length = 0;
        const char_t * s = nullptr;

        bool operator <  (const rt_string_t & str) const { return compare(str) <  0; }
        bool operator >  (const rt_string_t & str) const { return compare(str) >  0; }
        bool operator <= (const rt_string_t & str) const { return compare(str) <= 0; }
        bool operator >= (const rt_string_t & str) const { return compare(str) >= 0; }
        bool operator == (const rt_string_t & str) const { return equals(str);  }
        bool operator != (const rt_string_t & str) const { return !equals(str); }

        int  compare(const rt_string_t & str) const;
        bool equals(const rt_string_t & str) const;

        operator string_t() const
        {
            return !s? _T("") : s;
        }

    private:
        bool __reference_equals(const rt_string_t & str) const;
        int __compare(const rt_string_t & str) const;
    };

    //-------- ---------- ---------- ---------- ----------

    struct rt_sid_t : compare_operators_t<rt_sid_t, const rt_string_t *>
    {
        typedef const rt_string_t * __sid_value_t;

        rt_sid_t() = default;
        rt_sid_t(__sid_value_t value) : value(value) { }

        __sid_value_t value = nullptr;

        operator __sid_value_t () const { return value; }
        operator string_t() const { return !value? _T("") : (string_t)*value; }

        const char_t * c_str() const { return value? value->s : nullptr; }
    };

    //-------- ---------- ---------- ---------- ----------

    class rt_spool_t : public object_t
    {
    public:
        typedef rt_sid_t sid_t;

        rt_sid_t to_sid(const rt_string_t & s);
        rt_sid_t to_sid(const string_t & s);

    private:
        std::set<const rt_string_t> __string_set;
        al::heap_t<char_t[]> __heap;
    };

    ////////// ////////// ////////// ////////// //////////

    class rt_context_t : public object_t
    {
    public:
        rt_context_t(memory_t * memory, rt_heap_t * heap)
            : memory(memory), heap(heap)
        {
            _A(heap != nullptr);
        }

        rt_heap_t * heap;
        memory_t  * memory;
        rt_memory_t rt_memory;

        rt_spool_t spool;
    };

    ////////// ////////// ////////// ////////// //////////

    class __unknown_entity_t { };

    template<__tidx_t tidx> struct __rt_t
    {
        typedef __unknown_entity_t entity_t;
    };

    #define __RtEntity(tidx, _entity_t)             \
        class _entity_t;                            \
        template<> struct __rt_t<__tidx_t::tidx>    \
        {                                           \
            typedef _entity_t entity_t;             \
        };

    __RtEntity(assembly,            rt_assembly_t)
    __RtEntity(assembly_ref,        rt_assembly_ref_t)
    __RtEntity(type,                rt_general_type_t)
    __RtEntity(generic_type,        rt_generic_type_t)
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

    template<__tidx_t tidx>
    using rt_entity_t = typename __rt_t<tidx>::entity_t;

    ////////// ////////// ////////// ////////// //////////

    template<__tidx_t tidx> struct rt_mt_t;

    namespace
    {
        template<__tidx_t tidx, typename _self_t = rt_mt_t<tidx>>
        struct __rt_mt_base_t : mt_t<tidx>
        {
            typedef _self_t             __self_t;
            typedef mt_t<tidx>          __super_t;
            typedef rt_entity_t<tidx>   __entity_t;

            using __super_t::__super_t;

            __entity_t * rt_object;

            static void init(__self_t & self)
            {
                self.rt_object = nullptr;
            }

            static __entity_t * new_entity(memory_t * memory)
            {
                return memory_t::new_obj<__entity_t>(memory);
            }

            static void commit(__self_t & self) { }
        };
    }

    //-------- ---------- ---------- ---------- ----------

    template<__tidx_t tidx> struct rt_mt_t : __rt_mt_base_t<tidx>
    {
        typedef __rt_mt_base_t<tidx> __super_t;

        using __super_t::__super_t;
    };

    //-------- ---------- ---------- ---------- ----------

    template<> struct rt_mt_t<__tidx_t::constant> : mt_t<__tidx_t::constant>
    {
        typedef rt_mt_t<__tidx_t::constant> __self_t;
        typedef mt_t<__tidx_t::constant>    __super_t;

        static void init(__self_t & self) { }
        static void commit(__self_t & self) { }
    };

    //-------- ---------- ---------- ---------- ----------

    template<typename _entity_t> _entity_t * __new_entity(memory_t * memory)
    {
        return memory_t::new_obj<_entity_t>(memory);
    }

    ////////// ////////// ////////// ////////// //////////

    template<__tidx_t tidx>
    class rt_metadata_object_t
    {
    public:
        rt_mt_t<tidx> * mt = nullptr;

        rt_mt_t<tidx> * operator ->() { return mt; }
    };

    //-------- ---------- ---------- ---------- ----------

    class rt_member_t : public rt_object_t
    {
    public:

    };

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(rt_type_kind_t)

        general, generic, array,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(search_method_options_t)

        default_        = __default__,

        only_virtual    = 1 << 0,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    struct method_prototype_t;

    class rt_type_t : public rt_member_t
    {
    public:
        virtual void pre_new(assembly_analyzer_t & analyzer) = 0;

        virtual rt_sid_t get_name(rt_assembly_t * assembly) = 0;
        virtual rt_type_kind_t get_kind() = 0;

        virtual ttype_t get_ttype(assembly_analyzer_t & analyzer) { return ttype_t::__unknown__; }
        virtual rt_type_t * get_base_type(assembly_analyzer_t & analyzer,
                                       rt_assembly_t ** out_assembly) = 0;

        virtual int get_method_offset(assembly_analyzer_t & analyzer, ref_t method_ref) = 0;

        typedef std::function<bool (ref_t, rt_field_t *)> each_field_t;
        virtual void each_field(assembly_analyzer_t & analyzer, each_field_t f) = 0;

        typedef std::function<bool (ref_t, rt_method_t *)> each_method_t;
        virtual void each_method(assembly_analyzer_t & analyzer, each_method_t f) = 0;

        virtual ref_t search_method(assembly_analyzer_t & analyzer,
                    method_prototype_t & prototype,
                    search_method_options_t options = search_method_options_t::default_) = 0;

        msize_t get_size(assembly_analyzer_t & analyzer, storage_type_t * out_storage_type);
        msize_t get_size(assembly_analyzer_t & analyzer);

        virtual msize_t get_variable_size(assembly_analyzer_t & analyzer,
                                            storage_type_t * out_storage_type) = 0;

        msize_t get_size() { return __size; }

    protected:
        virtual msize_t on_caculate_size(assembly_analyzer_t & analyzer,
                                        storage_type_t * out_storage_type) = 0;

        msize_t         __size = unknown_msize;
        storage_type_t  __storage_type;
    };

    //-------- ---------- ---------- ---------- ----------

    class rt_general_type_t : public rt_type_t
                            , public rt_metadata_object_t<__tidx_t::type>
    {
    public:
        virtual void pre_new(assembly_analyzer_t & analyzer) override;

        virtual rt_sid_t get_name(rt_assembly_t * assembly) override;
        virtual rt_type_kind_t get_kind() override { return rt_type_kind_t::general; }

        virtual ttype_t get_ttype(assembly_analyzer_t & analyzer) override;
        virtual rt_type_t * get_base_type(assembly_analyzer_t & analyzer,
                                        rt_assembly_t ** out_assembly) override;

        virtual int get_method_offset(assembly_analyzer_t & analyzer, ref_t method_ref) override;

        virtual void each_field(assembly_analyzer_t & analyzer, each_field_t f) override;
        virtual void each_method(assembly_analyzer_t & analyzer, each_method_t f) override;

        virtual ref_t search_method(assembly_analyzer_t & analyzer,
                    method_prototype_t & prototype, search_method_options_t options) override;

        virtual msize_t get_variable_size(assembly_analyzer_t & analyzer,
                                            storage_type_t * out_storage_type) override;

    protected:
        virtual msize_t on_caculate_size(assembly_analyzer_t & analyzer,
                                        storage_type_t * out_storage_type) override;

    private:
        void __build_vtbl(assembly_analyzer_t & analyzer);
    };

    //-------- ---------- ---------- ---------- ----------

    class rt_type_ref_t : public rt_object_t
                        , public rt_metadata_object_t<__tidx_t::type_ref>
    {
    public:

    };

    //-------- ---------- ---------- ---------- ----------

    class rt_generic_type_t : public rt_type_t
                            , public rt_metadata_object_t<__tidx_t::generic_type>
    {
    public:
        virtual void pre_new(assembly_analyzer_t & analyzer) override { }

        virtual rt_sid_t get_name(rt_assembly_t * assembly) override { X_UNIMPLEMENTED(); }
        virtual rt_type_kind_t get_kind() override { return rt_type_kind_t::generic; }

        virtual ttype_t get_ttype(assembly_analyzer_t & analyzer) override;
        virtual rt_type_t * get_base_type(assembly_analyzer_t & analyzer,
                                        rt_assembly_t ** out_assembly) override;
        virtual int get_method_offset(assembly_analyzer_t & analyzer, ref_t method_ref) override;
        virtual void each_field(assembly_analyzer_t & analyzer, each_field_t f) override { }
        virtual void each_method(assembly_analyzer_t & analyzer, each_method_t f) override { }

        virtual ref_t search_method(assembly_analyzer_t & analyzer,
                    method_prototype_t & prototype, search_method_options_t options) override;

        virtual msize_t get_variable_size(assembly_analyzer_t & analyzer,
                                            storage_type_t * out_storage_type) override;
    protected:
        virtual msize_t on_caculate_size(assembly_analyzer_t & analyzer,
                                storage_type_t * out_storage_type) override;
    };

    //-------- ---------- ---------- ---------- ----------

    class rt_array_type_t : public rt_type_t
                          , public rt_metadata_object_t<__tidx_t::array_type>
    {
    public:
        virtual void pre_new(assembly_analyzer_t & analyzer) override;

        virtual rt_sid_t get_name(rt_assembly_t * assembly) override { X_UNIMPLEMENTED(); }
        virtual rt_type_kind_t get_kind() override { return rt_type_kind_t::array; }

        virtual ttype_t get_ttype(assembly_analyzer_t & analyzer) override;
        virtual rt_type_t * get_base_type(assembly_analyzer_t & analyzer,
                                        rt_assembly_t ** out_assembly) override;
        virtual int get_method_offset(assembly_analyzer_t & analyzer, ref_t method_ref) override;
        virtual void each_field(assembly_analyzer_t & analyzer, each_field_t f) override { }
        virtual void each_method(assembly_analyzer_t & analyzer, each_method_t f) override { }

        virtual ref_t search_method(assembly_analyzer_t & analyzer,
                    method_prototype_t & prototype,
                    search_method_options_t options = search_method_options_t::default_) override;

        virtual msize_t get_variable_size(assembly_analyzer_t & analyzer,
                                            storage_type_t * out_storage_type) override;

        rt_type_t * get_element_type() { return __element_type; }

    protected:
        virtual msize_t on_caculate_size(assembly_analyzer_t & analyzer,
                                storage_type_t * out_storage_type) override;

        rt_type_t * __element_type = nullptr;
    };

    //-------- ---------- ---------- ---------- ----------

    struct rt_bytes_t
    {
        const byte_t * bytes    = nullptr;
        size_t         length   = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    class rt_method_t : public rt_member_t
                      , public rt_metadata_object_t<__tidx_t::method>
    {
    public:

    };

    //-------- ---------- ---------- ---------- ----------

    class rt_method_ref_t : public rt_object_t
                          , public rt_metadata_object_t<__tidx_t::method_ref>
    {
    public:

    };

    //-------- ---------- ---------- ---------- ----------

    class rt_field_t : public rt_member_t
                     , public rt_metadata_object_t<__tidx_t::field>
    {
    public:
        msize_t     offset = unknown_msize;
    };

    //-------- ---------- ---------- ---------- ----------

    class rt_field_ref_t : public rt_object_t
                         , public rt_metadata_object_t<__tidx_t::field_ref>
    {
    public:

    };

    //-------- ---------- ---------- ---------- ----------

    class rt_event_t : public rt_member_t
                     , public rt_metadata_object_t<__tidx_t::event>
    {
    public:

    };

    //-------- ---------- ---------- ---------- ----------

    class rt_event_ref_t : public rt_object_t
                         , public rt_metadata_object_t<__tidx_t::event_ref>
    {
    public:

    };

    //-------- ---------- ---------- ---------- ----------

    class rt_property_t : public rt_member_t
                        , public rt_metadata_object_t<__tidx_t::property>
    {
    public:

    };

    //-------- ---------- ---------- ---------- ----------

    class rt_super_type_t : public rt_object_t
                          , public rt_metadata_object_t<__tidx_t::super_type>
    {
    public:

    };

    //-------- ---------- ---------- ---------- ----------

    class rt_nest_type_t : public rt_object_t
                         , public rt_metadata_object_t<__tidx_t::nest_type>
    {
    public:

    };

    //-------- ---------- ---------- ---------- ----------

    class rt_property_ref_t : public rt_object_t
                            , public rt_metadata_object_t<__tidx_t::property_ref>
    {
    public:

    };

    //-------- ---------- ---------- ---------- ----------

    class rt_generic_param_t : public rt_object_t
                             , public rt_metadata_object_t<__tidx_t::generic_param>
    {
    public:
        rt_mt_t<__tidx_t::generic_param> * mt = nullptr;
    };

    //-------- ---------- ---------- ---------- ----------

    class rt_param_t : public rt_object_t
                     , public rt_metadata_object_t<__tidx_t::param>
    {
    public:

    };

    //-------- ---------- ---------- ---------- ----------

    class rt_generic_argument_t : public rt_object_t
                                , public rt_metadata_object_t<__tidx_t::generic_argument>
    {
    public:

    };

    //-------- ---------- ---------- ---------- ----------

    class rt_attribute_t : public rt_object_t
                         , public rt_metadata_object_t<__tidx_t::attribute>
    {
    public:

    };

    //-------- ---------- ---------- ---------- ----------

    class rt_attribute_argument_t : public rt_object_t
                                  , public rt_metadata_object_t<__tidx_t::attribute_argument>
    {
    public:

    };

    //-------- ---------- ---------- ---------- ----------

    class rt_method_ref_param_t : public rt_object_t
                                , public rt_metadata_object_t<__tidx_t::method_ref_param>
    {
    public:

    };

    //-------- ---------- ---------- ---------- ----------

    class rt_assembly_ref_t : public rt_object_t
                            , public rt_metadata_object_t<__tidx_t::assembly_ref>
    {
    public:

    };

    //-------- ---------- ---------- ---------- ----------

    struct rt_ptype_t
    {
        rt_ptype_t() = default;
        rt_ptype_t(rt_type_t * type, param_type_t param_type)
            : type(type), param_type(param_type) { }

        rt_type_t *     type = nullptr;
        param_type_t    param_type = param_type_t::__default__;
    };

    //-------- ---------- ---------- ---------- ----------

    class rt_assembly_t : public rt_object_t
                        , public rt_metadata_object_t<__tidx_t::assembly>
                        , public object_t
    {
    public:
        virtual rt_sid_t to_sid(res_t s) = 0;
        virtual rt_sid_t get_name() = 0;

        virtual rt_method_t * get_entry_point() = 0;
        virtual rt_bytes_t get_method_body(rt_method_t * method) = 0;
        virtual rt_sid_t get_name(rt_type_t * type) = 0;
        virtual rt_sid_t get_name(rt_method_t * method) = 0;
        virtual rt_sid_t get_name(rt_field_t * field) = 0;

        virtual rt_method_ref_t * get_method_ref(ref_t ref) = 0;
        virtual rt_field_ref_t * get_field_ref(ref_t ref) = 0;
        virtual rt_type_ref_t * get_type_ref(ref_t ref) = 0;
        virtual rt_assembly_ref_t * get_assembly_ref(ref_t ref) = 0;

        virtual rt_sid_t get_name(rt_assembly_ref_t * assembly_ref) = 0;
        virtual rt_sid_t get_package(rt_assembly_ref_t * assembly_ref) = 0;

        virtual rt_general_type_t * get_type(ref_t ref) = 0;
        virtual rt_general_type_t * get_type(rt_sid_t ns, rt_sid_t name,
                                            int generic_param_count) = 0;
        virtual rt_sid_t get_name(rt_general_type_t * type) = 0;

        virtual rt_array_type_t * get_array_type(ref_t ref) = 0;
        virtual rt_generic_type_t * get_generic_type(ref_t ref) = 0;

        virtual rt_method_t * get_method(ref_t ref) = 0;
        virtual rt_field_t  * get_field(ref_t ref) = 0;
        virtual rt_param_t  * get_param(ref_t param_ref) = 0;

        virtual rt_super_type_t * get_super_type(ref_t ref) = 0;
        virtual rt_nest_type_t  * get_nest_type(ref_t ref) = 0;

        template<__tidx_t tidx>
        using each_func_t = std::function<bool(int index, rt_mt_t<tidx> & mt)>;

        typedef each_func_t<__tidx_t::method> __each_method_func_t;
        virtual bool each_methods(ref_t methods, __each_method_func_t f) = 0;

        typedef each_func_t<__tidx_t::field> __each_field_func_t;
        virtual bool each_fields(ref_t field, __each_field_func_t f) = 0;

        typedef each_func_t<__tidx_t::param> __each_param_func_t;
        virtual bool each_params(ref_t params, __each_param_func_t f) = 0;

        virtual rt_general_type_t * get_host_by_method_ref(ref_t method_ref) = 0;
        virtual rt_general_type_t * get_host_by_method(rt_method_t * method) = 0;
        virtual rt_general_type_t * get_host_by_field_ref(ref_t field_ref) = 0;
        virtual rt_general_type_t * get_host_by_field(rt_field_t * field) = 0;

        virtual rt_method_ref_param_t * get_method_ref_param(ref_t ref) = 0;

        int index = -1;
    };

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        template<__tidx_t tidx>
        struct __type_wrap_t
        {
            typedef rt_entity_t<tidx> __entity_t;

            rt_vtable_t *  vtbl = nullptr;
            __entity_t     type;
        };
    }

    template<> struct rt_mt_t<__tidx_t::type> : __rt_mt_base_t<__tidx_t::type>
    {
        typedef __rt_mt_base_t<tidx>    __super_t;
        typedef __type_wrap_t<tidx>     __wrap_t;

        using __super_t::__super_t;

        static __entity_t * new_entity(memory_t * memory)
        {
            return &memory_t::new_obj<__wrap_t>(memory)->type;
        }
    };

    template<typename _type_t>
    rt_vtable_t * get_vtable(_type_t * type)
    {
        _A(type != nullptr);

        return *((rt_vtable_t **)type - 1);
    }

    template<typename _type_t>
    void set_vtable(_type_t * type, rt_vtable_t * vtbl)
    {
        _A(type != nullptr);

        *((rt_vtable_t **)type - 1) = vtbl;
    }

    ////////// ////////// ////////// ////////// //////////

    template<typename _key_t, typename ... _values_t>
    class rt_pool_partial_t
    {
        typedef _key_t __key_t;
        typedef std::tuple<_values_t ...> __value_t;

    public:
        __value_t * get(const __key_t & key)
        {
            auto it = __map.find(key);
            if(it == __map.end())
                return nullptr;

            return &it->second;
        }

        template<typename creator_t>
        __value_t get(const __key_t & key, creator_t f)
        {
            __value_t * value = get(key);
            if(value != nullptr)
                return *value;

            __value_t new_value = f();
            __map[key] = new_value;

            return new_value;
        }

        void set(const __key_t & key, _values_t && ... values)
        {
            __map[key] = __value_t(std::forward<_values_t>(values) ...);
        }

    private:
        std::map<__key_t, __value_t> __map;
    };

    //-------- ---------- ---------- ---------- ----------

    namespace
    {
        typedef rt_pool_partial_t<
            std::tuple<rt_method_t *, rt_assembly_t *, ref_t>,
            rt_method_t *, rt_general_type_t *, rt_assembly_t *
        > __rt_method_pool_partial_t;

        typedef rt_pool_partial_t<
            std::tuple<rt_field_t *, rt_assembly_t *, ref_t>,
            rt_field_t *, rt_general_type_t *, rt_assembly_t *
        > __rt_field_pool_partial_t;
    }

    class rt_pool_t : public object_t, public no_copy_ctor_t
                    , __rt_method_pool_partial_t
                    , __rt_field_pool_partial_t
    {
    public:
        using __rt_method_pool_partial_t::get;
        using __rt_field_pool_partial_t::get;
    };

    //-------- ---------- ---------- ---------- ----------

    X_INTERFACE rt_assembly_provider_t : public object_t
    {
        virtual rt_assembly_t * load_main_assembly() = 0;
        virtual rt_assembly_t * load_assembly(const string_t & package, const string_t & name) = 0;
        virtual rt_assembly_t * at(int index) = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    struct method_prototype_t
    {
        typedef al::small_vector_t<rt_ptype_t, 8> __ptypes;

        rt_sid_t        name;
        rt_type_t *     return_type;
        __ptypes        arg_types;
        int             generic_param_count;

        int param_count() const { return arg_types.size(); }
    };

    //-------- ---------- ---------- ---------- ----------

    class assembly_analyzer_t : public object_t
    {
        typedef assembly_analyzer_t __self_t;
        typedef object_t            __super_t;

    public:
        assembly_analyzer_t(rt_pool_t & pool, rt_assembly_t * current,
            rt_assembly_provider_t * assembly_provider, memory_t * memory)
            : __pool(pool), current(current), assembly_provider(assembly_provider)
            , memory(memory)
        {
            _A(current != nullptr);
            _A(assembly_provider != nullptr);
            _A(memory != nullptr);
        }

        assembly_analyzer_t(assembly_analyzer_t & prototype, rt_assembly_t * current)
            : assembly_analyzer_t(prototype.__pool, current, prototype.assembly_provider,
              prototype.memory)
        { }

        rt_assembly_t *             current;
        rt_assembly_provider_t *    assembly_provider;
        memory_t *                  memory;

        rt_sid_t to_sid(res_t res) { return __to_sid(res); }

        rt_assembly_t * get_assembly_by_method_ref(ref_t ref,
            rt_general_type_t ** out_type = nullptr, rt_method_ref_t ** out_method_ref = nullptr);

        rt_assembly_t * get_assembly_by_field_ref(ref_t ref,
            rt_general_type_t ** out_type = nullptr, rt_field_ref_t ** out_field_ref = nullptr);

        rt_assembly_t * get_assembly(rt_method_ref_t * rt, rt_general_type_t ** out_type = nullptr);
        rt_assembly_t * get_assembly(rt_field_ref_t * rt, rt_general_type_t ** out_type = nullptr);
        rt_assembly_t * get_assembly(mt_method_ref_t * mt, rt_general_type_t ** out_type = nullptr);
        rt_assembly_t * get_assembly(mt_field_ref_t * mt, rt_general_type_t ** out_type = nullptr);
        rt_assembly_t * get_assembly_by_type_ref(ref_t ref, rt_general_type_t ** out_type = nullptr);
        rt_assembly_t * get_assembly(rt_type_ref_t * rt, rt_general_type_t ** out_type = nullptr);

        rt_assembly_t * get_assembly(mt_type_ref_t * mt);
        rt_assembly_t * get_assembly_by_assembly_ref(ref_t ref);
        rt_assembly_t * get_assembly(rt_assembly_ref_t * rt);
        rt_assembly_t * get_assembly(mt_assembly_ref_t * mt);

        rt_general_type_t * get_general_type(ref_t type_ref, rt_assembly_t ** out_assembly = nullptr);
        rt_type_t * get_type(ref_t type_ref, rt_assembly_t ** out_assembly = nullptr);

        rt_method_t * get_method(ref_t method_ref,
            rt_general_type_t ** out_type = nullptr, rt_assembly_t ** out_assembly = nullptr
        );

        rt_field_t * get_field(ref_t field_ref,
            rt_general_type_t ** out_type = nullptr, rt_assembly_t ** out_assembly = nullptr
        );

        const char_t * to_cstr(res_t res)
        {
            return __to_sid(res).c_str();
        }

        rt_pool_t & get_pool() { return __pool; }
        msize_t size_of(ref_t type, storage_type_t * out_storage_type = nullptr);
        msize_t size_of(rt_type_t * type, storage_type_t * out_storage_type = nullptr);

        msize_t variable_size_of(ref_t type_ref, storage_type_t * out_storage_type = nullptr);
        msize_t variable_size_of(rt_type_t * type, storage_type_t * out_storage_type = nullptr);

        rt_type_t * get_base_type(rt_type_t * type,
                                  rt_assembly_t ** out_assembly = nullptr);

        msize_t get_type_size(ref_t type_ref);
        msize_t get_field_offset(ref_t field_ref);

        int get_virtual_method_offset(assembly_analyzer_t & analyzer, rt_type_t * rt_type,
                                                                      ref_t method_ref);

        void get_prototype(ref_t method_ref, method_prototype_t * out_prototype);
        void get_prototype(rt_method_t * rt_method, method_prototype_t * out_prototype);

    private:
        rt_pool_t & __pool;

        bool __is_current(rt_assembly_t * assembly) { return assembly == this->current; }
        rt_sid_t __to_sid(res_t res) { return current->to_sid(res); }

        bool __compare_method(rt_assembly_t * rt_assembly, rt_mt_t<__tidx_t::method> & mt,
            rt_sid_t name, int generic_param_count, rt_type_t * return_type,
            int param_count, rt_ptype_t * params);

        __self_t __bind(rt_assembly_t * current)
        {
            return __self_t(*this, current);
        }

        rt_method_t * __get_method(ref_t method_ref,
            rt_general_type_t ** out_type, rt_assembly_t ** out_assembly
        );

        rt_field_t * __get_field(ref_t field_ref,
            rt_general_type_t ** out_type, rt_assembly_t ** out_assembly
        );

        string_t __join_method_name(rt_assembly_t * assembly,
            rt_general_type_t * type, mt_method_ref_t * mt_method, rt_ptype_t * ptypes);

        string_t __join_field_name(rt_assembly_t * assembly,
            rt_general_type_t * type, mt_field_ref_t * mt_field_ref);
    };

    ////////// ////////// ////////// ////////// //////////

    string_t join_assembly_name(const string_t & package, const string_t & name);

    rt_vtable_t * build_vtable(memory_t * memory, assembly_analyzer_t & analyzer,
                                                  rt_type_t & rt_type);

    ////////// ////////// ////////// ////////// //////////

} } }

#include <modules/rt/__assembly_loader.h>
#include <modules/rt/__layout.h>

#endif // __RT_H__

