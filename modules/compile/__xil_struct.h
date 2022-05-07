#ifndef __COMPILE_XIL_STRUCT_H__
#define __COMPILE_XIL_STRUCT_H__

namespace X_ROOT_NS::modules::compile {

    ////////// ////////// ////////// ////////// //////////

    // Algorighm xil.
    struct x_al_xil_t : xil_extra_t<al_xil_t>
    {
        typedef xil_extra_t<al_xil_t> __super_t;

        using __super_t::__super_t;
    };

    //-------- ---------- ---------- ---------- ----------

    // Bit xil.
    struct x_bit_xil_t : xil_extra_t<bit_xil_t>
    {
        typedef xil_extra_t<bit_xil_t> __super_t;

        using __super_t::__super_t;
    };

    //-------- ---------- ---------- ---------- ----------

    // Logic xil.
    struct x_logic_xil_t : xil_extra_t<logic_xil_t>
    {
        typedef xil_extra_t<logic_xil_t> __super_t;

        using __super_t::__super_t;
    };

    //-------- ---------- ---------- ---------- ----------

    // Compare xil.
    struct x_cmp_xil_t : xil_extra_t<cmp_xil_t>
    {
        typedef xil_extra_t<cmp_xil_t> __super_t;

        using __super_t::__super_t;
    };

    //-------- ---------- ---------- ---------- ----------

    // Cast xil.
    struct x_cast_xil_t : xil_extra_t<cast_xil_t>
    {
        typedef xil_extra_t<cast_xil_t> __super_t;

        using __super_t::__super_t;
    };

    //-------- ---------- ---------- ---------- ----------

    // Call xil.
    struct x_call_xil_t : xil_extra_t<call_xil_t>
    {
        typedef xil_extra_t<call_xil_t> __super_t;

        using __super_t::__super_t;
    };

    //-------- ---------- ---------- ---------- ----------

    // Method call xil.
    struct x_method_call_xil_t : x_call_xil_t
    {
        typedef x_call_xil_t __super_t;

        x_method_call_xil_t(xil_call_type_t call_type, ref_t method)
            : __super_t(call_type, method)
        {
            _A(method != ref_t::null);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Command call xil.
    struct x_command_call_xil_t : x_call_xil_t
    {
        typedef x_call_xil_t __super_t;

        x_command_call_xil_t(xil_call_command_t command) : __super_t(command)
        {
            // Empty.
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Pop empty xil.
    struct x_pop_empty_xil_t : xil_extra_t<pop_xil_t>
    {
        typedef xil_extra_t<pop_xil_t> __super_t;

        x_pop_empty_xil_t(ref_t struct_type_ref) : __super_t(struct_type_ref) { }

        x_pop_empty_xil_t(xil_type_t xil_type)
            : __super_t(xil_storage_type_t::empty, xil_type)
        { }
    };

    //-------- ---------- ---------- ---------- ----------

    // Pop array element xil.
    struct x_pop_array_element_xil_t : xil_extra_t<pop_xil_t>
    {
        typedef xil_extra_t<pop_xil_t> __super_t;

        x_pop_array_element_xil_t(xil_type_t dtype, ref_t array_type_ref)
            : __super_t(xil_storage_type_t::array_element, dtype)
        {
            this->set_ref(array_type_ref);
        }
    };

    //-------- ---------- ---------- ---------- ----------
    // New xil base.
    struct x_new_xil_base_t : xil_extra_t<new_xil_t>
    {
        typedef xil_extra_t<new_xil_t> __super_t;

        x_new_xil_base_t(xil_new_type_t new_type,  ref_t type_ref)
            : __super_t(new_type)
        {
            this->set_type_ref(type_ref);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // New xil.
    struct x_new_xil_t : x_new_xil_base_t
    {
        typedef x_new_xil_base_t __super_t;

        x_new_xil_t(ref_t type_ref) : __super_t(xil_new_type_t::default_, type_ref) { }
    };

    //-------- ---------- ---------- ---------- ----------

    // New array xil.
    struct x_new_array_xil_t : x_new_xil_base_t
    {
        typedef x_new_xil_base_t __super_t;

        x_new_array_xil_t(ref_t type_ref) : __super_t(xil_new_type_t::array, type_ref) { }
    };

    //-------- ---------- ---------- ---------- ----------

    // New stack_alloc xil.
    struct x_stack_alloc_xil_t : x_new_xil_base_t
    {
        typedef x_new_xil_base_t __super_t;

        x_stack_alloc_xil_t(ref_t type_ref) : __super_t(xil_new_type_t::stack_alloc, type_ref) { }
    };

    //-------- ---------- ---------- ---------- ----------

    // New stack_allocs xil.
    struct x_stack_allocs_xil_t : x_new_xil_base_t
    {
        typedef x_new_xil_base_t __super_t;

        x_stack_allocs_xil_t(ref_t type_ref, uint32_t count)
            : __super_t(xil_new_type_t::stack_allocs, type_ref)
        {
            this->set_count(count);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Begin init array.
    struct x_array_begin_xil_t : xil_extra_t<init_xil_t>
    {
        typedef xil_extra_t<init_xil_t> __super_t;

        x_array_begin_xil_t(xil_type_t dtype)
            : __super_t(xil_init_type_t::array_begin, dtype)
        { }
    };

    //-------- ---------- ---------- ---------- ----------

    // Init array element.
    struct x_array_element_xil_t : xil_extra_t<init_xil_t>
    {
        typedef xil_extra_t<init_xil_t> __super_t;

        x_array_element_xil_t(xil_type_t dtype)
            : __super_t(xil_init_type_t::array_element, dtype)
        { }

        x_array_element_xil_t(ref_t type_ref)
            : __super_t(xil_init_type_t::array_element, xil_type_t::empty)
        {
            this->set_type_ref(type_ref);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // End init array.
    struct x_array_end_xil_t : xil_extra_t<init_xil_t>
    {
        typedef xil_extra_t<init_xil_t> __super_t;

        x_array_end_xil_t(xil_type_t dtype)
            : __super_t(xil_init_type_t::array_end, dtype)
        { }
    };

    //-------- ---------- ---------- ---------- ----------

    // Push variable xil.
    struct x_push_variable_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        x_push_variable_xil_t(xil_storage_type_t storage_type, xil_type_t xtype, msize_t identity)
            : __super_t(storage_type, xtype)
        {
            this->set_identity(identity);
        }
    };

    //-------- ---------- ---------- ---------- ----------
    // Push this, calling bottom.

    // Push this xil. ( for ref object. )
    struct x_push_this_ref_xil_t : x_push_variable_xil_t
    {
        typedef x_push_variable_xil_t __super_t;

        x_push_this_ref_xil_t()
            : __super_t(xil_storage_type_t::argument, xil_type_t::object, 0)
        { }
    };

    // Push this xil. ( for val object. )
    struct x_push_this_val_xil_t : x_push_variable_xil_t
    {
        typedef x_push_variable_xil_t __super_t;

        x_push_this_val_xil_t()
            : __super_t(xil_storage_type_t::argument, xil_type_t::ptr, 0)
        { }
    };

    // Push this xil. ( for ref object. )
    struct x_push_this_content_xil_t : x_push_variable_xil_t
    {
        typedef x_push_variable_xil_t __super_t;

        x_push_this_content_xil_t(xil_type_t xtype)
            : __super_t(xil_storage_type_t::argument_content, xtype, 0)
        { }
    };

    //-------- ---------- ---------- ---------- ----------
    // Push variables.

    // Push local xil.
    struct x_push_local_xil_t : x_push_variable_xil_t
    {
        typedef x_push_variable_xil_t __super_t;

        x_push_local_xil_t(xil_type_t dtype, msize_t identity)
            : __super_t(xil_storage_type_t::local, dtype, identity)
        { }
    };

    // Push argument xil.
    struct x_push_argument_xil_t : x_push_variable_xil_t
    {
        typedef x_push_variable_xil_t __super_t;

        x_push_argument_xil_t(xil_type_t dtype, msize_t identity)
            : __super_t(xil_storage_type_t::argument, dtype, identity)
        { }
    };

    // Push field xil.
    struct x_push_field_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        x_push_field_xil_t(ref_t field_ref, xil_type_t dtype)
            : __super_t(xil_storage_type_t::field, dtype)
        {
            this->set_ref(field_ref);
        }
    };

    // Push array element xil.
    struct x_push_array_element_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        x_push_array_element_xil_t(xil_type_t dtype, ref_t array_type_ref)
            : __super_t(xil_storage_type_t::array_element, dtype)
        {
            this->set_ref(array_type_ref);
        }
    };

    //-------- ---------- ---------- ---------- ----------
    // Push variable address.

    // Push local address xil.
    struct x_push_local_addr_xil_t : x_push_variable_xil_t
    {
        typedef x_push_variable_xil_t __super_t;

        x_push_local_addr_xil_t(msize_t identity)
            : __super_t(xil_storage_type_t::local_addr, xil_type_t::ptr, identity)
        { }
    };


    // Push argument address xil.
    struct x_push_argument_addr_xil_t : x_push_variable_xil_t
    {
        typedef x_push_variable_xil_t __super_t;

        x_push_argument_addr_xil_t(msize_t identity)
            : __super_t(xil_storage_type_t::argument_addr, xil_type_t::ptr, identity)
        { }
    };

    // Push field address xil.
    struct x_push_field_addr_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        x_push_field_addr_xil_t(ref_t field_ref)
            : __super_t(xil_storage_type_t::field_addr, xil_type_t::ptr)
        {
            this->set_ref(field_ref);
        }
    };

    // Push array element address xil.
    struct x_push_array_element_addr_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        x_push_array_element_addr_xil_t(ref_t array_type_ref)
            : __super_t(xil_storage_type_t::array_element_addr, xil_type_t::ptr)
        {
            this->set_ref(array_type_ref);
        }
    };

    //-------- ---------- ---------- ---------- ----------
    // Push content.

    // Push content from address in local variable.
    struct x_push_local_content_xil_t : x_push_variable_xil_t
    {
        typedef x_push_variable_xil_t __super_t;

        x_push_local_content_xil_t(xil_type_t dtype, msize_t identity)
            : __super_t(xil_storage_type_t::local_content, dtype, identity)
        { }
    };

    // Push content from address in argument.
    struct x_push_argument_content_xil_t : x_push_variable_xil_t
    {
        typedef x_push_variable_xil_t __super_t;

        x_push_argument_content_xil_t(xil_type_t dtype, msize_t identity)
            : __super_t(xil_storage_type_t::argument_content, dtype, identity)
        { }
    };

    // Push content from address in field.
    struct x_push_field_content_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        x_push_field_content_xil_t(ref_t field_ref, xil_type_t dtype)
            : __super_t(xil_storage_type_t::field_content, dtype)
        {
            this->set_ref(field_ref);
        }
    };

    // Push content from address in array element.
    struct x_push_array_element_content_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        x_push_array_element_content_xil_t(xil_type_t dtype, ref_t array_type_ref)
            : __super_t(xil_storage_type_t::array_element_content, dtype)
        {
            this->set_ref(array_type_ref);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Push object xil.
    struct x_push_object_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        x_push_object_xil_t(xil_storage_object_type_t object_type, ref_t ref)
            : __super_t(xil_storage_type_t::object)
        {
            this->set_object_type(object_type);
            this->set_ref(ref);
        }
    };

    // Push calling bottom.
    struct x_push_calling_bottom_xil_t : x_push_variable_xil_t
    {
        typedef x_push_variable_xil_t __super_t;

        x_push_calling_bottom_xil_t()
            : __super_t(xil_storage_type_t::argument, xil_type_t::ptr, XIL_CALLING_BOTTOM_IDENTITY)
        { }
    };

    // Push duplicate xil.
    struct x_push_duplicate_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        x_push_duplicate_xil_t() : __super_t(xil_storage_type_t::duplicate) { }
    };

    // Push convert xil.
    struct x_push_convert_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        x_push_convert_xil_t(xil_type_t dtype1, xil_type_t dtype2)
            : __super_t(xil_storage_type_t::convert)
        {
            this->set_dtype(dtype1);
            this->set_dtype2(dtype2);
        }
    };

    // Push box xil.
    struct x_push_box_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        x_push_box_xil_t(ref_t type_ref, xil_box_type_t box_type = xil_box_type_t::pop)
            : __super_t(xil_storage_type_t::box)
        {
            this->set_ref(type_ref);
            this->set_box_type(box_type);
        }
    };

    // Push unbox xil.
    struct x_push_unbox_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        x_push_unbox_xil_t(ref_t type_ref)
            : __super_t(xil_storage_type_t::unbox)
        {
            this->set_dtype(xil_type_t::empty);
            this->set_ref(type_ref);
        }
    };

    // Push params address xil.
    struct x_push_params_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        x_push_params_xil_t() : __super_t(xil_storage_type_t::params) { }
    };

    // Push constant value xil.
    template<typename _value_t>
    struct x_push_const_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        x_push_const_xil_t(xil_type_t xtype, const tvalue_t & value)
            : __super_t(xil_storage_type_t::constant, xtype)
        {
            this->set_extra(value.get_value<_value_t>());
        }
    };

    // Push constant generic value xil.
    struct x_push_generic_const_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        x_push_generic_const_xil_t(ref_t type_ref)
            : __super_t(xil_storage_type_t::constant, xil_type_t::empty)
        {
            this->set_ref(type_ref);
        }
    };

    // Push string xil.
    struct x_push_string_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        x_push_string_xil_t(res_t res)
            : __super_t(xil_storage_type_t::constant, xil_type_t::string)
        {
            this->set_extra(res);
        }
    };

    // Push null xil.
    struct x_push_null_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        x_push_null_xil_t()
            : __super_t(xil_storage_type_t::constant, xil_type_t::object)
        {
            this->set_ref(ref_t::null);
        }
    };

    // Push type xil.
    struct x_push_type_xil_t : xil_extra_t<push_xil_t>
    {
        typedef xil_extra_t<push_xil_t> __super_t;

        x_push_type_xil_t(ref_t type_ref)
            : __super_t(xil_storage_type_t::constant, xil_type_t::object)
        {
            this->set_ref(type_ref);
        }
    };

    //-------- ---------- ---------- ---------- ----------
    // Pop.

    // Pop variable xil.
    struct x_pop_variable_xil_t : xil_extra_t<pop_xil_t>
    {
        typedef xil_extra_t<pop_xil_t> __super_t;

        x_pop_variable_xil_t(xil_storage_type_t storage_type, xil_type_t xtype, msize_t identity)
            : __super_t(storage_type, xtype)
        {
            this->set_identity(identity);
        }
    };

    // Pop local xil.
    struct x_pop_local_xil_t : x_pop_variable_xil_t
    {
        typedef x_pop_variable_xil_t __super_t;

        x_pop_local_xil_t(xil_type_t xtype, msize_t identity)
            : __super_t(xil_storage_type_t::local, xtype, identity)
        { }
    };

    // Pop argument xil.
    struct x_pop_argument_xil_t : x_pop_variable_xil_t
    {
        typedef x_pop_variable_xil_t __super_t;

        x_pop_argument_xil_t(xil_type_t xtype, msize_t identity)
            : __super_t(xil_storage_type_t::argument, xtype, identity)
        { }
    };

    // Pop field xil.
    struct x_pop_field_xil_t : xil_extra_t<pop_xil_t>
    {
        typedef xil_extra_t<pop_xil_t> __super_t;

        x_pop_field_xil_t(xil_type_t xtype, ref_t field_ref)
            : __super_t(xil_storage_type_t::field, xtype)
        {
            this->set_ref(field_ref);
        }
    };

    //-------- ---------- ---------- ---------- ----------
    // Pop variable address.

    // Pop local address xil.
    struct x_pop_local_addr_xil_t : x_pop_variable_xil_t
    {
        typedef x_pop_variable_xil_t __super_t;

        x_pop_local_addr_xil_t(xil_type_t xtype, msize_t identity)
            : __super_t(xil_storage_type_t::local_addr, xtype, identity)
        { }
    };

    // Pop argument address xil.
    struct x_pop_argument_addr_xil_t : x_pop_variable_xil_t
    {
        typedef x_pop_variable_xil_t __super_t;

        x_pop_argument_addr_xil_t(xil_type_t xtype, msize_t identity)
            : __super_t(xil_storage_type_t::argument_addr, xtype, identity)
        { }
    };

    // Pop field address xil.
    struct x_pop_field_addr_xil_t : xil_extra_t<pop_xil_t>
    {
        typedef xil_extra_t<pop_xil_t> __super_t;

        x_pop_field_addr_xil_t(xil_type_t xtype, ref_t field_ref)
            : __super_t(xil_storage_type_t::field_addr, xtype)
        {
            this->set_ref(field_ref);
        }
    };

    // Push array element address xil.
    struct x_pop_array_element_addr_xil_t : xil_extra_t<pop_xil_t>
    {
        typedef xil_extra_t<pop_xil_t> __super_t;

        x_pop_array_element_addr_xil_t(xil_type_t xtype, ref_t array_type_ref)
            : __super_t(xil_storage_type_t::array_element_addr, xtype)
        {
            this->set_ref(array_type_ref);
        }
    };

    //-------- ---------- ---------- ---------- ----------
    // Pick.

    // Pick variable xil.
    struct x_pick_variable_xil_t : xil_extra_t<pick_xil_t>
    {
        typedef xil_extra_t<pick_xil_t> __super_t;

        x_pick_variable_xil_t(xil_storage_type_t storage_type, xil_type_t xtype, msize_t identity)
            : __super_t(storage_type, xtype)
        {
            this->set_identity(identity);
        }
    };

    // Pick local xil.
    struct x_pick_local_xil_t : x_pick_variable_xil_t
    {
        typedef x_pick_variable_xil_t __super_t;

        x_pick_local_xil_t(xil_type_t xtype, msize_t identity)
            : __super_t(xil_storage_type_t::local, xtype, identity)
        { }
    };

    // Pick argument xil.
    struct x_pick_argument_xil_t : x_pick_variable_xil_t
    {
        typedef x_pick_variable_xil_t __super_t;

        x_pick_argument_xil_t(xil_type_t xtype, msize_t identity)
            : __super_t(xil_storage_type_t::argument, xtype, identity)
        { }
    };

    // Pick field xil.
    struct x_pick_field_xil_t : xil_extra_t<pick_xil_t>
    {
        typedef xil_extra_t<pick_xil_t> __super_t;

        x_pick_field_xil_t(xil_type_t xtype, ref_t field_ref) _NE
            : __super_t(xil_storage_type_t::field, xtype)
        {
            this->set_ref(field_ref);
        }
    };

    //-------- ---------- ---------- ---------- ----------
    // Pick variable address.

    // Pick local address xil.
    struct x_pick_local_addr_xil_t : x_pick_variable_xil_t
    {
        typedef x_pick_variable_xil_t __super_t;

        x_pick_local_addr_xil_t(xil_type_t xtype, msize_t identity)
            : __super_t(xil_storage_type_t::local_addr, xtype, identity)
        { }
    };

    // Pick argument address xil.
    struct x_pick_argument_addr_xil_t : x_pick_variable_xil_t
    {
        typedef x_pick_variable_xil_t __super_t;

        x_pick_argument_addr_xil_t(xil_type_t xtype, msize_t identity)
            : __super_t(xil_storage_type_t::argument_addr, xtype, identity)
        { }
    };

    // Pick field address xil.
    struct x_pick_field_addr_xil_t : xil_extra_t<pick_xil_t>
    {
        typedef xil_extra_t<pick_xil_t> __super_t;

        x_pick_field_addr_xil_t(xil_type_t xtype, ref_t field_ref)
            : __super_t(xil_storage_type_t::field_addr, xtype)
        {
            this->set_ref(field_ref);
        }
    };

    // Pick array element address xil.
    struct x_pick_array_element_addr_xil_t : xil_extra_t<pick_xil_t>
    {
        typedef xil_extra_t<pick_xil_t> __super_t;

        x_pick_array_element_addr_xil_t(xil_type_t xtype, ref_t array_type_ref)
            : __super_t(xil_storage_type_t::array_element_addr, xtype)
        {
            this->set_ref(array_type_ref);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Jmp xil.
    struct x_jmp_xil_t : xil_extra_t<jmp_xil_t>
    {
        typedef xil_extra_t<jmp_xil_t> __super_t;

        using __super_t::__super_t;
    };

    // Switch xil.
    struct x_switch_xil_t : xil_extra_t<jmp_xil_t>
    {
        typedef xil_extra_t<jmp_xil_t> __super_t;

        x_switch_xil_t(int tbl_idx) _NE : __super_t(xil_jmp_model_t::switch_)
        {
            this->set_tbl(tbl_idx);
        }
    };

    // Object copy xil.
    struct x_object_copy_xil_t : xil_extra_t<copy_xil_t>
    {
        typedef xil_extra_t<copy_xil_t> __super_t;
        typedef xil_copy_kind_t         __copy_kind_t;

        x_object_copy_xil_t(ref_t type_ref, __copy_kind_t copy_kind = __copy_kind_t::default_) _NE
            : __super_t(xil_copy_type_t::object_copy)
        {
            this->set_type_ref(type_ref);
            this->set_copy_kind(copy_kind);
        }
    };

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::compile

#endif // __COMPILE_XIL_STRUCT_H__
