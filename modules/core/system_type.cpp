#include <core.h>

namespace X_ROOT_NS { namespace modules { namespace core {

    typedef compile_error_code_t __e_t;

    ////////// ////////// ////////// ////////// //////////

    // Creates a object.
    template<typename t, typename ... args_t>
    t * __new(xpool_t & xpool, args_t && ... args)
    {
        return memory_t::new_obj<t>(xpool.memory, xpool, std::forward<args_t>(args) ...);
    }

    ////////// ////////// ////////// ////////// //////////

    // Interface for implement a type in core sysem.
    class __type_impls_t
    {
    public:
        __type_impls_t(xpool_t & xpool) : __xpool(xpool) { }

        // Appends a type impl with name.
        type_impl_t * append(const string_t & name, type_impl_t * impl)
        {
            _A(impl != nullptr);
            __impls[__to_name(name)] = impl;

            return impl;
        }

        // Appends a type impl by impl type.
        template<typename _impl_t> _impl_t * append()
        {
            return (_impl_t *)append(_impl_t::type_name, __new<_impl_t>(__xpool));
        }

        // Gets impl by name.
        type_impl_t * get_impl(name_t name)
        {
            auto it = __impls.find(name);
            if (it != __impls.end())
                return it->second;

            return nullptr;
        }

        // Gets impl by name.
        type_impl_t * get_impl(const string_t & name)
        {
            return get_impl(__to_name(name));
        }

    private:
        xpool_t & __xpool;
        std::map<name_t, type_impl_t *> __impls;

        // Converts a string to name_t by xpool.
        name_t __to_name(const string_t & s) { return name_t(__xpool.spool.to_sid(s)); }
    };

    ////////// ////////// ////////// ////////// //////////

    // System type's impl.
    class __system_type_impl_t : public type_impl_t
    {
        typedef type_impl_t __super_t;

    public:
        __system_type_impl_t(xpool_t & xpool) : __xpool(xpool) { }

        // Commits it.
        virtual void commit(eobject_commit_context_t & ctx, eobject_t * bind_object) override { }

    protected:
        xpool_t & __xpool;

        // Converts a string to name_t by xpool.
        name_t __to_name(const string_t & s) { return name_t(__xpool.spool.to_sid(s)); }
    };

    ////////// ////////// ////////// ////////// //////////

    // Internal type's impl.
    class __internal_type_impl_t : public __system_type_impl_t
    {
        typedef __system_type_impl_t __super_t;

    public:

        // Constructor.
        __internal_type_impl_t(xpool_t & xpool, vtype_t vtype = vtype_t::mobject_)
            : __system_type_impl_t(xpool), vtype(vtype) { }

        // Commits it.
        virtual void commit(eobject_commit_context_t & ctx, eobject_t * bind_object) override
        {
            general_type_t * type = as<general_type_t *>(bind_object);

            if (type != nullptr)
            {
                type->vtype = vtype;
                bind_type = type;
            }

            __super_t::commit(ctx, bind_object);
        }

        vtype_t vtype;
        general_type_t * bind_type = nullptr;
    };

    //-------- ---------- ---------- ---------- ----------

    // Core type's impl.
    template<vtype_t _vtype>
    class __tcore_type_impl_t : public __internal_type_impl_t
    {
        typedef __internal_type_impl_t __super_t;

    public:
        __tcore_type_impl_t(xpool_t & xpool) : __super_t(xpool, _vtype) { }

        // Commits it.
        virtual void commit(eobject_commit_context_t & ctx, eobject_t * bind_object) override
        {
            __super_t::commit(ctx, bind_object);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Type
    class __type_type_impl_t : public __tcore_type_impl_t<vtype_t::mobject_>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __type_type_impl_t::type_name[] = __CoreTypeName(CoreType_Type);

    //-------- ---------- ---------- ---------- ----------

    // Base impl for exception type.

    class __exception_type_impl_base_t : public __tcore_type_impl_t<vtype_t::mobject_>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject_> __super_t;

    public:
        using __super_t::__super_t;
    };

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Exception
    class __exception_type_impl_t : public __exception_type_impl_base_t
    {
        typedef __exception_type_impl_base_t __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __exception_type_impl_t::type_name[] = __CoreTypeName(CoreType_Exception);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.NullReferenceException
    class __null_reference_exception_type_impl_t : public __tcore_type_impl_t<vtype_t::mobject_>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __null_reference_exception_type_impl_t::type_name[]
                                        = __CoreTypeName(CoreType_NullReferenceException);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Void
    class __void_type_impl_t : public __tcore_type_impl_t<vtype_t::void_>
    {
        typedef __tcore_type_impl_t<vtype_t::void_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __void_type_impl_t::type_name[] = __CoreTypeName(CoreType_Void);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Object.
    class __object_type_impl_t : public __tcore_type_impl_t<vtype_t::mobject_>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __object_type_impl_t::type_name[] = __CoreTypeName(CoreType_Object);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Ptr.
    class __ptr_type_impl_t : public __tcore_type_impl_t<vtype_t::ptr_>
    {
        typedef __tcore_type_impl_t<vtype_t::ptr_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __ptr_type_impl_t::type_name[] = __CoreTypeName(CoreType_Ptr);

    //-------- ---------- ---------- ---------- ----------

    // Impl for attribute type.
    class __attribute_type_impl_base_t : public __tcore_type_impl_t<vtype_t::mobject_>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject_> __super_t;

    public:
        using __super_t::__super_t;
    };

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Attribute.
    class __attribute_type_impl_t : public __attribute_type_impl_base_t
    {
        typedef __attribute_type_impl_base_t __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __attribute_type_impl_t::type_name[] = __CoreTypeName(CoreType_Attribute);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Enum.
    class __enum_type_impl_t : public __tcore_type_impl_t<vtype_t::mobject_>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __enum_type_impl_t::type_name[] = __CoreTypeName(CoreType_Enum);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Array.
    class __array_type_impl_t : public __tcore_type_impl_t<vtype_t::mobject_>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __array_type_impl_t::type_name[] = __CoreTypeName(CoreType_Array);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Array<>
    class __t_array_type_impl_t : public __tcore_type_impl_t<vtype_t::mobject_>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __t_array_type_impl_t::type_name[] = __CoreTypeName(CoreType_TArray);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.ITuple<...>
    class __t_ituple_type_impl_t : public __tcore_type_impl_t<vtype_t::mobject_>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __t_ituple_type_impl_t::type_name[] = __CoreTypeName(CoreType_ITuple);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Tuple<...>
    class __t_tuple_type_impl_t : public __tcore_type_impl_t<vtype_t::mobject_>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __t_tuple_type_impl_t::type_name[] = __CoreTypeName(CoreType_Tuple);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Delegate
    class __t_delegate_type_impl_t : public __tcore_type_impl_t<vtype_t::mobject_>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __t_delegate_type_impl_t::type_name[] = __CoreTypeName(CoreType_Delegate);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.MulticastDelegate
    class __t_multicast_delegate_type_impl_t : public __tcore_type_impl_t<vtype_t::mobject_>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __t_multicast_delegate_type_impl_t::type_name[]
                                        = __CoreTypeName(CoreType_MulticastDelegate);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Int8.
    class __int8_type_impl_t : public __tcore_type_impl_t<vtype_t::int8_>
    {
        typedef __tcore_type_impl_t<vtype_t::int8_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __int8_type_impl_t::type_name[] = __CoreTypeName(CoreType_Int8);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Uint8.
    class __uint8_type_impl_t : public __tcore_type_impl_t<vtype_t::uint8_>
    {
        typedef __tcore_type_impl_t<vtype_t::uint8_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __uint8_type_impl_t::type_name[] = __CoreTypeName(CoreType_UInt8);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Int16.
    class __int16_type_impl_t : public __tcore_type_impl_t<vtype_t::int16_>
    {
        typedef __tcore_type_impl_t<vtype_t::int16_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __int16_type_impl_t::type_name[] = __CoreTypeName(CoreType_Int16);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Uint16.
    class __uint16_type_impl_t : public __tcore_type_impl_t<vtype_t::uint16_>
    {
        typedef __tcore_type_impl_t<vtype_t::uint16_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __uint16_type_impl_t::type_name[] = __CoreTypeName(CoreType_UInt16);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Int32.
    class __int32_type_impl_t : public __tcore_type_impl_t<vtype_t::int32_>
    {
        typedef __tcore_type_impl_t<vtype_t::int32_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __int32_type_impl_t::type_name[] = __CoreTypeName(CoreType_Int32);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Uint32.
    class __uint32_type_impl_t : public __tcore_type_impl_t<vtype_t::uint32_>
    {
        typedef __tcore_type_impl_t<vtype_t::uint32_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __uint32_type_impl_t::type_name[] = __CoreTypeName(CoreType_UInt32);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Int64.
    class __int64_type_impl_t : public __tcore_type_impl_t<vtype_t::int64_>
    {
        typedef __tcore_type_impl_t<vtype_t::int64_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __int64_type_impl_t::type_name[] = __CoreTypeName(CoreType_Int64);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Uint64.
    class __uint64_type_impl_t : public __tcore_type_impl_t<vtype_t::uint64_>
    {
        typedef __tcore_type_impl_t<vtype_t::uint64_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __uint64_type_impl_t::type_name[] = __CoreTypeName(CoreType_UInt64);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Boolean.
    class __bool_type_impl_t : public __tcore_type_impl_t<vtype_t::bool_>
    {
        typedef __tcore_type_impl_t<vtype_t::bool_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __bool_type_impl_t::type_name[] = __CoreTypeName(CoreType_Boolean);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Char.
    class __char_type_impl_t : public __tcore_type_impl_t<vtype_t::char_>
    {
        typedef __tcore_type_impl_t<vtype_t::char_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __char_type_impl_t::type_name[] = __CoreTypeName(CoreType_Char);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.String.
    class __string_type_impl_t : public __tcore_type_impl_t<vtype_t::string_>
    {
        typedef __tcore_type_impl_t<vtype_t::string_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __string_type_impl_t::type_name[] = __CoreTypeName(CoreType_String);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Single.
    class __single_type_impl_t : public __tcore_type_impl_t<vtype_t::float_>
    {
        typedef __tcore_type_impl_t<vtype_t::float_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __single_type_impl_t::type_name[] = __CoreTypeName(CoreType_Single);

    //-------- ---------- ---------- ---------- ----------

    // Impl for System.Double.
    class __double_type_impl_t : public __tcore_type_impl_t<vtype_t::double_>
    {
        typedef __tcore_type_impl_t<vtype_t::double_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __double_type_impl_t::type_name[] = __CoreTypeName(CoreType_Double);

    ////////// ////////// ////////// ////////// //////////

    // Impl for System.Diagnostics.TraceAttribute
    class __trace_attribute_type_impl_t : public __attribute_type_impl_base_t
    {
        typedef __attribute_type_impl_base_t __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __trace_attribute_type_impl_t::type_name[]
                                        = __CoreTypeName(CoreType_TraceAttribute);

    ////////// ////////// ////////// ////////// //////////

    // Impl for System.ReflectionInfo
    class __reflection_info_type_impl_t : public __tcore_type_impl_t<vtype_t::mobject_>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __reflection_info_type_impl_t::type_name[]
                                                = __CoreTypeName(CoreType_ReflectionInfo);

    ////////// ////////// ////////// ////////// //////////

    // Impl for System.Assembly
    class __assembly_type_impl_t : public __tcore_type_impl_t<vtype_t::mobject_>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __assembly_type_impl_t::type_name[] = __CoreTypeName(CoreType_Assembly);

    ////////// ////////// ////////// ////////// //////////

    // Impl for System.Member
    class __member_type_impl_t : public __tcore_type_impl_t<vtype_t::mobject_>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __member_type_impl_t::type_name[] = __CoreTypeName(CoreType_Member);

    ////////// ////////// ////////// ////////// //////////

    // Impl for System.Method
    class __method_type_impl_t : public __tcore_type_impl_t<vtype_t::mobject_>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __method_type_impl_t::type_name[] = __CoreTypeName(CoreType_Method);

    ////////// ////////// ////////// ////////// //////////

    // Impl for System.Property
    class __property_type_impl_t : public __tcore_type_impl_t<vtype_t::mobject_>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __property_type_impl_t::type_name[] = __CoreTypeName(CoreType_Property);

    ////////// ////////// ////////// ////////// //////////

    // Impl for System.Event
    class __event_type_impl_t : public __tcore_type_impl_t<vtype_t::mobject_>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __event_type_impl_t::type_name[] = __CoreTypeName(CoreType_Event);

    ////////// ////////// ////////// ////////// //////////

    // Impl for System.Field
    class __field_type_impl_t : public __tcore_type_impl_t<vtype_t::mobject_>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __field_type_impl_t::type_name[] = __CoreTypeName(CoreType_Field);

    ////////// ////////// ////////// ////////// //////////

    // Impl for System.Parameter
    class __parameter_type_impl_t : public __tcore_type_impl_t<vtype_t::mobject_>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __parameter_type_impl_t::type_name[] = __CoreTypeName(CoreType_Parameter);

    ////////// ////////// ////////// ////////// //////////

    // Impl for System.GenericParameter
    class __generic_parameter_type_impl_t : public __tcore_type_impl_t<vtype_t::mobject_>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __generic_parameter_type_impl_t::type_name[]
                                            = __CoreTypeName(CoreType_GenericParameter);

    ////////// ////////// ////////// ////////// //////////

    // Internal attribute type impls.
    class __internal_attribute_type_impls_t : public __type_impls_t
    {
        typedef __type_impls_t __super_t;

    public:
        using __super_t::__super_t;

        // Appends a impl_t by type.
        template<typename _impl_t> _impl_t * append()
        {
            static_assert(
                std::is_base_of<__internal_type_impl_t, _impl_t>::value,
                "not based of __internal_type_impl_t"
            );

            _impl_t * impl = __super_t::append<_impl_t>();
            __vtype_map[impl->vtype] = impl;

            return impl;
        }

        using __super_t::get_impl;

        // Gets a impl for vtype.
        __internal_type_impl_t * get_impl(vtype_t vtype)
        {
            auto it = __vtype_map.find(vtype);
            if (it == __vtype_map.end())
                return nullptr;

            return it->second;
        }

    private:
        std::map<vtype_t, __internal_type_impl_t *> __vtype_map;
    };

    //-------- ---------- ---------- ---------- ----------

    // Internal attribute type impl.
    class __internal_attribute_type_impl_t : public __system_type_impl_t
    {
        typedef __system_type_impl_t __super_t;

    public:

        // Constructors.
        __internal_attribute_type_impl_t(xpool_t & xpool)
            : __super_t(xpool), __impls(xpool)
        {
            // Core object types.
            __impls.append<__object_type_impl_t>();
            __impls.append<__type_type_impl_t>();
            __impls.append<__exception_type_impl_t>();
            __impls.append<__null_reference_exception_type_impl_t>();
            __impls.append<__attribute_type_impl_t>();
            __impls.append<__enum_type_impl_t>();
            __impls.append<__array_type_impl_t>();
            __impls.append<__t_array_type_impl_t>();
            __impls.append<__t_ituple_type_impl_t>();
            __impls.append<__t_tuple_type_impl_t>();
            __impls.append<__t_delegate_type_impl_t>();
            __impls.append<__t_multicast_delegate_type_impl_t>();

            // Core data types.
            __impls.append<__void_type_impl_t>();
            __impls.append<__ptr_type_impl_t>();
            __impls.append<__int8_type_impl_t>();
            __impls.append<__uint8_type_impl_t>();
            __impls.append<__int16_type_impl_t>();
            __impls.append<__uint16_type_impl_t>();
            __impls.append<__int32_type_impl_t>();
            __impls.append<__uint32_type_impl_t>();
            __impls.append<__int64_type_impl_t>();
            __impls.append<__uint64_type_impl_t>();
            __impls.append<__bool_type_impl_t>();
            __impls.append<__char_type_impl_t>();
            __impls.append<__string_type_impl_t>();
            __impls.append<__single_type_impl_t>();
            __impls.append<__double_type_impl_t>();

            // Attributes.
            __impls.append<__trace_attribute_type_impl_t>();

            // Reflection.
            __impls.append<__reflection_info_type_impl_t>();
            __impls.append<__assembly_type_impl_t>();
            __impls.append<__member_type_impl_t>();
            __impls.append<__method_type_impl_t>();
            __impls.append<__property_type_impl_t>();
            __impls.append<__event_type_impl_t>();
            __impls.append<__field_type_impl_t>();
            __impls.append<__parameter_type_impl_t>();
            __impls.append<__generic_parameter_type_impl_t>();
        }

        // Commits it.
        virtual void commit(eobject_commit_context_t & ctx, eobject_t * bind_object) override
        {
            general_type_t * type = as<general_type_t *>(bind_object);
            method_t * method;

            if (type != nullptr)
            {
                const string_t type_identity = type->to_identity();
                type_impl_t * impl = __impls.get_impl(type_identity);
                if (impl == nullptr)
                    throw _ECF(not_found, _T("cannot find internal type \"%1%\""), type_identity);

                type->impl = impl;
                impl->commit(ctx, type);
            }
            else if ((method = as<method_t *>(bind_object)) != nullptr)
            {
                #define __CplLogError(error_name, args ...)                             \
                cpl_log_errorf(ctx.logger, method,                                      \
                    __e_t::extern_method_prototype_mistake__##error_name, ##args)

                if (!method->is_static())
                    __CplLogError(should_be_static);

                if (!method->is_extern())
                    __CplLogError(should_be_extern);

                al::svector_t<const char_t *> not_allowed_list;
                #define __NotAllowed(name)                                              \
                    if (method->is_##name())                                            \
                        not_allowed_list.push_back(_S(name));

                __NotAllowed(sealed)
                __NotAllowed(override)
                __NotAllowed(virtual)
                __NotAllowed(abstract)
                __NotAllowed(new)
                __NotAllowed(const)
                __NotAllowed(readonly)

                if (!not_allowed_list.empty())
                {
                    auto & ls = not_allowed_list;
                    string_t names = al::join_str(ls.begin(), ls.end(), _T(", "));
                    __CplLogError(not_allowed, names);
                }

                #undef      __NotAllowed
                #undef      __CplLogError
            }
            else
            {
                // TODO: not supported
            }
        }

        // Gets internal type of vtype.
        general_type_t * get_internal_type(vtype_t vtype)
        {
            __internal_type_impl_t * impl = __impls.get_impl(vtype);

            if (impl == nullptr)
                return nullptr;

            return impl->bind_type;
        }

        // Gets internal type of name.
        general_type_t * get_internal_type(name_t name)
        {
            __internal_type_impl_t * impl = as<__internal_type_impl_t *>(__impls.get_impl(name));

            if (impl == nullptr)
                return nullptr;

            return impl->bind_type;
        }

        static const char_t type_name[];

    private:
       __internal_attribute_type_impls_t __impls;
    };

    const char_t __internal_attribute_type_impl_t::type_name[] = _T("__internal__");

    ////////// ////////// ////////// ////////// //////////

    // Impl for System.InlineAttribute.
    class __inline_attribute_type_impl_t : public __system_type_impl_t
    {
        typedef __system_type_impl_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void commit(eobject_commit_context_t & ctx, eobject_t * bind_object) override
        {
            method_t * method = as<method_t *>(bind_object);
            if (method != nullptr)
            {
                method->inline_type = method_inline_type_t::always;
            }
        }

        static const char_t type_name[];
    };

    const char_t __inline_attribute_type_impl_t::type_name[] = _T("InlineAttribute");

    ////////// ////////// ////////// ////////// //////////

    // Impl for System.EntryPointAttribute.
    class __entrypoint_attribute_type_impl_t : public __system_type_impl_t
    {
        typedef __system_type_impl_t __super_t;

    public:
        using __super_t::__super_t;

        // Commits it.
        virtual void commit(eobject_commit_context_t & ctx, eobject_t * bind_object) override
        {
            method_t * method = as<method_t *>(bind_object);
            if (method != nullptr)
            {
                assembly_t * assembly = get_assembly(method);
                _A(assembly != nullptr);

                __validate_entry_point(ctx, method);
                __set_entry_point(ctx, bind_object, assembly, method);
            }
        }

        // Validates entry point.
        void __validate_entry_point(eobject_commit_context_t & ctx, method_t * method)
        {
            type_t * ret_type = to_type(method->type_name);

            #define __CplLogError(error_name, args ...)                                 \
                cpl_log_errorf(ctx.logger, method,                                      \
                    __e_t::entry_point_prototype_mistake__##error_name, ##args)

            if (ret_type != nullptr && !is_void_type(ret_type))
                __CplLogError(should_no_return_type);

            if (method->is_generic())
                __CplLogError(should_not_generic);

            if (method->param_count() > 0)
                __CplLogError(should_no_params);

            decorate_value_t decorate = method->decorate?
                    *(decorate_value_t *)method->decorate : decorate_value_t::default_value;

            if (!decorate.is_static || !enum_has_flag(decorate.access, access_value_t::public_))
                __CplLogError(should_be_public_static);

            al::svector_t<const char_t *> not_allowed_keywords;

            #define __CheckDecorate(name)                                               \
                do                                                                      \
                {                                                                       \
                    if (decorate.is_##name)                                             \
                        not_allowed_keywords.push_back(_S(name));                       \
                } while (false)

            __CheckDecorate(sealed);
            __CheckDecorate(override);
            __CheckDecorate(virtual);
            __CheckDecorate(abstract);
            __CheckDecorate(new);
            __CheckDecorate(const);
            __CheckDecorate(readonly);

            if (not_allowed_keywords.size() > 0)
            {
                __CplLogError(decorate_not_allowed,
                    al::join_str(not_allowed_keywords.begin(), not_allowed_keywords.end(), _T(", "))
                );
            }

            #undef __CheckDecorate
            #undef __CplLogError
        }

        // Sets entry point.
        void __set_entry_point(eobject_commit_context_t & ctx, eobject_t * bind_object,
                                assembly_t * assembly, method_t * method)
        {
            if (assembly->entry_point != nullptr)
                cpl_log_errorf(ctx.logger, bind_object, __e_t::duplicated_entry_point);
            else
                assembly->entry_point = method;
        }

        static const char_t type_name[];
    };

    const char_t __entrypoint_attribute_type_impl_t::type_name[] = _T("EntryPointAttribute");

    ////////// ////////// ////////// ////////// //////////

    // Impl for System.AttributeUsageAttribute.
    class __attribute_usage_attribute_type_impl_t : public __system_type_impl_t
    {
        typedef __system_type_impl_t __super_t;

    public:
        using __super_t::__super_t; 

        virtual void commit(eobject_commit_context_t & ctx, eobject_t * bind_object) override
        {

        }

        static const char_t type_name[];
    };
    
    const char_t __attribute_usage_attribute_type_impl_t::type_name[]
                                                    = _T("AttributeUsageAttribute");

    ////////// ////////// ////////// ////////// //////////

    // Compile time attribute.
    // Marks a attribute is a compile attribute.
    class __compile_time_attribute_t : public attribute_t
    {
        typedef attribute_t __super_t;

    public:
        __compile_time_attribute_t(xpool_t & xpool)
            : __xpool(xpool), __impls(xpool)
        {
            __internal_attribute_type_impl = __impls.append<__internal_attribute_type_impl_t>();
            __impls.append<__inline_attribute_type_impl_t>();
            __impls.append<__entrypoint_attribute_type_impl_t>();
            __impls.append<__attribute_usage_attribute_type_impl_t>();
        }

        virtual void set_owner(eobject_t * object) override
        {
            general_type_t * type = as<general_type_t *>(object);
            if (type != nullptr)
            {
                type_impl_t * impl = __impls.get_impl(type->name);
                if (impl == nullptr)
                {
                    throw _ECF(not_found,
                        _T("internal type \"%1%\" implemention not found"), type->name
                    );
                }

                type->impl = impl;
            }

            // __super_t::set_owner(object);
        }

        __internal_attribute_type_impl_t * get_internal_attribute_type_impl()
        {
            return __internal_attribute_type_impl;
        }

    private:
        xpool_t &      __xpool;
        __type_impls_t __impls;

        __internal_attribute_type_impl_t * __internal_attribute_type_impl;
    };

    // Creates a compile time attribute.
    attribute_t * __new_compile_time_attribute(xpool_t & xpool)
    {
        return memory_t::new_obj<__compile_time_attribute_t>(xpool.memory, xpool);
    }

    // Gets type by spcified idx.
    template<typename idx_t>
    general_type_t * __t_get_internal_type(attribute_t * attribute, idx_t && idx)
    {
        _A(attribute != nullptr);

        __compile_time_attribute_t * attr = _M(__compile_time_attribute_t *, attribute);
        __internal_attribute_type_impl_t * impl = attr->get_internal_attribute_type_impl();

        _A(impl != nullptr);

        return impl->get_internal_type(std::forward<idx_t>(idx));
    }

    // Returns type by specified vtype.
    general_type_t * __get_internal_type(attribute_t * attribute, vtype_t vtype)
    {
        return __t_get_internal_type(attribute, vtype);
    }

    // Returns type by specified name.
    general_type_t * __get_internal_type(attribute_t * attribute, name_t name)
    {
        return __t_get_internal_type(attribute, name);
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::core


