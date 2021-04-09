#ifndef __CORE_XIL_H__
#define __CORE_XIL_H__

namespace X_ROOT_NS { namespace modules { namespace core {

    #define __Enum(name)    X_ENUM_(name, sizeof(byte_t))
    #define __EnumEnd       X_ENUM_END
    #define __E(value)      ((int8_t)(value))

    #define CORE_TRACE_XIL_READ      0
    #define CORE_TRACE_XIL_WRITE     X_DEBUG

    ////////// ////////// ////////// ////////// //////////
    // xil_t

    // Xil commands
    __Enum(xil_command_t)

        // Empty xil.
        empty           = 0,

        // Pushes a value to stack.
        push            = 1,

        // Pops a value from stack and remove it.
        pop             = 2,

        // Picks a value to stack and don't remove it.
        pick            = 3,

        // Calls a function.
        call            = 4,

        // Jmps to a specified position.
        jmp             = 5,

        // Simple commands: e.g. return.
        smp             = 6,

        // Algorithm: + - * / etc.
        al              = 7,

        // Compare: > < == etc.
        cmp             = 8,

        // Logic: && || !
        logic           = 9,

        // Bit: & | ~
        bit             = 10,

        // Creates a object or a array.
        new_            = 11,

        // Extern flag.
        external        = 15,

        // Copy block.
        copy            = 15,

        // Initialize objects.
        init            = 16,

    __EnumEnd

    //-------- ---------- ---------- ---------- ----------

    // Algorithm command.
    __Enum(xil_al_command_t)

        empty           = 0,

        add             = 1,        // +
    
        sub             = 2,        // -

        mul             = 3,        // *

        div             = 4,        // /

        mod             = 5,        // %

        minus           = 6,        // -

        positive        = 7,        // +

    __EnumEnd

    //-------- ---------- ---------- ---------- ----------

    // Bit command.
    __Enum(xil_bit_command_t)

        empty           = 0,

        bit_and         = 1,        // &

        bit_or          = 2,        // |

        bit_not         = 3,        // ~

        bit_xor         = 4,        // ^

        left_shift      = 5,        // <<

        right_shift     = 6,        // >>

    __EnumEnd

    //-------- ---------- ---------- ---------- ----------

    // Compare command.
    __Enum(xil_cmp_command_t)

        empty           = 0,

        greater         = 1,        // >

        greater_equal   = 2,        // >=

        less            = 3,        // <

        less_equal      = 4,        // <=

        equal           = 5,        // ==

        not_equal       = 6,        // !=

    __EnumEnd

    //-------- ---------- ---------- ---------- ----------

    // Logic command.
    __Enum(xil_logic_command_t)

        empty           = 0,

        and_            = 1,        // &&

        or_             = 2,        // ||

        not_            = 3,        // !

    __EnumEnd

    //-------- ---------- ---------- ---------- ----------

    // Xil types
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

    // Converts a vtype to xil_type.
    xil_type_t to_xil_type(vtype_t vtype);

    //-------- ---------- ---------- ---------- ----------

    // Method calling type.
    __Enum(xil_call_type_t)

        empty           = 0,

        virtual_        = 1,        // Virtual call

        instance        = 2,        // Instance call

        static_         = 3,        // Static call

        internal        = 4,        // Internal call, defined in core library.

    __EnumEnd

    //-------- ---------- ---------- ---------- ----------

    __Enum(xil_smp_t)

        empty           = 0,

        label           = 1,        // Label

        ret             = 2,        // Return

        throw_          = 3,        // Throw exceptions.

        rethrow         = 4,        // Rethrow exceptions.

        end_finally     = 5,        // At the end of finally blocks.

    __EnumEnd

    //-------- ---------- ---------- ---------- ----------

    // Jmp distance.
    __Enum(xil_jmp_distance_t)

        near        = 0,            // Near jmp.

        far         = 1,            // Far jmp.

    __EnumEnd

    //-------- ---------- ---------- ---------- ----------

    // Jmp models.
    __Enum(xil_jmp_model_t)

        none    = __default__,      // Jmp

        true_   = 1,                // Jmp when true.

        false_  = 2,                // Jmp when false.

        switch_ = 3,                // Switch...case.

        leave   = 4,                // Leaves from protected codes. ( try block )

    __EnumEnd

    //-------- ---------- ---------- ---------- ----------

    __Enum(xil_storage_type_t)

        empty           = 0,

        local           = 1,        // Local variable.

        argument        = 2,        // Argument.

        field           = 3,        // Field

        constant        = 4,        // Constant

		params			= 13,		// Stack address of dynamic arguments

        array_element   = 14,       // Array element

        duplicate       = 15,       // Duplicate top unit of the stack.

    __EnumEnd

    //-------- ---------- ---------- ---------- ----------

    __Enum(xil_new_type_t)

        default_        = __default__,  // Creates a new object.

        array           = 1,            // Creates a new array.

    __EnumEnd

    //-------- ---------- ---------- ---------- ----------

    __Enum(xil_copy_type_t)

        block_copy      = __default__,  // Copy a block.

        stack_copy      = 1,            // Copy from stack.

        res_copy        = 2,            // Copy from resource.

    __EnumEnd

    //-------- ---------- ---------- ---------- ----------

    // init_xil_t

    __Enum(xil_init_type_t)

        array_begin     = __default__,  // Begin init array

        array_element   = 1,            // Init array element

        array_end       = 2,            // End init array

    __EnumEnd

    ////////// ////////// ////////// ////////// //////////

    class method_base_t;
    xil_call_type_t call_type_of_method(method_base_t * method);

    // Trace write xil, for debug.
    template<typename _xil_t>
    void __trace_write_xil(const _xil_t & xil)
    {
        //_P(_str(xil));
    }

    ////////// ////////// ////////// ////////// //////////
    // struct ref_t

    // Object reference.
    struct ref_iterator_t;
    struct ref_t : compare_operators_t<ref_t, uint32_t>
    {
        typedef ref_t                                   __self_t;
        typedef compare_operators_t<ref_t, uint32_t>    __super_t;

        typedef ref_t           value_type;
        typedef ref_iterator_t  iterator;

        // Constructors.
        ref_t() = default;
        constexpr explicit ref_t(uint32_t index, uint32_t count = 1, int32_t extra = 0)
            : index(index), extra(extra), count(count)
        { }

        uint32_t    index   : 20;
        uint32_t    extra   : 3;
        uint32_t    count   : 9;

        // Converts to a string.
        operator string_t() const;

        // Converts to a uint32 alue.
        operator uint32_t() const { return *(uint32_t *)this; }

        // Returns ref_t of specified offset.
        ref_t operator [] (size_t offset) const
        {
            return ref_t(index + offset, 1, extra);
        }

		// Returns ref_t of specified position.
		ref_t at(size_t index) const
		{
			return ref_t(index, 1, extra);
		}

        // Returns difference of two refs.
        int operator - (ref_t f) const { return index - f.index; }

		// Returns a new ref with specified offset.
		ref_t operator + (int offset) const { return ref_t(index + offset, 1, extra); }

        // Moves to next ref_t.
        ref_t & operator ++ () { index++; return *this; }

        // Moves to next ref_t.
        ref_t   operator ++ (int) { ref_t f = *this; index++; return f; }

        // Returns whether two refs are equals.
        bool operator == (ref_t f) const
        {
            return __super_t::operator == (f) || (count == 0 && f.count == 0);
        }

        // Returns whether two refs are not equals.
        bool operator != (ref_t f) const
        {
            return ! operator == (f);
        }

        // Returns size of a ref.
        size_t size() const { return count; }

        // Returns whether the ref is empty.
        bool empty() const { return count == 0; }

        // Returns the begin iterator.
        ref_iterator_t begin() const;

        // Returns the end iterator.
        ref_iterator_t end()   const;

        static const ref_t null;
    };

    // Returns reference iterator.
    struct ref_iterator_t : iterator_base_t<ref_iterator_t>
    {
        typedef ref_iterator_t __self_t;
        ref_iterator_t(const ref_t & ref) : __ref(ref) { }

        // Returns ref.
        ref_t operator * () const { return __ref; }

        // Move to a position by offset.
        void increase(int inc)
        {
            __ref.index += inc;
        }

        // Returns whether two iterators are equals.
        bool equals(const __self_t & it) const
        {
            return __ref == it.__ref;
        }

        ref_t __ref;
    };

    // Writes a ref to a stream.
    template<typename stream_t>
    stream_t & operator << (stream_t & stream, ref_t r)
    {
        static_assert(sizeof(ref_t) == sizeof(uint32_t), "sizeof(ref_t) must equals 32");
        stream << *(uint32_t *)&r;
    }

    // Reads ref from a stream.
    template<typename stream_t>
    stream_t & operator >> (stream_t & stream, ref_t & r)
    {
        static_assert(sizeof(ref_t) == sizeof(uint32_t), "sizeof(ref_t) must equals 32");
        stream >> *(uint32_t *)&r;
    }

    ////////// ////////// ////////// ////////// //////////
    // struct res_t

    // Ref of resource.
    struct res_t : compare_operators_t<res_t, uint32_t>
    {
        res_t() = default;
        constexpr explicit res_t(uint32_t pos) : pos(pos) { }

        uint32_t    pos;

        // Converts to a string.
        operator string_t() const;

        // Returns whether a res is empty.
        bool empty() const { return pos == 0; }

        // The empty res reference.
        static const res_t null;

        // Converts to a uint32 value.
        operator uint32_t() const { return *(uint32_t *)this; }
    };

    // Writes a res to a stream.
    template<typename stream_t>
    stream_t & operator << (stream_t & stream, res_t r)
    {
        static_assert(sizeof(res_t) == sizeof(uint32_t), "sizeof(res_t) must equals 32");
        stream << *(uint32_t *)&r;
    }

    // Reads a res from a stream.
    template<typename stream_t>
    stream_t & operator >> (stream_t & stream, res_t & r)
    {
        static_assert(sizeof(res_t) == sizeof(uint32_t), "sizeof(res_t) must equals 32");
        stream >> *(uint32_t *)&r;
    }

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        // Byte array.
        template<size_t size> struct __array_t
        {
            byte_t data[size];
        };

        // Descripts a object as a byte array.
        template<typename _obj_t>
        struct __array_object_t : __array_t<sizeof(_obj_t)>
        {
            typedef __array_t<sizeof(_obj_t)> __super_t;
            typedef __array_object_t<_obj_t>  __self_t;
            typedef _obj_t obj_t;

            using __super_t::__super_t;

            // Constructor
            __array_object_t(const _obj_t & obj)
            {
                *(_obj_t *)__super_t::data = obj;
            }

            // Returns the object.
            operator _obj_t & () const
            {
                return *(_obj_t *)__super_t::data;
            }

            // Assigns a object.
            __self_t & operator = (const _obj_t & obj)
            {
                *(_obj_t *)__super_t::data = obj;
                return *this;
            }
        };

        typedef __array_object_t<ref_t> __ref_t;    // Object reference.
        typedef __array_object_t<res_t> __res_t;    // Resource reference.
    }

    template<typename _obj_t> using array_object_t = __array_object_t<_obj_t>;

    ////////// ////////// ////////// ////////// //////////

    // Xil header.
    struct xil_header_t
    {
        // Constructor.
        template<typename cmd2_t>
        xil_header_t(xil_command_t cmd1, cmd2_t cmd2)
            : __cmd1((byte_t)cmd1), __cmd2((byte_t)cmd2)
        {
            _A((byte_t)cmd1 < (byte_t)xil_command_t::external);
        }

        // Constructor.
        xil_header_t(xil_command_t cmd1)
            : __cmd1((byte_t)cmd1 >= (byte_t)xil_command_t::external?
                            (byte_t)xil_command_t::external : (byte_t)cmd1)
            , __cmd2((byte_t)cmd1 >= (byte_t)xil_command_t::external?
                            (byte_t)cmd1 - (byte_t)xil_command_t::external : 0)
        { }

        // Returns command.
        xil_command_t command() const
        {
            if((xil_command_t)__cmd1 == xil_command_t::external)
                return (xil_command_t)(__cmd2 + ((byte_t)xil_command_t::external));

            return (xil_command_t)__cmd1;
        }

        // Data, used by sub classes.
        byte_t hdata() const { return __cmd2; }

        // Set data, used by sub classes.
        void set_hdata(byte_t d) { __cmd2 = d; }

    private:
        byte_t __cmd1    : 4;
        byte_t __cmd2    : 4;
    };

    //-------- ---------- ---------- ---------- ----------

    // Base class of xils.
    struct xil_base_t : xil_header_t, xil_t
    {
        template<typename ... cmds_t>
        xil_base_t(cmds_t ... cmds) : xil_header_t(cmds ...) { }
    };

    ////////// ////////// ////////// ////////// //////////

    // Defines size of entity.
    #define __XilSize(entity_t)  ((size_t)&((entity_t *)nullptr)->____end)

    // Begins a xil structure.
    #define __BeginXil(name)                                        \
        struct name##_xil_t : xil_base_t                            \
        {                                                           \
            typedef name##_xil_t __self_t;                          \
            typedef xil_base_t   __super_t;                         \
            name##_xil_t() = default;

    //   ...

    // Ends a xil structure.
    #define __EndXil                                                \
        };

    ////////// ////////// ////////// ////////// //////////

    template<typename _xil_t> struct __max_size_of_t { };

    // Defines max size of a type.
    #define __DefineMaxSize(_type_t, _max)              \
        template<> struct __max_size_of_t<_type_t>      \
    {                                                   \
        static constexpr size_t value()                 \
        {                                               \
            return _max;                                \
        }                                               \
    };

    // Defines max size of a type.
    #define __MaxSizeOf(_type_t)                        \
        (__max_size_of_t<_type_t>::value())

    // Returns size of a xil type.
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

    // Defines max size of xil type.
    __DefineMaxSize(xil_type_t, 8)

    ////////// ////////// ////////// ////////// //////////

    /* push_xil_t:
        xil_command_t::push     : 4;
        xil_storage_type_t      : 4;
        xil_type_t dtype        : 4;
        uint8_t index           : 4;
    */

    // Push xil.
    __BeginXil(push)

        // Constructor.
        push_xil_t(xil_storage_type_t stype, xil_type_t dtype = xil_type_t::empty)
            : __super_t(xil_command_t::push, stype), __dtype((byte_t)dtype) { }

        byte_t __dtype          : 4;        // xil_type_t
        byte_t identity         : 4;

        byte_t __extra[0];

        // Returns storage type.
        xil_storage_type_t stype() const { return (xil_storage_type_t)hdata(); }

        // Returns data type.
        xil_type_t dtype() const { return (xil_type_t)__dtype; }

        // Returns identity for local / param variables.
        uint16_t get_identity() const
        {
            return identity < 15? identity : get_extra<uint16_t>();
        }

        // Sets identity for local / param variables.
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

        // Returns field ref.
        ref_t field_ref() const { return *(ref_t *)__extra; }

        // Sets field ref.
        void set_field_ref(ref_t ref) { *(ref_t *)__extra = ref; }

        // Returns type ref.
        ref_t type_ref() const { return *(ref_t *)__extra; }

        // Sets type ref.
        void set_type_ref(ref_t type_ref) { *(ref_t *)__extra = type_ref; }

        // Returns extra data.
        template<typename t> const t & get_extra() const
        {
            return *(t *)__extra;
        }

        // Converts to a string.
        operator string_t() const;

    __EndXil

    //-------- ---------- ---------- ---------- ----------

    // Returns size of push_xil_t.
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
            case xil_storage_type_t::array_element:
                size += sizeof(__ref_t);
                break;

            case xil_storage_type_t::constant:
                if(xil.dtype() == xil_type_t::empty)    // generic
                    size += sizeof(__ref_t);            // generic type ref
                else
                    size += size_of(xil.dtype());
                break;

            case xil_storage_type_t::duplicate:
			case xil_storage_type_t::params:
                size -= 1;      // __dtype, identity no use
                break;

            default:
                break;
        }

        return size + sizeof(push_xil_t);
    }

    // Defines max size of push_xil_t.
    __DefineMaxSize(push_xil_t,
        sizeof(push_xil_t) + max(__MaxSizeOf(xil_type_t), sizeof(uint16_t), sizeof(__ref_t))
    )

    // Writes a push_xil_t to a stream.
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

    // Pop xil.
    __BeginXil(pop)

        // Constructors.
        pop_xil_t(xil_storage_type_t stype, xil_type_t dtype)
            : __super_t(xil_command_t::pop, stype), __dtype((byte_t)dtype) { }

        // Constructors.
        pop_xil_t(ref_t struct_type_ref)
            : __super_t(xil_command_t::pop, xil_storage_type_t::empty)
            , __dtype((byte_t)xil_type_t::empty)
        {
            set_type_ref(struct_type_ref);
        }

        byte_t     __dtype      : 4;
        byte_t     identity     : 4;

        byte_t __extra[0];

        // Returns storage type. local / param.
        xil_storage_type_t stype() const { return (xil_storage_type_t)hdata(); }

        // Returns data type.
        xil_type_t dtype() const { return (xil_type_t)__dtype; }

        // Returns identity for local / param variables.
        uint16_t get_identity() const
        {
            return identity < 15? identity : get_extra<uint16_t>();
        }

        // Sets identity form local / param variables.
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

        // Returns type ref.
        ref_t type_ref() const { return *(ref_t *)__extra; }

        // Sets type ref.
        void set_type_ref(ref_t type_ref) { *(ref_t *)__extra = type_ref; }

        // Returns field ref.
        ref_t field_ref() const { return *(ref_t *)__extra; }

        // Sets field ref.
        void set_field_ref(ref_t ref) { *(ref_t *)__extra = ref; }

        // Returns extra data.
        template<typename t> const t & get_extra() const
        {
            return *(t *)__extra;
        }

        // Converts to a string.
        operator string_t() const;

    __EndXil

    // Returns size of a pop_xil_t.
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
            case xil_storage_type_t::array_element:
                size += sizeof(__ref_t);
                break;

            default:
                break;
        }

        return size + sizeof(pop_xil_t);
    }

    // Defines max size of pop_xil_t.
    __DefineMaxSize(pop_xil_t,
        sizeof(pop_xil_t) + max(__MaxSizeOf(xil_type_t), sizeof(uint16_t), sizeof(__ref_t))
    )

    // Writes a pop_xil_t to a stream.
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

    // Pick xil.
    __BeginXil(pick)

        // Constructor.
        pick_xil_t(xil_storage_type_t stype, xil_type_t dtype)
            : __super_t(xil_command_t::pick, stype), __dtype((byte_t)dtype) { }

        byte_t     __dtype      : 4;
        byte_t     identity     : 4;

        byte_t __extra[0];

        // Returns storage type.
        xil_storage_type_t stype() const { return (xil_storage_type_t)hdata(); }

        // Returns data type.
        xil_type_t dtype() const { return (xil_type_t)__dtype; }

        // Returns identity for local / param variables.
        uint16_t get_identity() const
        {
            return identity < 15? identity : get_extra<uint16_t>();
        }

        // Sets identity for local / param variables.
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

        // Returns field ref.
        ref_t field_ref() const { return *(ref_t *)__extra; }

        // Sets field ref.
        void set_field_ref(ref_t ref) { *(ref_t *)__extra = ref; }

        // Returns type ref.
        ref_t type_ref() const { return *(ref_t *)__extra; }

        // Sets type ref.
        void set_type_ref(ref_t ref) { *(ref_t *)__extra = ref; }

        // Returns extra data.
        template<typename t> const t & get_extra() const
        {
            return *(t *)__extra;
        }

        // Converts to a string.
        operator string_t() const;

    __EndXil

    // Returns size of pick_xil_t.
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
            case xil_storage_type_t::array_element:
                size += sizeof(__ref_t);
                break;

            default:
                break;
        }

        return size + sizeof(pick_xil_t);
    }

    // Defines max size of pick_xil_t.
    __DefineMaxSize(pick_xil_t,
        sizeof(pick_xil_t) + max(__MaxSizeOf(xil_type_t), sizeof(uint16_t), sizeof(__ref_t))
    )

    // Writes pick_xil_t to a stream.
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

    // Algorithm command.
    __BeginXil(al)

        // Constructor.
        al_xil_t(xil_al_command_t cmd, xil_type_t dtype1,
                                       xil_type_t dtype2 = xil_type_t::__default__)
            : __super_t(xil_command_t::al, cmd)
            , __dtype1((byte_t)dtype1), __dtype2((byte_t)dtype2) { }

        byte_t  __dtype1 : 4;
        byte_t  __dtype2 : 4;

        // First data type.
        xil_type_t dtype1() const { return (xil_type_t)__dtype1; }

        // Second data type.
        xil_type_t dtype2() const { return (xil_type_t)__dtype2; }

        // Algorithm command.
        xil_al_command_t cmd() const { return (xil_al_command_t)this->hdata(); }

        // Converts to a string.
        operator string_t() const;

    __EndXil

    // Returns size of al_xil_t.
    constexpr size_t size_of(const al_xil_t & xil)
    {
        return sizeof(al_xil_t);
    }

    // Returns max size of al_xil_t.
    __DefineMaxSize(al_xil_t, sizeof(al_xil_t));

    // Writes a al_xil_t to a stream.
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

    // Bit xil.
    __BeginXil(bit)

        // Constructor.
        bit_xil_t(xil_bit_command_t cmd, xil_type_t dtype1,
                                         xil_type_t dtype2 = xil_type_t::__default__)
            : __super_t(xil_command_t::bit, cmd)
            , __dtype1((byte_t)dtype1), __dtype2((byte_t)dtype2) { }

        byte_t  __dtype1 : 4;
        byte_t  __dtype2 : 4;

        // The first data type.
        xil_type_t dtype1() const { return (xil_type_t)__dtype1; }

        // The second data type.
        xil_type_t dtype2() const { return (xil_type_t)__dtype2; }

        // Algorighm cmmand.
        xil_bit_command_t cmd() const { return (xil_bit_command_t)this->hdata(); }

        // Converts to a string.
        operator string_t() const;

    __EndXil

    // Returns sizeof a bit_xil_t.
    constexpr size_t size_of(const bit_xil_t & xil)
    {
        return sizeof(bit_xil_t);
    }

    // Defines max size of bit_xil_t.
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

    // Logic xil.
    __BeginXil(logic)

        // Constructor.
        logic_xil_t(xil_logic_command_t cmd)
            : __super_t(xil_command_t::logic, cmd) { }

        // Logic command.
        xil_logic_command_t cmd() const { return (xil_logic_command_t)this->hdata(); }

        // Converts to a string.
        operator string_t() const;

    __EndXil

    // Returns size of a logic xil.
    constexpr size_t size_of(const logic_xil_t & xil)
    {
        return sizeof(logic_xil_t);
    }

    // Returns max size of logic xil.
    __DefineMaxSize(logic_xil_t, sizeof(logic_xil_t));

    // Writes a logic_xil_t to a stream.
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

    // Compare xil.
    __BeginXil(cmp)

        // Constructor.
        cmp_xil_t(xil_cmp_command_t cmd, xil_type_t dtype1, xil_type_t dtype2)
            : __super_t(xil_command_t::cmp, cmd)
            , __dtype1((byte_t)dtype1), __dtype2((byte_t)dtype2) { }

        byte_t  __dtype1 : 4;
        byte_t  __dtype2 : 4;

        // The first data type.
        xil_type_t dtype1() const { return (xil_type_t)__dtype1; }

        // The second data type.
        xil_type_t dtype2() const { return (xil_type_t)__dtype2; }

        // Command.
        xil_cmp_command_t cmd() const { return (xil_cmp_command_t)this->hdata(); }

        // Converts to a string.
        operator string_t() const;

    __EndXil

    // Returns size of a cmp_xil_t.
    constexpr size_t size_of(const cmp_xil_t & xil)
    {
        return sizeof(cmp_xil_t);
    }

    // Defines max size of cmp_xil_t.
    __DefineMaxSize(cmp_xil_t, sizeof(cmp_xil_t));

    // Writes a cmp_xil_t to a stream.
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

    // Method call xil.
    __BeginXil(call)

        // Constructor.
        call_xil_t(xil_call_type_t call_type, ref_t method)
            : __super_t(xil_command_t::call, call_type), method(method) { }

        __ref_t method;

        // Call type.
        xil_call_type_t call_type() const { return (xil_call_type_t)hdata(); }

        // Converts to a string.
        operator string_t() const;

    __EndXil

    // Returns size of a call_xil_t.
    constexpr size_t size_of(const call_xil_t & xil)
    {
        return sizeof(call_xil_t);
    }

    // Returns max size of call_xil_t.
    __DefineMaxSize(call_xil_t, sizeof(call_xil_t));

    // Writes a call_xil_t to a stream.
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

    // Simple xil.
    __BeginXil(smp)

        // Constructor.
        smp_xil_t(xil_smp_t smp)
            : __super_t(xil_command_t::smp, smp) { }

        // Command.
        xil_smp_t smp() const { return (xil_smp_t)hdata(); }

        // Converts to a string.
        operator string_t() const;

    __EndXil

    // Returns size of smp_xil_t.
    constexpr size_t size_of(const smp_xil_t & xil)
    {
        return sizeof(smp_xil_t);
    }

    // Defines max size of xil_smp_t.
    __DefineMaxSize(xil_smp_t, 1)

    ////////// ////////// ////////// ////////// //////////

    /* jmp
        xil_command_t::jmp      : 4;
        xil_jmp_direction_t     : 1;
        xil_jmp_model_t         : 3;
    */

    typedef int32_t xil_jmp_step_t;

    // Jmp xil.
    __BeginXil(jmp)

        // Constructor.
        jmp_xil_t(xil_jmp_model_t model, xil_jmp_step_t step = 0)
            : __super_t(xil_command_t::jmp, 0)
        {
            set_model(model);
            set_step(step);
        }

        // Returns jmp distance.
        xil_jmp_distance_t distance() const
        {
            return (xil_jmp_distance_t)((byte_t)hdata() >> 3);
        }

        // Sets jmp distance.
        void set_distance(xil_jmp_distance_t distance)
        {
            set_hdata((((byte_t)hdata() & 0x07) | ((byte_t)distance << 3)));
        }

        // Returns jmp model.
        xil_jmp_model_t model() const
        {
            return (xil_jmp_model_t)((byte_t)hdata() & 0x07);
        }

        // Sets jmp model.
        void set_model(xil_jmp_model_t model)
        {
            set_hdata((((byte_t)hdata() & 0x08) | (byte_t)model));
        }

        byte_t  __extra[0];

        // Sets step.
        void set_step(xil_jmp_step_t step);

        // Returns step.
        xil_jmp_step_t step() const;

        // Sets switch-case table index.
        void set_tbl(int32_t index);

        // Returns switch-case table index.
        int32_t tbl() const;

        // Converts to a string.
        operator string_t() const;

    __EndXil

    // Returns jmp xil step size.
    constexpr X_INLINE size_t jmp_xil_step_size(const jmp_xil_t & x)
    {
        if(x.model() == xil_jmp_model_t::switch_)
            return 1;

        if(x.distance() == xil_jmp_distance_t::far)
            return sizeof(xil_jmp_step_t) - 1;

        return 1;
    }

    // Jmp xil step size.
    struct __jmp_xil_step_size_t
    {
        static size_t size_of(jmp_xil_t & x)
        {
            return jmp_xil_step_size(x);
        }
    };

    // Returns size of jmp xil.
    constexpr size_t size_of(const jmp_xil_t & xil)
    {
        return sizeof(jmp_xil_t) + jmp_xil_step_size(xil);
    }

    // Defines jmp_xil_t.
    __DefineMaxSize(jmp_xil_t, 4)

    // Writes a jmp_xil_t to a stream.
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

    // New xil.
    __BeginXil(new)

        // Constructor.
        new_xil_t(xil_new_type_t new_type = xil_new_type_t::default_)
            : __super_t(xil_command_t::new_, new_type)
        { }

        __ref_t __type_ref;

        // Returns type_ref.
        ref_t type_ref() const { return *(ref_t *)&__type_ref; }

        // Sets type_ref.
        void set_type_ref(ref_t ref) { *(ref_t *)&__type_ref = ref; }

        // New type type.
        xil_new_type_t new_type() const { return (xil_new_type_t)this->hdata(); }

        // Converts to a string.
        operator string_t() const;

    __EndXil

    // Return size of a new_xil_t.
    constexpr size_t size_of(const new_xil_t & xil)
    {
        return sizeof(new_xil_t);
    }

    // Defines max size of new_xil_t.
    __DefineMaxSize(new_xil_t, sizeof(new_xil_t))

    // Writes new_xil_t to a stream.
    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const new_xil_t & xil)
    {
        __trace_write_xil(xil);
        return stream.write((const byte_t *)&xil, size_of(xil)), stream;
    }

    ////////// ////////// ////////// ////////// //////////
    // copy_xil_t

    // Copy xil.
    __BeginXil(copy)

        // Constructors.
        copy_xil_t(xil_copy_type_t copy_type, xil_type_t dtype)
            : __super_t(xil_command_t::copy)
            , __copy_type((byte_t)copy_type), __dtype((byte_t)dtype)
        { }

        // Returns copy type.
        xil_copy_type_t copy_type() const { return (xil_copy_type_t)__copy_type; }

        // Returns data type.
        xil_type_t dtype() const { return (xil_type_t)__dtype; }

        byte_t  __copy_type : 4;
        byte_t  __dtype     : 4;

        byte_t __extra[0];

        // Returns res.
        res_t res() const { return *(res_t *)__extra; }

        // Sets res.
        void set_res(res_t res) { *(res_t *)__extra = res; }

        // Converts to a string.
        operator string_t() const;

    __EndXil

    // Returns size of copy_xil_t.
    constexpr size_t size_of(const copy_xil_t & xil)
    {
        size_t size = sizeof(copy_xil_t);

        switch(xil.copy_type())
        {
            case xil_copy_type_t::stack_copy:
                break;

            case xil_copy_type_t::block_copy:
                break;

            case xil_copy_type_t::res_copy:
                size += sizeof(res_t);
                break;

            default:
                break;
        }

        return size;
    }

    // Defines max size of copy_xil_t.
    __DefineMaxSize(copy_xil_t, sizeof(copy_xil_t) + sizeof(res_t))

    // Writes copy_xil_t to a stream.
    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const copy_xil_t & xil)
    {
        __trace_write_xil(xil);
        return stream.write((const byte_t *)&xil, size_of(xil)), stream;
    }

    ////////// ////////// ////////// ////////// //////////

    // Init xil.
    __BeginXil(init)

        // Constructors.
        init_xil_t(xil_init_type_t init_type, xil_type_t dtype)
            : __super_t(xil_command_t::init)
            , __init_type((byte_t)init_type), __dtype((byte_t)dtype)
        { }

        // Returns init type.
        xil_init_type_t init_type() const { return (xil_init_type_t)__init_type; }

        // Returns data type.
        xil_type_t dtype() const { return (xil_type_t)__dtype; }

        byte_t  __init_type : 4;
        byte_t  __dtype     : 4;

        byte_t __extra[0];

        // Returns type ref.
        ref_t type_ref() const { return *(ref_t *)__extra; }

        // Sets type ref.
        void set_type_ref(ref_t ref) { *(ref_t *)__extra = ref; }

        // Converts to a string.
        operator string_t() const;

    __EndXil

    // Returns size of init_xil_t.
    constexpr size_t size_of(const init_xil_t & xil)
    {
        size_t size = sizeof(init_xil_t);

        if(xil.dtype() == xil_type_t::empty)
            size += sizeof(ref_t);

        return size;
    }

    // Defines max size of init_xil_t.
    __DefineMaxSize(init_xil_t, sizeof(init_xil_t) + sizeof(ref_t))

    // Writes init_xil_t to a stream.
    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const init_xil_t & xil)
    {
        __trace_write_xil(xil);
        return stream.write((const byte_t *)&xil, size_of(xil)), stream;
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns max size of a type.
    template<typename _type_t>
    constexpr size_t max_size_of()
    {
        return __MaxSizeOf(_type_t);
    }

    ////////// ////////// ////////// ////////// //////////

    // Xil extra.
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

    // Xil reader.
    class xil_reader_t : public object_t
    {
    public:

        // Constructor.
        xil_reader_t(const byte_t * bytes, size_t length)
            : __bytes(bytes), __bytes_end(bytes + length)
        {
            _A(bytes != nullptr);
        }

        // Constructor.
        xil_reader_t(const byte_t * bytes, const byte_t * bytes_end)
            : __bytes(bytes), __bytes_end(bytes_end)
        {
            _A(bytes != nullptr);
            _A(bytes_end != nullptr);
        }

        // Read next xil.
        bool read(const xil_base_t ** out_xil);

    private:
        const byte_t * __bytes, * __bytes_end;
    };

    ////////// ////////// ////////// ////////// //////////

    class xil_pool_t;
    class xil_buffer_t;

    // Xil buffer writer.
    class xil_buffer_writer_t : public object_t
    {
    public:

        // Constructor.
        xil_buffer_writer_t(xpool_t & xpool, xil_buffer_t & buffer, method_t * method);

        // Writes to buffer.
        size_t write(xil_pool_t & pool);

        // Returns total size.
        size_t size() const { return __size; }

        // Returns whether it's empty.
        bool empty() const { return __size == 0; }

    private:
        xil_buffer_t &  __buffer;
        method_t *      __method;
        size_t          __size = 0;
        xpool_t &       __xpool;

        #if CORE_TRACE_XIL_WRITE

        enum class __trace_type_t { unknown, trace, no_trace };
        __trace_type_t __trace_type = __trace_type_t::unknown;

        bool __traced_method = false;

        // Prints trace message.
        void __trace_method();

        // Returns trace type.
        __trace_type_t __get_trace_type();

        #endif
    };

    // Returns command of a xil.
    X_INLINE xil_command_t command_of(const xil_t * xil)
    {
        return (xil_command_t)((const xil_base_t *)xil)->command();
    }

    // Returns sub command of a xil.
    template<typename _cmd_t>
    X_INLINE _cmd_t sub_command_of(const xil_t * xil)
    {
        return (_cmd_t)((const xil_base_t *)xil)->hdata();
    }

    // Returns whether the xil is a label.
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


