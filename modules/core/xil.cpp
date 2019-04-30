
#include <core.h>

namespace X_ROOT_NS { namespace modules { namespace core {

    #define __TraceXilRead      0
    #define __TraceXilWrite     1

    ////////// ////////// ////////// ////////// //////////

    const ref_t ref_t::null(0, 0, 0);
    const res_t res_t::null(0);

    // Converts ref_t to a string.
    ref_t::operator string_t() const
    {
        if(*this == ref_t::null)
            return _T("<ref::null>");

        if(extra == 0)
        {
            if(count > 1) 
                return _F(_T("%1%-%2%"), index, index + count);

            return _F(_T("%1%"), index);
        }
        else
        {
            if(count > 1) 
                return _F(_T("%1%-%2%(%3%)"), index, index + count, extra);

            return _F(_T("%1%(%2%)"), index, extra);
        }
    }

    // Converts res_t to a string.
    res_t::operator string_t() const
    {
        if(*this == res_t::null)
            return _T("<res:null>");

        return _F(_T("%1%"), pos);
    }

    // Returns the begin iterator.
    ref_iterator_t ref_t::begin() const { return ref_iterator_t(ref_t(index, 1, extra)); }

    // Returns the end iterator.
    ref_iterator_t ref_t::end()   const { return ref_iterator_t(ref_t(index + count, 1, extra)); }

    ////////// ////////// ////////// ////////// //////////

    // Xil command.
    X_ENUM_INFO(xil_command_t)

        X_C(empty,          _T("empty"))

        X_C(push,           _T("push"))

        X_C(pop,            _T("pop"))

        X_C(pick,           _T("pick"))

        X_C(call,           _T("call"))

        X_C(jmp,            _T("jmp"))

        X_C(smp,            _T("smp"))

        X_C(al,             _T("al"))

        X_C(cmp,            _T("cmp"))

        X_C(logic,          _T("logic"))

        X_C(bit,            _T("bit"))

        X_C(new_,           _T("new"))

        X_C(copy,           _T("copy"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Xil algorithm command.
    X_ENUM_INFO(xil_al_command_t)

        X_C(empty,          _T("empty"))

        X_C(add,            _T("add"))

        X_C(sub,            _T("sub"))

        X_C(mul,            _T("mul"))

        X_C(div,            _T("div"))

        X_C(mod,            _T("mod"))

        X_C(minus,          _T("minus"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Xil bit command.
    X_ENUM_INFO(xil_bit_command_t)

        X_C(bit_and,        _T("bit_and"))

        X_C(bit_or,         _T("bit_or"))

        X_C(bit_not,        _T("bit_not"))

        X_C(bit_xor,        _T("bit_xor"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Xil Cmp command.
    X_ENUM_INFO(xil_cmp_command_t)

        X_C(empty,          _T("empty"))

        X_C(greater,        _T("greater"))

        X_C(greater_equal,  _T("greater_equal"))

        X_C(less,           _T("less"))

        X_C(less_equal,     _T("less_equal"))

        X_C(equal,          _T("equal"))

        X_C(not_equal,      _T("not_equal"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Xil logic command.
    X_ENUM_INFO(xil_logic_command_t)

        X_C(empty,          _T("empty"))

        X_C(and_,           _T("and"))

        X_C(or_,            _T("or"))

        X_C(not_,           _T("not"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Xil type.
    X_ENUM_INFO(xil_type_t)

        X_C(empty,          _T("empty"))

		X_C(int8,           _T("int8"))

		X_C(uint8,          _T("uint8"))

		X_C(int16,          _T("int16"))

		X_C(uint16,         _T("uint16"))

		X_C(int32,          _T("int32"))

		X_C(uint32,         _T("uint32"))

		X_C(int64,          _T("int64"))

		X_C(uint64,         _T("uint64"))

		X_C(float_,         _T("float"))

		X_C(double_,        _T("double"))

		X_C(bool_,          _T("bool"))

		X_C(char_,          _T("char"))

        X_C(string,         _T("string"))

        X_C(object,         _T("object"))

        X_C(ptr,            _T("ptr"))

    X_ENUM_INFO_END

    // Converts vtype to xil_type.
    xil_type_t to_xil_type(vtype_t vtype)
    {
        switch(vtype)
        {
            case vtype_t::int8_:
                return xil_type_t::int8;

            case vtype_t::uint8_:
                return xil_type_t::uint8;

            case vtype_t::int16_:
                return xil_type_t::int16;

            case vtype_t::uint16_:
                return xil_type_t::uint16;

            case vtype_t::int32_:
                return xil_type_t::int32;

            case vtype_t::uint32_:
                return xil_type_t::uint32;

            case vtype_t::int64_:
                return xil_type_t::int64;

            case vtype_t::uint64_:
                return xil_type_t::uint64;

            case vtype_t::float_:
                return xil_type_t::float_;

            case vtype_t::double_:
                return xil_type_t::double_;

            case vtype_t::bool_:
                return xil_type_t::bool_;

            case vtype_t::char_:
                return xil_type_t::char_;

            case vtype_t::mobject_:
                return xil_type_t::object;

            case vtype_t::ptr_:
                return xil_type_t::ptr;

            case vtype_t::string_:
                return xil_type_t::string;

            default:
                return xil_type_t::__unknown__;
        }
    }

    //-------- ---------- ---------- ---------- ----------

    // Method call type.
    X_ENUM_INFO(xil_call_type_t)

        X_C(empty,          _T("empty"))

        X_C(instance,       _T("instance"))

        X_C(static_,        _T("static"))

        X_C(virtual_,       _T("virtual"))

        X_C(internal,       _T("internal"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Simple xil.
    X_ENUM_INFO(xil_smp_t)

        X_C(empty,          _T("empty"))

        X_C(label,          _T("label"))

        X_C(ret,            _T("ret"))

        X_C(throw_,         _T("throw"))

        X_C(rethrow,        _T("rethrow"))

        X_C(leave,          _T("leave"))

        X_C(end_block,      _T("end_block"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Jmp direction.
    X_ENUM_INFO(xil_jmp_direction_t)

        X_C(near_backward,  _T("near_backward"))

        X_C(near_forward,   _T("near_forward"))

        X_C(backward,       _T("backward"))

        X_C(forward,        _T("forward"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Jmp condition.
    X_ENUM_INFO(xil_jmp_condition_t)

        X_C(none,           _T("none"))

        X_C(true_,          _T("true"))

        X_C(false_,         _T("false"))

        X_C(switch_,        _T("switch"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Storage type.
    X_ENUM_INFO(xil_storage_type_t)

        X_C(empty,          _T("empty"))

        X_C(local,          _T("local"))

        X_C(argument,       _T("argument"))

        X_C(field,          _T("field"))

        X_C(constant,       _T("constant"))

        X_C(array_element,  _T("array_element"))

        X_C(duplicate,      _T("duplicate"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // New type.
    X_ENUM_INFO(xil_new_type_t)

        X_C(default_,       _T("default"))

        X_C(array,          _T("array"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Copy type.
    X_ENUM_INFO(xil_copy_type_t)

        X_C(block_copy,     _T("block_copy"))

        X_C(stack_copy,     _T("stack_copy"))

        X_C(res_copy,       _T("res_copy"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Init type.
    X_ENUM_INFO(xil_init_type_t)

        X_C(array_begin,    _T("array_begin"))

        X_C(array_element,  _T("array_element"))

        X_C(array_end,      _T("array_end"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Convert xil to a string.
    template<typename _xil_t>
    string_t __xil_value_to_string(const _xil_t & xil, xil_type_t xil_type)
    {
        #define __ToStr(t) (al::to_str(xil.template get_extra<t>()))

        switch(xil_type)
        {
            case xil_type_t::int8:
                return __ToStr(int8_t);

            case xil_type_t::uint8:
                return __ToStr(uint8_t);

            case xil_type_t::int16:
                return __ToStr(int16_t);

            case xil_type_t::uint16:
                return __ToStr(uint16_t);

            case xil_type_t::int32:
                return __ToStr(int32_t);

            case xil_type_t::uint32:
                return __ToStr(uint32_t);

            case xil_type_t::int64:
                return __ToStr(int64_t);

            case xil_type_t::uint64:
                return __ToStr(uint64_t);

            case xil_type_t::float_:
                return __ToStr(float);

            case xil_type_t::double_:
                return __ToStr(double);

            case xil_type_t::bool_:
                return __ToStr(bool);

            case xil_type_t::char_:
                return __ToStr(char_t);

            case xil_type_t::string:
                return _T("<STRING>");

            default:
                return _T("?");
        }

        #undef __ToStr
    };
    
    ////////// ////////// ////////// ////////// //////////
    // push_xil_t

    // Converts push_xil_t to a string.
    push_xil_t::operator string_t() const
    {
        switch(stype())
        {
            case xil_storage_type_t::local:
                return _F(_T("push local (%1%)%2%"), dtype(), get_identity());

            case xil_storage_type_t::argument:
                return _F(_T("push argument (%1%)%2%"), dtype(), get_identity());

            case xil_storage_type_t::field:
                return _F(_T("push field (%1%)%2%"), dtype(), field_ref());

            case xil_storage_type_t::array_element:
                return _F(_T("push array element (%1%)%2%"), dtype(), type_ref());

            case xil_storage_type_t::constant:
                return _F(_T("push constant (%1%)%2%"),
                    dtype(), __xil_value_to_string(*this, dtype())
                );

            case xil_storage_type_t::duplicate:
                return _T("dup");

            default:
                return _T("push ?");
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Converts pop_xil_t to a string.
    pop_xil_t::operator string_t() const
    {
        switch(stype())
        {
            case xil_storage_type_t::local:
                return _F(_T("pop local (%1%)%2%"), dtype(), get_identity());

            case xil_storage_type_t::argument:
                return _F(_T("pop argument (%1%)%2%"), dtype(), get_identity());

            case xil_storage_type_t::field:
                return _F(_T("pop field (%1%)%2%"), dtype(), field_ref());

            case xil_storage_type_t::array_element:
                return _F(_T("pop array element (%1%)%2%"), dtype(), type_ref());

            case xil_storage_type_t::constant:
                return _F(_T("pop constant (%1%)%2%"),
                    dtype(), __xil_value_to_string(*this, dtype())
                );

            case xil_storage_type_t::empty:
                return _F(_T("pop %1%"), dtype() != xil_type_t::empty?
                    _str(dtype()) : _T("[struct]")
                );

            default:
                return _T("pop ?");
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // pick_xil_t

    // Converts pick_xil_t to a string.
    pick_xil_t::operator string_t() const
    {
        switch(stype())
        {
            case xil_storage_type_t::local:
                return _F(_T("pick local %1%"), get_identity());

            case xil_storage_type_t::argument:
                return _F(_T("pick argument %1%"), get_identity());

            case xil_storage_type_t::field:
                return _F(_T("pick field %1%(%2%)"), dtype(), field_ref());

            case xil_storage_type_t::array_element:
                return _F(_T("pick array element %1%(%2%)"), dtype(), type_ref());

            case xil_storage_type_t::constant:
                return _F(_T("pick constant (%1%)%2%"),
                    dtype(), __xil_value_to_string(*this, dtype())
                );

            default:
                return _T("pick ?");
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // al_xil_t

    // Converts algorithm xil to a string.
    al_xil_t::operator string_t() const
    {
        return _F(_T("%1% %2% %3%"), cmd(), dtype1(), dtype2());
    }

    ////////// ////////// ////////// ////////// //////////
    // bit_xil_t

    // Converts bit xil to a string.
    bit_xil_t::operator string_t() const
    {
        return _F(_T("%1% %2% %3%"), cmd(), dtype1(), dtype2());
    }

    ////////// ////////// ////////// ////////// //////////
    // logic_xil_t

    // Converts logic xil to a string.
    logic_xil_t::operator string_t() const
    {
        return _F(_T("%1%"), cmd());
    }

    ////////// ////////// ////////// ////////// //////////
    // cmp_xil_t

    // Converts cmp xil to a string.
    cmp_xil_t::operator string_t() const
    {
        return _F(_T("%1% %2% %3%"), cmd(), dtype1(), dtype2());
    }

    ////////// ////////// ////////// ////////// //////////
    // call_xil_t

    // Converts call xil to a string.
    call_xil_t::operator string_t() const
    {
        return _F(_T("call %1% %2%"), call_type(), *(ref_t *)&method);
    }

    ////////// ////////// ////////// ////////// //////////
    // smp_xil_t

    // Converts simple xil to a string.
    smp_xil_t::operator string_t() const
    {
        switch(smp())
        {
            case xil_smp_t::empty:
                return _T("empty");

            case xil_smp_t::label:
                return _T("label");

            case xil_smp_t::ret:
                return _T("ret");

            default:
                return _T("smp: ?");
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // jmp_xil_t

    // Converts jmp xil to a string.
    jmp_xil_t::operator string_t() const
    {
        return _F(_T("jmp %1% %2%"), condition(), step());
    }

    // Sets jmp xil step.
    void jmp_xil_t::set_step(int32_t step)
    {
        enum { positive, negative } sign = (step >= 0)? positive :
            ((step = - step), negative);

        typedef xil_jmp_direction_t d_t;
        if(step <= (int32_t)max_value<byte_t>())
        {
            __extra[0] = (byte_t)step;
            set_direction(sign == positive? d_t::near_forward : d_t::near_backward);
        }
        else
        {
            if(step > ((int32_t)0x00FFFFFF))
                throw _EC(overflow, _T("jmp step overflow"));

            *(int32_t *)__extra = step;
            set_direction(sign == positive? d_t::forward : d_t::backward);
        }
    }

    // Returns jmp step.
    int32_t jmp_xil_t::step() const
    {
        switch(direction())
        {
            case xil_jmp_direction_t::near_backward:
                return -__extra[0];

            case xil_jmp_direction_t::near_forward:
                return __extra[0];

            case xil_jmp_direction_t::backward:
                return -*(int32_t *)__extra;

            case xil_jmp_direction_t::forward:
                return *(int32_t *)__extra;

            default:
                X_UNEXPECTED();
        }
    }

    // Sets switch-case table index.
    void jmp_xil_t::set_tbl(int32_t index)
    {
        if(index < 0 || index > (int32_t)max_value<byte_t>())
            throw _EC(overflow, _T("switch table index overflow"));

        __extra[0] = (byte_t)index;
    }

    // Returns switch-case table index.
    int32_t jmp_xil_t::tbl() const
    {
        return __extra[0];
    }

    ////////// ////////// ////////// ////////// //////////

    // Converts new_xil_t to string.
    new_xil_t::operator string_t() const
    {
        switch(new_type())
        {
            case xil_new_type_t::array:
                return _F(_T("new array"));
 
            case xil_new_type_t::default_:
                return _F(_T("new"));

            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Converts copy xil to a string.
    copy_xil_t::operator string_t() const
    {
        switch(copy_type())
        {
            case xil_copy_type_t::stack_copy:
                return _T("stack copy");

            case xil_copy_type_t::block_copy:
                return _T("block copy");

            case xil_copy_type_t::res_copy:
                return _T("res copy");

            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Converts init xil to a string.
    init_xil_t::operator string_t() const
    {
        switch(init_type())
        {
            case xil_init_type_t::array_begin:
                return _T("begin init array");

            case xil_init_type_t::array_element:
                return _T("init array element");

            case xil_init_type_t::array_end:
                return _T("end init array");

            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Converts xil to a string.
    string_t __to_string(const xil_t * xil)
    {
        if(xil == nullptr)
            return _T("<NULL>");

        switch((xil_command_t)((const xil_base_t *)xil)->command())
        {
            case xil_command_t::empty:
                return _T("<EMPTY>");

            case xil_command_t::push:
                return _str(*(const push_xil_t *)xil);

            case xil_command_t::pop:
                return _str(*(const pop_xil_t *)xil);

            case xil_command_t::pick:
                return _str(*(const pick_xil_t *)xil);

            case xil_command_t::call:
                return _str(*(const call_xil_t *)xil);

            case xil_command_t::jmp:
                return _str(*(const jmp_xil_t *)xil);

            case xil_command_t::smp:
                return _str(*(const smp_xil_t *)xil);

            case xil_command_t::al:
                return _str(*(const al_xil_t *)xil);

            case xil_command_t::cmp:
                return _str(*(const cmp_xil_t *)xil);

            case xil_command_t::logic:
                return _str(*(const logic_xil_t *)xil);

            case xil_command_t::bit:
                return _str(*(const bit_xil_t *)xil);

            case xil_command_t::new_:
                return _str(*(const new_xil_t *)xil);

            case xil_command_t::copy:
                return _str(*(const copy_xil_t *)xil);

            case xil_command_t::init:
                return _str(*(const init_xil_t *)xil);

            default:
                return _T("<UNKNOWN>");
        }
    }

    // Returns size of xil_t.
    size_t __size_of(const xil_t * xil)
    {
        _A(xil != nullptr);

        switch(((const xil_base_t *)xil)->command())
        {
            case xil_command_t::empty:
                return 1;

            case xil_command_t::push:
                return size_of(*(const push_xil_t *)xil);

            case xil_command_t::pop:
                return size_of(*(const pop_xil_t *)xil);

            case xil_command_t::pick:
                return size_of(*(const pick_xil_t *)xil);

            case xil_command_t::call:
                return size_of(*(const call_xil_t *)xil);

            case xil_command_t::jmp:
                return size_of(*(const jmp_xil_t *)xil);

            case xil_command_t::smp:
                return size_of(*(const smp_xil_t *)xil);

            case xil_command_t::al:
                return size_of(*(const al_xil_t *)xil);

            case xil_command_t::cmp:
                return size_of(*(const cmp_xil_t *)xil);

            case xil_command_t::logic:
                return size_of(*(const logic_xil_t *)xil);

            case xil_command_t::bit:
                return size_of(*(const bit_xil_t *)xil);

            case xil_command_t::new_:
                return size_of(*(const new_xil_t *)xil);

            case xil_command_t::copy:
                return size_of(*(const copy_xil_t *)xil);

            case xil_command_t::init:
                return size_of(*(const init_xil_t *)xil);

            default:
                _PP(((const xil_base_t *)xil)->command());
                X_UNEXPECTED();
        }
    }

    // Reads a xil.
    bool xil_reader_t::read(const xil_base_t ** out_xil)
    {
        if(__bytes >= __bytes_end)
            return false;

        *out_xil = (const xil_base_t *)__bytes;

        #if __TraceXilRead

        _PF(_T("[%1%] %2%"), __size_of(*out_xil), __to_string(*out_xil));

        #endif

        __bytes += __size_of(*out_xil);
        return true;
    }

    ////////// ////////// ////////// ////////// //////////

    // Constructor.
    xil_buffer_writer_t::xil_buffer_writer_t(xil_buffer_t & buffer, method_t * method)
        : __buffer(buffer), __method(method)
    {
        _A(method != nullptr);
    }

    // Writes xils to a buffer.
    size_t xil_buffer_writer_t::write(xil_pool_t & pool)
    {
        __trace_method();

        method_xil_stub_t stub;
        stub.write(__buffer);

        size_t init_size = __buffer.size();

        for(xil_t * xil : pool)
        {
            #if __TraceXilWrite

            _PF(_T("[%1%] %2%"), __size_of(xil), __to_string(xil));

            #endif

            __buffer.write((const byte_t *)xil, __size_of(xil));
        }

        size_t size = __buffer.size() - init_size;
        __size += size;

        return size;
    }

    void xil_buffer_writer_t::__trace_method()
    {
        #if __TraceXilWrite

        if(!__traced_method)
        {
            _PF(_T("\n%1%"), __method);
            __traced_method = true;
        }

        #endif
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns method call type.
    xil_call_type_t call_type_of_method(method_base_t * method)
    {
        method_t * m = to_general(method);

        if(m->is_extern())
            return xil_call_type_t::internal;

        if(m->is_static())
            return xil_call_type_t::static_;

        if(m->is_virtual() || m->is_override() || m->is_abstract())
            return xil_call_type_t::virtual_;

        return xil_call_type_t::instance;
    }

    ////////// ////////// ////////// ////////// //////////

} } }

