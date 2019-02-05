
#ifndef __LANG_CS_WORD_TYPE_H__
#define __LANG_CS_WORD_TYPE_H__

namespace X_ROOT_NS { namespace modules { namespace lang_cs {

    ////////// ////////// ////////// ////////// //////////

    // Word Types
    X_ENUM_(cs_token_value_t, sizeof(int16_t))

        //-------- ---------- ---------- ---------- ----------
        __keyword_begin,

        // keywords: a...
		k_abstract,	    k_as,       k_add,
        
        // keywords: b...
        k_base,         k_bool,	    k_break,    k_byte,
        
        // keywords: c...
        k_case, 		k_catch, 	k_char, 	k_checked,  k_class,
        k_const,        k_continue,
        
        // keywords: d...
        k_decimal,      k_default,  k_delegate, k_do,       k_double,
        
        // keywords: e...
        k_else,         k_enum,     k_event,    k_explicit, k_extern,
        
        // keywords: f...
        k_false,        k_finally,  k_fixed,    k_float,    k_for,
        k_foreach,      k_from,
        
        // keywords: g...
        k_goto,         k_get,      k_global,

        // keywords: i...
		k_if,           k_implicit, k_in,       k_int,      k_interface,
		k_internal,     k_is,       k_import,
        
        // keywords: l...
        k_lock,         k_long,
        
        // keywords: n...
        k_namespace,    k_new,      k_null,
        
        // keywords: o...
        k_object,       k_operator, k_out,      k_override,
        
        // keywords: p...
        k_params,       k_private,  k_protected, k_public,

        // keywords: r...
		k_readonly,     k_ref,      k_return,   k_remove,
        
        // keywords: s...
        k_sbyte,        k_sealed,   k_short,    k_sizeof,   k_stackalloc,
        k_static,       k_string,   k_struct,   k_switch,   k_set,
        
        // keywords: t...
        k_this,         k_throw,    k_true,     k_try,      k_typeof,   k_typedef,
        
        // keywords: u...
        k_uint,         k_ulong,    k_unchecked, k_unsafe,  k_ushort,
        k_using,
        
        // keywords: v...
        k_virtual,      k_void,     k_volatile, k_var,
        
        // keywords: w...
        k_while,        k_where,

        __keyword_end,
        //-------- ---------- ---------- ---------- ----------
        __operator_begin,

        // algorithm
        add,            sub,        mul,        div,        mod,            // + - * / %
        add_assign,     sub_assign, mul_assign, div_assign, mod_assign,     // += -= *= /= %=
        left_increment, left_decrement, right_increment, right_decrement,   // ++ --
        minus,          positive,                                           // + -
        left_shift,     right_shift, left_shift_assign, right_shift_assign, // << >> <<= >>=
        bit_and,        bit_or,     bit_not,    bit_xor,                    // & | ~ ^
        bit_and_assign, bit_or_assign, bit_xor_assign,                      // &= |= ^=
        assign,                                                             // =

        //
        greater,        greater_equal,  less,   less_equal,     // > >= < <=
        equal,          not_equal,                              // == !=

        // logic
        logic_and,      logic_or,       logic_not,              // && || !

        // others
        member_point,                           // .
        question_mark,  question_colon,         // ? :
        comma,          double_colon,           // , ::

        __operator_end,
        //-------- ---------- ---------- ---------- ----------
        __bracket_begin,

        // brackets
        left_bracket,   right_bracket,  // ( )
        left_square,    right_square,   // [ ]
        left_angle,     right_angle,    // < >
        left_brace,     right_brace,    // { }

        __bracket_end,

        //-------- ---------- ---------- ---------- ----------

        // others ...
        name,           cvalue,     comment,
        semicolon,      base_mark,              // ; :
        dollar,         ampersand,              // $ @

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    // Cs operators.
    X_ENUM_(cs_operator_t, sizeof(int16_t))

        // ?
        question_mark = (int)core::operator_t::__user_defined__,

        // :
        question_colon,

        // as
        as,
        
        // is
        is,

        // ,
        comma,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

} } }  // X_ROOT_NS::modules::lang_cs

#endif  // __LANG_CS_WORD_TYPE_H__
