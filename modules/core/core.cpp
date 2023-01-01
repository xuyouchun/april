
#include <core.h>
#include "expression_executor.h"

namespace X_ROOT_NS::modules::core {

    using namespace al;

    vtype_t __get_vtype(type_t * type);

    ////////// ////////// ////////// ////////// //////////
    // assembly_type_t

    // Assembly types
    X_ENUM_INFO(assembly_type_t)

        // Unknown
        X_C(unknown,            _T("unknown"))

        // Dynamic library
        X_C(dynamic_library,    _T("dynamic_library"))

        // Executable library
        X_C(executable,         _T("executable"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Token error codes
    X_ENUM_INFO(token_error_code_t)

        // Comment end missing.
        X_C(comment_end_missing,    _T("multiline comment end \"*/\" missing"))

        // Unknown token.
        X_C(unknown_token,          _T("unknown token"))

        // String format error.
        X_C(string_format_error,    _T("string format error"))

        // Char format error.
        X_C(char_format_error,      _T("char format error"))

        // Numeric format error.
        X_C(numeric_format_error,   _T("numeric format error"))

        // Numeric overflow.
        X_C(numeric_overflow,       _T("numeric overflow"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Compile error codes.
    X_ENUM_INFO(compile_error_code_t)

        // Duplicated entry point
        X_D(duplicated_entry_point, _T("duplicated entry point"))

        // Entry point prototype mistake: should no return type.
        X_D(entry_point_prototype_mistake__should_no_return_type,
                                    _T("entry point method should no return type"))

        // Entry point prototype mistake: should not a generic method.
        X_D(entry_point_prototype_mistake__should_not_generic,
                                    _T("entry point method should not a generic method"))

        // Entry point prototype mistake: should no param.
        X_D(entry_point_prototype_mistake__should_no_params,
                                    _T("entry point method should has no params"))

        // Entry point prototype mistake: should be public and static.
        X_D(entry_point_prototype_mistake__should_be_public_static,
                                    _T("entry point method should be a public static method"))

        // Entry point prototype mistake: decorate not allowed.
        X_D(entry_point_prototype_mistake__decorate_not_allowed,
                                    _T("entry point method should not be %1%"))

        // Entry point prototype mistake: should be static.
        X_D(extern_method_prototype_mistake__should_be_static,
                                    _T("external method should be static"))

        // Entry point prototype mistake: should be extern.
        X_D(extern_method_prototype_mistake__should_be_extern,
                                    _T("internal method should be external"))

        // Entry point prototype mistake: no allowed.
        X_D(extern_method_prototype_mistake__not_allowed,
                                    _T("%1% method not allowed in external method"))

        // Type not found.
        X_D(type_not_found, _T("type '%1%' not found"))

        // Label not found.
        X_D(label_not_found, _T("label '%1%' not found"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Constant value types
    X_ENUM_INFO(cvalue_type_t)

        // Number
        X_C(number,     _T("number"))

        // String
        X_C(string,     _T("string"))

        // Null
        X_C(null,       _T("null"))

        // Type
        X_C(type,       _T("type"))

        // Nan
        X_C(nan,        _T("nan"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Operator direction.
    X_ENUM_INFO(operator_direct_t)

        // Left to right.
        X_C(left_to_right,      _T("left_to_right"))

        // Right to left.
        X_C(right_to_left,      _T("right_to_left"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Operator overload model.
    X_ENUM_INFO(operator_overload_model_t)

        // Can be overloaded.
        X_C(overload,           _T("overload"))

        // Cannot be overloaded.
        X_C(no_overloaded,      _T("no_overloaded"))

        // Cannot be overloaded, and need no to check. such as "as, is" operators.
        X_C(no_check,           _T("no_check"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Expression box side type.
    X_ENUM_INFO(expression_box_side_t)

        // At left
        X_C(left,       _T("left"))

        // At right
        X_C(right,      _T("right"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Common branch values.
    X_ENUM_INFO(common_branch_value_t)

        X_C(import,         _T("import"))
        X_C(using_,         _T("using"))

        X_C(expression,     _T("expression"))
        X_C(statement,      _T("statement"))
        X_C(type_name,      _T("type_name"))
        X_C(cvalue,         _T("cvalue"))

        X_C(attribute,      _T("attribute"))
        X_C(attributes,     _T("attributes"))

        X_C(type,           _T("type"))
        X_C(field,          _T("field"))
        X_C(method,         _T("method"))
        X_C(property,       _T("property"))
        X_C(event,          _T("event"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Operators
    X_ENUM_INFO(operator_t)

        X_C(add,                _T("+"),        _T("Add"))
        X_C(sub,                _T("-"),        _T("Sub"))
        X_C(mul,                _T("*"),        _T("Mul"))
        X_C(div,                _T("/"),        _T("Div"))
        X_C(mod,                _T("%"),        _T("Mod"))

        X_C(add_assign,         _T("+="),       _T("AddAssign"))
        X_C(sub_assign,         _T("-="),       _T("SubAssign"))
        X_C(mul_assign,         _T("*="),       _T("MulAssign"))
        X_C(div_assign,         _T("/="),       _T("DivAssign"))
        X_C(mod_assign,         _T("%="),       _T("ModAssign"))

        X_C(left_increment,     _T("++"),       _T("LeftIncrement"))
        X_C(left_decrement,     _T("--"),       _T("LeftDecrement"))
        X_C(right_increment,    _T("++"),       _T("RightIncrement"))
        X_C(right_decrement,    _T("--"),       _T("RightDecrement"))

        X_C(minus,              _T("-"),        _T("Minus"))
        X_C(positive,           _T("+"),        _T("Positive"))

        X_C(left_shift,         _T("<<"),       _T("LeftShift"))
        X_C(right_shift,        _T(">>"),       _T("RightShift"))
        X_C(left_shift_assign,  _T("<<="),      _T("LeftShiftAssign"))
        X_C(right_shift_assign, _T(">>="),      _T("RightShiftAssign"))

        X_C(bit_and,            _T("&"),        _T("BitAnd"))
        X_C(bit_or,             _T("|"),        _T("BitOr"))
        X_C(bit_not,            _T("~"),        _T("BitNot"))
        X_C(bit_xor,            _T("^"),        _T("BitXor"))

        X_C(bit_and_assign,     _T("&="),       _T("BitAndAssign"))
        X_C(bit_or_assign,      _T("|="),       _T("BitOrAssign"))
        X_C(bit_xor_assign,     _T("^="),       _T("BitXorAssign"))

        X_C(assign,             _T("="),        _T("Assign"))
        X_C(null_coalescing,    _T("??"),       _T("NullCoalescing"))
        X_C(null_coalescing_assign, _T("??""="),  _T("NullCoalescingAssign"))

        X_C(greater,            _T(">"),        _T("Greater"))
        X_C(greater_equal,      _T(">="),       _T("GreaterEqual"))
        X_C(less,               _T("<"),        _T("Less"))
        X_C(less_equal,         _T("<="),       _T("LessEqual"))
        X_C(equal,              _T("=="),       _T("Equal"))
        X_C(not_equal,          _T("!="),       _T("NotEqual"))

        X_C(logic_and,          _T("&&"),       _T("LogicAnd"))
        X_C(logic_or,           _T("||"),       _T("LogicOr"))
        X_C(logic_not,          _T("!"),        _T("LogicNot"))

        X_C(member_point,       _T("."),        _T("MemoberPoint"))

        X_C(as,                 _T("as"),       _T("as"))
        X_C(is,                 _T("is"),       _T("is"))

    X_ENUM_INFO_END

    // Returns string description of a operator.
    const string_t get_system_operator_string(operator_t op)
    {
        return _str(op);
    }

    ////////// ////////// ////////// ////////// //////////
    // log_level_t

    // Log levels
    X_ENUM_INFO(log_level_t)

        X_C(info,       _T("info"))
        X_C(warning,    _T("warning"))
        X_C(error,      _T("error"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // efamily_t

    // Eobject families.
    X_ENUM_INFO(efamily_t)

        X_C(assembly,       _T("assembly"))
        X_C(module,         _T("module"))
        X_C(document,       _T("document"))
        X_C(namespace_,     _T("namespace"))
        X_C(type,           _T("type"))
        X_C(method,         _T("method"))
        X_C(property,       _T("property"))
        X_C(field,          _T("field"))
        X_C(event,          _T("event"))
        X_C(type_def,       _T("type_def"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Converts a object to string.
    template<typename t>
    const string_t __to_string(const t * obj)
    {
        if (obj == nullptr)
            return empty_str;

        return _str(obj);
    }

    // Converts a object to string, returns default_string if empty.
    template<typename t, typename s_t>
    const string_t __to_string(const t * obj, s_t && default_str)
    {
        if (obj == nullptr)
            return default_str;

        return _str(obj);
    }

    ////////// ////////// ////////// ////////// //////////

    // Converts operator property to string.
    operator_property_t::operator string_t() const
    {
        return _F(_T("%1%: direct: %2%, priority:%3%, arity: %4%, ")
                  _T("adhere: %5%, is_assign: %6%"),
            op, direct, priority, arity, adhere, is_assign
        );
    }

    // System operator property.
    struct __system_operator_property_t : operator_property_t
        , al::property_t<__system_operator_property_t, operator_t>
    {
        static void init(enumerator_t & e)
        {
            using namespace std::placeholders;
            typedef operator_t op_t;

            e.each([](operator_t op, operator_property_t & p) {
                p.op   = op;
                p.name = _desc(op);
            });

            // priority
            auto set_priority = [](operator_property_t & p, operator_priority_t priority) {
                p.priority = priority;
            };

            #define __Pri(v) ((v) << 8)
            #define __BeginPriority(_pri)                                               \
                e.each(std::bind(set_priority, _2, __Pri(_pri)),
            #define __EndPriority   );

            __BeginPriority(1)
                op_t::member_point                                      // .
            __EndPriority

            __BeginPriority(2)
                op_t::minus,             op_t::positive,                // - +
                op_t::left_increment,    op_t::left_decrement,          // --x ++x
                op_t::right_increment,   op_t::right_decrement,         // x-- x++
                op_t::logic_not,         op_t::bit_not                  // ! ~
            __EndPriority

            __BeginPriority(3)
                op_t::div,   op_t::mul,   op_t::mod                     // / * %
            __EndPriority

            __BeginPriority(4)
                op_t::add,   op_t::sub                                  // + -
            __EndPriority

            __BeginPriority(5)
                op_t::left_shift,    op_t::right_shift                  // << >>
            __EndPriority

            __BeginPriority(6)
                op_t::greater,   op_t::greater_equal,                   // > >=
                op_t::less,      op_t::less_equal                       // < <=
            __EndPriority

            __BeginPriority(7)
                op_t::equal,     op_t::not_equal                        // == !=
            __EndPriority

            __BeginPriority(8)
                op_t::bit_and                                           // &
            __EndPriority

            __BeginPriority(9)
                op_t::bit_xor                                           // ^
            __EndPriority

            __BeginPriority(10)
                op_t::bit_or                                            // |
            __EndPriority

            __BeginPriority(11)
                op_t::logic_and                                         // &&
            __EndPriority

            __BeginPriority(12)
                op_t::logic_or                                          // ||
            __EndPriority

            __BeginPriority(13)
                op_t::as, op_t::is                                      // as, is
            __EndPriority

            __BeginPriority(14)
                op_t::null_coalescing
            __EndPriority

            __BeginPriority(15)
                op_t::assign,                                           // =
                op_t::add_assign,    op_t::sub_assign,                  // += -=
                op_t::mul_assign,    op_t::div_assign,    op_t::mod_assign, // *= /= %=
                op_t::left_shift_assign, op_t::right_shift_assign,      // <<= >>=
                op_t::bit_and_assign,op_t::bit_or_assign, op_t::bit_xor_assign, // &= |= ^=
                op_t::null_coalescing_assign
            __EndPriority

            // arity
            auto set_arity = [](operator_property_t & p, operator_arity_t arity) {
                p.arity = arity;
            };

            e.each(std::bind(set_arity, _2, 1),
                op_t::minus,     op_t::positive,                        // + -
                op_t::bit_not,   op_t::logic_not,                       // ~ !
                op_t::left_increment, op_t::left_decrement,             // ++x --x
                op_t::right_increment, op_t::right_decrement            // x++ x--
            );

            e.each(std::bind(set_arity, _2, 2),
                op_t::add, op_t::sub, op_t::mul, op_t::div, op_t::mod,  // + - * / %

                op_t::add_assign, op_t::sub_assign, op_t::mul_assign,   // += -= *=
                op_t::div_assign, op_t::mod_assign,                     // /= %=

                op_t::left_shift, op_t::right_shift,                    // << >>
                op_t::left_shift_assign, op_t::right_shift_assign,      // <<= >>=

                op_t::bit_and, op_t::bit_or, op_t::bit_xor,             // & | ^
                op_t::bit_and_assign, op_t::bit_or_assign,              // &= |=
                op_t::bit_xor_assign,                                   // ^=

                op_t::assign,        op_t::member_point,                // = .
                op_t::null_coalescing,                                  // ??
                op_t::null_coalescing_assign,                           // ??=

                op_t::greater,       op_t::greater_equal,               // > >=
                op_t::less,          op_t::less_equal,                  // < <=
                op_t::equal,         op_t::not_equal,                   // == !=

                op_t::logic_and,     op_t::logic_or,                    // && ||

                op_t::as,            op_t::is                           // as, is
            );

            // adhere
            auto set_adhere = [](operator_property_t & p, operator_adhere_t adhere) {
                p.adhere = adhere;
            };

            e.each(std::bind(set_adhere, _2, 1));

            e.each(std::bind(set_adhere, _2, 0),
                op_t::left_increment, op_t::left_decrement,             // ++x --x
                op_t::minus,          op_t::positive,                   // +x -x
                op_t::bit_not,        op_t::logic_not                   // ~ !
            );

            // direct
            auto set_direct = [](operator_property_t & p, operator_direct_t direct) {
                p.direct = direct;
            };

            e.each(std::bind(set_direct, _2, operator_direct_t::right_to_left),
                op_t::assign,
                op_t::add_assign, op_t::sub_assign, op_t::mul_assign,
                op_t::div_assign, op_t::mod_assign,
                op_t::left_shift_assign, op_t::right_shift_assign,
                op_t::bit_and_assign, op_t::bit_or_assign, op_t::bit_xor_assign,
                op_t::null_coalescing_assign
            );

            // assign
            auto set_assign = [](operator_property_t & p, bool is_assign) {
                p.is_assign = is_assign;
            };

            e.each(std::bind(set_assign, _2, true),
                op_t::assign,
                op_t::add_assign, op_t::sub_assign, op_t::mul_assign,
                op_t::div_assign, op_t::mod_assign,
                op_t::left_shift_assign, op_t::right_shift_assign,
                op_t::bit_and_assign, op_t::bit_or_assign, op_t::bit_xor_assign,
                op_t::left_increment, op_t::left_decrement,
                op_t::right_increment, op_t::right_decrement,
                op_t::null_coalescing_assign
            );

            // Can be overloaded.
            auto set_overload = [](operator_property_t & p, operator_overload_model_t model) {
                p.overload = model;
            };

            e.each(std::bind(set_overload, _2, operator_overload_model_t::no_overloaded),
                op_t::logic_and, op_t::logic_or
            );

            e.each(std::bind(set_overload, _2, operator_overload_model_t::no_check),
                op_t::assign, op_t::member_point, 
                op_t::as,     op_t::is,
                op_t::null_coalescing, op_t::null_coalescing_assign
            );
        }

        #undef __BeginPriority
        #undef __EndPriority
    };

    typedef __system_operator_property_t::properties_t<(size_t)operator_t::__inner_end__>
        __system_operator_properties_t;

    // Returns system operator property.
    const operator_property_t * get_system_operator_property(operator_t op)
    {
        static __system_operator_properties_t system_operator_properties;

        if (__system_operator_properties_t::overlimit(op))
            return nullptr;

        return &system_operator_properties[op];
    }

    ////////// ////////// ////////// ////////// //////////

    // Access values.
    X_ENUM_INFO(access_value_t)

        // Public
        X_C(public_,                _T("public"))

        // Protected
        X_C(protected_,             _T("protected"))

        // Private
        X_C(private_,               _T("private"))

        // Internal
        X_C(internal,               _T("internal"))

        // Protected or internal.
        X_C(protected_or_internal,  _T("protected | internal"))

        // Protected and internal.
        X_C(protected_and_internal, _T("protected & internal"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Converts a type_name to a string.
    X_DEFINE_TO_STRING(type_name_t)
    {
        return type? _str(type) : _T("?");
    }

    // Converts type_name to type.
    type_t * to_type(type_name_t * type_name)
    {
        if (type_name == nullptr)
            return nullptr;

        return type_name->type;
    }

    ////////// ////////// ////////// ////////// //////////

    // Sets the owner for the attribute.
    void attribute_t::set_owner(eobject_t * object)
    {
        _A(object != nullptr);

        this->bind_object = object;
    }

    // Commits it.
    void attribute_t::commit(eobject_commit_context_t & ctx)
    {
        type_impl_t * impl = __get_impl();

        if (impl != nullptr)
            impl->commit(ctx, this->bind_object);
    }

    type_impl_t * attribute_t::__get_impl()
    {
        general_type_t * type;

        if (type_name && (type = as<general_type_t *>(type_name->type)))
            return type->impl;

        return nullptr;
    }

    // Converts to string.
    X_DEFINE_TO_STRING(attribute_t)
    {
        return _F(_T("%1%(%2%)"), __to_string(type_name, _T("?")), arguments);
    }

    ////////// ////////// ////////// ////////// //////////

    // Base class with attribute.

    // Returns whether attribute matched speicifed type.
    static bool __attribute_matched(attribute_t * attribute, general_type_t * attribute_type,
                                                             bool inherit)
    {
        type_t * type = to_type(attribute->type_name);
        if (type == nullptr)
            return false;

        if (inherit)
            return is_type_compatible(type, attribute_type);

        return type == attribute_type;
    }

    // Returns all attributes with specified attribute type.
    void with_attributes_object_t::get_attributes(output_attributes_t & out_attributes,
                                general_type_t * attribute_type, bool inherit)
    {
        _A(attribute_type != nullptr);

        if (attributes == nullptr)
            return;

        for (attribute_t * attr : *attributes)
        {
            if (__attribute_matched(attr, attribute_type, inherit))
                out_attributes.push_back(attr);
        }
    }

    // Returns first attribute with specified attribute type.
    attribute_t * with_attributes_object_t::get_attribute(general_type_t * attribute_type,
                                                                bool inherit)
    {
        _A(attribute_type != nullptr);

        if (attributes == nullptr)
            return nullptr;

        for (attribute_t * attr : *attributes)
        {
            if (__attribute_matched(attr, attribute_type, inherit))
                return attr;
        }

        return nullptr;
    }

    ////////// ////////// ////////// ////////// //////////

    // Argument name type
    X_ENUM_INFO(argument_name_type_t)

        // Param.
        X_C(param,      _T("param"))

        // Field.
        X_C(field,      _T("field"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // argument_t

    // Returns type of argument.
    type_t * argument_t::get_type() const
    {
        if (expression != nullptr)
            return expression->get_type();

        return nullptr;
    }

    // Converts argument_t to a string.
    argument_t::operator string_t() const
    {
        stringstream_t ss;

        if (atype != param_type_t::__default__)
            ss << _title(atype) << _T(" ");

        if (!name.empty())
        {
            ss << _str(name)
               << (name_type == argument_name_type_t::param? _T(":") : _T("="));
        }

        ss << _str(expression);
        return ss.str();
    }

    ////////// ////////// ////////// ////////// //////////

    // Param types.
    X_ENUM_INFO(param_type_t)

        // Ref value
        X_C(ref,        _T("ref"))

        // Out value
        X_C(out,        _T("out"))

        // Params
        X_C(params,     _T("params"))

        // Extends
        X_C(extends,    _T("extends"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // Code unit structure.

    // Returns current line.
    string_t code_unit_t::current_line() const
    {
        auto pair = current_line_pos();
        return string_t(pair.first, pair.second);
    }

    // Returns current line position.
    std::pair<const char_t *, const char_t *> code_unit_t::current_line_pos() const
    {
        const char_t * source_code;
        const char_t * s = this->s, * start = s, * end = s + length;

        if (file != nullptr && (source_code = file->get_source_code()) != nullptr)
        {
            _A(start >= source_code);

            if (*start == _T('\n'))
            {
                start--;

                while (start >= source_code && *start == _T('\r'))
                    start--;
            }

            for (; start >= source_code; start--)
            {
                if (al::is_lineend(*start))
                {
                    start++;
                    break;
                }
            }
        }

        for (; *end != _T('\0'); end++)
        {
            if (al::is_lineend(*end))
            {
                end--;
                break;
            }
        }

        return std::make_pair(start, end);
    }

    ////////// ////////// ////////// ////////// //////////

    // Code element.
    class __code_element_t : public code_element_t
    {
        typedef code_element_t __super_t;

    public:

        // Constructor.
        __code_element_t(const char_t * s, int32_t length, const code_file_t * file)
            : __unit(s, length, file)
        { }

        // Returns code position information.
        virtual const code_unit_t * get_code_unit() override
        {
            return &__unit;
        }

    private:
        const code_unit_t __unit;
    };

    //-------- ---------- ---------- ---------- ----------

    // Combines code elements.
    const code_unit_t * combine_code_unit(memory_t * memory, const code_unit_t * from,
                                                       const code_unit_t * to)
    {
        if (from == nullptr || to == nullptr)
            return from != nullptr? from : to;

        if (from == nullptr || to == nullptr)
            return from != nullptr? from : to;

        _A(from->file == to->file);

        if (to->s < from->s)
            return from;

        return memory_t::new_obj<code_unit_t>(memory,
            from->s, to->s - from->s + to->length, from->file
        );
    }

    // Combines code units.
    code_unit_t * combine_code_units(memory_t * memory, const code_units_t & code_units)
    {
        const char_t * from = nullptr, * to = nullptr;
        const code_file_t * file = nullptr;

        for (const code_unit_t * cu : code_units)
        {
            if (cu == nullptr)
                continue;

            if (cu->file != nullptr)
            {
                if (file == nullptr)
                    file = cu->file;
                else
                    _A(cu->file == file);
            }

            if (cu->s != nullptr)
            {
                if (to == nullptr || cu->s + cu->length > to)
                    to = cu->s + cu->length;

                if (from == nullptr || cu->s < from)
                    from = cu->s;
            }
        }

        if (from != nullptr)
            return memory_t::new_obj<code_unit_t>(memory, from, to - from, file);

        return nullptr;
    }

    ////////// ////////// ////////// ////////// //////////

    // Writes a token to a string.
    ostream_t & operator << (ostream_t & stream, const token_t & w)
    {
        return stream << w.code_unit;
    }

    ////////// ////////// ////////// ////////// //////////
    // mname_t

    // Creates a new mname.
    mname_t * mname_operate_context_t::new_mname()
    {
        typedef obj_wrap_t<mname_t> wmname_t;
        return memory_t::new_obj<wmname_t>(memory);
    }

    // Creates a new mname with sid.
    mname_t * mname_operate_context_t::new_mname(const sid_t & sid)
    {
        typedef obj_wrap_t<mname_t> wmname_t;
        return memory_t::new_obj<wmname_t>(__Memory, sid);
    }

    // Creates a new mname.
    mname_t * mname_operate_context_t::new_mname(const mname_t & mname)
    {
        typedef obj_wrap_t<mname_t> wmname_t;
        mname_t * m = memory_t::new_obj<wmname_t>(__Memory);

        m->sid = mname.sid;
        al::copy(mname.parts, std::back_inserter(m->parts));

        return m;
    }

    // Commits mname.
    void mname_t::commit(al::spool_t & pool)
    {
        string_t s = al::join_str(parts.begin(), parts.end(), _T("."));
        this->sid = pool.to_sid(s);
    }

    // Returns part of specified indes as char_t *
    const char_t * mname_t::part_of(size_t index) const
    {
        _A(index < parts.size());

        sid_t sid = parts[index];
        return sid.c_str();
    }

    // Returns part of specified index as char_t *
    const char_t * mname_t::last_part() const
    {
        if (empty())
            return nullptr;

        return part_of(parts.size() - 1);
    }

    // Parses a string to mname_t.
    const mname_t * mname_t::parse(__context_t && ctx, const string_t & s)
    {
        return parse(ctx, s);
    }

    // Parses a string to mname_t.
    const mname_t * mname_t::parse(__context_t & ctx, const string_t & s)
    {
        if (s.length() == 0)
            throw _ECF(format_error, _T("mname \"%1%\" string empty"), s);

        sid_t sid = ctx.spool.to_sid(s);

        return ctx.from_cache(sid, [&ctx, &s, sid]() {
            mname_t * name = ctx.new_mname();

            al::split(s.begin(), s.end(), _T('.'), [&ctx, name](auto b, auto e) {
                name->parts.push_back(ctx.spool.to_sid(string_t(b, e)));
            });

            name->sid = sid;
            return name;
        });
    }

    // Combines two mnames.
    const mname_t * mname_t::combine(__context_t && ctx, const mname_t * name1,
                                                         const mname_t * name2)
    {
        return combine(ctx, name1, name2);
    }

    // Combines two mnames.
    const mname_t * mname_t::combine(__context_t & ctx, const mname_t * name1,
                                                        const mname_t * name2)
    {
        if (name1 == nullptr || name2 == nullptr)
            return name1 != nullptr? name1 : name2;

        stringstream_t ss;
        ss << ((string_t)name1->sid).c_str() << _T(".") << ((string_t)name2->sid).c_str();
        sid_t sid = ctx.spool.to_sid(ss.str());

        return ctx.from_cache(sid, [&ctx, sid, name1, name2]() {
            mname_t * name = ctx.new_mname();

            al::copy(name1->parts, std::back_inserter(name->parts));
            al::copy(name2->parts, std::back_inserter(name->parts));

            name->sid = sid;
            return name;
        });
    }

    // Combines two mnames.
    const mname_t * mname_t::combine(__context_t && ctx, const mname_t * name1,
                                                         const name_t & name2)
    {
        return combine(ctx, name1, name2);
    }

    // Combines two mnames.
    const mname_t * mname_t::combine(__context_t & ctx, const mname_t * name1,
                                                        const name_t & name2)
    {
        return combine(std::forward<__context_t>(ctx), name1, to_mname(ctx, name2));
    }

    // Returns parent of a mname.
    const mname_t * mname_t::get_parent(__context_t && ctx, const mname_t * name)
    {
        return get_parent(ctx, name);
    }

    // Returns parent of a mname.
    const mname_t * mname_t::get_parent(__context_t & ctx, const mname_t * name)
    {
        if (name == nullptr || name->parts.size() <= 1)
            return nullptr;

        if (name->parts.size() == 2)
        {
            return ctx.from_cache(name->parts[0], [&ctx, name]() {
                return ctx.new_mname(name->parts[0]);
            });
        }

        string_t s = al::join_str(name->parts.begin(), name->parts.end() - 1, _T("."));
        sid_t sid = ctx.spool.to_sid(s);

        return ctx.from_cache(sid, [&ctx, name, sid]() {
            mname_t * new_name = ctx.new_mname();

            std::copy(name->parts.begin(), name->parts.end() - 1,
                std::back_inserter(new_name->parts)
            );

            new_name->sid = sid;
            return new_name;
        });
    }

    // Returns child of a mname.
    const mname_t * mname_t::get_child(__context_t && ctx, const mname_t * name)
    {
        if (name == nullptr || name->parts.size() <= 1)
            return nullptr;

        if (name->parts.size() == 2)
        {
            return ctx.from_cache(name->parts[1], [&ctx, name]() {
                return ctx.new_mname(name->parts[1]);
            });
        }

        string_t s = al::join_str(name->parts.begin() + 1, name->parts.end(), _T("."));
        sid_t sid = ctx.spool.to_sid(s);

        return ctx.from_cache(sid, [&ctx, name, sid]() {
            mname_t * new_name = ctx.new_mname();

            std::copy(name->parts.begin() + 1, name->parts.end(),
                std::back_inserter(new_name->parts)
            );

            new_name->sid = sid;
            return new_name;
        });
    }

    // Convets a name_t to a mname_t.
    const mname_t * mname_t::to_mname(__context_t && ctx, const name_t & name)
    {
        return to_mname(ctx, name);
    }

    // Converts a name_t to a mname_t.
    const mname_t * mname_t::to_mname(__context_t &  ctx, const name_t & name)
    {
        return ctx.from_cache((sid_t)name, [&ctx, name]() {
            return ctx.new_mname((sid_t)name);
        });
    }

    // Pushes a mname to the pool.
    const mname_t * mname_t::to_internal(__context_t && ctx, const mname_t * mname)
    {
        return to_internal(ctx, mname);
    }

    // Pushes a mname to the pool.
    const mname_t * mname_t::to_internal(__context_t &  ctx, const mname_t * mname)
    {
        _A(mname != nullptr);

        return ctx.from_cache((sid_t)*mname, [&ctx, mname]() {
            return ctx.new_mname(*mname);
        });
    }

    const mname_t mname_t::null;

    ////////// ////////// ////////// ////////// //////////

    // Converts emname_t to string.
    X_DEFINE_TO_STRING(emname_t)
    {
        return _str(mname);
    }

    ////////// ////////// ////////// ////////// //////////
    // decorate_value_t

    const decorate_value_t decorate_value_t::default_value { };

    const decorate_value_t decorate_value_t::public_ {
        .access = access_value_t::public_
    };

    const decorate_value_t decorate_value_t::public_readonly {
        .is_readonly = true, .access = access_value_t::public_
    };

    const decorate_value_t decorate_value_t::private_ {
        .access = access_value_t::private_
    };

    const decorate_value_t decorate_value_t::private_readonly {
        .is_readonly = true, .access = access_value_t::private_
    };

    const decorate_t decorate_t::default_value { };

    // Converts decorate_value_t to a string.
    decorate_value_t::operator string_t() const
    {
        al::svector_t<const char_t * > ss;

        #define __AppStr(name)  \
            do { if (is_##name) ss.push_back(_S(name)); } while (false)

        if (access != access_value_t::__default__)
            ss.push_back(_title(access));

        __AppStr(static);
        __AppStr(sealed);
        __AppStr(override);
        __AppStr(virtual);
        __AppStr(abstract);
        __AppStr(new);
        __AppStr(const);
        __AppStr(readonly);

        #undef __AppStr

        return al::join_str(ss.begin(), ss.end(), _T(" "));
    }

    ////////// ////////// ////////// ////////// //////////
    // decorate_t

    X_DEFINE_TO_STRING(decorate_t)
    {
        return _str(*(decorate_value_t *)this);
    }

    // Returns access value from decorate_t;
    access_value_t to_access_value(const decorate_t * decorate)
    {
        return decorate == nullptr? empty_access_value : decorate->access;
    }

    decorate_t * with_decorate_object_t::__decorate()
    {
        return decorate != nullptr? decorate :
            (decorate = __XPool.new_obj<decorate_t>(decorate_value_t::default_value));
    }


    ////////// ////////// ////////// ////////// //////////
    // name_t

    // The null sigleton name.
    const name_t name_t::null(sid_t::null);

    ////////// ////////// ////////// ////////// //////////

    // Converts cvalue to a string.
    X_DEFINE_TO_STRING(cvalue_t)
    {
        switch (value_type)
        {
            case cvalue_type_t::string:
                return _F(_T("\"%1%\""), al::escape_string(string_t(string)));

            case cvalue_type_t::number:
                return string_t(number);

            case cvalue_type_t::null:
                return _T("null");

            case cvalue_type_t::type:
                return _str(mobject);

            case cvalue_type_t::nan:
                return _title(value_type);

            default:
                return _T("");
        }
    }

    // Returns whether two cvalue_t are equals.
    bool cvalue_t::operator == (const cvalue_t & other) const _NE
    {
        if (value_type != other.value_type)
            return false;

        switch (value_type)
        {
            case cvalue_type_t::string:
                return al::strcmp(string, other.string) == al::cmp_t::equals;

            case cvalue_type_t::number:
                return number == other.number;

            case cvalue_type_t::null:
                return true;

            case cvalue_type_t::type:
                return mobject == other.mobject;

            default:
                return true;
        }
    }

    // Returns whether two cvalue_t are equals.
    bool cvalue_t::operator != (const cvalue_t & other) const _NE
    {
        return !operator == (other);
    }

    // Returns whether a cvalue is less than other.
    bool cvalue_t::operator < (const cvalue_t & other) const _NE
    {
        if (value_type != other.value_type)
            return value_type < other.value_type;

        switch (value_type)
        {
            case cvalue_type_t::string:
                return al::strcmp(string, other.string) == al::cmp_t::less;

            case cvalue_type_t::number:
                return number < other.number;

            case cvalue_type_t::null:
                return false;

            case cvalue_type_t::type:
                return mobject < other.mobject;

            default:
                return false;
        }
    }

    // Returns whether a cvalue is less or equals than other.
    bool cvalue_t::operator <= (const cvalue_t & other) const _NE
    {
        if (value_type != other.value_type)
            return value_type <= other.value_type;

        switch (value_type)
        {
            case cvalue_type_t::string: {
                typedef al::cmp_t __cmp_t;
                __cmp_t r = al::strcmp(string, other.string);
                return r == __cmp_t::less || r == __cmp_t::equals;
            }   break;

            case cvalue_type_t::number:
                return number <= other.number;

            case cvalue_type_t::null:
                return true;

            case cvalue_type_t::type:
                return mobject <= other.mobject;

            default:
                return true;
        }
    }

    // Returns whether a cvalue is larger or equals than other.
    bool cvalue_t::operator >= (const cvalue_t & other) const _NE
    {
        return ! operator < (other);
    }

    // Returns whether a cvalue is larger than other.
    bool cvalue_t::operator > (const cvalue_t & other) const _NE
    {
        return ! operator <= (other);
    }

    // Returns two cvalues are equals.
    bool cvalue_t::operator == (bool value) const _NE
    {
        return value_type == cvalue_type_t::number
            && number.type == value_type_t::bool_
            && number.value.bool_value == value;
    }

    // Returns two cvalues are not equals.
    bool cvalue_t::operator != (bool value) const _NE
    {
        return ! operator == (value);
    }

    // The sigleton instance of nan.
    const cvalue_t cvalue_t::nan(cvalue_type_t::nan);

    // The sigleton instance of null.
    const cvalue_t cvalue_t::null(nullptr);

    cvalue_t cvalue_t::change_type(vtype_t vtype) const _NE
    {
        switch (vtype)
        {
            case vtype_t::string_:
                if (value_type == cvalue_type_t::string)
                    return *this;
                break;

            #define __Case(t)                                                           \
                case vtype_t::t##_:                                                     \
                    return cvalue_t(number.get_value<t##_t>());

            __Case(int8)
            __Case(uint8)
            __Case(int16)
            __Case(uint16)
            __Case(int32)
            __Case(uint32)
            __Case(int64)
            __Case(uint64)
            __Case(float)
            __Case(double)
            __Case(bool)
            __Case(char)

            #undef __Case

            default:
                break;
        }

        return nan;
    }

    ////////// ////////// ////////// ////////// //////////
    // statement_exit_type_t

    X_ENUM_INFO(statement_exit_type_t)

        // None.
        X_C(none,       _T("none"))

        // Pass through.
        X_C(pass,       _T("pass"))

        // Return.
        X_C(return_,    _T("return"))

        // Break.
        X_C(break_,     _T("break"))

        // Continue.
        X_C(continue_,  _T("continue"))

        // Throw.
        X_C(throw_,     _T("throw"))

        // Goto.
        X_C(goto_,      _T("goto"))

        // Dead cycle, cannot exit.
        X_C(dead_cycle, _T("dead_cycle"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // member_t

    // Returns a prototype name.
    string_t member_t::to_prototype() const
    {
        if (host_type == nullptr)
            return get_name();

        return _FT("%1%.%2%", host_type->to_short_name(), this->get_name());
    }

    // Returns access value of specified member.
    access_value_t member_t::get_access_value()
    {
        access_value_t av = to_access_value(decorate);

        if (av != empty_access_value)
            return av;

        member_type_t mtype = this_type();
        if (al::in(mtype, member_type_t::type, member_type_t::type_def)
            && host_type == nullptr)
        {
            return access_value_t::internal;
        }

        if (is_interface(host_type))
            return access_value_t::public_;

        return access_value_t::private_;
    }

    // Returns assembly of a member.
    assembly_t * member_t::get_assembly()
    {
        type_t * type;

        if (this->this_type() == member_type_t::type)
            type = (type_t *)this;
        else
            type = this->host_type;

        switch (type->this_gtype())
        {
            case gtype_t::general:
                return ((general_type_t *)type)->assembly;

            default:
                return nullptr;
        }
    }

    // Returns whether the two members has the same access modifier.
    bool is_same_access_modifier(member_t * member1, member_t * member2)
    {
        _A(member1 != nullptr);
        _A(member2 != nullptr);

        return member1->get_access_value() == member2->get_access_value();
    }

    ////////// ////////// ////////// ////////// //////////
    // param_t

    // Returns type of a param.
    type_t * param_t::get_type() const
    {
        return to_type(type_name);
    }

    // Converts param to string.
    param_t::operator string_t() const
    {
        if (ptype == param_type_t::__default__)
            return _F(_T("%1% %2%"), type_name, name);

        return _F(_T("%1% %2% %3%"), ptype, type_name, name);
    }

    // Append to a string stream.
    void param_t::append_to(stringstream_t & ss, bool include_name)
    {
        if (ptype != param_type_t::__default__ && ptype != param_type_t::extends)
            ss << _str(ptype) << _T(" ");

        ss << _str(type_name);

        if (include_name)
            ss << _T(" ") << _str(name);
    }

    ////////// ////////// ////////// ////////// //////////
    // method_t

    // Method inline types
    X_ENUM_INFO(method_inline_type_t)

        // Default, auto determined.
        X_C(default_,   _T("default"))

        // Always inline.
        X_C(always,     _T("always"))

        // Never inline.
        X_C(never,      _T("never"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Method trait.
    X_ENUM_INFO(method_trait_t)

        // Normal.
        X_C(normal,             _T("normal"))

        // Constructor.
        X_C(constructor,        _T("constructor"))

        // Static constructor.
        X_C(static_constructor, _T("static_constructor"))

        // Destructor.
        X_C(destructor,         _T("destructor"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Enumerate all local variables.
    void local_variables_t::each(std::function<void (local_variable_t *)> f) const
    {
        __node_t::each(__head, f);
    }

    // Return count of variables.
    size_t local_variables_t::size() const _NE
    {
        size_t size = 0;
        __node_t::each(__head, [&](auto) { size++; });

        return size;
    }

    // Write local variables to a buffer.
    void local_variables_t::write(__assembly_layout_t & layout, xil_buffer_t & buffer) const
    {
        int variable_count = this->size();
        if (variable_count == 0)
            return;

        method_variable_stub_t stub(variable_count);
        stub.write(buffer);

        al::svector_t<local_variable_t *, 16> variables;
        this->each([&](local_variable_t * variable) {
            variables.push_back(variable);
        });

        std::sort(std::begin(variables), std::end(variables), [](auto v1, auto v2) {
            return v1->identity < v2->identity;
        });

        for (local_variable_t * variable : variables)
        {
            type_t * type = variable->get_type();
            ref_t ref = layout.ref_of(type);
            if (ref == ref_t::null)
                throw _ED(compile_error_code_t::type_not_found, type);

            local_variable_defination_t defination = { ref, variable->index };
            defination.write(layout, buffer);

        };
    }

    // Converts to string.
    local_variables_t::operator string_t() const
    {
        stringstream_t ss;
        int index = 0;

        each([&](local_variable_t * variable) {

            if (index++ > 0)
                ss << _T("\n");

            ss << _F(_T("%1% %2% [%3%] %4%"), variable->type_name, variable->name,
                                         variable->identity,variable->index).c_str();
        });

        return ss.str();
    }

    //-------- ---------- ---------- ---------- ----------

    // Writes method stub to a buffer.
    void method_stub_t::write(xil_buffer_t & buffer)
    {
        buffer.write(*(uint32_t *)this);
    }

    //-------- ---------- ---------- ---------- ----------

    // Writes switch-case table to a buffer.
    void switch_table_t::write(xil_buffer_t & buffer)
    {
        switch_table_header_t header = *this;
        buffer.write(header);

        buffer.write(default_step);

        for (switch_row_t & row : *this)
        {
            buffer.write(row.value);
            buffer.write(row.step);
        }
    }

    //-------- ---------- ---------- ---------- ----------

    // Append a switch table with specified row count.
    switch_table_t * switch_manager_t::append_table(int row_count)
    {
        switch_table_t * tbl = new (__heap.alloc(
            sizeof(switch_table_t) + sizeof(switch_row_t) * row_count
        )) switch_table_t(__tbl_index++, row_count);

        __tables.push_back(tbl);
        return tbl;
    }

    // Returns index of a label.
    static xil_index_t __index_of(statement_compile_context_t & ctx, local_label_t label)
    {
        xil_t * label_xil = ctx.jmp_manager.label_xil(label);
        if (label_xil == nullptr)
            throw _ED(compile_error_code_t::label_not_found, label);

        return ctx.xil_pool().index_of(label_xil);
    }

    // Commits it.
    void switch_manager_t::commit(statement_compile_context_t & ctx)
    {
        for (switch_table_t * tbl : __tables)
        {
            _A(tbl->jmp_xil != nullptr);
            int switch_index = ctx.xil_pool().index_of(tbl->jmp_xil);

            for (switch_row_t & row : *tbl)
            {
                row.step = __index_of(ctx, row.label) - switch_index;
            }

            tbl->default_step = __index_of(ctx, tbl->default_label) - switch_index;
        }
    }

    // Writes all switch-case tables to a buffer.
    void switch_manager_t::write(xil_buffer_t & buffer)
    {
        method_switch_table_stub_t(__tables.size()).write(buffer);

        for (switch_table_t * tbl : __tables)
        {
            tbl->write(buffer);
        }
    }

    //-------- ---------- ---------- ---------- ----------

    // Default method 
    class __default_method_compile_controller_t : public method_compile_controller_t
    {
    public:

        // Returns whether specified code need optimized.
        virtual bool optimize(int code) override
        {
            return false;
        }
    };

    // A default compile controller.
    method_compile_controller_t * method_compile_controller_t::default_()
    {
        static __default_method_compile_controller_t default_;
        return &default_;
    }

    //-------- ---------- ---------- ---------- ----------

    // Converts generic arguments to a type collection.
    type_collection_t to_type_collection(generic_args_t * args)
    {
        type_collection_t tc;
        fill_type_collection(tc, args);

        return std::move(tc);
    }

    // Fill type collection with generic arguments.
    void fill_type_collection(type_collection_t & tc, generic_args_t * args)
    {
        if (args == nullptr)
            return;

        for (generic_arg_t * arg : *args)
        {
            tc.push_back(typex_t(arg->get_type(), arg->atype));
        }
    }

    //-------- ---------- ---------- ---------- ----------

    // Write local variable defination to a buffer.
    void local_variable_defination_t::write(__assembly_layout_t & layout, xil_buffer_t & buffer)
    {
        buffer.write(type);
        buffer.write(index);
    }

    //-------- ---------- ---------- ---------- ----------

    // Method stub type. Components of method.
    X_ENUM_INFO(method_xil_block_type_t)

        // When specified exception raised.
        X_C(catch_,         _T("catch"))

        // When leaved from protected block.
        X_C(finally_,       _T("finally"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Converts to string.
    method_xil_block_t::operator string_t() const
    {
        return _F(_T("%1%: %2%-%3% ->%4% %5%"),
            type, xil_start, xil_end, entry_point, relation_type
        );
    }

    //-------- ---------- ---------- ---------- ----------

    // Member families
    X_ENUM_INFO(member_family_t)

        // General member.
        X_C(general,        _T("general"))

        // Generic member.
        X_C(generic,        _T("generic"))

        // Generic member implement with specified type arguments.
        X_C(impl,           _T("impl"))

        // Specified member by position, from index 0.
        X_C(position,       _T("position"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Returns vtype at specified index.
    vtype_t param_types_t::param_vtype_at(size_t index) const
    {
        return __get_vtype(this->param_type_at(index));
    }

    //-------- ---------- ---------- ---------- ----------

    // Returns a prototype name.
    string_t method_t::to_prototype() const
    {
        stringstream_t ss;

        if (host_type != nullptr)
            ss << host_type->to_short_name().c_str() << _T(".");

        ss << _str(name);
        __append_params(ss, false);

        return ss.str();
    }

    // Returns param type of specified index.
    typex_t method_t::param_type_at(size_t index) const
    {
        param_t * param = param_at(index);
        _A(param != nullptr);

        return typex_t(param);
    }

    // Compiles method.
    void method_t::compile(method_compile_context_t & mctx)
    {
        if (body == nullptr)
            return;

        statement_compile_context_t sctx(mctx, this);

        // Compiles statements to xilxes.
        for (statement_t * statement : body->statements)
        {
            statement->compile(sctx);
        }

        statement_region_t * region = sctx.current_region();
        if (region == nullptr)
            throw _EC(unexpected, _T("region stack empty"));

        if (region->parent != nullptr)
            throw _EC(unexpected, _T("region stack error"));

        // Compiles xilxes to xils.
        xilx_block_manager_t block_manager;
        xilx_write_context_t xw_ctx(sctx, block_manager, region);

        while (!xw_ctx.regions.empty())
        {
            statement_region_t * region = al::queue_pop(xw_ctx.regions);

            region->each([&xw_ctx, &mctx](xilx_t * xilx) {
                xilx->write(xw_ctx, mctx.xil_pool);
            });
        }

        __local_variables.write(mctx.layout, mctx.buffer);

        mctx.xil_pool.commit();
        sctx.jmp_manager.commit(sctx);

        sctx.switch_manager.commit(sctx);
        sctx.switch_manager.write(mctx.buffer);

        xw_ctx.block_manager.commit(sctx);
        xw_ctx.block_manager.write(sctx, mctx.buffer);

        /* Show all variables.
        if (!__local_variables.empty())
            _PP(__local_variables);
        */
    }

    // Finds param by name.
    template<typename _params_t>
    auto __find_param(_params_t * params, const name_t & name)
        -> typename _params_t::value_type
    {
        if (params == nullptr)
            return nullptr;

        for (auto param : *params)
        {
            if (param->name == name)
                return param;
        }

        return nullptr;
    }


    // Returns return type of the method.
    type_t * method_t::get_type()
    {
        return to_type(type_name);
    }

    // Returns original type, returns generic param type if it's a generic_method.
    type_t * method_t::get_original_type()
    {
        return get_type();
    }

    // Finds generic param by name.
    generic_param_t * method_t::find_param(const name_t & name)
    {
        return __find_param(generic_params, name);
    }

    // Converts to a string.
    method_t::operator string_t() const
    {
        stringstream_t ss;
        if (type_name != nullptr)
            ss << _str(type_name) << _T(" ");

        ss << _str(name);
        __append_params(ss, true);

        return ss.str();
    }

    // Append generic params and params.
    void method_t::__append_params(stringstream_t & ss, bool include_param_name) const
    {
        if (generic_param_count() > 0)
            ss << _T("<") << _str(generic_params) << _T(">");

        ss << _T("(");

        if (param_count() > 0)
        {
            int index = 0;
            for (param_t * param : *params)
            {
                if (index++ > 0)
                    ss << _T(", ");

                param->append_to(ss, include_param_name);
            }
        }

        ss << _T(")");
    }

    // Gets the general method of a method.
    method_t * to_general(method_base_t * method)
    {
        if (method == nullptr)
            return nullptr;

        switch (method->this_family())
        {
            case member_family_t::general:
            case member_family_t::impl:
                return (method_t *)method;

            case member_family_t::generic:
                return ((generic_method_t *)method)->template_;

            default:
                X_UNEXPECTED();
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Type transform, __transform_func_t is type_t * (name_t)
    template<typename __transform_func_t>
    class __type_transform_t
    {
    public:
        __type_transform_t(__transform_func_t & func) : __func(func) { }
        
        // Transform type.
        type_t * transform_type(type_t * type)
        {
            switch (type->this_gtype())
            {
                case gtype_t::generic_param:
                    return __type_at(((generic_param_t *)type)->name);

                case gtype_t::array: {
                    array_type_t * array_type = (array_type_t *)type;
                    type_t * new_element_type = transform_type(array_type->element_type);
                    return __XPool.new_array_type(new_element_type, array_type->dimension);

                }   break;

                case gtype_t::generic:
                    return __transform_generic_type((generic_type_t *)type);

                default:
                    return type;
            }
        }

        // Transform type_name.
        type_name_t * transform_type_name(type_name_t * type_name)
        {
            type_t * type;
            if (type_name == nullptr || (type = type_name->type) == nullptr)
                return type_name;

            type_t * new_type = transform_type(type);
            if (type == new_type || new_type == nullptr)
                return type_name;

            return __XPool.to_type_name(new_type);
        }

    private:
        __transform_func_t & __func;

        type_t * __type_at(name_t name)
        {
            return __func(name);
        }

        // Transform a generic type.
        type_t * __transform_generic_type(generic_type_t * type)
        {
            type_collection_t type_args;
            al::transform(type->args, std::back_inserter(type_args),
                [this](type_t * arg_type) { return transform_type(arg_type); }
            );

            generic_type_t * new_type = __XPool.new_generic_type(
                type->template_, type_args, type->host_type
            );

            new_type->decorate  = type->decorate;
            new_type->host_type = nullptr;

            return new_type;
        }
    };

    // Transform generic type.
    type_t * transform_type(type_t * type, std::function<type_t * (name_t)> type_provider)
    {
        if (type == nullptr)
            return nullptr;

        __type_transform_t transform(type_provider);
        return transform.transform_type(type);
    }

    ////////// ////////// ////////// ////////// //////////
    // impl_method_t

    // Returns param count.
    size_t impl_method_t::param_count() const
    {
        return __super_t::param_count();
    }

    // Returns param type of specified index.
    typex_t impl_method_t::param_type_at(size_t index) const
    {
        return __super_t::param_type_at(index);
    }

    // Returns original type, returns generic param type if it's a generic_method.
    type_t * impl_method_t::get_original_type()
    {
        return raw->get_type();
    }

    // Builds with generic args.
    void impl_method_t::build(method_t * raw, type_collection_t & args)
    {
        xpool_t & xpool = __XPool;

        auto type_at = [&](name_t name) -> type_t * {

            for (size_t index = 0, count = raw->generic_param_count(); index < count; index++)
            {
                generic_param_t * gp = (*raw->generic_params)[index];

                if (gp->name == name)
                    return args[index];
            }

            X_UNEXPECTED();
        };

        __type_transform_t transformer(type_at);

        _A(raw != nullptr);
        _A(raw->generic_param_count() == args.size());

        this->raw = raw;

        this->owner_type_name = raw->owner_type_name;
        this->type_name   = transformer.transform_type_name(raw->type_name);
        this->generic_params = raw->generic_params;
        this->name        = raw->name;
        this->decorate    = raw->decorate;
        this->trait       = raw->trait;
        this->host_type   = raw->host_type;
        this->variable    = xpool.new_obj<method_variable_t>(this);

        if (raw->params != nullptr)
        {
            this->params = __XPool.new_obj<params_t>();

            for (param_t * param : *raw->params)
            {
                param_t * new_param = xpool.new_obj<param_t>();

                new_param->name = param->name;
                new_param->attributes = param->attributes;
                new_param->type_name = transformer.transform_type_name(param->type_name);
                new_param->ptype = param->ptype;
                new_param->default_value = param->default_value;

                this->params->push_back(new_param);
            }
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_method_t

    typex_t::operator string_t() const
    {
        __gatype_t atype = this->atype();
        type_t * type = this->type();

        if (atype == __gatype_t::__default__)
            return _str(type);

        return _F(_T("%s %s"), _str(atype), _str(type));
    }

    // Returns interface type that defined the method.
    type_t * generic_method_t::get_owner_type()
    {
        return __revise_type(__template()->get_owner_type());
    }

    // Returns the return type.
    type_t * generic_method_t::get_type()
    {
        return __revise_type(__template()->get_type());
    }

    // Returns original type, returns generic param type if it's a generic_method.
    type_t * generic_method_t::get_original_type()
    {
        return __template()->get_type();
    }

    // Returns param count.
    size_t generic_method_t::param_count() const
    {
        return __template()->param_count();
    }

    // Returns param type of specified index.
    typex_t generic_method_t::param_type_at(size_t index) const
    {
        typex_t typex = __template()->param_type_at(index);
        type_t * type = (type_t *)typex;

        return typex_t(__revise_type(type), (param_type_t)typex);
    }

    // Finds real type of generic param.
    template<typename _type_collection_t, typename _pred_t, typename _f_t>
    bool __search_type(generic_params_t & params, _type_collection_t & args,
                                                _pred_t pred, _f_t f)
    {
        for (size_t index = 0, count = params.size(); index < count; index++)
        {
            if (pred(params[index]))
            {
                f(args, index);
                return true;
            }
        }

        return false;
    }

    // Finds real type of generic param by name.
    template<typename _type_collection_t>
    type_t * __search_type(generic_params_t & params, const _type_collection_t & args, name_t name)
    {
        type_t * type = nullptr;

        __search_type(params, args,
            [&](generic_param_t * param) { return param->name == name; },
            [&](auto & args_, size_t index) { _A(index < args_.size()); type = args_[index]; }
        );

        return type;
    }

    // Replace to real type if it's a general param.
    type_t * generic_method_t::__revise_type(type_t * type) const
    {
        if (type == nullptr)
            return nullptr;

        if (is_generic_param(type))
        {
            name_t name = ((generic_param_t *)type)->name;
            type_t * type1 = __search_type(*__template()->generic_params, args, name);
            if (type1 != nullptr)
                return type1;

            if (is_generic(host_type) &&
                (type1 = ((generic_type_t *)host_type)->type_at(name)) != nullptr)
                return type1;

            return type;
        }

        if (has_generic_params(type))
        {
            generic_type_t * generic_type = (generic_type_t *)type;
            type_collection_t types;

            for (type_t * type : generic_type->args)
            {
                types.push_back(__revise_type(type));
            }

            return __XPool.new_generic_type(generic_type->template_, types,
                                            generic_type->host_type);
        }

        return type;
    }

    // Converts generic method to a string.
    X_DEFINE_TO_STRING(generic_method_t)
    {
        method_t * method = __template();

        stringstream_t ss;

        // Return type.
        type_t * return_type = __revise_type(to_type(method->type_name));
        if (return_type != nullptr)
            ss << return_type->to_full_name() << _T(" ");

        // Method name.
        ss << _str(method->name);

        // Generic arguments.
        if (!args.empty())
            ss << _T("<") << _str(args) << _T(">");

        // Arguments.
        ss << _T("(");
        if (method->params != nullptr)
        {
            int index = 0;
            for (param_t * param : *method->params)
            {
                if (index++ > 0)
                    ss << _T(", ");

                type_t * type = __revise_type(to_type(param->type_name));
                _A(type != nullptr);

                ss << type->to_full_name().c_str() << _T(" ") << _str(param->name).c_str();
            }
        }
        ss << _T(")");

        return ss.str();
    }

    ////////// ////////// ////////// ////////// //////////
    // property_t

    // Converts property to a string.
    property_t::operator string_t() const
    {
        if (is_empty(params))
            return _F(_T("%1% %2%"), _str(type_name), _str(name));

        return _F(_T("%1% this[%2%]"), _str(type_name), _str(params));
    }

    ////////// ////////// ////////// ////////// //////////
    // event_t

    // Converts event to a string.
    event_t::operator string_t() const
    {
        return _F(_T("%1% %2%"), _str(type_name), _str(name));
    }

    ////////// ////////// ////////// ////////// //////////
    // field_t

    // Converts field to a string.
    field_t::operator string_t() const
    {
        if (init_value == nullptr)
            return _F(_T("%1% %2%"), _str(type_name), _str(name));

        return _F(_T("%1% %2% = %3%"), _str(type_name), _str(name), _str(init_value));
    }

    // Returns size of a field.
    msize_t field_t::get_size()
    {
        if (__size != unknown_msize)
            return __size;

        return __size = __get_size();
    }

    // Returns size of a field.
    msize_t field_t::__get_size()
    {
        if (type_name == nullptr || type_name->type == nullptr)
            return unknown_msize;

        type_t * type = type_name->type;
        ttype_t ttype;

        switch (type->this_gtype())
        {
            case gtype_t::general:
                ttype = ((general_type_t *)type)->ttype;
                break;

            case gtype_t::generic:
                ttype = (((generic_type_t *)type)->template_)->ttype;
                break;

            default:
                ttype = ttype_t::class_;
                break;
        }

        switch (ttype)
        {
            case ttype_t::struct_:
                return type->get_vsize();

            case ttype_t::enum_:
                return 1;       // TODO

            default:
                return get_vtype_size(vtype_t::mobject_);
        }
    }

    // Returns vtype of this field.
    vtype_t field_t::get_vtype()
    {
        type_t * type = get_type();
        if (type == nullptr)
            return vtype_t::__unknown__;

        return type->this_vtype();
    }

    // Returns whether ttype is ref type.
    bool __is_ref_type(ttype_t ttype)
    {
        return is_ref_type(ttype);
    }

    // Returns whether it's a reference type.
    bool field_t::is_ref_type()
    {
        type_t * type = get_type();
        if (type == nullptr)
            return true;                    // treat as object

        ttype_t ttype = type->this_ttype();
        return __is_ref_type(ttype);
    }

    ////////// ////////// ////////// ////////// //////////
    // type_def_param_t

    // Returns members descripted by args.
    member_t * type_def_param_t::get_member(analyze_member_args_t & args)
    {
        return nullptr;
    }

    // Returns memeber descripted by args.
    void type_def_param_t::get_members(analyze_members_args_t & args)
    {
        // Empty.
    }

    // Returns all members.
    void type_def_param_t::get_all_members(members_t & out_members)
    {
        // Empty.
    }

    ////////// ////////// ////////// ////////// //////////
    // type_def_t

    // Finds param by the name.
    type_def_param_t * type_def_t::find_param(const name_t & name)
    {
        return __find_param(params, name);
    }

    // Converts to a string.
    type_def_t::operator string_t() const
    {
        if (param_count() == 0)
            return _F(_T("using %1% = %2%"), _str(name), _str(type_name));

        return _F(_T("using %1% = %2%<%3%>"), _str(name), _str(type_name), _str(params));
    }

    ////////// ////////// ////////// ////////// //////////
    // vtype_t

    // Vtypes
    X_ENUM_INFO(vtype_t)

        X_C(int8_,      _T("int8"))

        X_C(uint8_,     _T("uint8"))

        X_C(int16_,     _T("int16"))

        X_C(uint16_,    _T("uint16"))

        X_C(int32_,     _T("int32"))

        X_C(uint32_,    _T("uint32"))

        X_C(int64_,     _T("int64"))

        X_C(uint64_,    _T("uint64"))

        X_C(float_,     _T("float"))

        X_C(double_,    _T("double"))

        X_C(bool_,      _T("bool"))

        X_C(char_,      _T("char"))

        X_C(string_,    _T("string"))

        X_C(void_,      _T("void"))

        X_C(ptr_,       _T("ptr"))

        X_C(mobject_,   _T("mobject"))

    X_ENUM_INFO_END
    
    //-------- ---------- ---------- ---------- ----------

    #define __Case(_type, _kind)                                                        \
        case vtype_t::_type##_: {                                                       \
            static const _type##_t value = _type##_##_kind;                             \
            return (const void *)&value;                                                \
        }

    #define __Switch(_vtype, _kind)                                                     \
        switch (_vtype)                                                                 \
        {                                                                               \
            __Case(int8,    _kind)                                                      \
            __Case(uint8,   _kind)                                                      \
            __Case(int16,   _kind)                                                      \
            __Case(uint16,  _kind)                                                      \
            __Case(int32,   _kind)                                                      \
            __Case(uint32,  _kind)                                                      \
            __Case(int64,   _kind)                                                      \
            __Case(uint64,  _kind)                                                      \
            __Case(float,   _kind)                                                      \
            __Case(double,  _kind)                                                      \
            __Case(bool,    _kind)                                                      \
            __Case(char,    _kind)                                                      \
                                                                                        \
            default:    return nullptr;                                                 \
        }

    // Returns max value of vtype_t.
    const void * get_max_value(vtype_t vtype)
    {
        __Switch(vtype, max);
    }

    // Returns min value of vtype_t.
    const void * get_min_value(vtype_t vtype)
    {
        __Switch(vtype, min);
    }

    #undef __Switch
    #undef __Case

    //-------- ---------- ---------- ---------- ----------

    // Converts value_type (defines in common module) to vtype.
    vtype_t to_vtype(value_type_t value_type) _NE
    {
        if ((int)value_type >= 0 && (int)value_type <= (int)value_type_t::__end__)
            return (vtype_t)value_type;

        return vtype_t::__unknown__;
    }

    // Returns whether a vtype can convert to another type
    // E.g. int can convert to long.
    bool is_vtype_compatible(vtype_t from, vtype_t to) _NE
    {
        if (from == to)
            return true;

        if (!is_numberic_vtype(from) || !is_numberic_vtype(to))
            return false;

        if ((int)to < (int)from)
            return false;

        if (is_float_vtype(from))
            return is_float_vtype(to);

        return is_signed_vtype(from) == is_signed_vtype(to);
    }

    // Returns whether the vtype is between v1 and v2.
    X_ALWAYS_INLINE static constexpr bool __between(vtype_t v, vtype_t v1, vtype_t v2) _NE
    {
        return (int)v >= (int)v1 && (int)v <= (int)v2;
    }

    // Returns whether a vtype is a integer type.
    bool is_integer_vtype(vtype_t vtype) _NE
    {
        return __between(vtype, vtype_t::int8_, vtype_t::uint64_);
    }

    // Returns whether a vtype is a float type.
    bool is_float_vtype(vtype_t vtype) _NE
    {
        return __between(vtype, vtype_t::float_, vtype_t::double_);
    }

    // Returns whether a vtype is a numberic.
    bool is_numberic_vtype(vtype_t vtype) _NE
    {
        return __between(vtype, vtype_t::int8_, vtype_t::double_);
    }

    // Returns whether a vtype is signed.
    bool is_signed_vtype(vtype_t vtype) _NE
    {
        return is_float_vtype(vtype) ||
            (is_integer_vtype(vtype) && (int)vtype % 2 == (int)vtype_t::int8_ % 2);
    }

    // Returns whether a vtype is unsigned.
    bool is_unsigned_vtype(vtype_t vtype) _NE
    {
        return is_integer_vtype(vtype) && (int)vtype % 2 == (int)vtype_t::uint8_ % 2;
    }

    // Returns default value of a vtype.
    cvalue_t default_value_of(vtype_t vtype) _NE
    {
        switch (vtype)
        {
            case vtype_t::int8_:
                return cvalue_t((int8_t)0);

            case vtype_t::uint8_:
                return cvalue_t((uint8_t)0);

            case vtype_t::int16_:
                return cvalue_t((int16_t)0);

            case vtype_t::uint16_:
                return cvalue_t((uint16_t)0);

            case vtype_t::int32_:
                return cvalue_t((int32_t)0);

            case vtype_t::uint32_:
                return cvalue_t((uint32_t)0);

            case vtype_t::int64_:
                return cvalue_t((int64_t)0);

            case vtype_t::uint64_:
                return cvalue_t((uint64_t)0);

            case vtype_t::float_:
                return cvalue_t((float)0);

            case vtype_t::double_:
                return cvalue_t((double)0);

            case vtype_t::bool_:
                return cvalue_t(false);

            case vtype_t::char_:
                return cvalue_t((char_t)_T('\0'));

            case vtype_t::string_:
            case vtype_t::ptr_:
            case vtype_t::mobject_:
            case vtype_t::void_:
            default:
                return cvalue_t(nullptr);
        }
    }

    //-------- ---------- ---------- ---------- ----------

    // Mobject type.
    X_ENUM_INFO(mtype_t)

        X_C(tuple,          _T("tuple"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Category of a type.
    X_ENUM_INFO(ttype_t)

        X_C(class_,         _T("class"))

        X_C(struct_,        _T("struct"))

        X_C(interface_,     _T("interface"))

        X_C(enum_,          _T("enum"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Kind of a type.
    X_ENUM_INFO(gtype_t)

        X_C(general,        _T("general"))

        X_C(generic,        _T("generic"))

        X_C(generic_param,  _T("generic_param"))

        X_C(array,          _T("array"))

        X_C(type_def_param, _T("type_def_param"))

        X_C(null,           _T("null"))

    X_ENUM_INFO_END
    
    ////////// ////////// ////////// ////////// //////////
    // member_type_t

    // Member type.
    X_ENUM_INFO(member_type_t)

        X_C(field,      _T("field"))

        X_C(method,     _T("method"))

        X_C(property,   _T("property"))

        X_C(event,      _T("event"))

        X_C(type,       _T("type"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Generic argument type
    X_ENUM_INFO(generic_arg_type_t)

        X_C(ref,        _T("ref"))

        X_C(out,        _T("out"))

        X_C(params,     _T("params"))

    X_ENUM_INFO_END

    generic_arg_t::operator string_t() const
    {
        if (atype == generic_arg_type_t::__default__)
            return _str(type_name);

        return _F(_T("%s %s"), atype, type_name);
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_constraint_type_t

    // Generic constraint type.
    X_ENUM_INFO(generic_constraint_ttype_t)

        X_C(class_,     _T("class"))

        X_C(struct_,    _T("struct"))

        X_C(new_,       _T("new"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Collect super types.
    void __collect_super_types(type_t * type, std::set<type_t *> & super_types)
    {
        type->each_super_type([&super_types](type_t * super_type) {

            if (super_types.insert(super_type).second)
                __collect_super_types(super_type, super_types);

            return true;
        });
    }

    // Returns whether the type has specified super type.
    bool type_t::has_super_type(type_t * super)
    {
        _A(super != nullptr);

        if (__super_types.empty())
            __collect_super_types(this, __super_types);

        return __super_types.find(super) != __super_types.end();
    }

    // Returns base type.
    type_t * type_t::get_base_type()
    {
        return nullptr;
    }

    // Commits it.
    void type_t::commit(eobject_commit_context_t & ctx)
    {
        // Do nothing.
    }

    ////////// ////////// ////////// ////////// //////////

    #define __FindMemberAndRet(members, name)                                           \
        do                                                                              \
        {                                                                               \
            auto r = members.get(name);                                                 \
            if (r != nullptr)                                                           \
                return r;                                                               \
        } while (false)

    //-------- ---------- ---------- ---------- ----------

    // Method compare results.
    X_ENUM(__member_compare_t)
        not_match,                  // Not match, at least one argument are not matched.
        match,                      // Matched, but not all arguments are equals.
        equals,                     // Matched, All arguments are equals.
        same_name,                  // Matched, has generic params and no arguments.
        auto_determined,            // Matched, Generic params auto determined.
        auto_determined_failed,     // Not matched, failed to determine generic params.
    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Method compare results.
    X_ENUM_INFO(__member_compare_t)
        X_C(not_match,          _T("not_match"))
        X_C(match,              _T("match"))
        X_C(equals,             _T("equals"))
        X_C(same_name,          _T("same_name"))
        X_C(auto_determined,    _T("auto_determined"))
        X_C(auto_determined_failed, _T("auto_determined_failed"))
    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Constructor of atype_t.
    atype_t::atype_t(typex_t typex) _NE
        : type((type_t *)typex), ptype((param_type_t)typex)
    { }

    //-------- ---------- ---------- ---------- ----------

    // Returns whether a type is a super type of specified type, includes interfaces.
    bool is_super_type(type_t * drived, type_t * super)
    {
        if (drived == nullptr)
            return false;

        if (super == nullptr)
            return true;

        return drived->has_super_type(super);
    }

    // Returns whether a type is a a mobject.
    static bool __is_mobject(type_t * type)
    {
        return type == nullptr || type->this_vtype() == vtype_t::mobject_;
    }

    // Returns whether two type are compatiable.
    // Determines whether a type can convert to another type.
    // E.g. int32 -> int64. System.String -> System.Object.
    bool is_type_compatible(type_t * from, type_t * to)
    {
        _A(from != nullptr);
        _A(to != nullptr);

        if (from == to)
            return true;

        if (from->this_gtype() == gtype_t::null)
            return __is_mobject(from) || from->this_gtype() == gtype_t::null;

        if (to->this_gtype() == gtype_t::null)
            return __is_mobject(to) || from->this_gtype() == gtype_t::null;

        if (__is_mobject(from) || __is_mobject(to))
            return is_super_type(from, to);

        return is_vtype_compatible(from->this_vtype(), to->this_vtype());
    }

    // Argument type analyzer.
    class __argument_type_analyzer_t
    {
    public:
        __argument_type_analyzer_t(params_t * params) : __params(params) { }

        __argument_type_analyzer_t(method_t * method, generic_args_t * generic_args)
            : __generic_args(generic_args)
        {
            _A(method != nullptr);

            __params = method->params;

            if (generic_args != nullptr)
            {
                if (generic_args->size() != method->generic_param_count())
                    throw _ECF(argument_error, _T("invalid generic argument count"));

                for (int index = 0, count = method->generic_param_count();
                                                    index < count; index++)
                {
                    generic_arg_t * ga = generic_args->at(index);
                    _A(ga != nullptr);

                    generic_param_t * gp = method->generic_params->at(index);
                    _A(gp != nullptr);

                    __generic_param_map[gp->name] = ga->get_type();
                }
            }
        }

        // Gets real type of specified generic type name.
        type_t * get_type(name_t name)
        {
            if (auto it = __generic_param_map.find(name); it != __generic_param_map.end())
                return it->second;

            return nullptr;
        }

        // Gets real type of specified generic param.
        type_t * get_type(generic_param_t * gp)
        {
            _A(gp != nullptr);

            return get_type(gp->name);
        }

        // Compare to specified arguments.
        __member_compare_t compare(atypes_t * atypes)
        {
            size_t atype_count = al::size_of(atypes);
            size_t param_count = al::size_of(__params);

            if (atype_count != param_count)
                return __member_compare_t::not_match;

            __member_compare_t r = __member_compare_t::equals;
            for (size_t index = 0; index < param_count; index++)
            {
                param_t * param = __params->at(index);

                switch (__compare_argument(param, atypes->at(index)))
                {
                    case __member_compare_t::not_match:
                    case __member_compare_t::auto_determined_failed:
                        return __member_compare_t::not_match;

                    case __member_compare_t::match:
                        if (r != __member_compare_t::auto_determined)
                            r = __member_compare_t::match;
                        break;

                    case __member_compare_t::auto_determined:
                        r = __member_compare_t::auto_determined;
                        break;

                    case __member_compare_t::equals:
                        break;

                    default:
                        X_UNEXPECTED_F("unexpected member compare type");
                }
            }

            return r;
        }

    private:
        generic_args_t * __generic_args = nullptr;
        params_t * __params;
        std::map<name_t, type_t *> __generic_param_map;

        typedef std::map<generic_param_t *, type_t *> __generic_map_t;

        struct __matched_item_t
        {
            generic_type_t *    type;
            __generic_map_t     map;
        };

        // Compare.
        __member_compare_t __compare_argument(param_t * param, atype_t atype)
        {
            if (param == nullptr || atype.type == nullptr)
                return __member_compare_t::not_match;

            if (param->ptype != atype.ptype)
                return __member_compare_t::not_match;

            type_t * param_type = param->get_type();
            _A(param_type != nullptr);

            type_t * argument_type = atype.type;
            _A(argument_type != nullptr);

            __member_compare_t r;
            param_type = __to_real_type(param_type, argument_type, &r);

            if (r != __member_compare_t::__default__)
                return r;

            if (param_type == argument_type)
                return __member_compare_t::equals;

            if (param->ptype == param_type_t::ref)
                return __member_compare_t::not_match;

            if (param->ptype == param_type_t::out)
            {
                if (is_type_compatible(param_type, argument_type))
                    return __member_compare_t::match;
            }
            else
            {
                if (is_type_compatible(argument_type, param_type))
                    return __member_compare_t::match;
            }

            return __member_compare_t::not_match;
        }

        // Converts to a real type.
        type_t * __to_real_type(type_t * param_type, type_t * argument_type,
                                                __member_compare_t * out_r = nullptr)
        {
            al::assign_value(out_r, __member_compare_t::__default__);

            // Converts generic param.
            if (is_generic_param(param_type))
            {
                generic_param_t * gp = (generic_param_t *)param_type;
                if (type_t * param_type1 = get_type(gp); param_type1 != nullptr)
                {
                    return param_type1;
                }
                else
                {
                    al::assign_value(out_r, __member_compare_t::auto_determined);
                    __generic_param_map[gp->name] = argument_type;

                    return argument_type;
                }
            }

            // Generic types.
            if (has_generic_params(param_type))
            {
                generic_type_t * gp_type = (generic_type_t *)param_type;

                // Call in generic methods.
                if (is_general(argument_type) &&
                    is_type_compatible((general_type_t *)argument_type, gp_type->template_))
                {
                    return argument_type;
                }

                // Generic param types specified.
                if (__generic_args != nullptr)
                {
                    generic_type_t * type = __fill_generic_type(gp_type, __generic_args);
                    return type;
                }

                // Auto determine generic param types.
                {
                    general_type_t * gp_template = gp_type->template_;
                    std::vector<__matched_item_t> matched_items;

                    __each_types(argument_type, [&](type_t * type) -> bool {

                        if (is_generic(type) && ((generic_type_t *)type)->template_ == gp_template)
                        {
                            __generic_map_t map;
                            generic_type_t * ga_type = (generic_type_t *)type;
                            if (__determine_generic_param_types(gp_type, ga_type, map))
                            {
                                matched_items.push_back(__matched_item_t { ga_type, map });
                            }
                        }

                        return true;

                    });

                    if (matched_items.size() != 1)
                    {
                        al::assign_value(out_r, __member_compare_t::auto_determined_failed);
                        return nullptr;
                    }

                    __matched_item_t & matched_item = matched_items[0];

                    for (auto & it : matched_item.map)
                    {
                        generic_param_t * gp = it.first;
                        type_t * type = it.second;

                        __generic_param_map[gp->name] = type;
                    }

                    return matched_item.type;
                }
            }

        __end:
            return param_type;
        }

        generic_type_t * __fill_generic_type(generic_type_t * type, generic_args_t * generic_args)
        {
            _A(generic_args != nullptr);

            general_type_t * template_ = type->template_;
            
            type_collection_t types;
            for (type_t * arg_type : type->args)
            {
                if (is_generic_param(arg_type))
                {
                    generic_param_t * gp = (generic_param_t *)arg_type;
                    type_t * type = get_type(gp);
                    _A(type != nullptr);

                    types.push_back(type);
                }
                else if (is_generic(arg_type))
                {
                    type_t * type = __fill_generic_type((generic_type_t *)arg_type, generic_args);
                    types.push_back(type);
                }
                else
                {
                    types.push_back(arg_type);
                }
            }

            return __XPool.new_generic_type(template_, types, type->host_type);
        }

        // Determine generic param types.
        bool __determine_generic_param_types(generic_type_t * param_type,
                             generic_type_t * argument_type, __generic_map_t & map)
        {
            if (param_type->template_ != argument_type->template_)
                return false;

            _A(param_type->args.size() == argument_type->args.size());

            for (int index = 0, count = param_type->argument_count(); index < count; index++)
            {
                type_t * param_arg_type = param_type->args[index];
                if (is_generic_param(param_arg_type))
                {
                    generic_param_t * gp = (generic_param_t *)param_arg_type;
                    type_t * ptype = get_type(gp);

                    decltype(std::begin(map)) it;
                    if (ptype == nullptr && (it = map.find(gp)) != std::end(map))
                        ptype = it->second;

                    type_t * atype = argument_type->args[index];

                    if (ptype == nullptr)
                        map[gp] = atype;
                    else if (ptype != atype)
                        return false;
                }
                else if (is_generic(param_arg_type))
                {
                    generic_type_t * gp_type = (generic_type_t *)param_arg_type;
                    type_t * atype = argument_type->args[index];

                    if (!is_generic(atype) ||
                        !__determine_generic_param_types(gp_type, (generic_type_t *)atype, map))
                        return false;
                }
            }

            return true;
        }

        // Enum all super types and itself.
        template<typename _f_t> void __each_types(type_t * type, _f_t f)
        {
            if (!f(type))
                return;

            type->each_super_type(f);
        }
    };

    // Returns compatible type of two types.
    static type_t * __get_compatible_type(type_t * type1, type_t * type2)
    {
        if (type1 == type2)
            return type1;

        if (type1 == nullptr || type2 == nullptr)
            return nullptr;

        vtype_t vtype1 = type1->this_vtype(), vtype2 = type2->this_vtype();
        if (is_numberic_vtype(vtype1) && is_numberic_vtype(vtype2))
            return (int)vtype1 > (int)vtype2? type1 : type2;

        if (type1->has_super_type(type2))
            return type2;

        if (type2->has_super_type(type1))
            return type1;

        return nullptr;
    }

    // Returns compatible type of types.
    static type_t * __get_compatible_type(svector_t<type_t *> & types)
    {
        if (types.empty())
            return nullptr;

        type_t * type = types[0];
        for (size_t index = 1, count = types.size(); index < count; index++)
        {
            type = __get_compatible_type(type, types[index]);
            if (type == nullptr)
                break;
        }

        return type;
    }

    // Compare method.
    static __member_compare_t __compare_method(method_t * method, generic_args_t * generic_args,
                            atypes_t * atypes, arg_types_t & out_arg_types)
    {
        if (atypes == nullptr)
        {
            if (generic_args == nullptr || method->generic_param_count() == generic_args->size())
                return __member_compare_t::same_name;

            return __member_compare_t::not_match;
        }

        size_t atype_count = atypes->size();
        size_t param_count = method->param_count();

        if (atype_count == 0)
            return param_count == 0? __member_compare_t::equals : __member_compare_t::not_match;

        if (atype_count != param_count)
            return __member_compare_t::not_match;

        __argument_type_analyzer_t analyzer(method, generic_args);
        __member_compare_t r = analyzer.compare(atypes);

        if (method->is_generic() && generic_args == nullptr)    // auto determined generic params.
        {
            for (generic_param_t * gp : *method->generic_params)
            {
                type_t * type = analyzer.get_type(gp);
                if (type == nullptr)
                    return __member_compare_t::auto_determined_failed;

                out_arg_types.push_back(type);
            }
        }

        return r;
    }

    // Returns whether it's a specifed vtype.
    X_ALWAYS_INLINE static bool __is_vtype(type_t * type, vtype_t vtype)
    {
        _A(type != nullptr);

        return type->this_gtype() == gtype_t::general
            && ((general_type_t *)type)->vtype == vtype;
    }

    // Returns whether it's a void type.
    bool is_void_type(type_t * type)
    {
        return __is_vtype(type, vtype_t::void_);
    }

    // Returns whether it's a string type.
    bool is_string_type(type_t * type)
    {
        return __is_vtype(type, vtype_t::string_);
    }

    // Returns whether it's a pointer type.
    bool is_ptr_type(type_t * type)
    {
        return __is_vtype(type, vtype_t::ptr_);
    }

    // Returns whether a type is user defined class, struct or string.
    bool is_object(type_t * type)
    {
        _A(type != nullptr);

        return is_object(type->this_vtype());
    }

    // Returns whether a type is class, interface or null.
    bool is_ref_type(type_t * type)
    {
        _A(type != nullptr);

        if (type->this_gtype() == gtype_t::null)
            return true;

        return is_ref_type(type->this_ttype());
    }

    // Returns whether a type is struct.
    bool is_value_type(type_t * type)
    {
        _A(type != nullptr);

        return is_value_type(type->this_ttype());
    }

    // Returns whether a type is System.Array<T> type.
    bool is_tarray(type_t * type)
    {
        return is_generic(type) &&
            ((generic_type_t *)type)->template_ == __XPool.get_tarray_type();
    }

    // Returns whether it's a generic type with generic params.
    bool has_generic_params(type_t * type)
    {
        if (!is_generic(type))
            return false;

        generic_type_t * generic_type = (generic_type_t *)type;
        for (type_t * arg_type : generic_type->args)
        {
            if (is_generic_param(arg_type))
                return true;

            if (has_generic_params(arg_type))
                return true;
        }

        return false;
    }

    // Returns general type if it's a generic type, otherwise, returns itself.
    type_t * to_general(type_t * type)
    {
        if (type == nullptr)
            return nullptr;

        switch (type->this_gtype())
        {
            case gtype_t::generic:
                return ((generic_type_t *)type)->template_;

            case gtype_t::array:
                return to_general(((array_type_t *)type)->base_type);

            default:
                return type;
        }
    }

    // Returns whether it's a custom defined struct type.
    bool is_custom_struct(type_t * type)
    {
        return type != nullptr && type->this_ttype() == ttype_t::struct_
                    && type->this_vtype() == vtype_t::mobject_;
    }

    // Returns whether it's a custom defined struct type or class type.
    bool is_custom_class_or_struct(type_t * type)
    {
        ttype_t ttype;

        return type != nullptr
            && type->this_vtype() == vtype_t::mobject_
            && ( (ttype = type->this_ttype()) == ttype_t::struct_ || ttype == ttype_t::class_ );
    }

    // Returns whether it's a tuple type.
    bool is_tuple(type_t * type)
    {
        return type == __XPool.get_tuple_type() || type == __XPool.get_ituple_type();
    }

    ////////// ////////// ////////// ////////// //////////
    // analyze_member_args_t

    string_t analyze_member_args_t::get_name() const
    {
        if (name != name_t::null)
            return name;

        switch (method_trait)
        {
            case method_trait_t::constructor:
                return _T("constructor");

            case method_trait_t::destructor:
                return _T("destructor");

            case method_trait_t::static_constructor:
                return _T("static constructor");

            default:
                return _T("?");
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // Method analyzer.

    typedef typename __general_type_like_base_t::method_list_t   __method_list_t;
    typedef typename __general_type_like_base_t::property_list_t __property_list_t;

    // Method analyzer.
    class __method_analyzer_t
    {
        typedef __method_analyzer_t __self_t;

    public:

        // Constructor.
        __method_analyzer_t(__method_list_t & methods, analyze_member_args_t & args)
            : __methods(methods), __args(args) { }

        // Analyze methods.
        void analyze()
        {
            using namespace std::placeholders;
            auto analyze_method = std::bind(&__self_t::__analyze, this, _1);

            size_t generic_args_count = __generic_args_count();

            if (__args.method_trait == method_trait_t::__default__)
                __methods.each(__args.name, generic_args_count, analyze_method);
            else
                __methods.each(__args.method_trait, analyze_method);

            if (__not_found() && generic_args_count == 0)
            {
                __methods.each(__args.name, __unknown_size, [this](method_t * method) {
                    if (method->generic_param_count() > 0)
                        return __analyze(method);
                    return true;
                });
            }
        }

        // Gets method descripted by __args.
        method_t * get_method(bool exact_match = false)
        {
            #define __Throw(_error_code, _methods...)                                   \
                throw method_compare_error_t(method_compare_error_code_t::_error_code,  \
                                                                        ##_methods)

            if (__equaled_method.method != nullptr)
                return __pick_method(__equaled_method);

            if (exact_match)
            {
                if (__args.throw_on_not_found)
                    __Throw(not_match, __matched_methods.empty()? __not_matched_methods :
                                                                  __matched_methods);
                return nullptr;
            }

            if (__matched_methods.size() >= 2)
                __Throw(conflict, __matched_methods);

            if (!__matched_methods.empty())
                return __pick_method(__matched_methods[0]);

            if (!__determine_failed_methods.empty())
                __Throw(auto_determined_failed, __determine_failed_methods);
 
            if (__args.throw_on_not_found)
                __Throw(not_match, __not_matched_methods);

            return nullptr;

            #undef __Throw
        }

        // Gets method descripted by __args.
        void get_methods(members_t & out_members)
        {
            if (__equaled_method.method != nullptr)
                out_members.push_back(__equaled_method.method);

            for (__mi_t & mi : __matched_methods)
                out_members.push_back(mi.method);
        }

    private:
        __method_list_t &       __methods;
        analyze_member_args_t & __args;
        static const size_t __empty_size = (size_t)-1;

        struct __mi_t
        {
            method_t * method;
            size_t ga_index = __empty_size;

            operator method_t * () { return method; }
        };

        __mi_t __equaled_method { nullptr };
        typedef al::svector_t<__mi_t, 16> __methods_vector_t;
        __methods_vector_t  __matched_methods;
        __methods_vector_t  __determine_failed_methods;
        __methods_vector_t  __not_matched_methods;

        struct __arg_types_data_t
        {
            byte_t data[sizeof(arg_types_t)];
        };

        al::svector_t<__arg_types_data_t, 8> __arg_types_list;

        // Analyze method.
        bool __analyze(method_t * method)
        {
            if (to_type(method->owner_type_name) != __args.owner)
                return false;

            __member_compare_t compare_result = __compare_method(
                method, __args.generic_args, __args.atypes, *__next_arg_types()
            );

            switch (compare_result)
            {
                case __member_compare_t::equals:
                case __member_compare_t::auto_determined:
                    __equaled_method = __mi_t { method, __current_arg_types_index() };
                    break;

                case __member_compare_t::match:
                case __member_compare_t::same_name:
                    __matched_methods.push_back(__mi_t { method, __current_arg_types_index() });
                    break;

                case __member_compare_t::not_match:
                    __not_matched_methods.push_back(__mi_t { method, __current_arg_types_index() });
                    break;

                case __member_compare_t::auto_determined_failed:
                    __determine_failed_methods.push_back(__mi_t {
                        method, __current_arg_types_index()
                    });
                    break;

                default:
                    X_UNEXPECTED();
            }

            return true;
        }

        // Pick method. revises ga_index to a really type.
        method_t * __pick_method(__mi_t & mi)
        {
            if (mi.ga_index != __empty_size)
            {
                arg_types_t * arg_types = __arg_types_at(mi.ga_index);
                al::copy(*arg_types, std::back_inserter(__args.out_arg_types));
            }

            return mi.method;
        }

        // Returns argument types at specified index.
        arg_types_t * __arg_types_at(size_t index)
        {
            return (arg_types_t *)&__arg_types_list[index];
        }

        // Returns last argument types.
        arg_types_t * __last_arg_types()
        {
            return __arg_types_at(__arg_types_list.size() - 1);
        }

        // Returns next argument types.
        arg_types_t * __next_arg_types()
        {
            if (__arg_types_list.empty() || !__last_arg_types()->empty())
            {
                void * data = &__arg_types_list.push_back(__arg_types_data_t());
                return new (data) arg_types_t();
            }

            return __last_arg_types();
        }

        // Returns current argument types idnex.
        size_t __current_arg_types_index()
        {
            if (__arg_types_list.empty() || __last_arg_types()->empty())
                return __empty_size;

            return __arg_types_list.size() - 1;
        }

        // Returns generic argument count.
        size_t __generic_args_count() const
        {
            return __args.generic_args? __args.generic_args->size() : 0;
        }

        // Returns whether no matched method.
        bool __not_found() const
        {
            return __equaled_method.method == nullptr && __matched_methods.empty();
        }
    };

    // Analyze methods with specified args.
    method_t * __analyze_method(__method_list_t & methods, analyze_member_args_t & args)
    {
        __method_analyzer_t analyzer(methods, args);
        analyzer.analyze();

        return analyzer.get_method(args.exact_match);
    }

    // Analyze methods with specified args.
    void __analyze_methods(__method_list_t & methods, analyze_members_args_t & args)
    {
        __method_analyzer_t analyzer(methods, args);
        analyzer.analyze();

        analyzer.get_methods(args.out_members);
    }

    ////////// ////////// ////////// ////////// //////////

    // Property index analyzer.
    class __property_index_analyzer_t
    {
        typedef __property_index_analyzer_t __self_t;

    public:

        // Constructor.
        __property_index_analyzer_t(__property_list_t & properties, analyze_member_args_t & args)
            : __properties(properties), __args(args) { }

        // Analyze properties.
        void analyze()
        {
            __arg_count = __args.atypes? __args.atypes->size() : 0;

            for (property_t * property : __properties)
            {
                __analyzer(property);
            }
        }

        property_t * get_property(bool exact_match = false)
        {
            if (__equaled_property.property != nullptr)
                return __pick_property(__equaled_property);

            if (exact_match)
                return nullptr;

            if (__matched_properties.size() >= 2)
                throw _EC(conflict, _T("property index conflict"));

            if (!__matched_properties.empty())
                return __pick_property(__matched_properties[0]);

            return nullptr;
        }

        void get_properties(members_t & out_members)
        {
            if (__equaled_property.property != nullptr)
                out_members.push_back(__equaled_property.property);

            for (pi_t & pi : __matched_properties)
                out_members.push_back(pi.property);
        }

    private:
        __property_list_t &     __properties;
        analyze_member_args_t & __args;
        size_t                  __arg_count;

        struct pi_t { property_t * property; };
        typedef al::svector_t<pi_t, 5> __properties_vector_t;
        __properties_vector_t __matched_properties;
        pi_t __equaled_property { nullptr };

        // Analyzer property.
        void __analyzer(property_t * property)
        {
            if (to_type(property->owner_type_name) != __args.owner)
                return;

            if (property->name != __get_property_index_name() || property->params == nullptr)
                return;

            __member_compare_t compare_result = __compare_arguments(property->params);
            switch (compare_result)
            {
                case __member_compare_t::equals:
                    __equaled_property = pi_t { property };
                    break;

                case __member_compare_t::match:
                    __matched_properties.push_back(pi_t { property });
                    break;

                case __member_compare_t::not_match:
                    break;

                default:
                    X_UNEXPECTED_F("unexpected __method_compare_t::%1%", compare_result);
            }
        }

        // Compare arguments.
        __member_compare_t __compare_arguments(params_t * params)
        {
            size_t param_count = params->size();

            if (param_count != __arg_count)
                return __member_compare_t::not_match;

            __argument_type_analyzer_t analyzer(params);
            __member_compare_t r = analyzer.compare(__args.atypes);

            return r;
        }

        // Picks property.
        property_t * __pick_property(pi_t & pi)
        {
            return pi.property;
        }

        // Property index name.
        static name_t __get_property_index_name()
        {
            static name_t static_name;

            if (static_name == name_t::null)
                static_name = __XPool.to_name(PropertyIndexName);

            return static_name;
        }
    };

    // Analyze properties with speicified args.
    property_t * __analyze_property_index(__property_list_t & properties,
                                            analyze_member_args_t & args)
    {
        __property_index_analyzer_t analyzer(properties, args);
        analyzer.analyze();

        return analyzer.get_property(args.exact_match);
    }

    // Analyze properties with speicified args.
    void __analyze_property_indexes(__property_list_t & properties,
                                            analyze_members_args_t & args)
    {
        __property_index_analyzer_t analyzer(properties, args);
        analyzer.analyze();

        analyzer.get_properties(args.out_members);
    }

    ////////// ////////// ////////// ////////// //////////
    // Base class __general_type_like_base_t.

    // Returns members descripted by args.
    member_t * __general_type_like_base_t::get_member(analyze_member_args_t & args)
    {
        if (bit_has_flag(args.member_type, member_type_t::field))
            __FindMemberAndRet(fields, args.name);

        if (bit_has_flag(args.member_type, member_type_t::property))
        {
            if (args.name != name_t::null)       // Normal property
                __FindMemberAndRet(properties, args.name);

            // Index.
            property_t * property = __analyze_property_index(properties, args);
            if (property != nullptr)
                return property;
        }

        if (bit_has_flag(args.member_type, member_type_t::event))
            __FindMemberAndRet(events, args.name);

        if (bit_has_flag(args.member_type, member_type_t::type_def))
            __FindMemberAndRet(type_defs, args.name);

        if (bit_has_flag(args.member_type, member_type_t::type))
        {
            type_t * type = nest_types.get(args.name, args.generic_args_count);
            if (type != nullptr)
                return type;
        }

        if (bit_has_flag(args.member_type, member_type_t::method))
        {
            method_t * method = __analyze_method(methods, args);
            if (method != nullptr)
                return method;

            if (args.method_trait == method_trait_t::constructor
                    || args.method_trait == method_trait_t::static_constructor
                    || args.method_trait == method_trait_t::destructor)
                return nullptr;
        }

        return nullptr;
    }

    // Returns memeber descripted by args.
    void __general_type_like_base_t::get_members(analyze_members_args_t & args)
    {
        if (bit_has_flag(args.member_type, member_type_t::field))
        {
            if (args.name == name_t::null)
            {
                al::copy(fields, std::back_inserter(args.out_members));
            }
            else
            {
                field_t * field = fields.get(args.name);
                if (field != nullptr)
                    args.out_members.push_back(field);
            }
        }

        if (bit_has_flag(args.member_type, member_type_t::property))
        {
            if (args.name == name_t::null)
            {
                if (args.atypes == nullptr || args.atypes->size() == 0)
                    al::copy(properties, std::back_inserter(args.out_members));
                else
                    __analyze_property_indexes(properties, args);
            }
            else
            {
                property_t * property = properties.get(args.name);
                if (property != nullptr)
                    args.out_members.push_back(property);
            }
        }

        if (bit_has_flag(args.member_type, member_type_t::method))
        {
            __analyze_methods(methods, args);
        }
    }

    // Returns all members.
    void __general_type_like_base_t::get_all_members(members_t & out_members)
    {
        al::copy(methods,    std::back_inserter(out_members));
        al::copy(properties, std::back_inserter(out_members));
        al::copy(fields,     std::back_inserter(out_members));
        al::copy(events,     std::back_inserter(out_members));
        al::copy(nest_types, std::back_inserter(out_members));
    }

    template<typename _members_t, typename _f_t,
        typename _member_t = std::remove_pointer_t<typename _members_t::value_type>
    >
    _member_t * __search_member_(_members_t & members, name_t name, _f_t f)
    {
        return members.each(name, [&f](_member_t * member1) {
            return !f(member1);
        });
    }

    template<typename _members_t, typename _f_t,
        typename _member_t = std::remove_pointer_t<typename _members_t::value_type>
    >
    _member_t * __search_member(_members_t & members, name_t name, _member_t * member,
        type_t * owner_type, _f_t f)
    {
        bool found_self = false;
        return __search_member_(members, name,
                                [member, &f, &found_self, owner_type](_member_t * member1) {
            if (member == member1)
            {
                found_self = true;
                return false;
            }

            if (get_owner_type(member1) != owner_type)
                return false;

            return !found_self && f(member1);

        });
    }

    bool __is_params_equals(params_t * params1, params_t * params2)
    {
        if (params1 == nullptr)
            return params2 == nullptr || params2->empty();

        if (params2 == nullptr)
            return params1 == nullptr || params1->empty();

        if (params1->size() != params2->size())
            return false;

        for (size_t size = params1->size(), index = 0; index < size; index++)
        {
            param_t * p1 = (*params1)[index], * p2 = (*params2)[index];
            _A(p1 != nullptr && p2 != nullptr);

            type_t * type1 = p1->get_type(), * type2 = p2->get_type();
            _A(type1 != nullptr && type2 != nullptr);

            if (type1 != type2)
                return false;

            if (p1->ptype != p2->ptype)
                return false;
        }

        return true;
    }

    bool __is_params_equals(params_t * params1, generic_params_t * gparams1,
        params_t * params2, generic_params_t * gparams2)
    {
        #define __Size(_arg)    ((_arg) == nullptr? 0 : (_arg)->size())

        size_t params1_size = __Size(params1), params2_size = __Size(params2);
        if (params1_size != params2_size)
            return false;

        size_t gparams1_size = __Size(gparams1), gparams2_size = __Size(gparams2);
        if (gparams1_size != gparams2_size)
            return false;

        if (params1_size == 0)
            return true;

        for (size_t index = 0; index < params1_size; index++)
        {
            param_t * p1 = (*params1)[index], * p2 = (*params2)[index];
            _A(p1 != nullptr && p2 != nullptr);

            type_t * type1 = p1->get_type(), * type2 = p2->get_type();
            _A(type1 != nullptr && type2 != nullptr);

            if (type1->this_gtype() == gtype_t::generic_param)
            {
                if (type1->this_gtype() != gtype_t::generic_param ||
                    ((generic_param_t *)type1)->index != ((generic_param_t *)type2)->index)
                    return false;
            }
            else
            {
                if (type1 != type2)
                    return false;
            }
        }

        return true;

        #undef __Size
    }

    // Checks whether it's duplicate.
    member_t * __general_type_like_base_t::check_duplicate(member_t * member)
    {
        _A(member != nullptr);

        name_t name = member->get_name();
        type_t * owner_type = get_owner_type(member);

        #define __Search(_members)                                                      \
            __search_member(_members, name, member, owner_type, [](auto) { return true; })

        #define __SearchRet(_members)                                                   \
            do {                                                                        \
                if (auto member1 = __Search(_members); member1 != nullptr)              \
                    return member1;                                                     \
            } while (0)
        
        // Any member cannot duplicate with fields and events.
        __SearchRet(fields);
        __SearchRet(events);
        __SearchRet(nest_types);
        __SearchRet(type_defs);

        // Check property
        switch (member->this_type())
        {
            case member_type_t::field:
            case member_type_t::event:
            case member_type_t::type:
            case member_type_t::type_def:
                __SearchRet(methods);
                __SearchRet(properties);
                return nullptr;

            case member_type_t::method:
                __SearchRet(properties);
                
                // Check method duplicate.
                return __search_member(methods, name, (method_t *)member, owner_type,
                                                                [&](method_t * method) {
                    return __is_params_equals(
                        ((method_t *)member)->params, ((method_t *)member)->generic_params,
                        method->params, method->generic_params
                    );
                });

            case member_type_t::property:
                __SearchRet(methods);

                // Check property duplicate.
                return __search_member(properties, name, (property_t *)member, owner_type,
                                                            [&](property_t * property) {
                    return __is_params_equals(((property_t *)member)->params, property->params);
                });

            default:
                return nullptr;
        }

        #undef __Search
        #undef __SearchRet
    }

    // Returns vsize.
    msize_t __general_type_like_base_t::get_vsize()
    {
        __ensure_size_initialize();
        return __value_size;
    }

    // Returns count of reference types.
    msize_t __general_type_like_base_t::get_rcount()
    {
        __ensure_size_initialize();
        return __rcount;
    }

    // Returns base type.
    type_t * __general_type_like_base_t::get_base_type()
    {
        if (is_enum(this))
            return __XPool.get_enum_type();

        if (is_empty(super_type_names))
            return nullptr;

        type_t * base_type = super_type_names[0]->type;
        _A(base_type != nullptr);

        if (base_type->this_ttype() == ttype_t::class_)
            return base_type;

        return nullptr;
    }

    // Returns underlying type of enum type.
    vtype_t __general_type_like_base_t::get_underlying_vtype()
    {
        _A(is_enum(this));

        if (super_type_names.size() == 0)
            return default_enum_underlying_vtype;

        type_t * underlying_type = to_type(super_type_names[0]);
        _A(underlying_type != nullptr);

        if (underlying_type->this_ttype() == ttype_t::struct_)  // must be value type.
            return underlying_type->this_vtype();

        return default_enum_underlying_vtype;
    }

    // Ensure size of the type initialized.
    void __general_type_like_base_t::__ensure_size_initialize()
    {
        if (__rcount != unknown_msize)
            return;

        __rcount = 0;
        __value_size = 0;

        if (is_enum(this))
        {
            __value_size = get_vtype_size(get_underlying_vtype());
            return;
        }

        field_t * value_fields[fields.size()];
        field_t ** value_fields_end = value_fields;

        for (field_t * field : fields)
        {
            if (field->is_ref_type())
                __rcount++;
            else
                *value_fields_end++ = field;
        }

        if (value_fields_end > value_fields)
        {
            const size_t align_size = 4;

            std::sort(value_fields, value_fields_end, [](field_t * f1, field_t * f2) {
                return f1->get_size() > f2->get_size();
            });

            for (field_t * f : _range(value_fields, value_fields_end))
            {
                msize_t size = f->get_size();

                if (size >= 4)
                {
                    f->offset = __value_size;
                    __value_size += _alignf(size, align_size);
                }
                else
                {
                    if (__value_size % align_size + size > align_size)
                        __value_size = _alignf(__value_size, align_size);

                    f->offset = __value_size;
                    __value_size += size;
                }
            }

            if (__value_size > align_size)
                __value_size = _alignf(__value_size, align_size);
        }
    }

    // Enum super types by applies all super types for calling callback function.
    type_t * __general_type_like_base_t::each_super_type(each_super_type_callback_t callback)
    {
        for (type_name_t * super_type_name : super_type_names)
        {
            type_t * type = super_type_name->type;
            if (type != nullptr)
            {
                if (!callback(type))
                    return type;
            }
        }

        return nullptr;
    }

    // Commits it.
    void __general_type_like_base_t::commit(eobject_commit_context_t & ctx)
    {
        if (!__committed)
        {
            __ensure_size_initialize();

            __super_t::commit(ctx);
            __committed = true;
        }
    }

    // Context of transform types or members.
    class __general_type_like_base_t::__transform_context_t : no_copy_ctor_t
    {
        typedef __transform_context_t __self_t;
        typedef __type_transform_t<__self_t> __this_type_transform_t;

    public:
        __transform_context_t(__general_type_like_base_t * owner_object,
                general_type_t * template_, type_collection_t & args)
            : template_(template_), __owner_object(owner_object), args(args)
            , __type_transform(*this) { }

        general_type_t    * template_;
        type_collection_t & args;

        // Adds a relation of members.
        void add_relation(void * a, void * b)
        {
            __relation_map[a] = b;
        }

        // Gets relation.
        template<typename t> t * get_relation(t * a)
        {
            auto it = __relation_map.find(a);
            if (it == __relation_map.end())
                return nullptr;

            return (t *)it->second;
        }

        type_t * operator() (name_t name)
        {
            return __type_at(name);
        }

        // Transform type.
        type_t * transform_type(type_t * type)
        {
            return __type_transform.transform_type(type);
        }

        // Transform type_name.
        type_name_t * transform_type_name(type_name_t * type_name)
        {
            return __type_transform.transform_type_name(type_name);
        }

    private:
        std::map<void *, void *>     __relation_map;
        __this_type_transform_t      __type_transform;
        type_t *                     __host_type;
        __general_type_like_base_t * __owner_object;

        // Returns type of specified name.
        type_t * __type_at(name_t name)
        {
            return __owner_object->__type_at(template_, args, name);
        }
    };

    // Commits generic type.
    void __general_type_like_base_t::__transform(__transform_context_t & tctx)
    {
        if (this->committed())
            return;

        general_type_t * template_ = tctx.template_;

        _A(template_ != nullptr);
        _A(template_->committed());

        // Tuple types.
        if (template_ == __XPool.get_tuple_type() || template_ == __XPool.get_ituple_type())
        {
            __transform_tuple_type(tctx);
        }
        else
        {
            __transform_members(tctx, template_->fields, fields);
            __transform_members(tctx, template_->methods, methods);
            __transform_members(tctx, template_->properties, properties);
            __transform_members(tctx, template_->events, events);
            __transform_members(tctx, template_->nest_types, nest_types);
            __transform_members(tctx, template_->type_defs, type_defs);
            __transform_members(tctx, template_->super_type_names, super_type_names);
        }
    }

    // Transform tuple type.
    void __general_type_like_base_t::__transform_tuple_type(__tctx_t & tctx)
    {
        general_type_t * template_ = tctx.template_;
        type_collection_t & args = tctx.args;

        xpool_t & xpool = __XPool;

        // Methods.
        // __transform_members(tctx, template_->methods, methods);
        for (method_t * method : template_->methods)
        {
            if (method->trait == method_trait_t::constructor && method->generic_param_count() > 0)
            {
                impl_method_t * new_method = xpool.new_obj<impl_method_t>();
                new_method->raw = method;

                new_method->owner_type_name = nullptr;
                new_method->type_name       = nullptr;
                new_method->generic_params  = nullptr;
                new_method->name            = method->name;
                new_method->decorate        = method->decorate;
                new_method->trait           = method->trait;
                new_method->host_type       = this;
                new_method->body            = method->body;
                new_method->variable        = xpool.new_obj<method_variable_t>(new_method);

                new_method->params = xpool.new_obj<params_t>();

                for (int index = 0, count = args.size(); index < count; index++)
                {
                    type_t * type = args[index];
                    string_t name = _F(_T("item%1%"), index + 1);

                    param_t * new_param = xpool.new_obj<param_t>();

                    new_param->name         = xpool.to_name(name.c_str());
                    new_param->attributes   = nullptr;
                    new_param->type_name    = xpool.new_obj<type_name_t>(type);
                    new_param->ptype        = param_type_t::__default__;
                    new_param->default_value = nullptr;

                    new_method->params->push_back(new_param);
                }

                tctx.add_relation(method, new_method);
                methods.push_back(new_method);
            }
            else
            {
                method_t * new_method = __transform_member(tctx, method);
                methods.push_back(new_method);
            }
        }

        // Fields.
        __transform_members(tctx, template_->fields, fields);
        for (int index = 0, count = args.size(); index < count; index++)
        {
            type_t * type = args[index];
            type_name_t * type_name = xpool.new_obj<type_name_t>(type);

            // field.
            if (template_ == xpool.get_tuple_type())
            {
                position_field_t * new_field = xpool.new_obj<position_field_t>();
                new_field->position     = index;
                new_field->name         = xpool.to_name(_FT("__item%1%", index + 1));
                new_field->type_name    = type_name;
                new_field->init_value   = nullptr;
                new_field->decorate     = xpool.to_decorate(decorate_value_t::private_readonly);
                new_field->host_type    = this;
                new_field->variable     = xpool.new_obj<field_variable_t>(new_field);

                fields.push_back(new_field);
            }

            // method.
            method_t * new_method   = xpool.new_obj<method_t>();
            new_method->name = xpool.to_name(_FT("get_Item%1%", index + 1));
            new_method->type_name   = type_name;
            new_method->decorate    = xpool.to_decorate(decorate_value_t::public_);
            new_method->host_type   = this;
            new_method->variable    = xpool.new_obj<method_variable_t>(new_method);

            methods.push_back(new_method);

            // property.
            property_t * new_property = xpool.new_obj<property_t>();
            new_property->name      = xpool.to_name(_FT("Item%1%", index + 1));
            new_property->type_name = type_name;
            new_property->decorate  = xpool.to_decorate(decorate_value_t::public_);
            new_property->host_type = this;
            new_property->variable  = xpool.new_obj<property_variable_t>(new_property);
            new_property->get_method = new_method;

            properties.push_back(new_property);
        }

        // Transform members (except fields & methods).
        __transform_members(tctx, template_->properties, properties);
        __transform_members(tctx, template_->events, events);
        __transform_members(tctx, template_->nest_types, nest_types);
        __transform_members(tctx, template_->type_defs, type_defs);
        __transform_members(tctx, template_->super_type_names, super_type_names);
    }

    // Finds real type of generic param.
    template<typename _pred_t, typename _f_t>
    bool __general_type_like_base_t::__find_type(general_type_t * template_, 
            const type_collection_t & args, _pred_t pred, _f_t f) const
    {
        if (template_ != nullptr && template_->params != nullptr && args.size() > 0)
        {
            generic_params_t & params = *template_->params;
            __search_type(params, args, pred, f);
        }

        type_t * type = this->host_type;
        while (type != nullptr)
        {
            if (type->this_gtype() == gtype_t::generic)
            {
                generic_type_t * g = (generic_type_t *)type;
                if (__find_type(g->template_, g->args, pred, f))
                    return true;
            }

            type = type->host_type;
        }

        return false;
    }

    // Returns type of specified name.
    type_t * __general_type_like_base_t::__type_at(general_type_t * template_,
                                 const type_collection_t & args, name_t name) const
    {
        type_t * type = nullptr;

        __find_type(template_, args,
            [&](generic_param_t * param) { return param->name == name; },
            [&](auto & args_, size_t index) { _A(index < args_.size()); type = args_[index]; }
        );

        return type;
    }

    // Returns types of specified name. (for extends generic arguments)
    bool __general_type_like_base_t::__types_at(general_type_t * template_,
            const type_collection_t & args, name_t type_name, std::function<void (typex_t)> f)
    {
        bool found = __find_type(template_, args,
            [&](generic_param_t * param) { return param->name == type_name; },
            [&](auto & args_, size_t index) {

            for (size_t count = args_.size(); index < count; index++)
            {
                f(args_[index]);
            }

        });

        return found;
    }

    // Transform members.
    template<typename _members_t>
    void __general_type_like_base_t::__transform_members(__tctx_t & tctx,
                                    _members_t & members, _members_t & out_members)
    {
        typedef typename _members_t::value_type _member_t;
        for (_member_t member : members)
        {
            _member_t new_member = __transform_member(tctx, member);
            out_members.push_back(new_member);
        }
    }

    // Transform typename to its implemenation with type arguments.
    type_name_t * __general_type_like_base_t::__transform_type_name(__tctx_t & tctx,
                                                            type_name_t * type_name)
    {
        return tctx.transform_type_name(type_name);
    }

    // Transform field.
    field_t * __general_type_like_base_t::__transform_member(__tctx_t & tctx, field_t * field)
    {
        xpool_t & xpool = __XPool;

        impl_field_t * new_field = xpool.new_obj<impl_field_t>();
        new_field->raw = field;

        new_field->name       = field->name;
        new_field->type_name  = __transform_type_name(tctx, field->type_name);
        new_field->attributes = field->attributes;
        new_field->init_value = field->init_value;
        new_field->decorate   = field->decorate;
        new_field->host_type  = this;
        new_field->variable   = xpool.new_obj<field_variable_t>(new_field);

        return new_field;
    }

    // Transform params.
    params_t * __general_type_like_base_t::__transform_params(__tctx_t & tctx, params_t * params)
    {
        if (params == nullptr)
            return nullptr;

        xpool_t & xpool = __XPool;
        params_t * new_params = xpool.new_obj<params_t>();

        for (param_t * param : *params)
        {
            if (param->ptype == param_type_t::extends)
            {
                type_t * param_type = param->get_type();
                _A(is_generic_param(param_type));

                int index = 0;

                name_t type_name = ((generic_param_t *)param_type)->name;
                __types_at(tctx.template_, tctx.args, type_name, [&](typex_t typex) {

                    string_t name = _F(_T("arg%1%"), ++index);
                    param_t * new_param = xpool.new_obj<param_t>();

                    new_param->name       = xpool.to_name(name.c_str());
                    new_param->attributes = nullptr;
                    new_param->type_name  = xpool.to_type_name((type_t *)typex);
                    new_param->ptype      = (param_type_t)typex;
                    new_param->default_value = nullptr;

                    new_params->push_back(new_param);

                });
            }
            else
            {
                param_t * new_param = xpool.new_obj<param_t>();

                new_param->name          = param->name;
                new_param->attributes    = param->attributes;
                new_param->type_name     = __transform_type_name(tctx, param->type_name);
                new_param->ptype         = param->ptype;
                new_param->default_value = param->default_value;

                new_params->push_back(new_param);
            }
        }

        return new_params;
    }

    // Transform method.
    method_t * __general_type_like_base_t::__transform_member(__tctx_t & tctx, method_t * method)
    {
        xpool_t & xpool = __XPool;

        impl_method_t * new_method = xpool.new_obj<impl_method_t>();
        new_method->raw = method;

        new_method->owner_type_name = __transform_type_name(tctx, method->owner_type_name);
        new_method->type_name   = __transform_type_name(tctx, method->type_name);
        new_method->generic_params = method->generic_params;
        new_method->name        = method->name;
        new_method->decorate    = method->decorate;
        new_method->trait       = method->trait;
        new_method->host_type   = this;
        new_method->variable    = xpool.new_obj<method_variable_t>(new_method);
        new_method->params      = __transform_params(tctx, method->params);

        // Generic params.
        if (method->generic_params != nullptr)
        {
            for (generic_param_t * gp : *method->generic_params)
            {
                new_method->args.push_back(tctx.transform_type(gp));
            }
        }

        tctx.add_relation(method, new_method);

        return new_method;
    }

    // Transform property.
    property_t * __general_type_like_base_t::__transform_member(__tctx_t & tctx,
                                                                property_t * property)
    {
        xpool_t & xpool = __XPool;

        impl_property_t * new_property = xpool.new_obj<impl_property_t>();
        new_property->raw = property;

        new_property->owner_type_name = __transform_type_name(tctx, property->owner_type_name);
        new_property->name       = property->name;
        new_property->type_name  = __transform_type_name(tctx, property->type_name);
        new_property->params     = __transform_params(tctx, property->params);
        new_property->attributes = property->attributes;
        new_property->decorate   = property->decorate;
        new_property->host_type  = this;
        new_property->variable   = xpool.new_obj<property_variable_t>(new_property);

        new_property->get_method = tctx.get_relation<method_t>(property->get_method);
        new_property->set_method = tctx.get_relation<method_t>(property->set_method);

        return new_property;
    }

    // Transform event.
    event_t * __general_type_like_base_t::__transform_member(__tctx_t & tctx, event_t * event)
    {
        xpool_t & xpool = __XPool;

        impl_event_t * new_event = xpool.new_obj<impl_event_t>();
        new_event->raw = event;

        new_event->owner_type_name = __transform_type_name(tctx, event->owner_type_name);
        new_event->type_name  = __transform_type_name(tctx, event->type_name);
        new_event->attributes = event->attributes;
        new_event->decorate   = event->decorate;
        new_event->host_type  = this;
        new_event->variable   = xpool.new_obj<event_variable_t>(new_event);

        // TODO
        new_event->add_method = event->add_method;
        new_event->remove_method = event->remove_method;

        return new_event;
    }

    // Transform nest type.
    general_type_t * __general_type_like_base_t::__transform_member(__tctx_t & tctx,
                                                        type_t * nest_type)
    {
        xpool_t & xpool = __XPool;
        general_type_t * template_ = _M(general_type_t *, nest_type);
        general_type_t * new_nest_type = xpool.new_obj<general_type_t>();

        new_nest_type->name       = template_->name;
        new_nest_type->namespace_ = template_->namespace_;
        new_nest_type->assembly   = template_->assembly;
        new_nest_type->params     = template_->params;
        new_nest_type->vtype      = template_->vtype;
        new_nest_type->ttype      = template_->ttype;
        new_nest_type->mtype      = template_->mtype;
        new_nest_type->host_type  = this;

        type_collection_t args;
        __tctx_t tctx0(this, template_, args);
        new_nest_type->__transform(tctx0);

        xpool.append_new_type(new_nest_type);

        return new_nest_type;
    }

    // Transform typedef.
    type_def_t * __general_type_like_base_t::__transform_member(__tctx_t & tctx,
                                                                type_def_t * type_def)
    {
        xpool_t & xpool = __XPool;

        impl_type_def_t * new_type_def = xpool.new_obj<impl_type_def_t>();
        new_type_def->raw = type_def;

        new_type_def->type_name = __transform_type_name(tctx, type_def->type_name);
        new_type_def->name      = type_def->name;
        new_type_def->params    = type_def->params;
        new_type_def->decorate  = type_def->decorate;
        new_type_def->host_type = this;

        return new_type_def;
    }

    // Transform typename.
    type_name_t * __general_type_like_base_t::__transform_member(__tctx_t & tctx,
                                                     type_name_t * type_name)
    {
        return __transform_type_name(tctx, type_name);
    }


    ////////// ////////// ////////// ////////// //////////
    // default_type_impl_t

    // Default type impl.
    class default_type_impl_t : public type_impl_t
    {
    public:

        // On commit, do nothing.
        virtual void commit(eobject_commit_context_t & ctx, eobject_t * bind_object) override { }
    };

    // Instance of sigleton default_type_impl_t.
    type_impl_t * type_impl_t::default_()
    {
        static default_type_impl_t impl;
        return &impl;
    }

    ////////// ////////// ////////// ////////// //////////

    // Converts to a string.
    atype_t::operator string_t() const
    {
        if (ptype == param_type_t::__default__)
            return _str(type);

        return _F(_T("%1% %2%"), _title(ptype), type);
    }

    ////////// ////////// ////////// ////////// //////////
    // general_type_t

    // Returns vtype.
    vtype_t general_type_t::this_vtype()
    {
        return is_enum(this)? get_underlying_vtype() : vtype;
    }

    // Appends a member.
    void general_type_t::append_member(member_t * member)
    {
        _A(member != nullptr);

        switch (member->this_type())
        {
            case member_type_t::field:
                fields.push_back((field_t *)member);
                break;

            case member_type_t::method:
                methods.push_back((method_t *)member);
                break;

            case member_type_t::property:
                properties.push_back((property_t *)member);
                break;

            case member_type_t::event:
                events.push_back((event_t *)member);
                break;

            case member_type_t::type:
                nest_types.push_back((type_t *)member);
                break;

            case member_type_t::type_def:
                type_defs.push_back((type_def_t *)member);
                break;

            default:
                throw _EC(unexpected);
        }
    }

    // Nest type depth.
    size_t general_type_t::nest_depth() const
    {
        size_t depth = 0;
        const type_t * type = this;

        while ((type = type->host_type) != nullptr)
        {
            depth++;
        }

        return depth;
    }

    // Finds generic param by a name.
    generic_param_t * general_type_t::find_param(const name_t & name)
    {
        return __find_param(params, name);
    }

    // Gets short name of a general type.
    const string_t general_type_t::to_short_name() const
    {
        if (!is_generic())
            return _str(name);

        return _F(_T("%1%<%2%>"), _str(name), _str(params));
    }

    // Converts genreal type to string without generic arguments.
    static void __to_identity_no_gerneric_args(stringstream_t & ss, const general_type_t * type,
        type_t * host_type = nullptr)
    {
        if (type->assembly != nullptr)
            ss << _str(type->assembly) << _T(".");

        if (type->namespace_ != nullptr)
            ss << _str(type->namespace_) << _T(".");

        if (host_type == nullptr)
            host_type = type->host_type;

        if (host_type != nullptr)
        {
            std::stack<const type_t *> host_types;
            for (const type_t * htype = host_type; htype != nullptr; htype = htype->host_type)
            {
                host_types.push(htype);
            }

            for (; !host_types.empty(); host_types.pop())
            {
                const type_t * htype = host_types.top();
                ss << _str(htype->to_short_name()) << _T(".");
            }
        }

        ss << _str(type->name).c_str();
    }

    // Gets full name of a general type.
    const string_t general_type_t::to_full_name() const
    {
        stringstream_t ss;

        __to_identity_no_gerneric_args(ss, this);

        if (is_generic())
            ss << _T("<") << _str(params).c_str() << _T(">");

        return ss.str();
    }

    // Gets identity string of a general type.
    const string_t general_type_t::to_identity() const
    {
        stringstream_t ss;

        __to_identity_no_gerneric_args(ss, this);

        if (params != nullptr && params->size() > 0)
        {
            typedef generic_param_type_t gpt_t;
            ss << _T("<");

            for (int index = 0, size = params->size(); index < size; index++)
            {
                if (index > 0)
                    ss << _T(",");

                generic_param_t * param = (*params)[index];
                if (param->param_type == gpt_t::params)
                    ss << _T("...");
            }

            ss << _T(">");
        }

        return ss.str();
    }

    // Returns param at specified position.
    generic_param_t * general_type_t::param_at(size_t index) const
    {
        _A(index < this->params_count());
        return (*params)[index];
    }

    // Returns if specified param count match this type.
    bool general_type_t::is_match(size_t params_count) const
    {
        if (this->uncertain_params())
            return params_count >= this->params_count() - 1;

        return params_count == this->params_count();
    }

    // Returns if specified param count match this type.
    bool general_type_t::uncertain_params() const
    {
        size_t count = this->params_count();
        return count > 0 && this->param_at(count - 1)->param_type == generic_param_type_t::params;
    }

    // Commits general type.
    void general_type_t::commit(eobject_commit_context_t & ctx)
    {
        if (!__ensure_size_initialize())
            __super_t::commit(ctx);

        if (this->ttype == ttype_t::struct_)
            this->set_sealed(true);

        if (get_base_type() == nullptr)
        {
            type_name_t * object_type_name = __XPool.get_object_type_name();
            if (object_type_name->type != this && this_ttype() != ttype_t::interface_)
            {
                type_name_t * base_types[] = { object_type_name };
                super_type_names.push_front(base_types);
            }
        }

        if (this == __XPool.get_tuple_type())
            mtype = mtype_t::tuple;
    }

    // Returns vsize.
    msize_t general_type_t::get_vsize()
    {
        __ensure_size_initialize();

        return __super_t::get_vsize();
    }

    // Returns count of reference type.
    msize_t general_type_t::get_rcount()
    {
        __ensure_size_initialize();

        return __super_t::get_rcount();
    }

    // Ensure size is initialized.
    bool general_type_t::__ensure_size_initialize()
    {
        if (__rcount == unknown_msize)
        {
            if (vtype != vtype_t::mobject_)
            {
                __rcount = 0;
                __value_size = get_vtype_size(vtype);
            }
            else
            {
                return false;
            }
        }

        return true;
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_type_t

    // Returns type at specified index.
    type_t * generic_type_t::type_at(size_t index) const
    {
        if (index >= argument_count())
            throw _ECF(argument_error, _T("index out of range"));

        return args[index];
    }

    // Returns type of specified name.
    type_t * generic_type_t::type_at(name_t name) const
    {
        return __super_t::__type_at(template_, args, name);
    }

    // Returns param type count. (for delegate type)
    size_t generic_type_t::param_count() const
    {
        return argument_count();
    }

    // Returns type at specified index. (for delegate type)
    typex_t generic_type_t::param_type_at(size_t index) const
    {
        if (index >= argument_count())
            throw _ECF(argument_error, _T("index out of range"));

        return args[index];
    }

    // Returns a prototype name.
    string_t generic_type_t::to_prototype() const
    {
        stringstream_t ss;

        if (host_type != nullptr)
            ss << host_type->to_short_name().c_str() << _T(".");

        ss << get_name().c_str() << _T("<");

        for (int index = 0, size = args.size(); index < size; index++)
        {
            if (index > 0)
                ss << _T(",");

            type_t * arg_type = args[index];
            ss << arg_type->to_prototype().c_str();
        }

        ss << _T(">");

        return ss.str();
    }

    // Commits it.
    void generic_type_t::commit(eobject_commit_context_t & ctx)
    {
        __transform_context_t tctx(this, template_, args);
        __super_t::__transform(tctx);

        __super_t::commit(ctx);
    }

    // Gets the short name of a generic type.
    const string_t generic_type_t::to_short_name() const
    {
        stringstream_t ss;
        ss << (template_? template_->name : string_t(_T("?")));

        if (args.size() > 0)
        {
            ss << _T("<");

            for (int index = 0, count = args.size(); index < count; index++)
            {
                if (index > 0)
                    ss << _T(", ");

                type_t * type = args[index];
                if (type != nullptr)
                {
                    ss << _str(type);
                }
                else if (template_ && template_->params && index < template_->params->size())
                {
                    ss << _str((*template_->params)[index]);
                }
            }

            ss << _T(">");
        }

        return ss.str();
    }

    // Returns whether a generic type has any type def param.
    bool generic_type_t::any_type_def_param() const _NE
    {
        return al::any_of(args, [](type_t * t) {
            return t && t->this_gtype() == gtype_t::type_def_param;
        });
    }

    // Returns identity of a generic type.
    const string_t generic_type_t::to_identity() const
    {
        stringstream_t ss;

        __to_identity_no_gerneric_args(ss, template_, host_type);

        ss << _T("<");

        for (int index = 0, size = args.size(); index < size; index++)
        {
            if (index > 0)
                ss << _T(",");

            ss << _str(args[index]);
        }

        ss << _T(">");

        return ss.str();
    }

    ////////// ////////// ////////// ////////// //////////

    // Methods compare error code.
    X_ENUM_INFO(method_compare_error_code_t)

        X_C(not_match,      _T("not_match"))

        X_C(conflict,       _T("conflict"))

        X_C(auto_determined_failed, _T("auto_determined_failed"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    class __type_get_member_t
    {
        typedef al::svector_t<member_t *>   __not_matched_members_t;
        typedef method_compare_error_code_t __error_code_t;

    public:

        __type_get_member_t(type_t * type, analyze_member_args_t & args, bool include_base_types)
            : __type(type), __args(args), __include_base_types(include_base_types)
        { }

        member_t * get_member()
        {
            member_t * member = __get_member(__type);
            if (member == nullptr && __args.throw_on_not_found)
                throw method_compare_error_t(__error_code_t::not_match, __not_matched_methods);

            return member;
        }

    private: 

        type_t *                    __type;
        analyze_member_args_t &     __args;
        bool                        __include_base_types;

        std::set<type_t *>          __walked_types;
        al::svector_t<method_t *>   __not_matched_methods;

        // Returns members descripted by args.
        member_t * __get_member(type_t * type)
        {
            if (is_interface(type))
            {
                return __get_member_for_interface(type);
            }
            else
            {
                member_t * member = __try_get_member(type);
                if (member != nullptr || !__include_base_types)
                    return member;

                __walked_types.insert(type);
                return __get_member_for_non_interface(type->get_base_type());
            }
        }

        // Returns members descripted by args. (for non-interface types)
        member_t * __get_member_for_non_interface(type_t * type)
        {
            if (type == nullptr || !al::set_insert(__walked_types, type))
                return nullptr;

            member_t * member = __try_get_member(type);
            if (member != nullptr)
                return member;

            return __get_member_for_non_interface(type->get_base_type());
        }

        // Returns members descripted by args. (for interface types)
         member_t * __get_member_for_interface(type_t * type)
        {
            if (type == nullptr || !al::set_insert(__walked_types, type))
                return nullptr;

            member_t * member = __try_get_member(type);
            if (member != nullptr)
                return member;

            type->each_super_type([&, this](type_t * super_type) {
                member = __get_member_for_interface(super_type);
                return member == nullptr;
            });

            return member;
        }

        // Returns nullptr when not found, but not throw exceptions.
        member_t * __try_get_member(type_t * type)
        {
            try
            {
                return type->get_member(__args);
            }
            catch (method_compare_error_t & err)
            {
                if (err.error_code != method_compare_error_code_t::not_match)
                    throw err;

                al::copy(err.related_methods, std::back_inserter(__not_matched_methods));
                return nullptr;
            }
        }
    };

    // Returns members descripted by args.
    member_t * type_t::get_member(analyze_member_args_t & args, bool include_base_types)
    {
        return __type_get_member_t(this, args, include_base_types).get_member();
    }

    // Checks whether it's duplicate.
    static member_t * __check_duplicate(std::set<type_t *> walked_types, type_t * type,
                                        member_t * member)
    {
        if (type == nullptr || !al::set_insert(walked_types, type))
            return nullptr;

        member_t * member1 = type->check_duplicate(member);
        if (member1 != nullptr)
            return member1;

        return __check_duplicate(walked_types, type->get_base_type(), member);
    }

    // Checks whether it's duplicate.
    member_t * type_t::check_duplicate(member_t * member, bool include_base_types)
    {
        member_t * member1 = this->check_duplicate(member);
        if (member1 != nullptr || !include_base_types)
            return member1;

        std::set<type_t *> walked_types = { this };
        return __check_duplicate(walked_types, this->get_base_type(), member);
    }

    ////////// ////////// ////////// ////////// //////////
    // uncertain_type_t

    // Returns members descripted by args.
    member_t * uncertain_type_t::get_member(analyze_member_args_t & args)
    {
        return nullptr;
    }

    // Returns memeber descripted by args.
    void uncertain_type_t::get_members(analyze_members_args_t & args)
    {
        // Empty.
    }

    // Returns all members.
    void uncertain_type_t::get_all_members(members_t & out_members)
    {
        // Empty.
    }

    // The uncertain type sigleton instance.
    uncertain_type_t * uncertain_type_t::instance()
    {
        static uncertain_type_t __instance;
        return &__instance;
    }

    ////////// ////////// ////////// ////////// //////////
    // null_type_t

    // Returns members descripted by args.
    member_t * null_type_t::get_member(analyze_member_args_t & args)
    {
        return nullptr;
    }

    // Returns memeber descripted by args.
    void null_type_t::get_members(analyze_members_args_t & args)
    {
        // Empty.
    }

    // Returns all members.
    void null_type_t::get_all_members(members_t & out_members)
    {
        // Empty.
    }

    // The null type sigleton instance.
    null_type_t * null_type_t::instance()
    {
        static null_type_t __instance;
        return &__instance;
    }

    ////////// ////////// ////////// ////////// //////////
    // unknown_type_t

    // Returns members descripted by args.
    member_t * unknown_type_t::get_member(analyze_member_args_t & args)
    {
        return nullptr;
    }

    // Returns memeber descripted by args.
    void unknown_type_t::get_members(analyze_members_args_t & args)
    {
        // Empty.
    }

    // Returns all members.
    void unknown_type_t::get_all_members(members_t & out_members)
    {
        // Empty.
    }

    // The unknown type sigleton instance.
    unknown_type_t * unknown_type_t::instance()
    {
        static unknown_type_t __instance;
        return &__instance;
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_param_t

    X_ENUM_INFO(generic_param_type_t)

        // Default.
        X_C(default_,   _T("default"))

        // Only for writing.
        X_C(in,         _T("in"))

        // Only for reading.
        X_C(out,        _T("out"))

        // Multipy params.
        X_C(params,     _T("params"))

    X_ENUM_INFO_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Returns members descripted by args.
    member_t * generic_param_t::get_member(analyze_member_args_t & args)
    {
        return nullptr;
    }

    // Returns memeber descripted by args.
    void generic_param_t::get_members(analyze_members_args_t & args)
    {
        // Empty.
    }

    // Returns all members.
    void generic_param_t::get_all_members(members_t & out_members)
    {
        // Empty.
    }

    const string_t generic_param_t::to_identity() const
    {
        if (param_type == generic_param_type_t::params)
            return _T("?") + _str(name) + _T("...");

        return _T("?") + _str(name);
    }

    // Returns whether it's a multipy generic params.
    bool is_generic_params(type_t * type)
    {
        _A(type != nullptr);

        if (type->this_gtype() != gtype_t::generic_param)
            return false;

        return ((generic_param_t *)type)->param_type == generic_param_type_t::params;
    }

    ////////// ////////// ////////// ////////// //////////
    // array_type_t

    // Returns members descripted by args.
    member_t * array_type_t::get_member(analyze_member_args_t & args)
    {
        return base_type->get_member(args);
    }

    // Returns memeber descripted by args.
    void array_type_t::get_members(analyze_members_args_t & args)
    {
        base_type->get_members(args);
    }

    // Returns all members.
    void array_type_t::get_all_members(members_t & out_members)
    {
        base_type->get_all_members(out_members);
    }

    // Checks whether it's duplicate.
    member_t * array_type_t::check_duplicate(member_t * member)
    {
        return base_type->check_duplicate(member);
    }

    // Enum all super types.
    type_t * array_type_t::each_super_type(each_super_type_callback_t callback)
    {
        if (!callback(base_type))
            return base_type;

        return nullptr;
    }

    // Commits it.
    void array_type_t::commit(eobject_commit_context_t & ctx)
    {
        // Do nothing.
    }

    // Returns vsize.
    msize_t array_type_t::get_vsize()
    {
        return unknown_msize;
    }

    // Returns count of reference types.
    msize_t array_type_t::get_rcount()
    {
        return unknown_msize;
    }

    // Gets identity of the array type.
    const string_t array_type_t::to_identity() const
    {
        stringstream_t ss;

        if (element_type != nullptr)
            ss << element_type->to_identity().c_str();
        else
            ss << _T("?");

        ss << _T("[");

        for (int index = 1; index < dimension; index++)
        {
            ss << _T(",");
        }

        ss << _T("]");
        return ss.str();
    }

    ////////// ////////// ////////// ////////// //////////
    // namespace_t

    // Append a type to this namespace.
    void namespace_t::append_type(general_type_t * type)
    {
        _A(type != nullptr);

        types.push_back(type);
        type->namespace_ = this;
    }

    // Converts to a string.
    namespace_t::operator string_t() const
    {
        return full_name? _str(full_name) : name? _str(name) : empty_str;
    }

    ////////// ////////// ////////// ////////// //////////
    // import_t

    // Converts to a string.
    import_t::operator string_t() const
    {
        stringstream_t ss;
        if (package != nullptr)
            ss << _F(_T("from %1% "), _str(package));

        ss << _F(_T("import %1%"), _str(assembly));

        if (!alias.empty())
            ss << _F(_T(" as %1%"), _str(alias));

        return ss.str();
    }

    ////////// ////////// ////////// ////////// //////////
    // using_namespace_t

    // Converts to a string.
    using_namespace_t::operator string_t() const
    {
        if (alias.empty())
            return _F(_T("using %1%"), _str(ns));

        return _F(_T("using %1% = %2%"), _str(alias), _str(ns));
    }

    ////////// ////////// ////////// ////////// //////////

    // Ast error codes.
    X_ENUM_INFO(ast_error_t)

        // Variable duplicated.
        X_C(variable_duplicated, _T("variable_duplicated"))

        // Unexpected variable defination.
        X_C(variable_defination_unexpected, _T("variable_defination_unexpected"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // variable

    // Variable types.
    X_ENUM_INFO(variable_type_t)

        // Local variable.
        X_C(local,          _T("local"))

        // Param variable.
        X_C(param,          _T("param"))

        // Field variable.
        X_C(field,          _T("field"))

        // Property variable.
        X_C(property,       _T("property"))

        // Event variable.
        X_C(event,          _T("event"))

        // Array index variable.
        X_C(array_index,    _T("array_index"))

        // Property index variable.
        X_C(property_index, _T("property_index"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Returns vtype of specified type.
    vtype_t __get_vtype(type_t * type)
    {
        if (type == nullptr)
            return vtype_t::__unknown__;

        switch (type->this_gtype())
        {
            case gtype_t::general:
                return ((general_type_t *)type)->vtype;
                
            case gtype_t::generic:
            case gtype_t::null:
            case gtype_t::array:
                return vtype_t::mobject_;

            default:
                return vtype_t::__unknown__;
        }
    }

    // Returns vtype of specified type name.
    vtype_t __get_vtype(type_name_t * type_name)
    {
        if (type_name == nullptr)
            return vtype_t::__unknown__;

        return __get_vtype(type_name->type);
    }

    // Converts type_t to vtype.
    vtype_t to_vtype(type_t * type)
    {
        return __get_vtype(type);
    }

    //-------- ---------- ---------- ---------- ----------

    // Execute the variable.
    cvalue_t local_variable_t::execute()
    {
        return expression == nullptr || !constant?
            cvalue_t::nan : expression->execute();
    }

    // Returns vtype.
    vtype_t local_variable_t::get_vtype()
    {
        return __get_vtype(type_name);
    }

    //-------- ---------- ---------- ---------- ----------

    // Returns type of the param.
    type_t * param_variable_t::get_type()
    {
        if (param->ptype == param_type_t::extends)
            return __XPool.get_ptr_type();

        return to_type(param->type_name);
    }

    // Returns vtype of the param.
    vtype_t param_variable_t::get_vtype()
    {
        if (param->ptype == param_type_t::params)
            return vtype_t::ptr_;

        return __get_vtype(param->type_name);
    }

    //-------- ---------- ---------- ---------- ----------

    // Converts local variable to a string.
    X_DEFINE_TO_STRING(local_variable_t)
    {
        return name;
        //return _F(_T("%1% %2%"), type_name, name);
    }

    //-------- ---------- ---------- ---------- ----------

    // Converts param variable to a string.
    X_DEFINE_TO_STRING(param_variable_t)
    {
        return param->name;
        //return _F(_T("%1% %2%"), param->type_name, param->name);
    }

    //-------- ---------- ---------- ---------- ----------

    // Returns vtype.
    vtype_t field_variable_t::get_vtype()
    {
        return __get_vtype(field->type_name);
    }

    // Execute the variable.
    cvalue_t field_variable_t::execute()
    {
        _A(field != nullptr);

        if (!is_static_const(field))
            return cvalue_t::nan;

        if (field->init_value == nullptr)
            return default_value_of(get_vtype());

        return field->init_value->execute();
    }

    // Converts field variable to a string.
    X_DEFINE_TO_STRING(field_variable_t)
    {
        return field->name;
        //return _F(_T("%1% %2%"), field->type_name, field->name);
    }

    //-------- ---------- ---------- ---------- ----------

    // Returns vtype.
    vtype_t property_variable_t::get_vtype()
    {
        return __get_vtype(property->type_name);
    }

    // Converts property variable to a string.
    X_DEFINE_TO_STRING(property_variable_t)
    {
        return property->name;
        //return _F(_T("%1% %2%"), property->type_name, property->name);
    }

    //-------- ---------- ---------- ---------- ----------

    // Returns vtype.
    vtype_t property_index_variable_t::get_vtype()
    {
        return __get_vtype(property->type_name);
    }

    // Converts property index variable to a string.
    X_DEFINE_TO_STRING(property_index_variable_t)
    {
        return _F(_T("%1%[%2%]"), get_name(), arguments);
    }

    //-------- ---------- ---------- ---------- ----------

    // Constructor.
    array_index_variable_t::array_index_variable_t(expression_t * body, type_t * element_type,
                                                   arguments_t * arguments)
        : __super_t(arguments), body(body), element_type(element_type)
    {
        _A(body != nullptr);
        _A(element_type != nullptr);
    }

    // Returns name of the array index.
    name_t array_index_variable_t::get_name() const
    {
        return name_t::null;
    }

    // Returns type of array index variable.
    type_t * array_index_variable_t::get_type()
    {
        return element_type;
    }

    // Returns vtype.
    vtype_t array_index_variable_t::get_vtype()
    {
        return __get_vtype(element_type);
    }

    // Returns array type.
    type_t * array_index_variable_t::get_array_type()
    {
        _A(element_type != nullptr);
        return __XPool.new_array_type(element_type, dimension());
    }

    // Converts array index variable to a string.
    X_DEFINE_TO_STRING(array_index_variable_t)
    {
        return _F(_T("%1%[%2%]"), get_name(), arguments);
    }

    //-------- ---------- ---------- ---------- ----------

    // Converts event variable to a string.
    X_DEFINE_TO_STRING(event_variable_t)
    {
        return event->name;
        //return _F(_T("%1% %2%"), event->type_name, event->name);
    }

    // Returns vtype.
    vtype_t event_variable_t::get_vtype()
    {
        return __get_vtype(event->type_name);
    }

    //-------- ---------- ---------- ---------- ----------

    // Constructor.
    method_variable_t::method_variable_t(method_t * method) : method(method)
    {
        _A(method != nullptr);
    }

    // Returns type of the method.
    type_t * method_variable_t::get_type()
    {
        if (__type == nullptr)
        {
            type_collection_t types = { method->get_type() };
            for (size_t index = 0, count = method->param_count(); index < count; index++)
            {
                param_t * param = method->param_at(index);
                types.push_back(typex_t(param));
            }

            __type = __XPool.new_generic_type(
                __XPool.get_delegate_type(), types, nullptr
            );
        }

        return __type;
    }

    // Returns vtype.
    vtype_t method_variable_t::get_vtype()
    {
        return vtype_t::mobject_;
    }

    // Returns name of the method.
    name_t method_variable_t::get_name() const
    {
        return method->name;
    }

    // Converts method variable to a string.
    X_DEFINE_TO_STRING(method_variable_t)
    {
        if (method->generic_params != nullptr)
            return _F(_T("%1%<%2%>"), method->name, method->generic_params);

        return method->name;
    }

    ////////// ////////// ////////// ////////// //////////
    // xpool_t

    // Returns type collection id.
    type_collection_id_t xtype_collection_t::get_tcid()
    {
        if (tcid.empty())
            tcid = __XPool.type_collection.get_tcid(types);

        return tcid;
    }

    xpool_t::xpool_t(memory_t * memory) : memory(memory)
    {
        __current = this;
    }

    // Initialize.
    void xpool_t::initialize()
    {
        // Do nothing.
    }

    // Creates a new generic type.
    generic_type_t * xpool_t::new_generic_type(general_type_t * template_,
                                       xtype_collection_t & types, type_t * host_type)
    {
        _A(template_ != nullptr);
        _A(template_->is_match(types.size()));

        generic_type_cache_key_t key(template_, types.get_tcid(), host_type);
        generic_type_t * type = generic_type_cache->get(key);
        if (type == nullptr)
        {
            type = memory_t::new_obj<generic_type_t>(memory, template_,
                                    (const type_collection_t &)types, host_type);
            generic_type_cache->set(key, type);

            append_new_type(type);
        }

        return type;
    }

    // Creates a new generic type.
    generic_type_t * xpool_t::new_generic_type(general_type_t * template_,
                                       type_collection_t & types, type_t * host_type)
    {
        _A(template_ != nullptr);

        xtype_collection_t xtypes(types);
        return new_generic_type(template_, xtypes, host_type);
    }

    // Creates a new array type.
    array_type_t * xpool_t::new_array_type(type_t * element_type, dimension_t dimension)
    {
        if (element_type == nullptr)
            element_type = null_type_t::instance();

        array_type_cache_key_t key(element_type, dimension);
        array_type_t * type = array_type_cache->get(key);
        if (type == nullptr)
        {
            general_type_t * t_array_type = get_tarray_type();
            type_collection_t type_args({ element_type });
            type_t * base_type = new_generic_type(t_array_type, type_args, nullptr);

            type = memory_t::new_obj<array_type_t>(memory, base_type, element_type, dimension);
            array_type_cache->set(key, type);

            append_new_type(type);
        }

        return type;
    }

    // Creates a new generic method.
    generic_method_t * xpool_t::new_generic_method(method_t * template_,
                                xtype_collection_t & types, type_t * host_type)
    {
        _A(template_ != nullptr);
        _A(template_->generic_param_count() == types.size());

        generic_method_cache_key_t key(template_, types.get_tcid(), host_type);
        generic_method_t * method = generic_method_cache->get(key);
        if (method == nullptr)
        {
            method = memory_t::new_obj<generic_method_t>(memory, template_,
                                    (const type_collection_t &)types, host_type);
            generic_method_cache->set(key, method);
        }

        return method;
    }

    // Creates a new generic method.
    generic_method_t * xpool_t::new_generic_method(method_t * template_,
                                type_collection_t & types, type_t * host_type)
    {
        _A(template_ != nullptr);

        xtype_collection_t xtypes(types);
        return new_generic_method(template_, xtypes, host_type);
    }

    // Creates a new assembly reference.
    assembly_reference_t * xpool_t::new_reference(const mname_t * package_name,
                                                  assembly_t * assembly)
    {
        _A(assembly != nullptr);

        assembly_reference_cache_key_t key(package_name, assembly->name);
        assembly_reference_t * ref = assembly_reference_cache->get(key);
        if (ref == nullptr)
        {
            ref = memory_t::new_obj<assembly_reference_t>(memory, package_name, assembly);
            assembly_reference_cache->set(key, ref);
        }

        return ref;
    }

    // Returns compile time attribute.
    attribute_t * xpool_t::get_compile_time_attribute()
    {
        if (__compile_time_attribute == nullptr)
        {
            attribute_t * __new_compile_time_attribute(xpool_t & xpool);
            __compile_time_attribute = __new_compile_time_attribute(*this);
        }

        return __compile_time_attribute;
    }

    // Returns type of the vtype.
    general_type_t * xpool_t::get_internal_type(vtype_t vtype)
    {
        general_type_t * __get_internal_type(attribute_t * attribute, vtype_t vtype);
        return __get_internal_type(get_compile_time_attribute(), vtype);
    }

    // Returns type of the value type.
    general_type_t * xpool_t::get_internal_type(value_type_t value_type)
    {
        return get_internal_type(to_vtype(value_type));
    }

    // Returns type of the name.
    general_type_t * xpool_t::get_internal_type(name_t name)
    {
        general_type_t * __get_internal_type(attribute_t * attribute, name_t name);
        return __get_internal_type(get_compile_time_attribute(), name);
    }

    // Returns System.Object type.
    general_type_t * xpool_t::get_object_type()
    {
        return __get_specified_type(__CoreTypeName(CoreType_Object), __object_type);
    }

    // Returns System.Object typename.
    type_name_t * xpool_t::get_object_type_name()
    {
        if (__object_type_name.type == nullptr)
        {
            __object_type_name.type = __get_specified_type(
                __CoreTypeName(CoreType_Object), __object_type
            );
        }

        return &__object_type_name;
    }

    // Returns System.Void type.
    general_type_t * xpool_t::get_void_type()
    {
        return __get_specified_type(__CoreTypeName(CoreType_Void), __void_type);
    }

    // Returns System.Array<> type.
    general_type_t * xpool_t::get_tarray_type()
    {
        return __get_specified_type(__CoreTypeName(CoreType_TArray), __tarray_type);
    }

    // Returns System.Array type.
    general_type_t * xpool_t::get_array_type()
    {
        return __get_specified_type(__CoreTypeName(CoreType_Array), __array_type);
    }

    // Returns System.String type.
    general_type_t * xpool_t::get_string_type()
    {
        return __get_specified_type(__CoreTypeName(CoreType_String), __string_type);
    }

    // Returns System.Type type.
    general_type_t * xpool_t::get_type_type()
    {
        return __get_specified_type(__CoreTypeName(CoreType_Type), __type_type);
    }

    // Returns System.Enum type.
    general_type_t * xpool_t::get_enum_type()
    {
        return __get_specified_type(__CoreTypeName(CoreType_Enum), __enum_type);
    }

    // Returns System.Ptr type.
    general_type_t * xpool_t::get_ptr_type()
    {
        return __get_specified_type(__CoreTypeName(CoreType_Ptr), __ptr_type);
    }

    // Returns System.Delegate type.
    general_type_t * xpool_t::get_delegate_type()
    {
        return __get_specified_type(__CoreTypeName(CoreType_Delegate), __delegate_type);
    }

    // Returns System.MulticastDelegate type.
    general_type_t * xpool_t::get_multicast_delegate_type()
    {
        return __get_specified_type(__CoreTypeName(CoreType_MulticastDelegate),
            __multicast_delegate_type
        );
    }

    // Returns System.Exception type.
    general_type_t * xpool_t::get_exception_type()
    {
        return __get_specified_type(__CoreTypeName(CoreType_Exception), __exception_type);
    }

    // Returns System.Tuple<...> type.
    general_type_t * xpool_t::get_tuple_type()
    {
        return __get_specified_type(__CoreTypeName(CoreType_Tuple), __tuple_type);
    }

    // Returns System.ITuple<...> type.
    general_type_t * xpool_t::get_ituple_type()
    {
        return __get_specified_type(__CoreTypeName(CoreType_ITuple), __ituple_type);
    }

    // Returns System.Diagnostics.TraceAttribute type.
    general_type_t * xpool_t::get_trace_attribute_type()
    {
        return __get_specified_type(
            __CoreTypeName(CoreType_TraceAttribute), __trace_attribute_type
        );
    }

    // Returns System.Reflection.ReflectionInfo type.
    general_type_t * xpool_t::get_reflection_info_type()
    {
        return __get_specified_type(
            __CoreTypeName(CoreType_ReflectionInfo), __reflection_info_type
        );
    }

    // Returns System.Reflection.Assembly type.
    general_type_t * xpool_t::get_assembly_type()
    {
        return __get_specified_type(
            __CoreTypeName(CoreType_Assembly), __assembly_type
        );
    }

    // Returns System.Reflection.Member type.
    general_type_t * xpool_t::get_member_type()
    {
        return __get_specified_type(
            __CoreTypeName(CoreType_Member), __member_type
        );
    }

    // Returns System.Reflection.Method type.
    general_type_t * xpool_t::get_method_type()
    {
        return __get_specified_type(
            __CoreTypeName(CoreType_Method), __method_type
        );
    }

    // Returns System.Reflection.Property type.
    general_type_t * xpool_t::get_property_type()
    {
        return __get_specified_type(
            __CoreTypeName(CoreType_Property), __property_type
        );
    }

    // Returns System.Reflection.Event type.
    general_type_t * xpool_t::get_event_type()
    {
        return __get_specified_type(
            __CoreTypeName(CoreType_Event), __event_type
        );
    }

    // Returns System.Reflection.Field type.
    general_type_t * xpool_t::get_field_type()
    {
        return __get_specified_type(
            __CoreTypeName(CoreType_Field), __field_type
        );
    }

    // Returns System.Reflection.Parameter type.
    general_type_t * xpool_t::get_parameter_type()
    {
        return __get_specified_type(
            __CoreTypeName(CoreType_Parameter), __parameter_type
        );
    }

    // Returns System.Reflection.GenericParameter type.
    general_type_t * xpool_t::get_generic_parameter_type()
    {
        return __get_specified_type(
            __CoreTypeName(CoreType_GenericParameter), __generic_parameter_type
        );
    }

    // Returns specified type with cache.
    general_type_t * xpool_t::__get_specified_type(const char_t * name, general_type_t * & __cache)
    {
        if (__cache == nullptr)
        {
            auto it = __new_system_type_dict.find(spool.to_sid(name));
            if (it == __new_system_type_dict.end())
                throw _ED(compile_error_code_t::type_not_found, name);

            __cache = it->second;
        }

        return __cache;
    }

    // Append a new type.
    void xpool_t::append_new_type(type_t * type)
    {
        _A(type != nullptr);

        __new_types.push(type);

        if (type->this_gtype() == gtype_t::general)
        {
            general_type_t * gtype = (general_type_t *)type;
            if (is_core_assembly(gtype->assembly))
            {
                __new_system_type_dict[spool.to_sid(gtype->to_identity())] = gtype;
            }
        }
    }

    // Returns whether it's System namespace.
    bool xpool_t::is_core_assembly(assembly_t * assembly)
    {
        if (__core_assembly_name == sid_t::null)
            __core_assembly_name = spool.to_sid(CoreAssembly);

        return __core_assembly_name == assembly->name->sid;
    }

    // Commit types.
    void xpool_t::commit_types(logger_t & logger)
    {
        eobject_commit_context_t ctx(logger);
        while (!__new_types.empty())
        {
            type_t * type = __new_types.front();
            __new_types.pop();

            type->commit(ctx);
        }
    }

    // Converts vtype to typename.
    type_name_t * xpool_t::to_type_name(vtype_t vtype)
    {
        auto it = __vtype_type_name_map.find(vtype);
        if (it == __vtype_type_name_map.end())
        {
            type_name_t * type_name = to_type_name(get_internal_type(vtype));
            __vtype_type_name_map[vtype] = type_name;

            return type_name;
        }

        return it->second;
    }

    // Returns the typename of type.
    type_name_t * xpool_t::to_type_name(type_t * type)
    {
        auto it = __type_type_name_map.find(type);
        if (it == __type_type_name_map.end())
        {
            type_name_t * type_name = new_obj<type_name_t>(type);
            __type_type_name_map[type] = type_name;

            return type_name;
        }

        return it->second;;
    }

    // Returns cvalue_expression_t of specified cvalue.
    cvalue_expression_t * xpool_t::get_cvalue_expression(cvalue_t cvalue)
    {
        __cvalue_t cv = (__cvalue_t)cvalue;
        auto it = __cvalue_expression_map.find(cv);
        if (it == __cvalue_expression_map.end())
        {
            auto pair = __cvalue_expression_map.insert(
                std::make_pair(cv, (cvalue_expression_t *)nullptr)
            );

            it = pair.first;
            it->second = new_obj<cvalue_expression_t>(
                const_cast<__cvalue_t *>(&it->first)
            );
        }

        return it->second;
    }

    // Returns decorate of specified value.
    decorate_t * xpool_t::to_decorate(decorate_value_t value)
    {
        auto it = __decorate_map.find(value);
        if (it == __decorate_map.end())
        {
            decorate_t * decorate = new_obj<decorate_t>(value);
            __decorate_map[value] = decorate;

            return decorate;
        }

        return it->second;
    }

    xpool_t * xpool_t::__current;

    ////////// ////////// ////////// ////////// //////////
    // variable_region_t

    // Constructor.
    variable_region_t::variable_region_t(variable_region_t * previous,
                                         variable_type_t mask, object_t * owner)
        : previous(previous), __mask(mask)
        , __last_local_variable(previous? previous->__last_local_variable : nullptr)
    { }

    // Constructor.
    variable_region_t::variable_region_t(variable_region_t * previous,
                                         variable_type_t mask, method_t * method)
        : variable_region_t(previous, mask, (object_t *)method)
    {
        if (method != nullptr)
        {
            __local_variable_t ** head_addr = method->__local_variables.head_addr();
            __last_local_variable = *head_addr == nullptr?
                al::incorp((void *)head_addr, false) : al::incorp((void *)*head_addr, true);
        }
    }

    // Defines a local variable.
    local_variable_t * variable_region_t::define_local(type_name_t * type_name,
                    const name_t & name, bool constant, expression_t * expression)
    {
        _A(type_name != nullptr);
        _A(!name.empty());

        __local_variable_t * variable = __define<__local_variable_t>(type_name, name);
        variable->identity   = __next_local_identity();
        variable->index      = __next_local_index();
        variable->constant   = constant;
        variable->expression = expression;

        if (__last_local_variable != nullptr)
        {
            if (al::incorp_v<bool>(__last_local_variable))   // node
            {
                al::incorp_p((__local_variable_t *)__last_local_variable)->append_next(variable);
            }
            else    // head addr
            {
                __local_variable_t::append_next(
                    al::incorp_p((__local_variable_t **)__last_local_variable), variable
                );
            }

            __last_local_variable = al::incorp((void *)variable, true);
        }

        return variable;
    }

    // Defines a param variable.
    param_variable_t * variable_region_t::define_param(param_t * param)
    {
        _A(param != nullptr);

        return __define<param_variable_t>(param);
    }

    // Defines a field variable.
    field_variable_t * variable_region_t::define_field(field_t * field)
    {
        _A(field != nullptr);

        field_variable_t * variable = __define<field_variable_t>(field);
        field->variable = variable;

        return variable;
    }

    // Defines a property variable.
    property_variable_t * variable_region_t::define_property(property_t * property)
    {
        _A(property != nullptr);

        property_variable_t * variable = __define<property_variable_t>(property);
        property->variable = variable;

        return variable;
    }

    // Defines an event variable.
    event_variable_t * variable_region_t::define_event(event_t * event)
    {
        _A(event != nullptr);

        event_variable_t * variable = __define<event_variable_t>(event);
        event->variable = variable;

        return variable;
    }

    // Defines a method variable.
    method_variable_t * variable_region_t::define_method(method_t * method)
    {
        _A(method != nullptr);

        method_variable_t * variable = __define<method_variable_t>(method);
        method->variable = variable;

        return variable;
    }

    // Checks whether it has a variable with the same name in the region context.
    void variable_region_t::__check_duplicate(variable_t * variable)
    {
        name_t name = variable->get_name();

        if (name == name_t::null)    // for property index.
            return;

        auto it = __variables.find(name);
        if (it != __variables.end())
        {
            if (!variable->allow_duplicate() || it->second->this_type() != variable->this_type())
                throw _EF(ast_error_t::variable_duplicated, _T("variable \%1%\" duplicate"), name);
        }
    }

    // Returns next local identity.
    msize_t variable_region_t::__next_local_identity()
    {
        return __current_local_identity()++;
    }

    // Returns current local identity.
    msize_t & variable_region_t::__current_local_identity()
    {
        variable_region_t * root = this;
        while (root->previous != nullptr && root->previous->__is_local_supported())
            root = root->previous;

        return root->__local_identity;
    }

    // Returns current local index.
    msize_t & variable_region_t::__current_local_index()
    {
        if (__local_index == __empty_index)
        {
            if (previous != nullptr && previous->__is_local_supported())
                __local_index = previous->__current_local_index();
            else
                __local_index = 0;
        }

        return __local_index;
    }

    // Returns next local index.
    msize_t variable_region_t::variable_region_t::__next_local_index()
    {
        return __current_local_index()++;
    }

    // Gets the variable of the name.
    variable_t * variable_region_t::get(const name_t & name)
    {
        _A(!name.empty());

        auto it = __variables.find(name);
        if (it != __variables.end())
            return it->second;

        return previous? previous->get(name) : nullptr;
    }

    // Creates a new variable.
    template<typename _variable_t, typename ... _args_t>
    _variable_t * variable_region_t::__new_variable(_args_t && ... args)
    {
        _variable_t * variable = memory_t::new_obj<_variable_t>(
            __Memory, std::forward<_args_t>(args) ...
        );

        return variable;
    }

    // Defines a new variable.
    template<typename _variable_t, typename ... _args_t>
    _variable_t * variable_region_t::__define(_args_t && ... args)
    {
        _variable_t * variable = __new_variable<_variable_t>(std::forward<_args_t>(args) ...);
        name_t name = variable->get_name();

        _A(name != name_t::null);

        if (!__is_supported(_variable_t::type))
        {
            throw _EF(ast_error_t::variable_defination_unexpected,
                _T("%1% variable \"%2%\" cannot be defined here"), _desc(_variable_t::type), name
            );
        }

        __check_duplicate(variable);
        __variables.insert(std::make_pair(name, (variable_t *)variable));

        return variable;
    }

    ////////// ////////// ////////// ////////// //////////
    // module_t

    // Register commit eobject that needed.
    void module_t::register_commit(eobject_t * eobj)
    {
        _A(eobj != nullptr);

        __need_commit_eobjects.push_back(eobj);
    }

    // Commits it.
    void module_t::commit(eobject_commit_context_t & ctx)
    {
        for (eobject_t * eobj : __need_commit_eobjects)
        {
            eobj->commit(ctx);
        }
    }

    // Converts to string.
    X_DEFINE_TO_STRING(module_t)
    {
        return _T("module");
    }

    ////////// ////////// ////////// ////////// //////////
    // ast_walk_context_layer_t

    // Ast walk context layer state
    X_ENUM_INFO(ast_walk_context_layer_state_t)

        // Module
        X_C(module,         _T("module"))

        // Document
        X_C(document,       _T("document"))

        // Namespace
        X_C(namespace_,     _T("namespace"))

        // Type
        X_C(type,           _T("type"))

        // Method
        X_C(method,         _T("method"))

        // Property
        X_C(property,       _T("property"))

        // Event
        X_C(event,          _T("event"))

        // Field
        X_C(field,          _T("field"))

        // Typedef
        X_C(type_def,       _T("type_def"))

        // Region
        X_C(region,         _T("region"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Base type ast walk context layer.
    class __ast_walk_context_layer_base_t : public object_t
                                          , public ast_walk_context_layer_t
    {
        typedef ast_walk_context_layer_t __super_t;

    public:
        using __super_t::__super_t;

        // Gets the eobject of specified layer state.
        virtual void * get(__layer_state_t state) override
        {
            return previous->get(state);
        }

        X_TO_STRING_IMPL(_T("__ast_walk_context_layer_base_t"))
    };

    //-------- ---------- ---------- ---------- ----------

    // Empty ast walk context layer.
    class __empty_ast_walk_context_layer_t : public object_t
                                           , public ast_walk_context_layer_t
    {
        typedef ast_walk_context_layer_t __super_t;

    public:
        using __super_t::__super_t;

        // Gets the eobject of specified layer state.
        virtual void * get(__layer_state_t state) override
        {
            return nullptr;
        }

        X_TO_STRING_IMPL(_T("__empty_ast_walk_context_layer_t"))
    };

    //-------- ---------- ---------- ---------- ----------

    // Namespace ast walk context layer.
    class __namespace_ast_walk_context_layer_t : public __ast_walk_context_layer_base_t
    {
        typedef __ast_walk_context_layer_base_t __super_t;

    public:

        // Constructors.
        __namespace_ast_walk_context_layer_t(__context_t & context, __layer_t * previous,
                                                                        namespace_t * ns)
            : __super_t(context, previous, ns), __ns(ns)
        {
            _A(ns != nullptr);

            namespace_t * previous_ns = (namespace_t *)previous->get(__layer_state_t::namespace_);
            const mname_t * previous_name = previous_ns? previous_ns->full_name : nullptr;
            mname_operate_context_t mctx = to_mname_operate_context();

            if (previous_name == nullptr)
                __ns->full_name = to_mname(ns->name);
            else
                __ns->full_name = mname_t::combine(mctx, previous_name, to_mname(ns->name));
        }

        // Gets the eobject of specified layer state.
        virtual void * get(__layer_state_t state) override
        {
            switch (state)
            {
                case __layer_state_t::namespace_:
                    return __ns;

                default:
                    return __super_t::get(state);
            }
        }

    private:
        namespace_t * __ns;
    };

    //-------- ---------- ---------- ---------- ----------

    // Document ast walk context layer.
    class __document_ast_walk_context_layer_t : public __ast_walk_context_layer_base_t
    {
        typedef __ast_walk_context_layer_base_t __super_t;

    public:
        using __super_t::__super_t;

        // Gets the eobject of specified layer state.
        virtual void * get(__layer_state_t state) override
        {
            switch (state)
            {
                case __layer_state_t::document:
                    return &__document;

                default:
                    return __super_t::get(state);
            }
        }

        // Gets the document.
        document_t * get_document() { return &__document; }

    private:
        document_t __document;
    };

    //-------- ---------- ---------- ---------- ----------

    // Modules ast walk context layer.
    class __module_ast_walk_context_layer_t : public __ast_walk_context_layer_base_t
    {
        typedef __ast_walk_context_layer_base_t __super_t;

    public:
        using __super_t::__super_t;

        // Gets the eobject of specified layer state.
        virtual void * get(__layer_state_t state) override
        {
            switch (state)
            {
                case __layer_state_t::module:
                    return &__module;

                default:
                    return __super_t::get(state);
            }
        }

        // Gets the module.
        module_t * get_module() { return &__module; }

    private:
        module_t __module;
    };

    //-------- ---------- ---------- ---------- ----------

    // Region ast walk context layer.
    template<variable_type_t _variable_type_mask = (variable_type_t)0xFF>
    class __region_ast_walk_context_layer_t : public __ast_walk_context_layer_base_t
    {
        typedef __ast_walk_context_layer_base_t __super_t;

    public:

        // Constructor.
        template<typename _owner_t>
        __region_ast_walk_context_layer_t(__context_t & context, __layer_t * previous_layer,
                                      _owner_t * owner, variable_region_t * previous_region)
            : __super_t(context, previous_layer)
            , __variable_region(previous_region, _variable_type_mask, owner)
        { }

        // Gets the eobject of specified layer state.
        virtual void * get(__layer_state_t state) override
        {
            if (state == __layer_state_t::region)
                return &__variable_region;

            return __super_t::get(state);
        }

    private:
        variable_region_t __variable_region;
    };

    //-------- ---------- ---------- ---------- ----------

    // Base class of no region ast walk context layer.
    class __no_region_ast_walk_context_layer_t : public __ast_walk_context_layer_base_t
    {
        typedef __ast_walk_context_layer_base_t __super_t;

    public:

        // Constructor.
        template<typename _owner_t>
        __no_region_ast_walk_context_layer_t(__context_t & context, __layer_t * previous,
                                                                    _owner_t * owner)
            : __super_t(context, previous, owner)
        { }

        // Gets the eobject of specified layer state.
        virtual void * get(__layer_state_t state) override
        {
            if (state == __layer_state_t::region)
                return nullptr;

            return __super_t::get(state);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    typedef ast_walk_context_layer_state_t __layer_state_t;

    // Base class of ast walk context layer with state.
    template<typename _obj_t, __layer_state_t _state_t,
        typename _super_t = __ast_walk_context_layer_base_t
    >
    class __x_ast_walk_context_layer_t : public _super_t
    {
        typedef _super_t __super_t;
        typedef _obj_t   __obj_t;

        typedef ast_walk_context_t              __context_t;
        typedef ast_walk_context_layer_t        __layer_t;
        typedef ast_walk_context_layer_state_t  __layer_state_t;

    public:

        // Constructor.
        template<typename ... _args_t>
        __x_ast_walk_context_layer_t(__context_t & context, __layer_t * previous,
                                        __obj_t * obj, _args_t && ... args)
            : __super_t(context, previous, obj, std::forward<_args_t>(args) ...), __obj(obj)
        { }

        // Gets the eobject of specified layer state.
        virtual void * get(__layer_state_t state) override
        {
            switch (state)
            {
                case _state_t:
                    return __obj;

                default:
                    return __super_t::get(state);
            }
        }

    private:
        __obj_t * __obj;
    };

    //-------- ---------- ---------- ---------- ----------

    template<variable_type_t _variable_type_mask = (variable_type_t)0xFF>
    using __region_layer_t = __region_ast_walk_context_layer_t<_variable_type_mask>;

    // Type ast walk context layer.
    typedef __x_ast_walk_context_layer_t<
        type_t, __layer_state_t::type,
        __region_layer_t<bit_or(variable_type_t::field, variable_type_t::property,
                                                         variable_type_t::method)>
    > __type_ast_walk_context_layer_t;

    // Method ast walk context layer.
    typedef __x_ast_walk_context_layer_t<
        method_t, __layer_state_t::method,
        __region_layer_t<bit_or(variable_type_t::param, variable_type_t::local)>
    > __method_ast_walk_context_layer_t;

    // Property ast walk context layer.
    typedef __x_ast_walk_context_layer_t<
        property_t, __layer_state_t::property   //, __no_region_ast_walk_context_layer_t
    > __property_ast_walk_context_layer_t;

    // Event ast walk context layer.
    typedef __x_ast_walk_context_layer_t<
        event_t, __layer_state_t::event         //, __no_region_ast_walk_context_layer_t
    > __event_ast_walk_context_layer_t;

    // Field ast walk context layer.
    typedef __x_ast_walk_context_layer_t<
        field_t, __layer_state_t::field         //, __no_region_ast_walk_context_layer_t
    > __field_ast_walk_context_layer_t;

    // Typedef ast walk context layer.
    typedef __x_ast_walk_context_layer_t<
        type_def_t, __layer_state_t::type_def   //, __no_region_ast_walk_context_layer_t
    > __type_def_ast_walk_context_layer_t;

    ////////// ////////// ////////// ////////// //////////
    // ast_walk_context_t

    // Ast walk context.
    ast_walk_context_t::ast_walk_context_t(assembly_t & assembly,
            assembly_loader_t * assembly_loader, logger_t & logger, __layer_t * layer)
        : assembly(assembly), __assembly_loader(assembly_loader), logger(logger)
    {
        _A(assembly_loader != nullptr);

        if (layer == nullptr)
            __push_new_layer<__empty_ast_walk_context_layer_t>();
        else
            __current_layer = layer;
    }

    // Pushes a namespace
    void ast_walk_context_t::push(namespace_t * namespace_)
    {
        _A(namespace_ != nullptr);

        __push_new_layer<__namespace_ast_walk_context_layer_t>(namespace_);
    }

    // Pushes a type.
    void ast_walk_context_t::push(type_t * type)
    {
        _A(type != nullptr);

        __push_new_layer<__type_ast_walk_context_layer_t>(type, current_region());
    }

    // Pushes a method.
    void ast_walk_context_t::push(method_t * method)
    {
        _A(method != nullptr);

        __push_new_layer<__method_ast_walk_context_layer_t>(method, current_region());
    }

    // Pushes a property.
    void ast_walk_context_t::push(property_t * property)
    {
        _A(property != nullptr);

        __push_new_layer<__property_ast_walk_context_layer_t>(property);
    }

    // Pushes a event.
    void ast_walk_context_t::push(event_t * event)
    {
        _A(event != nullptr);

        __push_new_layer<__event_ast_walk_context_layer_t>(event);
    }

    // Pushes a field.
    void ast_walk_context_t::push(field_t * field)
    {
        _A(field != nullptr);

        __push_new_layer<__field_ast_walk_context_layer_t>(field);
    }

    // Pushes a typedef.
    void ast_walk_context_t::push(type_def_t * type_def)
    {
        _A(type_def != nullptr);

        __push_new_layer<__type_def_ast_walk_context_layer_t>(type_def);
    }

    // Pushes a new region.
    void ast_walk_context_t::push_new_region()
    {
        typedef __region_ast_walk_context_layer_t<variable_type_t::local> layer_t;
        __push_new_layer<layer_t>((object_t *)nullptr, current_region());
    }

    // Pushes a new document.
    document_t * ast_walk_context_t::push_new_document()
    {
        return __push_new_layer<__document_ast_walk_context_layer_t>()->get_document();
    }

    // Pushes new module.
    module_t * ast_walk_context_t::push_new_module()
    {
        return __push_new_layer<__module_ast_walk_context_layer_t>()->get_module();
    }

    // Pop top element.
    void ast_walk_context_t::pop()
    {
        __current_layer = __current_layer->previous;

        _A(__current_layer != nullptr);
    }

    // Returns parent of a mname.
    const mname_t * ast_walk_context_t::parent_of(const mname_t * mname)
    {
        if (mname == nullptr || mname->parts.size() <= 1)
            return nullptr;

        return al::map_get(__parent_mnames, mname, [this, mname]() {
            return mname_t::get_parent(to_mname_operate_context(), mname);
        });
    }

    // Returns child of a mname.
    const mname_t * ast_walk_context_t::child_of(const mname_t * mname)
    {
        if (mname == nullptr || mname->parts.size() <= 1)
            return nullptr;

        return al::map_get(__child_mnames, mname, [this, mname]() {
            return mname_t::get_child(to_mname_operate_context(), mname);
        });
    }

    // Combines two mnames.
    const mname_t * ast_walk_context_t::combine(const mname_t * name1, const mname_t * name2)
    {
        if (name1 == nullptr || name2 == nullptr)
            return name1? name1 : name2;

        return al::map_get(__combine_names, __combine_name_key_t(name1, name2),
            [this, name1, name2]() {
                return mname_t::combine(to_mname_operate_context(), name1, name2);
            }
        );
    }

    // Combines two mnames.
    const mname_t * ast_walk_context_t::combine(const mname_t * name1, const name_t & name2)
    {
        if (name2.empty())
            return name1;

        return al::map_get(__combine_names2, __combine_name_key2_t(name1, name2),
            [this, name1, name2]() {
                return mname_t::combine(to_mname_operate_context(), name1, name2);
            }
        );
    }

    // Walk
    void ast_walk_context_t::__walk()
    {
        __layer_t * previous_layer = __current_layer;

        __walk_item_t walk_item;
        while (__walk_items.deque(&walk_item))
        {
            __current_layer = walk_item.layer;
            walk_item.node->on_walk(*this, walk_item.step, walk_item.tag);
        }

        __current_layer = previous_layer;
    }

    // Load assembly by package name and assembly name.
    assembly_t * ast_walk_context_t::load_assembly(const mname_t * package_name,
                                                   const mname_t * assembly_name)
    {
        return al::map_get(__assembly_cache, __assembly_cache_key_t(package_name, assembly_name),
            [this, package_name, assembly_name]() {
                return __assembly_loader->load_assembly(package_name, assembly_name);
            }
        );
    }

    // Commit types.
    void ast_walk_context_t::commit_types()
    {
        __XPool.commit_types(logger);
    }

    // Enums each ast nodes.
    void __each_eobjects(ast_node_t * node, std::function<void (eobject_t *)> f)
    {
        eobject_ast_base_t * ast = as<eobject_ast_base_t *>(node);
        if (ast != nullptr)
        {
            ast->each_eobjects(f);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // ast_node_t

    // On walk the ast node.
    bool ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        bool r = true;

        for (size_t index = 0, count = child_count(); index < count; index++)
        {
            ast_node_t * child = child_at(index);
            if (child != nullptr && !child->on_walk(context, 0, nullptr))
            {
                r = false;
            }
        }

        return r;
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns child count of a multipy ast node.
    size_t multipy_ast_node_t::child_count() const
    {
        return __childs.size();
    }

    // Returns child at specified index.
    ast_node_t * multipy_ast_node_t::child_at(size_t index) const
    {
        return __childs[index];
    }

    // Set child at specified index.
    void multipy_ast_node_t::set_child(size_t index, ast_node_t * node)
    {
        while (index >= __childs.size())
            __childs.push_back(nullptr);

        __childs[index] = node;
    }

    // Append a new node.
    void multipy_ast_node_t::append(ast_node_t * node)
    {
        __childs.push_back(node);
    }

    ////////// ////////// ////////// ////////// //////////

    // Applies all subnodes for callback function.
    void expand_ast_node(ast_node_t * node, std::function<void (ast_node_t *)> callback)
    {
        if (node == nullptr)
            return;

        multipy_ast_node_t * multipy_node = as<multipy_ast_node_t *>(node);
        if (multipy_node != nullptr)
        {
            for (size_t index = 0, count = multipy_node->child_count(); index < count; index++)
            {
                ast_node_t * node0 = multipy_node->child_at(index);
                callback(node0);
            }
        }
        else
        {
            callback(node);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // document_ast_node_t

    // On walk document ast node.
    bool document_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        context.push_new_document();
        bool r = __super_t::on_walk(context, step, tag);
        context.pop();

        return r;
    }

    // On commit document ast node.
    void document_ast_node_t::on_commit()
    {
        __super_t::on_commit();
    }

    ////////// ////////// ////////// ////////// //////////
    // module_ast_node_t

    // On walk module ast node.
    bool module_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __module = context.push_new_module();
        bool r = do_walk(context, step, tag);
        context.pop();

        return r;
    }

    // Do walk module ast node.
    bool module_ast_node_t::do_walk(ast_walk_context_t & context, int step, void * tag)
    {
        return __super_t::on_walk(context, step, tag);
    }

    // Commits ast node.
    void module_ast_node_t::on_commit()
    {
        __super_t::on_commit();
    }

    ////////// ////////// ////////// ////////// //////////
    // ast_sulution_t

    // Add a project to a solution.
    void ast_solution_t::add_project(ast_project_t * cproject)
    {
        _A(cproject != nullptr);

        __project_map[to_sid(cproject->name)] = cproject;
        __projects.push_back(cproject);

        cproject->solution = this;
    }

    // Get project of specified name.
    ast_project_t * ast_solution_t::get_project(const mname_t * name)
    {
        auto it = __project_map.find(to_sid(name));
        if (it != __project_map.end())
            return it->second;

        return nullptr;
    }

    ////////// ////////// ////////// ////////// //////////
    // ast_project_t

    // Add a file to a project.
    void ast_project_t::add_file(ast_file_t * cfile)
    {
        _A(cfile != nullptr);

        __file_map[to_sid(cfile->name)] =  cfile;
        __files.push_back(cfile);

        cfile->project = this;
    }

    // Get file of specified name.
    ast_file_t * ast_project_t::get_file(const mname_t * name)
    {
        auto it = __file_map.find(to_sid(name));
        if (it != __file_map.end())
            return it->second;

        return nullptr;
    }

    ////////// ////////// ////////// ////////// //////////
    // expression_execute_error_code_t

    // Expression execute error codes.
    X_ENUM_INFO(expression_execute_error_code_t)

        // Expected a constant expression.
        X_D(expect_const_expression,    _T("'%1%' is not a constant expression"))

        // Invalid binary operator.
        X_D(invalid_binary_operator,    _T("operator '%1%' cannot use for '%2%' and '%3%'"))

        // Invalid unitary operator.
        X_D(invalid_unitary_operator,   _T("operator '%1%' cannot use for '%2%'"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // expression_compile_context_t

    // Constructor.
    expression_compile_context_t::expression_compile_context_t(xilx_write_context_t & ctx)
        : xilx_ctx(ctx), statement_ctx((statement_compile_context_t &)ctx)
        , variable_region(ctx.variable_region)
    { }


    // Relation method compile context.
    expression_compile_context_t::operator method_compile_context_t & () const
    {
        return statement_ctx.mctx;
    }

    // Defines a local variable.
    local_variable_t * expression_compile_context_t::define_temp_local(type_t * type)
    {
        _A(type != nullptr);
        _A(variable_region != nullptr);

        xpool_t & xpool = __XPool;
        variable_region_t * new_variable_region = (variable_region_t *)&__new_variable_region;

        if (__new_variable_region[0] == 0)  // variable region not initialized.
            new ((void *) new_variable_region) variable_region_t(variable_region);

        name_t name = xpool.to_name(_F(_T("__temp_var_%1%__"),
            new_variable_region->current_local_index()
        ));

        return new_variable_region->define_local(xpool.to_type_name(type), name);
    }

    ////////// ////////// ////////// ////////// //////////
    // expression

    // Expression family.
    X_ENUM_INFO(expression_family_t)

        X_C(name,           _T("name"))
        X_C(cvalue,         _T("cvalue"))
        X_C(unitary,        _T("unitary"))
        X_C(binary,         _T("binary"))
        X_C(expressions,    _T("expressions"))
        X_C(function,       _T("function"))
        X_C(index,          _T("index"))
        X_C(new_,           _T("new"))
        X_C(new_array,      _T("new_array"))
        X_C(default_value,  _T("default_value"))
        X_C(type_of,        _T("type_of"))
        X_C(type_name,      _T("type_name"))
        X_C(type_cast,      _T("type_cast"))
        X_C(this_,          _T("this"))
        X_C(base,           _T("base"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // expression_behaviour_t

    // Expression behaviour.
    X_ENUM_INFO(expression_behaviour_t)

        // Default
        X_C(default_,       _T("default"))

        // Assign expression, e.g. =, +=
        X_C(assign,         _T("assign"))

        // Execute, e.g. function_expression.
        X_C(execute,        _T("execute"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // expression_t

    X_ENUM_INFO(type_mark_t)

        // Default, unspecified.
        X_C(default_,       _T("default"))

        // Ref type.
        X_C(ref,            _T("ref"))

        // Value type.
        X_C(value,          _T("value"))

    X_ENUM_INFO_END

    // Converts to string.
    expression_compile_environment_t::operator string_t() const
    {
        if (type != nullptr)
            return type->to_string();

        return _str(dtype);
    }

    // Returns whether it's ref type.
    bool expression_compile_environment_t::is_value() const
    {
        switch (type_mark)
        {
            case type_mark_t::ref:
                return false;

            case type_mark_t::value:
                return true;

            default:
                if (type == nullptr)
                    return false;

                return is_value_type(type);
        }
    }

    // Returns whether it's value type.
    bool expression_compile_environment_t::is_ref() const
    {
        switch (type_mark)
        {
            case type_mark_t::ref:
                return true;

            case type_mark_t::value:
                return false;

            default:
                if (type == nullptr)
                    return false;

                return is_ref_type(type);
        }
    }

    const expression_compile_environment_t expression_compile_environment_t::empty;

    // Converts to a string.
    const string_t expression_t::to_string() const
    {
        auto w = std::bind(al::_wrap_index<size_t>, std::placeholders::_1);
        return al::join_str(w(0), w(expression_count()), _T(", "),
            [this](size_t index) { return _str(expression_at(index)); }
        );
    }

    // Returns code position information.
    const code_unit_t * expression_t::get_code_unit()
    {
        if (code_unit != nullptr)
            return code_unit;

        // TODO: auto execute
        return code_unit;
    }

    ////////// ////////// ////////// ////////// //////////
    // name_expression_t

    // Name expression type.
    X_ENUM_INFO(name_expression_type_t)

        // Variable.
        X_C(variable,   _T("variable"))

        // Type.
        X_C(type,       _T("type"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Converts an expression to a variable.
    // Returns nullptr if it is not a variable expression.
    variable_t * to_variable(expression_t * exp)
    {
        if (exp == nullptr)
            return nullptr;
            
        variable_expression_t * var_exp = as<variable_expression_t *>(exp);
        if (var_exp == nullptr || !var_exp->is_variable_expression())
            return nullptr;

        return var_exp->get_variable();
    }

    //-------- ---------- ---------- ---------- ----------

    // Returns vtype.
    vtype_t name_expression_t::get_vtype() const
    {
        if (variable == nullptr)
            return vtype_t::__unknown__;

        switch (expression_type)
        {
            case name_expression_type_t::variable:
                return variable->get_vtype();

            case name_expression_type_t::type:
                return type->this_vtype();

            case name_expression_type_t::type_def: {
                type_t * type = type_def->get_type();
                return type? type->this_vtype() : vtype_t::__unknown__;
            }   break;

            default:
                return vtype_t::__unknown__;
        }
    }

    // Returns type of name expression.
    type_t * name_expression_t::get_type() const
    {
        if (variable == nullptr)
            return nullptr;

        switch (expression_type)
        {
            case name_expression_type_t::variable:
                return variable->get_type();

            case name_expression_type_t::type:
                return type;

            case name_expression_type_t::type_def:
                return type_def->get_type();

            default:
                return nullptr;
        }
    }

    // Returns type if this is type or type_def
    type_t * name_expression_t::get_actual_type() const
    {
        switch (expression_type)
        {
            case name_expression_type_t::type:
            case name_expression_type_t::type_def:
                return get_type();

            default:
                return nullptr;
        }
    }

    // Sets variable to a name expression.
    void name_expression_t::set(variable_t * variable)
    {
        _A(variable != nullptr);

        this->expression_type = name_expression_type_t::variable;
        this->variable = variable;
    }

    // Sets type to a name expression.
    void name_expression_t::set(type_t * type)
    {
        _A(type != nullptr);

        this->expression_type = name_expression_type_t::type;
        this->type = type;
    }

    // Sets typedef to a name expression.
    void name_expression_t::set(type_def_t * type_def)
    {
        _A(type_def != nullptr);

        this->expression_type = name_expression_type_t::type_def;
        this->type_def = type_def;
    }

    // Executes the expression.
    cvalue_t name_expression_t::execute()
    {
        switch (expression_type)
        {
            case name_expression_type_t::variable:
                return variable == nullptr? cvalue_t::nan : variable->execute();

            case name_expression_type_t::type:
                return type;

            case name_expression_type_t::type_def:
                return cvalue_t::nan;

            default:
                return cvalue_t::nan;
        }
    }

    // Returns behaviour.
    expression_behaviour_t name_expression_t::get_behaviour() const
    {
        if (expression_type == name_expression_type_t::variable && variable != nullptr)
        {
            return variable->is_calling()? expression_behaviour_t::execute
                                         : expression_behaviour_t::default_;
        }

        return expression_behaviour_t::default_;
    }

    // Convert name_expression to a string.
    const string_t name_expression_t::to_string() const
    {
        switch (expression_type)
        {
            case name_expression_type_t::variable:
                return variable? _str(variable) : _str(name);

            case name_expression_type_t::type:
                return type? _str(type) : _str(name);

            case name_expression_type_t::type_def:
                return _str(type_def);

            default:
                return _str(name);
        }
    }

    // Returns type if specified expression is name_expression_t or name_unit_expression_t,
    // and it is a type or type_def.
    type_t * to_actual_type(expression_t * exp)
    {
        _A(exp != nullptr);

        switch (exp->this_family())
        {
            case expression_family_t::name:
            case expression_family_t::name_unit:
                return ((name_expression_t *)exp)->get_actual_type();

            case expression_family_t::type_name:
                return ((type_name_expression_t *)exp)->get_type();

            default:
                return nullptr;
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // name_unit_expression_t

    const string_t name_unit_expression_t::to_string() const
    {
        return _F(_T("%1%<%2%>"), name, _str(generic_args));
    }

    // Returns whether it's a name_expression_t or name_unit_expression_t.
    bool is_name_expression(expression_t * exp)
    {
        if (exp == nullptr)
            return false;

        expression_family_t family = exp->this_family();
        return al::in(family, expression_family_t::name, expression_family_t::name_unit);
    }

    // Returns whether it's a type expression.
    bool is_type_expression(expression_t * exp)
    {
        if (!is_name_expression(exp))
            return false;

        return al::in(((name_expression_t *)exp)->expression_type,
            name_expression_type_t::type, name_expression_type_t::type_def
        );
    }

    // Returns whether it's a variable expression.
    bool is_variable_expression(expression_t * exp, variable_t ** out_variable)
    {
        if (!is_name_expression(exp))
            return false;

        name_expression_t * name_exp = (name_expression_t *)exp;
        if (name_exp->expression_type != name_expression_type_t::variable)
            return false;

        variable_t * variable = name_exp->variable;

        if (variable != nullptr)
        {
            al::assign_value(out_variable, variable);
            return true;
        }

        return false;
    }

    // Returns whetner it's a variable expression.
    bool is_field_variable_expression(expression_t * exp, field_variable_t ** out_variable)
    {
        variable_t * variable;
        if (!is_variable_expression(exp, &variable))
            return false;

        if (variable->this_type() != variable_type_t::field)
            return false;

        al::assign_value(out_variable, (field_variable_t *)variable);
        return true;
    }

    ////////// ////////// ////////// ////////// //////////
    // type_name_expression_t

    // Returns vtype of a type_name expression.
    vtype_t type_name_expression_t::get_vtype() const
    {
        return __get_vtype(type_name);
    }

    // Returns type of type_name expression.
    type_t * type_name_expression_t::get_type() const
    {
        if (type_name == nullptr)
            return nullptr;

        return type_name->type;
    }

    ////////// ////////// ////////// ////////// //////////
    // type_cast_expression_t

    // Set expression.
    void type_cast_expression_t::set_expression(expression_t * expression)
    {
        _A(expression != nullptr);

        exps[0] = expression;
        expression->parent = this;
    }

    // Returns vtype of a type_cast expression.
    vtype_t type_cast_expression_t::get_vtype() const
    {
        return __get_vtype(type_name);
    }

    // Returns type of a type_cast expression.
    type_t * type_cast_expression_t::get_type() const
    {
        return to_type(type_name);
    }

    ////////// ////////// ////////// ////////// //////////
    // cvalue_expression_t

    // Returns vtype of a cvalue expression.
    vtype_t cvalue_expression_t::get_vtype() const
    {
        if (value == nullptr)
            return vtype_t::__unknown__;

        switch (value->value_type)
        {
            case cvalue_type_t::number:
                return to_vtype(value->number.type);

            case cvalue_type_t::string:
                return vtype_t::string_;

            case cvalue_type_t::null:
            case cvalue_type_t::type:
                return vtype_t::mobject_;

            default:
                return vtype_t::__unknown__;
        }
    }

    // Returns type of a cvalue expression.
    type_t * cvalue_expression_t::get_type() const
    {
        if (value == nullptr)
            return nullptr;

        switch (value->value_type)
        {
            case cvalue_type_t::number:
                return __XPool.get_internal_type(to_vtype(value->number.type));

            case cvalue_type_t::string:
                return __XPool.get_string_type();

            case cvalue_type_t::null:
                return null_type_t::instance();

            case cvalue_type_t::type:
                return __XPool.get_type_type();

            default:
                return nullptr;
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // function_expression_t

    X_ENUM_INFO(function_expression_type_t)

        X_C(method,     _T("method"))

        X_C(variable,   _T("variable"))

    X_ENUM_INFO_END

    // Sets namex, namex maybe a name or an expression.
    void function_expression_t::set_namex(expression_t * namex) _NE
    {
        _A(namex != nullptr);

        this->namex = namex;
        namex->parent = this;
    }

    // Sets method.
    void function_expression_t::set_method(method_base_t * method) _NE
    {
        _A(method != nullptr);
        __relation_data.set(method, __ftype_t::method);
    }

    // Gets method.
    method_base_t * function_expression_t::get_method() const _NE
    {
        _A(get_ftype() == __ftype_t::method);
        return (method_base_t *)__relation_data;
    }

    // Sets variable.
    void function_expression_t::set_variable(variable_t * variable) _NE
    {
        _A(variable != nullptr);
        __relation_data.set(variable, __ftype_t::variable);
    }

    // Gets variable.
    variable_t * function_expression_t::get_variable() const _NE
    {
        _A(get_ftype()  == __ftype_t::variable);
        return (variable_t *)__relation_data;
    }

    // Returns name of function_expression.
    name_t function_expression_t::get_name() const
    {
        if (namex != nullptr && namex->this_family() == expression_family_t::name)
            return ((name_expression_t *)namex)->name;

        return name_t::null;
    }

    // Returns delegate return type, (type at index 0).
    type_t * get_delegate_return_type(type_t * type)
    {
        _A(type != nullptr);
        _A(type->this_gtype() == gtype_t::generic);

        generic_type_t * generic_type = (generic_type_t *)type;
        _A(generic_type->template_ == __XPool.get_delegate_type());

        return get_delegate_return_type(generic_type);
    }

    // Returns delegate return type, (type at index 0).
    type_t * get_delegate_return_type(generic_type_t * type)
    {
        _A(type != nullptr);

        return type->type_at(0);  // index 0 is return type of delegate type.
    }

    // Returns type.
    type_t * function_expression_t::get_type() const
    {
        switch (get_ftype())
        {
            case __ftype_t::method:
                return get_method()->get_type();

            case __ftype_t::variable: {
                type_t * type = get_variable()->get_type();
                return get_delegate_return_type(type);
            }

            default: {
                type_t * type;
                if (namex == nullptr || (type = namex->get_type()) == nullptr)
                    return nullptr;

                return get_delegate_return_type(type);
            }
        }
    }

    // Returns vtype of function_expression.
    vtype_t function_expression_t::get_vtype() const
    {
        return __get_vtype(get_type());
    }

    // Converts a function expression to a string.
    const string_t function_expression_t::to_string() const
    {
        if (generic_args == nullptr)
            return _F(_T("%1%(%2%)"), _str(namex), __to_string(arguments()));

        return _F(_T("%1%<%2%>(%3%)"), _str(namex), _str(generic_args), __to_string(arguments()));
    }

    ////////// ////////// ////////// ////////// //////////

    // Sets name to a expression.
    void index_expression_t::set_namex(expression_t * exp)
    {
        _A(exp != nullptr);

        __namex = exp;
        __namex->parent = this;
    }

    // Gets name of a expression.
    expression_t * index_expression_t::namex() const
    {
        return __namex;
    }

    // Returns vtype of a index expression.
    vtype_t index_expression_t::get_vtype() const
    {
        if (variable == nullptr)
            return vtype_t::__unknown__;

        return variable->get_vtype();
    }

    // Returns type of a index expression.
    type_t * index_expression_t::get_type() const
    {
        if (variable == nullptr)
            return nullptr;

        return variable->get_type();
    }

    // Converts index expression to a string.
    const string_t index_expression_t::to_string() const
    {
        return _F(_T("%1%[%2%]"), _str(namex()), __to_string(arguments()));
    }

    ////////// ////////// ////////// ////////// //////////
    // new_expression_t

    // Converts new_expression to a string.
    const string_t new_expression_t::to_string() const
    {
        stringstream_t ss;
        ss << _T("new ") << _str(type_name);

        if (arguments() != nullptr)
            ss << _T("(") << _str(arguments()) << _T(")");
        else
            ss << _T("()");

        return ss.str();
    }

    // Returns vtype of a new_expression.
    vtype_t new_expression_t::get_vtype() const
    {
        return __get_vtype(type_name);
    }

    // Returns type.
    type_t * new_expression_t::get_type() const
    {
        return to_type(type_name);
    }

    ////////// ////////// ////////// ////////// //////////
    // new_array_expression_t

    // Array initialize element types.
    X_ENUM_INFO(array_initialize_element_type_t)

        // Initializer.
        X_C(initializer,        _T("initializer"))

        // Expression.
        X_C(expression,         _T("expression"))

    X_ENUM_INFO_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Array initialize check error codes.
    X_ENUM_INFO(array_initialize_check_error_code_t)

        // Musture elements.
        X_D(mixture_elements,   _T("mixture elements"))

        // Unaliged.
        X_D(unaligned,          _T("unaligned"))

        // Dimension overflow.
        X_D(dimension_overflow, _T("dimension overflow"))

    X_ENUM_INFO_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Converts array initialize element to a string.
    array_initialize_element_t::operator string_t() const
    {
        switch (type)
        {
            case __type_t::initializer:
                return _str(initializer);

            case __type_t::expression:
                return _str(expression);

            default:
                return _T("");
        }
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Appends an expression to an array initializer.
    void array_initializer_t::append(expression_t * expression)
    {
        elements.push_back(array_initialize_element_t(expression));
    }

    // Appends an initializer to an array initializer.
    void array_initializer_t::append(array_initializer_t * initializer)
    {
        elements.push_back(array_initialize_element_t(initializer));
    }

    // Enumerates all elements.
    bool array_initializer_t::each_element(each_element_callback_t f)
    {
        for (array_initialize_element_t & element : elements)
        {
            switch (element.type)
            {
                case __type_t::initializer:
                    if (!element.initializer->each_element(f))
                        return false;
                    break;

                case __type_t::expression:
                    if (!f(element.expression))
                        return false;
                    break;

                default:
                    X_UNEXPECTED();
            }
        }

        return true;
    }

    const size_t __unknown_array_length = (size_t)-1;

    // Array dimension.
    struct __array_dimension_t
    {
        size_t length;
        array_initialize_element_type_t type;
    };

    // Checks array initializer.
    static void __check(array_initializer_t * initializer, __array_dimension_t * dimensions,
                                                           size_t dimension)
    {
        typedef array_initialize_element_t          element_t;
        typedef array_initialize_element_type_t     etype_t;
        typedef array_initialize_check_error_code_t error_code_t;

        __array_dimension_t & d = dimensions[dimension];

        auto & elements = initializer->elements;
        etype_t type = elements[0].type;

        if (d.length == __unknown_array_length)
            d.length = elements.size();
        else if (d.length != elements.size())
            throw _ED(error_code_t::unaligned);

        if (d.length == 0)
            return;

        if (d.type == etype_t::__unknown__)
            d.type = elements[0].type;
        else if (d.type != elements[0].type)
            throw _ED(error_code_t::mixture_elements);

        if (d.type == etype_t::initializer && dimension + 1 > max_array_dimension)
            throw _ED(error_code_t::dimension_overflow);

        for (element_t & element : elements)
        {
            if (element.type != d.type)
                throw _ED(error_code_t::mixture_elements);

            if (d.type == etype_t::initializer)
            {
                array_initializer_t * sub_initializer = element.initializer;
                __check(sub_initializer, dimensions, dimension + 1);
            }
        }
    }

    // Checks array initializer.
    void array_initializer_t::check()
    {
        __array_dimension_t empty_dimension {
            __unknown_array_length, array_initialize_element_type_t::__unknown__
        };

        __array_dimension_t dimensions[max_array_dimension];
        al::assign_array(dimensions, empty_dimension, max_array_dimension);

        __check(this, dimensions, 0);
    }

    // Sets parent for all element expressions.
    void array_initializer_t::set_parent(expression_t * parent)
    {
        this->each_element([parent](expression_t * element_exp) -> bool {
            return element_exp->parent = parent, true;
        });
    }

    // Converts an array initializer to a string.
    X_DEFINE_TO_STRING(array_initializer_t)
    {
        return al::join_str(elements.begin(), elements.end(), _T(", "),
            [](auto it) { return _str(it); }
        );
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    void new_array_expression_t::set_initializer(array_initializer_t * initializer)
    {
        __initializer = initializer;

        if (initializer != nullptr)
            initializer->set_parent(this);
    }

    // Sets lengths for an array expression.
    void new_array_expression_t::set_lengths(array_lengths_t * lengths)
    {
        __lengths = lengths;

        if (lengths != nullptr)
        {
            for (expression_t * exp : *lengths)
            {
                if (exp != nullptr)
                    exp->parent = this;
            }
        }
    }

    // Gets length ast specified dimension.
    expression_t * new_array_expression_t::length_at(dimension_t dimension) const
    {
        if (__lengths == nullptr || dimension >= __lengths->size())
            throw _EC(overflow, _T("overflow when reading array length at specified dimension"));

        return (*__lengths)[dimension];
    }

    // Set length for specified dimension.
    void new_array_expression_t::set_length(dimension_t dimension, expression_t * length)
    {
        if (__lengths == nullptr || dimension >= __lengths->size())
            throw _EC(overflow, _T("overflow when set array length of specified dimension"));

        length->parent = this;
        (*__lengths)[dimension] = length;
    }

    // Converts array expression to a string.
    const string_t new_array_expression_t::to_string() const
    {
        stringstream_t ss;
        ss << _T("new ") << _str(type_name) << _T("[") << _str(lengths()) << _T("]");

        return ss.str();
    }

    // Returns array type.
    type_t * new_array_expression_t::to_array_type() const
    {
        return __XPool.new_array_type(get_element_type(), dimension());
    }

    // Returns element type.
    type_t * new_array_expression_t::get_element_type() const
    {
        return to_type(type_name);
    }

    // Returns vtype.
    vtype_t new_array_expression_t::get_vtype() const
    {
        return vtype_t::mobject_;
    }

    // Returns type.
    type_t * new_array_expression_t::get_type() const
    {
        return to_array_type();
    }

    ////////// ////////// ////////// ////////// //////////
    // default_value_expression_t

    // Returns cvalue of default value expression.
    cvalue_t default_value_expression_t::execute()
    {
        type_t * type;
        if (type_name == nullptr || (type = type_name->type) == nullptr)
            return cvalue_t(nullptr);

        if (type->this_gtype() != gtype_t::general)
            return cvalue_t(nullptr);

        return default_value_of(((general_type_t *)type)->vtype);
    }

    // Returns vtype of default value expression.
    vtype_t default_value_expression_t::get_vtype() const
    {
        return __get_vtype(to_type(type_name));
    }

    // Returns type of default value expression.
    type_t * default_value_expression_t::get_type() const
    {
        return to_type(type_name);
    }

    // Converts default value expression to a string.
    const string_t default_value_expression_t::to_string() const
    {
        return _F(_T("default(%1%)"), _str(type_name));
    }

    ////////// ////////// ////////// ////////// //////////
    // type_of_expression_t

    // Converts a typeof expression to a string.
    const string_t type_of_expression_t::to_string() const
    {
        return _F(_T("typeof(%1%)"), _str(type_name));
    }

    // Executes the typeof expression.
    cvalue_t type_of_expression_t::execute()
    {
        if (type_name == nullptr)
            return cvalue_t();

        return cvalue_t(type_name->type);
    }

    // Returns vtype of a type_of expression.
    vtype_t type_of_expression_t::get_vtype() const
    {
        return vtype_t::mobject_;
    }

    // Returns type of a type_of expression.
    type_t * type_of_expression_t::get_type() const
    {
        return __XPool.get_type_type();
    }

    ////////// ////////// ////////// ////////// //////////
    // this_expression_t

    // Returns vtype of this expression.
    vtype_t this_expression_t::get_vtype() const
    {
        return vtype_t::__unknown__;
    }

    // Returns type of this expression.
    type_t * this_expression_t::get_type() const
    {
        return __type;
    }

    // Converts this expression to a string.
    const string_t this_expression_t::to_string() const
    {
        return _T("this");
    }

    // Executes this expression.
    cvalue_t this_expression_t::execute()
    {
        return cvalue_t::nan;
    }

    ////////// ////////// ////////// ////////// //////////
    // base_expression_t

    // Returns vtype of the base expression.
    vtype_t base_expression_t::get_vtype() const
    {
        return vtype_t::__unknown__;
    }

    // Returns type of the base expression.
    type_t * base_expression_t::get_type() const
    {
        return __type;
    }

    // Converts base expression to a string.
    const string_t base_expression_t::to_string() const
    {
        return _T("base");
    }

    // Executes the base expression.
    cvalue_t base_expression_t::execute()
    {
        return cvalue_t::nan;
    }

    ////////// ////////// ////////// ////////// //////////
    // unitary_expression_t

    // Returns vtype of the unitary expression.
    type_t * unitary_expression_t::get_type() const
    {
        type_t * type = __super_t::get_type();
        if (type != nullptr)
            return type;

        vtype_t vtype = get_vtype();
        return __XPool.get_internal_type(vtype);
    }

    // Converts unitary expression to a string.
    const string_t unitary_expression_t::to_string() const
    {
        return sprintf(_T("%1%%2%"), op(), expression_at(0)->to_string());
    }

    ////////// ////////// ////////// ////////// //////////
    // binary_expression_t

    // Returns type of the binary expression.
    type_t * binary_expression_t::get_type() const
    {
        type_t * type = __super_t::get_type();
        if (type != nullptr)
            return type;

        switch (op())
        {
            case operator_t::member_point:
                if (expression_t * e2 = exp2(); e2 != nullptr)
                    return e2->get_type();
                return nullptr;

            case operator_t::as:
                return to_actual_type(exp2());

            case operator_t::assign:
                return exp1()->get_type();

            case operator_t::null_coalescing_assign:
                return exp1()->get_type();

            case operator_t::null_coalescing:
                if (type_t * type = exp1()->get_type(); !is_null(type))
                    return type;
                return exp2()->get_type();

            default:
                return __XPool.get_internal_type(get_vtype());
        }
    }

    // Returns behaviour.
    expression_behaviour_t binary_expression_t::get_behaviour() const
    {
        switch (op())
        {
            case operator_t::member_point:
                return exp2()->get_behaviour();

            default:
                return __super_t::get_behaviour();
        }
    }

    // Converts binary expression to a string.
    const string_t binary_expression_t::to_string() const
    {
        if (op() == operator_t::member_point)
            return _F(_T("%1%.%2%"), expression_at(0)->to_string(), expression_at(1)->to_string());

        return sprintf(_T("%1% %2% %3%"),
            expression_at(0)->to_string(), op(), expression_at(1)->to_string()
        );
    }

    ////////// ////////// ////////// ////////// //////////
    // expressions_t

    // Return vtype of expressions.
    vtype_t expressions_t::get_vtype() const
    {
        if (__expressions.size() == 0)
            return vtype_t::__unknown__;

        return __expressions[__expressions.size() - 1]->get_vtype();
    }

    // Return type of expressions.
    type_t * expressions_t::get_type() const
    {
        if (__expressions.size() == 0)
            return nullptr;

        return __expressions[__expressions.size() - 1]->get_type();
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns an expression type.
    type_t * get_expression_type(expression_t * expression)
    {
        if (expression == nullptr)
            return nullptr;

        type_t * type = expression->get_type();
        if (type != nullptr)
            return type;

        vtype_t vtype = expression->get_vtype();
        if (vtype == vtype_t::__unknown__)
            return nullptr;

        return __XPool.get_internal_type(vtype);
    }

    ////////// ////////// ////////// ////////// //////////

    // Statement exit point types.
    X_ENUM_INFO(statement_exit_point_type_t)

        X_C(none,       _T("none"))

        // A break point.
        X_C(break_,     _T("break"))

        // A continue point.
        X_C(continue_,  _T("continue"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Appends block.
    void xilx_block_manager_t::append_block(__block_type_t type,
            local_label_t begin, local_label_t end, local_label_t entry_point,
            type_t * relation_type)
    {
        __blocks.push_back(__xilx_block_t {
            type, begin, end, entry_point, relation_type
        });
    }

    // Commits it.
    void xilx_block_manager_t::commit(statement_compile_context_t & ctx)
    {
        // Sort blocks.
    }

    // Writes to buffer.
    void xilx_block_manager_t::write(statement_compile_context_t & ctx, xil_buffer_t & buffer)
    {
        if (__blocks.empty())
            return;

        method_xil_block_stub_t stub(__blocks.size());
        buffer.write(stub);

        al::svector_t<method_xil_block_t, 5> xil_blocks;

        for (__xilx_block_t & block : __blocks)
        {
            method_xil_block_t xil_block;

            xil_block.type          = block.type;
            xil_block.xil_start     = ctx.index_of(block.begin);
            xil_block.xil_end       = ctx.index_of(block.end);
            xil_block.entry_point   = ctx.index_of(block.entry_point);

            _A(xil_block.xil_start <= xil_block.xil_end);

            xil_block.relation_type = ctx.mctx.layout.ref_of(block.relation_type);
            xil_blocks.push_back(xil_block);
        }

        /*
        al::sort(xil_blocks, [](method_xil_block_t & b1, method_xil_block_t b2) {
            return b1.xil_end < b2.xil_end? true  :
                   b1.xil_end > b2.xil_end? false :
                   b1.xil_start > b2.xil_start? true  :
                   b1.xil_start < b2.xil_start? false :
                   b1.entry_point < b2.entry_point;
        });
        */

        for (method_xil_block_t & xil_block : xil_blocks)
        {
            buffer.write(xil_block);
        }
    }

    //-------- ---------- ---------- ---------- ----------

    // Constructor.
    xilx_write_context_t::xilx_write_context_t(statement_compile_context_t & sc_context,
                    xilx_block_manager_t & block_manager, statement_region_t * region)
        : sc_context(sc_context), block_manager(block_manager)
    {
        _A( region != nullptr );
        _A( region->variable_region != nullptr );

        regions.push(region);
        this->variable_region = region->variable_region;
    }

    // Converts to method context.
    xilx_write_context_t::operator method_compile_context_t & () const
    {
        return (method_compile_context_t &)sc_context;
    }

    //-------- ---------- ---------- ---------- ----------

    // Statement exit stateme point.
    class statement_exit_statement_point_t : public statement_point_t
    {
    public:

        // Constructors.
        statement_exit_statement_point_t(statement_region_t * region)
            : __region(region) { }

        // Converts to a position of local label.
        virtual local_label_t to_position() override { return 0; }

    private:
        statement_region_t * __region;
    };

    //-------- ---------- ---------- ---------- ----------

    // Statement region properties.
    X_ENUM_INFO(statement_region_property_t)

        // Default property.
        X_C(none,               _T("none"))

        // A protected block, such as try block.
        X_C(protected_block,    _T("protected_block"))

        // A protected block, with finally.
        X_C(with_finally,       _T("with_finally"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Constructor
    statement_region_t::statement_region_t()
    {
        al::zero_array(__points);
    }

    // Sets point with point type.
    void statement_region_t::set_point(__point_type_t point_type, statement_point_t * point)
    {
        __points[(size_t)point_type] = point;
    }

    // Gets points of point type.
    statement_point_t * statement_region_t::get_point(__point_type_t point_type)
    {
        return __points[(size_t)point_type];
    }

    // Write xilxes to a pool.
    void statement_region_t::write(xilx_write_context_t & ctx, xil_pool_t & pool)
    {
        variable_region_t * old_region = ctx.variable_region;
        ctx.variable_region = al::nvl(this->variable_region, old_region);

        this->each([&ctx, &pool](xilx_t * xilx) {
            xilx->write(ctx, pool);
        });

        ctx.variable_region = old_region;
    }

    //-------- ---------- ---------- ---------- ----------

    // Writes region xilx to a pool.
    void region_xilx_t::write(xilx_write_context_t & ctx, xil_pool_t & pool)
    {
        __region->write(ctx, pool);
    }

    //-------- ---------- ---------- ---------- ----------

    // Writes bytes to a xil buffer.
    void xil_buffer_t::write(const byte_t * bytes, size_t size)
    {
        _A(bytes != nullptr);

        __ensure_size(this->size() + size);
        std::copy(bytes, bytes + size, __p);
        __p += size;
    }

    // Ensure current size is enough to storage specified size.
    void xil_buffer_t::__ensure_size(size_t need_size)
    {
        if (__buffer == nullptr)
        {
            __size = _alignd(need_size, 128);
            __buffer = this->__alloc_objs<byte_t>(__size);
            __p = __buffer;
        }
        else if (__size < need_size)
        {
            __size = _alignd(__size, need_size);
            byte_t * new_buffer = this->__alloc_objs<byte_t>(__size);
            std::copy(__buffer, __p, new_buffer);
            this->__free(__buffer);

            __p = new_buffer + (__p - __buffer);
            __buffer = new_buffer;
        }
    }

    // Read bytes from xil buffer.
    size_t xil_buffer_t::read(byte_t * buffer, size_t size)
    {
        _A(buffer != nullptr);

        if (__p_read == nullptr)
            __p_read = __buffer;

        size = al::min(size, __p_read - __buffer);
        if (size > 0)
        {
            std::copy(__p_read, __p_read + size, buffer);
            __p += size;
        }

        return size;
    }

    // Destructor.
    xil_buffer_t::~xil_buffer_t()
    {
        if (__buffer != nullptr)
        {
            this->__free(__buffer);
        }
    }

    //-------- ---------- ---------- ---------- ----------

    // Root statement region.
    class root_statement_region_t : public statement_region_t
    {
    public:

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return _T("root");
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // xil_pool_t

    // Commits xil pool.
    void xil_pool_t::commit()
    {
        int index = 0;
        auto it_begin = __xils.begin(), it = it_begin, it0 = it;

        for (auto it_end = __xils.end(); it < it_end; it++)
        {
            xil_t * xil = *it;
            __index_of(xil) = index;

            switch (command_of(xil))
            {
                case xil_command_t::smp: {

                    switch (sub_command_of<xil_smp_t>(xil))
                    {
                        case xil_smp_t::label:
                            continue;

                        default:
                            break;
                    }

                }   break;

                default:
                    break;
            }

            *it0++ = xil;
            index++;
        }

        __xils.truncate(it0 - it_begin);
    }

    // Returns index of specified xil.
    xil_index_t xil_pool_t::index_of(xil_t * xil)
    {
        return __index_of(xil);
    }

    ////////// ////////// ////////// ////////// //////////
    // jmp_manager_t

    // Appends jmp xil with label.
    void jmp_manager_t::append_jmp(jmp_xil_t * xil, local_label_t label)
    {
        _A(xil != nullptr);

        __local_jmp_map[xil] = label;
    }

    // Appends jmp xil with name.
    void jmp_manager_t::append_jmp(jmp_xil_t * xil, name_t name)
    {
        _A(xil != nullptr);

        __global_jmp_map[xil] = name;
    }

    // Sets jmp point with label.
    void jmp_manager_t::set_point(local_label_t label, xil_t * xil)
    {
        _A(xil != nullptr);

        __local_point_map[label] = xil;
    }

    // Sets jmp point with name.
    void jmp_manager_t::set_point(name_t name, xil_t * xil)
    {
        _A(xil != nullptr);

        __global_point_map[name] = xil;
    }

    // Returns xil of label, nullptr when not found
    xil_t * jmp_manager_t::label_xil(local_label_t label)
    {
        auto it = __local_point_map.find(label);
        if (it == __local_point_map.end())
            return nullptr;

        return it->second;
    }

    // Commits it.
    void jmp_manager_t::commit(statement_compile_context_t & ctx)
    {
        for (auto && it : __local_jmp_map)
        {
            jmp_xil_t * jmp_xil = it.first;
            local_label_t label = it.second;

            xil_t * label_xil = this->label_xil(label);
            if (label_xil == nullptr)
                throw _ED(compile_error_code_t::label_not_found, label);

            xil_index_t index1 = ctx.xil_pool().index_of(jmp_xil);
            xil_index_t index2 = ctx.xil_pool().index_of(label_xil);

            jmp_xil->set_step(index2 - index1);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // statement_compile_context_t

    // Context for compile statements.
    statement_compile_context_t::statement_compile_context_t(
                method_compile_context_t & mctx, method_t * method)
        : mctx(mctx), method(method), __xheap(_T("statement_compile_context"))
    {
        _A(method != nullptr && method->body != nullptr);

        auto * region = begin_region<root_statement_region_t>();
        region->variable_region = method->body->variable_region;

        _A( region->variable_region != nullptr );
    }

    // Completes a region.
    statement_region_t * statement_compile_context_t::end_region()
    {
        _A(__current_region != nullptr);
        
        __current_region = __current_region->parent;
        return __current_region;
    }

    // Returns assembly.
    assembly_t * statement_compile_context_t::get_assembly()
    {
        if (__assembly == nullptr)
            __assembly = method->get_assembly();

        return __assembly;
    }

    // Sets point with point_type.
    void statement_compile_context_t::set_point(__point_type_t point_type, statement_point_t * point)
    {
        statement_region_t * region = current_region();
        _A(region != nullptr);

        region->set_point(point_type, point);
    }

    // Returns index of local label.
    xil_index_t statement_compile_context_t::index_of(local_label_t label)
    {
        xil_t * label_xil = jmp_manager.label_xil(label);
        if (label_xil == nullptr)
            throw _ED(compile_error_code_t::label_not_found, label);

        return xil_pool().index_of(label_xil);
    }

    ////////// ////////// ////////// ////////// //////////

    type_name_t * __get_owner_type_name(member_t * member)
    {
        switch (member->this_type())
        {
            case member_type_t::property:
                return ((property_t *)member)->owner_type_name;

            case member_type_t::event:
                return ((event_t *)member)->owner_type_name;

            case member_type_t::method:
                return ((method_t *)member)->owner_type_name;

            default:
                return nullptr;
        }
    }

    // Returns owner type of member.
    type_t * get_owner_type(member_t * member)
    {
        _A(member != nullptr);

        return to_type(__get_owner_type_name(member));
    }

    // Check whether the access is allowed.
    bool check_access(type_t * from_type, member_t * to_member)
    {
        _A(from_type != nullptr);
        _A(to_member != nullptr);

        type_t * host_type = to_member->host_type;
        _A(host_type != nullptr);

        return check_access(from_type, host_type, to_member->get_access_value());
    }

    // Check whether the access is allowed.
    bool check_access(type_t * from_type, method_base_t * to_method)
    {
        _A(from_type != nullptr);
        _A(to_method != nullptr);

        type_t * host_type = to_method->get_host_type();
        _A(host_type != nullptr);

        return check_access(from_type, host_type, to_method->get_access_value());
    }

    // Check member access permission.
    bool __check_member_access(type_t * from_type, type_t * host_type, access_value_t av,
                                                                    bool cross_assembly)
    {
        typedef access_value_t t;

        if (cross_assembly) // Cross assembly.
        {
            if (av == t::public_)
                return true;

            if (is_super_type(from_type, host_type) &&
                al::in(av, t::protected_, t::protected_or_internal))
                return true;

            return false;
        }
        else    // Inner assembly.
        {
            if (al::in(av, t::public_, t::internal, t::protected_or_internal))
                return true;

            if (is_super_type(from_type, host_type) && al::in(av, t::protected_,
                t::protected_and_internal))
                return true;

            return false;
        }
    }

    // Check type access permission.
    bool __check_type_access(type_t * from_type, type_t * type, bool cross_assembly)
    {
        type_t * host_type = to_general(type->host_type);

        if (host_type != nullptr)   // Nest type
        {
            if (host_type == from_type)
                return true;

            if (!__check_type_access(from_type, host_type, cross_assembly))
                return false;

            access_value_t av = type->get_access_value();
            return __check_member_access(from_type, host_type, av, cross_assembly);
        }
        else
        {
            access_value_t av = type->get_access_value();

            if (cross_assembly)
                return av == access_value_t::public_;

            return true;
        }
    }

    // Check type access permission.
    bool __check_type_access(assembly_t * from_assembly, type_t * type, bool cross_assembly)
    {
        type_t * host_type = to_general(type->host_type);

        if (host_type != nullptr)   // Nest type
        {
            if (!__check_type_access(from_assembly, host_type, cross_assembly))
                return false;

            access_value_t av = type->get_access_value();

            if (cross_assembly)
                return av == access_value_t::public_;

            return al::in(av, access_value_t::public_, access_value_t::internal);
        }
        else
        {
            access_value_t av = type->get_access_value();

            if (cross_assembly)
                return av == access_value_t::public_;

            return true;
        }
    }

    type_t * __to_general_with_check(type_t * type)
    {
        _A(type != nullptr);

        type = to_general(type);
        _A(type != nullptr);

        return type;
    }

    // Check whether the access is allowed.
    bool check_access(type_t * from_type, type_t * host_type, access_value_t av)
    {
        from_type = __to_general_with_check(from_type);
        host_type = __to_general_with_check(host_type);

        // Inner type or nest type.
        {
            type_t * host_type1 = from_type;

            do
            {
                if (host_type1 == host_type)
                    return true;

            } while (to_general((host_type1 = host_type1->host_type)) != nullptr);
        }

        // Others.
        bool cross_assembly = (host_type->get_assembly() != from_type->get_assembly());
        if (!__check_type_access(from_type, host_type, cross_assembly))
            return false;

        return __check_member_access(from_type, host_type, av, cross_assembly);
    }

    // Check whether the access is allowed.
    bool check_access(type_t * from_type, type_t * to_type)
    {
        from_type = __to_general_with_check(from_type);
        to_type = __to_general_with_check(to_type);

        if (to_type->this_gtype() != gtype_t::general)
            return true;

        bool cross_assembly = (to_type->get_assembly() != from_type->get_assembly());
        return __check_type_access(from_type, to_type, cross_assembly);
    }

    // Check whether the access is allowed.
    bool check_access(assembly_t * from_assembly, type_t * host_type)
    {
        _A(from_assembly != nullptr);
        host_type = __to_general_with_check(host_type);

        bool cross_assembly = (host_type->get_assembly() != from_assembly);
        return __check_type_access(from_assembly, host_type, cross_assembly);
    }

    // Check whether the access is allowed.
    bool __check_member_access(assembly_t * from_assembly, type_t * host_type, access_value_t av)
    {
        bool cross_assembly = (host_type->get_assembly() != from_assembly);

        if (!cross_assembly)
            return true;

        return av == access_value_t::public_;
    }

    // Check whether the access is allowed.
    bool check_access(assembly_t * from_assembly, member_t * to_member)
    {
        _A(from_assembly != nullptr);
        _A(to_member != nullptr);

        type_t * host_type = to_member->host_type;
        _A(host_type != nullptr);

        if (!__check_member_access(from_assembly, host_type, to_member->get_access_value()))
            return false;

        return check_access(from_assembly, host_type);
    }

    // Check whether the access is allowed.
    bool check_access(assembly_t * from_assembly, method_base_t * to_method)
    {
        _A(from_assembly != nullptr);
        _A(to_method != nullptr);

        type_t * host_type = to_method->get_host_type();
        _A(host_type != nullptr);

        access_value_t av = to_method->get_access_value();
        if (!__check_member_access(from_assembly, host_type, av))
            return false;

        return check_access(from_assembly, host_type);
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::core


