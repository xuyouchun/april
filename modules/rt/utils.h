
#ifndef __RT_UTILITS_H__
#define __RT_UTILITS_H__

#include <common.h>

namespace X_ROOT_NS::modules::rt {

    ////////// ////////// ////////// ////////// //////////

    #define __RtAssemblyError(_msg) _E(rt_error_code_t::assembly_error, _msg)
    #define __RtAssemblyErrorF(_format, _args ...)  __RtAssemblyError(_FT(_format, ##_args))

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::rt

#endif  // __RT_UTILITS_H__

