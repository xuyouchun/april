#ifndef __EXEC_PARSER_H__
#define __EXEC_PARSER_H__

#include <exec.h>
#include "utils.h"
#include "commands.h"

namespace X_ROOT_NS::modules::exec {

    ////////// ////////// ////////// ////////// //////////

    // Method reader.
    class method_reader_t : public object_t
    {
    public:

        // Constructor.
        method_reader_t(const byte_t * bytes, size_t length, memory_t * memory)
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

    //-------- ---------- ---------- ---------- ----------

    // Method parser.
    class method_parser_t : public object_t
    {
    public:
        // Constructors.
        method_parser_t(exec_method_t * exec_method, executor_env_t & env, rt_method_t * method);
        method_parser_t(exec_method_t * exec_method, executor_env_t & env,
                                                            rt_generic_method_t * generic_method);

        // Initialize.
        void initialize();

        // Parse this method.
        command_t ** parse_commands();

        // Deconstructor.
        virtual ~method_parser_t() override;

    private:

        void __initialize();                // Initialize.
        void __parse_info();                // Parse method info.
        command_t ** __parse_commands();    // Parse commands.

        executor_env_t &            __env;
        assembly_analyzer_t *       __analyzer;
        rt_method_t *               __method;
        rt_method_base_t *          __raw_method;
        rt_type_t *                 __host_type = nullptr;
        rt_assembly_t *             __assembly;
        rt_bytes_t                  __body;

        generic_param_manager_t *    __gp_manager = nullptr;
        command_creating_context_t * __creating_ctx;
        method_reader_t            * __method_reader;
        exec_method_t *             __exec_method;

        pool_t                      __pool;
    };

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::exec

#endif // __EXEC_PARSER_H__
