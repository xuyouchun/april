#include "commands.h"
#include <rtlib.h>
#include <mm.h>
#include "utils.h"
#include "parser.h"

namespace X_ROOT_NS { namespace modules { namespace exec {

    #define __AlwaysInline  X_ALWAYS_INLINE
    #define __Mv(x)         std::move(x)

    #define __BeginExecute(_ctx, _cmd_value)                                    \
                                                                                \
        static const cmd_value_t cmd_value = (cmd_value_t)_cmd_value;           \
                                                                                \
        virtual cmd_value_t get_cmd_value() override                            \
        {                                                                       \
            return cmd_value;                                                   \
        }                                                                       \
                                                                                \
        __AlwaysInline void do_execute(command_execute_context_t & _ctx)        \
        {

    #define __EndExecute()                                                      \
        }                                                                       \
                                                                                \
        virtual void execute(command_execute_context_t & ctx) override          \
        {                                                                       \
            do_execute(ctx);                                                    \
        }

    #define __ToCmdValue(_cmd, value)                                           \
        ( ((cmd_value_t)(xil_command_t::_cmd) << 24) | (cmd_value_t)(value) )

    ////////// ////////// ////////// ////////// //////////

    typedef command_creating_context_t __context_t;

    using namespace rt;
    using namespace rtlib;

    typedef int __nokey_t;
    const __nokey_t __nokey = 0;

    typedef rt_stack_unit_t __unit_t;
    const msize_t __stack_stub_size = unit_size_of(sizeof(__calling_stub_t));

    template<typename _cmd_t, typename ... _args_t>
    _cmd_t * __new_command(memory_t * memory, _args_t && ... args)
    {
        _cmd_t * cmd = memory_t::new_obj<_cmd_t>(memory, std::forward<_args_t>(args) ...);
        ((command_t *)cmd)->cmd_value = _cmd_t::cmd_value;

        return cmd;
    }

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        template<xil_type_t _xil_type> struct __vnum_traits_t
        {
            typedef vnum_t<to_vtype(_xil_type)> num_t;
        };

        template<> struct __vnum_traits_t<xil_type_t::string>
        {
            typedef rt_ref_t num_t;
        };

        template<> struct __vnum_traits_t<xil_type_t::object>
        {
            typedef rt_ref_t num_t;
        };
    }

    template<xil_type_t _xil_type>
    using __vnum_t = typename __vnum_traits_t<_xil_type>::num_t;

    ////////// ////////// ////////// ////////// //////////

    static al::xheap_t __command_heap;

    template<typename t, typename _xil_t> t __read_extra(const _xil_t & xil)
    {
        typedef const xil_extra_t<_xil_t> exxil_t;
        return *(t *)((const exxil_t &)xil).extra;
    }

    typedef msize_t __offset_t;

    ////////// ////////// ////////// ////////// //////////

    msize_t command_creating_context_t::ret_unit_size()
    {
        if(__ret_unit_size == unknown_msize)
        {
            msize_t size = variable_size_of((*method)->type, &__ret_storage_type);
            __ret_unit_size = unit_size_of(size);
        }

        return __ret_unit_size;
    }

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        template<bool _first_is_nokey, typename ... _args_t>
        struct __command_key_impl_t : std::tuple<_args_t ...>
        {
            typedef std::tuple<_args_t ...> __super_t;

            template<typename ... _targs_t>
            __command_key_impl_t(_targs_t && ... args)
                : __super_t(std::forward<_args_t>(args) ...)
            { }
        };

        template<typename ... _args_t>
        struct __command_key_impl_t<true, _args_t ...> : std::tuple<_args_t ...>
        {
            typedef std::tuple<_args_t ...> __super_t;

            template<typename ... _targs_t>
            __command_key_impl_t(__nokey_t, _targs_t && ... args)
                : __super_t(std::forward<_args_t>(args) ...)
            { }
        };
    }

    template<typename ... _args_t>
    using __command_key_t = __command_key_impl_t<
        std::is_same<typeat<0, _args_t ...>, __nokey_t>::value,
        _args_t ...
    >;

    //-------- ---------- ---------- ---------- ----------

    template<typename _command_template_t, typename ... _template_keys_t>
    class __command_manager_t : public object_t
    {
    public:
        template<typename ... args_t> class with_args_t
        {
            typedef __command_key_t<_template_keys_t ..., args_t ...> __key_t;

        public:
            with_args_t(memory_t * memory) : __memory(memory) { }
            with_args_t() : with_args_t(&__command_heap) { }

            template<_template_keys_t ... keys, typename ... others_t>
            command_t * get_command(args_t && ... args, others_t && ... others)
            {
                __key_t key(keys ..., args ...);
                auto it = __map.find(key);
                if(it != __map.end())
                    return it->second;

                command_t * command = _command_template_t::template new_command<keys ...>(
                    __memory, std::forward<args_t>(args) ..., std::forward<others_t>(others) ...
                );

                __map[key] = command;
                return command;
            }

            template<typename ... _others_t>
            command_t * get_command(_others_t && ... others)
            {
                return get_command<__nokey_t, _others_t ...>(
                    std::forward<_others_t>(others) ...
                );
            }

        private:
            std::map<__key_t, command_t *> __map;
            memory_t * __memory;
        };
    };

    ////////// ////////// ////////// ////////// //////////

    class __command_base_t : public command_t
    {
    public:
        virtual const string_t to_string(command_execute_context_t & ctx) const override
        {
            return _T("[command]");
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // Class __empty_command_t

    class __empty_command_t : public __command_base_t
    {
    public:
        __BeginExecute(ctx, __ToCmdValue(empty, 0))

            // Do nothing

        __EndExecute()

        virtual const string_t to_string(command_execute_context_t & ctx) const override
        {
            return _T("[empty]");
        }
    };

    __empty_command_t __empty_command;

    ////////// ////////// ////////// ////////// //////////
    // Class __execute_end_command_t

    class __execute_end_command_t : public __command_base_t
    {
    public:
        __execute_end_command_t()
        {
            __command_base_t::cmd_value = cmd_value;
        }

        __BeginExecute(ctx, 0)

            throw _E(exec_env_code_t::end);

        __EndExecute()

        virtual const string_t to_string(command_execute_context_t & ctx) const override
        {
            return _T("[end]");
        }
    };

    __execute_end_command_t __execute_end_command;

    ////////// ////////// ////////// ////////// //////////
    // Class __push_command_t

    template<xil_storage_type_t, xil_type_t>
    class __push_command_t { };

    struct __push_command_template_t
    {
        template<xil_storage_type_t stype, xil_type_t dtype, typename ... args_t>
        static auto new_command(memory_t * memory, args_t && ... args)
        {
            typedef __push_command_t<stype, dtype> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    namespace
    {
        template<xil_type_t _xil_type>
        struct __const_vnum_traits_t : __vnum_traits_t<_xil_type> { };

        template<> struct __const_vnum_traits_t<xil_type_t::string>
        {
            typedef const char_t * num_t;
        };

        template<> struct __const_vnum_traits_t<xil_type_t::object>
        {
            typedef void * num_t;
        };
    }

    template<xil_type_t _xil_type>
    using __const_vnum_t = typename __const_vnum_traits_t<_xil_type>::num_t;

    //-------- ---------- ---------- ---------- ----------

    #define __Local(type_t, offset)    *(type_t *)((byte_t *)ctx.stack.lp() + offset)

    #define __ToPushCmdValue(_stype, _xil_type)                                     \
        __ToCmdValue(push,                                                          \
            ((cmd_value_t)xil_storage_type_t::_stype << 8) | (cmd_value_t)_xil_type \
        )

    template<xil_type_t _xil_type>
    class __push_command_t<xil_storage_type_t::constant, _xil_type>
        : public __command_base_t
    {
        typedef __command_base_t            __super_t;
        typedef __const_vnum_t<_xil_type>   __value_t;

    public:
        __push_command_t(__value_t value) : __value(value) { }

        __BeginExecute(ctx, __ToPushCmdValue(constant, _xil_type))

            ctx.stack.push(__value);

        __EndExecute()

        virtual const string_t to_string(command_execute_context_t & ctx) const override
        {
            return _F(_T("push constant %1%"), __value);
        }

    private:
        __value_t __value;
    };

    // define other types ...

    #undef __BeginConstantPushCommand
    #undef __EndConstantPushCommand
    #undef __ConstantPushConstantCommand

    //-------- ---------- ---------- ---------- ----------

    #define __BeginPushCommand(s, d, v_t)                                       \
        template<> class __push_command_t<xil_storage_type_t::s, xil_type_t::d> \
            : public __command_base_t                                           \
        {                                                                       \
            typedef v_t     __value_t;                                          \
                                                                                \
        public:                                                                 \
            __push_command_t(__offset_t offset) : __offset(offset) { }          \
                                                                                \
            virtual const string_t to_string(command_execute_context_t & ctx) const override \
            {                                                                   \
                return _F(_T("push %1% %2%"), xil_storage_type_t::s, __offset); \
            }                                                                   \
                                                                                \
            __BeginExecute(ctx, __ToPushCmdValue(s, xil_type_t::d))

                // Method Body

    #define __EndPushCommand()                                                  \
            __EndExecute()                                                      \
                                                                                \
        private:                                                                \
            __offset_t __offset;                                                \
        };


    #define __LocalPushCommand(d, v_t)                                          \
        __BeginPushCommand(local, d, v_t)                                       \
            /*_PF(_T("PUSH %1% (%2%)"), __Local(__value_t, __offset), __offset);*/  \
            ctx.stack.push(__Local(__value_t, __offset));                       \
        __EndPushCommand()

    __LocalPushCommand(int8,      int8_t)
    __LocalPushCommand(uint8,     uint8_t)

    __LocalPushCommand(int16,     int16_t)
    __LocalPushCommand(uint16,    uint16_t)

    __LocalPushCommand(int32,     int32_t)
    __LocalPushCommand(uint32,    uint32_t)

    __LocalPushCommand(int64,     int64_t)
    __LocalPushCommand(uint64,    uint64_t)

    __LocalPushCommand(float_,    float_t)
    __LocalPushCommand(double_,   double_t)

    __LocalPushCommand(char_,     char_t)
    __LocalPushCommand(bool_,     bool_t)

    __BeginPushCommand(local, object, rt_ref_t)
        ctx.stack.push(__Local(rt_ref_t, __offset));
    __EndPushCommand()

    __BeginPushCommand(local, string, rt_ref_t)
        ctx.stack.push(__Local(rt_ref_t, __offset));
    __EndPushCommand()

    #undef __LocalPushCommand

    //-------- ---------- ---------- ---------- ----------

    #define __Argument(type_t, offset) *(type_t *)((rt_stack_unit_t *)ctx.stack.lp() - (offset))

    #define __BeginArgumentPushCommand(d, v_t)                                  \
        template<> class __push_command_t<xil_storage_type_t::argument, xil_type_t::d> \
            : public __command_base_t                                           \
        {                                                                       \
            typedef v_t     __value_t;                                          \
                                                                                \
        public:                                                                 \
            __push_command_t(__offset_t offset) : __offset(offset) { }          \
                                                                                \
            virtual const string_t to_string(command_execute_context_t & ctx) const override \
            {                                                                   \
                return _F(_T("push argument %1%"), __offset);                   \
            }                                                                   \
                                                                                \
            __BeginExecute(ctx, __ToPushCmdValue(argument, xil_type_t::d))

                // Method Body

    #define __EndArgumentPushCommand()                                          \
            __EndExecute()                                                      \
                                                                                \
        private:                                                                \
            __offset_t __offset;                                                \
        };


    #define __ArgumentPushCommand(d, v_t)                                       \
        __BeginArgumentPushCommand(d, v_t)                                      \
            ctx.stack.push(__Argument(__value_t, __offset + __stack_stub_size));  \
        __EndArgumentPushCommand()

    //-------- ---------- ---------- ---------- ----------

    __ArgumentPushCommand(int8,      int8_t)
    __ArgumentPushCommand(uint8,     uint8_t)

    __ArgumentPushCommand(int16,     int16_t)
    __ArgumentPushCommand(uint16,    uint16_t)

    __ArgumentPushCommand(int32,     int32_t)
    __ArgumentPushCommand(uint32,    uint32_t)

    __ArgumentPushCommand(int64,     int64_t)
    __ArgumentPushCommand(uint64,    uint64_t)

    __ArgumentPushCommand(float_,    float_t)
    __ArgumentPushCommand(double_,   double_t)

    __ArgumentPushCommand(char_,     char_t)
    __ArgumentPushCommand(bool_,     bool_t)

    __BeginPushCommand(argument, object, rt_ref_t)
        ctx.stack.push(__Argument(rt_ref_t, __offset + __stack_stub_size));
    __EndPushCommand()

    __BeginPushCommand(argument, string, rt_ref_t)
        ctx.stack.push(__Argument(rt_ref_t, __offset + __stack_stub_size));
    __EndPushCommand()

    #undef __ArgumentPushCommand

    //-------- ---------- ---------- ---------- ----------

    static msize_t __field_offset(__context_t & ctx, ref_t field_ref)
    {
        return ctx.get_field_offset(field_ref); // + sizeof(rt_type_t *);
    }

    #define __PField(type_t, p, offset)     ((type_t *)((byte_t *)p + offset))
    #define __Field(type_t, p, offset)      (*__PField(type_t, p, offset))

    #define __BeginFieldPushCommand(d, v_t)                                     \
        template<> class __push_command_t<xil_storage_type_t::field, xil_type_t::d> \
            : public __command_base_t                                           \
        {                                                                       \
            typedef v_t     __value_t;                                          \
                                                                                \
        public:                                                                 \
            __push_command_t(__offset_t offset) : __offset(offset) { }          \
                                                                                \
            virtual const string_t to_string(command_execute_context_t & ctx) const override \
            {                                                                   \
                return _F(_T("push field %1%"), __offset);                      \
            }                                                                   \
                                                                                \
            __BeginExecute(ctx, __ToPushCmdValue(field, xil_type_t::d))

                // Method Body

    #define __EndFieldPushCommand()                                             \
            __EndExecute()                                                      \
                                                                                \
        private:                                                                \
            __offset_t __offset;                                                \
        };


    #define __FieldPushCommand(d, v_t)                                          \
        __BeginFieldPushCommand(d, v_t)                                         \
            ctx.stack.push(__Field(__value_t, ctx.stack.pop<rt_ref_t>(), __offset)); \
        __EndFieldPushCommand()

    __FieldPushCommand(int8,      int8_t)
    __FieldPushCommand(uint8,     uint8_t)

    __FieldPushCommand(int16,     int16_t)
    __FieldPushCommand(uint16,    uint16_t)

    __FieldPushCommand(int32,     int32_t)
    __FieldPushCommand(uint32,    uint32_t)

    __FieldPushCommand(int64,     int64_t)
    __FieldPushCommand(uint64,    uint64_t)

    __FieldPushCommand(float_,    float_t)
    __FieldPushCommand(double_,   double_t)

    __FieldPushCommand(char_,     char_t)
    __FieldPushCommand(bool_,     bool_t)

    __BeginPushCommand(field, object, rt_ref_t)
        ctx.stack.push(__Field(rt_ref_t, ctx.stack.pop<rt_ref_t>(), __offset));
    __EndPushCommand()

    __BeginPushCommand(field, string, rt_ref_t)
        ctx.stack.push(__Field(rt_ref_t, ctx.stack.pop<rt_ref_t>(), __offset));
    __EndPushCommand()

    #undef __FieldPushCommand

    //-------- ---------- ---------- ---------- ----------

    class __duplicate_command_t : public __command_base_t
    {
    public:
        __BeginExecute(ctx, __ToPushCmdValue(duplicate, 0))

            ctx.stack.push<rt_stack_unit_t>(
                ctx.stack.pick<rt_stack_unit_t>()
            );

        __EndExecute()

        virtual const string_t to_string(command_execute_context_t & ctx) const override
        {
            return _T("duplicate");
        }

    } __duplicate_command;

    //-------- ---------- ---------- ---------- ----------

    #define __RtTypeOf(rt_ref)  (*((rt_type_t **)(void *)rt_ref - 1))

    template<xil_type_t _xil_type>
    class __push_command_t<xil_storage_type_t::array_element, _xil_type>
        : public __command_base_t
    {
        typedef __command_base_t __super_t;

    public:
        __push_command_t(dimension_t dimension) : __dimension(dimension) { }

        virtual const string_t to_string(command_execute_context_t & ctx) const override
        {
            return _F(_T("push array element (%1%)"), _xil_type);
        }

        __BeginExecute(ctx, __ToPushCmdValue(array_element, _xil_type))

            rt_ref_t array_ref = ctx.stack.pop<rt_ref_t>(); 
            array_length_t index;

            if(__dimension == 1)
            {
                //array_length_t length = *mm::get_array_lengths(array_ref);
                array_length_t index  = ctx.stack.pop<array_length_t>();
                typedef __vnum_t<_xil_type> element_t;

                ctx.stack.push<element_t>(
                    mm::get_array_element<element_t>(array_ref, index)
                );
            }
            else
            {
                X_UNEXPECTED();
                /*
                array_length_t * p_length = (array_length_t *)((byte_t *)obj - 1);

                for(dimension_t dimension = 1; dimension < __dimension; dimension++)
                {
                    index *= *--p_length;
                }
                */
            }

        __EndExecute()

    private:
        dimension_t __dimension;
    };

    //-------- ---------- ---------- ---------- ----------

    static dimension_t __array_dimension(__context_t & ctx, ref_t type_ref)
    {
        rt_type_t * rt_type = ctx.get_type(type_ref);
        _A(rt_type->get_kind() == rt_type_kind_t::array);

        rt_array_type_t * arr_type = (rt_array_type_t *)rt_type;

        return (*arr_type)->dimension;
    }

    static command_t * __new_push_command(__context_t & ctx, const push_xil_t & xil)
    {
        #define __CommandManagerT(v_t)                                      \
            __command_manager_t<                                            \
                __push_command_template_t, xil_storage_type_t, xil_type_t   \
            >::with_args_t<v_t>

        #define __DefineConstantCommandManagerT(name)   \
            static __CommandManagerT(name##_t)  name##_push_command_manager;

        __DefineConstantCommandManagerT(int8)
        __DefineConstantCommandManagerT(uint8)

        __DefineConstantCommandManagerT(int16)
        __DefineConstantCommandManagerT(uint16)

        __DefineConstantCommandManagerT(int32)
        __DefineConstantCommandManagerT(uint32)

        __DefineConstantCommandManagerT(int64)
        __DefineConstantCommandManagerT(uint64)

        __DefineConstantCommandManagerT(float)
        __DefineConstantCommandManagerT(double)

        __DefineConstantCommandManagerT(bool)
        __DefineConstantCommandManagerT(char)

        static __CommandManagerT(const char_t *)    __string_push_command_manager;

        static __command_manager_t<
            __push_command_template_t, xil_storage_type_t, xil_type_t
        >::with_args_t<__offset_t> __local_command_manager;

        static __command_manager_t<
            __push_command_template_t, xil_storage_type_t, xil_type_t
        >::with_args_t<msize_t> __argument_command_manager;

        static __command_manager_t<
            __push_command_template_t, xil_storage_type_t, xil_type_t
        >::with_args_t<msize_t> __field_command_manager;

        static __command_manager_t<
            __push_command_template_t, xil_storage_type_t, xil_type_t
        >::with_args_t<dimension_t> __push_array_element_command_manager;

        if(xil.stype() == xil_storage_type_t::duplicate)
            return &__duplicate_command;

        #define __V(s, d) (((uint32_t)(s) << 16) | (uint32_t)(d))

        switch(__V(xil.stype(), xil.dtype()))
        {
            #define __CaseConstant(name, t, d)                                  \
                case __V(xil_storage_type_t::constant, xil_type_t::d):          \
                    return name##_push_command_manager.template get_command<    \
                        xil_storage_type_t::constant, xil_type_t::d             \
                    >(__read_extra<t>(xil));

            case __V(xil_storage_type_t::constant, xil_type_t::string):
                return __string_push_command_manager.template get_command<
                    xil_storage_type_t::constant, xil_type_t::string
                >(ctx.to_cstr(__read_extra<res_t>(xil)));

            __CaseConstant(int8,    int8_t,     int8)
            __CaseConstant(uint8,   uint8_t,    uint8)

            __CaseConstant(int16,   int16_t,    int16)
            __CaseConstant(uint16,  uint16_t,   uint16)

            __CaseConstant(int32,   int32_t,    int32)
            __CaseConstant(uint32,  uint32_t,   uint32)

            __CaseConstant(int64,   int64_t,    int64)
            __CaseConstant(uint64,  uint64_t,   uint64)

            __CaseConstant(float,   float_t,    float_)
            __CaseConstant(double,  double_t,   double_)

            __CaseConstant(bool,    bool_t,     bool_)
            __CaseConstant(char,    char_t,     char_)

            #undef __CaseConstant

            // - - - - - - - - - - - - - - - - - - - - - - - - - -

            #define __CaseLocal(d)                                          \
                case __V(xil_storage_type_t::local, xil_type_t::d):         \
                    return __local_command_manager.template get_command<    \
                        xil_storage_type_t::local, xil_type_t::d            \
                    >(ctx.locals_layout.offset_of(xil.get_identity()));

            __CaseLocal(int8)
            __CaseLocal(uint8)

            __CaseLocal(int16)
            __CaseLocal(uint16)

            __CaseLocal(int32)
            __CaseLocal(uint32)

            __CaseLocal(int64)
            __CaseLocal(uint64)

            __CaseLocal(float_)
            __CaseLocal(double_)

            __CaseLocal(char_)
            __CaseLocal(bool_)

            __CaseLocal(object)
            __CaseLocal(string)

            #undef __CaseLocal

            // - - - - - - - - - - - - - - - - - - - - - - - - - -

            #define __CaseArgument(d)                                       \
                case __V(xil_storage_type_t::argument, xil_type_t::d):      \
                    return __argument_command_manager.template get_command< \
                        xil_storage_type_t::argument, xil_type_t::d         \
                    >(ctx.params_layout.offset_of(xil.get_identity()));

            __CaseArgument(int8)
            __CaseArgument(uint8)

            __CaseArgument(int16)
            __CaseArgument(uint16)

            __CaseArgument(int32)
            __CaseArgument(uint32)

            __CaseArgument(int64)
            __CaseArgument(uint64)

            __CaseArgument(float_)
            __CaseArgument(double_)

            __CaseArgument(char_)
            __CaseArgument(bool_)

            __CaseArgument(object)
            __CaseArgument(string)

            #undef __CaseArgument

            // - - - - - - - - - - - - - - - - - - - - - - - - - -
            // field

            #define __CaseField(d)                                          \
                case __V(xil_storage_type_t::field, xil_type_t::d):         \
                    return __field_command_manager.template get_command<    \
                        xil_storage_type_t::field, xil_type_t::d            \
                    >(__field_offset(ctx, xil.field_ref()));

            __CaseField(int8)
            __CaseField(uint8)

            __CaseField(int16)
            __CaseField(uint16)

            __CaseField(int32)
            __CaseField(uint32)

            __CaseField(int64)
            __CaseField(uint64)

            __CaseField(float_)
            __CaseField(double_)

            __CaseField(char_)
            __CaseField(bool_)

            __CaseField(object)
            __CaseField(string)

            #undef __CaseField

            // - - - - - - - - - - - - - - - - - - - - - - - - - -
            // array element

            #define __CaseArrayElement(d)                                               \
                case __V(xil_storage_type_t::array_element, xil_type_t::d):             \
                    return __push_array_element_command_manager.template get_command<   \
                        xil_storage_type_t::array_element, xil_type_t::d                \
                    >(__array_dimension(ctx, xil.type_ref()));

            __CaseArrayElement(int8)
            __CaseArrayElement(uint8)

            __CaseArrayElement(int16)
            __CaseArrayElement(uint16)

            __CaseArrayElement(int32)
            __CaseArrayElement(uint32)

            __CaseArrayElement(int64)
            __CaseArrayElement(uint64)

            __CaseArrayElement(float_)
            __CaseArrayElement(double_)

            __CaseArrayElement(char_)
            __CaseArrayElement(bool_)

            __CaseArrayElement(object)
            __CaseArrayElement(string)

            #undef __CaseArrayElement

            // - - - - - - - - - - - - - - - - - - - - - - - - - -
            // TODO: case other types ...


            default:
                _PF(_T("stype: %1%, dtype: %2%"), xil.stype(), xil.dtype());
                X_UNEXPECTED();

        }

        #undef __V
    }

    ////////// ////////// ////////// ////////// //////////
    // Class __pop_command_t

    #define __ToPopCmdValue(_stype, _xil_type)                                  \
        __ToCmdValue(pop, ((cmd_value_t)xil_storage_type_t::_stype << 4)        \
            | (cmd_value_t)_xil_type)

    template<xil_storage_type_t, xil_type_t> class __pop_command_t { };

    //-------- ---------- ---------- ---------- ----------

    #define __BeginPopCommand(s, d)                                             \
                                                                                \
        template<> class __pop_command_t<xil_storage_type_t::s, xil_type_t::d>  \
            : public __command_base_t                                           \
        {                                                                       \
        public:                                                                 \
            __pop_command_t(__offset_t offset) : __offset(offset) { }           \
                                                                                \
            virtual const string_t to_string(command_execute_context_t & ctx) const override \
            {                                                                   \
                return _F(_T("pop %1% %2%"), xil_storage_type_t::s, __offset);  \
            }                                                                   \
                                                                                \
            __BeginExecute(ctx, __ToPopCmdValue(s, xil_type_t::d))


        #define __EndPopCommand()                                               \
            __EndExecute()                                                      \
                                                                                \
        private:                                                                \
            __offset_t __offset;                                                \
        };

    //-------- ---------- ---------- ---------- ----------

    #define __LocalPopCommand(type_t, type)                                     \
                                                                                \
        __BeginPopCommand(local, type)                                          \
                                                                                \
            __Local(type_t, __offset) = ctx.stack.pop<type_t>();                \
            /*_PF(_T("POP %1% (%2%)"), __Local(type_t, __offset), __offset);*/  \
                                                                                \
        __EndPopCommand()

    __LocalPopCommand(int8_t,       int8)
    __LocalPopCommand(uint8_t,      uint8)

    __LocalPopCommand(int16_t,      int16)
    __LocalPopCommand(uint16_t,     uint16)

    __LocalPopCommand(int32_t,      int32)
    __LocalPopCommand(uint32_t,     uint32)

    __LocalPopCommand(int64_t,      int64)
    __LocalPopCommand(uint64_t,     uint64)

    __LocalPopCommand(float,        float_)
    __LocalPopCommand(double,       double_)

    __LocalPopCommand(char_t,       char_)
    __LocalPopCommand(bool,         bool_)

    __BeginPopCommand(local, object)
        __Local(rt_ref_t, __offset) = ctx.stack.pop<rt_ref_t>();
    __EndPopCommand();

    __BeginPopCommand(local, string)
        __Local(rt_ref_t, __offset) = ctx.stack.pop<rt_ref_t>();
    __EndPopCommand();

    //-------- ---------- ---------- ---------- ----------

    #define __ArgumentPopCommand(type_t, type)                                  \
        __BeginPopCommand(argument, type)                                       \
            __Argument(type_t, __offset + __stack_stub_size) = ctx.stack.pop<type_t>();    \
        __EndPopCommand()

    __ArgumentPopCommand(int8_t,       int8)
    __ArgumentPopCommand(uint8_t,      uint8)

    __ArgumentPopCommand(int16_t,      int16)
    __ArgumentPopCommand(uint16_t,     uint16)

    __ArgumentPopCommand(int32_t,      int32)
    __ArgumentPopCommand(uint32_t,     uint32)

    __ArgumentPopCommand(int64_t,      int64)
    __ArgumentPopCommand(uint64_t,     uint64)

    __ArgumentPopCommand(float,        float_)
    __ArgumentPopCommand(double,       double_)

    __ArgumentPopCommand(char_t,       char_)
    __ArgumentPopCommand(bool,         bool_)

    __BeginPopCommand(argument, object)
        __Argument(rt_ref_t, __offset + __stack_stub_size) = ctx.stack.pop<rt_ref_t>();
    __EndPopCommand();

    __BeginPopCommand(argument, string)
        __Argument(rt_ref_t, __offset + __stack_stub_size) = ctx.stack.pop<rt_ref_t>();
    __EndPopCommand();

    //-------- ---------- ---------- ---------- ----------

    #define __FieldPopCommand(type_t, type)                                     \
                                                                                \
        __BeginPopCommand(field, type)                                          \
                                                                                \
            type_t * field = __PField(type_t, ctx.stack.pop<rt_ref_t>(), __offset); \
            *field = ctx.stack.pop<type_t>();                                   \
                                                                                \
        __EndPopCommand()

    __FieldPopCommand(int8_t,       int8)
    __FieldPopCommand(uint8_t,      uint8)

    __FieldPopCommand(int16_t,      int16)
    __FieldPopCommand(uint16_t,     uint16)

    __FieldPopCommand(int32_t,      int32)
    __FieldPopCommand(uint32_t,     uint32)

    __FieldPopCommand(int64_t,      int64)
    __FieldPopCommand(uint64_t,     uint64)

    __FieldPopCommand(float,        float_)
    __FieldPopCommand(double,       double_)

    __FieldPopCommand(char_t,       char_)
    __FieldPopCommand(bool,         bool_)

    __BeginPopCommand(field, object)
        rt_ref_t * field = __PField(rt_ref_t, ctx.stack.pop<rt_ref_t>(), __offset);
        *field = ctx.stack.pop<rt_ref_t>();
    __EndPopCommand()

    __BeginPopCommand(field, string)
        rt_ref_t * field = __PField(rt_ref_t, ctx.stack.pop<rt_ref_t>(), __offset);
        *field = ctx.stack.pop<rt_ref_t>();
    __EndPopCommand()

    //-------- ---------- ---------- ---------- ----------

    template<xil_type_t _xil_type>
    class __pop_command_t<xil_storage_type_t::array_element, _xil_type>
        : public __command_base_t
    {
    public:
        __pop_command_t(dimension_t dimension) : __dimension(dimension) { }

        virtual const string_t to_string(command_execute_context_t & ctx) const override
        {
            return _F(_T("pop array element"));
        }

        __BeginExecute(ctx, __ToPopCmdValue(array_element, _xil_type))

            rt_ref_t array_ref = ctx.stack.pop<rt_ref_t>(); 

            if(__dimension == 1)
            {
                //array_length_t length = *mm::get_array_lengths(array_ref);
                array_length_t index  = ctx.stack.pop<array_length_t>();

                typedef __vnum_t<_xil_type> element_t;

                mm::set_array_element<element_t>(
                    array_ref, index, ctx.stack.pop<element_t>()
                );
            }
            else
            {
                X_UNEXPECTED();

                /*
                array_length_t * p_length = (array_length_t *)((byte_t *)obj - 1);

                for(dimension_t dimension = 1; dimension < __dimension; dimension++)
                {
                    index *= *--p_length;
                }
                */
            }

        __EndExecute()

    private:
        dimension_t __dimension;
    };

    //-------- ---------- ---------- ---------- ----------

    struct __pop_command_template_t
    {
        template<xil_storage_type_t stype, xil_type_t dtype, typename ... args_t>
        static auto new_command(memory_t * memory, args_t && ... args)
        {
            typedef __pop_command_t<stype, dtype> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    template<__nokey_t>
    class __pop_empty_command_t : public __command_base_t
    {
    public:
        __pop_empty_command_t(msize_t unit_size) : __unit_size(unit_size) { }

        __BeginExecute(ctx, __ToPopCmdValue(empty, 0))

            ctx.stack.pop(__unit_size);

        __EndExecute()

    private:
        msize_t __unit_size;
    };

    struct __pop_empty_command_template_t
    {
        template<__nokey_t nokey, typename ... args_t>
        static auto new_command(memory_t * memory, args_t && ... args)
        {
            typedef __pop_empty_command_t<nokey> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    static msize_t __size_of_pop_empty(__context_t & ctx, const pop_xil_t & xil)
    {
        _A(xil.stype() == xil_storage_type_t::empty);

        if(xil.dtype() != xil_type_t::empty)
            return get_vtype_size(to_vtype(xil.dtype()));

        ref_t type_ref = xil.type_ref();
        return ctx.size_of(type_ref);
    }

    static msize_t __unit_size_of_pop_empty(__context_t & ctx, const pop_xil_t & xil)
    {
        return unit_size_of(__size_of_pop_empty(ctx, xil));
    }

    static command_t * __new_pop_command(__context_t & ctx, const pop_xil_t & xil)
    {
        static __command_manager_t<
            __pop_command_template_t, xil_storage_type_t, xil_type_t
        >::with_args_t<msize_t> __variable_command_manager;

        static __command_manager_t<
            __pop_command_template_t, xil_storage_type_t, xil_type_t
        >::with_args_t<msize_t> __argument_command_manager;

        static __command_manager_t<
            __pop_empty_command_template_t, __nokey_t
        >::with_args_t<msize_t> __empty_command_manager;

        static __command_manager_t<
            __pop_command_template_t, xil_storage_type_t, xil_type_t
        >::with_args_t<msize_t> __field_command_manager;

        static __command_manager_t<
            __pop_command_template_t, xil_storage_type_t, xil_type_t
        >::with_args_t<dimension_t> __array_element_command_manager;

        if(xil.stype() == xil_storage_type_t::empty)
            return __empty_command_manager.template get_command<__nokey>(
                __unit_size_of_pop_empty(ctx, xil)
            );

        #define __V(s, d) (((uint32_t)(s) << 16) | (uint32_t)(d))

        switch(__V(xil.stype(), xil.dtype()))
        {
            #define __Case(s, d)                                                \
                case __V(xil_storage_type_t::s, xil_type_t::d):                 \
                    return __variable_command_manager.template get_command<     \
                        xil_storage_type_t::s, xil_type_t::d                    \
                    >(ctx.locals_layout.offset_of(xil.get_identity()));

            #define __CaseLocal(d)  __Case(local, d)

            __CaseLocal(int8)
            __CaseLocal(uint8)

            __CaseLocal(int16)
            __CaseLocal(uint16)

            __CaseLocal(int32)
            __CaseLocal(uint32)

            __CaseLocal(int64)
            __CaseLocal(uint64)

            __CaseLocal(float_)
            __CaseLocal(double_)

            __CaseLocal(char_)
            __CaseLocal(bool_)

            __CaseLocal(object)
            __CaseLocal(string)

            #undef __CaseLocal

            #define __CaseArgument(d)                                       \
                case __V(xil_storage_type_t::argument, xil_type_t::d):      \
                    return __argument_command_manager.template get_command< \
                        xil_storage_type_t::argument, xil_type_t::d         \
                    >(ctx.params_layout.offset_of(xil.get_identity()));

            __CaseArgument(int8)
            __CaseArgument(uint8)

            __CaseArgument(int16)
            __CaseArgument(uint16)

            __CaseArgument(int32)
            __CaseArgument(uint32)

            __CaseArgument(int64)
            __CaseArgument(uint64)

            __CaseArgument(float_)
            __CaseArgument(double_)

            __CaseArgument(char_)
            __CaseArgument(bool_)

            __CaseArgument(object)
            __CaseArgument(string)

            #undef __CaseArgument
            
            #define __CaseField(d)                                          \
                case __V(xil_storage_type_t::field, xil_type_t::d):         \
                    return __field_command_manager.template get_command<    \
                        xil_storage_type_t::field, xil_type_t::d            \
                    >(__field_offset(ctx, xil.field_ref()));

            __CaseField(int8)
            __CaseField(uint8)

            __CaseField(int16)
            __CaseField(uint16)

            __CaseField(int32)
            __CaseField(uint32)

            __CaseField(int64)
            __CaseField(uint64)

            __CaseField(float_)
            __CaseField(double_)

            __CaseField(char_)
            __CaseField(bool_)

            __CaseField(object)
            __CaseField(string)

            #undef __CaseField

            #define __CaseArrayElement(d)                                           \
                case __V(xil_storage_type_t::array_element, xil_type_t::d):         \
                    return __array_element_command_manager.template get_command<    \
                        xil_storage_type_t::array_element, xil_type_t::d            \
                    >(__array_dimension(ctx, xil.type_ref()));                      \
                    break;

            __CaseArrayElement(int8)
            __CaseArrayElement(uint8)

            __CaseArrayElement(int16)
            __CaseArrayElement(uint16)

            __CaseArrayElement(int32)
            __CaseArrayElement(uint32)

            __CaseArrayElement(int64)
            __CaseArrayElement(uint64)

            __CaseArrayElement(float_)
            __CaseArrayElement(double_)

            __CaseArrayElement(char_)
            __CaseArrayElement(bool_)

            __CaseArrayElement(object)
            __CaseArrayElement(string)

            #undef __CaseArrayElement

            #undef __Case

            default:
                X_UNEXPECTED();
        }

        #undef __V
    }

    ////////// ////////// ////////// ////////// //////////
    // Class __pick_command_t

    template<xil_storage_type_t, xil_type_t> class __pick_command_t { };

    //-------- ---------- ---------- ---------- ----------

    #define __BeginLocalPickCommand(s, d)                                       \
                                                                                \
        template<> class __pick_command_t<xil_storage_type_t::s, xil_type_t::d> \
            : public __command_base_t                                           \
        {                                                                       \
        public:                                                                 \
            __pick_command_t(__offset_t offset) : __offset(offset) { }          \
                                                                                \
            virtual const string_t to_string(command_execute_context_t & ctx) const override \
            {                                                                   \
                return _F(_T("pick %1% %2%"), xil_storage_type_t::s, __offset); \
            }                                                                   \
                                                                                \
            __BeginExecute(ctx, __ToPopCmdValue(s, xil_type_t::d))


        #define __EndLocalPickCommand()                                         \
            __EndExecute()                                                      \
                                                                                \
        private:                                                                \
            __offset_t __offset;                                                \
        };

    //-------- ---------- ---------- ---------- ----------

    #define __LocalPickCommand(type_t, type)                                    \
                                                                                \
        __BeginLocalPickCommand(local, type)                                    \
                                                                                \
            __Local(type_t, __offset) = ctx.stack.pick<type_t>();               \
            /*_PF(_T("POP %1% (%2%)"), __Local(type_t, __offset), __offset);*/  \
                                                                                \
        __EndLocalPickCommand()

    __LocalPickCommand(int8_t,       int8)
    __LocalPickCommand(uint8_t,      uint8)

    __LocalPickCommand(int16_t,      int16)
    __LocalPickCommand(uint16_t,     uint16)

    __LocalPickCommand(int32_t,      int32)
    __LocalPickCommand(uint32_t,     uint32)

    __LocalPickCommand(int64_t,      int64)
    __LocalPickCommand(uint64_t,     uint64)

    __LocalPickCommand(float,        float_)
    __LocalPickCommand(double,       double_)

    __LocalPickCommand(char_t,       char_)
    __LocalPickCommand(bool,         bool_)

    //-------- ---------- ---------- ---------- ----------

    struct __pick_command_template_t
    {
        template<xil_storage_type_t stype, xil_type_t dtype, typename ... args_t>
        static auto new_command(memory_t * memory, args_t && ... args)
        {
            typedef __pick_command_t<stype, dtype> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    static command_t * __new_pick_command(__context_t & ctx, const pick_xil_t & xil)
    {
        static __command_manager_t<
            __pick_command_template_t, xil_storage_type_t, xil_type_t
        >::with_args_t<msize_t> __variable_command_manager;

        static __command_manager_t<
            __pick_command_template_t, xil_storage_type_t, xil_type_t
        >::with_args_t<res_t> __field_command_manager;

        #define __V(s, d) (((uint32_t)(s) << 16) | (uint32_t)(d))

        switch(__V(xil.stype(), xil.dtype()))
        {
            #define __Case(s, d)                                                \
                case __V(xil_storage_type_t::s, xil_type_t::d):                 \
                    return __variable_command_manager.template get_command<     \
                        xil_storage_type_t::s, xil_type_t::d                    \
                    >(ctx.locals_layout.offset_of(xil.get_identity()));

            #define __CaseLocal(d)  __Case(local, d)

            __CaseLocal(int8)
            __CaseLocal(uint8)

            __CaseLocal(int16)
            __CaseLocal(uint16)

            __CaseLocal(int32)
            __CaseLocal(uint32)

            __CaseLocal(int64)
            __CaseLocal(uint64)

            __CaseLocal(float_)
            __CaseLocal(double_)

            __CaseLocal(char_)
            __CaseLocal(bool_)

            #undef __CaseLocal
            #undef __Case

            // TODO: case other types ...

            default:
                X_UNEXPECTED();
        }

        #undef __V
    }

    #undef __Local

    ////////// ////////// ////////// ////////// //////////

    __AlwaysInline static void __pre_new(command_execute_context_t & ctx, rt_type_t * type)
    {
        _A(type != nullptr);
        type->pre_new(ctx.env);
    }

    __AlwaysInline static void __pre_static_call(command_execute_context_t & ctx, rt_type_t * type)
    {
        _A(type != nullptr);
        type->pre_static_call(ctx.env);
    }

    ////////// ////////// ////////// ////////// //////////
    // Class __call_command_t

    #define __ToCallCmdValue_(_call_type, _method_t)                                \
        __ToCmdValue(call,                                                          \
            ((cmd_value_t)xil_call_type_t::_call_type << 8) |                       \
            (cmd_value_t)rt_tidx<_method_t>()                                       \
        )

    #define __ToCallCmdValue(_call_type)                                            \
        __ToCmdValue(call, xil_call_type_t::_call_type)

    typedef rtlib::libfunc_t __libfunc_t;

    template<__nokey_t>
    class __internal_call_command_t : public __command_base_t
    {
    public:
        __internal_call_command_t(__libfunc_t func, int stack_unit_size)
            : __func(func), __stack_unit_size(stack_unit_size)
        {
            _A(func != nullptr);
        }

        __BeginExecute(ctx, __ToCallCmdValue(internal))

            rtlib_context_t context(ctx.stack.top() - __stack_unit_size);
            __func(context);

            ctx.stack.pop(__stack_unit_size);

        __EndExecute()

        virtual const string_t to_string(command_execute_context_t & ctx) const override
        {
            return _F(_T("call internal %1%"), (void *)__func);
        }

    private:
        int __stack_unit_size;
        __libfunc_t __func;
    };

    struct __internal_call_command_template_t
    {
        template<__nokey_t nokey, typename ... args_t>
        static auto new_command(memory_t * memory, __libfunc_t libfunc,
                        __context_t & ctx, rt_method_t * rt_method)
        {
            typedef __internal_call_command_t<nokey> this_command_t;
            this_command_t * cmd = __new_command<this_command_t>(
                memory, libfunc, __get_method_stack_unit_size(ctx, rt_method)
            );

            return cmd;
        }

        constexpr static int __stack_unit_size(size_t size)
        {
            return _alignf(size, sizeof(rt_stack_unit_t)) / sizeof(rt_stack_unit_t);
        }

        static int __get_method_stack_unit_size(__context_t & ctx, rt_method_t * rt_method)
        {
            int unit_size = 0;
            rt_assembly_t * rt_assembly = rt_method->get_assembly();

            assembly_analyzer_t analyzer(ctx, rt_assembly);
            rt_assembly->each_params((*rt_method)->params, [&](int, auto & param) {

                rt_type_t * param_type = analyzer.get_type(param.type);
                switch((mt_type_extra_t)param.type.extra)
                {
                    case mt_type_extra_t::type_ref:
                    case mt_type_extra_t::general: {

                        rt_general_type_t * t = (rt_general_type_t *)param_type;
                        size_t arg_size = get_vtype_size((vtype_t)(*t)->vtype);
                        unit_size += __stack_unit_size(arg_size);

                    }   break;

                    default:
                        unit_size += __stack_unit_size(sizeof(void *));
                        break;
                }

                return true;
            });

            return unit_size;
        }
    };

    static command_t * __new_internal_call_command(__context_t & ctx, const call_xil_t & xil)
    {
        static __command_manager_t<
            __internal_call_command_template_t, __nokey_t
        >::with_args_t<__libfunc_t> __internal_call_command_manager;

        ref_t method_ref = (ref_t)xil.method;

        rt_method_t * rt_method = ctx.get_method(method_ref);
        rt_sid_t method_name = rt_method->get_name();

        __libfunc_t libfunc = rtlib::get_libfunc(method_name);
        if(libfunc == nullptr)
            throw _ED(exec_error_code_t::internal_function_not_found, method_name);

        command_t * cmd = __internal_call_command_manager.template get_command<__nokey>(
            __Mv(libfunc), ctx, rt_method
        );

        return cmd;
    }

    //-------- ---------- ---------- ---------- ----------

    template<typename _rt_method_t>
    class __call_command_base_t : public __command_base_t
    {
    public:
        __call_command_base_t(_rt_method_t * rt_method) : __rt_method(rt_method)
        { }

        virtual const string_t to_string(command_execute_context_t & ctx) const override
        {
            return _F(_T("call %1%"), this->__get_name(ctx));
        }

    protected:
        __AlwaysInline exec_method_t * __get_method(command_execute_context_t & ctx)
        {
            if(__method == nullptr)
                __method = __parse_method(ctx);

            return __method;
        }

        string_t __get_name(command_execute_context_t & ctx) const
        {
            return __rt_method->get_name();
        }

        _rt_method_t * __rt_method;

        rt_type_t * get_type() { return __rt_method->get_host_type(); }

    private:
        exec_method_t * __method = nullptr;

        exec_method_t * __parse_method(command_execute_context_t & ctx)
        {
            return ctx.env.exec_method_of(__rt_method);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    template<typename _rt_method_t>
    class __static_call_command_t : public __call_command_base_t<_rt_method_t>
    {
        typedef __call_command_base_t<_rt_method_t> __super_t;

    public:
        using __super_t::__super_t;

        __BeginExecute(ctx, __ToCallCmdValue_(static_, _rt_method_t))

            __pre_static_call(ctx, this->get_type());

            exec_method_t * method = this->__get_method(ctx);
            ctx.push_calling(method->commands);
            ctx.stack.increase_top(method->stack_unit_size);

        __EndExecute()

        virtual const string_t to_string(command_execute_context_t & ctx) const override
        {
            return _F(_T("call static %1%"), this->__get_name(ctx));
        }
    };

    template<typename _rt_method_t>
    struct __static_call_command_template_t
    {
        template<__nokey_t, typename ... args_t>
        static auto new_command(memory_t * memory, _rt_method_t * rt_method, __context_t & ctx)
        {
            typedef __static_call_command_t<_rt_method_t> this_command_t;
            return __new_command<this_command_t>(memory, rt_method);
        }
    };

    command_t * new_static_call_command(__context_t & ctx, ref_t method_ref)
    {
        if((mt_member_extra_t)method_ref.extra == mt_member_extra_t::generic)
        {
            static __command_manager_t<
                __static_call_command_template_t<rt_generic_method_t>, __nokey_t
            >::with_args_t<rt_generic_method_t *> __static_generic_call_command_manager;

            rt_generic_method_t * rt_generic_method = ctx.get_generic_method(method_ref);

            _PF(_T("---- new_static_call_command: %1% %2%"), rt_generic_method->get_name(),
                                                             method_ref);

            return __static_generic_call_command_manager.template get_command<__nokey>(
                __Mv(rt_generic_method), ctx
            );
        }
        else
        {
            static __command_manager_t<
                __static_call_command_template_t<rt_method_t>, __nokey_t
            >::with_args_t<rt_method_t *> __static_call_command_manager;

            rt_method_t * rt_method = ctx.get_method(method_ref);
            return __static_call_command_manager.template get_command<__nokey>(
                __Mv(rt_method), ctx
            );
        }
    }

    static command_t * __new_static_call_command(__context_t & ctx, const call_xil_t & xil)
    {
        ref_t method_ref = (ref_t)xil.method;

        return new_static_call_command(ctx, method_ref);
    }

    //-------- ---------- ---------- ---------- ----------

    template<typename _rt_method_t>
    class __instance_call_command_t : public __call_command_base_t<_rt_method_t>
    {
        typedef __call_command_base_t<_rt_method_t> __super_t;

    public:
        using __super_t::__super_t;

        __BeginExecute(ctx, __ToCallCmdValue_(instance, _rt_method_t))

            exec_method_t * method = this->__get_method(ctx);
            ctx.push_calling(method->commands);
            ctx.stack.increase_top(method->stack_unit_size);

        __EndExecute()

        virtual const string_t to_string(command_execute_context_t & ctx) const override
        {
            return _F(_T("call instance %1%"), this->__get_name(ctx));
        }
    };

    template<typename _rt_method_t>
    struct __instance_call_command_template_t
    {
        template<__nokey_t nokey, typename ... args_t>
        static auto new_command(memory_t * memory, _rt_method_t * rt_method, __context_t & ctx)
        {
            typedef __instance_call_command_t<_rt_method_t> this_command_t;
            return __new_command<this_command_t>(memory, rt_method);
        }
    };

    static command_t * __new_instance_call_command(__context_t & ctx, const call_xil_t & xil)
    {
        ref_t method_ref = (ref_t)xil.method;

        static __command_manager_t<
            __instance_call_command_template_t<rt_method_t>, __nokey_t
        >::with_args_t<rt_method_t *> __instance_call_command_manager;

        rt_method_t * rt_method = ctx.get_method(method_ref);

        return __instance_call_command_manager.template get_command<__nokey>(
            __Mv(rt_method), ctx
        );
    }

    //-------- ---------- ---------- ---------- ----------

    assembly_analyzer_t __assembly_analyzer(command_execute_context_t & ctx,
                                            rt_assembly_t * current_assembly)
    {
        return assembly_analyzer_t(ctx.env, current_assembly);
    }

    assembly_analyzer_t __assembly_analyzer(command_execute_context_t & ctx,
                                            rt_type_t * rt_type)
    {
        return __assembly_analyzer(ctx, rt_type->get_assembly());
    }

    //-------- ---------- ---------- ---------- ----------

    template<__nokey_t>
    class __virtual_call_command_t : public __command_base_t
    {
        typedef __command_base_t __super_t;

    public:
        __virtual_call_command_t(int offset) :__offset(offset) { }

        __BeginExecute(ctx, __ToCallCmdValue(virtual_))

            rt_ref_t rt_ref = __Argument(rt_ref_t, 0);
            rt_type_t * rt_type = __RtTypeOf(rt_ref);

            rt_vtable_t * vtbl = get_vtable(rt_type);
            rt_vfunction_t func = vtbl->functions[__offset];

            if(!func.is_method())
            {
                rt_assembly_t * rt_assembly = ctx.env.assemblies.at(func.assembly_idx);

                rt_method_t * rt_method = __assembly_analyzer(ctx, rt_assembly).get_method(
                    ref_t(func.method_idx)
                );

                func.method = ctx.env.exec_method_of(rt_method);
            }

            exec_method_t * method = (exec_method_t *)func.method;
            ctx.push_calling(method->commands);
            ctx.stack.increase_top(method->stack_unit_size);

        __EndExecute()

        virtual const string_t to_string(command_execute_context_t & ctx) const override
        {
            return _F(_T("call virtual %1%"), __offset);
        }

        int __offset;
    };

    struct __virtual_call_command_template_t
    {
        template<__nokey_t nokey, typename ... args_t>
        static auto new_command(memory_t * memory, int offset, __context_t & ctx)
        {
            typedef __virtual_call_command_t<nokey> this_command_t;
            return __new_command<this_command_t>(memory, offset);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    static command_t * __new_virtual_call_command(__context_t & ctx, const call_xil_t & xil)
    {
        ref_t method_ref = (ref_t)xil.method;
        _A((mt_member_extra_t)method_ref.extra == mt_member_extra_t::internal);

        static __command_manager_t<
            __virtual_call_command_template_t, __nokey_t
        >::with_args_t<int> __virtual_call_command_manager;

        rt_method_t * rt_method = ctx.get_method(method_ref);
        int offset = ctx.get_virtual_method_offset(ctx, method_ref);

        return __virtual_call_command_manager.template get_command<__nokey>(
            __Mv(offset), ctx
        );
    }

    //-------- ---------- ---------- ---------- ----------

    static command_t * __new_call_command(__context_t & ctx, const call_xil_t & xil)
    {
        switch(xil.call_type())
        {
            case xil_call_type_t::internal:
                return __new_internal_call_command(ctx, xil);

            case xil_call_type_t::static_:
                return __new_static_call_command(ctx, xil);

            case xil_call_type_t::instance:
                return __new_instance_call_command(ctx, xil);

            case xil_call_type_t::virtual_:
                return __new_virtual_call_command(ctx, xil);

            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        template<typename _command_t> struct __xil_command_trait_t { };

        template<> struct __xil_command_trait_t<xil_al_command_t>
        {
            static const xil_command_t command = xil_command_t::al;
        };

        template<> struct __xil_command_trait_t<xil_logic_command_t>
        {
            static const xil_command_t command = xil_command_t::logic;
        };

        template<> struct __xil_command_trait_t<xil_bit_command_t>
        {
            static const xil_command_t command = xil_command_t::bit;
        };

        template<> struct __xil_command_trait_t<xil_cmp_command_t>
        {
            static const xil_command_t command = xil_command_t::cmp;
        };

        template<typename _command_t>
        constexpr xil_command_t to_xil_command()
        {
            return __xil_command_trait_t<_command_t>::command;
        }
    };

    #define __ToBinaryAlCmdValue(cmd, dtype1, dtype2)                                   \
        __ToCmdValue(al,                                                                \
            ((cmd_value_t)to_xil_command<decltype(cmd)>() << 16) |                      \
            ((cmd_value_t)cmd << 8) |                                                   \
            ((cmd_value_t)dtype1 << 4) | (cmd_value_t)dtype2                            \
        )

    #define __ToUnitaryAlCmdValue(cmd, dtype)                                           \
        __ToCmdValue(al,                                                                \
            ((cmd_value_t)to_xil_command<decltype(cmd)>() << 16) |                      \
            ((cmd_value_t)cmd << 8) | (cmd_value_t)dtype1                               \
        )

    template<typename _command_t, _command_t cmd, xil_type_t dtype1, xil_type_t dtype2>
    class __binary_command_t : public __command_base_t
    {
    public:
        __BeginExecute(ctx, __ToBinaryAlCmdValue(cmd, dtype1, dtype2))

            const vtype_t vtype1 = to_vtype(dtype1);
            const vtype_t vtype2 = to_vtype(dtype2);

            typedef vnum_t<vtype1> t1;
            typedef vnum_t<vtype2> t2;

            const msize_t unit_size1 = unit_size_of(get_vtype_size(vtype1));
            const msize_t unit_size2 = unit_size_of(get_vtype_size(vtype2));

            rt_stack_unit_t * top = ctx.stack.pop<unit_size1 + unit_size2>();

            ctx.stack.push(
                binary_operator_t<to_operator(cmd), vtype1, vtype2>()(
                    *(t1 *)top, *(t2 *)(top + unit_size1)
                )
            );

        __EndExecute()

        virtual const string_t to_string(command_execute_context_t & ctx) const override
        {
            return _str(cmd);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    template<typename _command_t>
    struct __binary_command_template_t
    {
        template<_command_t cmd, xil_type_t dtype1, xil_type_t dtype2, typename ... args_t>
        static auto new_command(memory_t * memory, args_t && ... args)
        {
            typedef __binary_command_t<_command_t, cmd, dtype1, dtype2> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<args_t>(args) ...);
        }
    };

    template<typename _command_t>
    using __binary_command_manager_t = typename __command_manager_t<
        __binary_command_template_t<_command_t>, _command_t, xil_type_t, xil_type_t
    >::template with_args_t<>;

    //-------- ---------- ---------- ---------- ----------

    template<typename _command_t, _command_t cmd, xil_type_t dtype1>
    class __unitary_command_t : public __command_base_t
    {
    public:
        __BeginExecute(ctx, __ToUnitaryAlCmdValue(cmd, dtype1))

            const vtype_t vtype1 = to_vtype(dtype1);

            typedef vnum_t<vtype1> t1;
            t1 v1 = ctx.stack.pop<t1>();

            ctx.stack.push(
                unitary_operator_t<to_operator(cmd), vtype1>()(v1)
            );
        __EndExecute()

        virtual const string_t to_string(command_execute_context_t & ctx) const override
        {
            return _str(cmd);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    template<typename _command_t>
    struct __unitary_command_template_t
    {
        template<_command_t cmd, xil_type_t dtype1, typename ... args_t>
        static auto new_command(memory_t * memory, args_t && ... args)
        {
            typedef __unitary_command_t<_command_t, cmd, dtype1> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<args_t>(args) ...);
        }
    };

    template<typename _command_t>
    using __unitary_command_manager_t = typename __command_manager_t<
        __unitary_command_template_t<_command_t>, _command_t, xil_type_t
    >::template with_args_t<>;

    //-------- ---------- ---------- ---------- ----------

    static command_t * __new_al_command(__context_t & ctx, const al_xil_t & xil)
    {
        static __binary_command_manager_t<xil_al_command_t> __binary_command_manager;
        static __unitary_command_manager_t<xil_al_command_t> __unitary_command_manager;

        #define __V(cmd, dtype1, dtype2) \
            (((uint32_t)(cmd) << 16) | ((uint32_t)(dtype1) << 8) | (uint32_t)(dtype2))

        switch(__V(xil.cmd(), xil.dtype1(), xil.dtype2()))
        {
            #define __CaseBinary(cmd, dtype1, dtype2)                                       \
                case __V(xil_al_command_t::cmd, xil_type_t::dtype1, xil_type_t::dtype2):    \
                    return __binary_command_manager.template get_command<                   \
                        xil_al_command_t::cmd, xil_type_t::dtype1, xil_type_t::dtype2       \
                    >();

            #define __C1(cmd, dtype1, dtype2)    __CaseBinary(cmd, dtype1, dtype2)
            #define __C2(cmd, dtype1, dtype2)                                               \
                __C1(cmd, dtype1, dtype2)                                                   \
                __C1(cmd, dtype2, dtype1)

            #define __CaseBinaries(cmd)                                                     \
                __C1(cmd, int8,   int8)                                                     \
                __C2(cmd, int8,   uint8)                                                    \
                                                                                            \
                __C1(cmd, uint8,  uint8)                                                    \
                                                                                            \
                __C2(cmd, int16,  int8)                                                     \
                __C2(cmd, int16,  uint8)                                                    \
                __C1(cmd, int16,  int16)                                                    \
                __C2(cmd, int16,  uint16)                                                   \
                                                                                            \
                __C2(cmd, uint16, int8)                                                     \
                __C2(cmd, uint16, uint8)                                                    \
                __C1(cmd, uint16, uint16)                                                   \
                                                                                            \
                __C2(cmd, int32,  int8)                                                     \
                __C2(cmd, int32,  uint8)                                                    \
                __C2(cmd, int32,  int16)                                                    \
                __C2(cmd, int32,  uint16)                                                   \
                __C1(cmd, int32,  int32)                                                    \
                                                                                            \
                __C2(cmd, uint32, int8)                                                     \
                __C2(cmd, uint32, uint8)                                                    \
                __C2(cmd, uint32, int16)                                                    \
                __C2(cmd, uint32, uint16)                                                   \
                __C2(cmd, uint32, int32)                                                    \
                __C1(cmd, uint32, uint32)                                                   \
                                                                                            \
                __C2(cmd, int64,  int8)                                                     \
                __C2(cmd, int64,  uint8)                                                    \
                __C2(cmd, int64,  int16)                                                    \
                __C2(cmd, int64,  uint16)                                                   \
                __C2(cmd, int64,  int32)                                                    \
                __C2(cmd, int64,  uint32)                                                   \
                __C1(cmd, int64,  int64)                                                    \
                                                                                            \
                __C2(cmd, uint64,  int8)                                                    \
                __C2(cmd, uint64,  uint8)                                                   \
                __C2(cmd, uint64,  int16)                                                   \
                __C2(cmd, uint64,  uint16)                                                  \
                __C2(cmd, uint64,  int32)                                                   \
                __C2(cmd, uint64,  uint32)                                                  \
                __C2(cmd, uint64,  int64)                                                   \
                __C1(cmd, uint64,  uint64)

                #define __CaseFloatBinaries(cmd)                                            \
                __C2(cmd, float_,  int8)                                                    \
                __C2(cmd, float_,  uint8)                                                   \
                __C2(cmd, float_,  int16)                                                   \
                __C2(cmd, float_,  uint16)                                                  \
                __C2(cmd, float_,  int32)                                                   \
                __C2(cmd, float_,  uint32)                                                  \
                __C2(cmd, float_,  int64)                                                   \
                __C1(cmd, float_,  uint64)                                                  \
                                                                                            \
                __C2(cmd, double_,  int8)                                                   \
                __C2(cmd, double_,  uint8)                                                  \
                __C2(cmd, double_,  int16)                                                  \
                __C2(cmd, double_,  uint16)                                                 \
                __C2(cmd, double_,  int32)                                                  \
                __C2(cmd, double_,  uint32)                                                 \
                __C2(cmd, double_,  int64)                                                  \
                __C1(cmd, double_,  uint64)                                                 \
                __C2(cmd, double_,  float_)                                                 \
                __C1(cmd, double_,  double_)

            __CaseBinaries(add)
            __CaseFloatBinaries(add)

            __CaseBinaries(sub)
            __CaseFloatBinaries(sub)

            __CaseBinaries(mul)
            __CaseFloatBinaries(mul)

            __CaseBinaries(div)
            __CaseFloatBinaries(div)

            __CaseBinaries(mod)

            #undef __C1
            #undef __C2
            #undef __CaseBinaries
            #undef __CaseFloatBinaries
            #undef __CaseBinary

            // - - - - - - - - - - - - - - - - - - - - - - - - - -

            #define __CaseUnitary(cmd, dtype)                                               \
                case __V(xil_al_command_t::cmd, xil_type_t::dtype, xil_type_t::__default__):  \
                    return __unitary_command_manager.template get_command<                  \
                        xil_al_command_t::cmd, xil_type_t::dtype                            \
                    >();

            #define __C(cmd, dtype)    __CaseUnitary(cmd, dtype)

            #define __CaseUnitaries(cmd)                                                    \
                __C(cmd, int8)                                                              \
                __C(cmd, uint8)                                                             \
                __C(cmd, int16)                                                             \
                __C(cmd, uint16)                                                            \
                __C(cmd, int32)                                                             \
                __C(cmd, uint32)                                                            \
                __C(cmd, int64)                                                             \
                __C(cmd, uint64)

            #define __CaseFloatUnitaries(cmd)                                               \
                __C(cmd, float_)                                                            \
                __C(cmd, double_)

            __CaseUnitaries(minus)
            __CaseFloatUnitaries(minus)

            __CaseUnitaries(positive)
            __CaseFloatUnitaries(positive)

            #undef __CaseUnitary
            #undef __C
            #undef __CaseUnitaries
            #undef __CaseFloatUnitaries

            default:
                X_UNEXPECTED();

        }

        return nullptr;
    }

    ////////// ////////// ////////// ////////// //////////

    static command_t * __new_bit_command(__context_t & ctx, const bit_xil_t & xil)
    {
        static __binary_command_manager_t<xil_bit_command_t> __binary_command_manager;
        static __unitary_command_manager_t<xil_bit_command_t> __unitary_command_manager;

        #define __V(cmd, dtype1, dtype2) \
            (((uint32_t)(cmd) << 16) | ((uint32_t)(dtype1) << 8) | (uint32_t)(dtype2))

        switch(__V(xil.cmd(), xil.dtype1(), xil.dtype2()))
        {
            #define __CaseBinary(cmd, dtype1, dtype2)                                       \
                case __V(xil_bit_command_t::cmd, xil_type_t::dtype1, xil_type_t::dtype2):   \
                    return __binary_command_manager.template get_command<                   \
                        xil_bit_command_t::cmd, xil_type_t::dtype1, xil_type_t::dtype2      \
                    >();

            #define __C1(cmd, dtype1, dtype2)    __CaseBinary(cmd, dtype1, dtype2)
            #define __C2(cmd, dtype1, dtype2)                                               \
                __C1(cmd, dtype1, dtype2)                                                   \
                __C1(cmd, dtype2, dtype1)

            #define __CaseBinaries(cmd)                                                     \
                __C1(cmd, int8,   int8)                                                     \
                __C2(cmd, int8,   uint8)                                                    \
                                                                                            \
                __C1(cmd, uint8,  uint8)                                                    \
                                                                                            \
                __C2(cmd, int16,  int8)                                                     \
                __C2(cmd, int16,  uint8)                                                    \
                __C1(cmd, int16,  int16)                                                    \
                __C2(cmd, int16,  uint16)                                                   \
                                                                                            \
                __C2(cmd, uint16, int8)                                                     \
                __C2(cmd, uint16, uint8)                                                    \
                __C1(cmd, uint16, uint16)                                                   \
                                                                                            \
                __C2(cmd, int32,  int8)                                                     \
                __C2(cmd, int32,  uint8)                                                    \
                __C2(cmd, int32,  int16)                                                    \
                __C2(cmd, int32,  uint16)                                                   \
                __C1(cmd, int32,  int32)                                                    \
                                                                                            \
                __C2(cmd, uint32, int8)                                                     \
                __C2(cmd, uint32, uint8)                                                    \
                __C2(cmd, uint32, int16)                                                    \
                __C2(cmd, uint32, uint16)                                                   \
                __C2(cmd, uint32, int32)                                                    \
                __C1(cmd, uint32, uint32)                                                   \
                                                                                            \
                __C2(cmd, int64,  int8)                                                     \
                __C2(cmd, int64,  uint8)                                                    \
                __C2(cmd, int64,  int16)                                                    \
                __C2(cmd, int64,  uint16)                                                   \
                __C2(cmd, int64,  int32)                                                    \
                __C2(cmd, int64,  uint32)                                                   \
                __C1(cmd, int64,  int64)                                                    \
                                                                                            \
                __C2(cmd, uint64,  int8)                                                    \
                __C2(cmd, uint64,  uint8)                                                   \
                __C2(cmd, uint64,  int16)                                                   \
                __C2(cmd, uint64,  uint16)                                                  \
                __C2(cmd, uint64,  int32)                                                   \
                __C2(cmd, uint64,  uint32)                                                  \
                __C2(cmd, uint64,  int64)                                                   \
                __C1(cmd, uint64,  uint64)

            __CaseBinaries(bit_and)
            __CaseBinaries(bit_or)
            __CaseBinaries(bit_xor)
            __CaseBinaries(left_shift)
            __CaseBinaries(right_shift)

            #undef __C1
            #undef __C2
            #undef __CaseBinaries
            #undef __CaseFloatBinaries
            #undef __CaseBinary

            // - - - - - - - - - - - - - - - - - - - - - - - - - -

            #define __CaseUnitary(cmd, dtype)                                               \
                case __V(xil_bit_command_t::cmd, xil_type_t::dtype, xil_type_t::__default__):  \
                    return __unitary_command_manager.template get_command<                  \
                        xil_bit_command_t::cmd, xil_type_t::dtype                           \
                    >();

            #define __C(cmd, dtype)    __CaseUnitary(cmd, dtype)

            #define __CaseUnitaries(cmd)                                                    \
                __C(cmd, int8)                                                              \
                __C(cmd, uint8)                                                             \
                __C(cmd, int16)                                                             \
                __C(cmd, uint16)                                                            \
                __C(cmd, int32)                                                             \
                __C(cmd, uint32)                                                            \
                __C(cmd, int64)                                                             \
                __C(cmd, uint64)

            __CaseUnitaries(bit_not)

            #undef __CaseUnitary
            #undef __C
            #undef __CaseUnitaries

            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    static command_t * __new_logic_command(__context_t & ctx, const logic_xil_t & xil)
    {
        static __binary_command_manager_t<xil_logic_command_t> __binary_command_manager;
        static __unitary_command_manager_t<xil_logic_command_t> __unitary_command_manager;

        switch(xil.cmd())
        {
            case xil_logic_command_t::and_:
                return __binary_command_manager.template get_command<
                    xil_logic_command_t::and_, xil_type_t::bool_, xil_type_t::bool_
                >();

            case xil_logic_command_t::or_:
                return __binary_command_manager.template get_command<
                    xil_logic_command_t::or_, xil_type_t::bool_, xil_type_t::bool_
                >();

            case xil_logic_command_t::not_:
                return __unitary_command_manager.template get_command<
                    xil_logic_command_t::not_, xil_type_t::bool_
                >();

            default:
                X_UNEXPECTED();
        }

        return nullptr;
    }

    ////////// ////////// ////////// ////////// //////////

    static command_t * __new_cmp_command(__context_t & ctx, const cmp_xil_t & xil)
    {
        static __binary_command_manager_t<xil_cmp_command_t> __command_manager;

        #define __V(cmd, dtype1, dtype2) \
            (((uint32_t)(cmd) << 16) | ((uint32_t)(dtype1) << 8) | (uint32_t)(dtype2))

        switch(__V(xil.cmd(), xil.dtype1(), xil.dtype2()))
        {
            #define __CaseBinary(cmd, dtype1, dtype2)                                       \
                case __V(xil_cmp_command_t::cmd, xil_type_t::dtype1, xil_type_t::dtype2):   \
                    return __command_manager.template get_command<                          \
                        xil_cmp_command_t::cmd, xil_type_t::dtype1, xil_type_t::dtype2      \
                    >();

            #define __C1(cmd, dtype1, dtype2)    __CaseBinary(cmd, dtype1, dtype2)
            #define __C2(cmd, dtype1, dtype2)                                               \
                __C1(cmd, dtype1, dtype2)                                                   \
                __C1(cmd, dtype2, dtype1)

            #define __CaseBinaries(cmd)                                                     \
                __C1(cmd, int8,   int8)                                                     \
                __C2(cmd, int8,   uint8)                                                    \
                                                                                            \
                __C1(cmd, uint8,  uint8)                                                    \
                                                                                            \
                __C2(cmd, int16,  int8)                                                     \
                __C2(cmd, int16,  uint8)                                                    \
                __C1(cmd, int16,  int16)                                                    \
                __C2(cmd, int16,  uint16)                                                   \
                                                                                            \
                __C2(cmd, uint16, int8)                                                     \
                __C2(cmd, uint16, uint8)                                                    \
                __C1(cmd, uint16, uint16)                                                   \
                                                                                            \
                __C2(cmd, int32,  int8)                                                     \
                __C2(cmd, int32,  uint8)                                                    \
                __C2(cmd, int32,  int16)                                                    \
                __C2(cmd, int32,  uint16)                                                   \
                __C1(cmd, int32,  int32)                                                    \
                                                                                            \
                __C2(cmd, uint32, int8)                                                     \
                __C2(cmd, uint32, uint8)                                                    \
                __C2(cmd, uint32, int16)                                                    \
                __C2(cmd, uint32, uint16)                                                   \
                __C2(cmd, uint32, int32)                                                    \
                __C1(cmd, uint32, uint32)                                                   \
                                                                                            \
                __C2(cmd, int64,  int8)                                                     \
                __C2(cmd, int64,  uint8)                                                    \
                __C2(cmd, int64,  int16)                                                    \
                __C2(cmd, int64,  uint16)                                                   \
                __C2(cmd, int64,  int32)                                                    \
                __C2(cmd, int64,  uint32)                                                   \
                __C1(cmd, int64,  int64)                                                    \
                                                                                            \
                __C2(cmd, uint64,  int8)                                                    \
                __C2(cmd, uint64,  uint8)                                                   \
                __C2(cmd, uint64,  int16)                                                   \
                __C2(cmd, uint64,  uint16)                                                  \
                __C2(cmd, uint64,  int32)                                                   \
                __C2(cmd, uint64,  uint32)                                                  \
                __C2(cmd, uint64,  int64)                                                   \
                __C1(cmd, uint64,  uint64)

            __CaseBinaries(greater)
            __CaseBinaries(greater_equal)
            __CaseBinaries(less)
            __CaseBinaries(less_equal)
            __CaseBinaries(equal)
            __CaseBinaries(not_equal)

            __C1(equal, bool_, bool_)
            __C1(not_equal, bool_, bool_)
            __C1(equal, char_, char_)
            __C1(not_equal, char_, char_)

            #undef __C1
            #undef __C2
            #undef __CaseBinaries
            #undef __CaseFloatBinaries
            #undef __CaseBinary

            default:
                X_UNEXPECTED();
        }
    }

    namespace
    {
        template<typename t, size_t size> struct __copy_array_t
        {
            static void copy_array(const t * src, t * dst)
            {
                *dst = *src;
                __copy_array_t<t, size - 1>::copy_array(src + 1, dst + 1);
            }
        };

        template<typename t> struct __copy_array_t<t, 0>
        {
            static void copy_array(const t * src, t * dst) { }
        };
    }

    template<size_t size, typename t> void copy_array(const t * src, t * dst)
    {
        __copy_array_t<t, size>::copy_array(src, dst);
    }

    ////////// ////////// ////////// ////////// //////////

    #define __ToRetCmdValue(_ret_size)                                                  \
        __ToCmdValue(smp, ((cmd_value_t)xil_smp_t::ret << 8) | _ret_size)

    class __ret_command_base_t : public __command_base_t { };

    template<msize_t _ret_size>
    class __ret_command_t : public __ret_command_base_t
    {
    public:
        __ret_command_t(msize_t local_unit_size, msize_t param_unit_size)
            : __total_unit_size(param_unit_size + __stack_stub_size + local_unit_size)
            , __param_unit_ret_size(param_unit_size - _ret_size)
            , __total_unit_ret_size(__total_unit_size - _ret_size)
        { }

        __BeginExecute(ctx, __ToRetCmdValue(_ret_size))

            // param, stub, local, ret
            rt_stack_unit_t * p = ctx.stack.pop(__total_unit_size);
            ctx.pop_calling((const __calling_stub_t *)(p + __param_unit_ret_size));
            copy_array<_ret_size>(p + __total_unit_ret_size, p - _ret_size);

        __EndExecute()

        virtual const string_t to_string(command_execute_context_t & ctx) const override
        {
            return _T("ret");
        }

    private:
        int __total_unit_size;      // param_unit_size + __stack_stub_size + local_unit_size
        int __param_unit_ret_size;  // param_unit_size - _ret_size
        int __total_unit_ret_size;  // __total_unit_size - _ret_size
    };

    //-------- ---------- ---------- ---------- ----------

    const msize_t __large_ret_unit_size = max_value<msize_t>();

    template<>
    class __ret_command_t<__large_ret_unit_size> : public __ret_command_base_t
    {
    public:
        __ret_command_t(msize_t local_unit_size, msize_t param_unit_size, msize_t ret_unit_size)
            : __total_unit_size(param_unit_size + __stack_stub_size + local_unit_size)
            , __param_unit_ret_size(param_unit_size - ret_unit_size)
            , __total_unit_ret_size(__total_unit_size - ret_unit_size)
            , __ret_unit_size(ret_unit_size)
        { }

        __BeginExecute(ctx, __ToRetCmdValue(256))

            // param, stub, local, ret
            rt_stack_unit_t * p = ctx.stack.pop(__total_unit_size);
            ctx.pop_calling((const __calling_stub_t *)(p + __param_unit_ret_size));

            // TODO: optimize
            for(rt_stack_unit_t * dst = p - __ret_unit_size, * dst_end = dst + __ret_unit_size,
                * src = p + __total_unit_ret_size; dst < dst_end;)
            {
                *dst++ = *src++;
            }

        __EndExecute()

    private:
        msize_t __total_unit_size;      // param_unit_size + __stack_stub_size + local_unit_size
        msize_t __param_unit_ret_size;  // param_unit_size - _ret_size
        msize_t __total_unit_ret_size;  // __total_unit_size - _ret_size
        msize_t __ret_unit_size;
    };

    //-------- ---------- ---------- ---------- ----------

    struct __ret_command_template_t
    {
        template<msize_t msize, typename ... args_t>
        static auto new_command(memory_t * memory, args_t && ... args)
        {
            typedef __ret_command_t<msize> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<args_t>(args) ...);
        }
    };

    command_t * new_ret_command(__context_t & ctx)
    {
        static __command_manager_t<
            __ret_command_template_t, msize_t
        >::with_args_t<msize_t, msize_t> __ret_command_manager;

        msize_t ret_unit_size = ctx.ret_unit_size();

        switch(ret_unit_size)
        {
            #define __CaseRet(size)                                                     \
                case size:                                                              \
                    return __ret_command_manager.template get_command<size>(            \
                        ctx.locals_layout.unit_size(), ctx.params_layout.unit_size()    \
                    );

            __CaseRet(0)
            __CaseRet(1)
            __CaseRet(2)
            __CaseRet(3)
            __CaseRet(4)
            __CaseRet(5)
            __CaseRet(6)
            __CaseRet(7)
            __CaseRet(8)

            #undef __CaseRet

            default:
                return __ret_command_manager.template get_command<__large_ret_unit_size>(
                    ctx.locals_layout.unit_size(), ctx.params_layout.unit_size(), ret_unit_size
                );
        }
    }

    bool is_ret_command(command_t * command)
    {
        _A(command != nullptr);

        return is<__ret_command_base_t *>(command);
    }

    //-------- ---------- ---------- ---------- ----------

    class __label_command_t : public __command_base_t
    {
    public:
        __BeginExecute(ctx, __ToCmdValue(smp, xil_smp_t::label))

            // do nothing

        __EndExecute()

    } __label_command;

    //-------- ---------- ---------- ---------- ----------

    static command_t * __new_smp_command(__context_t & ctx, const smp_xil_t & xil)
    {
        switch(xil.smp())
        {
            case xil_smp_t::ret:
                return new_ret_command(ctx);

            case xil_smp_t::empty:
                return &__empty_command;

            case xil_smp_t::label:
                return &__label_command;

            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    template<xil_jmp_condition_t condition> struct __jmp_condition_t { };

    template<> struct __jmp_condition_t<xil_jmp_condition_t::none>
    {
        __AlwaysInline bool operator()(command_execute_context_t & ctx)
        {
            return true;
        }

        operator string_t() const
        {
            return _T("jmp");
        }
    };

    //-------- ---------- ---------- ---------- ----------

    template<> struct __jmp_condition_t<xil_jmp_condition_t::true_>
    {
        __AlwaysInline bool operator()(command_execute_context_t & ctx)
        {
            //_P(_T("jmp true: "), ctx.stack.pick<bool>());
            return ctx.stack.pop<bool>();
        }

        operator string_t() const
        {
            return _T("jmp true");
        }
    };

    //-------- ---------- ---------- ---------- ----------

    template<> struct __jmp_condition_t<xil_jmp_condition_t::false_>
    {
        __AlwaysInline bool operator()(command_execute_context_t & ctx)
        {
            //_P(_T("jmp false: "), ctx.stack.pick<bool>());
            return !ctx.stack.pop<bool>();
        }

        operator string_t() const
        {
            return _T("jmp false");
        }
    };

    //-------- ---------- ---------- ---------- ----------

    template<xil_jmp_condition_t condition>
    class __jmp_command_t : public __command_base_t
    {
        typedef __jmp_condition_t<condition> __jmp_condition_t;

    public:
        __jmp_command_t(int step) : __step(step) { }

        __BeginExecute(ctx, __ToCmdValue(jmp, condition))

            if(__jmp_condition_t()(ctx))
            {
                ctx.current += __step;
            }

        __EndExecute()

        virtual const string_t to_string(command_execute_context_t & ctx) const override
        {
            return _F(_T("%1% %2%"), __jmp_condition_t(), __step);
        }

    private:
        int __step;
    };

    //-------- ---------- ---------- ---------- ----------

    struct __jmp_command_template_t
    {
        template<xil_jmp_condition_t condition, typename ... args_t>
        static auto new_command(memory_t * memory, args_t && ... args)
        {
            typedef __jmp_command_t<condition> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    class __switch_jmp_command_t : public __command_base_t
    {
    public:
        __switch_jmp_command_t(exec_switch_table_t * table) : __table(table) { }

        __BeginExecute(ctx, __ToCmdValue(jmp, xil_jmp_condition_t::switch_))

            int32_t value = ctx.stack.pop<int32_t>();

            exec_switch_row_t * rows = (exec_switch_row_t *)__table->rows;
            exec_switch_row_t * rows_end = rows + __table->row_count;

            exec_switch_row_t * row = __binary_search(rows, rows_end, value);

            if(row != nullptr)
                ctx.current += row->step - 1;
            else
                ctx.current += __table->default_step - 1;

        __EndExecute()

        virtual const string_t to_string(command_execute_context_t & ctx) const override
        {
            return _T("switch");
        }

    private:
        exec_switch_table_t * __table;

        __AlwaysInline static exec_switch_row_t * __binary_search(
                exec_switch_row_t * begin, exec_switch_row_t * end, int32_t value)
        {
            if(begin == end)
                return nullptr;

            exec_switch_row_t * r = begin + (end - begin) / 2;
            if(value < r->value)
                return __binary_search(begin, r, value);

            if(value > r->value)
                return __binary_search(r + 1, end, value);

            return r;
        }
    };

    static command_t * __new_jmp_command(__context_t & ctx, const jmp_xil_t & xil)
    {
        static __command_manager_t<
            __jmp_command_template_t, xil_jmp_condition_t
        >::with_args_t<int> __command_manager;

        typedef xil_jmp_condition_t condition_t;

        switch(xil.condition())
        {
            #define __Case(condition)                                           \
                case xil_jmp_condition_t::condition:                            \
                    return __command_manager.template get_command<condition_t::condition>( \
                        xil.step() - 1                                          \
                    );

            __Case(none)
            __Case(true_)
            __Case(false_)

            #undef __Case

            case xil_jmp_condition_t::switch_:
                return __new_command<__switch_jmp_command_t>(
                    (memory_t *)&__command_heap, ctx.switch_manager.get_table(xil.tbl())
                );

            default:
                X_UNEXPECTED();
        }
    }

    template<xil_new_type_t _new_type> class __new_command_t { };

    template<>
    class __new_command_t<xil_new_type_t::default_> : public __command_base_t
    {
    public:
        __new_command_t(rt_type_t * type) : __type(type) { }

        __BeginExecute(ctx, __ToCmdValue(new_, xil_new_type_t::default_))

            __pre_new(ctx, __type);
            ctx.stack.push<rt_ref_t>(ctx.heap->new_obj(__type));

        __EndExecute()

        virtual const string_t to_string(command_execute_context_t & ctx) const override
        {
            return _F(_T("new %1%"), __assembly_analyzer(ctx, __type).get_name(__type));
        }

    private:
        rt_type_t * __type;
    };

    //-------- ---------- ---------- ---------- ----------

    template<>
    class __new_command_t<xil_new_type_t::array> : public __command_base_t
    {
    public:
        __new_command_t(rt_type_t * type) : __type(_M(rt_array_type_t *, type)) { }

        __BeginExecute(ctx, __ToCmdValue(new_, xil_new_type_t::array))

            __pre_new(ctx, __type);
            dimension_t dimension = (*__type)->dimension;

            array_length_t lengths[dimension];
            for(int * len = lengths + dimension - 1; len >= lengths; len--)
            {
                *len = ctx.stack.pop<int32_t>();
            }

            ctx.stack.push(
                ctx.heap->new_array(__type, dimension, lengths)
            );

        __EndExecute()

        virtual const string_t to_string(command_execute_context_t & ctx) const override
        {
            return _F(_T("new array %1%"), __assembly_analyzer(ctx, __type).get_name(__type));
        }

    private:
        rt_array_type_t * __type;
    };

    //-------- ---------- ---------- ---------- ----------

    struct __new_command_template_t
    {
        template<xil_new_type_t new_type, typename ... args_t>
        static auto new_command(memory_t * memory, args_t && ... args)
        {
            typedef __new_command_t<new_type> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    static command_t * __new_new_command(__context_t & ctx, const new_xil_t & xil)
    {
        static __command_manager_t<
            __new_command_template_t, xil_new_type_t
        >::with_args_t<rt_type_t *> __command_manager;

        ref_t type_ref = xil.type_ref();

        rt_type_t * type = ctx.get_type(type_ref);
        if(type == nullptr)
            throw _ED(exec_error_code_t::type_not_found, type_ref);

        switch(xil.new_type())
        {
            case xil_new_type_t::default_:
                return __command_manager.template get_command<xil_new_type_t::default_>(
                    __Mv(type)
                );

            case xil_new_type_t::array:
                return __command_manager.template get_command<xil_new_type_t::array>(
                    __Mv(type)
                );

            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    class __copy_command_base_t : public __command_base_t
    {
    public:
        virtual const string_t to_string(command_execute_context_t & ctx) const override
        {
            return _T("copy");
        }
    };

    //-------- ---------- ---------- ---------- ----------

    template<xil_copy_type_t _copy_type> class __copy_command_t { };

    //-------- ---------- ---------- ---------- ----------

    template<>
    class __copy_command_t<xil_copy_type_t::stack_copy> : public __copy_command_base_t
    {
        typedef __copy_command_base_t __super_t;

    public:
        __copy_command_t(xil_type_t dtype) : __dtype(dtype) { }

        __BeginExecute(ctx, __ToCmdValue(copy, xil_copy_type_t::stack_copy))
            
        __EndExecute()

    private:
        xil_type_t __dtype;
    };

    //-------- ---------- ---------- ---------- ----------

    template<>
    class __copy_command_t<xil_copy_type_t::block_copy> : public __copy_command_base_t
    {
        typedef __copy_command_base_t __super_t;

    public:
        __copy_command_t() { }

        __BeginExecute(ctx, __ToCmdValue(copy, xil_copy_type_t::block_copy))
            
        __EndExecute()
    };

    //-------- ---------- ---------- ---------- ----------

    template<>
    class __copy_command_t<xil_copy_type_t::res_copy> : public __copy_command_base_t
    {
        typedef __copy_command_base_t __super_t;

    public:
        __copy_command_t(res_t res) : __res(res) { }

        __BeginExecute(ctx, __ToCmdValue(copy, xil_copy_type_t::res_copy))
            
        __EndExecute()

    private:
        res_t __res;
    };

    //-------- ---------- ---------- ---------- ----------

    template<xil_copy_type_t _copy_type>
    struct __copy_command_template_t
    {
        template<__nokey_t _nokey, typename ... _args_t>
        static auto new_command(memory_t * memory, _args_t && ... args)
        {
            typedef __copy_command_t<_copy_type> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<_args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    static command_t * __new_copy_command(__context_t & ctx, const copy_xil_t & xil)
    {
        static __command_manager_t<
            __copy_command_template_t<xil_copy_type_t::stack_copy>, __nokey_t
        >::with_args_t<xil_type_t> __stack_copy_command_manager;

        static __command_manager_t<
            __copy_command_template_t<xil_copy_type_t::res_copy>, __nokey_t
        >::with_args_t<res_t> __res_copy_command_manager;

        static __copy_command_t<xil_copy_type_t::block_copy> __block_copy_command;

        switch(xil.copy_type())
        {
            case xil_copy_type_t::stack_copy:
                return __stack_copy_command_manager.template get_command<__nokey>(xil.dtype());

            case xil_copy_type_t::block_copy:
                return &__block_copy_command;

            case xil_copy_type_t::res_copy:
                return __res_copy_command_manager.template get_command<__nokey>(xil.res());

            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    #undef __Local

    ////////// ////////// ////////// ////////// //////////

    command_t * new_command(command_creating_context_t & ctx, const xil_base_t * xil)
    {
        //_P(_T(">> new_command: "), (xil_command_t)xil->command());
        switch(xil->command())
        {
            case xil_command_t::empty:
                return &__empty_command;

            case xil_command_t::push:
                return __new_push_command(ctx, *(const push_xil_t *)xil);

            case xil_command_t::pop:
                return __new_pop_command(ctx, *(const pop_xil_t *)xil);

            case xil_command_t::pick:
                return __new_pick_command(ctx, *(const pick_xil_t *)xil);

            case xil_command_t::call:
                return __new_call_command(ctx, *(const call_xil_t *)xil);

            case xil_command_t::al:
                return __new_al_command(ctx, *(const al_xil_t *)xil);

            case xil_command_t::bit:
                return __new_bit_command(ctx, *(const bit_xil_t *)xil);

            case xil_command_t::logic:
                return __new_logic_command(ctx, *(const logic_xil_t *)xil);

            case xil_command_t::cmp:
                return __new_cmp_command(ctx, *(const cmp_xil_t *)xil);

            case xil_command_t::smp:
                return __new_smp_command(ctx, *(const smp_xil_t *)xil);

            case xil_command_t::jmp:
                return __new_jmp_command(ctx, *(const jmp_xil_t *)xil);

            case xil_command_t::new_:
                return __new_new_command(ctx, *(const new_xil_t *)xil);

            case xil_command_t::copy:
                return __new_copy_command(ctx, *(const copy_xil_t *)xil);

            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    command_t * new_end_command()
    {
        return &__execute_end_command;
    }

    ////////// ////////// ////////// ////////// //////////

    void __execute_commands(command_execute_context_t & ctx)
    {
        #define __Next()                                                                \
              goto __begin__;

        #define __Case(_t, _ts...)                                                      \
            case _t, ##_ts::cmd_value:                                                  \
                ((_t, ##_ts *)command)->do_execute(ctx);                                \
                __Next();

        command_t ** & commands = ctx.current;

        // while(true)
        // {
        //     (*commands++)->execute(ctx);
        // }

    __begin__:

        command_t * command = *commands++;

        switch(command->cmd_value)
        {
            __Case( __push_command_t<xil_storage_type_t::constant, xil_type_t::int32> )
            __Case( __push_command_t<xil_storage_type_t::constant, xil_type_t::string> )
            __Case( __push_command_t<xil_storage_type_t::local, xil_type_t::int32> )
            __Case( __push_command_t<xil_storage_type_t::argument, xil_type_t::int32> )

            __Case( __pop_command_t<xil_storage_type_t::local, xil_type_t::int32> )

            // al
            __Case( __binary_command_t<
                xil_al_command_t, xil_al_command_t::add, xil_type_t::int32, xil_type_t::int32
            > )

            __Case( __binary_command_t<
                xil_cmp_command_t, xil_cmp_command_t::less, xil_type_t::int32, xil_type_t::int32
            > )

            // Jmp
            __Case( __jmp_command_t<xil_jmp_condition_t::none> )
            __Case( __jmp_command_t<xil_jmp_condition_t::true_> )
            __Case( __jmp_command_t<xil_jmp_condition_t::false_> )
            __Case( __switch_jmp_command_t )


            // Call
            __Case( __internal_call_command_t<__nokey> )
            __Case( __static_call_command_t<rt_method_t> )
            __Case( __instance_call_command_t<rt_method_t> )
            __Case( __virtual_call_command_t<__nokey> )

            __Case( __ret_command_t<0> )
            __Case( __ret_command_t<1> )

            __Case( __execute_end_command_t )

            default:
                _P(_T("running command:"), command->to_string(ctx));
                command->execute(ctx);
                __Next();
        }

        #undef __Case
        #undef __Next
    }

    void execute_commands(command_execute_context_t & ctx, command_t ** commands)
    {
        ctx.current = commands;

        try
        {
            __execute_commands(ctx);
        }
        catch(logic_error_t<exec_env_code_t> & e)
        {
            // End
        }
    }

    ////////// ////////// ////////// ////////// //////////


} } }
