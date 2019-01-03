#include <core.h>

namespace X_ROOT_NS { namespace modules { namespace core {

    typedef compile_error_code_t __e_t;

    ////////// ////////// ////////// ////////// //////////

    template<typename t, typename ... args_t>
    t * __new(xpool_t & xpool, args_t && ... args)
    {
        return memory_t::new_obj<t>(xpool.memory, xpool, std::forward<args_t>(args) ...);
    }

    ////////// ////////// ////////// ////////// //////////

    class __type_impls_t
    {
    public:
        __type_impls_t(xpool_t & xpool) : __xpool(xpool) { }

        type_impl_t * append(const string_t & name, type_impl_t * impl)
        {
            _A(impl != nullptr);
            __impls[__to_name(name)] = impl;

            return impl;
        }

        template<typename _impl_t> _impl_t * append()
        {
            return (_impl_t *)append(_impl_t::type_name, __new<_impl_t>(__xpool));
        }

        type_impl_t * get_impl(name_t name)
        {
            auto it = __impls.find(name);
            if(it != __impls.end())
                return it->second;

            return nullptr;
        }

        type_impl_t * get_impl(const string_t & name)
        {
            return get_impl(__to_name(name));
        }

    private:
        xpool_t & __xpool;
        std::map<name_t, type_impl_t *> __impls;

        name_t __to_name(const string_t & s) { return name_t(__xpool.spool.to_sid(s)); }
    };

    ////////// ////////// ////////// ////////// //////////

    class __system_type_impl_t : public type_impl_t
    {
        typedef type_impl_t __super_t;

    public:
        __system_type_impl_t(xpool_t & xpool) : __xpool(xpool) { }

        virtual void commit(eobject_commit_context_t & ctx, eobject_t * bind_object) override { }

    protected:
        xpool_t & __xpool;
        name_t __to_name(const string_t & s) { return name_t(__xpool.spool.to_sid(s)); }
    };

    ////////// ////////// ////////// ////////// //////////

    class __internal_type_impl_t : public __system_type_impl_t
    {
        typedef __system_type_impl_t __super_t;

    public:
        __internal_type_impl_t(xpool_t & xpool, vtype_t vtype = vtype_t::mobject)
            : __system_type_impl_t(xpool), vtype(vtype) { }

        virtual void commit(eobject_commit_context_t & ctx, eobject_t * bind_object) override
        {
            general_type_t * type = as<general_type_t *>(bind_object);
            if(type != nullptr)
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

    template<vtype_t _vtype>
    class __tcore_type_impl_t : public __internal_type_impl_t
    {
        typedef __internal_type_impl_t __super_t;

    public:
        __tcore_type_impl_t(xpool_t & xpool) : __super_t(xpool, _vtype) { }

        virtual void commit(eobject_commit_context_t & ctx, eobject_t * bind_object) override
        {
            __super_t::commit(ctx, bind_object);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    class __type_type_impl_t : public __tcore_type_impl_t<vtype_t::mobject>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __type_type_impl_t::type_name[] = _T("Type");

    //-------- ---------- ---------- ---------- ----------

    class __void_type_impl_t : public __tcore_type_impl_t<vtype_t::void_>
    {
        typedef __tcore_type_impl_t<vtype_t::void_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __void_type_impl_t::type_name[] = _T("Void");

    //-------- ---------- ---------- ---------- ----------

    class __object_type_impl_t : public __tcore_type_impl_t<vtype_t::mobject>
    {
        typedef __tcore_type_impl_t<vtype_t::mobject> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __object_type_impl_t::type_name[] = _T("Object");

    //-------- ---------- ---------- ---------- ----------

    class __ptr_type_impl_t : public __tcore_type_impl_t<vtype_t::ptr>
    {
        typedef __tcore_type_impl_t<vtype_t::ptr> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __ptr_type_impl_t::type_name[] = _T("Ptr");

    //-------- ---------- ---------- ---------- ----------

    class __int8_type_impl_t : public __tcore_type_impl_t<vtype_t::int8_>
    {
        typedef __tcore_type_impl_t<vtype_t::int8_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __int8_type_impl_t::type_name[] = _T("Int8");

    //-------- ---------- ---------- ---------- ----------

    class __uint8_type_impl_t : public __tcore_type_impl_t<vtype_t::uint8_>
    {
        typedef __tcore_type_impl_t<vtype_t::uint8_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __uint8_type_impl_t::type_name[] = _T("UInt8");

    //-------- ---------- ---------- ---------- ----------

    class __int16_type_impl_t : public __tcore_type_impl_t<vtype_t::int16_>
    {
        typedef __tcore_type_impl_t<vtype_t::int16_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __int16_type_impl_t::type_name[] = _T("Int16");

    //-------- ---------- ---------- ---------- ----------

    class __uint16_type_impl_t : public __tcore_type_impl_t<vtype_t::uint16_>
    {
        typedef __tcore_type_impl_t<vtype_t::uint16_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __uint16_type_impl_t::type_name[] = _T("UInt16");

    //-------- ---------- ---------- ---------- ----------

    class __int32_type_impl_t : public __tcore_type_impl_t<vtype_t::int32_>
    {
        typedef __tcore_type_impl_t<vtype_t::int32_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __int32_type_impl_t::type_name[] = _T("Int32");

    //-------- ---------- ---------- ---------- ----------

    class __uint32_type_impl_t : public __tcore_type_impl_t<vtype_t::uint32_>
    {
        typedef __tcore_type_impl_t<vtype_t::uint32_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __uint32_type_impl_t::type_name[] = _T("UInt32");

    //-------- ---------- ---------- ---------- ----------

    class __int64_type_impl_t : public __tcore_type_impl_t<vtype_t::int64_>
    {
        typedef __tcore_type_impl_t<vtype_t::int64_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __int64_type_impl_t::type_name[] = _T("Int64");

    //-------- ---------- ---------- ---------- ----------

    class __uint64_type_impl_t : public __tcore_type_impl_t<vtype_t::uint64_>
    {
        typedef __tcore_type_impl_t<vtype_t::uint64_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __uint64_type_impl_t::type_name[] = _T("UInt64");

    //-------- ---------- ---------- ---------- ----------

    class __bool_type_impl_t : public __tcore_type_impl_t<vtype_t::bool_>
    {
        typedef __tcore_type_impl_t<vtype_t::bool_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __bool_type_impl_t::type_name[] = _T("Boolean");

    //-------- ---------- ---------- ---------- ----------

    class __char_type_impl_t : public __tcore_type_impl_t<vtype_t::char_>
    {
        typedef __tcore_type_impl_t<vtype_t::char_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __char_type_impl_t::type_name[] = _T("Char");

    //-------- ---------- ---------- ---------- ----------

    class __string_type_impl_t : public __tcore_type_impl_t<vtype_t::string>
    {
        typedef __tcore_type_impl_t<vtype_t::string> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __string_type_impl_t::type_name[] = _T("String");

    //-------- ---------- ---------- ---------- ----------

    class __single_type_impl_t : public __tcore_type_impl_t<vtype_t::float_>
    {
        typedef __tcore_type_impl_t<vtype_t::float_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __single_type_impl_t::type_name[] = _T("Single");

    //-------- ---------- ---------- ---------- ----------

    class __double_type_impl_t : public __tcore_type_impl_t<vtype_t::double_>
    {
        typedef __tcore_type_impl_t<vtype_t::double_> __super_t;

    public:
        using __super_t::__super_t;

        static const char_t type_name[];
    };

    const char_t __double_type_impl_t::type_name[] = _T("Double");

    ////////// ////////// ////////// ////////// //////////

    class __internal_attribute_type_impls_t : public __type_impls_t
    {
        typedef __type_impls_t __super_t;

    public:
        using __super_t::__super_t;

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

        __internal_type_impl_t * get_impl(vtype_t vtype)
        {
            auto it = __vtype_map.find(vtype);
            if(it == __vtype_map.end())
                return nullptr;

            return it->second;
        }

    private:
        std::map<vtype_t, __internal_type_impl_t *> __vtype_map;
    };

    //-------- ---------- ---------- ---------- ----------

    class __internal_attribute_type_impl_t : public __system_type_impl_t
    {
        typedef __system_type_impl_t __super_t;

    public:
        __internal_attribute_type_impl_t(xpool_t & xpool)
            : __super_t(xpool), __impls(xpool)
        {
            __impls.append<__type_type_impl_t>();
            __impls.append<__void_type_impl_t>();
            __impls.append<__object_type_impl_t>();
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
        }

        virtual void commit(eobject_commit_context_t & ctx, eobject_t * bind_object) override
        {
            general_type_t * type = as<general_type_t *>(bind_object);
            method_t * method;

            if(type != nullptr)
            {
                type_impl_t * impl = __impls.get_impl(type->name);
                if(impl == nullptr)
                    throw _ECF(not_found, _T("cannot find internal type \"%1%\""), type->name);

                type->impl = impl;
                impl->commit(ctx, type);
            }
            else if((method = as<method_t *>(bind_object)) != nullptr)
            {
                #define __CplLogError(error_name, args ...)                             \
                cpl_log_errorf(ctx.logger, method,                                      \
                    __e_t::extern_method_prototype_mistake__##error_name, ##args)

                if(!method->is_static())
                    __CplLogError(should_be_static);

                if(!method->is_extern())
                    __CplLogError(should_be_extern);

                al::small_vector_t<const char_t *> not_allowed_list;
                #define __NotAllowed(name)                                              \
                    if(method->is_##name())                                             \
                        not_allowed_list.push_back(_S(name));

                __NotAllowed(sealed)
                __NotAllowed(override)
                __NotAllowed(virtual)
                __NotAllowed(abstract)
                __NotAllowed(new)
                __NotAllowed(const)
                __NotAllowed(readonly)

                if(!not_allowed_list.empty())
                {
                    auto & ls = not_allowed_list;
                    string_t names = al::join_str(ls.begin(), ls.end(), _T(", "));
                    __CplLogError(now_allowed, names);
                }

                #undef      __NotAllowed
                #undef      __CplLogError
            }
            else
            {
                // TODO: not supported
            }
        }

        general_type_t * get_internal_type(vtype_t vtype)
        {
            __internal_type_impl_t * impl = __impls.get_impl(vtype);

            if(impl == nullptr)
                return nullptr;

            return impl->bind_type;
        }

        general_type_t * get_internal_type(name_t name)
        {
            __internal_type_impl_t * impl = as<__internal_type_impl_t *>(__impls.get_impl(name));

            if(impl == nullptr)
                return nullptr;

            return impl->bind_type;
        }

        static const char_t type_name[];

    private:
       __internal_attribute_type_impls_t __impls;
    };

    const char_t __internal_attribute_type_impl_t::type_name[] = _T("__internal__");

    ////////// ////////// ////////// ////////// //////////

    class __inline_attribute_type_impl_t : public __system_type_impl_t
    {
        typedef __system_type_impl_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void commit(eobject_commit_context_t & ctx, eobject_t * bind_object) override
        {
            method_t * method = as<method_t *>(bind_object);
            if(method != nullptr)
            {
                method->inline_type = method_inline_type_t::always;
            }
        }

        static const char_t type_name[];
    };

    const char_t __inline_attribute_type_impl_t::type_name[] = _T("InlineAttribute");

    ////////// ////////// ////////// ////////// //////////

    class __entrypoint_attribute_type_impl_t : public __system_type_impl_t
    {
        typedef __system_type_impl_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void commit(eobject_commit_context_t & ctx, eobject_t * bind_object) override
        {
            method_t * method = as<method_t *>(bind_object);
            if(method != nullptr)
            {
                assembly_t * assembly = get_assembly(method);
                _A(assembly != nullptr);

                __validate_entry_point(ctx, method);
                __set_entry_point(ctx, bind_object, assembly, method);
            }
        }

        void __validate_entry_point(eobject_commit_context_t & ctx, method_t * method)
        {
            type_t * ret_type = to_type(method->type_name);

            #define __CplLogError(error_name, args ...)                             \
                cpl_log_errorf(ctx.logger, method,                                  \
                    __e_t::entry_point_prototype_mistake__##error_name, ##args)

            if(ret_type != nullptr && !is_void_type(ret_type))
                __CplLogError(should_no_return_type);

            if(method->is_generic())
                __CplLogError(should_not_generic);

            if(method->param_count() > 0)
                __CplLogError(should_no_params);

            decorate_value_t decorate = method->decorate?
                    *(decorate_value_t *)method->decorate : decorate_value_t::default_value;

            if(!decorate.is_static || !enum_has_flag(decorate.access, access_value_t::public_))
                __CplLogError(should_be_public_static);

            al::small_vector_t<const char_t *> not_allowed_keywords;

            #define __CheckDecorate(name)                           \
                do                                                  \
                {                                                   \
                    if(decorate.is_##name)                          \
                        not_allowed_keywords.push_back(_S(name));   \
                } while(false)

            __CheckDecorate(sealed);
            __CheckDecorate(override);
            __CheckDecorate(virtual);
            __CheckDecorate(abstract);
            __CheckDecorate(new);
            __CheckDecorate(const);
            __CheckDecorate(readonly);

            if(not_allowed_keywords.size() > 0)
            {
                __CplLogError(decorate_not_allowed,
                    al::join_str(not_allowed_keywords.begin(), not_allowed_keywords.end(), _T(", "))
                );
            }

            #undef __CheckDecorate
            #undef __CplLogError
        }

        void __set_entry_point(eobject_commit_context_t & ctx, eobject_t * bind_object,
                                assembly_t * assembly, method_t * method)
        {
            if(assembly->entry_point != nullptr)
                cpl_log_errorf(ctx.logger, bind_object, __e_t::duplicated_entry_point);
            else
                assembly->entry_point = method;
        }

        static const char_t type_name[];
    };

    const char_t __entrypoint_attribute_type_impl_t::type_name[] = _T("EntryPointAttribute");

    ////////// ////////// ////////// ////////// //////////

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
            if(type != nullptr)
            {
                type_impl_t * impl = __impls.get_impl(type->name);
                if(impl == nullptr)
                {
                    throw _ECF(not_found,
                        _T("internal type \"%1%\" implemention not found"), type->name
                    );
                }

                type->impl = impl;
            }

            //__super_t::set_owner(object);
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

    attribute_t * __new_compile_time_attribute(xpool_t & xpool)
    {
        return memory_t::new_obj<__compile_time_attribute_t>(xpool.memory, xpool);
    }

    template<typename idx_t>
    general_type_t * __t_get_internal_type(attribute_t * attribute, idx_t && idx)
    {
        _A(attribute != nullptr);

        __compile_time_attribute_t * attr = _M(__compile_time_attribute_t *, attribute);
        __internal_attribute_type_impl_t * impl = attr->get_internal_attribute_type_impl();

        _A(impl != nullptr);

        return impl->get_internal_type(std::forward<idx_t>(idx));
    }

    general_type_t * __get_internal_type(attribute_t * attribute, vtype_t vtype)
    {
        return __t_get_internal_type(attribute, vtype);
    }

    general_type_t * __get_internal_type(attribute_t * attribute, name_t name)
    {
        return __t_get_internal_type(attribute, name);
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::core


