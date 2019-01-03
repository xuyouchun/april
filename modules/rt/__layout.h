#ifndef __RT_LAYOUT_H__
#define __RT_LAYOUT_H__

namespace X_ROOT_NS { namespace modules { namespace rt {

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        class __variables_layout_t : public object_t, public no_copy_ctor_t
        {
            typedef __variables_layout_t __self_t;

        public:
            __variables_layout_t(assembly_analyzer_t & ctx)
                : __ctx(ctx) { }

        protected:
            assembly_analyzer_t & __ctx;
        };
    }

    ////////// ////////// ////////// ////////// //////////

    class locals_layout_t : public __variables_layout_t
    {
        typedef locals_layout_t      __self_t;
        typedef __variables_layout_t __super_t;

    public:
        locals_layout_t(assembly_analyzer_t & ctx);

        void append(local_variable_defination_t & def);
        void commit();
        msize_t offset_of(int index);
        msize_t unit_size() { return __local_unit_size; }

    private:
        struct __item_t
        {
            __item_t(msize_t index, msize_t identity, msize_t size, storage_type_t storage_type)
                : index(index), identity(identity), size(size), storage_type(storage_type)
            { }

            msize_t index;
            msize_t identity;
            msize_t size;

            storage_type_t storage_type;
            bool region_flag = false;

            msize_t offset;
            msize_t new_index;

            operator string_t() const;
        };

        msize_t __identity = 0;
        msize_t __local_unit_size = 0;
        static const msize_t __empty_msize = max_value<msize_t>();

        al::heap_t<__item_t> __items;

        struct __partial_t
        {
            __partial_t(msize_t index, msize_t begin)
                : index(index), begin(begin) { }

            msize_t index, begin;
            msize_t offset = 0;

            operator string_t() const
            {
                return _F(_T("index: %1%, offset: %2%"), index, offset);
            }
        };

        typedef al::small_vector_t<__partial_t *, 16> __partials_t;
        typedef typename __partials_t::iterator __partial_iterator_t;

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

        msize_t __find_reset_index(msize_t index, storage_type_t storage_type);

        msize_t __current_identity() { return __identity++; }

        template<typename f_t>
        msize_t __arrange_variables_f(__group_t & g, msize_t offset,
                    __partial_iterator_t it_begin, __partial_iterator_t it_end, f_t f);

        msize_t __arrange_variables(__group_t & g, msize_t offset,
                    __partial_iterator_t it_begin, __partial_iterator_t it_end);

        msize_t __arrange_mixture_variables(__group_t & g, msize_t offset,
                    __partial_iterator_t it_begin, __partial_iterator_t it_end);

        msize_t __min_index(__partial_iterator_t it_begin,
                    __partial_iterator_t it_end, msize_t * out_end);

        msize_t __set_offsets(msize_t offset,
                    __item_iterator_t it_begin, __item_iterator_t it_end);

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

    class params_layout_t : public __variables_layout_t
    {
        typedef params_layout_t         __self_t;
        typedef __variables_layout_t    __super_t;

    public:
        using __super_t::__super_t;

        void append(ref_t type_ref, param_type_t param_type);
        void append(rt_type_t * type, param_type_t param_type);

        msize_t offset_of(int index);
        void commit();

        msize_t unit_size() const { return __current_offset; }

    private:
        struct __item_t
        {
            rt_type_t * type;
            param_type_t param_type;
            msize_t offset;
        };

        typedef small_vector_t<__item_t, 32> __items_t;
        __items_t __items;

        msize_t __current_offset = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    class type_layout_t : public __variables_layout_t
    {
        typedef type_layout_t        __self_t;
        typedef __variables_layout_t __super_t;

    public:
        type_layout_t(assembly_analyzer_t & ctx, msize_t offset = 0);

        void append(rt_field_t * field);
        void commit();

        msize_t type_size() { return __offset; }
        storage_type_t storage_type() { return __storage_type; }

    private:
        struct __item_t
        {
            rt_field_t *    field;
            msize_t         size;
            storage_type_t  storage_type;
        };

        typedef al::small_vector_t<__item_t, 32> __items_t;

        msize_t         __offset;
        storage_type_t  __storage_type;
        __items_t       __items;
    };

    ////////// ////////// ////////// ////////// //////////

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

    constexpr msize_t unit_is_aligned(msize_t offset, msize_t size)
    {
        return unit_align(offset, size) == offset;
    }

    constexpr msize_t unit_size_of(msize_t size)
    {
        return _alignf(size, sizeof(rt_stack_unit_t)) / sizeof(rt_stack_unit_t);
    }

    ////////// ////////// ////////// ////////// //////////

} } }

#endif  // __RT_LAYOUT_H__
