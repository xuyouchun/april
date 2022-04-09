#include <exec.h>
#include "parser.h"
#include "commands.h"
#include "utils.h"

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

    // Executes commands.
    void executor_t::execute()
    {
        rt_assembly_t * main_assembly = env.assemblies.load_main_assembly();
        rt_method_t * entry_point = main_assembly->get_entry_point();

        if (entry_point == nullptr)
            throw _ED(exec_error_code_t::assembly_no_entry_point, main_assembly);

        rt_type_t * host_type = entry_point->get_host_type();
        _A(host_type != nullptr);

        command_creating_context_t creating_ctx(env, main_assembly, host_type, entry_point);

        command_t * commands[] = {
            new_static_call_command(creating_ctx,  (*main_assembly)->entry_point),
            new_end_command(),
        };

        command_execute_context_t exec_ctx(__ctx.heap, env);
        execute_commands(exec_ctx, commands);

        _A(exec_ctx.stack.bottom() == exec_ctx.stack.top());
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
    void execute(rt_context_t & ctx)
    {
        executor_t(ctx).execute();
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::exec

