#include <core.h>

namespace X_ROOT_NS { namespace modules { namespace core {

    typedef assembly_error_code_t __e_t;

    ////////// ////////// ////////// ////////// //////////
    // assembly_error_code_t

    // Assembly error codes
    X_ENUM_INFO(assembly_error_code_t)

        // Various types of missing.
        X_D(missing,                        _T("%1% '%2%' %3% missing"))

        // Invalid ref.
        X_D(invalid_ref,                    _T("invalid '%1%' ref"))

        // Assembly duplicated.
        X_D(assembly_duplicated,            _T("assembly '%1%' duplicated"))

        // Assembly reference not found.
        X_D(assembly_reference_not_found,   _T("assembly '%1%' not found"))

        // Assembly load error.
        X_D(assembly_load_error,            _T("cannot load '%1%'"))

        // Assembly load error in specified package.
        X_D(assembly_load_error_in_package, _T("cannot load '%1%' in package '%1%'"))

        // Assembly empty.
        X_D(assembly_empty,                 _T("assembly '%1%' empty"))

        // Unexpected constant value.
        X_D(unexpected_const_value,         _T("unexpected const value '%1%'"))

        // Unexpected constant type.
        X_D(unexpected_const_type,          _T("unexpected type '%1%', expect '%2%'"))

        // Mismatched argument count.
        X_D(mismatched_argument_count,      _T("mismatched argument count"))

        // Unexpected attribute type.
        X_D(unexpected_attribute_type,      _T("unexpected attribute type: %1%"))

        // Type not found.
        X_D(type_not_found,                 _T("type '%1%' not found"))

        // Host type of method empty.
        X_D(method_host_type_empty,         _T("method '%1%' host type empty"))

        // Method reutrn type empty.
        X_D(method_return_type_empty,       _T("method '%1%' return type empty"))

        // Method argument type empty.
        X_D(method_argument_type_empty,     _T("method '%1%' argument type '%2%' empty"))

        // Method return type not matched.
        X_D(method_return_type_not_matched, _T("method '%1%' return type not matched"))

        // Type load error.
        X_D(type_load_error,                _T("cannot load type '%1%'  in assembly '%2%'"))

        // Invalid host type.
        X_D(invalid_host_type,              _T("invalid host type '%1%'"))

        // Method not found.
        X_D(method_not_found,               _T("method '%1%' not found"))

        // Format error.
        X_D(format_error,                   _T("%1%"))

        // Unsupported layout.
        X_D(unsupported_layout,             _T("cannot read assembly with layout '%1%'"))

        // Unexpected heap size.
        X_D(unexpected_heap_size,           _T("unexpected heap size"))

        // Overlimit.
        X_D(overlimit,                      _T("'%1%' overlimit"))

        // Write unexpected nest type.
        X_D(write_unexpected_nest_type,
                        _T("nest type '%1%' must be general type in the same assembly"))

        // Read unexpected nest type.
        X_D(read_unexpected_nest_type,
                        _T("nest type must be general type in the same assembly"))

        // Unexpected generic template type.
        X_D(unexpected_generic_template_type,
                        _T("generic template type '%1%' must be general_type"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Constant type.
    X_ENUM_INFO(constant_type_t)

        X_C(int8_,          _T("int8"))
        X_C(uint8_,         _T("uint8"))
        X_C(int16_,         _T("int16"))
        X_C(uint16_,        _T("uint16"))
        X_C(int32_,         _T("int32"))
        X_C(uint32_,        _T("uint32"))
        X_C(int64_,         _T("int64"))
        X_C(uint64_,        _T("uint64"))
        X_C(float_,         _T("float"))
        X_C(double_,        _T("double"))
        X_C(bool_,          _T("bool"))
        X_C(char_,          _T("char"))
        X_C(string_,        _T("string"))
        X_C(type_,          _T("type"))
        X_C(null_,          _T("null"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // mt_table_index_t

    // Metadata table index
    X_ENUM_INFO(mt_table_index_t)

        X_C(guide,              _T("guide"))
        X_C(header,             _T("header"))
        X_C(table,              _T("table"))
        X_C(assembly,           _T("assembly"))
        X_C(assembly_ref,       _T("assembly_ref"))

        X_C(constant,           _T("constant"))
        X_C(type_ref,           _T("type_ref"))
        X_C(type,               _T("type"))
        X_C(generic_type,       _T("generic_type"))
        X_C(array_type,         _T("array_type"))

        X_C(super_type,         _T("super_type"))
        X_C(nest_type,          _T("nest_type"))
        X_C(field,              _T("field"))
        X_C(method,             _T("method"))
        X_C(property,           _T("property"))
        X_C(event,              _T("event"))
        X_C(type_def,           _T("type_def"))

        X_C(field_ref,          _T("field_ref"))
        X_C(method_ref,         _T("method_ref"))
        X_C(property_ref,       _T("property_ref"))
        X_C(event_ref,          _T("event_ref"))

        X_C(generic_field,      _T("generic_field"))
        X_C(generic_method,     _T("generic_method"))
        X_C(generic_property,   _T("generic_property"))
        X_C(generic_event,      _T("generic_event"))
        X_C(generic_type_def,   _T("generic_type_def"))

        X_C(type_def_param,     _T("type_def_param"))

        X_C(generic_param,      _T("generic_param"))
        X_C(param,              _T("param"))
        X_C(generic_argument,   _T("generic_argument"))

        X_C(attribute,          _T("attribute"))
        X_C(attribute_argument, _T("attribute_argument"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // mt_type_extra_t

    // Type extra.
    X_ENUM_INFO(mt_type_extra_t)

        X_C(general,                _T("general"))              // General type.
        X_C(type_ref,               _T("type_ref"))             // Typeref
        X_C(generic,                _T("generic"))              // Generic type.
        X_C(array,                  _T("array"))                // Array type.
        X_C(generic_param,          _T("generic_param"))        // Generic param.
        X_C(generic_param_index,    _T("generic_param_index"))  // Generic param index.

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // mt_member_extra_t

    // Member extra.
    X_ENUM_INFO(mt_member_extra_t)

        X_C(internal,           _T("internal"))         // Internal, Defined in self assembly.
        X_C(import,             _T("import"))           // Import from other assemblies.
        X_C(generic,            _T("generic"))          // Generic method.

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // mt_heap_buffer_t

    // Constructor.
    mt_heap_buffer_t::mt_heap_buffer_t(memory_t * memory)
        : memory_base_t(memory)
    {
        *acquire(1) = 0;
    }

    // Acquire a buffer of specified length.
    byte_t * mt_heap_buffer_t::acquire(size_t length)
    {
        __check_grow_size(length);

        byte_t * p = __p;
        __p += length;

        return p;
    }

    // Checks memory is enought to storage specified length.
    void mt_heap_buffer_t::__check_grow_size(size_t length)
    {
        size_t need_size = __p - __buffer + length;
        if(__buffer == nullptr || need_size > __size)
        {
            while(__size < need_size)
                __size <<= 1;

            byte_t * new_buffer = this->__alloc_objs<byte_t>(__size);
            if(__buffer != nullptr)
            {
                std::copy(__buffer, __p, new_buffer);
                this->__free(__buffer);
            }

            __p = new_buffer + (__p - __buffer);
            __buffer = new_buffer;
        }
    }

    // Writes buffer to a heap.
    void mt_heap_buffer_t::write_to(xostream_t & stream)
    {
        stream.write(__buffer + 1, size() - 1);
    }

    // Destructor.
    mt_heap_buffer_t::~mt_heap_buffer_t()
    {
        if(__buffer != nullptr)
            this->__free(__buffer);
    }

    ////////// ////////// ////////// ////////// //////////
    // mt_heap_t

    // Append string.
    res_t mt_heap_t::append_string(sid_t sid)
    {
        if(sid.is_empty())
            return res_t::null;

        auto it = __map.find(sid);
        if(it != __map.end())
            return it->second;

        std::string s = string_convert<char_t, char>(sid.c_str());

        size_t current_index = __buffer.current_index();
        byte_t * buffer = __buffer.acquire(s.length() + 1);
        byte_t * bytes = (byte_t *)s.c_str();
        std::copy(bytes, bytes + s.length() + 1, buffer);

        res_t res(current_index);
        __map[sid] = res;

        return res;
    }

    // Append string.
    res_t mt_heap_t::append_string(name_t name)
    {
        return append_string((sid_t)name);
    }

    // Append mname.
    res_t mt_heap_t::append_string(const mname_t * mname)
    {
        if(mname == nullptr)
            return res_t::null;

        return append_string(mname->sid);
    }

    // Append a block.
    res_t mt_heap_t::append_block(const byte_t * bytes, size_t length)
    {
        if(bytes == nullptr || length == 0)
            return res_t::null;

        byte_t size_buffer[16];
        size_t size_length = al::to_varint<uint32_t>(length, size_buffer);

        size_t current_index = __buffer.current_index();
        byte_t * buffer = __buffer.acquire(size_length + length);

        std::copy(size_buffer, size_buffer + size_length, buffer);
        std::copy(bytes, bytes + length, buffer + size_length);

        return res_t(current_index);
    }

    // Append bytes.
    res_t mt_heap_t::append_bytes(const byte_t * bytes, size_t length)
    {
        if(bytes == nullptr || length == 0)
            return res_t::null;

        size_t current_index = __buffer.current_index();
        byte_t * buffer = __buffer.acquire(length);

        std::copy(bytes, bytes + length, buffer);

        return res_t(current_index);
    }

    // Read block.
    const byte_t * mt_heap_t::read_block(res_t res, size_t * out_length) const
    {
        const byte_t * bytes = at(res); 
        size_t byte_length;
        uint32_t length = al::from_varint<uint32_t>(bytes, &byte_length);

        al::assign_value<size_t>(out_length, length);
        return bytes + byte_length;
    }

    // Write buffer to a stream.
    void mt_heap_t::write_to(xostream_t & stream)
    {
        return __buffer.write_to(stream);
    }

    // Returns field count.
    template<size_t n>
    size_t __field_count(const __mt_field_t (&fields)[n], __lv_t lv)
    {
        return 0;
    }

    ////////// ////////// ////////// ////////// //////////
    // metadatas

    #define __DefineEntityIOFunctions                                   \
        __mt_field_t __mt_fields_t<__Mt::tidx>::fields[] = {


    #define __EndDefineEntityIOFunctions                                \
        };                                                              \
                                                                        \
        size_t __mt_fields_t<__Mt::tidx>::field_count(__lv_t lv)        \
        {                                                               \
            return __field_count(fields, lv);                           \
        }

    #define __MtMember(mt_t, member)                                    \
        __mt_field_t                                                    \
        {                                                               \
            (uint16_t)(size_t)(&((const mt_t *)nullptr)->member),       \
            (uint16_t)(sizeof(((const mt_t *)nullptr)->member))         \
        }

    #define __M(member)         __MtMember(__Mt, member)
    #define __V(lv)             __mt_field_t{lv, 0}
    #define __ToLv(field)       ((field).size == 0? (field).offset : 0)

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_guide_t

    // Metadata: guide
    __DefineEntityIOFunctions
        __M(flag),  __M(layout),  __M(__reserved),  __V(1),
    __EndDefineEntityIOFunctions

    #undef __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_header_t

    // Metadata: header
    __DefineEntityIOFunctions
        __M(heap_offset), __M(heap_size), __V(1),
    __EndDefineEntityIOFunctions

    #undef  __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_table_t

    // Metadata: table
    __DefineEntityIOFunctions
        __M(offset), __M(rows), __V(1),
    __EndDefineEntityIOFunctions

    #undef  __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_assembly_t

    // Metadata: assembly
    __DefineEntityIOFunctions
        __M(name), __M(version), __M(entry_point), __M(info), __V(1),
    __EndDefineEntityIOFunctions

    #undef  __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_assembly_ref_t

    // Metadata: Assembly ref.
    __DefineEntityIOFunctions
        __M(package), __M(name), __V(1),
    __EndDefineEntityIOFunctions

    #undef  __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_generic_argument_t

    // Metadata: Generic argument.
    __DefineEntityIOFunctions
        __M(type), __V(1),
    __EndDefineEntityIOFunctions

    #undef  __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_constant_t

    // Metadata: Constant.
    __DefineEntityIOFunctions
        __M(constant_type), __M(constant_flag), __M(data1), __M(data2), __V(1),
    __EndDefineEntityIOFunctions

    #undef  __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_type_ref_t

    // Metadata: Typeref
    __DefineEntityIOFunctions
        __M(assembly), __M(host), __M(namespace_), __M(name), __M(generic_param_count), __V(1),
    __EndDefineEntityIOFunctions

    #undef  __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_type_t

    // Metadata: Generic type
    __DefineEntityIOFunctions
        __M(name), __M(attributes), __M(decorate), __M(namespace_), __M(__tv_type),
        __M(generic_params), __M(super_types), __M(methods), __M(properties),
        __M(fields), __M(events), __M(nest_types), __V(1),
    __EndDefineEntityIOFunctions

    #undef  __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_generic_type_t

    // Metadata: Generic type
    __DefineEntityIOFunctions
        __M(template_), __M(args), __M(host), __V(1),
    __EndDefineEntityIOFunctions

    #undef  __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_array_type_t

    // Metadata: Array type.
    __DefineEntityIOFunctions
        __M(element_type),  __M(dimension), __V(1),
    __EndDefineEntityIOFunctions

    #undef  __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_field_ref_t

    // Metadata: Field ref.
    __DefineEntityIOFunctions
        __M(host), __M(type), __M(name), __V(1),
    __EndDefineEntityIOFunctions

    #undef __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_property_ref_t

    // Metadata: Property ref.
    __DefineEntityIOFunctions
        __M(host), __M(type), __M(name), __V(1),
    __EndDefineEntityIOFunctions

    #undef __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_event_ref_t

    // Metadata: Event ref.
    __DefineEntityIOFunctions
        __M(host), __M(type), __M(name), __V(1),
    __EndDefineEntityIOFunctions

    #undef __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_method_ref_t

    // Metadata: Method ref.
    __DefineEntityIOFunctions
        __M(host), __M(type), __M(name), __M(params), __M(generic_param_count), __V(1),
    __EndDefineEntityIOFunctions

    #undef __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_method_ref_param_t

    // Metadata: Method ref param.
    __DefineEntityIOFunctions
        __M(ptype), __M(type), __V(1),
    __EndDefineEntityIOFunctions

    #undef __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_super_type_t

    // Metadata: Super type.
    __DefineEntityIOFunctions
        __M(type), __V(1),
    __EndDefineEntityIOFunctions

    #undef  __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_nest_type_t

    // Metadata: Nest type.
    __DefineEntityIOFunctions
        __M(type), __V(1),
    __EndDefineEntityIOFunctions

    #undef  __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_field_t

    // Metadata: Field.
    __DefineEntityIOFunctions
        __M(name), __M(decorate), __M(type), __M(attributes), __M(init_value), __V(1),
    __EndDefineEntityIOFunctions

    #undef __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_property_t

    // Metadata: Property.
    __DefineEntityIOFunctions
        __M(name), __M(decorate), __M(type), __M(attributes),
        __M(params), __M(get_method), __M(set_method), __V(1),
    __EndDefineEntityIOFunctions

    #undef __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_method_t

    // Metadata: Method.
    __DefineEntityIOFunctions
        __M(name), __M(decorate), __M(type),
        __M(attributes), __M(generic_params), __M(params), __M(body), __M(trait), __V(1),
    __EndDefineEntityIOFunctions

    #undef __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_event_t

    // Metadata: Event.
    __DefineEntityIOFunctions
        __M(name), __M(decorate), __M(type),
        __M(attributes), __M(add_method), __M(remove_method), __V(1),
    __EndDefineEntityIOFunctions

    #undef __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_type_def_t

    // Metadata: Typedef.
    __DefineEntityIOFunctions
        __M(name), __M(decorate), __M(type), __M(params), __M(namespace_), __V(1),
    __EndDefineEntityIOFunctions

    #undef __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_generic_field_t

    // Metadata: Generic field.
    __DefineEntityIOFunctions
        __M(host), __M(template_), __V(1),
    __EndDefineEntityIOFunctions

    #undef __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_generic_method_t

    // Metadata: Generic method.
    __DefineEntityIOFunctions
        __M(host), __M(template_),  __M(args), __V(1),
    __EndDefineEntityIOFunctions

    #undef __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_generic_property_t

    // Metadata: Generic property.
    __DefineEntityIOFunctions
        __M(host), __M(template_), __V(1),
    __EndDefineEntityIOFunctions

    #undef __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_generic_event_t

    // Metadata: Generic event.
    __DefineEntityIOFunctions
        __M(host), __M(template_), __V(1),
    __EndDefineEntityIOFunctions

    #undef __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_generic_type_def_t

    // Metadata: Generic typedef.
    __DefineEntityIOFunctions
        __M(host), __M(template_), __V(1),
    __EndDefineEntityIOFunctions

    #undef __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_type_def_param_t

    // Metadata: Typeref param.
    __DefineEntityIOFunctions
        __M(name), __M(attributes), __V(1),
    __EndDefineEntityIOFunctions

    #undef __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_param_t

    // Metadata: Param.
    __DefineEntityIOFunctions
        __M(name), __M(attributes), __M(type), __M(default_value), __V(1),
    __EndDefineEntityIOFunctions

    #undef __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_generic_param_t

    // Metadata: Generic param.
    __DefineEntityIOFunctions
        __M(name), __M(attributes), __V(1),
    __EndDefineEntityIOFunctions

    #undef __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_attribute_t

    // Metadata: Attribute.
    __DefineEntityIOFunctions
        __M(type), __M(flag), __M(constructor), __M(arguments), __V(1),
    __EndDefineEntityIOFunctions

    #undef __Mt

    //-------- ---------- ---------- ---------- ----------

    #define __Mt mt_attribute_argument_t

    // Metadata: Attribute argument.
    __DefineEntityIOFunctions
        __M(name), __M(value), __M(name_type), __V(1),
    __EndDefineEntityIOFunctions

    #undef __Mt

    ////////// ////////// ////////// ////////// //////////

    typedef __field_count_t (&__field_counts_t)[__current_lv][__tidx_count];

    // Field manager initializer.
    template<__tidx_t tidx = __tidx_t::__default__>
    struct __mt_fields_manager_initialize_t
    {
        typedef mt_t<tidx>          __mt_t;
        typedef __mt_fields_t<tidx> __fields_t;

        static void initialize(__field_counts_t fields_count)
        {
            __initialize(fields_count);
            __mt_fields_manager_initialize_t<(__tidx_t)((int)tidx + 1)>::initialize(fields_count);
        }

        static void __initialize(__field_counts_t fields_count)
        {
            const size_t field_count = X_ARRAY_SIZE(__fields_t::fields);
            __mt_field_t * f0 = __fields_t::fields, *f = f0;
            int last_lv = 1;

            for(size_t index = 0; index < field_count; index++)
            {
                __mt_field_t field = __fields_t::fields[index];
                __lv_t lv = __ToLv(field);

                if(lv > 0)  // layout version
                {
                    _A(lv <= (int)__current_lv);

                    while(last_lv <= lv)
                    {
                        fields_count[last_lv - 1][(int)tidx] = (__field_count_t)(f - f0);
                        last_lv++;
                    }
                }
                else    // lv
                {
                    *f++ = field;
                }
            }

            while(last_lv <= __current_lv)
            {
                fields_count[last_lv - 1][(int)tidx] = fields_count[last_lv - 2][(int)tidx];
                last_lv++;
            }
        }
    };

    template<>
    struct __mt_fields_manager_initialize_t<__tidx_t::__end__>
    {
        static void initialize(__field_counts_t fields_count) { }
    };

    // Constructor.
    __mt_fields_manager_t::__mt_fields_manager_t()
    {
        __mt_fields_manager_initialize_t<>::initialize(__field_counts);
    }

    // Returns field count.
    size_t __mt_fields_manager_t::field_count(__tidx_t tidx, __lv_t lv)
    {
        return __field_counts[(int)lv - 1][(int)tidx];
    }

    // Returns fields manager.
    __mt_fields_manager_t __mt_fields_manager()
    {
        static __mt_fields_manager_t mt_fields_manager;
        return mt_fields_manager;
    }

    // Returns field count.
    __field_count_t __get_field_count(__tidx_t tidx, __lv_t lv)
    {
        return __mt_fields_manager().field_count(tidx, lv);
    }

    ////////// ////////// ////////// ////////// //////////

} } }
