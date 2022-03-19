#include "parser.h"
#include "commands.h"
#include "utils.h"
#include <lib.h>

namespace X_ROOT_NS::modules::exec {

    typedef exec_error_code_t __e_t;
    using namespace core;

    ////////// ////////// ////////// ////////// //////////
    // Method parser.

    // Constructor.
    method_parser_t::method_parser_t(exec_method_t * exec_method, executor_env_t & env,
                                                                    rt_method_t * method)
        : __exec_method(exec_method), __env(env), __method(method)
    {
        _A(exec_method != nullptr);
        _A(method != nullptr);

        __host_type = method->get_host_type();
        _A(__host_type != nullptr);

        __analyzer = __pool.new_obj<assembly_analyzer_t>(to_analyzer(env, __host_type));

        __initialize();
    }

    #define __LogPrefix    _T("* ")

    // Constructor.
    method_parser_t::method_parser_t(exec_method_t * exec_method, executor_env_t & env,
                                                            rt_generic_method_t * generic_method)
        : __exec_method(exec_method), __env(env)
    {
        _A(exec_method != nullptr);
        _A(generic_method != nullptr);

        __method = generic_method->template_;
        _A(__method != nullptr);

        assembly_analyzer_t analyzer = to_analyzer(env, __method->get_host_type());
        __gp_manager = __pool.new_obj<generic_param_manager_t>(analyzer, generic_method);

        __host_type = generic_method->get_host_type();

        __analyzer = __pool.new_obj<assembly_analyzer_t>(
            to_analyzer(env, __host_type, __gp_manager)
        );

        __initialize();
    }

    void method_parser_t::__initialize()
    {
        __assembly = __host_type->get_assembly();
        _A(__assembly != nullptr);

        __body = __assembly->get_method_body(__method);
        __creating_ctx = __pool.new_obj<command_creating_context_t>(
            __env, __assembly, __host_type, __method, __gp_manager
        );

        __method_reader = __pool.new_obj<method_reader_t>(__body.bytes, __body.length, __env.memory);
    }

    // Initialize.
    void method_parser_t::initialize()
    {
        this->__parse_info();
    }

    // Parse this method.
    command_t ** method_parser_t::parse_commands()
    {
        #if EXEC_TRACE
        // _PF(_T("parse_commands: %1%.%2%"), host_type->get_name(env), analyzer.get_name(method));

        _PFC(dark_gray, _T("\n%1% %2%.%3%"), __LogPrefix,
            __host_type->get_name(__env), __analyzer->get_name(__method)
        );
        #endif  // EXEC_TRACE

        command_t ** commands = this->__parse_commands();

        #if EXEC_TRACE

        _PFC(dark_gray, _T("%1% End of %2%.%3%\n"), __LogPrefix,
            __host_type->get_name(__env), __analyzer->get_name(__method)
        );

        #endif

        return commands;
    }

    // Parse method info.
    void method_parser_t::__parse_info()
    {
        // Read local variables
        uint16_t ref_objects = 0;

        locals_layout_t & locals_layout = __creating_ctx->locals_layout;
        __method_reader->each_variable([&locals_layout](local_variable_defination_t & def) {
            locals_layout.append(def);
        });
        locals_layout.commit();

        // _P((string_t)locals_layout);

        // Read params variables
        params_layout_t & params_layout = __creating_ctx->params_layout;
        if (!((decorate_value_t)(*__method)->decorate).is_static)
            params_layout.append(__host_type, param_type_t::default_, param_layout_type_t::this_);

        __assembly->each_params((*__method)->params, [&](int index, mt_param_t & mt_param) {

            // _PP(assembly->to_sid(mt_param.name));
            if (mt_param.param_type == param_type_t::extends)
            {
                ref_t ref = (*__method)->params.at(index);
                __analyzer->each_extend_params(ref, [&](rt_type_t * type) {
                    // _PP(type->get_name(analyzer.env));
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

        // _P( (string_t)params_layout );

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
    command_t ** method_parser_t::__parse_commands()
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

    // Deconstructor.
    method_parser_t::~method_parser_t()
    {
        // Empty.
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::exec

