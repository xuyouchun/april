#ifndef __RT_LAYOUT_H__
#define __RT_LAYOUT_H__

namespace X_ROOT_NS { namespace modules { namespace rt {

    ////////// ////////// ////////// ////////// //////////
    // generic_context_t

    // Generic context.
    class generic_context_t : public object_t
    {
    public:
        generic_context_t() { }

        // Converts generic params to runtime type.
        rt_type_t * parse(rt_generic_param_t * rt_generic_param);

    private:
    };

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        // Base class of variables layout.
        class __variables_layout_t : public object_t, public no_copy_ctor_t
        {
            typedef __variables_layout_t __self_t;

        public:

            // Constructor.
            __variables_layout_t(assembly_analyzer_t & ctx, rt_type_t ** generic_types)
                : __ctx(ctx), __generic_types(generic_types) { }

        protected:
            assembly_analyzer_t & __ctx;                // Context of assembly analyzer.
            rt_type_t **          __generic_types;      // Generic types.

            // Gets runtime type of typeref.
            rt_type_t * __get_type(ref_t type_ref)
            {
                switch((mt_type_extra_t)type_ref.extra)
                {
                    case mt_type_extra_t::generic_param:
                        X_UNEXPECTED();

                    default:
                        return __ctx.get_type(type_ref);
                }
            }
        };
    }

    ////////// ////////// ////////// ////////// //////////
    // locals_layout_t

    // Locals variables layout.
    class locals_layout_t : public __variables_layout_t
    {
        typedef locals_layout_t      __self_t;
        typedef __variables_layout_t __super_t;

    public:

        // Constructors.
        locals_layout_t(assembly_analyzer_t & ctx, rt_type_t ** generic_types);

        // Appends a local variable defination.
        void append(local_variable_defination_t & def);

        // Commits it.
        void commit();

        // Returns offset of local variable index.
        msize_t offset_of(int index);

        // Returns unit size of variables.
        msize_t unit_size() { return __local_unit_size; }

        // Converts to a string.
        operator string_t() const;

    private:

        // A local variable item.
        struct __item_t
        {
            // Constructor.
            __item_t(msize_t index, msize_t identity, msize_t size, storage_type_t storage_type)
                : index(index), identity(identity), size(size), storage_type(storage_type)
            { }

            msize_t index;                  // Index.
            msize_t identity;               // Identity.
            msize_t size;                   // Size.

            storage_type_t storage_type;    // Storage type.
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
            for(auto && it : __items)
            {
                f(index++, it->offset);
            }
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // params_layout_t

    // Params layout.
    class params_layout_t : public __variables_layout_t
    {
        typedef params_layout_t         __self_t;
        typedef __variables_layout_t    __super_t;

    public:
        using __super_t::__super_t;

        // Appends a field type.
        void append(ref_t type_ref, param_type_t param_type);

        // Appends a field type.
        void append(rt_type_t * type, param_type_t param_type);

        // Returns offset of field index.
        msize_t offset_of(int index);

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
    };

    ////////// ////////// ////////// ////////// //////////
    // type_layout_t

    // Type layout.
    class type_layout_t : public __variables_layout_t
    {
        typedef type_layout_t        __self_t;
        typedef __variables_layout_t __super_t;

    public:

        // Constructor.
        type_layout_t(assembly_analyzer_t & ctx, msize_t offset = 0,
                                                 rt_type_t ** generic_types = nullptr);

        // Appends field.
        void append(rt_field_t * field);

        // Commits it.
        void commit();

        // Returns type size.
        msize_t type_size() { return __offset; }

        // Returns storage type.
        storage_type_t storage_type() { return __storage_type; }

    private:

        // Item.
        struct __item_t
        {
            rt_field_t *    field;          // Field.
            msize_t         size;           // Field size.
            storage_type_t  storage_type;   // Storage type.
        };

        typedef al::svector_t<__item_t, 32> __items_t;

        msize_t         __offset;           // Offset.
        storage_type_t  __storage_type;     // Storage type.
        __items_t       __items;            // Items.
    };

    ////////// ////////// ////////// ////////// //////////

    // Returns aligned unit.
    constexpr msize_t unit_align(msize_t offset, msize_t size)
    {
        switch(size)
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
    constexpr msize_t unit_is_aligned(msize_t offset, msize_t size)
    {
        return unit_align(offset, size) == offset;
    }

    // Returns unit size.
    constexpr msize_t unit_size_of(msize_t size)
    {
        return _alignf(size, sizeof(rt_stack_unit_t)) / sizeof(rt_stack_unit_t);
    }

    ////////// ////////// ////////// ////////// //////////

} } }

#endif  // __RT_LAYOUT_H__
