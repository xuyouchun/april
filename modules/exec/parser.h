#ifndef __EXEC_PARSER_H__
#define __EXEC_PARSER_H__

#include <exec.h>

namespace X_ROOT_NS { namespace modules { namespace exec {

    ////////// ////////// ////////// ////////// //////////

    exec_method_t * parse_commands(executor_env_t & env,
            rt_method_t * method, rt_type_t ** generic_types = nullptr);

    ////////// ////////// ////////// ////////// //////////

} } }

#endif // __EXEC_PARSER_H__
