#include <rt.h>
#include "utils.h"

namespace X_ROOT_NS::modules::rt {

    using namespace core;

    ////////// ////////// ////////// ////////// //////////

    // The flags of a decorate: the access (access_value_t) in the 4 lowest bits, then one bit for
    // static, sealed, override, virtual, abstract, new, const, readonly, extern and partial.
    static int64_t __decorate_flags(const decorate_value_t & d)
    {
        int64_t flags = (int64_t)d.access;

        flags |= d.is_static?   (1 << 4)  : 0;
        flags |= d.is_sealed?   (1 << 5)  : 0;
        flags |= d.is_override? (1 << 6)  : 0;
        flags |= d.is_virtual?  (1 << 7)  : 0;
        flags |= d.is_abstract? (1 << 8)  : 0;
        flags |= d.is_new?      (1 << 9)  : 0;
        flags |= d.is_const?    (1 << 10) : 0;
        flags |= d.is_readonly? (1 << 11) : 0;
        flags |= d.is_extern?   (1 << 12) : 0;
        flags |= d.is_partial?  (1 << 13) : 0;

        return flags;
    }

    // Makes a member.
    rt_reflect_member_t * rt_reflection_t::__new_member(const rt_char_t * name, void * declaring,
                                                        void * type, int64_t flags)
    {
        __member_pool.emplace_back();

        rt_reflect_member_t * m = &__member_pool.back();
        static const rt_char_t __empty[] = { 0 };
        m->name      = name != nullptr? name : __empty;
        m->declaring = declaring;
        m->type      = type;
        m->flags     = flags | reflect_flag_member;

        __member_set.insert(m);

        return m;
    }

    // Makes a list of members, null if there is none.
    rt_reflect_member_t ** rt_reflection_t::__new_list(
                                    const std::vector<rt_reflect_member_t *> & items)
    {
        if (items.empty())
            return nullptr;

        __list_pool.push_back(items);
        return __list_pool.back().data();
    }

    // Returns the member that stands for an assembly (it has only its name).
    rt_reflect_member_t * rt_reflection_t::__assembly_of(rt_assembly_t * assembly)
    {
        auto it = __assemblies.find(assembly);
        if (it != __assemblies.end())
            return it->second;

        rt_reflect_member_t * m = __new_member(assembly->get_name().ce_str(), nullptr, nullptr, 0);
        __assemblies[assembly] = m;
        __set_attributes(m, assembly, assembly->get_attributes());

        return m;
    }

    // Keeps the attributes of a member or of a type.
    void rt_reflection_t::__set_attributes(void * key, rt_assembly_t * assembly, ref_t refs)
    {
        if (refs == ref_t::null)
            return;

        __attributes[key] = rt_reflect_attributes_t { assembly, refs };
    }

    // Returns the attributes of a member or of a type.
    bool rt_reflection_t::get_attributes(void * key, rt_reflect_attributes_t * out)
    {
        auto it = __attributes.find(key);
        if (it == __attributes.end())
            return false;

        *out = it->second;
        return true;
    }

    // Returns the reflection of a type.
    rt_reflect_type_t * rt_reflection_t::get_type(rt_type_t * type)
    {
        auto it = __types.find(type);
        if (it != __types.end())
            return it->second;

        __type_pool.emplace_back();
        rt_reflect_type_t * info = &__type_pool.back();
        __types[type] = info;

        __build(info, type);
        return info;
    }

    // Builds the reflection of a type, from the metadata of its template.
    void rt_reflection_t::__build(rt_reflect_type_t * info, rt_type_t * type)
    {
        typedef rt_mt_t<__tidx_t::param> mt_param_t;

        rt_type_kind_t kind = type->get_kind();
        if (kind != rt_type_kind_t::general && kind != rt_type_kind_t::generic)
            return;

        rt_general_type_t * template_;
        rt_generic_type_t * owner = nullptr;

        if (kind == rt_type_kind_t::generic)
        {
            owner = static_cast<rt_generic_type_t *>(type);
            template_ = owner->template_;
            if (template_ == nullptr)
                return;
        }
        else
        {
            template_ = static_cast<rt_general_type_t *>(type);
        }

        rt_assembly_t * assembly = template_->assembly;

        info->flags = ((int64_t)type->get_ttype(__env) << reflect_flag_ttype_shift)
                    | __decorate_flags((decorate_value_t)(*template_)->decorate);
        info->base = type->get_base_type(__env);

        // The type of the number of an enum: the native back end reads the number of a box by it.
        if (type->get_ttype(__env) == ttype_t::enum_)
            info->flags |= (int64_t)type->get_vtype(__env) << reflect_flag_underlying_shift;
        info->assembly = __assembly_of(assembly);
        __set_attributes(type, assembly, (*template_)->attributes);

        // The types of a tuple are made by the runtime: they have no members of metadata.
        if (is_tuple_type(type))
            return;

        // The types of the members of a generic type are those of its arguments.
        assembly_analyzer_t analyzer(__env, assembly);
        std::unique_ptr<generic_param_manager_t> gp_mgr;
        if (owner != nullptr)
            gp_mgr.reset(new generic_param_manager_t(analyzer, owner));

        assembly_analyzer_t ga(__env, assembly, gp_mgr.get());

        // The types of the members of a generic type that is not made of arguments (List<T>) are
        // not known: they are null.
        bool open = owner == nullptr && (*template_)->generic_params.count > 0;

        auto type_of = [&](ref_t ref, bool is_generic_method) -> void * {
            if (open || is_generic_method || ref == ref_t::null)
                return nullptr;

            // The types of a params generic parameter (TArgs ... args) are many: none of them.
            if (ga.is_generic_params(ref))
                return nullptr;

            return ga.get_type(ref);
        };

        std::vector<rt_reflect_member_t *> fields, methods, properties, events, gparams;
        std::map<rt_method_t *, rt_reflect_member_t *> method_map;

        // How the value of a parameter or of a result is read and written (a void is none).
        auto describe = [&](rt_reflect_member_t * m, rt_type_t * t) {
            if (t == nullptr || t->get_vtype(__env) == vtype_t::void_)
                return;

            ttype_t ttype = t->get_ttype(__env);
            bool is_string = t->get_vtype(__env) == vtype_t::string_;

            m->value_kind = (int64_t)(is_string? rt_reflect_value_kind_t::string :
                    is_ref_type(ttype)? rt_reflect_value_kind_t::reference
                                      : rt_reflect_value_kind_t::value);
            m->size = (int64_t)(ttype == ttype_t::struct_? t->get_size(__env)
                                                          : t->get_variable_size(__env));
        };

        // Parameters of a method or a property.
        auto make_params = [&](ref_t params, bool is_generic_method) {
            std::vector<rt_reflect_member_t *> list;

            assembly->each_params(params, [&](int, mt_param_t & p) {

                // The items of a params generic parameter (TArgs ... args) of a type that is made of
                // arguments (Delegate<R, A, B>): a parameter for each of them, with its type.
                rt_generic_param_t * pack_gp = nullptr;
                if (!open && !is_generic_method && owner != nullptr
                        && ga.is_generic_params(p.type, &pack_gp))
                {
                    int pack_start = (int)(*template_)->generic_params.count - 1;
                    for (int k = pack_start; k < owner->atype_count(); k++)
                    {
                        list.push_back(__new_member(analyzer.to_cstr(p.name), type,
                            (void *)owner->atypes[k],
                            ((int64_t)p.param_type << reflect_flag_param_shift)
                                | ((int64_t)list.size() << reflect_flag_position_shift)));
                        describe(list.back(), (rt_type_t *)list.back()->type);
                        __set_attributes(list.back(), assembly, p.attributes);
                    }

                    return true;
                }

                list.push_back(__new_member(analyzer.to_cstr(p.name), type,
                    type_of(p.type, is_generic_method),
                    ((int64_t)p.param_type << reflect_flag_param_shift)
                        | ((int64_t)list.size() << reflect_flag_position_shift)));
                describe(list.back(), (rt_type_t *)list.back()->type);
                __set_attributes(list.back(), assembly, p.attributes);
                return true;
            });

            return list;
        };

        // Fields.
        template_->each_field(__env, [&](ref_t ref, rt_field_base_t * field_base) {
            rt_field_t * field = dynamic_cast<rt_field_t *>(field_base);
            if (field == nullptr)
                return true;

            rt_type_t * field_type = open? nullptr : field->get_field_type(ga, owner);
            rt_reflect_member_t * m = __new_member(assembly->get_name(field).ce_str(), type,
                field_type, __decorate_flags((decorate_value_t)(*field)->decorate)
                                | reflect_flag_field);

            // How the value is read and written (a constant has no place in the objects).
            if (field_type != nullptr && !((decorate_t)(*field)->decorate).is_const)
            {
                type->pre_new(__env);
                m->offset = (int64_t)type->get_field_offset(__env, ref);

                ttype_t ttype = field_type->get_ttype(__env);
                bool is_string = field_type->get_vtype(__env) == vtype_t::string_;

                m->value_kind = (int64_t)(is_string? rt_reflect_value_kind_t::string :
                        is_ref_type(ttype)? rt_reflect_value_kind_t::reference
                                          : rt_reflect_value_kind_t::value);
                m->size = (int64_t)(ttype == ttype_t::struct_? field_type->get_size(__env)
                                                              : field_type->get_variable_size(__env));
            }

            // The value of a constant of a number or an enum (an item of an enum) is in the table: the offset.
            else if (field_type != nullptr && (*field)->init_value != ref_t::null
                        && (field_type->get_ttype(__env) == ttype_t::enum_
                            || is_integer(field_type->get_vtype(__env))))
            {
                cvalue_t value = assembly->get_constant((*field)->init_value);

                if (value.value_type == cvalue_type_t::number)
                {
                    m->offset     = value.change_type(vtype_t::int64_).number.get_value<int64_t>();
                    m->value_kind = (int64_t)rt_reflect_value_kind_t::value;
                    m->size       = (int64_t)field_type->get_variable_size(__env);
                }
            }

            __set_attributes(m, assembly, (*field)->attributes);

            fields.push_back(m);
            return true;
        });

        // Methods.
        template_->each_method(__env, [&](ref_t ref, rt_method_base_t * method_base) {
            rt_method_t * method = dynamic_cast<rt_method_t *>(method_base);
            if (method == nullptr)
                return true;

            // The types of a generic method are made of its own arguments: they are null.
            bool is_generic_method = method->generic_param_count() > 0;

            rt_reflect_member_t * m = __new_member(method->get_name().ce_str(), type,
                type_of((*method)->type, is_generic_method),
                __decorate_flags((decorate_value_t)(*method)->decorate)
                    | ((int64_t)(*method)->trait << reflect_flag_trait_shift));

            // The result, and the offset of a virtual method (the native back end calls it
            // through the descriptor of the object).
            describe(m, (rt_type_t *)m->type);
            m->offset = -1;

            // The methods of a number or of a string take the value for the this: the object is its
            // box (the native back end passes the bytes of the box).
            {
                vtype_t this_vtype = type->get_vtype(__env);
                if (!((decorate_t)(*method)->decorate).is_static
                        && (is_system_value_type(this_vtype) || this_vtype == vtype_t::string_
                            || type->get_ttype(__env) == ttype_t::enum_))
                {
                    m->flags |= reflect_flag_value_this
                            | ((int64_t)(this_vtype == vtype_t::string_? 8 : type->get_size(__env)) << 24);
                }
            }

            decorate_t method_decorate = (decorate_t)(*method)->decorate;

            // A method of an interface: its slot among those of the interface (a static method
            // has none), the object that it is called on has the function in its table of it.
            if (!is_generic_method && type->get_ttype(__env) == ttype_t::interface_
                    && !method_decorate.is_static)
            {
                m->offset = (int64_t)template_->get_method_offset(__env, ref);
            }

            if (!is_generic_method && type->get_ttype(__env) != ttype_t::interface_)
            {
                if (owner == nullptr)
                {
                    if (method_decorate.is_virtual || method_decorate.is_override
                                                    || method_decorate.is_abstract)
                    {
                        try
                        {
                            m->offset = (int64_t)analyzer.get_virtual_method_offset(ref);
                        }
                        catch (...)
                        {
                            m->offset = -1;
                        }
                    }
                }
                else if (method_decorate.is_virtual || method_decorate.is_abstract)
                {
                    // The virtual functions of an instance of a generic type: those of its base type,
                    // then its own (an override takes the place of one of the base type: it is called
                    // directly).
                    int64_t offset = (int64_t)template_->get_method_offset(__env, ref);

                    rt_type_t * base_type = type->get_base_type(__env);
                    if (base_type != nullptr)
                        offset += base_type->get_virtual_count(__env);

                    m->offset = offset;
                }
            }

            // The method that is called by Invoke (the interpreter): that of the arguments of a
            // generic type is a generic method.
            m->static_data = owner != nullptr?
                (void *)analyzer.to_generic_method(method, owner) : (void *)method;

            std::vector<rt_reflect_member_t *> params = make_params((*method)->params,
                                                                            is_generic_method);
            m->param_count = (int64_t)params.size();
            m->params = __new_list(params);

            std::vector<rt_reflect_member_t *> gps;
            ref_t gp_refs = (*method)->generic_params;
            for (ref_t gp_ref : gp_refs)
            {
                rt_generic_param_t * gp = analyzer.get_generic_param(gp_ref);
                gps.push_back(__new_member(analyzer.to_cstr((*gp)->name), type, nullptr,
                    ((int64_t)(*gp)->param_type << reflect_flag_param_shift)
                        | ((int64_t)(gp_ref.index - gp_refs.index) << reflect_flag_position_shift)));
                __set_attributes(gps.back(), assembly, (*gp)->attributes);
            }

            m->gparam_count = (int64_t)gps.size();
            m->gparams = __new_list(gps);

            __set_attributes(m, assembly, (*method)->attributes);

            methods.push_back(m);
            method_map[method] = m;
            return true;
        });

        // The accessor of a property or an event: one of the methods.
        auto accessor = [&](ref_t ref) -> rt_reflect_member_t * {
            if (ref == ref_t::null)
                return nullptr;

            auto it = method_map.find(assembly->get_method(ref));
            return it == method_map.end()? nullptr : it->second;
        };

        // Properties.
        for (ref_t ref : (*template_)->properties)
        {
            rt_property_t * property = assembly->get_property(ref);

            rt_reflect_member_t * m = __new_member(analyzer.to_cstr((*property)->name), type,
                type_of((*property)->type, false),
                __decorate_flags((decorate_value_t)(*property)->decorate));

            m->accessor_a = accessor((*property)->get_method);
            m->accessor_b = accessor((*property)->set_method);

            std::vector<rt_reflect_member_t *> params = make_params((*property)->params, false);
            m->param_count = (int64_t)params.size();
            m->params = __new_list(params);

            describe(m, (rt_type_t *)m->type);
            __set_attributes(m, assembly, (*property)->attributes);

            properties.push_back(m);
        }

        // Events.
        for (ref_t ref : (*template_)->events)
        {
            rt_event_t * event = assembly->get_event(ref);

            rt_reflect_member_t * m = __new_member(analyzer.to_cstr((*event)->name), type,
                type_of((*event)->type, false),
                __decorate_flags((decorate_value_t)(*event)->decorate));

            m->accessor_a = accessor((*event)->add_method);
            m->accessor_b = accessor((*event)->remove_method);
            m->accessor_c = accessor((*event)->raise_method);

            __set_attributes(m, assembly, (*event)->attributes);

            events.push_back(m);
        }

        // The generic parameters of the type.
        ref_t type_gp_refs = (*template_)->generic_params;
        for (ref_t gp_ref : type_gp_refs)
        {
            rt_generic_param_t * gp = analyzer.get_generic_param(gp_ref);
            gparams.push_back(__new_member(analyzer.to_cstr((*gp)->name), type, nullptr,
                ((int64_t)(*gp)->param_type << reflect_flag_param_shift)
                    | ((int64_t)(gp_ref.index - type_gp_refs.index) << reflect_flag_position_shift)));
            __set_attributes(gparams.back(), assembly, (*gp)->attributes);
        }

        // The nest types of a type: those of the instances of generic types are not listed.
        std::vector<rt_reflect_member_t *> nested;
        if (owner == nullptr)
        {
            for (ref_t ref : (*template_)->nest_types)
            {
                rt_nest_type_t * nest = assembly->get_nest_type(ref);
                rt_type_t * nested_type = nest != nullptr? analyzer.get_type((*nest)->type) : nullptr;

                if (nested_type != nullptr)
                    nested.push_back((rt_reflect_member_t *)(void *)nested_type);
            }
        }

        info->nested.count = (int64_t)nested.size();
        info->nested.items = __new_list(nested);

        std::vector<rt_reflect_member_t *> * lists[] = { &fields, &methods, &properties, &events, &gparams };
        for (int k = 0; k < 5; k++)
        {
            info->lists[k].count = (int64_t)lists[k]->size();
            info->lists[k].items = __new_list(*lists[k]);
        }
    }

    // Calls f for the types that the tables of a type hold.
    void rt_reflection_t::each_referenced_type(rt_reflect_type_t * info,
                                                std::function<void (rt_type_t *)> f)
    {
        auto call = [&](void * t) {
            if (t != nullptr)
                f((rt_type_t *)t);
        };

        call(info->base);

        for (int64_t k = 0; k < info->nested.count; k++)
            call((void *)info->nested.items[k]);

        for (auto & list : info->lists)
        {
            for (int64_t k = 0; k < list.count; k++)
            {
                rt_reflect_member_t * m = list.items[k];
                call(m->type);

                for (int64_t i = 0; i < m->param_count; i++)
                    call(m->params[i]->type);
            }
        }
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::rt
