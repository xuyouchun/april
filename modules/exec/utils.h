
#ifndef __EXEC_UTILS_H__
#define __EXEC_UTILS_H__

#include <exec.h>

namespace X_ROOT_NS::modules::exec {

    ////////// ////////// ////////// ////////// //////////
    // exec_switch_manager_t

    struct exec_switch_row_t
    {
        int32_t value;
        int32_t step;
    };

    //-------- ---------- ---------- ---------- ----------

    struct exec_switch_table_t
    {
        exec_switch_table_t() = default;
        exec_switch_table_t(int16_t row_count, int32_t default_step)
            : row_count(row_count), default_step(default_step)
        { }

        int16_t row_count;
        int32_t default_step;

        exec_switch_row_t rows[0];
    };

    //-------- ---------- ---------- ---------- ----------

    rt_assembly_t * get_assembly(exec_assemblies_t & assemblies,
                                    const string_t & package, const string_t & name);

    string_t join_method_name(const string_t & assembly,
        const string_t & type_name, int type_generic_param_count,
        const string_t & method_name, int generic_param_count,
        const string_t & return_type, int param_count, const string_t * params);

    ////////// ////////// ////////// ////////// //////////

    class exec_switch_manager_t : public object_t, public no_copy_ctor_t
    {
    public:
        void append_table(exec_switch_table_t * tbl);
        exec_switch_table_t * get_table(int index);

        X_TO_STRING_IMPL(_T("exec_switch_manager_t"))

    private:
        al::svector_t<exec_switch_table_t *, 2> __tables;
    };

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::exec

#endif  // __EXEC_UTILS_H__

