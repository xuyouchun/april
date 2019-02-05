
#include "ast.h"

namespace X_ROOT_NS { namespace modules { namespace lang_cs {

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
        __argument.expression = __to_eobject<expression_t *>(expression);
        __argument.name_type  = argument_name_type_t::field;

        this->__check_name_empty(__argument.name, _T("attribute param"));
    }

    // Returns this eobject.
    argument_t * _attribute_group_assign_ast_node_t::to_eobject()
    {
        return &__argument;
    }

    // Walks this node.
    void _attribute_group_assign_ast_node_t::on_walk(ast_walk_context_t & context,
                                                            int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // _attribute_group

    // Commits this node.
    void _attribute_group_ast_node_t::on_commit()
    {

    }

    // Returns eobject count.
    size_t _attribute_group_ast_node_t::eobject_count() const
    {
        return this->child_count(items);
    }

    // Returns eobject at specified index.
    attribute_t * _attribute_group_ast_node_t::eobject_at(size_t index) const
    {
        return ((_attribute_group_item_ast_node_t *)this->child_at(items, index))->to_eobject();
    }

    // Walks this node.
    void _attribute_group_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////
    // _attribute_group_item

    // Attribute group item ast node.
    void _attribute_group_item_ast_node_t::on_walk(ast_walk_context_t & context, int step,
                                                                                 void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////

    // Walks this node.
    void _attribute_type_name_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        switch((walk_step_t)step)
        {
            case walk_step_t::default_:
                this->__delay(context, walk_step_t::confirm);
                __super_t::on_walk(context, step, tag);
                break;

            case walk_step_t::confirm:
                __walk_confirm(context);
                break;

            default: break;
        }
    }

    // Walks confirm step.
    void _attribute_type_name_ast_node_t::__walk_confirm(ast_walk_context_t & context)
    {
        typedef ascertain_type_error_t e_t;

        general_type_name_t * type_name = as<general_type_name_t *>(to_eobject());
        if(type_name != nullptr)
        {
            e_t error_code = __ascertain_type(context, type_name);
            if(error_code == e_t::unknown_type)
            {
                type_name_unit_t * unit;
                if(type_name->units.size() > 0 && 
                    (unit = type_name->units[type_name->units.size() - 1]) != nullptr)
                {
                    name_t old_name = unit->name;
                    unit->name = __to_name(_F(_T("%s%s"), _str(old_name), _T("Attribute")));

                    if(__ascertain_type(context, type_name) == e_t::unknown_type)
                    {
                        unit->name = old_name;
                    }
                }
            }
        }
    }

    // Ascertains type.
    ascertain_type_error_t _attribute_type_name_ast_node_t::__ascertain_type(
                    ast_walk_context_t & context, general_type_name_t * type_name)
    {
        try
        {
            type_t * type = ascertain_type(this->__context, context, type_name);
            return ascertain_type_error_t::__default__;
        }
        catch(const logic_error_t<ascertain_type_error_t> & e)
        {
            return e.code;
        }
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
        __item.expression = __to_eobject<expression_t *>(expression);

        this->__check_name_empty(__item.name, _T("variable"));
    }

    // Returns this eobject.
    defination_statement_item_t * _defination_st_item_ast_node_t::to_eobject()
    {
        return &__item;
    }

    // Walks this node.
    void _defination_st_item_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////

    // Commits this node.
    void _fields_ast_node_t::on_commit()
    {
        this->each_child<field_t *>(items, [this](field_t * field) {
            field->type_name = __to_eobject<type_name_t *>(type_name);
            field->decorate  = __to_eobject<decorate_t *>(decorate);
        });
    }

    // Returns eobject count.
    size_t _fields_ast_node_t::eobject_count() const
    {
        return this->child_count(items);
    }

    // Returns eobject at specified index.
    field_t * _fields_ast_node_t::eobject_at(size_t index) const
    {
        return ((_fields_item_ast_node_t *)this->child_at(items, index))->to_eobject();
    }

    // Walks this node.
    void _fields_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    ////////// ////////// ////////// ////////// //////////

} } }   // namespace X_ROOT_NS::modules::lang_cs

