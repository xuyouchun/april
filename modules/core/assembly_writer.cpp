
#include <core.h>

namespace X_ROOT_NS::modules::core {

    // Empty values.
    template<typename t>
    struct __empty_value_t
    {
        static bool is_empty(const t & v) { return v.empty(); }
    };

    template<typename t>
    struct __empty_value_t<t *>
    {
        static bool is_empty(const t * v) { return v == nullptr; }
    };

    template<typename t> bool __is_empty_value(const t & v)
    {
        typedef std::remove_const_t<t> t0;
        return __empty_value_t<t0>::is_empty(v);
    }

    typedef assembly_error_code_t __e_t;

    ////////// ////////// ////////// ////////// //////////

    // Checks limit.
    #define __CheckLimit(value, max_value, name)                        \
        do                                                              \
        {                                                               \
            if (value > max_value)                                      \
                throw _ECF(overflow, _T("'%1%(%2%)' overlimit: '%3%' "),\
                        name, value, max_value);                        \
        } while (false)

    ////////// ////////// ////////// ////////// //////////

    #define __S(s) this->__heap.append_string(s)

    // Searches metadata by specified entity.
    // Returns metadata if found.
    #define __SearchRet(mgr, entity)                                    \
        do                                                              \
        {                                                               \
            if (entity == nullptr)                                      \
                return mgr.current_null();                              \
                                                                        \
            ref_t ref = mgr.search_ref(entity);                         \
            if (ref != ref_t::null)                                     \
                return ref;                                             \
        } while (false)

    // Checks if it's empty.
    // Raise exception if empty.
    #define __CheckEmptyV(element, value, element_name, field_name)     \
        do                                                              \
        {                                                               \
            if (__is_empty_value(value))                                \
                throw _ED(__e_t::missing, _T("") element_name, element, \
                    _T("") field_name);                                 \
        } while (false)

    // Checks empty.
    #define __CheckEmpty(element, field, element_name, field_name)      \
            __CheckEmptyV(element, element->field, element_name, field_name)

    #define __Unexpected() X_UNEXPECTED()

    #define __W ((writer_t *)this)

    ////////// ////////// ////////// ////////// //////////

    // Base class of assembly writer.
    template<typename writer_t, __lv_t lv, __lv_t rlv = lv>
    class __assembly_writer_super_t { };

    ////////// ////////// ////////// ////////// //////////

    // Base class of assembly writer.
    template<typename writer_t, __lv_t rlv>
    class __assembly_writer_super_t<writer_t, 0, rlv>
            : public compile_assembly_layout_t<al::spool_t, rlv, default_mt_template_t>
    {
        typedef compile_assembly_layout_t<al::spool_t, rlv, default_mt_template_t> __super_t;

    public:

        // Constructors.
        __assembly_writer_super_t(xostream_t & stream, assembly_t & assembly, logger_t & logger,
                                  method_compile_controller_t * controller)
            : __stream(stream), __assembly(assembly), __logger(logger), __controller(controller)
        { }

    protected:
        xostream_t & __stream;      // Output stream.
        assembly_t & __assembly;    // Assembly.
        logger_t   & __logger;      // Logger.

        method_compile_controller_t * __controller;

        // Returns whether it is references from other assembly.
        bool __is_extern(member_t * member)
        {
            assembly_t * assembly = member->get_assembly();
            return assembly != nullptr && assembly != &__assembly;
        }

        // Assigns assembly metadata.
        void __assign_mt(mt_assembly_t * mt, assembly_t * assembly)
        {
            mt->name        = __S(assembly->name);
            mt->version     = assembly->version;
            mt->entry_point = __W->__commit_method(assembly->entry_point);
            mt->info        = __S(assembly->information);
        }

        // Assigns assembly ref metadata.
        void __assign_mt(mt_assembly_ref_t * mt, assembly_reference_t * reference)
        {
            __CheckEmpty(reference, assembly, "reference", "assembly");
            __CheckEmpty(reference->assembly, name, "reference->assembly", "name");

            mt->name    = __S(reference->assembly->name);
            mt->package = __S(reference->package_name);
        }

        // Sorts members. 
        template<typename _members_t> void __sort_members(_members_t & members)
        {
            al::sort(members, [](auto m1, auto m2) {
                return (int)m1->is_static() < (int)m2->is_static();
            });
        }

        // Assigns generic type metadata.
        void __assign_mt(mt_type_t * mt, general_type_t * type)
        {
            mt->name  = __S(type->name);

            if (type->decorate != nullptr)
                mt->decorate = *type->decorate;

            if (type->namespace_ != nullptr)
                mt->namespace_ = __S(type->namespace_->full_name->sid);

            if (type->host_type != nullptr)
                __W->__commit_type(type->host_type);

            al::sort(type->methods, [](method_t * m1, method_t * m2) {
                return (int)call_type_of_method(m1) < (int)call_type_of_method(m2);
            });

            __sort_members(type->fields);
            __sort_members(type->properties);
            __sort_members(type->events);

            mt->generic_params  = __W->__commit_generic_params(type->params);
            mt->methods         = __W->__commit_methods(type->methods);
            mt->properties      = __W->__commit_properties(type->properties);
            mt->fields          = __W->__commit_fields(type->fields);
            mt->events          = __W->__commit_events(type->events);
            mt->type_defs       = __W->__commit_type_defs(type->type_defs);
            mt->nest_types      = __W->__commit_nest_types(type->nest_types);
            mt->super_types     = __W->__commit_super_type_names(type->super_type_names);
            mt->attributes      = __W->__commit_attributes(type->attributes);

            mt->ttype           = (uint8_t)type->ttype;
            mt->vtype           = (uint8_t)type->vtype;
            mt->mtype           = (uint8_t)type->mtype;
        }

        // Assigns type_ref metadata.
        void __assign_mt(mt_type_ref_t * mt, general_type_t * type)
        {
            mt->assembly = __W->__commit_extern_assembly(type->assembly);
            mt->host     = __W->__commit_type(type->host_type);
            mt->name     = __S(type->name);
            mt->generic_param_count = type->params_count();

            const mname_t * mname;
            if (type->namespace_ && (mname = type->namespace_->full_name) != nullptr)
            {
                mt->namespace_ = __S(mname->sid);
            }
        }

        // Assigns generic type metadata.
        void __assign_mt(mt_generic_type_t * mt, generic_type_t * type)
        {
            __CheckEmpty(type, template_, "type", "template");

            mt->template_ = __W->__commit_type(type->template_);
            mt->args      = __W->__commit_generic_arguments(type->args);
            mt->host      = __W->__commit_type(type->host_type);
        }

        // Assigns array type metadata.
        void __assign_mt(mt_array_type_t * mt, array_type_t * type)
        {
            __CheckEmpty(type, element_type, "type", "element type");

            mt->element_type = __W->__commit_type(type->element_type);
            mt->dimension    = type->dimension;
        }

        // Assigns method ref metadata.
        void __assign_mt(mt_method_ref_t * mt, method_t * method)
        {
            mt->host    = __W->__commit_type(method->host_type);
            mt->name    = __S(method->name);
            mt->type    = __W->__commit_type(to_type(method->type_name));
            mt->params  = __W->__commit_method_ref_params(method);

            mt->generic_param_count = method->generic_param_count();
        }

        // Assigns method ref param metadata.
        void __assign_mt(mt_method_ref_param_t * mt, param_t * param)
        {
            mt->ptype = param->ptype;

            type_t * type = to_type(param->type_name);
            switch (type->this_gtype())
            {
                case gtype_t::generic_param: {
                    generic_param_t * p = (generic_param_t *)type;
                    mt->type = ref_t(p->index, 1, (uint32_t)mt_type_extra_t::generic_param_index);
                }   break;

                default:
                    mt->type = __W->__commit_type(type);
                    break;
            }
        }

        // Assigns attribute metadata.
        void __assign_mt(mt_attribute_t * mt, attribute_t * attr)
        {
            type_t * attr_type = to_type(attr->type_name);
            __CheckEmptyV(attr, attr_type, "attribute", "type");

            if (attr_type->this_gtype() != gtype_t::general ||
                    ((general_type_t *)attr_type)->is_generic())
            {
                throw _ED(__e_t::unexpected_attribute_type,
                        _T("generic, array type is not supported"));
            }

            __CheckEmpty(attr, constructor, "attribute", "constructor");

            mt->type        = __W->__commit_type(attr_type);
            mt->constructor = __W->__commit_method(attr->constructor);
            mt->arguments   = __W->__commit_attribute_arguments(attr->arguments,
                                                                attr->constructor);
            mt->flag.compile_time = is_compile_time_attribute(attr);
        }

        // Assigns attribute argument metadata.
        void __assign_mt(mt_attribute_argument_t * mt, argument_t * arg, type_t * param_type)
        {
            mt->name        = __S(arg->name);
            mt->name_type   = arg->name_type;
            mt->value       = __W->__commit_constant(arg->expression, param_type);
        }

        // Assigns generic param metadata.
        void __assign_mt(mt_generic_param_t * mt, generic_param_t * generic_param)
        {
            mt->name       = __S(generic_param->name);
            mt->attributes = __W->__commit_attributes(generic_param->attributes);
            mt->param_type = generic_param->param_type;
        }

        // Assigns generic argument metadata.
        void __assign_mt(mt_generic_argument_t * mt, typex_t typex)
        {
            mt->type  = __W->__commit_type((type_t *)typex);
            mt->atype = (uint8_t)(generic_arg_type_t)typex;
        }

        // Assigns type def param metadata.
        void __assign_mt(mt_type_def_param_t * mt, type_def_param_t * param)
        {
            __CheckEmpty(param, name, "type def param", "name");

            mt->name = __S(param->name);
            mt->attributes = __W->__commit_attributes(param->attributes);
        }

        // Assigns method metadata.
        void __assign_mt(mt_method_t * mt, method_t * method)
        {
            mt->name = __S(method->name);

            if (method->decorate)
                mt->decorate = *method->decorate;

            __CheckEmpty(method, name, "method", "name");
            mt->name = __S(method->name);

            mt->trait = method->trait;

            type_t * method_type = to_type(method->type_name);

            typedef method_trait_t __t;
            if (method_type == nullptr && !al::in(method->trait, __t::constructor,
                                        __t::static_constructor, __t::destructor))
            __CheckEmptyV(method, method_type, "method", "type");
            mt->generic_params = __W->__commit_generic_params(method->generic_params);
            mt->type  = __W->__commit_type(method_type);
            mt->owner = __W->__commit_type(to_type(method->owner_type_name));

            mt->attributes = __W->__commit_attributes(method->attributes);
            mt->params     = __W->__commit_params(method->params);
            mt->body       = res_t::null;
        }

        // Assigns generic method metadata.
        void __assign_mt(mt_generic_method_t * mt, generic_method_t * method)
        {
            mt->host      = __W->__commit_type(method->host_type);
            mt->template_ = __W->__commit_method(method->template_);
            mt->args      = __W->__commit_generic_arguments(method->args);
        }

        // Assigns generic method metadata.
        void __assign_mt(mt_generic_method_t * mt, method_t * template_, type_t * host)
        {
            mt->host      = __W->__commit_type(host);
            mt->template_ = __W->__commit_method(template_);
        }

        // Assigns generic field metadata.
        void __assign_mt(mt_generic_field_t * mt, field_t * template_, type_t * host)
        {
            mt->host      = __W->__commit_type(host);
            mt->template_ = __W->__commit_field(template_);
        }

        // Assigns position field metadata.
        void __assign_mt(mt_position_field_t * mt, uint16_t position, type_t * host)
        {
            mt->host      = __W->__commit_type(host);
            mt->position  = position;;
        }

        // Assigns param metadata.
        void __assign_mt(mt_param_t * mt, param_t * param)
        {
            __CheckEmpty(param, name, "param", "name");
            mt->name = __S(param->name);

            type_t * param_type = to_type(param->type_name);
            __CheckEmptyV(param, param_type, "param", "type");
            mt->type = __W->__commit_type(param_type);

            mt->param_type    = param->ptype;
            mt->default_value = __W->__commit_constant(param->default_value, param_type);
            mt->attributes    = __W->__commit_attributes(param->attributes);
        }

        // Assigns property metadata.
        void __assign_mt(mt_property_t * mt, property_t * property)
        {
            if (property->decorate)
                mt->decorate = *property->decorate;

            //__CheckEmpty(property, name, "property", "name"); property index has no name.
            mt->name = __S(property->name);

            type_t * property_type = to_type(property->type_name);
            __CheckEmptyV(property, property_type, "property", "type");
            mt->type  = __W->__commit_type(property_type);
            mt->owner = __W->__commit_type(to_type(property->owner_type_name));

            mt->attributes = __W->__commit_attributes(property->attributes);
            mt->params     = __W->__commit_params(property->params);
            mt->get_method = __W->__commit_method(property->get_method);
            mt->set_method = __W->__commit_method(property->set_method);
        }

        // Assigns field metadata.
        void __assign_mt(mt_field_t * mt, field_t * field)
        {
            if (field->decorate)
                mt->decorate = *field->decorate;

            __CheckEmpty(field, name, "field", "name");
            mt->name = __S(field->name);

            type_t * field_type = to_type(field->type_name);
            __CheckEmptyV(field, field_type, "field", "type");

            mt->type       = __W->__commit_type(field_type);
            mt->attributes = __W->__commit_attributes(field->attributes);
            mt->init_value = __W->__commit_constant(field->init_value, field_type);
        }

        // Assigns event metadata.
        void __assign_mt(mt_event_t * mt, event_t * event)
        {
            if (event->decorate)
                mt->decorate = *event->decorate;

            __CheckEmpty(event, name, "event", "name");
            mt->name = __S(event->name);

            type_t * event_type = to_type(event->type_name);
            __CheckEmptyV(event, event_type, "event", "type");
            mt->type  = __W->__commit_type(event_type);
            mt->owner = __W->__commit_type(to_type(event->owner_type_name));

            mt->attributes    = __W->__commit_attributes(event->attributes);
            mt->add_method    = __W->__commit_method(event->add_method);
            mt->remove_method = __W->__commit_method(event->remove_method);
        }

        // Assigns type def metadata.
        void __assign_mt(mt_type_def_t * mt, type_def_t * type_def)
        {
            type_t * type = to_type(type_def->type_name);
            __CheckEmptyV(type_def, type, "type_def", "type");

            if (type_def->decorate != nullptr)
                mt->decorate = *type_def->decorate;

            mt->params = __W->__commit_type_def_params(type_def->params);
            mt->type   = __W->__commit_type(type);
            mt->name   = __S(type_def->name);

            if (type_def->namespace_ != nullptr)
                mt->namespace_ = __S(type_def->namespace_->full_name->sid);
        }

        // Assigns nest type.
        void __assign_mt(mt_nest_type_t * mt, type_t * type)
        {
            mt->type = __W->__commit_type(type);
        }

        // Assigns super type.
        void __assign_mt(mt_super_type_t * mt, type_t * type)
        {
            mt->type = __W->__commit_type(type);
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // Base class of assembly writer.
    template<typename writer_t, __lv_t rlv>
    class __assembly_writer_super_t<writer_t, 1, rlv>
        : public __assembly_writer_super_t<writer_t, 0, rlv>
    {
        typedef __assembly_writer_super_t<writer_t, 0, rlv> __super_t;

    public:
        using __super_t::__super_t;
    };

    ////////// ////////// ////////// ////////// //////////

    // Assembly writer.
    template<__lv_t lv>
    class __assembly_writer_t : public __assembly_writer_super_t<__assembly_writer_t<lv>, lv>
    {
        typedef __assembly_writer_t<lv> __self_t;
        typedef __assembly_writer_super_t<__assembly_writer_t<lv>, lv> __super_t;

        using typename __super_t::mt_template_t;

        template<__tidx_t tidx>
        using __mt_t = typename mt_template_t::template mt_t<tidx>;

        friend class __assembly_writer_super_t<__self_t, 0, lv>;
        friend class __assembly_writer_super_t<__self_t, 1, lv>;

    public:
        using __super_t::__super_t;

        // Build and write to a stream.
        void write()
        {
            __commit_eobjects();
            __compile();
            __fill_metadatas();

            __write();
        }

    private:
        using __super_t::__mt_manager;
        using __super_t::__assign_mt;

        struct __assign_item_t { __tidx_t tidx; void * mt, * entity, * extern_; };
        std::queue<__assign_item_t> __assign_queue;

        // Enque a assign item.
        void __enque_assign(__tidx_t tidx, void * mt, void * entity, void * extern_ = nullptr)
        {
            __assign_queue.push(__assign_item_t { tidx, mt, entity, extern_ });
        }

        // Returns metadata manager.
        template<__tidx_t tidx> mt_manager_t<tidx, __mt_t<tidx>> & __mt_manager()
        {
            return __super_t::template __mt_manager<tidx>();
        }

        // Returns current null.
        template<__tidx_t tidx> ref_t __current_null()
        {
            return __mt_manager<tidx>().current_null();
        }

        // Commits eobjects.
        void __commit_eobjects()
        {
            this->__assembly.types.each_general_types([this](general_type_t * general_type) {
                __commit_internal_general_type(general_type);
            });

            this->__assembly.types.each_type_defs([this](type_def_t * type_def) {
                __commit_type_def(type_def);
            });

            __commit_assembly();
            __assign_mts();
        }

        // Assigns metadata.
        void __assign_mts()
        {
            // assign
            while (__assign_queue.size() > 0)
            {
                __assign_item_t & it = __assign_queue.front();

                switch (it.tidx)
                {
                    #define __AssignMtCase(tidx)                                        \
                        case __tidx_t::tidx:                                            \
                            __assign_mt(                                                \
                                (mt_t<__tidx_t::tidx> *)it.mt,                          \
                                (typename mt_t<__tidx_t::tidx>::entity_t)it.entity      \
                            );                                                          \
                            break;

                    #define __AssignMtCaseEx(tidx, extern_t)                            \
                        case __tidx_t::tidx:                                            \
                            __assign_mt(                                                \
                                (mt_t<__tidx_t::tidx> *)it.mt,                          \
                                (typename mt_t<__tidx_t::tidx>::entity_t)it.entity,     \
                                (extern_t)it.extern_                                    \
                            );                                                          \
                            break;

                    __AssignMtCase(assembly)
                    __AssignMtCase(assembly_ref)
                    __AssignMtCase(type)
                    __AssignMtCase(type_ref)
                    __AssignMtCase(generic_type)
                    __AssignMtCase(array_type)
                    __AssignMtCase(attribute)
                    __AssignMtCaseEx(attribute_argument, type_t *)
                    __AssignMtCase(generic_param)
                    __AssignMtCase(generic_argument)
                    __AssignMtCase(type_def_param)
                    __AssignMtCase(method)
                    __AssignMtCase(param)
                    __AssignMtCase(property)
                    __AssignMtCase(field)
                    __AssignMtCase(event)
                    __AssignMtCase(type_def)
                    __AssignMtCase(method_ref)
                    __AssignMtCase(method_ref_param)
                    __AssignMtCase(nest_type)
                    __AssignMtCase(super_type)

                    default: ;
                        _PP(it.tidx);
                        __Unexpected();

                    #undef __AssignMtCaseEx
                    #undef __AssignMtCase
                }

                __assign_queue.pop();
            }
        }

        // Commits assembly.
        ref_t __commit_assembly()
        {
            auto & mgr = __mt_manager<__tidx_t::assembly>();
            mt_assembly_t * mt_assembly;
            ref_t ref = mgr.append(&this->__assembly, &mt_assembly);

            __enque_assign(__tidx_t::assembly, mt_assembly, &this->__assembly);

            return ref;
        }

        // Commits extern assembly.
        ref_t __commit_extern_assembly(assembly_t * assembly)
        {
            auto & mgr = __mt_manager<__tidx_t::assembly_ref>();
            assembly_reference_t * reference = this->__assembly.references.get_reference(assembly);

            if (reference == nullptr)
                throw _ED(__e_t::assembly_reference_not_found, assembly->name);

            __SearchRet(mgr, reference);

            mt_assembly_ref_t * mt_assembly_ref;
            ref_t ref = mgr.append(reference, &mt_assembly_ref);

            __enque_assign(__tidx_t::assembly_ref, mt_assembly_ref, reference);

            return ref;
        }

        // Commits general types.
        ref_t __commit_general_type(general_type_t * type)
        {
            if (type->assembly == &this->__assembly)
                return __commit_internal_general_type(type);

            return __commit_extern_general_type(type);
        }

        // Commits internal general type.
        ref_t __commit_internal_general_type(general_type_t * type)
        {
            auto & mgr = __mt_manager<__tidx_t::type>();
            __SearchRet(mgr, type);

            mt_type_t * mt_type;
            ref_t ref = mgr.append(type, &mt_type);

            __enque_assign(__tidx_t::type, mt_type, type);

            return ref;
        }

        // Commits extern general type.
        ref_t __commit_extern_general_type(general_type_t * type)
        {
            auto & mgr = __mt_manager<__tidx_t::type_ref>();

            __SearchRet(mgr, type);

            mt_type_ref_t * mt_type_ref;

            ref_t ref = mgr.append(type, &mt_type_ref);
            __enque_assign(__tidx_t::type_ref, mt_type_ref, type);

            return ref;
        }

        // Commits generic type.
        ref_t __commit_generic_type(generic_type_t * type)
        {
            auto & mgr = __mt_manager<__tidx_t::generic_type>();
            __SearchRet(mgr, type);

            mt_generic_type_t * mt_generic_type;
            ref_t ref = mgr.append(type, &mt_generic_type);
            __enque_assign(__tidx_t::generic_type, mt_generic_type, type);

            return ref;
        }

        // Commits array type.
        ref_t __commit_array_type(array_type_t * type)
        {
            auto & mgr = __mt_manager<__tidx_t::array_type>();
            __SearchRet(mgr, type);

            mt_array_type_t * mt_array_type;
            ref_t ref = mgr.append(type, &mt_array_type);
            __enque_assign(__tidx_t::array_type, mt_array_type, type);

            return ref;
        }

        // Commits attributes.
        ref_t __commit_attributes(attributes_t * attributes)
        {
            auto & mgr = __mt_manager<__tidx_t::attribute>();

            if (__size(attributes) == 0)
                return mgr.current_null();

            return mgr.acquire(*attributes,
                [this](ref_t ref0, attribute_t * attr, mt_attribute_t * mt_attr) {
                __enque_assign(__tidx_t::attribute, mt_attr, attr);
            });
        }

        // Commits attribute arguments.
        ref_t __commit_attribute_arguments(arguments_t * arguments, method_t * method)
        {
            if (method->param_count() != __size(arguments))
                throw _ED(__e_t::mismatched_argument_count);

            auto & mgr = __mt_manager<__tidx_t::attribute_argument>();

            if (arguments == nullptr || arguments->empty())
                return mgr.current_null();

            size_t param_index = 0;

            return mgr.acquire(*arguments, [this, method, &param_index]
                (ref_t ref0, argument_t * arg, mt_attribute_argument_t * mt_arg) {

                param_t * param = method->param_at(param_index++);
                __CheckEmptyV(method, param, "method", "param");

                type_t * param_type = param->get_type();
                __CheckEmptyV(method, param_type, "method", "param_type");

                __enque_assign(__tidx_t::attribute_argument, mt_arg, arg, param_type);
            });
        }

        // Commits generic param.
        ref_t __commit_generic_param(generic_param_t * param)
        {
            auto & mgr = __mt_manager<__tidx_t::generic_param>();

            ref_t ref = mgr.search_ref(param);
            __SearchRet(mgr, param);

            return ref_t(param->index, 1, (uint32_t)mt_type_extra_t::generic_param_index);
        }

        // Commits generic params.
        ref_t __commit_generic_params(generic_params_t * params)
        {
            auto & mgr = __mt_manager<__tidx_t::generic_param>();

            if (__size(params) == 0)
                return mgr.current_null();

            return mgr.acquire(*params,
                [this](ref_t ref, generic_param_t * generic_param, mt_generic_param_t * mt) {
                __enque_assign(__tidx_t::generic_param, mt, generic_param);
            });
        }

        // Commits generic arguments.
        ref_t __commit_generic_arguments(type_collection_t & types)
        {
            auto & mgr = __mt_manager<__tidx_t::generic_argument>();

            return mgr.acquire(types, [this](ref_t ref, typex_t typex, mt_generic_argument_t * mt) {
                __enque_assign(__tidx_t::generic_argument, mt, typex);
            });
        }

        // Commits type def param.
        ref_t __commit_type_def_param(type_def_param_t * param)
        {
            auto & mgr = __mt_manager<__tidx_t::type_def_param>();
            __SearchRet(mgr, param);

            __Unexpected();
        }

        // Commits type def params.
        ref_t __commit_type_def_params(type_def_params_t * params)
        {
            auto & mgr = __mt_manager<__tidx_t::type_def_param>();

            if (__size(params) == 0)
                return mgr.current_null();

            return mgr.acquire(*params,
                [this](ref_t ref, type_def_param_t * param, mt_type_def_param_t * mt) {
                __enque_assign(__tidx_t::type_def_param, mt, param);
            });
        }

        // Commits null type.
        ref_t __commit_null_type(null_type_t * type)
        {
            return ref_t::null;
        }

        // Commits type.
        ref_t __commit_type(type_t * type)
        {
            if (type == nullptr)
                return ref_t::null;

            switch (type->this_gtype())
            {
                case gtype_t::general:
                    return __commit_general_type((general_type_t *)type);

                case gtype_t::generic:
                    return __commit_generic_type((generic_type_t *)type);

                case gtype_t::array:
                    return __commit_array_type((array_type_t *)type);

                case gtype_t::generic_param:
                    return __commit_generic_param((generic_param_t *)type);

                case gtype_t::type_def_param:
                    return __commit_type_def_param((type_def_param_t *)type);

                case gtype_t::null:
                    return __commit_null_type((null_type_t *)type);

                default:
                    throw _EC(unexpected);
            }

            return ref_t::null;
        }

        // Returns whether it's a internal type.
        bool __is_internal_type(type_t * type)
        {
            return is_general(type) && ((general_type_t *)type)->assembly == &this->__assembly;
        }

        // Commits nest types.
        template<typename _types_t>
        ref_t __commit_nest_types(_types_t & types)
        {
            auto & mgr = __mt_manager<__tidx_t::nest_type>();

            if (types.empty())
                return mgr.current_null();

            return mgr.acquire(types, [this](ref_t ref0, type_t * type, mt_nest_type_t * mt) {
                if (!__is_internal_type(type))
                    throw _ED(__e_t::write_unexpected_nest_type, _str(type));
                __enque_assign(__tidx_t::nest_type, mt, type);
            });
        }

        // Commits super types.
        template<typename _type_names_t>
        ref_t __commit_super_type_names(_type_names_t & type_names)
        {
            auto & mgr = __mt_manager<__tidx_t::super_type>();

            if (type_names.empty())
                return mgr.current_null();

            al::svector_t<type_t *> types;
            al::transform(type_names, std::back_inserter(types),
                [](type_name_t * type_name) { return type_name->type; }
            );

            return mgr.acquire(types, [this](ref_t ref0, type_t * type, mt_super_type_t * mt) {
                __enque_assign(__tidx_t::super_type, mt, type);
            });
        }

        // Commits methods.
        template<typename _methods_t>
        ref_t __commit_methods(_methods_t & methods)
        {
            auto & mgr = __mt_manager<__tidx_t::method>();

            return mgr.acquire(methods, [this](ref_t ref0, method_t * method,
                                                    mt_method_t * mt_method) {
                __enque_assign(__tidx_t::method, mt_method, method);

                if (method->body != nullptr)
                    __compile_elements.push_back(__compile_element_t { method, mt_method, ref0 });
            });
        }

        // Commit method.
        ref_t __commit_method(method_t * method)
        {
            if (method == nullptr)
                return ref_t::null;

            if (!this->__is_extern(method))
            {
                auto & mgr = __mt_manager<__tidx_t::method>();
                __SearchRet(mgr, method);

                type_t * host_type = method->host_type;
                if (method->this_family() == member_family_t::impl)
                    return __commit_impl_method((impl_method_t *)method);

                if (host_type != nullptr)
                {
                    switch (host_type->this_gtype())
                    {
                        case gtype_t::general:
                            __commit_type(method->host_type);
                            __SearchRet(mgr, method);
                            __Unexpected();

                        case gtype_t::generic:
                            __Unexpected();

                        default:
                            __Unexpected();
                    }
                }

                __Unexpected();
            }
            else
            {
                return __commit_method_ref(method);
            }
        }

        // Returns Impl method.
        static method_base_t * __raw_method_of(impl_method_t * impl_method)
        {
            method_base_t * raw = impl_method->raw;
            _A(raw != nullptr);

            if (raw->this_family() == member_family_t::impl)
                return __raw_method_of((impl_method_t *)raw);

            return raw;
        }

        // Commits impl method.
        ref_t __commit_impl_method(impl_method_t * impl_method)
        {
            if (impl_method == nullptr)
                return ref_t::null;

            method_base_t * raw = __raw_method_of(impl_method);
            type_t * host = impl_method->host_type;

            switch (raw->this_family())
            {
                case member_family_t::generic:
                    return __commit_generic_method((generic_method_t *)raw);

                case member_family_t::general: {
                    auto & mgr = __mt_manager<__tidx_t::generic_method>();
                    __SearchRet(mgr, (generic_method_t *)impl_method);

                    mt_generic_method_t * mt_generic_method;
                    ref_t ref = mgr.append((generic_method_t *)impl_method, &mt_generic_method);
                    __assign_mt(mt_generic_method, (method_t *)raw, host);

                    return ref;
                }

                default:
                    __Unexpected();
            }
        }

        // Commits generic method.
        ref_t __commit_generic_method(generic_method_t * method)
        {
            if (method == nullptr)
                return __current_null<__tidx_t::generic_method>();

            auto & mgr = __mt_manager<__tidx_t::generic_method>();
            __SearchRet(mgr, method);

            mt_generic_method_t * mt_generic_method;
            ref_t ref = mgr.append(method, &mt_generic_method);

            __assign_mt(mt_generic_method, method);

            return ref;
        }

        // Commits generic field.
        ref_t __commit_generic_field(impl_field_t * field)
        {
            if (field == nullptr)
                return __current_null<__tidx_t::generic_field>();

            auto & mgr = __mt_manager<__tidx_t::generic_field>();
            __SearchRet(mgr, field);

            mt_generic_field_t * mt_generic_field;
            ref_t ref = mgr.append(field, &mt_generic_field);

            __assign_mt(mt_generic_field, field->raw, field->host_type);

            return ref;
        }

        // Commits position field.
        ref_t __commit_position_field(position_field_t * field)
        {
            if (field == nullptr)
                return __current_null<__tidx_t::position_field>();

            auto & mgr = __mt_manager<__tidx_t::position_field>();
            __SearchRet(mgr, field);

            mt_position_field_t * mt_position_field;
            ref_t ref = mgr.append(field, &mt_position_field);

            __assign_mt(mt_position_field, field->position, field->host_type);

            return ref;
        }

        // Commits params
        template<typename _params_t>
        ref_t __commit_params(_params_t & params)
        {
            auto & mgr = __mt_manager<__tidx_t::param>();

            return mgr.acquire(params, [this](ref_t ref0, param_t * param, mt_param_t * mt_param) {
                __enque_assign(__tidx_t::param, mt_param, param);
            });
        }

        // Commits params.
        template<typename _params_t>
        ref_t __commit_params(_params_t * params)
        {
            if (params == nullptr)
                return __current_null<__tidx_t::param>();

            return __commit_params(*params);
        }

        // Commits param.
        ref_t __commit_param(param_t * param)
        {
            auto & mgr = __mt_manager<__tidx_t::param>();
            __SearchRet(mgr, param);

            __Unexpected();
        }

        // Commits properties.
        template<typename _properties_t>
        ref_t __commit_properties(_properties_t & properties)
        {
            auto & mgr = __mt_manager<__tidx_t::property>();

            return mgr.acquire(properties, [this](ref_t ref0, property_t * property,
                                                            mt_property_t * mt_property) {
                __enque_assign(__tidx_t::property, mt_property, property);
            });
        }

        // Commits property.
        ref_t __commit_property(property_t * property)
        {
            auto & mgr = __mt_manager<__tidx_t::property>();
            __SearchRet(mgr, property);

            __commit_type(property->host_type);
            __SearchRet(mgr, property);

            __Unexpected();
        }

        // Commits fields.
        template<typename _fields_t>
        ref_t __commit_fields(_fields_t & fields)
        {
            auto & mgr = __mt_manager<__tidx_t::field>();

            return mgr.acquire(fields, [this](ref_t ref0, field_t * field, mt_field_t * mt_field) {
                __enque_assign(__tidx_t::field, mt_field, field);
            });
        }

        // Commit field.
        ref_t __commit_field(field_t * field)
        {
            if (field == nullptr)
                return ref_t::null;

            if (field->this_family() == member_family_t::impl)
                return __commit_generic_field((impl_field_t *)field);

            if (field->this_family() == member_family_t::position)
                return __commit_position_field((position_field_t *)field);

            auto & mgr = __mt_manager<__tidx_t::field>();
            __SearchRet(mgr, field);

            __commit_type(field->host_type);
            __SearchRet(mgr, field);

            __Unexpected();
        }

        // Commit events.
        template<typename _events_t>
        ref_t __commit_events(_events_t & events)
        {
            auto & mgr = __mt_manager<__tidx_t::event>();
            return mgr.acquire(events, [this](ref_t ref0, event_t * event, mt_event_t * mt_event) {
                __enque_assign(__tidx_t::event, mt_event, event);
            });
        }

        // Commits event.
        ref_t __commit_event(event_t * event)
        {
            if (event == nullptr)
                return ref_t::null;

            auto & mgr = __mt_manager<__tidx_t::event>();
            __SearchRet(mgr, event);

            __commit_type(event->host_type);
            __SearchRet(mgr, event);

            __Unexpected();
        }

        // Commits type defs.
        template<typename _type_defs_t>
        ref_t __commit_type_defs(_type_defs_t & type_defs)
        {
            auto & mgr = __mt_manager<__tidx_t::type_def>();
            return mgr.acquire(type_defs,
                [this](ref_t ref, type_def_t * type_def, mt_type_def_t * mt_type_def) {
                __enque_assign(__tidx_t::type_def, mt_type_def, type_def);
            });
        }

        // Commits type def.
        ref_t __commit_type_def(type_def_t * type_def)
        {
            if (type_def == nullptr)
                return ref_t::null;

            auto & mgr = __mt_manager<__tidx_t::type_def>();
            __SearchRet(mgr, type_def);

            __commit_type(type_def->host_type);
            __SearchRet(mgr, type_def);

            mt_type_def_t * mt_type_def;
            ref_t ref = mgr.append(type_def, &mt_type_def);
            __enque_assign(__tidx_t::type_def, mt_type_def, type_def);

            return ref;
        }

        // Commits constants.
        ref_t __commit_constant(expression_t * exp, type_t ** out_type)
        {
            if (exp == nullptr)
            {
                al::assign_value(out_type, (type_t *)nullptr);
                return __current_null<__tidx_t::constant>();
            }

            cvalue_t value = exp->execute();

            if (is_nan(value))
                throw _ED(__e_t::unexpected_const_value, exp);

            return __commit_constant(value, out_type);
        }

        // Commits constant.
        ref_t __commit_constant(const cvalue_t & value, type_t ** out_type)
        {
            switch (value.value_type)
            {
                case cvalue_type_t::number:
                    al::assign_value(out_type, this->__get_internal_type(value.number.type));
                    break;

                case cvalue_type_t::string:
                    al::assign_value(out_type, this->__get_string_type());
                    break;

                case cvalue_type_t::null:
                    al::assign_value(out_type, (type_t *)nullptr);
                    break;

                case cvalue_type_t::type:
                    al::assign_value(out_type, this->__get_type_type());
                    break;

                case cvalue_type_t::nan:
                default:
                    throw _ED(__e_t::unexpected_const_value, value);
            }


            auto & mgr = __mt_manager<__tidx_t::constant>();
            __SearchRet(mgr, value);

            switch (value.value_type)
            {
                case cvalue_type_t::number:
                    return __commit_constant_number(mgr, value.number);

                case cvalue_type_t::string:
                    return __commit_constant_string(mgr, value.string);

                case cvalue_type_t::null:
                    return __commit_constant_null(mgr);

                case cvalue_type_t::type:
                    return __commit_constant_type(mgr, (type_t *)value.mobject);

                case cvalue_type_t::nan:
                default:
                    throw _ED(__e_t::unexpected_const_value, value);
            }
        }

        // Checks whether two types are compatible.
        void __check_type_compatible(type_t * from, type_t * to)
        {
            if (to == nullptr)
                return;

            if (!is_type_compatible(from, to))
                throw _ED(__e_t::unexpected_const_type, from, to);
        }

        // Commits constant value with expected tpye.
        template<typename value_t>
        ref_t __commit_constant_with_expect(value_t && value, type_t * expect_type)
        {
            type_t * type;
            ref_t ref = __commit_constant(std::forward<value_t>(value), &type);

            if (ref != ref_t::null)
                __check_type_compatible(type, expect_type);

            return ref;
        }

        // Commits constant value.
        ref_t __commit_constant(expression_t * exp, type_t * expect_type)
        {
            return __commit_constant_with_expect(exp, expect_type);
        }

        // Commits constant value.
        ref_t __commit_constant(const cvalue_t & value, type_t * expect_type)
        {
            return __commit_constant_with_expect(value, expect_type);
        }

        // Assigns uint64 constant value.
        void __assign_uint64(mt_manager_t<__tidx_t::constant> & mgr,
                                                    mt_constant_t & c, uint64_t v)
        {
            *(uint32_t *)c.data2 = (uint32_t)v;
            uint32_t v1 = (uint32_t)(v >> 32);

            if (v1 <= max_value<uint16_t>())
            {
                *(uint16_t *)c.data1 = v1;
            }
            else if ((v1 = ~v1) <= max_value<uint16_t>())
            {
                c.constant_flag.cpl = true;
                *(uint16_t *)c.data1 = v1;
            }
            else
            {
                c.constant_flag.extern_ = true;
                *(res_t *)c.data2 = this->__heap.append(v);
            }
        }

        // Assigns constant number.
        ref_t __commit_constant_number(__constant_mgr_t & mgr, const tvalue_t & value)
        {
            mt_constant_t * c;
            ref_t ref = mgr.append(value, &c);
            c->constant_type = (constant_type_t)(((int)cvalue_type_t::number << 5)
                                                    | (int)value.type);
            switch (value.type)
            {
                case value_type_t::int8_:
                    *(int8_t *)c->data2 = (int8_t)value;
                    break;

                case value_type_t::uint8_:
                    *(uint8_t *)c->data2 = (uint8_t)value;
                    break;

                case value_type_t::int16_:
                    *(int16_t *)c->data2 = (int16_t)value;
                    break;

                case value_type_t::uint16_:
                    *(uint16_t *)c->data2 = (uint16_t)value;
                    break;

                case value_type_t::int32_:
                    *(int32_t *)c->data2 = (int32_t)value;
                    break;

                case value_type_t::uint32_:
                    *(uint32_t *)c->data2 = (uint32_t)value;
                    break;

                case value_type_t::int64_: {
                    int64_t v = (int64_t)value;
                    __assign_uint64(mgr, *c, *(uint64_t *)&v);
                }   break;

                case value_type_t::uint64_:
                    __assign_uint64(mgr, *c, (uint64_t)value);
                    break;

                case value_type_t::float_:
                    *(float *)c->data2 = (float)value;
                    break;

                case value_type_t::double_: {
                    double v = (double)value;
                    __assign_uint64(mgr, *c, *(uint64_t *)&v);
                }   break;

                case value_type_t::bool_:
                    *(bool *)c->data2 = (bool)value;
                    break;

                case value_type_t::char_:
                    *(char_t *)c->data2 = (char_t)value;
                    break;

                default:
                    throw _ED(__e_t::unexpected_const_value, value);
            }

            return ref;
        }

        // Commits constant string.
        ref_t __commit_constant_string(__constant_mgr_t & mgr, const char_t * string)
        {
            mt_constant_t * c;
            ref_t ref = mgr.append(cvalue_t(string), &c);
            c->constant_type = constant_type_t::string_;
            c->constant_flag.extern_ = true;
            *(res_t *)c->data2 = __S(__SPool.to_sid(string));

            return ref;
        }

        // Commits constant type.
        ref_t __commit_constant_type(__constant_mgr_t & mgr, type_t * type)
        {
            mt_constant_t * c;
            ref_t ref = mgr.append(cvalue_t(type), &c);
            c->constant_type = constant_type_t::type_;
            c->constant_flag.extern_ = true;
            *(ref_t *)c->data2 = __commit_type(type);

            return ref;
        }

        // Commits null.
        ref_t __commit_constant_null(__constant_mgr_t & mgr)
        {
            mt_constant_t * c;
            ref_t ref = mgr.append(cvalue_t::null, &c);
            c->constant_type = constant_type_t::null_;
            *(int32_t *)c->data2 = 0;

            return ref;
        }

        // On load ref for specified entity.
        virtual ref_t on_load_ref(__tidx_t tidx, void * entity) override
        {
            if (entity == nullptr)
                return ref_t::null;

            switch (tidx)
            {
                case __tidx_t::method_ref:
                    return __commit_method_ref((method_t *)entity);

                case __tidx_t::method:
                    return __commit_method((method_t *)entity);

                case __tidx_t::field_ref:
                    return __commit_field_ref((field_t *)entity);

                case __tidx_t::field:
                    return __commit_field((field_t *)entity);

                case __tidx_t::generic_method:
                    return __commit_generic_method((generic_method_t *)entity);

                case __tidx_t::generic_param:
                    return __commit_type((generic_param_t *)entity);

                case __tidx_t::generic_field:
                    return __commit_generic_field((impl_field_t *)entity);

                case __tidx_t::position_field:
                    return __commit_position_field((position_field_t *)entity);

                default:
                    X_UNEXPECTED();
            }
        }

        // On load ref for specified type.
        virtual ref_t on_load_type_ref(type_t * type) override
        {
            return __commit_type(type);
        }

        // Commits method ref.
        ref_t __commit_method_ref(method_t * method)
        {
            auto & mgr = __mt_manager<__tidx_t::method_ref>();
            __SearchRet(mgr, method);

            mt_method_ref_t * method_ref;
            ref_t ref = mgr.append(method, &method_ref);
            __enque_assign(__tidx_t::method_ref, method_ref, method);

            return ref;
        }

        // Commits method ref params.
        ref_t __commit_method_ref_params(method_t * method)
        {
            auto & mgr = __mt_manager<__tidx_t::method_ref_param>();

            params_t * params = method->params;
            if (params == nullptr)
                return mgr.current_null();

            return mgr.acquire(*params, [this](ref_t ref0, param_t * param,
                                mt_method_ref_param_t * mt_method_ref_param) {
                __enque_assign(__tidx_t::method_ref_param, mt_method_ref_param, param);
            });
        }

        // Commits field ref.
        ref_t __commit_field_ref(field_t * field)
        {
            member_family_t family = field->this_family();

            if (family == member_family_t::position)
                throw _EC(unexpected);

            auto & mgr = __mt_manager<__tidx_t::field_ref>();
            __SearchRet(mgr, field);

            mt_field_ref_t * field_ref;
            ref_t ref = mgr.append(field, &field_ref);
            __enque_assign(__tidx_t::field_ref, field_ref, field);

            return ref;
        }

        // Compiles methods.
        void __compile()
        {
            for (__compile_element_t & e : __compile_elements)
            {
                e.mt_method->body = __compile_method(e.method);
            }

            __assign_mts();
        }

        // Compiles method.
        res_t __compile_method(method_t * method)
        {
            xil_buffer_t buffer;
            xil_pool_t   pool;

            method_compile_context_t ctx(*this, buffer, pool, this->__logger, this->__controller);
            method->compile(ctx);

            xil_buffer_writer_t writer(buffer, method);
            writer.write(pool);

            if (writer.empty())
                return res_t::null;

            const byte_t * bytes = buffer.bytes();
            size_t size = buffer.size();

            return this->__heap.append_block(bytes, size);
        }

        // Returns table size of specified table index.
        size_t __table_size(__tidx_t tidx)
        {
            mt_manager_object_t * mt_obj = __mt_manager((int)tidx);
            return mt_obj->row_size(lv) * mt_obj->row_count();
        }

        // Fills metadatas.
        void __fill_metadatas()
        {
            // guide
            auto & guide_mgr = __mt_manager<__tidx_t::guide>();
            mt_guide_t * mt_guide;
            guide_mgr.append(nullptr, &mt_guide);

            mt_guide->flag   = guid_t::parse(JC_ASSEMBLY_FLAG);
            mt_guide->layout = lv;

            // header
            auto & header_mgr = __mt_manager<__tidx_t::header>();

            mt_header_t * mt_header;
            header_mgr.append(nullptr, &mt_header);

            // tables
            auto & table_mgr = __mt_manager<__tidx_t::table>();
            size_t offset = __table_size(__tidx_t::header) + __table_size(__tidx_t::table);
            for (int index = __mttbl_count; index < (int)__tidx_t::__end__; index++)
            {
                mt_manager_object_t * mgr = __mt_manager(index);
                size_t row_size = mgr->row_size(lv);
                size_t row_count = mgr->row_count();

                mt_table_t * table;
                table_mgr.append(nullptr, &table);
                table->offset = offset;
                table->rows   = row_count;

                offset += row_size * row_count;
            }

            mt_header->heap_offset = offset;
            mt_header->heap_size   = this->__heap.size() - 1;

            __CheckLimit(mt_header->heap_size, JC_MAX_HEAP_SIZE, _T("heap size"));
        }

        // Writes to stream.
        void __write()
        {
            int table_count = this->value_of(__ln_t::table_count);
            for (int index = 0; index < table_count; index++)
            {
                mt_manager_object_t * mgr = __mt_manager(index);
                mgr->write_to(this->__stream, lv);
            }

            this->__heap.write_to(this->__stream);
        }

    private:
        struct __compile_element_t { method_t * method; mt_method_t * mt_method; ref_t ref; };
        std::vector<__compile_element_t> __compile_elements;
    };

    ////////// ////////// ////////// ////////// //////////

    // Writes assembly to a stream.
    void assembly_write(xostream_t & stream, assembly_t & assembly, __lv_t lv, logger_t & logger,
                        method_compile_controller_t * controller)
    {
        switch (lv)
        {
            case 1:
                __assembly_writer_t<1>(stream, assembly, logger, controller).write();
                break;

            default:
                throw _ECF(unexpected, _T("assembly layout '%1%' not supported"), lv);
        }
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::core
