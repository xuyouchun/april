#include <rt.h>
#include "utils.h"

namespace X_ROOT_NS::modules::rt {

    ////////// ////////// ////////// ////////// //////////

    // Constructor.
    rt_dynamic_method_body_builder_t::rt_dynamic_method_body_builder_t()
    {
        // Do nothing.
    }

    // Builds method body and returns it.
    rt_dynamic_method_body_t * rt_dynamic_method_body_builder_t::to_method_body(memory_t * memory)
    {
        rt_dynamic_method_body_t * body = memory_t::new_obj<rt_dynamic_method_body_t>(memory);
        byte_t * bytes = (byte_t *)memory_t::alloc(memory, __total_size), * p = bytes;

        for (std::tuple<rt_command_t *, size_t> & it : __commands)
        {
            rt_command_t * command = std::get<rt_command_t *>(it);
            size_t size = std::get<size_t>(it);

            _A(size < max_value<byte_t>());
            *p++ = (byte_t)size;

            al::quick_copy((void *)p, (void *)command, size);

            p += size;
        }

        _A(p - bytes == __total_size);

        body->commands = rt_bytes_t { .bytes = bytes, .length = __total_size };
        return body;
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::rt
