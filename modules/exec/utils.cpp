
#include "utils.h"
#include "commands.h"
#include "exec.h"

namespace X_ROOT_NS { namespace modules { namespace exec {

    using namespace core;
    using namespace rt;

    typedef exec_error_code_t __e_t;

    ////////// ////////// ////////// ////////// //////////

    rt_assembly_t * get_assembly(rt_assemblies_t & assemblies,
                                    const string_t & package, const string_t & name)
    {
        _A(name.length() > 0);

        rt_assembly_t * assembly = assemblies.get(package, name);
        if(assembly == nullptr)
            throw _ED(exec_error_code_t::assembly_not_found, join_assembly_name(package, name));

        return assembly;
    }

    ////////// ////////// ////////// ////////// //////////
    // exec_switch_manager_t

    void exec_switch_manager_t::append_table(exec_switch_table_t * tbl)
    {
        _A(tbl != nullptr);
        __tables.push_back(tbl);
    }

    exec_switch_table_t * exec_switch_manager_t::get_table(int index)
    {
        if(index >= __tables.size())
            throw _ED(__e_t::switch_table_index_overflow);

        return __tables[index];
    }

    ////////// ////////// ////////// ////////// //////////

} } }

