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

    class command_creating_context_t : public assembly_analyzer_t
                                     , public no_copy_ctor_t
    {
        typedef command_creating_context_t  __self_t;
        typedef assembly_analyzer_t         __super_t;

    public:
        command_creating_context_t(executor_env_t & env, rt_assembly_t * assembly,
                                                         rt_method_t * method)
            : __super_t(env.pool, assembly, env.assembly_provider, env.get_memory())
            , method(method), env(env)
            , locals_layout(*this), params_layout(*this)
        {
            _A(assembly != nullptr);
            _A(method != nullptr);
        }

        rt_method_t * method;
        locals_layout_t locals_layout;
        params_layout_t params_layout;
        exec_switch_manager_t switch_manager;
        executor_env_t & env;

        msize_t ret_unit_size();

    private:
        msize_t __ret_unit_size = unknown_msize;
        storage_type_t __ret_storage_type;
    };

    command_t * new_command(command_creating_context_t & ctx, const xil_base_t * xil);
    command_t * new_static_call_command(command_creating_context_t & ctx, ref_t method_ref);

    command_t * new_ret_command(command_creating_context_t & ctx);
    command_t * new_end_command();

    bool is_ret_command(command_t * command);

    ////////// ////////// ////////// ////////// //////////

} } }

#endif // __EXEC_COMMANDS_H__
