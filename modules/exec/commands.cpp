#include <exec.h>
#include "commands.h"
#include <rtlib.h>
#include <mm.h>
#include "utils.h"
#include "parser.h"
#include "fusion.h"
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
        __AlwaysInline void execute(command_execute_context_t & _ctx)                   \
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

    #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_INLINE

    // The switch of the INLINE model has a case for each class of the commands: APRIL_CHECK_COMMANDS=1 tells a class that was made and
    // has none (its command would be "unexpected" when it is run).
    static void __check_command_registered(command_value_t value, const char * class_name);

    #endif

    template<typename _cmd_t, typename ... _args_t>
    _cmd_t * __new_command(memory_t * memory, _args_t && ... args)
    {
        _cmd_t * cmd = memory_t::new_obj<_cmd_t>(memory, std::forward<_args_t>(args) ...);

        #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_INLINE
        __check_command_registered(_cmd_t::cmd_value, typeid(_cmd_t).name());
        #endif

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
    __DS(__get_divide_by_zero_exception,    CoreType_DivideByZeroException)
    __DS(__get_overflow_exception,          CoreType_OverflowException)
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
    #define __RaiseDivideByZeroException(_ctx)  __RaiseException(_ctx, divide_by_zero)
    #define __RaiseOverflowException(_ctx)      __RaiseException(_ctx, overflow)

    __AlwaysInline static void __pre_new(command_execute_context_t & ctx, rt_type_t * type);

    ////////// ////////// ////////// ////////// //////////

    // The number type of a xil type: the objects, the strings and the pointers are references.
    template<xil_type_t _xil_type>
    constexpr auto __vnum_type()
    {
        if constexpr (_xil_type == xil_type_t::string || _xil_type == xil_type_t::object
                                                      || _xil_type == xil_type_t::ptr)
            return std::type_identity<rt_ref_t>();
        else
            return std::type_identity<vnum_t<to_vtype(_xil_type)>>();
    }

    template<xil_type_t _xil_type>
    using __vnum_t = typename decltype(__vnum_type<_xil_type>())::type;

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

    // Push commands: the constants, the local variables, the arguments (and their contents), the
    // fields, and the conversions of the value at the top of the stack. The type of the value
    // that is read is _xt1, the type of the value that is pushed is _xt2.
    template<command_value_t _cv, xil_storage_type_t _st, xil_type_t _xt1, xil_type_t _xt2>
    class __push_command_t;

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

    // The number type of a constant: a string is a pointer of chars, an object is a pointer.
    template<xil_type_t _xil_type>
    constexpr auto __const_vnum_type()
    {
        if constexpr (_xil_type == xil_type_t::string)
            return std::type_identity<const rt_char_t *>();
        else if constexpr (_xil_type == xil_type_t::object)
            return std::type_identity<void *>();
        else
            return std::type_identity<__vnum_t<_xil_type>>();
    }

    template<xil_type_t _xil_type>
    using __const_vnum_t = typename decltype(__const_vnum_type<_xil_type>())::type;

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

            // An array is a reference.
            case mt_type_extra_t::array:
                return sizeof(rt_ref_t);

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


    //-------- ---------- ---------- ---------- ----------
    // Push constants

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(push, constant, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    //-------- ---------- ---------- ---------- ----------
    // Push local commands.

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(push, local, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    //-------- ---------- ---------- ---------- ----------
    // Push local address commands.

    __DefineStAddrCmdValue(push, argument_addr)
    __DefineStAddrCmdValue(push, field_addr)
    template<xil_storage_type_t _st> class __push_address_command_t;


    __DefineStAddrCmdValue(push, local_addr)

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

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(push, argument, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    //-------- ---------- ---------- ---------- ----------
    // Push argument address commands.


    //-------- ---------- ---------- ---------- ----------
    // Push argument content.

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(push, argument_content, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    //-------- ---------- ---------- ---------- ----------
    // Push ptr content, pop ptr content: the value of the type is read from, or written to, the address at the
    // top of the stack (a pointer, Ptr<T>.Value). The types are those of the unmanaged values.

    #define __EachPtrContentType()                                                      \
        __CasePtrContent( int8 )    __CasePtrContent( uint8 )                           \
        __CasePtrContent( int16 )   __CasePtrContent( uint16 )                          \
        __CasePtrContent( int32 )   __CasePtrContent( uint32 )                          \
        __CasePtrContent( int64 )   __CasePtrContent( uint64 )                          \
        __CasePtrContent( float_ )  __CasePtrContent( double_ )                         \
        __CasePtrContent( char_ )   __CasePtrContent( bool_ )                           \
        __CasePtrContent( ptr )

    #define __CasePtrContent( _xt )                                                     \
        __DefineStCmdValue(push, ptr_content, _xt, _xt)                                 \
        __DefineStCmdValue(pop,  ptr_content, _xt, _xt)
    __EachPtrContentType()
    #undef __CasePtrContent

    // The conversions between a pointer and an integer: the same bits.
    __DefineStCmdValue(push, convert, ptr,    int64)
    __DefineStCmdValue(push, convert, int64,  ptr)
    __DefineStCmdValue(push, convert, ptr,    uint64)
    __DefineStCmdValue(push, convert, uint64, ptr)

    //-------- ---------- ---------- ---------- ----------
    // Push field commands.

    static bool __is_current_template(__context_t & ctx, rt_type_t * type)
    {
        if (ctx.type->get_kind() != rt_type_kind_t::generic)
            return false;

        rt_general_type_t * template_ = ((rt_generic_type_t *)ctx.type)->template_;
        return is_host_type_template(type, template_);
    }

    // The type of a generic template that is in its own methods (the this of a struct) is the
    // type of the instance that is executing.
    static rt_type_t * __instance_of_template(__context_t & ctx, rt_type_t * type)
    {
        if (is_general(type) && ((rt_general_type_t *)type)->is_generic_template())
            return ctx.to_generic_type((rt_general_type_t *)type);

        return type;
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

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(push, field, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    //-------- ---------- ---------- ---------- ----------
    // Push field address commands.


    // Push address commands: of a local variable, an argument, a field.
    template<xil_storage_type_t _st>
    class __push_address_command_t : public __command_base_t<
        _st == xil_storage_type_t::local_addr?    __StAddrCmd(push, local_addr)
      : _st == xil_storage_type_t::argument_addr? __StAddrCmd(push, argument_addr)
                                                : __StAddrCmd(push, field_addr)
    >
    {
        typedef __push_address_command_t    __self_t;

    public:
        __push_address_command_t(__offset_t offset) : __offset(offset) { }

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("push %1% address [%2%]"), _st, __This->__offset);
        __EndToString()

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            if constexpr (_st == xil_storage_type_t::local_addr)
            {
                ctx.stack.push(__LocalAddress(__This->__offset));
            }
            else if constexpr (_st == xil_storage_type_t::argument_addr)
            {
                ctx.stack.push(__ArgumentAddress(__This->__offset + __stack_stub_size));
            }
            else    // field_addr
            {
                rt_ref_t obj = ctx.stack.pop<rt_ref_t>();
                __CheckNullReference(ctx, obj);

                ctx.stack.push(__FieldAddress(obj, __This->__offset));
            }

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

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(push, convert, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    //-------- ---------- ---------- ---------- ----------
    // Push checked convert commands: OverflowException when the value does not fit the type.

    namespace
    {
        // Returns whether a number fits a numeric type, a floating point number is truncated.
        template<typename _to_t, typename _from_t>
        bool __number_fits(_from_t value)
        {
            if constexpr (std::is_same<_to_t, bool>::value || std::is_same<_from_t, bool>::value)
            {
                return true;
            }
            else if constexpr (std::is_integral<_to_t>::value)
            {
                typedef std::numeric_limits<_to_t> limits_t;

                if constexpr (std::is_floating_point<_from_t>::value)
                {
                    if (value != value)
                        return false;

                    double t = std::trunc((double)value);
                    double lo = limits_t::is_signed? -std::ldexp(1.0, limits_t::digits) : 0.0;
                    double hi = std::ldexp(1.0, limits_t::digits);

                    return t >= lo && t < hi;
                }
                else
                {
                    __int128 v = (__int128)value;
                    return v >= (__int128)limits_t::min() && v <= (__int128)limits_t::max();
                }
            }
            else
            {
                return true;
            }
        }
    }

    // Converts a value: the pointers and the integers have the same bits.
    template<typename _to_t, typename _from_t>
    __AlwaysInline static _to_t __convert_value(_from_t value)
    {
        if constexpr (std::is_same<_from_t, rt_ref_t>::value && std::is_integral<_to_t>::value)
            return (_to_t)(uintptr_t)value.ptr;
        else if constexpr (std::is_same<_to_t, rt_ref_t>::value && std::is_integral<_from_t>::value)
            return rt_ref_t((void *)(uintptr_t)value);
        else
            return static_cast<_to_t>(value);
    }

    template<command_value_t _cv, xil_storage_type_t _st, xil_type_t _xt1, xil_type_t _xt2>
    class __push_command_t : public __command_base_t<_cv>
    {
        typedef __push_command_t    __self_t;

        static constexpr bool __is_constant = _st == xil_storage_type_t::constant;
        static constexpr bool __is_argument = _st == xil_storage_type_t::argument
                                           || _st == xil_storage_type_t::argument_content;
        static constexpr bool __is_convert  = _st == xil_storage_type_t::convert
                                           || _st == xil_storage_type_t::checked_convert;

        // The constants and the fields have the number types of the constants.
        static constexpr bool __const_types = __is_constant || _st == xil_storage_type_t::field;

        typedef std::conditional_t<__const_types, __const_vnum_t<_xt1>, __vnum_t<_xt1>> __value1_t;
        typedef std::conditional_t<__const_types, __const_vnum_t<_xt2>, __vnum_t<_xt2>> __value2_t;

        // The constant, or the offset of the variable. The conversions have no data.
        typedef std::conditional_t<__is_constant, __value1_t, __offset_t> __data_t;

        // The arguments are after the stub of the calling.
        static __data_t __to_data(__data_t data)
        {
            if constexpr (__is_argument)
                return data + __stack_stub_size;
            else
                return data;
        }

    public:
        __push_command_t(__data_t data = __data_t()) : __data(__to_data(data)) { }

        #if EXEC_TRACE

        __BeginToString()

            if constexpr (__is_convert)
                return _F(_T("push %1% %2%=>%3%"), _st, _xt1, _xt2);
            else if constexpr (_xt1 == _xt2)
                return _F(_T("push %1% %2% [%3%]"), _st, _xt1, __This->__data);
            else
                return _F(_T("push %1% %2%=>%3% [%4%]"), _st, _xt1, _xt2, __This->__data);

        __EndToString()

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            if constexpr (_st == xil_storage_type_t::constant)
            {
                ctx.stack.push(static_cast<__value2_t>(__This->__data));
            }
            else if constexpr (_st == xil_storage_type_t::local)
            {
                ctx.stack.push(static_cast<__value2_t>(__Local(__value1_t, __This->__data)));
            }
            else if constexpr (_st == xil_storage_type_t::argument)
            {
                ctx.stack.push(static_cast<__value2_t>(__Argument(__value1_t, __This->__data)));
            }
            else if constexpr (_st == xil_storage_type_t::argument_content)
            {
                ctx.stack.push(static_cast<__value2_t>(
                    *(__value1_t *)__Argument(void *, __This->__data)
                ));
            }
            else if constexpr (_st == xil_storage_type_t::ptr_content)
            {
                // The address is not checked: it is what the program says.
                rt_ref_t address = ctx.stack.pop<rt_ref_t>();
                ctx.stack.push(static_cast<__value2_t>(*(__value1_t *)address.ptr));
            }
            else if constexpr (_st == xil_storage_type_t::field)
            {
                rt_ref_t obj = ctx.stack.pop<rt_ref_t>();
                __CheckNullReference(ctx, obj);

                ctx.stack.push(static_cast<__value2_t>(__Field(__value1_t, obj, __This->__data)));
            }
            else if constexpr (_st == xil_storage_type_t::convert)
            {
                if constexpr (!std::is_same<__value1_t, __value2_t>::value)
                {
                    ctx.stack.push(__convert_value<__value2_t>(ctx.stack.pop<__value1_t>()));
                }
            }
            else    // checked_convert: OverflowException when the value does not fit the type.
            {
                if constexpr (!std::is_same<__value1_t, __value2_t>::value)
                {
                    __value1_t value = ctx.stack.pop<__value1_t>();

                    if constexpr (std::is_arithmetic<__value1_t>::value
                                    && std::is_arithmetic<__value2_t>::value)
                    {
                        if (!__number_fits<__value2_t>(value))
                            __RaiseOverflowException(ctx);
                    }

                    ctx.stack.push(static_cast<__value2_t>(value));
                }
            }

        __EndExecute()

    private:
        __data_t __data;
    };

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(push, checked_convert, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    template<xil_storage_type_t _st>
    struct __push_convert_command_template_t
    {
        template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2, typename ... _args_t>
        static auto new_command(memory_t * memory, _args_t && ... args)
        {
            typedef __push_command_t<_cv, _st, _xt1, _xt2> this_command_t;
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
                    if constexpr (_d == 0)
                        return 0;
                    else
                        return stack.pop<__l_t>() * (*ls) + __mul_t<_d - 1>()(stack, ls - 1);
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

    // The commands of the elements of arrays: the element is pushed, or the value at the top of the
    // stack is written to it, it is popped (pop) or it stays in the stack (pick). The type of
    // the elements is _xt1, the type of the value is _xt2. The dimension 0 is for the arrays that
    // have more dimensions than the ones that are known: the dimension is given to the command.
    enum class __array_element_op_t { push, pop, pick };

    template<command_value_t _cv, __array_element_op_t _op, xil_type_t _xt1, xil_type_t _xt2,
            dimension_t _dimension>
    class __array_element_command_t : public __command_base_t<_cv>
    {
        typedef __array_element_command_t   __self_t;
        typedef __vnum_t<_xt1>              __value1_t;
        typedef __vnum_t<_xt2>              __value2_t;

    public:

        __array_element_command_t(dimension_t dimension = 0) : __dimension(dimension) { }

        #if EXEC_TRACE

        __ToString(_op == __array_element_op_t::push? _T("push array element")
                 : _op == __array_element_op_t::pop?  _T("pop array element")
                                                     : _T("pick array element"));

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            rt_ref_t array_ref = ctx.stack.pop<rt_ref_t>();
            __CheckNullReference(ctx, array_ref);
            array_length_t index = ctx.stack.pop<array_length_t>();

            if constexpr (_dimension == 0)
            {
                array_length_t * lengths = mm::get_array_lengths(array_ref);
                index += __array_index(ctx.stack, __This->__dimension - 1, lengths);
            }
            else if constexpr (_dimension >= 2)
            {
                array_length_t * lengths = mm::get_array_lengths(array_ref);
                index += __mul_array_index<_dimension - 1>(ctx.stack, lengths);
            }

            if ((uint32_t)index >= (uint32_t)mm::get_array_length(array_ref))
                __RaiseOutOfRangeException(ctx);

            if constexpr (_op == __array_element_op_t::push)
            {
                ctx.stack.push<__value2_t>(static_cast<__value2_t>(
                    mm::get_array_element<__value1_t>(array_ref, index)
                ));
            }
            else if constexpr (_op == __array_element_op_t::pop)
            {
                mm::set_array_element<__value1_t>(
                    array_ref, index, static_cast<__value1_t>(ctx.stack.pop<__value2_t>())
                );
            }
            else
            {
                mm::set_array_element<__value1_t>(
                    array_ref, index, static_cast<__value1_t>(ctx.stack.pick<__value2_t>())
                );
            }

        __EndExecute()

    private:
        dimension_t __dimension;
    };

    template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2, dimension_t _dimension>
    using __push_array_element_command_t =
        __array_element_command_t<_cv, __array_element_op_t::push, _xt1, _xt2, _dimension>;

    template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2, dimension_t _dimension>
    using __pop_array_element_command_t =
        __array_element_command_t<_cv, __array_element_op_t::pop, _xt1, _xt2, _dimension>;

    template<command_value_t _cv, xil_type_t _xt1, xil_type_t _xt2, dimension_t _dimension>
    using __pick_array_element_command_t =
        __array_element_command_t<_cv, __array_element_op_t::pick, _xt1, _xt2, _dimension>;

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
    // Pop the element of an array that is a custom struct, when the code does not know it (Array<T>.this[] = value, the type is a
    // generic param): the stack has the bytes of the value (copied to a place of the stack by the argument), the index and the
    // array; the bytes are copied to the element and popped.

    #define __PopStructElementCmd   __Cmd(pop, struct_array_element)
    __DefineCmdValue_(__PopStructElementCmd)

    class __pop_struct_element_command_t : public __command_base_t<__PopStructElementCmd>
    {
        typedef __pop_struct_element_command_t  __self_t;

    public:
        __pop_struct_element_command_t(msize_t size) : __size(size) { }

        #if EXEC_TRACE

        __ToString(_T("pop struct array element"));

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            rt_ref_t array_ref = ctx.stack.pop<rt_ref_t>();
            __CheckNullReference(ctx, array_ref);
            array_length_t index = ctx.stack.pop<array_length_t>();

            if ((uint32_t)index >= (uint32_t)mm::get_array_length(array_ref))
                __RaiseOutOfRangeException(ctx);

            size_t units = _alignf(__This->__size, sizeof(rt_stack_unit_t)) / sizeof(rt_stack_unit_t);

            al::quick_copy((void *)(array_ref + index * __This->__size), (void *)(ctx.stack.top() - units), __This->__size);
            ctx.stack.pop(units);

        __EndExecute()

    private:
        msize_t __size;
    };

    static rt_type_t * __array_element_type(__context_t & ctx, ref_t type_ref);

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

        // The dimension 0 is for the arrays that have more dimensions than the ones that are
        // known: the dimension is given to the command.
        __push_array_element_address_command_t(dimension_t dimension, msize_t element_size)
            : __dimension(dimension), __element_size(element_size)
        { }

        #if EXEC_TRACE

            __ToString(_T("push array element address"));

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            rt_ref_t array_ref = ctx.stack.pop<rt_ref_t>();
            __CheckNullReference(ctx, array_ref);
            array_length_t index = ctx.stack.pop<array_length_t>();

            if constexpr (_dimension == 0)
            {
                array_length_t * lengths = mm::get_array_lengths(array_ref);
                index += __array_index(ctx.stack, __This->__dimension - 1, lengths);
            }
            else if constexpr (_dimension >= 2)
            {
                array_length_t * lengths = mm::get_array_lengths(array_ref);
                index += __mul_array_index<_dimension - 1>(ctx.stack, lengths);
            }

            if ((uint32_t)index >= (uint32_t)mm::get_array_length(array_ref))
                __RaiseOutOfRangeException(ctx);

            ctx.stack.push(array_ref + index * __element_size);

        __EndExecute()

    private:
        dimension_t __dimension = 0;
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
                (rt_stack_unit_t *)ctx.stack.lp() - __This->__offset - __stack_stub_size
            );

        __EndExecute()

    private:
        __offset_t __offset;
    };

    //-------- ---------- ---------- ---------- ----------
    // Push params values: the units of the items of the params generic parameter are copied to
    // the top of the stack, they are the arguments of the call.

    #define __PushParamsValuesCmd  __Cmd(push, params_values)
    __DefineCmdValue_(__PushParamsValuesCmd)

    class __push_params_values_command_t : public __command_base_t<__PushParamsValuesCmd>
    {
        typedef __push_params_values_command_t __self_t;

    public:
        __push_params_values_command_t(__offset_t offset)
            : __offset(offset)
        { }

        #if EXEC_TRACE

        __ToString(_T("push params values"));

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            // The items are the last params: they are all that is between the first of them and
            // the top of the params.
            rt_stack_unit_t * units = (rt_stack_unit_t *)ctx.stack.lp()
                                                    - __This->__offset - __stack_stub_size;

            for (__offset_t k = 0; k < __This->__offset; k++)
                ctx.stack.push<rt_stack_unit_t>(units[k]);

        __EndExecute()

    private:
        __offset_t __offset;
    };

    //-------- ---------- ---------- ---------- ----------
    // Push tuple values: the tuple is popped, the items of it are pushed, one after the other, as
    // the arguments of a call (f(tuple ...)): each is the units of its size, the layout of the
    // arguments of the items of a params generic parameter.

    #define __PushTupleValuesCmd  __Cmd(push, tuple_values)
    __DefineCmdValue_(__PushTupleValuesCmd)

    class __push_tuple_values_command_t : public __command_base_t<__PushTupleValuesCmd>
    {
        typedef __push_tuple_values_command_t __self_t;

    public:
        __push_tuple_values_command_t(rt_type_t * type) : __type(type) { }

        #if EXEC_TRACE

        __ToString(_T("push tuple values"));

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            byte_t * object = (byte_t *)(void *)ctx.stack.pop<rt_ref_t>();
            __CheckNullReference(ctx, object);

            rt_generic_type_t * tuple = (rt_generic_type_t *)__This->__type;
            for (int k = 0, count = tuple->atype_count(); k < count; k++)
            {
                rt_type_t * item_type = tuple->atypes[k];
                msize_t offset = tuple->get_field_offset(ctx.env, k);
                msize_t size = item_type->get_variable_size(ctx.env);

                rt_stack_unit_t * units = ctx.stack.alloc(size);
                memset((void *)units, 0, _alignf(size, sizeof(rt_stack_unit_t)));
                memcpy((void *)units, object + offset, size);
            }

        __EndExecute()

    private:
        rt_type_t * __type;
    };

    struct __push_tuple_values_template_t
    {
        template<command_value_t _cv, typename ... _args_t>
        static auto new_command(memory_t * memory, _args_t && ... args)
        {
            typedef __push_tuple_values_command_t this_command_t;
            return __new_command<this_command_t>(memory, std::forward<_args_t>(args) ...);
        }
    };

    struct __push_params_values_template_t
    {
        template<command_value_t _cv, typename ... _args_t>
        static auto new_command(memory_t * memory, _args_t && ... args)
        {
            typedef __push_params_values_command_t this_command_t;
            return __new_command<this_command_t>(memory, std::forward<_args_t>(args) ...);
        }
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

    // The number type of the value of a box: a struct that is not an internal type is copied.
    template<msize_t _size>
    constexpr auto __box_value_type()
    {
        if constexpr (_size == __CustomStructSize)
            return std::type_identity<void *>();
        else
            return std::type_identity<uint_type_t<_size>>();
    }

    template<command_value_t _cv, msize_t _size, xil_box_type_t _box_type>
    class __push_box_command_t : public __command_base_t<_cv>
    {
        typedef __push_box_command_t    __self_t;
        typedef typename decltype(__box_value_type<_size>())::type  __value_t;

        static constexpr bool __is_struct = _size == __CustomStructSize;

    public:
        // The size is that of the struct, if it is a custom one.
        __push_box_command_t(rt_type_t * type, msize_t size = 0) : __type(type), __size(size) { }

        #if EXEC_TRACE

        __ToString(_FT("box [%1%]", _box_type));

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            if constexpr (__is_struct)
            {
                void * addr = __fetch_box_addr<_box_type, void *>(ctx);

                __pre_new(ctx, __type);
                rt_ref_t obj = ctx.heap->new_obj(__type);

                al::quick_copy((void *)obj, addr, __size);
                ctx.stack.push<rt_ref_t>(obj);
            }
            else
            {
                __value_t value = __fetch_box_addr<_box_type, __value_t>(ctx);

                __pre_new(ctx, __type);
                rt_ref_t obj = ctx.heap->new_obj(__type);

                *(__value_t *)(void *)obj = value;
                ctx.stack.push<rt_ref_t>(obj);
            }

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
        typedef typename decltype(__box_value_type<_size>())::type  __value_t;

        static constexpr bool __is_struct = _size == __CustomStructSize;

    public:
        // The size is that of the struct, if it is a custom one.
        __push_unbox_command_t(rt_type_t * type, msize_t size = 0) : __type(type), __size(size) { }

        #if EXEC_TRACE

        __ToString(_FT("unbox [%1%]", _box_type));

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            rt_ref_t obj = __fetch_box_addr<_box_type, rt_ref_t>(ctx);
            __CheckNullReference(ctx, obj);

            rt_type_t * type = __RtTypeOf(obj);
            if (type != __type)
                __RaiseInvalidCastException(ctx);

            if constexpr (__is_struct)
            {
                void * val = ctx.stack.pop<void *>();
                al::quick_copy(val, (void *)obj, __size);
            }
            else
            {
                ctx.stack.push<__value_t>(*(__value_t *)(void *)obj);
            }

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

    // Pushes the object that holds the thread static fields of a type, for the running thread.
    #define __PushThreadStaticCmd  __Cmd(push, thread_static)
    __DefineCmdValue_(__PushThreadStaticCmd)

    class __push_thread_static_command_t : public __command_base_t<__PushThreadStaticCmd>
    {
        typedef __push_thread_static_command_t __self_t;

    public:
        __push_thread_static_command_t(rt_type_t * type) : __type(type) { }

        __BeginExecute(ctx)

            ctx.stack.push((void *)ctx.env.thread_static_data_of(__This->__type));

        __EndExecute()

        #if EXEC_TRACE

        __ToString(_T("push thread static"))

        #endif  // EXEC_TRACE

    private:
        rt_type_t * __type;
    };

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

        // The method of a generic type (a lambda, or a method that is a delegate, in Box<T>) is
        // the one of the instance that is running: Box<Int32>.
        if (method->this_type() == rt_member_type_t::general)
        {
            rt_type_t * host = method->get_host_type();
            if (host != nullptr && is_general(host)
                    && ((rt_general_type_t *)host)->is_generic_template())
            {
                rt_type_t * instance = ctx.to_generic_type((rt_general_type_t *)host);
                if (instance != nullptr && instance != host)
                    method = ctx.to_generic_method((rt_method_t *)method, instance);
            }
        }

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

            case xil_storage_object_type_t::static_data: {
                // The static fields of a generic type are those of its instance: Box<Int32> and
                // Box<String> have their own.
                rt_type_t * type = __instance_of_template(ctx, ctx.get_type(ref));
                #if EXEC_TRACE
                *out_name = type->get_name(ctx.env);
                #endif
                object = (rt_object_t *)(void *)ctx.env.static_data_of(type);
            }   break;

            // The object of a thread is not known when the commands are made: the command asks it each time, the object
            // that is given here is the type.
            case xil_storage_object_type_t::thread_static_data: {
                rt_type_t * type = __instance_of_template(ctx, ctx.get_type(ref));
                #if EXEC_TRACE
                *out_name = type->get_name(ctx.env);
                #endif
                object = (rt_object_t *)(void *)type;
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

        // The this of a method of String is a string, it is pushed as a reference (an object): the
        // value is the same.
        if (*out_dtype1 == xil_type_t::string && *out_dtype2 == xil_type_t::object)
            *out_dtype2 = xil_type_t::string;
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

            // tuple_values.
            struct
            {
                rt_type_t *     type;
            } tuple;
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

        static __CommandManagerT(const rt_char_t *) __string_push_command_manager;
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
            __push_convert_command_template_t<xil_storage_type_t::convert>,
            xil_type_t, xil_type_t
        >::with_args_t<> __convert_command_manager;

        static __command_manager_t<
            __push_convert_command_template_t<xil_storage_type_t::checked_convert>,
            xil_type_t, xil_type_t
        >::with_args_t<> __checked_convert_command_manager;

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

            case xil_storage_type_t::params_values: {   // Push params values command.

                static __command_manager_t<
                    __push_params_values_template_t
                >::with_args_t<__offset_t> __params_values_command_manager;

                return __params_values_command_manager.template get_command<__PushParamsValuesCmd>(
                    args.params.offset
                );
            }

            case xil_storage_type_t::tuple_values: {    // Push tuple values command.

                static __command_manager_t<
                    __push_tuple_values_template_t
                >::with_args_t<rt_type_t *> __tuple_values_command_manager;

                return __tuple_values_command_manager.template get_command<__PushTupleValuesCmd>(
                    args.tuple.type
                );
            }

            case xil_storage_type_t::object: {       // Push object command.

                if (args.object.type == xil_storage_object_type_t::thread_static_data)
                    return __new_command<__push_thread_static_command_t>(&__command_heap, (rt_type_t *)(void *)args.object.object);

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
            (args.constant.dtype == xil_type_t::empty? default_value_of<const rt_char_t *>()   \
                : ctx.to_cstr(args.constant.get_data<res_t>()))

        // The this of a struct is pushed as a pointer: an argument of an object type is the same
        // stack unit.
        if (stype == xil_storage_type_t::argument && xt1 == xil_type_t::object
                                                  && xt2 == xil_type_t::ptr)
        {
            xt1 = xil_type_t::ptr;
        }

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
            // Push ptr content

            #define __CasePtrContent(_xt)                                               \
                case __V(xil_storage_type_t::ptr_content, xil_type_t::_xt, xil_type_t::_xt): \
                    return __command_manager.template get_command<                      \
                        __StCmd(push, ptr_content, _xt, _xt),                           \
                        xil_storage_type_t::ptr_content, xil_type_t::_xt, xil_type_t::_xt \
                    >(0);

            __EachPtrContentType()

            #undef __CasePtrContent

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
            __CaseConvert(ptr,    int64)
            __CaseConvert(int64,  ptr)
            __CaseConvert(ptr,    uint64)
            __CaseConvert(uint64, ptr)

            // Push Checked Convert

            #define __CaseCheckedConvert(_xt1, _xt2)                                    \
                case __V(xil_storage_type_t::checked_convert, xil_type_t::_xt1, xil_type_t::_xt2):  \
                    return __checked_convert_command_manager.template get_command<      \
                        __StCmd(push, checked_convert, _xt1, _xt2),                     \
                        xil_type_t::_xt1, xil_type_t::_xt2                              \
                    >();

            #define __CaseCheckedConverts(_xt2)                                         \
                __CaseCheckedConvert(int8,       _xt2)                                  \
                __CaseCheckedConvert(uint8,      _xt2)                                  \
                __CaseCheckedConvert(int16,      _xt2)                                  \
                __CaseCheckedConvert(uint16,     _xt2)                                  \
                __CaseCheckedConvert(int32,      _xt2)                                  \
                __CaseCheckedConvert(uint32,     _xt2)                                  \
                __CaseCheckedConvert(int64,      _xt2)                                  \
                __CaseCheckedConvert(uint64,     _xt2)                                  \
                __CaseCheckedConvert(float_,     _xt2)                                  \
                __CaseCheckedConvert(double_,    _xt2)                                  \
                __CaseCheckedConvert(char_,      _xt2)                                  \
                __CaseCheckedConvert(bool_,      _xt2)

            __CaseCheckedConverts(int8)
            __CaseCheckedConverts(uint8)
            __CaseCheckedConverts(int16)
            __CaseCheckedConverts(uint16)
            __CaseCheckedConverts(int32)
            __CaseCheckedConverts(uint32)
            __CaseCheckedConverts(int64)
            __CaseCheckedConverts(uint64)
            __CaseCheckedConverts(float_)
            __CaseCheckedConverts(double_)
            __CaseCheckedConverts(char_)
            __CaseCheckedConverts(bool_)
            __CaseCheckedConvert(object, object)
            __CaseCheckedConvert(string, string)
            __CaseCheckedConvert(ptr,    ptr)

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

        // sizeof(T): a constant of the size of the type (of the instance when it is a generic param).
        int64_t type_size_value = 0;

        switch (args.stype)
        {
            case xil_storage_type_t::type_size:
                type_size_value = (int64_t)ctx.size_of(xil.get_ref());

                args.stype = xil_storage_type_t::constant;
                args.constant.dtype = xil_type_t::int64;
                args.constant.data  = &type_size_value;
                xt1 = xt2 = xil_type_t::int64;
                break;

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
            case xil_storage_type_t::params_values:
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

            case xil_storage_type_t::ptr_content:
                xt1 = xt2 = xil.dtype();
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
            case xil_storage_type_t::checked_convert:
                xt1 = xil.dtype();
                xt2 = xil.dtype2();
                break;

            case xil_storage_type_t::box:
                args.box.type     = __instance_of_template(ctx, ctx.get_type(xil.get_ref()));
                args.box.box_type = xil.get_box_type();
                break;

            case xil_storage_type_t::unbox:
                args.unbox.type     = __instance_of_template(ctx, ctx.get_type(xil.get_ref()));
                args.unbox.box_type = xil.get_box_type();
                break;

            case xil_storage_type_t::tuple_values:
                args.tuple.type = __instance_of_template(ctx, ctx.get_type(xil.get_ref()));
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

    // Pop commands, and pick commands: the value at the top of the stack is written to a local
    // variable, an argument (or its address), or a field. A pick command leaves it in the
    // stack. The type of the variable is _xt1, the type of the value is _xt2.
    template<command_value_t _cv, bool _pick, xil_storage_type_t _st,
                                                xil_type_t _xt1, xil_type_t _xt2>
    class __store_command_t : public __command_base_t<_cv>
    {
        typedef __store_command_t   __self_t;
        typedef __vnum_t<_xt1>      __value1_t;
        typedef __vnum_t<_xt2>      __value2_t;

        static constexpr bool __is_argument = _st == xil_storage_type_t::argument
                                           || _st == xil_storage_type_t::argument_addr;

        // The arguments are after the stub of the calling.
        static __offset_t __to_offset(__offset_t offset)
        {
            if constexpr (__is_argument)
                return offset + __stack_stub_size;
            else
                return offset;
        }

        // The value at the top of the stack.
        static __value1_t __value(command_execute_context_t & ctx)
        {
            if constexpr (_pick)
                return static_cast<__value1_t>(ctx.stack.pick<__value2_t>());
            else
                return static_cast<__value1_t>(ctx.stack.pop<__value2_t>());
        }

    public:
        __store_command_t(__offset_t offset) : __offset(__to_offset(offset)) { }

        #if EXEC_TRACE

        __BeginToString()

            if constexpr (_xt1 == _xt2)
                return _F(_T("%1% %2% %3% [%4%]"), _pick? _T("pick") : _T("pop "), _st, _xt1,
                                                                            __This->__offset);
            else
                return _F(_T("%1% %2% %3%=>%4% [%5%]"), _pick? _T("pick") : _T("pop "), _st,
                                                        _xt2, _xt1, __This->__offset);

        __EndToString()

        #endif  // EXEC_TRACE

        __BeginExecute(ctx)

            if constexpr (_st == xil_storage_type_t::local)
            {
                __Local(__value1_t, __This->__offset) = __value(ctx);
            }
            else if constexpr (_st == xil_storage_type_t::argument)
            {
                __Argument(__value1_t, __This->__offset) = __value(ctx);
            }
            else if constexpr (_st == xil_storage_type_t::argument_addr)
            {
                *(__value1_t *)__Argument(void *, __This->__offset) = __value(ctx);
            }
            else if constexpr (_st == xil_storage_type_t::ptr_content)
            {
                // The value is at the top, the address is below it.
                __value1_t value = __value(ctx);
                rt_ref_t address = ctx.stack.pop<rt_ref_t>();

                *(__value1_t *)address.ptr = value;
            }
            else    // field
            {
                rt_ref_t obj = ctx.stack.pop<rt_ref_t>();
                __CheckNullReference(ctx, obj);

                void * field = __FieldAddress(obj, __This->__offset);
                *(__value1_t *)field = __value(ctx);
            }

        __EndExecute()

    private:
        __offset_t __offset;
    };

    template<command_value_t _cv, xil_storage_type_t _st, xil_type_t _xt1, xil_type_t _xt2>
    using __pop_command_t = __store_command_t<_cv, false, _st, _xt1, _xt2>;

    template<command_value_t _cv, xil_storage_type_t _st, xil_type_t _xt1, xil_type_t _xt2>
    using __pick_command_t = __store_command_t<_cv, true, _st, _xt1, _xt2>;

    //-------- ---------- ---------- ---------- ----------


    //-------- ---------- ---------- ---------- ----------
    // Pop local commands

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(pop, local, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    //-------- ---------- ---------- ---------- ----------
    // Pop argument commands

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(pop, argument, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    //-------- ---------- ---------- ---------- ----------
    // Pop argument_addr commands

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(pop, argument_addr, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    //-------- ---------- ---------- ---------- ----------

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
        // The result of a call whose type is a generic param: nothing is left when the type is a
        // custom struct (the callee writes it where the caller said), otherwise the address that
        // was pushed for it and the value.
        if (xil.is_call_result())
        {
            rt_type_t * type = ctx.get_type(xil.get_ref());
            if (is_custom_struct(ctx.env, type))
                return 0;

            return 1 + unit_size_of(ctx.size_of(xil.get_ref()));
        }

        return unit_size_of(__size_of_pop_empty(ctx, xil));
    }

    static command_t * __new_pop_command(__context_t & ctx, const pop_xil_t & xil)
    {
        // An element that is a custom struct: it is copied by its bytes.
        if (xil.stype() == xil_storage_type_t::array_element)
        {
            rt_type_t * element_type = __array_element_type(ctx, xil.get_ref());
            if (is_custom_struct(ctx.env, element_type))
            {
                return __new_command<__pop_struct_element_command_t>(
                    &__command_heap, element_type->get_size(ctx.env)
                );
            }
        }

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
                        __StCmd(pop, argument_addr, _xt1, _xt2),                        \
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
            // Pop ptr content

            #define __CasePtrContent(_xt)                                               \
                case __V(xil_storage_type_t::ptr_content, xil_type_t::_xt, xil_type_t::_xt): \
                    return __command_manager.template get_command<                      \
                        __StCmd(pop, ptr_content, _xt, _xt),                            \
                        xil_storage_type_t::ptr_content, xil_type_t::_xt, xil_type_t::_xt \
                    >((msize_t)0);

            __EachPtrContentType()

            #undef __CasePtrContent

            // - - - - - - - - - - - - - - - - - - - - - - - - - -

            default:
                X_UNEXPECTED_F("unexpected pop data types: %1% %2% %3%", stype, dtype1, dtype2);
        }

        X_UNEXPECTED();

        #undef __V
    }

    ////////// ////////// ////////// ////////// //////////
    // Class __pick_command_t


    //-------- ---------- ---------- ---------- ----------


    //-------- ---------- ---------- ---------- ----------
    // Pick local commands

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(pick, local, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    //-------- ---------- ---------- ---------- ----------
    // Pick argument commands

    #define __CaseXilTypePair( _xt1, _xt2 ) __DefineStCmdValue(pick, argument, _xt1, _xt2)
    __EachXilTypePairs()
    #undef __CaseXilTypePair

    //-------- ---------- ---------- ---------- ----------
    // Pick field commands

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
        __internal_call_command_t(__libfunc_t func, msize_t this_offset, int param_unit_size,
                                  int ret_unit_size
    #if EXEC_TRACE
            , rt_sid_t method_name
    #endif  // EXEC_TRACE
        )
            : __func(func), __this_offset(this_offset), __param_unit_size(param_unit_size)
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

            context.heap = ctx.heap;
            context.exec_env = &ctx.env;
            context.caller_this = __This->__this_offset == __no_this_offset? nullptr
                    : *(void **)((rt_stack_unit_t *)ctx.stack.lp() - __This->__this_offset
                                                               - __stack_stub_size);

            __This->__func(context);

            if (context.ret_code == rtlib::rtlib_ret_null_reference)
                __RaiseException(ctx, null_reference);

            if (context.ret_code == rtlib::rtlib_ret_invalid_cast)
                __RaiseInvalidCastException(ctx);

            // The function has fewer arguments than its result has units (Time_UtcNowTicks()): the result is above the
            // top, and the stack grows by it.
            if (__This->__pop_unit_size >= 0)
                ctx.stack.pop((size_t)__This->__pop_unit_size);
            else
                ctx.stack.alloc_units((size_t)(-__This->__pop_unit_size));

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("call internal %1%"), __This->__method_name);
        __EndToString()

        #endif

    private:

        static constexpr msize_t __no_this_offset = (msize_t)-1;

        msize_t __this_offset;
        int __param_unit_size, __pop_unit_size;
        __libfunc_t __func;

        #if EXEC_TRACE
        rt_sid_t __method_name;    
        #endif
    };

    struct __internal_call_command_template_t
    {
        template<command_value_t _cv, typename ... _args_t>
        static auto new_command(memory_t * memory, __libfunc_t libfunc, msize_t this_offset,
                        __context_t & ctx, rt_method_t * rt_method)
        {
            typedef __internal_call_command_t this_command_t;

            int param_unit_size, ret_unit_size;
            __get_method_unit_size(ctx, rt_method, &param_unit_size, &ret_unit_size);

            this_command_t * cmd = __new_command<this_command_t>(
                memory, libfunc, this_offset, param_unit_size, ret_unit_size
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

    // The command of Reflection_Invoke: it calls a method that is made of the reflection.
    static command_t * __new_reflection_invoke_command(__context_t & ctx);

    static command_t * __new_internal_call_command(__context_t & ctx, const call_xil_t & xil)
    {
        static __command_manager_t<
            __internal_call_command_template_t
        >::with_args_t<__libfunc_t, msize_t> __internal_call_command_manager;

        ref_t method_ref = (ref_t)xil.method;

        rt_method_t * rt_method = _M(rt_method_t *, ctx.get_method(method_ref));
        rt_sid_t method_name = rt_method->get_name();

        // A function that is not of the library: it calls methods, the library cannot.
        if (string_t(method_name) == _T("Reflection_Invoke"))
            return __new_reflection_invoke_command(ctx);

        __libfunc_t libfunc = rtlib::get_libfunc(method_name);
        if (libfunc == nullptr)
            throw _ED(exec_error_code_t::internal_function_not_found, method_name);

        // The this of the calling method: a function can use it (the constructor of a tuple).
        msize_t this_offset = (msize_t)-1;
        if (ctx.params_layout.unit_size() > 0)
            this_offset = __offset_of_this_argument(ctx);

        command_t * cmd = __internal_call_command_manager.template
            get_command<__CallCmd(internal)>(libfunc, this_offset, ctx, rt_method);

        return cmd;
    }

    //-------- ---------- ---------- ---------- ----------

    // The methods that only read or write a field of this (the accessors of the properties): the call does it without a frame.
    // kind 1: push this, push field, ret. kind 2: push value, push this, pop field, ret. See analyze_trivial_method.
    #define __TRIVIAL_TYPES(_m)                                                             \
        _m(int8) _m(uint8) _m(int16) _m(uint16) _m(int32) _m(uint32) _m(int64) _m(uint64)   \
        _m(float_) _m(double_) _m(char_) _m(bool_) _m(object) _m(string)

    #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL

    // A virtual or interface call that counts what it goes to (exec_call_profile_t).
    class __osr_call_t
    {
    public:
        exec_call_profile_t * profile = nullptr;

        // Where the object of the call is, from the top of the stack.
        virtual msize_t receiver_offset() = 0;
    };

    // Counts a call. When the call has been made many times, the second version of the commands of the method is made, and the execution
    // goes on in it from the call (true: the call is not made, the commands are the new ones).
    static bool __osr_record(command_execute_context_t & ctx, exec_call_profile_t * p, exec_method_t * method)
    {
        // The type of the object: the method is the one of the type, and the check of the second version is the type.
        rt_type_t * type = __RtTypeOf(*(rt_ref_t *)(ctx.stack.top() - p->this_offset));

        if (p->type == type)
            p->hits++;
        else if (p->type == nullptr)
            p->type = type, p->target = method, p->hits = 1;
        else
            p->misses++;

        exec_osr_t * osr = p->osr;
        int state = osr->state.load(std::memory_order_relaxed);

        if (state == 0 && p->hits + p->misses >= (uint32_t)exec_osr_threshold())
        {
            if (!p->owner->osr_upgrade())
                return false;

            state = 2;
        }

        if (state != 2)
            return false;

        ctx.current = osr->commands + osr->command_of_xil[p->orig_index];
        return true;
    }

    // The call of Invoke of a delegate that counts the method that the delegate has (the same as the record of the other calls): true
    // when the execution goes on in the second version of the commands.
    static bool __osr_record_delegate(command_execute_context_t & ctx, exec_call_profile_t * p, rt_method_base_t * method,
                                      exec_method_t * target, bool is_static)
    {
        if (p->delegate_method == method)
            p->hits++;
        else if (p->delegate_method == nullptr)
            p->delegate_method = method, p->target = target, p->delegate_static = is_static, p->hits = 1;
        else
            p->misses++;

        exec_osr_t * osr = p->osr;
        int state = osr->state.load(std::memory_order_relaxed);

        if (state == 0 && p->hits + p->misses >= (uint32_t)exec_osr_threshold())
        {
            if (!p->owner->osr_upgrade())
                return false;

            state = 2;
        }

        if (state != 2)
            return false;

        ctx.current = osr->commands + osr->command_of_xil[p->orig_index];
        return true;
    }

    #define __OsrBase           , public __osr_call_t

    #else

    #define __OsrBase

    #endif  // EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL

    // The call of Invoke of a delegate (kind 4): it goes to the method that the delegate has, without the frame of Invoke.
    static void __delegate_invoke_direct(command_execute_context_t & ctx, exec_method_t * invoke);

    // Counts the call of a delegate (the second version of the commands may be made, and the execution goes on in it: true).
    static bool __delegate_osr(command_execute_context_t & ctx, exec_method_t * invoke, exec_call_profile_t * profile);

    static void __trivial_call(command_execute_context_t & ctx, exec_method_t * method)
    {
        if (method->trivial_kind.load(std::memory_order_relaxed) == 4)
        {
            __delegate_invoke_direct(ctx, method);
            return;
        }

        // A method that does nothing (the constructor of Object, and those that only call one): this is popped.
        if (method->trivial_kind.load(std::memory_order_relaxed) == 3)
        {
            ctx.stack.pop<rt_ref_t>();
            return;
        }

        byte_t * self = (byte_t *)*(rt_ref_t *)(ctx.stack.top() - method->this_offset);
        const int32_t offset = method->trivial_offset;
        const xil_type_t type = (xil_type_t)method->trivial_type;

        if (method->trivial_kind.load(std::memory_order_relaxed) == 1)
        {
            // The value replaces this.
            switch (type)
            {
                #define __Get(_a)                                                               \
                    case xil_type_t::_a:                                                        \
                        ctx.stack.replace<__vnum_t<xil_type_t::_a>>(*(__vnum_t<xil_type_t::_a> *)(self + offset)); \
                        break;

                __TRIVIAL_TYPES(__Get)

                #undef __Get

                default:
                    X_UNEXPECTED();
            }
        }
        else
        {
            // The value is at the top, this is below it.
            switch (type)
            {
                #define __Set(_a)                                                               \
                    case xil_type_t::_a: {                                                      \
                        __vnum_t<xil_type_t::_a> value = ctx.stack.pop<__vnum_t<xil_type_t::_a>>(); \
                        ctx.stack.pop<rt_ref_t>();                                              \
                        *(__vnum_t<xil_type_t::_a> *)(self + offset) = value;                   \
                    }   break;

                __TRIVIAL_TYPES(__Set)

                #undef __Set

                default:
                    X_UNEXPECTED();
            }
        }
    }

    bool exec_call_stats_on = std::getenv("APRIL_CALL_STATS") != nullptr;

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
            if (exec_call_stats_on) exec_call_count(method, 0);
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

    // The command of a static call of a runtime method.
    static command_t * __new_static_call_command_of(__context_t & ctx, rt_method_base_t * method);

    command_t * new_static_call_command(__context_t & ctx, ref_t method_ref)
    {
        switch ((mt_member_extra_t)method_ref.extra)
        {
            case mt_member_extra_t::generic: {

                static __command_manager_t<
                    __static_call_command_template_t<rt_generic_method_t>
                >::with_args_t<rt_generic_method_t *> __static_generic_call_command_manager;

                auto * rt_generic_method = _M(rt_generic_method_t *, ctx.get_method(method_ref));

                // Through a generic param: the method of the type that it is.
                if (rt_method_base_t * target = resolve_constrained_static(ctx.env, rt_generic_method))
                    return __new_static_call_command_of(ctx, target);

                return __static_generic_call_command_manager.template
                    get_command<__CallCmd_(static_, generic)>(rt_generic_method, ctx);

            }   break;

            case mt_member_extra_t::import:
            case mt_member_extra_t::internal: {

                static __command_manager_t<
                    __static_call_command_template_t<rt_method_t>
                >::with_args_t<rt_method_t *> __static_call_command_manager;

                rt_method_t * rt_method = _M(rt_method_t *, ctx.get_method(method_ref));

                // A static method of the generic type that is running (Twice(n) in Box<T>): the one of the instance.
                if (__is_current_template(ctx, rt_method->host_type))
                {
                    return __new_static_call_command_of(
                        ctx, ctx.to_generic_method(rt_method, ctx.type)
                    );
                }

                return __static_call_command_manager.template
                    get_command<__CallCmd_(static_, general)>(rt_method, ctx);

            }   break;

            default:
                X_UNEXPECTED();
        }
    }

    static command_t * __new_static_call_command_of(__context_t & ctx, rt_method_base_t * method)
    {
        static __command_manager_t<
            __static_call_command_template_t<rt_method_t>
        >::with_args_t<rt_method_t *> __static_call_command_manager;

        static __command_manager_t<
            __static_call_command_template_t<rt_generic_method_t>
        >::with_args_t<rt_generic_method_t *> __static_generic_call_command_manager;

        if (method->this_type() == rt_member_type_t::generic)
        {
            return __static_generic_call_command_manager.template get_command<
                __CallCmd_(static_, generic)>((rt_generic_method_t *)method, ctx);
        }

        return __static_call_command_manager.template get_command<
            __CallCmd_(static_, general)>(_M(rt_method_t *, method), ctx);
    }

    static command_t * __new_static_call_command(__context_t & ctx, const call_xil_t & xil)
    {
        ref_t method_ref = (ref_t)xil.method;

        return new_static_call_command(ctx, method_ref);
    }

    //-------- ---------- ---------- ---------- ----------

    template<command_value_t _cv, typename _rt_method_t>
    class __instance_call_command_t : public __call_command_base_t<_cv, _rt_method_t> __OsrBase
    {
        typedef __instance_call_command_t                   __self_t;
        typedef __call_command_base_t<_cv, _rt_method_t>    __super_t;

    public:
        using __super_t::__super_t;

        #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL

        // The profile of a call of a delegate has the place of the object already.
        virtual msize_t receiver_offset() override { return 0; }

        #endif

        __BeginExecute(ctx)

            exec_method_t * method = __This->__get_method(ctx);

            // The object of a class is a reference, that of a struct or a primitive is not.
            if (__This->__check_this < 0)
            {
                __This->__check_this =
                    __This->get_type()->get_ttype(ctx.env) == ttype_t::class_? 1 : 0;
            }

            if (__This->__check_this)
            {
                rt_ref_t self = *(rt_ref_t *)(ctx.stack.top() - method->this_offset);
                __CheckNullReference(ctx, self);
            }

            // An accessor of a property (the method was parsed already by a call before this one).
            if (method->trivial_kind.load(std::memory_order_acquire) != 0)
            {
                #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL
                if (__This->profile != nullptr && method->trivial_kind.load(std::memory_order_relaxed) == 4
                        && __delegate_osr(ctx, method, __This->profile))
                    return;
                #endif

                if (exec_call_stats_on) exec_call_count(method, 5);
                __trivial_call(ctx, method);
                return;
            }

            if (exec_call_stats_on) exec_call_count(method, 1);
            ctx.push_calling(method);

        __EndExecute()

    private:
        int8_t __check_this = -1;

    public:

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

                // A call that counts what the delegate has a command of its own.
                if (ctx.osr_candidate)
                {
                    return __instance_call_command_template_t<rt_generic_method_t>::template
                        new_command<__CallCmd_(instance, generic)>((memory_t *)&__command_heap, rt_generic_method, ctx);
                }

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

                if (ctx.osr_candidate)
                {
                    return __instance_call_command_template_t<rt_method_t>::template
                        new_command<__CallCmd_(instance, general)>((memory_t *)&__command_heap, rt_method, ctx);
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

    // The method that a call of a static method goes to (nullptr for the ones that the call command resolves by itself).
    static exec_method_t * __static_call_target(__context_t & ctx, const call_xil_t & xil)
    {
        ref_t method_ref = (ref_t)xil.method;

        // A generic method of a generic param (constrained) is resolved to the method of the type that it is: not inlined.
        if ((mt_member_extra_t)method_ref.extra != mt_member_extra_t::import
                && (mt_member_extra_t)method_ref.extra != mt_member_extra_t::internal)
            return nullptr;

        rt_method_t * rt_method = _M(rt_method_t *, ctx.get_method(method_ref));
        if (rt_method == nullptr)
            return nullptr;

        if (__is_current_template(ctx, rt_method->host_type))
            return ctx.env.exec_method_of(ctx.to_generic_method(rt_method, ctx.type));

        return ctx.env.exec_method_of(rt_method);
    }

    // The method that a call of an instance method goes to (the one that __new_instance_call_command makes the command for).
    static exec_method_t * __instance_call_target(__context_t & ctx, const call_xil_t & xil)
    {
        rt_method_base_t * method = ctx.get_method((ref_t)xil.method);
        if (method == nullptr)
            return nullptr;

        switch (method->this_type())
        {
            case rt_member_type_t::generic:
                return ctx.env.exec_method_of((rt_generic_method_t *)method);

            case rt_member_type_t::general: {
                rt_method_t * rt_method = (rt_method_t *)method;

                if (__is_current_template(ctx, rt_method->host_type))
                    return ctx.env.exec_method_of(ctx.to_generic_method(rt_method, ctx.type));

                return ctx.env.exec_method_of(rt_method);
            }

            default:
                return nullptr;
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

    // The cache of a call site (an interface call: the search of the function of the interface in the table of the type is made each time):
    // the types of the objects that were seen (up to four) and the method that each one has. A cache never changes once it is seen by
    // other threads: a new type makes a new cache (the old one is left, it is read by the threads that have it), at most four times.
    struct __call_site_cache_t
    {
        static const int max_count = 4;

        int             count = 0;
        rt_type_t *     types[max_count];
        exec_method_t * methods[max_count];
    };

    // changed: the receiver that is a value was put in the stack in the place of the object.
    template<typename _fetch_t>
    __AlwaysInline exec_method_t * __get_virtual_exec_method(command_execute_context_t & ctx,
                    msize_t this_offset, rt_generic_method_t * call_site, _fetch_t fetch_func,
                    std::atomic<__call_site_cache_t *> * cache = nullptr, bool * changed = nullptr)
    {
        rt_ref_t obj = *(rt_ref_t *)(ctx.stack.top() - this_offset);
        if (obj == nullptr)
            return nullptr;

        rt_type_t * rt_type = __RtTypeOf(obj);

        // A reference type that was seen at this site: the method is the same (the value types have a fix of the receiver below).
        __call_site_cache_t * cached = nullptr;
        if (cache != nullptr)
        {
            cached = cache->load(std::memory_order_acquire);
            if (cached != nullptr)
            {
                for (int k = 0; k < cached->count; k++)
                {
                    if (cached->types[k] == rt_type)
                        return cached->methods[k];
                }
            }
        }

        rt_vtable_t * vtbl = get_vtable(rt_type);
        rt_vfunction_t func = fetch_func(vtbl);

        if (!func.initialized())
        {
            rt_assembly_t * rt_assembly = ctx.env.assemblies.at(func.assembly_idx);
            assembly_analyzer_t analyzer = __assembly_analyzer(ctx, rt_assembly);
            rt_method_base_t * rt_method = analyzer.get_method(ref_t(func.method_idx));

            // The generic params of a method are those of the call: the method that implements
            // Make<TKey> is found by the object, its TKey is the one of the call.
            rt_type_t ** method_atypes = nullptr;
            int method_atype_count = 0;

            if (call_site != nullptr && rt_method->this_type() == rt_member_type_t::general
                    && ((rt_method_t *)rt_method)->generic_param_count() > 0)
            {
                method_atypes = call_site->atypes;
                method_atype_count = call_site->atype_count();
            }

            // The methods of an array are those of Array<T>, that is the same type.
            rt_type_t * instance_type = rt_type;
            if (rt_type->get_kind() == rt_type_kind_t::array)
                instance_type = ((rt_array_type_t *)rt_type)->as_generic(ctx.env);

            if (instance_type->get_kind() == rt_type_kind_t::generic || method_atype_count > 0)
            {
                rt_generic_method_t * m = analyzer.to_generic_method(
                    _M(rt_method_t *, rt_method), instance_type, method_atypes, method_atype_count
                );
                func.method = ctx.env.exec_method_of(m);
            }
            else
            {
                func.method = ctx.env.exec_method_of(rt_method);
            }
        }

        // A method of a primitive type (Int32.CompareTo) takes its value, and the receiver of
        // the call is the object that has the value in a box: the value is put in the place of
        // the object (a value and a reference take one unit of the stack).
        exec_method_t * exec_method = (exec_method_t *)func.method;
        vtype_t rt_vtype = rt_type->get_vtype(ctx.env);
        if ((is_system_value_type(rt_vtype) || rt_vtype == vtype_t::string_)
                && exec_method->method != nullptr && exec_method->method->get_host_type() == rt_type)
        {
            if (changed != nullptr)
                *changed = true;

            rt_stack_unit_t * slot = ctx.stack.top() - this_offset;
            rt_stack_unit_t unit = 0;

            memcpy(&unit, (void *)obj, rt_vtype == vtype_t::string_? sizeof(rt_ref_t)
                                                                   : get_vtype_size(rt_vtype));
            *(rt_stack_unit_t *)slot = unit;
        }
        else if (cache != nullptr && (cached == nullptr || cached->count < __call_site_cache_t::max_count))
        {
            // A type of a site that is a reference type: the next calls with it do not search.
            __call_site_cache_t * next = new __call_site_cache_t();
            if (cached != nullptr)
                *next = *cached;

            next->types[next->count] = rt_type;
            next->methods[next->count] = exec_method;
            next->count++;

            cache->store(next, std::memory_order_release);
        }

        return exec_method;
    }


    // Normal virtual call command.
    class __virtual_call_command_t : public __command_base_t<__CallCmd(virtual_)> __OsrBase
    {
        typedef __virtual_call_command_t                __self_t;
        typedef __command_base_t<__CallCmd(virtual_)>   __super_t;

    public:
        __virtual_call_command_t(msize_t virtual_offset, msize_t this_offset,
                                                         rt_generic_method_t * call_site)
            : __virtual_offset(virtual_offset), __this_offset(this_offset)
            , __call_site(call_site) { }

        #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL

        virtual msize_t receiver_offset() override { return __this_offset; }

        #endif

        __BeginExecute(ctx)

            bool changed = false;
            exec_method_t * method = __get_virtual_exec_method(ctx, __This->__this_offset,
                __This->__call_site,
                [offset=__This->__virtual_offset](rt_vtable_t * vtbl) {
                    return vtbl->functions[offset];
                },
                nullptr, &changed
            );

            __CheckNullReference(ctx, method);

            #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL
            if (__This->profile != nullptr && !changed && __osr_record(ctx, __This->profile, method))
                return;
            #endif

            if (exec_call_stats_on) exec_call_count(method, 2);
            ctx.push_calling(method);

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            return _F(_T("call virtual [%1% %2%]"), __This->__virtual_offset, __This->__this_offset);
        __EndToString()

        #endif  // EXEC_TRACE

        msize_t __virtual_offset, __this_offset;
        rt_generic_method_t * __call_site;      // The method of the call, if it has generic params.
    };

    struct __virtual_call_command_template_t
    {
        template<command_value_t _cv, typename ... args_t>
        static auto new_command(memory_t * memory, msize_t virtual_offset, msize_t this_offset,
                                rt_generic_method_t * call_site, __context_t & ctx)
        {
            static_assert(_cv == __CallCmd(virtual_), "invalid command value");

            typedef __virtual_call_command_t this_command_t;
            return __new_command<this_command_t>(memory, virtual_offset, this_offset, call_site);
        }
    };

    // Interface virtual call command.
    class __interface_call_command_t : public __command_base_t<__CallCmd(interface_)> __OsrBase
    {
        typedef __interface_call_command_t                __self_t;
        typedef __command_base_t<__CallCmd(interface_)>   __super_t;

    public:
        __interface_call_command_t(rt_type_t * interface_type, msize_t virtual_offset,
                                   msize_t this_offset, rt_generic_method_t * call_site)
            : __interface_type(interface_type), __virtual_offset(virtual_offset),
                                                __this_offset(this_offset), __call_site(call_site)
        { }

        #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL

        virtual msize_t receiver_offset() override { return __this_offset; }

        #endif

        __BeginExecute(ctx)

            bool changed = false;
            exec_method_t * method = __get_virtual_exec_method(ctx, __This->__this_offset,
                __This->__call_site,
                [o=__This->__virtual_offset, t=__This->__interface_type, &env=ctx.env]
                                                                        (rt_vtable_t * vtbl) {
                    return vtbl->search_vtable_interface_function(env, t, o);
                },
                &__This->__cache, &changed
            );

            __CheckNullReference(ctx, method);

            #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL
            if (__This->profile != nullptr && !changed && __osr_record(ctx, __This->profile, method))
                return;
            #endif

            if (exec_call_stats_on) exec_call_count(method, 3);
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
        rt_generic_method_t * __call_site;      // The method of the call, if it has generic params.
        std::atomic<__call_site_cache_t *> __cache { nullptr };
    };

    struct __interface_call_command_template_t
    {
        template<command_value_t _cv, typename ... args_t>
        static auto new_command(memory_t * memory, rt_type_t * interface_type,
                                msize_t virtual_offset, rt_generic_method_t * call_site,
                                msize_t this_offset, __context_t & ctx)
        {
            static_assert(_cv == __CallCmd(interface_), "invalid command value");

            typedef __interface_call_command_t this_command_t;
            return __new_command<this_command_t>(memory, interface_type,
                                                 virtual_offset, this_offset, call_site);
        }
    };

    static command_t * __new_virtual_call_command(__context_t & ctx, const call_xil_t & xil)
    {
        ref_t method_ref = (ref_t)xil.method;
        rt_method_base_t * method = ctx.get_method(method_ref);
        _A(method != nullptr);

        // The item of a tuple, read through its interface: the method of the tuple is called.
        if (method->this_type() == rt_member_type_t::dynamic)
            return __new_instance_call_command(ctx, method);

        rt_type_t * host_type = method->get_host_type();
        _A(host_type != nullptr);

        exec_method_t * exec_method = ctx.env.exec_method_of(method);
        msize_t this_offset = exec_method->this_offset;

        // The method of the call has generic params of its own: the method that is called is
        // found when the call is executed, it gets them.
        rt_generic_method_t * call_site = nullptr;
        if (method->this_type() == rt_member_type_t::generic
                && ((rt_generic_method_t *)method)->template_->generic_param_count() > 0)
        {
            call_site = (rt_generic_method_t *)method;
        }

        // Interface virtual call.
        if (host_type->get_ttype(ctx.env) == ttype_t::interface_)
        {
            static __command_manager_t<
                __interface_call_command_template_t
            >::with_args_t<rt_type_t *, msize_t, rt_generic_method_t *>
                                                    __interface_call_command_manager;

            msize_t virtual_offset = ctx.get_virtual_method_offset(method_ref, host_type);

            // A call that counts what it goes to has a command of its own.
            if (ctx.osr_candidate)
            {
                return __new_command<__interface_call_command_t>(
                    (memory_t *)&__command_heap, host_type, virtual_offset, this_offset, call_site
                );
            }

            return __interface_call_command_manager.template
                get_command<__CallCmd(interface_)>(host_type, virtual_offset, call_site,
                                                                         this_offset, ctx);
        }
        else    // Normal virtual call.
        {
            static __command_manager_t<
                __virtual_call_command_template_t
            >::with_args_t<msize_t, msize_t, rt_generic_method_t *> __virtual_call_command_manager;

            msize_t virtual_offset = ctx.get_virtual_method_offset(method_ref, host_type);

            if (ctx.osr_candidate)
            {
                return __new_command<__virtual_call_command_t>(
                    (memory_t *)&__command_heap, virtual_offset, this_offset, call_site
                );
            }

            return __virtual_call_command_manager.template
                get_command<__CallCmd(virtual_)>(virtual_offset, this_offset, call_site, ctx);
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
        __delegate_call_command_base_t(msize_t this_offset, bool struct_ret = false)
            : __this_offset(this_offset + __stack_stub_size), __struct_ret(struct_ret)
        { }

    protected:
        const msize_t __this_offset;
        const bool    __struct_ret;     // The delegate returns a custom struct.

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

    // Returns the method that a delegate of a virtual method calls: the most derived
    // implementation in the type of the instance, like a virtual call.
    static rt_method_base_t * __resolve_delegate_method(command_execute_context_t & ctx,
                                        rt_object_t * instance, rt_method_base_t * method)
    {
        if (instance == nullptr || method == nullptr
                || method->this_type() != rt_member_type_t::general)
        {
            return method;
        }

        rt_method_t * rt_method = (rt_method_t *)method;
        decorate_t decorate = (decorate_t)(*rt_method)->decorate;
        if (!(decorate.is_virtual || decorate.is_override || decorate.is_abstract))
            return method;

        rt_type_t * type = mm::get_object_type((rt_ref_t)(void *)instance);
        if (type == nullptr || type->get_kind() == rt_type_kind_t::generic)
            return method;

        method_prototype_t prototype;
        assembly_analyzer_t(ctx.env, rt_method->get_assembly()).get_prototype(
            rt_method, &prototype
        );

        for (rt_type_t * t = type; t != nullptr; t = t->get_base_type(ctx.env))
        {
            if (t->get_kind() == rt_type_kind_t::generic)
                break;

            ref_t ref = t->search_method(ctx.env, prototype);
            if (ref == ref_t::null)
                continue;

            rt_method_base_t * found = assembly_analyzer_t(ctx.env, t->get_assembly())
                                                                .get_method(ref);
            return found != nullptr? found : method;
        }

        return method;
    }

    // Returns whether the method is static.
    static bool __is_static_method(rt_method_base_t * method)
    {
        rt_method_t * general = nullptr;

        if (method->this_type() == rt_member_type_t::general)
            general = (rt_method_t *)method;
        else if (method->this_type() == rt_member_type_t::generic)
            general = ((rt_generic_method_t *)method)->template_;

        return general != nullptr && ((decorate_value_t)(*general)->decorate).is_static;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - -
    // delegate invoke init command.

    // The commands of the delegates: the init of the object, the init with a call type, and the
    // invoke.
    template<command_value_t _cv>
    class __delegate_call_command_t : public __delegate_call_command_base_t<_cv>
    {
        typedef __delegate_call_command_t                   __self_t;
        typedef __delegate_call_command_base_t<_cv>         __super_t;

    public:
        using __super_t::__super_t;

        __BeginExecute(ctx)

            if constexpr (_cv == __CallCmd(delegate_init))
            {
                rt_object_t * self = __This->__get_self(ctx);

                // A virtual method is bound to the implementation of the instance.
                rt_object_t * instance =
                    __Argument(rt_object_t *, __delegate_instance_argument_offset);

                rt_method_base_t * method = __resolve_delegate_method(
                    ctx, instance, __Argument(rt_method_base_t *, __delegate_method_argument_offset)
                );

                __This->__set_instance(ctx, self, instance);
                __This->__set_method(ctx, self, method);
            }
            else if constexpr (_cv == __CallCmd(delegate_init_with_call_type))
            {
                rt_object_t * self = __This->__get_self(ctx);

                rt_object_t * instance =
                    __Argument(rt_object_t *, __delegate_instance_argument_offset2);
                rt_method_base_t * method =
                    __Argument(rt_method_base_t *, __delegate_method_argument_offset2);
                int32_t call_type = __Argument(int32_t, __delegate_call_type_argument_offset2);

                __This->__set_instance(ctx, self, instance);
                __This->__set_method(ctx, self, al::incorp(method, call_type));
            }
            else    // delegate_invoke
            {
                rt_object_t * self = __This->__get_self(ctx);
                rt_object_t * object = __This->__get_instance(ctx, self);
                rt_method_base_t * method = __This->__get_method(ctx, self);

                xil_call_type_t call_type = (xil_call_type_t)al::incorp_v<int32_t>(method);
                method = al::incorp_p(method);

                exec_method_t * exec_method = ctx.env.exec_method_of(method);

                // A static method has no `this`: it cannot take the frame of Invoke over,
                // it is called with a copy of the arguments, and Invoke returns its result.
                if (__is_static_method(method))
                {
                    rt_stack_unit_t * args =
                        (rt_stack_unit_t *)__ArgumentAddress(__This->__this_offset) + 1;
                    size_t count = __This->__this_offset - __stack_stub_size - 1;

                    // The address of the struct result, below the arguments.
                    if (__This->__struct_ret)
                        ctx.stack.push(*(void **)__ArgumentAddress(__This->__this_offset + 1));

                    for (size_t k = 0; k < count; k++)
                        ctx.stack.push(args[k]);

                    if (exec_call_stats_on) exec_call_count(exec_method, 4);
                    ctx.push_calling(exec_method);
                    return;
                }

                __Argument(rt_object_t *, __This->__this_offset) = object;
                ctx.switch_to(exec_method);
            }

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            if constexpr (_cv == __CallCmd(delegate_init))
                return _T("call delegate init");
            else if constexpr (_cv == __CallCmd(delegate_init_with_call_type))
                return _T("call delegate init (with call type)");
            else
                return _T("call delegate invoke");
        __EndToString()

        #endif  // EXEC_TRACE
    };





    // The call of Invoke of a delegate: the args are in the stack with the delegate at the bottom of them (the this of Invoke). The method that the
    // delegate has gets them as they are: its object (the one of the lambda) takes the place of the delegate, a static method has no this: the args
    // go down by one unit.
    static void __delegate_invoke_direct(command_execute_context_t & ctx, exec_method_t * invoke)
    {
        rt_stack_unit_t * slot = ctx.stack.top() - invoke->this_offset;

        rt_object_t * self = *(rt_object_t **)slot;
        rt_object_t * object = __Field(rt_object_t *, self, __delegate_instance_field_offset);
        rt_method_base_t * method = __Field(rt_method_base_t *, self, __delegate_method_field_offset);

        method = al::incorp_p(method);
        exec_method_t * target = ctx.env.exec_method_of(method);

        if (__is_static_method(method))
        {
            for (msize_t k = 1; k < invoke->this_offset; k++)
                slot[k - 1] = slot[k];

            ctx.stack.set_top(ctx.stack.top() - 1);
        }
        else
        {
            *(rt_object_t **)slot = object;
        }

        if (exec_call_stats_on) exec_call_count(target, 4);
        ctx.push_calling(target);
    }

    static bool __delegate_osr(command_execute_context_t & ctx, exec_method_t * invoke, exec_call_profile_t * profile)
    {
        #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL

        rt_object_t * self = *(rt_object_t **)(ctx.stack.top() - invoke->this_offset);
        rt_method_base_t * method = al::incorp_p(__Field(rt_method_base_t *, self, __delegate_method_field_offset));

        return __osr_record_delegate(ctx, profile, method, ctx.env.exec_method_of(method), __is_static_method(method));

        #else

        return false;

        #endif
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    struct __delegate_command_template_t
    {
        template<command_value_t _cv, typename ... args_t>
        static auto new_command(memory_t * memory, msize_t this_offset,
                                xil_call_command_t command, bool struct_ret)
        {
            typedef __delegate_call_command_t<_cv> this_command_t;
            return __new_command<this_command_t>(memory, this_offset, struct_ret);
        }
    };

    template<command_value_t _cv>
    command_t * __new_delegate_command(__context_t & ctx, const call_xil_t & xil)
    {
        static __command_manager_t<
            __delegate_command_template_t
        >::with_args_t<msize_t, xil_call_command_t, bool> __delegate_command_manager;

        msize_t this_offset = __offset_of_argument(ctx, 0);
        bool struct_ret = is_custom_struct(ctx.env, ctx.method->get_return_type(ctx.env));

        return __delegate_command_manager.template get_command<_cv>(
            this_offset, xil.command, struct_ret
        );
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

    // A call of a member of a generic param (t.Get(), where T : IFoo): the method of the type that
    // the param is, found by the method of the constraint; a class takes the virtual table.
    static command_t * __new_constrained_call_command(__context_t & ctx, const call_xil_t & xil)
    {
        analyzer_env_t & env = ctx.env;

        rt_method_base_t * base = ctx.get_method((ref_t)xil.method);
        _A(base != nullptr);

        if (base->this_type() != rt_member_type_t::generic)
            throw _ECF(invalid_operation, _T("the call of a member of a generic parameter is not generic"));

        constrained_target_t t = resolve_constrained_instance(env, (rt_generic_method_t *)base);

        if (t.is_virtual)
        {
            static __command_manager_t<
                __virtual_call_command_template_t
            >::with_args_t<msize_t, msize_t, rt_generic_method_t *> __virtual_call_command_manager;

            msize_t virtual_offset = t.virtual_offset;
            msize_t this_offset = ctx.env.exec_method_of(t.target)->this_offset;

            return __virtual_call_command_manager.template
                get_command<__CallCmd(virtual_)>(virtual_offset, this_offset,
                                                 (rt_generic_method_t *)nullptr, ctx);
        }

        return __new_instance_call_command(ctx, t.target);
    }

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

            case xil_call_type_t::constrained:
                return __new_constrained_call_command(ctx, xil);

            case xil_call_type_t::command:
                return __new_command_call_command(ctx, xil);

            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // The xil command of a type of commands.
    template<typename _command_t>
    constexpr xil_command_t to_xil_command()
    {
        if constexpr (std::is_same<_command_t, xil_al_command_t>::value)
            return xil_command_t::al;
        else if constexpr (std::is_same<_command_t, xil_logic_command_t>::value)
            return xil_command_t::logic;
        else if constexpr (std::is_same<_command_t, xil_bit_command_t>::value)
            return xil_command_t::bit;
        else
            return xil_command_t::cmp;
    }

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

            // Integer division: the divisor is not zero, and the result fits.
            if constexpr (to_operator(_op) == operator_t::div || to_operator(_op) == operator_t::mod)
            {
                if constexpr (std::is_integral<t1>::value && std::is_integral<t2>::value)
                {
                    t2 divisor = *(t2 *)(top + unit_size1);
                    if (divisor == 0)
                        __RaiseDivideByZeroException(ctx);

                    if constexpr (std::is_signed<t1>::value && std::is_signed<t2>::value)
                    {
                        if (divisor == (t2)-1 && *(t1 *)top == std::numeric_limits<t1>::min())
                            __RaiseOverflowException(ctx);
                    }
                }
            }

            if constexpr (is_checked_command(_op))
            {
                // The result raises OverflowException when it does not fit its type.
                typedef decltype(binary_operator_t<to_operator(_op), vtype1, vtype2>()(
                    *(t1 *)top, *(t2 *)(top + unit_size1)
                )) r_t;

                r_t a = (r_t)*(t1 *)top, b = (r_t)*(t2 *)(top + unit_size1), r;
                bool overflow;

                if constexpr (to_operator(_op) == operator_t::add)
                    overflow = __builtin_add_overflow(a, b, &r);
                else if constexpr (to_operator(_op) == operator_t::sub)
                    overflow = __builtin_sub_overflow(a, b, &r);
                else
                    overflow = __builtin_mul_overflow(a, b, &r);

                if (overflow)
                    __RaiseOverflowException(ctx);

                ctx.stack.push(r);
            }
            else
            {
                ctx.stack.push(
                    binary_operator_t<to_operator(_op), vtype1, vtype2>()(
                        *(t1 *)top, *(t2 *)(top + unit_size1)
                    )
                );
            }

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

    __CaseBinaries(checked_add)
    __CaseBinaries(checked_sub)
    __CaseBinaries(checked_mul)

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

            if constexpr (is_checked_command(_op))
            {
                // - x raises OverflowException when the result does not fit its type.
                typedef decltype(unitary_operator_t<to_operator(_op), vtype>()(v)) r_t;

                r_t r;
                if (__builtin_sub_overflow((r_t)0, (r_t)v, &r))
                    __RaiseOverflowException(ctx);

                ctx.stack.push(r);
            }
            else
            {
                ctx.stack.push(
                    unitary_operator_t<to_operator(_op), vtype>()(v)
                );
            }

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

    __CaseUnitaries(checked_minus)

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

            __CaseBinaries(checked_add)
            __CaseBinaries(checked_sub)
            __CaseBinaries(checked_mul)

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

            __CaseUnitaries(checked_minus)

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

    // The same casts to an interface: the value of the cast to a class plus 3.
    #define __CastInterfaceCmd(_cmd)    __Cmd(cast_interface, _cmd)
    __DefineCmdValue_(__CastInterfaceCmd(default_))
    __DefineCmdValue_(__CastInterfaceCmd(is))
    __DefineCmdValue_(__CastInterfaceCmd(as))

    template<bool _to_type_is_interface>
    bool __cast_test(analyzer_env_t & env, rt_type_t * from_type, rt_type_t * to_type)
    {
        rt_vtable_t * vtbl = get_vtable(from_type);
        if constexpr (_to_type_is_interface)
        {
            return vtbl->search_vtable_interface_variant(env, to_type) != nullptr;
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
                    if (!__cast_test<_is_interface>(ctx.env, rt_type, __type))
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
                    ctx.stack.replace(__cast_test<_is_interface>(ctx.env, rt_type, __type));
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

                    if (!__cast_test<_is_interface>(ctx.env, rt_type, __type))
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
                typedef __cast_command_t<(command_value_t)(_cv + 3), _cast_cmd, true> this_command_t;
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
        // A class that is nested in a generic type (the state of an iterator of Box<T>) is the one
        // of the instance that runs.
        rt_type_t * type = __instance_of_template(ctx, ctx.get_type(xil.type_ref()));
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

    #define __IsRetCmdValue(_cv)    ((_cv) >= __RetCmd(0) && (_cv) <= __StructRetCmd)

    template<command_value_t _cv>
    class __ret_command_base_t : public __command_base_t<_cv> { };
    class __ret_command_identity_t { };

    // Return commands: the value of _ret_size units is moved to the place of the arguments, a struct
    // is returned through the address that is below them.
    template<command_value_t _cv, msize_t _ret_size>
    class __ret_command_t : public __ret_command_base_t<_cv>,
                            public __ret_command_identity_t
    {
        typedef __ret_command_t     __self_t;

        static constexpr bool __is_struct = _ret_size == __StructRetUnitSize;

    public:
        __ret_command_t(msize_t local_unit_size, msize_t param_unit_size)
            : __total_unit_size(param_unit_size + __stack_stub_size + local_unit_size)
            , __param_unit_ret_size(__is_struct? param_unit_size : param_unit_size - _ret_size)
            , __total_unit_ret_size(__is_struct? 0 : __total_unit_size - _ret_size)
        {
            #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_MANUAL
            __ret_commands.insert(this);
            #endif  // EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_MANUAL
        }

        __BeginExecute(ctx)

            __calling_stub_t * stub = ctx.stub();
            rt_stack_unit_t * p = ctx.stack.pop(__This->__total_unit_size);
            ctx.pop_calling(stub);

            if constexpr (__is_struct)
                ctx.stack.pop<void *>();    // pop the bottom struct object address.
            else
                copy_array<_ret_size>(p + __This->__total_unit_ret_size, p - _ret_size);

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            if constexpr (__is_struct)
                return _T("ret  [struct]");
            else
                return _F(_T("ret  [%1%]"), _ret_size);
        __EndToString()

        #endif  // EXEC_TRACE

    private:
        int __total_unit_size;      // param_unit_size + __stack_stub_size + local_unit_size
        int __param_unit_ret_size;  // param_unit_size - _ret_size
        int __total_unit_ret_size;  // __total_unit_size - _ret_size
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

            // Throwing null raises a NullReferenceException.
            __CheckNullReference(ctx, exception);

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

    // The condition of a jump.
    template<xil_jmp_model_t _model> struct __jmp_model_t
    {
        __AlwaysInline bool operator()(command_execute_context_t & ctx)
        {
            if constexpr (_model == xil_jmp_model_t::none)
                return true;
            else if constexpr (_model == xil_jmp_model_t::true_)
                return ctx.stack.pop<bool>();
            else
                return !ctx.stack.pop<bool>();
        }

        operator string_t() const
        {
            if constexpr (_model == xil_jmp_model_t::none)
                return _T("jmp");
            else if constexpr (_model == xil_jmp_model_t::true_)
                return _T("jmp  true");
            else
                return _T("jmp  false");
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // The jumps, and the leave: the finally blocks run before it jumps.
    template<command_value_t _cv, xil_jmp_model_t _model>
    class __jmp_command_t : public __command_base_t<_cv>
    {
        typedef __jmp_command_t         __self_t;

    public:
        __jmp_command_t(xil_jmp_step_t step) : __step(step) { }

        __BeginExecute(ctx)

            if constexpr (_model == xil_jmp_model_t::leave)
            {
                __process_finally(ctx, ctx.current + __This->__step);
            }
            else if (__jmp_model_t<_model>()(ctx))
            {
                // A jump that goes back is a loop: the other threads get their turn.
                if (__This->__step < 0)
                    rtlib::thread_poll();

                ctx.current += __This->__step;
            }

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            if constexpr (_model == xil_jmp_model_t::leave)
                return _F(_T("leave %1%"), __This->__step + 1);
            else
                return _F(_T("%1% %2%"), __jmp_model_t<_model>(), __This->__step);
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

        // The step of the command: the commands are not the xils when the method has superinstructions.
        auto step_of = [&ctx, &xil]() -> int {
            if (ctx.plan == nullptr)
                return xil.step() - 1;

            return (int)ctx.plan->index_map[ctx.xil_index + xil.step()] - (int)ctx.plan->index_map[ctx.xil_index] - 1;
        };

        switch (xil.model())
        {
            #define __Case(_model)                                                      \
                case xil_jmp_model_t::_model:                                           \
                    return __jmp_command_manager.template get_command<                  \
                        __JmpCmd(_model), model_t::_model>(step_of());

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
                    get_command<__JmpCmd(leave), model_t::leave>(step_of());

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

    // The commands that create objects: an object, an array, units of the stack.
    template<xil_new_type_t _new_type>
    class __new_command_t : public __command_base_t<
        _new_type == xil_new_type_t::default_?    __NewCmd(default_)
      : _new_type == xil_new_type_t::array?       __NewCmd(array)
                                                  : __NewCmd(stack_alloc)
    >
    {
        typedef __new_command_t     __self_t;

        static constexpr bool __is_default = _new_type == xil_new_type_t::default_;
        static constexpr bool __new_array   = _new_type == xil_new_type_t::array;

    public:
        #if EXEC_TRACE
        __new_command_t(rt_type_t * type, rt_sid_t name)
            : __type(type), __name(name) { }
        #else
        __new_command_t(rt_type_t * type) : __type(type) { }
        #endif  // EXEC_TRACE

        __new_command_t(size_t units) _NE : __units(units) { }

        __BeginExecute(ctx)

            if constexpr (__is_default)
            {
                __new_object(ctx, __This->__type);
            }
            else if constexpr (__new_array)
            {
                rt_array_type_t * type = _M(rt_array_type_t *, __This->__type);

                __pre_new(ctx, type);
                dimension_t dimension = type->dimension;

                array_length_t lengths[dimension];
                bool negative = false;
                for (int * len = lengths + dimension - 1; len >= lengths; len--)
                {
                    *len = ctx.stack.pop<int32_t>();
                    negative |= *len < 0;
                }

                if (negative)
                    __RaiseOutOfRangeException(ctx);

                ctx.stack.push(
                    ctx.heap->new_array(type, lengths)
                );
            }
            else    // stack_alloc
            {
                ctx.stack.push(
                    ctx.stack.alloc_units(__This->__units)
                );
            }

        __EndExecute()

        #if EXEC_TRACE

        __BeginToString()
            if constexpr (__is_default)
                return _F(_T("new %1%"), __This->__name);
            else if constexpr (__new_array)
                return _F(_T("new array %1%"), __This->__name);
            else
                return _F(_T("stack_alloc %1% units"), __This->__units);
        __EndToString()

        #endif  // EXEC_TRACE

    private:
        rt_type_t * __type = nullptr;
        size_t      __units = 0;

        #if EXEC_TRACE
        rt_sid_t    __name;
        #endif  // EXEC_TRACE
    };

    //-------- ---------- ---------- ---------- ----------


    //-------- ---------- ---------- ---------- ----------


    //-------- ---------- ---------- ---------- ----------


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

        type = __instance_of_template(ctx, type);

        switch (xil.new_type())
        {
            case xil_new_type_t::default_:
                return __new_command_manager.template
                    get_command<__NewCmd(default_), xil_new_type_t::default_>(type, ctx);

            case xil_new_type_t::array:
                return __new_command_manager.template
                    get_command<__NewCmd(array), xil_new_type_t::array>(type, ctx);

            case xil_new_type_t::stack_alloc: {

                // The size of a variable: the generic param of an argument is a class, a number or
                // a string, as well as a struct (it is the reference that is in the stack, not
                // the object).
                msize_t obj_size = type->get_variable_size(ctx.env);
                size_t units = _alignf(obj_size, sizeof(rt_stack_unit_t)) / sizeof(rt_stack_unit_t);
                return __alloc_command_manager.template
                    get_command<__NewCmd(stack_alloc), xil_new_type_t::stack_alloc>(units);

            }   break;

            case xil_new_type_t::generic_stack_alloc: {

                // The destination of the struct that a call returns, when the generic param is
                // one; else the value is returned in the stack.
                if (!is_custom_struct(ctx.env, type))
                    __ReturnInstance(__empty_command_t);

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

    // Copies, in the order of the stack: the destination is the first, the source is the
    // first when it is reverse, or the destination is zeroed.
    template<size_t _size, xil_copy_kind_t _kind = xil_copy_kind_t::default_>
    struct __memory_copy_operation_t
    {
        __AlwaysInline static void copy(command_execute_context_t & ctx)
        {
            if constexpr (_kind == xil_copy_kind_t::zero)
            {
                void * p = ctx.stack.pop<void *>();

                al::quick_zero<_size>(p);
            }
            else if constexpr (_kind == xil_copy_kind_t::reverse)
            {
                void * src = ctx.stack.pop<void *>();
                void * dst = ctx.stack.pop<void *>();

                al::quick_copy<_size>(dst, src);
            }
            else
            {
                void * dst = ctx.stack.pop<void *>();
                void * src = ctx.stack.pop<void *>();

                al::quick_copy<_size>(dst, src);
            }
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

        __memory_copy_command_ex_t(size_t size, xil_copy_kind_t kind,
                                   msize_t struct_ret_offset = 0) _NE
            : __size(size), __kind(kind), __struct_ret_offset(struct_ret_offset) { }

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
            else if(__This->__kind == xil_copy_kind_t::store)
            {
                // The value is in the stack, the destination is below it.
                size_t units = (__This->__size + sizeof(rt_stack_unit_t) - 1)
                                                        / sizeof(rt_stack_unit_t);
                if (units == 0)
                    units = 1;

                void * src = (void *)(ctx.stack.top() - units);
                void * dst = *(void **)(ctx.stack.top() - units - 1);

                al::quick_copy(dst, src, __This->__size);
                ctx.stack.pop(units + 1);
            }
            else if(__This->__kind == xil_copy_kind_t::call_ret)
            {
                // A call returned a struct: the call has written it in the region that is in the
                // stack, the caller of this method gets it at its destination.
                size_t units = (__This->__size + sizeof(rt_stack_unit_t) - 1)
                                                        / sizeof(rt_stack_unit_t);
                if (units == 0)
                    units = 1;

                void * src = (void *)(ctx.stack.top() - units);
                void * dst = __Argument(void *, __This->__struct_ret_offset);

                al::quick_copy(dst, src, __This->__size);
                ctx.stack.pop(units);
            }
            else if(__This->__kind == xil_copy_kind_t::ret && __This->__struct_ret_offset != 0)
            {
                // A custom struct is returned, written at the address of the caller,
                // that is below the arguments.
                void * src = ctx.stack.pop<void *>();
                void * dst = __Argument(void *, __This->__struct_ret_offset);

                al::quick_copy(dst, src, __This->__size);
            }
            else if(__This->__kind == xil_copy_kind_t::ret)
            {
                // The value of the object is pushed into the stack.
                void * src = ctx.stack.pop<void *>();

                size_t units = (__This->__size + sizeof(rt_stack_unit_t) - 1)
                                                        / sizeof(rt_stack_unit_t);
                if (units == 0)
                    units = 1;

                rt_stack_unit_t * dst = ctx.stack.alloc_units(units);
                dst[units - 1] = 0;

                al::quick_copy((void *)dst, src, __This->__size);
            }


        __EndExecute()

    private:
        size_t          __size;
        xil_copy_kind_t __kind;
        msize_t         __struct_ret_offset;    // Returns a custom struct: the offset of the
                                                //   address of the result, otherwise 0.
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

    // The command that makes an object of the value that is at the address on the top: the box
    // of a generic param that is not a reference.
    #define __GenericBoxCmd     __Cmd(box, generic)
    __DefineCmdValue_(__GenericBoxCmd)

    class __generic_box_command_t : public __command_base_t<__GenericBoxCmd>
    {
        typedef __generic_box_command_t     __self_t;

    public:
        __generic_box_command_t(rt_type_t * type, size_t size, bool is_string)
            : __type(type), __size(size), __is_string(is_string) { }

        __BeginExecute(ctx)

            void * src = ctx.stack.pop<void *>();

            // A string that is null is a null object.
            if (__This->__is_string && *(void **)src == nullptr)
            {
                ctx.stack.push<rt_ref_t>(nullptr);
                return;
            }

            __pre_new(ctx, __This->__type);
            rt_ref_t obj = ctx.heap->new_obj(__This->__type);

            al::quick_copy((void *)obj, src, __This->__size);
            ctx.stack.push<rt_ref_t>(obj);

        __EndExecute()

        #if EXEC_TRACE

        __ToString(_T("box generic"));

        #endif  // EXEC_TRACE

    private:
        rt_type_t * __type;
        size_t      __size;
        bool        __is_string;
    };

    // The command of (T)obj, T is a generic param whose argument is a number, an enum or a string: the value of the
    // box is pushed (a string that is null is a null string); the one of a custom struct is copied to the destination
    // that is below the object.
    #define __GenericUnboxCmd   __Cmd(unbox, generic)
    __DefineCmdValue_(__GenericUnboxCmd)

    class __generic_unbox_command_t : public __command_base_t<__GenericUnboxCmd>
    {
        typedef __generic_unbox_command_t   __self_t;

    public:
        __generic_unbox_command_t(rt_type_t * type, size_t size, bool is_string, bool is_struct)
            : __type(type), __size(size), __is_string(is_string), __is_struct(is_struct) { }

        __BeginExecute(ctx)

            rt_ref_t obj = ctx.stack.pop<rt_ref_t>();

            if (__This->__is_string && obj == nullptr)
            {
                ctx.stack.push<void *>(nullptr);
                return;
            }

            __CheckNullReference(ctx, obj);

            if (__RtTypeOf(obj) != __This->__type)
                __RaiseInvalidCastException(ctx);

            if (__This->__is_struct)
            {
                void * dst = ctx.stack.pop<void *>();
                al::quick_copy(dst, (void *)obj, __This->__size);
                return;
            }

            switch (__This->__size)
            {
                case 1: ctx.stack.push<uint8_t>(*(uint8_t *)(void *)obj); break;
                case 2: ctx.stack.push<uint16_t>(*(uint16_t *)(void *)obj); break;
                case 4: ctx.stack.push<uint32_t>(*(uint32_t *)(void *)obj); break;
                default: ctx.stack.push<uint64_t>(*(uint64_t *)(void *)obj); break;
            }

        __EndExecute()

        #if EXEC_TRACE

        __ToString(_T("unbox generic"));

        #endif  // EXEC_TRACE

    private:
        rt_type_t * __type;
        size_t      __size;
        bool        __is_string;
        bool        __is_struct;
    };

    struct __generic_unbox_command_template_t
    {
        template<command_value_t _cv, typename ... _args_t>
        static auto new_command(memory_t * memory, rt_type_t * type, size_t size, bool is_string, bool is_struct)
        {
            return __new_command<__generic_unbox_command_t>(memory, type, size, is_string, is_struct);
        }
    };

    static command_t * __get_generic_unbox_command(rt_type_t * type, size_t size, bool is_string, bool is_struct)
    {
        static __command_manager_t<
            __generic_unbox_command_template_t
        >::with_args_t<rt_type_t *, size_t, bool, bool> __generic_unbox_command_manager;

        return __generic_unbox_command_manager.template get_command<__GenericUnboxCmd>(
            type, size, is_string, is_struct
        );
    }

    struct __generic_box_command_template_t
    {
        template<command_value_t _cv, typename ... _args_t>
        static auto new_command(memory_t * memory, rt_type_t * type, size_t size, bool is_string)
        {
            return __new_command<__generic_box_command_t>(memory, type, size, is_string);
        }
    };

    static command_t * __get_generic_box_command(rt_type_t * type, size_t size, bool is_string)
    {
        static __command_manager_t<
            __generic_box_command_template_t
        >::with_args_t<rt_type_t *, size_t, bool> __generic_box_command_manager;

        return __generic_box_command_manager.template get_command<__GenericBoxCmd>(
            type, size, is_string
        );
    }

    // Creates the command that returns a generic param whose argument is a custom struct.
    static command_t * __get_struct_ret_copy_command(size_t size, msize_t struct_ret_offset,
                                        xil_copy_kind_t kind = xil_copy_kind_t::ret)
    {
        static __command_manager_t<
            __memory_copy_command_ex_template_t
        >::with_args_t<size_t, xil_copy_kind_t, msize_t> __struct_ret_copy_manager;

        return __struct_ret_copy_manager.template get_command<__CopyCmdEx>(
            size, kind, struct_ret_offset
        );
    }

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

            case xil_copy_kind_t::ret:
            case xil_copy_kind_t::store:
                return __memory_copy_command_ex_manager.template
                    get_command<__CopyCmdEx>(size, kind);

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

                // An object: a reference is loaded, a value (a struct, a number, a string, an
                // enum) is boxed.
                if (kind == xil_copy_kind_t::box)
                {
                    if (is_ref_type(type->get_ttype(ctx.env))
                            && type->get_vtype(ctx.env) != vtype_t::string_)
                        return __get_object_copy_command(size, xil_copy_kind_t::ret);

                    return __get_generic_box_command(
                        type, size, type->get_vtype(ctx.env) == vtype_t::string_
                    );
                }

                // (T)obj: a reference is the object (it was checked by the cast).
                if (kind == xil_copy_kind_t::unbox)
                {
                    bool is_string = type->get_vtype(ctx.env) == vtype_t::string_;

                    if (is_ref_type(type->get_ttype(ctx.env)) && !is_string)
                        __ReturnInstance(__empty_command_t);

                    return __get_generic_unbox_command(
                        type, size, is_string, is_custom_struct(ctx.env, type)
                    );
                }

                // The object of a call: a reference is loaded, the address of a struct is kept.
                if (kind == xil_copy_kind_t::receiver)
                {
                    if (is_custom_struct(ctx.env, type))
                        __ReturnInstance(__empty_command_t);

                    return __get_object_copy_command(size, xil_copy_kind_t::ret);
                }

                // A value that a call returned is stored at the destination that is below it: a
                // struct was written there by the call.
                if (kind == xil_copy_kind_t::store)
                {
                    if (is_custom_struct(ctx.env, type))
                        __ReturnInstance(__empty_command_t);

                    return __get_object_copy_command(size, xil_copy_kind_t::store);
                }

                // A value that a call returned: a struct was written at the destination that the
                // call had, it is copied to the one of the caller; else it is in the stack.
                if (kind == xil_copy_kind_t::call_ret)
                {
                    if (!is_custom_struct(ctx.env, type))
                        __ReturnInstance(__empty_command_t);

                    return __get_struct_ret_copy_command(
                        size, __offset_of_argument(ctx, XIL_CALLING_BOTTOM_IDENTITY) + __stack_stub_size,
                        xil_copy_kind_t::call_ret
                    );
                }

                // The generic param is a custom struct: it is returned like a struct.
                if (kind == xil_copy_kind_t::ret && is_custom_struct(ctx.env, type))
                {
                    return __get_struct_ret_copy_command(
                        size, __offset_of_argument(ctx, XIL_CALLING_BOTTOM_IDENTITY) + __stack_stub_size
                    );
                }

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

    #define __InitCmd(_init_type, _size)        __Cmd(init, _init_type##_##_size)

    __DefineCmdValue_(__InitCmd(array_begin, 0))

    __DefineCmdValue_(__InitCmd(array_element, 0))
    __DefineCmdValue_(__InitCmd(array_element, 1))
    __DefineCmdValue_(__InitCmd(array_element, 2))
    __DefineCmdValue_(__InitCmd(array_element, 4))
    __DefineCmdValue_(__InitCmd(array_element, 8))

    __DefineCmdValue_(__InitCmd(array_end, 0))

    //-------- ---------- ---------- ---------- ----------

    // The commands that initialize the elements of an array: the begin, the elements (the size of
    // an element is _size, or it is given when it is not known), the end.
    template<command_value_t _cv, xil_init_type_t _init_type, msize_t _size>
    class __init_command_t : public __init_command_base_t<_cv>
    {
        typedef __init_command_t    __self_t;

    public:
        __init_command_t(msize_t size = 0) : __size(size) { }

        __BeginExecute(ctx)

            if constexpr (_init_type == xil_init_type_t::array_begin)
            {
                ctx.stack.push<void *>(
                    (void *)ctx.stack.pick<rt_ref_t>()
                );
            }
            else if constexpr (_init_type == xil_init_type_t::array_element)
            {
                if constexpr (_size == 0)
                {
                    // TODO: implement it.
                    X_UNEXPECTED();
                }
                else
                {
                    typedef uint_type_t<_size> t;
                    *ctx.stack.pick_reference<t *>()++ = ctx.stack.pop<t>();
                }
            }
            else    // array_end
            {
                ctx.stack.pop<void *>();
            }

        __EndExecute()

    private:
        msize_t __size;
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

        // References are runtime addresses: the size in an assembly is that of a metadata ref.
        if (al::in(xil.dtype(), xil_type_t::object, xil_type_t::string, xil_type_t::ptr))
            return rt_ref_size;

        // A char of the runtime is 16 bits, as the elements of the arrays that are read and written by
        // the other commands.
        if (xil.dtype() == xil_type_t::char_)
            return sizeof(rt_char_t);

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
    // Superinstructions (modules/exec/fusion.h): a command for several xils.

    #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL || EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_INLINE

    // The variables of the commands (the macros of the other commands are not defined here).
    #define __SkLocal(type_t, offset)       (*((type_t *)((byte_t *)ctx.stack.lp() + (offset))))
    #define __SkArgument(type_t, offset)    (*((type_t *)((rt_stack_unit_t *)ctx.stack.lp() - (offset))))
    #define __SkField(type_t, p, offset)    (*((type_t *)((byte_t *)(p) + (offset))))

    // The values of the commands of the superinstructions: from a number that the other commands do not reach (it is checked at the end of
    // the file), by the form, the operator and the types. The switch of the INLINE model needs a value for each class.
    constexpr command_value_t __SuperBase = 7800;

    constexpr int __sc_type_index(xil_type_t x)
    {
        return x == xil_type_t::int32? 0 : x == xil_type_t::uint32? 1 : x == xil_type_t::int64? 2
             : x == xil_type_t::uint64? 3 : x == xil_type_t::float_? 4 : 5;
    }

    // The pair of types of an operation: the same type (0 - 5), or an Int64 and an Int32 (6).
    constexpr int __sc_pair_index(xil_type_t x1, xil_type_t x2)
    {
        return (x1 == xil_type_t::int64 && x2 == xil_type_t::int32)? 6 : __sc_type_index(x1);
    }

    constexpr int __sc_op_index(operator_t op)
    {
        return op == operator_t::add? 0 : op == operator_t::sub? 1 : op == operator_t::mul? 2 : op == operator_t::div? 3
             : op == operator_t::mod? 4 : op == operator_t::bit_and? 5 : op == operator_t::bit_or? 6 : op == operator_t::bit_xor? 7
             : op == operator_t::left_shift? 8 : op == operator_t::right_shift? 9 : op == operator_t::greater? 10
             : op == operator_t::greater_equal? 11 : op == operator_t::less? 12 : op == operator_t::less_equal? 13
             : op == operator_t::equal? 14 : 15;
    }

    constexpr command_value_t __sc_value_binary(operator_t op, xil_type_t x1, xil_type_t x2, int tail)
    {
        return (command_value_t)(__SuperBase + (__sc_op_index(op) * 8 + __sc_pair_index(x1, x2)) * 8 + tail);
    }

    constexpr command_value_t __sc_value_push2(xil_type_t x1, xil_type_t x2)
    {
        return (command_value_t)(__SuperBase + 1024 + __sc_type_index(x1) * 6 + __sc_type_index(x2));
    }

    constexpr command_value_t __sc_value_copy(xil_type_t x)         { return (command_value_t)(__SuperBase + 1100 + __sc_type_index(x)); }
    constexpr command_value_t __sc_value_store_field(xil_type_t x, bool local)
    {
        return (command_value_t)(__SuperBase + (local? 1260 : 1110) + __sc_type_index(x));
    }

    constexpr command_value_t __sc_value_this_field(xil_type_t x, bool local)
    {
        return (command_value_t)(__SuperBase + (local? 1250 : 1120) + __sc_type_index(x));
    }

    constexpr command_value_t __sc_value_convert(xil_type_t x1, xil_type_t x2)
    {
        return (command_value_t)(__SuperBase + 1130 + __sc_type_index(x1) * 6 + __sc_type_index(x2));
    }

    // The types of the elements of the arrays that a command has.
    constexpr int __sc_atype_index(xil_type_t x)
    {
        return x == xil_type_t::int8? 0 : x == xil_type_t::uint8? 1 : x == xil_type_t::int16? 2 : x == xil_type_t::uint16? 3
             : x == xil_type_t::int32? 4 : x == xil_type_t::uint32? 5 : x == xil_type_t::int64? 6 : x == xil_type_t::uint64? 7
             : x == xil_type_t::float_? 8 : x == xil_type_t::double_? 9 : x == xil_type_t::char_? 10 : x == xil_type_t::bool_? 11
             : x == xil_type_t::object? 12 : 13;
    }

    constexpr command_value_t __sc_value_array_load(xil_type_t x)            { return (command_value_t)(__SuperBase + 1170 + __sc_atype_index(x)); }
    constexpr command_value_t __sc_value_array_store(xil_type_t x, int tail) { return (command_value_t)(__SuperBase + 1190 + __sc_atype_index(x) * 2 + tail); }
    constexpr command_value_t __sc_value_push_ret(xil_type_t x)              { return (command_value_t)(__SuperBase + 1230 + __sc_atype_index(x)); }

    constexpr command_value_t __SuperSequenceCmd = __SuperBase + 1300;
    constexpr command_value_t __TierCmd          = __SuperBase + 1301;
    constexpr command_value_t __ProfileCmd       = __SuperBase + 1302;
    constexpr command_value_t __InlineEnterCmd   = __SuperBase + 1303;
    constexpr command_value_t __OsrGuardCmd      = __SuperBase + 1304;

    // Runs one command (the commands of a sequence, and the one that a profile command counts).
    static void __execute_one(command_execute_context_t & ctx, command_t * command);

    // An operand of a superinstruction: a local variable, an argument, a constant, or the stack.
    struct __sk_operand_t
    {
        fusion_operand_t    kind = fusion_operand_t::stack;
        int32_t             offset = 0;
        uint64_t            bits = 0;           // The constant.
    };

    // Gets the value of an operand.
    template<typename _v_t>
    __AlwaysInline static _v_t __sk_get(command_execute_context_t & ctx, const __sk_operand_t & operand)
    {
        switch (operand.kind)
        {
            case fusion_operand_t::local:
                return __SkLocal(_v_t, operand.offset);

            case fusion_operand_t::argument:
                return __SkArgument(_v_t, operand.offset);

            case fusion_operand_t::constant: {
                _v_t value;
                memcpy(&value, &operand.bits, sizeof(_v_t));
                return value;
            }

            default:
                return ctx.stack.pop<_v_t>();
        }
    }

    // The return of a method that returns one unit (the value is at the top of the stack): the commands of the xil ret do it.
    __AlwaysInline static void __sc_return(command_execute_context_t & ctx, int total_unit_size)
    {
        __calling_stub_t * stub = ctx.stub();
        rt_stack_unit_t * p = ctx.stack.pop(total_unit_size);
        ctx.pop_calling(stub);

        copy_array<1>(p + (total_unit_size - 1), p - 1);
    }

    // x OP y: the operands are locals, arguments, constants or the stack; the result is pushed (_tail 0), stored in a local variable
    // (_tail 1), or tells a jump (_tail 2: the jump goes when the result is the value that the command was made with).
    template<operator_t _op, xil_type_t _x1, xil_type_t _x2, int _tail>
    class __sc_binary_t : public __command_base_t<__sc_value_binary(_op, _x1, _x2, _tail)>
    {
        typedef __sc_binary_t           __self_t;
        typedef __vnum_t<_x1>           __t1;
        typedef __vnum_t<_x2>           __t2;

        static constexpr vtype_t __vt1 = to_vtype(_x1);
        static constexpr vtype_t __vt2 = to_vtype(_x2);

        typedef decltype(binary_operator_t<_op, __vt1, __vt2>()(std::declval<__t1>(), std::declval<__t2>())) __r_t;

    public:
        __sc_binary_t(const __sk_operand_t & left, const __sk_operand_t & right, int32_t aux, bool when, int32_t step)
            : __left(left), __right(right), __aux(aux), __when(when), __step(step) { }

        __BeginExecute(ctx)

            // The right operand is on the top of the stack, the left one is below it.
            __t2 b = __sk_get<__t2>(ctx, __This->__right);
            __t1 a = __sk_get<__t1>(ctx, __This->__left);

            // Integer division: the divisor is not zero, and the result fits (as the command of the operation says).
            if constexpr ((_op == operator_t::div || _op == operator_t::mod) && std::is_integral<__t1>::value && std::is_integral<__t2>::value)
            {
                if (b == 0)
                    __RaiseDivideByZeroException(ctx);

                if constexpr (std::is_signed<__t1>::value && std::is_signed<__t2>::value)
                {
                    if (b == (__t2)-1 && a == std::numeric_limits<__t1>::min())
                        __RaiseOverflowException(ctx);
                }
            }

            __r_t r = binary_operator_t<_op, __vt1, __vt2>()(a, b);

            if constexpr (_tail == 0)
            {
                ctx.stack.push(r);
            }
            else if constexpr (_tail == 1)
            {
                __SkLocal(__t1, __This->__aux) = static_cast<__t1>(r);
            }
            else if constexpr (_tail == 2)
            {
                if (((bool)r) == __This->__when)
                {
                    // A jump that goes back is a loop: the other threads get their turn.
                    if (__This->__step < 0)
                        rtlib::thread_poll();

                    ctx.current += __This->__step;
                }
            }
            else if constexpr (_tail == 3)    // stored, and a jump that always goes
            {
                __SkLocal(__t1, __This->__aux) = static_cast<__t1>(r);

                if (__This->__step < 0)
                    rtlib::thread_poll();

                ctx.current += __This->__step;
            }
            else    // returned (aux is the size of the frame)
            {
                ctx.stack.push(r);
                __sc_return(ctx, __This->__aux);
            }

        __EndExecute()

    private:
        __sk_operand_t  __left, __right;
        int32_t         __aux;
        bool            __when;
        int32_t         __step;
    };

    // push x, push y.
    template<xil_type_t _x1, xil_type_t _x2>
    class __sc_push2_t : public __command_base_t<__sc_value_push2(_x1, _x2)>
    {
        typedef __sc_push2_t    __self_t;
        typedef __vnum_t<_x1>   __t1;
        typedef __vnum_t<_x2>   __t2;

    public:
        __sc_push2_t(const __sk_operand_t & first, const __sk_operand_t & second) : __first(first), __second(second) { }

        __BeginExecute(ctx)

            __t1 a = __sk_get<__t1>(ctx, __This->__first);
            __t2 b = __sk_get<__t2>(ctx, __This->__second);

            ctx.stack.push(a);
            ctx.stack.push(b);

        __EndExecute()

    private:
        __sk_operand_t  __first, __second;
    };

    // push x, pop local.
    template<xil_type_t _x>
    class __sc_copy_t : public __command_base_t<__sc_value_copy(_x)>
    {
        typedef __sc_copy_t     __self_t;
        typedef __vnum_t<_x>    __t;

    public:
        __sc_copy_t(const __sk_operand_t & source, int32_t destination) : __source(source), __destination(destination) { }

        __BeginExecute(ctx)

            __SkLocal(__t, __This->__destination) = __sk_get<__t>(ctx, __This->__source);

        __EndExecute()

    private:
        __sk_operand_t  __source;
        int32_t         __destination;
    };

    // push x, convert.
    template<xil_type_t _x1, xil_type_t _x2>
    class __sc_convert_t : public __command_base_t<__sc_value_convert(_x1, _x2)>
    {
        typedef __sc_convert_t  __self_t;
        typedef __vnum_t<_x1>   __t1;
        typedef __vnum_t<_x2>   __t2;

    public:
        __sc_convert_t(const __sk_operand_t & source) : __source(source) { }

        __BeginExecute(ctx)

            __t1 a = __sk_get<__t1>(ctx, __This->__source);

            // The command of a conversion between the same type does nothing.
            if constexpr (std::is_same<__t1, __t2>::value)
                ctx.stack.push(a);
            else
                ctx.stack.push(__convert_value<__t2>(a));

        __EndExecute()

    private:
        __sk_operand_t  __source;
    };

    // [push x], push argument (an object) or a local variable that is an object, pop field: this.Field = x (x is in the stack when it is
    // not an operand).
    template<xil_type_t _x, bool _local>
    class __sc_store_field_t : public __command_base_t<__sc_value_store_field(_x, _local)>
    {
        typedef __sc_store_field_t  __self_t;
        typedef __vnum_t<_x>        __t;

    public:
        __sc_store_field_t(const __sk_operand_t & value, int32_t argument, int32_t field)
            : __value(value), __argument(argument), __field(field) { }

        __BeginExecute(ctx)

            rt_ref_t obj;
            if constexpr (_local)
                obj = __SkLocal(rt_ref_t, __This->__argument);
            else
                obj = __SkArgument(rt_ref_t, __This->__argument);

            __CheckNullReference(ctx, obj);

            __SkField(__t, obj, __This->__field) = __sk_get<__t>(ctx, __This->__value);

        __EndExecute()

    private:
        __sk_operand_t  __value;
        int32_t         __argument, __field;
    };

    // push argument (an object) or a local variable that is an object, push field: the field of the object (this.Field).
    template<xil_type_t _x, bool _local>
    class __sc_this_field_t : public __command_base_t<__sc_value_this_field(_x, _local)>
    {
        typedef __sc_this_field_t   __self_t;
        typedef __vnum_t<_x>        __t;

    public:
        __sc_this_field_t(int32_t argument, int32_t field) : __argument(argument), __field(field) { }

        __BeginExecute(ctx)

            rt_ref_t obj;
            if constexpr (_local)
                obj = __SkLocal(rt_ref_t, __This->__argument);
            else
                obj = __SkArgument(rt_ref_t, __This->__argument);

            __CheckNullReference(ctx, obj);

            ctx.stack.push(__SkField(__t, obj, __This->__field));

        __EndExecute()

    private:
        int32_t         __argument, __field;
    };

    // push x, ret: the value is returned.
    template<xil_type_t _x>
    class __sc_push_ret_t : public __command_base_t<__sc_value_push_ret(_x)>
    {
        typedef __sc_push_ret_t     __self_t;
        typedef __vnum_t<_x>        __t;

    public:
        __sc_push_ret_t(const __sk_operand_t & value, int32_t total_unit_size) : __value(value), __total(total_unit_size) { }

        __BeginExecute(ctx)

            ctx.stack.push<__t>(__sk_get<__t>(ctx, __This->__value));
            __sc_return(ctx, __This->__total);

        __EndExecute()

    private:
        __sk_operand_t  __value;
        int32_t         __total;
    };

    // [push index] [push array] push array_element: the element of an array of one dimension (the index and the array may be in the stack).
    template<xil_type_t _x>
    class __sc_array_load_t : public __command_base_t<__sc_value_array_load(_x)>
    {
        typedef __sc_array_load_t   __self_t;
        typedef __vnum_t<_x>        __t;

    public:
        __sc_array_load_t(const __sk_operand_t & index, const __sk_operand_t & array) : __index(index), __array(array) { }

        __BeginExecute(ctx)

            // The array is on the top of the stack, the index is below it.
            rt_ref_t array_ref = __sk_get<rt_ref_t>(ctx, __This->__array);
            __CheckNullReference(ctx, array_ref);
            array_length_t index = __sk_get<array_length_t>(ctx, __This->__index);

            if ((uint32_t)index >= (uint32_t)mm::get_array_length(array_ref))
                __RaiseOutOfRangeException(ctx);

            ctx.stack.push<__t>(mm::get_array_element<__t>(array_ref, index));

        __EndExecute()

    private:
        __sk_operand_t  __index, __array;
    };

    // [push x] [push index] [push array] pop array_element [jmp]: _tail 1 is a jump that always goes, after the store.
    template<xil_type_t _x, int _tail>
    class __sc_array_store_t : public __command_base_t<__sc_value_array_store(_x, _tail)>
    {
        typedef __sc_array_store_t  __self_t;
        typedef __vnum_t<_x>        __t;

    public:
        __sc_array_store_t(const __sk_operand_t & value, const __sk_operand_t & index, const __sk_operand_t & array, int32_t step)
            : __value(value), __index(index), __array(array), __step(step) { }

        __BeginExecute(ctx)

            rt_ref_t array_ref = __sk_get<rt_ref_t>(ctx, __This->__array);
            __CheckNullReference(ctx, array_ref);
            array_length_t index = __sk_get<array_length_t>(ctx, __This->__index);

            if ((uint32_t)index >= (uint32_t)mm::get_array_length(array_ref))
                __RaiseOutOfRangeException(ctx);

            mm::set_array_element<__t>(array_ref, index, __sk_get<__t>(ctx, __This->__value));

            if constexpr (_tail == 1)
            {
                if (__This->__step < 0)
                    rtlib::thread_poll();

                ctx.current += __This->__step;
            }

        __EndExecute()

    private:
        __sk_operand_t  __value, __index, __array;
        int32_t         __step;
    };

    // The commands of a group of xils, one after the other: for a group that has a variable that the superinstruction does not know (a local
    // variable that has not the type of the xil). It is one command, as the plan says.
    class __sc_sequence_t : public __command_base_t<__SuperSequenceCmd>
    {
        typedef __sc_sequence_t __self_t;

    public:
        __sc_sequence_t(command_t ** commands, size_t count) : __commands(commands), __count(count) { }

        __BeginExecute(ctx)

            for (size_t k = 0; k < __This->__count; k++)
                __execute_one(ctx, __This->__commands[k]);

        __EndExecute()

    private:
        command_t ** __commands;
        size_t       __count;
    };

    //-------- ---------- ---------- ---------- ----------

    struct __sc_args_t
    {
        __sk_operand_t  left, right;
        int32_t         aux = 0;
        bool            when = false;
        int32_t         step = 0;
    };

    template<operator_t _op> constexpr bool __sc_is_cmp()
    {
        return _op == operator_t::greater || _op == operator_t::greater_equal || _op == operator_t::less
            || _op == operator_t::less_equal || _op == operator_t::equal || _op == operator_t::not_equal;
    }

    template<operator_t _op> constexpr bool __sc_is_bit()
    {
        return _op == operator_t::bit_and || _op == operator_t::bit_or || _op == operator_t::bit_xor
            || _op == operator_t::left_shift || _op == operator_t::right_shift;
    }

    constexpr bool __sc_is_float(xil_type_t type) { return type == xil_type_t::float_ || type == xil_type_t::double_; }

    template<operator_t _op, xil_type_t _x1, xil_type_t _x2>
    static command_t * __sc_make_binary(int tail, const __sc_args_t & a)
    {
        if constexpr (__sc_is_bit<_op>() && (__sc_is_float(_x1) || __sc_is_float(_x2)))
        {
            return nullptr;
        }
        else
        {
            switch (tail)
            {
                case 0:
                    return __new_command<__sc_binary_t<_op, _x1, _x2, 0>>((memory_t *)&__command_heap, a.left, a.right, a.aux, a.when, a.step);

                case 1:
                    if constexpr (!__sc_is_cmp<_op>())
                        return __new_command<__sc_binary_t<_op, _x1, _x2, 1>>((memory_t *)&__command_heap, a.left, a.right, a.aux, a.when, a.step);
                    else
                        return nullptr;

                case 3:
                    if constexpr (!__sc_is_cmp<_op>())
                        return __new_command<__sc_binary_t<_op, _x1, _x2, 3>>((memory_t *)&__command_heap, a.left, a.right, a.aux, a.when, a.step);
                    else
                        return nullptr;

                case 4:
                    return __new_command<__sc_binary_t<_op, _x1, _x2, 4>>((memory_t *)&__command_heap, a.left, a.right, a.aux, a.when, a.step);

                default:
                    if constexpr (__sc_is_cmp<_op>())
                        return __new_command<__sc_binary_t<_op, _x1, _x2, 2>>((memory_t *)&__command_heap, a.left, a.right, a.aux, a.when, a.step);
                    else
                        return nullptr;
            }
        }
    }

    template<operator_t _op>
    static command_t * __sc_dispatch_types(xil_type_t x1, xil_type_t x2, int tail, const __sc_args_t & a)
    {
        #define __SC(_a, _b)                                                                    \
            if (x1 == xil_type_t::_a && x2 == xil_type_t::_b)                                   \
                return __sc_make_binary<_op, xil_type_t::_a, xil_type_t::_b>(tail, a);

        __SC(int32, int32) __SC(uint32, uint32) __SC(int64, int64) __SC(uint64, uint64)
        __SC(float_, float_) __SC(double_, double_) __SC(int64, int32)

        #undef __SC

        return nullptr;
    }

    static command_t * __sc_dispatch_op(operator_t op, xil_type_t x1, xil_type_t x2, int tail, const __sc_args_t & a)
    {
        #define __SC(_o) case operator_t::_o: return __sc_dispatch_types<operator_t::_o>(x1, x2, tail, a);

        switch (op)
        {
            __SC(add) __SC(sub) __SC(mul) __SC(div) __SC(mod)
            __SC(bit_and) __SC(bit_or) __SC(bit_xor) __SC(left_shift) __SC(right_shift)
            __SC(greater) __SC(greater_equal) __SC(less) __SC(less_equal) __SC(equal) __SC(not_equal)

            default:
                return nullptr;
        }

        #undef __SC
    }

    template<xil_type_t _x1>
    static command_t * __sc_dispatch_push2_second(xil_type_t x2, const __sk_operand_t & first, const __sk_operand_t & second)
    {
        #define __SC(_b)                                                                        \
            if (x2 == xil_type_t::_b)                                                           \
                return __new_command<__sc_push2_t<_x1, xil_type_t::_b>>((memory_t *)&__command_heap, first, second);

        __SC(int32) __SC(uint32) __SC(int64) __SC(uint64) __SC(float_) __SC(double_)

        #undef __SC

        return nullptr;
    }

    static command_t * __sc_dispatch_push2(xil_type_t x1, xil_type_t x2, const __sk_operand_t & first, const __sk_operand_t & second)
    {
        #define __SC(_a)                                                                        \
            if (x1 == xil_type_t::_a)                                                           \
                return __sc_dispatch_push2_second<xil_type_t::_a>(x2, first, second);

        __SC(int32) __SC(uint32) __SC(int64) __SC(uint64) __SC(float_) __SC(double_)

        #undef __SC

        return nullptr;
    }

    static command_t * __sc_dispatch_copy(xil_type_t x, const __sk_operand_t & source, int32_t destination)
    {
        #define __SC(_a)                                                                        \
            if (x == xil_type_t::_a)                                                            \
                return __new_command<__sc_copy_t<xil_type_t::_a>>((memory_t *)&__command_heap, source, destination);

        __SC(int32) __SC(uint32) __SC(int64) __SC(uint64) __SC(float_) __SC(double_)

        #undef __SC

        return nullptr;
    }

    template<xil_type_t _x1>
    static command_t * __sc_dispatch_convert_to(xil_type_t x2, const __sk_operand_t & source)
    {
        #define __SC(_b)                                                                        \
            if (x2 == xil_type_t::_b)                                                           \
                return __new_command<__sc_convert_t<_x1, xil_type_t::_b>>((memory_t *)&__command_heap, source);

        __SC(int32) __SC(uint32) __SC(int64) __SC(uint64) __SC(float_) __SC(double_)

        #undef __SC

        return nullptr;
    }

    static command_t * __sc_dispatch_convert(xil_type_t x1, xil_type_t x2, const __sk_operand_t & source)
    {
        #define __SC(_a)                                                                        \
            if (x1 == xil_type_t::_a)                                                           \
                return __sc_dispatch_convert_to<xil_type_t::_a>(x2, source);

        __SC(int32) __SC(uint32) __SC(int64) __SC(uint64) __SC(float_) __SC(double_)

        #undef __SC

        return nullptr;
    }

    static command_t * __sc_dispatch_store_field(xil_type_t x, bool local, const __sk_operand_t & value, int32_t argument, int32_t field)
    {
        #define __SC(_a)                                                                        \
            if (x == xil_type_t::_a)                                                            \
            {                                                                                   \
                if (local)                                                                      \
                    return __new_command<__sc_store_field_t<xil_type_t::_a, true>>((memory_t *)&__command_heap, value, argument, field);  \
                                                                                                \
                return __new_command<__sc_store_field_t<xil_type_t::_a, false>>((memory_t *)&__command_heap, value, argument, field);     \
            }

        __SC(int32) __SC(uint32) __SC(int64) __SC(uint64) __SC(float_) __SC(double_)

        #undef __SC

        return nullptr;
    }

    static command_t * __sc_dispatch_this_field(xil_type_t x, bool local, int32_t argument, int32_t field)
    {
        #define __SC(_a)                                                                        \
            if (x == xil_type_t::_a)                                                            \
            {                                                                                   \
                if (local)                                                                      \
                    return __new_command<__sc_this_field_t<xil_type_t::_a, true>>((memory_t *)&__command_heap, argument, field);  \
                                                                                                \
                return __new_command<__sc_this_field_t<xil_type_t::_a, false>>((memory_t *)&__command_heap, argument, field);     \
            }

        __SC(int32) __SC(uint32) __SC(int64) __SC(uint64) __SC(float_) __SC(double_)

        #undef __SC

        return nullptr;
    }

    #define __SC_ATYPES(_m)                                                                 \
        _m(int8) _m(uint8) _m(int16) _m(uint16) _m(int32) _m(uint32) _m(int64) _m(uint64)   \
        _m(float_) _m(double_) _m(char_) _m(bool_) _m(object) _m(string)

    static command_t * __sc_dispatch_push_ret(xil_type_t x, const __sk_operand_t & value, int32_t total)
    {
        #define __SC(_a)                                                                        \
            if (x == xil_type_t::_a)                                                            \
                return __new_command<__sc_push_ret_t<xil_type_t::_a>>((memory_t *)&__command_heap, value, total);

        __SC_ATYPES(__SC)

        #undef __SC

        return nullptr;
    }

    // The size of the frame of the method that returns one unit, false when it does not (a struct, nothing).
    static bool __sc_return_frame(__context_t & ctx, int32_t * total)
    {
        if (is_custom_struct(ctx.env, ctx.method->get_return_type(ctx.env)) || ctx.ret_unit_size() != 1)
            return false;

        *total = (int32_t)(ctx.params_layout.unit_size() + __stack_stub_size + ctx.locals_layout.unit_size());
        return true;
    }

    static command_t * __sc_dispatch_array_load(xil_type_t x, const __sk_operand_t & index, const __sk_operand_t & array)
    {
        #define __SC(_a)                                                                        \
            if (x == xil_type_t::_a)                                                            \
                return __new_command<__sc_array_load_t<xil_type_t::_a>>((memory_t *)&__command_heap, index, array);

        __SC_ATYPES(__SC)

        #undef __SC

        return nullptr;
    }

    static command_t * __sc_dispatch_array_store(xil_type_t x, int tail, const __sk_operand_t & value, const __sk_operand_t & index,
                                                 const __sk_operand_t & array, int32_t step)
    {
        #define __SC(_a)                                                                        \
            if (x == xil_type_t::_a)                                                            \
            {                                                                                   \
                if (tail == 0)                                                                  \
                    return __new_command<__sc_array_store_t<xil_type_t::_a, 0>>((memory_t *)&__command_heap, value, index, array, step); \
                                                                                                \
                return __new_command<__sc_array_store_t<xil_type_t::_a, 1>>((memory_t *)&__command_heap, value, index, array, step);     \
            }

        __SC_ATYPES(__SC)

        #undef __SC

        return nullptr;
    }

    // Makes the operand of a push xil of a number: false when the variable has not the type of the xil.
    static bool __sk_make_operand(__context_t & ctx, const push_xil_t & push, xil_type_t want, __sk_operand_t * out)
    {
        xil_type_t xt1, xt2;

        switch (push.stype())
        {
            case xil_storage_type_t::local:
                __analyze_xil_types(ctx, push, &xt1, &xt2, false);
                if (xt1 != want || xt2 != want)
                    return false;

                out->kind = fusion_operand_t::local;
                out->offset = (int32_t)ctx.locals_layout.offset_of(push.get_identity());
                return true;

            case xil_storage_type_t::argument:
                __analyze_xil_types(ctx, push, &xt1, &xt2, false);
                if (xt1 != want || xt2 != want)
                    return false;

                out->kind = fusion_operand_t::argument;
                out->offset = (int32_t)(__offset_of_argument(ctx, push.get_identity()) + __stack_stub_size);
                return true;

            case xil_storage_type_t::constant:
                if (push.dtype() != want)
                    return false;

                out->kind = fusion_operand_t::constant;
                out->bits = 0;
                memcpy(&out->bits, &push.get_extra<byte_t>(), size_of(want));
                return true;

            default:
                return false;
        }
    }

    // The operand of the push of an object that has a field read or written: an argument (its type is not checked, it is the this of a class
    // or a struct), or a local variable that is an object. The kind is the stack when the variable has not the type.
    static __sk_operand_t __sk_object_operand(__context_t & ctx, const push_xil_t & push)
    {
        __sk_operand_t operand;

        if (push.stype() == xil_storage_type_t::local)
        {
            if (!__sk_make_operand(ctx, push, xil_type_t::object, &operand))
                operand.kind = fusion_operand_t::stack;

            return operand;
        }

        operand.kind = fusion_operand_t::argument;
        operand.offset = (int32_t)(__offset_of_argument(ctx, push.get_identity()) + __stack_stub_size);
        return operand;
    }

    // The index of the command of an xil that is a jump, and the step that its command takes.
    static int32_t __sc_jump_step(const fusion_plan_t & plan, size_t xil_index, const jmp_xil_t & jmp)
    {
        return (int32_t)plan.index_map[xil_index + jmp.step()] - (int32_t)plan.index_map[xil_index] - 1;
    }

    // The groups that became a sequence of commands (the statistics of the calls tell it).
    static std::atomic<uint64_t> __sequences_made { 0 };

    // Makes the superinstruction of a group, nullptr when something is not what the xils say.
    static command_t * __sc_build(__context_t & ctx, const fusion_plan_t & plan, const xil_base_t * const * x,
                                  const fusion_group_t & group, const fusion_shape_t & shape)
    {
        switch (shape.form)
        {
            case fusion_form_t::binary: {
                const xil_base_t * op_xil = x[shape.op_index];
                operator_t op;
                xil_type_t x1, x2;

                switch (command_of(op_xil))
                {
                    case xil_command_t::al:
                        op = to_operator(((const al_xil_t *)op_xil)->cmd());
                        x1 = ((const al_xil_t *)op_xil)->dtype1(), x2 = ((const al_xil_t *)op_xil)->dtype2();
                        break;

                    case xil_command_t::bit:
                        op = to_operator(((const bit_xil_t *)op_xil)->cmd());
                        x1 = ((const bit_xil_t *)op_xil)->dtype1(), x2 = ((const bit_xil_t *)op_xil)->dtype2();
                        break;

                    default:
                        op = to_operator(((const cmp_xil_t *)op_xil)->cmd());
                        x1 = ((const cmp_xil_t *)op_xil)->dtype1(), x2 = ((const cmp_xil_t *)op_xil)->dtype2();
                        break;
                }

                __sc_args_t a;

                if (shape.left != fusion_operand_t::stack
                        && !__sk_make_operand(ctx, *(const push_xil_t *)x[shape.left_index], x1, &a.left))
                    return nullptr;

                if (shape.right != fusion_operand_t::stack
                        && !__sk_make_operand(ctx, *(const push_xil_t *)x[shape.right_index], x2, &a.right))
                    return nullptr;

                if (shape.tail == 1 || shape.tail == 3)
                {
                    const pop_xil_t & pop = *(const pop_xil_t *)x[shape.tail_index];
                    xil_type_t pt1, pt2;
                    __analyze_xil_types(ctx, pop, &pt1, &pt2, false);
                    if (pt1 != x1 || pt2 != x1)
                        return nullptr;

                    a.aux = (int32_t)ctx.locals_layout.offset_of(pop.get_identity());

                    if (shape.tail == 3)
                        a.step = __sc_jump_step(plan, group.start + shape.jump_index, *(const jmp_xil_t *)x[shape.jump_index]);
                }
                else if (shape.tail == 2)
                {
                    const jmp_xil_t & jmp = *(const jmp_xil_t *)x[shape.tail_index];
                    a.when = jmp.model() == xil_jmp_model_t::true_;
                    a.step = __sc_jump_step(plan, group.start + shape.tail_index, jmp);
                }
                else if (shape.tail == 4)
                {
                    if (!__sc_return_frame(ctx, &a.aux))
                        return nullptr;
                }

                return __sc_dispatch_op(op, x1, x2, shape.tail, a);
            }

            case fusion_form_t::push2: {
                const push_xil_t & p1 = *(const push_xil_t *)x[0], & p2 = *(const push_xil_t *)x[1];
                __sk_operand_t first, second;
                if (!__sk_make_operand(ctx, p1, p1.dtype(), &first) || !__sk_make_operand(ctx, p2, p2.dtype(), &second))
                    return nullptr;

                return __sc_dispatch_push2(p1.dtype(), p2.dtype(), first, second);
            }

            case fusion_form_t::copy: {
                const push_xil_t & p = *(const push_xil_t *)x[0];
                const pop_xil_t & pop = *(const pop_xil_t *)x[1];

                __sk_operand_t source;
                if (!__sk_make_operand(ctx, p, p.dtype(), &source))
                    return nullptr;

                xil_type_t pt1, pt2;
                __analyze_xil_types(ctx, pop, &pt1, &pt2, false);
                if (pt1 != p.dtype() || pt2 != p.dtype())
                    return nullptr;

                return __sc_dispatch_copy(p.dtype(), source, (int32_t)ctx.locals_layout.offset_of(pop.get_identity()));
            }

            case fusion_form_t::convert: {
                const push_xil_t & p = *(const push_xil_t *)x[shape.left_index], & conv = *(const push_xil_t *)x[shape.op_index];

                __sk_operand_t source;
                if (!__sk_make_operand(ctx, p, p.dtype(), &source))
                    return nullptr;

                return __sc_dispatch_convert(conv.dtype(), conv.dtype2(), source);
            }

            case fusion_form_t::store_field: {
                const push_xil_t & self = *(const push_xil_t *)x[shape.right_index];
                const pop_xil_t & pop = *(const pop_xil_t *)x[shape.op_index];

                xil_type_t ft1, ft2;
                __analyze_xil_types(ctx, pop, &ft1, &ft2, false);
                if (ft1 != pop.dtype() || ft2 != pop.dtype())
                    return nullptr;

                __sk_operand_t value;
                if (shape.left != fusion_operand_t::stack
                        && !__sk_make_operand(ctx, *(const push_xil_t *)x[shape.left_index], pop.dtype(), &value))
                    return nullptr;

                // The object: an argument, or a local variable that is an object.
                __sk_operand_t object = __sk_object_operand(ctx, self);
                if (object.kind == fusion_operand_t::stack)
                    return nullptr;

                return __sc_dispatch_store_field(pop.dtype(), object.kind == fusion_operand_t::local, value, object.offset,
                    (int32_t)__field_offset(ctx, pop.get_ref()));
            }

            case fusion_form_t::this_field: {
                const push_xil_t & p = *(const push_xil_t *)x[0], & field = *(const push_xil_t *)x[1];

                xil_type_t ft1, ft2;
                __analyze_xil_types(ctx, field, &ft1, &ft2, false);
                if (ft1 != field.dtype() || ft2 != field.dtype())
                    return nullptr;

                __sk_operand_t object = __sk_object_operand(ctx, p);
                if (object.kind == fusion_operand_t::stack)
                    return nullptr;

                return __sc_dispatch_this_field(field.dtype(), object.kind == fusion_operand_t::local, object.offset,
                    (int32_t)__field_offset(ctx, field.get_ref()));
            }

            case fusion_form_t::ret_push: {
                const push_xil_t & p = *(const push_xil_t *)x[0];

                int32_t total;
                if (!__sc_return_frame(ctx, &total))
                    return nullptr;

                // The type of the variable (the type of the return value is the one of the xil).
                xil_type_t t1, t2;
                if (p.stype() != xil_storage_type_t::constant)
                {
                    __analyze_xil_types(ctx, p, &t1, &t2, false);
                    if (t1 != t2)
                        return nullptr;
                }

                __sk_operand_t value;
                if (!__sk_make_operand(ctx, p, p.dtype(), &value))
                    return nullptr;

                return __sc_dispatch_push_ret(p.dtype(), value, total);
            }

            case fusion_form_t::array_load:
            case fusion_form_t::array_store: {
                bool is_store = shape.form == fusion_form_t::array_store;

                // The array has one dimension only (the commands of the others take an index for each one).
                const xil_base_t * op = x[shape.op_index];
                const push_xil_t & array_push = *(const push_xil_t *)x[shape.array_index];

                xil_type_t et1, et2;
                if (is_store)
                {
                    const pop_xil_t & pop = *(const pop_xil_t *)op;
                    __analyze_xil_types(ctx, pop, &et1, &et2, false);
                    if (__array_dimension(ctx, pop.get_ref()) != 1)
                        return nullptr;
                }
                else
                {
                    const push_xil_t & push = *(const push_xil_t *)op;
                    __analyze_xil_types(ctx, push, &et1, &et2, false);
                    if (__array_dimension(ctx, push.get_ref()) != 1)
                        return nullptr;
                }

                // The types of the element and of the value in the stack are the same (the command of the xil makes the others).
                if (et1 != et2)
                    return nullptr;

                __sk_operand_t array, index, value;
                if (!__sk_make_operand(ctx, array_push, xil_type_t::object, &array))
                    return nullptr;

                if (shape.index != fusion_operand_t::stack
                        && !__sk_make_operand(ctx, *(const push_xil_t *)x[shape.index_index], xil_type_t::int32, &index))
                    return nullptr;

                if (!is_store)
                    return __sc_dispatch_array_load(et1, index, array);

                if (shape.left != fusion_operand_t::stack
                        && !__sk_make_operand(ctx, *(const push_xil_t *)x[shape.left_index], et1, &value))
                    return nullptr;

                int32_t step = 0;
                if (shape.tail == 1)
                    step = __sc_jump_step(plan, group.start + shape.jump_index, *(const jmp_xil_t *)x[shape.jump_index]);

                return __sc_dispatch_array_store(et1, shape.tail, value, index, array, step);
            }

            default:
                return nullptr;
        }
    }

    command_t * new_super_command(command_creating_context_t & ctx, const fusion_plan_t & plan,
                                  const xil_base_t * const * xils, const fusion_group_t & group)
    {
        const xil_base_t * const * x = xils + group.start;

        fusion_shape_t shape;
        command_t * command = nullptr;

        if (fusion_match(x, group.length, group.length, &shape) && shape.length == group.length)
            command = __sc_build(ctx, plan, x, group, shape);

        if (command != nullptr)
            return command;

        // The xils one after the other.
        __sequences_made++;
        command_t ** members = (command_t **)((memory_t *)&__command_heap)->alloc(sizeof(command_t *) * group.length);
        for (uint32_t k = 0; k < group.length; k++)
        {
            ctx.xil_index = group.start + k;
            members[k] = new_command(ctx, xils[group.start + k]);
        }

        return __new_command<__sc_sequence_t>((memory_t *)&__command_heap, members, (size_t)group.length);
    }

    //-------- ---------- ---------- ---------- ----------
    // The inlining of a call.

    // The first command of the body of an inlined call: the args that the caller pushed are moved from the stack to their places in the
    // frame of the caller (the body reads them as local variables), the type of a static method is initialized, the object of a call
    // of a class is checked for null: what the command of the call did before the frame was made.
    class __inline_enter_command_t : public __command_base_t<__InlineEnterCmd>
    {
        typedef __inline_enter_command_t    __self_t;

    public:
        __inline_enter_command_t(const inline_site_t & site)
            : __count(site.stack_args), __check_this(site.check_this), __static_type(site.static_type), __delegate_mode(site.delegate_mode)
        {
            for (int k = 0; k < site.arg_count; k++)
                __offsets[k] = site.arg_offsets[k];
        }

        __BeginExecute(ctx)

            if (__This->__static_type != nullptr)
                __This->__static_type->pre_static_call(ctx.env);

            // A call of a delegate: the delegate is the first unit. The method has the object of the delegate for this, or it is static (it has
            // no this: the delegate is dropped).
            const int skip = __This->__delegate_mode == 2? 1 : 0;
            rt_stack_unit_t * args = ctx.stack.pop((size_t)(__This->__count + skip));
            byte_t * frame = (byte_t *)ctx.stack.lp();

            if (__This->__delegate_mode == 1)
                args[0] = (rt_stack_unit_t)(uintptr_t)__Field(rt_object_t *, *(rt_object_t **)&args[0], __delegate_instance_field_offset);

            for (int k = 0; k < __This->__count; k++)
                *(rt_stack_unit_t *)(frame + __This->__offsets[k]) = args[k + skip];

            if (__This->__check_this)
            {
                rt_ref_t self = *(rt_ref_t *)(frame + __This->__offsets[0]);
                __CheckNullReference(ctx, self);
            }

        __EndExecute()

    private:
        int         __count;
        bool        __check_this;
        rt_type_t * __static_type;
        int         __delegate_mode;
        int32_t     __offsets[8];
    };

    command_t * new_inline_enter_command(const inline_site_t & site)
    {
        return __new_command<__inline_enter_command_t>((memory_t *)&__command_heap, site);
    }

    // The method of a call that can be inlined.
    exec_method_t * inline_call_target(command_creating_context_t & ctx, const call_xil_t & xil, bool * is_static)
    {
        switch (xil.call_type())
        {
            case xil_call_type_t::static_:
                *is_static = true;
                return __static_call_target(ctx, xil);

            case xil_call_type_t::instance:
                *is_static = false;
                return __instance_call_target(ctx, xil);

            default:
                return nullptr;
        }
    }

    // Whether the body of a callee can be put in the frame of the caller.
    bool inline_check_body(command_creating_context_t & callee_ctx, const std::vector<const xil_base_t *> & xils, size_t max_xils)
    {
        if (xils.size() < 1 || xils.size() > max_xils)
            return false;

        // The params are values of one unit each, the result is nothing or one unit (not a struct).
        if (!callee_ctx.params_layout.is_simple() || callee_ctx.params_layout.count() > 8)
            return false;

        if (is_custom_struct(callee_ctx.env, callee_ctx.method->get_return_type(callee_ctx.env)) || callee_ctx.ret_unit_size() > 1)
            return false;

        // The last xil is a ret, the jumps stay in the body, no protected block (the caller checks that), no switch, nothing that needs a
        // frame of its own. A ret that is not the last one becomes a jump to the end of the body.
        const xil_base_t * last = xils.back();
        if (command_of(last) != xil_command_t::smp || ((const smp_xil_t *)last)->smp() != xil_smp_t::ret)
            return false;

        for (size_t k = 0; k + 1 < xils.size(); k++)
        {
            const xil_base_t * xil = xils[k];

            switch (command_of(xil))
            {
                case xil_command_t::push: {
                    const push_xil_t * push = (const push_xil_t *)xil;
                    switch (push->stype())
                    {
                        case xil_storage_type_t::local:
                        case xil_storage_type_t::local_addr:
                        case xil_storage_type_t::constant:
                        case xil_storage_type_t::field:
                        case xil_storage_type_t::field_addr:
                        case xil_storage_type_t::array_element:
                        case xil_storage_type_t::array_element_addr:
                        case xil_storage_type_t::convert:
                        case xil_storage_type_t::checked_convert:
                        case xil_storage_type_t::duplicate:
                        case xil_storage_type_t::object:
                            break;

                        case xil_storage_type_t::argument:
                        case xil_storage_type_t::argument_addr:
                            if (push->get_identity() == XIL_CALLING_BOTTOM_IDENTITY)
                                return false;

                            break;

                        default:
                            return false;
                    }
                }   break;

                case xil_command_t::pop: {
                    const pop_xil_t * pop = (const pop_xil_t *)xil;
                    switch (pop->stype())
                    {
                        case xil_storage_type_t::local:
                        case xil_storage_type_t::argument:
                        case xil_storage_type_t::field:
                        case xil_storage_type_t::array_element:
                        case xil_storage_type_t::empty:
                            break;

                        default:
                            return false;
                    }
                }   break;

                case xil_command_t::al:
                case xil_command_t::bit:
                case xil_command_t::logic:
                case xil_command_t::cmp:
                case xil_command_t::cast:
                    break;

                case xil_command_t::jmp: {
                    // A jump to a place of the body (the end of it is the place of the last ret).
                    const jmp_xil_t * jmp = (const jmp_xil_t *)xil;
                    if (jmp->model() == xil_jmp_model_t::switch_)
                        return false;

                    // A loop is not inlined: the loop is the cost, the call is not (and its own calls are counted when it is a method).
                    if (jmp->step() <= 0)
                        return false;

                    int64_t target = (int64_t)k + jmp->step();
                    if (target < 0 || target > (int64_t)xils.size() - 1)
                        return false;
                }   break;

                case xil_command_t::smp: {
                    xil_smp_t smp = ((const smp_xil_t *)xil)->smp();
                    if (smp != xil_smp_t::ret && smp != xil_smp_t::throw_)
                        return false;
                }   break;

                case xil_command_t::call: {
                    // The calls that use the frame of the callee (the arguments of the internal functions) are not here.
                    xil_call_type_t type = ((const call_xil_t *)xil)->call_type();
                    if (type != xil_call_type_t::static_ && type != xil_call_type_t::instance && type != xil_call_type_t::virtual_)
                        return false;
                }   break;

                case xil_command_t::new_: {
                    xil_new_type_t type = ((const new_xil_t *)xil)->new_type();
                    if (type != xil_new_type_t::default_ && type != xil_new_type_t::array)
                        return false;
                }   break;

                case xil_command_t::copy: {
                    // The copies of the memory take the addresses from the stack. A generic param that is a custom struct is
                    // returned through the address that is below the args (the frame of the callee): not here.
                    const copy_xil_t * copy = (const copy_xil_t *)xil;

                    if (copy->copy_type() == xil_copy_type_t::generic_copy)
                    {
                        switch (copy->copy_kind())
                        {
                            case xil_copy_kind_t::ret:
                            case xil_copy_kind_t::receiver:
                            case xil_copy_kind_t::call_ret:
                            case xil_copy_kind_t::store:
                                if (is_custom_struct(callee_ctx.env, callee_ctx.get_type(copy->type_ref())))
                                    return false;

                                break;

                            default:
                                return false;
                        }
                    }
                    else if (copy->copy_type() != xil_copy_type_t::object_copy)
                    {
                        return false;
                    }
                }   break;

                default:
                    return false;
            }
        }

        return true;
    }

    // Whether the body is the one of an accessor.
    bool inline_is_accessor_shape(const std::vector<const xil_base_t *> & xils)
    {
        auto is_ret = [](const xil_base_t * xil) {
            return command_of(xil) == xil_command_t::smp && ((const smp_xil_t *)xil)->smp() == xil_smp_t::ret;
        };

        auto is_push = [](const xil_base_t * xil, xil_storage_type_t stype) {
            return command_of(xil) == xil_command_t::push && ((const push_xil_t *)xil)->stype() == stype;
        };

        switch (xils.size())
        {
            case 1:
                return is_ret(xils[0]);

            case 3:
                return is_ret(xils[2]) && is_push(xils[0], xil_storage_type_t::argument)
                    && (is_push(xils[1], xil_storage_type_t::field) || command_of(xils[1]) == xil_command_t::call);

            case 4:
                return is_ret(xils[3]) && is_push(xils[0], xil_storage_type_t::argument) && is_push(xils[1], xil_storage_type_t::argument)
                    && command_of(xils[2]) == xil_command_t::pop && ((const pop_xil_t *)xils[2])->stype() == xil_storage_type_t::field;

            default:
                return false;
        }
    }

    // The xil of the body of a callee for the caller: the args are local variables.
    const xil_base_t * inline_rewrite_xil(const inline_site_t & site, const xil_base_t * xil, std::deque<std::vector<byte_t>> & storage,
                                          size_t index, size_t size)
    {
        // A ret in the middle of the body: the value is at the top of the stack, the jump goes to the end (the last ret is not copied).
        if (command_of(xil) == xil_command_t::smp && ((const smp_xil_t *)xil)->smp() == xil_smp_t::ret)
        {
            storage.emplace_back(sizeof(jmp_xil_t) + 4, 0);
            return new (storage.back().data()) jmp_xil_t(xil_jmp_model_t::none, (xil_jmp_step_t)(size - 1 - index));
        }

        if (command_of(xil) == xil_command_t::push)
        {
            const push_xil_t * push = (const push_xil_t *)xil;
            xil_storage_type_t stype = push->stype();

            // An arg that the caller did not push: the variable (the constant) that it was made of.
            if (stype == xil_storage_type_t::argument && site.forward_kind[push->get_identity()] != 0)
            {
                int k = push->get_identity();

                if (site.forward_kind[k] == 3)
                    return site.forward_xil[k];

                storage.emplace_back(sizeof(push_xil_t) + 4, 0);

                push_xil_t * clone = new (storage.back().data()) push_xil_t(
                    site.forward_kind[k] == 1? xil_storage_type_t::local : xil_storage_type_t::local_addr, push->dtype()
                );

                clone->set_identity((uint16_t)site.arg_identities[k]);
                return clone;
            }

            if (stype == xil_storage_type_t::argument || stype == xil_storage_type_t::argument_addr)
            {
                storage.emplace_back(sizeof(push_xil_t) + 4, 0);

                push_xil_t * clone = new (storage.back().data()) push_xil_t(
                    stype == xil_storage_type_t::argument? xil_storage_type_t::local : xil_storage_type_t::local_addr, push->dtype()
                );

                clone->set_identity((uint16_t)site.arg_identities[push->get_identity()]);
                return clone;
            }
        }
        else if (command_of(xil) == xil_command_t::pop)
        {
            const pop_xil_t * pop = (const pop_xil_t *)xil;

            if (pop->stype() == xil_storage_type_t::argument)
            {
                storage.emplace_back(sizeof(pop_xil_t) + 4, 0);

                pop_xil_t * clone = new (storage.back().data()) pop_xil_t(xil_storage_type_t::local, pop->dtype());
                clone->set_identity((uint16_t)site.arg_identities[pop->get_identity()]);
                return clone;
            }
        }

        return xil;
    }

    //-------- ---------- ---------- ---------- ----------
    // The replacing of the commands of a method (exec_osr_t).

    #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL

    bool exec_osr_enabled()
    {
        static const bool enabled = []() {
            const char * value = std::getenv("APRIL_OSR");
            return value == nullptr || std::string(value) != "0";
        }();

        return enabled;
    }

    int exec_osr_threshold()
    {
        static const int threshold = []() {
            const char * value = std::getenv("APRIL_OSR_THRESHOLD");
            int n = value != nullptr? std::atoi(value) : 0;
            return n > 0? n : 2000;
        }();

        return threshold;
    }

    bool exec_osr_is_candidate(command_creating_context_t & ctx, const call_xil_t & xil)
    {
        if (xil.call_type() != xil_call_type_t::virtual_)
            return false;

        rt_method_base_t * method = ctx.get_method((ref_t)xil.method);
        return method != nullptr && method->this_type() != rt_member_type_t::dynamic;
    }

    bool exec_osr_attach(command_t * command, exec_call_profile_t * profile)
    {
        __osr_call_t * call = dynamic_cast<__osr_call_t *>(command);
        if (call == nullptr)
            return false;

        call->profile = profile;
        profile->command = command;

        if (!profile->is_delegate)
            profile->this_offset = call->receiver_offset();

        return true;
    }

    void exec_osr_detach(exec_call_profile_t * profile)
    {
        if (profile->command != nullptr)
        {
            if (__osr_call_t * call = dynamic_cast<__osr_call_t *>(profile->command))
                call->profile = nullptr;
        }
    }

    bool exec_osr_qualifies(const exec_call_profile_t & p)
    {
        // The same method for at least 95% of the calls, and almost all of the calls that were counted.
        return p.target != nullptr && (p.type != nullptr || p.delegate_method != nullptr)
            && p.hits >= (uint32_t)exec_osr_threshold() * 9 / 10 && p.misses * 20 <= p.hits;
    }

    // The check of the object of a call that was replaced by a body: of the type that the profile saw, the body goes on; of another one
    // (or null), the commands go to the call.
    class __osr_guard_command_t : public __command_base_t<__OsrGuardCmd>
    {
        typedef __osr_guard_command_t   __self_t;

    public:
        __osr_guard_command_t(rt_type_t * type, msize_t this_offset, int step) : __type(type), __this_offset(this_offset), __step(step) { }

        __BeginExecute(ctx)

            rt_ref_t obj = *(rt_ref_t *)(ctx.stack.top() - __This->__this_offset);

            if (obj == nullptr || __RtTypeOf(obj) != __This->__type)
                ctx.current += __This->__step;

        __EndExecute()

    private:
        rt_type_t * __type;
        msize_t     __this_offset;
        int         __step;
    };

    // The check of the delegate of a call of Invoke: it has the method that the profile saw.
    class __osr_delegate_guard_command_t : public __command_base_t<__OsrGuardCmd>
    {
        typedef __osr_delegate_guard_command_t   __self_t;

    public:
        __osr_delegate_guard_command_t(rt_method_base_t * method, msize_t this_offset, int step)
            : __method(method), __this_offset(this_offset), __step(step) { }

        __BeginExecute(ctx)

            rt_object_t * self = *(rt_object_t **)(ctx.stack.top() - __This->__this_offset);

            if (self == nullptr
                    || al::incorp_p(__Field(rt_method_base_t *, self, __delegate_method_field_offset)) != __This->__method)
                ctx.current += __This->__step;

        __EndExecute()

    private:
        rt_method_base_t *  __method;
        msize_t             __this_offset;
        int                 __step;
    };

    command_t * new_osr_guard_command(exec_call_profile_t * profile, int step)
    {
        if (profile->is_delegate)
        {
            return __new_command<__osr_delegate_guard_command_t>(
                (memory_t *)&__command_heap, profile->delegate_method, profile->this_offset, step
            );
        }

        return __new_command<__osr_guard_command_t>((memory_t *)&__command_heap, profile->type, profile->this_offset, step);
    }

    bool exec_osr_is_delegate_invoke(command_creating_context_t & callee_ctx, const std::vector<const xil_base_t *> & body)
    {
        if (body.size() != 2)
            return false;

        const xil_base_t * call = body[0], * ret = body[1];

        return command_of(call) == xil_command_t::call
            && ((const call_xil_t *)call)->call_type() == xil_call_type_t::command
            && ((const call_xil_t *)call)->command == xil_call_command_t::delegate_invoke
            && command_of(ret) == xil_command_t::smp && ((const smp_xil_t *)ret)->smp() == xil_smp_t::ret
            && callee_ctx.ret_unit_size() <= 1 && !is_custom_struct(callee_ctx.env, callee_ctx.method->get_return_type(callee_ctx.env));
    }

    #else

    bool exec_osr_enabled() { return false; }
    int  exec_osr_threshold() { return 0; }
    bool exec_osr_is_candidate(command_creating_context_t &, const call_xil_t &) { return false; }
    bool exec_osr_attach(command_t *, exec_call_profile_t *) { return false; }
    bool exec_osr_qualifies(const exec_call_profile_t &) { return false; }
    void exec_osr_detach(exec_call_profile_t *) { }
    command_t * new_osr_guard_command(exec_call_profile_t *, int) { X_UNEXPECTED(); }
    bool exec_osr_is_delegate_invoke(command_creating_context_t &, const std::vector<const xil_base_t *> &) { return false; }

    #endif  // EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL

    // The statistics of the calls (APRIL_CALL_STATS=1): for the choice of what the interpreter inlines.
    namespace
    {
        struct __call_stat_t
        {
            string_t    name;
            uint32_t    xils = 0;
            uint8_t     reason = 0;         // 0: a call could be inlined, 1: blocks, 2: switch, 3: params, 4: return, 5: size
            uint64_t    calls[6] = { 0, 0, 0, 0, 0, 0 };     // static, instance, virtual, interface, delegate, accessor
        };

        std::mutex                      __call_stats_lock;
        std::vector<__call_stat_t *>    __call_stats;

        void __call_stats_report()
        {
            static const wchar_t * names[] = { L"static", L"instance", L"virtual", L"interface", L"delegate", L"accessor" };
            uint64_t kinds[6] = { 0, 0, 0, 0, 0, 0 };

            // The direct calls (static and instance) by the number of xils of the method that is called, for the methods that could be
            // inlined (no blocks, no switch, the params and the result of one unit each).
            const uint32_t limits[] = { 2, 4, 6, 8, 12, 16, 24, 32, 64, 1000000 };
            uint64_t by_size[10] = { 0 };
            uint64_t by_reason[6] = { 0 };

            for (__call_stat_t * s : __call_stats)
            {
                for (int k = 0; k < 6; k++)
                    kinds[k] += s->calls[k];

                uint64_t direct = s->calls[0] + s->calls[1];
                by_reason[s->reason == 5? 0 : s->reason] += direct;

                if (s->reason == 0 || s->reason == 5)
                {
                    for (int b = 0; b < 10; b++)
                    {
                        if (s->xils <= limits[b])
                        {
                            by_size[b] += direct;
                            break;
                        }
                    }
                }
            }

            uint64_t all = kinds[0] + kinds[1] + kinds[2] + kinds[3] + kinds[4] + kinds[5];
            std::wostream & out = std::wcerr;
            out << L"\n== call stats\nsuperinstructions that are a sequence of commands: " << __sequences_made.load() << L"\ncalls: " << all << L"  (";
            for (int k = 0; k < 6; k++)
                out << names[k] << L" " << kinds[k] << (k < 5? L", " : L"");
            out << L")\n";
            out << L"direct calls that cannot be inlined, by the reason: blocks " << by_reason[1] << L", switch " << by_reason[2]
                << L", params " << by_reason[3] << L", return " << by_reason[4] << L"\n";
            out << L"direct calls of the other methods, by the number of xils of the method:\n";

            uint64_t sum = 0;
            for (int b = 0; b < 10; b++)
            {
                sum += by_size[b];
                if (limits[b] < 1000000)
                    out << L"  <= " << limits[b] << L"\t" << by_size[b] << L"\t(sum " << sum << L")\n";
                else
                    out << L"  more\t" << by_size[b] << L"\n";
            }

            // The methods that are called most (direct calls).
            std::vector<__call_stat_t *> sorted(__call_stats.begin(), __call_stats.end());
            std::sort(sorted.begin(), sorted.end(), [](__call_stat_t * a, __call_stat_t * b) {
                return a->calls[0] + a->calls[1] > b->calls[0] + b->calls[1];
            });

            out << L"the methods that are called most (direct): calls, xils, reason, name\n";
            for (size_t k = 0; k < sorted.size() && k < 12 && sorted[k]->calls[0] + sorted[k]->calls[1] > 0; k++)
            {
                out << L"  " << (sorted[k]->calls[0] + sorted[k]->calls[1]) << L"\t" << sorted[k]->xils << L"\t" << (int)sorted[k]->reason
                    << L"\t" << sorted[k]->name << L"\n";
            }

            out.flush();
        }
    }

    void exec_call_count(exec_method_t * method, int kind)
    {
        __call_stat_t * entry = (__call_stat_t *)method->stat_entry;
        if (entry != nullptr)
            entry->calls[kind]++;
    }

    void exec_call_stats_register(command_creating_context_t & ctx, exec_method_t * method, size_t xils, bool has_blocks, bool has_switch)
    {
        uint8_t reason = 0;

        if (has_blocks)
            reason = 1;
        else if (has_switch)
            reason = 2;
        else if (ctx.params_layout.unit_size() > 6)
            reason = 3;
        else if (is_custom_struct(ctx.env, ctx.method->get_return_type(ctx.env)) || ctx.ret_unit_size() > 1)
            reason = 4;
        else if (xils > 16)
            reason = 5;

        __call_stat_t * entry = new __call_stat_t();
        entry->name = method->get_name();
        entry->xils = (uint32_t)xils;
        entry->reason = reason;

        std::lock_guard<std::mutex> guard(__call_stats_lock);

        static bool registered = false;
        if (!registered)
        {
            registered = true;
            std::atexit(__call_stats_report);
        }

        __call_stats.push_back(entry);
        method->stat_entry = entry;
    }

    void analyze_trivial_method(command_creating_context_t & ctx, exec_method_t * method, bool is_instance,
                                const xil_base_t * const * xils, size_t count)
    {
        static const bool enabled = []() {
            const char * value = std::getenv("APRIL_INLINE_ACCESSORS");
            return value == nullptr || std::string(value) != "0";
        }();

        if (!enabled || !is_instance || (count != 1 && count != 2 && count != 3 && count != 4))
            return;

        // The methods that are being analyzed (the one that is called is analyzed first: a method that calls itself is not empty).
        static thread_local std::vector<exec_method_t *> analyzing;
        struct __scope_t
        {
            __scope_t(exec_method_t * m) { analyzing.push_back(m); }
            ~__scope_t() { analyzing.pop_back(); }
        } scope(method);

        auto is_ret = [](const xil_base_t * xil) {
            return command_of(xil) == xil_command_t::smp && ((const smp_xil_t *)xil)->smp() == xil_smp_t::ret;
        };

        // push argument <identity>: the type of the xil, the argument is not the bottom of the calling.
        auto argument = [](const xil_base_t * xil, int identity, xil_type_t * type) -> bool {
            if (command_of(xil) != xil_command_t::push)
                return false;

            const push_xil_t * push = (const push_xil_t *)xil;
            if (push->stype() != xil_storage_type_t::argument || push->get_identity() != identity)
                return false;

            *type = push->dtype();
            return true;
        };

        auto is_field_type = [](xil_type_t type) {
            switch (type)
            {
                case xil_type_t::int8: case xil_type_t::uint8: case xil_type_t::int16: case xil_type_t::uint16:
                case xil_type_t::int32: case xil_type_t::uint32: case xil_type_t::int64: case xil_type_t::uint64:
                case xil_type_t::float_: case xil_type_t::double_: case xil_type_t::char_: case xil_type_t::bool_:
                case xil_type_t::object: case xil_type_t::string:
                    return true;

                default:
                    return false;
            }
        };

        xil_type_t type = xil_type_t::empty, field_type1, field_type2;
        uint8_t kind = 0;
        int32_t offset = 0;

        if (count == 2 && is_ret(xils[1]) && command_of(xils[0]) == xil_command_t::call
                && ((const call_xil_t *)xils[0])->call_type() == xil_call_type_t::command
                && ((const call_xil_t *)xils[0])->command == xil_call_command_t::delegate_invoke)
        {
            // Invoke of a delegate: the result is nothing or one unit (a struct result has an address under the args: not here).
            if (ctx.params_layout.unit_size() >= 1 && ctx.ret_unit_size() <= 1
                    && !is_custom_struct(ctx.env, ctx.method->get_return_type(ctx.env)))
                kind = 4;
        }
        else if (count == 1 && is_ret(xils[0]))
        {
            // Nothing but the return: the constructor of Object.
            if (ctx.params_layout.unit_size() == 1 && ctx.ret_unit_size() == 0)
                kind = 3;
        }
        else if (count == 3 && is_ret(xils[2]) && argument(xils[0], 0, &type) && type == xil_type_t::object
                && command_of(xils[1]) == xil_command_t::call
                && ((const call_xil_t *)xils[1])->call_type() == xil_call_type_t::instance)
        {
            // push this, call a method, ret: the constructor that only calls the one of the base. It does nothing when that does nothing.
            if (ctx.params_layout.unit_size() == 1 && ctx.ret_unit_size() == 0)
            {
                exec_method_t * target = __instance_call_target(ctx, *(const call_xil_t *)xils[1]);

                if (target != nullptr && std::find(analyzing.begin(), analyzing.end(), target) == analyzing.end())
                {
                    target->get_commands();     // It is analyzed when its commands are made.

                    if (target->trivial_kind.load(std::memory_order_acquire) == 3)
                        kind = 3;
                }
            }
        }
        else if (count == 3 && is_ret(xils[2]) && argument(xils[0], 0, &type) && type == xil_type_t::object
                && command_of(xils[1]) == xil_command_t::push
                && ((const push_xil_t *)xils[1])->stype() == xil_storage_type_t::field)
        {
            // The getter: this has the only argument, the value is returned.
            const push_xil_t & field = *(const push_xil_t *)xils[1];
            __analyze_xil_types(ctx, field, &field_type1, &field_type2, false);

            if (field_type1 == field_type2 && field_type1 == field.dtype() && is_field_type(field_type1)
                    && ctx.params_layout.unit_size() == 1 && ctx.ret_unit_size() == 1
                    && !is_custom_struct(ctx.env, ctx.method->get_return_type(ctx.env)))
            {
                kind = 1;
                type = field_type1;
                offset = (int32_t)__field_offset(ctx, field.get_ref());
            }
        }
        else if (count == 4 && is_ret(xils[3]) && argument(xils[0], 1, &type) && is_field_type(type)
                && argument(xils[1], 0, &field_type1) && field_type1 == xil_type_t::object
                && command_of(xils[2]) == xil_command_t::pop
                && ((const pop_xil_t *)xils[2])->stype() == xil_storage_type_t::field)
        {
            // The setter: the value is the second argument, nothing is returned.
            const pop_xil_t & field = *(const pop_xil_t *)xils[2];
            __analyze_xil_types(ctx, field, &field_type1, &field_type2, false);

            if (field_type1 == field_type2 && field_type1 == field.dtype() && field_type1 == type
                    && ctx.params_layout.unit_size() == 2 && ctx.ret_unit_size() == 0)
            {
                kind = 2;
                offset = (int32_t)__field_offset(ctx, field.get_ref());
            }
        }

        if (kind == 0)
            return;

        // The type of the argument (a generic one) is the type of the field.
        if (kind == 2)
        {
            xil_type_t a1, a2;
            __analyze_xil_types(ctx, *(const push_xil_t *)xils[0], &a1, &a2, false);
            if (a1 != type || a2 != type)
                return;
        }

        method->trivial_type = (uint8_t)type;
        method->trivial_offset = offset;
        method->trivial_kind.store(kind, std::memory_order_release);
    }

    // The command at the beginning of a method that is not fast yet: it counts the calls, and when they are many, the method gets its
    // superinstructions (exec_method_t::tier_up) and runs from the beginning of them.
    class __tier_command_t : public __command_base_t<__TierCmd>
    {
        typedef __tier_command_t __self_t;

    public:
        __tier_command_t(exec_method_t * method, int threshold) : __method(method), __count(threshold) { }

        __BeginExecute(ctx)

            if (--__This->__count == 0)
            {
                command_t ** fast = __This->__method->tier_up();
                if (fast != nullptr)
                    ctx.current = fast;
            }

        __EndExecute()

    private:
        exec_method_t * __method;
        int             __count;
    };

    command_t * new_tier_command(exec_method_t * method, int threshold)
    {
        return __new_command<__tier_command_t>((memory_t *)&__command_heap, method, threshold);
    }

    #else

    command_t * new_super_command(command_creating_context_t & ctx, const fusion_plan_t & plan,
                                  const xil_base_t * const * xils, const fusion_group_t & group)
    {
        X_UNEXPECTED();
    }

    command_t * new_tier_command(exec_method_t * method, int threshold)
    {
        X_UNEXPECTED();
    }

    void analyze_trivial_method(command_creating_context_t & ctx, exec_method_t * method, bool is_instance,
                                const xil_base_t * const * xils, size_t count)
    {
        // Empty.
    }

    exec_method_t * inline_call_target(command_creating_context_t &, const call_xil_t &, bool *) { return nullptr; }

    bool inline_check_body(command_creating_context_t &, const std::vector<const xil_base_t *> &, size_t) { return false; }

    const xil_base_t * inline_rewrite_xil(const inline_site_t &, const xil_base_t * xil, std::deque<std::vector<byte_t>> &, size_t, size_t) { return xil; }

    command_t * new_inline_enter_command(const inline_site_t &) { X_UNEXPECTED(); }

    #endif  // EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL

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

    ////////// ////////// ////////// ////////// //////////
    // Method.Invoke: the stub that calls a method with the arguments of an array of objects.
    //
    // The stub is a dynamic method of two params (the object, the array of the arguments) that
    // has three commands: the call takes the arguments out of the objects, pushes them like the
    // code of a call does and calls the method (it is a call of the executor, nothing is nested:
    // an exception that the method throws goes the way of any other), the finish makes an object
    // of the value that the method returned, and the ret.

    // A value that is passed in an object: a reference is the object itself, the others (a number,
    // an enum, a char, a bool, a string) are boxed. A struct of the user is not supported.
    static bool __invoke_is_boxed(analyzer_env_t & env, rt_type_t * type)
    {
        return type->get_vtype(env) == vtype_t::string_ || !is_ref_type(type->get_ttype(env));
    }

    // The size of a value that is boxed.
    static size_t __invoke_value_size(analyzer_env_t & env, rt_type_t * type)
    {
        return type->get_vtype(env) == vtype_t::string_? sizeof(void *)
                                                        : (size_t)type->get_variable_size(env);
    }

    // Returns whether an object is a value of a type.
    static bool __invoke_is_instance_of(analyzer_env_t & env, void * obj, rt_type_t * type)
    {
        rt_type_t * obj_type = mm::get_object_type(obj);

        for (rt_type_t * t = obj_type; t != nullptr; t = t->get_base_type(env))
        {
            if (t == type)
                return true;
        }

        rt_vtable_t * vtbl = get_vtable(obj_type);
        return vtbl != nullptr && vtbl->search_vtable_interface_variant(env, type) != nullptr;
    }

    // Returns whether the type is one of the objects of reflection (Type, Method, ...): they are
    // handles, they have no header with the type of the object.
    static bool __invoke_is_handle_type(analyzer_env_t & env, rt_type_t * type)
    {
        rt_sid_t sid = type->get_full_name(env);
        string_t name = sid.value != nullptr? (string_t)*sid.value : string_t();

        return name.find(_T("Reflection.")) == 0 || name.find(_T("System.Reflection.")) == 0;
    }

    // Pushes the argument of a call, returns false when the object is not a value of the type.
    static bool __push_invoke_argument(command_execute_context_t & ctx, rt_type_t * type, void * item)
    {
        analyzer_env_t & env = ctx.env;

        if (type == nullptr)
            return false;

        // A struct is passed by value: its bytes are in the stack, the units that its size needs
        // (it is a copy, the box does not change).
        if (is_custom_struct(env, type))
        {
            if (item == nullptr || mm::get_object_type(item) != type)
                return false;

            size_t size = (size_t)type->get_size(env);
            rt_stack_unit_t * units = ctx.stack.alloc(size);

            memset((void *)units, 0, _alignf(size, sizeof(rt_stack_unit_t)));
            memcpy((void *)units, item, size);

            return true;
        }

        if (!__invoke_is_boxed(env, type))
        {
            if (item != nullptr && !__invoke_is_handle_type(env, type)
                    && !__invoke_is_instance_of(env, item, type))
                return false;

            ctx.stack.push(item);
            return true;
        }

        size_t size = __invoke_value_size(env, type);
        rt_stack_unit_t * units = ctx.stack.alloc(size);
        memset((void *)units, 0, _alignf(size, sizeof(rt_stack_unit_t)));

        // A string can be null, the others are the box of a value of exactly that type.
        if (item == nullptr)
            return type->get_vtype(env) == vtype_t::string_;

        if (mm::get_object_type(item) != type)
            return false;

        memcpy((void *)units, item, size);
        return true;
    }

    #define __InvokeCallCmd     __Cmd(call, invoke)
    #define __InvokeFinishCmd   __Cmd(call, invoke_finish)

    __DefineCmdValue_(__InvokeCallCmd)
    __DefineCmdValue_(__InvokeFinishCmd)

    // Takes the arguments out of the array, and calls the method.
    class __invoke_call_command_t : public __command_base_t<__InvokeCallCmd>
    {
        typedef __invoke_call_command_t     __self_t;

    public:
        __invoke_call_command_t(rt_method_base_t * method, rt_reflect_member_t * member,
                                msize_t target_offset, msize_t args_offset)
            : __method(method), __member(member)
            , __target_offset(target_offset), __args_offset(args_offset) { }

        __BeginExecute(ctx)

            analyzer_env_t & env = ctx.env;

            void * target = __Argument(void *, __This->__target_offset);
            void * args   = __Argument(void *, __This->__args_offset);

            rt_method_base_t * method = __This->__method;
            rt_reflect_member_t * member = __This->__member;
            exec_method_t * exec_method = nullptr;

            // A struct is returned through an address, that is pushed before the arguments: the
            // object that has it (it is the result), and once more under it, that stays when the
            // method returns.
            rt_type_t * return_type = method->get_return_type(env);
            if (is_custom_struct(env, return_type))
            {
                __pre_new(ctx, return_type);
                void * result = (void *)ctx.heap->new_obj(return_type);

                ctx.stack.push(result);
                ctx.stack.push(result);
            }

            if (__is_static_method(method))
            {
                __pre_static_call(ctx, method->get_host_type());
            }
            else
            {
                if (target == nullptr)
                    __RaiseException(ctx, null_reference);

                rt_type_t * host = method->get_host_type();
                bool is_number = !is_custom_struct(env, host) && __invoke_is_boxed(env, host);

                // The methods of a number (Int32.ToString) take the value for the this: the
                // target is its box, the value is pushed.
                if (is_number)
                {
                    if (mm::get_object_type(target) != host)
                        __RaiseInvalidCastException(ctx);

                    size_t size = __invoke_value_size(env, host);
                    rt_stack_unit_t * units = ctx.stack.alloc(size);

                    memset((void *)units, 0, _alignf(size, sizeof(rt_stack_unit_t)));
                    memcpy((void *)units, target, size);
                }
                else
                {
                    if (!__invoke_is_instance_of(env, target, host))
                        __RaiseInvalidCastException(ctx);

                    ctx.stack.push(target);
                }

                // The methods of an interface have no code: the function is in the table of the
                // interface of the object.
                if (host->get_ttype(env) == ttype_t::interface_)
                {
                    if (member->offset < 0)
                        __RaiseInvalidCastException(ctx);

                    int64_t offset = member->offset;
                    exec_method = __get_virtual_exec_method(ctx, 1, nullptr,
                        [offset, host, &env](rt_vtable_t * vtbl) {
                            return vtbl->search_vtable_interface_function(env, host, offset);
                        });

                    if (exec_method == nullptr)
                        __RaiseInvalidCastException(ctx);
                }
                else if (member->offset >= 0)
                {
                    // A virtual method is called like a virtual call does: the function of the
                    // virtual table of the object.
                    int64_t offset = member->offset;
                    exec_method = __get_virtual_exec_method(ctx, 1, nullptr,
                        [offset](rt_vtable_t * vtbl) { return vtbl->functions[offset]; });
                }
            }

            for (int64_t k = 0; k < member->param_count; k++)
            {
                void * item = mm::get_array_element<void *>(args, (array_length_t)k);

                if (!__push_invoke_argument(ctx, (rt_type_t *)member->params[k]->type, item))
                    __RaiseInvalidCastException(ctx);
            }

            ctx.push_calling(exec_method != nullptr? exec_method : ctx.env.exec_method_of(method));

        __EndExecute()

        #if EXEC_TRACE

        __ToString(_T("invoke"))

        #endif  // EXEC_TRACE

    private:
        rt_method_base_t *      __method;
        rt_reflect_member_t *   __member;
        msize_t                 __target_offset, __args_offset;
    };

    // Makes an object of the value that the method returned.
    class __invoke_finish_command_t : public __command_base_t<__InvokeFinishCmd>
    {
        typedef __invoke_finish_command_t   __self_t;

    public:
        __invoke_finish_command_t(rt_method_base_t * method) : __method(method) { }

        __BeginExecute(ctx)

            analyzer_env_t & env = ctx.env;
            rt_type_t * type = __This->__method->get_return_type(env);

            // A method of void returns null.
            if (type == nullptr || type->get_vtype(env) == vtype_t::void_)
            {
                ctx.stack.push((void *)nullptr);
                return;
            }

            // A struct: the object that has it is under the result.
            if (is_custom_struct(env, type))
                return;

            // A reference is the object.
            if (!__invoke_is_boxed(env, type))
                return;

            size_t size = __invoke_value_size(env, type);
            msize_t units = (msize_t)(_alignf(size, sizeof(rt_stack_unit_t)) / sizeof(rt_stack_unit_t));

            // The value is on the stack while the object is made (the gc scans the stack).
            void * boxed = nullptr;
            bool is_null_string = type->get_vtype(env) == vtype_t::string_
                                    && *(void **)(ctx.stack.top() - units) == nullptr;

            if (!is_null_string)
            {
                __pre_new(ctx, type);
                boxed = (void *)ctx.heap->new_obj(type);

                memcpy(boxed, (void *)(ctx.stack.top() - units), size);
            }

            ctx.stack.pop(units);
            ctx.stack.push(boxed);

        __EndExecute()

        #if EXEC_TRACE

        __ToString(_T("invoke finish"))

        #endif  // EXEC_TRACE

    private:
        rt_method_base_t *  __method;
    };

    // Creates the commands of the stub.
    static command_t * __new_invoke_command(command_creating_context_t & ctx,
                                            rt_invoke_command_t * rt_cmd)
    {
        switch (rt_cmd->kind)
        {
            case rt_invoke_kind_t::call:
                return __new_command<__invoke_call_command_t>(&__command_heap, rt_cmd->method,
                    rt_cmd->member, __offset_of_argument(ctx, 1) + __stack_stub_size,
                    __offset_of_argument(ctx, 2) + __stack_stub_size);

            case rt_invoke_kind_t::finish:
                return __new_command<__invoke_finish_command_t>(&__command_heap, rt_cmd->method);

            default:
                X_UNEXPECTED();
        }
    }

    // The function Reflection_Invoke(method, target, args): the stub of the method takes the place
    // of the arguments (the method is the this that the stub does not use), and is called.
    #define __ReflectionInvokeCmd   __Cmd(call, reflection_invoke)
    __DefineCmdValue_(__ReflectionInvokeCmd)

    class __reflection_invoke_command_t : public __command_base_t<__ReflectionInvokeCmd>
    {
        typedef __reflection_invoke_command_t   __self_t;

    public:
        __BeginExecute(ctx)

            rt_stack_unit_t * args = ctx.stack.top() - 3;
            rt_reflect_member_t * member = *(rt_reflect_member_t **)args;

            exec_method_t * stub = __stub_of(ctx, member);
            if (stub == nullptr)
                __RaiseInvalidCastException(ctx);

            *(void **)args = nullptr;
            ctx.push_calling(stub);

        __EndExecute()

        #if EXEC_TRACE

        __ToString(_T("call reflection invoke"))

        #endif  // EXEC_TRACE

    private:

        // Returns the stub of a method (it is made the first time), null when the method is not
        // known to the interpreter.
        static exec_method_t * __stub_of(command_execute_context_t & ctx,
                                                        rt_reflect_member_t * member)
        {
            static std::map<rt_reflect_member_t *, exec_method_t *> stubs;

            auto it = stubs.find(member);
            if (it != stubs.end())
                return it->second;

            rt_method_base_t * method = (rt_method_base_t *)member->static_data;
            if (method == nullptr)
                return nullptr;

            analyzer_env_t & env = ctx.env;
            rt_type_t * object_type = __get_core_type(ctx, _T(""), CoreType_Object);

            rt_dynamic_method_t * stub = env.rpool.new_dynamic_method_array(1);
            new ((void *)stub) rt_dynamic_method_t();

            stub->host_type   = object_type;
            stub->return_type = object_type;
            stub->name        = object_type->get_assembly()->to_sid(_T("__invoke"));

            rt_dynamic_param_t * params = (rt_dynamic_param_t *)memory_t::alloc(
                env.memory, sizeof(rt_dynamic_param_t) * 2
            );

            for (int k = 0; k < 2; k++)
            {
                new ((void *)&params[k]) rt_dynamic_param_t();
                params[k].type = object_type;
            }

            stub->params      = params;
            stub->param_count = 2;

            rt_dynamic_method_body_builder_t builder;
            builder.append<rt_invoke_command_t>(rt_invoke_kind_t::call, method, member);
            builder.append<rt_invoke_command_t>(rt_invoke_kind_t::finish, method, member);
            builder.append<rt_ret_command_t>();

            stub->body = builder.to_method_body(env.memory);

            exec_method_t * exec_method = ctx.env.exec_method_of((rt_method_base_t *)stub);
            stubs[member] = exec_method;

            return exec_method;
        }
    };

    static command_t * __new_reflection_invoke_command(__context_t & ctx)
    {
        __ReturnInstance(__reflection_invoke_command_t);
    }

    // Creates a push command.
    command_t * __new_push_command(command_creating_context_t & ctx, rt_push_command_base_t * rt_cmd)
    {
        switch (rt_cmd->stype)
        {
            case xil_storage_type_t::argument: {
                rt_push_argument_command_t * cmd = (rt_push_argument_command_t *)rt_cmd;

                // The address that a struct is returned through.
                if (cmd->identity == XIL_CALLING_BOTTOM_IDENTITY)
                {
                    __new_push_command_args_t bottom_args {
                        .stype = xil_storage_type_t::argument,
                        .xt1   = xil_type_t::ptr,
                        .xt2   = xil_type_t::ptr,
                        .argument.offset = __offset_of_argument(ctx, cmd->identity),
                    };

                    return __new_push_command(ctx, bottom_args);
                }

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

            case xil_storage_type_t::field_addr: {
                rt_push_field_addr_command_t * cmd = (rt_push_field_addr_command_t *)rt_cmd;

                __new_push_command_args_t args {
                    .stype = xil_storage_type_t::field_addr,
                    .xt1   = xil_type_t::ptr,
                    .xt2   = xil_type_t::ptr,
                    .field.offset = cmd->field->offset,
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

            case xil_command_t::call:
                return __new_invoke_command(ctx, (rt_invoke_command_t *)rt_cmd);

            case xil_command_t::copy: {
                rt_type_t * type = ((rt_copy_command_t *)rt_cmd)->type;
                return __get_object_copy_command(type->get_size(ctx.env), xil_copy_kind_t::default_);
            }

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

    //-------- ---------- ---------- ---------- ----------

    #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL || EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_INLINE

    // A command that counts the executions of another one (APRIL_EXEC_PROFILE).

    class __profile_command_t : public __command_base_t<__ProfileCmd>
    {
        typedef __profile_command_t __self_t;

    public:
        __profile_command_t(command_t * target, uint64_t * counter) : __target(target), __counter(counter) { }

        __BeginExecute(ctx)

            ++*__This->__counter;
            __execute_one(ctx, __This->__target);

        __EndExecute()

    private:
        command_t * __target;
        uint64_t *  __counter;
    };

    command_t * new_profile_command(command_t * target, uint64_t * counter)
    {
        return __new_command<__profile_command_t>(&__command_heap, target, counter);
    }

    #else

    command_t * new_profile_command(command_t * target, uint64_t * counter)
    {
        return target;      // The profile is made for the virtual model of the commands.
    }

    #endif  // EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL

    //-------- ---------- ---------- ---------- ----------

    // Pushes a reference: the argument of the method that a new thread runs.
    #define __PushRefCmd    __Cmd(push, ref_value)
    __DefineCmdValue_(__PushRefCmd)

    class __push_ref_command_t : public __command_base_t<__PushRefCmd>
    {
        typedef __push_ref_command_t    __self_t;

    public:
        __push_ref_command_t(void * ref) : __ref(ref) { }

        __BeginExecute(ctx)

            ctx.stack.push(__This->__ref);

        __EndExecute()

        #if EXEC_TRACE

        __ToString(_T("push ref"))

        #endif  // EXEC_TRACE

    private:
        void *  __ref;
    };

    command_t * new_push_ref_command(void * ref)
    {
        return __new_command<__push_ref_command_t>(&__command_heap, ref);
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


    // The pointers: the content of the address that is at the top of the stack.
    #define __SC_PTR_CONTENT(_xt)                                                       \
        __SwitchCommands_CaseSt(push, ptr_content, _xt, _xt)                            \
        __SwitchCommands_CaseSt(pop,  ptr_content, _xt, _xt)

    #define __SC_PTR_CONTENTS                                                           \
        __SC_PTR_CONTENT(int8)   __SC_PTR_CONTENT(uint8)  __SC_PTR_CONTENT(int16)       \
        __SC_PTR_CONTENT(uint16) __SC_PTR_CONTENT(int32)  __SC_PTR_CONTENT(uint32)      \
        __SC_PTR_CONTENT(int64)  __SC_PTR_CONTENT(uint64) __SC_PTR_CONTENT(float_)      \
        __SC_PTR_CONTENT(double_) __SC_PTR_CONTENT(char_) __SC_PTR_CONTENT(bool_)       \
        __SC_PTR_CONTENT(ptr)

    #define __SC_CONVERT_PTR(_a, _b)                                                    \
        __Case( __push_command_t<__StCmd(push, convert, _a, _b),                        \
            xil_storage_type_t::convert, xil_type_t::_a, xil_type_t::_b> )

    #define __SC_BOX(_size, _bt)                                                        \
        __CaseC( __push_box_command_t<__PushBoxCmd(_size, _bt), _size, xil_box_type_t::_bt> )
    #define __SC_UNBOX(_size, _bt)                                                      \
        __CaseC( __push_unbox_command_t<__PushUnboxCmd(_size, _bt), _size, xil_box_type_t::_bt> )
    #define __SC_BOXES(_size)       __SC_BOX(_size, pop) __SC_BOX(_size, pick)
    #define __SC_UNBOXES(_size)     __SC_UNBOX(_size, pop) __SC_UNBOX(_size, pick)

    #define __SC_ARRAY_ADDR(_d)                                                         \
        __Case( __push_array_element_address_command_t<__StArrayElementAddrCmd(push, _d), _d> )

    #define __SC_INIT(_type, _size)                                                     \
        __Case( __init_command_t<__InitCmd(_type, _size), xil_init_type_t::_type, _size> )

    #define __SwitchCommands_Others                                                     \
        __SwitchCommands_CaseAddress(field_addr)                                        \
        __SC_ARRAY_ADDR(0) __SC_ARRAY_ADDR(1) __SC_ARRAY_ADDR(2) __SC_ARRAY_ADDR(3)     \
        __SC_ARRAY_ADDR(4) __SC_ARRAY_ADDR(5) __SC_ARRAY_ADDR(6) __SC_ARRAY_ADDR(7)     \
        __SC_ARRAY_ADDR(8)                                                              \
        __SwitchCommands_CaseSts(push, argument_content)                                \
        __SwitchCommands_CaseSts(pop, argument_addr)                                    \
        __SC_PTR_CONTENTS                                                               \
        __SC_CONVERT_PTR(ptr, int64) __SC_CONVERT_PTR(int64, ptr)                       \
        __SC_CONVERT_PTR(ptr, uint64) __SC_CONVERT_PTR(uint64, ptr)                     \
        __SC_BOXES(0) __SC_BOXES(1) __SC_BOXES(2) __SC_BOXES(4) __SC_BOXES(8)           \
        __SC_BOXES(__CustomStructSize)                                                  \
        __SC_UNBOXES(0) __SC_UNBOXES(1) __SC_UNBOXES(2) __SC_UNBOXES(4) __SC_UNBOXES(8) \
        __SC_UNBOXES(__CustomStructSize)                                                \
        __SC_INIT(array_begin, 0) __SC_INIT(array_end, 0)                               \
        __SC_INIT(array_element, 0) __SC_INIT(array_element, 1) __SC_INIT(array_element, 2) \
        __SC_INIT(array_element, 4) __SC_INIT(array_element, 8)

    // The superinstructions: the commands of each form, operator and types that the factory can make.
    #define __SC_B(_op, _a, _b, _t)     __Case( __sc_binary_t<operator_t::_op, xil_type_t::_a, xil_type_t::_b, _t> )
    #define __SC_ARITH(_op, _a, _b)     __SC_B(_op, _a, _b, 0) __SC_B(_op, _a, _b, 1) __SC_B(_op, _a, _b, 3) __SC_B(_op, _a, _b, 4)
    #define __SC_CMP(_op, _a, _b)       __SC_B(_op, _a, _b, 0) __SC_B(_op, _a, _b, 2) __SC_B(_op, _a, _b, 4)

    #define __SC_ALL_PAIRS(_m, _op)                                                     \
        _m(_op, int32, int32) _m(_op, uint32, uint32) _m(_op, int64, int64)             \
        _m(_op, uint64, uint64) _m(_op, float_, float_) _m(_op, double_, double_)       \
        _m(_op, int64, int32)

    #define __SC_INT_PAIRS(_m, _op)                                                     \
        _m(_op, int32, int32) _m(_op, uint32, uint32) _m(_op, int64, int64)             \
        _m(_op, uint64, uint64) _m(_op, int64, int32)

    #define __SC_X2(_m, _a)                                                             \
        _m(_a, int32) _m(_a, uint32) _m(_a, int64) _m(_a, uint64) _m(_a, float_) _m(_a, double_)

    #define __SC_PUSH2(_a, _b)      __Case( __sc_push2_t<xil_type_t::_a, xil_type_t::_b> )
    #define __SC_CONVERT(_a, _b)    __Case( __sc_convert_t<xil_type_t::_a, xil_type_t::_b> )
    #define __SC_X1(_m)                                                                 \
        __SC_X2(_m, int32) __SC_X2(_m, uint32) __SC_X2(_m, int64) __SC_X2(_m, uint64)   \
        __SC_X2(_m, float_) __SC_X2(_m, double_)

    #define __SC_COPY(_a)           __Case( __sc_copy_t<xil_type_t::_a> )
    #define __SC_STORE_FIELD(_a)    __Case( __sc_store_field_t<xil_type_t::_a, false> ) __Case( __sc_store_field_t<xil_type_t::_a, true> )
    #define __SC_THIS_FIELD(_a)     __Case( __sc_this_field_t<xil_type_t::_a, false> ) __Case( __sc_this_field_t<xil_type_t::_a, true> )
    #define __SC_X0(_m)             _m(int32) _m(uint32) _m(int64) _m(uint64) _m(float_) _m(double_)
    #define __SC_PUSH_RET(_a)       __Case( __sc_push_ret_t<xil_type_t::_a> )
    #define __SC_ARRAY_LOAD(_a)     __Case( __sc_array_load_t<xil_type_t::_a> )
    #define __SC_ARRAY_STORE(_a)    __Case( __sc_array_store_t<xil_type_t::_a, 0> ) __Case( __sc_array_store_t<xil_type_t::_a, 1> )

    #define __SwitchCommands_Supers                                                     \
        __SC_ALL_PAIRS(__SC_ARITH, add)                                                 \
        __SC_ALL_PAIRS(__SC_ARITH, sub)                                                 \
        __SC_ALL_PAIRS(__SC_ARITH, mul)                                                 \
        __SC_ALL_PAIRS(__SC_ARITH, div)                                                 \
        __SC_INT_PAIRS(__SC_ARITH, mod)                                                 \
        __SC_INT_PAIRS(__SC_ARITH, bit_and)                                             \
        __SC_INT_PAIRS(__SC_ARITH, bit_or)                                              \
        __SC_INT_PAIRS(__SC_ARITH, bit_xor)                                             \
        __SC_INT_PAIRS(__SC_ARITH, left_shift)                                          \
        __SC_INT_PAIRS(__SC_ARITH, right_shift)                                         \
        __SC_ALL_PAIRS(__SC_CMP, greater)                                               \
        __SC_ALL_PAIRS(__SC_CMP, greater_equal)                                         \
        __SC_ALL_PAIRS(__SC_CMP, less)                                                  \
        __SC_ALL_PAIRS(__SC_CMP, less_equal)                                            \
        __SC_ALL_PAIRS(__SC_CMP, equal)                                                 \
        __SC_ALL_PAIRS(__SC_CMP, not_equal)                                             \
        __SC_X1(__SC_PUSH2)                                                             \
        __SC_X1(__SC_CONVERT)                                                           \
        __SC_X0(__SC_COPY)                                                              \
        __SC_X0(__SC_STORE_FIELD)                                                       \
        __SC_X0(__SC_THIS_FIELD)                                                        \
        __SC_ATYPES(__SC_PUSH_RET)                                                      \
        __SC_ATYPES(__SC_ARRAY_LOAD)                                                    \
        __SC_ATYPES(__SC_ARRAY_STORE)                                                   \
        __Case( __inline_enter_command_t )                                              \
        __CaseC( __sc_sequence_t )                                                       \
        __CaseC( __tier_command_t )                                                      \
        __CaseC( __profile_command_t )

    // A command that is rare or big: the loop of the INLINE model calls it (noinline), it does not grow the loop.
    // Everywhere else it is a plain case.
    #define __CaseC(_t, _ts...)     __Case(_t, ##_ts)

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
            __SwitchCommands_CaseSts(push, checked_convert)                             \
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
            __CaseC( __internal_call_command_t )                                         \
            __CaseC( __static_call_command_t<__CallCmd_(static_, generic), rt_generic_method_t> ) \
            __CaseC( __static_call_command_t<__CallCmd_(static_, general), rt_method_t> ) \
            __CaseC( __instance_call_command_t<__CallCmd_(instance, generic), rt_generic_method_t> ) \
            __CaseC( __instance_call_command_t<__CallCmd_(instance, general), rt_method_t> ) \
            __CaseC( __instance_call_command_t<__CallCmd_(instance, dynamic), rt_dynamic_method_t> ) \
            __CaseC( __virtual_call_command_t )                                          \
            __CaseC( __interface_call_command_t )                                        \
            __CaseC( __delegate_call_command_t<__CallCmd(delegate_init)> )               \
            __CaseC( __delegate_call_command_t<__CallCmd(delegate_init_with_call_type)> ) \
            __CaseC( __delegate_call_command_t<__CallCmd(delegate_invoke)> )             \
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
            __SwitchCommands_CaseBinaries(al, checked_add)                              \
            __SwitchCommands_CaseBinaries(al, checked_sub)                              \
            __SwitchCommands_CaseBinaries(al, checked_mul)                              \
            __SwitchCommands_CaseUnitaries(al, minus)                                   \
            __SwitchCommands_CaseFloatUnitaries(al, minus)                              \
            __SwitchCommands_CaseUnitaries(al, checked_minus)                           \
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
            __CaseC( __cast_command_t<__CastDefaultCmd, xil_cast_command_t::default_, false> ) \
            __CaseC( __cast_command_t<__CastInterfaceCmd(default_), xil_cast_command_t::default_, true> ) \
            __CaseC( __cast_command_t<__CastAsCmd, xil_cast_command_t::as, false> )      \
            __CaseC( __cast_command_t<__CastInterfaceCmd(as), xil_cast_command_t::as, true> ) \
            __CaseC( __cast_command_t<__CastIsCmd, xil_cast_command_t::is, false> )      \
            __CaseC( __cast_command_t<__CastInterfaceCmd(is), xil_cast_command_t::is, true> ) \
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
            __CaseC( __throw_command_t )                                                 \
            __CaseC( __rethrow_command_t )                                               \
            __CaseC( __end_finally_command_t )                                           \
                                                                                        \
            /* jmp */                                                                   \
            __SwitchCommands_CaseJmp(none)                                              \
            __SwitchCommands_CaseJmp(true_)                                             \
            __SwitchCommands_CaseJmp(false_)                                            \
            __SwitchCommands_CaseJmp(leave)                                             \
            __CaseC( __leave_ret_command_t )                                             \
            __Case( __switch_jmp_command_t )                                            \
                                                                                        \
            /* new */                                                                   \
            __CaseC( __new_command_t<xil_new_type_t::default_> )                         \
            __CaseC( __new_command_t<xil_new_type_t::array> )                            \
            __CaseC( __new_command_t<xil_new_type_t::stack_alloc> )                      \
                                                                                        \
            /* copy */                                                                  \
            __SwitchCommands_CaseCopies(default_)                                       \
            __SwitchCommands_CaseCopies(reverse)                                        \
            __SwitchCommands_CaseCopies(zero)                                           \
            __CaseC( __memory_copy_command_ex_t )                                        \
                                                                                        \
            /* the commands that were not in the switch */                              \
            __SwitchCommands_Others                                                     \
            __Case( __push_object_command_t )                                           \
            __CaseC( __push_thread_static_command_t )                                    \
            __CaseC( __push_params_address_command_t )                                   \
            __CaseC( __push_params_values_command_t )                                    \
            __CaseC( __push_tuple_values_command_t )                                     \
            __Case( __push_ref_command_t )                                              \
            __Case( __pop_struct_element_command_t )                                    \
            __CaseC( __generic_box_command_t )                                           \
            __CaseC( __generic_unbox_command_t )                                         \
            __CaseC( __invoke_call_command_t )                                           \
            __CaseC( __invoke_finish_command_t )                                         \
            __CaseC( __reflection_invoke_command_t )                                     \
                                                                                        \
            /* the superinstructions, the command that counts the calls, the profile */    \
            __SwitchCommands_Supers                                                     \
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

    #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL

    static void __execute_one(command_execute_context_t & ctx, command_t * command)
    {
        command->execute(ctx);
    }

    #endif

    #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_INLINE

    static void __check_command_registered(command_value_t value, const char * class_name)
    {
        static const bool enabled = std::getenv("APRIL_CHECK_COMMANDS") != nullptr;
        if (!enabled)
            return;

        static std::map<command_value_t, std::string> registered = []() {
            std::map<command_value_t, std::string> values;

            // Each case inserts its value and falls through to the next one: the switch starts at the first case.
            #define __Case(_t, _ts...)      case _t, ##_ts::cmd_value: values[_t, ##_ts::cmd_value] = typeid(_t, ##_ts).name();

            __BeginSwitchCommands(__EmptyCmd)
            default:
                break;
            __EndSwitchCommands()

            #undef __Case
            return values;
        }();

        static std::mutex lock;
        static std::set<command_value_t> reported;

        std::lock_guard<std::mutex> guard(lock);
        if (value == __EndCmd)
            return;

        // The class of the case of the value must be this class: a value that two classes have runs the wrong one.
        auto it = registered.find(value);
        if ((it == registered.end() || it->second != class_name) && reported.insert(value).second)
            std::wcerr << L"command is not in the switch: " << value << L" " << class_name << std::endl;
    }

    #endif

    #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_INLINE

    // The rare and big commands are not inline in the loop.
    template<typename _command_t>
    #if defined __clang__ || defined __GNUC__
    __attribute__((noinline))
    #endif
    static void __execute_cold(_command_t * command, command_execute_context_t & ctx)
    {
        command->execute(ctx);
    }

    static void __execute_one(command_execute_context_t & ctx, command_t * command)
    {
        #define __Case(_t, _ts...)                                                      \
            case _t, ##_ts::cmd_value:                                                  \
                ((_t, ##_ts *)command)->execute(ctx);                                   \
                return;

        __BeginSwitchCommands(command->cmd_value)

            case __EndCmd:
                return;

            default:
                throw _EC(unexpected, _F(_T("unexpected command %1%"), command->cmd_value));

        __EndSwitchCommands()

        #undef __Case
    }

    #endif

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

        try
        {
    __begin__:

            command_t * command = *commands++;

            __BeginExecuteCommand()

            #define __Case(_t, _ts...)                                                      \
                case _t, ##_ts::cmd_value:                                                  \
                    ((_t, ##_ts *)command)->execute(ctx);                                   \
                    goto __begin__;

            #undef __CaseC
            #define __CaseC(_t, _ts...)                                                     \
                case _t, ##_ts::cmd_value:                                                  \
                    __execute_cold((_t, ##_ts *)command, ctx);                              \
                    goto __begin__;

            __BeginSwitchCommands(command->cmd_value)

                case __EndCmd:
                    return _E(exec_env_code_t::end);

                default:
                    throw _EC(unexpected, _F(_T("unexpected command %1%"), command->cmd_value));

            __EndSwitchCommands()

            #undef __Case
            #undef __CaseC
            #define __CaseC(_t, _ts...)     __Case(_t, ##_ts)

            __EndExecuteCommand()
        }
        catch (__exec_env_error_t & e)
        {
            return e;
        }


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

                std::wcerr << _T("Unhandled exception: ")
                           << string_t(exception_type->get_name(ctx.env)) << std::endl;
                ctx.unhandled = true;

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

    // The values of the superinstructions are above those that the other commands got.
    static_assert(__COUNTER__ < __SuperBase, "the values of the commands reach those of the superinstructions (__SuperBase)");

}   // namespace X_ROOT_NS::modules::exec
