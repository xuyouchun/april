
#include <rt.h>

namespace X_ROOT_NS { namespace modules { namespace rt {

    typedef rt_error_code_t __e_t;
    using namespace rt;

    ////////// ////////// ////////// ////////// //////////
    // generic_param_manager_t

    // Append a generic type with name.
    void generic_param_manager_t::append(rt_sid_t name, rt_type_t * atype)
    {
        _A(atype != nullptr);
        _A(__type_map.find(name) == __type_map.end());

        __type_map[name] = __type_item_t(atype, __atypes.size());
        __atypes.push_back(atype);
    }

    // Append a generic params types.
    void generic_param_manager_t::append(rt_type_t * atype)
    {
        _A(atype != nullptr);

        __atypes.push_back(atype);
    }

    // Append generic param types with name.
    void generic_param_manager_t::append(rt_sid_t name, int index, int count)
    {
        __named_atypes[name] = std::make_tuple(index, count);
    }

    // Returns type at index.
    rt_type_t * generic_param_manager_t::type_at(int index) const
    {
        if (index < 0 || index >= __atypes.size())
            throw _ED(__e_t::generic_param_index_out_of_range);

        return __atypes[index];
    }

    // Returns type of specified name.
    rt_type_t * generic_param_manager_t::type_at(rt_sid_t sid, int * out_index) const
    {
        auto it = __type_map.find(sid);
        if (it == __type_map.end())
            return nullptr;

        if (out_index != nullptr)
            *out_index = std::get<int>(it->second);

        return std::get<rt_type_t *>(it->second);
    }

    // Returns types position of specified name, for generic params.
    bool generic_param_manager_t::types_of(rt_sid_t sid, int * out_index, int * out_count) const
    {
        auto it = __named_atypes.find(sid);
        if (it == __named_atypes.end())
            return false;

        std::tuple<int, int> & r = it->value;

        if (out_index != nullptr)
            *out_index = std::get<0>(r);

        if (out_count != nullptr)
            *out_count = std::get<1>(r);

        return true;
    }

    // Returns the empty manager.
    const generic_param_manager_t * generic_param_manager_t::empty_instance()
    {
        static generic_param_manager_t empty_;
        return &empty_;
    }

    ////////// ////////// ////////// ////////// //////////

    // Type placeholder.
    class __type_place_holder_t : public rt_type_t, public object_t
    {
        // Pre new object.
        virtual void pre_new(analyzer_env_t & env) override { }

        // Pre call static method.
        virtual void pre_static_call(analyzer_env_t & env) override { }

        // Gets type name.
        virtual rt_sid_t get_name(analyzer_env_t & env) override { return rt_sid_t(); }

        // Gets type kind.
        virtual rt_type_kind_t get_kind() override { return rt_type_kind_t::__unknown__; }

        // Gets base type.
        virtual rt_type_t * get_base_type(analyzer_env_t & env,
                                          const __gp_mgr_t * gp_manager = nullptr) override
        {
            return nullptr;
        }

        // Gets method offset.
        virtual int get_method_offset(analyzer_env_t & env, ref_t method_ref) override
        {
            return 0;
        }

        // Gets field offset.
        virtual msize_t get_field_offset(analyzer_env_t & env, ref_t ref) override
        {
            return 0;
        }

        // Gets field offset.
        virtual msize_t get_field_offset(analyzer_env_t & env, int position) override
        {
            return 0;
        }

        virtual void each_field(analyzer_env_t & env, each_field_t f) override { }

        // Enums all methods.
        virtual void each_method(analyzer_env_t & env, each_method_t f) override { }

        // Searches method.
        virtual ref_t search_method(analyzer_env_t & env,
                    method_prototype_t & prototype,
                    search_method_options_t options = search_method_options_t::default_) override
        {
            return ref_t::null;
        }

        // Gets variable size.
        virtual msize_t get_variable_size(analyzer_env_t & env,
                                          storage_type_t * out_storage_type) override
        {
            return 0;
        }

        // Gets assembly.
        virtual rt_assembly_t * get_assembly() override { return nullptr; }

        // Gets data type.
        virtual vtype_t get_vtype(analyzer_env_t & env) override { return vtype_t::__unknown__; }

        // Returns host type.
        virtual rt_type_t * get_host_type() override { return nullptr; }

        X_TO_STRING_IMPL(_T("__type_place_holder_t"))

    protected:

        // When caculate size.
        virtual msize_t on_caculate_size(analyzer_env_t & env,
                                        storage_type_t * out_storage_type) override { return 0; }


        // When caculate layout.
        virtual msize_t on_caculate_layout(analyzer_env_t & env, msize_t base_size,
                                        storage_type_t * out_storage_type) override { return 0; }

    };

    //-------- ---------- ---------- ---------- ----------

    // Type placeholder manager.
    class type_place_holder_manager_t : private memory_base_t
    {
        typedef memory_base_t __super_t;
        typedef __type_place_holder_t __holder_t;

    public:

        // Constructor.
        type_place_holder_manager_t(memory_t * memory = nullptr) : __super_t(memory)
        {
            for (__holder_t ** p = (__holder_t **)__types, ** p_end = p + __types_size;
                                                                    p < p_end; p++)
            {
                *p = this->__new_obj<__holder_t>();
            }
        }

        // Returns placeholder for index.
        rt_type_t * get(int index)
        {
            if (index < __types_size)
                return __types[index];

            _L(__mutex);

            auto it = __type_map.find(index);
            if (it != __type_map.end())
                return it->second;

            __holder_t * type = this->__new_obj<__holder_t>();
            __type_map[index] = type;

            return type;
        }

        // Destructor.
        virtual ~type_place_holder_manager_t()
        {
            /* TODO:
            for (__holder_t * p : __types)
            {
                this->__delete_obj(p);
            }

            for (auto && it : __type_map)
            {
                this->__delete_obj(it.second);
            }
            */
        }

    private:
        std::map<int, __holder_t *> __type_map;
        static const size_t __types_size = 16;
        __holder_t * __types[__types_size];

        std::mutex __mutex;
    };

    //-------- ---------- ---------- ---------- ----------

    // Returns placeholder for index.
    rt_type_t * __get_type_place_holder(int index)
    {
        static type_place_holder_manager_t mgr;
        return mgr.get(index);
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_param_manager_builder_t

    // Appends generic params
    void generic_param_manager_builder_t::append(ref_t generic_params, rt_type_t ** types,
                                                 int type_count)
    {
        _A(generic_params.count <= type_count);

        rt_type_t ** types_end = types + type_count;

        for (ref_t gp_ref : generic_params)
        {
            rt_generic_param_t * gp = __analyzer.get_generic_param(gp_ref);
            rt_sid_t name = __analyzer.to_sid((*gp)->name);

            if ((generic_param_type_t)(*gp)->param_type == generic_param_type_t::params)
            {
                __gp_mgr.append(name, __gp_mgr.size(), types_end - types);
                while (types < types_end)
                {
                    // _PP((*types)->get_name(__analyzer.env));
                    __gp_mgr.append(*types++);
                }

                break;
            }
            else
            {
                // _P(_T("default type"), name, (*types)->get_name(__analyzer.env));
                __gp_mgr.append(name, *types++);
            }
        }
    }

    // Appends generic params
    void generic_param_manager_builder_t::append(ref_t generic_params)
    {
        for (ref_t gp_ref : generic_params)
        {
            rt_generic_param_t * gp = __analyzer.get_generic_param(gp_ref);
            rt_sid_t name = __analyzer.to_sid((*gp)->name);

            __gp_mgr.append(name, __get_type_place_holder(__gp_mgr.size()));
        }
    }

    // Append generic params of given generic method.
    void generic_param_manager_builder_t::append(rt_generic_method_t * m)
    {
        _A(m != nullptr);

        append((*m->template_)->generic_params, m->atypes, m->atype_count());
    }

    // Append generic params of given generic type.
    void generic_param_manager_builder_t::append(rt_generic_type_t * t)
    {
        _A(t != nullptr);

        append((*t->template_)->generic_params, t->atypes, t->atype_count());
    }

    // Append generic params of given general type.
    void generic_param_manager_builder_t::append(rt_general_type_t * t)
    {
        _A(t != nullptr);

        append((*t)->generic_params);
    }

    // Appends generic params
    void generic_param_manager_builder_t::append(rt_type_t * t)
    {
        _A(t != nullptr);

        switch (t->get_kind())
        {
            case rt_type_kind_t::generic:
                append((rt_generic_type_t *)t);
                break;

            case rt_type_kind_t::general:
                append((rt_general_type_t *)t);
                break;

            default:
                break;
        }
    }

    // Enumerates all generic host types.
    template<typename _f_t> void __each_generic_hosts(rt_type_t * type, _f_t f)
    {
        rt_type_t * host_type = type->get_host_type();
        if (host_type != nullptr)
            __each_generic_hosts((rt_generic_type_t *)host_type, f);

        f(type);
    }

    // Imports generic params of given method.
    void generic_param_manager_builder_t::import(rt_type_t * t)
    {
        _A(t != nullptr);

        __each_generic_hosts(t, [&, this](rt_type_t * t0) {
            this->append(t0);
        });
    }

    // Imports generic params of given generic method.
    void generic_param_manager_builder_t::import(rt_generic_method_t * m)
    {
        _A(m != nullptr);

        rt_type_t * host_type = m->get_host_type();
        import(host_type);

        append(m);
    }

    // Imports generic params of given general method.
    void generic_param_manager_builder_t::import(rt_method_t * m)
    {
        _A(m != nullptr);

        rt_type_t * host_type = m->get_host_type();
        import(host_type);
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_context_t

    // Converts generic param to runtime type.
    rt_type_t * generic_context_t::parse(rt_generic_param_t * rt_generic_param)
    {
        return nullptr;
    }

    ////////// ////////// ////////// ////////// //////////

    // Constructor.
    __variables_layout_t::__variables_layout_t(assembly_analyzer_t & ctx)
        : __ctx(ctx) { }

    ////////// ////////// ////////// ////////// //////////
    // __variables_layout_t

    // Gets runtime type of typeref.
    rt_type_t * __variables_layout_t::__get_type(ref_t type_ref,
            rt_generic_param_t ** out_generic_param, int * out_index)
    {
        switch ((mt_type_extra_t)type_ref.extra)
        {
            case mt_type_extra_t::generic_param: {

                rt_generic_param_t * gp = __ctx.get_generic_param(type_ref);
                _A(gp != nullptr);
                al::assign_value(out_generic_param, gp);

                rt_sid_t name = __ctx.to_sid((*gp)->name);
                return __ctx.gp_manager->type_at(name, out_index);

            }   break;

            default:
                al::assign_value(out_generic_param, (rt_generic_param_t *)nullptr);
                return __ctx.get_type(type_ref);
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Constructor.
    locals_layout_t::locals_layout_t(assembly_analyzer_t & ctx)
        : __super_t(ctx)
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
        for (int k = __items.size() - 1; k >= 0; k--)
        {
            __item_t & it = __items[k];
            if (it.index > index || it.storage_type != storage_type)
                continue;

            if (it.index == index)
                return it.new_index;

            return it.new_index + 1;
        }

        return 0;
    }

    // Appends a variable defination.
    void locals_layout_t::append(local_variable_defination_t & def)
    {
        storage_type_t storage_type;
        rt_type_t * type = __get_type(def.type);
        msize_t size = __ctx.variable_size_of(type, &storage_type);

        if (__items.size() > 0 && def.index <= __items[__items.size() - 1].index)
        {
            for (int k = 0; k < X_ARRAY_SIZE(__groups); k++)
            {
                __group_t & g = __groups[k];
                msize_t reset_index = __find_reset_index(def.index, (storage_type_t)k);

                g.reset(reset_index);
            }
        }

        __item_t * item = __items.new_obj(
            def.index, __current_identity(), size, storage_type, type
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

        if (__is_reset)
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
        for (int storage_type = 0; storage_type < (int)storage_type_t::__end__; storage_type++)
        {
            __group_t & g = __groups[storage_type];
            if (!g.items.empty())
            {
                switch ((storage_type_t)storage_type)
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

        if (child_index != __empty_msize)
        {
            msize_t max_offset = 0;

            __partial_t bk = **it_begin;
            **it_begin = __partial_t(child_index, end - g.items.begin());
            max_offset = al::max(max_offset, caller(g, new_offset, it_begin, it_end));
            **it_begin = bk;

            for (__partial_iterator_t it = it_begin + 1; it < it_end; it++)
            {
                __partial_t * partial1 = *it;
                if (partial1->index <= partial->index)
                    break;

                if (partial1->index == child_index)
                    max_offset = al::max(max_offset, caller(g, new_offset, it, it_end));
            }

            return al::max(new_offset, max_offset);
        }

        if (it_begin + 1 < it_end)
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
        for (__partial_iterator_t it = it_begin + 1; it < it_end; it++)
        {
            __partial_t * partial1 = *it;

            if (partial1->index <= partial->index)
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

        for (; it_end > it_begin + 1 && !unit_is_aligned(offset, (*it_begin)->size); )
        {
            __item_t * it = *(it_end - 1);
            msize_t offset0 = unit_align(offset, it->size) + it->size;
            if (offset0 <= offset_aligned)
            {
                it->offset = offset;
                offset = offset0;
                //_PF(_T("* %1%: size %2% offset %3%"), it->identity, it->size, it->offset);
                it_end--;
            }

            if (offset0 >= offset_aligned)
                break;
        }

        for (__item_iterator_t it = it_begin; it < it_end; it++)
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
        if (index >= __items.size())
            throw _ED(__e_t::local_index_out_of_range);

        return __items[index].offset;
    }

    // Returns type of local variable index.
    rt_type_t * locals_layout_t::type_at(int index)
    {
        if (index >= __items.size())
            throw _ED(__e_t::local_index_out_of_range);

        return __items[index].type;
    }

    // Converts to string.
    locals_layout_t::operator string_t() const
    {
        stringstream_t ss;
        ss << _T("identity index offset size\n");

        for (const __group_t & g : __groups)
        {
            for (const __item_t * it : g.items)
            {
                ss << it->identity << _T("\t") << it->index << _T("\t")
                   << it->offset << _T("\t") << it->size << _T("\n");
            }
        }

        return ss.str();
    }

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(param_layout_type_t)

        X_C(default_,   _T("default"))

        X_C(this_,      _T("this"))

    X_ENUM_INFO_END

    // Appends a param.
    void params_layout_t::append(ref_t type_ref, param_type_t param_type,
                param_layout_type_t layout_type)
    {
        if (param_type == param_type_t::extends)
        {
            if (__extends_params_offset < 0)
                __extends_params_offset = __current_offset;

            param_type = param_type_t::__default__;
        }

        rt_generic_param_t * gp;
        int index;

        rt_type_t * type = __get_type(type_ref, &gp, &index);
        append(type, param_type, layout_type);

        // Params generic type.
        if (gp != nullptr && (generic_param_type_t)(*gp)->param_type == generic_param_type_t::params)
        {
            for (size_t index1 = index + 1, count = __ctx.gp_manager->size();
                index1 < count; index1++)
            {
                rt_type_t * type1 = __ctx.gp_manager->type_at(index1);
                append(type1, param_type, layout_type);
            }
        }
    }

    // Appends a param.
    void params_layout_t::append(rt_type_t * type, param_type_t param_type,
                param_layout_type_t layout_type)
    {
        typedef param_type_t p_t;

        _A(type != nullptr);

        storage_type_t storage_type;
        msize_t unit_size = param_type == p_t::ref || param_type == p_t::out
                            || layout_type == param_layout_type_t::this_? 1
            : unit_size_of(type->get_variable_size(__ctx.env, &storage_type));

        __items.push_back(__item_t { type, param_type, __current_offset });
        __current_offset += unit_size;
    }

    // Commits it.
    void params_layout_t::commit()
    {
        // Nothing to do.
    }

    // Returns offset of index.
    msize_t params_layout_t::offset_of(int index)
    {
        if (index >= __items.size())
        {
            if (index == __items.size())
                return 0;

            throw _ED(__e_t::argument_index_out_of_range);
        }

        return __current_offset - __items[index].offset;
    }

    // Returns offset of the first extends param.
    msize_t params_layout_t::extends_offset()
    {
        if (__extends_params_offset < 0)
            return 0;

        return __current_offset - __extends_params_offset;
    }

    // Returns param type of index.
    rt_type_t * params_layout_t::type_at(int index)
    {
        if (index >= __items.size())
            throw _ED(__e_t::argument_index_out_of_range);

        return __items[index].type;
    }

    ////////// ////////// ////////// ////////// //////////

} } }

