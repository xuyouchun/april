#include <exec.h>
#include "commands.h"
#include <rtlib.h>
#include <mm.h>
#include "utils.h"
#include "parser.h"
#include <lib.h>

namespace X_ROOT_NS::modules::exec {

    #define __AlwaysInline  X_ALWAYS_INLINE

    #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_MANUAL

    #define __This  (static_cast<__self_t *>(command))
    static std::set<command_t *> __ret_commands;

    #define __BeginExecute(_ctx)                                                        \
                                                                                        \
        static void execute(command_t * command, command_execute_context_t & _ctx)      \
        {

    #define __BeginToString()                                                           \
        static const string_t to_string(command_t * command)                            \
        {

    #elif EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_INLINE   // EXEC_EXECUTE_MODEL == xxx

    #define __This  this
    #define __NoThrow

    #define __BeginExecute(_ctx)                                                        \
                                                                                        \
        __AlwaysInline void execute(command_execute_context_t & _ctx) _NE               \
        {

    #define __BeginToString()                                                           \
        __AlwaysInline const string_t to_string()                                       \
        {

    #elif EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL // EXEC_EXECUTE_MODEL == xxx

    #define __This  this

    #define __BeginExecute(_ctx)                                                        \
                                                                                        \
        virtual void execute(command_execute_context_t & ctx) override                  \
        {

    #define __BeginToString()                                                           \
                                                                                        \
        virtual const string_t to_string() const override                               \
        {

    #else
        
        #error unknown EXEC_EXECUTE_MODEL

    #endif  // EXEC_EXECUTE_MODEL == xxx

    #define __EndExecute()                                                              \
        }

    #define __EndToString()                                                             \
        }

    #define __DefineCmdValue_(_name)    constexpr command_value_t _name = __COUNTER__;
    #define __Cmd(_cmd, _n)             __cmd_##_cmd##_##_n
    #define __DefineCmdValue(_cmd, _n)  __DefineCmdValue_(__Cmd(_cmd, _n));

    #define __ToString(_s)                                                              \
        __BeginToString()                                                               \
            return _s;                                                                  \
        __EndToString()

    #define __Terminal(_msg)    throw _E(exec_env_code_t::terminal, _msg)

    #define __TerminalOnNull(_exp)   do {                                               \
            _A(_exp != nullptr);                                                        \
            if ((_exp) == nullptr) __Terminal(_S(_exp) " is null");                     \
        } while (false)

    ////////// ////////// ////////// ////////// //////////

    using namespace rt;
    using namespace rtlib;

    void __execute_command(command_execute_context_t & ctx, command_t * command);
    const string_t to_command_string(command_t * command);

    void __process_exception(command_execute_context_t & ctx);

    ////////// ////////// ////////// ////////// //////////
    // command_creating_context_t

    command_creating_context_t::command_creating_context_t(executor_env_t & env,
        rt_assembly_t * assembly, rt_type_t * type, rt_method_base_t * method,
        const generic_param_manager_t * gp_manager)
        : __super_t(env, assembly, gp_manager), type(type), method(method), env(env)
        , locals_layout(*this), params_layout(*this)
    {
        _A(assembly != nullptr);
        _A(type != nullptr);
        _A(method != nullptr);

        _A(is_runnable_type(type));
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

    typedef rt_stack_unit_t __unit_t;
    const msize_t __stack_stub_size = unit_size_of(sizeof(__calling_stub_t));

    static al::xheap_t __command_heap(_T("commands"));

    template<typename _cmd_t, typename ... _args_t>
    _cmd_t * __new_command(memory_t * memory, _args_t && ... args)
    {
        _cmd_t * cmd = memory_t::new_obj<_cmd_t>(memory, std::forward<_args_t>(args) ...);

        #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_MANUAL
        
        ((command_t *)cmd)->execute_method = (command_execute_method_t)_cmd_t::execute;

        #if EXEC_TRACE
        ((command_t *)cmd)->to_string_method = (command_to_string_method_t)_cmd_t::to_string;
        #endif  // EXEC_TRACE

        #endif

        return cmd;
    }

    #define __ReturnInstance(_class_name)                                               \
        do {                                                                            \
            static _class_name * command = __new_command<_class_name>(&__command_heap); \
            return command;                                                             \
        } while (false)

    ////////// ////////// ////////// ////////// //////////
    // Core types

    static rt_type_t * __get_core_type(command_execute_context_t & ctx,
                const string_t & ns, const string_t & name, int generic_param_count = 0)
    {
        rt_type_t * type = ctx.env.get_core_type(ns, name, generic_param_count);
        __TerminalOnNull(type);

        return type;
    }

    #define __DefineGetCoreTypeFunction(_func_name, _ns, _name, _generic_param_count)   \
        static rt_type_t * _func_name(command_execute_context_t & ctx)                  \
        {                                                                               \
            static rt_type_t * type = nullptr;                                          \
            if (type == nullptr)                                                        \
                type = __get_core_type(ctx, _ns, _name, _generic_param_count);          \
                                                                                        \
            return type;                                                                \
        }

    #define __DefineSimpleGetCoreTypeFunction(_func_name, _name)                        \
        __DefineGetCoreTypeFunction(_func_name, _T(""), _name, 0)

    #define __DS    __DefineSimpleGetCoreTypeFunction
    __DS(__get_invalid_cast_exception,      CoreType_InvalidCastException)
    __DS(__get_null_reference_exception,    CoreType_NullReferenceException)
    __DS(__get_out_of_range_exception,      CoreType_OutOfRangeException)
    #undef __DS

    // Raise exception with specified type.
    static void __raise_exception(command_execute_context_t & ctx,
                                rt_type_t * exception_type, const char_t * message = nullptr);

    // Raise System.NullReferenceException when specified obj is null.
    __AlwaysInline static bool __check_null_reference(command_execute_context_t & ctx, rt_ref_t obj)
    {
        if (obj == nullptr)
        {
            rt_type_t * exception_type = __get_null_reference_exception(ctx);
            __raise_exception(ctx, exception_type);

            return false;
        }

        return true;
    }

    #define __CheckNullReference(_ctx, _obj)                                            \
        do {                                                                            \
            if (!__check_null_reference(_ctx, _obj))                                    \
                return;                                                                 \
        } while (false)

    #define __RaiseException(_ctx, _exception_name)                                     \
        do {                                                                            \
            rt_type_t * exception_type = __get_##_exception_name##_exception(_ctx);     \
            __raise_exception(_ctx, exception_type);                                    \
            return;                                                                     \
        } while (false)

    #define __RaiseInvalidCastException(_ctx)   __RaiseException(_ctx, invalid_cast)
    #define __RaiseOutOfRangeException(_ctx)    __RaiseException(_ctx, out_of_range)

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
            msize_t size = variable_size_of(method->get_return_type(env), &__ret_storage_type);
            __ret_unit_size = unit_size_of(size);
        }

        return __ret_unit_size;
    }

    // Returns command index.
    int __command_index(command_execute_context_t & ctx, command_t ** command)
    {
        exec_method_t * method = ctx.current_method();
        return command - method->get_commands();
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

    template<typename _command_template_t, typename ... _template_keys_t>
    class __command_manager_t : public object_t
    {
    public:
        template<typename ... _args_t> class with_args_t
        {
            typedef std::tuple<command_value_t, _args_t ...> __key_t;

        public:
            with_args_t(memory_t * memory) : __memory(memory) { }
            with_args_t() : with_args_t(&__command_heap) { }

            template<command_value_t _cv, _template_keys_t ... _keys, typename ... _others_t>
            command_t * get_command(_args_t ... args, _others_t && ... others)
            {
                __key_t key(_cv, args ...);

                auto it = __map.find(key);
                if (it != __map.end())
                    return it->second;

                command_t * command = _command_template_t::template new_command<_cv, _keys ...>(
                    __memory, std::forward<_args_t>(args) ..., std::forward<_others_t>(others) ...
                );

                __map[key] = command;
                return command;
            }

        private:
            std::map<__key_t, command_t *> __map;
            memory_t * __memory;
        };

        X_TO_STRING_IMPL(_T("__command_manager_t"))
    };

    ////////// ////////// ////////// ////////// //////////

    template<command_value_t _cv>
    class __command_base_t : public command_t
    {
        typedef command_t __super_t;

    public:

        #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_INLINE

        __command_base_t() _NE : __super_t(_cv) { }

        static constexpr command_value_t cmd_value = _cv;

        #endif  // EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_INLINE

        #if EXEC_TRACE

        __ToString(_T("[command]"));

        #endif      // EXEC_TRACE
    };

    ////////// ////////// ////////// ////////// //////////
    // Class __empty_command_t

    #define __EmptyCmd          __Cmd(empty, none)
    __DefineCmdValue_(__EmptyCmd)

    class __empty_command_t : public __command_base_t<__EmptyCmd>
    {
        typedef __empty_command_t   __self_t;

    public:

        __BeginExecute(ctx)

            // Do nothing

        __EndExecute()

        #if EXEC_TRACE

        __ToString(_T("[empty]"));

        #endif      // EXEC_TRACE
    };

    ////////// ////////// ////////// ////////// //////////
    // Class __execute_end_command_t

    #define __EndCmd            __Cmd(empty, end)
    __DefineCmdValue_(__EndCmd)

    class __execute_end_command_t : public __command_base_t<__EndCmd>
    {
        typedef __execute_end_command_t __self_t;

    public:

        __BeginExecute(ctx)

            #ifndef __NoThrow

            throw _E(exec_env_code_t::end);

            #endif  // __NoThrow

        __EndExecute()

        #if EXEC_TRACE

        __ToString(_T("[end]"));

        #endif  // EXEC_TRACE
    };

    ////////// ////////// ////////// ////////// //////////

    #define __EachXilTypePair(_xt1)                                                     \
        __CaseXilTypePair( _xt1, int8 )                                                 \
        __CaseXilTypePair( _xt1, uint8 )                                                \
        __CaseXilTypePair( _xt1, int16 )                                                \
        __CaseXilTypePair( _xt1, uint16 )                                               \
        __CaseXilTypePair( _xt1, int32 )                                                \
        __CaseXilTypePair( _xt1, uint32 )                                               \
        __CaseXilTypePair( _xt1, int64 )                                                \
        __CaseXilTypePair( _xt1, uint64 )                                               \
        __CaseXilTypePair( _xt1, float_ )                                               \
        __CaseXilTypePair( _xt1, double_ )                                              \
        __CaseXilTypePair( _xt1, char_ )                                                \
        __CaseXilTypePair( _xt1, bool_ )

    #define __EachXilTypePairs()                                                        \
        __EachXilTypePair( int8 )                                                       \
        __EachXilTypePair( uint8 )                                                      \
        __EachXilTypePair( int16 )                                                      \
        __EachXilTypePair( uint16 )                                                     \
        __EachXilTypePair( int32 )                                                      \
        __EachXilTypePair( uint32 )                                                     \
        __EachXilTypePair( int64 )                                                      \
        __EachXilTypePair( uint64 )                                                     \
        __EachXilTypePair( float_ )                                                     \
        __EachXilTypePair( double_ )                                                    \
        __EachXilTypePair( char_ )                                                      \
        __EachXilTypePair( bool_ )                                                      \
                                                                                        \
        __CaseXilTypePair( object, object )                                             \
        __CaseXilTypePair( string, string )                                             \
        __CaseXilTypePair( ptr, ptr )


    #define __EachDimensionXilTypePair_(_xt1, _dimension)                               \
        __CaseDimensionXilTypePair( _xt1, int8,    _dimension )                         \
        __CaseDimensionXilTypePair( _xt1, uint8,   _dimension )                         \
        __CaseDimensionXilTypePair( _xt1, int16,   _dimension )                         \
        __CaseDimensionXilTypePair( _xt1, uint16,  _dimension )                         \
        __CaseDimensionXilTypePair( _xt1, int32,   _dimension )                         \
        __CaseDimensionXilTypePair( _xt1, uint32,  _dimension )                         \
        __CaseDimensionXilTypePair( _xt1, int64,   _dimension )                         \
        __CaseDimensionXilTypePair( _xt1, uint64,  _dimension )                         \
        __CaseDimensionXilTypePair( _xt1, float_,  _dimension )                         \
        __CaseDimensionXilTypePair( _xt1, double_, _dimension )                         \
        __CaseDimensionXilTypePair( _xt1, char_,   _dimension )                         \
        __CaseDimensionXilTypePair( _xt1, bool_,   _dimension )

    #define __EachDimensionXilTypePairs_(_dimension)                                    \
        __EachDimensionXilTypePair_( int8,   _dimension )                               \
        __EachDimensionXilTypePair_( uint8,  _dimension )                               \
        __EachDimensionXilTypePair_( int16,  _dimension )                               \
        __EachDimensionXilTypePair_( uint16, _dimension )                               \
        __EachDimensionXilTypePair_( int32,  _dimension )                               \
        __EachDimensionXilTypePair_( uint32, _dimension )                               \
        __EachDimensionXilTypePair_( int64,  _dimension )                               \
        __EachDimensionXilTypePair_( uint64, _dimension )                               \
        __EachDimensionXilTypePair_( float_, _dimension )                               \
        __EachDimensionXilTypePair_( double_,_dimension )                               \
        __EachDimensionXilTypePair_( char_,  _dimension )                               \
        __EachDimensionXilTypePair_( bool_,  _dimension )                               \
                                                                                        \
        __CaseDimensionXilTypePair( object, object, _dimension )                        \
        __CaseDimensionXilTypePair( string, string, _dimension )                        \
        __CaseDimensionXilTypePair( ptr, ptr, _dimension )

    #define __EachDimensionXilTypePairs()                                               \
        __EachDimensionXilTypePairs_(0)                                                 \
        __EachDimensionXilTypePairs_(1)                                                 \
        __EachDimensionXilTypePairs_(2)                                                 \
        __EachDimensionXilTypePairs_(3)                                                 \
        __EachDimensionXilTypePairs_(4)                                                 \
        __EachDimensionXilTypePairs_(5)                                                 \
        __EachDimensionXilTypePairs_(6)                                                 \
        __EachDimensionXilTypePairs_(7)                                                 \
        __EachDimensionXilTypePairs_(8)
    
    #define __StCmd(_cmd, _st, _xt1, _xt2)      __Cmd(_cmd, _st##_##_xt1##_##_xt2)
    #define __DefineStCmdValue(_cmd, _st, _xt1, _xt2)                                   \
                                    __DefineCmdValue_(__StCmd(_cmd, _st, _xt1, _xt2))

    #define __StAddrCmd(_cmd, _st)              __Cmd(_cmd, _st)
    #define __DefineStAddrCmdValue(_cmd, _st)   __DefineCmdValue_(__StAddrCmd(_cmd, _st))

    #define __StArrayCmd(_cmd, _xt1, _xt2, _dimension)                                  \
                                    __Cmd(_cmd, array_element_##_xt1##_##_xt2##_##_dimension)

    #define __DefineStArrayCmdValue(_cmd, _xt1, _xt2, _dimension)                       \
                    __DefineCmdValue_(__StArrayCmd(_cmd, _xt1, _xt2, _dimension))

    #define __StArrayElementAddrCmd(_cmd, _dimension)                                   \
                    __Cmd(_cmd, array_element_addr_##_dimension)

    #define __DefineStArrayElementAddrCmdValue(_cmd, _dimension)                        \
                    __DefineCmdValue_(__StArrayElementAddrCmd(_cmd, _dimension))

    ////////// ////////// ////////// ////////// //////////
    // Class __push_command_t

    template<command_value_t _cv, xil_storage_type_t, xil_type_t, xil_type_t>
    class __push_command_t { };

    struct __push_command_template_t
    {
        template<command_value_t _cv,
            xil_storage_type_t _st, xil_type_t _xt1, xil_type_t _xt2, typename ... _args_t
        >
        static auto new_command(memory_t * memory, _args_t && ... args)
        {
            typedef __push_command_t<_cv, _st, _xt1, _xt2> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<_args_t>(args) ...);
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

    // Returns xil_type of rt_type.
    static xil_type_t __xil_type_of(__context_t & ctx, rt_type_t * type)
    {
        _A(type != nullptr);

        vtype_t vtype = type->get_vtype(ctx.env);
        return to_xil_type(vtype);
    }

    static xil_type_t __xil_type_of_argument(__context_t & ctx, uint16_t identity)
    {
        param_type_t param_type;
        rt_type_t * type = ctx.params_layout.type_at(identity, nullptr, &param_type);

        if (is_addr_param(param_type))
            return xil_type_t::ptr;

        return __xil_type_of(ctx, type);
    }

    static xil_type_t __xil_type_of_local(__context_t & ctx, uint16_t identity)
    {
        rt_type_t * type = ctx.locals_layout.type_at(identity);
        return __xil_type_of(ctx, type);
    }

    // Fetches the data type of specified xil.
    template<typename _xil_t>
    xil_type_t __fetch_dtype(__context_t & ctx, const _xil_t & xil)
    {
        xil_type_t dtype = xil.dtype();

        if (dtype != xil_type_t::empty)
            return dtype;

        xil_storage_type_t storage_type = xil.stype();
        switch (storage_type)
        {
            case xil_storage_type_t::field:
            case xil_storage_type_t::field_addr: {
                return __xil_type_of(ctx, __field_type(ctx, xil.get_ref()));
            }

            case xil_storage_type_t::constant: {
                ref_t type_ref = xil.get_ref();
                return __xil_type_of(ctx, ctx.get_type(type_ref));
            }

            case xil_storage_type_t::argument:
            case xil_storage_type_t::argument_addr: {
                int index = xil.get_identity();
                return __xil_type_of_argument(ctx, xil.get_identity());
            }

            case xil_storage_type_t::local:
            case xil_storage_type_t::local_addr: {
                int index = xil.get_identity();
                return __xil_type_of_local(ctx, xil.get_identity());
            }

            default:
                return xil_type_t::empty;
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
            if (_xt1 == _xt2)                                                           \
                return _F(_T("push %1% %2% [%3%]"), xil_storage_type_t::_st,            \
                    _xt1, __This->_v);                                                  \
                                                                                        \
            return _F(_T("push %1% %2%=>%3% [%4%]"), xil_storage_type_t::_st,           \
                _xt1, _xt2, __This->_v);                                                \
                                                                                        \
        __EndToString()

    #else

        #define __PushCommand_ToString(_st, _xt1, _xt2, _v)

    #endif

    //-------- ---------- ---------- ---------- ----------
    // Push constants

    template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2>
    class __push_command_t<_cv, xil_storage_type_t::constant, _xt1, _xt2>
        : public __command_base_t<_cv>
    {
        typedef __push_command_t        __self_t;
        typedef __command_base_t<_cv>   __super_t;
        typedef __const_vnum_t<_xt1>    __value1_t;
        typedef __const_vnum_t<_xt2>    __value2_t;

    public:
        __push_command_t(__value1_t value) : __value(value) { }

        __BeginExecute(ctx)

            ctx.stack.push(static_cast<__value2_t>(__This->__value));

        __EndExecute()

        __PushCommand_ToString(constant, _xt1, _xt2, __value)

    private:
        __value1_t __value;
    };

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(push, constant, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    //-------- ---------- ---------- ---------- ----------
    // Push local commands.

    template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2>
    class __push_command_t<_cv, xil_storage_type_t::local, _xt1, _xt2>
        : public __command_base_t<_cv>
    {
        typedef __push_command_t    __self_t;
        typedef __vnum_t<_xt1>      __value1_t;
        typedef __vnum_t<_xt2>      __value2_t;

    public:
        __push_command_t(__offset_t offset) : __offset(offset) { }

        __PushCommand_ToString(local, _xt1, _xt2, __offset)

        __BeginExecute(ctx)

            ctx.stack.push(static_cast<__value2_t>(__Local(__value1_t, __This->__offset)));

        __EndExecute()

    private:
        __offset_t __offset;
    };

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(push, local, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

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

    __DefineStAddrCmdValue(push, local_addr)

    template<>
    class __push_address_command_t<xil_storage_type_t::local_addr>
        : public __command_base_t<__StAddrCmd(push, local_addr)>
    {
        typedef __push_address_command_t    __self_t;

    public:
        __push_address_command_t(__offset_t offset) : __offset(offset) { }

        __PushAddressCommand_ToString(local_addr, __offset)

        __BeginExecute(ctx)

            ctx.stack.push(__LocalAddress(__This->__offset));

        __EndExecute()

    private:
        __offset_t __offset;
    };

    struct __push_address_command_template_t
    {
        template<command_value_t _cv, xil_storage_type_t _st, typename ... _args_t>
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

    template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2>
    class __push_command_t<_cv, xil_storage_type_t::argument, _xt1, _xt2>
        : public __command_base_t<_cv>
    {
        typedef __push_command_t    __self_t;
        typedef __vnum_t<_xt1>      __value1_t;
        typedef __vnum_t<_xt2>      __value2_t;

    public:
        __push_command_t(__offset_t offset)
            : __real_offset(offset + __stack_stub_size) { }

        __PushCommand_ToString(argument, _xt1, _xt2, __real_offset - __stack_stub_size)

        __BeginExecute(ctx)

            ctx.stack.push(static_cast<__value2_t>(
                __Argument(__value1_t, __This->__real_offset)
            ));

        __EndExecute()

    private:
        __offset_t __real_offset;
    };

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(push, argument, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    //-------- ---------- ---------- ---------- ----------
    // Push argument address commands.

    __DefineStAddrCmdValue(push, argument_addr)

    template<>
    class __push_address_command_t<xil_storage_type_t::argument_addr>
        : public __command_base_t<__StAddrCmd(push, argument_addr)>
    {
        typedef __push_address_command_t    __self_t;

    public:
        __push_address_command_t(__offset_t offset) : __offset(offset) { }

        __PushAddressCommand_ToString(argument_addr, __offset)

        __BeginExecute(ctx)

            ctx.stack.push(__ArgumentAddress(__This->__offset + __stack_stub_size));

        __EndExecute()

    private:
        __offset_t __offset;
    };

    //-------- ---------- ---------- ---------- ----------
    // Push argument content.

    template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2>
    class __push_command_t<_cv, xil_storage_type_t::argument_content, _xt1, _xt2>
        : public __command_base_t<_cv>
    {
        typedef __push_command_t    __self_t;
        typedef __vnum_t<_xt1>      __value1_t;
        typedef __vnum_t<_xt2>      __value2_t;

    public:
        __push_command_t(__offset_t offset)
            : __real_offset(offset + __stack_stub_size) { }

        __PushCommand_ToString(argument, _xt1, _xt2, __real_offset - __stack_stub_size)

        __BeginExecute(ctx)

            ctx.stack.push(static_cast<__value2_t>(
                *(__value1_t *)__Argument(void *, __This->__real_offset)
            ));

        __EndExecute()

    private:
        __offset_t __real_offset;
    };

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(push, argument_content, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    //-------- ---------- ---------- ---------- ----------
    // Push field commands.

    static bool __is_current_template(__context_t & ctx, rt_type_t * type)
    {
        if (ctx.type->get_kind() != rt_type_kind_t::generic)
            return false;

        rt_general_type_t * template_ = ((rt_generic_type_t *)ctx.type)->template_;
        return is_host_type_template(type, template_);
    }

    // Returns field type.
    static rt_type_t * __field_type(__context_t & ctx, ref_t field_ref)
    {
        mt_member_extra_t extra = (mt_member_extra_t)field_ref.extra;

        switch (extra)
        {
            case mt_member_extra_t::import:
            case mt_member_extra_t::internal: {

                rt_type_t * rt_type;
                rt_field_base_t * rt_field = ctx.get_field(field_ref, &rt_type);

                _A( rt_field != nullptr );
                _A( rt_type != nullptr );

                rt_type_t * rt_field_type = rt_field->get_field_type(ctx,
                                        dynamic_cast<rt_generic_type_t *>(ctx.type));

                _A( rt_field_type != nullptr );
                if (!is_general(rt_field_type))
                    return rt_field_type;

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

                mt_position_field_t * mt = ctx.current->mt_of_position_field(field_ref);
                _A(mt != nullptr);

                int position = mt->position;
                _A(position < generic_type->atype_count());

                return generic_type->atypes[position];
            }

            case mt_member_extra_t::generic: {

                rt_type_t * type;
                rt_field_base_t * field = ctx.get_field(field_ref, &type);
                _A(type->get_kind() == rt_type_kind_t::generic);

                generic_param_manager_t gp_mgr(ctx.env, (rt_generic_type_t *)type);
                assembly_analyzer_t analyzer(ctx, &gp_mgr);

                rt_type_t * field_type = field->get_field_type(ctx, (rt_generic_type_t *)type);
                return field_type;
            }

            default:
                X_UNEXPECTED_F("unexpected member type '%1%'", extra);
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

    template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2>
    class __push_command_t<_cv, xil_storage_type_t::field, _xt1, _xt2>
        : public __command_base_t<_cv>
    {
        typedef __push_command_t        __self_t;
        typedef __command_base_t<_cv>   __super_t;
        typedef __const_vnum_t<_xt1>    __value1_t;
        typedef __const_vnum_t<_xt2>    __value2_t;

    public:
        __push_command_t(__offset_t offset) : __offset(offset) { }

        __BeginExecute(ctx)

            ctx.stack.push(static_cast<__value2_t>(
                __Field(__value1_t, ctx.stack.pop<rt_ref_t>(), __This->__offset)
            ));

        __EndExecute()

        __PushCommand_ToString(field, _xt1, _xt2, __offset)

    private:
        __offset_t __offset;
    };

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(push, field, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    //-------- ---------- ---------- ---------- ----------
    // Push field address commands.

    __DefineStAddrCmdValue(push, field_addr)

    template<>
    class __push_address_command_t<xil_storage_type_t::field_addr>
        : public __command_base_t<__StAddrCmd(push, field_addr)>
    {
        typedef __push_address_command_t    __self_t;

    public:
        __push_address_command_t(__offset_t offset) : __offset(offset) { }

        __PushAddressCommand_ToString(field_addr, __offset)

        __BeginExecute(ctx)

            ctx.stack.push(
                __FieldAddress(ctx.stack.pop<rt_ref_t>(), __This->__offset)
            );

        __EndExecute()

    private:
        __offset_t __offset;
    };

    //-------- ---------- ---------- ---------- ----------

    #define __PushDuplicateCmd  __Cmd(push, duplicate)
    __DefineCmdValue_(__PushDuplicateCmd)

    class __duplicate_command_t : public __command_base_t<__PushDuplicateCmd>
    {
        typedef __duplicate_command_t   __self_t;

    public:
        __BeginExecute(ctx)

            ctx.stack.push<rt_stack_unit_t>(
                ctx.stack.pick<rt_stack_unit_t>()
            );

        __EndExecute()

        #if EXEC_TRACE

        __ToString(_T("duplicate"));

        #endif  // EXEC_TRACE
    };

    //-------- ---------- ---------- ---------- ----------
    // Push convert commands..

    template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2>
    class __push_command_t<_cv, xil_storage_type_t::convert, _xt1, _xt2>
        : public __command_base_t<_cv>
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

        __BeginExecute(ctx)

            if constexpr (!std::is_same<__value1_t, __value2_t>::value)
            {
                ctx.stack.push(static_cast<__value2_t>(
                    ctx.stack.pop<__value1_t>()
                ));
            }

        __EndExecute()
    };

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(push, convert, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    struct __push_convert_command_template_t
    {
        template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2, typename ... _args_t>
        static auto new_command(memory_t * memory, _args_t && ... args)
        {
            typedef __push_command_t<_cv, xil_storage_type_t::convert, _xt1, _xt2> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<_args_t>(args) ...);
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
    // Push array element.

    #define __RtTypeOf(rt_ref)  (mm::get_object_type(rt_ref))

    template<command_value_t _cv, xil_type_t _xil_type1, xil_type_t _xil_type2,
            dimension_t _dimension>
    class __push_array_element_command_t : public __command_base_t<_cv>
    {
        typedef __push_array_element_command_t  __self_t;
        typedef __vnum_t<_xil_type1>            __value1_t;
        typedef __vnum_t<_xil_type2>            __value2_t;

    public:

        #if EXEC_TRACE

        __ToString(_T("push array element"));

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            rt_ref_t array_ref = ctx.stack.pop<rt_ref_t>(); 
            array_length_t index = ctx.stack.pop<array_length_t>();

            if constexpr (_dimension >= 2)
            {
                array_length_t * lengths = mm::get_array_lengths(array_ref);
                index += __mul_array_index<_dimension - 1>(ctx.stack, lengths);
            }

            if (index >= mm::get_array_length(array_ref))
                __RaiseOutOfRangeException(ctx);

            ctx.stack.push<__value2_t>(static_cast<__value2_t>(
                mm::get_array_element<__value1_t>(array_ref, index)
            ));

        __EndExecute()
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2>
    class __push_array_element_command_t<_cv, _xt1, _xt2, 0>
        : public __command_base_t<_cv>
    {
        typedef __vnum_t<_xt1>                  __value1_t;
        typedef __vnum_t<_xt2>                  __value2_t;
        typedef __push_array_element_command_t  __self_t;

    public:

        __push_array_element_command_t(dimension_t dimension) : __dimension(dimension) { }

        #if EXEC_TRACE

        __ToString(_T("push array element"));

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            rt_ref_t array_ref = ctx.stack.pop<rt_ref_t>(); 
            array_length_t index = ctx.stack.pop<array_length_t>();
            array_length_t * lengths = mm::get_array_lengths(array_ref);

            index += __array_index(ctx.stack, __This->__dimension - 1, lengths);

            if (index >= mm::get_array_length(array_ref))
                __RaiseOutOfRangeException(ctx);

            ctx.stack.push<__value2_t>(static_cast<__value2_t>(
                mm::get_array_element<__value1_t>(array_ref, index)
            ));

        __EndExecute()

    private:
        dimension_t __dimension;
    };

    #define __CaseDimensionXilTypePair( _xt1, _xt2, _dimension )                        \
        __DefineStArrayCmdValue(push,_xt1, _xt2, _dimension)

    __EachDimensionXilTypePairs()

    #undef __CaseDimensionXilTypePair

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    struct __push_array_element_command_template_t
    {
        template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2,
            dimension_t _dimension, typename ... _args_t
        >
        static command_t * new_command(memory_t * memory, dimension_t dimension,
            _args_t && ... args)
        {
            if constexpr (_dimension == 0)
            {
                return __new_command<__push_array_element_command_t<_cv, _xt1, _xt2, 0>>(
                    memory, dimension, std::forward<_args_t>(args) ...
                );
            }
            else
            {
                return __new_command<__push_array_element_command_t<_cv, _xt1, _xt2, _dimension>>(
                    memory, std::forward<_args_t>(args) ...
                );
            }
        }
    };

    //-------- ---------- ---------- ---------- ----------
    // Push array element address.

    template<command_value_t _cv, dimension_t _dimension>
    class __push_array_element_address_command_t : public __command_base_t<_cv>
    {
        typedef __push_array_element_address_command_t  __self_t;

    public:

        __push_array_element_address_command_t(msize_t element_size)
            : __element_size(element_size)
        { }

        #if EXEC_TRACE

            __ToString(_T("push array element address"));

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            rt_ref_t array_ref = ctx.stack.pop<rt_ref_t>(); 
            array_length_t index = ctx.stack.pop<array_length_t>();

            if constexpr (_dimension >= 2)
            {
                array_length_t * lengths = mm::get_array_lengths(array_ref);
                index += __mul_array_index<_dimension - 1>(ctx.stack, lengths);
            }

            if (index >= mm::get_array_length(array_ref))
                __RaiseOutOfRangeException(ctx);

            ctx.stack.push(array_ref + index * __element_size);

        __EndExecute();

    private:
        msize_t     __element_size;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    template<command_value_t _cv>
    class __push_array_element_address_command_t<_cv, 0> : public __command_base_t<_cv>
    {
        typedef __push_array_element_address_command_t  __self_t;

    public:

        __push_array_element_address_command_t(dimension_t dimension, msize_t element_size)
            : __dimension(dimension), __element_size(element_size) { }

        #if EXEC_TRACE

            __ToString(_T("push array element address"));

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            rt_ref_t array_ref = ctx.stack.pop<rt_ref_t>(); 
            array_length_t index = ctx.stack.pop<array_length_t>();
            array_length_t * lengths = mm::get_array_lengths(array_ref);

            index += __array_index(ctx.stack, __This->__dimension - 1, lengths);

            if (index >= mm::get_array_length(array_ref))
                __RaiseOutOfRangeException(ctx);

            ctx.stack.push(array_ref + index * __element_size);

        __EndExecute()

    private:
        dimension_t __dimension;
        msize_t     __element_size;
    };

    __DefineStArrayElementAddrCmdValue(push, 0)
    __DefineStArrayElementAddrCmdValue(push, 1)
    __DefineStArrayElementAddrCmdValue(push, 2)
    __DefineStArrayElementAddrCmdValue(push, 3)
    __DefineStArrayElementAddrCmdValue(push, 4)
    __DefineStArrayElementAddrCmdValue(push, 5)
    __DefineStArrayElementAddrCmdValue(push, 6)
    __DefineStArrayElementAddrCmdValue(push, 7)
    __DefineStArrayElementAddrCmdValue(push, 8)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    struct __push_array_element_address_command_template_t
    {
        template<command_value_t _cv, dimension_t _dimension, typename ... _args_t>
        static command_t * new_command(memory_t * memory, dimension_t dimension,
            msize_t element_size, _args_t && ... args)
        {
            if constexpr (_dimension == 0)
            {
                return __new_command<__push_array_element_address_command_t<_cv, 0>>(
                    memory, dimension, element_size, std::forward<_args_t>(args) ...
                );
            }
            else
            {
                return __new_command<__push_array_element_address_command_t<_cv, _dimension>>(
                    memory, element_size, std::forward<_args_t>(args) ...
                );
            }
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
    // Push params address.

    #define __PushParamsAddrCmd  __Cmd(push, params_addr)
    __DefineCmdValue_(__PushParamsAddrCmd)

    class __push_params_address_command_t : public __command_base_t<__PushParamsAddrCmd>
    {
        typedef __push_params_address_command_t __self_t;

    public:
        __push_params_address_command_t(__offset_t offset)
            : __offset(offset)
        { }

        #if EXEC_TRACE

        __ToString(_T("push params argument address"));

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            ctx.stack.push<rt_stack_unit_t *>(
                (rt_stack_unit_t *)ctx.stack.lp() - __This->__offset - 1
            );

        __EndExecute()

    private:
        __offset_t __offset;
    };

    struct __push_params_address_template_t
    {
        template<command_value_t _cv, typename ... _args_t>
        static auto new_command(memory_t * memory, _args_t && ... args)
        {
            typedef __push_params_address_command_t this_command_t;
            return __new_command<this_command_t>(memory, std::forward<_args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------
    // Push box.

    #define __CustomStructSize  65535
    #define __PushBoxCmd(_size, _box_type)                                              \
            __Cmd(push, box##_##_size##_##_box_type)

    #define __DefinePushBoxCmd(_size)                                                   \
        __DefineCmdValue_(__PushBoxCmd(_size, pop))                                     \
        __DefineCmdValue_(__PushBoxCmd(_size, pick))

    __DefinePushBoxCmd(0)
    __DefinePushBoxCmd(1)
    __DefinePushBoxCmd(2)
    __DefinePushBoxCmd(4)
    __DefinePushBoxCmd(8)
    __DefinePushBoxCmd(__CustomStructSize)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    template<xil_box_type_t _box_type, typename _t>
    __AlwaysInline _t __fetch_box_addr(command_execute_context_t & ctx)
    {
        if constexpr (_box_type == xil_box_type_t::pop)
        {
            return ctx.stack.pop<_t>();
        }
        else if constexpr (_box_type == xil_box_type_t::pick)
        {
            return ctx.stack.pick<_t>();
        }
        else
        {
            X_UNEXPECTED_F("unexpected box type '%1%'", _box_type);
        }
    }

    template<command_value_t _cv, msize_t _size, xil_box_type_t _box_type>
    class __push_box_command_t : public __command_base_t<_cv>
    {
        typedef __push_box_command_t    __self_t;
        typedef __command_base_t<_cv>   __super_t;
        typedef uint_type_t<_size>      __value_t;

    public:
        __push_box_command_t(rt_type_t * type) : __type(type) { }

        #if EXEC_TRACE

        __ToString(_FT("box [%1%]", _box_type));

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            __value_t value = __fetch_box_addr<_box_type, __value_t>(ctx);

            __pre_new(ctx, __type);
            rt_ref_t obj = ctx.heap->new_obj(__type);

            *(__value_t *)(void *)obj = value;
            ctx.stack.push<rt_ref_t>(obj);

        __EndExecute()

    private:
        rt_type_t * __type;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    template<command_value_t _cv, xil_box_type_t _box_type>
    class __push_box_command_t<_cv, __CustomStructSize, _box_type> : public __command_base_t<_cv>
    {
        typedef __push_box_command_t    __self_t;
        typedef __command_base_t<_cv>   __super_t;

    public:
        __push_box_command_t(rt_type_t * type, msize_t size)
            : __type(type), __size(size) { }

        #if EXEC_TRACE

        __ToString(_FT("box [%1%]", _box_type));

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            void * addr = __fetch_box_addr<_box_type, void *>(ctx);

            __pre_new(ctx, __type);
            rt_ref_t obj = ctx.heap->new_obj(__type);

            al::quick_copy((void *)obj, addr, __size);
            ctx.stack.push<rt_ref_t>(obj);

        __EndExecute()

    private:
        rt_type_t * __type;
        msize_t     __size;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    struct __push_box_command_template_t
    {
        template<command_value_t _cv, msize_t _size, xil_box_type_t _box_type,
                                                        typename ... _args_t>
        static auto new_command(memory_t * memory, rt_type_t * type, msize_t real_size,
                                                        _args_t && ... args)
        {
            typedef __push_box_command_t<_cv, _size, _box_type> this_command_t;
            if constexpr (_size == __CustomStructSize)
            {
                return __new_command<this_command_t>(memory, type, real_size,
                                                        std::forward<_args_t>(args) ...);
            }
            else
            {
                return __new_command<this_command_t>(memory, type,
                                                        std::forward<_args_t>(args) ...);
            }
        }
    };

    //-------- ---------- ---------- ---------- ----------
    // Push unbox.

    #define __PushUnboxCmd(_size, _box_type)                                            \
        __Cmd(push, unbox##_##_size##_##_box_type)

    #define __DefinePushUnboxCmd(_size)                                                 \
        __DefineCmdValue_(__PushUnboxCmd(_size, pop))                                   \
        __DefineCmdValue_(__PushUnboxCmd(_size, pick))

    __DefinePushUnboxCmd(0)
    __DefinePushUnboxCmd(1)
    __DefinePushUnboxCmd(2)
    __DefinePushUnboxCmd(3)
    __DefinePushUnboxCmd(4)
    __DefinePushUnboxCmd(8)
    __DefinePushUnboxCmd(__CustomStructSize)

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    template<command_value_t _cv, msize_t _size, xil_box_type_t _box_type>
    class __push_unbox_command_t : public __command_base_t<_cv>
    {
        typedef __push_unbox_command_t      __self_t;
        typedef __command_base_t<_cv>       __super_t;
        typedef uint_type_t<_size>          __value_t;

    public:
        __push_unbox_command_t(rt_type_t * type) : __type(type) { }

        #if EXEC_TRACE

        __ToString(_FT("unbox [%1%]", _box_type));

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            rt_ref_t obj = __fetch_box_addr<_box_type, rt_ref_t>(ctx);
            __CheckNullReference(ctx, obj); 

            rt_type_t * type = __RtTypeOf(obj);
            if (type != __type)
                __RaiseInvalidCastException(ctx);

            ctx.stack.push<__value_t>(*(__value_t *)(void *)obj);

        __EndExecute()

    private:
        rt_type_t * __type;
    };
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    template<command_value_t _cv, xil_box_type_t _box_type>
    class __push_unbox_command_t<_cv, __CustomStructSize, _box_type> : public __command_base_t<_cv>
    {
        typedef __push_unbox_command_t          __self_t;
        typedef __command_base_t<_cv>           __super_t;

    public:
        __push_unbox_command_t(rt_type_t * type, msize_t size)
            : __type(type), __size(size) { }

        #if EXEC_TRACE

        __ToString(_FT("unbox [%1%]", _box_type));

        #endif      // EXEC_TRACE

        __BeginExecute(ctx)

            rt_ref_t obj = __fetch_box_addr<_box_type, rt_ref_t>(ctx);
            __CheckNullReference(ctx, obj); 

            rt_type_t * type = __RtTypeOf(obj);
            if (type != __type)
                __RaiseInvalidCastException(ctx);

            void * val = ctx.stack.pop<void *>();
            al::quick_copy(val, (void *)obj, __size);

        __EndExecute()

    private:
        rt_type_t * __type;
        msize_t     __size;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    struct __push_unbox_command_template_t
    {
        template<command_value_t _cv, size_t _size, xil_box_type_t _box_type,
                                                    typename ... _args_t>
        static auto new_command(memory_t * memory, rt_type_t * type, msize_t real_size,
                                                    _args_t && ... args)
        {
            typedef __push_unbox_command_t<_cv, _size, _box_type> this_command_t;
            if constexpr (_size == __CustomStructSize)
            {
                return __new_command<this_command_t>(memory, type, real_size,
                                            std::forward<_args_t>(args) ...);
            }
            else
            {
                return __new_command<this_command_t>(memory, type,
                                            std::forward<_args_t>(args) ...);
            }
        }
    };

    //-------- ---------- ---------- ---------- ----------

    #define __PushObjectCmd  __Cmd(push, object)
    __DefineCmdValue_(__PushObjectCmd)

    class __push_object_command_t : public __command_base_t<__PushObjectCmd>
    {
        typedef __push_object_command_t __self_t;

    public:
        
        #if EXEC_TRACE
        __push_object_command_t(rt_object_t * object, xil_storage_object_type_t object_type,
                                                      rt_sid_t name)
            : __object(object), __object_type(object_type), __name(name) { }
        #else
        __push_object_command_t(rt_object_t * object) : __object(object) { }
        #endif  // EXEC_TRACE

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("push object [%1%]%2%"), __object_type, __name);
        __EndToString()

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            ctx.stack.push(__object);

        __EndExecute()

    private:
        rt_object_t * __object;

        #if EXEC_TRACE
        xil_storage_object_type_t __object_type;
        rt_sid_t                  __name;
        #endif  // EXEC_TRACE
    };

    struct __push_object_template_t
    {
        template<command_value_t _cv, typename ... _args_t>
        static auto new_command(memory_t * memory, rt_object_t * object
        #if EXEC_TRACE
            , xil_storage_object_type_t object_type, rt_sid_t name
        #endif
        )
        {
            return __new_command<__push_object_command_t>(memory, object
            #if EXEC_TRACE
                , object_type, name
            #endif
            );
        }   
    };

    // Returns method info.
    static rt_object_t * __get_method_info(__context_t & ctx, ref_t method_ref
        #if EXEC_TRACE
        , rt_sid_t * out_name
        #endif
    )
    {
        rt_method_base_t * method = ctx.get_method(method_ref);

        #if EXEC_TRACE
        *out_name = method->get_name();
        #endif

        return (rt_object_t *)method;
    }

    // Returns type info.
    static rt_object_t * __get_type_info(__context_t & ctx, ref_t type_ref
        #if EXEC_TRACE
        , rt_sid_t * out_name
        #endif
    )
    {
        rt_type_t * type = ctx.get_type(type_ref);

        #if EXEC_TRACE
        *out_name = type->get_name(ctx.env);
        #endif

        return type;
    }

    // Returns object of specified ref_t.
    static rt_object_t * __get_object(__context_t & ctx,
                                      xil_storage_object_type_t object_type, ref_t ref
        #if EXEC_TRACE
        , rt_sid_t * out_name
        #endif
    )
    {
        rt_object_t * object;

        switch (object_type)
        {
            case xil_storage_object_type_t::method_info: {
                object = __get_method_info(ctx, ref
                #if EXEC_TRACE
                    , out_name
                #endif
                );
            }   break;

            case xil_storage_object_type_t::type_info: {
                object = __get_type_info(ctx, ref
                #if EXEC_TRACE
                    , out_name
                #endif
                );
            }   break;

            default:
                X_UNEXPECTED_F("unexpected object type %1%", object_type);
        }

        return object;
    }

    //-------- ---------- ---------- ---------- ----------

    /*
        need_content_type: used when it's ref/out argument.
            when pop a value from stack to an argument variable, should use content type.
            when push address into stack, should use ptr type.
    */
    template<typename _xil_t>
    void __analyze_xil_types(__context_t & ctx, const _xil_t & xil,
                            xil_type_t * out_dtype1, xil_type_t * out_dtype2,
                            bool need_content_type)
    {
        xil_storage_type_t stype = xil.stype();

        // Gets xil_type of data source.
        switch (stype)
        {
            case xil_storage_type_t::local:
            case xil_storage_type_t::local_addr:
                *out_dtype1 = __xil_type_of_local(ctx, xil.get_identity());
                *out_dtype2 = __fetch_dtype(ctx, xil);
                break;

            case xil_storage_type_t::argument:
            case xil_storage_type_t::argument_addr: {
                uint16_t identity = xil.get_identity();
                if (identity == XIL_CALLING_BOTTOM_IDENTITY)
                {
                    *out_dtype1 = xil_type_t::ptr;
                    *out_dtype2 = xil_type_t::ptr;
                }
                else
                {
                    if (need_content_type)
                        *out_dtype1 = __xil_type_of(ctx, ctx.params_layout.type_at(identity));
                    else
                        *out_dtype1 = __xil_type_of_argument(ctx, xil.get_identity());

                    *out_dtype2 = __fetch_dtype(ctx, xil);
                }
            }   break;

            case xil_storage_type_t::field:
            case xil_storage_type_t::field_addr:
                *out_dtype1 = __xil_type_of(ctx, __field_type(ctx, xil.get_ref()));
                *out_dtype2 = __fetch_dtype(ctx, xil);
                break;

            case xil_storage_type_t::array_element:
            case xil_storage_type_t::array_element_addr:
                *out_dtype1 = __xil_type_of(ctx, __array_element_type(ctx, xil.get_ref()));
                *out_dtype2 = __fetch_dtype(ctx, xil);
                if (*out_dtype2 == xil_type_t::empty)
                    *out_dtype2 = *out_dtype1;
                break;

            default:
                *out_dtype2 = __fetch_dtype(ctx, xil);
                *out_dtype1 = *out_dtype2;
                break;
        }
    }

    msize_t __offset_of_argument(command_creating_context_t & ctx, uint16_t identity)
    {
        if (identity == XIL_CALLING_BOTTOM_IDENTITY)
            return ctx.params_layout.unit_size() + 1;

        return ctx.params_layout.offset_of(identity);
    }

    msize_t __offset_of_this_argument(command_creating_context_t & ctx)
    {
        return __offset_of_argument(ctx, 0);
    }

    struct __new_push_command_args_t
    {
        xil_storage_type_t  stype;
        xil_type_t xt1, xt2;

        union
        {
            // constant
            struct
            {
                xil_type_t  dtype;

                union
                {
                    void *      data;   // Constant data.
                    rt_type_t * type;   // System.Type
                };

                template<typename _t> _t get_data() _NE { return *(_t *)data; }

            } constant;

            // params
            struct
            {
                __offset_t  offset;
            } params;

            // object
            struct
            {
                xil_storage_object_type_t   type;
                rt_object_t *               object;

                #if EXEC_TRACE
                rt_sid_t name;
                #endif

            } object;

            // local_addr
            struct
            {
                msize_t     offset;
            } local_addr;

            // argument_addr
            struct
            {
                msize_t     offset;
            } argument_addr;

            // field_addr
            struct
            {
                msize_t     offset;
            } field_addr;

            // local
            struct
            {
                msize_t     offset;
            } local;

            // argument
            struct
            {
                msize_t     offset;
            } argument;

            // argument content
            struct
            {
                msize_t     offset;
            } argument_content;

            // field
            struct
            {
                msize_t     offset;
            } field;

            // array element.
            struct
            {
                dimension_t dimension;
            } array_element;

            // array element addr.
            struct
            {
                msize_t     element_size;
                dimension_t dimension;
            } array_element_addr;

            // box.
            struct
            {
                rt_type_t *     type;
                xil_box_type_t  box_type;
            } box;

            // unbox.
            struct
            {
                rt_type_t *     type;
                xil_box_type_t  box_type;
            } unbox;
        };
    };

    static command_t * __new_push_command(__context_t & ctx, __new_push_command_args_t & args)
    {
        typedef xil_storage_type_t stype_t;

        #define __CommandManagerT(_v_t)                                                 \
            __command_manager_t<                                                        \
                __push_command_template_t, xil_storage_type_t, xil_type_t, xil_type_t   \
            >::with_args_t<_v_t>

        #define __DefineConstantCommandManagerT(_name)                                  \
            static __CommandManagerT(_name##_t)  _name##_push_command_manager;

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
            __push_array_element_command_template_t, xil_type_t, xil_type_t, dimension_t
        >::with_args_t<dimension_t> __push_array_element_command_manager;

        static __command_manager_t<
            __push_convert_command_template_t, xil_type_t, xil_type_t
        >::with_args_t<> __convert_command_manager;

        xil_storage_type_t stype = args.stype;
        xil_type_t xt1 = args.xt1, xt2 = args.xt2;

        switch (stype)
        {
            case xil_storage_type_t::duplicate:     // Duplicate command.
                __ReturnInstance(__duplicate_command_t);

            case xil_storage_type_t::params: {      // Push params argument command.

                static __command_manager_t<
                    __push_params_address_template_t
                >::with_args_t<__offset_t> __params_address_command_manager;

                return __params_address_command_manager.template get_command<__PushParamsAddrCmd>(
                    args.params.offset
                );
            }

            case xil_storage_type_t::object: {       // Push object command.

                static __command_manager_t<
                    __push_object_template_t
                >::with_args_t<rt_object_t *> __object_command_manager;

                return __object_command_manager.template get_command<__PushObjectCmd>(
                    args.object.object
                #if EXEC_TRACE
                    , args.object.type, args.object.name
                #endif
                );
            }

            case xil_storage_type_t::local_addr: {  // Push local address command.
                return __push_address_command_manager.template get_command<
                    __StAddrCmd(push, local_addr), stype_t::local_addr>(
                    args.local_addr.offset
                );
            }

            case xil_storage_type_t::argument_addr: // Push agrument address command.
                return __push_address_command_manager.template get_command<
                    __StAddrCmd(push, argument_addr), stype_t::argument_addr>(
                    args.argument_addr.offset
                );

            case xil_storage_type_t::field_addr:    // Push field address command.
                return __push_address_command_manager.template get_command<
                    __StAddrCmd(push, field_addr), stype_t::field_addr>(
                    args.field_addr.offset
                );

            case xil_storage_type_t::array_element_addr: { // Push array element address command.

                static __command_manager_t<
                    __push_array_element_address_command_template_t, dimension_t
                >::with_args_t<msize_t, dimension_t> __push_array_element_address_command_manager;

                dimension_t dimension = args.array_element_addr.dimension;
                msize_t element_size  = args.array_element_addr.element_size;

                switch (dimension <= 8? dimension : 0)
                {
                    #define __Case(_dimension)                                          \
                        case _dimension: {                                              \
                            constexpr command_value_t cmd = __StArrayElementAddrCmd(    \
                                push, _dimension                                        \
                            );                                                          \
                            return __push_array_element_address_command_manager.template    \
                                get_command<cmd, _dimension>(dimension, element_size);  \
                        }   break;

                    __Case(0)
                    __Case(1)
                    __Case(2)
                    __Case(3)
                    __Case(4)
                    __Case(5)
                    __Case(6)
                    __Case(7)
                    __Case(8)

                    #undef __Case
                }

            }   break;

            case xil_storage_type_t::box: {         // Push box command.

                static __command_manager_t<
                    __push_box_command_template_t, msize_t, xil_box_type_t
                >::with_args_t<rt_type_t *> __box_command_manager;

                #define __BoxV(_size, _box_type)    (((int)_size << 8) | (int)_box_type)

                rt_type_t * type = args.box.type;
                _A(type != nullptr);

                msize_t real_size = type->get_size(ctx.env);
                _A(real_size > 0);

                msize_t size = is_custom_struct(ctx.env, type)? __CustomStructSize : real_size;

                switch (__BoxV(size, args.box.box_type))
                {
                    #define __CaseBox_(_size, _box_type)                                \
                        case __BoxV(_size, xil_box_type_t::_box_type):                  \
                            return __box_command_manager.template get_command<          \
                                __PushBoxCmd(_size, _box_type), _size,                  \
                                xil_box_type_t::_box_type> (args.box.type, real_size);

                    #define __CaseBox(_size)                                            \
                        __CaseBox_(_size, pop)                                          \
                        __CaseBox_(_size, pick)

                    __CaseBox(0)
                    __CaseBox(1)
                    __CaseBox(2)
                    __CaseBox(4)
                    __CaseBox(8)
                    __CaseBox(__CustomStructSize)

                    #undef __CaseBox
                    #undef __CaseBox_

                    default:
                        X_UNEXPECTED_F("unexpected box command %1% [%2%]", size,
                                                                       args.box.box_type);
                }

                #undef __BoxV

            }   break;

            case xil_storage_type_t::unbox: {         // Push unbox command.

                static __command_manager_t<
                    __push_unbox_command_template_t, msize_t, xil_box_type_t
                >::with_args_t<rt_type_t *> __unbox_command_manager;

                #define __UnboxV(_size, _box_type)    (((int)_size << 8) | (int)_box_type)

                rt_type_t * type = args.unbox.type;
                _A(type != nullptr);

                msize_t real_size = type->get_size(ctx.env);
                _A(real_size > 0);

                msize_t size = is_custom_struct(ctx.env, type)? __CustomStructSize : real_size;

                switch (__UnboxV(size, args.unbox.box_type))
                {
                    #define __CaseUnbox_(_size, _box_type)                              \
                        case __UnboxV(_size, xil_box_type_t::_box_type):                \
                            return __unbox_command_manager.template get_command<        \
                                __PushUnboxCmd(_size, _box_type), _size,                \
                                xil_box_type_t::_box_type> (args.unbox.type, real_size);

                    #define __CaseUnbox(_size)                                          \
                        __CaseUnbox_(_size, pop)                                        \
                        __CaseUnbox_(_size, pick)

                    __CaseUnbox(0)
                    __CaseUnbox(1)
                    __CaseUnbox(2)
                    __CaseUnbox(4)
                    __CaseUnbox(8)
                    __CaseUnbox(__CustomStructSize)

                    #undef __CaseUnbox
                    #undef __CaseUnbox_

                    default:
                        X_UNEXPECTED_F("unexpected unbox command %1% [%2%]", size,
                                                                       args.unbox.box_type);
                }

                #undef __BoxV

            }   break;


            default:
                break;
        }

        #define __V(s, d1, d2) (((uint32_t)(s) << 16)                                   \
            | ((uint32_t)(d1) << 12) | ((uint32_t)(d2)) << 8)

        #define __ConstantValue(_xt)                                                    \
            (args.constant.dtype == xil_type_t::empty? default_value_of<__const_vnum_t<_xt>>()   \
                : args.constant.get_data<__const_vnum_t<_xt>>())

        #define __ConstantStringValue()                                                 \
            (args.constant.dtype == xil_type_t::empty? default_value_of<const char_t *>()   \
                : ctx.to_cstr(args.constant.get_data<res_t>()))

        switch (__V(stype, xt1, xt2))
        {
            // Constant values.
            #define __CaseConstant(_name, _xt1, _xt2)                                   \
                case __V(xil_storage_type_t::constant, xil_type_t::_xt1, xil_type_t::_xt2): \
                    return _name##_push_command_manager.template get_command<           \
                        __StCmd(push, constant, _xt1, _xt2),                            \
                        xil_storage_type_t::constant, xil_type_t::_xt1, xil_type_t::_xt2 \
                    >(__ConstantValue(xil_type_t::_xt1));

            case __V(xil_storage_type_t::constant, xil_type_t::string, xil_type_t::string):
                return __string_push_command_manager.template get_command<
                    __StCmd(push, constant, string, string),
                    xil_storage_type_t::constant, xil_type_t::string, xil_type_t::string
                >(__ConstantStringValue());

            // System.Type.
            case __V(xil_storage_type_t::constant, xil_type_t::object, xil_type_t::object):
                return __object_push_command_manager.template get_command<
                    __StCmd(push, constant, object, object),
                    xil_storage_type_t::constant, xil_type_t::object, xil_type_t::object
                >(args.constant.type);

            // Empty.
            case __V(xil_storage_type_t::constant, xil_type_t::__unknown__, xil_type_t::__unknown__):
                __ReturnInstance(__empty_command_t);

            #define __CaseConstants(_xt2)                                               \
                __CaseConstant(int8,    int8,    _xt2)                                  \
                __CaseConstant(uint8,   uint8,   _xt2)                                  \
                __CaseConstant(int16,   int16,   _xt2)                                  \
                __CaseConstant(uint16,  uint16,  _xt2)                                  \
                __CaseConstant(int32,   int32,   _xt2)                                  \
                __CaseConstant(uint32,  uint32,  _xt2)                                  \
                __CaseConstant(int64,   int64,   _xt2)                                  \
                __CaseConstant(uint64,  uint64,  _xt2)                                  \
                __CaseConstant(float,   float_,  _xt2)                                  \
                __CaseConstant(double,  double_, _xt2)                                  \
                __CaseConstant(bool,    bool_,   _xt2)                                  \
                __CaseConstant(char,    char_,   _xt2)


            __CaseConstants(int8)
            __CaseConstants(uint8)
            __CaseConstants(int16)
            __CaseConstants(uint16)
            __CaseConstants(int32)
            __CaseConstants(uint32)
            __CaseConstants(int64)
            __CaseConstants(uint64)
            __CaseConstants(float_)
            __CaseConstants(double_)
            __CaseConstants(bool_)
            __CaseConstants(char_)

            #undef __CaseConstant
            #undef __CaseConstants

            // - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Push Local

            #define __CaseLocal(_xt1, _xt2)                                             \
                case __V(xil_storage_type_t::local, xil_type_t::_xt1, xil_type_t::_xt2): \
                    return __command_manager.template get_command<                      \
                        __StCmd(push, local, _xt1, _xt2),                               \
                        xil_storage_type_t::local, xil_type_t::_xt1, xil_type_t::_xt2   \
                    >(args.local.offset);

            #define __CaseLocals(_xt2)                                                  \
                __CaseLocal(int8,       _xt2)                                           \
                __CaseLocal(uint8,      _xt2)                                           \
                __CaseLocal(int16,      _xt2)                                           \
                __CaseLocal(uint16,     _xt2)                                           \
                __CaseLocal(int32,      _xt2)                                           \
                __CaseLocal(uint32,     _xt2)                                           \
                __CaseLocal(int64,      _xt2)                                           \
                __CaseLocal(uint64,     _xt2)                                           \
                __CaseLocal(float_,     _xt2)                                           \
                __CaseLocal(double_,    _xt2)                                           \
                __CaseLocal(char_,      _xt2)                                           \
                __CaseLocal(bool_,      _xt2)


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

            #define __CaseArgument(_xt1, _xt2)                                          \
                case __V(xil_storage_type_t::argument, xil_type_t::_xt1, xil_type_t::_xt2): \
                    return __command_manager.template get_command<                      \
                        __StCmd(push, argument, _xt1, _xt2),                            \
                        xil_storage_type_t::argument, xil_type_t::_xt1, xil_type_t::_xt2    \
                    >(args.argument.offset);


            #define __CaseArguments(_xt2)                                               \
                __CaseArgument(int8,    _xt2)                                           \
                __CaseArgument(uint8,   _xt2)                                           \
                __CaseArgument(int16,   _xt2)                                           \
                __CaseArgument(uint16,  _xt2)                                           \
                __CaseArgument(int32,   _xt2)                                           \
                __CaseArgument(uint32,  _xt2)                                           \
                __CaseArgument(int64,   _xt2)                                           \
                __CaseArgument(uint64,  _xt2)                                           \
                __CaseArgument(float_,  _xt2)                                           \
                __CaseArgument(double_, _xt2)                                           \
                __CaseArgument(char_,   _xt2)                                           \
                __CaseArgument(bool_,   _xt2)

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
            // Push Argument content

            #define __CaseArgumentContent(_xt1, _xt2)                                   \
                case __V(xil_storage_type_t::argument_content, xil_type_t::_xt1, xil_type_t::_xt2): \
                    return __command_manager.template get_command<                      \
                        __StCmd(push, argument_content, _xt1, _xt2),                    \
                        xil_storage_type_t::argument_content, xil_type_t::_xt1, xil_type_t::_xt2 \
                    >(args.argument_content.offset);


            #define __CaseArgumentContents(_xt2)                                        \
                __CaseArgumentContent(int8,    _xt2)                                    \
                __CaseArgumentContent(uint8,   _xt2)                                    \
                __CaseArgumentContent(int16,   _xt2)                                    \
                __CaseArgumentContent(uint16,  _xt2)                                    \
                __CaseArgumentContent(int32,   _xt2)                                    \
                __CaseArgumentContent(uint32,  _xt2)                                    \
                __CaseArgumentContent(int64,   _xt2)                                    \
                __CaseArgumentContent(uint64,  _xt2)                                    \
                __CaseArgumentContent(float_,  _xt2)                                    \
                __CaseArgumentContent(double_, _xt2)                                    \
                __CaseArgumentContent(char_,   _xt2)                                    \
                __CaseArgumentContent(bool_,   _xt2)

            __CaseArgumentContents(int8)
            __CaseArgumentContents(uint8)
            __CaseArgumentContents(int16)
            __CaseArgumentContents(uint16)
            __CaseArgumentContents(int32)
            __CaseArgumentContents(uint32)
            __CaseArgumentContents(int64)
            __CaseArgumentContents(uint64)
            __CaseArgumentContents(float_)
            __CaseArgumentContents(double_)
            __CaseArgumentContents(char_)
            __CaseArgumentContents(bool_)
            __CaseArgumentContent(object,  object)
            __CaseArgumentContent(string,  string)
            __CaseArgumentContent(ptr,     ptr)

            #undef __CaseArgumentContent
            #undef __CaseArgumentContents

            // - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Push Field

            #define __CaseField(_xt1, _xt2)                                             \
                case __V(xil_storage_type_t::field, xil_type_t::_xt1, xil_type_t::_xt2):    \
                    return __command_manager.template get_command<                      \
                        __StCmd(push, field, _xt1, _xt2),                               \
                        xil_storage_type_t::field, xil_type_t::_xt1, xil_type_t::_xt2   \
                    >(args.field.offset);

            #define __CaseFields(_xt2)                                                  \
                __CaseField(int8,       _xt2)                                           \
                __CaseField(uint8,      _xt2)                                           \
                __CaseField(int16,      _xt2)                                           \
                __CaseField(uint16,     _xt2)                                           \
                __CaseField(int32,      _xt2)                                           \
                __CaseField(uint32,     _xt2)                                           \
                __CaseField(int64,      _xt2)                                           \
                __CaseField(uint64,     _xt2)                                           \
                __CaseField(float_,     _xt2)                                           \
                __CaseField(double_,    _xt2)                                           \
                __CaseField(char_,      _xt2)                                           \
                __CaseField(bool_,      _xt2)


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

            #define __CaseArrayElement_Dimension(_xt1, _xt2, _dimension, dimension)     \
                case _dimension:                                                        \
                    return __push_array_element_command_manager.template get_command<   \
                        __StArrayCmd(push, _xt1, _xt2, _dimension),                     \
                        xil_type_t::_xt1, xil_type_t::_xt2, _dimension                  \
                    >(dimension);

            #define __CaseArrayElement(_xt1, _xt2)                                      \
                case __V(xil_storage_type_t::array_element, xil_type_t::_xt1, xil_type_t::_xt2): { \
                    dimension_t dimension = args.array_element.dimension;               \
                    switch (dimension <= 8? dimension : 0)                              \
                    {                                                                   \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 0, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 1, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 2, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 3, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 4, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 5, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 6, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 7, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 8, dimension)          \
                    }                                                                   \
                }   break;

            #define __CaseArrayElements(_xt2)                                           \
                __CaseArrayElement(int8,    _xt2)                                       \
                __CaseArrayElement(uint8,   _xt2)                                       \
                __CaseArrayElement(int16,   _xt2)                                       \
                __CaseArrayElement(uint16,  _xt2)                                       \
                __CaseArrayElement(int32,   _xt2)                                       \
                __CaseArrayElement(uint32,  _xt2)                                       \
                __CaseArrayElement(int64,   _xt2)                                       \
                __CaseArrayElement(uint64,  _xt2)                                       \
                __CaseArrayElement(float_,  _xt2)                                       \
                __CaseArrayElement(double_, _xt2)                                       \
                __CaseArrayElement(char_,   _xt2)                                       \
                __CaseArrayElement(bool_,   _xt2)

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
            #undef __CaseArrayElement_Dimension

            // - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Push Convert

            #define __CaseConvert(_xt1, _xt2)                                           \
                case __V(xil_storage_type_t::convert, xil_type_t::_xt1, xil_type_t::_xt2):  \
                    return __convert_command_manager.template get_command<              \
                        __StCmd(push, convert, _xt1, _xt2),                             \
                        xil_type_t::_xt1, xil_type_t::_xt2                              \
                    >();

            #define __CaseConverts(_xt2)                                                \
                __CaseConvert(int8,       _xt2)                                         \
                __CaseConvert(uint8,      _xt2)                                         \
                __CaseConvert(int16,      _xt2)                                         \
                __CaseConvert(uint16,     _xt2)                                         \
                __CaseConvert(int32,      _xt2)                                         \
                __CaseConvert(uint32,     _xt2)                                         \
                __CaseConvert(int64,      _xt2)                                         \
                __CaseConvert(uint64,     _xt2)                                         \
                __CaseConvert(float_,     _xt2)                                         \
                __CaseConvert(double_,    _xt2)                                         \
                __CaseConvert(char_,      _xt2)                                         \
                __CaseConvert(bool_,      _xt2)

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
                X_UNEXPECTED_F("unexpected command: push %1% %2% %3%", args.stype, xt1, xt2);
        }

        X_UNEXPECTED();

        #undef __V
        #undef __ConstantStringValue
        #undef __ConstantValue
    }

    static command_t * __new_push_command(__context_t & ctx, const push_xil_t & xil)
    {
        __new_push_command_args_t args {
            .stype = xil.stype()
        };

        xil_type_t xt1 = xil_type_t::empty;
        xil_type_t xt2 = xil_type_t::empty;

        typedef xil_extra_t<push_xil_t> exxil_t;

        switch (args.stype)
        {
            case xil_storage_type_t::constant:
                __analyze_xil_types(ctx, xil, &xt1, &xt2, false);

                args.constant.dtype = xil.dtype();
                args.constant.data  = (void *)((const exxil_t &)xil).extra;

                if (args.constant.dtype == xil_type_t::object)  // System.Type
                    args.constant.type = ctx.get_type(xil.get_ref());

                break;

            case xil_storage_type_t::duplicate:
                break;

            case xil_storage_type_t::params:
                args.params.offset = ctx.params_layout.extends_offset();
                break;

            case xil_storage_type_t::object:
                args.object.type = xil.get_object_type();
                args.object.object = __get_object(ctx, args.object.type, xil.get_ref()
                #if EXEC_TRACE
                    , &args.object.name
                #endif
                );
                break;

            case xil_storage_type_t::local_addr:
                args.local_addr.offset = ctx.locals_layout.offset_of(xil.get_identity());
                break;

            case xil_storage_type_t::argument_addr:
                args.argument_addr.offset = __offset_of_argument(ctx, xil.get_identity());
                break;

            case xil_storage_type_t::field_addr:
                args.field_addr.offset = __field_offset(ctx, xil.get_ref());
                break;

            case xil_storage_type_t::local:
                __analyze_xil_types(ctx, xil, &xt1, &xt2, false);
                args.local.offset = ctx.locals_layout.offset_of(xil.get_identity()); 
                break;

            case xil_storage_type_t::argument:
                __analyze_xil_types(ctx, xil, &xt1, &xt2, false);
                args.argument.offset = __offset_of_argument(ctx, xil.get_identity());
                break;

            case xil_storage_type_t::argument_content:
                __analyze_xil_types(ctx, xil, &xt1, &xt2, false);
                args.argument_content.offset = __offset_of_argument(ctx, xil.get_identity());
                break;

            case xil_storage_type_t::field:
                __analyze_xil_types(ctx, xil, &xt1, &xt2, false);
                args.field.offset = __field_offset(ctx, xil.get_ref());
                break;

            case xil_storage_type_t::array_element:
                __analyze_xil_types(ctx, xil, &xt1, &xt2, false);
                args.array_element.dimension = __array_dimension(ctx, xil.get_ref());
                break;

            case xil_storage_type_t::array_element_addr: {
                rt_array_type_t * type = __array_type(ctx, xil.get_ref());
                rt_type_t * element_type = type->element_type;

                args.array_element_addr.dimension = type->dimension;
                args.array_element_addr.element_size = element_type->get_variable_size(ctx.env);
            }   break;

            case xil_storage_type_t::convert:
                xt1 = xil.dtype();
                xt2 = xil.dtype2();
                break;

            case xil_storage_type_t::box:
                args.box.type     = ctx.get_type(xil.get_ref());
                args.box.box_type = xil.get_box_type();
                break;

            case xil_storage_type_t::unbox:
                args.unbox.type     = ctx.get_type(xil.get_ref());
                args.unbox.box_type = xil.get_box_type();
                break;

            default:
                __analyze_xil_types(ctx, xil, &xt1, &xt2, false);
                break;
        }

        args.xt1 = xt1;
        args.xt2 = xt2;

        return __new_push_command(ctx, args);
    }

    ////////// ////////// ////////// ////////// //////////
    // Class __pop_command_t

    template<command_value_t _cv, xil_storage_type_t _st, xil_type_t _xt1, xil_type_t _xt2>
    class __pop_command_t { };

    //-------- ---------- ---------- ---------- ----------

    #if EXEC_TRACE

    #define __PopCommand_ToString(_st, _xt1, _xt2, _v)                                  \
        __BeginToString()                                                               \
                                                                                        \
            if (_xt1 == _xt2)                                                           \
                return _F(_T("pop  %1% %2% [%3%]"), xil_storage_type_t::_st, _xt1, _v); \
                                                                                        \
            return _F(_T("pop  %1% %2%=>%3% [%4%]"), xil_storage_type_t::_st,           \
                        _xt2, _xt1, _v);                                                \
        __EndToString()

    #else
        
    #define __PopCommand_ToString(_st, _xt1, _xt2, _v)

    #endif  // EXEC_TRACE

    //-------- ---------- ---------- ---------- ----------
    // Pop local commands

    template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2>
    class __pop_command_t<_cv, xil_storage_type_t::local, _xt1, _xt2>
        : public __command_base_t<_cv>
    {
        typedef __pop_command_t     __self_t;
        typedef __vnum_t<_xt1>      __value1_t;
        typedef __vnum_t<_xt2>      __value2_t;

    public:
        __pop_command_t(__offset_t offset) : __offset(offset) { }

        __PopCommand_ToString(local, _xt1, _xt2, __This->__offset)

        __BeginExecute(ctx)

            __Local(__value1_t, __This->__offset) = static_cast<__value1_t>(
                ctx.stack.pop<__value2_t>()
            );

        __EndExecute()

    private:
        __offset_t __offset;
    };

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(pop, local, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    //-------- ---------- ---------- ---------- ----------
    // Pop argument commands

    template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2>
    class __pop_command_t<_cv, xil_storage_type_t::argument, _xt1, _xt2>
        : public __command_base_t<_cv>
    {
        typedef __pop_command_t     __self_t;
        typedef __vnum_t<_xt1>      __value1_t;
        typedef __vnum_t<_xt2>      __value2_t;

    public:
        __pop_command_t(__offset_t offset) : __real_offset(offset + __stack_stub_size) { }

        __PopCommand_ToString(argument, _xt1, _xt2, __This->__real_offset - __stack_stub_size)

        __BeginExecute(ctx)

            __Argument(__value1_t, __This->__real_offset) =
                static_cast<__value1_t>(ctx.stack.pop<__value2_t>());

        __EndExecute()

    private:
        __offset_t __real_offset;
    };

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(pop, argument, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    //-------- ---------- ---------- ---------- ----------
    // Pop argument_addr commands

    template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2>
    class __pop_command_t<_cv, xil_storage_type_t::argument_addr, _xt1, _xt2>
        : public __command_base_t<_cv>
    {
        typedef __pop_command_t     __self_t;
        typedef __vnum_t<_xt1>      __value1_t;
        typedef __vnum_t<_xt2>      __value2_t;

    public:
        __pop_command_t(__offset_t offset) : __real_offset(offset + __stack_stub_size) { }

        __PopCommand_ToString(argument, _xt1, _xt2, __This->__real_offset - __stack_stub_size)

        __BeginExecute(ctx)

            *(__value1_t *)__Argument(void *, __This->__real_offset) =
                static_cast<__value1_t>(ctx.stack.pop<__value2_t>());
            
        __EndExecute()

    private:
        __offset_t __real_offset;
    };

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(pop, argument_addr, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    //-------- ---------- ---------- ---------- ----------

    template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2>
    class __pop_command_t<_cv, xil_storage_type_t::field, _xt1, _xt2>
        : public __command_base_t<_cv>
    {
        typedef __pop_command_t     __self_t;
        typedef __vnum_t<_xt1>      __value1_t;
        typedef __vnum_t<_xt2>      __value2_t;

    public:
        __pop_command_t(__offset_t offset) : __offset(offset) { }

        __PopCommand_ToString(field, _xt1, _xt2, __This->__offset)

        __BeginExecute(ctx)

            void * field = __FieldAddress(ctx.stack.pop<rt_ref_t>(), __This->__offset);
            *(__value1_t *)field = static_cast<__value1_t>(ctx.stack.pop<__value2_t>());

        __EndExecute()

    private:
        __offset_t __offset;
    };

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(pop, field, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    //-------- ---------- ---------- ---------- ----------

    struct __pop_command_template_t
    {
        template<command_value_t _cv, xil_storage_type_t _st, xil_type_t _xt1, xil_type_t _xt2,
                                                            typename ... _args_t>
        static auto new_command(memory_t * memory, _args_t && ... args)
        {
            typedef __pop_command_t<_cv, _st, _xt1, _xt2> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<_args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2, int _dimension>
    class __pop_array_element_command_t : public __command_base_t<_cv>
    {
        typedef __vnum_t<_xt1> __value1_t;
        typedef __vnum_t<_xt2> __value2_t;

    public:

        #if EXEC_TRACE

        __ToString(_T("pop array element"));

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            rt_ref_t array_ref = ctx.stack.pop<rt_ref_t>(); 
            array_length_t index = ctx.stack.pop<array_length_t>();

            if (_dimension >= 2)
            {
                array_length_t * lengths = mm::get_array_lengths(array_ref);
                index += __mul_array_index<_dimension - 1>(ctx.stack, lengths);
            }

            if (index >= mm::get_array_length(array_ref))
                __RaiseOutOfRangeException(ctx);

            mm::set_array_element<__value1_t>(
                array_ref, index, static_cast<__value1_t>(ctx.stack.pop<__value2_t>())
            );

        __EndExecute()
    };

    //-------- ---------- ---------- ---------- ----------

    template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2>
    class __pop_array_element_command_t<_cv, _xt1, _xt2, 0> : public __command_base_t<_cv>
    {
        typedef __pop_array_element_command_t   __self_t;
        typedef __vnum_t<_xt1>                  __value1_t;
        typedef __vnum_t<_xt2>                  __value2_t;

    public:

        __pop_array_element_command_t(dimension_t dimension) : __dimension(dimension) { }

        #if EXEC_TRACE

        __ToString(_T("pop array element"));

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

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

    #define __CaseDimensionXilTypePair( _xt1, _xt2, _dimension )                       \
        __DefineStArrayCmdValue(pop, _xt1, _xt2, _dimension)

    __EachDimensionXilTypePairs()

    #undef __CaseDimensionXilTypePair

    //-------- ---------- ---------- ---------- ----------

    struct __pop_array_element_command_template_t
    {
        template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2,
            dimension_t _dimension, typename ... _args_t
        >
        static command_t * new_command(memory_t * memory, dimension_t dimension,
            _args_t && ... args)
        {
            if constexpr (_dimension == 0)
            {
                return __new_command<__pop_array_element_command_t<_cv, _xt1, _xt2, 0>>(
                    memory, dimension, std::forward<_args_t>(args) ...
                );
            }
            else
            {
                return __new_command<__pop_array_element_command_t<_cv, _xt1, _xt2, _dimension>>(
                    memory, std::forward<_args_t>(args) ...
                );
            }
        }
    };


    //-------- ---------- ---------- ---------- ----------

    #define __PopEmptyCmd   __Cmd(pop, empty)
    __DefineCmdValue_(__PopEmptyCmd)

    class __pop_empty_command_t : public __command_base_t<__PopEmptyCmd>
    {
        typedef __pop_empty_command_t   __self_t;

    public:
        __pop_empty_command_t(msize_t unit_size) : __unit_size(unit_size) { }

        #if EXEC_TRACE

        __ToString(_T("pop empty"));

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            ctx.stack.pop(__This->__unit_size);

        __EndExecute()

    private:
        msize_t __unit_size;
    };

    struct __pop_empty_command_template_t
    {
        template<command_value_t _cv, typename ... args_t>
        static auto new_command(memory_t * memory, args_t && ... args)
        {
            typedef __pop_empty_command_t this_command_t;
            return __new_command<this_command_t>(memory, std::forward<args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    static msize_t __size_of_pop_empty(__context_t & ctx, const pop_xil_t & xil)
    {
        _A(xil.stype() == xil_storage_type_t::empty);

        if (xil.dtype() != xil_type_t::empty)
            return get_vtype_size(to_vtype(xil.dtype()));

        ref_t type_ref = xil.get_ref();
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
            __pop_empty_command_template_t
        >::with_args_t<msize_t> __empty_command_manager;

        static __command_manager_t<
            __pop_array_element_command_template_t, xil_type_t, xil_type_t, dimension_t
        >::with_args_t<dimension_t> __array_element_command_manager;

        if (xil.stype() == xil_storage_type_t::empty)
        {
            return __empty_command_manager.template get_command<__PopEmptyCmd>(
                __unit_size_of_pop_empty(ctx, xil)
            );
        }

        #define __V(_s, _d1, _d2)                                                       \
            (((uint32_t)(_s) << 16) | ((uint32_t)(_d1) << 12) | ((uint32_t)(_d2)) << 8)

        xil_type_t dtype1, dtype2;
        __analyze_xil_types(ctx, xil, &dtype1, &dtype2, true);

        xil_storage_type_t stype = xil.stype();

        switch (__V(stype, dtype1, dtype2))
        {
            // - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Pop Local

            #define __CaseLocal(_xt1, _xt2)                                             \
                case __V(xil_storage_type_t::local, xil_type_t::_xt1, xil_type_t::_xt2):\
                    return __command_manager.template get_command<                      \
                        __StCmd(pop, local, _xt1, _xt2),                                \
                        xil_storage_type_t::local, xil_type_t::_xt1, xil_type_t::_xt2   \
                    >(ctx.locals_layout.offset_of(xil.get_identity()));

            #define __CaseLocals(_xt2)                                                  \
                __CaseLocal(int8,       _xt2)                                           \
                __CaseLocal(uint8,      _xt2)                                           \
                __CaseLocal(int16,      _xt2)                                           \
                __CaseLocal(uint16,     _xt2)                                           \
                __CaseLocal(int32,      _xt2)                                           \
                __CaseLocal(uint32,     _xt2)                                           \
                __CaseLocal(int64,      _xt2)                                           \
                __CaseLocal(uint64,     _xt2)                                           \
                __CaseLocal(float_,     _xt2)                                           \
                __CaseLocal(double_,    _xt2)                                           \
                __CaseLocal(char_,      _xt2)                                           \
                __CaseLocal(bool_,      _xt2)

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

            #define __CaseArgument(_xt1, _xt2)                                          \
                case __V(xil_storage_type_t::argument, xil_type_t::_xt1, xil_type_t::_xt2): \
                    return __command_manager.template get_command<                      \
                        __StCmd(pop, argument, _xt1, _xt2),                             \
                        xil_storage_type_t::argument, xil_type_t::_xt1, xil_type_t::_xt2\
                    >(ctx.params_layout.offset_of(xil.get_identity()));

            #define __CaseArguments(_xt2)                                               \
                __CaseArgument(int8,    _xt2)                                           \
                __CaseArgument(uint8,   _xt2)                                           \
                __CaseArgument(int16,   _xt2)                                           \
                __CaseArgument(uint16,  _xt2)                                           \
                __CaseArgument(int32,   _xt2)                                           \
                __CaseArgument(uint32,  _xt2)                                           \
                __CaseArgument(int64,   _xt2)                                           \
                __CaseArgument(uint64,  _xt2)                                           \
                __CaseArgument(float_,  _xt2)                                           \
                __CaseArgument(double_, _xt2)                                           \
                __CaseArgument(char_,   _xt2)                                           \
                __CaseArgument(bool_,   _xt2)

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
            // Pop Argument addr

            #define __CaseArgumentAddr(_xt1, _xt2)                                      \
                case __V(xil_storage_type_t::argument_addr, xil_type_t::_xt1, xil_type_t::_xt2): \
                    return __command_manager.template get_command<                      \
                        __StCmd(pop, argument, _xt1, _xt2),                             \
                        xil_storage_type_t::argument_addr, xil_type_t::_xt1, xil_type_t::_xt2 \
                    >(ctx.params_layout.offset_of(xil.get_identity()));

            #define __CaseArgumentAddrs(_xt2)                                           \
                __CaseArgumentAddr(int8,    _xt2)                                       \
                __CaseArgumentAddr(uint8,   _xt2)                                       \
                __CaseArgumentAddr(int16,   _xt2)                                       \
                __CaseArgumentAddr(uint16,  _xt2)                                       \
                __CaseArgumentAddr(int32,   _xt2)                                       \
                __CaseArgumentAddr(uint32,  _xt2)                                       \
                __CaseArgumentAddr(int64,   _xt2)                                       \
                __CaseArgumentAddr(uint64,  _xt2)                                       \
                __CaseArgumentAddr(float_,  _xt2)                                       \
                __CaseArgumentAddr(double_, _xt2)                                       \
                __CaseArgumentAddr(char_,   _xt2)                                       \
                __CaseArgumentAddr(bool_,   _xt2)

            __CaseArgumentAddrs(int8)
            __CaseArgumentAddrs(uint8)
            __CaseArgumentAddrs(int16)
            __CaseArgumentAddrs(uint16)
            __CaseArgumentAddrs(int32)
            __CaseArgumentAddrs(uint32)
            __CaseArgumentAddrs(int64)
            __CaseArgumentAddrs(uint64)
            __CaseArgumentAddrs(float_)
            __CaseArgumentAddrs(double_)
            __CaseArgumentAddrs(char_)
            __CaseArgumentAddrs(bool_)
            __CaseArgumentAddr(object,  object)
            __CaseArgumentAddr(string,  string)
            __CaseArgumentAddr(ptr,     ptr)

            #undef __CaseArgumentAddr
            #undef __CaseArgumentAddrs

            // - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Pop Field

            #define __CaseField(_xt1, _xt2)                                             \
                case __V(xil_storage_type_t::field, xil_type_t::_xt1, xil_type_t::_xt2):    \
                    return __command_manager.template get_command<                      \
                        __StCmd(pop, field, _xt1, _xt2),                                \
                        xil_storage_type_t::field, xil_type_t::_xt1, xil_type_t::_xt2   \
                    >(__field_offset(ctx, xil.get_ref()));

            #define __CaseFields(_xt2)                                                  \
                __CaseField(int8,       _xt2)                                           \
                __CaseField(uint8,      _xt2)                                           \
                __CaseField(int16,      _xt2)                                           \
                __CaseField(uint16,     _xt2)                                           \
                __CaseField(int32,      _xt2)                                           \
                __CaseField(uint32,     _xt2)                                           \
                __CaseField(int64,      _xt2)                                           \
                __CaseField(uint64,     _xt2)                                           \
                __CaseField(float_,     _xt2)                                           \
                __CaseField(double_,    _xt2)                                           \
                __CaseField(char_,      _xt2)                                           \
                __CaseField(bool_,      _xt2)

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

            #define __CaseArrayElement_Dimension(_xt1, _xt2, _dimension, dimension)     \
                case _dimension:                                                        \
                    return __array_element_command_manager.template get_command<        \
                        __StArrayCmd(pop, _xt1, _xt2, _dimension),                      \
                        xil_type_t::_xt1, xil_type_t::_xt2, _dimension                  \
                    >(dimension);

            #define __CaseArrayElement(_xt1, _xt2)                                      \
                case __V(xil_storage_type_t::array_element, xil_type_t::_xt1, xil_type_t::_xt2): { \
                    dimension_t dimension = __array_dimension(ctx, xil.get_ref());      \
                    switch (dimension <= 8? dimension : 0)                              \
                    {                                                                   \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 0, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 1, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 2, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 3, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 4, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 5, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 6, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 7, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 8, dimension)          \
                    }                                                                   \
                }   break;

            #define __CaseArrayElements(_xt2)                                           \
                __CaseArrayElement(int8,    _xt2)                                       \
                __CaseArrayElement(uint8,   _xt2)                                       \
                __CaseArrayElement(int16,   _xt2)                                       \
                __CaseArrayElement(uint16,  _xt2)                                       \
                __CaseArrayElement(int32,   _xt2)                                       \
                __CaseArrayElement(uint32,  _xt2)                                       \
                __CaseArrayElement(int64,   _xt2)                                       \
                __CaseArrayElement(uint64,  _xt2)                                       \
                __CaseArrayElement(float_,  _xt2)                                       \
                __CaseArrayElement(double_, _xt2)                                       \
                __CaseArrayElement(char_,   _xt2)                                       \
                __CaseArrayElement(bool_,   _xt2)

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
            #undef __CaseArrayElement_Dimension

            // - - - - - - - - - - - - - - - - - - - - - - - - - -

            default:
                X_UNEXPECTED_F("unexpected pop data types: %1% %2% %3%", stype, dtype1, dtype2);
        }

        X_UNEXPECTED();

        #undef __V
    }

    ////////// ////////// ////////// ////////// //////////
    // Class __pick_command_t

    template<command_value_t _cv, xil_storage_type_t _st, xil_type_t _xt1, xil_type_t _xt2>
    class __pick_command_t { };

    //-------- ---------- ---------- ---------- ----------

    #if EXEC_TRACE

    #define __PickCommand_ToString(_st, _xt1, _xt2, _v)                                 \
                                                                                        \
        __BeginToString()                                                               \
                                                                                        \
            if (_xt1 == _xt2)                                                           \
                return _F(_T("pick %1% %2% [%3%]"), xil_storage_type_t::_st,_xt1, _v);  \
                                                                                        \
            return _F(_T("pick %1% %2%=>%3% [%4%]"), xil_storage_type_t::_st,           \
                        _xt2, _xt1, _v);                                                \
                                                                                        \
        __EndToString()                                                                 \

    #else

    #define __PickCommand_ToString(_st, _xt1, _xt2, _v)

    #endif  // EXEC_TRACE

    //-------- ---------- ---------- ---------- ----------
    // Pick local commands

    template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2>
    class __pick_command_t<_cv, xil_storage_type_t::local, _xt1, _xt2>
        : public __command_base_t<_cv>
    {
        typedef __pick_command_t        __self_t;
        typedef __vnum_t<_xt1>          __value1_t;
        typedef __vnum_t<_xt2>          __value2_t;

    public:
        __pick_command_t(__offset_t offset) : __offset(offset) { }

        __PickCommand_ToString(local, _xt1, _xt2, __This->__offset)

        __BeginExecute(ctx)

            __Local(__value1_t, __This->__offset) = static_cast<__value1_t>(
                ctx.stack.pick<__value2_t>()
            );

        __EndExecute()

    private:
        __offset_t __offset;
    };

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(pick, local, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    //-------- ---------- ---------- ---------- ----------
    // Pick argument commands

    template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2>
    class __pick_command_t<_cv, xil_storage_type_t::argument, _xt1, _xt2>
        : public __command_base_t<_cv>
    {
        typedef __pick_command_t        __self_t;
        typedef __vnum_t<_xt1>          __value1_t;
        typedef __vnum_t<_xt2>          __value2_t;

    public:
        __pick_command_t(__offset_t offset) : __offset(offset) { }

        __PickCommand_ToString(argument, _xt1, _xt2, __This->__offset)

        __BeginExecute(ctx)

            __Argument(__value1_t, __This->__offset + __stack_stub_size) =
                static_cast<__value1_t>(ctx.stack.pick<__value2_t>());

        __EndExecute()

    private:
        __offset_t __offset;
    };

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(pick, argument, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    //-------- ---------- ---------- ---------- ----------
    // Pick field commands

    template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2>
    class __pick_command_t<_cv, xil_storage_type_t::field, _xt1, _xt2>
        : public __command_base_t<_cv>
    {
        typedef __pick_command_t        __self_t;
        typedef __vnum_t<_xt1>          __value1_t;
        typedef __vnum_t<_xt2>          __value2_t;

    public:
        __pick_command_t(__offset_t offset) : __offset(offset) { }

        __PickCommand_ToString(field, _xt1, _xt2, __This->__offset)

        __BeginExecute(ctx)

            void * field = __FieldAddress(ctx.stack.pop<rt_ref_t>(), __This->__offset);
            *(__value1_t *)field = static_cast<__value1_t>(ctx.stack.pick<__value2_t>());

        __EndExecute()

    private:
        __offset_t __offset;
    };

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(pick, field, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    //-------- ---------- ---------- ---------- ----------

    struct __pick_command_template_t
    {
        template<command_value_t _cv, xil_storage_type_t _st, xil_type_t _xt1, xil_type_t _xt2,
                                                            typename ... _args_t>
        static auto new_command(memory_t * memory, _args_t && ... args)
        {
            typedef __pick_command_t<_cv, _st, _xt1, _xt2> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<_args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2, int _dimension>
    class __pick_array_element_command_t : public __command_base_t<_cv>
    {
        typedef __pick_array_element_command_t  __self_t;
        typedef __vnum_t<_xt1>                  __value1_t;
        typedef __vnum_t<_xt2>                  __value2_t;

    public:

        #if EXEC_TRACE

        __ToString(_T("pick array element"));

        #endif      // EXEC_TRACE

        __BeginExecute(ctx)

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

    template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2>
    class __pick_array_element_command_t<_cv, _xt1, _xt2, 0> : public __command_base_t<_cv>
    {
        typedef __pick_array_element_command_t  __self_t;
        typedef __vnum_t<_xt1>                  __value1_t;
        typedef __vnum_t<_xt2>                  __value2_t;

    public:

        __pick_array_element_command_t(dimension_t dimension) : __dimension(dimension) { }

        #if EXEC_TRACE

        __ToString(_T("pick array element"));

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

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

    #define __CaseDimensionXilTypePair( _xt1, _xt2, _dimension )                        \
        __DefineStArrayCmdValue(pick, _xt1, _xt2, _dimension)

    __EachDimensionXilTypePairs()

    #undef __CaseDimensionXilTypePair

    //-------- ---------- ---------- ---------- ----------

    struct __pick_array_element_command_template_t
    {
        template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2,
            dimension_t _dimension, typename ... _args_t
        >
        static command_t * new_command(memory_t * memory, dimension_t dimension,
            _args_t && ... args)
        {
            if constexpr (_dimension == 0)
            {
                return __new_command<__pick_array_element_command_t<_cv, _xt1, _xt2, 0>>(
                    memory, dimension, std::forward<_args_t>(args) ...
                );
            }
            else
            {
                return __new_command<__pick_array_element_command_t<_cv, _xt1, _xt2, _dimension>>(
                    memory, std::forward<_args_t>(args) ...
                );
            }
        }
    };

    //-------- ---------- ---------- ---------- ----------

    static command_t * __new_pick_command(__context_t & ctx, const pick_xil_t & xil)
    {
        static __command_manager_t<
            __pick_command_template_t, xil_storage_type_t, xil_type_t, xil_type_t
        >::with_args_t<msize_t> __command_manager;

        static __command_manager_t<
            __pick_array_element_command_template_t, xil_type_t, xil_type_t, dimension_t
        >::with_args_t<dimension_t> __array_element_command_manager;

        #define __V(_s, _d1, _d2)                                                       \
            (((uint32_t)(_s) << 16) | ((uint32_t)(_d1) << 12) | ((uint32_t)(_d2)) << 8)

        xil_type_t dtype1, dtype2;
        __analyze_xil_types(ctx, xil, &dtype1, &dtype2, true);

        xil_storage_type_t stype = xil.stype();

        switch (__V(stype, dtype1, dtype2))
        {
            // - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Pick Local

            #define __CaseLocal(_xt1, _xt2)                                             \
                case __V(xil_storage_type_t::local, xil_type_t::_xt1, xil_type_t::_xt2):    \
                    return __command_manager.template get_command<                      \
                        __StCmd(pick, local, _xt1, _xt2),                               \
                        xil_storage_type_t::local, xil_type_t::_xt1, xil_type_t::_xt2   \
                    >(ctx.locals_layout.offset_of(xil.get_identity()));

            #define __CaseLocals(_xt2)                                                  \
                __CaseLocal(int8,       _xt2)                                           \
                __CaseLocal(uint8,      _xt2)                                           \
                __CaseLocal(int16,      _xt2)                                           \
                __CaseLocal(uint16,     _xt2)                                           \
                __CaseLocal(int32,      _xt2)                                           \
                __CaseLocal(uint32,     _xt2)                                           \
                __CaseLocal(int64,      _xt2)                                           \
                __CaseLocal(uint64,     _xt2)                                           \
                __CaseLocal(float_,     _xt2)                                           \
                __CaseLocal(double_,    _xt2)                                           \
                __CaseLocal(char_,      _xt2)                                           \
                __CaseLocal(bool_,      _xt2)


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

            #define __CaseArgument(_xt1, _xt2)                                          \
                case __V(xil_storage_type_t::argument, xil_type_t::_xt1, xil_type_t::_xt2): \
                    return __command_manager.template get_command<                      \
                        __StCmd(pick, argument, _xt1, _xt2),                            \
                        xil_storage_type_t::argument, xil_type_t::_xt1, xil_type_t::_xt2    \
                    >(ctx.params_layout.offset_of(xil.get_identity()));

            #define __CaseArguments(_xt2)                                               \
                __CaseArgument(int8,    _xt2)                                           \
                __CaseArgument(uint8,   _xt2)                                           \
                __CaseArgument(int16,   _xt2)                                           \
                __CaseArgument(uint16,  _xt2)                                           \
                __CaseArgument(int32,   _xt2)                                           \
                __CaseArgument(uint32,  _xt2)                                           \
                __CaseArgument(int64,   _xt2)                                           \
                __CaseArgument(uint64,  _xt2)                                           \
                __CaseArgument(float_,  _xt2)                                           \
                __CaseArgument(double_, _xt2)                                           \
                __CaseArgument(char_,   _xt2)                                           \
                __CaseArgument(bool_,   _xt2)

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

            #define __CaseField(_xt1, _xt2)                                             \
                case __V(xil_storage_type_t::field, xil_type_t::_xt1, xil_type_t::_xt2):    \
                    return __command_manager.template get_command<                      \
                        __StCmd(pick, field, _xt1, _xt2),                               \
                        xil_storage_type_t::field, xil_type_t::_xt1, xil_type_t::_xt2   \
                    >(__field_offset(ctx, xil.get_ref()));

            #define __CaseFields(_xt2)                                                  \
                __CaseField(int8,       _xt2)                                           \
                __CaseField(uint8,      _xt2)                                           \
                __CaseField(int16,      _xt2)                                           \
                __CaseField(uint16,     _xt2)                                           \
                __CaseField(int32,      _xt2)                                           \
                __CaseField(uint32,     _xt2)                                           \
                __CaseField(int64,      _xt2)                                           \
                __CaseField(uint64,     _xt2)                                           \
                __CaseField(float_,     _xt2)                                           \
                __CaseField(double_,    _xt2)                                           \
                __CaseField(char_,      _xt2)                                           \
                __CaseField(bool_,      _xt2)

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

            #define __CaseArrayElement_Dimension(_xt1, _xt2, _dimension, dimension)     \
                case _dimension:                                                        \
                    return __array_element_command_manager.template get_command<        \
                        __StArrayCmd(pick, _xt1, _xt2, _dimension),                     \
                        xil_type_t::_xt1, xil_type_t::_xt2, _dimension                  \
                    >(dimension);

            #define __CaseArrayElement(_xt1, _xt2)                                      \
                case __V(xil_storage_type_t::array_element, xil_type_t::_xt1, xil_type_t::_xt2): { \
                    dimension_t dimension = __array_dimension(ctx, xil.get_ref());      \
                    switch (dimension <= 8? dimension : 0)                              \
                    {                                                                   \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 0, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 1, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 2, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 3, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 4, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 5, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 6, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 7, dimension)          \
                        __CaseArrayElement_Dimension(_xt1, _xt2, 8, dimension)          \
                    }                                                                   \
                }   break;

            #define __CaseArrayElements(_xt2)                                           \
                __CaseArrayElement(int8,    _xt2)                                       \
                __CaseArrayElement(uint8,   _xt2)                                       \
                __CaseArrayElement(int16,   _xt2)                                       \
                __CaseArrayElement(uint16,  _xt2)                                       \
                __CaseArrayElement(int32,   _xt2)                                       \
                __CaseArrayElement(uint32,  _xt2)                                       \
                __CaseArrayElement(int64,   _xt2)                                       \
                __CaseArrayElement(uint64,  _xt2)                                       \
                __CaseArrayElement(float_,  _xt2)                                       \
                __CaseArrayElement(double_, _xt2)                                       \
                __CaseArrayElement(char_,   _xt2)                                       \
                __CaseArrayElement(bool_,   _xt2)

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
            #undef __CaseArrayElement_Dimension

            // - - - - - - - - - - - - - - - - - - - - - - - - - -

            default:
                X_UNEXPECTED();
        }

        X_UNEXPECTED();

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

    __AlwaysInline static void __new_object(command_execute_context_t & ctx, rt_type_t * type)
    {
        _A(type != nullptr);

        __pre_new(ctx, type);
        ctx.stack.push<rt_ref_t>(ctx.heap->new_obj(type));
    }

    // Raise exception with specified type.
    static void __raise_exception(command_execute_context_t & ctx,
                                rt_type_t * exception_type, const char_t * message)
    {
        __pre_new(ctx, exception_type);
        rt_ref_t exception = ctx.heap->new_obj(exception_type);

        ctx.restore_stack();
        ctx.push_exception(exception);

        __process_exception(ctx);
    }

    ////////// ////////// ////////// ////////// //////////
    // Class __call_command_t

    #define __CallCmd_(_call_type, _method_type)    __Cmd(call, _call_type##_##_method_type)
    #define __CallCmd(_call_type)                   __Cmd(call, _call_type)

    __DefineCmdValue_(__CallCmd(virtual_))
    __DefineCmdValue_(__CallCmd(interface_))

    __DefineCmdValue_(__CallCmd_(instance, general))
    __DefineCmdValue_(__CallCmd_(instance, generic))
    __DefineCmdValue_(__CallCmd_(instance, dynamic))

    __DefineCmdValue_(__CallCmd(internal))

    __DefineCmdValue_(__CallCmd_(static_, general))
    __DefineCmdValue_(__CallCmd_(static_, generic))
    // __DefineCmdValue_(__CallCmd_(static_, dynamic))

    __DefineCmdValue_(__CallCmd(delegate_init))
    __DefineCmdValue_(__CallCmd(delegate_init_with_call_type))
    __DefineCmdValue_(__CallCmd(delegate_invoke))
    __DefineCmdValue_(__CallCmd(array_get_value))
    __DefineCmdValue_(__CallCmd(array_set_value))

    typedef rtlib::libfunc_t __libfunc_t;

    class __internal_call_command_t : public __command_base_t<__CallCmd(internal)>
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

        __BeginExecute(ctx)

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
        template<command_value_t _cv, typename ... _args_t>
        static auto new_command(memory_t * memory, __libfunc_t libfunc,
                        __context_t & ctx, rt_method_t * rt_method)
        {
            typedef __internal_call_command_t this_command_t;

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
            __internal_call_command_template_t
        >::with_args_t<__libfunc_t> __internal_call_command_manager;

        ref_t method_ref = (ref_t)xil.method;

        rt_method_t * rt_method = _M(rt_method_t *, ctx.get_method(method_ref));
        rt_sid_t method_name = rt_method->get_name();

        __libfunc_t libfunc = rtlib::get_libfunc(method_name);
        if (libfunc == nullptr)
            throw _ED(exec_error_code_t::internal_function_not_found, method_name);

        command_t * cmd = __internal_call_command_manager.template
            get_command<__CallCmd(internal)>(libfunc, ctx, rt_method);

        return cmd;
    }

    //-------- ---------- ---------- ---------- ----------

    class __call_command_base_t_
    {
    public:
        virtual exec_method_t * get_method(command_execute_context_t & ctx) = 0;

        virtual void switch_to(command_execute_context_t & ctx) = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    template<command_value_t _cv, typename _rt_method_t>
    class __call_command_base_t : public __command_base_t<_cv>
                                , public __call_command_base_t_
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

        virtual exec_method_t * get_method(command_execute_context_t & ctx) override
        {
            return this->__get_method(ctx);
        }

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

    template<command_value_t _cv, typename _rt_method_t>
    class __static_call_command_t : public __call_command_base_t<_cv, _rt_method_t>
    {
        typedef __static_call_command_t                     __self_t;
        typedef __call_command_base_t<_cv, _rt_method_t>    __super_t;

    public:
        using __super_t::__super_t;

        __BeginExecute(ctx)

            __pre_static_call(ctx, __This->get_type());

            exec_method_t * method = __This->__get_method(ctx);
            ctx.push_calling(method);

        __EndExecute()

        virtual void switch_to(command_execute_context_t & ctx) override
        {
            __pre_static_call(ctx, __This->get_type());

            exec_method_t * method = __This->__get_method(ctx);
            ctx.current = method->get_commands();
        }

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("call static %1%"), __This->__method_name);
        __EndToString()

        #endif      // EXEC_TRACE
    };

    template<typename _rt_method_t>
    struct __static_call_command_template_t
    {
        template<command_value_t _cv, typename ... args_t>
        static auto new_command(memory_t * memory, _rt_method_t * rt_method, __context_t & ctx)
        {
            typedef __static_call_command_t<_cv, _rt_method_t> this_command_t;
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
                    __static_call_command_template_t<rt_generic_method_t>
                >::with_args_t<rt_generic_method_t *> __static_generic_call_command_manager;

                auto * rt_generic_method = _M(rt_generic_method_t *, ctx.get_method(method_ref));

                return __static_generic_call_command_manager.template
                    get_command<__CallCmd_(static_, generic)>(rt_generic_method, ctx);

            }   break;

            case mt_member_extra_t::import:
            case mt_member_extra_t::internal: {

                static __command_manager_t<
                    __static_call_command_template_t<rt_method_t>
                >::with_args_t<rt_method_t *> __static_call_command_manager;

                rt_method_t * rt_method = _M(rt_method_t *, ctx.get_method(method_ref));
                return __static_call_command_manager.template
                    get_command<__CallCmd_(static_, general)>(rt_method, ctx);

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

    template<command_value_t _cv, typename _rt_method_t>
    class __instance_call_command_t : public __call_command_base_t<_cv, _rt_method_t>
    {
        typedef __instance_call_command_t                   __self_t;
        typedef __call_command_base_t<_cv, _rt_method_t>    __super_t;

    public:
        using __super_t::__super_t;

        __BeginExecute(ctx)

            exec_method_t * method = __This->__get_method(ctx);
            ctx.push_calling(method);

        __EndExecute()

        virtual void switch_to(command_execute_context_t & ctx) override
        {
            exec_method_t * method = __This->__get_method(ctx);
            ctx.current = method->get_commands();
        }

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("call instance %1%"), __This->__method_name);
        __EndToString()

        #endif  // EXEC_TRACE
    };

    template<typename _rt_method_t>
    struct __instance_call_command_template_t
    {
        template<command_value_t _cv, typename ... args_t>
        static auto new_command(memory_t * memory, _rt_method_t * rt_method, __context_t & ctx)
        {
            typedef __instance_call_command_t<_cv, _rt_method_t> this_command_t;
            return __new_command<this_command_t>(memory, rt_method

            #if EXEC_TRACE
                , rt_method->get_name()
            #endif // EXEC_TRACE

            );
        }
    };

    static command_t * __new_instance_call_command(__context_t & ctx, rt_method_base_t * method)
    {
        switch (method->this_type())
        {
            case rt_member_type_t::generic: {

                static __command_manager_t<
                    __instance_call_command_template_t<rt_generic_method_t>
                >::with_args_t<rt_generic_method_t *> __instance_generic_call_command_manager;

                rt_generic_method_t * rt_generic_method = (rt_generic_method_t *)method;

                return __instance_generic_call_command_manager.template
                    get_command<__CallCmd_(instance, generic)>(rt_generic_method, ctx);

            }   break;

            case rt_member_type_t::general: {

                static __command_manager_t<
                    __instance_call_command_template_t<rt_method_t>
                >::with_args_t<rt_method_t *> __instance_call_command_manager;

                rt_method_t * rt_method = (rt_method_t *)method;
                if (__is_current_template(ctx, rt_method->host_type))
                {
                    rt_generic_method_t * rt_generic_method = ctx.to_generic_method(
                        rt_method, ctx.type
                    );

                    return __new_instance_call_command(ctx, rt_generic_method);
                }

                return __instance_call_command_manager.template
                    get_command<__CallCmd_(instance, general)>(rt_method, ctx);

            }   break;

            case rt_member_type_t::dynamic: {

                static __command_manager_t<
                    __instance_call_command_template_t<rt_dynamic_method_t>
                >::with_args_t<rt_dynamic_method_t *> __dynamic_call_command_manager;

                rt_dynamic_method_t * rt_dynamic_method = (rt_dynamic_method_t *)method;

                return __dynamic_call_command_manager.template
                    get_command<__CallCmd_(instance, dynamic)>(rt_dynamic_method, ctx);

            }   break;

            default:
                X_UNEXPECTED_F("unexpected member type '%1%'", method->this_type());
        }
    }

    static command_t * __new_instance_call_command(__context_t & ctx, const call_xil_t & xil)
    {
        ref_t method_ref = (ref_t)xil.method;
        rt_method_base_t * method = ctx.get_method(method_ref);

        return __new_instance_call_command(ctx, method);
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

    template<typename _fetch_t>
    __AlwaysInline exec_method_t * __get_virtual_exec_method(command_execute_context_t & ctx,
                                                msize_t this_offset, _fetch_t fetch_func)
    {
        rt_ref_t obj = *(rt_ref_t *)(ctx.stack.top() - this_offset);
        if (obj == nullptr)
            return nullptr;

        rt_type_t * rt_type = __RtTypeOf(obj);

        rt_vtable_t * vtbl = get_vtable(rt_type);
        rt_vfunction_t func = fetch_func(vtbl);

        if (!func.initialized())
        {
            rt_assembly_t * rt_assembly = ctx.env.assemblies.at(func.assembly_idx);
            assembly_analyzer_t analyzer = __assembly_analyzer(ctx, rt_assembly);
            rt_method_base_t * rt_method = analyzer.get_method(ref_t(func.method_idx));

            if (rt_type->get_kind() == rt_type_kind_t::generic)
            {
                rt_generic_method_t * m = analyzer.to_generic_method(
                    _M(rt_method_t *, rt_method), rt_type
                );
                func.method = ctx.env.exec_method_of(m);
            }
            else
            {
                func.method = ctx.env.exec_method_of(rt_method);
            }
        }
       
        return (exec_method_t *)func.method;
    }

    // Normal virtual call command.
    class __virtual_call_command_t : public __command_base_t<__CallCmd(virtual_)>
    {
        typedef __virtual_call_command_t                __self_t;
        typedef __command_base_t<__CallCmd(virtual_)>   __super_t;

    public:
        __virtual_call_command_t(msize_t virtual_offset, msize_t this_offset)
            : __virtual_offset(virtual_offset), __this_offset(this_offset) { }

        __BeginExecute(ctx)

            exec_method_t * method = __get_virtual_exec_method(ctx, __This->__this_offset,
                [offset=__This->__virtual_offset](rt_vtable_t * vtbl) {
                    return vtbl->functions[offset];
                }
            );

            __CheckNullReference(ctx, method);
            ctx.push_calling(method);

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("call virtual [%1% %2%]"), __This->__virtual_offset, __This->__this_offset);
        __EndToString()

        #endif  // EXEC_TRACE

        msize_t __virtual_offset, __this_offset;
    };

    struct __virtual_call_command_template_t
    {
        template<command_value_t _cv, typename ... args_t>
        static auto new_command(memory_t * memory, msize_t virtual_offset, msize_t this_offset,
                                                                        __context_t & ctx)
        {
            static_assert(_cv == __CallCmd(virtual_), "invalid command value");

            typedef __virtual_call_command_t this_command_t;
            return __new_command<this_command_t>(memory, virtual_offset, this_offset);
        }
    };

    // Interface virtual call command.
    class __interface_call_command_t : public __command_base_t<__CallCmd(interface_)>
    {
        typedef __interface_call_command_t                __self_t;
        typedef __command_base_t<__CallCmd(interface_)>   __super_t;

    public:
        __interface_call_command_t(rt_type_t * interface_type, msize_t virtual_offset,
                                                               msize_t this_offset)
            : __interface_type(interface_type), __virtual_offset(virtual_offset),
                                                __this_offset(this_offset)
        { }

        __BeginExecute(ctx)

            exec_method_t * method = __get_virtual_exec_method(ctx, __This->__this_offset,
                [o=__This->__virtual_offset, t=__This->__interface_type] (rt_vtable_t * vtbl) {
                    return vtbl->search_vtable_interface_function(t, o); 
                }
            );

            __CheckNullReference(ctx, method);
            ctx.push_calling(method);

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("call interface [%1% %2%]"), __This->__virtual_offset,
                                                      __This->__this_offset);
        __EndToString()

        #endif  // EXEC_TRACE

        rt_type_t * __interface_type;
        msize_t __virtual_offset, __this_offset;
    };

    struct __interface_call_command_template_t
    {
        template<command_value_t _cv, typename ... args_t>
        static auto new_command(memory_t * memory, rt_type_t * interface_type,
                                msize_t virtual_offset, msize_t this_offset, __context_t & ctx)
        {
            static_assert(_cv == __CallCmd(interface_), "invalid command value");

            typedef __interface_call_command_t this_command_t;
            return __new_command<this_command_t>(memory, interface_type,
                                                         virtual_offset, this_offset);
        }
    };

    static command_t * __new_virtual_call_command(__context_t & ctx, const call_xil_t & xil)
    {
        ref_t method_ref = (ref_t)xil.method;
        rt_method_base_t * method = ctx.get_method(method_ref);
        _A(method != nullptr);

        rt_type_t * host_type = method->get_host_type();
        _A(host_type != nullptr);

        exec_method_t * exec_method = ctx.env.exec_method_of(method);
        msize_t this_offset = exec_method->this_offset;

        // Interface virtual call.
        if (host_type->get_ttype(ctx.env) == ttype_t::interface_)
        {
            static __command_manager_t<
                __interface_call_command_template_t
            >::with_args_t<rt_type_t *, msize_t> __interface_call_command_manager;

            msize_t virtual_offset = ctx.get_virtual_method_offset(method_ref);

            return __interface_call_command_manager.template
                get_command<__CallCmd(interface_)>(host_type, virtual_offset, this_offset, ctx);
        }
        else    // Normal virtual call.
        {
            static __command_manager_t<
                __virtual_call_command_template_t
            >::with_args_t<msize_t, msize_t> __virtual_call_command_manager;

            msize_t virtual_offset = ctx.get_virtual_method_offset(method_ref);

            return __virtual_call_command_manager.template
                get_command<__CallCmd(virtual_)>(virtual_offset, this_offset, ctx);
        }
    }

    //-------- ---------- ---------- ---------- ----------
    // Delegate call command.

    static msize_t __delegate_method_field_offset       = unknown_msize;
    static msize_t __delegate_instance_field_offset     = unknown_msize;

    static msize_t __delegate_method_argument_offset    = unknown_msize;
    static msize_t __delegate_instance_argument_offset  = unknown_msize;

    // Used in init with type.
    static msize_t __delegate_method_argument_offset2   = unknown_msize;
    static msize_t __delegate_instance_argument_offset2 = unknown_msize;
    static msize_t __delegate_call_type_argument_offset2 = unknown_msize;

    template<command_value_t _cv>
    class __delegate_call_command_base_t : public __command_base_t<_cv>
    {
        typedef __delegate_call_command_base_t  __self_t;
        typedef __command_base_t<_cv>           __super_t;

    public:
        __delegate_call_command_base_t(msize_t this_offset)
            : __this_offset(this_offset + __stack_stub_size)
        { }

    protected:
        const msize_t __this_offset;

        typedef command_execute_context_t       __ctx_t;

        rt_object_t * __get_self(__ctx_t & ctx) _NE
        {
            return __Argument(rt_object_t *, __This->__this_offset);
        }

        void __set_self(__ctx_t & ctx, rt_object_t * self) _NE
        {
            __Argument(rt_object_t *, __This->__this_offset) = self;
        }

        rt_object_t * __get_instance(__ctx_t & ctx, rt_object_t * self) _NE
        {
            return __Field(rt_object_t *, self, __delegate_instance_field_offset);
        }

        void __set_instance(__ctx_t & ctx, rt_object_t * self, rt_object_t * instance) _NE
        {
            __Field(rt_object_t *, self, __delegate_instance_field_offset) = instance;
        }

        rt_method_base_t * __get_method(__ctx_t & ctx, rt_object_t * self) _NE
        {
            return __Field(rt_method_base_t *, self, __delegate_method_field_offset);
        }

        void __set_method(__ctx_t & ctx, rt_object_t * self, rt_method_base_t * method) _NE
        {
            __Field(rt_method_base_t *, self, __delegate_method_field_offset) = method;
        }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -
    // delegate invoke init command.

    template<command_value_t _cv> class __delegate_call_command_t { };

    template<>
    class __delegate_call_command_t<__CallCmd(delegate_init)>
        : public __delegate_call_command_base_t<__CallCmd(delegate_init)>
    {
        typedef __delegate_call_command_t       __self_t;
        typedef __delegate_call_command_base_t<__CallCmd(delegate_init)>  __super_t;

    public:
        using __super_t::__super_t;

        __BeginExecute(ctx)

            rt_object_t * self = __This->__get_self(ctx);

            __This->__set_instance(ctx, self, __get_instance_argument(ctx));
            __This->__set_method(ctx, self, __get_method_argument(ctx));

        __EndExecute()

        #if EXEC_TRACE

        __ToString(_T("call delegate init"));

        #endif  // EXEC_TRACE

    private:

        rt_object_t * __get_instance_argument(__ctx_t & ctx) _NE
        {
            return __Argument(rt_object_t *, __delegate_instance_argument_offset);
        }

        rt_method_base_t * __get_method_argument(__ctx_t & ctx) _NE
        {
            return __Argument(rt_method_base_t *, __delegate_method_argument_offset);
        }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -
    // delegate invoke init with call type command.

    template<>
    class __delegate_call_command_t<__CallCmd(delegate_init_with_call_type)>
        : public __delegate_call_command_base_t<__CallCmd(delegate_init_with_call_type)>
    {
        typedef __delegate_call_command_t       __self_t;
        typedef __delegate_call_command_base_t<__CallCmd(delegate_init_with_call_type)> 
                                                __super_t;
    public:
        using __super_t::__super_t;

        __BeginExecute(ctx)

            rt_object_t * self = __This->__get_self(ctx);

            __This->__set_instance(ctx, self, __get_instance_argument(ctx));
            __This->__set_method(ctx, self,
                al::incorp(__get_method_argument(ctx), __get_call_type_argument(ctx))
            );

        __EndExecute()

        #if EXEC_TRACE

        __ToString(_T("call delegate init (with call type)"));

        #endif  // EXEC_TRACE

    private:

        rt_object_t * __get_instance_argument(__ctx_t & ctx) _NE
        {
            return __Argument(rt_object_t *, __delegate_instance_argument_offset2);
        }

        rt_method_base_t * __get_method_argument(__ctx_t & ctx) _NE
        {
            return __Argument(rt_method_base_t *, __delegate_method_argument_offset2);
        }

        int32_t __get_call_type_argument(__ctx_t & ctx) _NE
        {
            return __Argument(int32_t, __delegate_call_type_argument_offset2);
        }
    };


    // - - - - - - - - - - - - - - - - - - - - - - - - - -
    // delegate invoke call command.

    template<>
    class __delegate_call_command_t<__CallCmd(delegate_invoke)>
        : public __delegate_call_command_base_t<__CallCmd(delegate_invoke)>
    {
        typedef __delegate_call_command_t       __self_t;
        typedef __delegate_call_command_base_t<__CallCmd(delegate_invoke)>  __super_t;

    public:
        using __super_t::__super_t;

        __BeginExecute(ctx)

            rt_object_t * self = __This->__get_self(ctx);
            rt_object_t * object = __This->__get_instance(ctx, self);
            rt_method_base_t * method = __This->__get_method(ctx, self);

            xil_call_type_t call_type = (xil_call_type_t)al::incorp_v<int32_t>(method);
            method = al::incorp_p(method);

            // _P(_T("call delegate"), method->get_name());

            exec_method_t * exec_method = ctx.env.exec_method_of(method);

            __Argument(rt_object_t *, __this_offset) = object;
            ctx.switch_to(exec_method);

        __EndExecute()

        #if EXEC_TRACE

        __ToString(_T("call delegate invoke"));

        #endif  // EXEC_TRACE

    };


    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    struct __delegate_command_template_t
    {
        template<command_value_t _cv, typename ... args_t>
        static auto new_command(memory_t * memory, msize_t this_offset, xil_call_command_t command)
        {
            typedef __delegate_call_command_t<_cv> this_command_t;
            return __new_command<this_command_t>(memory, this_offset);
        }
    };

    template<command_value_t _cv>
    command_t * __new_delegate_command(__context_t & ctx, const call_xil_t & xil)
    {
        static __command_manager_t<
            __delegate_command_template_t
        >::with_args_t<msize_t, xil_call_command_t> __delegate_command_manager;

        msize_t this_offset = __offset_of_argument(ctx, 0);
        return __delegate_command_manager.template get_command<_cv>(this_offset, xil.command);
    }

    // Creates command delegate call command.
    static command_t * __new_command_delegate_call_command(__context_t & ctx,
                                                            const call_xil_t & xil)
    {
        #define __InitOffset(_name, _identity)                                          \
            _name = __offset_of_argument(ctx, _identity) + __stack_stub_size;

        #define __IsOffsetEmpty(_name) (_name == unknown_msize)

        if (__IsOffsetEmpty(__delegate_method_field_offset))
        {
            rt_generic_type_t * delegate_type = _M(rt_generic_type_t *, ctx.type);

            delegate_type->each_field(ctx.env, [&](ref_t field_ref, rt_field_base_t * rt_field) {

                #define __ThisFieldOffset (delegate_type->get_field_offset(ctx.env, field_ref))
                #define __RtSidOf(_name) ctx.to_sid(_name)

                auto name = rt_field->get_field_type(ctx, delegate_type)->get_name(ctx.env);
                if (name == __RtSidOf(_T("Method")))
                    __delegate_method_field_offset = __ThisFieldOffset;
                else if (name == __RtSidOf(_T("Object")))
                    __delegate_instance_field_offset = __ThisFieldOffset;

                #undef __RtSidOf
                #undef __ThisFieldOffset

                return true;
            });
        }

        switch (xil.command)
        {
            case xil_call_command_t::delegate_init:

                if (__IsOffsetEmpty(__delegate_instance_argument_offset))
                {
                    __InitOffset(__delegate_instance_argument_offset, 1);
                    __InitOffset(__delegate_method_argument_offset, 2);
                }

                return __new_delegate_command<__CallCmd(delegate_init)>(ctx, xil);

            case xil_call_command_t::delegate_init_with_call_type:

                if (__IsOffsetEmpty(__delegate_instance_argument_offset2))
                {
                    __InitOffset(__delegate_instance_argument_offset2, 1);
                    __InitOffset(__delegate_method_argument_offset2, 2);
                    __InitOffset(__delegate_call_type_argument_offset2, 3);
                }

                return __new_delegate_command<__CallCmd(delegate_init_with_call_type)>(ctx, xil);

            case xil_call_command_t::delegate_invoke:
                return __new_delegate_command<__CallCmd(delegate_invoke)>(ctx, xil);

            default:
                X_UNEXPECTED_F("unexpected xil command command '%1%'", xil.command);
        }

        #undef __IsOffsetEmpty
        #undef __InitOffset
    }

    //-------- ---------- ---------- ---------- ----------

    // Creates command call command.
    static command_t * __new_command_call_command(__context_t & ctx, const call_xil_t & xil)
    {
        switch (xil.command)
        {
            case xil_call_command_t::delegate_init:
            case xil_call_command_t::delegate_init_with_call_type:
            case xil_call_command_t::delegate_invoke:
                return __new_command_delegate_call_command(ctx, xil);

            default:
                X_UNEXPECTED_F("unexpected xil command command '%1%'", xil.command);
        }
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

            case xil_call_type_t::command:
                return __new_command_call_command(ctx, xil);

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

    #define __BinaryCmd(_cmd, _op, _xt1, _xt2)  __Cmd(_cmd, _op##_##_xt1##_##_xt2)
    #define __UnitaryCmd(_cmd, _op, _xt)        __Cmd(_cmd, _op##_##_xt)

    // Binary operators defination.
    #define __CaseBinary1(_op, _xt1, _xt2)   __CaseBinary(_op, _xt1, _xt2)

    #define __CaseBinary2(_op, _xt1, _xt2)                                              \
        __CaseBinary1(_op, _xt1, _xt2)                                                  \
        __CaseBinary1(_op, _xt2, _xt1)

    #define __CaseBinaries(_op)                                                         \
        __CaseBinary1(_op, int8,   int8)                                                \
        __CaseBinary2(_op, int8,   uint8)                                               \
                                                                                        \
        __CaseBinary1(_op, uint8,  uint8)                                               \
                                                                                        \
        __CaseBinary2(_op, int16,  int8)                                                \
        __CaseBinary2(_op, int16,  uint8)                                               \
        __CaseBinary1(_op, int16,  int16)                                               \
        __CaseBinary2(_op, int16,  uint16)                                              \
                                                                                        \
        __CaseBinary2(_op, uint16, int8)                                                \
        __CaseBinary2(_op, uint16, uint8)                                               \
        __CaseBinary1(_op, uint16, uint16)                                              \
                                                                                        \
        __CaseBinary2(_op, int32,  int8)                                                \
        __CaseBinary2(_op, int32,  uint8)                                               \
        __CaseBinary2(_op, int32,  int16)                                               \
        __CaseBinary2(_op, int32,  uint16)                                              \
        __CaseBinary1(_op, int32,  int32)                                               \
                                                                                        \
        __CaseBinary2(_op, uint32, int8)                                                \
        __CaseBinary2(_op, uint32, uint8)                                               \
        __CaseBinary2(_op, uint32, int16)                                               \
        __CaseBinary2(_op, uint32, uint16)                                              \
        __CaseBinary2(_op, uint32, int32)                                               \
        __CaseBinary1(_op, uint32, uint32)                                              \
                                                                                        \
        __CaseBinary2(_op, int64,  int8)                                                \
        __CaseBinary2(_op, int64,  uint8)                                               \
        __CaseBinary2(_op, int64,  int16)                                               \
        __CaseBinary2(_op, int64,  uint16)                                              \
        __CaseBinary2(_op, int64,  int32)                                               \
        __CaseBinary2(_op, int64,  uint32)                                              \
        __CaseBinary1(_op, int64,  int64)                                               \
                                                                                        \
        __CaseBinary2(_op, uint64,  int8)                                               \
        __CaseBinary2(_op, uint64,  uint8)                                              \
        __CaseBinary2(_op, uint64,  int16)                                              \
        __CaseBinary2(_op, uint64,  uint16)                                             \
        __CaseBinary2(_op, uint64,  int32)                                              \
        __CaseBinary2(_op, uint64,  uint32)                                             \
        __CaseBinary2(_op, uint64,  int64)                                              \
        __CaseBinary1(_op, uint64,  uint64)

    #define __CaseFloatBinaries(_op)                                                    \
        __CaseBinary2(_op, float_,  int8)                                               \
        __CaseBinary2(_op, float_,  uint8)                                              \
        __CaseBinary2(_op, float_,  int16)                                              \
        __CaseBinary2(_op, float_,  uint16)                                             \
        __CaseBinary2(_op, float_,  int32)                                              \
        __CaseBinary2(_op, float_,  uint32)                                             \
        __CaseBinary2(_op, float_,  int64)                                              \
        __CaseBinary1(_op, float_,  uint64)                                             \
        __CaseBinary1(_op, float_,  float_)                                             \
                                                                                        \
        __CaseBinary2(_op, double_,  int8)                                              \
        __CaseBinary2(_op, double_,  uint8)                                             \
        __CaseBinary2(_op, double_,  int16)                                             \
        __CaseBinary2(_op, double_,  uint16)                                            \
        __CaseBinary2(_op, double_,  int32)                                             \
        __CaseBinary2(_op, double_,  uint32)                                            \
        __CaseBinary2(_op, double_,  int64)                                             \
        __CaseBinary1(_op, double_,  uint64)                                            \
        __CaseBinary2(_op, double_,  float_)                                            \
        __CaseBinary1(_op, double_,  double_)

        // Unitary operators defination.
        #define __C(_op, _xt)    __CaseUnitary(_op, _xt)

        #define __CaseUnitaries(_op)                                                    \
            __CaseUnitary(_op, int8)                                                    \
            __CaseUnitary(_op, uint8)                                                   \
            __CaseUnitary(_op, int16)                                                   \
            __CaseUnitary(_op, uint16)                                                  \
            __CaseUnitary(_op, int32)                                                   \
            __CaseUnitary(_op, uint32)                                                  \
            __CaseUnitary(_op, int64)                                                   \
            __CaseUnitary(_op, uint64)

        #define __CaseFloatUnitaries(_op)                                               \
            __CaseUnitary(_op, float_)                                                  \
            __CaseUnitary(_op, double_)


    template<command_value_t _cv, typename _op_t, _op_t _op, xil_type_t _xt1, xil_type_t _xt2>
    class __binary_command_t : public __command_base_t<_cv>
    {
        typedef __binary_command_t  __self_t;

    public:
        __BeginExecute(ctx)

            const vtype_t vtype1 = to_vtype(_xt1);
            const vtype_t vtype2 = to_vtype(_xt2);

            typedef vnum_t<vtype1> t1;
            typedef vnum_t<vtype2> t2;

            const msize_t unit_size1 = unit_size_of(get_vtype_size(vtype1));
            const msize_t unit_size2 = unit_size_of(get_vtype_size(vtype2));

            rt_stack_unit_t * top = ctx.stack.pop<unit_size1 + unit_size2>();

            ctx.stack.push(
                binary_operator_t<to_operator(_op), vtype1, vtype2>()(
                    *(t1 *)top, *(t2 *)(top + unit_size1)
                )
            );

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("%|1$-4| %2% %3%"), _str(_op), _xt1, _xt2);
        __EndToString()

        #endif  // EXEC_TRACE
    };

    #define __CaseBinary(_op, _xt1, _xt2)   __DefineCmdValue_(__BinaryCmd(al, _op, _xt1, _xt2))

    __CaseBinaries(add)
    __CaseFloatBinaries(add)

    __CaseBinaries(sub)
    __CaseFloatBinaries(sub)

    __CaseBinaries(mul)
    __CaseFloatBinaries(mul)

    __CaseBinaries(div)
    __CaseFloatBinaries(div)

    __CaseBinaries(mod)

    #undef __CaseBinary

    #define __CaseBinary(_op, _xt1, _xt2)   __DefineCmdValue_(__BinaryCmd(bit, _op, _xt1, _xt2))

    __CaseBinaries(bit_and)
    __CaseBinaries(bit_or)
    __CaseBinaries(bit_xor)
    __CaseBinaries(left_shift)
    __CaseBinaries(right_shift)

    #undef __CaseBinary

    __DefineCmdValue_(__BinaryCmd(logic, and_, bool_, bool_))
    __DefineCmdValue_(__BinaryCmd(logic, or_, bool_, bool_))
    __DefineCmdValue_(__UnitaryCmd(logic, not_, bool_))

    #define __CaseBinary(_op, _xt1, _xt2)   __DefineCmdValue_(__BinaryCmd(cmp, _op, _xt1, _xt2))

    __CaseBinaries(greater)
    __CaseFloatBinaries(greater)
    __CaseBinaries(greater_equal)
    __CaseFloatBinaries(greater_equal)
    __CaseBinaries(less)
    __CaseFloatBinaries(less)
    __CaseBinaries(less_equal)
    __CaseFloatBinaries(less_equal)
    __CaseBinaries(equal)
    __CaseFloatBinaries(equal)
    __CaseBinaries(not_equal)
    __CaseFloatBinaries(not_equal);

    __CaseBinary(equal, bool_, bool_)
    __CaseBinary(not_equal, bool_, bool_)
    __CaseBinary(equal, char_, char_)
    __CaseBinary(not_equal, char_, char_)
    __CaseBinary(greater, char_, char_)
    __CaseBinary(greater_equal, char_, char_)
    __CaseBinary(less, char_, char_)
    __CaseBinary(less_equal, char_, char_)

    __CaseBinary(equal, object, object)
    __CaseBinary(not_equal, object, object)

    __CaseBinary(equal, ptr, ptr)
    __CaseBinary(not_equal, ptr, ptr)

    #undef __CaseBinary


    //-------- ---------- ---------- ---------- ----------

    template<typename _op_t>
    struct __binary_command_template_t
    {
        template<command_value_t _cv, _op_t _op,
            xil_type_t _xt1, xil_type_t _xt2, typename ... _args_t>
        static auto new_command(memory_t * memory, _args_t && ... args)
        {
            typedef __binary_command_t<_cv, _op_t, _op, _xt1, _xt2> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<_args_t>(args) ...);
        }
    };

    template<typename _op_t>
    using __binary_command_manager_t = typename __command_manager_t<
        __binary_command_template_t<_op_t>, _op_t, xil_type_t, xil_type_t
    >::template with_args_t<>;

    //-------- ---------- ---------- ---------- ----------

    template<command_value_t _cv, typename _op_t, _op_t _op, xil_type_t _xt>
    class __unitary_command_t : public __command_base_t<_cv>
    {
        typedef __unitary_command_t     __self_t;

    public:
        __BeginExecute(ctx)

            const vtype_t vtype = to_vtype(_xt);

            typedef vnum_t<vtype> t;
            t v = ctx.stack.pop<t>();

            ctx.stack.push(
                unitary_operator_t<to_operator(_op), vtype>()(v)
            );

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("%1% %2%"), _str(_op), _xt);
        __EndToString()

        #endif  // EXEC_TRACE
    };


    #define __CaseUnitary(_op, _xt)     __DefineCmdValue_(__UnitaryCmd(al, _op, _xt))

    __CaseUnitaries(minus)
    __CaseFloatUnitaries(minus)

    __CaseUnitaries(positive)
    __CaseFloatUnitaries(positive)

    #undef __CaseUnitary


    #define __CaseUnitary(_op, _xt)     __DefineCmdValue_(__UnitaryCmd(bit, _op, _xt))

    __CaseUnitaries(bit_not)

    #undef __CaseUnitary

    //-------- ---------- ---------- ---------- ----------

    template<typename _op_t>
    struct __unitary_command_template_t
    {
        template<command_value_t _cv, _op_t _op, xil_type_t _xt, typename ... args_t>
        static auto new_command(memory_t * memory, args_t && ... args)
        {
            typedef __unitary_command_t<_cv, _op_t, _op, _xt> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<args_t>(args) ...);
        }
    };

    template<typename _op_t>
    using __unitary_command_manager_t = typename __command_manager_t<
        __unitary_command_template_t<_op_t>, _op_t, xil_type_t
    >::template with_args_t<>;

    //-------- ---------- ---------- ---------- ----------

    static command_t * __new_al_command(__context_t & ctx, const al_xil_t & xil)
    {
        static __binary_command_manager_t<xil_al_command_t> __binary_command_manager;
        static __unitary_command_manager_t<xil_al_command_t> __unitary_command_manager;

        #define __V(_op, _xt1, _xt2)                                                    \
            (((uint32_t)(_op) << 16) | ((uint32_t)(_xt1) << 8) | (uint32_t)(_xt2))

        switch (__V(xil.cmd(), xil.dtype1(), xil.dtype2()))
        {
            #define __CaseBinary(_op, _xt1, _xt2)                                       \
                case __V(xil_al_command_t::_op, xil_type_t::_xt1, xil_type_t::_xt2):    \
                    return __binary_command_manager.template get_command<               \
                        __BinaryCmd(al, _op, _xt1, _xt2),                               \
                        xil_al_command_t::_op, xil_type_t::_xt1, xil_type_t::_xt2       \
                    >();

            __CaseBinaries(add)
            __CaseFloatBinaries(add)

            __CaseBinaries(sub)
            __CaseFloatBinaries(sub)

            __CaseBinaries(mul)
            __CaseFloatBinaries(mul)

            __CaseBinaries(div)
            __CaseFloatBinaries(div)

            __CaseBinaries(mod)

            #undef __CaseBinary

            // - - - - - - - - - - - - - - - - - - - - - - - - - -

            #define __CaseUnitary(_op, _xt)                                             \
                case __V(xil_al_command_t::_op, xil_type_t::_xt, xil_type_t::__default__):  \
                    return __unitary_command_manager.template get_command<              \
                        __UnitaryCmd(al, _op, _xt),                                     \
                        xil_al_command_t::_op, xil_type_t::_xt                          \
                    >();

            __CaseUnitaries(minus)
            __CaseFloatUnitaries(minus)

            __CaseUnitaries(positive)
            __CaseFloatUnitaries(positive)

            #undef __CaseUnitary

            default:
                _P(xil.cmd(), xil.dtype1(), xil.dtype2());
                X_UNEXPECTED();
        }

        return nullptr;
        X_UNEXPECTED();

        #undef __V
    }

    ////////// ////////// ////////// ////////// //////////

    static command_t * __new_bit_command(__context_t & ctx, const bit_xil_t & xil)
    {
        static __binary_command_manager_t<xil_bit_command_t> __binary_command_manager;
        static __unitary_command_manager_t<xil_bit_command_t> __unitary_command_manager;

        #define __V(_op, _xt1, _xt2)                                                    \
            (((uint32_t)(_op) << 16) | ((uint32_t)(_xt1) << 8) | (uint32_t)(_xt2))

        switch (__V(xil.cmd(), xil.dtype1(), xil.dtype2()))
        {
            #define __CaseBinary(_op, _xt1, _xt2)                                       \
                case __V(xil_bit_command_t::_op, xil_type_t::_xt1, xil_type_t::_xt2):   \
                    return __binary_command_manager.template get_command<               \
                        __BinaryCmd(bit, _op, _xt1, _xt2),                              \
                        xil_bit_command_t::_op, xil_type_t::_xt1, xil_type_t::_xt2      \
                    >();

            __CaseBinaries(bit_and)
            __CaseBinaries(bit_or)
            __CaseBinaries(bit_xor)
            __CaseBinaries(left_shift)
            __CaseBinaries(right_shift)

            #undef __CaseBinary

            // - - - - - - - - - - - - - - - - - - - - - - - - - -

            #define __CaseUnitary(_op, _xt)                                             \
                case __V(xil_bit_command_t::_op, xil_type_t::_xt, xil_type_t::__default__):  \
                    return __unitary_command_manager.template get_command<              \
                        __UnitaryCmd(bit, _op, _xt),                                    \
                        xil_bit_command_t::_op, xil_type_t::_xt                         \
                    >();

            __CaseUnitaries(bit_not)

            #undef __CaseUnitary

            default:
                X_UNEXPECTED();
        }

        #undef __V
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
                    __BinaryCmd(logic, and_, bool_, bool_),
                    xil_logic_command_t::and_, xil_type_t::bool_, xil_type_t::bool_
                >();

            case xil_logic_command_t::or_:
                return __binary_command_manager.template get_command<
                    __BinaryCmd(logic, or_, bool_, bool_),
                    xil_logic_command_t::or_, xil_type_t::bool_, xil_type_t::bool_
                >();

            case xil_logic_command_t::not_:
                return __unitary_command_manager.template get_command<
                    __UnitaryCmd(logic, not_, bool_),
                    xil_logic_command_t::not_, xil_type_t::bool_
                >();

            default:
                X_UNEXPECTED();
        }

        X_UNEXPECTED();
    }

    ////////// ////////// ////////// ////////// //////////

    static command_t * __new_cmp_command(__context_t & ctx, const cmp_xil_t & xil)
    {
        static __binary_command_manager_t<xil_cmp_command_t> __command_manager;

        #define __V(_op, _xt1, _xt2)                                                    \
            (((uint32_t)(_op) << 16) | ((uint32_t)(_xt1) << 8) | (uint32_t)(_xt2))

        switch (__V(xil.cmd(), xil.dtype1(), xil.dtype2()))
        {
            #define __CaseBinary(_op, _xt1, _xt2)                                       \
                case __V(xil_cmp_command_t::_op, xil_type_t::_xt1, xil_type_t::_xt2):   \
                    return __command_manager.template get_command<                      \
                        __BinaryCmd(cmp, _op, _xt1, _xt2),                              \
                        xil_cmp_command_t::_op, xil_type_t::_xt1, xil_type_t::_xt2      \
                    >();

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

            __CaseBinary(equal, bool_, bool_)
            __CaseBinary(not_equal, bool_, bool_)
            __CaseBinary(equal, char_, char_)
            __CaseBinary(not_equal, char_, char_)
            __CaseBinary(greater, char_, char_)
            __CaseBinary(greater_equal, char_, char_)
            __CaseBinary(less, char_, char_)
            __CaseBinary(less_equal, char_, char_)

            __CaseBinary(equal, object, object)
            __CaseBinary(not_equal, object, object)

            __CaseBinary(equal, ptr, ptr)
            __CaseBinary(not_equal, ptr, ptr)

            #undef __CaseBinary

            default:
                _P(xil.dtype1(), xil.dtype2());
                X_UNEXPECTED();
        }

        #undef __V
    }

    namespace
    {
        template<typename t, size_t size> struct __copy_array_t
        {
            __AlwaysInline static void copy_array(const t * src, t * dst)
            {
                *dst = *src;
                __copy_array_t<t, size - 1>::copy_array(src + 1, dst + 1);
            }
        };

        template<typename t> struct __copy_array_t<t, 0>
        {
            __AlwaysInline static void copy_array(const t * src, t * dst) { }
        };
    }

    template<size_t size, typename t> void copy_array(const t * src, t * dst)
    {
        __copy_array_t<t, size>::copy_array(src, dst);
    }

    ////////// ////////// ////////// ////////// //////////

    #define __CastDefaultCmd        __Cmd(cast, default_)
    #define __CastIsCmd             __Cmd(cast, is)
    #define __CastAsCmd             __Cmd(cast, as)

    __DefineCmdValue_(__CastDefaultCmd)
    __DefineCmdValue_(__CastIsCmd)
    __DefineCmdValue_(__CastAsCmd)

    template<bool _to_type_is_interface>
    bool __cast_test(rt_type_t * from_type, rt_type_t * to_type)
    {
        rt_vtable_t * vtbl = get_vtable(from_type);
        if constexpr (_to_type_is_interface)
        {
            return vtbl->try_search_vtable_interface(to_type);
        }
        else
        {
            if (from_type == to_type)
                return true;

            return vtbl->is_base_type(to_type);
        }
    }

    template<command_value_t _cv, xil_cast_command_t _cast_cmd, bool _is_interface>
    class __cast_command_t : public __command_base_t<_cv>
    {
        typedef __cast_command_t        __self_t;
        typedef __command_base_t<_cv>   __super_t;

    public:
        #if EXEC_TRACE
        __cast_command_t(rt_type_t * type, rt_sid_t type_name)
            : __type(type), __type_name(type_name) { }
        #else
        __cast_command_t(rt_type_t * type): __type(type) { }
        #endif

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("%1% %2%"), _cast_cmd, __This->__type_name);
        __EndToString()

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            // Default cast, raise exception when failed.
            if constexpr (_cast_cmd == xil_cast_command_t::default_)
            {
                rt_ref_t obj = ctx.stack.pick<rt_ref_t>();
                if (obj != nullptr)
                {
                    rt_type_t * rt_type = __RtTypeOf(obj);
                    if (!__cast_test<_is_interface>(rt_type, __type))
                        __RaiseInvalidCastException(ctx);
                }
            }

            // Test whether a cast is valid.
            else if constexpr (_cast_cmd == xil_cast_command_t::is)
            {
                rt_ref_t obj = ctx.stack.pick<rt_ref_t>();
                if (obj != nullptr)
                {
                    rt_type_t * rt_type = __RtTypeOf(obj);
                    ctx.stack.replace(__cast_test<_is_interface>(rt_type, __type));
                }
                else
                {
                    ctx.stack.replace(false);
                }
            }

            // Returns null when cast is invalid.
            else if constexpr (_cast_cmd == xil_cast_command_t::as)
            {
                rt_ref_t obj = ctx.stack.pick<rt_ref_t>();
                if (obj != nullptr)
                {
                    rt_type_t * rt_type = __RtTypeOf(obj);

                    if (!__cast_test<_is_interface>(rt_type, __type))
                        ctx.stack.replace_to_null();
                }
            }

        __EndExecute()

    private:
        rt_type_t *     __type;

        #if EXEC_TRACE
        rt_sid_t        __type_name;
        #endif
    };

    template<xil_cast_command_t _cast_cmd>
    struct __cast_command_template_t
    {
        template<command_value_t _cv, typename ... _args_t>
        static auto new_command(memory_t * memory, bool is_interface, rt_type_t * type,
                                                    _args_t ... args) -> command_t *
        {
            if (is_interface)
            {
                typedef __cast_command_t<_cv, _cast_cmd, true> this_command_t;
                return __new_command<this_command_t>(memory, type, std::forward<_args_t>(args) ...);
            }
            else
            {
                typedef __cast_command_t<_cv, _cast_cmd, false> this_command_t;
                return __new_command<this_command_t>(memory, type, std::forward<_args_t>(args) ...);
            }
        }
    };

    // Creates a cast command.
    static command_t * __new_cast_command(__context_t & ctx, const cast_xil_t & xil)
    {
        rt_type_t * type = ctx.get_type(xil.type_ref());
        bool is_interface = (type->get_ttype(ctx.env) == ttype_t::interface_);

        switch (xil.cmd())
        {
            case xil_cast_command_t::default_: {

                static __command_manager_t<
                    __cast_command_template_t<xil_cast_command_t::default_>
                >::with_args_t<bool, rt_type_t *> __manager;

                return __manager.template get_command<__CastDefaultCmd>(is_interface, type
                #if EXEC_TRACE
                    , type->get_name(ctx.env)
                #endif
                );

            }   break;

            case xil_cast_command_t::is: {

                static __command_manager_t<
                    __cast_command_template_t<xil_cast_command_t::is>
                >::with_args_t<bool, rt_type_t *> __manager;

                return __manager.template get_command<__CastIsCmd>(is_interface, type
                #if EXEC_TRACE
                    , type->get_name(ctx.env)
                #endif
                );

            }   break;

            case xil_cast_command_t::as: {

                static __command_manager_t<
                    __cast_command_template_t<xil_cast_command_t::as>
                >::with_args_t<bool, rt_type_t *> __manager;

                return __manager.template get_command<__CastAsCmd>(is_interface, type
                #if EXEC_TRACE
                    , type->get_name(ctx.env)
                #endif
                );

            }   break;

            default:
                X_UNEXPECTED(_T("unknown cast command"));
        }
    }

    ////////// ////////// ////////// ////////// //////////

    #define __RetCmd(_ret_size)     __Cmd(smp, ret_##_ret_size)
    #define __StructRetUnitSize     65535
    #define __StructRetCmd          __RetCmd(65535)

    __DefineCmdValue_(__RetCmd(0))
    __DefineCmdValue_(__RetCmd(1))
    __DefineCmdValue_(__StructRetCmd)

    #define __IsRetCmdValue(_cv)    ((_cv) >= __RetCmd(0) && (_cv) <= __LargeRetCmd)

    template<command_value_t _cv>
    class __ret_command_base_t : public __command_base_t<_cv> { };
    class __ret_command_identity_t { };

    template<command_value_t _cv, msize_t _ret_size>
    class __ret_command_t : public __ret_command_base_t<_cv>,
                            public __ret_command_identity_t
    {
        typedef __ret_command_t     __self_t;

    public:
        __ret_command_t(msize_t local_unit_size, msize_t param_unit_size)
            : __total_unit_size(param_unit_size + __stack_stub_size + local_unit_size)
            , __param_unit_ret_size(param_unit_size - _ret_size)
            , __total_unit_ret_size(__total_unit_size - _ret_size)
        {
            #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_MANUAL
            __ret_commands.insert(this);
            #endif  // EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_MANUAL
        }

        __BeginExecute(ctx)

            __calling_stub_t * stub = ctx.stub();
            rt_stack_unit_t * p = ctx.stack.pop(__This->__total_unit_size);
            ctx.pop_calling(stub);

            copy_array<_ret_size>(p + __This->__total_unit_ret_size, p - _ret_size);

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("ret  [%1%]"), _ret_size);
        __EndToString()

        #endif  // EXEC_TRACE

    private:
        int __total_unit_size;      // param_unit_size + __stack_stub_size + local_unit_size
        int __param_unit_ret_size;  // param_unit_size - _ret_size
        int __total_unit_ret_size;  // __total_unit_size - _ret_size
    };

    //-------- ---------- ---------- ---------- ----------

    template<>
    class __ret_command_t<__StructRetCmd, __StructRetUnitSize>
        : public __ret_command_base_t<__StructRetCmd>
        , public __ret_command_identity_t
    {
        typedef __ret_command_t     __self_t;

    public:
        __ret_command_t(msize_t local_unit_size, msize_t param_unit_size)
            : __total_unit_size(param_unit_size + __stack_stub_size + local_unit_size)
            , __param_unit_ret_size(param_unit_size)
        {
            #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_MANUAL
            __ret_commands.insert(this);
            #endif  // EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_MANUAL
        }

        __BeginExecute(ctx)

            __calling_stub_t * stub = ctx.stub();
            rt_stack_unit_t * p = ctx.stack.pop(__This->__total_unit_size);
            ctx.pop_calling(stub);

            ctx.stack.pop<void *>();    // pop the bottom struct object address.

        __EndExecute()

        #if EXEC_TRACE

        __ToString(_T("ret  [struct]"));

        #endif  // EXEC_TRACE

    private:
        msize_t __total_unit_size;      // param_unit_size + __stack_stub_size + local_unit_size
        msize_t __param_unit_ret_size;  // param_unit_size - _ret_size
    };

    //-------- ---------- ---------- ---------- ----------

    // Return command factory.
    struct __ret_command_template_t
    {
        template<msize_t _ret_size> struct __new_t
        {
            template<command_value_t _cv>
            static command_t * new_(memory_t * memory,
                            msize_t local_size, msize_t param_size, msize_t ret_size)
            {
                return __new_command<__ret_command_t<_cv, _ret_size>>(
                    memory, local_size, param_size
                );
            }
        };

        template<> struct __new_t<__StructRetUnitSize>
        {
            template<command_value_t _cv>
            static command_t * new_(memory_t * memory,
                            msize_t local_size, msize_t param_size, msize_t ret_size)
            {
                return __new_command<__ret_command_t<__StructRetCmd, __StructRetUnitSize>>(
                    memory, local_size, param_size
                );
            }
        };


        template<command_value_t _cv, msize_t _ret_size>
        static auto new_command(memory_t * memory,
                            msize_t local_size, msize_t param_size, msize_t ret_size)
        {
            return __new_t<_ret_size>::template new_<_cv>(
                memory, local_size, param_size, ret_size
            );
        }
    };

    // Creates a return command.
    command_t * new_ret_command(__context_t & ctx)
    {
        static __command_manager_t<
            __ret_command_template_t, msize_t
        >::with_args_t<msize_t, msize_t, msize_t> __ret_command_manager;

        msize_t ret_unit_size   = ctx.ret_unit_size();
        msize_t local_unit_size = ctx.locals_layout.unit_size();
        msize_t param_unit_size = ctx.params_layout.unit_size();

        // _PF(_T("local:%1% param:%2% ret:%3%"), local_unit_size, param_unit_size, ret_unit_size);

        rt_type_t * ret_type = ctx.method->get_return_type(ctx.env);

        if (is_custom_struct(ctx.env, ret_type))
        {
            return __ret_command_manager.template get_command<__StructRetCmd, __StructRetUnitSize>(
                local_unit_size, param_unit_size, 0
            );
        }

        switch (ret_unit_size)
        {
            #define __CaseRet(_size)                                                    \
                case _size:                                                             \
                    return __ret_command_manager.template                               \
                        get_command<__RetCmd(_size), _size>(                            \
                            local_unit_size, param_unit_size, ret_unit_size             \
                        );

            __CaseRet(0)
            __CaseRet(1)

            default:
                X_UNEXPECTED();


            #undef __CaseRet
        }

        X_UNEXPECTED();
    }

    // Returns if it's a return command.
    bool is_ret_command(command_t * command)
    {
        _A(command != nullptr);

    #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_MANUAL

        return __ret_commands.find(command) != __ret_commands.end();

    #elif EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_INLINE   // EXEC_EXECUTE_MODEL == xxx

        return __IsRetCmdValue(command->cmd_value);

    #elif EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL // EXEC_EXECUTE_MODEL == xxx

        return is<__ret_command_identity_t *>(command);

    #endif  // EXEC_EXECUTE_MODEL == xxx
    }

    //-------- ---------- ---------- ---------- ----------

    // Label command.

    #define __LabelCmd      __Cmd(smp, label)
    __DefineCmdValue_(__LabelCmd)

    class __label_command_t : public __command_base_t<__LabelCmd>
    {
        typedef __label_command_t   __self_t;

    public:
        __BeginExecute(ctx)

            // Do nothing

        __EndExecute()

        #if EXEC_TRACE

        __ToString(_T("label"));

        #endif  // EXEC_TRACE
    };

    //-------- ---------- ---------- ---------- ----------

    // Returns if a type is a base type of another type.
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
    __process_block_nodes_ret __process_block_nodes(command_execute_context_t & ctx)
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
    void __process_exception(command_execute_context_t & ctx)
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
    void __process_finally(command_execute_context_t & ctx,
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
    void __process_finally_for_return(command_execute_context_t & ctx,
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

    #define __ThrowCmd      __Cmd(smp, throw_)
    __DefineCmdValue_(__ThrowCmd)

    class __throw_command_t : public __command_base_t<__ThrowCmd>
    {
        typedef __throw_command_t   __self_t;

    public:
        __BeginExecute(ctx)

            rt_ref_t exception = ctx.stack.pop<rt_ref_t>();
            ctx.restore_stack();

            ctx.push_exception(exception);
            __process_exception(ctx);

        __EndExecute()

        #if EXEC_TRACE

        __ToString(_T("throw"))

        #endif  // EXEC_TRACE
    };

    //-------- ---------- ---------- ---------- ----------

    // Rethrow command.

    #define __RethrowCmd    __Cmd(smp, rethrow)
    __DefineCmdValue_(__RethrowCmd)

    class __rethrow_command_t : public __command_base_t<__RethrowCmd>
    {
        typedef __rethrow_command_t     __self_t;

    public:
        __BeginExecute(ctx)

            // Do nothing.

        __EndExecute()

        #if EXEC_TRACE

        __ToString(_T("rethrow"))

        #endif  // EXEC_TRACE
    };

    //-------- ---------- ---------- ---------- ----------

    // End finally command.

    #define __EndFinallyCmd     __Cmd(smp, end_finally)
    __DefineCmdValue_(__EndFinallyCmd)

    class __end_finally_command_t : public __command_base_t<__EndFinallyCmd>
    {
        typedef __end_finally_command_t     __self_t;

    public:

        // Execute.
        __BeginExecute(ctx)

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
                    #ifndef __NoThrow
                    X_UNEXPECTED()
                    #endif
                    ;
            }

            #ifndef __NoThrow
            X_UNEXPECTED();
            #endif

        __EndExecute()

        #if EXEC_TRACE

        __ToString(_T("end finally"));

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

    #define __JmpCmd(_model)     __Cmd(jmp, _model)

    __DefineCmdValue_(__JmpCmd(none))
    __DefineCmdValue_(__JmpCmd(true_))
    __DefineCmdValue_(__JmpCmd(false_))
    __DefineCmdValue_(__JmpCmd(switch_))
    __DefineCmdValue_(__JmpCmd(leave))
    __DefineCmdValue_(__JmpCmd(leave_ret))

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
            return !ctx.stack.pop<bool>();
        }

        operator string_t() const
        {
            return _T("jmp  false");
        }
    };

    //-------- ---------- ---------- ---------- ----------

    template<command_value_t _cv, xil_jmp_model_t _model>
    class __jmp_command_t : public __command_base_t<_cv>
    {
        typedef __jmp_command_t         __self_t;
        typedef __jmp_model_t<_model>   __jmp_model_t;

    public:
        __jmp_command_t(xil_jmp_step_t step) : __step(step) { }

        __BeginExecute(ctx)

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
    template<command_value_t _cv>
    class __jmp_command_t<_cv, xil_jmp_model_t::leave> : public __command_base_t<_cv>
    {
        typedef __jmp_command_t __self_t;

    public:
        __jmp_command_t(xil_jmp_step_t step) : __step(step) { }

        // Execute
        __BeginExecute(ctx)

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
        template<command_value_t _cv, xil_jmp_model_t _model, typename ... _args_t>
        static auto new_command(memory_t * memory, _args_t && ... args)
        {
            typedef __jmp_command_t<_cv, _model> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<_args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Leave and return.
    class __leave_ret_command_t : public __command_base_t<__JmpCmd(leave_ret)>
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
        __BeginExecute(ctx)

            __process_finally_for_return(ctx, __This->__following_command);

        __EndExecute()

        // To string

        #if EXEC_TRACE

        __ToString(_T("leave.ret"));

        #endif  // EXEC_TRACE

    private:
        command_t * __following_command;
    };

    //-------- ---------- ---------- ---------- ----------

    struct __leave_ret_command_template_t
    {
        template<command_value_t _cv, typename ... _args_t>
        static auto new_command(memory_t * memory, command_t * command, _args_t && ... args)
        {
            static_assert(_cv == __JmpCmd(leave), "invalid command value");

            typedef __leave_ret_command_t this_command_t;
            return __new_command<this_command_t>(memory, command, std::forward<_args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    class __switch_jmp_command_t : public __command_base_t<__JmpCmd(switch_)>
    {
        typedef __switch_jmp_command_t      __self_t;

    public:
        __switch_jmp_command_t(exec_switch_table_t * table) : __table(table) { }

        __BeginExecute(ctx)

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

        __ToString(_T("switch"));

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
            __leave_ret_command_template_t
        >::with_args_t<command_t *> __leave_ret_command_manager;

        typedef xil_jmp_model_t model_t;

        switch (xil.model())
        {
            #define __Case(_model)                                                      \
                case xil_jmp_model_t::_model:                                           \
                    return __jmp_command_manager.template get_command<                  \
                        __JmpCmd(_model), model_t::_model>(xil.step() - 1);

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
                    return __leave_ret_command_manager.template get_command<__JmpCmd(leave)>(
                        new_ret_command(ctx)
                    );
                }

                return __jmp_command_manager.template
                    get_command<__JmpCmd(leave), model_t::leave>(xil.step() - 1);

            default:
                X_UNEXPECTED();
        }
    }

    //-------- ---------- ---------- ---------- ----------
    // New commands.

    #define __NewCmd(_new_type)     __Cmd(new_, _new_type)
    __DefineCmdValue_(__NewCmd(default_))
    __DefineCmdValue_(__NewCmd(array))
    __DefineCmdValue_(__NewCmd(stack_alloc))

    template<xil_new_type_t _new_type> class __new_command_t { };

    //-------- ---------- ---------- ---------- ----------

    template<>
    class __new_command_t<xil_new_type_t::default_>
        : public __command_base_t<__NewCmd(default_)>
    {
        typedef __new_command_t     __self_t;

    public:
        #if EXEC_TRACE
        __new_command_t(rt_type_t * type, rt_sid_t name) : __type(type), __name(name) { }
        #else
        __new_command_t(rt_type_t * type) : __type(type) { }
        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            __new_object(ctx, __This->__type);

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
    class __new_command_t<xil_new_type_t::array>
        : public __command_base_t<__NewCmd(array)>
    {
        typedef __new_command_t     __self_t;

    public:
        #if EXEC_TRACE
        __new_command_t(rt_type_t * type, rt_sid_t name)
            : __type(_M(rt_array_type_t *, type)), __name(name) { }
        #else
        __new_command_t(rt_type_t * type) : __type(_M(rt_array_type_t *, type)) { }
        #endif // EXEC_TRACE

        __BeginExecute(ctx)

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
    class __new_command_t<xil_new_type_t::stack_alloc>
        : public __command_base_t<__NewCmd(stack_alloc)>
    {
        typedef __new_command_t     __self_t;

    public:
        __new_command_t(size_t units) _NE : __units(units) { }

        __BeginExecute(ctx)

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
        template<command_value_t _cv, xil_new_type_t _new_type, typename ... _args_t>
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

    struct __new_alloc_command_template_t
    {
        template<command_value_t _cv, xil_new_type_t _new_type, typename ... _args_t>
        static auto new_command(memory_t * memory, _args_t && ... args)
        {
            typedef __new_command_t<_new_type> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<_args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    static command_t * __new_new_command(__context_t & ctx, const new_xil_t & xil)
    {
        static __command_manager_t<
            __new_command_template_t, xil_new_type_t
        >::with_args_t<rt_type_t *> __new_command_manager;

        static __command_manager_t<
            __new_alloc_command_template_t, xil_new_type_t
        >::with_args_t<size_t> __alloc_command_manager;

        ref_t type_ref = xil.type_ref();
        rt_type_t * type = ctx.get_type(type_ref);

        if (type == nullptr)
            throw _ED(exec_error_code_t::type_not_found, type_ref);

        switch (xil.new_type())
        {
            case xil_new_type_t::default_:
                return __new_command_manager.template
                    get_command<__NewCmd(default_), xil_new_type_t::default_>(type, ctx);

            case xil_new_type_t::array:
                return __new_command_manager.template
                    get_command<__NewCmd(array), xil_new_type_t::array>(type, ctx);

            case xil_new_type_t::stack_alloc: {

                msize_t obj_size = type->get_size(ctx.env);
                size_t units = _alignf(obj_size, sizeof(rt_stack_unit_t)) / sizeof(rt_stack_unit_t);
                return __alloc_command_manager.template
                    get_command<__NewCmd(stack_alloc), xil_new_type_t::stack_alloc>(units);

            }   break;

            case xil_new_type_t::stack_allocs: {

                msize_t obj_size = type->get_size(ctx.env);
                uint32_t count = xil.count();

                size_t units = _alignf(obj_size * count, sizeof(rt_stack_unit_t))
                                                        / sizeof(rt_stack_unit_t);

                return __alloc_command_manager.template
                    get_command<__NewCmd(stack_alloc), xil_new_type_t::stack_alloc>(units);

            }   break;

            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    #define __CopyCmd(_kind, _size)    __Cmd(copy, _kind##_##_size)

    #define __DefineCopyCmdValues(_kind)                                                \
        __DefineCmdValue_(__CopyCmd(_kind, 1))                                          \
        __DefineCmdValue_(__CopyCmd(_kind, 2))                                          \
        __DefineCmdValue_(__CopyCmd(_kind, 4))                                          \
        __DefineCmdValue_(__CopyCmd(_kind, 8))                                          \
        __DefineCmdValue_(__CopyCmd(_kind, 12))                                         \
        __DefineCmdValue_(__CopyCmd(_kind, 16))                                         \
        __DefineCmdValue_(__CopyCmd(_kind, 20))                                         \
        __DefineCmdValue_(__CopyCmd(_kind, 24))                                         \
        __DefineCmdValue_(__CopyCmd(_kind, 28))                                         \
        __DefineCmdValue_(__CopyCmd(_kind, 32))

    __DefineCopyCmdValues(default_)
    __DefineCopyCmdValues(reverse)
    __DefineCopyCmdValues(zero)

    #define __CopyCmdEx                 __Cmd(copy, 0)
    __DefineCmdValue_(__CopyCmdEx)

    template<command_value_t _cv>
    class __copy_command_base_t : public __command_base_t<_cv>
    {
    public:

        #if EXEC_TRACE

        __ToString(_T("copy"));

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

    template<command_value_t _cv, size_t _size, xil_copy_kind_t _kind = xil_copy_kind_t::default_>
    class __memory_copy_command_t : public __copy_command_base_t<_cv>
    {
        typedef __memory_copy_command_t     __self_t;
        typedef __copy_command_base_t<_cv>  __super_t;

    public:

        __BeginExecute(ctx)
            
            __memory_copy_operation_t<_size, _kind>::copy(ctx);

        __EndExecute()
    };

    class __memory_copy_command_ex_t : public __copy_command_base_t<__CopyCmdEx>
    {
        typedef __memory_copy_command_ex_t  __self_t;
        typedef __copy_command_base_t       __super_t;

    public:

        __memory_copy_command_ex_t(size_t size, xil_copy_kind_t kind) _NE
            : __size(size), __kind(kind) { }

        __BeginExecute(ctx)
            
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
        template<command_value_t _cv, size_t _size, xil_copy_kind_t _kind, typename ... _args_t>
        static auto new_command(memory_t * memory, _args_t && ... args)
        {
            typedef __memory_copy_command_t<_cv, _size, _kind> this_command_t;
            return __new_command<this_command_t>(memory, std::forward<_args_t>(args) ...);
        }
    };

    struct __memory_copy_command_ex_template_t
    {
        template<command_value_t _cv, typename ... _args_t>
        static auto new_command(memory_t * memory, _args_t && ... args)
        {
            static_assert(_cv == __CopyCmdEx, "invalid command value");

            typedef __memory_copy_command_ex_t this_command_t;
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
            __memory_copy_command_ex_template_t
        >::with_args_t<size_t, xil_copy_kind_t> __memory_copy_command_ex_manager;


        #define __Case(_size, _kind)                                                    \
            case _size:                                                                 \
                return __memory_copy_command_manager.template                           \
                    get_command<__CopyCmd(_kind, _size), _size, xil_copy_kind_t::_kind>();

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
                    return __memory_copy_command_ex_manager.template                    \
                        get_command<__CopyCmdEx>(size, kind);                           \
            }

        switch (kind)
        {
            case xil_copy_kind_t::default_:
                __Switch(size, default_)
                break;

            case xil_copy_kind_t::reverse:
                __Switch(size, reverse)
                break;

            case xil_copy_kind_t::zero:
                __Switch(size, zero);
                break;

            default:
                X_UNEXPECTED_F("unexpected xil copy kind '%1%'", kind);
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

            case xil_copy_type_t::generic_copy: {
                
                ref_t type_ref = xil.type_ref();
                rt_type_t * type = ctx.get_type(type_ref);
                msize_t size = type->get_variable_size(ctx.env);
                xil_copy_kind_t kind = xil.copy_kind();

                return __get_object_copy_command(size, kind);

            }   break;

            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    template<command_value_t _cv>
    class __init_command_base_t : public __command_base_t<_cv>
    {
    public:

        #if EXEC_TRACE

        __ToString(_T("init"));

        #endif  // EXEC_TRACE
    };

    //-------- ---------- ---------- ---------- ----------

    template<command_value_t _cv, xil_init_type_t _init_type, msize_t _size>
    class __init_command_t { };

    #define __InitCmd(_init_type, _size)        __Cmd(init, _init_type##_##_size)

    __DefineCmdValue_(__InitCmd(array_begin, 0))

    __DefineCmdValue_(__InitCmd(array_element, 0))
    __DefineCmdValue_(__InitCmd(array_element, 1))
    __DefineCmdValue_(__InitCmd(array_element, 2))
    __DefineCmdValue_(__InitCmd(array_element, 4))
    __DefineCmdValue_(__InitCmd(array_element, 8))

    __DefineCmdValue_(__InitCmd(array_end, 0))

    //-------- ---------- ---------- ---------- ----------

    template<command_value_t _cv>
    class __init_command_t<_cv, xil_init_type_t::array_begin, 0>
        : public __init_command_base_t<_cv>
    {
        typedef __init_command_t            __self_t;
        typedef __init_command_base_t<_cv>   __super_t;

    public:
        using __super_t::__super_t;

        __BeginExecute(ctx)
            
            ctx.stack.push<void *>(
                (void *)ctx.stack.pick<rt_ref_t>()
            );

        __EndExecute()
    };

    //-------- ---------- ---------- ---------- ----------

    template<command_value_t _cv, msize_t _size>
    class __init_command_t<_cv, xil_init_type_t::array_element, _size>
        : public __init_command_base_t<_cv>
    {
        typedef __init_command_t            __self_t;
        typedef __init_command_base_t<_cv>  __super_t;

    public:
        using __super_t::__super_t;

        __BeginExecute(ctx)

            typedef uint_type_t<_size> t;
            *ctx.stack.pick_reference<t *>()++ = ctx.stack.pop<t>();

        __EndExecute()
    };

    //-------- ---------- ---------- ---------- ----------

    template<command_value_t _cv>
    class __init_command_t<_cv, xil_init_type_t::array_element, 0>
        : public __init_command_base_t<_cv>

    {
        typedef __init_command_t            __self_t;
        typedef __init_command_base_t<_cv>  __super_t;

    public:
        __init_command_t(msize_t size) : __size(size) { }

        __BeginExecute(ctx)
            
            // TODO: implement it.
            X_UNEXPECTED();

        __EndExecute()

    private:
        msize_t __size;
    };

    //-------- ---------- ---------- ---------- ----------

    template<command_value_t _cv>
    class __init_command_t<_cv, xil_init_type_t::array_end, 0>
        : public __init_command_base_t<_cv>
    {
        typedef __init_command_t            __self_t;
        typedef __init_command_base_t<_cv>  __super_t;

    public:
        using __super_t::__super_t;

        __BeginExecute(ctx)

            ctx.stack.pop<void *>();

        __EndExecute()
    };

    //-------- ---------- ---------- ---------- ----------

    struct __init_command_template_t
    {
        template<command_value_t _cv, xil_init_type_t _init_type, msize_t _size,
                                                        typename ... _args_t>
        static auto new_command(memory_t * memory, _args_t && ... args)
        {
            typedef __init_command_t<_cv, _init_type, _size> this_command_t;
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
                    get_command<__InitCmd(array_begin, 0), xil_init_type_t::array_begin, 0>();

            case xil_init_type_t::array_element: {
                msize_t size = __size_of_array_element(ctx, xil);
                switch (size)
                {
                    #define __Case(_size)                                               \
                        case _size:                                                     \
                            return __stack_init_command_manager.template get_command<   \
                                __InitCmd(array_element, _size),                        \
                                xil_init_type_t::array_element, _size>();

                    __Case(1)
                    __Case(2)
                    __Case(4)
                    __Case(8)

                    default:
                        return __stack_init_command_manager.template get_command<
                            __InitCmd(array_element, 0),
                            xil_init_type_t::array_element, 0>(size);
                   
                    #undef __Case
                }

            }   break;

            case xil_init_type_t::array_end:
                return __stack_init_command_manager.template get_command<
                    __InitCmd(array_end, 0),
                    xil_init_type_t::array_end, 0>();

            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    command_t * new_command(command_creating_context_t & ctx, const xil_base_t * xil)
    {
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

            case xil_command_t::cast:
                return __new_cast_command(ctx, *(const cast_xil_t *)xil);

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
                X_UNEXPECTED_F("unexpected xil command '%1%'", xil->command());
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Creates a push command.
    command_t * __new_push_command(command_creating_context_t & ctx, rt_push_command_base_t * rt_cmd)
    {
        switch (rt_cmd->stype)
        {
            case xil_storage_type_t::argument: {
                rt_push_argument_command_t * cmd = (rt_push_argument_command_t *)rt_cmd;
                xil_type_t xt = __xil_type_of_argument(ctx, cmd->identity);

                __new_push_command_args_t args {
                    .stype = xil_storage_type_t::argument,
                    .xt1   = xt,
                    .xt2   = xt,
                    .argument.offset = __offset_of_argument(ctx, cmd->identity),
                };

                return __new_push_command(ctx, args);
            }

            case xil_storage_type_t::field: {
                rt_push_field_command_t * cmd = (rt_push_field_command_t *)rt_cmd;
                rt_field_base_t * field = cmd->field;
                rt_type_t * field_type = field->get_field_type(ctx,
                                        dynamic_cast<rt_generic_type_t *>(ctx.type));

                _A(field_type != nullptr);
                xil_type_t xt = __xil_type_of(ctx, field_type);

                __new_push_command_args_t args {
                    .stype = xil_storage_type_t::field,
                    .xt1   = xt,
                    .xt2   = xt,
                    .field.offset = field->offset,
                };

                return __new_push_command(ctx, args);
            }

            default:
                X_UNEXPECTED_F("unexpected storage type '%1%'", rt_cmd->stype);
        }
    }

    // Creates a smp command.
    command_t * __new_smp_command(command_creating_context_t & ctx, rt_smp_command_base_t * rt_cmd)
    {
        _A(rt_cmd != nullptr);

        switch (rt_cmd->smp)
        {
            case xil_smp_t::ret:
                return new_ret_command(ctx);

            default:
                X_UNEXPECTED_F("unexpected smp type %1%", rt_cmd->smp);
        }
    }

    // Creates command.
    command_t * new_command(command_creating_context_t & ctx, rt_command_t * rt_cmd)
    {
        _A(rt_cmd != nullptr);

        switch (rt_cmd->cmd)
        {
            case xil_command_t::push:
                return __new_push_command(ctx, (rt_push_command_base_t *)rt_cmd);

            case xil_command_t::smp:
                return __new_smp_command(ctx, (rt_smp_command_base_t *)rt_cmd);

            default:
                X_UNEXPECTED_F("unexpected xil command '%1%", rt_cmd->cmd);
        }

        return nullptr;
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
        _PFC(cyan, _T("   %|1$-50|[%2%]"), to_command_string(command), top0);           \

    #elif EXEC_TRACE >= 2   // EXEC_TRACE >= 3

    #define __BeginExecuteCommand()                                                     \
        void * top0 = ctx.stack.top();

    #endif  // EXEC_TRACE >= 3

    #define __EndExecuteCommand()                                                       \
        void * top1 = ctx.stack.top();                                                  \
        int    diff = (int)((byte_t *)top1 - (byte_t *)top0);                           \
        _PFC(light_cyan, _T("-> %|1$-50|[%2%] %3%%4%"), to_command_string(command),     \
            top1, diff >= 0? _T("+") : _T(""), diff                                     \
        );

    #else   // EXEC_TRACE >= 2

    #define __BeginExecuteCommand()
    #define __EndExecuteCommand()

    #endif  // EXEC_TRACE >= 2

        
    #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_MANUAL

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
    #define __SwitchCommands_CaseSt(_cmd, _st, _xt1, _xt2)                              \
        __Case( __##_cmd##_command_t<__StCmd(_cmd, _st, _xt1, _xt2),                    \
            xil_storage_type_t::_st, xil_type_t::_xt1, xil_type_t::_xt2> )

    #define __SwitchCommands_CaseSts_(_cmd, _st, _xt1)                                  \
        __SwitchCommands_CaseSt(_cmd, _st, _xt1, int8)                                  \
        __SwitchCommands_CaseSt(_cmd, _st, _xt1, uint8)                                 \
        __SwitchCommands_CaseSt(_cmd, _st, _xt1, int16)                                 \
        __SwitchCommands_CaseSt(_cmd, _st, _xt1, uint16)                                \
        __SwitchCommands_CaseSt(_cmd, _st, _xt1, int32)                                 \
        __SwitchCommands_CaseSt(_cmd, _st, _xt1, uint32)                                \
        __SwitchCommands_CaseSt(_cmd, _st, _xt1, int64)                                 \
        __SwitchCommands_CaseSt(_cmd, _st, _xt1, uint64)                                \
        __SwitchCommands_CaseSt(_cmd, _st, _xt1, float_)                                \
        __SwitchCommands_CaseSt(_cmd, _st, _xt1, double_)                               \
        __SwitchCommands_CaseSt(_cmd, _st, _xt1, char_)                                 \
        __SwitchCommands_CaseSt(_cmd, _st, _xt1, bool_)

    #define __SwitchCommands_CaseSts(_cmd, _st)                                         \
        __SwitchCommands_CaseSts_(_cmd, _st, int8)                                      \
        __SwitchCommands_CaseSts_(_cmd, _st, uint8)                                     \
        __SwitchCommands_CaseSts_(_cmd, _st, int16)                                     \
        __SwitchCommands_CaseSts_(_cmd, _st, uint16)                                    \
        __SwitchCommands_CaseSts_(_cmd, _st, int32)                                     \
        __SwitchCommands_CaseSts_(_cmd, _st, uint32)                                    \
        __SwitchCommands_CaseSts_(_cmd, _st, int64)                                     \
        __SwitchCommands_CaseSts_(_cmd, _st, uint64)                                    \
        __SwitchCommands_CaseSts_(_cmd, _st, float_)                                    \
        __SwitchCommands_CaseSts_(_cmd, _st, double_)                                   \
        __SwitchCommands_CaseSts_(_cmd, _st, char_)                                     \
        __SwitchCommands_CaseSts_(_cmd, _st, bool_)                                     \
        __SwitchCommands_CaseSt(_cmd, _st, object, object)                              \
        __SwitchCommands_CaseSt(_cmd, _st, string, string)                              \
        __SwitchCommands_CaseSt(_cmd, _st, ptr, ptr)

    #define __SwitchCommands_CaseArray(_cmd, _xt1, _xt2, _dimension)                    \
        __Case( __##_cmd##_array_element_command_t<                                     \
            __StArrayCmd(_cmd, _xt1, _xt2, _dimension),                                 \
            xil_type_t::_xt1, xil_type_t::_xt2, _dimension> )

    #define __SwitchCommands_CaseArrays_(_cmd, _xt1, _dimension)                        \
        __SwitchCommands_CaseArray(_cmd, _xt1, int8,    _dimension)                     \
        __SwitchCommands_CaseArray(_cmd, _xt1, uint8,   _dimension)                     \
        __SwitchCommands_CaseArray(_cmd, _xt1, int16,   _dimension)                     \
        __SwitchCommands_CaseArray(_cmd, _xt1, uint16,  _dimension)                     \
        __SwitchCommands_CaseArray(_cmd, _xt1, int32,   _dimension)                     \
        __SwitchCommands_CaseArray(_cmd, _xt1, uint32,  _dimension)                     \
        __SwitchCommands_CaseArray(_cmd, _xt1, int64,   _dimension)                     \
        __SwitchCommands_CaseArray(_cmd, _xt1, uint64,  _dimension)                     \
        __SwitchCommands_CaseArray(_cmd, _xt1, float_,  _dimension)                     \
        __SwitchCommands_CaseArray(_cmd, _xt1, double_, _dimension)                     \
        __SwitchCommands_CaseArray(_cmd, _xt1, char_,   _dimension)                     \
        __SwitchCommands_CaseArray(_cmd, _xt1, bool_,   _dimension)

    // Push/Pick/Pop Array commands.
    #define __SwitchCommands_CaseArrays(_cmd, _dimension)                               \
        __SwitchCommands_CaseArrays_(_cmd, int8,    _dimension)                         \
        __SwitchCommands_CaseArrays_(_cmd, uint8,   _dimension)                         \
        __SwitchCommands_CaseArrays_(_cmd, int16,   _dimension)                         \
        __SwitchCommands_CaseArrays_(_cmd, uint16,  _dimension)                         \
        __SwitchCommands_CaseArrays_(_cmd, int32,   _dimension)                         \
        __SwitchCommands_CaseArrays_(_cmd, uint32,  _dimension)                         \
        __SwitchCommands_CaseArrays_(_cmd, int64,   _dimension)                         \
        __SwitchCommands_CaseArrays_(_cmd, uint64,  _dimension)                         \
        __SwitchCommands_CaseArrays_(_cmd, float_,  _dimension)                         \
        __SwitchCommands_CaseArrays_(_cmd, double_, _dimension)                         \
        __SwitchCommands_CaseArrays_(_cmd, char_,   _dimension)                         \
        __SwitchCommands_CaseArrays_(_cmd, bool_,   _dimension)                         \
        __SwitchCommands_CaseArray(_cmd, object, object, _dimension)                    \
        __SwitchCommands_CaseArray(_cmd, string, string, _dimension)                    \
        __SwitchCommands_CaseArray(_cmd, ptr, ptr, _dimension)

    // Push Constants.

    #define __SwitchCommands_CaseConstant(_xt1, _xt2)                                   \
        __Case( __push_command_t<                                                       \
                __StCmd(push, constant, _xt1, _xt2),                                    \
                xil_storage_type_t::constant, xil_type_t::_xt1, xil_type_t::_xt2> )
        
    #define __SwitchCommands_CaseConstants_(_xt1)                                       \
        __SwitchCommands_CaseConstant(_xt1, int8)                                       \
        __SwitchCommands_CaseConstant(_xt1, uint8)                                      \
        __SwitchCommands_CaseConstant(_xt1, int16)                                      \
        __SwitchCommands_CaseConstant(_xt1, uint16)                                     \
        __SwitchCommands_CaseConstant(_xt1, int32)                                      \
        __SwitchCommands_CaseConstant(_xt1, uint32)                                     \
        __SwitchCommands_CaseConstant(_xt1, int64)                                      \
        __SwitchCommands_CaseConstant(_xt1, uint64)                                     \
        __SwitchCommands_CaseConstant(_xt1, float_)                                     \
        __SwitchCommands_CaseConstant(_xt1, double_)                                    \
        __SwitchCommands_CaseConstant(_xt1, char_)                                      \
        __SwitchCommands_CaseConstant(_xt1, bool_)

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

    #define __SwitchCommands_CaseAddress(_st)                                           \
        __Case( __push_address_command_t<xil_storage_type_t::_st> )

    // binary, unitary commands
    #define __SwitchCommands_CaseBinary(_cmd, _op, _xt1, _xt2)                          \
        __Case( __binary_command_t<                                                     \
            __BinaryCmd(_cmd, _op, _xt1, _xt2),                                         \
            decltype(xil_##_cmd##_command_t::_op), xil_##_cmd##_command_t::_op,         \
            xil_type_t::_xt1, xil_type_t::_xt2                                          \
        > )

    #define __SwitchCommands_CaseUnitary(_cmd, _op, _xt)                                \
        __Case( __unitary_command_t<                                                    \
            __UnitaryCmd(_cmd, _op, _xt),                                               \
            decltype(xil_##_cmd##_command_t::_op), xil_##_cmd##_command_t::_op,         \
            xil_type_t::_xt                                                             \
        > )

    #define __SwitchCommands_CaseBinary_1(_cmd, _op, _xt1, _xt2)                        \
        __SwitchCommands_CaseBinary(_cmd, _op, _xt1, _xt2)

    #define __SwitchCommands_CaseBinary_2(_cmd, _op, _xt1, _xt2)                        \
        __SwitchCommands_CaseBinary(_cmd, _op, _xt1, _xt2)                              \
        __SwitchCommands_CaseBinary(_cmd, _op, _xt2, _xt1)

    #define __SwitchCommands_CaseBinaries(_cmd, _op)                                    \
            __SwitchCommands_CaseBinary_1(_cmd, _op, int8,   int8)                      \
            __SwitchCommands_CaseBinary_2(_cmd, _op, int8,   uint8)                     \
                                                                                        \
            __SwitchCommands_CaseBinary_1(_cmd, _op, uint8,  uint8)                     \
                                                                                        \
            __SwitchCommands_CaseBinary_2(_cmd, _op, int16,  int8)                      \
            __SwitchCommands_CaseBinary_2(_cmd, _op, int16,  uint8)                     \
            __SwitchCommands_CaseBinary_1(_cmd, _op, int16,  int16)                     \
            __SwitchCommands_CaseBinary_2(_cmd, _op, int16,  uint16)                    \
                                                                                        \
            __SwitchCommands_CaseBinary_2(_cmd, _op, uint16, int8)                      \
            __SwitchCommands_CaseBinary_2(_cmd, _op, uint16, uint8)                     \
            __SwitchCommands_CaseBinary_1(_cmd, _op, uint16, uint16)                    \
                                                                                        \
            __SwitchCommands_CaseBinary_2(_cmd, _op, int32,  int8)                      \
            __SwitchCommands_CaseBinary_2(_cmd, _op, int32,  uint8)                     \
            __SwitchCommands_CaseBinary_2(_cmd, _op, int32,  int16)                     \
            __SwitchCommands_CaseBinary_2(_cmd, _op, int32,  uint16)                    \
            __SwitchCommands_CaseBinary_1(_cmd, _op, int32,  int32)                     \
                                                                                        \
            __SwitchCommands_CaseBinary_2(_cmd, _op, uint32, int8)                      \
            __SwitchCommands_CaseBinary_2(_cmd, _op, uint32, uint8)                     \
            __SwitchCommands_CaseBinary_2(_cmd, _op, uint32, int16)                     \
            __SwitchCommands_CaseBinary_2(_cmd, _op, uint32, uint16)                    \
            __SwitchCommands_CaseBinary_2(_cmd, _op, uint32, int32)                     \
            __SwitchCommands_CaseBinary_1(_cmd, _op, uint32, uint32)                    \
                                                                                        \
            __SwitchCommands_CaseBinary_2(_cmd, _op, int64,  int8)                      \
            __SwitchCommands_CaseBinary_2(_cmd, _op, int64,  uint8)                     \
            __SwitchCommands_CaseBinary_2(_cmd, _op, int64,  int16)                     \
            __SwitchCommands_CaseBinary_2(_cmd, _op, int64,  uint16)                    \
            __SwitchCommands_CaseBinary_2(_cmd, _op, int64,  int32)                     \
            __SwitchCommands_CaseBinary_2(_cmd, _op, int64,  uint32)                    \
            __SwitchCommands_CaseBinary_1(_cmd, _op, int64,  int64)                     \
                                                                                        \
            __SwitchCommands_CaseBinary_2(_cmd, _op, uint64,  int8)                     \
            __SwitchCommands_CaseBinary_2(_cmd, _op, uint64,  uint8)                    \
            __SwitchCommands_CaseBinary_2(_cmd, _op, uint64,  int16)                    \
            __SwitchCommands_CaseBinary_2(_cmd, _op, uint64,  uint16)                   \
            __SwitchCommands_CaseBinary_2(_cmd, _op, uint64,  int32)                    \
            __SwitchCommands_CaseBinary_2(_cmd, _op, uint64,  uint32)                   \
            __SwitchCommands_CaseBinary_2(_cmd, _op, uint64,  int64)                    \
            __SwitchCommands_CaseBinary_1(_cmd, _op, uint64,  uint64)

        #define __SwitchCommands_CaseFloatBinaries(_cmd, _op)                           \
            __SwitchCommands_CaseBinary_2(_cmd, _op, float_,  int8)                     \
            __SwitchCommands_CaseBinary_2(_cmd, _op, float_,  uint8)                    \
            __SwitchCommands_CaseBinary_2(_cmd, _op, float_,  int16)                    \
            __SwitchCommands_CaseBinary_2(_cmd, _op, float_,  uint16)                   \
            __SwitchCommands_CaseBinary_2(_cmd, _op, float_,  int32)                    \
            __SwitchCommands_CaseBinary_2(_cmd, _op, float_,  uint32)                   \
            __SwitchCommands_CaseBinary_2(_cmd, _op, float_,  int64)                    \
            __SwitchCommands_CaseBinary_1(_cmd, _op, float_,  uint64)                   \
            __SwitchCommands_CaseBinary_1(_cmd, _op, float_,  float_)                   \
                                                                                        \
            __SwitchCommands_CaseBinary_2(_cmd, _op, double_,  int8)                    \
            __SwitchCommands_CaseBinary_2(_cmd, _op, double_,  uint8)                   \
            __SwitchCommands_CaseBinary_2(_cmd, _op, double_,  int16)                   \
            __SwitchCommands_CaseBinary_2(_cmd, _op, double_,  uint16)                  \
            __SwitchCommands_CaseBinary_2(_cmd, _op, double_,  int32)                   \
            __SwitchCommands_CaseBinary_2(_cmd, _op, double_,  uint32)                  \
            __SwitchCommands_CaseBinary_2(_cmd, _op, double_,  int64)                   \
            __SwitchCommands_CaseBinary_1(_cmd, _op, double_,  uint64)                  \
            __SwitchCommands_CaseBinary_2(_cmd, _op, double_,  float_)                  \
            __SwitchCommands_CaseBinary_1(_cmd, _op, double_,  double_)

        #define __SwitchCommands_CaseUnitaries(_cmd, _op)                               \
            __SwitchCommands_CaseUnitary(_cmd, _op, int8)                               \
            __SwitchCommands_CaseUnitary(_cmd, _op, uint8)                              \
            __SwitchCommands_CaseUnitary(_cmd, _op, int16)                              \
            __SwitchCommands_CaseUnitary(_cmd, _op, uint16)                             \
            __SwitchCommands_CaseUnitary(_cmd, _op, int32)                              \
            __SwitchCommands_CaseUnitary(_cmd, _op, uint32)                             \
            __SwitchCommands_CaseUnitary(_cmd, _op, int64)                              \
            __SwitchCommands_CaseUnitary(_cmd, _op, uint64)

        #define __SwitchCommands_CaseFloatUnitaries(_cmd, _op)                          \
            __SwitchCommands_CaseUnitary(_cmd, _op, float_)                             \
            __SwitchCommands_CaseUnitary(_cmd, _op, double_)

        #define __SwitchCommands_CaseRet(_ret_size)                                     \
            __Case( __ret_command_t<__RetCmd(_ret_size), _ret_size> )

        #define __SwitchCommands_CaseCopy(_size, _kind)                                 \
            __Case( __memory_copy_command_t<                                            \
                __CopyCmd(_kind, _size),_size, xil_copy_kind_t::_kind                   \
            > )

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

        #define __SwitchCommands_CaseJmp(_model)                                        \
            __Case( __jmp_command_t<__JmpCmd(_model), xil_jmp_model_t::_model> )


    #define __BeginSwitchCommands(_cmd_value)                                           \
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
            __Case( __pop_empty_command_t )                                             \
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
            __Case( __internal_call_command_t )                                         \
            __Case( __static_call_command_t<__CallCmd_(static_, generic), rt_generic_method_t> ) \
            __Case( __static_call_command_t<__CallCmd_(static_, general), rt_method_t> ) \
            __Case( __instance_call_command_t<__CallCmd_(instance, generic), rt_generic_method_t> ) \
            __Case( __instance_call_command_t<__CallCmd_(instance, general), rt_method_t> ) \
            __Case( __instance_call_command_t<__CallCmd_(instance, dynamic), rt_method_t> ) \
            __Case( __virtual_call_command_t )                                          \
            __Case( __interface_call_command_t )                                        \
            __Case( __delegate_call_command_t<__CallCmd(delegate_init) )                \
            __Case( __delegate_call_command_t<__CallCmd(delegate_invoke) )              \
                                                                                        \
            /* al */                                                                    \
            __SwitchCommands_CaseBinaries(al, add)                                      \
            __SwitchCommands_CaseFloatBinaries(al, add)                                 \
            __SwitchCommands_CaseBinaries(al, sub)                                      \
            __SwitchCommands_CaseFloatBinaries(al, sub)                                 \
            __SwitchCommands_CaseBinaries(al, mul)                                      \
            __SwitchCommands_CaseFloatBinaries(al, mul)                                 \
            __SwitchCommands_CaseBinaries(al, div)                                      \
            __SwitchCommands_CaseFloatBinaries(al, div)                                 \
            __SwitchCommands_CaseBinaries(al, mod)                                      \
            __SwitchCommands_CaseUnitaries(al, minus)                                   \
            __SwitchCommands_CaseFloatUnitaries(al, minus)                              \
            __SwitchCommands_CaseUnitaries(al, positive)                                \
            __SwitchCommands_CaseFloatUnitaries(al, positive)                           \
                                                                                        \
            /* bit */                                                                   \
            __SwitchCommands_CaseBinaries(bit, bit_and)                                 \
            __SwitchCommands_CaseBinaries(bit, bit_or)                                  \
            __SwitchCommands_CaseBinaries(bit, bit_xor)                                 \
            __SwitchCommands_CaseBinaries(bit, left_shift)                              \
            __SwitchCommands_CaseBinaries(bit, right_shift)                             \
            __SwitchCommands_CaseUnitaries(bit, bit_not)                                \
                                                                                        \
            /* logic */                                                                 \
            __SwitchCommands_CaseBinary(logic, and_, bool_, bool_)                      \
            __SwitchCommands_CaseBinary(logic, or_, bool_, bool_)                       \
            __SwitchCommands_CaseUnitary(logic, not_, bool_)                            \
                                                                                        \
            /* cmp */                                                                   \
            __SwitchCommands_CaseBinaries(cmp, greater)                                 \
            __SwitchCommands_CaseFloatBinaries(cmp, greater)                            \
            __SwitchCommands_CaseBinaries(cmp, greater_equal)                           \
            __SwitchCommands_CaseFloatBinaries(cmp, greater_equal)                      \
            __SwitchCommands_CaseBinaries(cmp, less)                                    \
            __SwitchCommands_CaseFloatBinaries(cmp, less)                               \
            __SwitchCommands_CaseBinaries(cmp, less_equal)                              \
            __SwitchCommands_CaseFloatBinaries(cmp, less_equal)                         \
            __SwitchCommands_CaseBinaries(cmp, equal)                                   \
            __SwitchCommands_CaseFloatBinaries(cmp, equal)                              \
            __SwitchCommands_CaseBinaries(cmp, not_equal)                               \
            __SwitchCommands_CaseFloatBinaries(cmp, not_equal)                          \
            __SwitchCommands_CaseBinary(cmp, equal, bool_, bool_)                       \
            __SwitchCommands_CaseBinary(cmp, not_equal, bool_, bool_)                   \
            __SwitchCommands_CaseBinary(cmp, equal, char_, char_)                       \
            __SwitchCommands_CaseBinary(cmp, not_equal, char_, char_)                   \
            __SwitchCommands_CaseBinary(cmp, greater, char_, char_)                     \
            __SwitchCommands_CaseBinary(cmp, greater_equal, char_, char_)               \
            __SwitchCommands_CaseBinary(cmp, less, char_, char_)                        \
            __SwitchCommands_CaseBinary(cmp, less_equal, char_, char_)                  \
            __SwitchCommands_CaseBinary(cmp, equal, object, object)                     \
            __SwitchCommands_CaseBinary(cmp, not_equal, object, object)                 \
            __SwitchCommands_CaseBinary(cmp, equal, ptr, ptr)                           \
            __SwitchCommands_CaseBinary(cmp, not_equal, ptr, ptr)                       \
                                                                                        \
            /* cast */                                                                  \
            __Case( __cast_command_template_t<xil_cast_command_t::default_, true> )     \
            __Case( __cast_command_template_t<xil_cast_command_t::default_, false> )    \
            __Case( __cast_command_template_t<xil_cast_command_t::as, true> )           \
            __Case( __cast_command_template_t<xil_cast_command_t::as, false> )          \
            __Case( __cast_command_template_t<xil_cast_command_t::is, true> )           \
            __Case( __cast_command_template_t<xil_cast_command_t::is, false> )          \
                                                                                        \
            /* ret */                                                                   \
            __SwitchCommands_CaseRet(0)                                                 \
            __SwitchCommands_CaseRet(1)                                                 \
            __SwitchCommands_CaseRet(__StructRetUnitSize)                               \
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
            __SwitchCommands_CaseJmp(none)                                              \
            __SwitchCommands_CaseJmp(true_)                                             \
            __SwitchCommands_CaseJmp(false_)                                            \
            __SwitchCommands_CaseJmp(leave)                                             \
            __Case( __leave_ret_command_t )                                             \
            __Case( __switch_jmp_command_t )                                            \
                                                                                        \
            /* new */                                                                   \
            __Case( __new_command_t<xil_new_type_t::default_> )                         \
            __Case( __new_command_t<xil_new_type_t::array> )                            \
            __Case( __new_command_t<xil_new_type_t::stack_alloc> )                      \
            __Case( __new_command_t<xil_new_type_t::temp_alloc> )                       \
                                                                                        \
            /* copy */                                                                  \
            __SwitchCommands_CaseCopies(default_)                                       \
            __SwitchCommands_CaseCopies(reverse)                                        \
            __SwitchCommands_CaseCopies(zero)                                           \
            __Case( __memory_copy_command_ex_t )                                        \
                                                                                        \
            /* end */                                                                   \
            /* __Case( __execute_end_command_t ) */                                     \
                                                                                        \

    
    #define __EndSwitchCommands(_cmd_value)                                             \
        }

    //-------- ---------- ---------- ---------- ----------

    #if EXEC_TRACE
    const string_t to_command_string(command_t * command)
    {
        #define __Case(_t, _ts...)                                                      \
            case _t, ##_ts::cmd_value:                                                  \
                return ((_t, ##_ts *)command)->to_string();

        __BeginSwitchCommands(command->cmd_value)

            case __EndCmd:
                return _T("[end]");

            default:
                return _F(_T("[%1%]"), _cmd_value);

        __EndSwitchCommands(command->cmd_value)

        #undef __Case
    }

    #endif  // EXEC_TRACE

    #elif EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL // EXEC_EXECUTE_MODEL == xxx

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

    #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_MANUAL
        try
        {
    __begin__:
            command_t * command = *commands++;

            __BeginExecuteCommand()
            command->execute_method(command, ctx);
            __EndExecuteCommand()

            goto __begin__;
        }
        catch (__exec_env_error_t & e)
        {
            return e;
        }


    #elif EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_INLINE

    __begin__:

        command_t * command = *commands++;

        __BeginExecuteCommand()

        #define __Case(_t, _ts...)                                                      \
            case _t, ##_ts::cmd_value:                                                  \
                ((_t, ##_ts *)command)->execute(ctx);                                   \
                goto __begin__;

        __BeginSwitchCommands(command->cmd_value)

            case __EndCmd:
                return _E(exec_env_code_t::end);

            default:
                throw _EC(unexpected, _F(_T("unexpected command %1%"), command->cmd_value));

        __EndSwitchCommands()

        #undef __Case

        __EndExecuteCommand()


    #elif EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL

        try
        {
    __begin__:
            command_t * command = *commands++;

            __BeginExecuteCommand()
            command->execute(ctx);
            __EndExecuteCommand()

            goto __begin__;
        }
        catch (__exec_env_error_t & e)
        {
            return e;
        }

    #else
        
        #error unknown EXEC_EXECUTE_MODEL

    #endif  // EXEC_EXECUTE_MODEL == xxx
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

                _PF(_T("Uncaught Exception: %1%"), exception_type->get_name(ctx.env));

            }   break;

            default:
                X_UNEXPECTED();
        }
    }

    void execute_commands(command_execute_context_t & ctx, command_t ** commands)
    {
        #if EXEC_TRACE
        _PF(_T("~ %1% command classes in total ~"), __COUNTER__);
        _PFC(light_cyan, _T("-> %|1$-50|[%2%]"), _T("<begin>"), ctx.stack.top());
        #endif  // EXEC_TRACE

        ctx.push_calling(commands);

        __exec_env_error_t e = __execute_commands(ctx, ctx.current);
        __process_application_exception(ctx, e);

        ctx.pop_calling();

        #if EXEC_TRACE
        _PFC(light_cyan, _T("-> %|1$-50|[%2%]"), _T("<end>"), ctx.stack.top());
        #endif  // EXEC_TRACE
    }

    ////////// ////////// ////////// ////////// //////////

    #undef __PrintCurrent

}   // namespace X_ROOT_NS::modules::exec
