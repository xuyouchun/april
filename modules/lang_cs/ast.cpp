
#include "ast.h"

namespace X_ROOT_NS::modules::lang_cs {

    using namespace core;
    using namespace compile;

    ////////// ////////// ////////// ////////// //////////
    // _attribute_group_assign

    // Sets name.
    void _attribute_group_assign_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__argument.name, name, el, _T("attribute param"));
    }

    // Commits this node.
    void _attribute_group_assign_ast_node_t::on_commit()
    {
        __argument.expression = __to_eobject<expression_t *>(__expression__);
        __argument.name_type  = argument_name_type_t::field;

        this->__check_name_empty(__argument.name, _T("attribute param"));
    }

    // Returns this eobject.
    argument_t * _attribute_group_assign_ast_node_t::to_eobject()
    {
        return &__argument;
    }

    // Walks this node.
    bool _attribute_group_assign_ast_node_t::on_walk(ast_walk_context_t & context,
                                                            int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // _attribute_group

    // Commits this node.
    void _attribute_group_ast_node_t::on_commit()
    {
        // Empty.
    }

    // Returns eobject count.
    size_t _attribute_group_ast_node_t::eobject_count() const
    {
        return this->child_count(__items__);
    }

    // Returns eobject at specified index.
    attribute_t * _attribute_group_ast_node_t::eobject_at(size_t index) const
    {
        return ((_attribute_group_item_ast_node_t *)this->child_at(__items__, index))
                                                        ->to_eobject();
    }

    // Walks this node.
    bool _attribute_group_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // _attribute_group_item

    // Attribute group item ast node.
    bool _attribute_group_item_ast_node_t::on_walk(ast_walk_context_t & context, int step,
                                                                                 void * tag)
    {
        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////

    // Walks this node.
    bool _attribute_type_name_ast_node_t::on_walk(ast_walk_context_t & context, int step,
                                                                                void * tag)
    {
        switch ((walk_step_t)step)
        {
            case walk_step_t::default_:
                this->__delay(context, walk_step_t::confirm);
                return __super_t::on_walk(context, step, tag);

            case walk_step_t::confirm:
                return __walk_confirm(context);

            default: return true;
        }
    }

    // Walks confirm step.
    bool _attribute_type_name_ast_node_t::__walk_confirm(ast_walk_context_t & context)
    {
        typedef ascertain_type_error_t e_t;

        general_type_name_t * type_name = as<general_type_name_t *>(to_eobject());
        if (type_name != nullptr)
        {
            e_t e;

            if (!__try_resolve_with_postfix(context, type_name) &&
                (e = __ascertain_type(context, type_name)) != e_t::__default__)
            {
                this->__log(e, type_name);
            }
        }

        return true;
    }

    // Ascertains type.
    ascertain_type_error_t _attribute_type_name_ast_node_t::__ascertain_type(
                    ast_walk_context_t & context, general_type_name_t * type_name)
    {
        try
        {
            type_t * type = ascertain_type(this->__context, context, type_name);
            type_name->type = type;

            return ascertain_type_error_t::__default__;
        }
        catch (const logic_error_t<ascertain_type_error_t> & e)
        {
            type_name->type = unknown_type_t::instance();
            return e.code;
        }
    }

    // Try resolve type with postfix 'Attribute'.
    bool _attribute_type_name_ast_node_t::__try_resolve_with_postfix(
            ast_walk_context_t & context, general_type_name_t * type_name)
    {
        name_unit_t * unit;
        if (type_name->units.size() > 0 && 
            (unit = type_name->units[type_name->units.size() - 1]) != nullptr)
        {
            name_t old_name = unit->name;
            unit->name = __to_name(_F(_T("%s%s"), _str(old_name), _T("Attribute")));

            if (__ascertain_type(context, type_name) == ascertain_type_error_t::unknown_type)
            {
                unit->name = old_name;
                return false;
            }

            return true;
        }

        return false;
    }

    ////////// ////////// ////////// ////////// //////////
    // _defination_st_item

    // Sets name.
    void _defination_st_item_ast_node_t::set_name(name_t name, __el_t * el)
    {
        this->__assign_name(__item.name, name, el, _T("variable"));
    }

    // Commits this node.
    void _defination_st_item_ast_node_t::on_commit()
    {
        __item.expression = __to_eobject<expression_t *>(__expression__);

        this->__check_name_empty(__item.name, _T("variable"));
    }

    // Returns this eobject.
    defination_statement_item_t * _defination_st_item_ast_node_t::to_eobject()
    {
        return &__item;
    }

    // Walks this node.
    bool _defination_st_item_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////

    // Commits this node.
    void _fields_ast_node_t::on_commit()
    {
        this->each_child<field_t *>(__items__, [this](field_t * field) {
            field->type_name = __to_eobject<type_name_t *>(__type_name__);
            field->decorate  = __to_eobject<decorate_t *>(__decorate__);
        });
    }

    // Returns eobject count.
    size_t _fields_ast_node_t::eobject_count() const
    {
        return this->child_count(__items__);
    }

    // Returns eobject at specified index.
    field_t * _fields_ast_node_t::eobject_at(size_t index) const
    {
        return ((_fields_item_ast_node_t *)this->child_at(__items__, index))->to_eobject();
    }

    // Walks this node.
    bool _fields_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::lang_cs

