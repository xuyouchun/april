#include <rtlib.h>
#include <mm.h>

namespace X_ROOT_NS::modules::rtlib {

    ////////// ////////// ////////// ////////// //////////

    // Array arguments
    struct __reflection_args_t
    {
        robject_t   obj;

        void * get_object() { return (void *)(rt_ref_t)obj; }
    };

    ////////// ////////// ////////// ////////// //////////

    // Get object type.

    struct __reflection_get_type_args_t : __reflection_args_t
    {
        // Empty.
    };

    void reflection_get_type(rtlib_context_t & ctx)
    {
        __reflection_get_type_args_t * args = ctx.args<__reflection_get_type_args_t>();

        void * obj = args->get_object();
        rt_type_t * type = mm::get_object_type(obj);

    ctx.return_(type);
    }

    ////////// ////////// ////////// ////////// //////////

    // Get type name.

    struct __reflection_get_type_name_args_t : __reflection_args_t
    {
        // Empty.
    };

    void reflection_get_type_name(rtlib_context_t & ctx)
    {
        __reflection_get_type_name_args_t * args = ctx.args<__reflection_get_type_name_args_t>();

        rt_type_t * type = (rt_type_t *)args->get_object();
        rt_sid_t name = type->get_name(ctx.analyzer.env);

        // The name is a string of the pool of the runtime, it lives as long as the program.
        ctx.return_<const rt_char_t *>(name.value != nullptr? name.value->s : rt_empty_string());
    }

    // Reflection_GetTypeFullName: the name of the type with its namespace and generic arguments.
    void reflection_get_type_full_name(rtlib_context_t & ctx)
    {
        __reflection_get_type_name_args_t * args = ctx.args<__reflection_get_type_name_args_t>();

        rt_type_t * type = (rt_type_t *)args->get_object();
        rt_sid_t name = type->get_full_name(ctx.analyzer.env);

        ctx.return_<const rt_char_t *>(name.value != nullptr? name.value->s : rt_empty_string());
    }

    ////////// ////////// ////////// ////////// //////////

    // The members of the types: System.Reflection reads the tables that rt_reflection_t builds
    // (the native back end writes the same tables into its programs). A Type is rt_type_t *, a
    // member, a parameter and an assembly are rt_reflect_member_t *.

    // The tables of the runtime.
    static rt_reflection_t & __reflection_of(rtlib_context_t & ctx)
    {
        static std::map<void *, std::unique_ptr<rt_reflection_t>> reflections;

        // The environment of the execution: the one of the analyzer of the context cannot load
        // the assemblies that the types refer to.
        analyzer_env_t & env = ctx.exec_env != nullptr? *ctx.exec_env : ctx.analyzer.env;
        std::unique_ptr<rt_reflection_t> & reflection = reflections[&env.rpool];
        if (reflection == nullptr)
            reflection.reset(new rt_reflection_t(env));

        return *reflection;
    }

    struct __type_args_t
    {
        rt_type_t *             type;
    };

    struct __type_index_args_t : __type_args_t
    {
        rint32_t                index;
    };

    struct __type_kind_args_t : __type_args_t
    {
        rint32_t                kind;
    };

    struct __type_kind_index_args_t : __type_kind_args_t
    {
        rint32_t                index;
    };

    struct __info_args_t
    {
        rt_reflect_member_t *   info;
    };

    struct __info_index_args_t : __info_args_t
    {
        rint32_t                index;
    };

    // Reflection_GetTypeBase.
    void reflection_type_base(rtlib_context_t & ctx)
    {
        __type_args_t * args = ctx.args<__type_args_t>();
        ctx.return_<void *>(__reflection_of(ctx).get_type(args->type)->base);
    }

    // Reflection_GetTypeFlags.
    void reflection_type_flags(rtlib_context_t & ctx)
    {
        __type_args_t * args = ctx.args<__type_args_t>();
        ctx.return_<int64_t>(__reflection_of(ctx).get_type(args->type)->flags);
    }

    // Reflection_GetTypeAssembly.
    void reflection_type_assembly(rtlib_context_t & ctx)
    {
        __type_args_t * args = ctx.args<__type_args_t>();
        ctx.return_<void *>(__reflection_of(ctx).get_type(args->type)->assembly);
    }

    // Reflection_GetMemberCount.
    void reflection_member_count(rtlib_context_t & ctx)
    {
        __type_kind_args_t * args = ctx.args<__type_kind_args_t>();
        rt_reflect_type_t * info = __reflection_of(ctx).get_type(args->type);

        _A(args->kind >= 0 && args->kind < 5);
        ctx.return_<int32_t>((int32_t)info->lists[args->kind].count);
    }

    // The member of a kind at an index of a type.
    static void __member_of(rtlib_context_t & ctx, rt_reflect_kind_t kind)
    {
        __type_index_args_t * args = ctx.args<__type_index_args_t>();
        rt_reflect_list_t & list = __reflection_of(ctx).get_type(args->type)->lists[(int)kind];

        _A(args->index >= 0 && args->index < list.count);
        ctx.return_<void *>(list.items[args->index]);
    }

    // Reflection_GetField, Reflection_GetMethod, Reflection_GetProperty, Reflection_GetEvent,
    // Reflection_GetTypeGenericParameter.
    void reflection_field(rtlib_context_t & ctx)    { __member_of(ctx, rt_reflect_kind_t::field); }
    void reflection_method(rtlib_context_t & ctx)   { __member_of(ctx, rt_reflect_kind_t::method); }
    void reflection_property(rtlib_context_t & ctx) { __member_of(ctx, rt_reflect_kind_t::property); }
    void reflection_event(rtlib_context_t & ctx)    { __member_of(ctx, rt_reflect_kind_t::event); }
    void reflection_type_generic_parameter(rtlib_context_t & ctx)
    {
        __member_of(ctx, rt_reflect_kind_t::generic_param);
    }

    // Reflection_GetNestedTypeCount.
    void reflection_nested_type_count(rtlib_context_t & ctx)
    {
        __type_args_t * args = ctx.args<__type_args_t>();
        ctx.return_<int32_t>((int32_t)__reflection_of(ctx).get_type(args->type)->nested.count);
    }

    // Reflection_GetNestedType.
    void reflection_nested_type(rtlib_context_t & ctx)
    {
        __type_index_args_t * args = ctx.args<__type_index_args_t>();
        rt_reflect_list_t & list = __reflection_of(ctx).get_type(args->type)->nested;

        _A(args->index >= 0 && args->index < list.count);
        ctx.return_<void *>((void *)list.items[args->index]);
    }

    // Reflection_GetInfoName.
    void reflection_info_name(rtlib_context_t & ctx)
    {
        ctx.return_<const rt_char_t *>(ctx.args<__info_args_t>()->info->name);
    }

    // Reflection_GetInfoDeclaringType.
    void reflection_info_declaring_type(rtlib_context_t & ctx)
    {
        ctx.return_<void *>(ctx.args<__info_args_t>()->info->declaring);
    }

    // Reflection_GetInfoType.
    void reflection_info_type(rtlib_context_t & ctx)
    {
        ctx.return_<void *>(ctx.args<__info_args_t>()->info->type);
    }

    // Reflection_GetInfoFlags.
    void reflection_info_flags(rtlib_context_t & ctx)
    {
        ctx.return_<int64_t>(ctx.args<__info_args_t>()->info->flags);
    }

    // Reflection_GetPropertyGetter, Reflection_GetEventAdder.
    void reflection_property_getter(rtlib_context_t & ctx)
    {
        ctx.return_<void *>(ctx.args<__info_args_t>()->info->accessor_a);
    }

    void reflection_event_adder(rtlib_context_t & ctx)
    {
        ctx.return_<void *>(ctx.args<__info_args_t>()->info->accessor_a);
    }

    // Reflection_GetPropertySetter, Reflection_GetEventRemover.
    void reflection_property_setter(rtlib_context_t & ctx)
    {
        ctx.return_<void *>(ctx.args<__info_args_t>()->info->accessor_b);
    }

    void reflection_event_remover(rtlib_context_t & ctx)
    {
        ctx.return_<void *>(ctx.args<__info_args_t>()->info->accessor_b);
    }

    // Reflection_GetEventRaiser.
    void reflection_event_raiser(rtlib_context_t & ctx)
    {
        ctx.return_<void *>(ctx.args<__info_args_t>()->info->accessor_c);
    }

    // Reflection_GetParameterCount.
    void reflection_parameter_count(rtlib_context_t & ctx)
    {
        ctx.return_<int32_t>((int32_t)ctx.args<__info_args_t>()->info->param_count);
    }

    // Reflection_GetParameter.
    void reflection_parameter(rtlib_context_t & ctx)
    {
        __info_index_args_t * args = ctx.args<__info_index_args_t>();

        _A(args->index >= 0 && args->index < args->info->param_count);
        ctx.return_<void *>(args->info->params[args->index]);
    }

    // Reflection_GetGenericParameterCount.
    void reflection_generic_parameter_count(rtlib_context_t & ctx)
    {
        ctx.return_<int32_t>((int32_t)ctx.args<__info_args_t>()->info->gparam_count);
    }

    // Reflection_GetGenericParameter.
    void reflection_generic_parameter(rtlib_context_t & ctx)
    {
        __info_index_args_t * args = ctx.args<__info_index_args_t>();

        _A(args->index >= 0 && args->index < args->info->gparam_count);
        ctx.return_<void *>(args->info->gparams[args->index]);
    }

    ////////// ////////// ////////// ////////// //////////

    // The value of a field, and a value for it: a reference is the object, the others (a number, a
    // struct, an enum, a string) are boxed.

    struct __field_value_args_t : __info_args_t
    {
        robject_t               obj;
    };

    struct __field_set_args_t : __field_value_args_t
    {
        robject_t               value;
    };

    struct __field_check_args_t : __field_set_args_t
    {
        rint32_t                mode;
    };

    // Returns whether an object can be the value of a field of the type.
    static bool __is_value_of(analyzer_env_t & env, rt_ref_t value, rt_type_t * type)
    {
        if (value == nullptr)
            return true;

        // The objects of reflection (Type, Method, ...) are handles: they have no header.
        rt_sid_t sid = type->get_full_name(env);
        string_t name = sid.value != nullptr? (string_t)*sid.value : string_t();
        if (name.find(_T("Reflection.")) == 0 || name.find(_T("System.Reflection.")) == 0)
            return true;

        rt_type_t * value_type = mm::get_object_type((void *)value);

        for (rt_type_t * t = value_type; t != nullptr; t = t->get_base_type(env))
        {
            if (t == type)
                return true;
        }

        rt_vtable_t * vtbl = get_vtable(value_type);
        return vtbl != nullptr && vtbl->search_vtable_interface_variant(env, type) != nullptr;
    }

    // Reflection_CheckFieldAccess: 0 when the field of the object can be read (mode 0) or written
    // with the value (mode 1), 1 when the object is null, 2 when the object or the value are not of
    // the types that are needed.
    void reflection_check_field_access(rtlib_context_t & ctx)
    {
        __field_check_args_t * args = ctx.args<__field_check_args_t>();
        rt_reflect_member_t * info = args->info;
        analyzer_env_t & env = *ctx.exec_env;

        rt_ref_t obj = (rt_ref_t)args->obj;
        rt_ref_t value = (rt_ref_t)args->value;
        rt_type_t * type = (rt_type_t *)info->type;

        // A constant has its value in the table, it is read, not written.
        bool is_const = (info->flags & (1 << 10)) != 0;
        if (is_const)
            return ctx.return_<int32_t>(info->value_kind == (int64_t)rt_reflect_value_kind_t::value
                                                        && *args->mode == 0? 0 : 2);

        if (info->value_kind == (int64_t)rt_reflect_value_kind_t::none || info->offset < 0)
            return ctx.return_<int32_t>(2);

        if ((info->flags & 16) == 0)
        {
            if (obj == nullptr)
                return ctx.return_<int32_t>(1);

            // The object is of the type that has the field, or of a type that is derived from it.
            bool derived = false;
            for (rt_type_t * t = mm::get_object_type((void *)obj); t != nullptr && !derived;
                                                        t = t->get_base_type(env))
            {
                derived = t == (rt_type_t *)info->declaring;
            }

            if (!derived)
                return ctx.return_<int32_t>(2);
        }

        if (*args->mode == 1)
        {
            switch ((rt_reflect_value_kind_t)info->value_kind)
            {
                case rt_reflect_value_kind_t::reference:
                    if (!__is_value_of(env, value, type))
                        return ctx.return_<int32_t>(2);

                    break;

                case rt_reflect_value_kind_t::string:
                    if (value != nullptr && mm::get_object_type((void *)value) != type)
                        return ctx.return_<int32_t>(2);

                    break;

                default:
                    if (value == nullptr || mm::get_object_type((void *)value) != type)
                        return ctx.return_<int32_t>(2);

                    break;
            }
        }

        ctx.return_<int32_t>(0);
    }

    // Returns the address of the field of the object (or of the static data).
    static byte_t * __field_address(rtlib_context_t & ctx, rt_reflect_member_t * info, rt_ref_t obj)
    {
        if ((info->flags & 16) != 0)
        {
            rt_ref_t data = ctx.exec_env->get_static_data((rt_type_t *)info->declaring);
            return data == nullptr? nullptr : (byte_t *)(void *)data + info->offset;
        }

        return (byte_t *)(void *)obj + info->offset;
    }

    // Reflection_GetFieldValue: the access is checked (Reflection_CheckFieldAccess).
    void reflection_get_field_value(rtlib_context_t & ctx)
    {
        __field_value_args_t * args = ctx.args<__field_value_args_t>();
        rt_reflect_member_t * info = args->info;

        // A constant: the number is the offset.
        if ((info->flags & (1 << 10)) != 0)
        {
            rt_type_t * const_type = (rt_type_t *)info->type;
            const_type->pre_new(*ctx.exec_env);

            rt_ref_t boxed = ctx.heap->new_obj(const_type);
            set_variable_value((void *)boxed, &info->offset, (msize_t)info->size);

            return ctx.return_(boxed);
        }

        byte_t * address = __field_address(ctx, info, (rt_ref_t)args->obj);
        if (address == nullptr)
            return ctx.return_((rt_ref_t)nullptr);

        if (info->value_kind == (int64_t)rt_reflect_value_kind_t::reference)
        {
            rt_ref_t item = nullptr;
            set_variable_value(&item, address, (msize_t)info->size);

            return ctx.return_(item);
        }

        // A value is boxed, a string that is null is null.
        rt_type_t * type = (rt_type_t *)info->type;

        if (info->value_kind == (int64_t)rt_reflect_value_kind_t::string
                && *(void **)address == nullptr)
            return ctx.return_((rt_ref_t)nullptr);

        type->pre_new(*ctx.exec_env);
        rt_ref_t boxed = ctx.heap->new_obj(type);

        set_variable_value((void *)boxed, address, (msize_t)info->size);
        ctx.return_(boxed);
    }

    // Reflection_SetFieldValue: the access is checked (Reflection_CheckFieldAccess).
    void reflection_set_field_value(rtlib_context_t & ctx)
    {
        __field_set_args_t * args = ctx.args<__field_set_args_t>();
        rt_reflect_member_t * info = args->info;

        byte_t * address = __field_address(ctx, info, (rt_ref_t)args->obj);
        if (address == nullptr)
            return;

        rt_ref_t value = (rt_ref_t)args->value;

        if (info->value_kind == (int64_t)rt_reflect_value_kind_t::reference)
        {
            set_variable_value(address, &value, (msize_t)info->size);
            return;
        }

        if (value == nullptr)
        {
            void * null_string = nullptr;
            set_variable_value(address, &null_string, (msize_t)info->size);
            return;
        }

        set_variable_value(address, (void *)value, (msize_t)info->size);
    }

    ////////// ////////// ////////// ////////// //////////

    // The attributes of a member or of a type: the interpreter reads the metadata of the assembly
    // (the attributes are not in the tables), the key is the member (rt_reflect_member_t *) or the
    // type (rt_type_t *).

    struct __attribute_args_t
    {
        void *                  key;
        rint32_t                index;
    };

    struct __attribute_argument_args_t : __attribute_args_t
    {
        rint32_t                position;
    };

    // The attribute at the index of the key.
    struct __attribute_info_t
    {
        rt_assembly_t *     assembly = nullptr;
        rt_attribute_t *    attribute = nullptr;
        ref_t               arguments = ref_t::null;
    };

    // Returns the count of the attributes of the key, and the one at the index when it is >= 0.
    static int __find_attribute(rtlib_context_t & ctx, void * key, bool is_type, int index,
                                                            __attribute_info_t * out = nullptr)
    {
        rt_reflection_t & reflection = __reflection_of(ctx);

        // The attributes of a type are known when its tables are built.
        if (is_type)
            reflection.get_type((rt_type_t *)key);

        rt_reflect_attributes_t attributes;
        if (!reflection.get_attributes(key, &attributes))
            return 0;

        int count = 0;
        for (ref_t ref : attributes.refs)
        {
            if (count == index && out != nullptr)
            {
                out->assembly  = attributes.assembly;
                out->attribute = attributes.assembly->get_attribute(ref);
                out->arguments = (*out->attribute)->arguments;
            }

            count++;
        }

        return count;
    }

    // Reflection_GetAttributeCount.
    template<bool _is_type> static void __attribute_count(rtlib_context_t & ctx)
    {
        void * key = ctx.args<__attribute_args_t>()->key;
        ctx.return_<int32_t>(__find_attribute(ctx, key, _is_type, -1));
    }

    void reflection_attribute_count(rtlib_context_t & ctx)      { __attribute_count<false>(ctx); }
    void reflection_type_attribute_count(rtlib_context_t & ctx) { __attribute_count<true>(ctx); }

    // Returns the reflection of the constructor of an attribute.
    static rt_reflect_member_t * __constructor_member(rtlib_context_t & ctx,
                                const __attribute_info_t & info, rt_method_base_t ** out_method)
    {
        assembly_analyzer_t analyzer(*ctx.exec_env, info.assembly);
        rt_method_base_t * constructor = analyzer.get_method((*info.attribute)->constructor);
        *out_method = constructor;

        if (constructor == nullptr)
            return nullptr;

        rt_reflect_type_t * type = __reflection_of(ctx).get_type(constructor->get_host_type());
        rt_reflect_list_t & list = type->lists[(int)rt_reflect_kind_t::method];

        for (int64_t k = 0; k < list.count; k++)
        {
            if (list.items[k]->static_data == (void *)constructor)
                return list.items[k];
        }

        return nullptr;
    }

    // Reflection_GetAttributeConstructor.
    template<bool _is_type> static void __attribute_constructor(rtlib_context_t & ctx)
    {
        __attribute_args_t * args = ctx.args<__attribute_args_t>();

        __attribute_info_t info;
        __find_attribute(ctx, args->key, _is_type, *args->index, &info);

        rt_method_base_t * constructor = nullptr;
        ctx.return_<void *>(info.attribute == nullptr? nullptr
                                        : __constructor_member(ctx, info, &constructor));
    }

    void reflection_attribute_constructor(rtlib_context_t & ctx)
    {
        __attribute_constructor<false>(ctx);
    }

    void reflection_type_attribute_constructor(rtlib_context_t & ctx)
    {
        __attribute_constructor<true>(ctx);
    }

    // Reflection_GetAttributeArgumentCount.
    template<bool _is_type> static void __attribute_argument_count(rtlib_context_t & ctx)
    {
        __attribute_args_t * args = ctx.args<__attribute_args_t>();

        __attribute_info_t info;
        __find_attribute(ctx, args->key, _is_type, *args->index, &info);

        int count = 0;
        if (info.attribute != nullptr)
        {
            for (ref_t ref : info.arguments)
            {
                if ((*info.assembly->get_attribute_argument(ref))->name_type != argument_name_type_t::field)
                    count++;
            }
        }

        ctx.return_<int32_t>(count);
    }

    void reflection_attribute_argument_count(rtlib_context_t & ctx)
    {
        __attribute_argument_count<false>(ctx);
    }

    void reflection_type_attribute_argument_count(rtlib_context_t & ctx)
    {
        __attribute_argument_count<true>(ctx);
    }

    // Makes the box of a constant, of the type of the param that it is the argument of; a param
    // that is not a number or a string (Object) takes the type of the value.
    static rt_ref_t __box_constant(rtlib_context_t & ctx, const cvalue_t & value, rt_type_t * type,
                                   rt_type_t * value_type = nullptr)
    {
        analyzer_env_t & env = *ctx.exec_env;

        if (value.is_null())
            return nullptr;

        if (type == nullptr || type->get_vtype(env) == vtype_t::mobject_)
            type = value_type;

        if (type == nullptr)
            return nullptr;

        vtype_t vtype = type->get_vtype(env);

        type->pre_new(env);

        if (vtype == vtype_t::string_)
        {
            if (value.value_type != cvalue_type_t::string)
                return nullptr;

            rt_ref_t boxed = ctx.heap->new_obj(type);
            *(const rt_char_t **)(void *)boxed = value.rt_string();

            return boxed;
        }

        if (value.value_type != cvalue_type_t::number)
            return nullptr;

        cvalue_t number = value.change_type(vtype);
        rt_ref_t boxed = ctx.heap->new_obj(type);
        void * p = (void *)boxed;

        switch (vtype)
        {
            #define __Case(_vt, _t)                                                     \
                case vtype_t::_vt: *(_t *)p = number.number.get_value<_t>(); break;

            __Case(bool_,   bool)
            __Case(char_,   rt_char_t)
            __Case(int8_,   int8_t)
            __Case(uint8_,  uint8_t)
            __Case(int16_,  int16_t)
            __Case(uint16_, uint16_t)
            __Case(int32_,  int32_t)
            __Case(uint32_, uint32_t)
            __Case(int64_,  int64_t)
            __Case(uint64_, uint64_t)
            __Case(float_,  float)
            __Case(double_, double)

            #undef __Case

            default:
                return nullptr;
        }

        return boxed;
    }

    // The array that an argument is (new Int32[] { 1, 2 }, new Int32[,] { { 1 }, { 2 } }, new Int32[0]):
    // the constants that follow one another are the rank, the length of each dimension, and the
    // elements. The extra of the ref is 1.
    static rt_ref_t __make_array(rtlib_context_t & ctx, const __attribute_info_t & info,
                                 rt_attribute_argument_t * argument)
    {
        analyzer_env_t & env = *ctx.exec_env;
        ref_t value_ref = (*argument)->value;

        ref_t type_ref = (*argument)->value_type;
        if (type_ref == ref_t::null || value_ref.count < 2)
            return nullptr;

        // The header: the rank, then the lengths.
        auto int_at = [&](uint32_t k) {
            cvalue_t value = info.assembly->get_constant(ref_t(value_ref.index + k, 1, 0));
            return value.change_type(vtype_t::int32_).number.get_value<int32_t>();
        };

        int32_t rank = int_at(0);
        if (rank < 1 || (uint32_t)rank + 1 > value_ref.count)
            return nullptr;

        std::vector<array_length_t> lengths;
        size_t total = 1;
        for (int32_t d = 0; d < rank; d++)
        {
            lengths.push_back((array_length_t)int_at(1 + d));
            total *= (size_t)lengths.back();
        }

        uint32_t first = 1 + (uint32_t)rank;       // The first element.
        if (total != (size_t)(value_ref.count - first))
            return nullptr;

        assembly_analyzer_t analyzer(env, info.assembly);
        rt_type_t * element = analyzer.get_type(type_ref);
        if (element == nullptr)
            return nullptr;

        element->pre_new(env);

        rt_array_type_t * array_type = analyzer.to_array_type(element, (dimension_t)rank);
        array_type->pre_new(env);

        rt_ref_t array = ctx.heap->new_array(array_type, lengths.data());

        // The array is in the stack (the place of the result): the collector knows it while the
        // strings are made.
        ctx.return_(array);

        vtype_t vtype = element->get_vtype(env);
        for (uint32_t k = 0; k < (uint32_t)total; k++)
        {
            cvalue_t value = info.assembly->get_constant(ref_t(value_ref.index + first + k, 1, 0));

            // A string is a pointer to its characters (they are in the assembly).
            if (vtype == vtype_t::string_)
            {
                mm::set_array_element<const rt_char_t *>((void *)array, (array_length_t)k,
                        value.value_type == cvalue_type_t::string? value.rt_string() : nullptr);
                continue;
            }

            cvalue_t number = value.change_type(vtype);

            switch (vtype)
            {
                #define __Case(_vt, _t)                                                 \
                    case vtype_t::_vt:                                                  \
                        mm::set_array_element<_t>((void *)array, (array_length_t)k,     \
                                                  number.number.get_value<_t>());       \
                        break;

                __Case(bool_,   bool)
                __Case(char_,   rt_char_t)
                __Case(int8_,   int8_t)
                __Case(uint8_,  uint8_t)
                __Case(int16_,  int16_t)
                __Case(uint16_, uint16_t)
                __Case(int32_,  int32_t)
                __Case(uint32_, uint32_t)
                __Case(int64_,  int64_t)
                __Case(uint64_, uint64_t)
                __Case(float_,  float)
                __Case(double_, double)

                #undef __Case

                default:
                    break;
            }
        }

        return array;
    }

    // The type of the value of an argument (int, String): the type of the object of an Object param.
    static rt_type_t * __value_type_of(rtlib_context_t & ctx, const __attribute_info_t & info,
                                       rt_attribute_argument_t * argument)
    {
        ref_t type_ref = (*argument)->value_type;
        if (type_ref == ref_t::null)
            return nullptr;

        return assembly_analyzer_t(*ctx.exec_env, info.assembly).get_type(type_ref);
    }

    // Reflection_GetAttributeArgument: the argument of the constructor of an attribute, boxed.
    template<bool _is_type> static void __attribute_argument(rtlib_context_t & ctx)
    {
        __attribute_argument_args_t * args = ctx.args<__attribute_argument_args_t>();

        __attribute_info_t info;
        __find_attribute(ctx, args->key, _is_type, *args->index, &info);

        if (info.attribute == nullptr)
            return ctx.return_((rt_ref_t)nullptr);

        // The type of the param is that of the constructor.
        rt_method_base_t * constructor = nullptr;
        rt_reflect_member_t * member = __constructor_member(ctx, info, &constructor);

        int position = *args->position, k = 0;
        for (ref_t ref : info.arguments)
        {
            rt_attribute_argument_t * argument = info.assembly->get_attribute_argument(ref);
            if ((*argument)->name_type == argument_name_type_t::field || k++ != position)
                continue;

            rt_type_t * type = nullptr;
            if (member != nullptr && position < member->param_count)
                type = (rt_type_t *)member->params[position]->type;

            // null.
            if ((*argument)->value == ref_t::null)
                return ctx.return_((rt_ref_t)nullptr);

            // An array.
            if ((*argument)->value.extra == 1)
                return ctx.return_(__make_array(ctx, info, argument));

            // typeof(A): the Type is the type.
            ref_t type_ref;
            assembly_analyzer_t analyzer(*ctx.exec_env, info.assembly);
            if (info.assembly->get_constant_type((*argument)->value, &type_ref))
                return ctx.return_((rt_ref_t)(void *)analyzer.get_type(type_ref));

            cvalue_t value = info.assembly->get_constant((*argument)->value);

            return ctx.return_(__box_constant(ctx, value, type, __value_type_of(ctx, info, argument)));
        }

        ctx.return_((rt_ref_t)nullptr);
    }

    void reflection_attribute_argument(rtlib_context_t & ctx)      { __attribute_argument<false>(ctx); }
    void reflection_type_attribute_argument(rtlib_context_t & ctx) { __attribute_argument<true>(ctx); }

    // Reflection_CheckInvoke: the interpreter checks the call when it makes it (the stub of the
    // method raises the exceptions), there is nothing to do before.
    void reflection_check_invoke(rtlib_context_t & ctx)
    {
        ctx.return_<int32_t>(0);
    }

    // Reflection_IsMember: whether the object of reflection is the table of a member (and not a
    // Type).
    void reflection_is_member(rtlib_context_t & ctx)
    {
        void * handle = ctx.args<__info_args_t>()->info;
        ctx.return_<int32_t>(__reflection_of(ctx).is_member(handle)? 1 : 0);
    }

    // The named arguments follow the others: [Author("ann", Level = 3)].
    static bool __is_named(rt_attribute_argument_t * argument)
    {
        return (*argument)->name_type == argument_name_type_t::field;
    }

    // Reflection_GetAttributeNamedCount.
    template<bool _is_type> static void __attribute_named_count(rtlib_context_t & ctx)
    {
        __attribute_args_t * args = ctx.args<__attribute_args_t>();

        __attribute_info_t info;
        __find_attribute(ctx, args->key, _is_type, *args->index, &info);

        int count = 0;
        if (info.attribute != nullptr)
        {
            for (ref_t ref : info.arguments)
            {
                if (__is_named(info.assembly->get_attribute_argument(ref)))
                    count++;
            }
        }

        ctx.return_<int32_t>(count);
    }

    void reflection_attribute_named_count(rtlib_context_t & ctx)      { __attribute_named_count<false>(ctx); }
    void reflection_type_attribute_named_count(rtlib_context_t & ctx) { __attribute_named_count<true>(ctx); }

    // The named argument at a position.
    static rt_attribute_argument_t * __named_argument(const __attribute_info_t & info, int position)
    {
        int k = 0;
        for (ref_t ref : info.arguments)
        {
            rt_attribute_argument_t * argument = info.assembly->get_attribute_argument(ref);
            if (__is_named(argument) && k++ == position)
                return argument;
        }

        return nullptr;
    }

    // Reflection_GetAttributeNamedName: the name of the field or the property.
    template<bool _is_type> static void __attribute_named_name(rtlib_context_t & ctx)
    {
        __attribute_argument_args_t * args = ctx.args<__attribute_argument_args_t>();

        __attribute_info_t info;
        __find_attribute(ctx, args->key, _is_type, *args->index, &info);

        rt_attribute_argument_t * argument = info.attribute == nullptr? nullptr
                                                : __named_argument(info, *args->position);

        assembly_analyzer_t analyzer(*ctx.exec_env, info.assembly);
        ctx.return_<const rt_char_t *>(argument == nullptr? rt_empty_string() : analyzer.to_cstr((*argument)->name));
    }

    void reflection_attribute_named_name(rtlib_context_t & ctx)      { __attribute_named_name<false>(ctx); }
    void reflection_type_attribute_named_name(rtlib_context_t & ctx) { __attribute_named_name<true>(ctx); }

    // Reflection_GetAttributeNamedValue: the value, boxed in the type of the field or the property.
    template<bool _is_type> static void __attribute_named_value(rtlib_context_t & ctx)
    {
        __attribute_argument_args_t * args = ctx.args<__attribute_argument_args_t>();
        analyzer_env_t & env = *ctx.exec_env;

        __attribute_info_t info;
        __find_attribute(ctx, args->key, _is_type, *args->index, &info);

        rt_attribute_argument_t * argument = info.attribute == nullptr? nullptr
                                                : __named_argument(info, *args->position);
        if (argument == nullptr)
            return ctx.return_((rt_ref_t)nullptr);

        assembly_analyzer_t analyzer(env, info.assembly);
        rt_method_base_t * constructor = analyzer.get_method((*info.attribute)->constructor);
        if (constructor == nullptr)
            return ctx.return_((rt_ref_t)nullptr);

        ustring_t name = analyzer.to_cstr((*argument)->name);

        // The field or the property of the attribute, or of its base types.
        for (rt_type_t * t = constructor->get_host_type(); t != nullptr; t = t->get_base_type(env))
        {
            rt_reflect_type_t * type = __reflection_of(ctx).get_type(t);

            for (int kind : { (int)rt_reflect_kind_t::field, (int)rt_reflect_kind_t::property })
            {
                rt_reflect_list_t & list = type->lists[kind];
                for (int64_t k = 0; k < list.count; k++)
                {
                    if (ustring_t(list.items[k]->name) != name)
                        continue;

                    if ((*argument)->value == ref_t::null)
                        return ctx.return_((rt_ref_t)nullptr);

                    if ((*argument)->value.extra == 1)
                        return ctx.return_(__make_array(ctx, info, argument));

                    ref_t type_ref;
                    if (info.assembly->get_constant_type((*argument)->value, &type_ref))
                        return ctx.return_((rt_ref_t)(void *)analyzer.get_type(type_ref));

                    cvalue_t value = info.assembly->get_constant((*argument)->value);
                    return ctx.return_(__box_constant(ctx, value, (rt_type_t *)list.items[k]->type,
                                                      __value_type_of(ctx, info, argument)));
                }
            }
        }

        ctx.return_((rt_ref_t)nullptr);
    }

    void reflection_attribute_named_value(rtlib_context_t & ctx)      { __attribute_named_value<false>(ctx); }
    void reflection_type_attribute_named_value(rtlib_context_t & ctx) { __attribute_named_value<true>(ctx); }

    // Reflection_NewObject: an object of a type that no constructor has run for.
    struct __new_object_args_t
    {
        rt_type_t *             type;
    };

    void reflection_new_object(rtlib_context_t & ctx)
    {
        rt_type_t * type = ctx.args<__new_object_args_t>()->type;
        type->pre_new(*ctx.exec_env);

        ctx.return_(ctx.heap->new_obj(type));
    }

    ////////// ////////// ////////// ////////// //////////

    // Object_GetHashCode: the hash code of the identity of an object (the objects do not move).
    void object_get_hash_code(rtlib_context_t & ctx)
    {
        __reflection_args_t * args = ctx.args<__reflection_args_t>();

        uintptr_t address = (uintptr_t)args->get_object();
        ctx.return_<int32_t>((int32_t)((address >> 4) & 0x7fffffff));
    }

    // The number of a box of an enum, of its underlying type (a signed one is extended by its sign).
    static int64_t __enum_number_of(analyzer_env_t & env, rt_type_t * type, void * obj)
    {
        switch (type->get_vtype(env))
        {
            #define __Case(_vt, _t)     case vtype_t::_vt: return (int64_t)*(_t *)obj;

            __Case(int8_,   int8_t)
            __Case(uint8_,  uint8_t)
            __Case(int16_,  int16_t)
            __Case(uint16_, uint16_t)
            __Case(int32_,  int32_t)
            __Case(uint32_, uint32_t)
            __Case(int64_,  int64_t)
            __Case(uint64_, uint64_t)

            #undef __Case

            default:
                return 0;
        }
    }

    // Enum_GetNumber: the number of a boxed enum.
    void enum_get_number(rtlib_context_t & ctx)
    {
        __reflection_args_t * args = ctx.args<__reflection_args_t>();

        void * obj = args->get_object();
        ctx.return_<int64_t>(obj == nullptr? 0 : __enum_number_of(*ctx.exec_env, mm::get_object_type(obj), obj));
    }

    // Enum_ToString: the text of a boxed enum: the name of its item, the number when it has no item.
    void enum_to_string(rtlib_context_t & ctx)
    {
        __reflection_args_t * args = ctx.args<__reflection_args_t>();

        void * obj = args->get_object();
        if (obj == nullptr)
            return ctx.return_<const rt_char_t *>(rt_empty_string());

        rt_type_t * type = mm::get_object_type(obj);
        int64_t number = __enum_number_of(*ctx.exec_env, type, obj);

        ctx.return_<const rt_char_t *>(enum_text_of(__reflection_of(ctx).get_type(type), number));
    }

    // Enum_Parse: the box of the value of a text, null when the text is not a value of the enum.
    void enum_parse(rtlib_context_t & ctx)
    {
        struct args_t { robject_t type; rstring_t text; rbool_t ignore_case; };
        args_t * args = ctx.args<args_t>();

        analyzer_env_t & env = *ctx.exec_env;
        rt_type_t * type = (rt_type_t *)(void *)(rt_ref_t)args->type;

        int64_t number;
        if (type == nullptr || type->get_ttype(env) != ttype_t::enum_
                || !enum_parse_text(__reflection_of(ctx).get_type(type), *args->text, *args->ignore_case, &number))
            return ctx.return_((rt_ref_t)nullptr);

        type->pre_new(env);

        rt_ref_t boxed = ctx.heap->new_obj(type);
        set_variable_value((void *)boxed, &number, (msize_t)type->get_variable_size(env));

        ctx.return_(boxed);
    }

    // Object_ReferenceEquals: whether the two are the same object.
    struct __reference_equals_args_t
    {
        robject_t   a;
        robject_t   b;
    };

    void object_reference_equals(rtlib_context_t & ctx)
    {
        __reference_equals_args_t * args = ctx.args<__reference_equals_args_t>();

        ctx.return_<bool>((void *)(rt_ref_t)args->a == (void *)(rt_ref_t)args->b);
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::rtlib

