#include <exec.h>
#include "parser.h"
#include "commands.h"
#include "utils.h"

namespace X_ROOT_NS { namespace modules { namespace exec {

    using namespace core;
    using namespace rt;

    #define __Debug     0

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(exec_error_code_t)

        X_D(assembly_not_found, _T("assembly '%1% not found"))

        X_D(assembly_no_entry_point, _T("assembly '%1%' has no entry point"))

        X_D(method_no_body, _T("method '%1% has no body"))

        X_D(internal_function_not_found, _T("internal funciton '%1%' not found"))

        X_D(method_parse_error__unexpected_index, _T("method parse error: unexpected index"))

        X_D(switch_table_index_overflow, _T("switch table index overflow"))

        X_D(type_not_found, _T("type '%1%' not found"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(exec_env_code_t)

        X_C(end,        _T("end"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // command_execute_context_t

    void command_execute_context_t::push_calling(command_t ** command)
    {
        stack.push(stack.lp());
        stack.push(current);

        this->current = command;
        stack.set_lp(stack.top());
    }

    void command_execute_context_t::pop_calling()
    {
        current = stack.pop<command_t **>();
        stack.set_lp(stack.pop<rt_stack_unit_t *>());
    }

    void command_execute_context_t::pop_calling(const __calling_stub_t * p)
    {
        current = p->current;
        stack.set_lp(p->lp);
    }

    ////////// ////////// ////////// ////////// //////////
    // executor_env_t

    exec_method_t * executor_env_t::exec_method_of(rt_method_t * rt_method,
                                     rt_type_t * rt_type, rt_assembly_t * rt_assembly)
    {
        _A(rt_method != nullptr);
        _A(rt_type != nullptr);
        _A(rt_assembly != nullptr);

        auto it = __method_map.find(rt_method);
        if(it != __method_map.end())
            return it->second;

        exec_method_t * exec_method = parse_commands(*this, rt_assembly, rt_type, rt_method);
        __method_map[rt_method] = exec_method;

        return exec_method;
    }

    ////////// ////////// ////////// ////////// //////////
    // executor_t

    void executor_t::execute()
    {
        rt_assembly_t * main_assembly = env.assembly_provider->load_main_assembly();
        rt_method_t * entry_point = main_assembly->get_entry_point();

        if(entry_point == nullptr)
            throw _ED(exec_error_code_t::assembly_no_entry_point, main_assembly);

        command_creating_context_t creating_ctx(
            env, main_assembly, entry_point
        );

        command_t * commands[] = {
            new_static_call_command(creating_ctx,  (*main_assembly)->entry_point),
            new_end_command(),
        };

        command_execute_context_t exec_ctx(__ctx.heap, env);
        __execute_method(exec_ctx, commands);
    }

    void executor_t::__execute_method(command_execute_context_t & ctx, command_t ** commands)
    {
        ctx.current = commands;

        try
        {
            _begin:

            #define __ExecuteCmd        (*ctx.current++)->execute(ctx)

            #if __Debug
                #define __ExecuteCommand()                                      \
                    _P(*ctx.current);                                           \
                    __ExecuteCmd
            #else
                #define __ExecuteCommand()                                      \
                    __ExecuteCmd
            #endif

            #define __ExecuteCommands()                                         \
                __ExecuteCommand(); __ExecuteCommand(); __ExecuteCommand(); __ExecuteCommand();

            __ExecuteCommands()
            __ExecuteCommands()
            __ExecuteCommands()
            __ExecuteCommands()

            #undef __ExecuteCommands
            #undef __ExecuteCommand

            goto _begin;
        }
        catch(logic_error_t<exec_env_code_t> & e)
        {
            // Exit
        }
    }

    ////////// ////////// ////////// ////////// //////////

    void rt_assemblies_t::append(rt_assembly_t * assembly, 
                                const string_t & package, const string_t & name)
    {
        _A(assembly != nullptr);

        string_t assembly_name = name;
        if(assembly_name.empty())
            assembly_name = assembly->get_name();

        __map[__key_t(package, assembly_name)] = assembly;
        __last = assembly;
    }

    rt_assembly_t * rt_assemblies_t::get(const string_t & package, const string_t & name)
    {
        auto it = __map.find(__key_t(package, name));
        if(it == __map.end())
            return nullptr;

        return it->second;
    }

    ////////// ////////// ////////// ////////// //////////

    class __rt_assembly_provider_t : public rt_assembly_provider_t
    {
    public:
        __rt_assembly_provider_t(rt_assemblies_t & assemblies, rt_assembly_t & entrypoint)
            : __assemblies(assemblies), __entrypoint(entrypoint)
        { }

        virtual rt_assembly_t * load_main_assembly() override
        {
            __revise_assembly(&__entrypoint);
            return &__entrypoint;
        }

        virtual rt_assembly_t * load_assembly(const string_t & package,
                                              const string_t & name) override
        {
            rt_assembly_t * assembly = get_assembly(__assemblies, package, name);
            __revise_assembly(assembly);

            return assembly;
        }

        virtual rt_assembly_t * at(int index) override
        {
            if(index < 0 || index >= __assembly_vector.size())
                throw _EC(overflow);

            return __assembly_vector[index];
        }

    private:
        rt_assemblies_t & __assemblies;
        rt_assembly_t &   __entrypoint;

        std::vector<rt_assembly_t * > __assembly_vector;

        void __revise_assembly(rt_assembly_t * assembly)
        {
            if(assembly->index < 0)
            {
                assembly->index = __assembly_vector.size();
                __assembly_vector.push_back(assembly);
            }
        }
    };

    void execute(rt_context_t & ctx, rt_assemblies_t & assemblies, rt_assembly_t & entrypoint)
    {
        __rt_assembly_provider_t rt_assembly_provider(assemblies, entrypoint);
        executor_t(ctx, &rt_assembly_provider).execute();
    }

    void execute(rt_context_t & ctx, rt_assembly_t & assembly)
    {
        rt_assemblies_t assemblies;
        assemblies.append(&assembly);

        execute(ctx, assemblies, assembly);
    }

    void execute(rt_context_t & ctx, rt_assemblies_t & assemblies)
    {
        _A(assemblies.size() > 0);

        execute(ctx, assemblies, *assemblies.last());
    }

    ////////// ////////// ////////// ////////// //////////

} } }
