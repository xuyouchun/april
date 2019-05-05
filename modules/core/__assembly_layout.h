#ifndef __CORE_ASSEMBLY_LAYOUT_H__
#define __CORE_ASSEMBLY_LAYOUT_H__

namespace X_ROOT_NS { namespace modules { namespace core {

    ////////// ////////// ////////// ////////// //////////
    // assembly_error_code_t

    // Assembly error codes.
    X_ENUM(assembly_error_code_t)

        // Various types of missing.
        missing,

        // Invalid ref.
        invalid_ref,

        // Entity empty.
        entity_empty,

        // Assembly duplicated.
        assembly_duplicated,

        // Assembly reference not found.
        assembly_reference_not_found,

        // Assembly load error.
        assembly_load_error,

        // Assembly load error in specified package.
        assembly_load_error_in_package,

        // Assembly empty.
        assembly_empty,

        // Unexpected constant value.
        unexpected_const_value,

        // Unexpected constant type.
        unexpected_const_type,

        // Mismatched argument count.
        mismatched_argument_count,

        // Unexpected attribute type.
        unexpected_attribute_type,

        // Type not found.
        type_not_found,

        // Host type of method empty.
        method_host_type_empty,

        // Method return type empty.
        method_return_type_empty,

        // Method argument type empty.
        method_argument_type_empty,

        // Method return type not matched.
        method_return_type_not_matched,

        // Type load error.
        type_load_error,

        // Invalid host type.
        invalid_host_type,

        // Method not found.
        method_not_found,

        // Format error.
        format_error,

        // Unsupported layout.
        unsupported_layout,

        // Unexpected heap size.
        unexpected_heap_size,

        // Overlimit.
        overlimit,

        // Write unexpected nest type.
        write_unexpected_nest_type,

        // Read unexpected nest type.
        read_unexpected_nest_type,

        // Unexpected generic template type.
        unexpected_generic_template_type,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////
    // mt_table_index_t

    X_ENUM(mt_table_index_t)

        // Guide, Magic code.
        guide = __default__,
        
        // Header
        header              =   1,
        
        // Table informations.
        table               =   2,
        
        // Assembly information.
        assembly            =   3,
        
        // Constants.
        constant            =   4,
        
        // Assembly ref.
        assembly_ref        =   5,
        
        // Type ref.
        type_ref            =   6,

        // Type.
        type                =   7,
        
        // Generic type.
        generic_type        =   8,
        
        // Array type.
        array_type          =   9,

        // Field ref.
        field_ref           =   10,
        
        // Method ref
        method_ref          =   11,
        
        // Method ref param.
        method_ref_param    =   12,
        
        // Property ref.
        property_ref        =   13,
        
        // Event ref.
        event_ref           =   14,
        
        // Super type
        super_type          =   15,
        
        // Nest type.
        nest_type           =   16,
        
        // Field.
        field               =   17,
        
        // Method
        method              =   18,
        
        // Property
        property            =   19,
        
        // Event
        event               =   20,
        
        // Typedef
        type_def            =   21,
        
        // Generic field.
        generic_field       =   22,

        // Generic method.
        generic_method      =   23,

        // Generic property.
        generic_property    =   24,

        // Generic event.
        generic_event       =   25,

        // Generic typedef.
        generic_type_def    =   26,
        
        // Typedef param.
        type_def_param      =   27,
        
        // Generic param
        generic_param       =   28,
        
        // Param
        param               =   29,
        
        // Generic argument
        generic_argument    =   30,

        // Attribute
        attribute           =   31,
        
        // Attribute argument.
        attribute_argument  =   32,
        
        // Vesion1 assembly.
        __V1 = attribute_argument,

    X_ENUM_END

    typedef mt_table_index_t __tidx_t;
    const int __tidx_count = (int)mt_table_index_t::__end__;
    const int __mttbl_count = ((int)mt_table_index_t::table + 1);

    ////////// ////////// ////////// ////////// //////////
    // mt_type_extra_t

    // Type extra data.
    X_ENUM(mt_type_extra_t)

        // General type.
        general = __default__,

        // Type Ref
        type_ref,

        // Generic type.
        generic,

        // Array type.
        array,

        // Generic param.
        generic_param,

        // Generic param index.
        generic_param_index,

        // Typedef param.
        type_def_param,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    // Returns size of object collection.
    template<typename objs_t> bool __size(objs_t * objs)
    {
        return objs == nullptr? 0 : objs->size();
    }

    ////////// ////////// ////////// ////////// //////////

    // Metadata field offset and size.
    struct __mt_field_t { uint16_t offset, size; };
    template<__tidx_t tidx> struct __mt_fields_t { };

    template<__tidx_t tidx> struct mt_t { };

    // Base class of metadata.
    template<__tidx_t _tidx, typename _entity_t = mt_t<_tidx> *, int _extra = 0>
    struct mt_base_t
    {
        static const __tidx_t tidx = _tidx;
        typedef _entity_t entity_t;
        static const int extra = _extra;
    };

    ////////// ////////// ////////// ////////// //////////

    typedef uint8_t __field_count_t;

    // Metadata fields mamanger.
    class __mt_fields_manager_t
    {
    public:
        __mt_fields_manager_t();

        // Returns field count of specified table index and version.
        size_t field_count(__tidx_t tidx, __lv_t lv);

    private:
        __field_count_t __field_counts[__current_lv][__tidx_count];
    };

    // Returns metadata fileds manager.
    __mt_fields_manager_t __mt_fields_manager();

    // Returns field count of specified table index and version.
    __field_count_t __get_field_count(__tidx_t tidx, __lv_t lv);

    ////////// ////////// ////////// ////////// //////////
    // __entity_operation_t

    // Entity operation.
    template<typename mt_t> struct __entity_operation_t
    {
        typedef __mt_fields_t<mt_t::tidx> __ef_t;

        // Returns field count of specified version.
        static size_t get_field_count(__lv_t lv)
        {
            return __get_field_count(mt_t::tidx, lv);
        }

        // Returns size of specified version.
        static size_t get_size(__lv_t lv)
        {
            size_t size = 0, field_count = get_field_count(lv);
            for(size_t index = 0; index < field_count; index++)
            {
                size += __ef_t::fields[index].size;
            }

            return size;
        }

        // Writes metadata to a stream.
        template<typename stream_t>
        static void write(stream_t & stream, const mt_t & e, int field_count)
        {
            for(size_t index = 0; index < field_count; index++)
            {
                __mt_field_t ef = __ef_t::fields[index];
                stream.write((const byte_t *)&e + ef.offset, ef.size);
            }
        }

        // Reads metadata from a stream.
        template<typename stream_t>
        static void read(stream_t & stream, mt_t & e, int field_count)
        {
            for(size_t index = 0; index < field_count; index++)
            {
                __mt_field_t ef = __ef_t::fields[index];
                if(stream.read((byte_t *)&e + ef.offset, ef.size) != ef.size)
                    throw _ECF(invalid_operation, _T("assembly format error: EOF"));
            }
        }

        // Read metadata from a stream.
        template<typename stream_t>
        static void read(stream_t & stream, mt_t & e)
        {
            read(stream, e, get_field_count(1));
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // mt_manager_t
    
    // Metadata manger object.
    class mt_manager_object_t : public object_t, public no_copy_ctor_t
    {
        typedef object_t __super_t;

    public:

        // Writes to a stream.
        virtual void write_to(xostream_t & stream, __lv_t lv) = 0;

        // Returns row size of the specified version.
        virtual size_t row_size(__lv_t lv) const = 0;

        // Returns row count.
        virtual size_t row_count() const = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    // Base class of metadata manager.
    template<__tidx_t tidx, typename _mt_t=mt_t<tidx>>
    class mt_manager_base_t : public mt_manager_object_t
    {
        typedef mt_manager_object_t __super_t;

    public:
        typedef _mt_t mt_t;
        typedef al::heap_t<mt_t> __heap_t;

        typedef typename mt_t::entity_t entity_t;
        typedef typename __heap_t::iterator   iterator;
        typedef typename __heap_t::value_type value_type;

        mt_manager_base_t() { }

        // Appends a entity.
        ref_t append(const entity_t & entity, mt_t ** out_mt)
        {
            uint32_t index = __heap.size();
            mt_t * mt = __heap.new_obj();
            al::zero_object(*mt);

            if(out_mt != nullptr)
                *out_mt = mt;

            ref_t ref(index, 1, mt_t::extra);
            __mt_map[entity] = ref;

            return ref;
        }

        // Assigns an entity at specified ref.
        void assign(ref_t ref, entity_t entity, const mt_t & mt)
        {
            _A(entity != nullptr);

            __heap[ref.index] = mt;
            __mt_map[entity] = ref;
        }

        // Returns metadata at specified ref.
        mt_t & get(ref_t ref)
        {
            return __heap[ref.index];
        }

        // Returns metadata at specified index.
        mt_t & get(size_t index)
        {
            return __heap[index];
        }

        // Returns metadata at specified ref.
        mt_t & operator [] (ref_t ref)
        {
            return get(ref);
        }

        // Returns metadata at specified index.
        mt_t & operator [] (size_t index)
        {
            return get(index);
        }

        // Enums metadatas for specified ref.
        template<typename f_t> bool each(ref_t ref, f_t f)
        {
            if(ref.count == 0)
                return true;

            if(ref.index + ref.count > __heap.size())
                throw _EC(overflow, _T("ref overflow"));

            for(size_t index = ref.index, end = index + ref.count; index < end; index++)
            {
                if(!f(index, __heap[index]))
                    return false;
            }

            return true;
        }

        // Enums metadatas.
        template<typename f_t> bool each(f_t f)
        {
            for(size_t index = 0, end = index + count(); index < end; index++)
            {
                if(!f(index, __heap[index]))
                    return false;
            }

            return true;
        }

        // Search ref for a entity.
        ref_t search_ref(const entity_t & entity)
        {
            auto it = __mt_map.find(entity);
            if(it == __mt_map.end())
                return ref_t(0, 0, 0);

            return it->second;
        }

        // Search metadata for a entity.
        mt_t * search_mt(const entity_t & entity)
        {
            ref_t ref = (*this)[entity];
            if(ref.empty())
                return nullptr;

            return &get(ref);
        }

        // Batch acquires metadatas of specified count.
        template<typename output_itor_t>
        ref_t acquire(size_t count, output_itor_t output_itor)
        {
            if(count == 0)
                return current_null();

            size_t index = __heap.new_objs(count, output_itor);
            return ref_t(index, count, mt_t::extra);
        }

        // Batch acquires metadatas of specified count.
        // Assigns with specified entities.
        template<typename entities_t, typename f_t>
        ref_t acquire(const entities_t & entities, f_t f)
        {
            typedef typename entities_t::value_type entity_t;

            if(entities.empty())
                return current_null();

            __vector_t<mt_t *> mts;
            ref_t ref = acquire(entities.size(), std::back_inserter(mts));

            for(auto && it : al::zip(ref, entities, mts))
            {
                ref_t       ref0;
                mt_t *      mt;
                entity_t    entity;

                al::tie(ref0, entity, mt) = it;

                al::zero_object(*mt);
                __mt_map[entity] = ref0;

                f(ref0, entity, mt);
            }

            return ref;
        }

        // Returns the begin iterator.
        auto begin() const { return __heap.begin(); }

        // Returns the end iterator.
        auto end()   const { return __heap.end();   }

        // Write to a stream of specified version.
        virtual void write_to(xostream_t & stream, __lv_t lv) override
        {
            typedef __entity_operation_t<mt_t> entity_operation_t;
            size_t field_count = entity_operation_t::get_field_count(lv);

            for(size_t index = 0, count = __heap.size(); index < count; index++)
            {
                entity_operation_t::write(stream, __heap[index], field_count);
            }
        }

        // Returns row size of specified version.
        virtual size_t row_size(__lv_t lv) const override
        {
            return __entity_operation_t<mt_t>::get_size(lv);
        }

        // Returns row count.
        virtual size_t row_count() const override
        {
            return __heap.size();
        }

        // Returns all metadatas count.
        size_t count() const { return __heap.size(); }

        // Returns a null ref with current index.
        ref_t current_null() { return ref_t(__heap.size(), 0); }

    private:
        std::map<entity_t, ref_t> __mt_map;
        __heap_t __heap;
    };

    //-------- ---------- ---------- ---------- ----------

    // Metadata manager.
    template<__tidx_t tidx, typename _mt_t=mt_t<tidx>>
    class mt_manager_t : public mt_manager_base_t<tidx, _mt_t>
    {
        typedef mt_manager_base_t<tidx, _mt_t> __super_t;

    public:
        using __super_t::__super_t;
    };

    typedef mt_manager_t<__tidx_t::constant> __constant_mgr_t;

    ////////// ////////// ////////// ////////// //////////
    // mt_heap_buffer_t

    // Metadata heap buffer.
    class mt_heap_buffer_t : public memory_base_t
    {
    public:

        // Constructor.
        mt_heap_buffer_t(memory_t * memory = nullptr);

        // Acquires a buffer of specified length.
        byte_t * acquire(size_t length);

        // Deconstructor.
        virtual ~mt_heap_buffer_t();

        // Returns current index.
        size_t current_index() const { return size(); }

        // Returns curreint size.
        size_t size() const { return __p - __buffer; }

        // Writes to a stream.
        void write_to(xostream_t & stream);

        // Returns buffer at specified position.
        const byte_t * at(size_t pos) const
        {
            return __buffer + pos;
        }

    private:
        byte_t * __buffer = nullptr, * __p = nullptr;
        size_t   __size = 1;

        void __check_grow_size(size_t length);
    };

    ////////// ////////// ////////// ////////// //////////
    // mt_heap_t

    // Metadata heap.
    class mt_heap_t : public object_t
    {
    public:

        // Constructor.
        mt_heap_t() { }

        // Appends a string.
        res_t append_string(sid_t sid);

        // Appends a string.
        res_t append_string(name_t name);

        // Appends a string.
        res_t append_string(const mname_t * mname);

        // Appends a block.
        res_t append_block(const byte_t * bytes, size_t length);

        // Appends bytes.
        res_t append_bytes(const byte_t * bytes, size_t length);

        // Appends a array.
        template<typename t, size_t size> res_t append_array(const t (&arr)[size])
        {
            return append_bytes((const byte_t *)arr, sizeof(t) * size);
        }

        // Appends an entity.
        template<typename t> res_t append(const t & entity)
        {
            return append_bytes((const byte_t *)&entity, sizeof(t));
        }

        // Writes to a stream.
        void write_to(xostream_t & stream);

        // Returns size of the buffer.
        size_t size() const { return __buffer.size(); }

        // Returns buffer at specified position.
        const byte_t * at(size_t pos) const { return __buffer.at(pos); }

        // Returns buffer of specified res.
        const byte_t * at(res_t res) const  { return at(res.pos); }

        // Read block of specified res.
        const byte_t * read_block(res_t res, size_t * out_length) const;

        // Returns buffer at specified offset.
        const byte_t * operator + (size_t pos) const { return at(pos); }

        // Returns buffer at specified offset.
        const byte_t * operator + (res_t res) const  { return at(res); }

        // Reads entity at specified pos.
        template<typename t> t read(size_t pos) const
        {
            if(pos + sizeof(t) >= __buffer.size())
                throw _ED(assembly_error_code_t::unexpected_heap_size);

            return *(const t *)at(pos);
        }

        // Reads entity at specified res.
        template<typename t> t read(res_t res) const { return read<t>(res.pos); }

    private:
        std::map<sid_t, res_t> __map;
        mt_heap_buffer_t       __buffer;
    };

    #define __DefineMt(tidx, _entity_t ...)                             \
        template<> struct mt_t<__tidx_t::tidx>                          \
            : mt_base_t<__tidx_t::tidx, ##_entity_t>                    \
        {

    #define __EndDefineMt(tidx)                                         \
        };                                                              \
                                                                        \
        typedef mt_t<__tidx_t::tidx> mt_##tidx##_t;                     \
                                                                        \
        template<> struct __mt_fields_t<__tidx_t::tidx>                 \
        {                                                               \
            static __mt_field_t fields[];                               \
            static size_t field_count(__lv_t lv);                       \
        };

    ////////// ////////// ////////// ////////// //////////
    // mt_guide_t

    // Metadata: guide
    __DefineMt(guide)

        guid_t      flag;           // Flag, magic code.
        __lv_t      layout;         // Assembly format version
        uint16_t    __reserved;   

    __EndDefineMt(guide)

    ////////// ////////// ////////// ////////// //////////
    // mt_header_t

    // Metadata: header.
    __DefineMt(header)

        uint32_t    heap_offset;        // Heap offset.
        uint32_t    heap_size;          // Heap size.

    __EndDefineMt(header)

    ////////// ////////// ////////// ////////// //////////
    // mt_table_t

    // Metadata: table.
    __DefineMt(table)

        uint32_t            offset;     // Metadata offset
        uint32_t            rows;       // Metadata rows.

    __EndDefineMt(table)
    
    ////////// ////////// ////////// ////////// //////////
    // mt_assembly_t

    // Metadata: assembly
    __DefineMt(assembly, assembly_t *)

        res_t       name;               // Assembly name.
        version_t   version;            // Assembly version.
        ref_t       entry_point;        // Assembly entry point.
        res_t       info;               // Assembly information.

    __EndDefineMt(assembly)

    ////////// ////////// ////////// ////////// //////////
    // mt_assembly_ref_t

    // Metadata: assembly_ref
    __DefineMt(assembly_ref, assembly_reference_t *)

        res_t       package;            // Package
        res_t       name;               // Name

    __EndDefineMt(assembly_ref)

    ////////// ////////// ////////// ////////// //////////
    // mt_generic_argument_t

    // Metadata: generic_argument
    __DefineMt(generic_argument, type_t *)

        ref_t       type;               // Argument type.

    __EndDefineMt(generic_argument)

    ////////// ////////// ////////// ////////// //////////

    #define __ConstantType(cvalue_type, vtype)                                      \
        (int8_t)(                                                                   \
            (uint8_t)(((int)cvalue_type_t::cvalue_type << 5) | (int)vtype_t::vtype) \
        )

    X_ENUM(constant_type_t)

        int8_       = __ConstantType(number, int8_),

        uint8_      = __ConstantType(number, uint8_),

        int16_      = __ConstantType(number, int16_),

        uint16_     = __ConstantType(number, uint16_),

        int32_      = __ConstantType(number, int32_),

        uint32_     = __ConstantType(number, uint32_),

        int64_      = __ConstantType(number, int64_),

        uint64_     = __ConstantType(number, uint64_),

        float_      = __ConstantType(number, float_),

        double_     = __ConstantType(number, double_),

        bool_       = __ConstantType(number, bool_),

        char_       = __ConstantType(number, char_),

        string_     = __ConstantType(string, __default__),

        type_       = __ConstantType(type, __default__),

        null_       = __ConstantType(null, __default__),

    X_ENUM_END

    #undef __ConstantType

    // Returns constant type.
    X_INLINE constexpr cvalue_type_t to_cvalue_type(constant_type_t ct)
    {
        return (cvalue_type_t)((uint8_t)ct >> 5);
    }

    // Returns value type.
    X_INLINE constexpr value_type_t to_value_type(constant_type_t ct)
    {
        return (value_type_t)((uint8_t)ct & (0xFF >> 3));
    }

    //-------- ---------- ---------- ---------- ----------

    // Constant flag.
    struct constant_flag_t
    {
        bool      extern_     : 1;      // Storage externsion.
        bool      cpl         : 1;      // Cpl
    };

    //-------- ---------- ---------- ---------- ----------
    // mt_constant_t

    // Metadata: constant
    __DefineMt(constant, cvalue_t)

        constant_type_t     constant_type;  // Constant type
        constant_flag_t     constant_flag;  // Constant flag
        byte_t              data1[2];
        byte_t              data2[4];

    __EndDefineMt(constant)

    ////////// ////////// ////////// ////////// //////////
    // mt_type_ref_t

    // Metadata: type_ref
    __DefineMt(type_ref, general_type_t *, (int)mt_type_extra_t::type_ref)

        ref_t       assembly;               // Assembly
        ref_t       host;                   // Host
        res_t       namespace_;             // Namespace
        res_t       name;                   // Name
        uint16_t    generic_param_count;    // Generic param count

    __EndDefineMt(type_ref)

    ////////// ////////// ////////// ////////// //////////
    // mt_type_t

    typedef array_object_t<decorate_value_t> __decorate_t;

    // Metadata: generic type
    __DefineMt(type, general_type_t *, (int)mt_type_extra_t::general)

        res_t           name;               // Name
        ref_t           attributes;         // Attributes
        __decorate_t    decorate;           // Decorate
        res_t           namespace_;         // Namespace

        union
        {
            struct
            {
                uint8_t         ttype   : 3;    // ttype
                uint8_t         vtype   : 5;    // vtype
            };

            uint8_t     __tv_type;
        };

        ref_t           generic_params;     // Generic params
        ref_t           super_types;        // Super types
        ref_t           methods;            // Methods
        ref_t           properties;         // Properties
        ref_t           fields;             // Fields
        ref_t           events;             // Events
        ref_t           type_defs;          // Typedefs
        ref_t           nest_types;         // Nest types.

    __EndDefineMt(type)

    ////////// ////////// ////////// ////////// //////////
    // mt_generic_type_t

    // Metadata: generic_type
    __DefineMt(generic_type, generic_type_t *, (int)mt_type_extra_t::generic)

        ref_t           template_;          // Template
        ref_t           args;               // Arguments
        ref_t           host;               // Host type

    __EndDefineMt(generic_type)

    ////////// ////////// ////////// ////////// //////////
    // mt_array_type_t

    // Metadata: array_type
    __DefineMt(array_type, array_type_t *, (int)mt_type_extra_t::array)

        ref_t           element_type;       // Element type
        dimension_t     dimension;          // Dimension.

    __EndDefineMt(array_type)

    ////////// ////////// ////////// ////////// //////////
    // mt_member_extra_t

    // Metadata: member extra
    X_ENUM(mt_member_extra_t)

        internal = __default__,             // Internal, defined in self assembly.

        import,                             // Import, defined in other assembly.

        generic,                            // Generic method.

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////
    // mt_field_ref_t

    // Metadata: Field ref
    __DefineMt(field_ref, field_t *, (int)mt_member_extra_t::import)

        ref_t       host;                   // Host type
        ref_t       type;                   // Field type
        res_t       name;                   // Field name.

    __EndDefineMt(field_ref)

    ////////// ////////// ////////// ////////// //////////
    // mt_property_ref_t

    // Metadata: Property ref
    __DefineMt(property_ref, property_t *, (int)mt_member_extra_t::import)

        ref_t       host;                   // Host type
        ref_t       type;                   // Property type
        res_t       name;                   // Property name.

    __EndDefineMt(property_ref)

    ////////// ////////// ////////// ////////// //////////
    // mt_event_ref_t

    // Metadata: Event ref
    __DefineMt(event_ref, event_t *, (int)mt_member_extra_t::import)

        ref_t       host;
        ref_t       type;
        res_t       name;

    __EndDefineMt(event_ref)

    ////////// ////////// ////////// ////////// //////////
    // mt_method_ref_t

    // Metadata: Method ref
    __DefineMt(method_ref, method_t *, (int)mt_member_extra_t::import)

        ref_t       host;                   // Host type
        ref_t       type;                   // Method type
        res_t       name;                   // Method name
        ref_t       params;                 // Params
        uint16_t    generic_param_count;    // Generic param count.

    __EndDefineMt(method_ref)

    ////////// ////////// ////////// ////////// //////////
    // mt_method_ref_param_t

    // Metadata: Method ref param.
    __DefineMt(method_ref_param, param_t *)

        param_type_t    ptype;              // Param ptype: ref out params.
        ref_t           type;               // Param type

    __EndDefineMt(method_ref_param)

    ////////// ////////// ////////// ////////// //////////
    // mt_super_type_t

    // Metadata: Super type
    __DefineMt(super_type, type_t *)

        ref_t   type;                       // Type

    __EndDefineMt(super_type)

    ////////// ////////// ////////// ////////// //////////
    // mt_nest_type_t

    // Metadata: Nest type
    __DefineMt(nest_type, type_t *)

        ref_t   type;                       // Type

    __EndDefineMt(nest_type)

    ////////// ////////// ////////// ////////// //////////
    // mt_field_t

    // Metadata: Field
    __DefineMt(field, field_t *)

        res_t           name;               // Field name
        __decorate_t    decorate;           // Decorate

        ref_t           type;               // Field type
        ref_t           attributes;         // Field attributes
        ref_t           init_value;         // Field default value.

    __EndDefineMt(field)

    ////////// ////////// ////////// ////////// //////////
    // mt_property_t

    // Metadata: Property
    __DefineMt(property, property_t *)

        res_t           name;               // Property name
        __decorate_t    decorate;           // Decorate

        ref_t           type;               // Property type
        ref_t           attributes;         // Property attributes
        ref_t           params;             // Property params, for index.

        ref_t           get_method;         // The get method
        ref_t           set_method;         // The set method.

    __EndDefineMt(property)

    ////////// ////////// ////////// ////////// //////////
    // mt_method_t

    // Metadata: Method
    __DefineMt(method, method_t *)

        res_t           name;               // Method name
        __decorate_t    decorate;           // Method decorate

        ref_t           type;               // Method type
        ref_t           attributes;         // Method attributes
        ref_t           generic_params;     // Method generic params
        ref_t           params;             // Method params

        res_t           body;               // Method body

        method_trait_t  trait;              // Method trait.

    __EndDefineMt(method)

    ////////// ////////// ////////// ////////// //////////
    // mt_event_t

    // Metadata: Event
    __DefineMt(event, event_t *)

        res_t           name;               // Event name
        __decorate_t    decorate;           // Event decorate

        ref_t           type;               // Event type
        ref_t           attributes;         // Event attributes.

        ref_t           add_method;         // The add method
        ref_t           remove_method;      // The remove method.

    __EndDefineMt(event)

    ////////// ////////// ////////// ////////// //////////
    // mt_type_def_t

    // Metadata: Typedef
    __DefineMt(type_def, type_def_t *)

        res_t           name;               // Name
        __decorate_t    decorate;           // Decorate

        ref_t           type;               // Host type
        ref_t           params;             // Params
        res_t           namespace_;         // Namespace

    __EndDefineMt(type_def)

    ////////// ////////// ////////// ////////// //////////
    // mt_generic_field_t
    __DefineMt(generic_field, impl_field_t *, (int)mt_member_extra_t::generic)

        ref_t           host;               // Host type.
        ref_t           template_;          // Field template.

    __EndDefineMt(generic_field)

    ////////// ////////// ////////// ////////// //////////
    // mt_generic_method_t

    // Metadata: Generic method
    __DefineMt(generic_method, generic_method_t *, (int)mt_member_extra_t::generic)

        ref_t           host;               // Host type
        ref_t           template_;          // Method template.
        ref_t           args;               // Method arguments.

    __EndDefineMt(generic_method)

    ////////// ////////// ////////// ////////// //////////
    // mt_generic_property_t

    // Metadata: Generic property
    __DefineMt(generic_property, impl_property_t *, (int)mt_member_extra_t::generic)

        ref_t           host;               // Host type
        ref_t           template_;          // Property template.

    __EndDefineMt(generic_property)

    ////////// ////////// ////////// ////////// //////////
    // mt_generic_event_t

    // Metadata: Generic event
    __DefineMt(generic_event, impl_event_t *, (int)mt_member_extra_t::generic)

        ref_t           host;               // Host type
        ref_t           template_;          // Event template.

    __EndDefineMt(generic_event)

    ////////// ////////// ////////// ////////// //////////
    // mt_generic_type_def_t

    // Metadata: Generic generic typedef
    __DefineMt(generic_type_def, impl_type_def_t *, (int)mt_member_extra_t::generic)

        ref_t           host;               // Host type
        ref_t           template_;          // Typedef template.

    __EndDefineMt(generic_type_def);

    ////////// ////////// ////////// ////////// //////////
    // mt_type_def_param_t

    // Metadata: Typedef param
    __DefineMt(type_def_param, type_def_param_t *, (int)mt_type_extra_t::type_def_param)

        res_t       name;                   // Param name
        ref_t       attributes;             // Param attributes

    __EndDefineMt(type_def_param)

    ////////// ////////// ////////// ////////// //////////
    // mt_param_t

    // Metadata: Param
    __DefineMt(param, param_t *)

        res_t           name;               // Param name
        ref_t           attributes;         // Param attributes
        ref_t           type;               // Param type
        param_type_t    param_type;         // Param type: ref, out, params
        ref_t           default_value;      // Default value.

    __EndDefineMt(param)

    ////////// ////////// ////////// ////////// //////////
    // mt_generic_param_t

    // Metadata: Generic param.
    __DefineMt(generic_param, generic_param_t *, (int)mt_type_extra_t::generic_param)

        res_t           name;               // Param name
        ref_t           attributes;         // Param attributes.

    __EndDefineMt(generic_param)

    ////////// ////////// ////////// ////////// //////////
    // mt_attribute_t

    // Metadata attribute flags
    struct mt_attribute_flag_t
    {
        bool    compile_time   : 1;         // Is a compile time attribute.
    };

    // Metadata: Attribute
    __DefineMt(attribute, attribute_t *)

        ref_t               type;           // Attribute type
        mt_attribute_flag_t flag;           // Attribute flag
        ref_t               constructor;    // Attribute Constructor.
        ref_t               arguments;      // Attribute arguments.

    __EndDefineMt(attribute)

    ////////// ////////// ////////// ////////// //////////
    // mt_attribute_argument_t

    // Metadata: Attribute argument
    __DefineMt(attribute_argument, argument_t *)

        res_t       name;                   // Attribute arguemnt name.
        ref_t       value;                  // Attribute argument value.

        argument_name_type_t name_type;     // Attribute argument name type: argument, field.

    __EndDefineMt(attribute_argument)

    ////////// ////////// ////////// ////////// //////////

    // Assembly layout name
    X_ENUM(assembly_layout_name_t)

        table_count,                        // Table count.

    X_ENUM_END

    typedef assembly_layout_name_t __ln_t;

    //-------- ---------- ---------- ---------- ----------

    // Default metadata template.
    struct default_mt_template_t
    {
        template<__tidx_t tidx> using mt_t = mt_t<tidx>;
    };

    //-------- ---------- ---------- ---------- ----------

    namespace
    {
        typedef default_mt_template_t __mt_template_t;

        //-------- ---------- ---------- ---------- ----------

        // Metadata build manager.
        template<typename mt_template_t, __tidx_t tidx = __tidx_t::__default__>
        struct __build_mt_manager_t
        {
            static void build(pool_t & pool, mt_manager_object_t ** mt_objects)
            {
                _A(mt_objects != nullptr);

                typedef typename mt_template_t::template mt_t<tidx> mt_t;
                typedef mt_manager_t<tidx, mt_t> mgr_t;
                mgr_t * mgr = pool.new_obj<mgr_t>();
                mt_objects[(size_t)tidx] = mgr;

                __build_mt_manager_t<mt_template_t, (__tidx_t)((int)tidx + 1)>::build(
                    pool, mt_objects
                );
            }
        };

        //-------- ---------- ---------- ---------- ----------

        template<typename mt_template_t>
        struct __build_mt_manager_t<mt_template_t, __tidx_t::__end__>
        {
            static void build(pool_t & pool, mt_manager_object_t ** mt_objects) { }
        };
    }

    // Build metadata.
    template<typename mt_template_t>
    X_INLINE void build_mt_manager(pool_t & pool, mt_manager_object_t ** mt_objects)
    {
        __build_mt_manager_t<mt_template_t>::build(pool, mt_objects);
    }

    //-------- ---------- ---------- ---------- ----------

    template<typename spool_t, __lv_t lv, typename mt_template_t>
    class assembly_layout_t;

    // Assembly layout.
    template<typename spool_t, typename _mt_template_t>
    class assembly_layout_t<spool_t, 0, _mt_template_t> : public object_t
    {
        template<__tidx_t tidx>
        using __mt_t = typename _mt_template_t::template mt_t<tidx>;

    public:
        typedef _mt_template_t mt_template_t;

        // Constructors.
        assembly_layout_t(spool_t & spool) : __spool(spool)
        {
            build_mt_manager<mt_template_t>(__this_pool, __mt_objects);
        }

        // Returns value of specified item.
        constexpr static int32_t value_of(__ln_t ln)
        {
            switch(ln)
            {
                default:
                    return 0;
            }
        }

        // Returns the ref of specified entity.
        template<__tidx_t tidx>
        ref_t ref_of(const typename mt_t<tidx>::entity_t & entity)
        {
            ref_t ref = ((mt_manager_base_t<tidx> *)__mt_objects[(int)tidx])->search_ref(entity);
            if(ref == ref_t::null)
                ref = on_load_ref(tidx, entity);

            return ref;
        }

        // Returns the ref of specified type.
        ref_t ref_of(type_t * type)
        {
            if(type == nullptr)
                return ref_t::null;

            return on_load_type_ref(type);
        }

        // Returns the res of specified string.
        res_t res_of(const char_t * s)
        {
            sid_t sid = __spool.to_sid(s);
            return __heap.append_string(sid);
        }

        // Returns string pool.
        spool_t & get_spool() const { return __spool; }

    protected:
        mt_manager_object_t * __mt_objects[(size_t)mt_table_index_t::__end__];
        mt_heap_t  __heap;

        // On load ref.
        virtual ref_t on_load_ref(__tidx_t tidx, void * entity)
        {
            return ref_t::null;
        }

        // On load type ref.
        virtual ref_t on_load_type_ref(type_t * type)
        {
            return ref_t::null;
        }

        // Returns the metadata manager of specified table index.
        template<__tidx_t tidx>
        mt_manager_t<tidx, __mt_t<tidx>> & __mt_manager()
        {
            return *(mt_manager_t<tidx, __mt_t<tidx>> *)this->__mt_objects[(size_t)tidx];
        }

        // Returns the metadata manager of specified table index.
        mt_manager_object_t * __mt_manager(int index)
        {
            return this->__mt_objects[index];
        }

    private:
        pool_t    __this_pool;
        spool_t & __spool;
    };

    //-------- ---------- ---------- ---------- ----------

    // Assembly layout.
    template<typename spool_t, typename mt_template_t>
    class assembly_layout_t<spool_t, 1, mt_template_t>
        : public assembly_layout_t<spool_t, 0, mt_template_t>
    {
        typedef assembly_layout_t<spool_t, 0, mt_template_t> __super_t;

    public:
        using __super_t::__super_t;

        // Returns the value of specified item.
        constexpr static int32_t value_of(__ln_t ln)
        {
            switch(ln)
            {
                case __ln_t::table_count:
                    return (int32_t)__tidx_t::__V1 + 1;

                default:
                    return __super_t::value_of(ln);
            }
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // Compile assembly layout.
    template<typename spool_t, __lv_t lv, typename mt_template_t>
    class compile_assembly_layout_t : public assembly_layout_t<spool_t, lv, mt_template_t>
    {
        typedef assembly_layout_t<spool_t, lv, mt_template_t> __super_t;

    public:

        // Constructor.
        compile_assembly_layout_t(xpool_t & xpool)
            : __xpool(xpool), __super_t(xpool.spool)
        { }

    protected:
        xpool_t &  __xpool;

        template<typename t> using __mt_list_t = std::vector<t>;
        template<typename t> using __vector_t = al::svector_t<t>;

        type_t * __string_type = nullptr, * __type_type = nullptr;

        // Returns type of System.String.
        type_t * __get_string_type()
        {
            return __get_internal_type(__string_type, vtype_t::string_);
        }

        // Returns type of System.Type.
        type_t * __get_type_type()
        {
            return __get_internal_type(__type_type, __to_name(__CoreTypeName(_T("Type"))));
        }

        // Returns system types.
        template<typename identity_t>   // identity_t: name_t, vtype_t, value_type_t
        type_t * __get_internal_type(identity_t identity)
        {
            type_t * type = __xpool.get_internal_type(identity);
            if(type == nullptr)
                throw _ED(assembly_error_code_t::type_not_found, identity);

            return type;
        }

        // Returns system types.
        template<typename identity_t>
        type_t * __get_internal_type(type_t *& cache, identity_t identity)
        {
            if(cache == nullptr)
            {
                cache = __xpool.get_internal_type(identity);
                if(cache == nullptr)
                    throw _ED(assembly_error_code_t::type_not_found, identity);
            }

            return cache;
        }

        // Creates a new object.
        template<typename t, typename ... args_t>
        t * __new_obj(args_t && ... args)
        {
            return __xpool.new_obj<t>(std::forward<args_t>(args) ...);
        }

    private:

        // Converts a string to name_t.
        name_t __to_name(const char_t * name)
        {
            return name_t(__xpool.spool.to_sid(name));
        }
    };

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        typedef std::vector<mt_table_t *> __mt_tables_t;

        // Context of assembly table reader.
        template<typename _mt_template_t>
        struct __assembly_table_reader_ctx_t
        {
            template<__tidx_t _tidx>
            using __mt_t = typename _mt_template_t::template mt_t<_tidx>;

            xistream_t &            stream;
            mt_manager_object_t **  mt_manager_objects;
            __mt_tables_t &         tables;
            __lv_t                  lv;

            // Returns metadata manager.
            template<__tidx_t _tidx> mt_manager_t<_tidx, __mt_t<_tidx>> & mt_manager()
            {
                return *(mt_manager_t<_tidx, __mt_t<_tidx>> *)(mt_manager_objects[(int)_tidx]);
            }
        };

        // Assembly table reader.
        template<__lv_t _lv, int _table_count, typename _mt_template_t, __tidx_t _read_to,
            int _x = ((int)_read_to + 1) - __mttbl_count
        >
        struct __assembly_table_reader_t
        {
            static const __tidx_t tidx = (__tidx_t)((int)_read_to + 1 - _x);
            typedef typename _mt_template_t::template mt_t<tidx> __mt_t;

            typedef mt_manager_t<tidx, __mt_t> __mt_manager_t;
            typedef __assembly_table_reader_ctx_t<_mt_template_t> __ctx_t;

            // Read metadata.
            static void read(__ctx_t & ctx)
            {
                __mt_manager_t & mgr = ctx.template mt_manager<tidx>();
                mt_table_t * mt_table = ctx.tables[(int)tidx - __mttbl_count];
                size_t rows = mt_table->rows;

                if(rows > 0)
                {
                    __mt_t * mts[rows];
                    mgr.acquire(rows, mts);

                    int field_count = __entity_operation_t<__mt_t>::get_field_count(_lv);
                    for(__mt_t ** p_mt = mts, ** p_mt_end = p_mt + rows; p_mt < p_mt_end; p_mt++)
                    {
                        __mt_t * mt = *p_mt;
                        __mt_t::init(*mt);
                        __entity_operation_t<__mt_t>::read(ctx.stream, *mt, field_count);
                        __mt_t::commit(*mt);
                    }
                }

                __assembly_table_reader_t<
                    _lv, _table_count, _mt_template_t, _read_to, _x - 1>
                ::read(ctx);
            }
        };

        template<
            __lv_t _lv, int _table_count, typename _mt_template_t, __tidx_t _read_to, int _x
        >
        const __tidx_t __assembly_table_reader_t<
            _lv, _table_count, _mt_template_t, _read_to, _x
        >::tidx;

        // Assemble table reader.
        template<__lv_t _lv, int _table_count, typename _mt_template_t, __tidx_t _read_to>
        struct __assembly_table_reader_t<_lv, _table_count, _mt_template_t, _read_to, 0>
        {
            typedef __assembly_table_reader_ctx_t<_mt_template_t> __ctx_t;

            static void read(__ctx_t & ctx) { }
        };
    }

    ////////// ////////// ////////// ////////// //////////
    // metadata_reader_t

    // Metadata reader, be use for read metadatas from a assembly.
    template<__lv_t _rlv, int _table_count, typename _mt_template_t, __tidx_t _read_to>
    class metadata_reader_t : public object_t
    {
        template<__tidx_t _tidx>
        using __mt_t = typename _mt_template_t::template mt_t<_tidx>;

    public:

        // Constructor.
        metadata_reader_t(xistream_t & stream, mt_manager_object_t ** mt_objects, mt_heap_t * heap)
            : __stream(stream), __mt_objects(mt_objects), __heap(heap)
        {
            _A(mt_objects != nullptr);
        }

        void read()
        {
            // Read header
            __header = __read_header();

            // Read tables
            __read_tables(__tables);

            // Read datas
            __assembly_table_reader_ctx_t<_mt_template_t> ctx { 
                __stream, this->__mt_objects, __tables
            };

            typedef __assembly_table_reader_t<
                _rlv, _table_count, _mt_template_t, _read_to
            > table_reader_t;

            table_reader_t::read(ctx);

            // Read heap
            if(__heap != nullptr)
                __read_heap();
        }

    private:
        xistream_t  & __stream;         // Assembly stream
        mt_header_t * __header;         // Metadata header
        __mt_tables_t __tables;         // Metadata tables
        mt_heap_t   * __heap;           // Heap.

        mt_manager_object_t ** __mt_objects;

        // Returns metadata manager.
        template<__tidx_t _tidx>
        mt_manager_t<_tidx, __mt_t<_tidx>> & __mt_manager()
        {
            return *(mt_manager_t<_tidx, __mt_t<_tidx>> *)__mt_objects[(size_t)_tidx];
        }

        // Reads header.
        mt_header_t * __read_header()
        {
            __mt_t<__tidx_t::header> * header;
            __mt_manager<__tidx_t::header>().append(nullptr, &header);
            __read(header);

            // TODO: check header

            return header;
        }

        // Reads tables.
        void __read_tables(__mt_tables_t & tables)
        {
            size_t tbl_count = _table_count - __mttbl_count;
            __mt_manager<__tidx_t::table>().acquire(tbl_count, std::back_inserter(tables));

            int idx = __mttbl_count;
            for(mt_table_t * mt_table : tables)
            {
                __read(mt_table);
            }
        }

        // Reads metadata.
        template<typename mt_t> void __read(mt_t * mt)
        {
            __entity_operation_t<mt_t>::read(__stream, *mt);
        }

        // Reads heap.
        void __read_heap()
        {
            byte_t buffer[1024];
            size_t length, total = __header->heap_size;

            while((length = __stream.read(buffer, al::min(sizeof(buffer), total))) > 0)
            {
                this->__heap->append_bytes(buffer, length);
                total -= length;
            }

            if(total != 0)
                throw _E(assembly_error_code_t::unexpected_heap_size);
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // Writes assembly to a stream.
    void assembly_write(xostream_t & stream, assembly_t & assembly, __lv_t lv, logger_t & logger,
                        method_compile_controller_t * controller);

    // Reads assembly from a stream.
    void assembly_read(xistream_t & stream, assembly_t & assembly,
                assembly_loader_t * loader, __lv_t * out_lv = nullptr, bool brief = false);

    // Reads Metadata from a stream. 
    template<__lv_t _rlv, int _table_count, typename _mt_template_t,
        __tidx_t _read_to = (__tidx_t)(_table_count - 1)
    > 
    void read_metadata(xistream_t & stream, mt_manager_object_t ** mt_objects, mt_heap_t * heap)
    {
        typedef metadata_reader_t<_rlv, _table_count, _mt_template_t, _read_to> reader_t;
        reader_t(stream, mt_objects, heap).read();
    }

    // Read guide from a assembly.
    void assembly_read_guide(xistream_t & stream, mt_guide_t * guide);

    // Converts a mt_constant_t to a cvalue.
    cvalue_t to_cvalue(mt_constant_t & mt);

    // Gets assembly type.
    assembly_type_t get_assembly_type(xistream_t & stream);

    // Gets assembly type.
    assembly_type_t get_assembly_type(const string_t & path);

    ////////// ////////// ////////// ////////// //////////

    // Resource reader.
    template<typename _spool_t>
    class res_reader_t : public object_t
    {
        typedef assembly_error_code_t __e_t;

    public:
        typedef _spool_t spool_t;
        typedef typename spool_t::sid_t sid_t;

        // Constructor.
        res_reader_t(mt_heap_t & heap, spool_t & spool)
            : __heap(heap), __spool(spool) { }

        // Read a constant value.
        cvalue_t read_const(const mt_constant_t & mt)
        {
            cvalue_type_t cvalue_type = to_cvalue_type(mt.constant_type);
            value_type_t  value_type  = to_value_type(mt.constant_type);

            switch(cvalue_type)
            {
                case cvalue_type_t::number:
                    return __read_const_number(mt, value_type);

                case cvalue_type_t::string:
                    return __read_const_string(mt);

                //case cvalue_type_t::type:
                //    return __read_const_type(mt);

                case cvalue_type_t::null:
                    return __read_const_null(mt);

                case cvalue_type_t::nan:
                default:
                    throw _ED(__e_t::unexpected_const_value, _T("?"));
            }
        }

        // Read sid.
        sid_t read_sid(res_t res)
        {
            auto it = __str_map.find(res);
            if(it != __str_map.end())
                return it->second;

            const char * p =  (const char *)__heap_at(res);
            string_t s = string_convert<char, char_t>(p);

            sid_t sid = __spool.to_sid(s);
            __str_map[res] = sid;

            return sid;
        }

    private:

        // Reads element from a heap.
        template<typename t> t __read_heap(res_t res)
        {
            return __heap.template read<t>(res);
        }

        // Returns heap at res.
        const byte_t * __heap_at(res_t res)
        {
            if(res.pos >= this->__heap.size())
                throw _E(__e_t::unexpected_heap_size);

            return this->__heap + res.pos;
        }

        // Reads uint64 by a metadata.
        uint64_t __read_uint64(const mt_constant_t & mt)
        {
            if(!mt.constant_flag.extern_)
            {
                uint32_t v2 = *(uint32_t *)mt.data2;
                uint32_t v1 = *(uint32_t *)mt.data1;

                if(mt.constant_flag.cpl)
                    v1 = ~v1;

                return ((uint64_t)v1 << 32) | v2;
            }
            else
            {
                return __read_heap<uint64_t>(*(res_t *)mt.data2);
            }
        }

        // Read constant number.
        cvalue_t __read_const_number(const mt_constant_t & mt, value_type_t value_type)
        {
            switch(value_type)
            {
                case value_type_t::int8_:
                    return cvalue_t(*(int8_t *)mt.data2);

                case value_type_t::uint8_:
                    return cvalue_t(*(uint8_t *)mt.data2);

                case value_type_t::int16_:
                    return cvalue_t(*(int16_t *)mt.data2);

                case value_type_t::uint16_:
                    return cvalue_t(*(uint16_t *)mt.data2);

                case value_type_t::int32_:
                    return cvalue_t(*(int32_t *)mt.data2);

                case value_type_t::uint32_:
                    return cvalue_t(*(uint32_t *)mt.data2);

                case value_type_t::int64_: {
                    uint64_t v = __read_uint64(mt);
                    return cvalue_t(*(int64_t *)&v);
                }

                case value_type_t::uint64_:
                    return cvalue_t(__read_uint64(mt));

                case value_type_t::float_:
                    return cvalue_t(*(float *)mt.data2);

                case value_type_t::double_: {
                    uint64_t v = __read_uint64(mt);
                    return cvalue_t(*(double *)&v);
                }

                case value_type_t::bool_:
                    return cvalue_t(*(bool *)mt.data2);

                case value_type_t::char_:
                    return cvalue_t(*(char_t *)mt.data2);

                default:
                    throw _ED(__e_t::unexpected_const_value, _T(""));
            }
        }

        // Read constant string.
        cvalue_t __read_const_string(const mt_constant_t & mt)
        {
            res_t res = *(res_t *)mt.data2;
            sid_t s = read_sid(res);
            return cvalue_t(s.c_str());
        }

        // Read constant null.
        cvalue_t __read_const_null(const mt_constant_t & mt)
        {
            return cvalue_t();
        }

        mt_heap_t & __heap;                     // Heap
        spool_t   & __spool;                    // String pool
        std::map<res_t, sid_t> __str_map;       // String map
    };

    ////////// ////////// ////////// ////////// //////////

    #undef __DefineMt
    #undef __EndDefineMt

} } }

#endif  // __CORE_ASSEMBLY_LAYOUT_H__

