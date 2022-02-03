
#ifndef __ALGORITHM_NUMERIC_H__
#define __ALGORITHM_NUMERIC_H__


namespace X_ROOT_NS::algorithm {

    ////////// ////////// ////////// ////////// //////////

    // Numeric paring error codes.
    X_ENUM(parse_numeric_error_code_t)

        // Success
        success,

        // Format error
        format_error,

        // Overflow
        overflow,

    X_ENUM_END

    // Parses a string to a number.
    tvalue_t parse_numeric(cptr_t<char_t> &p);

    // Tries to parse a string to a number.
    parse_numeric_error_code_t try_parse_numeric(
            cptr_t<char_t> &p, tvalue_t * out_value = nullptr);

    // Parses a string to a number.
    tvalue_t parse_numeric(const char_t * s);

    // Tries to parse a string to a number.
    parse_numeric_error_code_t try_parse_numeric(
            const char_t * s, tvalue_t * out_value = nullptr);

    ////////// ////////// ////////// ////////// //////////

}  // X_ROOT_NS::algorithm

#endif  // __ALGORITHM_NUMERIC_H__
