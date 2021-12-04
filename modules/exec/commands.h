#ifndef __EXEC_COMMANDS_H__
#define __EXEC_COMMANDS_H__

#include <exec.h>
#include "utils.h"

namespace X_ROOT_NS { namespace modules { namespace exec {

    namespace
    {
        using namespace rt;
    }

    ////////// ////////// ////////// ////////// //////////

    // Context for creating command.
    class command_creating_context_t : public assembly_analyzer_t
                                     , public no_copy_ctor_t
    {
        typedef command_creating_context_t  __self_t;
        typedef assembly_analyzer_t         __super_t;

    public:

        // Constructor.
        command_creating_context_t(executor_env_t & env, rt_assembly_t * assembly,
            rt_type_t * type, rt_method_t * method,
            const generic_param_manager_t * gp_manager = nullptr);

        rt_type_t * const           type;               // Runtime type.
        rt_method_t * const         method;             // Runtime method.
        locals_layout_t             locals_layout;      // Locals layout.
        params_layout_t             params_layout;      // Params layout.
        exec_switch_manager_t       switch_manager;     // Switch...case method.
        executor_env_t &            env;                // Executor environment.

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

    // Creates static call command.
    command_t * new_static_call_command(command_creating_context_t & ctx, ref_t method_ref);

    // Creates ret command.
    command_t * new_ret_command(command_creating_context_t & ctx);

    // Creates end command.
    command_t * new_end_command();

    // Returns whether it's a ret command.
    bool is_ret_command(command_t * command);

    // Converts command to string.
    const string_t to_command_string(command_t * command);

    // Executes a set of commands.
    void execute_commands(command_execute_context_t & ctx, command_t ** commands);

    ////////// ////////// ////////// ////////// //////////

} } }

#endif // __EXEC_COMMANDS_H__
