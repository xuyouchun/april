#ifndef __EXEC_PARSER_H__
#define __EXEC_PARSER_H__

#include <exec.h>

namespace X_ROOT_NS { namespace modules { namespace exec {

    ////////// ////////// ////////// ////////// //////////

    exec_method_t * parse_commands(executor_env_t & env,
        rt_assembly_t * assembly, rt_type_t * type, rt_method_t * method);

    ////////// ////////// ////////// ////////// //////////

} } }

#endif // __EXEC_PARSER_H__
