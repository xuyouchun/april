
#include <compile.h>

namespace X_ROOT_NS::modules::compile {

    using namespace core;

    ////////// ////////// ////////// ////////// //////////

    // Finds a marker in the code, what is not code is skipped.
    bool lang_service_base_t::find_marker(const char_t * code, size_t length, size_t start,
            const char_t * const * markers, size_t marker_count,
            size_t & position, size_t & marker_index)
    {
        size_t i = start;
        while (i < length)
        {
            size_t next = skip_non_code(code, length, i);
            if (next != i)
            {
                i = next;
                continue;
            }

            for (size_t k = 0; k < marker_count; k++)
            {
                const char_t * marker = markers[k];
                size_t n = std::char_traits<char_t>::length(marker);

                if (i + n <= length && std::equal(marker, marker + n, code + i))
                {
                    position = i, marker_index = k;
                    return true;
                }
            }

            i++;
        }

        return false;
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns the service of the type: the service of the language has all of them.
    lang_service_t * lang_base_t::get_service(lang_service_type_t service_type)
    {
        typedef lang_service_type_t t;

        switch (service_type)
        {
            case t::token_read:         return (lang_token_read_service_t *)__service;
            case t::token_property:     return (lang_token_property_service_t *)__service;
            case t::operator_property:  return (lang_operator_property_service_t *)__service;
            case t::expression_build:   return (lang_expression_build_service_t *)__service;
            case t::statement_analyze:  return (lang_statement_analyze_service_t *)__service;
            case t::ast_build:          return (lang_ast_build_service_t *)__service;
            case t::analyze:            return (lang_analyze_service_t *)__service;
            case t::marker:             return (lang_marker_service_t *)__service;
            default:                    return nullptr;
        }
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::compile
