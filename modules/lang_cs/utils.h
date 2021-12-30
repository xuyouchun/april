
#ifndef __LANG_CS_UTILS_H__
#define __LANG_CS_UTILS_H__

#include <lang_cs.h>
#include <algorithm.h>

namespace X_ROOT_NS { namespace modules { namespace lang_cs {

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        using namespace core;
    }

    ////////// ////////// ////////// ////////// //////////

    enum class __code_char_flag_t
    {
        name_prefix = (1 << 0),

        numeric_prefix = (1 << 1),
    };

    //-------- ---------- ---------- ---------- ----------

    struct __code_char_property_t
        : public al::flag_property_t<__code_char_property_t, char, __code_char_flag_t>
    {
        static void init(enumerator_t & e);
    };

    //-------- ---------- ---------- ---------- ----------

    typedef __code_char_property_t::properties_t<> __code_char_properties_t;
            
    extern const __code_char_properties_t __code_char_properties;

    //-------- ---------- ---------- ---------- ----------

    X_INLINE bool __char_has_flag(char_t c, __code_char_flag_t flag, bool default_value=false)
    {
        return (size_t)c >= __code_char_properties_t::size? default_value
                : __code_char_properties[(char)c].has_flag(flag);
    }

    X_INLINE bool is_name_prefix(char_t c)
    {
        return __char_has_flag(c, __code_char_flag_t::name_prefix);
    }

    X_INLINE bool is_numeric_prefix(char_t c)
    {
        return __char_has_flag(c, __code_char_flag_t::numeric_prefix);
    }

    ////////// ////////// ////////// ////////// //////////

    enum class __cs_token_flag_t
    {
        is_keyword  = (1 << 0),

        is_name     = (1 << 1),

        is_value    = (1 << 2),

        is_bracket  = (1 << 3),
    };

    //-------- ---------- ---------- ---------- ----------

    struct __cs_token_property_t
        : public al::flag_property_t<__cs_token_property_t, cs_token_value_t, 
                                                __cs_token_flag_t>
        , token_property_t
        , operator_property_t
        , expression_box_property_t
    {
        static void init(enumerator_t & e);
    };

    //-------- ---------- ---------- ---------- ----------

    typedef __cs_token_property_t::properties_t<> __cs_token_properties_t;

    extern const __cs_token_properties_t __cs_token_properties;

    ////////// ////////// ////////// ////////// //////////

    X_INLINE const __cs_token_property_t * get_token_property(cs_token_value_t value)
    {
        return &__cs_token_properties[value];
    }

    X_INLINE bool __token_has_flag(cs_token_value_t value, __cs_token_flag_t flag)
    {
        return __cs_token_properties[value].has_flag(flag);
    }

    X_INLINE bool is_operator(cs_token_value_t value)
    {
        return __cs_token_properties[value].is_operator;
    }

    const operator_property_t * get_operator_property(cs_token_value_t value);

    const string_t get_token_string(cs_token_value_t value);

    ////////// ////////// ////////// ////////// //////////

} } }  // X_ROOT_NS::modules::lang_cs

#endif  //__LANG_CS_UTILS_H__
