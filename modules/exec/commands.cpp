#include <exec.h>
#include "commands.h"
#include <rtlib.h>
#include <mm.h>
#include "utils.h"
#include "parser.h"

namespace X_ROOT_NS { namespace modules { namespace exec {

    #define __AlwaysInline  X_ALWAYS_INLINE
    #define __Mv(x)         std::move(x)

    #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_MANUAL_METHOD

    #define __This  (static_cast<__self_t *>(command))
    static std::set<command_t *> __ret_commands;

    #define __BeginExecute(_ctx, _cmd_value)                                            \
                                                                                        \
        static void execute(command_t * command, command_execute_context_t & _ctx)      \
        {

    #define __BeginToString()                                                           \
        static const string_t to_string(command_t * command)                            \
        {

    #elif EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_INLINE   // EXEC_EXECUTE_MODEL == xxx

    #define __This  this

    #define __BeginExecute(_ctx, _cmd_value)                                            \
                                                                                        \
        static const cmd_value_t cmd_value = (cmd_value_t)_cmd_value;                   \
                                                                                        \
        __AlwaysInline void execute(command_execute_context_t & _ctx)                   \
        {

    #define __BeginToString()                                                           \
        __AlwaysInline const string_t to_string()                                       \
        {

    #elif EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL_METHOD // EXEC_EXECUTE_MODEL == xxx

    #define __This  this

    #define __BeginExecute(_ctx, _cmd_value)                                            \
                                                                                        \
        virtual void execute(command_execute_context_t & ctx) override                  \
        {

    #define __BeginToString()                                                           \
                                                                                        \
        virtual const string_t to_string() override                                     \
        {

    #else
        
        #error unknown EXEC_EXECUTE_MODEL

    #endif  // EXEC_EXECUTE_MODEL == xxx

    #define __EndExecute()                                                              \
        }

    #define __EndToString()                                                             \
        }

    #define __ToCmdValue(_cmd, value)                                                   \
        ( ((cmd_value_t)(xil_command_t::_cmd) << 24) | (cmd_value_t)(value) )

    #define __Cmd(value)    ((value) >> 24)

    ////////// ////////// ////////// ////////// //////////

    using namespace rt;
    using namespace rtlib;

    void __execute_command(command_execute_context_t & ctx, command_t * command);
    const string_t to_command_string(command_t * command);

    ////////// ////////// ////////// ////////// //////////
    // command_creating_context_t

    command_creating_context_t::command_creating_context_t(executor_env_t & env,
        rt_assembly_t * assembly, rt_type_t * type, rt_method_t * method,
        const generic_param_manager_t * gp_manager)
        : __super_t(env, assembly, gp_manager), type(type), method(method), env(env)
        , locals_layout(*this), params_layout(*this)
    {
        _A(assembly != nullptr);
        _A(type != nullptr);
        _A(method != nullptr);
    }

    // Converts template to a generic type. (with cache)
    rt_type_t * command_creating_context_t::to_generic_type(rt_general_type_t * template_)
    {
        _A(template_ != nullptr);

        auto it = __generic_type_map.find(template_);
        if (it != __generic_type_map.end())
            return it->second;

        rt_type_t * type = __super_t::to_generic_type(template_);
        __generic_type_map[template_] = type;

        return type;
    }

    typedef command_creating_context_t __context_t;
    typedef method_xil_block_type_t __block_type_t;

    ////////// ////////// ////////// ////////// //////////

    typedef unsigned long long __nokey_t;
    const __nokey_t __nokey = (__nokey_t)0;

    typedef rt_stack_unit_t __unit_t;
    const msize_t __stack_stub_size = unit_size_of(sizeof(__calling_stub_t));

    static al::xheap_t __command_heap;

    template<typename _cmd_t, typename ... _args_t>
    _cmd_t * __new_command(memory_t * memory, _args_t && ... args)
    {
        _cmd_t * cmd = memory_t::new_obj<_cmd_t>(memory, std::forward<_args_t>(args) ...);

        #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_MANUAL_METHOD
        
        ((command_t *)cmd)->execute_method = (command_execute_method_t)_cmd_t::execute;

        #if EXEC_TRACE
        ((command_t *)cmd)->to_string_method = (command_to_string_method_t)_cmd_t::to_string;
        #endif  // EXEC_TRACE

        #elif EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_INLINE   // EXEC_EXECUTE_MODEL == xxx

        ((command_t *)cmd)->cmd_value = _cmd_t::cmd_value;

        #elif EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL_METHOD // EXEC_EXECUTE_MODEL == xxx

        // Do nothing

        #endif

        return cmd;
    }

    #define __ReturnInstance(_class_name)                                               \
        do {                                                                            \
            static _class_name * command = __new_command<_class_name>(&__command_heap); \
            return command;                                                             \
        } while (false)

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

        template<> struct __vnum_traits_t<xil_type_t::ptr>
        {
            typedef rt_ref_t num_t;
        };
    }

    template<xil_type_t _xil_type>
    using __vnum_t = typename __vnum_traits_t<_xil_type>::num_t;

    ////////// ////////// ////////// ////////// //////////

    template<typename t, typename _xil_t> t __read_extra(const _xil_t & xil)
    {
        typedef const xil_extra_t<_xil_t> exxil_t;
        return *(t *)((const exxil_t &)xil).extra;
    }

    typedef msize_t __offset_t;

    ////////// ////////// ////////// ////////// //////////

    msize_t command_creating_context_t::ret_unit_size()
    {
        if (__ret_unit_size == unknown_msize)
        {
            msize_t size = variable_size_of((*method)->type, &__ret_storage_type);
            __ret_unit_size = unit_size_of(size);
        }

        return __ret_unit_size;
    }

    // Returns command index.
    int __command_index(command_execute_context_t & ctx, command_t ** command)
    {
        exec_method_t * method = ctx.current_method();
        return command - method->commands;
    }

    // Returns current command index.
    int __current_command_index(command_execute_context_t & ctx)
    {
        return __command_index(ctx, ctx.current);
    }

    #if EXEC_TRACE

    // Prints current command.
    void __print_current(command_execute_context_t ctx)
    {
        _PF(_T("CURRENT: %1% [%2%] %3%"),
            ctx.current_method(), __current_command_index(ctx) - 1,
            to_command_string(*(ctx.current - 1)).c_str()
        );
    }

    #define __PrintCurrent()    __print_current(ctx)

    #else

    #define __PrintCurrent()

    #endif  // EXEC_TRACE

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
                if (it != __map.end())
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

        #if EXEC_TRACE

        __BeginToString()

            return _T("[command]");

        __EndToString()

        #endif      // EXEC_TRACE
    };

    ////////// ////////// ////////// ////////// //////////
    // Class __empty_command_t

    class __empty_command_t : public __command_base_t
    {
        typedef __empty_command_t   __self_t;

    public:

        __BeginExecute(ctx, __ToCmdValue(empty, 1))

            // Do nothing

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _T("[empty]");
        __EndToString()

        #endif      // EXEC_TRACE
    };

    ////////// ////////// ////////// ////////// //////////
    // Class __execute_end_command_t

    class __execute_end_command_t : public __command_base_t
    {
        typedef __execute_end_command_t __self_t;

    public:

        __BeginExecute(ctx, __ToCmdValue(empty, 2))

            throw _E(exec_env_code_t::end);

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()

            return _T("[end]");
        
        __EndToString()

        #endif  // EXEC_TRACE
    };

    ////////// ////////// ////////// ////////// //////////
    // Class __push_command_t

    template<xil_storage_type_t, xil_type_t, xil_type_t>
    class __push_command_t { };

    struct __push_command_template_t
    {
        template<xil_storage_type_t stype, xil_type_t dtype1, xil_type_t dtype2,
                                                            typename ... args_t>
        static auto new_command(memory_t * memory, args_t && ... args)
        {
            typedef __push_command_t<stype, dtype1, dtype2> this_command_t;
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

    static msize_t __size_of(assembly_analyzer_t & analyzer, ref_t type_ref)
    {
        if (type_ref.empty())
            return 0;

        rt_type_t * rt_type = analyzer.get_type(type_ref);

        switch ((mt_type_extra_t)type_ref.extra)
        {
            case mt_type_extra_t::type_ref:
            case mt_type_extra_t::general: {

                rt_general_type_t * t = (rt_general_type_t *)rt_type;
                size_t arg_size = get_vtype_size((vtype_t)(*t)->vtype); // TODO: struct?
                return arg_size;

            }   break;

            case mt_type_extra_t::generic: {
                X_UNEXPECTED();
            }   break;

            default:
                X_UNEXPECTED();
                return sizeof(void *);
        }
    }

    constexpr static int __unit_size_of(size_t size)
    {
        return _alignf(size, sizeof(rt_stack_unit_t)) / sizeof(rt_stack_unit_t);
    }

    static msize_t __unit_size_of(assembly_analyzer_t & analyzer, ref_t type_ref)
    {
        return __unit_size_of(__size_of(analyzer, type_ref));
    }

    // Converts type_ref to xil_type_t
    xil_type_t __to_dtype(__context_t & ctx, rt_type_t * type)
    {
        _A(type != nullptr);

        vtype_t vtype = type->get_vtype(ctx.env);
        return to_xil_type(vtype);
    }

    // Fetches the data type of specified xil.
    template<typename _xil_t>
    xil_type_t __fetch_dtype(__context_t & ctx, const _xil_t & xil)
    {
        xil_type_t dtype = xil.dtype();

        if (dtype != xil_type_t::empty)
            return dtype;

        switch (xil.stype())
        {
            case xil_storage_type_t::field:
            case xil_storage_type_t::field_addr: {

                rt_field_base_t * rt_field = ctx.get_field(xil.field_ref());
                _A(rt_field != nullptr);

                return __to_dtype(ctx, rt_field->get_field_type(ctx));
            }

            case xil_storage_type_t::constant: {

                ref_t type_ref = xil.type_ref();
                return __to_dtype(ctx, ctx.get_type(type_ref));
            }

            case xil_storage_type_t::argument:
            case xil_storage_type_t::argument_addr: {

                int index = xil.get_identity();
                rt_type_t * type = ctx.params_layout.type_at(index);
                return __to_dtype(ctx, type);
            }

            default:
                X_UNEXPECTED();
        }
    }

    //-------- ---------- ---------- ---------- ----------

    // _xil_type1: the xil type of constant value
    // _xil_type2: the xil type of stack unit

    #define __LocalAddress(offset)      ((void *)((byte_t *)ctx.stack.lp() + offset))
    #define __Local(type_t, offset)     (*((type_t *)__LocalAddress(offset)))

    #if EXEC_TRACE

    #define __PushCommand_ToString(_st, _xt1, _xt2, _v)                                 \
                                                                                        \
        __BeginToString()                                                               \
                                                                                        \
            if (xil_type_t::_xt1 == xil_type_t::_xt2)                                   \
                return _F(_T("push %1% %2% [%3%]"), xil_storage_type_t::_st,            \
                    xil_type_t::_xt1, __This->_v);                                      \
                                                                                        \
            return _F(_T("push %1% %2%=>%3% [%4%]"), xil_storage_type_t::_st,           \
                xil_type_t::_xt1, xil_type_t::_xt2, __This->_v);                        \
                                                                                        \
        __EndToString()

    #else

        #define __PushCommand_ToString(_st, _xt1, _xt2, _v)

    #endif

    #define __ToPushCmdValue(_st, _xt1, _xt2)                                           \
        __ToCmdValue(push,                                                              \
            ((cmd_value_t)xil_storage_type_t::_st << 16) |                              \
            ((cmd_value_t)_xt1 << 8) |                                                  \
            ((cmd_value_t)_xt2)                                                         \
        )

    //-------- ---------- ---------- ---------- ----------
    // Push constants

    template<xil_type_t _xt1, xil_type_t _xt2>
    class __push_command_t<xil_storage_type_t::constant, _xt1, _xt2>
        : public __command_base_t
    {
        typedef __push_command_t        __self_t;
        typedef __command_base_t        __super_t;
        typedef __const_vnum_t<_xt1>    __value1_t;
        typedef __const_vnum_t<_xt2>    __value2_t;

    public:
        __push_command_t(__value1_t value) : __value(value) { }

        __BeginExecute(ctx, __ToPushCmdValue(constant, _xt1, _xt2))

            ctx.stack.push(static_cast<__value2_t>(__This->__value));

        __EndExecute()

        __PushCommand_ToString(constant, _xt1, _xt2, __value)

    private:
        __value1_t __value;
    };

    //-------- ---------- ---------- ---------- ----------
    // Push local commands.

    template<xil_type_t _xt1, xil_type_t _xt2>
    class __push_command_t<xil_storage_type_t::local, _xt1, _xt2>
        : public __command_base_t
    {
        typedef __push_command_t    __self_t;
        typedef __vnum_t<_xt1>      __value1_t;
        typedef __vnum_t<_xt2>      __value2_t;

    public:
        __push_command_t(__offset_t offset) : __offset(offset) { }

        __PushCommand_ToString(local, _xt1, _xt2, __offset)

        __BeginExecute(ctx, __ToPushCmdValue(local, _xt1, _xt2))

            ctx.stack.push(static_cast<__value2_t>(__Local(__value1_t, __This->__offset)));

        __EndExecute()

    private:
        __offset_t __offset;
    };

    //-------- ---------- ---------- ---------- ----------
    // Push local address commands.

    template<xil_storage_type_t _stype> class __push_address_command_t { };

    #if EXEC_TRACE

    #define __PushAddressCommand_ToString(_st, _v)                                      \
                                                                                        \
        __BeginToString()                                                               \
                                                                                        \
            return _F(_T("push %1% address [%2%]"), xil_storage_type_t::_st,            \
                                                __This->_v);                            \
                                                                                        \
        __EndToString()                                                                 \
   
    #else

        #define __PushAddressCommand_ToString(_st, _v)

    #endif // EXEC_TRACE

    template<>
    class __push_address_command_t<xil_storage_type_t::local_addr>
        : public __command_base_t
    {
        typedef __push_address_command_t    __self_t;

    public:
        __push_address_command_t(__offset_t offset) : __offset(offset) { }

        __PushAddressCommand_ToString(local_addr, __offset)

        __BeginExecute(ctx, __ToPushCmdValue(local_addr, 0, 0))

            ctx.stack.push(__LocalAddress(__This->__offset));

        __EndExecute()

    private:
        __offset_t __offset;
    };

    struct __push_address_command_template_t
    {
        template<xil_storage_type_t _st, typename ... _args_t>
        static auto new_command(memory_t * memory, _args_t && ... args)
        {
            typedef __push_address_command_t<_st> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<_args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------
    // Push argument commands.

    #define __ArgumentAddress(offset) ((void *)((rt_stack_unit_t *)ctx.stack.lp() - (offset)))
    #define __Argument(type_t, offset) (*(type_t *)__ArgumentAddress(offset))

    template<xil_type_t _xt1, xil_type_t _xt2>
    class __push_command_t<xil_storage_type_t::argument, _xt1, _xt2>
        : public __command_base_t
    {
        typedef __push_command_t    __self_t;
        typedef __vnum_t<_xt1>      __value1_t;
        typedef __vnum_t<_xt2>      __value2_t;

    public:
        __push_command_t(__offset_t offset) : __offset(offset) { }

        __PushCommand_ToString(argument, _xt1, _xt2, __offset)

        __BeginExecute(ctx, __ToPushCmdValue(argument, _xt1, _xt2))

            ctx.stack.push(static_cast<__value2_t>(
                __Argument(__value1_t, __This->__offset + __stack_stub_size)
            ));

        __EndExecute()

    private:
        __offset_t __offset;
    };

    //-------- ---------- ---------- ---------- ----------
    // Push argument address commands.

    template<>
    class __push_address_command_t<xil_storage_type_t::argument_addr>
        : public __command_base_t
    {
        typedef __push_address_command_t    __self_t;

    public:
        __push_address_command_t(__offset_t offset) : __offset(offset) { }

        __PushAddressCommand_ToString(argument_addr, __offset)

        __BeginExecute(ctx, __ToPushCmdValue(argument_addr, 0, 0))

            ctx.stack.push(__ArgumentAddress(__This->__offset + __stack_stub_size));

        __EndExecute()

    private:
        __offset_t __offset;
    };

    //-------- ---------- ---------- ---------- ----------
    // Push field commands.

    static rt_type_t * __field_type(__context_t & ctx, ref_t field_ref)
    {
        switch ((mt_member_extra_t)field_ref.extra)
        {
            case mt_member_extra_t::import:
            case mt_member_extra_t::internal: {

                rt_type_t * rt_type;
                rt_field_base_t * rt_field = ctx.get_field(field_ref, &rt_type);

                _A( rt_field != nullptr );
                _A( rt_type != nullptr );

                // _A(is_general(rt_type));

                rt_type_t * rt_field_type = rt_field->get_field_type(ctx);
                _A( rt_field_type != nullptr );
                _A( is_general(rt_field_type) );

                rt_general_type_t * rt_general_field_type = (rt_general_type_t *)rt_field_type;
                if (!rt_general_field_type->is_generic_template())
                    return rt_general_field_type;

                return ctx.to_generic_type(rt_general_field_type);
            }

            case mt_member_extra_t::position: {

                rt_type_t * type = ctx.get_host_type_by_field_ref(field_ref);
                _A(type != nullptr);
                _A(type->get_kind() == rt_type_kind_t::generic);

                rt_generic_type_t * generic_type = (rt_generic_type_t *)type;
                _A(generic_type->atypes != nullptr);

                int index = field_ref.index;
                _A(index < generic_type->atype_count());

                return generic_type->atypes[index];
            }

            case mt_member_extra_t::generic: {

                rt_type_t * type = ctx.get_host_type_by_field_ref(field_ref);
                _A(type != nullptr);

                return type;
            }

            default:
                X_UNEXPECTED();
        }
    }

    static msize_t __field_offset(__context_t & ctx, ref_t field_ref)
    {
        switch ((mt_member_extra_t)field_ref.extra)
        {
            case mt_member_extra_t::import:
            case mt_member_extra_t::internal: {

                rt_type_t * rt_type;
                rt_field_base_t * rt_field = ctx.get_field(field_ref, &rt_type);

                _A(rt_field != nullptr);
                _A(rt_type != nullptr);
                _A(is_general(rt_type));

                rt_general_type_t * general_type = (rt_general_type_t *)rt_type;

                if (!general_type->is_generic_template())
                    return rt_type->get_field_offset(ctx.env, field_ref);

                rt_type_t * type = ctx.to_generic_type(general_type);
                _A(type != nullptr);

                return type->get_field_offset(ctx.env, field_ref);
            }

            case mt_member_extra_t::position: {

                rt_type_t * type = ctx.get_host_type_by_field_ref(field_ref);
                _A(type != nullptr);

                mt_position_field_t * mt = ctx.current->mt_of_position_field(field_ref);
                return type->get_field_offset(ctx.env, mt->position);
            }

            case mt_member_extra_t::generic: {

                rt_type_t * type = ctx.get_host_type_by_field_ref(field_ref);
                _A(type != nullptr);

                mt_generic_field_t * mt = ctx.current->mt_of_generic_field(field_ref);
                return type->get_field_offset(ctx.env, mt->template_);
            }

            default:
                X_UNEXPECTED();
        }
    }

    #define __FieldAddress(p, offset)       ((void *)((byte_t *)p + offset))
    #define __Field(type_t, p, offset)      (*(type_t *)__FieldAddress(p, offset))

    template<xil_type_t _xt1, xil_type_t _xt2>
    class __push_command_t<xil_storage_type_t::field, _xt1, _xt2>
        : public __command_base_t
    {
        typedef __push_command_t        __self_t;
        typedef __command_base_t        __super_t;
        typedef __const_vnum_t<_xt1>    __value1_t;
        typedef __const_vnum_t<_xt2>    __value2_t;

    public:
        __push_command_t(__offset_t offset) : __offset(offset) { }

        __BeginExecute(ctx, __ToPushCmdValue(field, _xt1, _xt2))

            ctx.stack.push(static_cast<__value2_t>(
                __Field(__value1_t, ctx.stack.pop<rt_ref_t>(), __This->__offset)
            ));

        __EndExecute()

        __PushCommand_ToString(field, _xt1, _xt2, __offset)

    private:
        __offset_t __offset;
    };

    //-------- ---------- ---------- ---------- ----------
    // Push field address commands.

    template<>
    class __push_address_command_t<xil_storage_type_t::field_addr>
        : public __command_base_t
    {
        typedef __push_address_command_t    __self_t;

    public:
        __push_address_command_t(__offset_t offset) : __offset(offset) { }

        __PushAddressCommand_ToString(field_addr, __offset)

        __BeginExecute(ctx, __ToPushCmdValue(field_addr, 0, 0))

            ctx.stack.push(
                __FieldAddress(ctx.stack.pop<rt_ref_t>(), __This->__offset)
            );

        __EndExecute()

    private:
        __offset_t __offset;
    };

    //-------- ---------- ---------- ---------- ----------

    class __duplicate_command_t : public __command_base_t
    {
        typedef __duplicate_command_t   __self_t;

    public:
        __BeginExecute(ctx, __ToPushCmdValue(duplicate, 0, 0))

            ctx.stack.push<rt_stack_unit_t>(
                ctx.stack.pick<rt_stack_unit_t>()
            );

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _T("duplicate");
        __EndToString()

        #endif  // EXEC_TRACE
    };

    //-------- ---------- ---------- ---------- ----------
    // Push convert commands..

    template<xil_type_t _xt1, xil_type_t _xt2>
    class __push_command_t<xil_storage_type_t::convert, _xt1, _xt2>
        : public __command_base_t
    {
        typedef __push_command_t    __self_t;
        typedef __vnum_t<_xt1>      __value1_t;
        typedef __vnum_t<_xt2>      __value2_t;

    public:

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("push convert %1%=>%2%"), _xt1, _xt2);
        __EndToString()

        #endif  // EXEC_TRACE

        __BeginExecute(ctx, __ToPushCmdValue(convert, _xt1, _xt2))

            if (!std::is_same<__value1_t, __value2_t>::value)
            {
                ctx.stack.push(static_cast<__value2_t>(
                    ctx.stack.pop<__value1_t>()
                ));
            }

        __EndExecute()
    };

    struct __push_convert_command_template_t
    {
        template<xil_type_t _xt1, xil_type_t _xt2, typename ... args_t>
        static auto new_command(memory_t * memory, args_t && ... args)
        {
            typedef __push_command_t<xil_storage_type_t::convert, _xt1, _xt2> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    namespace
    {
        namespace __mul_array_index_ns
        {
            typedef dimension_t         __d_t;
            typedef array_length_t      __l_t;
            typedef executor_stack_t    __stack_t;

            template<__d_t _d> struct __mul_t
            {
                X_ALWAYS_INLINE __l_t operator()(__stack_t & stack, __l_t * ls)
                {
                    return stack.pop<__l_t>() * (*ls) + __mul_t<_d - 1>()(stack, ls - 1);
                }
            };

            template<> struct __mul_t<0>
            {
                X_ALWAYS_INLINE __l_t operator()(__stack_t & stack, __l_t * p)
                {
                    return 0;
                }
            };
        }

        template<dimension_t _dimension>
        X_ALWAYS_INLINE array_length_t __mul_array_index(executor_stack_t & stack,
                                                         array_length_t * lengths)
        {
            return __mul_array_index_ns::__mul_t<_dimension>()(stack, lengths);
        }
    }

    __AlwaysInline static array_length_t __array_index(executor_stack_t & stack,
                                dimension_t dimension, array_length_t * lengths)
    {
        array_length_t index = 0;
        const dimension_t partical_dimension = 8;

        while (true)
        {
            switch (dimension)
            {
                case 0: return index;

                #define __Case(_d)                                                      \
                    case _d:                                                            \
                        return index + __mul_array_index<_d>(stack, lengths);

                __Case(2)
                __Case(3)
                __Case(4)
                __Case(5)
                __Case(6)
                __Case(7)
                __Case(8)

                #undef __Case

                default:
                    index += __mul_array_index<partical_dimension>(stack, lengths);
                    lengths -= partical_dimension;
                    dimension -= partical_dimension;
                    continue;
            }
        }
    }

    //-------- ---------- ---------- ---------- ----------

    #define __RtTypeOf(rt_ref)  (mm::get_object_type(rt_ref))

    #define __ToPushArrayElementCmdValue(_xil_type1, _xil_type2, _dimension) (          \
        __ToCmdValue(push,                                                              \
            ((cmd_value_t)xil_storage_type_t::array_element << 16)                      \
            | (((cmd_value_t)_xil_type1) << 12)                                         \
            | (((cmd_value_t)_xil_type2) << 8)                                          \
        )   | (cmd_value_t)_dimension                                                   \
    )

    template<xil_type_t _xil_type1, xil_type_t _xil_type2, int _dimension>
    class __push_array_element_command_t : public __command_base_t
    {
        typedef __push_array_element_command_t  __self_t;
        typedef __vnum_t<_xil_type1>            __value1_t;
        typedef __vnum_t<_xil_type2>            __value2_t;

    public:
        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("push array element"));
        __EndToString()

        #endif  // EXEC_TRACE

        __BeginExecute(ctx, __ToPushArrayElementCmdValue(_xil_type1, _xil_type2, _dimension))

            rt_ref_t array_ref = ctx.stack.pop<rt_ref_t>(); 
            array_length_t index = ctx.stack.pop<array_length_t>();

            if (_dimension >= 2)
            {
                array_length_t * lengths = mm::get_array_lengths(array_ref);
                index += __mul_array_index<_dimension - 1>(ctx.stack, lengths);
            }

            ctx.stack.push<__value2_t>(static_cast<__value2_t>(
                mm::get_array_element<__value1_t>(array_ref, index)
            ));

        __EndExecute()
    };

    //-------- ---------- ---------- ---------- ----------

    template<xil_type_t _xil_type1, xil_type_t _xil_type2>
    class __push_array_element_command_t<_xil_type1, _xil_type2, 0> : public __command_base_t
    {
        typedef __vnum_t<_xil_type1>            __value1_t;
        typedef __vnum_t<_xil_type2>            __value2_t;
        typedef __push_array_element_command_t  __self_t;

    public:

        __push_array_element_command_t(dimension_t dimension) : __dimension(dimension) { }

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("push array element"));
        __EndToString()

        #endif  // EXEC_TRACE

        __BeginExecute(ctx, __ToPushArrayElementCmdValue(_xil_type1, _xil_type2, 0))

            rt_ref_t array_ref = ctx.stack.pop<rt_ref_t>(); 
            array_length_t index = ctx.stack.pop<array_length_t>();
            array_length_t * lengths = mm::get_array_lengths(array_ref);

            index += __array_index(ctx.stack, __This->__dimension - 1, lengths);

            ctx.stack.push<__value2_t>(static_cast<__value2_t>(
                mm::get_array_element<__value1_t>(array_ref, index)
            ));

        __EndExecute()

    private:
        dimension_t __dimension;
    };

    //-------- ---------- ---------- ---------- ----------

    struct __push_array_element_command_template_t
    {
        template<xil_type_t _xt1, xil_type_t _xt2, typename ... _args_t>
        static auto new_command(memory_t * memory, int dimension, _args_t && ... args)
            -> command_t *
        {
            #define __NewCommand(_dimension)                                            \
                __new_command<__push_array_element_command_t<_xt1, _xt2, _dimension>>(  \
                    memory, std::forward<_args_t>(args) ...                             \
                )

            switch (dimension)
            {
                #define __Case(_dimension)                                              \
                    case _dimension:                                                    \
                        return __NewCommand(_dimension);

                __Case(1)
                __Case(2)
                __Case(3)
                __Case(4)
                __Case(5)
                __Case(6)
                __Case(7)
                __Case(8)

                default:
                    return __new_command<__push_array_element_command_t<_xt1, _xt2, 0>>(
                        memory, dimension, std::forward<_args_t>(args) ...
                    );

                #undef __Case
            }

            #undef __NewCommand
        }
    };

    //-------- ---------- ---------- ---------- ----------

    static rt_array_type_t * __array_type(__context_t & ctx, ref_t type_ref)
    {
        rt_type_t * rt_type = ctx.get_type(type_ref);
        _A(rt_type->get_kind() == rt_type_kind_t::array);

        return (rt_array_type_t *)rt_type;
    }

    static rt_type_t * __array_element_type(__context_t & ctx, ref_t type_ref)
    {
        rt_array_type_t * array_type = __array_type(ctx, type_ref);
        rt_type_t * element_type = array_type->element_type;

        _A(element_type != nullptr);

        return element_type;
    }

    static dimension_t __array_dimension(__context_t & ctx, ref_t type_ref)
    {
        return __array_type(ctx, type_ref)->dimension;
    }

    //-------- ---------- ---------- ---------- ----------

    class __push_params_address_command_t : public __command_base_t
    {
        typedef __push_params_address_command_t __self_t;

    public:
        __push_params_address_command_t(__offset_t offset)
            : __offset(offset)
        { }

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("push params argument address"));
        __EndToString()

        #endif  // EXEC_TRACE

        __BeginExecute(ctx, __ToCmdValue(push, ((cmd_value_t)xil_storage_type_t::params << 16)))

            ctx.stack.push<rt_stack_unit_t *>(
                (rt_stack_unit_t *)ctx.stack.lp() - __This->__offset - 1
            );

        __EndExecute()

    private:
        __offset_t __offset;
    };

    struct __push_params_address_template_t
    {
        template<__nokey_t nokey, typename ... args_t>
        static auto new_command(memory_t * memory, args_t && ... args)
        {
            typedef __push_params_address_command_t this_command_t;
            return __new_command<this_command_t>(memory, std::forward<args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    static xil_type_t __xil_type_of(__context_t & ctx, rt_type_t * type)
    {
        _A(type != nullptr);

        vtype_t vtype = type->get_vtype(ctx.env);
        return to_xil_type(vtype);
    }

    template<typename _xil_t>
    void __analyze_xil_types(__context_t & ctx, const _xil_t & xil,
                            xil_type_t * out_dtype1, xil_type_t * out_dtype2)
    {
        xil_storage_type_t stype = xil.stype();
        *out_dtype2 = __fetch_dtype(ctx, xil);

        // Gets xil_type of data source.
        switch (stype)
        {
            case xil_storage_type_t::local:
            case xil_storage_type_t::local_addr:
                *out_dtype1 = __xil_type_of(ctx, ctx.locals_layout.type_at(xil.get_identity()));
                break;

            case xil_storage_type_t::argument:
            case xil_storage_type_t::argument_addr:
                *out_dtype1 = __xil_type_of(ctx, ctx.params_layout.type_at(xil.get_identity()));
                break;

            case xil_storage_type_t::field:
            case xil_storage_type_t::field_addr:
                *out_dtype1 = __xil_type_of(ctx, __field_type(ctx, xil.field_ref()));
                break;

            case xil_storage_type_t::array_element:
            case xil_storage_type_t::array_element_addr:
                *out_dtype1 = __xil_type_of(ctx, __array_element_type(ctx, xil.type_ref()));
                break;

            default:
                *out_dtype1 = *out_dtype2;
                break;
        }
    }

    static command_t * __new_push_command(__context_t & ctx, const push_xil_t & xil)
    {
        typedef xil_storage_type_t stype_t;

        #define __CommandManagerT(v_t)                                                  \
            __command_manager_t<                                                        \
                __push_command_template_t, xil_storage_type_t, xil_type_t, xil_type_t   \
            >::with_args_t<v_t>

        #define __DefineConstantCommandManagerT(name)                                   \
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
        static __CommandManagerT(void *)            __object_push_command_manager;

        static __command_manager_t<
            __push_command_template_t, xil_storage_type_t, xil_type_t, xil_type_t
        >::with_args_t<__offset_t> __command_manager;

        static __command_manager_t<
            __push_address_command_template_t, xil_storage_type_t
        >::with_args_t<__offset_t> __push_address_command_manager;

        static __command_manager_t<
            __push_array_element_command_template_t, xil_type_t, xil_type_t
        >::with_args_t<dimension_t> __array_element_command_manager;

        static __command_manager_t<
            __push_params_address_template_t, __nokey_t
        >::with_args_t<__offset_t> __params_address_command_manager;

        static __command_manager_t<
            __push_convert_command_template_t, xil_type_t, xil_type_t
        >::with_args_t<> __convert_command_manager;

        xil_storage_type_t stype = xil.stype();
        xil_type_t dtype1, dtype2;

        switch (stype)
        {
            case xil_storage_type_t::duplicate:     // Duplicate command.
                __ReturnInstance(__duplicate_command_t);

            case xil_storage_type_t::params:        // Push params argument command.
                return __params_address_command_manager.template get_command<__nokey>(
                    ctx.params_layout.extends_offset()
                );

            case xil_storage_type_t::local_addr:    // Push local address command.
                return __push_address_command_manager.template get_command<stype_t::local_addr>(
                    ctx.locals_layout.offset_of(xil.get_identity())
                );

            case xil_storage_type_t::argument_addr: // Push agrument address command.
                return __push_address_command_manager.template get_command<stype_t::argument_addr>(
                    ctx.params_layout.offset_of(xil.get_identity())
                );

            case xil_storage_type_t::convert:
                dtype1 = xil.dtype();
                dtype2 = xil.dtype2();
                break;

            default:
                __analyze_xil_types(ctx, xil, &dtype1, &dtype2);
                break;
        }

        #define __V(s, d1, d2) (((uint32_t)(s) << 16)                                   \
            | ((uint32_t)(d1) << 12) | ((uint32_t)(d2)) << 8)

        #define __ConstantValue(_xil, _t)                                               \
            (_xil.dtype() == xil_type_t::empty?                                         \
                default_value_of<_t>() : __read_extra<_t>(_xil))

        #define __ConstantStringValue(_xil)                                             \
            (_xil.dtype() == xil_type_t::empty?                                         \
                default_value_of<const char_t *>() : ctx.to_cstr(__read_extra<res_t>(_xil)))


        switch (__V(stype, dtype1, dtype2))
        {
            // Constant values.
            #define __CaseConstant(_name, _t1, _d1, _t2, _d2)                           \
                case __V(xil_storage_type_t::constant, xil_type_t::_d1, xil_type_t::_d2):\
                    return _name##_push_command_manager.template get_command<           \
                        xil_storage_type_t::constant, xil_type_t::_d1, xil_type_t::_d2  \
                    >(__ConstantValue(xil, _t1));

            case __V(xil_storage_type_t::constant, xil_type_t::string, xil_type_t::string):
                return __string_push_command_manager.template get_command<
                    xil_storage_type_t::constant, xil_type_t::string, xil_type_t::string
                >(__ConstantStringValue(xil));

            case __V(xil_storage_type_t::constant, xil_type_t::object, xil_type_t::object):
                return __object_push_command_manager.template get_command<
                    xil_storage_type_t::constant, xil_type_t::object, xil_type_t::object
                >(ctx.get_type(xil.type_ref()));

            #define __CaseConstants(_t2, _d2)                                           \
                                                                                        \
                __CaseConstant(int8,    int8_t,     int8,   _t2,    _d2)                \
                __CaseConstant(uint8,   uint8_t,    uint8,  _t2,    _d2)                \
                                                                                        \
                __CaseConstant(int16,   int16_t,    int16,  _t2,    _d2)                \
                __CaseConstant(uint16,  uint16_t,   uint16, _t2,    _d2)                \
                                                                                        \
                __CaseConstant(int32,   int32_t,    int32,  _t2,    _d2)                \
                __CaseConstant(uint32,  uint32_t,   uint32, _t2,    _d2)                \
                                                                                        \
                __CaseConstant(int64,   int64_t,    int64,  _t2,    _d2)                \
                __CaseConstant(uint64,  uint64_t,   uint64, _t2,    _d2)                \
                                                                                        \
                __CaseConstant(float,   float_t,    float_, _t2,    _d2)                \
                __CaseConstant(double,  double_t,   double_, _t2,   _d2)                \
                                                                                        \
                __CaseConstant(bool,    bool_t,     bool_,  _t2,    _d2)                \
                __CaseConstant(char,    char_t,     char_,  _t2,    _d2)


            __CaseConstants(int8_t,     int8)
            __CaseConstants(uint8_t,    uint8)

            __CaseConstants(int16_t,    int16)
            __CaseConstants(uint16_t,   uint16)

            __CaseConstants(int32_t,    int32)
            __CaseConstants(uint32_t,   uint32)

            __CaseConstants(int64_t,    int64)
            __CaseConstants(uint64_t,   uint64)

            __CaseConstants(float_t,    float_)
            __CaseConstants(double_t,   double_)

            __CaseConstants(bool_t,     bool_)
            __CaseConstants(char_t,     char_)

            #undef __CaseConstant
            #undef __CaseConstants

            // - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Push Local

            #define __CaseLocal(_d1, _d2)                                               \
                case __V(xil_storage_type_t::local, xil_type_t::_d1, xil_type_t::_d2):  \
                    return __command_manager.template get_command<                      \
                        xil_storage_type_t::local, xil_type_t::_d1, xil_type_t::_d2     \
                    >(ctx.locals_layout.offset_of(xil.get_identity()));

            #define __CaseLocals(_d2)                                                   \
                                                                                        \
                __CaseLocal(int8,       _d2)                                            \
                __CaseLocal(uint8,      _d2)                                            \
                                                                                        \
                __CaseLocal(int16,      _d2)                                            \
                __CaseLocal(uint16,     _d2)                                            \
                                                                                        \
                __CaseLocal(int32,      _d2)                                            \
                __CaseLocal(uint32,     _d2)                                            \
                                                                                        \
                __CaseLocal(int64,      _d2)                                            \
                __CaseLocal(uint64,     _d2)                                            \
                                                                                        \
                __CaseLocal(float_,     _d2)                                            \
                __CaseLocal(double_,    _d2)                                            \
                                                                                        \
                __CaseLocal(char_,      _d2)                                            \
                __CaseLocal(bool_,      _d2)


            __CaseLocals(int8)
            __CaseLocals(uint8)

            __CaseLocals(int16)
            __CaseLocals(uint16)

            __CaseLocals(int32)
            __CaseLocals(uint32)

            __CaseLocals(int64)
            __CaseLocals(uint64)

            __CaseLocals(float_)
            __CaseLocals(double_)

            __CaseLocals(char_)
            __CaseLocals(bool_)

            __CaseLocal(object, object)
            __CaseLocal(string, string)
            __CaseLocal(ptr,    ptr)

            #undef __CaseLocal
            #undef __CaseLocals

            // - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Push Argument

            #define __CaseArgument(_d1, _d2)                                            \
                case __V(xil_storage_type_t::argument, xil_type_t::_d1, xil_type_t::_d2):\
                    return __command_manager.template get_command<                      \
                        xil_storage_type_t::argument, xil_type_t::_d1, xil_type_t::_d2  \
                    >(ctx.params_layout.offset_of(xil.get_identity()));


            #define __CaseArguments(_d2)                                                \
                                                                                        \
                __CaseArgument(int8,    _d2)                                            \
                __CaseArgument(uint8,   _d2)                                            \
                                                                                        \
                __CaseArgument(int16,   _d2)                                            \
                __CaseArgument(uint16,  _d2)                                            \
                                                                                        \
                __CaseArgument(int32,   _d2)                                            \
                __CaseArgument(uint32,  _d2)                                            \
                                                                                        \
                __CaseArgument(int64,   _d2)                                            \
                __CaseArgument(uint64,  _d2)                                            \
                                                                                        \
                __CaseArgument(float_,  _d2)                                            \
                __CaseArgument(double_, _d2)                                            \
                                                                                        \
                __CaseArgument(char_,   _d2)                                            \
                __CaseArgument(bool_,   _d2)

            __CaseArguments(int8)
            __CaseArguments(uint8)

            __CaseArguments(int16)
            __CaseArguments(uint16)

            __CaseArguments(int32)
            __CaseArguments(uint32)

            __CaseArguments(int64)
            __CaseArguments(uint64)

            __CaseArguments(float_)
            __CaseArguments(double_)

            __CaseArguments(char_)
            __CaseArguments(bool_)

            __CaseArgument(object,  object)
            __CaseArgument(string,  string)
            __CaseArgument(ptr,     ptr)

            #undef __CaseArgument
            #undef __CaseArguments

            // - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Push Field

            #define __CaseField(_d1, _d2)                                               \
                case __V(xil_storage_type_t::field, xil_type_t::_d1, xil_type_t::_d2):  \
                    return __command_manager.template get_command<                      \
                        xil_storage_type_t::field, xil_type_t::_d1, xil_type_t::_d2     \
                    >(__field_offset(ctx, xil.field_ref()));

            #define __CaseFields(_d2)                                                   \
                                                                                        \
                __CaseField(int8,       _d2)                                            \
                __CaseField(uint8,      _d2)                                            \
                                                                                        \
                __CaseField(int16,      _d2)                                            \
                __CaseField(uint16,     _d2)                                            \
                                                                                        \
                __CaseField(int32,      _d2)                                            \
                __CaseField(uint32,     _d2)                                            \
                                                                                        \
                __CaseField(int64,      _d2)                                            \
                __CaseField(uint64,     _d2)                                            \
                                                                                        \
                __CaseField(float_,     _d2)                                            \
                __CaseField(double_,    _d2)                                            \
                                                                                        \
                __CaseField(char_,      _d2)                                            \
                __CaseField(bool_,      _d2)


            __CaseFields(int8)
            __CaseFields(uint8)

            __CaseFields(int16)
            __CaseFields(uint16)

            __CaseFields(int32)
            __CaseFields(uint32)

            __CaseFields(int64)
            __CaseFields(uint64)

            __CaseFields(float_)
            __CaseFields(double_)

            __CaseFields(char_)
            __CaseFields(bool_)


            __CaseField(object,     object)
            __CaseField(string,     string)
            __CaseField(ptr,        ptr)

            #undef __CaseField
            #undef __CaseFields

            // - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Push array element

            #define __CaseArrayElement(_d1, _d2)                                        \
                case __V(xil_storage_type_t::array_element, xil_type_t::_d1, xil_type_t::_d2):  \
                    return __array_element_command_manager.template get_command<        \
                       xil_type_t::_d1, xil_type_t::_d2                                 \
                    >(__array_dimension(ctx, xil.type_ref()));

            #define __CaseArrayElements(_d2)                                            \
                                                                                        \
                __CaseArrayElement(int8,        _d2)                                    \
                __CaseArrayElement(uint8,       _d2)                                    \
                                                                                        \
                __CaseArrayElement(int16,       _d2)                                    \
                __CaseArrayElement(uint16,      _d2)                                    \
                                                                                        \
                __CaseArrayElement(int32,       _d2)                                    \
                __CaseArrayElement(uint32,      _d2)                                    \
                                                                                        \
                __CaseArrayElement(int64,       _d2)                                    \
                __CaseArrayElement(uint64,      _d2)                                    \
                                                                                        \
                __CaseArrayElement(float_,      _d2)                                    \
                __CaseArrayElement(double_,     _d2)                                    \
                                                                                        \
                __CaseArrayElement(char_,       _d2)                                    \
                __CaseArrayElement(bool_,       _d2)

            __CaseArrayElements(int8)
            __CaseArrayElements(uint8)

            __CaseArrayElements(int16)
            __CaseArrayElements(uint16)

            __CaseArrayElements(int32)
            __CaseArrayElements(uint32)

            __CaseArrayElements(int64)
            __CaseArrayElements(uint64)

            __CaseArrayElements(float_)
            __CaseArrayElements(double_)

            __CaseArrayElements(char_)
            __CaseArrayElements(bool_)

            __CaseArrayElement(object,  object)
            __CaseArrayElement(string,  string)
            __CaseArrayElement(ptr,     ptr)

            #undef __CaseArrayElement
            #undef __CaseArrayElements

            // - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Push Convert

            #define __CaseConvert(_d1, _d2)                                             \
                case __V(xil_storage_type_t::convert, xil_type_t::_d1, xil_type_t::_d2):\
                    return __convert_command_manager.template get_command<              \
                        xil_type_t::_d1, xil_type_t::_d2                                \
                    >();

            #define __CaseConverts(_d2)                                                 \
                                                                                        \
                __CaseConvert(int8,       _d2)                                          \
                __CaseConvert(uint8,      _d2)                                          \
                                                                                        \
                __CaseConvert(int16,      _d2)                                          \
                __CaseConvert(uint16,     _d2)                                          \
                                                                                        \
                __CaseConvert(int32,      _d2)                                          \
                __CaseConvert(uint32,     _d2)                                          \
                                                                                        \
                __CaseConvert(int64,      _d2)                                          \
                __CaseConvert(uint64,     _d2)                                          \
                                                                                        \
                __CaseConvert(float_,     _d2)                                          \
                __CaseConvert(double_,    _d2)                                          \
                                                                                        \
                __CaseConvert(char_,      _d2)                                          \
                __CaseConvert(bool_,      _d2)


            __CaseConverts(int8)
            __CaseConverts(uint8)

            __CaseConverts(int16)
            __CaseConverts(uint16)

            __CaseConverts(int32)
            __CaseConverts(uint32)

            __CaseConverts(int64)
            __CaseConverts(uint64)

            __CaseConverts(float_)
            __CaseConverts(double_)

            __CaseConverts(char_)
            __CaseConverts(bool_)

            __CaseConvert(object, object)
            __CaseConvert(string, string)
            __CaseConvert(ptr,    ptr)

            // - - - - - - - - - - - - - - - - - - - - - - - - - -

            default:
                _PF(_T("push %1% %2% %3%"), xil.stype(), dtype1, dtype2);
                X_UNEXPECTED();
        }

        #undef __V
        #undef __ConstantStringValue
        #undef __ConstantValue
    }

    ////////// ////////// ////////// ////////// //////////
    // Class __pop_command_t

    #define __ToPopCmdValue(_stype, _xil_type1, _xil_type2)                             \
        __ToCmdValue(pop,                                                               \
            ((cmd_value_t)xil_storage_type_t::_stype << 16) |                           \
            ((cmd_value_t)_xil_type1 << 8) |                                            \
            ((cmd_value_t)_xil_type2)                                                   \
        )

    template<xil_storage_type_t _st, xil_type_t _xt1, xil_type_t _xt2>
    class __pop_command_t { };

    //-------- ---------- ---------- ---------- ----------

    #if EXEC_TRACE

    #define __PopCommand_ToString(_st, _xt1, _xt2, _v)                                  \
        __BeginToString()                                                               \
                                                                                        \
            if (xil_type_t::_xt1 == xil_type_t::_xt2)                                   \
                return _F(_T("pop  %1% %2% [%3%]"), xil_storage_type_t::_st,            \
                    xil_type_t::_xt1, _v);                                              \
                                                                                        \
            return _F(_T("pop  %1% %2%=>%3% [%4%]"), xil_storage_type_t::_st,           \
                        xil_type_t::_xt2, xil_type_t::_xt1, _v);                        \
        __EndToString()

    #else
        
    #define __PopCommand_ToString(_st, _xt1, _xt2, _v)

    #endif  // EXEC_TRACE

    //-------- ---------- ---------- ---------- ----------
    // Pop local commands

    template<xil_type_t _xt1, xil_type_t _xt2>
    class __pop_command_t<xil_storage_type_t::local, _xt1, _xt2>
        : public __command_base_t
    {
        typedef __pop_command_t     __self_t;
        typedef __vnum_t<_xt1>      __value1_t;
        typedef __vnum_t<_xt2>      __value2_t;

    public:
        __pop_command_t(__offset_t offset) : __offset(offset) { }

        __PopCommand_ToString(local, _xt1, _xt2, __This->__offset)

        __BeginExecute(ctx, __ToPopCmdValue(local, _xt1, _xt2))

            __Local(__value1_t, __This->__offset) = static_cast<__value1_t>(
                ctx.stack.pop<__value2_t>()
            );

        __EndExecute()

    private:
        __offset_t __offset;
    };

    //-------- ---------- ---------- ---------- ----------
    // Pop argument commands

    template<xil_type_t _xt1, xil_type_t _xt2>
    class __pop_command_t<xil_storage_type_t::argument, _xt1, _xt2>
        : public __command_base_t
    {
        typedef __pop_command_t     __self_t;
        typedef __vnum_t<_xt1>      __value1_t;
        typedef __vnum_t<_xt2>      __value2_t;

    public:
        __pop_command_t(__offset_t offset) : __offset(offset) { }

        __PopCommand_ToString(argument, _xt1, _xt2, __This->__offset)

        __BeginExecute(ctx, __ToPopCmdValue(argument, _xt1, _xt2))

            __Argument(__value1_t, __This->__offset + __stack_stub_size) =
                static_cast<__value1_t>(ctx.stack.pop<__value2_t>());

        __EndExecute()

    private:
        __offset_t __offset;
    };

    //-------- ---------- ---------- ---------- ----------

    template<xil_type_t _xt1, xil_type_t _xt2>
    class __pop_command_t<xil_storage_type_t::field, _xt1, _xt2>
        : public __command_base_t
    {
        typedef __pop_command_t     __self_t;
        typedef __vnum_t<_xt1>      __value1_t;
        typedef __vnum_t<_xt2>      __value2_t;

    public:
        __pop_command_t(__offset_t offset) : __offset(offset) { }

        __PopCommand_ToString(field, _xt1, _xt2, __This->__offset)

        __BeginExecute(ctx, __ToPopCmdValue(field, _xt1, _xt2))

            void * field = __FieldAddress(ctx.stack.pop<rt_ref_t>(), __This->__offset);
            *(__value1_t *)field = static_cast<__value1_t>(ctx.stack.pop<__value2_t>());

        __EndExecute()

    private:
        __offset_t __offset;
    };

    //-------- ---------- ---------- ---------- ----------

    struct __pop_command_template_t
    {
        template<xil_storage_type_t _stype, xil_type_t _dtype1, xil_type_t _dtype2,
                                                            typename ... _args_t>
        static auto new_command(memory_t * memory, _args_t && ... args)
        {
            typedef __pop_command_t<_stype, _dtype1, _dtype2> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<_args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    #define __ToPopArrayElementCmdValue(_xil_type1, _xil_type2, _dimension) (           \
        __ToCmdValue(pop,                                                               \
            ((cmd_value_t)xil_storage_type_t::array_element << 16)                      \
            | (((cmd_value_t)_xil_type1) << 12)                                         \
            | (((cmd_value_t)_xil_type2) << 8)                                          \
        )   | (cmd_value_t)_dimension                                                   \
    )

    template<xil_type_t _xil_type1, xil_type_t _xil_type2, int _dimension>
    class __pop_array_element_command_t : public __command_base_t
    {
        typedef __vnum_t<_xil_type1> __value1_t;
        typedef __vnum_t<_xil_type2> __value2_t;

    public:

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("pop array element"));
        __EndToString()

        #endif  // EXEC_TRACE

        __BeginExecute(ctx, __ToPopArrayElementCmdValue(_xil_type1, _xil_type2, _dimension))

            rt_ref_t array_ref = ctx.stack.pop<rt_ref_t>(); 
            array_length_t index = ctx.stack.pop<array_length_t>();

            if (_dimension >= 2)
            {
                array_length_t * lengths = mm::get_array_lengths(array_ref);
                index += __mul_array_index<_dimension - 1>(ctx.stack, lengths);
            }

            mm::set_array_element<__value1_t>(
                array_ref, index, static_cast<__value1_t>(ctx.stack.pop<__value2_t>())
            );

        __EndExecute()
    };

    //-------- ---------- ---------- ---------- ----------

    template<xil_type_t _xil_type1, xil_type_t _xil_type2>
    class __pop_array_element_command_t<_xil_type1, _xil_type2, 0> : public __command_base_t
    {
        typedef __pop_array_element_command_t   __self_t;
        typedef __vnum_t<_xil_type1>            __value1_t;
        typedef __vnum_t<_xil_type2>            __value2_t;

    public:

        __pop_array_element_command_t(dimension_t dimension) : __dimension(dimension) { }

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("pop array element"));
        __EndToString()

        #endif  // EXEC_TRACE

        __BeginExecute(ctx, __ToPopArrayElementCmdValue(_xil_type1, _xil_type2, 0))

            rt_ref_t array_ref = ctx.stack.pop<rt_ref_t>(); 
            array_length_t index = ctx.stack.pop<array_length_t>();
            array_length_t * lengths = mm::get_array_lengths(array_ref);

            index += __array_index(ctx.stack, __This->__dimension - 1, lengths);

            mm::set_array_element<__value1_t>(
                array_ref, index, static_cast<__value1_t>(ctx.stack.pop<__value2_t>())
            );

        __EndExecute()

    private:
        dimension_t __dimension;
    };

    //-------- ---------- ---------- ---------- ----------

    struct __pop_array_element_command_template_t
    {
        template<xil_type_t _dtype1, xil_type_t _dtype2, typename ... _args_t>
        static auto new_command(memory_t * memory, int dimension, _args_t && ... args)
            -> command_t *
        {
            #define __NewCommand(_dimension)                                            \
                __new_command<__pop_array_element_command_t<_dtype1, _dtype2, _dimension>>(   \
                    memory, std::forward<_args_t>(args) ...                             \
                )

            switch (dimension)
            {
                #define __Case(_dimension)                                              \
                    case _dimension:                                                    \
                        return __NewCommand(_dimension);

                __Case(1)
                __Case(2)
                __Case(3)
                __Case(4)
                __Case(5)
                __Case(6)
                __Case(7)
                __Case(8)

                default:
                    return __new_command<__pop_array_element_command_t<_dtype1, _dtype2, 0>>(
                        memory, dimension, std::forward<_args_t>(args) ...
                    );

                #undef __Case
            }

            #undef __NewCommand
        }
    };

    //-------- ---------- ---------- ---------- ----------

    template<__nokey_t>
    class __pop_empty_command_t : public __command_base_t
    {
        typedef __pop_empty_command_t   __self_t;
    public:
        __pop_empty_command_t(msize_t unit_size) : __unit_size(unit_size) { }

        __BeginExecute(ctx, __ToPopCmdValue(empty, 0, 0))

            ctx.stack.pop(__This->__unit_size);

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

        if (xil.dtype() != xil_type_t::empty)
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
            __pop_command_template_t, xil_storage_type_t, xil_type_t, xil_type_t
        >::with_args_t<msize_t> __command_manager;

        static __command_manager_t<
            __pop_empty_command_template_t, __nokey_t
        >::with_args_t<msize_t> __empty_command_manager;

        static __command_manager_t<
            __pop_array_element_command_template_t, xil_type_t, xil_type_t
        >::with_args_t<dimension_t> __array_element_command_manager;

        if (xil.stype() == xil_storage_type_t::empty)
        {
            return __empty_command_manager.template get_command<__nokey>(
                __unit_size_of_pop_empty(ctx, xil)
            );
        }

        #define __V(_s, _d1, _d2)                                                       \
            (((uint32_t)(_s) << 16) | ((uint32_t)(_d1) << 12) | ((uint32_t)(_d2)) << 8)

        xil_type_t dtype1, dtype2;
        __analyze_xil_types(ctx, xil, &dtype1, &dtype2);

        xil_storage_type_t stype = xil.stype();

        switch (__V(stype, dtype1, dtype2))
        {
            #define __Case(_s, _d1, _d2)                                                \
                case __V(xil_storage_type_t::_s, xil_type_t::_d1, xil_type_t::_d2):     \
                    return __command_manager.template get_command<                      \
                        xil_storage_type_t::_s, xil_type_t::_d1, xil_type_t::_d2        \
                    >(ctx.locals_layout.offset_of(xil.get_identity()));

            // - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Pop Local

            #define __CaseLocal(_d1, _d2)  __Case(local, _d1, _d2)

            #define __CaseLocals(_d2)                                                   \
                                                                                        \
                __CaseLocal(int8,       _d2)                                            \
                __CaseLocal(uint8,      _d2)                                            \
                                                                                        \
                __CaseLocal(int16,      _d2)                                            \
                __CaseLocal(uint16,     _d2)                                            \
                                                                                        \
                __CaseLocal(int32,      _d2)                                            \
                __CaseLocal(uint32,     _d2)                                            \
                                                                                        \
                __CaseLocal(int64,      _d2)                                            \
                __CaseLocal(uint64,     _d2)                                            \
                                                                                        \
                __CaseLocal(float_,     _d2)                                            \
                __CaseLocal(double_,    _d2)                                            \
                                                                                        \
                __CaseLocal(char_,      _d2)                                            \
                __CaseLocal(bool_,      _d2)

            __CaseLocals(int8)
            __CaseLocals(uint8)

            __CaseLocals(int16)
            __CaseLocals(uint16)

            __CaseLocals(int32)
            __CaseLocals(uint32)

            __CaseLocals(int64)
            __CaseLocals(uint64)

            __CaseLocals(float_)
            __CaseLocals(double_)

            __CaseLocals(char_)
            __CaseLocals(bool_)

            __CaseLocal(object, object)
            __CaseLocal(string, string)
            __CaseLocal(ptr,    ptr)

            #undef __CaseLocal
            #undef __CaseLocals

            // - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Pop Argument

            #define __CaseArgument(_d1, _d2)                                            \
                case __V(xil_storage_type_t::argument, xil_type_t::_d1, xil_type_t::_d2):\
                    return __command_manager.template get_command<                      \
                        xil_storage_type_t::argument, xil_type_t::_d1, xil_type_t::_d2  \
                    >(ctx.params_layout.offset_of(xil.get_identity()));

            #define __CaseArguments(_d2)                                                \
                                                                                        \
                __CaseArgument(int8,    _d2)                                            \
                __CaseArgument(uint8,   _d2)                                            \
                                                                                        \
                __CaseArgument(int16,   _d2)                                            \
                __CaseArgument(uint16,  _d2)                                            \
                                                                                        \
                __CaseArgument(int32,   _d2)                                            \
                __CaseArgument(uint32,  _d2)                                            \
                                                                                        \
                __CaseArgument(int64,   _d2)                                            \
                __CaseArgument(uint64,  _d2)                                            \
                                                                                        \
                __CaseArgument(float_,  _d2)                                            \
                __CaseArgument(double_, _d2)                                            \
                                                                                        \
                __CaseArgument(char_,   _d2)                                            \
                __CaseArgument(bool_,   _d2)


            __CaseArguments(int8)
            __CaseArguments(uint8)

            __CaseArguments(int16)
            __CaseArguments(uint16)

            __CaseArguments(int32)
            __CaseArguments(uint32)

            __CaseArguments(int64)
            __CaseArguments(uint64)

            __CaseArguments(float_)
            __CaseArguments(double_)

            __CaseArguments(char_)
            __CaseArguments(bool_)

            __CaseArgument(object,  object)
            __CaseArgument(string,  string)
            __CaseArgument(ptr,     ptr)

            #undef __CaseArgument
            #undef __CaseArguments
            
            // - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Pop Field

            #define __CaseField(_d1, _d2)                                               \
                case __V(xil_storage_type_t::field, xil_type_t::_d1, xil_type_t::_d2):  \
                    return __command_manager.template get_command<                      \
                        xil_storage_type_t::field, xil_type_t::_d1, xil_type_t::_d2     \
                    >(__field_offset(ctx, xil.field_ref()));

            #define __CaseFields(_d2)                                                   \
                                                                                        \
                __CaseField(int8,       _d2)                                            \
                __CaseField(uint8,      _d2)                                            \
                                                                                        \
                __CaseField(int16,      _d2)                                            \
                __CaseField(uint16,     _d2)                                            \
                                                                                        \
                __CaseField(int32,      _d2)                                            \
                __CaseField(uint32,     _d2)                                            \
                                                                                        \
                __CaseField(int64,      _d2)                                            \
                __CaseField(uint64,     _d2)                                            \
                                                                                        \
                __CaseField(float_,     _d2)                                            \
                __CaseField(double_,    _d2)                                            \
                                                                                        \
                __CaseField(char_,      _d2)                                            \
                __CaseField(bool_,      _d2)

            __CaseFields(int8)
            __CaseFields(uint8)

            __CaseFields(int16)
            __CaseFields(uint16)

            __CaseFields(int32)
            __CaseFields(uint32)

            __CaseFields(int64)
            __CaseFields(uint64)

            __CaseFields(float_)
            __CaseFields(double_)

            __CaseFields(char_)
            __CaseFields(bool_)

            __CaseField(object, object)
            __CaseField(string, string)
            __CaseField(ptr,    ptr)

            #undef __CaseField
            #undef __CaseFields

            // - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Pop array element

            #define __CaseArrayElement(_d1, _d2)                                        \
                case __V(xil_storage_type_t::array_element, xil_type_t::_d1, xil_type_t::_d2):  \
                    return __array_element_command_manager.template get_command<        \
                        xil_type_t::_d1, xil_type_t::_d2                                \
                    >(__array_dimension(ctx, xil.type_ref()));                          \
                    break;

            #define __CaseArrayElements(_d2)                                            \
                                                                                        \
                __CaseArrayElement(int8,        _d2)                                    \
                __CaseArrayElement(uint8,       _d2)                                    \
                                                                                        \
                __CaseArrayElement(int16,       _d2)                                    \
                __CaseArrayElement(uint16,      _d2)                                    \
                                                                                        \
                __CaseArrayElement(int32,       _d2)                                    \
                __CaseArrayElement(uint32,      _d2)                                    \
                                                                                        \
                __CaseArrayElement(int64,       _d2)                                    \
                __CaseArrayElement(uint64,      _d2)                                    \
                                                                                        \
                __CaseArrayElement(float_,      _d2)                                    \
                __CaseArrayElement(double_,     _d2)                                    \
                                                                                        \
                __CaseArrayElement(char_,       _d2)                                    \
                __CaseArrayElement(bool_,       _d2)


            __CaseArrayElements(int8)
            __CaseArrayElements(uint8)

            __CaseArrayElements(int16)
            __CaseArrayElements(uint16)

            __CaseArrayElements(int32)
            __CaseArrayElements(uint32)

            __CaseArrayElements(int64)
            __CaseArrayElements(uint64)

            __CaseArrayElements(float_)
            __CaseArrayElements(double_)

            __CaseArrayElements(char_)
            __CaseArrayElements(bool_)

            __CaseArrayElement(object,  object)
            __CaseArrayElement(string,  string)
            __CaseArrayElement(ptr,     ptr)

            #undef __CaseArrayElement
            #undef __CaseArrayElements

            // - - - - - - - - - - - - - - - - - - - - - - - - - -

            #undef __Case

            default:
                _PF(_T("pop %1% %2% %3%"), stype, dtype1, dtype2);
                X_UNEXPECTED();
        }

        #undef __V
    }

    ////////// ////////// ////////// ////////// //////////
    // Class __pick_command_t

    #define __ToPickCmdValue(_stype, _xil_type1, _xil_type2)                            \
        __ToCmdValue(pick,                                                              \
            ((cmd_value_t)xil_storage_type_t::_stype << 16) |                           \
            ((cmd_value_t)_xil_type1 << 8) |                                            \
            ((cmd_value_t)_xil_type2)                                                   \
        )

    template<xil_storage_type_t _st, xil_type_t _xt1, xil_type_t _xt2>
    class __pick_command_t { };

    //-------- ---------- ---------- ---------- ----------

    #if EXEC_TRACE

    #define __PickCommand_ToString(_st, _xt1, _xt2, _v)                                 \
                                                                                        \
        __BeginToString()                                                               \
                                                                                        \
            if (xil_type_t::_xt1 == xil_type_t::_xt2)                                   \
                return _F(_T("pick %1% %2% [%3%]"), xil_storage_type_t::_st,            \
                    xil_type_t::_xt1, _v);                                              \
                                                                                        \
            return _F(_T("pick %1% %2%=>%3% [%4%]"), xil_storage_type_t::_st,           \
                        xil_type_t::_xt2, xil_type_t::_xt1, _v);                        \
                                                                                        \
        __EndToString()                                                                 \

    #else

    #define __PickCommand_ToString(_st, _xt1, _xt2, _v)

    #endif  // EXEC_TRACE

    //-------- ---------- ---------- ---------- ----------
    // Pick local commands

    template<xil_type_t _xt1, xil_type_t _xt2>
    class __pick_command_t<xil_storage_type_t::local, _xt1, _xt2>
        : public __command_base_t
    {
        typedef __pick_command_t        __self_t;
        typedef __vnum_t<_xt1>          __value1_t;
        typedef __vnum_t<_xt2>          __value2_t;

    public:
        __pick_command_t(__offset_t offset) : __offset(offset) { }

        __PickCommand_ToString(local, _xt1, _xt2, __This->__offset)

        __BeginExecute(ctx, __ToPickCmdValue(local, _xt1, _xt2))

            __Local(__value1_t, __This->__offset) = static_cast<__value1_t>(
                ctx.stack.pick<__value2_t>()
            );

        __EndExecute()

    private:
        __offset_t __offset;
    };

    //-------- ---------- ---------- ---------- ----------
    // Pick argument commands

    template<xil_type_t _xt1, xil_type_t _xt2>
    class __pick_command_t<xil_storage_type_t::argument, _xt1, _xt2>
        : public __command_base_t
    {
        typedef __pick_command_t        __self_t;
        typedef __vnum_t<_xt1>          __value1_t;
        typedef __vnum_t<_xt2>          __value2_t;

    public:
        __pick_command_t(__offset_t offset) : __offset(offset) { }

        __PickCommand_ToString(argument, _xt1, _xt2, __This->__offset)

        __BeginExecute(ctx, __ToPickCmdValue(argument, _xt1, _xt2))

            __Argument(__value1_t, __This->__offset + __stack_stub_size) =
                static_cast<__value1_t>(ctx.stack.pick<__value2_t>());

        __EndExecute()

    private:
        __offset_t __offset;
    };

    //-------- ---------- ---------- ---------- ----------
    // Pick field commands

    template<xil_type_t _xt1, xil_type_t _xt2>
    class __pick_command_t<xil_storage_type_t::field, _xt1, _xt2>
        : public __command_base_t
    {
        typedef __pick_command_t        __self_t;
        typedef __vnum_t<_xt1>          __value1_t;
        typedef __vnum_t<_xt2>          __value2_t;

    public:
        __pick_command_t(__offset_t offset) : __offset(offset) { }

        __PickCommand_ToString(field, _xt1, _xt2, __This->__offset)

        __BeginExecute(ctx, __ToPickCmdValue(field, _xt1, _xt2))

            void * field = __FieldAddress(ctx.stack.pop<rt_ref_t>(), __This->__offset);
            *(__value1_t *)field = static_cast<__value1_t>(ctx.stack.pick<__value2_t>());

        __EndExecute()

    private:
        __offset_t __offset;
    };

    //-------- ---------- ---------- ---------- ----------

    struct __pick_command_template_t
    {
        template<xil_storage_type_t _stype, xil_type_t _dtype1, xil_type_t _dtype2,
                                                                typename ... _args_t>
        static auto new_command(memory_t * memory, _args_t && ... args)
        {
            typedef __pick_command_t<_stype, _dtype1, _dtype2> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<_args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    #define __ToPickArrayElementCmdValue(_xil_type1, _xil_type2, _dimension) (          \
        __ToCmdValue(pick,                                                              \
            ((cmd_value_t)xil_storage_type_t::array_element << 16)                      \
            | (((cmd_value_t)_xil_type1) << 12)                                         \
            | (((cmd_value_t)_xil_type2) << 8)                                          \
        )   | (cmd_value_t)_dimension                                                   \
    )


    template<xil_type_t _xil_type1, xil_type_t _xil_type2, int _dimension>
    class __pick_array_element_command_t : public __command_base_t
    {
        typedef __pick_array_element_command_t  __self_t;
        typedef __vnum_t<_xil_type1>            __value1_t;
        typedef __vnum_t<_xil_type2>            __value2_t;

    public:

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("pick array element"));
        __EndToString()

        #endif      // EXEC_TRACE

        __BeginExecute(ctx, __ToPickArrayElementCmdValue(_xil_type1, _xil_type2, _dimension))

            rt_ref_t array_ref = ctx.stack.pop<rt_ref_t>(); 
            array_length_t index = ctx.stack.pop<array_length_t>();

            if (_dimension >= 2)
            {
                array_length_t * lengths = mm::get_array_lengths(array_ref);
                index += __mul_array_index<_dimension - 1>(ctx.stack, lengths);
            }

            mm::set_array_element<__value1_t>(
                array_ref, index, static_cast<__value1_t>(ctx.stack.pick<__value2_t>())
            );

        __EndExecute()
    };

    //-------- ---------- ---------- ---------- ----------

    template<xil_type_t _xil_type1, xil_type_t _xil_type2>
    class __pick_array_element_command_t<_xil_type1, _xil_type2, 0> : public __command_base_t
    {
        typedef __pick_array_element_command_t  __self_t;
        typedef __vnum_t<_xil_type1>            __value1_t;
        typedef __vnum_t<_xil_type2>            __value2_t;

    public:

        __pick_array_element_command_t(dimension_t dimension) : __dimension(dimension) { }

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("pick array element"));
        __EndToString()

        #endif  // EXEC_TRACE

        __BeginExecute(ctx, __ToPickArrayElementCmdValue(_xil_type1, _xil_type2, 0))

            rt_ref_t array_ref = ctx.stack.pop<rt_ref_t>(); 
            array_length_t index = ctx.stack.pop<array_length_t>();
            array_length_t * lengths = mm::get_array_lengths(array_ref);

            index += __array_index(ctx.stack, __This->__dimension - 1, lengths);

            mm::set_array_element<__value1_t>(
                array_ref, index, static_cast<__value1_t>(ctx.stack.pick<__value2_t>())
            );

        __EndExecute()

    private:
        dimension_t __dimension;
    };

    //-------- ---------- ---------- ---------- ----------

    struct __pick_array_element_command_template_t
    {
        template<xil_type_t _dtype1, xil_type_t _dtype2, typename ... _args_t>
        static auto new_command(memory_t * memory, int dimension, _args_t && ... args)
            -> command_t *
        {
            #define __NewCommand(_dimension)                                            \
                __new_command<__pick_array_element_command_t<_dtype1, _dtype2, _dimension>>(   \
                    memory, std::forward<_args_t>(args) ...                             \
                )

            switch (dimension)
            {
                #define __Case(_dimension)                                              \
                    case _dimension:                                                    \
                        return __NewCommand(_dimension);

                __Case(1)
                __Case(2)
                __Case(3)
                __Case(4)
                __Case(5)
                __Case(6)
                __Case(7)
                __Case(8)

                default:
                    return __new_command<__pick_array_element_command_t<_dtype1, _dtype2, 0>>(
                        memory, dimension, std::forward<_args_t>(args) ...
                    );

                #undef __Case
            }

            #undef __NewCommand
        }
    };


    //-------- ---------- ---------- ---------- ----------

    static command_t * __new_pick_command(__context_t & ctx, const pick_xil_t & xil)
    {
        static __command_manager_t<
            __pick_command_template_t, xil_storage_type_t, xil_type_t, xil_type_t
        >::with_args_t<msize_t> __command_manager;

        static __command_manager_t<
            __pick_array_element_command_template_t, xil_type_t, xil_type_t
        >::with_args_t<dimension_t> __array_element_command_manager;

        #define __V(_s, _d1, _d2)                                                       \
            (((uint32_t)(_s) << 16) | ((uint32_t)(_d1) << 12) | ((uint32_t)(_d2)) << 8)

        xil_type_t dtype1, dtype2;
        __analyze_xil_types(ctx, xil, &dtype1, &dtype2);

        xil_storage_type_t stype = xil.stype();

        switch (__V(stype, dtype1, dtype2))
        {
            #define __Case(_s, _d1, _d2)                                                \
                case __V(xil_storage_type_t::_s, xil_type_t::_d1, xil_type_t::_d2):     \
                    return __command_manager.template get_command<                      \
                        xil_storage_type_t::_s, xil_type_t::_d1, xil_type_t::_d2        \
                    >(ctx.locals_layout.offset_of(xil.get_identity()));

            // - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Pick Local

            #define __CaseLocal(_d1, _d2)  __Case(local, _d1, _d2)

            #define __CaseLocals(_d2)                                                   \
                                                                                        \
                __CaseLocal(int8,       _d2)                                            \
                __CaseLocal(uint8,      _d2)                                            \
                                                                                        \
                __CaseLocal(int16,      _d2)                                            \
                __CaseLocal(uint16,     _d2)                                            \
                                                                                        \
                __CaseLocal(int32,      _d2)                                            \
                __CaseLocal(uint32,     _d2)                                            \
                                                                                        \
                __CaseLocal(int64,      _d2)                                            \
                __CaseLocal(uint64,     _d2)                                            \
                                                                                        \
                __CaseLocal(float_,     _d2)                                            \
                __CaseLocal(double_,    _d2)                                            \
                                                                                        \
                __CaseLocal(char_,      _d2)                                            \
                __CaseLocal(bool_,      _d2)


            __CaseLocals(int8)
            __CaseLocals(uint8)

            __CaseLocals(int16)
            __CaseLocals(uint16)

            __CaseLocals(int32)
            __CaseLocals(uint32)

            __CaseLocals(int64)
            __CaseLocals(uint64)

            __CaseLocals(float_)
            __CaseLocals(double_)

            __CaseLocals(char_)
            __CaseLocals(bool_)

            __CaseLocal(object, object)
            __CaseLocal(string, string)
            __CaseLocal(ptr,    ptr)

            #undef __CaseLocal
            #undef __CaseLocals

            // - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Pick Argument

            #define __CaseArgument(_d1, _d2)                                            \
                case __V(xil_storage_type_t::argument, xil_type_t::_d1, xil_type_t::_d2):   \
                    return __command_manager.template get_command<                      \
                        xil_storage_type_t::argument, xil_type_t::_d1, xil_type_t::_d2  \
                    >(ctx.params_layout.offset_of(xil.get_identity()));

            #define __CaseArguments(_d2)                                                \
                                                                                        \
                __CaseArgument(int8,    _d2)                                            \
                __CaseArgument(uint8,   _d2)                                            \
                                                                                        \
                __CaseArgument(int16,   _d2)                                            \
                __CaseArgument(uint16,  _d2)                                            \
                                                                                        \
                __CaseArgument(int32,   _d2)                                            \
                __CaseArgument(uint32,  _d2)                                            \
                                                                                        \
                __CaseArgument(int64,   _d2)                                            \
                __CaseArgument(uint64,  _d2)                                            \
                                                                                        \
                __CaseArgument(float_,  _d2)                                            \
                __CaseArgument(double_, _d2)                                            \
                                                                                        \
                __CaseArgument(char_,   _d2)                                            \
                __CaseArgument(bool_,   _d2)


            __CaseArguments(int8)
            __CaseArguments(uint8)

            __CaseArguments(int16)
            __CaseArguments(uint16)

            __CaseArguments(int32)
            __CaseArguments(uint32)

            __CaseArguments(int64)
            __CaseArguments(uint64)

            __CaseArguments(float_)
            __CaseArguments(double_)

            __CaseArguments(char_)
            __CaseArguments(bool_)

            __CaseArgument(object,  object)
            __CaseArgument(string,  string)
            __CaseArgument(ptr,     ptr)

            #undef __CaseArgument
            #undef __CaseArguments

            // - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Pick Field

            #define __CaseField(_d1, _d2)                                               \
                case __V(xil_storage_type_t::field, xil_type_t::_d1, xil_type_t::_d2):  \
                    return __command_manager.template get_command<                      \
                        xil_storage_type_t::field, xil_type_t::_d1, xil_type_t::_d2     \
                    >(__field_offset(ctx, xil.field_ref()));

            #define __CaseFields(_d2)                                                   \
                                                                                        \
                __CaseField(int8,       _d2)                                            \
                __CaseField(uint8,      _d2)                                            \
                                                                                        \
                __CaseField(int16,      _d2)                                            \
                __CaseField(uint16,     _d2)                                            \
                                                                                        \
                __CaseField(int32,      _d2)                                            \
                __CaseField(uint32,     _d2)                                            \
                                                                                        \
                __CaseField(int64,      _d2)                                            \
                __CaseField(uint64,     _d2)                                            \
                                                                                        \
                __CaseField(float_,     _d2)                                            \
                __CaseField(double_,    _d2)                                            \
                                                                                        \
                __CaseField(char_,      _d2)                                            \
                __CaseField(bool_,      _d2)

            __CaseFields(int8)
            __CaseFields(uint8)

            __CaseFields(int16)
            __CaseFields(uint16)

            __CaseFields(int32)
            __CaseFields(uint32)

            __CaseFields(int64)
            __CaseFields(uint64)

            __CaseFields(float_)
            __CaseFields(double_)

            __CaseFields(char_)
            __CaseFields(bool_)

            __CaseField(object, object)
            __CaseField(string, string)
            __CaseField(ptr,    ptr)

            #undef __CaseField
            #undef __CaseFields

            // - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Pick array element

            #define __CaseArrayElement(_d1, _d2)                                        \
                case __V(xil_storage_type_t::array_element, xil_type_t::_d1, xil_type_t::_d2):  \
                    return __array_element_command_manager.template get_command<        \
                        xil_type_t::_d1, xil_type_t::_d2                                \
                    >(__array_dimension(ctx, xil.type_ref()));                          \
                    break;

            #define __CaseArrayElements(_d2)                                            \
                                                                                        \
                __CaseArrayElement(int8,        _d2)                                    \
                __CaseArrayElement(uint8,       _d2)                                    \
                                                                                        \
                __CaseArrayElement(int16,       _d2)                                    \
                __CaseArrayElement(uint16,      _d2)                                    \
                                                                                        \
                __CaseArrayElement(int32,       _d2)                                    \
                __CaseArrayElement(uint32,      _d2)                                    \
                                                                                        \
                __CaseArrayElement(int64,       _d2)                                    \
                __CaseArrayElement(uint64,      _d2)                                    \
                                                                                        \
                __CaseArrayElement(float_,      _d2)                                    \
                __CaseArrayElement(double_,     _d2)                                    \
                                                                                        \
                __CaseArrayElement(char_,       _d2)                                    \
                __CaseArrayElement(bool_,       _d2)


            __CaseArrayElements(int8)
            __CaseArrayElements(uint8)

            __CaseArrayElements(int16)
            __CaseArrayElements(uint16)

            __CaseArrayElements(int32)
            __CaseArrayElements(uint32)

            __CaseArrayElements(int64)
            __CaseArrayElements(uint64)

            __CaseArrayElements(float_)
            __CaseArrayElements(double_)

            __CaseArrayElements(char_)
            __CaseArrayElements(bool_)

            __CaseArrayElement(object,  object)
            __CaseArrayElement(string,  string)
            __CaseArrayElement(ptr,     ptr)

            #undef __CaseArrayElement
            #undef __CaseArrayElements

            // - - - - - - - - - - - - - - - - - - - - - - - - - -

            #undef __Case

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

    #define __ToCallCmdValue_(_call_type, _method_t)                                    \
        __ToCmdValue(call,                                                              \
            ((cmd_value_t)xil_call_type_t::_call_type << 8) |                           \
            (cmd_value_t)rt_tidx<_method_t>()                                           \
        )

    #define __ToCallCmdValue(_call_type)                                                \
        __ToCmdValue(call, xil_call_type_t::_call_type)

    typedef rtlib::libfunc_t __libfunc_t;

    template<__nokey_t>
    class __internal_call_command_t : public __command_base_t
    {
        typedef __internal_call_command_t   __self_t;
    public:
        __internal_call_command_t(__libfunc_t func, int param_unit_size, int ret_unit_size
    #if EXEC_TRACE
            , rt_sid_t method_name
    #endif  // EXEC_TRACE
        )
            : __func(func), __param_unit_size(param_unit_size)
            , __pop_unit_size(param_unit_size - ret_unit_size)

        #if EXEC_TRACE
            , __method_name(method_name)
        #endif
        {
            _A(func != nullptr);
        }

        __BeginExecute(ctx, __ToCallCmdValue(internal))

            rtlib_context_t context(
                (assembly_analyzer_t &)ctx, ctx.stack.top() - __This->__param_unit_size
            );

            __This->__func(context);

            ctx.stack.pop(__This->__pop_unit_size);

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("call internal %1%"), __This->__method_name);
        __EndToString()

        #endif

    private:

        int __param_unit_size, __pop_unit_size;
        __libfunc_t __func;

        #if EXEC_TRACE
        rt_sid_t __method_name;    
        #endif
    };

    struct __internal_call_command_template_t
    {
        template<__nokey_t nokey, typename ... args_t>
        static auto new_command(memory_t * memory, __libfunc_t libfunc,
                        __context_t & ctx, rt_method_t * rt_method)
        {
            typedef __internal_call_command_t<nokey> this_command_t;

            int param_unit_size, ret_unit_size;
            __get_method_unit_size(ctx, rt_method, &param_unit_size, &ret_unit_size);

            this_command_t * cmd = __new_command<this_command_t>(
                memory, libfunc, param_unit_size, ret_unit_size
    #if EXEC_TRACE
                , rt_method->get_name()
    #endif
            );

            return cmd;
        }

        static void __get_method_unit_size(__context_t & ctx, rt_method_t * rt_method,
                        int * out_param_unit_size, int * out_ret_unit_size)
        {
            int unit_size = 0;
            rt_assembly_t * rt_assembly = rt_method->get_assembly();

            assembly_analyzer_t analyzer(ctx, rt_assembly);
            rt_assembly->each_params((*rt_method)->params, [&](int, auto & param) {
               return unit_size += __unit_size_of(analyzer, param.type), true;
            });

            *out_param_unit_size = unit_size;
            *out_ret_unit_size   = __unit_size_of(analyzer, (*rt_method)->type);
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
        if (libfunc == nullptr)
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
        typedef __call_command_base_t   __self_t;

    public:
        #if EXEC_TRACE
        __call_command_base_t(_rt_method_t * rt_method, rt_sid_t method_name)
            : __rt_method(rt_method), __method_name(method_name) { }
        #else
        __call_command_base_t(_rt_method_t * rt_method) : __rt_method(rt_method) { }
        #endif

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("call %1%"), __This->__method_name);
        __EndToString()

        #endif  // EXEC_TRACE

    protected:
        __AlwaysInline exec_method_t * __get_method(command_execute_context_t & ctx)
        {
            if (__method == nullptr)
                __method = __parse_method(ctx);

            return __method;
        }

        #if EXEC_TRACE
        rt_sid_t __method_name;
        #endif

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
        typedef __static_call_command_t             __self_t;
        typedef __call_command_base_t<_rt_method_t> __super_t;

    public:
        using __super_t::__super_t;

        __BeginExecute(ctx, __ToCallCmdValue_(static_, _rt_method_t))

            __pre_static_call(ctx, __This->get_type());

            exec_method_t * method = __This->__get_method(ctx);
            ctx.push_calling(method);
            ctx.stack.increase_top(method->stack_unit_size);

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("call static %1%"), __This->__method_name);
        __EndToString()

        #endif      // EXEC_TRACE
    };

    template<typename _rt_method_t>
    struct __static_call_command_template_t
    {
        template<__nokey_t, typename ... args_t>
        static auto new_command(memory_t * memory, _rt_method_t * rt_method, __context_t & ctx)
        {
            typedef __static_call_command_t<_rt_method_t> this_command_t;
            return __new_command<this_command_t>(memory, rt_method

            #if EXEC_TRACE
            , rt_method->get_name()
            #endif
            );
        }
    };

    command_t * new_static_call_command(__context_t & ctx, ref_t method_ref)
    {
        switch ((mt_member_extra_t)method_ref.extra)
        {
            case mt_member_extra_t::generic: {

                static __command_manager_t<
                    __static_call_command_template_t<rt_generic_method_t>, __nokey_t
                >::with_args_t<rt_generic_method_t *> __static_generic_call_command_manager;

                rt_generic_method_t * rt_generic_method = ctx.get_generic_method(method_ref);

                return __static_generic_call_command_manager.template get_command<__nokey>(
                    __Mv(rt_generic_method), ctx
                );

            }   break;

            case mt_member_extra_t::import:
            case mt_member_extra_t::internal: {

                static __command_manager_t<
                    __static_call_command_template_t<rt_method_t>, __nokey_t
                >::with_args_t<rt_method_t *> __static_call_command_manager;

                rt_method_t * rt_method = ctx.get_method(method_ref);
                return __static_call_command_manager.template get_command<__nokey>(
                    __Mv(rt_method), ctx
                );

            }   break;

            default:
                X_UNEXPECTED();
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
        typedef __instance_call_command_t           __self_t;
        typedef __call_command_base_t<_rt_method_t> __super_t;

    public:
        using __super_t::__super_t;

        __BeginExecute(ctx, __ToCallCmdValue_(instance, _rt_method_t))

            exec_method_t * method = __This->__get_method(ctx);
            ctx.push_calling(method);
            ctx.stack.increase_top(method->stack_unit_size);

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("call instance %1%"), __This->__method_name);
        __EndToString()

        #endif  // EXEC_TRACE
    };

    template<typename _rt_method_t>
    struct __instance_call_command_template_t
    {
        template<__nokey_t nokey, typename ... args_t>
        static auto new_command(memory_t * memory, _rt_method_t * rt_method, __context_t & ctx)
        {
            typedef __instance_call_command_t<_rt_method_t> this_command_t;
            return __new_command<this_command_t>(memory, rt_method

            #if EXEC_TRACE
                , rt_method->get_name()
            #endif // EXEC_TRACE

            );
        }
    };

    static command_t * __new_instance_call_command(__context_t & ctx, const call_xil_t & xil)
    {
        ref_t method_ref = (ref_t)xil.method;

        switch ((mt_member_extra_t)method_ref.extra)
        {
            case mt_member_extra_t::generic: {

                static __command_manager_t<
                    __instance_call_command_template_t<rt_generic_method_t>, __nokey_t
                >::with_args_t<rt_generic_method_t *> __instance_generic_call_command_manager;

                rt_generic_method_t * rt_generic_method = ctx.get_generic_method(method_ref);

                return __instance_generic_call_command_manager.template get_command<__nokey>(
                    __Mv(rt_generic_method), ctx
                );

            }   break;

            case mt_member_extra_t::import:
            case mt_member_extra_t::internal: {

                static __command_manager_t<
                    __instance_call_command_template_t<rt_method_t>, __nokey_t
                >::with_args_t<rt_method_t *> __instance_call_command_manager;

                rt_method_t * rt_method = ctx.get_method(method_ref);

                return __instance_call_command_manager.template get_command<__nokey>(
                    __Mv(rt_method), ctx
                );

            }   break;

            default:
                X_UNEXPECTED();
        }
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
        typedef __virtual_call_command_t    __self_t;
        typedef __command_base_t            __super_t;

    public:
        __virtual_call_command_t(int offset) : __offset(offset) { }

        __BeginExecute(ctx, __ToCallCmdValue(virtual_))

            rt_ref_t rt_ref = __Argument(rt_ref_t, 0);
            rt_type_t * rt_type = __RtTypeOf(rt_ref);

            rt_vtable_t * vtbl = get_vtable(rt_type);
            rt_vfunction_t func = vtbl->functions[__This->__offset];

            if (!func.is_method())
            {
                rt_assembly_t * rt_assembly = ctx.env.assemblies.at(func.assembly_idx);

                rt_method_t * rt_method = __assembly_analyzer(ctx, rt_assembly).get_method(
                    ref_t(func.method_idx)
                );

                func.method = ctx.env.exec_method_of(rt_method);
            }

            exec_method_t * method = (exec_method_t *)func.method;
            ctx.push_calling(method);
            ctx.stack.increase_top(method->stack_unit_size);

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("call virtual %1%"), __This->__offset);
        __EndToString()

        #endif  // EXEC_TRACE

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
        switch (xil.call_type())
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
        typedef __binary_command_t  __self_t;

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

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("%|1$-4| %2% %3%"), _str(cmd), dtype1, dtype2);
        __EndToString()

        #endif  // EXEC_TRACE
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
        typedef __unitary_command_t     __self_t;

    public:
        __BeginExecute(ctx, __ToUnitaryAlCmdValue(cmd, dtype1))

            const vtype_t vtype1 = to_vtype(dtype1);

            typedef vnum_t<vtype1> t1;
            t1 v1 = ctx.stack.pop<t1>();

            ctx.stack.push(
                unitary_operator_t<to_operator(cmd), vtype1>()(v1)
            );
        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("%1% %2%"), _str(cmd), dtype1);
        __EndToString()

        #endif  // EXEC_TRACE
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

        switch (__V(xil.cmd(), xil.dtype1(), xil.dtype2()))
        {
            #define __CaseBinary(cmd, dtype1, dtype2)                                   \
                case __V(xil_al_command_t::cmd, xil_type_t::dtype1, xil_type_t::dtype2):\
                    return __binary_command_manager.template get_command<               \
                        xil_al_command_t::cmd, xil_type_t::dtype1, xil_type_t::dtype2   \
                    >();

            #define __C1(cmd, dtype1, dtype2)    __CaseBinary(cmd, dtype1, dtype2)
            #define __C2(cmd, dtype1, dtype2)                                           \
                __C1(cmd, dtype1, dtype2)                                               \
                __C1(cmd, dtype2, dtype1)

            #define __CaseBinaries(cmd)                                                 \
                __C1(cmd, int8,   int8)                                                 \
                __C2(cmd, int8,   uint8)                                                \
                                                                                        \
                __C1(cmd, uint8,  uint8)                                                \
                                                                                        \
                __C2(cmd, int16,  int8)                                                 \
                __C2(cmd, int16,  uint8)                                                \
                __C1(cmd, int16,  int16)                                                \
                __C2(cmd, int16,  uint16)                                               \
                                                                                        \
                __C2(cmd, uint16, int8)                                                 \
                __C2(cmd, uint16, uint8)                                                \
                __C1(cmd, uint16, uint16)                                               \
                                                                                        \
                __C2(cmd, int32,  int8)                                                 \
                __C2(cmd, int32,  uint8)                                                \
                __C2(cmd, int32,  int16)                                                \
                __C2(cmd, int32,  uint16)                                               \
                __C1(cmd, int32,  int32)                                                \
                                                                                        \
                __C2(cmd, uint32, int8)                                                 \
                __C2(cmd, uint32, uint8)                                                \
                __C2(cmd, uint32, int16)                                                \
                __C2(cmd, uint32, uint16)                                               \
                __C2(cmd, uint32, int32)                                                \
                __C1(cmd, uint32, uint32)                                               \
                                                                                        \
                __C2(cmd, int64,  int8)                                                 \
                __C2(cmd, int64,  uint8)                                                \
                __C2(cmd, int64,  int16)                                                \
                __C2(cmd, int64,  uint16)                                               \
                __C2(cmd, int64,  int32)                                                \
                __C2(cmd, int64,  uint32)                                               \
                __C1(cmd, int64,  int64)                                                \
                                                                                        \
                __C2(cmd, uint64,  int8)                                                \
                __C2(cmd, uint64,  uint8)                                               \
                __C2(cmd, uint64,  int16)                                               \
                __C2(cmd, uint64,  uint16)                                              \
                __C2(cmd, uint64,  int32)                                               \
                __C2(cmd, uint64,  uint32)                                              \
                __C2(cmd, uint64,  int64)                                               \
                __C1(cmd, uint64,  uint64)

            #define __CaseFloatBinaries(cmd)                                            \
                __C2(cmd, float_,  int8)                                                \
                __C2(cmd, float_,  uint8)                                               \
                __C2(cmd, float_,  int16)                                               \
                __C2(cmd, float_,  uint16)                                              \
                __C2(cmd, float_,  int32)                                               \
                __C2(cmd, float_,  uint32)                                              \
                __C2(cmd, float_,  int64)                                               \
                __C1(cmd, float_,  uint64)                                              \
                __C1(cmd, float_,  float_)                                              \
                                                                                        \
                __C2(cmd, double_,  int8)                                               \
                __C2(cmd, double_,  uint8)                                              \
                __C2(cmd, double_,  int16)                                              \
                __C2(cmd, double_,  uint16)                                             \
                __C2(cmd, double_,  int32)                                              \
                __C2(cmd, double_,  uint32)                                             \
                __C2(cmd, double_,  int64)                                              \
                __C1(cmd, double_,  uint64)                                             \
                __C2(cmd, double_,  float_)                                             \
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

            #define __CaseUnitary(cmd, dtype)                                           \
                case __V(xil_al_command_t::cmd, xil_type_t::dtype, xil_type_t::__default__):  \
                    return __unitary_command_manager.template get_command<              \
                        xil_al_command_t::cmd, xil_type_t::dtype                        \
                    >();

            #define __C(cmd, dtype)    __CaseUnitary(cmd, dtype)

            #define __CaseUnitaries(cmd)                                                \
                __C(cmd, int8)                                                          \
                __C(cmd, uint8)                                                         \
                __C(cmd, int16)                                                         \
                __C(cmd, uint16)                                                        \
                __C(cmd, int32)                                                         \
                __C(cmd, uint32)                                                        \
                __C(cmd, int64)                                                         \
                __C(cmd, uint64)

            #define __CaseFloatUnitaries(cmd)                                           \
                __C(cmd, float_)                                                        \
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
                _P(xil.cmd(), xil.dtype1(), xil.dtype2());
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

        switch (__V(xil.cmd(), xil.dtype1(), xil.dtype2()))
        {
            #define __CaseBinary(cmd, dtype1, dtype2)                                   \
                case __V(xil_bit_command_t::cmd, xil_type_t::dtype1, xil_type_t::dtype2):   \
                    return __binary_command_manager.template get_command<               \
                        xil_bit_command_t::cmd, xil_type_t::dtype1, xil_type_t::dtype2  \
                    >();

            #define __C1(cmd, dtype1, dtype2)    __CaseBinary(cmd, dtype1, dtype2)
            #define __C2(cmd, dtype1, dtype2)                                           \
                __C1(cmd, dtype1, dtype2)                                               \
                __C1(cmd, dtype2, dtype1)

            #define __CaseBinaries(cmd)                                                 \
                __C1(cmd, int8,   int8)                                                 \
                __C2(cmd, int8,   uint8)                                                \
                                                                                        \
                __C1(cmd, uint8,  uint8)                                                \
                                                                                        \
                __C2(cmd, int16,  int8)                                                 \
                __C2(cmd, int16,  uint8)                                                \
                __C1(cmd, int16,  int16)                                                \
                __C2(cmd, int16,  uint16)                                               \
                                                                                        \
                __C2(cmd, uint16, int8)                                                 \
                __C2(cmd, uint16, uint8)                                                \
                __C1(cmd, uint16, uint16)                                               \
                                                                                        \
                __C2(cmd, int32,  int8)                                                 \
                __C2(cmd, int32,  uint8)                                                \
                __C2(cmd, int32,  int16)                                                \
                __C2(cmd, int32,  uint16)                                               \
                __C1(cmd, int32,  int32)                                                \
                                                                                        \
                __C2(cmd, uint32, int8)                                                 \
                __C2(cmd, uint32, uint8)                                                \
                __C2(cmd, uint32, int16)                                                \
                __C2(cmd, uint32, uint16)                                               \
                __C2(cmd, uint32, int32)                                                \
                __C1(cmd, uint32, uint32)                                               \
                                                                                        \
                __C2(cmd, int64,  int8)                                                 \
                __C2(cmd, int64,  uint8)                                                \
                __C2(cmd, int64,  int16)                                                \
                __C2(cmd, int64,  uint16)                                               \
                __C2(cmd, int64,  int32)                                                \
                __C2(cmd, int64,  uint32)                                               \
                __C1(cmd, int64,  int64)                                                \
                                                                                        \
                __C2(cmd, uint64,  int8)                                                \
                __C2(cmd, uint64,  uint8)                                               \
                __C2(cmd, uint64,  int16)                                               \
                __C2(cmd, uint64,  uint16)                                              \
                __C2(cmd, uint64,  int32)                                               \
                __C2(cmd, uint64,  uint32)                                              \
                __C2(cmd, uint64,  int64)                                               \
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

            #define __CaseUnitary(cmd, dtype)                                           \
                case __V(xil_bit_command_t::cmd, xil_type_t::dtype, xil_type_t::__default__):  \
                    return __unitary_command_manager.template get_command<              \
                        xil_bit_command_t::cmd, xil_type_t::dtype                       \
                    >();

            #define __C(cmd, dtype)    __CaseUnitary(cmd, dtype)

            #define __CaseUnitaries(cmd)                                                \
                __C(cmd, int8)                                                          \
                __C(cmd, uint8)                                                         \
                __C(cmd, int16)                                                         \
                __C(cmd, uint16)                                                        \
                __C(cmd, int32)                                                         \
                __C(cmd, uint32)                                                        \
                __C(cmd, int64)                                                         \
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

        switch (xil.cmd())
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

        switch (__V(xil.cmd(), xil.dtype1(), xil.dtype2()))
        {
            #define __CaseBinary(cmd, dtype1, dtype2)                                   \
                case __V(xil_cmp_command_t::cmd, xil_type_t::dtype1, xil_type_t::dtype2):\
                    return __command_manager.template get_command<                      \
                        xil_cmp_command_t::cmd, xil_type_t::dtype1, xil_type_t::dtype2  \
                    >();

            #define __C1(cmd, dtype1, dtype2)    __CaseBinary(cmd, dtype1, dtype2)
            #define __C2(cmd, dtype1, dtype2)                                           \
                __C1(cmd, dtype1, dtype2)                                               \
                __C1(cmd, dtype2, dtype1)

            #define __CaseBinaries(cmd)                                                 \
                __C1(cmd, int8,   int8)                                                 \
                __C2(cmd, int8,   uint8)                                                \
                                                                                        \
                __C1(cmd, uint8,  uint8)                                                \
                                                                                        \
                __C2(cmd, int16,  int8)                                                 \
                __C2(cmd, int16,  uint8)                                                \
                __C1(cmd, int16,  int16)                                                \
                __C2(cmd, int16,  uint16)                                               \
                                                                                        \
                __C2(cmd, uint16, int8)                                                 \
                __C2(cmd, uint16, uint8)                                                \
                __C1(cmd, uint16, uint16)                                               \
                                                                                        \
                __C2(cmd, int32,  int8)                                                 \
                __C2(cmd, int32,  uint8)                                                \
                __C2(cmd, int32,  int16)                                                \
                __C2(cmd, int32,  uint16)                                               \
                __C1(cmd, int32,  int32)                                                \
                                                                                        \
                __C2(cmd, uint32, int8)                                                 \
                __C2(cmd, uint32, uint8)                                                \
                __C2(cmd, uint32, int16)                                                \
                __C2(cmd, uint32, uint16)                                               \
                __C2(cmd, uint32, int32)                                                \
                __C1(cmd, uint32, uint32)                                               \
                                                                                        \
                __C2(cmd, int64,  int8)                                                 \
                __C2(cmd, int64,  uint8)                                                \
                __C2(cmd, int64,  int16)                                                \
                __C2(cmd, int64,  uint16)                                               \
                __C2(cmd, int64,  int32)                                                \
                __C2(cmd, int64,  uint32)                                               \
                __C1(cmd, int64,  int64)                                                \
                                                                                        \
                __C2(cmd, uint64,  int8)                                                \
                __C2(cmd, uint64,  uint8)                                               \
                __C2(cmd, uint64,  int16)                                               \
                __C2(cmd, uint64,  uint16)                                              \
                __C2(cmd, uint64,  int32)                                               \
                __C2(cmd, uint64,  uint32)                                              \
                __C2(cmd, uint64,  int64)                                               \
                __C1(cmd, uint64,  uint64)

            #define __CaseFloatBinaries(cmd)                                            \
                __C2(cmd, float_,  int8)                                                \
                __C2(cmd, float_,  uint8)                                               \
                __C2(cmd, float_,  int16)                                               \
                __C2(cmd, float_,  uint16)                                              \
                __C2(cmd, float_,  int32)                                               \
                __C2(cmd, float_,  uint32)                                              \
                __C2(cmd, float_,  int64)                                               \
                __C1(cmd, float_,  uint64)                                              \
                __C1(cmd, float_,  float_)                                              \
                                                                                        \
                __C2(cmd, double_,  int8)                                               \
                __C2(cmd, double_,  uint8)                                              \
                __C2(cmd, double_,  int16)                                              \
                __C2(cmd, double_,  uint16)                                             \
                __C2(cmd, double_,  int32)                                              \
                __C2(cmd, double_,  uint32)                                             \
                __C2(cmd, double_,  int64)                                              \
                __C1(cmd, double_,  uint64)                                             \
                __C2(cmd, double_,  float_)                                             \
                __C1(cmd, double_,  double_)

            __CaseBinaries(greater)
            __CaseFloatBinaries(greater);
            __CaseBinaries(greater_equal)
            __CaseFloatBinaries(greater_equal);
            __CaseBinaries(less)
            __CaseFloatBinaries(less);
            __CaseBinaries(less_equal)
            __CaseFloatBinaries(less_equal);
            __CaseBinaries(equal)
            __CaseFloatBinaries(equal);
            __CaseBinaries(not_equal)
            __CaseFloatBinaries(not_equal);

            __C1(equal, bool_, bool_)
            __C1(not_equal, bool_, bool_)
            __C1(equal, char_, char_)
            __C1(not_equal, char_, char_)
            __C1(greater, char_, char_)
            __C1(greater_equal, char_, char_)
            __C1(less, char_, char_)
            __C1(less_equal, char_, char_)

            __C1(equal, object, object)
            __C1(not_equal, object, object)

            __C1(equal, ptr, ptr)
            __C1(not_equal, ptr, ptr)

            #undef __C1
            #undef __C2
            #undef __CaseBinaries
            #undef __CaseFloatBinaries
            #undef __CaseBinary

            default:
                _P(xil.dtype1(), xil.dtype2());
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

    #define __IsRetCmdValue(v)                                                          \
        ( (xil_command_t)__Cmd(v) == xil_command_t::smp &&                              \
        (xil_smp_t)((v & 0x0000FF00) >> 8) == xil_smp_t::ret )

    class __ret_command_base_t : public __command_base_t { };

    template<msize_t _ret_size>
    class __ret_command_t : public __ret_command_base_t
    {
        typedef __ret_command_t     __self_t;

    public:
        __ret_command_t(msize_t local_unit_size, msize_t param_unit_size)
            : __total_unit_size(param_unit_size + __stack_stub_size + local_unit_size)
            , __param_unit_ret_size(param_unit_size - _ret_size)
            , __total_unit_ret_size(__total_unit_size - _ret_size)
        {
            #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_MANUAL_METHOD
            __ret_commands.insert(this);
            #endif  // EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_MANUAL_METHOD
        }

        __BeginExecute(ctx, __ToRetCmdValue(_ret_size))

            // param, stub, local, ret
            // _P(_T("ret: "), __total_unit_size, __param_unit_ret_size, _ret_size);
            rt_stack_unit_t * p = ctx.stack.pop(__This->__total_unit_size);
            ctx.pop_calling((const __calling_stub_t *)(p + __This->__param_unit_ret_size));
            copy_array<_ret_size>(p + __This->__total_unit_ret_size, p - _ret_size);

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _T("ret");
        __EndToString()

        #endif  // EXEC_TRACE

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
        typedef __ret_command_t     __self_t;

    public:
        __ret_command_t(msize_t local_unit_size, msize_t param_unit_size, msize_t ret_unit_size)
            : __total_unit_size(param_unit_size + __stack_stub_size + local_unit_size)
            , __param_unit_ret_size(param_unit_size - ret_unit_size)
            , __total_unit_ret_size(__total_unit_size - ret_unit_size)
            , __ret_unit_size(ret_unit_size)
        {
            #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_MANUAL_METHOD
            __ret_commands.insert(this);
            #endif  // EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_MANUAL_METHOD
        }

        __BeginExecute(ctx, __ToRetCmdValue(256))

            // param, stub, local, ret
            rt_stack_unit_t * p = ctx.stack.pop(__This->__total_unit_size);
            ctx.pop_calling((const __calling_stub_t *)(p + __This->__param_unit_ret_size));

            // TODO: optimize
            for (rt_stack_unit_t * dst = p - __This->__ret_unit_size,
                    * dst_end = dst + __This->__ret_unit_size,
                    * src = p + __This->__total_unit_ret_size;
                dst < dst_end;)
            {
                *dst++ = *src++;
            }

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _T("ret");
        __EndToString()

        #endif  // EXEC_TRACE

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

        switch (ret_unit_size)
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

    #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_MANUAL_METHOD

        return __ret_commands.find(command) == __ret_commands.end();

    #elif EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_INLINE   // EXEC_EXECUTE_MODEL == xxx

        return __IsRetCmdValue(command->cmd_value);

    #elif EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL_METHOD // EXEC_EXECUTE_MODEL == xxx

        return is<__ret_command_base_t *>(command);

    #endif  // EXEC_EXECUTE_MODEL == xxx
    }

    //-------- ---------- ---------- ---------- ----------

    // Label command.
    class __label_command_t : public __command_base_t
    {
        typedef __label_command_t   __self_t;

    public:
        __BeginExecute(ctx, __ToCmdValue(smp, xil_smp_t::label))

            // Do nothing

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _T("label");
        __EndToString()

        #endif  // EXEC_TRACE
    };

    //-------- ---------- ---------- ---------- ----------

    bool __is_base_type(analyzer_env_t & env, rt_type_t * type, rt_type_t * base_type)
    {
        while (type != base_type)
        {
            if (type == nullptr)
                return false;

            type = type->get_base_type(env);
        }

        return true;
    }

    //-------- ---------- ---------- ---------- ----------

    // Finds block.
    template<__block_type_t _type, typename _f_t>
    __AlwaysInline void __find_block(command_execute_context_t & ctx, _f_t f)
    {
        exec_method_t * method = ctx.current_method();
        _A(method != nullptr);

        // _PP(ctx.current_method());
        // _PP(__command_index(ctx, ctx.current - 1));

        method->find_block<_type>(ctx.current - 1, f);
    }

    // Function process_block_node return value.
    X_ENUM(__process_block_nodes_ret)

        // The queue is empty.
        empty = __default__,

        // Empty in current method.
        current_empty,

        // Succeed to execute.
        success,

    X_ENUM_END

    X_ENUM_INFO(__process_block_nodes_ret)

        // The queue is empty.
        X_C(empty,              _T("empty"))

        // Empty in current method.
        X_C(current_empty,      _T("current_empty"))

        // Succeed to execute.
        X_C(success,            _T("success"))

    X_ENUM_INFO_END

    // Process block nodes.
    __AlwaysInline __process_block_nodes_ret __process_block_nodes(command_execute_context_t & ctx)
    {
        block_node_t * node = ctx.block_queue.pick();
        if (node == nullptr)
            return __process_block_nodes_ret::empty;

        if (node->identity != (block_node_identity_t *)ctx.stack.lp())
            return __process_block_nodes_ret::current_empty;

        // Exception block.
        if (node->exception_node != nullptr)
        {
            ctx.block_queue.deque();
            ctx.stack.push(node->exception_node->exception);
            ctx.current = node->block->entry_point;
        }
        else    // Finally block.
        {
            ctx.block_queue.deque_for_executing();
            ctx.current = node->block->entry_point;
        }

        return __process_block_nodes_ret::success;
    }

    // Pops to calling method.
    __AlwaysInline void __pop_calling(command_execute_context_t & ctx)
    {
        ctx.pop_calling();

        if (ctx.stack_empty())
            throw _E(exec_env_code_t::terminal);

        ctx.restore_stack();
    }

    // Process exceptions
    __AlwaysInline void __process_exception(command_execute_context_t & ctx)
    {
        exception_node_t * node = ctx.exception_stack.head;
        _A(node != nullptr);

        rt_ref_t exception = node->exception;
        rt_type_t * exception_type = mm::get_object_type(exception);
        _A(exception_type != nullptr);

    __continue_find__:

        ctx.block_queue.begin_group(ctx.stack.lp());

        __find_block<__block_type_t::__unknown__>(ctx, [&](exec_method_block_t * block) {

            switch (block->type)
            {
                case __block_type_t::catch_:

                    if (__is_base_type(ctx.env, exception_type, block->relation_type))
                    {
                        ctx.block_queue.enque_catch(block, node);
                        ctx.pop_exception();

                        return false;
                    }

                    return true;

                case __block_type_t::finally_:
                    ctx.block_queue.enque_finally(block);
                    return true;

                default:
                    return true;
            }
        });

        if (ctx.block_queue.end_group())
        {
            _A(ctx.block_queue.current_executing() != nullptr);

            ctx.block_queue.pop_executing();
        }

        // Pop to calling method.
        block_node_t * last_node = ctx.block_queue.last_current();
        if (last_node == nullptr)
        {
            __pop_calling(ctx);
            goto __continue_find__;
        }

        __process_block_nodes(ctx);
    }

    // Process finally blocks.
    __AlwaysInline void __process_finally(command_execute_context_t & ctx,
                                                            command_t ** next_command)
    {
        ctx.block_queue.begin_group(ctx.stack.lp());

        __find_block<__block_type_t::finally_>(ctx, [&](exec_method_block_t * block) {

            if (block->exclude(next_command))
            {
                ctx.block_queue.enque_finally(block);
                return true;
            }

            return false;
        });

        ctx.block_queue.end_group();
        block_node_t * last_node = ctx.block_queue.last_current();

        if (last_node != nullptr)
        {
            // Executes finally blocks.
            last_node->set_next_command(next_command);
            __process_block_nodes(ctx);
        }
    }

    // Process finally blocks. ( for return )
    __AlwaysInline void __process_finally_for_return(command_execute_context_t & ctx,
                                                     command_t * return_command)
    {
        ctx.block_queue.begin_group(ctx.stack.lp());

        __find_block<__block_type_t::finally_>(ctx, [&](exec_method_block_t * block) {
            return ctx.block_queue.enque_finally(block), true;
        });

        ctx.block_queue.end_group();
        block_node_t * last_node = ctx.block_queue.last_current();

        if (last_node != nullptr)
        {
            // Executes finally blocks.
            last_node->set_following_command(return_command);
            __process_block_nodes(ctx);
        }
        else
        {
            __execute_command(ctx, return_command);
        }
    }

    // Throw command.
    class __throw_command_t : public __command_base_t
    {
        typedef __throw_command_t   __self_t;

    public:
        __BeginExecute(ctx, __ToCmdValue(smp, xil_smp_t::throw_))

            rt_ref_t exception = ctx.stack.pop<rt_ref_t>();
            ctx.restore_stack();

            ctx.push_exception(exception);
            __process_exception(ctx);

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _T("throw");
        __EndToString()

        #endif  // EXEC_TRACE
    };

    //-------- ---------- ---------- ---------- ----------

    // Rethrow command.
    class __rethrow_command_t : public __command_base_t
    {
        typedef __rethrow_command_t     __self_t;

    public:
        __BeginExecute(ctx, __ToCmdValue(smp, xil_smp_t::rethrow))

            // Do nothing.

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _T("rethrow");
        __EndToString()

        #endif  // EXEC_TRACE
    };

    //-------- ---------- ---------- ---------- ----------

    // End finally command.
    class __end_finally_command_t : public __command_base_t
    {
        typedef __end_finally_command_t     __self_t;

    public:

        // Execute.
        __BeginExecute(ctx, __ToCmdValue(smp, xil_smp_t::end_finally))

            _A(ctx.block_queue.current_executing() != nullptr);

            block_node_t * node = ctx.block_queue.pop_executing();

            switch (node->action)
            {
                case block_node_finally_action_t::jmp_to_next_command:
                    _A(node->next_command != nullptr);
                    ctx.current = node->next_command;
                    return;

                case block_node_finally_action_t::execute_following_command:
                    _A(node->following_command != nullptr);
                    __execute_command(ctx, node->following_command);
                    return;

                default:
                    break;
            }

        __retry__:

            switch (__process_block_nodes(ctx))
            {
                case __process_block_nodes_ret::current_empty:
                case __process_block_nodes_ret::empty:
                    __pop_calling(ctx);
                    if (!ctx.exception_stack.empty())
                    {
                        __process_exception(ctx);
                        return;
                    }

                    break;

                case __process_block_nodes_ret::success:
                    return;

                default:
                    X_UNEXPECTED();
            }

            X_UNEXPECTED();

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _T("end finally");
        __EndToString()

        #endif  // EXEC_TRACE
    };

    //-------- ---------- ---------- ---------- ----------

    static command_t * __new_smp_command(__context_t & ctx, const smp_xil_t & xil)
    {
        switch (xil.smp())
        {
            case xil_smp_t::ret:
                return new_ret_command(ctx);

            case xil_smp_t::empty:
                __ReturnInstance(__empty_command_t);

            case xil_smp_t::label:
                __ReturnInstance(__label_command_t);

            case xil_smp_t::throw_:
                __ReturnInstance(__throw_command_t);

            case xil_smp_t::rethrow:
                __ReturnInstance(__rethrow_command_t);

            case xil_smp_t::end_finally:
                __ReturnInstance(__end_finally_command_t);

            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    #define __ToJmpCmdValue_(_model, _flag)                                             \
        __ToCmdValue(jmp, ((cmd_value_t)_model << 8) | (cmd_value_t)_flag)

    #define __ToJmpCmdValue(_model) __ToJmpCmdValue_(_model, 0)

    template<xil_jmp_model_t _model> struct __jmp_model_t { };

    template<> struct __jmp_model_t<xil_jmp_model_t::none>
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

    template<> struct __jmp_model_t<xil_jmp_model_t::true_>
    {
        __AlwaysInline bool operator()(command_execute_context_t & ctx)
        {
            //_P(_T("jmp true: "), ctx.stack.pick<bool>());
            return ctx.stack.pop<bool>();
        }

        operator string_t() const
        {
            return _T("jmp  true");
        }
    };

    //-------- ---------- ---------- ---------- ----------

    template<> struct __jmp_model_t<xil_jmp_model_t::false_>
    {
        __AlwaysInline bool operator()(command_execute_context_t & ctx)
        {
            //_P(_T("jmp false: "), ctx.stack.pick<bool>());
            return !ctx.stack.pop<bool>();
        }

        operator string_t() const
        {
            return _T("jmp  false");
        }
    };

    //-------- ---------- ---------- ---------- ----------

    template<xil_jmp_model_t model>
    class __jmp_command_t : public __command_base_t
    {
        typedef __jmp_command_t     __self_t;
        typedef __jmp_model_t<model> __jmp_model_t;

    public:
        __jmp_command_t(xil_jmp_step_t step) : __step(step) { }

        __BeginExecute(ctx, __ToJmpCmdValue(model))

            if (__jmp_model_t()(ctx))
            {
                ctx.current += __This->__step;
            }

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("%1% %2%"), __jmp_model_t(), __This->__step);
        __EndToString()
        
        #endif  // EXEC_TRACE

    private:
        xil_jmp_step_t __step;
    };

    //-------- ---------- ---------- ---------- ----------

    // Leave and jmp.
    template<>
    class __jmp_command_t<xil_jmp_model_t::leave> : public __command_base_t
    {
        typedef __jmp_command_t<xil_jmp_model_t::leave> __self_t;

    public:
        __jmp_command_t(xil_jmp_step_t step) : __step(step) { }

        // Execute
        __BeginExecute(ctx, __ToJmpCmdValue(xil_jmp_model_t::leave))

            __process_finally(ctx, ctx.current + __This->__step);

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("leave %1%"), __This->__step + 1);
        __EndToString()

        #endif  // EXEC_TRACE

    private:
        xil_jmp_step_t __step;
    };

    //-------- ---------- ---------- ---------- ----------

    struct __jmp_command_template_t
    {
        template<xil_jmp_model_t model, typename ... args_t>
        static auto new_command(memory_t * memory, args_t && ... args)
        {
            typedef __jmp_command_t<model> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Leave and return.
    class __leave_ret_command_t : public __command_base_t
    {
        typedef __leave_ret_command_t __self_t;

    public:

        // Constructors.
        __leave_ret_command_t(command_t * following_command) _NE
            : __following_command(following_command)
        {
            _A(following_command != nullptr);
        }

        // Execute
        __BeginExecute(ctx, __ToJmpCmdValue_(xil_jmp_model_t::leave, 1))

            __process_finally_for_return(ctx, __This->__following_command);

        __EndExecute()

        // To string

        #if EXEC_TRACE

        __BeginToString()
            return _T("leave.ret");
        __EndToString()

        #endif  // EXEC_TRACE

    private:
        command_t * __following_command;
    };

    //-------- ---------- ---------- ---------- ----------

    struct __leave_ret_command_template_t
    {
        template<__nokey_t, typename ... args_t>
        static auto new_command(memory_t * memory, command_t * command, args_t && ... args)
        {
            typedef __leave_ret_command_t this_command_t;
            return __new_command<this_command_t>(memory, command, std::forward<args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    class __switch_jmp_command_t : public __command_base_t
    {
        typedef __switch_jmp_command_t      __self_t;

    public:
        __switch_jmp_command_t(exec_switch_table_t * table) : __table(table) { }

        __BeginExecute(ctx, __ToJmpCmdValue(xil_jmp_model_t::switch_))

            int32_t value = ctx.stack.pop<int32_t>();

            exec_switch_row_t * rows = (exec_switch_row_t *)__This->__table->rows;
            exec_switch_row_t * rows_end = rows + __This->__table->row_count;

            exec_switch_row_t * row = __binary_search(rows, rows_end, value);

            if (row != nullptr)
                ctx.current += row->step - 1;
            else
                ctx.current += __This->__table->default_step - 1;

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _T("switch");
        __EndToString()

        #endif  // EXEC_TRACE

    private:
        exec_switch_table_t * __table;

        __AlwaysInline static exec_switch_row_t * __binary_search(
                exec_switch_row_t * begin, exec_switch_row_t * end, int32_t value)
        {
            if (begin == end)
                return nullptr;

            exec_switch_row_t * r = begin + (end - begin) / 2;
            if (value < r->value)
                return __binary_search(begin, r, value);

            if (value > r->value)
                return __binary_search(r + 1, end, value);

            return r;
        }
    };

    static command_t * __new_jmp_command(__context_t & ctx, const jmp_xil_t & xil)
    {
        static __command_manager_t<
            __jmp_command_template_t, xil_jmp_model_t
        >::with_args_t<int> __jmp_command_manager;

        static __command_manager_t<
            __leave_ret_command_template_t, __nokey_t
        >::with_args_t<command_t *> __leave_ret_command_manager;

        typedef xil_jmp_model_t model_t;

        switch (xil.model())
        {
            #define __Case(_model)                                                      \
                case xil_jmp_model_t::_model:                                           \
                    return __jmp_command_manager.template get_command<model_t::_model>( \
                        xil.step() - 1                                                  \
                    );

            __Case(none)
            __Case(true_)
            __Case(false_)

            #undef __Case

            case xil_jmp_model_t::switch_:
                return __new_command<__switch_jmp_command_t>(
                    (memory_t *)&__command_heap, ctx.switch_manager.get_table(xil.tbl())
                );

            case xil_jmp_model_t::leave:
                if (xil.step() == 0)
                {
                    return __leave_ret_command_manager.template get_command<__nokey>(
                        new_ret_command(ctx)
                    );
                }

                return __jmp_command_manager.template get_command<model_t::leave>(
                    xil.step() - 1
                );
                

            default:
                X_UNEXPECTED();
        }
    }

    template<xil_new_type_t _new_type> class __new_command_t { };

    template<>
    class __new_command_t<xil_new_type_t::default_> : public __command_base_t
    {
        typedef __new_command_t     __self_t;

    public:
        #if EXEC_TRACE
        __new_command_t(rt_type_t * type, rt_sid_t name) : __type(type), __name(name) { }
        #else
        __new_command_t(rt_type_t * type) : __type(type) { }
        #endif  // EXEC_TRACE

        __BeginExecute(ctx, __ToCmdValue(new_, xil_new_type_t::default_))

            __pre_new(ctx, __This->__type);
            ctx.stack.push<rt_ref_t>(ctx.heap->new_obj(__This->__type));

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("new %1%"), __This->__name);
        __EndToString()

        #endif  // EXEC_TRACE

    private:
        rt_type_t * __type;

        #if EXEC_TRACE
        rt_sid_t    __name;
        #endif  // EXEC_TRACE
    };

    //-------- ---------- ---------- ---------- ----------

    template<>
    class __new_command_t<xil_new_type_t::array> : public __command_base_t
    {
        typedef __new_command_t     __self_t;

    public:
        #if EXEC_TRACE
        __new_command_t(rt_type_t * type, rt_sid_t name)
            : __type(_M(rt_array_type_t *, type)), __name(name) { }
        #else
        __new_command_t(rt_type_t * type) : __type(_M(rt_array_type_t *, type)) { }
        #endif // EXEC_TRACE

        __BeginExecute(ctx, __ToCmdValue(new_, xil_new_type_t::array))

            __pre_new(ctx, __This->__type);
            dimension_t dimension = __This->__type->dimension;

            array_length_t lengths[dimension];
            for (int * len = lengths + dimension - 1; len >= lengths; len--)
            {
                *len = ctx.stack.pop<int32_t>();
            }

            ctx.stack.push(
                ctx.heap->new_array(__This->__type, lengths)
            );

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("new array %1%"), __This->__name);
        __EndToString()

        #endif  // EXEC_TRACE

    private:
        rt_array_type_t * __type;

        #if EXEC_TRACE
        rt_sid_t    __name;
        #endif  // EXEC_TRACE
    };

    //-------- ---------- ---------- ---------- ----------

    template<>
    class __new_command_t<xil_new_type_t::stack_alloc> : public __command_base_t
    {
        typedef __new_command_t     __self_t;

    public:
        __new_command_t(size_t units) _NE : __units(units) { }

        __BeginExecute(ctx, __ToCmdValue(new_, xil_new_type_t::stack_alloc))

            ctx.stack.push(
                ctx.stack.alloc_units(__This->__units)
            );

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("stack_alloc %1% units"), __This->__units);
        __EndToString()

        #endif  // EXEC_TRACE

    private:
        size_t __units;
    };

    //-------- ---------- ---------- ---------- ----------

    struct __new_command_template_t
    {
        template<xil_new_type_t _new_type, typename ... _args_t>
        static auto new_command(memory_t * memory, rt_type_t * type,
                                __context_t & ctx, _args_t && ... args)
        {
            typedef __new_command_t<_new_type> this_command_t;
            return __new_command<this_command_t>(memory, type,

            #if EXEC_TRACE
                type->get_name(ctx.env),
            #endif  // EXEC_TRACE

            std::forward<_args_t>(args) ...);
        }
    };

    struct __new_stack_alloc_command_template_t
    {
        template<__nokey_t, typename ... _args_t>
        static auto new_command(memory_t * memory, _args_t && ... args)
        {
            typedef __new_command_t<xil_new_type_t::stack_alloc> this_command_t;
            return __new_command<this_command_t>(memory,  std::forward<_args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    static command_t * __new_new_command(__context_t & ctx, const new_xil_t & xil)
    {
        static __command_manager_t<
            __new_command_template_t, xil_new_type_t
        >::with_args_t<rt_type_t *> __new_command_manager;

        static __command_manager_t<
            __new_stack_alloc_command_template_t, __nokey_t
        >::with_args_t<size_t> __stack_alloc_command_manager;

        ref_t type_ref = xil.type_ref();

        rt_type_t * type = ctx.get_type(type_ref);

        if (type == nullptr)
            throw _ED(exec_error_code_t::type_not_found, type_ref);

        switch (xil.new_type())
        {
            case xil_new_type_t::default_:
                return __new_command_manager.template get_command<xil_new_type_t::default_>(
                    __Mv(type), ctx
                );

            case xil_new_type_t::array:
                return __new_command_manager.template get_command<xil_new_type_t::array>(
                    __Mv(type), ctx
                );

            case xil_new_type_t::stack_alloc: {

                msize_t obj_size = type->get_size(ctx.env);
                size_t units = _alignf(obj_size, sizeof(rt_stack_unit_t)) / sizeof(rt_stack_unit_t);
                return __stack_alloc_command_manager.template get_command<__nokey>(__Mv(units));

            }   break;

            case xil_new_type_t::stack_allocs: {

                msize_t obj_size = type->get_size(ctx.env);
                uint32_t count = xil.count();

                size_t units = _alignf(obj_size * count, sizeof(rt_stack_unit_t))
                                                        / sizeof(rt_stack_unit_t);

                return __stack_alloc_command_manager.template get_command<__nokey>(__Mv(units));

            }   break;

            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    #define __ToCopyCmdValue(_copy_type, _flag)                                         \
        __ToCmdValue(copy, ((uint32_t)xil_copy_type_t::_copy_type << 16) | (uint32_t)_flag)

    class __copy_command_base_t : public __command_base_t
    {
    public:

        #if EXEC_TRACE

        __BeginToString()
            return _T("copy");
        __EndToString()

        #endif  // EXEC_TRACE

    };

    //-------- ---------- ---------- ---------- ----------

    template<size_t _size, xil_copy_kind_t _kind = xil_copy_kind_t::default_>
    struct __memory_copy_operation_t
    {
        __AlwaysInline static void copy(command_execute_context_t & ctx)
        {
            void * dst = ctx.stack.pop<void *>();
            void * src = ctx.stack.pop<void *>();

            al::quick_copy<_size>(dst, src);
        }
    };

    template<size_t _size>
    struct __memory_copy_operation_t<_size, xil_copy_kind_t::reverse>
    {
        __AlwaysInline static void copy(command_execute_context_t & ctx)
        {
            void * src = ctx.stack.pop<void *>();
            void * dst = ctx.stack.pop<void *>();

            al::quick_copy<_size>(dst, src);
        }
    };

    template<size_t _size>
    struct __memory_copy_operation_t<_size, xil_copy_kind_t::zero>
    {
        __AlwaysInline static void copy(command_execute_context_t & ctx)
        {
            void * p = ctx.stack.pop<void *>();

            al::quick_zero<_size>(p);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    template<size_t _size, xil_copy_kind_t _kind = xil_copy_kind_t::default_>
    class __memory_copy_command_t : public __copy_command_base_t
    {
        typedef __copy_command_base_t                   __super_t;
        typedef __memory_copy_command_t<_size, _kind>   __self_t;

    public:

        __BeginExecute(ctx, __ToCopyCmdValue(__default__, (((uint32_t)_kind << 8) | _size)))
            
            __memory_copy_operation_t<_size, _kind>::copy(ctx);

        __EndExecute()
    };

    template<__nokey_t>
    class __memory_copy_command_ex_t : public __copy_command_base_t
    {
        typedef __memory_copy_command_ex_t  __self_t;
        typedef __copy_command_base_t       __super_t;

    public:

        __memory_copy_command_ex_t(size_t size, xil_copy_kind_t kind) _NE
            : __size(size), __kind(kind) { }

        __BeginExecute(ctx, __ToCopyCmdValue(__default__, 0xFFFF))
            
            if (__This->__kind == xil_copy_kind_t::default_)
            {
                void * dst = ctx.stack.pop<void *>();
                void * src = ctx.stack.pop<void *>();

                al::quick_copy(dst, src, __This->__size);
            }
            else if(__This->__kind == xil_copy_kind_t::reverse)
            {
                void * src = ctx.stack.pop<void *>();
                void * dst = ctx.stack.pop<void *>();

                al::quick_copy(dst, src, __This->__size);
            }
            else if(__This->__kind == xil_copy_kind_t::zero)
            {
                void * p = ctx.stack.pop<void *>();
                al::quick_zero(p, __This->__size);
            }


        __EndExecute()

    private:
        size_t          __size;
        xil_copy_kind_t __kind;
    };

    struct __memory_copy_command_template_t
    {
        template<size_t _size, xil_copy_kind_t _kind, typename ... _args_t>
        static auto new_command(memory_t * memory, _args_t && ... args)
        {
            typedef __memory_copy_command_t<_size, _kind> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<_args_t>(args) ...);
        }
    };

    struct __memory_copy_command_ex_template_t
    {
        template<__nokey_t _nokey, typename ... _args_t>
        static auto new_command(memory_t * memory, _args_t && ... args)
        {
            typedef __memory_copy_command_ex_t<_nokey> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<_args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    static command_t * __get_object_copy_command(size_t size, xil_copy_kind_t kind)
    {
        static __command_manager_t<
            __memory_copy_command_template_t, size_t, xil_copy_kind_t
        >::with_args_t<> __memory_copy_command_manager;

        static __command_manager_t<
            __memory_copy_command_ex_template_t, __nokey_t
        >::with_args_t<size_t, xil_copy_kind_t> __memory_copy_command_ex_manager;


        #define __Case(_size, _kind)                                                    \
            case _size:                                                                 \
                return __memory_copy_command_manager.template get_command<_size, _kind>(\
                );                                                                      \

        #define __Switch(_size, _kind)                                                  \
            switch (_size)                                                              \
            {                                                                           \
                __Case(1,   _kind)                                                      \
                __Case(2,   _kind)                                                      \
                __Case(4,   _kind)                                                      \
                __Case(8,   _kind)                                                      \
                __Case(12,  _kind)                                                      \
                __Case(16,  _kind)                                                      \
                __Case(20,  _kind)                                                      \
                __Case(24,  _kind)                                                      \
                __Case(28,  _kind)                                                      \
                __Case(32,  _kind)                                                      \
                                                                                        \
                default:                                                                \
                    return __memory_copy_command_ex_manager.template get_command<__nokey>( \
                        __Mv(size), __Mv(kind)                                          \
                    );                                                                  \
            }

        switch (kind)
        {
            case xil_copy_kind_t::default_:
                __Switch(size, xil_copy_kind_t::default_)
                break;

            case xil_copy_kind_t::reverse:
                __Switch(size, xil_copy_kind_t::reverse)
                break;

            case xil_copy_kind_t::zero:
                __Switch(size, xil_copy_kind_t::zero);
                break;

            default:
                X_UNEXPECTED();
        }

        #undef __Case
        #undef __Switch
    }

    static command_t * __new_copy_command(__context_t & ctx, const copy_xil_t & xil)
    {
        switch (xil.copy_type())
        {
            case xil_copy_type_t::object_copy: {

                ref_t type_ref = xil.type_ref();
                rt_type_t * type = ctx.get_type(type_ref);
                msize_t size = type->get_size(ctx.env);
                xil_copy_kind_t kind = xil.copy_kind();

                return __get_object_copy_command(size, kind);

            }   break;

            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    class __init_command_base_t : public __command_base_t
    {
    public:

        #if EXEC_TRACE

        __BeginToString()
            return _T("init");
        __EndToString()

        #endif  // EXEC_TRACE
    };

    //-------- ---------- ---------- ---------- ----------

    template<xil_init_type_t _init_type, msize_t _size> class __init_command_t { };

    #define __ToInitCmdValue(_init_type, _size)                                         \
        __ToCmdValue(init, ((cmd_value_t)xil_init_type_t::_init_type << 16) | (cmd_value_t)_size)

    //-------- ---------- ---------- ---------- ----------

    template<>
    class __init_command_t<xil_init_type_t::array_begin, 0>
        : public __init_command_base_t
    {
        typedef __init_command_t        __self_t;
        typedef __init_command_base_t   __super_t;

    public:
        using __super_t::__super_t;

        __BeginExecute(ctx, __ToInitCmdValue(array_begin, 0))
            
            ctx.stack.push<void *>(
                (void *)ctx.stack.pick<rt_ref_t>()
            );

        __EndExecute()
    };

    //-------- ---------- ---------- ---------- ----------

    template<msize_t _size>
    class __init_command_t<xil_init_type_t::array_element, _size>
        : public __init_command_base_t
    {
        typedef __init_command_t        __self_t;
        typedef __init_command_base_t   __super_t;

    public:
        using __super_t::__super_t;

        __BeginExecute(ctx, __ToInitCmdValue(array_element, _size))

            typedef uint_type_t<_size> t;
            *ctx.stack.pick_reference<t *>()++ = ctx.stack.pop<t>();

        __EndExecute()
    };

    //-------- ---------- ---------- ---------- ----------

    template<>
    class __init_command_t<xil_init_type_t::array_element, 0>
        : public __init_command_base_t

    {
        typedef __init_command_t        __self_t;
        typedef __init_command_base_t   __super_t;

    public:
        __init_command_t(msize_t size) : __size(size) { }

        __BeginExecute(ctx, __ToInitCmdValue(array_element, 0))
            
            // TODO: implement it.
            X_UNEXPECTED();

        __EndExecute()

    private:
        msize_t __size;
    };

    //-------- ---------- ---------- ---------- ----------

    template<>
    class __init_command_t<xil_init_type_t::array_end, 0>
        : public __init_command_base_t
    {
        typedef __init_command_t        __self_t;
        typedef __init_command_base_t   __super_t;

    public:
        using __super_t::__super_t;

        __BeginExecute(ctx, __ToInitCmdValue(array_end, 0))

            ctx.stack.pop<void *>();

        __EndExecute()
    };

    //-------- ---------- ---------- ---------- ----------

    struct __init_command_template_t
    {
        template<xil_init_type_t _init_type, msize_t _size, typename ... _args_t>
        static auto new_command(memory_t * memory, _args_t && ... args)
        {
            typedef __init_command_t<_init_type, _size> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<_args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    static msize_t __size_of_array_element(__context_t & ctx, const init_xil_t & xil)
    {
        if (xil.dtype() == xil_type_t::empty)
        {
            rt_type_t * rt_type = ctx.get_type(xil.type_ref());
            //assembly_analyzer_t analyzer = __assembly_analyzer(ctx, rt_type);
            //return __size_of(analyzer, rt_type);

            X_UNEXPECTED();
            return 0;
        }

        return size_of(xil.dtype());
    }

    static command_t * __new_init_command(__context_t & ctx, const init_xil_t & xil)
    {
        static __command_manager_t<
            __init_command_template_t, xil_init_type_t, msize_t
        >::with_args_t<> __stack_init_command_manager;

        switch (xil.init_type())
        {
            case xil_init_type_t::array_begin:
                return __stack_init_command_manager.template
                    get_command<xil_init_type_t::array_begin, 0>();

            case xil_init_type_t::array_element: {
                msize_t size = __size_of_array_element(ctx, xil);
                switch (size)
                {
                    #define __Case(_size)                                               \
                        case _size:                                                     \
                            return __stack_init_command_manager.template                \
                                get_command<xil_init_type_t::array_element, _size>();   \

                    __Case(1)
                    __Case(2)
                    __Case(4)
                    __Case(8)

                    default:
                        return __stack_init_command_manager.template
                            get_command<xil_init_type_t::array_element, 0>(size);
                   
                    #undef __Case
                }

            }   break;

            case xil_init_type_t::array_end:
                return __stack_init_command_manager.template
                    get_command<xil_init_type_t::array_end, 0>();

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
        switch (xil->command())
        {
            case xil_command_t::empty:
                __ReturnInstance(__empty_command_t);

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

            case xil_command_t::init:
                return __new_init_command(ctx, *(const init_xil_t *)xil);

            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    command_t * new_end_command()
    {
        __ReturnInstance(__execute_end_command_t);
    }

    ////////// ////////// ////////// ////////// //////////

    #if EXEC_TRACE >= 2

    #if EXEC_TRACE >= 3

    #define __BeginExecuteCommand()                                                     \
        void * top0 = ctx.stack.top();                                                  \
        _PF(_T("   %|1$-50|[%2%]"), to_command_string(command), top0);

    #elif EXEC_TRACE >= 2   // EXEC_TRACE >= 3

    #define __BeginExecuteCommand()                                                     \
        void * top0 = ctx.stack.top();

    #endif  // EXEC_TRACE >= 3

    #define __EndExecuteCommand()                                                       \
        void * top1 = ctx.stack.top();                                                  \
        int    diff = (int)((byte_t *)top1 - (byte_t *)top0);                           \
        _PF(_T("-> %|1$-50|[%2%] %3%%4%"), to_command_string(command), top1,            \
                diff >= 0? _T("+") : _T(""), diff                                       \
        );

    #else   // EXEC_TRACE >= 2

    #define __BeginExecuteCommand()
    #define __EndExecuteCommand()

    #endif  // EXEC_TRACE >= 2

        
    #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_MANUAL_METHOD

    __AlwaysInline void __inline_execute_command(command_execute_context_t & ctx,
                                                 command_t * command)
    {
        __BeginExecuteCommand()

        command->execute_method(command, ctx);

        __EndExecuteCommand()
    }

    #if EXEC_TRACE

    const string_t to_command_string(command_t * command)
    {
        return command->to_string_method(command);
    }

    #endif  // EXEC_QUICK_EXECUTE

    #elif EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_INLINE   // EXEC_EXECUTE_MODEL == xxx

    //-------- ---------- ---------- ---------- ----------
    // __SwitchCommands

    // Push/Pop/Pick commands.
    #define __SwitchCommands_CaseSt(_name, _stype, _dtype1, _dtype2)                    \
        __Case( __##_name##_command_t<xil_storage_type_t::_stype,                       \
                            xil_type_t::_dtype1, xil_type_t::_dtype2> )

    #define __SwitchCommands_CaseSts_(_name, _stype, _dtype1)                           \
        __SwitchCommands_CaseSt(_name, _stype, _dtype1, int8)                           \
        __SwitchCommands_CaseSt(_name, _stype, _dtype1, uint8)                          \
        __SwitchCommands_CaseSt(_name, _stype, _dtype1, int16)                          \
        __SwitchCommands_CaseSt(_name, _stype, _dtype1, uint16)                         \
        __SwitchCommands_CaseSt(_name, _stype, _dtype1, int32)                          \
        __SwitchCommands_CaseSt(_name, _stype, _dtype1, uint32)                         \
        __SwitchCommands_CaseSt(_name, _stype, _dtype1, int64)                          \
        __SwitchCommands_CaseSt(_name, _stype, _dtype1, uint64)                         \
        __SwitchCommands_CaseSt(_name, _stype, _dtype1, float_)                         \
        __SwitchCommands_CaseSt(_name, _stype, _dtype1, double_)                        \
        __SwitchCommands_CaseSt(_name, _stype, _dtype1, char_)                          \
        __SwitchCommands_CaseSt(_name, _stype, _dtype1, bool_)

    #define __SwitchCommands_CaseSts(_name, _stype)                                     \
        __SwitchCommands_CaseSts_(_name, _stype, int8)                                  \
        __SwitchCommands_CaseSts_(_name, _stype, uint8)                                 \
        __SwitchCommands_CaseSts_(_name, _stype, int16)                                 \
        __SwitchCommands_CaseSts_(_name, _stype, uint16)                                \
        __SwitchCommands_CaseSts_(_name, _stype, int32)                                 \
        __SwitchCommands_CaseSts_(_name, _stype, uint32)                                \
        __SwitchCommands_CaseSts_(_name, _stype, int64)                                 \
        __SwitchCommands_CaseSts_(_name, _stype, uint64)                                \
        __SwitchCommands_CaseSts_(_name, _stype, float_)                                \
        __SwitchCommands_CaseSts_(_name, _stype, double_)                               \
        __SwitchCommands_CaseSts_(_name, _stype, char_)                                 \
        __SwitchCommands_CaseSts_(_name, _stype, bool_)                                 \
        __SwitchCommands_CaseSt(_name, _stype, object, object)                          \
        __SwitchCommands_CaseSt(_name, _stype, string, string)                          \
        __SwitchCommands_CaseSt(_name, _stype, ptr, ptr)

    #define __SwitchCommands_CaseArray(_name, _dtype1, _dtype2, _dimension)             \
        __Case( __##_name##_array_element_command_t<xil_type_t::_dtype1,                \
                                                    xil_type_t::_dtype2, _dimension> )

    #define __SwitchCommands_CaseArrays_(_name, _dtype1, _dimension)                    \
        __SwitchCommands_CaseArray(_name, _dtype1, int8,    _dimension)                 \
        __SwitchCommands_CaseArray(_name, _dtype1, uint8,   _dimension)                 \
        __SwitchCommands_CaseArray(_name, _dtype1, int16,   _dimension)                 \
        __SwitchCommands_CaseArray(_name, _dtype1, uint16,  _dimension)                 \
        __SwitchCommands_CaseArray(_name, _dtype1, int32,   _dimension)                 \
        __SwitchCommands_CaseArray(_name, _dtype1, uint32,  _dimension)                 \
        __SwitchCommands_CaseArray(_name, _dtype1, int64,   _dimension)                 \
        __SwitchCommands_CaseArray(_name, _dtype1, uint64,  _dimension)                 \
        __SwitchCommands_CaseArray(_name, _dtype1, float_,  _dimension)                 \
        __SwitchCommands_CaseArray(_name, _dtype1, double_, _dimension)                 \
        __SwitchCommands_CaseArray(_name, _dtype1, char_,   _dimension)                 \
        __SwitchCommands_CaseArray(_name, _dtype1, bool_,   _dimension)

    // Push/Pick/Pop Array commands.
    #define __SwitchCommands_CaseArrays(_name, _dimension)                              \
        __SwitchCommands_CaseArrays_(_name, int8,    _dimension)                        \
        __SwitchCommands_CaseArrays_(_name, uint8,   _dimension)                        \
        __SwitchCommands_CaseArrays_(_name, int16,   _dimension)                        \
        __SwitchCommands_CaseArrays_(_name, uint16,  _dimension)                        \
        __SwitchCommands_CaseArrays_(_name, int32,   _dimension)                        \
        __SwitchCommands_CaseArrays_(_name, uint32,  _dimension)                        \
        __SwitchCommands_CaseArrays_(_name, int64,   _dimension)                        \
        __SwitchCommands_CaseArrays_(_name, uint64,  _dimension)                        \
        __SwitchCommands_CaseArrays_(_name, float_,  _dimension)                        \
        __SwitchCommands_CaseArrays_(_name, double_, _dimension)                        \
        __SwitchCommands_CaseArrays_(_name, char_,   _dimension)                        \
        __SwitchCommands_CaseArrays_(_name, bool_,   _dimension)                        \
        __SwitchCommands_CaseArray(_name, object, object, _dimension)                   \
        __SwitchCommands_CaseArray(_name, string, string, _dimension)                   \
        __SwitchCommands_CaseArray(_name, ptr, ptr, _dimension)

    // Push Constants.

    #define __SwitchCommands_CaseConstant(_dtype1, _dtype2)                             \
        __Case( __push_command_t<xil_storage_type_t::constant,                          \
                    xil_type_t::_dtype1, xil_type_t::_dtype2> )
        
    #define __SwitchCommands_CaseConstants_(_dtype1)                                    \
        __SwitchCommands_CaseConstant(_dtype1, int8)                                    \
        __SwitchCommands_CaseConstant(_dtype1, uint8)                                   \
        __SwitchCommands_CaseConstant(_dtype1, int16)                                   \
        __SwitchCommands_CaseConstant(_dtype1, uint16)                                  \
        __SwitchCommands_CaseConstant(_dtype1, int32)                                   \
        __SwitchCommands_CaseConstant(_dtype1, uint32)                                  \
        __SwitchCommands_CaseConstant(_dtype1, int64)                                   \
        __SwitchCommands_CaseConstant(_dtype1, uint64)                                  \
        __SwitchCommands_CaseConstant(_dtype1, float_)                                  \
        __SwitchCommands_CaseConstant(_dtype1, double_)                                 \
        __SwitchCommands_CaseConstant(_dtype1, char_)                                   \
        __SwitchCommands_CaseConstant(_dtype1, bool_)

    #define __SwitchCommands_CaseConstants()                                            \
        __SwitchCommands_CaseConstants_(int8)                                           \
        __SwitchCommands_CaseConstants_(uint8)                                          \
        __SwitchCommands_CaseConstants_(int16)                                          \
        __SwitchCommands_CaseConstants_(uint16)                                         \
        __SwitchCommands_CaseConstants_(int32)                                          \
        __SwitchCommands_CaseConstants_(uint32)                                         \
        __SwitchCommands_CaseConstants_(int64)                                          \
        __SwitchCommands_CaseConstants_(uint64)                                         \
        __SwitchCommands_CaseConstants_(float_)                                         \
        __SwitchCommands_CaseConstants_(double_)                                        \
        __SwitchCommands_CaseConstants_(char_)                                          \
        __SwitchCommands_CaseConstants_(bool_)                                          \
        __SwitchCommands_CaseConstant(object, object)                                   \
        __SwitchCommands_CaseConstant(string, string)                                   \
        __SwitchCommands_CaseConstant(ptr, ptr)

    #define __SwitchCommands_CaseAddress(_stype)                                        \
        __Case( __push_address_command_t<xil_storage_type_t::_stype> )

    // binary, unitary commands
    #define __SwitchCommands_CaseBinary(_cmd, _dtype1, _dtype2)                         \
        __Case( __binary_command_t<decltype(_cmd), _cmd, xil_type_t::_dtype1, xil_type_t::_dtype2> )

    #define __SwitchCommands_CaseUnitary(_cmd, _dtype1)                                 \
        __Case( __unitary_command_t<decltype(_cmd), _cmd, xil_type_t::_dtype1> )

    #define __SwitchCommands_CaseBinary_1(_cmd, _dtype1, _dtype2)                       \
        __SwitchCommands_CaseBinary(_cmd, _dtype1, _dtype2)

    #define __SwitchCommands_CaseBinary_2(_cmd, _dtype1, _dtype2)                       \
        __SwitchCommands_CaseBinary(_cmd, _dtype1, _dtype2)                             \
        __SwitchCommands_CaseBinary(_cmd, _dtype2, _dtype1)

    #define __SwitchCommands_CaseBinaries(_cmd)                                         \
            __SwitchCommands_CaseBinary_1(_cmd, int8,   int8)                           \
            __SwitchCommands_CaseBinary_2(_cmd, int8,   uint8)                          \
                                                                                        \
            __SwitchCommands_CaseBinary_1(_cmd, uint8,  uint8)                          \
                                                                                        \
            __SwitchCommands_CaseBinary_2(_cmd, int16,  int8)                           \
            __SwitchCommands_CaseBinary_2(_cmd, int16,  uint8)                          \
            __SwitchCommands_CaseBinary_1(_cmd, int16,  int16)                          \
            __SwitchCommands_CaseBinary_2(_cmd, int16,  uint16)                         \
                                                                                        \
            __SwitchCommands_CaseBinary_2(_cmd, uint16, int8)                           \
            __SwitchCommands_CaseBinary_2(_cmd, uint16, uint8)                          \
            __SwitchCommands_CaseBinary_1(_cmd, uint16, uint16)                         \
                                                                                        \
            __SwitchCommands_CaseBinary_2(_cmd, int32,  int8)                           \
            __SwitchCommands_CaseBinary_2(_cmd, int32,  uint8)                          \
            __SwitchCommands_CaseBinary_2(_cmd, int32,  int16)                          \
            __SwitchCommands_CaseBinary_2(_cmd, int32,  uint16)                         \
            __SwitchCommands_CaseBinary_1(_cmd, int32,  int32)                          \
                                                                                        \
            __SwitchCommands_CaseBinary_2(_cmd, uint32, int8)                           \
            __SwitchCommands_CaseBinary_2(_cmd, uint32, uint8)                          \
            __SwitchCommands_CaseBinary_2(_cmd, uint32, int16)                          \
            __SwitchCommands_CaseBinary_2(_cmd, uint32, uint16)                         \
            __SwitchCommands_CaseBinary_2(_cmd, uint32, int32)                          \
            __SwitchCommands_CaseBinary_1(_cmd, uint32, uint32)                         \
                                                                                        \
            __SwitchCommands_CaseBinary_2(_cmd, int64,  int8)                           \
            __SwitchCommands_CaseBinary_2(_cmd, int64,  uint8)                          \
            __SwitchCommands_CaseBinary_2(_cmd, int64,  int16)                          \
            __SwitchCommands_CaseBinary_2(_cmd, int64,  uint16)                         \
            __SwitchCommands_CaseBinary_2(_cmd, int64,  int32)                          \
            __SwitchCommands_CaseBinary_2(_cmd, int64,  uint32)                         \
            __SwitchCommands_CaseBinary_1(_cmd, int64,  int64)                          \
                                                                                        \
            __SwitchCommands_CaseBinary_2(_cmd, uint64,  int8)                          \
            __SwitchCommands_CaseBinary_2(_cmd, uint64,  uint8)                         \
            __SwitchCommands_CaseBinary_2(_cmd, uint64,  int16)                         \
            __SwitchCommands_CaseBinary_2(_cmd, uint64,  uint16)                        \
            __SwitchCommands_CaseBinary_2(_cmd, uint64,  int32)                         \
            __SwitchCommands_CaseBinary_2(_cmd, uint64,  uint32)                        \
            __SwitchCommands_CaseBinary_2(_cmd, uint64,  int64)                         \
            __SwitchCommands_CaseBinary_1(_cmd, uint64,  uint64)

        #define __SwitchCommands_CaseFloatBinaries(_cmd)                                \
            __SwitchCommands_CaseBinary_2(_cmd, float_,  int8)                          \
            __SwitchCommands_CaseBinary_2(_cmd, float_,  uint8)                         \
            __SwitchCommands_CaseBinary_2(_cmd, float_,  int16)                         \
            __SwitchCommands_CaseBinary_2(_cmd, float_,  uint16)                        \
            __SwitchCommands_CaseBinary_2(_cmd, float_,  int32)                         \
            __SwitchCommands_CaseBinary_2(_cmd, float_,  uint32)                        \
            __SwitchCommands_CaseBinary_2(_cmd, float_,  int64)                         \
            __SwitchCommands_CaseBinary_1(_cmd, float_,  uint64)                        \
            __SwitchCommands_CaseBinary_1(_cmd, float_,  float_)                        \
                                                                                        \
            __SwitchCommands_CaseBinary_2(_cmd, double_,  int8)                         \
            __SwitchCommands_CaseBinary_2(_cmd, double_,  uint8)                        \
            __SwitchCommands_CaseBinary_2(_cmd, double_,  int16)                        \
            __SwitchCommands_CaseBinary_2(_cmd, double_,  uint16)                       \
            __SwitchCommands_CaseBinary_2(_cmd, double_,  int32)                        \
            __SwitchCommands_CaseBinary_2(_cmd, double_,  uint32)                       \
            __SwitchCommands_CaseBinary_2(_cmd, double_,  int64)                        \
            __SwitchCommands_CaseBinary_1(_cmd, double_,  uint64)                       \
            __SwitchCommands_CaseBinary_2(_cmd, double_,  float_)                       \
            __SwitchCommands_CaseBinary_1(_cmd, double_,  double_)

        #define __SwitchCommands_CaseUnitaries(_cmd)                                    \
            __SwitchCommands_CaseUnitary(_cmd, int8)                                    \
            __SwitchCommands_CaseUnitary(_cmd, uint8)                                   \
            __SwitchCommands_CaseUnitary(_cmd, int16)                                   \
            __SwitchCommands_CaseUnitary(_cmd, uint16)                                  \
            __SwitchCommands_CaseUnitary(_cmd, int32)                                   \
            __SwitchCommands_CaseUnitary(_cmd, uint32)                                  \
            __SwitchCommands_CaseUnitary(_cmd, int64)                                   \
            __SwitchCommands_CaseUnitary(_cmd, uint64)

        #define __SwitchCommands_CaseFloatUnitaries(_cmd)                               \
            __SwitchCommands_CaseUnitary(_cmd, float_)                                  \
            __SwitchCommands_CaseUnitary(_cmd, double_)

        #define __SwitchCommands_CaseRet(_ret_size)                                     \
            __Case( __ret_command_t<_ret_size> )

        #define __SwitchCommands_CaseCopy(_size, _kind)                                 \
            __Case( __memory_copy_command_t<_size, xil_copy_kind_t::_kind> )

        #define __SwitchCommands_CaseCopies(_kind)                                      \
            __SwitchCommands_CaseCopy(1,  _kind)                                        \
            __SwitchCommands_CaseCopy(2,  _kind)                                        \
            __SwitchCommands_CaseCopy(4,  _kind)                                        \
            __SwitchCommands_CaseCopy(8,  _kind)                                        \
            __SwitchCommands_CaseCopy(12, _kind)                                        \
            __SwitchCommands_CaseCopy(16, _kind)                                        \
            __SwitchCommands_CaseCopy(20, _kind)                                        \
            __SwitchCommands_CaseCopy(24, _kind)                                        \
            __SwitchCommands_CaseCopy(28, _kind)                                        \
            __SwitchCommands_CaseCopy(32, _kind)

    #define __SwitchCommands(_cmd_value)                                                \
        switch (_cmd_value)                                                             \
        {                                                                               \
            /* empty */                                                                 \
            __Case( __empty_command_t )                                                 \
                                                                                        \
            /* push */                                                                  \
            __SwitchCommands_CaseConstants()                                            \
            __SwitchCommands_CaseSts(push, local)                                       \
            __SwitchCommands_CaseSts(push, argument)                                    \
            __SwitchCommands_CaseSts(push, field)                                       \
            __SwitchCommands_CaseSts(push, convert)                                     \
            __SwitchCommands_CaseArrays(push, 0)                                        \
            __SwitchCommands_CaseArrays(push, 1)                                        \
            __SwitchCommands_CaseArrays(push, 2)                                        \
            __SwitchCommands_CaseArrays(push, 3)                                        \
            __SwitchCommands_CaseArrays(push, 4)                                        \
            __SwitchCommands_CaseArrays(push, 5)                                        \
            __SwitchCommands_CaseArrays(push, 6)                                        \
            __SwitchCommands_CaseArrays(push, 7)                                        \
            __SwitchCommands_CaseArrays(push, 8)                                        \
            __SwitchCommands_CaseAddress(local_addr)                                    \
            __SwitchCommands_CaseAddress(argument_addr)                                 \
            __Case( __duplicate_command_t )                                             \
                                                                                        \
            /* pop */                                                                   \
            __SwitchCommands_CaseSts(pop, local)                                        \
            __SwitchCommands_CaseSts(pop, argument)                                     \
            __SwitchCommands_CaseSts(pop, field)                                        \
            __SwitchCommands_CaseArrays(pop, 0)                                         \
            __SwitchCommands_CaseArrays(pop, 1)                                         \
            __SwitchCommands_CaseArrays(pop, 2)                                         \
            __SwitchCommands_CaseArrays(pop, 3)                                         \
            __SwitchCommands_CaseArrays(pop, 4)                                         \
            __SwitchCommands_CaseArrays(pop, 5)                                         \
            __SwitchCommands_CaseArrays(pop, 6)                                         \
            __SwitchCommands_CaseArrays(pop, 7)                                         \
            __SwitchCommands_CaseArrays(pop, 8)                                         \
            __Case( __pop_empty_command_t<__nokey> )                                    \
                                                                                        \
             /* pick */                                                                 \
            __SwitchCommands_CaseSts(pick, local)                                       \
            __SwitchCommands_CaseSts(pick, argument)                                    \
            __SwitchCommands_CaseSts(pick, field)                                       \
            __SwitchCommands_CaseArrays(pick, 0)                                        \
            __SwitchCommands_CaseArrays(pick, 1)                                        \
            __SwitchCommands_CaseArrays(pick, 2)                                        \
            __SwitchCommands_CaseArrays(pick, 3)                                        \
            __SwitchCommands_CaseArrays(pick, 4)                                        \
            __SwitchCommands_CaseArrays(pick, 5)                                        \
            __SwitchCommands_CaseArrays(pick, 6)                                        \
            __SwitchCommands_CaseArrays(pick, 7)                                        \
            __SwitchCommands_CaseArrays(pick, 8)                                        \
                                                                                        \
            /* call */                                                                  \
            __Case( __internal_call_command_t<__nokey> )                                \
            __Case( __static_call_command_t<rt_generic_method_t> )                      \
            __Case( __static_call_command_t<rt_method_t> )                              \
            __Case( __instance_call_command_t<rt_generic_method_t> )                    \
            __Case( __instance_call_command_t<rt_method_t> )                            \
            __Case( __virtual_call_command_t<__nokey> )                                 \
                                                                                        \
            /* al */                                                                    \
            __SwitchCommands_CaseBinaries(xil_al_command_t::add)                        \
            __SwitchCommands_CaseFloatBinaries(xil_al_command_t::add)                   \
            __SwitchCommands_CaseBinaries(xil_al_command_t::sub)                        \
            __SwitchCommands_CaseFloatBinaries(xil_al_command_t::sub)                   \
            __SwitchCommands_CaseBinaries(xil_al_command_t::mul)                        \
            __SwitchCommands_CaseFloatBinaries(xil_al_command_t::mul)                   \
            __SwitchCommands_CaseBinaries(xil_al_command_t::div)                        \
            __SwitchCommands_CaseFloatBinaries(xil_al_command_t::div)                   \
            __SwitchCommands_CaseBinaries(xil_al_command_t::mod)                        \
            __SwitchCommands_CaseUnitaries(xil_al_command_t::minus)                     \
            __SwitchCommands_CaseFloatUnitaries(xil_al_command_t::minus)                \
            __SwitchCommands_CaseUnitaries(xil_al_command_t::positive)                  \
            __SwitchCommands_CaseFloatUnitaries(xil_al_command_t::positive)             \
                                                                                        \
            /* bit */                                                                   \
            __SwitchCommands_CaseBinaries(xil_bit_command_t::bit_and)                   \
            __SwitchCommands_CaseBinaries(xil_bit_command_t::bit_or)                    \
            __SwitchCommands_CaseBinaries(xil_bit_command_t::bit_xor)                   \
            __SwitchCommands_CaseBinaries(xil_bit_command_t::left_shift)                \
            __SwitchCommands_CaseBinaries(xil_bit_command_t::right_shift)               \
            __SwitchCommands_CaseUnitaries(xil_bit_command_t::bit_not)                  \
                                                                                        \
            /* logic */                                                                 \
            __SwitchCommands_CaseBinary(xil_logic_command_t::and_, bool_, bool_)        \
            __SwitchCommands_CaseBinary(xil_logic_command_t::or_, bool_, bool_)         \
            __SwitchCommands_CaseUnitary(xil_logic_command_t::or_, bool_)               \
                                                                                        \
            /* cmp */                                                                   \
            __SwitchCommands_CaseBinaries(xil_cmp_command_t::greater)                   \
            __SwitchCommands_CaseFloatBinaries(xil_cmp_command_t::greater)              \
            __SwitchCommands_CaseBinaries(xil_cmp_command_t::greater_equal)             \
            __SwitchCommands_CaseFloatBinaries(xil_cmp_command_t::greater_equal)        \
            __SwitchCommands_CaseBinaries(xil_cmp_command_t::less)                      \
            __SwitchCommands_CaseFloatBinaries(xil_cmp_command_t::less)                 \
            __SwitchCommands_CaseBinaries(xil_cmp_command_t::less_equal)                \
            __SwitchCommands_CaseFloatBinaries(xil_cmp_command_t::less_equal)           \
            __SwitchCommands_CaseBinaries(xil_cmp_command_t::equal)                     \
            __SwitchCommands_CaseFloatBinaries(xil_cmp_command_t::equal)                \
            __SwitchCommands_CaseBinaries(xil_cmp_command_t::not_equal)                 \
            __SwitchCommands_CaseFloatBinaries(xil_cmp_command_t::not_equal)            \
            __SwitchCommands_CaseBinary(xil_cmp_command_t::equal, bool_, bool_)         \
            __SwitchCommands_CaseBinary(xil_cmp_command_t::not_equal, bool_, bool_)     \
            __SwitchCommands_CaseBinary(xil_cmp_command_t::equal, char_, char_)         \
            __SwitchCommands_CaseBinary(xil_cmp_command_t::not_equal, char_, char_)     \
            __SwitchCommands_CaseBinary(xil_cmp_command_t::greater, char_, char_)       \
            __SwitchCommands_CaseBinary(xil_cmp_command_t::greater_equal, char_, char_) \
            __SwitchCommands_CaseBinary(xil_cmp_command_t::less, char_, char_)          \
            __SwitchCommands_CaseBinary(xil_cmp_command_t::less_equal, char_, char_)    \
            __SwitchCommands_CaseBinary(xil_cmp_command_t::equal, object, object)       \
            __SwitchCommands_CaseBinary(xil_cmp_command_t::not_equal, object, object)   \
            __SwitchCommands_CaseBinary(xil_cmp_command_t::equal, ptr, ptr)             \
            __SwitchCommands_CaseBinary(xil_cmp_command_t::not_equal, ptr, ptr)         \
                                                                                        \
            /* ret */                                                                   \
            __SwitchCommands_CaseRet(0)                                                 \
            __SwitchCommands_CaseRet(1)                                                 \
            __SwitchCommands_CaseRet(2)                                                 \
            __SwitchCommands_CaseRet(3)                                                 \
            __SwitchCommands_CaseRet(4)                                                 \
            __SwitchCommands_CaseRet(5)                                                 \
            __SwitchCommands_CaseRet(6)                                                 \
            __SwitchCommands_CaseRet(7)                                                 \
            __SwitchCommands_CaseRet(8)                                                 \
            __SwitchCommands_CaseRet(__large_ret_unit_size)                             \
                                                                                        \
            /* label */                                                                 \
            __Case( __label_command_t )                                                 \
                                                                                        \
            /* throw */                                                                 \
            __Case( __throw_command_t )                                                 \
            __Case( __rethrow_command_t )                                               \
            __Case( __end_finally_command_t )                                           \
                                                                                        \
            /* jmp */                                                                   \
            __Case( __jmp_command_t<xil_jmp_model_t::none> )                            \
            __Case( __jmp_command_t<xil_jmp_model_t::true_> )                           \
            __Case( __jmp_command_t<xil_jmp_model_t::false_> )                          \
            __Case( __jmp_command_t<xil_jmp_model_t::leave> )                           \
            __Case( __leave_ret_command_t )                                             \
            __Case( __switch_jmp_command_t )                                            \
                                                                                        \
            /* new */                                                                   \
            __Case( __new_command_t<xil_new_type_t::default_> )                         \
            __Case( __new_command_t<xil_new_type_t::array> )                            \
            __Case( __new_command_t<xil_new_type_t::stack_alloc> )                      \
                                                                                        \
            /* copy */                                                                  \
            __SwitchCommands_CaseCopies(default_)                                       \
            __SwitchCommands_CaseCopies(reverse)                                        \
            __SwitchCommands_CaseCopies(zero)                                           \
            __Case( __memory_copy_command_ex_t<__nokey> )                               \
                                                                                        \
            /* end */                                                                   \
            __Case( __execute_end_command_t )                                           \
                                                                                        \
        default:                                                                        \
            __Default(_cmd_value)                                                       \
        }

    //-------- ---------- ---------- ---------- ----------

    __AlwaysInline void __inline_execute_command(command_execute_context_t & ctx,
                                                 command_t * command)
    {
        __BeginExecuteCommand()

        #define __Case(_t, _ts...)                                                      \
            case _t, ##_ts::cmd_value:                                                  \
                ((_t, ##_ts *)command)->execute(ctx);                                   \
                break;

        #define __Default(_cmd_value)                                                   \
            throw _EC(unexpected, _F(_T("unexpected command %1%"), __Cmd(_cmd_value)));

        __SwitchCommands(command->cmd_value)

        #undef __Case
        #undef __Default

        __EndExecuteCommand()
    }

    #if EXEC_TRACE
    const string_t to_command_string(command_t * command)
    {
        #define __Case(_t, _ts...)                                                      \
            case _t, ##_ts::cmd_value:                                                  \
                return ((_t, ##_ts *)command)->to_string();

        #define __Default(_cmd_value)                                                   \
            return _F(_T("[%1%]"), _cmd_value);

        __SwitchCommands(command->cmd_value)

        #undef __Default
        #undef __Case
    }

    #endif  // EXEC_TRACE

    #elif EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL_METHOD // EXEC_EXECUTE_MODEL == xxx

    __AlwaysInline void __inline_execute_command(command_execute_context_t & ctx,
                                                 command_t * command)
    {
        __BeginExecuteCommand()

        command->execute(ctx);

        __EndExecuteCommand()
    }

    #if EXEC_TRACE

    const string_t to_command_string(command_t * command)
    {
        return command->to_string();
    }

    #endif  // EXEC_TRACE

    #endif  // EXEC_EXECUTE_MODEL == xxx

    typedef logic_error_t<exec_env_code_t> __exec_env_error_t;

    __exec_env_error_t __execute_commands(command_execute_context_t & ctx,
                                          command_t ** & commands)
    {
        try
        {
    __begin__:
            __inline_execute_command(ctx, *commands++);
            goto __begin__;
        }
        catch (__exec_env_error_t & e)
        {
            return e;
        }
    }

    void __execute_command(command_execute_context_t & ctx, command_t * command)
    {
        command_t * commands[] = { command, new_end_command() };
        command_t ** commands1 = commands;

        __execute_commands(ctx, commands1);
    }

    // Process the exception at the end of executing.
    void __process_application_exception(command_execute_context_t & ctx, __exec_env_error_t & e)
    {
        switch (e.code)
        {
            case exec_env_code_t::end:
                _A(ctx.exception_stack.empty());
                break;

            case exec_env_code_t::terminal: {

                exception_node_t * node = ctx.exception_stack.head;

                _A(node != nullptr);
                rt_type_t * exception_type = mm::get_object_type(node->exception);

                _PF(_T("Uncaught Exception: %1%"), node->exception);

            }   break;

            default:
                X_UNEXPECTED();
        }
    }

    void execute_commands(command_execute_context_t & ctx, command_t ** commands)
    {
        #if EXEC_TRACE
        _PF(_T("-> %|1$-50|[%2%]"), _T("<begin>"), ctx.stack.top());
        #endif  // EXEC_TRACE

        ctx.push_calling(commands);

        __exec_env_error_t e = __execute_commands(ctx, ctx.current);
        __process_application_exception(ctx, e);

        ctx.pop_calling();

        #if EXEC_TRACE
        _PF(_T("-> %|1$-50|[%2%]"), _T("<end>"), ctx.stack.top());
        #endif  // EXEC_TRACE
    }

    ////////// ////////// ////////// ////////// //////////

    #undef __PrintCurrent

} } }
