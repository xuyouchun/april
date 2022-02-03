#include <exec.h>

namespace X_ROOT_NS::modules::exec {

    using namespace core;
    using namespace rt;

    ////////// ////////// ////////// ////////// //////////

    // Returns runtime string for specified string.
    rt_string_t * rt_string_pool_t::get(const char_t * s, rt_length_t length)
    {
        auto it = __map.find(__string_key_t(s, length));
        if (it != __map.end())
            return it->second;

        rt_string_t * rt_str = __heap.new_obj(s, length);
        __map[__string_key_t(s, length)] = rt_str;

        return rt_str;
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::exec

