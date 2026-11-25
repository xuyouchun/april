#ifndef __EXEC_COMMANDS_H__
#define __EXEC_COMMANDS_H__

#include <exec.h>
#include "utils.h"

namespace X_ROOT_NS::modules::exec {

    namespace
    {
        using namespace rt;
    }

    ////////// ////////// ////////// ////////// //////////

    struct fusion_plan_t;
    struct fusion_group_t;

    // Context for creating command.
    class command_creating_context_t : public assembly_analyzer_t
                                     , public no_copy_ctor_t
    {
        typedef command_creating_context_t  __self_t;
        typedef assembly_analyzer_t         __super_t;

    public:

        // Constructor.
        command_creating_context_t(executor_env_t & env, rt_assembly_t * assembly,
            rt_type_t * type, rt_method_base_t * method,
            const generic_param_manager_t * gp_manager = nullptr);

        rt_type_t * const           type;               // Runtime type.
        rt_method_base_t * const    method;             // Runtime method.
        locals_layout_t             locals_layout;      // Locals layout.
        params_layout_t             params_layout;      // Params layout.
        exec_switch_manager_t       switch_manager;     // Switch...case method.
        executor_env_t &            env;                // Executor environment.

        // The plan of the superinstructions of the method (the commands are not the xils then), and the index of the xil that a command
        // is made for.
        const fusion_plan_t *       plan = nullptr;
        uint32_t                    xil_index = 0;

        msize_t ret_unit_size();                        // Returns unit size.

        // Converts template to a generic type. (with cache)
        rt_type_t * to_generic_type(rt_general_type_t * template_);

    private:
        msize_t __ret_unit_size = unknown_msize;
        storage_type_t __ret_storage_type;

        std::map<rt_general_type_t *, rt_type_t *> __generic_type_map;
    };

    // Creates command.
    command_t * new_command(command_creating_context_t & ctx, const xil_base_t * xil);

    // Creates command.
    command_t * new_command(command_creating_context_t & ctx, rt_command_t * cmd);

    // Creates static call command.
    command_t * new_static_call_command(command_creating_context_t & ctx, ref_t method_ref);

    // Creates ret command.
    command_t * new_ret_command(command_creating_context_t & ctx);

    // Creates end command.
    command_t * new_end_command();

    // Returns a command that counts the executions of another one, and then runs it (the profile of the commands, exec_profile.h).
    command_t * new_profile_command(command_t * target, uint64_t * counter);

    // Creates the command of a group of xils (a superinstruction).
    command_t * new_super_command(command_creating_context_t & ctx, const fusion_plan_t & plan,
                                  const core::xil_base_t * const * xils, const fusion_group_t & group);

    // Creates the command at the beginning of a method that counts the calls (the method gets its superinstructions at the threshold).
    command_t * new_tier_command(exec_method_t * method, int threshold);

    // Creates the command that pushes a reference.
    command_t * new_push_ref_command(void * ref);

    // Returns whether it's a ret command.
    bool is_ret_command(command_t * command);

    // Converts command to string.
    const string_t to_command_string(command_t * command);

    // Executes a set of commands.
    void execute_commands(command_execute_context_t & ctx, command_t ** commands);

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::exec

#endif // __EXEC_COMMANDS_H__
