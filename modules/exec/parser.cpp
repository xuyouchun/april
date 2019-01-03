#include "parser.h"
#include "commands.h"
#include "utils.h"

namespace X_ROOT_NS { namespace modules { namespace exec {

    typedef exec_error_code_t __e_t;
    using namespace core;

    ////////// ////////// ////////// ////////// //////////

    class method_reader_t : public object_t
    {
    public:
        method_reader_t(memory_t * memory, const byte_t * bytes, size_t length)
            : __memory(memory), __bytes(bytes), __bytes_end(bytes + length)
        { }

        template<typename f_t> void each_variable(f_t f)
        {
            method_variable_stub_t * stub = __read_stub<method_variable_stub_t>();
            if(stub == nullptr)
                return;

            int variable_count = stub->count();
            for(int k = 0; k < variable_count; k++)
            {
                local_variable_defination_t def = {
                    __read<ref_t>(), __read<uint16_t>()
                };

                f(def);
            }
        }

        template<typename f_t> void each_switch_table(f_t f)
        {
            method_switch_table_stub_t * stub = __read_stub<method_switch_table_stub_t>();
            if(stub == nullptr)
                return;

            int tbl_count = stub->count();

            for(int k = 0; k < tbl_count; k++)
            {
                switch_table_header_t header = __read<switch_table_header_t>();

                exec_switch_table_t * tbl = new (__memory->alloc(
                    sizeof(exec_switch_table_t) + sizeof(exec_switch_row_t) * header.count
                )) exec_switch_table_t(header.count, __read<int32_t>());

                exec_switch_row_t * rows = (exec_switch_row_t *)tbl->rows;
                for(int row_idx = 0, row_count =  header.count; row_idx < row_count; row_idx++)
                {
                    *rows++ = __read<exec_switch_row_t>();
                }

                f(tbl);
            }
        }

        template<typename f_t> void each_xil(f_t f)
        {
            method_xil_stub_t * stub = __read_stub<method_xil_stub_t>();
            if(stub == nullptr)
                return;

            xil_reader_t reader(__bytes, __bytes_end);

            const xil_base_t * xil;
            while(reader.read(&xil))
            {
                f(xil);
            }
        }

    private:
        memory_t * __memory;
        const byte_t * __bytes, * __bytes_end;

        template<typename t> t __read()
        {
            if(__bytes_end - __bytes < sizeof(t))
                throw _EC(overflow, _T("read method body overflow"));

            t value = *(t *)__bytes;
            __bytes += sizeof(t);
            return value;
        }

        template<typename _stub_t> _stub_t * __read_stub()
        {
            if(__bytes >= __bytes_end)
                return nullptr;

            method_stub_t * stub = (method_stub_t *)__bytes;
            if(stub->type() == _stub_t::__type__)
            {
                __bytes += sizeof(method_stub_t);
                return (_stub_t *)stub;
            }

            return nullptr;
        }
    };

    ////////// ////////// ////////// ////////// //////////

    exec_method_t * parse_commands(executor_env_t & env,
                    rt_assembly_t * assembly, rt_type_t * type, rt_method_t * method)
    {
        _A(assembly != nullptr);
        _A(type != nullptr);
        _A(method != nullptr);

        //_PF(_T("\nparse method: %1%.%2%"), assembly.get_name(type), assembly.get_name(method));

        rt_bytes_t body = assembly->get_method_body(method);
        memory_t * memory = env.get_memory();

        command_creating_context_t creating_ctx(env, assembly, method);
        method_reader_t reader(memory, body.bytes, body.length);

        // Read local variables
        uint16_t ref_objects = 0;

        locals_layout_t & locals_layout = creating_ctx.locals_layout;
        reader.each_variable([&](local_variable_defination_t & def) {
            locals_layout.append(def);
        });
        locals_layout.commit();

        // Read params variables
        params_layout_t & params_layout = creating_ctx.params_layout;
        if(!((decorate_value_t)(*method)->decorate).is_static)
            params_layout.append(type, param_type_t::__default__);

        assembly->each_params((*method)->params, [&](int index, mt_param_t & mt_param) {
            return params_layout.append(mt_param.type, mt_param.param_type), true;
        });
        params_layout.commit();

        // Read switch tables
        reader.each_switch_table([&](exec_switch_table_t * tbl) {
            creating_ctx.switch_manager.append_table(tbl);
        });

        // Read xils
        al::small_vector_t<command_t *, 128> commands;

        const xil_base_t * xil;
        reader.each_xil([&creating_ctx, &commands](const xil_base_t * xil) {
            command_t * command = new_command(creating_ctx, xil);
            _A(command != nullptr);
            commands.push_back(command);
        });

        command_t ** command_arr = memory_t::alloc_objs<command_t *>(memory, commands.size() + 1);
        std::copy(commands.begin(), commands.end(), command_arr);

        if(commands.size() == 0 || !is_ret_command(commands[commands.size() - 1]))
            command_arr[commands.size()] = new_ret_command(creating_ctx);

        return memory_t::new_obj<exec_method_t>(memory,
            command_arr, ref_objects, locals_layout.unit_size()
        );
    }

    ////////// ////////// ////////// ////////// //////////

} } }

