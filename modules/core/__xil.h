#ifndef __CORE_XIL_H__
#define __CORE_XIL_H__

namespace X_ROOT_NS { namespace modules { namespace core {

    #define __Enum(name)    X_ENUM_(name, sizeof(byte_t))
    #define __EnumEnd       X_ENUM_END
    #define __E(value)      ((int8_t)(value))

    ////////// ////////// ////////// ////////// //////////
    // xil_t

    __Enum(xil_command_t)

        empty           = 0,

        push            = 1,

        pop             = 2,

        pick            = 3,

        call            = 4,

        jmp             = 5,

        smp             = 6,

        al              = 7,

        cmp             = 8,

        logic           = 9,

        bit             = 10,

        new_            = 11,

    __EnumEnd

    //-------- ---------- ---------- ---------- ----------

    __Enum(xil_al_command_t)

        empty           = 0,

        add             = 1,

        sub             = 2,

        mul             = 3,

        div             = 4,

        mod             = 5,

        minus           = 6,

        positive        = 7,

    __EnumEnd

    //-------- ---------- ---------- ---------- ----------

    __Enum(xil_bit_command_t)

        empty           = 0,

        bit_and         = 1,

        bit_or          = 2,

        bit_not         = 3,

        bit_xor         = 4,

        left_shift      = 5,

        right_shift     = 6,

    __EnumEnd

    //-------- ---------- ---------- ---------- ----------

    __Enum(xil_cmp_command_t)

        empty           = 0,

        greater         = 1,

        greater_equal   = 2,

        less            = 3,

        less_equal      = 4,

        equal           = 5,

        not_equal       = 6,

    __EnumEnd

    //-------- ---------- ---------- ---------- ----------

    __Enum(xil_logic_command_t)

        empty           = 0,

        and_            = 1,

        or_             = 2,

        not_            = 3,

    __EnumEnd

    //-------- ---------- ---------- ---------- ----------

    __Enum(xil_type_t)

        empty           = 0,

		int8            = 1,

		uint8           = 2,

		int16           = 3,

		uint16          = 4,

		int32           = 5,

		uint32          = 6,

		int64           = 7,

		uint64          = 8,

		float_          = 9,

		double_         = 10,

		bool_           = 11,

		char_           = 12,

        string          = 13,

        object          = 14,

        ptr             = 15,

    __EnumEnd

    xil_type_t to_xil_type(vtype_t vtype);

    //-------- ---------- ---------- ---------- ----------

    __Enum(xil_call_type_t)

        empty           = 0,

        virtual_        = 1,

        instance        = 2,

        static_         = 3,

        internal        = 4,

    __EnumEnd

    //-------- ---------- ---------- ---------- ----------

    __Enum(xil_smp_t)

        empty           = 0,

        label           = 1,

        ret             = 2,

    __EnumEnd

    //-------- ---------- ---------- ---------- ----------

    __Enum(xil_jmp_direction_t)

        near_backward   = 0,

        near_forward    = 1,

        backward        = 2,

        forward         = 3,

    __EnumEnd

    //-------- ---------- ---------- ---------- ----------

    __Enum(xil_jmp_condition_t)

        none    = __default__,

        true_   = 1,

        false_  = 2,

        switch_ = 3,

    __EnumEnd

    //-------- ---------- ---------- ---------- ----------

    __Enum(xil_storage_type_t)

        empty           = 0,

        local           = 1,

        argument        = 2,

        field           = 3,

        constant        = 4,

        duplicate       = 5,

    __EnumEnd

    //-------- ---------- ---------- ---------- ----------

    __Enum(xil_new_type_t)

        default_        = __default__,

        array           = 1,

    __EnumEnd

    ////////// ////////// ////////// ////////// //////////

    xil_call_type_t call_type_of_method(method_t * method);

    template<typename _xil_t>
    void __trace_write_xil(const _xil_t & xil)
    {
        //_P(_str(xil));
    }

    ////////// ////////// ////////// ////////// //////////
    // struct ref_t

    struct ref_iterator_t;
    struct ref_t : compare_operators_t<ref_t, uint32_t>
    {
        typedef ref_t                                   __self_t;
        typedef compare_operators_t<ref_t, uint32_t>    __super_t;

        typedef ref_t           value_type;
        typedef ref_iterator_t  iterator;

        ref_t() = default;
        constexpr ref_t(uint32_t index, uint32_t count = 1, int32_t extra = 0)
            : index(index), extra(extra), count(count)
        { }

        uint32_t    index   : 20;
        uint32_t    extra   : 3;
        uint32_t    count   : 9;

        operator string_t() const;

        operator uint32_t() const { return *(uint32_t *)this; }

        ref_t operator [] (size_t offset) const
        {
            return ref_t(index + offset, 1, extra);
        }

        int operator - (ref_t f) const { return index - f.index; }

        ref_t & operator ++ () { index++; return *this; }
        ref_t   operator ++ (int) { ref_t f = *this; index++; return f; }

        bool operator == (ref_t f) const
        {
            return __super_t::operator == (f) || (count == 0 && f.count == 0);
        }

        bool operator != (ref_t f) const
        {
            return ! operator == (f);
        }

        size_t size() const { return count; }
        bool empty() const { return count == 0; }

        ref_iterator_t begin() const;
        ref_iterator_t end()   const;

        static const ref_t null;
    };

    struct ref_iterator_t : iterator_base_t<ref_iterator_t>
    {
        typedef ref_iterator_t __self_t;
        ref_iterator_t(const ref_t & ref) : __ref(ref) { }

        ref_t operator * () const { return __ref; }

        void increase(int inc)
        {
            __ref.index += inc;
        }

        bool equals(const __self_t & it) const
        {
            return __ref == it.__ref;
        }

        ref_t __ref;
    };

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, ref_t r)
    {
        static_assert(sizeof(ref_t) == sizeof(uint32_t), "sizeof(ref_t) must equals 32");
        stream << *(uint32_t *)&r;
    }

    template<typename stream_t>
    stream_t & operator >> (stream_t & stream, ref_t & r)
    {
        static_assert(sizeof(ref_t) == sizeof(uint32_t), "sizeof(ref_t) must equals 32");
        stream >> *(uint32_t *)&r;
    }

    ////////// ////////// ////////// ////////// //////////
    // struct res_t

    struct res_t : compare_operators_t<res_t, uint32_t>
    {
        res_t() = default;
        constexpr res_t(uint32_t pos) : pos(pos) { }

        uint32_t    pos;

        operator string_t() const;

        bool empty() const { return pos == 0; }

        static const res_t null;

        operator uint32_t() const { return *(uint32_t *)this; }
    };

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, res_t r)
    {
        static_assert(sizeof(res_t) == sizeof(uint32_t), "sizeof(res_t) must equals 32");
        stream << *(uint32_t *)&r;
    }

    template<typename stream_t>
    stream_t & operator >> (stream_t & stream, res_t & r)
    {
        static_assert(sizeof(res_t) == sizeof(uint32_t), "sizeof(res_t) must equals 32");
        stream >> *(uint32_t *)&r;
    }

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        template<size_t size>
        struct __array_t
        {
            byte_t data[size];
        };

        template<typename _obj_t>
        struct __array_object_t : __array_t<sizeof(_obj_t)>
        {
            typedef __array_t<sizeof(_obj_t)> __super_t;
            typedef __array_object_t<_obj_t>  __self_t;
            typedef _obj_t obj_t;

            using __super_t::__super_t;

            __array_object_t(const _obj_t & obj)
            {
                *(_obj_t *)__super_t::data = obj;
            }

            operator _obj_t & () const
            {
                return *(_obj_t *)__super_t::data;
            }

            __self_t & operator = (const _obj_t & obj)
            {
                *(_obj_t *)__super_t::data = obj;
                return *this;
            }
        };

        typedef __array_object_t<ref_t> __ref_t;
        typedef __array_object_t<res_t> __res_t;
    }

    template<typename _obj_t> using array_object_t = __array_object_t<_obj_t>;

    ////////// ////////// ////////// ////////// //////////

    struct xil_header_t
    {
        template<typename cmd1_t, typename cmd2_t>
        xil_header_t(cmd1_t cmd1, cmd2_t cmd2)
            : cmd1((byte_t)cmd1), cmd2((byte_t)cmd2) { }

        template<typename cmd1_t>
        xil_header_t(cmd1_t cmd1) : cmd1((byte_t)cmd1), cmd2(0) { }

        byte_t cmd1    : 4;
        byte_t cmd2    : 4;
    };

    //-------- ---------- ---------- ---------- ----------

    struct xil_base_t : xil_header_t, xil_t
    {
        template<typename ... cmds_t>
        xil_base_t(cmds_t ... cmds) : xil_header_t(cmds ...) { }
    };

    ////////// ////////// ////////// ////////// //////////

    #define __XilSize(entity_t)  ((size_t)&((entity_t *)nullptr)->____end)

    #define __BeginXil(name)                                        \
        struct name##_xil_t : xil_base_t                            \
        {                                                           \
            typedef name##_xil_t __self_t;                          \
            typedef xil_base_t   __super_t;                         \
            name##_xil_t() = default;

    //   ...

    #define __EndXil                                                \
        };

    ////////// ////////// ////////// ////////// //////////

    template<typename _xil_t> struct __max_size_of_t { };

    #define __DefineMaxSize(_type_t, _max)              \
        template<> struct __max_size_of_t<_type_t>      \
    {                                                   \
        static constexpr size_t value()                 \
        {                                               \
            return _max;                                \
        }                                               \
    };

    #define __MaxSizeOf(_type_t)                        \
        (__max_size_of_t<_type_t>::value())

    constexpr size_t size_of(xil_type_t type)
    {
        switch(type)
        {
            case xil_type_t::int8:
            case xil_type_t::uint8:
            case xil_type_t::bool_:
                return 1;

            case xil_type_t::int16:
            case xil_type_t::uint16:
                return 2;

            case xil_type_t::int32:
            case xil_type_t::uint32:
            case xil_type_t::float_:
		    case xil_type_t::char_:
                return 4;

            case xil_type_t::int64:
            case xil_type_t::uint64:
            case xil_type_t::double_:
                return 8;

            case xil_type_t::object:
            case xil_type_t::ptr:
                return sizeof(__ref_t);

            case xil_type_t::string:
                return sizeof(__res_t);

            default:
                return 0;
        }
    }

    __DefineMaxSize(xil_type_t, 8)

    ////////// ////////// ////////// ////////// //////////

    /* push_xil_t:
        xil_command_t::push     : 4;
        xil_storage_type_t      : 4;
        xil_type_t dtype        : 4;
        uint8_t index           : 4;
    */

    __BeginXil(push)

        push_xil_t(xil_storage_type_t stype, xil_type_t dtype = xil_type_t::empty)
            : __super_t(xil_command_t::push, stype), __dtype((byte_t)dtype) { }

        byte_t __dtype          : 4;        // xil_type_t
        byte_t identity         : 4;

        byte_t __extra[0];

        xil_storage_type_t stype() const { return (xil_storage_type_t)cmd2; }
        xil_type_t dtype() const { return (xil_type_t)__dtype; }

        uint16_t get_identity() const
        {
            return identity < 15? identity : get_extra<uint16_t>();
        }

        void set_identity(uint16_t identity)
        {
            if(identity < 15)
            {
                this->identity = identity;
            }
            else
            {
                this->identity = 15;
                *(uint16_t *)__extra = identity;
            }
        }

        ref_t field_ref() const { return *(ref_t *)__extra; }
        void set_field_ref(ref_t ref) { *(ref_t *)__extra = ref; }

        template<typename t> const t & get_extra() const
        {
            return *(t *)__extra;
        }

        operator string_t() const;

    __EndXil

    //-------- ---------- ---------- ---------- ----------

    constexpr size_t size_of(const push_xil_t & xil)
    {
        size_t size = 0;
        switch(xil.stype())
        {
            case xil_storage_type_t::local:
            case xil_storage_type_t::argument:
                if(xil.identity == 15)
                    size += sizeof(uint16_t);
                break;

            case xil_storage_type_t::field:
                size += sizeof(__ref_t);
                break;

            case xil_storage_type_t::constant:
                size += size_of(xil.dtype());
                break;

            case xil_storage_type_t::duplicate:
                size -= 1;      // __dtype, identity no use
                break;

            default:
                break;
        }

        return size + sizeof(push_xil_t);
    }

    __DefineMaxSize(push_xil_t,
        sizeof(push_xil_t) + max(__MaxSizeOf(xil_type_t), sizeof(uint16_t), sizeof(__ref_t))
    )

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const push_xil_t & xil)
    {
        __trace_write_xil(xil);
        return stream.write((const byte_t *)&xil, size_of(xil)), stream;
    }

    ////////// ////////// ////////// ////////// //////////

    /* pop_xil_t:
        xil_command_t::pop      : 4;
        xil_storage_type_t      : 4;
        xil_type_t dtype        : 4;
        uint8_t index           : 4;
    */

    __BeginXil(pop)

        pop_xil_t(xil_storage_type_t stype, xil_type_t dtype)
            : __super_t(xil_command_t::pop, stype), __dtype((byte_t)dtype) { }

        pop_xil_t(ref_t struct_type_ref)
            : __super_t(xil_command_t::pop, xil_storage_type_t::empty)
            , __dtype((byte_t)xil_type_t::empty)
        {
            set_type_ref(struct_type_ref);
        }

        byte_t     __dtype      : 4;
        byte_t     identity     : 4;

        byte_t __extra[0];

        xil_storage_type_t stype() const { return (xil_storage_type_t)cmd2; }
        xil_type_t dtype() const { return (xil_type_t)__dtype; }

        uint16_t get_identity() const
        {
            return identity < 15? identity : get_extra<uint16_t>();
        }

        void set_identity(uint16_t identity)
        {
            if(identity < 15)
            {
                this->identity = identity;
            }
            else
            {
                this->identity = 15;
                *(uint16_t *)__extra = identity;
            }
        }

        ref_t type_ref() const { return *(ref_t *)__extra; }
        void set_type_ref(ref_t type_ref) { *(ref_t *)__extra = type_ref; }

        ref_t field_ref() const { return *(ref_t *)__extra; }
        void set_field_ref(ref_t ref) { *(ref_t *)__extra = ref; }

        template<typename t> const t & get_extra() const
        {
            return *(t *)__extra;
        }

        operator string_t() const;

    __EndXil

    constexpr size_t size_of(const pop_xil_t & xil)
    {
        size_t size = 0;
        switch(xil.stype())
        {
            case xil_storage_type_t::empty:
                if(xil.dtype() == xil_type_t::empty)
                    size += sizeof(ref_t);
                break;

            case xil_storage_type_t::local:
            case xil_storage_type_t::argument:
                if(xil.identity == 15)
                    size += sizeof(uint16_t);
                break;

            case xil_storage_type_t::field:
                size += sizeof(__ref_t);
                break;

            default:
                break;
        }

        return size + sizeof(pop_xil_t);
    }

    __DefineMaxSize(pop_xil_t,
        sizeof(pop_xil_t) + max(__MaxSizeOf(xil_type_t), sizeof(uint16_t), sizeof(__ref_t))
    )

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const pop_xil_t & xil)
    {
        __trace_write_xil(xil);
        return stream.write((const byte_t *)&xil, size_of(xil)), stream;
    }

    ////////// ////////// ////////// ////////// //////////

    /* pick_xil_t:
        xil_command_t::pop      : 4;
        xil_storage_type_t      : 4;
        xil_type_t dtype        : 4;
        uint8_t index           : 4;
    */

    __BeginXil(pick)

        pick_xil_t(xil_storage_type_t stype, xil_type_t dtype)
            : __super_t(xil_command_t::pick, stype), __dtype((byte_t)dtype) { }

        byte_t     __dtype      : 4;
        byte_t     identity     : 4;

        byte_t __extra[0];

        xil_storage_type_t stype() const { return (xil_storage_type_t)cmd2; }
        xil_type_t dtype() const { return (xil_type_t)__dtype; }

        uint16_t get_identity() const
        {
            return identity < 15? identity : get_extra<uint16_t>();
        }

        void set_identity(uint16_t identity)
        {
            if(identity < 15)
            {
                this->identity = identity;
            }
            else
            {
                this->identity = 15;
                *(uint16_t *)__extra = identity;
            }
        }

        ref_t field_ref() const { return *(ref_t *)__extra; }
        void set_field_ref(ref_t ref) { *(ref_t *)__extra = ref; }

        template<typename t> const t & get_extra() const
        {
            return *(t *)__extra;
        }

        operator string_t() const;

    __EndXil

    constexpr size_t size_of(const pick_xil_t & xil)
    {
        size_t size = 0;
        switch(xil.stype())
        {
            case xil_storage_type_t::local:
            case xil_storage_type_t::argument:
                if(xil.identity == 15)
                    size += sizeof(uint16_t);
                break;

            case xil_storage_type_t::field:
                size += sizeof(__ref_t);
                break;

            default:
                break;
        }

        return size + sizeof(pick_xil_t);
    }

    __DefineMaxSize(pick_xil_t,
        sizeof(pick_xil_t) + max(__MaxSizeOf(xil_type_t), sizeof(uint16_t), sizeof(__ref_t))
    )

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const pick_xil_t & xil)
    {
        __trace_write_xil(xil);
        return stream.write((const byte_t *)&xil, size_of(xil)), stream;
    }

    ////////// ////////// ////////// ////////// //////////

    /* al_xil_t:
        xil_command_t::al   : 4;
        xil_al_command_t    : 4;
        xil_type_t dtype1   : 4;
        xil_type_t dtype2   : 4;
    */

    __BeginXil(al)

        al_xil_t(xil_al_command_t cmd, xil_type_t dtype1,
                                       xil_type_t dtype2 = xil_type_t::__default__)
            : __super_t(xil_command_t::al, cmd)
            , __dtype1((byte_t)dtype1), __dtype2((byte_t)dtype2) { }

        byte_t  __dtype1 : 4;
        byte_t  __dtype2 : 4;

        xil_type_t dtype1() const { return (xil_type_t)__dtype1; }
        xil_type_t dtype2() const { return (xil_type_t)__dtype2; }

        xil_al_command_t cmd() const { return (xil_al_command_t)this->cmd2; }

        operator string_t() const;

    __EndXil

    constexpr size_t size_of(const al_xil_t & xil)
    {
        return sizeof(al_xil_t);
    }

    __DefineMaxSize(al_xil_t, sizeof(al_xil_t));

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const al_xil_t & xil)
    {
        __trace_write_xil(xil);
        return stream.write((const byte_t *)&xil, size_of(xil)), stream;
    }

    ////////// ////////// ////////// ////////// //////////

    /* bit:
        xil_command_t::bit      : 4;
        xil_bit_command_t       : 4;
    */

    __BeginXil(bit)

        bit_xil_t(xil_bit_command_t cmd, xil_type_t dtype1,
                                         xil_type_t dtype2 = xil_type_t::__default__)
            : __super_t(xil_command_t::bit, cmd)
            , __dtype1((byte_t)dtype1), __dtype2((byte_t)dtype2) { }

        byte_t  __dtype1 : 4;
        byte_t  __dtype2 : 4;

        xil_type_t dtype1() const { return (xil_type_t)__dtype1; }
        xil_type_t dtype2() const { return (xil_type_t)__dtype2; }

        xil_bit_command_t cmd() const { return (xil_bit_command_t)this->cmd2; }

        operator string_t() const;

    __EndXil

    constexpr size_t size_of(const bit_xil_t & xil)
    {
        return sizeof(bit_xil_t);
    }

    __DefineMaxSize(bit_xil_t, sizeof(bit_xil_t));

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const bit_xil_t & xil)
    {
        __trace_write_xil(xil);
        return stream.write((const byte_t *)&xil, size_of(xil)), stream;
    }

    ////////// ////////// ////////// ////////// //////////

    /* logic:
        xil_command_t:logic     : 4;
        xil_logic_command_t     : 4;
    */

    __BeginXil(logic)

        logic_xil_t(xil_logic_command_t cmd)
            : __super_t(xil_command_t::logic, cmd) { }

        xil_logic_command_t cmd() const { return (xil_logic_command_t)this->cmd2; }

        operator string_t() const;

    __EndXil

    constexpr size_t size_of(const logic_xil_t & xil)
    {
        return sizeof(logic_xil_t);
    }

    __DefineMaxSize(logic_xil_t, sizeof(logic_xil_t));

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const logic_xil_t & xil)
    {
        __trace_write_xil(xil);
        return stream.write((const byte_t *)&xil, size_of(xil)), stream;
    }

    //-------- ---------- ---------- ---------- ----------

    /* cmp:
        xil_command_t::cmp  : 4;
        xil_cmp_command_t   : 4;
        xil_type_t dtype1   : 4;
        xil_type_t dtype2   : 4;
    */

    __BeginXil(cmp)

        cmp_xil_t(xil_cmp_command_t cmd, xil_type_t dtype1, xil_type_t dtype2)
            : __super_t(xil_command_t::cmp, cmd)
            , __dtype1((byte_t)dtype1), __dtype2((byte_t)dtype2) { }

        byte_t  __dtype1 : 4;
        byte_t  __dtype2 : 4;

        xil_type_t dtype1() const { return (xil_type_t)__dtype1; }
        xil_type_t dtype2() const { return (xil_type_t)__dtype2; }


        xil_cmp_command_t cmd() const { return (xil_cmp_command_t)this->cmd2; }

        operator string_t() const;

    __EndXil

    constexpr size_t size_of(const cmp_xil_t & xil)
    {
        return sizeof(cmp_xil_t);
    }

    __DefineMaxSize(cmp_xil_t, sizeof(cmp_xil_t));

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const cmp_xil_t & xil)
    {
        __trace_write_xil(xil);
        return stream.write((const byte_t *)&xil, size_of(xil)), stream;
    }

    //-------- ---------- ---------- ---------- ----------

    /* call:
        xil_command_t::call     : 4;
        xil_call_type_t         : 4;
        ref_t method            : 32;
    */

    __BeginXil(call)

        call_xil_t(xil_call_type_t call_type, ref_t method)
            : __super_t(xil_command_t::call, call_type), method(method) { }

        __ref_t method;

        xil_call_type_t call_type() const { return (xil_call_type_t)cmd2; }

        operator string_t() const;

    __EndXil

    constexpr size_t size_of(const call_xil_t & xil)
    {
        return sizeof(call_xil_t);
    }

    __DefineMaxSize(call_xil_t, sizeof(call_xil_t));

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const call_xil_t & xil)
    {
        __trace_write_xil(xil);
        return stream.write((const byte_t *)&xil, size_of(xil)), stream;
    }

    ////////// ////////// ////////// ////////// //////////

    /* simple
        xil_command_t::smp      : 4;
        xil_smp_t               : 4;
    */

    __BeginXil(smp)

        smp_xil_t(xil_smp_t smp)
            : __super_t(xil_command_t::smp, smp) { }

        xil_smp_t smp() const { return (xil_smp_t)cmd2; }

        operator string_t() const;

    __EndXil

    constexpr size_t size_of(const smp_xil_t & xil)
    {
        return sizeof(smp_xil_t);
    }

    __DefineMaxSize(xil_smp_t, 1)

    ////////// ////////// ////////// ////////// //////////

    /* jmp
        xil_command_t::jmp      : 4;
        xil_jmp_direction_t     : 2;
        xil_jmp_condition_t     : 2;
    */

    __BeginXil(jmp)

        jmp_xil_t(xil_jmp_condition_t condition, int step = 0)
            : __super_t(xil_command_t::jmp, 0)
        {
            set_condition(condition);
            set_step(step);
        }

        xil_jmp_direction_t direction() const
        {
            return (xil_jmp_direction_t)((byte_t)cmd2 >> 2);
        }

        void set_direction(xil_jmp_direction_t direction)
        {
            cmd2 = (((byte_t)cmd2 & 0x03) | ((byte_t)direction << 2));
        }

        xil_jmp_condition_t condition() const
        {
            return (xil_jmp_condition_t)((byte_t)cmd2 & 0x03);
        }

        void set_condition(xil_jmp_condition_t condition)
        {
            cmd2 = (((byte_t)cmd2 & 0x0C) | (byte_t)condition);
        }

        byte_t  __extra[0];

        void set_step(int32_t step);
        int32_t step() const;

        void set_tbl(int32_t index);
        int32_t tbl() const;

        operator string_t() const;

    __EndXil

    constexpr X_INLINE size_t jmp_xil_step_size(const jmp_xil_t & x)
    {
        if(x.condition() == xil_jmp_condition_t::switch_)
            return 1;

        switch(x.direction())
        {
            case xil_jmp_direction_t::backward:
            case xil_jmp_direction_t::forward:
                return 3;

            default:
                return 1;
        }
    }

    struct __jmp_xil_step_size_t
    {
        static size_t size_of(jmp_xil_t & x)
        {
            return jmp_xil_step_size(x);
        }
    };

    constexpr size_t size_of(const jmp_xil_t & xil)
    {
        return sizeof(jmp_xil_t) + jmp_xil_step_size(xil);
    }

    __DefineMaxSize(jmp_xil_t, 4)

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const jmp_xil_t & xil)
    {
        __trace_write_xil(xil);
        return stream.write((const byte_t *)&xil, size_of(xil)), stream;
    }

    ////////// ////////// ////////// ////////// //////////

    /* new
       xil_command_t::new_          : 4;
       xil_new_type_t               : 4;
       ref_t                        : 4;
    */

    __BeginXil(new)

        new_xil_t(xil_new_type_t new_type = xil_new_type_t::default_)
            : __super_t(xil_command_t::new_, new_type)
        { }

        __ref_t __type_ref;

        ref_t type_ref() const { return *(ref_t *)&__type_ref; }
        void set_type_ref(ref_t ref) { *(ref_t *)&__type_ref = ref; }

        xil_new_type_t new_type() const { return (xil_new_type_t)this->cmd2; }

        operator string_t() const;

    __EndXil

    constexpr size_t size_of(const new_xil_t & xil)
    {
        return sizeof(new_xil_t);
    }

    __DefineMaxSize(new_xil_t, sizeof(new_xil_t))

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const new_xil_t & xil)
    {
        __trace_write_xil(xil);
        return stream.write((const byte_t *)&xil, size_of(xil)), stream;
    }

    ////////// ////////// ////////// ////////// //////////

    template<typename _type_t>
    constexpr size_t max_size_of()
    {
        return __MaxSizeOf(_type_t);
    }

    ////////// ////////// ////////// ////////// //////////

    template<typename _xil_t>
    struct xil_extra_t : _xil_t
    {
        using _xil_t::_xil_t;
        byte_t extra[max_size_of<_xil_t>() - sizeof(_xil_t)];

        template<typename t> void set_extra(const t & value)
        {
            static_assert(sizeof(value) <= sizeof(extra), "value size too large");

            *(t *)extra = value;
        }
    };

    ////////// ////////// ////////// ////////// //////////

    class xil_reader_t : public object_t
    {
    public:
        xil_reader_t(const byte_t * bytes, size_t length)
            : __bytes(bytes), __bytes_end(bytes + length)
        {
            _A(bytes != nullptr);
        }

        xil_reader_t(const byte_t * bytes, const byte_t * bytes_end)
            : __bytes(bytes), __bytes_end(bytes_end)
        {
            _A(bytes != nullptr);
            _A(bytes_end != nullptr);
        }

        bool read(const xil_base_t ** out_xil);

    private:
        const byte_t * __bytes, * __bytes_end;
    };

    ////////// ////////// ////////// ////////// //////////

    class xil_pool_t;
    class xil_buffer_t;

    size_t write_to_buffer(xil_pool_t & pool, xil_buffer_t & buffer);

    X_INLINE xil_command_t command_of(const xil_t * xil)
    {
        return (xil_command_t)((const xil_base_t *)xil)->cmd1;
    }

    template<typename _cmd_t>
    X_INLINE _cmd_t sub_command_of(const xil_t * xil)
    {
        return (_cmd_t)((const xil_base_t *)xil)->cmd2;
    }

    X_INLINE bool is_label(const xil_t * xil)
    {
        return command_of(xil) == xil_command_t::smp
            && sub_command_of<xil_smp_t>(xil) == xil_smp_t::label;
    }

    ////////// ////////// ////////// ////////// //////////


    #undef __Cmd
    #undef __XilSize

    #undef __BeginXil
    #undef __EndXil

    #undef __Enum
    #undef __EnumEnd
    #undef __E

    #undef __DefineMaxSize
    #undef __MaxSizeOf

} } }

#endif  // __CORE_XIL_H__


