
#ifndef __ALGORITHM_NUMERIC_H__
#define __ALGORITHM_NUMERIC_H__


namespace X_ROOT_NS { namespace algorithm {

    ////////// ////////// ////////// ////////// //////////

    //enum class parse_numeric_error_code_t
    X_ENUM(parse_numeric_error_code_t)

        success,

        format_error,

        overflow,

    X_ENUM_END

    tvalue_t parse_numeric(cptr_t<char_t> &p);

    parse_numeric_error_code_t try_parse_numeric(
            cptr_t<char_t> &p, tvalue_t * out_value=nullptr);

    ////////// ////////// ////////// ////////// //////////

} }  // X_ROOT_NS::algorithm

#endif  // __ALGORITHM_NUMERIC_H__
