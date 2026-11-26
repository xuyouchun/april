#include "parser.h"
#include "commands.h"
#include "utils.h"
#include "exec_profile.h"
#include "fusion.h"
#include <lib.h>
#include <chrono>

namespace X_ROOT_NS::modules::exec {

    typedef exec_error_code_t __e_t;
    using namespace core;

    ////////// ////////// ////////// ////////// //////////
    // dynamic_method_reader_t

    // Read next command, returns nullptr when reached end.
    rt_command_t * dynamic_method_reader_t::read_next()
    {
        if (__bytes >= __bytes_end)
            return nullptr;

        byte_t size = *__bytes++;
        _A(size <= (__bytes_end - __bytes));

        rt_command_t * command = (rt_command_t *)__bytes;
        __bytes += size;

        return command;
    }

    ////////// ////////// ////////// ////////// //////////
    // Method parser.

    // Constructor.
    method_parser_t::method_parser_t(exec_method_t * exec_method, executor_env_t & env,
                                rt_method_t * method, bool readonly)
        : __exec_method(exec_method), __env(env), __method(method), __raw_method(method), __readonly(readonly)
    {
        _A(exec_method != nullptr);
        _A(method != nullptr);

        __host_type = method->get_host_type();
        _A(__host_type != nullptr);
        _A(is_runnable_type(__host_type));

        __analyzer = __pool.new_obj<assembly_analyzer_t>(to_analyzer(env, __host_type));

        __initialize_ex();
    }

    #define __LogPrefix    _T("* ")

    // Constructor.
    method_parser_t::method_parser_t(exec_method_t * exec_method, executor_env_t & env,
                                                    rt_generic_method_t * generic_method, bool readonly)
        : __exec_method(exec_method), __env(env), __raw_method(generic_method), __readonly(readonly)
    {
        _A(exec_method != nullptr);
        _A(generic_method != nullptr);

        __method = generic_method->template_;
        _A(__method != nullptr);

        assembly_analyzer_t analyzer = to_analyzer(env, __method->get_host_type());
        __gp_manager = __pool.new_obj<generic_param_manager_t>(analyzer, generic_method);

        __host_type = generic_method->get_host_type();
        _A(is_runnable_type(__host_type));

        __analyzer = __pool.new_obj<assembly_analyzer_t>(
            to_analyzer(env, __host_type, __gp_manager)
        );

        __initialize_ex();
    }

    // Constructor.
    method_parser_t::method_parser_t(exec_method_t * exec_method, executor_env_t & env,
                                                    rt_dynamic_method_t * dynamic_method, bool readonly)
        : __exec_method(exec_method), __env(env), __raw_method(dynamic_method), __readonly(readonly)
    {
        _A(exec_method != nullptr);
        _A(dynamic_method != nullptr);

        __host_type = dynamic_method->get_host_type();
        _A(is_runnable_type(__host_type));

        __initialize();

        rt_dynamic_method_body_t * body = dynamic_method->body;
        _A(body != nullptr);

        rt_bytes_t commands = body->commands;
        _A(commands.bytes != nullptr);

        __dynamic_method = dynamic_method;
        __dynamic_method_reader = __pool.new_obj<dynamic_method_reader_t>(
            commands.bytes, commands.length, __env.memory
        );
    }

    // Initialize.
    void method_parser_t::__initialize()
    {
        __assembly = __host_type->get_assembly();
        _A(__assembly != nullptr);

        __creating_ctx = __pool.new_obj<command_creating_context_t>(
            __env, __assembly, __host_type, __raw_method, __gp_manager
        );
    }

    // Initialize with method body and method reader.
    void method_parser_t::__initialize_ex()
    {
        __initialize();

        rt_bytes_t body = __assembly->get_method_body(__method);
        __method_reader = __pool.new_obj<method_reader_t>(body.bytes, body.length, __env.memory);
    }

    // Initialize.
    void method_parser_t::initialize()
    {
        if (__raw_method->this_type() == rt_member_type_t::dynamic)
            this->__parse_dynamic_info();
        else
            this->__parse_normal_info();
    }

    // Parse this method.
    command_t ** method_parser_t::parse_commands(bool fast)
    {
        if (__raw_method->this_type() == rt_member_type_t::dynamic)
            return fast? nullptr : __parse_dynamic_commands();

        return __parse_normal_commands(fast);
    }

    ////////// ////////// ////////// ////////// //////////
    // The inlining of calls

    // What the commands of a method need when the body of the callees is in its xils: the context of each xil, the first command of each
    // body, the places where a command must begin, and what must stay alive until the commands are made.
    struct inline_state_t
    {
        std::vector<command_creating_context_t *>   contexts;       // For each xil: the context of the callee (nullptr: the method).
        std::vector<const inline_site_t *>          enters;         // For each xil: the site that it is the first command of.
        std::vector<uint8_t>                        cuts;           // The places where a command must begin.
        std::deque<inline_site_t>                   sites;
        std::vector<method_parser_t *>              parsers;        // The parsers of the callees (their contexts).
        std::deque<std::vector<byte_t>>             storage;        // The xils that are made again.

        ~inline_state_t()
        {
            for (method_parser_t * parser : parsers)
                delete parser;
        }
    };

    // The xil that is the place of the first command of a body.
    static smp_xil_t __inline_enter_xil(xil_smp_t::empty);

    static bool __inline_enabled()
    {
        static const bool enabled = []() {
            const char * value = std::getenv("APRIL_INLINE_CALLS");
            return value == nullptr || std::string(value) != "0";
        }();

        return enabled;
    }

    static size_t __inline_max_xils()
    {
        static const size_t size = []() {
            const char * value = std::getenv("APRIL_INLINE_SIZE");
            int n = value != nullptr? std::atoi(value) : 0;
            return n > 0? (size_t)n : (size_t)24;
        }();

        return size;
    }

    void method_parser_t::__expand_calls(std::vector<const xil_base_t *> & xils, al::svector_t<method_xil_block_t, 10> & blocks,
                                         inline_state_t & state)
    {
        const size_t count = xils.size();

        // The places that a jump, a switch or a block goes to: the pushes of the args of a call that are forwarded must not have one
        // in them (but at the first), and the places that the locals have their address taken.
        std::vector<uint8_t> targets(count + 1, 0);
        std::set<int> address_taken;
        std::set<exec_switch_table_t *> tables;

        for (size_t i = 0; i < count; i++)
        {
            if (command_of(xils[i]) == xil_command_t::jmp)
            {
                const jmp_xil_t * jmp = (const jmp_xil_t *)xils[i];

                if (jmp->model() == xil_jmp_model_t::switch_)
                {
                    exec_switch_table_t * table = __creating_ctx->switch_manager.get_table(jmp->tbl());
                    if (table == nullptr || !tables.insert(table).second)
                        continue;

                    for (int k = 0; k < table->row_count; k++)
                    {
                        int64_t t = (int64_t)i + table->rows[k].step;
                        if (t >= 0 && t <= (int64_t)count)
                            targets[t] = 1;
                    }

                    int64_t t = (int64_t)i + table->default_step;
                    if (t >= 0 && t <= (int64_t)count)
                        targets[t] = 1;
                }
                else
                {
                    int64_t t = (int64_t)i + jmp->step();
                    if (t >= 0 && t <= (int64_t)count)
                        targets[t] = 1;
                }
            }
            else if (command_of(xils[i]) == xil_command_t::push
                        && ((const push_xil_t *)xils[i])->stype() == xil_storage_type_t::local_addr)
            {
                address_taken.insert(((const push_xil_t *)xils[i])->get_identity());
            }
        }

        for (const method_xil_block_t & block : blocks)
        {
            if (block.xil_start <= count)   targets[block.xil_start] = 1;
            if (block.xil_end <= count)     targets[block.xil_end] = 1;
            if (block.entry_point <= count) targets[block.entry_point] = 1;
        }

        // The calls that are inlined: the site and the xils of the body (without the ret).
        std::vector<inline_site_t *> sites(count, nullptr);
        std::vector<std::vector<const xil_base_t *>> bodies(count);
        std::vector<uint8_t> dropped(count, 0);
        size_t budget = 400;

        for (size_t i = 0; i < count; i++)
        {
            if (command_of(xils[i]) != xil_command_t::call)
                continue;

            bool is_static = false;
            exec_method_t * target = inline_call_target(*__creating_ctx, *(const call_xil_t *)xils[i], &is_static);
            if (target == nullptr || target == __exec_method)
                continue;

            method_parser_t * parser = (method_parser_t *)target->new_inline_parser();
            if (parser == nullptr)
                continue;

            std::vector<const xil_base_t *> body;
            bool has_blocks = false;
            parser->read_body(&body, &has_blocks);

            command_creating_context_t * ctx = parser->creating_context();
            if (has_blocks || inline_is_accessor_shape(body) || !inline_check_body(*ctx, body, __inline_max_xils()) || body.size() > budget)
            {
                delete parser;
                continue;
            }

            // The tools of the tests: APRIL_INLINE_LIMIT=n inlines the first n calls only, APRIL_INLINE_TRACE=1 tells each one.
            {
                static std::atomic<int> sites_made { 0 };
                static const int limit = std::getenv("APRIL_INLINE_LIMIT") != nullptr? std::atoi(std::getenv("APRIL_INLINE_LIMIT")) : -1;
                static const bool trace = std::getenv("APRIL_INLINE_TRACE") != nullptr;

                int number = sites_made++;
                if (limit >= 0 && number >= limit)
                {
                    delete parser;
                    continue;
                }

                if (trace)
                    std::wcerr << L"inline #" << number << L": " << __exec_method->get_name() << L" <- " << target->get_name() << L" (" << body.size() << L" xils)\n";
            }

            budget -= body.size();
            state.parsers.push_back(parser);

            // The variables of the callee are in the frame of this method, after its own variables; the args follow them.
            inline_site_t & site = state.sites.emplace_back();
            site.callee = target;
            site.ctx = ctx;
            site.arg_count = ctx->params_layout.count();
            site.is_static = is_static;

            rt_type_t * host_type = target->method->get_host_type();
            site.static_type = is_static? host_type : nullptr;
            site.check_this = !is_static && host_type->get_ttype(__env) == ttype_t::class_;

            msize_t locals_units = ctx->locals_layout.unit_size();
            msize_t base = __creating_ctx->locals_layout.reserve(locals_units + site.arg_count);
            ctx->locals_layout.set_shift(base);

            // The args that the body changes (or takes the address of) have a place of their own.
            bool changed[8] = { false, false, false, false, false, false, false, false };
            for (size_t k = 0; k + 1 < body.size() + 1 && k < body.size(); k++)
            {
                const xil_base_t * xil = body[k];

                if (command_of(xil) == xil_command_t::pop && ((const pop_xil_t *)xil)->stype() == xil_storage_type_t::argument)
                    changed[((const pop_xil_t *)xil)->get_identity()] = true;
                else if (command_of(xil) == xil_command_t::push && ((const push_xil_t *)xil)->stype() == xil_storage_type_t::argument_addr)
                    changed[((const push_xil_t *)xil)->get_identity()] = true;
            }

            // The last args that were pushed just before the call (a local variable, its address, a constant) are not pushed: the body
            // reads the variable. The first one may be a place that something goes to, the others not (nor the call).
            int forwarded = 0;
            for (int k = site.arg_count - 1; k >= 0 && i >= (size_t)(site.arg_count - k); k--)
            {
                size_t j = i - (size_t)(site.arg_count - k);

                if (changed[k] || sites[j] != nullptr || dropped[j] || command_of(xils[j]) != xil_command_t::push)
                    break;

                if (forwarded == 0 ? targets[i] != 0 : targets[j + 1] != 0)
                    break;

                if (k == 0 && site.check_this)
                    break;

                const push_xil_t * push = (const push_xil_t *)xils[j];
                uint8_t kind = 0;

                if (push->stype() == xil_storage_type_t::local && address_taken.count(push->get_identity()) == 0)
                    kind = 1;
                else if (push->stype() == xil_storage_type_t::local_addr)
                    kind = 2;
                else if (push->stype() == xil_storage_type_t::constant && push->dtype() != xil_type_t::empty
                            && push->dtype() != xil_type_t::string && push->dtype() != xil_type_t::object && push->dtype() != xil_type_t::ptr)
                    kind = 3;     // The numbers and the booleans only: a string or a type of the constant is made with the context of the callee.

                if (kind == 0)
                    break;

                if (forwarded > 0 && targets[j + 1] != 0)
                    break;

                site.forward_kind[k] = kind;
                forwarded++;
            }

            // Where each arg is: forwarded (the variable of the caller), or moved from the stack to its place by the first command.
            site.stack_args = site.arg_count - forwarded;

            for (int k = 0; k < site.arg_count; k++)
            {
                site.arg_offsets[k] = (int32_t)(base + (locals_units + k) * sizeof(rt_stack_unit_t));

                if (site.forward_kind[k] == 0)
                {
                    site.arg_identities[k] = ctx->locals_layout.add_extra(site.arg_offsets[k], ctx->params_layout.type_at(k));
                    continue;
                }

                size_t j = i - (size_t)(site.arg_count - k);
                const push_xil_t * push = (const push_xil_t *)xils[j];
                dropped[j] = 1;

                if (site.forward_kind[k] == 3)
                {
                    site.forward_xil[k] = push;
                }
                else
                {
                    int identity = push->get_identity();
                    site.arg_identities[k] = ctx->locals_layout.add_extra(
                        (msize_t)__creating_ctx->locals_layout.offset_of(identity), __creating_ctx->locals_layout.type_at(identity)
                    );
                }
            }

            sites[i] = &site;
            body.pop_back();                // The ret.
            bodies[i] = std::move(body);
        }

        if (state.sites.empty())
            return;

        // The new xils: the first command of a body (it moves the args), then the xils of it.
        std::vector<const xil_base_t *> expanded;
        std::vector<int32_t> map(count + 1, 0);

        state.contexts.clear();
        state.enters.clear();

        for (size_t i = 0; i < count; i++)
        {
            map[i] = (int32_t)expanded.size();

            // The push of an arg that the body reads from the variable itself.
            if (dropped[i])
                continue;

            if (sites[i] == nullptr)
            {
                expanded.push_back(xils[i]);
                state.contexts.push_back(nullptr);
                state.enters.push_back(nullptr);
                continue;
            }

            const inline_site_t & site = *sites[i];

            if (site.stack_args > 0 || site.check_this || site.static_type != nullptr)
            {
                expanded.push_back(&__inline_enter_xil);
                state.contexts.push_back(nullptr);
                state.enters.push_back(sites[i]);
            }

            const size_t body_size = bodies[i].size() + 1;       // With the last ret.

            for (size_t k = 0; k < bodies[i].size(); k++)
            {
                expanded.push_back(inline_rewrite_xil(site, bodies[i][k], state.storage, k, body_size));
                state.contexts.push_back(site.ctx);
                state.enters.push_back(nullptr);
            }
        }

        map[count] = (int32_t)expanded.size();

        // The places that the commands of different bodies do not share: the begin and the end of each one.
        state.cuts.assign(expanded.size() + 1, 0);
        for (size_t i = 0; i < count; i++)
        {
            if (sites[i] != nullptr)
            {
                state.cuts[map[i]] = 1;
                state.cuts[map[i + 1]] = 1;
            }
        }

        // The jumps, the switch tables and the blocks go to the new places.
        std::set<exec_switch_table_t *> remapped;

        for (size_t i = 0; i < count; i++)
        {
            if (command_of(xils[i]) != xil_command_t::jmp || sites[i] != nullptr)
                continue;

            const jmp_xil_t * jmp = (const jmp_xil_t *)xils[i];

            if (jmp->model() == xil_jmp_model_t::switch_)
            {
                exec_switch_table_t * table = __creating_ctx->switch_manager.get_table(jmp->tbl());
                if (table == nullptr || !remapped.insert(table).second)
                    continue;

                auto remap = [&](int32_t step) -> int32_t { return map[i + step] - map[i]; };

                for (int k = 0; k < table->row_count; k++)
                    table->rows[k].step = remap(table->rows[k].step);

                table->default_step = remap(table->default_step);
                continue;
            }

            int32_t step = map[i + jmp->step()] - map[i];
            if (step != jmp->step())
            {
                state.storage.emplace_back(sizeof(jmp_xil_t) + 4, 0);
                jmp_xil_t * clone = new (state.storage.back().data()) jmp_xil_t(jmp->model(), step);
                expanded[map[i]] = clone;
            }
        }

        for (method_xil_block_t & block : blocks)
        {
            if (block.xil_start <= count)   block.xil_start = (uint32_t)map[block.xil_start];
            if (block.xil_end <= count)     block.xil_end = (uint32_t)map[block.xil_end];
            if (block.entry_point <= count) block.entry_point = (uint32_t)map[block.entry_point];
        }

        xils = std::move(expanded);

        // The frame is bigger: the commands of the returns and the exceptions use its size (a parser that only reads gives the same size).
        if (!__readonly)
            __exec_method->stack_unit_size = __creating_ctx->locals_layout.unit_size();
    }

    // Parse normal method.
    command_t ** method_parser_t::__parse_normal_commands(bool fast)
    {
        #if EXEC_TRACE
        // _PF(_T("parse_commands: %1%.%2%"), host_type->get_name(env), analyzer.get_name(method));

        string_t ss = __raw_method->to_string(__env, true);
        _PFC(dark_gray, _T("\n%1% %2%"), __LogPrefix, ss.c_str());

        #endif  // EXEC_TRACE

        command_t ** commands = this->__parse_normal_commands_(fast);

        #if EXEC_TRACE

        _PFC(dark_gray, _T("%1% End of %2%\n"), __LogPrefix, ss.c_str());

        #endif

        return commands;
    }

    // Parse method info.
    void method_parser_t::__parse_normal_info()
    {
        // Read local variables
        uint16_t ref_objects = 0;

        locals_layout_t & locals_layout = __creating_ctx->locals_layout;
        __method_reader->each_variable([&locals_layout](local_variable_defination_t & def) {
            locals_layout.append(def);
        });
        locals_layout.commit();

        // Read params variables
        params_layout_t & params_layout = __creating_ctx->params_layout;
        if (!((decorate_value_t)(*__method)->decorate).is_static)
            params_layout.append(__host_type, param_type_t::default_, param_layout_type_t::this_);

        __assembly->each_params((*__method)->params, [&](int index, mt_param_t & mt_param) {

            if (mt_param.param_type == param_type_t::extends)
            {
                ref_t ref = (*__method)->params.at(index);
                __analyzer->each_extend_params(ref, [&](rt_type_t * type) {
                    return params_layout.append(type, param_type_t::extends), true;
                });
            }
            else
            {
                params_layout.append(mt_param.type, mt_param.param_type);
            }

            return true;
        });

        params_layout.commit();
        msize_t this_offset = params_layout.offset_of(0);

        // Read switch tables
        exec_switch_manager_t & switch_manager = __creating_ctx->switch_manager;
        __method_reader->each_switch_table([&switch_manager](exec_switch_table_t * tbl) {
            switch_manager.append_table(tbl);
        });

        if (!__readonly)
        {
            __exec_method->ref_objects = ref_objects;
            __exec_method->stack_unit_size = locals_layout.unit_size();
            __exec_method->this_offset = this_offset;

            locals_layout.value_region(&__exec_method->gc_value_begin, &__exec_method->gc_value_end);
        }
    }

    // Reads the xils of the body.
    void method_parser_t::read_body(std::vector<const xil_base_t *> * xils, bool * has_blocks)
    {
        bool blocks = false;
        __method_reader->each_xil_block([&blocks](method_xil_block_t &) { blocks = true; });
        *has_blocks = blocks;

        __method_reader->each_xil([xils](const xil_base_t * xil) { xils->push_back(xil); });
    }

    // Parse method commands. fast: the method gets its superinstructions now (it was parsed without them: exec_method_t::tier_up),
    // nullptr when it has none.
    command_t ** method_parser_t::__parse_normal_commands_(bool fast)
    {
        // Read xil blocks.
        al::svector_t<method_xil_block_t, 10> block_array;
        __method_reader->each_xil_block([&block_array](method_xil_block_t & block) {
            block_array.push_back(block);
        });

        // Read xils
        std::vector<const xil_base_t *> xils;
        __method_reader->each_xil([&xils](const xil_base_t * xil) {
            xils.push_back(xil);
        });

        // The profile of the commands (APRIL_EXEC_PROFILE) wants a command for each xil: no superinstructions then.
        const bool profile = exec_profile_enabled();

        if (exec_call_stats_on && !fast && __exec_method != nullptr)
        {
            bool has_switch = false;
            for (const xil_base_t * xil : xils)
            {
                if (command_of(xil) == xil_command_t::jmp && ((const jmp_xil_t *)xil)->model() == xil_jmp_model_t::switch_)
                    has_switch = true;
            }

            exec_call_stats_register(*__creating_ctx, __exec_method, xils.size(), !block_array.empty(), has_switch);
        }

        // The accessor of a property (it reads or writes a field of this): a call of it needs no frame.
        if (!fast && !profile && __exec_method != nullptr)
        {
            analyze_trivial_method(*__creating_ctx, __exec_method, !((decorate_value_t)(*__method)->decorate).is_static,
                                   xils.data(), xils.size());
        }

        // The calls of small methods are put in the xils (the callees are read with their own contexts: the library is the same).
        inline_state_t inline_state;
        if (!profile && __exec_method != nullptr && __inline_enabled())
            __expand_calls(xils, block_array, inline_state);

        const bool inlined = !inline_state.sites.empty();

        auto context_of = [&](size_t i) -> command_creating_context_t & {
            return inlined && inline_state.contexts[i] != nullptr? *inline_state.contexts[i] : *__creating_ctx;
        };

        auto parse_begin = std::chrono::steady_clock::now();

        // The superinstructions: a plan that tells the xils that are one command. A method that has a loop gets them at once (a call
        // of it can run for long), a method that has none after it was called many times (the first command counts the calls), when it
        // has no protected block (the blocks and the commands that were run are not those of the new commands).
        const fusion_mode_t mode = profile? fusion_mode_t::off : fusion_mode();
        const fusion_plan_t * plan = nullptr;
        bool fused = false, tiered = false;

        if (mode != fusion_mode_t::off || fast)
        {
            bool has_loop = false;
            plan = fusion_plan_of(xils, __creating_ctx->switch_manager, block_array, &has_loop,
                                  inlined? &inline_state.cuts : nullptr);

            if (!plan->groups.empty())
            {
                if (fast || mode == fusion_mode_t::eager || plan->has_loop)
                    fused = true;
                else if (block_array.empty() && __exec_method != nullptr)
                    tiered = true;
            }
        }

        if (fast && !fused)
            return nullptr;

        if (fast && !block_array.empty())
            return nullptr;

        al::svector_t<command_t *, 256> commands;

        if (fused)
        {
            // The switch tables go to commands.
            std::set<exec_switch_table_t *> remapped;
            for (size_t i = 0; i < xils.size(); i++)
            {
                if (command_of(xils[i]) != xil_command_t::jmp || ((const jmp_xil_t *)xils[i])->model() != xil_jmp_model_t::switch_)
                    continue;

                exec_switch_table_t * table = __creating_ctx->switch_manager.get_table(((const jmp_xil_t *)xils[i])->tbl());
                if (table == nullptr || !remapped.insert(table).second)
                    continue;

                auto remap = [&](int32_t step) -> int32_t {
                    return (int32_t)plan->index_map[i + step] - (int32_t)plan->index_map[i];
                };

                for (int k = 0; k < table->row_count; k++)
                    table->rows[k].step = remap(table->rows[k].step);

                table->default_step = remap(table->default_step);
            }

            __creating_ctx->plan = plan;
            for (inline_site_t & site : inline_state.sites)
                site.ctx->plan = plan;

            size_t group_index = 0;
            for (size_t i = 0; i < xils.size();)
            {
                command_creating_context_t & ctx = context_of(i);
                ctx.xil_index = (uint32_t)i;

                command_t * command;
                if (group_index < plan->groups.size() && plan->groups[group_index].start == i)
                {
                    const fusion_group_t & group = plan->groups[group_index++];
                    command = new_super_command(ctx, *plan, xils.data(), group);
                    i += group.length;
                }
                else if (inlined && inline_state.enters[i] != nullptr)
                {
                    command = new_inline_enter_command(*inline_state.enters[i]);
                    i++;
                }
                else
                {
                    command = new_command(ctx, xils[i]);
                    i++;
                }

                _A(command != nullptr);
                commands.push_back(command);
            }

            _A(commands.size() == plan->command_count());
        }
        else
        {
            for (size_t i = 0; i < xils.size(); i++)
            {
                command_t * command = inlined && inline_state.enters[i] != nullptr?
                    new_inline_enter_command(*inline_state.enters[i]) : new_command(context_of(i), xils[i]);

                _A(command != nullptr);
                commands.push_back(command);

                #if EXEC_TRACE

                _PFC(dark_gray, _T("%1%  %2%"), __LogPrefix, to_command_string(command));

                #endif  // EXEC_TRACE
            }
        }

        // Copies to a array: the command that counts the calls is the first of a method that gets its superinstructions later.
        const size_t base = tiered? 1 : 0;
        command_t ** command_arr = __env.new_commands(commands.size() + base);

        if (tiered)
            command_arr[0] = new_tier_command(__exec_method, fusion_threshold());

        std::copy(commands.begin(), commands.end(), command_arr + base);
        command_arr += base;

        // The index of a command of an xil.
        auto command_index = [&](uint32_t xil_index) -> size_t {
            return fused? plan->index_map[xil_index] : xil_index;
        };

        // Converts xil blocks.
        exec_method_block_manager_t * block_manager = nullptr;
        if (!block_array.empty())
        {
            block_manager = memory_t::new_obj<exec_method_block_manager_t>(__env.memory);
            block_manager->count = block_array.size();
            block_manager->blocks = __env.new_blocks(block_manager->count);

            exec_method_block_t * p_block = block_manager->blocks;

            for (method_xil_block_t & block : block_array)
            {
                #define __CommandAt(_index) (                                           \
                    _index < xils.size()? command_arr + command_index(_index) :         \
                        (throw _ED(__e_t::xil_block_index_out_of_range), nullptr)       \
                    )

                p_block->start       = __CommandAt(block.xil_start);
                p_block->end         = __CommandAt(block.xil_end);
                p_block->entry_point = __CommandAt(block.entry_point);
                p_block->type        = block.type;
                p_block->parent      = nullptr;

                // _PF(_T("BLOCK [%1%] %2% %3% %4%"),
                //     block.type, block.xil_start, block.xil_end, block.entry_point);

                if (block.relation_type != ref_t::null)
                {
                    p_block->relation_type = __analyzer->get_type(block.relation_type);
                    if (p_block->relation_type == nullptr)
                        throw _ED(__e_t::type_not_found, block.relation_type);
                }
                else
                {
                    p_block->relation_type = nullptr;
                }

                p_block++;

                #undef __CommandAt
            }

            for (exec_method_block_t * p_block = block_manager->blocks,
                    * p_block_end = p_block + block_manager->count;
                    p_block < p_block_end; p_block++)
            {
                for (exec_method_block_t * p_block1 = p_block + 1;
                        p_block1 < p_block_end; p_block1++)
                {
                    if (p_block1->include(p_block))
                    {
                        p_block->parent = p_block1;
                        break;
                    }
                }
            }
        }

        if (!fast)
            __exec_method->block_manager = block_manager;

        if (profile)
        {
            auto parse_end = std::chrono::steady_clock::now();

            bool has_loop = false;
            auto cut_begin = std::chrono::steady_clock::now();
            std::vector<uint8_t> cuts = exec_cut_points(xils, __creating_ctx->switch_manager, block_array, &has_loop);
            auto cut_end = std::chrono::steady_clock::now();

            std::vector<string_t> shapes;
            for (const xil_base_t * profile_xil : xils)
                shapes.push_back(exec_xil_shape(profile_xil));

            uint64_t * counters = exec_profile_register(
                __exec_method->get_name(), std::move(shapes), std::move(cuts), has_loop,
                (uint64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(parse_end - parse_begin).count(),
                (uint64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(cut_end - cut_begin).count(),
                std::vector<const xil_base_t *>(xils)
            );

            for (size_t k = 0; k < commands.size(); k++)
                command_arr[k] = new_profile_command(command_arr[k], counters + k);
        }

        // The array that is given is that of the commands, the command that counts the calls is before it.
        return tiered? command_arr - 1 : command_arr;
    }

    // Parse method info.
    void method_parser_t::__parse_dynamic_info()
    {
        // Read local variables
        uint16_t ref_objects = 0;

        locals_layout_t & locals_layout = __creating_ctx->locals_layout;
        /*
        __method_reader->each_variable([&locals_layout](local_variable_defination_t & def) {
            locals_layout.append(def);
        });
        locals_layout.commit();
        */

        // Read params variables
        params_layout_t & params_layout = __creating_ctx->params_layout;
        if (!__dynamic_method->decorate.is_static)
            params_layout.append(__host_type, param_type_t::default_, param_layout_type_t::this_);

        for (rt_dynamic_param_t * param = __dynamic_method->params,
                * param_end = param + __dynamic_method->param_count;
             param < param_end; param++)
        {
            params_layout.append(param->type, param->param_type);
        }

        params_layout.commit();
        msize_t this_offset = params_layout.offset_of(0);

        // Read switch tables
        /*
        exec_switch_manager_t & switch_manager = __creating_ctx->switch_manager;
        __method_reader->each_switch_table([&switch_manager](exec_switch_table_t * tbl) {
            switch_manager.append_table(tbl);
        });
        */

        __exec_method->ref_objects = ref_objects;
        __exec_method->stack_unit_size = locals_layout.unit_size();
        __exec_method->this_offset = this_offset;
    }

    // Parse dynamic commands.
    command_t ** method_parser_t::__parse_dynamic_commands()
    {
        _A(__dynamic_method_reader != nullptr);

        al::svector_t<command_t *, 256> commands;

        rt_command_t * rt_command;
        while((rt_command = __dynamic_method_reader->read_next()) != nullptr)
        {
            command_t * command = new_command(*__creating_ctx, rt_command);
            _A(command != nullptr);

            commands.push_back(command);
        }

        // Copies to a array.
        command_t ** command_arr = __env.new_commands(commands.size());
        std::copy(commands.begin(), commands.end(), command_arr);

        return command_arr;
    }

    // Deconstructor.
    method_parser_t::~method_parser_t()
    {
        // Empty.
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::exec

