#ifndef __EXEC_COMMANDS_H__
#define __EXEC_COMMANDS_H__

#include <exec.h>
#include "utils.h"
#include <deque>

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

    // The inlining of a call (parser.cpp makes the plan of a method, this file knows the xils and the commands): the body of the callee
    // is put in the xils of the caller, its params and variables have a place in the frame of the caller.
    struct inline_site_t
    {
        exec_method_t *                 callee = nullptr;
        command_creating_context_t *    ctx = nullptr;          // The context of the callee: its variables are in the frame of the caller.
        int                             arg_count = 0;
        int                             stack_args = 0;         // The args that the caller pushed to the stack: the first ones.
        int32_t                         arg_offsets[8];         // The places of the args, in bytes from the local variables of the caller.
        int                             arg_identities[8];      // The extra variables of the layout that stand for the args.

        // The last args that are the push of a local variable, its address or a constant, just before the call, are not pushed: the body
        // reads the variable (or the constant) itself. 0: the arg is in the stack, 1: a local, 2: the address of a local, 3: a constant.
        uint8_t                         forward_kind[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
        const core::xil_base_t *        forward_xil[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
        bool                            is_static = false;
        bool                            check_this = false;     // The object of the call is checked for null (a class).
        rt_type_t *                     static_type = nullptr;  // The type of a static method: it is initialized by the call.
    };

    // The method that a call xil goes to, when the call can be inlined: a static method, or an instance method that is not virtual.
    exec_method_t * inline_call_target(command_creating_context_t & ctx, const core::call_xil_t & xil, bool * is_static);

    // Whether the body of a callee can be put in the frame of the caller: the xils (the last one is the ret), the params and the result.
    bool inline_check_body(command_creating_context_t & callee_ctx, const std::vector<const core::xil_base_t *> & xils, size_t max_xils);

    // Whether the body is the one of an accessor (a getter, a setter, a method that does nothing or only calls the one of the base): a call of
    // it is made without a frame already (analyze_trivial_method).
    bool inline_is_accessor_shape(const std::vector<const core::xil_base_t *> & xils);

    // The xil of the body of a callee, for the caller: the args are local variables of the identities of the site. The clone is in the
    // storage (it must stay while the commands are made). Returns the xil itself when it does not change.
    // index and size: the place of the xil in the body (with its last ret): a ret that is not the last becomes a jump to the end.
    const core::xil_base_t * inline_rewrite_xil(const inline_site_t & site, const core::xil_base_t * xil,
                                                std::deque<std::vector<byte_t>> & storage, size_t index, size_t size);

    // The command that begins the body of an inlined call: it moves the args from the stack to their places.
    command_t * new_inline_enter_command(const inline_site_t & site);

    // The statistics of the calls (APRIL_CALL_STATS=1): kind 0 static, 1 instance, 2 virtual, 3 interface, 4 delegate.
    extern bool exec_call_stats_on;
    void exec_call_count(exec_method_t * method, int kind);
    void exec_call_stats_register(command_creating_context_t & ctx, exec_method_t * method, size_t xils, bool has_blocks, bool has_switch);

    // Finds out whether a method only reads or writes a field of this (the accessor of a property), the call of it needs no frame then.
    void analyze_trivial_method(command_creating_context_t & ctx, exec_method_t * method, bool is_instance,
                                const core::xil_base_t * const * xils, size_t count);

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
