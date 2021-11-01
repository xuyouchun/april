#include "parser.h"
#include "commands.h"
#include "utils.h"

namespace X_ROOT_NS { namespace modules { namespace exec {

    typedef exec_error_code_t __e_t;
    using namespace core;

    ////////// ////////// ////////// ////////// //////////

    // Method reader.
    class method_reader_t : public object_t
    {
    public:

        // Constructor.
        method_reader_t(memory_t * memory, const byte_t * bytes, size_t length)
            : __memory(memory), __bytes(bytes), __bytes_end(bytes + length)
        { }

        // Enums variables.
        template<typename f_t> void each_variable(f_t f)
        {
            method_variable_stub_t * stub = __read_stub<method_variable_stub_t>();
            if (stub == nullptr)
                return;

            int variable_count = stub->count();
            for (int k = 0; k < variable_count; k++)
            {
                local_variable_defination_t def = {
                    __read<ref_t>(), __read<uint16_t>()
                };

                f(def);
            }
        }

        // Enums switch tables.
        template<typename f_t> void each_switch_table(f_t f)
        {
            method_switch_table_stub_t * stub = __read_stub<method_switch_table_stub_t>();
            if (stub == nullptr)
                return;

            int tbl_count = stub->count();

            for (int k = 0; k < tbl_count; k++)
            {
                switch_table_header_t header = __read<switch_table_header_t>();

                exec_switch_table_t * tbl = new (__memory->alloc(
                    sizeof(exec_switch_table_t) + sizeof(exec_switch_row_t) * header.count
                )) exec_switch_table_t(header.count, __read<int32_t>());

                exec_switch_row_t * rows = (exec_switch_row_t *)tbl->rows;
                for (int row_idx = 0, row_count =  header.count; row_idx < row_count; row_idx++)
                {
                    *rows++ = __read<exec_switch_row_t>();
                }

                f(tbl);
            }
        }

        // Enums xil blocks.
        template<typename f_t> void each_xil_block(f_t f)
        {
            method_xil_block_stub_t * stub = __read_stub<method_xil_block_stub_t>();
            if (stub == nullptr)
                return;

            int tbl_count = stub->count();

            for (int k = 0; k < tbl_count; k++)
            {
                method_xil_block_t block = __read<method_xil_block_t>();
                f(block);
            }
        }

        // Enums all xils.
        template<typename f_t> void each_xil(f_t f)
        {
            method_xil_stub_t * stub = __read_stub<method_xil_stub_t>();
            if (stub == nullptr)
                return;

            xil_reader_t reader(__bytes, __bytes_end);

            const xil_base_t * xil;
            while (reader.read(&xil))
            {
                f(xil);
            }
        }

        X_TO_STRING_IMPL(_T("method_reader_t"))

    private:
        memory_t * __memory;                        // Memory management.
        const byte_t * __bytes, * __bytes_end;      // Buffer.

        // Reads next entity.
        template<typename t> t __read()
        {
            if (__bytes_end - __bytes < sizeof(t))
                throw _EC(overflow, _T("read method body overflow"));

            t value = *(t *)__bytes;
            __bytes += sizeof(t);
            return value;
        }

        // Reads next stub.
        template<typename _stub_t> _stub_t * __read_stub()
        {
            if (__bytes >= __bytes_end)
                return nullptr;

            method_stub_t * stub = (method_stub_t *)__bytes;
            if (stub->type() == _stub_t::__type__)
            {
                __bytes += sizeof(method_stub_t);
                return (_stub_t *)stub;
            }

            return nullptr;
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // Parses commands.
    exec_method_t * parse_commands(executor_env_t & env, rt_method_t * method,
            rt_type_t * host_type, const generic_param_manager_t * gp_manager)
    {
        _A(method != nullptr);

        if (host_type == nullptr)
            host_type = method->get_host_type();

        _A(host_type != nullptr);

        assembly_analyzer_t analyzer = to_analyzer(env, host_type, gp_manager);

        const char_t * prefix = _T("* ");

        #if EXEC_TRACE
        // _PF(_T("parse_commands: %1%.%2%"), host_type->get_name(env), analyzer.get_name(method));
        _PF(_T("\n\033[01;30m%1% %2%.%3%\033[0m"), prefix, 
            host_type->get_name(env), analyzer.get_name(method));
        #endif  // EXEC_TRACE

        rt_assembly_t * assembly = host_type->get_assembly();

        rt_bytes_t body = assembly->get_method_body(method);
        memory_t * memory = env.get_memory();

        command_creating_context_t creating_ctx(env, assembly, host_type, method, gp_manager);
        method_reader_t reader(memory, body.bytes, body.length);

        // Read local variables
        uint16_t ref_objects = 0;

        locals_layout_t & locals_layout = creating_ctx.locals_layout;
        reader.each_variable([&](local_variable_defination_t & def) {
            locals_layout.append(def);
        });
        locals_layout.commit();

        // _P((string_t)locals_layout);

        // Read params variables
        params_layout_t & params_layout = creating_ctx.params_layout;
        if (!((decorate_value_t)(*method)->decorate).is_static)
            params_layout.append(host_type, param_type_t::default_, param_layout_type_t::this_);

        assembly->each_params((*method)->params, [&](int index, mt_param_t & mt_param) {

            // _PP(assembly->to_sid(mt_param.name));
            if (mt_param.param_type == param_type_t::extends)
            {
                ref_t ref = (*method)->params.at(index);
                analyzer.each_extend_params(ref, [&](rt_type_t * type) {
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

        // _P( (string_t)params_layout );

        // Read switch tables
        reader.each_switch_table([&](exec_switch_table_t * tbl) {
            creating_ctx.switch_manager.append_table(tbl);
        });

        // Read xil blocks.
        al::svector_t<method_xil_block_t, 10> block_array;
        reader.each_xil_block([&](method_xil_block_t & block) {
            block_array.push_back(block);
        });

        // Read xils
        al::svector_t<command_t *, 128> commands;

        const xil_base_t * xil;
        reader.each_xil([&](const xil_base_t * xil) {
            command_t * command = new_command(creating_ctx, xil);
            _A(command != nullptr);
            commands.push_back(command);

            #if EXEC_TRACE

            _PF(_T("\033[01;30m%1%  %2%\033[0m"), prefix, to_command_string(command));

            #endif  // EXEC_TRACE
        });

        // Copies to a array.
        command_t ** command_arr = env.new_commands(commands.size());

        std::copy(commands.begin(), commands.end(), command_arr);

        // Converts xil blocks.
        exec_method_block_manager_t * block_manager = nullptr;
        if (!block_array.empty())
        {
            block_manager = memory_t::new_obj<exec_method_block_manager_t>(memory);
            block_manager->count = block_array.size();
            block_manager->blocks = env.new_blocks(block_manager->count);

            exec_method_block_t * p_block = block_manager->blocks;

            for (method_xil_block_t & block : block_array)
            {
                #define __CommandAt(_index) (                                       \
                    _index < commands.size()? command_arr + _index :                \
                        (throw _ED(__e_t::xil_block_index_out_of_range), nullptr)   \
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
                    p_block->relation_type = analyzer.get_type(block.relation_type);
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

        exec_method_t * exec_method = memory_t::new_obj<exec_method_t>(memory,
            command_arr, ref_objects, locals_layout.unit_size()
        );

        exec_method->block_manager = block_manager;

        #if EXEC_TRACE
        _PF(_T("\033[01;30m%1% End of %2%.%3%\033[0m\n"), prefix,
                host_type->get_name(env), analyzer.get_name(method));
        #endif

        return exec_method;
    }

    ////////// ////////// ////////// ////////// //////////

} } }

