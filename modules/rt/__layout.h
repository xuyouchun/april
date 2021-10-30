#ifndef __RT_LAYOUT_H__
#define __RT_LAYOUT_H__

namespace X_ROOT_NS { namespace modules { namespace rt {

    ////////// ////////// ////////// ////////// //////////

    // Returns aligned unit.
    constexpr msize_t unit_align(msize_t offset, msize_t size) _NE
    {
        switch (size)
        {
            case 1:
                return offset;

            case 2:
                return _alignf(offset, 2);

            case 3:
            case 4:
                return _alignf(offset, 4);

            default:
                return _alignf(offset, sizeof(rt_stack_unit_t));
        }
    }

    // Returns whether a size is aligned.
    constexpr msize_t unit_is_aligned(msize_t offset, msize_t size) _NE
    {
        return unit_align(offset, size) == offset;
    }

    // Returns unit size.
    constexpr msize_t unit_size_of(msize_t size) _NE
    {
        return _alignf(size, sizeof(rt_stack_unit_t)) / sizeof(rt_stack_unit_t);
    }

    template<msize_t size, typename _src_t>
    X_ALWAYS_INLINE void set_variable_value_t(void * dst, const _src_t * src) _NE
    {
        typedef uint_type_t<size> t;
        *(t *)dst = *(const t *)src;
    }

    template<typename _src_t>
    X_ALWAYS_INLINE void set_variable_value(void * dst, const _src_t * src, msize_t size) _NE
    {
        switch (size)
        {
            case 0:
                break;

            case 1:
                set_variable_value_t<1>(dst, src);
                break;

            case 2:
                set_variable_value_t<2>(dst, src);
                break;

            case 3:
            case 4:
                set_variable_value_t<4>(dst, src);
                break;

            case 5:
            case 6:
            case 7:
            case 8:
                set_variable_value_t<8>(dst, src);
                break;

            default:  // > 8
                set_variable_value_t<8>(dst, src);
                set_variable_value((byte_t *)dst + 8, (byte_t *)src + 8, size - 8);
                break;
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_context_t

    // Generic context.
    class generic_context_t : public object_t
    {
    public:
        generic_context_t() { }

        // Converts generic params to runtime type.
        rt_type_t * parse(rt_generic_param_t * rt_generic_param);

        X_TO_STRING_IMPL(_T("generic_context_t"))
    };

    ////////// ////////// ////////// ////////// //////////

    class generic_param_manager_t;

    // Generic param manager builder.
    class generic_param_manager_builder_t : public object_t
    {
    public:
        // Constructor.
        generic_param_manager_builder_t(assembly_analyzer_t & analyzer,
                                        generic_param_manager_t & gp_mgr)
            : __analyzer(analyzer), __gp_mgr(gp_mgr)
        { }

        // Appends generic params
        void append(ref_t generic_params, rt_type_t ** types, int type_count);

        // Appends generic params ( use placeholders. )
        void append(ref_t generic_params);

        // Append generic params of given generic method.
        void append(rt_generic_method_t * m);

        // Append generic params of given generic type.
        void append(rt_generic_type_t * t);

        // Append generic params of given general type.
        void append(rt_general_type_t * t);

        // Append generic params of given template type.
        void append(rt_type_t * t);

        // Imports generic params of given generic method.
        void import(rt_generic_method_t * m);

        // Imports generic params of given general method.
        void import(rt_method_t * m);

        // Append generic params of given type.
        void import(rt_type_t * t);

        X_TO_STRING_IMPL(_T("generic_param_manager_builder_t"))

    private:
        generic_param_manager_t & __gp_mgr;
        assembly_analyzer_t &     __analyzer;
    };

    //-------- ---------- ---------- ---------- ----------

    // Generic param manager.
    class generic_param_manager_t : public object_t, public no_copy_ctor_t
    {
        typedef object_t __super_t;
        typedef generic_param_manager_t __self_t;
        typedef std::tuple<rt_type_t *, int> __type_item_t;

    public:
        // Constructor.
        generic_param_manager_t() = default;

        // Constructor with type / method.
        template<typename _rt_entity_t>
        generic_param_manager_t(assembly_analyzer_t & analyzer, _rt_entity_t * entity)
        {
            generic_param_manager_builder_t(analyzer, *this).import(entity);
        }

        // Append a generic param type with name.
        void append(rt_sid_t name, rt_type_t * atype);

        // Append generic param types with name.
        void append(rt_sid_t name, int index, int count);

        // Append a generic param types has params attribute.
        void append(rt_type_t * atype);

        // Returns type at index.
        rt_type_t * type_at(int index) const;

        // Returns type of specified name.
        rt_type_t * type_at(rt_sid_t sid, int * out_index = nullptr) const;

        // Returns types position of specified name, for generic params.
        bool types_of(rt_sid_t sid, int * out_index, int * out_count) const;

        // Returns whether it is empty.
        bool empty() const { return __atypes.empty(); }

        // Returns size.
        size_t size() const { return __atypes.size(); }

        // Returns the empty manager.
        static const __self_t * empty_instance();

        X_TO_STRING_IMPL(_T("generic_param_manager_t"))

    private:
        al::svector_t<rt_type_t *, 10> __atypes;
        al::small_map_t<rt_sid_t, std::tuple<int, int>> __named_atypes;
        std::map<rt_sid_t, __type_item_t> __type_map;
    };

    ////////// ////////// ////////// ////////// //////////

    // Base class of variables layout.
    class __variables_layout_t : public object_t, public no_copy_ctor_t
    {
        typedef __variables_layout_t __self_t;

    public:

        // Constructor.
        __variables_layout_t(assembly_analyzer_t & ctx);

        X_TO_STRING_IMPL(_T("__variables_layout_t"))

    protected:
        assembly_analyzer_t & __ctx;          // Context of assembly analyzer.

        // Gets runtime type of typeref.
        rt_type_t * __get_type(ref_t type_ref,
            rt_generic_param_t ** out_generic_param = nullptr, int * out_index = nullptr);
    };

    ////////// ////////// ////////// ////////// //////////
    // locals_layout_t

    // Locals variables layout.
    class locals_layout_t : public __variables_layout_t
    {
        typedef locals_layout_t      __self_t;
        typedef __variables_layout_t __super_t;

    public:

        // Constructors.
        locals_layout_t(assembly_analyzer_t & ctx);

        // Appends a local variable defination.
        void append(local_variable_defination_t & def);

        // Commits it.
        void commit();

        // Returns offset of local variable index.
        msize_t offset_of(int index);

        // Returns type of local variable index.
        rt_type_t * type_at(int index);

        // Returns unit size of variables.
        msize_t unit_size() { return __local_unit_size; }

        // Converts to a string.
        operator string_t() const;

    private:

        // A local variable item.
        struct __item_t
        {
            // Constructor.
            __item_t(msize_t index, msize_t identity, msize_t size,
                                storage_type_t storage_type, rt_type_t * type)
                : index(index), identity(identity), size(size)
                , storage_type(storage_type), type(type)
            { }

            msize_t index;                  // Index.
            msize_t identity;               // Identity.
            msize_t size;                   // Size.

            storage_type_t storage_type;    // Storage type.
            rt_type_t *     type;           // Xil type.
            bool region_flag = false;       // Region flag.

            msize_t offset;                 // Offset
            msize_t new_index;              // The new index.

            // Converts to string.
            operator string_t() const;
        };

        msize_t __identity = 0;             // Identity.
        msize_t __local_unit_size = 0;      // Local unit size.
        static const msize_t __empty_msize = max_value<msize_t>();

        al::heap_t<__item_t> __items;

        // Local variable partial.
        struct __partial_t
        {
            // Constructor.
            __partial_t(msize_t index, msize_t begin)
                : index(index), begin(begin) { }

            msize_t index, begin;
            msize_t offset = 0;

            // Converts to string.
            operator string_t() const
            {
                return _F(_T("index: %1%, offset: %2%"), index, offset);
            }
        };

        typedef al::svector_t<__partial_t *, 16> __partials_t;
        typedef typename __partials_t::iterator __partial_iterator_t;

        // Local variable group.
        struct __group_t
        {
            void append(__item_t * item);
            void reset(msize_t index);

            __self_t * __owner;
            msize_t __index = 0;
            bool __is_reset = true;
            storage_type_t storage_type;

            __vector_t<__item_t *> items;
            __partials_t partials;

        } __groups[(size_t)storage_type_t::__end__];

        typedef typename __vector_t<__item_t *>::iterator __item_iterator_t;
        al::heap_t<__partial_t> __partial_heap;

        // Finds reset index.
        msize_t __find_reset_index(msize_t index, storage_type_t storage_type);

        // Returns current identity.
        msize_t __current_identity() { return __identity++; }

        // Arrange variables.
        template<typename f_t, typename caller_t>
        msize_t __arrange_variables_f(__group_t & g, msize_t offset,
                    __partial_iterator_t it_begin, __partial_iterator_t it_end,
                    f_t f, caller_t caller);

        // Arrange variables.
        msize_t __arrange_variables(__group_t & g, msize_t offset,
                    __partial_iterator_t it_begin, __partial_iterator_t it_end);

        // Arrange mixture variables.
        msize_t __arrange_mixture_variables(__group_t & g, msize_t offset,
                    __partial_iterator_t it_begin, __partial_iterator_t it_end);

        // The min index of specified local variable sequences.
        msize_t __min_index(__partial_iterator_t it_begin,
                    __partial_iterator_t it_end, msize_t * out_end);

        // Sets offset.
        msize_t __set_offsets(msize_t offset,
                    __item_iterator_t it_begin, __item_iterator_t it_end);

        // Each variable items.
        template<typename f_t> void each(f_t f)
        {
            msize_t index = 0;
            for (auto && it : __items)
            {
                f(index++, it->offset);
            }
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // params_layout_t

    X_ENUM(param_layout_type_t)

        default_    = __default__,

        this_,

    X_ENUM_END

    // Params layout.
    class params_layout_t : public __variables_layout_t
    {
        typedef params_layout_t         __self_t;
        typedef __variables_layout_t    __super_t;

    public:
        using __super_t::__super_t;

        // Appends a param.
        void append(ref_t type_ref, param_type_t param_type,
                param_layout_type_t layout_type = param_layout_type_t::default_);

        // Appends a param.
        void append(rt_type_t * type, param_type_t param_type,
                param_layout_type_t layout_type = param_layout_type_t::default_);

        // Returns offset of param index.
        msize_t offset_of(int index);

        // Returns offset of the first extends param.
        msize_t extends_offset();

        // Returns param type of param index.
        rt_type_t * type_at(int index);

        // Commits it.
        void commit();

        // Returns unit size.
        msize_t unit_size() const { return __current_offset; }

    private:

        // Field variable item.
        struct __item_t
        {
            rt_type_t * type;
            param_type_t param_type;
            msize_t offset;
        };

        typedef svector_t<__item_t, 32> __items_t;
        __items_t __items;

        msize_t __current_offset = 0;
        msize_t __extends_params_offset = -1;
    };

    ////////// ////////// ////////// ////////// //////////
    // type_layout_t

    X_INTERFACE field_like_t
    {
        // Returns field type.
        virtual rt_type_t * get_type() = 0;

        // Sets field offset.
        virtual void set_offset(msize_t offset) = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    // Type layout.
    template<typename _field_t>
    class type_layout_t : public __variables_layout_t
    {
        typedef type_layout_t        __self_t;
        typedef __variables_layout_t __super_t;

    public:

        // Constructor.
        type_layout_t(assembly_analyzer_t & ctx, msize_t offset)
            : __super_t(ctx), __offset(offset)
        { }

        // Appends field.
        void append(_field_t field, rt_type_t * type)
        {
            _A(field != nullptr);
            _A(type != nullptr);

            storage_type_t storage_type;
            msize_t size = type->get_variable_size(__ctx.env, &storage_type);

            __items.push_back(__item_t { field, type, size, storage_type });
        }

        // Commits it.
        void commit()
        {
            al::sort(__items, [](__item_t & it1, __item_t & it2) {
                return std::make_tuple(it1.storage_type, it1.size)
                    <  std::make_tuple(it2.storage_type, it2.size);
            });

            __storage_type = storage_type_t::__unknown__;

            for (__item_t & it : __items)
            {
                __offset = unit_align(__offset, it.size);
                it.field->set_offset(__offset);
                __offset += it.size;

                if (__storage_type == storage_type_t::__unknown__)
                    __storage_type = it.storage_type;
                else if (__storage_type != it.storage_type)
                    __storage_type = storage_type_t::mixture;
            }

            if (__storage_type == storage_type_t::__unknown__)
                __storage_type = storage_type_t::value;
        }

        // Returns type size.
        msize_t type_size() { return __offset; }

        // Returns storage type.
        storage_type_t storage_type() { return __storage_type; }

    private:

        // Item.
        struct __item_t
        {
            _field_t            field;          // Field.
            rt_type_t *         type;           // type.
            msize_t             size;           // Field size.
            storage_type_t      storage_type;   // Storage type.
        };

        typedef al::svector_t<__item_t, 32> __items_t;

        msize_t         __offset;               // Offset.
        storage_type_t  __storage_type;         // Storage type.
        __items_t       __items;                // Items.
    };

    ////////// ////////// ////////// ////////// //////////

} } }

#endif  // __RT_LAYOUT_H__
