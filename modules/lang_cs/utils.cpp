
#include "utils.h"

namespace X_ROOT_NS::modules::lang_cs {

    using namespace core;
    using namespace compile;

    ////////// ////////// ////////// ////////// //////////

    // Initialize code char property.
    void __code_char_property_t::init(enumerator_t & e)
    {
        typedef __code_char_flag_t f_t;
        set_flag(e, 'a', 'z', f_t::name_prefix);
        set_flag(e, 'A', 'Z', f_t::name_prefix);
        set_flag(e, '_',      f_t::name_prefix);

        set_flag(e, '0', '9', f_t::numeric_prefix);
        set_flag(e, '.',      f_t::numeric_prefix);
    }

    const __code_char_properties_t __code_char_properties;

    ////////// ////////// ////////// ////////// //////////

    // Initialize token property.
    void __cs_token_property_t::init(enumerator_t & e)
    {
        using namespace std::placeholders;
        typedef cs_token_value_t w_t;

        // type
        set_flag(e, w_t::__keyword_begin,  w_t::__keyword_end, __cs_token_flag_t::is_keyword);
        set_flag(e, __cs_token_flag_t::is_keyword, w_t::k_as, w_t::k_is);
        set_flag(e, w_t::__bracket_begin,  w_t::__bracket_end, __cs_token_flag_t::is_bracket);
        set_flag(e, w_t::name,   __cs_token_flag_t::is_name);
        set_flag(e, w_t::cvalue, __cs_token_flag_t::is_value);

        auto set_operator = [](__cs_token_property_t & p, bool is_operator) {
            p.is_operator = is_operator;
        };

        e.each(std::bind(set_operator, _2, true), w_t::k_is, w_t::k_as);
        e.each(w_t::__operator_begin, w_t::__operator_end, std::bind(set_operator, _2, true));

        // operator

        #define __Op(op_) e[w_t::op_].op = operator_t::op_;
        #define __KOp(op_) e[w_t::k_##op_].op = operator_t::op_;

        __Op(add)   __Op(sub)   __Op(mul)   __Op(div)   __Op(mod)

        __Op(add_assign) __Op(sub_assign) __Op(mul_assign) __Op(div_assign) __Op(mod_assign)

        __Op(left_increment) __Op(left_decrement) __Op(right_increment) __Op(right_decrement)

        __Op(minus) __Op(positive)

        __Op(left_shift) __Op(right_shift) __Op(left_shift_assign) __Op(right_shift_assign)

        __Op(bit_and) __Op(bit_or) __Op(bit_not) __Op(bit_xor)

        __Op(bit_and_assign) __Op(bit_or_assign) __Op(bit_xor_assign)

        __Op(assign)

        __Op(greater)   __Op(greater_equal)  __Op(less)   __Op(less_equal)

        __Op(equal)     __Op(not_equal)

        __Op(logic_and) __Op(logic_or)       __Op(logic_not)

        __Op(member_point)

        __KOp(as)       __KOp(is)

        #define __CsOp_(_name, _w)                                                      \
            e[w_t::_w].op   = (operator_t)cs_operator_t::_name;                         \
            e[w_t::_w].name = _desc(cs_operator_t::_name);

        #define __CsOp(_name) __CsOp_(_name, _name)

        __CsOp(question_mark)   __CsOp(question_colon)
        __CsOp(comma)

        #define __Pri(_v) ((_v) << 8)

        // priority
        auto set_priority = [](__cs_token_property_t & p, operator_priority_t priority) {
            p.priority = priority;
        };

        e.each(std::bind(set_priority, _2, __Pri(13)),
            w_t::question_colon                                 // `:
        );

        e.each(std::bind(set_priority, _2, __Pri(13)),
            w_t::question_mark                                  // ?
        );

        e.each(std::bind(set_priority, _2, __Pri(15)),
            w_t::comma                                          // ,
        );

        // arity
        auto set_arity = [](__cs_token_property_t & p, operator_arity_t arity) {
            p.arity = arity;
        };

        e.each(std::bind(set_arity, _2, 2),
            w_t::comma,                                         // ,
            w_t::question_mark, w_t::question_colon
        );

        // adhere
        auto set_adhere = [](__cs_token_property_t & p, operator_adhere_t adhere) {
            p.adhere = adhere;
        };

        e.each(std::bind(set_adhere, _2, 1),
            w_t::comma,
            w_t::question_mark, w_t::question_colon
        );

        // direct
        auto set_direct = [](operator_property_t & p, operator_direct_t direct) {
            p.direct = direct;
        };

        e.each(std::bind(set_direct, _2, operator_direct_t::right_to_left), // ?
            w_t::question_mark, w_t::question_colon
        );

        // parent_op
        auto set_parent_op = [](operator_property_t & p, operator_t op) {
            p.parent_op = op;
        };

        e.each(std::bind(set_parent_op, _2, (operator_t)cs_operator_t::question_mark),
            w_t::question_colon
        );

        // expression box
        auto set_is_expression_box = [](__cs_token_property_t & p, bool is_expression_box) {
            p.is_expression_box = is_expression_box;
        };

        e.each(std::bind(set_is_expression_box, _2, true),
            w_t::left_bracket,      w_t::right_bracket,
            w_t::left_square,       w_t::right_square
        );

        auto set_expression_box_pair = [&e](w_t left, w_t right) {
            e[left].side  = expression_box_side_t::left;
            e[right].side = expression_box_side_t::right;

            e[right].pair_to = (token_value_t)left;
            e[left].pair_to  = (token_value_t)right;
        };

        set_expression_box_pair(w_t::left_bracket, w_t::right_bracket);
        set_expression_box_pair(w_t::left_square,  w_t::right_square);

        // invisible
        auto set_is_invisible = [](__cs_token_property_t & p, bool is_invisible) {
            p.is_invisible = is_invisible;
        };

        e.each(std::bind(set_is_invisible, _2, true),
            w_t::comment, w_t::empty
        );
    }

    const __cs_token_properties_t __cs_token_properties;

    ////////// ////////// ////////// ////////// //////////

    const operator_property_t * get_operator_property(cs_token_value_t value)
    {
        const __cs_token_property_t * property = get_token_property(value);
        operator_t op = property->op;

        return is_system_operator(op)?
            get_system_operator_property(op) : (const operator_property_t *)property;
    }

    const string_t get_token_string(cs_token_value_t value)
    {
        const __cs_token_property_t * property = get_token_property(value);

        if (property->is_operator)
        {
            operator_t op = property->op;

            if (is_system_operator(op))
                return get_system_operator_string(op);
        }

        return _str(value);
    }

    ////////// ////////// ////////// ////////// //////////

}   // X_ROOT_NS::modules::lang_cs

