
#include <rt.h>

namespace X_ROOT_NS { namespace modules { namespace rt {

    typedef rt_error_code_t __e_t;
    using namespace rt;

    ////////// ////////// ////////// ////////// //////////
    // generic_context_t

    // Converts generic param to runtime type.
    rt_type_t * generic_context_t::parse(rt_generic_param_t * rt_generic_param)
    {
        return nullptr;
    }

    ////////// ////////// ////////// ////////// //////////

    // Constructor.
    locals_layout_t::locals_layout_t(assembly_analyzer_t & ctx, rt_type_t ** generic_types)
        : __super_t(ctx, generic_types)
    {
        int storage_type = 0;
        al::for_each(__groups, [this, &storage_type](__group_t & g) {
            g.storage_type = (storage_type_t)storage_type++;
            g.__owner = this;
        });
    }

    // Finds reset index.
    msize_t locals_layout_t::__find_reset_index(msize_t index,
                                        storage_type_t storage_type)
    {
        for(int k = __items.size() - 1; k >= 0; k--)
        {
            __item_t & it = __items[k];
            if(it.index > index || it.storage_type != storage_type)
                continue;

            if(it.index == index)
                return it.new_index;

            return it.new_index + 1;
        }

        return 0;
    }

    // Appends a variable defination.
    void locals_layout_t::append(local_variable_defination_t & def)
    {
        storage_type_t storage_type;
        msize_t size = __ctx.variable_size_of(__get_type(def.type), &storage_type);

        if(__items.size() > 0 && def.index <= __items[__items.size() - 1].index)
        {
            for(int k = 0; k < X_ARRAY_SIZE(__groups); k++)
            {
                __group_t & g = __groups[k];
                msize_t reset_index = __find_reset_index(def.index, (storage_type_t)k);

                g.reset(reset_index);
            }
        }

        __item_t * item = __items.new_obj(
            def.index, __current_identity(), size, storage_type
        );

        __groups[(size_t)storage_type].append(item);
    }

    // Converts to string.
    locals_layout_t::__item_t::operator string_t() const
    {
        return _F(_T("ix:%1% id:%2% sz:%3% of:%4%"),
            index, identity, size, offset
        );
    }

    // Append a iterm to a group.
    void locals_layout_t::__group_t::append(__item_t * item)
    {
        item->new_index = __index++;
        items.push_back(item);

        if(__is_reset)
        {
            __is_reset = false;
            msize_t new_index = item->new_index;

            partials.push_back(
                __owner->__partial_heap.new_obj(new_index, items.size() - 1)
            );
        }
    }

    // Resets the index.
    void locals_layout_t::__group_t::reset(msize_t index)
    {
        __index = index;
        __is_reset = true;
    }

    // Commits it.
    void locals_layout_t::commit()
    {
        msize_t offset = 0;
        for(int storage_type = 0; storage_type < (int)storage_type_t::__end__; storage_type++)
        {
            __group_t & g = __groups[storage_type];
            if(!g.items.empty())
            {
                switch((storage_type_t)storage_type)
                {
                    case storage_type_t::ref:
                    case storage_type_t::value:
                        offset = __arrange_variables(
                            g, offset, g.partials.begin(), g.partials.end()
                        );
                        break;

                    case storage_type_t::mixture:
                        offset = __arrange_mixture_variables(
                            g, offset, g.partials.begin(), g.partials.end()
                        );
                        break;

                    default:
                        X_UNEXPECTED();
                }
            }
        }

        __local_unit_size = unit_align(offset, sizeof(rt_stack_unit_t)) / sizeof(rt_stack_unit_t);
    }

    // Arrange variables.
    template<typename f_t, typename caller_t>
    msize_t locals_layout_t::__arrange_variables_f(__group_t & g,
            msize_t offset, __partial_iterator_t it_begin, __partial_iterator_t it_end,
            f_t f, caller_t caller)
    {
        __partial_t * partial = *it_begin;

        msize_t end_idx;
        msize_t child_index = __min_index(it_begin, it_end, &end_idx);

        __item_iterator_t begin = g.items.begin() + partial->begin;
        __item_iterator_t end   = (child_index != __empty_msize)?
                  begin + (child_index - partial->index)
                : (end_idx != __empty_msize)? g.items.begin() + end_idx : g.items.end();

        msize_t new_offset = f(offset, begin, end);

        if(child_index != __empty_msize)
        {
            __partial_t bk = **it_begin;
            **it_begin = __partial_t(child_index, end - g.items.begin());
            caller(g, new_offset, it_begin, it_end);
            **it_begin = bk;

            for(__partial_iterator_t it = it_begin + 1; it < it_end; it++)
            {
                __partial_t * partial1 = *it;
                if(partial1->index <= partial->index)
                    break;

                if(partial1->index == child_index)
                    caller(g, new_offset, it, it_end);
            }

            return new_offset;
        }

        if(it_begin + 1 < it_end)
        {
            return al::max(new_offset,
                __arrange_variables_f(g, offset, it_begin + 1, it_end, f, caller)
            );
        }

        return new_offset;
    }

    // Arrange variables.
    msize_t locals_layout_t::__arrange_variables(__group_t & g,
            msize_t offset, __partial_iterator_t it_begin, __partial_iterator_t it_end)
    {
        using namespace std::placeholders;
        return __arrange_variables_f(g, offset, it_begin, it_end,
            std::bind(&locals_layout_t::__set_offsets, this, _1, _2, _3),
            std::bind(&locals_layout_t::__arrange_variables, this, _1, _2, _3, _4)
        );
    }

    // Arrange mixture variables.
    msize_t locals_layout_t::__arrange_mixture_variables(__group_t & g,
        msize_t offset, __partial_iterator_t it_begin, __partial_iterator_t it_end)
    {
        using namespace std::placeholders;
        __vector_t<__item_t *> items;

        __arrange_variables_f(g, 0, it_begin, it_end, [&](msize_t offset,
            __item_iterator_t it_begin, __item_iterator_t it_end) {
            return std::copy(it_begin, it_end, std::back_inserter(items)), 0;
        }, std::bind(&locals_layout_t::__arrange_mixture_variables, this, _1, _2, _3, _4));

        return __set_offsets(offset, items.begin(), items.end());
    }

    // The min index of specified local variable sequences.
    msize_t locals_layout_t::__min_index(__partial_iterator_t it_begin,
                                         __partial_iterator_t it_end, msize_t * out_end)
    {
        __partial_t * partial = *it_begin;

        msize_t min_index = __empty_msize;
        for(__partial_iterator_t it = it_begin + 1; it < it_end; it++)
        {
            __partial_t * partial1 = *it;

            if(partial1->index <= partial->index)
            {
                *out_end = partial1->begin;
                return min_index;
            }

            min_index = al::min(partial1->index, min_index);
        }

        *out_end = __empty_msize;
        return min_index;
    }

    // Sets offset.
    msize_t locals_layout_t::__set_offsets(msize_t offset,
                            __item_iterator_t it_begin, __item_iterator_t it_end)
    {
        //_PF(_T("__set_offsets: %1%, %2%"), (*it_begin)->identity, (*(it_end - 1))->identity);

        std::sort(it_begin, it_end, [](__item_t * it1, __item_t * it2) {
            return it1->size > it2->size;
        });

        msize_t offset_aligned = unit_align(offset, (*it_begin)->size);

        for(; it_end > it_begin + 1 && !unit_is_aligned(offset, (*it_begin)->size); )
        {
            __item_t * it = *(it_end - 1);
            msize_t offset0 = unit_align(offset, it->size) + it->size;
            if(offset0 <= offset_aligned)
            {
                it->offset = offset;
                offset = offset0;
                //_PF(_T("* %1%: size %2% offset %3%"), it->identity, it->size, it->offset);
                it_end--;
            }

            if(offset0 >= offset_aligned)
                break;
        }

        for(__item_iterator_t it = it_begin; it < it_end; it++)
        {
            msize_t size = (*it)->size;
            (*it)->offset = offset;

            offset = unit_align(offset, size) + size;
            //_PF(_T("# %1%: size %2% offset %3%"), (*it)->identity, size, (*it)->offset);
        }

        return offset;
    }

    // Returns offset of field index.
    msize_t locals_layout_t::offset_of(int index)
    {
        if(index >= __items.size())
            throw _ED(__e_t::local_index_out_of_range);

        return __items[index].offset;
    }

    // Converts to string.
    locals_layout_t::operator string_t() const
    {
        stringstream_t ss;

        for(const __group_t & g : __groups)
        {
            for(const __item_t * it : g.items)
            {
                ss << it->identity << _T("\t") << it->index << _T("\t")
                   << it->offset << _T("\t") << it->size << _T("\n");
            }
        }

        return ss.str();
    }

    ////////// ////////// ////////// ////////// //////////

    // Appends a field type.
    void params_layout_t::append(ref_t type_ref, param_type_t param_type)
    {
        rt_type_t * type = __get_type(type_ref);
        append(type, param_type);
    }

    // Appends a field type.
    void params_layout_t::append(rt_type_t * type, param_type_t param_type)
    {
        _A(type != nullptr);

        storage_type_t storage_type;
        msize_t unit_size = unit_size_of(type->get_variable_size(__ctx.env, &storage_type));

        __items.push_back(__item_t { type, param_type, __current_offset });
        __current_offset += unit_size;
    }

    // Commits it.
    void params_layout_t::commit()
    {

    }

    // Returns offset of field index.
    msize_t params_layout_t::offset_of(int index)
    {
        if(index >= __items.size())
            throw _ED(__e_t::argument_index_out_of_range);

        return __current_offset - __items[index].offset;
    }

    ////////// ////////// ////////// ////////// //////////

    // Constructor.
    type_layout_t::type_layout_t(assembly_analyzer_t & ctx, msize_t offset,
                                                            rt_type_t ** generic_types)
        : __super_t(ctx, generic_types), __offset(offset)
    { }

    // Appends field.
    void type_layout_t::append(rt_field_t * field)
    {
        _A(field != nullptr);

        rt_type_t * field_type = __get_type((*field)->type);
        _A(field_type != nullptr);

        storage_type_t storage_type;
        msize_t size = __ctx.size_of(field_type, &storage_type);

        __items.push_back(__item_t { field, size, storage_type });
    }

    // Commits it.
    void type_layout_t::commit()
    {
        al::sort(__items, [](__item_t & it1, __item_t & it2) {
            return std::make_tuple(it1.storage_type, it1.size)
                <  std::make_tuple(it2.storage_type, it2.size);
        });

        __storage_type = storage_type_t::__unknown__;

        for(__item_t & it : __items)
        {
            it.field->offset = __offset;
            __offset = unit_align(__offset, it.size) + it.size;

            if(__storage_type == storage_type_t::__unknown__)
                __storage_type = it.storage_type;
            else if(__storage_type != it.storage_type)
                __storage_type = storage_type_t::mixture;
        }
    }

    ////////// ////////// ////////// ////////// //////////

} } }

