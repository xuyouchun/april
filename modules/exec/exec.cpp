#include <exec.h>
#include "parser.h"
#include "commands.h"
#include "utils.h"
#include "fusion.h"

namespace X_ROOT_NS::modules::exec {

    using namespace core;
    using namespace rt;

    #define __Debug     0

    ////////// ////////// ////////// ////////// //////////

    // Execute error codes.
    X_ENUM_INFO(exec_error_code_t)

        // Assembly not found.
        X_D(assembly_not_found, _T("assembly '%1% not found"))

        // Assembly no entry point.
        X_D(assembly_no_entry_point, _T("assembly '%1%' has no entry point"))

        // Method no body.
        X_D(method_no_body, _T("method '%1% has no body"))

        // Internal function not found.
        X_D(internal_function_not_found, _T("internal funciton '%1%' not found"))

        // Method parse error: unexpected index.
        X_D(method_parse_error__unexpected_index, _T("method parse error: unexpected index"))

        // Switch table index overflow.
        X_D(switch_table_index_overflow, _T("switch table index overflow"))

        // Type not found.
        X_D(type_not_found, _T("type '%1%' not found"))

        // Xil block index out of range.
        X_D(xil_block_index_out_of_range, _T("xil block index out of range"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Execute environment codes.
    X_ENUM_INFO(exec_env_code_t)

        // At the end of commands.
        X_C(end,        _T("end"))

        // Raised by uncaughted exception.
        X_C(terminal,   _T("terminal"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // The action after finally block executed.
    X_ENUM_INFO(block_node_finally_action_t)
    
        // No action.
        X_C(default_,   _T("default_"))

        // Jump to next command.
        X_C(jmp_to_next_command,    _T("jmp_to_next_command"))

        // Executes following command.
        X_C(execute_following_command,  _T("execute_following_command"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // exec_method_t

    // Constructor.
    template<typename _rt_method_t>
    exec_method_t::exec_method_t(executor_env_t & env, _rt_method_t * method)
    {
        _A(method != nullptr);

        this->method = method;
        __commands = nullptr;

        // _P(_T("--------------- METHOD:"), method->get_name());

        __method_parser = new method_parser_t(this, env, method);
        __commands_uninitialized = true;

        // The parser that reads the body, for the callers that inline this method.
        if constexpr (!std::is_same<_rt_method_t, rt_dynamic_method_t>::value)
        {
            __inline_parser_factory = [this, &env, method]() -> object_t * {
                method_parser_t * parser = new method_parser_t(this, env, method, true);
                parser->initialize();
                return parser;
            };
        }

        // The parser is made again when the method gets its superinstructions (it is deleted when the commands are made).
        if (fusion_mode() == fusion_mode_t::automatic)
        {
            __parser_factory = [this, &env, method]() -> object_t * {
                method_parser_t * parser = new method_parser_t(this, env, method, true);
                parser->initialize();
                return parser;
            };
        }
    }

    // Gets the superinstructions of a method.
    command_t ** exec_method_t::tier_up()
    {
        if (!__parser_factory || __tiered.exchange(true))
            return nullptr;

        try
        {
            method_parser_t * parser = (method_parser_t *)__parser_factory();
            command_t ** commands = parser->parse_commands(true);
            delete parser;

            if (commands != nullptr)
                __commands = commands;

            return commands;
        }
        catch (...)
        {
            return nullptr;
        }
    }

    // Makes a parser that only reads.
    object_t * exec_method_t::new_inline_parser()
    {
        return __inline_parser_factory? __inline_parser_factory() : nullptr;
    }

    // Initialize this executable method.
    void exec_method_t::initialize()
    {
        method_parser_t * parser = (method_parser_t *)__get_method_parser();
        parser->initialize();
    }

    // Returns method name.
    string_t exec_method_t::get_name() const
    {
        _A(method != nullptr);
        return method->get_name();
    }

    // Returns commands.
    command_t ** exec_method_t::get_commands()
    {
        if (__commands_uninitialized)
        {
            method_parser_t * parser = (method_parser_t *)__get_method_parser();
            __commands = parser->parse_commands();

            delete parser;
        }

        return __commands;
    }

    // Returns method parser.
    object_t * exec_method_t::__get_method_parser()
    {
        return (object_t *)((arch_uint_t)__method_parser & ~0x01);
    }

    // Converts to string.
    X_DEFINE_TO_STRING(exec_method_t)
    {
        return get_name();
    }

    // Deconstructor.
    exec_method_t::~exec_method_t()
    {
        if (__commands_uninitialized)
            delete __get_method_parser();
    }

    ////////// ////////// ////////// ////////// //////////
    // executor_env_t

    // Returns execute method of runtime method / generic method.
    template<typename _rt_method_t>
    exec_method_t * executor_env_t::__exec_method_of(_rt_method_t * method)
    {
        exec_method_t * exec_method = __from_cache(method);
        if (exec_method != nullptr)
            return exec_method;

        exec_method = memory_t::new_obj<exec_method_t>(memory, *this, method);
        __method_map[(void *)method] = exec_method;
        exec_method->initialize();

        return exec_method;
    }

    // Returns execute method of runtime method.
    exec_method_t * executor_env_t::exec_method_of(rt_method_t * method)
    {
        _A(method != nullptr);

        //_P(_T("exec_method_of: "), method->get_name());

        return __exec_method_of(method);
    }

    // Returns execute method of runtime generic method.
    exec_method_t * executor_env_t::exec_method_of(rt_generic_method_t * generic_method)
    {
        _A(generic_method != nullptr);

        /*
        _PF(_T("exec_method_of: %1%.%2%"),
            generic_method->host_type->get_name(*this), generic_method->get_name()
        );
        */

        return __exec_method_of(generic_method);
    }

    // Returns execute method of runtime dynamic method.
    exec_method_t * executor_env_t::exec_method_of(rt_dynamic_method_t * dynamic_method)
    {
        _A(dynamic_method != nullptr);

        /*
        _PF(_T("exec_method_of: %1%.%2%"),
            dynamic_method->get_host_type()->get_name(*this), dynamic_method->get_name()
        );
        */

        return __exec_method_of(dynamic_method);
    }

    // Returns execute method of runtime method or generic method.
    exec_method_t * executor_env_t::exec_method_of(rt_method_base_t * rt_method_base)
    {
        _A(rt_method_base != nullptr);

        switch (rt_method_base->this_type())
        {
            case rt_member_type_t::general:
                return exec_method_of((rt_method_t *)rt_method_base);

            case rt_member_type_t::generic:
                return exec_method_of((rt_generic_method_t *)rt_method_base);

            case rt_member_type_t::dynamic:
                return exec_method_of((rt_dynamic_method_t *)rt_method_base);

            default:
                X_UNEXPECTED(_T("unexpected member type when generate exec_method_t"));
        }
    }

    // Creates an array of command_t *.
    command_t ** executor_env_t::new_commands(size_t count)
    {
        return __command_heap.acquire(count);
    }

    // Creates an array of exec_method_block_t.
    exec_method_block_t * executor_env_t::new_blocks(size_t count)
    {
        return __method_block_heap.acquire(count);
    }

    #undef __CheckCache

    // Returns the object that holds the static fields of the type.
    rt_ref_t executor_env_t::static_data_of(rt_type_t * type)
    {
        auto it = __static_data_map.find(type);
        if (it != __static_data_map.end())
            return it->second;

        _A(static_heap != nullptr);

        type->pre_new(*this);
        rt_ref_t obj = static_heap->new_obj(type);
        __static_data_map[type] = obj;

        return obj;
    }

    // Returns the object that holds the thread static fields of the type for the running thread.
    rt_ref_t executor_env_t::thread_static_data_of(rt_type_t * type)
    {
        rtlib::thread_t * thread = rtlib::thread_manager_t::instance().current();
        int64_t id = thread != nullptr? thread->id() : 1;

        auto key = std::make_pair(id, type);

        auto it = __thread_static_map.find(key);
        if (it != __thread_static_map.end())
            return it->second;

        _A(static_heap != nullptr);

        type->pre_new(*this);
        rt_ref_t obj = static_heap->new_obj(type);
        __thread_static_map[key] = obj;

        return obj;
    }

    // Gets exec_method_t from cache, returns nullptr if not found.
    exec_method_t * executor_env_t::__from_cache(void * method)
    {
        auto it = __method_map.find((void *)method);
        if (it != __method_map.end())
            return it->second;

        return nullptr;
    }

    ////////// ////////// ////////// ////////// //////////
    // executor_t

    // Ends the process with a stack overflow.
    void __stack_overflow()
    {
        std::wcerr << _T("Stack overflow.") << std::endl;
        std::exit(1);
    }

    namespace
    {
        // Runs the threads of the program: the method System.Threading.Thread.__Entry(thread) in a new context.
        class __thread_host_t : public rtlib::thread_host_t
        {
        public:
            __thread_host_t(executor_env_t & env, rt_heap_t * heap) : __env(env), __heap(heap) { }

            virtual bool run(rtlib::thread_t * thread) override
            {
                rt_assembly_t * core_assembly = __env.get_core_assembly();
                rt_general_type_t * type = __env.get_core_type(_T("Threading"), _T("Thread"), 0);

                ref_t entry_ref = ref_t::null;
                rt_method_base_t * entry_method = nullptr;

                type->each_method(__env, [&](ref_t ref, rt_method_base_t * method) {
                    if (string_t(method->get_name()) == _T("__Entry"))
                    {
                        entry_ref = ref;
                        entry_method = method;
                        return false;
                    }

                    return true;
                });

                if (entry_method == nullptr)
                    throw _ED(exec_error_code_t::method_no_body, _T("System.Threading.Thread.__Entry"));

                command_creating_context_t creating_ctx(__env, core_assembly, type, entry_method);

                command_t * commands[] = {
                    new_push_ref_command(thread->entry()),
                    new_static_call_command(creating_ctx, entry_ref),
                    new_end_command(),
                };

                command_execute_context_t exec_ctx(__heap, __env);
                execute_commands(exec_ctx, commands);

                return !exec_ctx.unhandled;
            }

        private:
            executor_env_t &    __env;
            rt_heap_t *         __heap;
        };
    }

    // Executes commands.
    bool executor_t::execute()
    {
        rt_assembly_t * main_assembly = env.assemblies.load_main_assembly();
        rt_method_t * entry_point = main_assembly->get_entry_point();

        if (entry_point == nullptr)
            throw _ED(exec_error_code_t::assembly_no_entry_point, main_assembly);

        rt_type_t * host_type = entry_point->get_host_type();
        _A(host_type != nullptr);

        command_creating_context_t creating_ctx(env, main_assembly, host_type, entry_point);

        // The static constructors of the libraries (the assemblies that are not the main one) run first: the main assembly
        // calls its own at the beginning of its entry point.
        std::vector<command_t *> commands;

        auto add_static_constructors = [&](rt_assembly_t * assembly) {
            if (assembly == main_assembly)
                return;

            assembly->each_static_constructor([&](ref_t ref) {
                rt_general_type_t * host = assembly->get_host_by_method_ref(ref);
                rt_method_t * method = assembly->get_method(ref);

                command_creating_context_t ctor_ctx(env, assembly, host, method);
                commands.push_back(new_static_call_command(ctor_ctx, ref));
            });
        };

        // The core library first: the others use it.
        add_static_constructors(env.get_core_assembly());

        for (int k = 0; k < env.assemblies.count(); k++)
        {
            if (env.assemblies.at(k) != env.get_core_assembly())
                add_static_constructors(env.assemblies.at(k));
        }

        commands.push_back(new_static_call_command(creating_ctx, (*main_assembly)->entry_point));
        commands.push_back(new_end_command());

        // Lets the gc tell which parts of objects hold references.
        gc_type_resolver_scope_t gc_resolver_scope(__ctx.heap, env);

        env.static_heap = __ctx.heap;
        env.static_data_getter = [this](rt_type_t * type) { return env.static_data_of(type); };
        __ctx.heap->add_root_source(&env);

        // The program runs in the main thread; the other threads run when it gives them the lock.
        rtlib::thread_manager_t & threads = rtlib::thread_manager_t::instance();
        threads.attach_main();

        __thread_host_t thread_host(env, __ctx.heap);
        threads.set_host(&thread_host);

        command_execute_context_t exec_ctx(__ctx.heap, env);
        execute_commands(exec_ctx, commands.data());

        // The program ends when the threads that are not in the background end.
        if (!exec_ctx.unhandled)
            threads.wait_for_foreground();

        threads.set_host(nullptr);
        __ctx.heap->remove_root_source(&env);
        env.static_heap = nullptr;

        _A(exec_ctx.stack.bottom() == exec_ctx.stack.top() || exec_ctx.unhandled);
        return !exec_ctx.unhandled;
    }

    ////////// ////////// ////////// ////////// //////////

    // Appends a assembly.
    void exec_assemblies_t::append(rt_assembly_t * assembly, 
                                const string_t & package, const string_t & name)
    {
        _A(assembly != nullptr);

        string_t assembly_name = name;
        if (assembly_name.empty())
            assembly_name = assembly->get_name();

        __map[__key_t(package, assembly_name)] = assembly;
        __last = assembly;
    }

    // Gets a assembly by package and name.
    rt_assembly_t * exec_assemblies_t::get(const string_t & package, const string_t & name)
    {
        auto it = __map.find(__key_t(package, name));
        if (it == __map.end())
            return nullptr;

        return it->second;
    }

    ////////// ////////// ////////// ////////// //////////

    // Executes with specified context.
    bool execute(rt_context_t & ctx)
    {
        return executor_t(ctx).execute();
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::exec

