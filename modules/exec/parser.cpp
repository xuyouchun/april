#include "parser.h"
#include "commands.h"
#include "utils.h"
#include <lib.h>

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
                                rt_method_t * method)
        : __exec_method(exec_method), __env(env), __method(method), __raw_method(method)
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
                                                    rt_generic_method_t * generic_method)
        : __exec_method(exec_method), __env(env), __raw_method(generic_method)
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
                                                    rt_dynamic_method_t * dynamic_method)
        : __exec_method(exec_method), __env(env), __raw_method(dynamic_method)
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
        _A(body.bytes != nullptr);

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
    command_t ** method_parser_t::parse_commands()
    {
        if (__raw_method->this_type() == rt_member_type_t::dynamic)
            return __parse_dynamic_commands();

        return __parse_normal_commands();
    }

    // Parse normal method.
    command_t ** method_parser_t::__parse_normal_commands()
    {
        #if EXEC_TRACE
        // _PF(_T("parse_commands: %1%.%2%"), host_type->get_name(env), analyzer.get_name(method));

        string_t ss = __raw_method->to_string(__env, true);
        _PFC(dark_gray, _T("\n%1% %2%"), __LogPrefix, ss.c_str());

        #endif  // EXEC_TRACE

        command_t ** commands = this->__parse_normal_commands_();

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

        __exec_method->ref_objects = ref_objects;
        __exec_method->stack_unit_size = locals_layout.unit_size();
        __exec_method->this_offset = this_offset;
    }

    // Parse method commands.
    command_t ** method_parser_t::__parse_normal_commands_()
    {
        // Read xil blocks.
        al::svector_t<method_xil_block_t, 10> block_array;
        __method_reader->each_xil_block([&block_array](method_xil_block_t & block) {
            block_array.push_back(block);
        });

        // Read xils
        al::svector_t<command_t *, 256> commands;

        const xil_base_t * xil;
        __method_reader->each_xil([this, &commands](const xil_base_t * xil) {
            command_t * command = new_command(*__creating_ctx, xil);
            _A(command != nullptr);
            commands.push_back(command);

            #if EXEC_TRACE

            _PFC(dark_gray, _T("%1%  %2%"), __LogPrefix, to_command_string(command));

            #endif  // EXEC_TRACE
        });

        // Copies to a array.
        command_t ** command_arr = __env.new_commands(commands.size());

        std::copy(commands.begin(), commands.end(), command_arr);

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
                    _index < commands.size()? command_arr + _index :                    \
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

        __exec_method->block_manager = block_manager;
        return command_arr;
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

