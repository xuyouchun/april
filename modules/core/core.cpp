
#include <core.h>
#include "expression_executor.h"

namespace X_ROOT_NS { namespace modules { namespace core {

    using namespace al;

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(token_error_code_t)
        X_C(comment_end_missing,    _T("multiline comment end \"*/\" missing"))
        X_C(unknown_token,          _T("unknown token"))
        X_C(string_format_error,    _T("string format error"))
        X_C(char_format_error,      _T("char format error"))
        X_C(numeric_format_error,   _T("numeric format error"))
        X_C(numeric_overflow,       _T("numeric overflow"))
    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    X_ENUM_INFO(compile_error_code_t)

        X_D(duplicated_entry_point, _T("duplicated entry point"))

        X_D(entry_point_prototype_mistake__should_no_return_type,
                                    _T("entry point method should no return type"))

        X_D(entry_point_prototype_mistake__should_not_generic,
                                    _T("entry point method should not a generic method"))

        X_D(entry_point_prototype_mistake__should_no_params,
                                    _T("entry point method should has no params"))

        X_D(entry_point_prototype_mistake__should_be_public_static,
                                    _T("entry point method should be a public static method"))

        X_D(entry_point_prototype_mistake__decorate_not_allowed,
                                    _T("entry point method should not be %1%"))

        X_D(extern_method_prototype_mistake__should_be_static,
                                    _T("external method should be static"))

        X_D(extern_method_prototype_mistake__should_be_extern,
                                    _T("internal method should be external"))

        X_D(extern_method_prototype_mistake__now_allowed,
                                    _T("%1% method not allowed in external method"))

        X_D(type_not_found, _T("type '%1%' not found"))

        X_D(label_not_found, _T("label '%1%' not found"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    X_ENUM_INFO(cvalue_type_t)
        X_C(number,     _T("number"))
        X_C(string,     _T("string"))
        X_C(null,       _T("null"))
        X_C(type,       _T("type"))
        X_C(nan,        _T("nan"))
    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    X_ENUM_INFO(operator_direct_t)

        X_C(left_to_right,      _T("left_to_right"))

        X_C(right_to_left,      _T("right_to_left"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    X_ENUM_INFO(expression_box_side_t)

        X_C(left,       _T("left"))
        X_C(right,      _T("right"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

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

    X_ENUM_INFO(operator_t)

        X_C(add,                _T("+"))
        X_C(sub,                _T("-"))
        X_C(mul,                _T("*"))
        X_C(div,                _T("/"))
        X_C(mod,                _T("%"))

        X_C(add_assign,         _T("+="))
        X_C(sub_assign,         _T("-="))
        X_C(mul_assign,         _T("*="))
        X_C(div_assign,         _T("/="))
        X_C(mod_assign,         _T("%="))

        X_C(left_increment,     _T("++"))
        X_C(left_decrement,     _T("--"))
        X_C(right_increment,    _T("++"))
        X_C(right_decrement,    _T("--"))

        X_C(minus,              _T("-"))
        X_C(positive,           _T("+"))

        X_C(left_shift,         _T("<<"))
        X_C(right_shift,        _T(">>"))
        X_C(left_shift_assign,  _T("<<="))
        X_C(right_shift_assign, _T(">>="))

        X_C(bit_and,            _T("&"))
        X_C(bit_or,             _T("|"))
        X_C(bit_not,            _T("~"))
        X_C(bit_xor,            _T("^"))

        X_C(bit_and_assign,     _T("&="))
        X_C(bit_or_assign,      _T("|="))
        X_C(bit_xor_assign,     _T("^="))

        X_C(assign,             _T("="))

        X_C(greater,            _T(">"))
        X_C(greater_equal,      _T(">="))
        X_C(less,               _T("<"))
        X_C(less_equal,         _T("<="))
        X_C(equal,              _T("=="))
        X_C(not_equal,          _T("!="))

        X_C(logic_and,          _T("&&"))
        X_C(logic_or,           _T("||"))
        X_C(logic_not,          _T("!"))

        X_C(member_point,       _T("."))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // log_level_t

    X_ENUM_INFO(log_level_t)

        X_C(info,       _T("info"))
        X_C(warning,    _T("warning"))
        X_C(error,      _T("error"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // efamily_t

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

    template<typename t>
    const string_t __to_string(const t * obj)
    {
        if(obj == nullptr)
            return empty_str;

        return _str(obj);
    }

    template<typename t, typename s_t>
    const string_t __to_string(const t * obj, s_t && default_str)
    {
        if(obj == nullptr)
            return default_str;

        return _str(obj);
    }

    ////////// ////////// ////////// ////////// //////////

    operator_property_t::operator string_t() const
    {
        return _F(_T("%1%: direct: %2%, priority:%3%, arity: %4%, ")
                  _T("adhere: %5%, is_assign: %6%"),
            op, direct, priority, arity, adhere, is_assign
        );
    }

    struct __system_operator_property_t : operator_property_t
        , al::property_t<__system_operator_property_t, operator_t>
    {
        static void init(enumerator_t & e)
        {
            using namespace std::placeholders;
            typedef operator_t op_t;

            e.each([](operator_t op, operator_property_t & p) { p.op = op; });

            // priority
            auto set_priority = [](operator_property_t & p, operator_priority_t priority) {
                p.priority = priority;
            };

            #define __Pri(v) ((v) << 8)

            e.each(std::bind(set_priority, _2, __Pri(1)),
                op_t::member_point                                    // .
            );

            e.each(std::bind(set_priority, _2, __Pri(2)), 
                op_t::minus,             op_t::positive,              // - +
                op_t::left_increment,    op_t::left_decrement,        // --x ++x
                op_t::right_increment,   op_t::right_decrement,       // x-- x++
                op_t::logic_not,         op_t::bit_not                // ! ~
            );

            e.each(std::bind(set_priority, _2, __Pri(3)),
                op_t::div,   op_t::mul,   op_t::mod                    // / * %
            );

            e.each(std::bind(set_priority, _2, __Pri(4)),
                op_t::add,   op_t::sub                                // + -
            );

            e.each(std::bind(set_priority, _2, __Pri(5)),
                op_t::left_shift,    op_t::right_shift                // << >>
            );

            e.each(std::bind(set_priority, _2, __Pri(6)),
                op_t::greater,   op_t::greater_equal,                 // > >=
                op_t::less,      op_t::less_equal                     // < <=
            );

            e.each(std::bind(set_priority, _2, __Pri(7)),
                op_t::equal,     op_t::not_equal                      // == !=
            );

            e.each(std::bind(set_priority, _2, __Pri(8)),
                op_t::bit_and                                        // &
            );

            e.each(std::bind(set_priority, _2, __Pri(9)),
                op_t::bit_xor                                        // ^
            );

            e.each(std::bind(set_priority, _2, __Pri(10)),
                op_t::bit_or                                         // |
            );

            e.each(std::bind(set_priority, _2, __Pri(11)),
                op_t::logic_and                                      // &&
            );

            e.each(std::bind(set_priority, _2, __Pri(12)),
                op_t::logic_or                                       // ||
            );

            e.each(std::bind(set_priority, _2, __Pri(13)),
                op_t::assign,                                        // =
                op_t::add_assign,    op_t::sub_assign,                // += -=
                op_t::mul_assign,    op_t::div_assign,    op_t::mod_assign, // *= /= %=
                op_t::left_shift_assign, op_t::right_shift_assign,    // <<= >>=
                op_t::bit_and_assign,op_t::bit_or_assign, op_t::bit_xor_assign // &= |= ^=
            );

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

                op_t::greater,       op_t::greater_equal,               // > >=
                op_t::less,          op_t::less_equal,                  // < <=
                op_t::equal,         op_t::not_equal,                   // == !=

                op_t::logic_and,     op_t::logic_or                     // && ||
            );

            // adhere
            auto set_adhere = [](operator_property_t & p, operator_adhere_t adhere) {
                p.adhere = adhere;
            };

            e.each(std::bind(set_adhere, _2, 1));

            e.each(std::bind(set_adhere, _2, 0),
                op_t::left_increment, op_t::left_decrement,           // ++x --x
                op_t::minus,          op_t::positive,                 // +x -x
                op_t::bit_not,        op_t::logic_not                 // ~ !
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
                op_t::bit_and_assign, op_t::bit_or_assign, op_t::bit_xor_assign
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
                op_t::right_increment, op_t::right_decrement
            );
        }
    };

    typedef __system_operator_property_t::properties_t<(size_t)operator_t::__inner_end__>
        __system_operator_properties_t;

    const operator_property_t * get_system_operator_property(operator_t op)
    {
        static __system_operator_properties_t system_operator_properties;

        if(__system_operator_properties_t::overlimit(op))
            return nullptr;

        return &system_operator_properties[op];
    }

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(access_value_t)

        X_C(public_,                _T("public"))
        X_C(protected_,             _T("protected"))
        X_C(private_,               _T("private"))
        X_C(internal,               _T("internal"))
        X_C(protected_or_internal,  _T("protected | internal"))
        X_C(protected_and_internal, _T("protected & internal"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    X_DEFINE_TO_STRING(type_name_t)
    {
        return type? _str(type) : _T("?");
    }

    type_t * to_type(type_name_t * type_name)
    {
        if(type_name == nullptr)
            return nullptr;

        return type_name->type;
    }

    ////////// ////////// ////////// ////////// //////////

    void attribute_t::set_owner(eobject_t * object)
    {
        _A(object != nullptr);

        this->bind_object = object;
    }

    void attribute_t::commit(eobject_commit_context_t & ctx)
    {
        general_type_t * type;

        if(type_name && (type = as<general_type_t *>(type_name->type))
            && type->impl != nullptr)
        {
            type->impl->commit(ctx, this->bind_object);
        }
    }

    X_DEFINE_TO_STRING(attribute_t)
    {
        return _F(_T("%1%(%2%)"), __to_string(type_name, _T("?")), arguments);
    }

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(argument_name_type_t)

        X_C(param,      _T("param"))
        X_C(field,      _T("field"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // argument_t

    type_t * argument_t::get_type(xpool_t & xpool) const
    {
        if(expression != nullptr)
            return expression->get_type(xpool);

        return nullptr;
    }

    argument_t::operator string_t() const
    {
        stringstream_t ss;

        if(atype != param_type_t::__default__)
            ss << _title(atype) << _T(" ");

        if(!name.empty())
        {
            ss << _str(name)
               << (name_type == argument_name_type_t::param? _T(":") : _T("="));
        }

        ss << _str(expression);
        return ss.str();
    }

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(param_type_t)

        X_C(ref,        _T("ref"))
        X_C(out,        _T("out"))
        X_C(params,     _T("params"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    class __code_element_t : public code_element_t
    {
    public:
        __code_element_t(const char_t * s, int32_t length, ast_file_t * file)
            : __unit(s, length, file), code_element_t(&__unit)
        { }

    private:
        code_unit_t __unit;
    };

    //-------- ---------- ---------- ---------- ----------

    code_element_t * combine(memory_t * memory, code_element_t * from, code_element_t * to)
    {
        if(from == nullptr || to == nullptr)
            return from != nullptr? from : to;

        if(from->code_unit == nullptr || to->code_unit == nullptr)
            return from->code_unit != nullptr? from : to;

        _A(from->code_unit->file == to->code_unit->file);

        if(to->code_unit->s < from->code_unit->s)
            return from;

        return memory_t::new_obj<__code_element_t>(memory,
            from->code_unit->s,
            to->code_unit->s - from->code_unit->s + to->code_unit->length,
            from->code_unit->file
        );
    }

    ////////// ////////// ////////// ////////// //////////

    ostream_t & operator << (ostream_t & stream, const token_t & w)
    {
        return stream << w.code_unit;
    }

    ////////// ////////// ////////// ////////// //////////
    // mname_t

    mname_t * mname_operate_context_t::new_mname()
    {
        typedef obj_wrap_t<mname_t> wmname_t;
        return memory_t::new_obj<wmname_t>(memory);
    }

    mname_t * mname_operate_context_t::new_mname(const sid_t & sid)
    {
        typedef obj_wrap_t<mname_t> wmname_t;
        return memory_t::new_obj<wmname_t>(memory, sid);
    }

    mname_t * mname_operate_context_t::new_mname(const mname_t & mname)
    {
        typedef obj_wrap_t<mname_t> wmname_t;
        return memory_t::new_obj<wmname_t>(memory, mname);
    }

    void mname_t::commit(al::spool_t & pool)
    {
        string_t s = al::join_str(parts.begin(), parts.end(), _T("."));
        this->sid = pool.to_sid(s);
    }

    const char_t * mname_t::part_of(size_t index) const
    {
        _A(index < parts.size());

        sid_t sid = parts[index];
        return sid.c_str();
    }

    const char_t * mname_t::last_part() const
    {
        if(empty())
            return nullptr;

        return part_of(parts.size() - 1);
    }

    const mname_t * mname_t::parse(__context_t && ctx, const string_t & s)
    {
        return parse(ctx, s);
    }

    const mname_t * mname_t::parse(__context_t & ctx, const string_t & s)
    {
        if(s.length() == 0)
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

    const mname_t * mname_t::combine(__context_t && ctx, const mname_t * name1,
                                                         const mname_t * name2)
    {
        return combine(ctx, name1, name2);
    }

    const mname_t * mname_t::combine(__context_t & ctx, const mname_t * name1,
                                                        const mname_t * name2)
    {
        if(name1 == nullptr || name2 == nullptr)
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

    const mname_t * mname_t::combine(__context_t && ctx, const mname_t * name1,
                                                         const name_t & name2)
    {
        return combine(ctx, name1, name2);
    }

    const mname_t * mname_t::combine(__context_t & ctx, const mname_t * name1,
                                                        const name_t & name2)
    {
        return combine(std::forward<__context_t>(ctx), name1, to_mname(ctx, name2));
    }

    const mname_t * mname_t::get_parent(__context_t && ctx, const mname_t * name)
    {
        if(name == nullptr || name->parts.size() <= 1)
            return nullptr;

        if(name->parts.size() == 2)
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

    const mname_t * mname_t::get_parent(__context_t & ctx, const mname_t * name)
    {
        return get_parent(ctx, name);
    }

    const mname_t * mname_t::get_child(__context_t && ctx, const mname_t * name)
    {
        if(name == nullptr || name->parts.size() <= 1)
            return nullptr;

        if(name->parts.size() == 2)
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


    const mname_t * mname_t::to_mname(__context_t && ctx, const name_t & name)
    {
        return to_mname(ctx, name);
    }

    const mname_t * mname_t::to_mname(__context_t &  ctx, const name_t & name)
    {
        return ctx.from_cache((sid_t)name, [&ctx, name]() {
            return ctx.new_mname((sid_t)name);
        });
    }

    const mname_t * mname_t::to_internal(__context_t && ctx, const mname_t * mname)
    {
        return to_internal(ctx, mname);
    }

    const mname_t * mname_t::to_internal(__context_t &  ctx, const mname_t * mname)
    {
        _A(mname != nullptr);

        return ctx.from_cache((sid_t)*mname, [&ctx, mname]() {
            return ctx.new_mname(*mname);
        });
    }

    const mname_t mname_t::null;

    ////////// ////////// ////////// ////////// //////////
    // decorate_value_t

    const decorate_value_t decorate_value_t::default_value{};
    const decorate_t decorate_t::default_value{};

    decorate_value_t::operator string_t() const
    {
        al::small_vector_t<const char_t * > ss;

        #define __AppStr(name)  \
            do { if(is_##name) ss.push_back(_S(name)); } while(false)

        if(access != access_value_t::__default__)
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
    // name_t

    const name_t name_t::null(sid_t::null);

    ////////// ////////// ////////// ////////// //////////

    X_DEFINE_TO_STRING(cvalue_t)
    {
        switch(value_type)
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

    bool cvalue_t::operator == (const cvalue_t & other) const
    {
        if(value_type != other.value_type)
            return false;

        switch(value_type)
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

    bool cvalue_t::operator != (const cvalue_t & other) const
    {
        return !operator == (other);
    }

    bool cvalue_t::operator < (const cvalue_t & other) const
    {
        if(value_type != other.value_type)
            return value_type < other.value_type;

        switch(value_type)
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

    bool cvalue_t::operator <= (const cvalue_t & other) const
    {
        if(value_type != other.value_type)
            return value_type <= other.value_type;

        switch(value_type)
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

    bool cvalue_t::operator >= (const cvalue_t & other) const
    {
        return ! operator < (other);
    }

    bool cvalue_t::operator > (const cvalue_t & other) const
    {
        return ! operator <= (other);
    }

    bool cvalue_t::operator == (bool value) const
    {
        return value_type == cvalue_type_t::number
            && number.type == value_type_t::bool_
            && number.value.bool_value == value;
    }

    bool cvalue_t::operator != (bool value) const
    {
        return ! operator == (value);
    }

    const cvalue_t cvalue_t::nan(cvalue_type_t::nan);
    const cvalue_t cvalue_t::null(nullptr);

    ////////// ////////// ////////// ////////// //////////
    // member_t

    assembly_t * get_assembly(member_t * member)
    {
        _A(member != nullptr);

        type_t * type;
        if(member->this_type() == member_type_t::type)
            type = (type_t *)member;
        else
            type = member->host_type;

        switch(type->this_gtype())
        {
            case gtype_t::general:
                return ((general_type_t *)type)->assembly;

            default:
                return nullptr;
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // param_t

    type_t * param_t::get_type() const
    {
        if(type_name != nullptr)
            return type_name->type;

        return nullptr;
    }

    param_t::operator string_t() const
    {
        return _F(_T("%1% %2%"), type_name, name);
    }

    ////////// ////////// ////////// ////////// //////////
    // method_t

    X_ENUM_INFO(method_inline_type_t)

        X_C(default_,   _T("default"))

        X_C(always,     _T("always"))

        X_C(never,      _T("never"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    X_ENUM_INFO(method_trait_t)

        X_C(normal,             _T("normal"))

        X_C(constructor,        _T("constructor"))

        X_C(static_constructor, _T("static_constructor"))

        X_C(destructor,         _T("destructor"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    void local_variables_t::append(local_variable_t * variable)
    {
        _A(variable != nullptr);

        __variables.push_back(variable);
    }

    void local_variables_t::write(xpool_t & xpool, __assembly_layout_t & layout,
                                                        xil_buffer_t & buffer)
    {
        int variable_count = __variables.size();
        if(variable_count == 0)
            return;

        method_variable_stub_t stub(__variables.size());
        stub.write(buffer);

        for(local_variable_t * variable : __variables)
        {
            type_t * type = variable->get_type();
            ref_t ref = layout.ref_of(type);
            if(ref == ref_t::null)
                throw _ED(compile_error_code_t::type_not_found, type);

            local_variable_defination_t defination = { ref, variable->index };
            defination.write(xpool, layout, buffer);
        }
    }

    //-------- ---------- ---------- ---------- ----------

    void method_stub_t::write(xil_buffer_t & buffer)
    {
        buffer.write(*(uint32_t *)this);
    }

    //-------- ---------- ---------- ---------- ----------

    void switch_table_t::write(xil_buffer_t & buffer)
    {
        switch_table_header_t header = *this;
        buffer.write(header);

        buffer.write(default_step);

        for(switch_row_t & row : *this)
        {
            buffer.write(row.value);
            buffer.write(row.step);
        }
    }

    //-------- ---------- ---------- ---------- ----------

    switch_table_t * switch_manager_t::append_table(int row_count)
    {
        switch_table_t * tbl = new (__heap.alloc(
            sizeof(switch_table_t) + sizeof(switch_row_t) * row_count
        )) switch_table_t(__tbl_index++, row_count);

        __tables.push_back(tbl);
        return tbl;
    }

    static int __index_of(statement_compile_context_t & ctx, local_label_t label)
    {
        xil_t * label_xil = ctx.jmp_manager.label_xil(label);
        if(label_xil == nullptr)
            throw _ED(compile_error_code_t::label_not_found, label);

        return ctx.xil_pool().index_of(label_xil);
    }

    void switch_manager_t::commit(statement_compile_context_t & ctx)
    {
        for(switch_table_t * tbl : __tables)
        {
            _A(tbl->jmp_xil != nullptr);
            int switch_index = ctx.xil_pool().index_of(tbl->jmp_xil);

            for(switch_row_t & row : *tbl)
            {
                row.step = __index_of(ctx, row.label) - switch_index;
            }

            tbl->default_step = __index_of(ctx, tbl->default_label) - switch_index;
        }
    }

    void switch_manager_t::write(xil_buffer_t & buffer)
    {
        method_switch_table_stub_t(__tables.size()).write(buffer);

        for(switch_table_t * tbl : __tables)
        {
            tbl->write(buffer);
        }
    }

    //-------- ---------- ---------- ---------- ----------

    void local_variable_defination_t::write(xpool_t & xpool, __assembly_layout_t & layout,
                                                            xil_buffer_t & buffer)
    {
        buffer.write(type);
        buffer.write(index);
    }

    //-------- ---------- ---------- ---------- ----------

    void method_t::compile(method_compile_context_t & mctx)
    {
        if(body == nullptr)
            return;

        statement_compile_context_t sctx(mctx, this);

        __local_variables.write(mctx.xpool, mctx.layout, mctx.buffer);

        // body
        for(statement_t * statement : body->statements)
        {
            statement->compile(sctx);
        }

        statement_region_t * region = sctx.current_region();
        if(region == nullptr)
            throw _EC(unexpected, _T("region stack empty"));

        if(region->parent != nullptr)
            throw _EC(unexpected, _T("region stack error"));

        region->each([&sctx, &mctx](xilx_t * xilx) {
            xilx->write(sctx, mctx.xil_pool);
        });

        mctx.xil_pool.commit();
        sctx.jmp_manager.commit(sctx);

        sctx.switch_manager.commit(sctx);
        sctx.switch_manager.write(mctx.buffer);
    }

    template<typename _params_t>
    auto __find_param(_params_t * params, const name_t & name)
        -> typename _params_t::value_type
    {
        if(params == nullptr)
            return nullptr;

        for(auto param : *params)
        {
            if(param->name == name)
                return param;
        }

        return nullptr;
    }

    generic_param_t * method_t::find_param(const name_t & name)
    {
        return __find_param(generic_params, name);
    }

    void method_t::append_local(local_variable_t * variable)
    {
        __local_variables.append(variable);
    }

    method_t::operator string_t() const
    {
        stringstream_t ss;
        if(type_name != nullptr)
            ss << _str(type_name) << _T(" ");

        ss << _str(name);

        if(generic_param_count() > 0)
            ss << _T("<") << _str(generic_params) << _T(">");

        ss << _T("(");
        if(param_count() > 0)
            ss << _str(params);
        ss << _T(")");

        return ss.str();
    }

    ////////// ////////// ////////// ////////// //////////
    // property_t

    property_t::operator string_t() const
    {
        return _F(_T("%1% %2%"), _str(type_name), _str(name));
    }

    ////////// ////////// ////////// ////////// //////////
    // event_t

    event_t::operator string_t() const
    {
        return _F(_T("%1% %2%"), _str(type_name), _str(name));
    }

    ////////// ////////// ////////// ////////// //////////
    // field_t

    field_t::operator string_t() const
    {
        if(init_value == nullptr)
            return _F(_T("%1% %2%"), _str(type_name), _str(name));

        return _F(_T("%1% %2% = %3%"), _str(type_name), _str(name), _str(init_value));
    }

    msize_t field_t::get_size()
    {
        if(__size != unknown_msize)
            return __size;

        return __size = __get_size();
    }

    msize_t field_t::__get_size()
    {
        if(type_name == nullptr || type_name->type == nullptr)
            return unknown_msize;

        type_t * type = type_name->type;
        ttype_t ttype;

        switch(type->this_gtype())
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

        switch(ttype)
        {
            case ttype_t::struct_:
                return type->get_vsize();

            case ttype_t::enum_:
                return 1;       // TODO

            default:
                return get_vtype_size(vtype_t::mobject);
        }
    }

    type_t * field_t::get_type() const
    {
        return type_name? type_name->type : nullptr;
    }

    bool field_t::is_ref_type() const
    {
        type_t * type = get_type();
        if(type == nullptr)
            return true;                    // treat as object

        ttype_t ttype;

        switch(type->this_gtype())
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

        return ttype != ttype_t::struct_;
    }

    ////////// ////////// ////////// ////////// //////////
    // type_def_param_t

    member_t * type_def_param_t::get_member(analyze_member_args_t & args)
    {
        return nullptr;
    }

    void type_def_param_t::get_members(analyze_members_args_t & args)
    {
        
    }

    void type_def_param_t::get_all_members(members_t & out_members)
    {

    }

    ////////// ////////// ////////// ////////// //////////
    // type_def_t

    type_def_param_t * type_def_t::find_param(const name_t & name)
    {
        return __find_param(params, name);
    }

    type_t * type_def_t::get_type() const
    {
        if(type_name == nullptr)
            return nullptr;

        return type_name->type;
    }

    type_def_t::operator string_t() const
    {
        if(param_count() == 0)
            return _F(_T("typedef %1% %2%"), _str(type_name), _str(name));

        return _F(_T("typedef %1% %2%<%3%>"), _str(type_name), _str(name), _str(params));
    }

    ////////// ////////// ////////// ////////// //////////
    // vtype_t

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

        X_C(string,     _T("string"))

        X_C(void_,      _T("void"))

        X_C(ptr,        _T("ptr"))

        X_C(mobject,    _T("mobject"))

    X_ENUM_INFO_END
    
    //-------- ---------- ---------- ---------- ----------

    vtype_t to_vtype(value_type_t value_type)
    {
        if((int)value_type >= 0 && (int)value_type <= (int)value_type_t::__end__)
            return (vtype_t)value_type;

        return vtype_t::__unknown__;
    }

    bool is_vtype_compatible(vtype_t from, vtype_t to)
    {
        if(from == to)
            return true;

        return is_numberic_type(from) && is_numberic_type(to);
    }

    static bool __between(vtype_t v, vtype_t v1, vtype_t v2)
    {
        return (int)v >= (int)v1 && (int)v <= (int)v2;
    }

    bool is_integer_vtype(vtype_t vtype)
    {
        return __between(vtype, vtype_t::int8_, vtype_t::uint64_);
    }

    bool is_float_vtype(vtype_t vtype)
    {
        return __between(vtype, vtype_t::float_, vtype_t::double_);
    }

    bool is_numberic_type(vtype_t vtype)
    {
        return __between(vtype, vtype_t::int8_, vtype_t::double_);
    }

    cvalue_t default_value_of(vtype_t vtype)
    {
        switch(vtype)
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

            case vtype_t::string:
            case vtype_t::ptr:
            case vtype_t::mobject:
            case vtype_t::void_:
            default:
                return cvalue_t(nullptr);
        }
    }

    X_ENUM_INFO(ttype_t)

        X_C(class_,         _T("class"))

        X_C(struct_,        _T("struct"))

        X_C(interface_,     _T("interface"))

        X_C(enum_,          _T("enum"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

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

    X_ENUM_INFO(member_type_t)

        X_C(field,      _T("field"))

        X_C(method,     _T("method"))

        X_C(property,   _T("property"))

        X_C(event,      _T("event"))

        X_C(type,       _T("type"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // generic_constraint_type_t

    X_ENUM_INFO(generic_constraint_ttype_t)

        X_C(class_,     _T("class"))

        X_C(struct_,    _T("struct"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    void __collect_super_types(type_t * type, std::set<type_t *> & super_types)
    {
        type->each_super_type([&super_types](type_t * super_type) {
            if(super_types.insert(super_type).second)
                __collect_super_types(super_type, super_types);
        });
    }

    bool type_t::has_super_type(type_t * super)
    {
        _A(super != nullptr);

        if(__super_types.empty())
            __collect_super_types(this, __super_types);

        return __super_types.find(super) != __super_types.end();
    }

    void type_t::commit(eobject_commit_context_t & ctx)
    {

    }

    ////////// ////////// ////////// ////////// //////////

    msize_t get_type_size(type_t * type)
    {
        _A(type != nullptr);

        return 0;        
    }

    ////////// ////////// ////////// ////////// //////////

    #define __FindMemberAndRet(members, name)                   \
        do                                                      \
        {                                                       \
            auto r = members.get(name);                         \
            if(r != nullptr)                                    \
                return r;                                       \
        } while(false)

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(__method_compare_t)
        not_match, match, equals, same_name,
        auto_determined, auto_determined_failed,
    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    X_ENUM_INFO(__method_compare_t)
        X_C(not_match,          _T("not_match"))
        X_C(match,              _T("match"))
        X_C(equals,             _T("equals"))
        X_C(same_name,          _T("same_name"))
        X_C(auto_determined,    _T("auto_determined"))
        X_C(auto_determined_failed, _T("auto_determined_failed"))
    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    bool is_super_type(type_t * drived, type_t * super)
    {
        if(drived == nullptr)
            return false;

        if(super == nullptr)
            return true;

        return drived->has_super_type(super);
    }

    static bool __is_mobject(type_t * type)
    {
        return type == nullptr || type->this_vtype() == vtype_t::mobject;
    }

    bool is_type_compatible(type_t * from, type_t * to)
    {
        if(from == to)
            return true;

        if(__is_mobject(from) || __is_mobject(to))
            return is_super_type(from, to);

        return is_vtype_compatible(from->this_vtype(), to->this_vtype());
    }

    static __method_compare_t __compare_argument(param_t * param, generic_args_t * generic_args,
                        atype_t atype, type_t ** out_gp_type = nullptr)
    {
        if(param == nullptr || atype.type == nullptr)
            return __method_compare_t::not_match;

        type_t * param_type = param->get_type();
        type_t * argument_type = atype.type;

        if(param_type != nullptr && param_type->this_gtype() == gtype_t::generic_param)
        {
            generic_param_t * generic_param = (generic_param_t *)param_type;
            if(generic_args != nullptr && generic_param->index < generic_args->size())
            {
                param_type = (*generic_args)[generic_param->index]->get_type();
            }
            else
            {
                param_type = atype.type;
                al::assign(out_gp_type, param_type);
                return __method_compare_t::auto_determined;
            }
        }

        if(param->ptype != atype.atype)
            return __method_compare_t::not_match;

        if(param_type == argument_type)
            return __method_compare_t::equals;

        if(param->ptype == param_type_t::ref)
            return __method_compare_t::not_match;

        if(param->ptype == param_type_t::out)
        {
            if(is_type_compatible(param_type, argument_type))
                return __method_compare_t::match;
        }
        else
        {
            if(is_type_compatible(argument_type, param_type))
                return __method_compare_t::match;
        }

        return __method_compare_t::not_match;
    }

    static type_t * __get_compatible_type(type_t * type1, type_t * type2)
    {
        if(type1 == type2)
            return type1;

        if(type1 == nullptr || type2 == nullptr)
            return nullptr;

        vtype_t vtype1 = type1->this_vtype(), vtype2 = type2->this_vtype();
        if(is_numberic_type(vtype1) && is_numberic_type(vtype2))
        {
            return (int)vtype1 > (int)vtype2? type1 : type2;
        }

        if(type1->has_super_type(type2))
            return type2;

        if(type2->has_super_type(type1))
            return type1;

        return nullptr;
    }

    static type_t * __get_compatible_type(small_vector_t<type_t *> & types)
    {
        if(types.empty())
            return nullptr;

        type_t * type = types[0];
        for(size_t index = 1, count = types.size(); index < count; index++)
        {
            type = __get_compatible_type(type, types[index]);
            if(type == nullptr)
                break;
        }

        return type;
    }

    static __method_compare_t __compare_method(method_t * method, generic_args_t * generic_args,
                            atypes_t * atypes, arg_types_t & out_arg_types)
    {
        if(atypes == nullptr)
        {
            if(generic_args == nullptr || method->generic_param_count() == generic_args->size())
                return __method_compare_t::same_name;

            return __method_compare_t::not_match;
        }

        size_t atype_count = atypes->size();
        size_t param_count = method->param_count();

        if(atype_count == 0)
            return param_count == 0? __method_compare_t::equals : __method_compare_t::not_match;

        if(atype_count != param_count)
            return __method_compare_t::not_match;

        struct gp_match_t { generic_param_t * gparam; type_t * type; };
        al::small_vector_t<gp_match_t, 8> gp_matches;

        __method_compare_t r = __method_compare_t::equals;
        for(size_t index = 0; index < param_count; index++)
        {
            type_t * gp_type = nullptr;
            param_t * param = method->param_at(index);
            switch(__compare_argument(param, generic_args, (*atypes)[index], &gp_type))
            {
                case __method_compare_t::not_match:
                case __method_compare_t::auto_determined_failed:
                    return __method_compare_t::not_match;

                case __method_compare_t::match:
                    r = __method_compare_t::match;
                    break;

                case __method_compare_t::auto_determined:
                    if(gp_type != nullptr)
                    {
                        gp_matches.push_back(gp_match_t {
                            (generic_param_t *)param->get_type(), gp_type
                        });
                    }
                    break;

                case __method_compare_t::equals:
                default:
                    break;
            }
        }

        if(method->is_generic() && generic_args == nullptr)
        {
            for(generic_param_t * gparam : *method->generic_params)
            {
                al::small_vector_t<type_t *> types;
                for(gp_match_t & m : gp_matches)
                {
                    if(m.gparam == gparam)
                        types.push_back(m.type);
                }

                type_t * type = __get_compatible_type(types);
                if(type == nullptr)
                    return __method_compare_t::auto_determined_failed;

                out_arg_types.push_back(type);
            }
        }

        return r;
    }

    X_ALWAYS_INLINE static bool __is_vtype(type_t * type, vtype_t vtype)
    {
        _A(type != nullptr);

        return type->this_gtype() == gtype_t::general
            && ((general_type_t *)type)->vtype == vtype;
    }

    bool is_void_type(type_t * type)
    {
        return __is_vtype(type, vtype_t::void_);
    }

    bool is_string_type(type_t * type)
    {
        return __is_vtype(type, vtype_t::string);
    }

    bool is_ptr_type(type_t * type)
    {
        return __is_vtype(type, vtype_t::ptr);
    }

    typedef typename __general_type_like_base_t::method_list_t __method_list_t;

    class __method_analyzer_t
    {
    public:
        __method_analyzer_t(__method_list_t & methods, analyze_member_args_t & args)
            : __methods(methods), __args(args) { }

        void analyze()
        {
            using namespace std::placeholders;
            auto analyze_method = std::bind(&__method_analyzer_t::__analyze, this, _1);

            size_t generic_args_count = __generic_args_count();
            if(__args.method_trait == method_trait_t::__default__)
                __methods.each(__args.name, generic_args_count, analyze_method);
            else
                __methods.each(__args.method_trait, analyze_method);

            if(__not_found() && generic_args_count == 0)
            {
                __methods.each(__args.name, __unknown_size, [this](method_t * method) {
                    if(method->generic_param_count() > 0)
                        __analyze(method);
                });
            }
        }

        method_t * get_method(bool exact_match = false)
        {
            if(__equaled_method.method != nullptr)
                return __pick_method(__equaled_method);

            if(exact_match)
                return nullptr;

            if(__matched_methods.size() >= 2)
                throw _ECF(conflict, _T("method \"%1%\" conflict"), __args.name);

            if(!__matched_methods.empty())
                return __pick_method(__matched_methods[0]);
 
            return nullptr;
        }

        void get_methods(members_t & out_members)
        {
            if(__equaled_method.method != nullptr)
                out_members.push_back(__equaled_method.method);

            for(mi_t & mi : __matched_methods)
                out_members.push_back(mi.method);
        }

    private:
        __method_list_t &       __methods;
        analyze_member_args_t & __args;
        static const size_t __empty_size = (size_t)-1;

        struct mi_t { method_t * method; size_t ga_index = __empty_size; };

        mi_t __equaled_method { nullptr };
        typedef al::small_vector_t<mi_t, 5> __methods_vector_t;
        __methods_vector_t __matched_methods;
        //__methods_vector_t __not_matched_methods;

        struct __arg_types_data_t
        {
            byte_t data[sizeof(arg_types_t)];
        };

        al::small_vector_t<__arg_types_data_t, 8> __arg_types_list;

        void __analyze(method_t * method)
        {
            switch(__compare_method(method, __args.generic_args, __args.atypes, *__next_arg_types()))
            {
                case __method_compare_t::equals:
                case __method_compare_t::auto_determined:
                    __equaled_method = mi_t { method, __current_arg_types_index() };
                    break;

                case __method_compare_t::match:
                case __method_compare_t::same_name:
                    __matched_methods.push_back(mi_t { method, __current_arg_types_index() });
                    break;

                case __method_compare_t::not_match:
                case __method_compare_t::auto_determined_failed:
                    //__not_matched_methods.push_back(method);
                    break;

                default:
                    X_UNEXPECTED();
            }
        }

        method_t * __pick_method(mi_t & mi)
        {
            if(mi.ga_index != __empty_size)
            {
                arg_types_t * arg_types = __arg_types_at(mi.ga_index);
                al::copy(*arg_types, std::back_inserter(__args.out_arg_types));
            }

            return mi.method;
        }

        arg_types_t * __arg_types_at(size_t index)
        {
            return (arg_types_t *)&__arg_types_list[index];
        }

        arg_types_t * __last_arg_types()
        {
            return __arg_types_at(__arg_types_list.size() - 1);
        }

        arg_types_t * __next_arg_types()
        {
            if(__arg_types_list.empty() || !__last_arg_types()->empty())
            {
                void * data = &__arg_types_list.push_back(__arg_types_data_t());
                return new (data) arg_types_t();
            }

            return __last_arg_types();
        }

        size_t __current_arg_types_index()
        {
            if(__arg_types_list.empty() || __last_arg_types()->empty())
                return __empty_size;

            return __arg_types_list.size() - 1;
        }

        size_t __generic_args_count() const
        {
            return __args.generic_args? __args.generic_args->size() : 0;
        }

        bool __not_found() const
        {
            return __equaled_method.method == nullptr && __matched_methods.empty();
        }
    };

    method_t * __analyze_method(__method_list_t & methods, analyze_member_args_t & args)
    {
        __method_analyzer_t analyzer(methods, args);
        analyzer.analyze();

        return analyzer.get_method(args.exact_match);
    }

    void __analyze_methods(__method_list_t & methods, analyze_members_args_t & args)
    {
        __method_analyzer_t analyzer(methods, args);
        analyzer.analyze();

        analyzer.get_methods(args.out_members);
    }

    member_t * __general_type_like_base_t::get_member(analyze_member_args_t & args)
    {
        if(enum_has_flag(args.member_type, member_type_t::field))
            __FindMemberAndRet(fields, args.name);

        if(enum_has_flag(args.member_type, member_type_t::property))
            __FindMemberAndRet(properties, args.name);

        if(enum_has_flag(args.member_type, member_type_t::event))
            __FindMemberAndRet(events, args.name);

        if(enum_has_flag(args.member_type, member_type_t::type_def))
            __FindMemberAndRet(type_defs, args.name);

        if(enum_has_flag(args.member_type, member_type_t::type))
            __FindMemberAndRet(nest_types, args.name);

        if(enum_has_flag(args.member_type, member_type_t::method))
        {
            method_t * method = __analyze_method(methods, args);
            if(method != nullptr)
                return method;

            if(args.method_trait == method_trait_t::constructor
                    || args.method_trait == method_trait_t::static_constructor
                    || args.method_trait == method_trait_t::destructor)
                return nullptr;
        }

        type_t * type = get_base_type();
        if(type != nullptr)
            return type->get_member(args);

        return nullptr;
    }

    void __general_type_like_base_t::get_members(analyze_members_args_t & args)
    {
        if(enum_has_flag(args.member_type, member_type_t::field))
        {
            if(args.name == name_t::null)
            {
                al::copy(fields, std::back_inserter(args.out_members));
            }
            else
            {
                field_t * field = fields.get(args.name);
                if(field != nullptr)
                    args.out_members.push_back(field);
            }
        }

        if(enum_has_flag(args.member_type, member_type_t::property))
        {
            if(args.name == name_t::null)
            {
                al::copy(properties, std::back_inserter(args.out_members));
            }
            else
            {
                property_t * property = properties.get(args.name);
                if(property != nullptr)
                    args.out_members.push_back(property);
            }
        }

        if(enum_has_flag(args.member_type, member_type_t::method))
        {
            __analyze_methods(methods, args);
        }
    }

    void __general_type_like_base_t::get_all_members(members_t & out_members)
    {
        al::copy(methods,    std::back_inserter(out_members));
        al::copy(properties, std::back_inserter(out_members));
        al::copy(fields,     std::back_inserter(out_members));
        al::copy(events,     std::back_inserter(out_members));
        al::copy(nest_types, std::back_inserter(out_members));
    }

    msize_t __general_type_like_base_t::get_vsize()
    {
        __ensure_size_initialize();
        return __value_size;
    }

    msize_t __general_type_like_base_t::get_rcount()
    {
        __ensure_size_initialize();
        return __rcount;
    }

    type_t * __general_type_like_base_t::get_base_type()
    {
        if(is_empty(super_type_names))
            return nullptr;

        type_t * base_type = super_type_names[0]->type;
        _A(base_type != nullptr);

        if(base_type->this_ttype() == ttype_t::class_)
        {
            return base_type;
        }

        return nullptr;
    }

    void __general_type_like_base_t::__ensure_size_initialize()
    {
        if(__rcount != unknown_msize)
            return;

        __rcount = 0;
        __value_size = 0;

        field_t * value_fields[fields.size()];
        field_t ** value_fields_end = value_fields;

        for(field_t * field : fields)
        {
            if(field->is_ref_type())
                __rcount++;
            else
                *value_fields_end++ = field;
        }

        if(value_fields_end > value_fields)
        {
            const size_t align_size = 4;

            std::sort(value_fields, value_fields_end, [](field_t * f1, field_t * f2) {
                return f1->get_size() > f2->get_size();
            });

            for(field_t * f : _range(value_fields, value_fields_end))
            {
                msize_t size = f->get_size();

                if(size >= 4)
                {
                    f->offset = __value_size;
                    __value_size += _alignf(size, align_size);
                }
                else
                {
                    if(__value_size % align_size + size > align_size)
                        __value_size = _alignf(__value_size, align_size);

                    f->offset = __value_size;
                    __value_size += size;
                }
            }

            if(__value_size > align_size)
                __value_size = _alignf(__value_size, align_size);
        }
    }

    void __general_type_like_base_t::each_super_type(each_super_type_callback_t callback)
    {
        for(type_name_t * super_type_name : super_type_names)
        {
            if(super_type_name->type != nullptr)
                callback(super_type_name->type);
        }
    }

    void __general_type_like_base_t::commit(eobject_commit_context_t & ctx)
    {
        if(!__committed)
        {
            __ensure_size_initialize();

            __super_t::commit(ctx);
            __committed = true;
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // default_type_impl_t

    class default_type_impl_t : public type_impl_t
    {
    public:
        virtual void commit(eobject_commit_context_t & ctx, eobject_t * bind_object) override { }
    };

    type_impl_t * type_impl_t::default_()
    {
        static default_type_impl_t impl;
        return &impl;
    }

    ////////// ////////// ////////// ////////// //////////

    atype_t::operator string_t() const
    {
        if(atype == param_type_t::__default__)
            return _str(type);

        return _F(_T("%1% %2%"), _title(atype), type);
    }

    ////////// ////////// ////////// ////////// //////////
    // general_type_t

    void general_type_t::append_member(member_t * member)
    {
        _A(member != nullptr);

        switch(member->this_type())
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

    size_t general_type_t::nest_depth() const
    {
        size_t depth = 0;
        const type_t * type = this;

        while((type = type->host_type) != nullptr)
        {
            depth++;
        }

        return depth;
    }

    generic_param_t * general_type_t::find_param(const name_t & name)
    {
        return __find_param(params, name);
    }

    const string_t general_type_t::to_short_name() const
    {
        if(!is_generic())
            return _str(name);

        return _F(_T("%1%<%2%>"), _str(name), _str(params));
    }

    general_type_t::operator string_t() const
    {
        stringstream_t ss;

        if(assembly != nullptr)
            ss << _str(assembly) << _T(".");

        if(namespace_ != nullptr)
            ss << _str(namespace_) << _T(".");

        if(host_type == nullptr)
        {
            ss << to_short_name();
        }
        else
        {
            std::stack<const type_t *> host_types;
            for(const type_t * type = this->host_type; type != nullptr; type = type->host_type)
            {
                host_types.push(type);
            }

            for(; !host_types.empty(); host_types.pop())
            {
                const type_t * type = host_types.top();
                ss << _str(type->to_short_name()) << _T(".");
            }

            ss << to_short_name();
        }

        return ss.str();
    }

    void general_type_t::commit(eobject_commit_context_t & ctx)
    {
        if(!__ensure_size_initialize())
            __super_t::commit(ctx);
    }

    msize_t general_type_t::get_vsize()
    {
        __ensure_size_initialize();

        return __super_t::get_vsize();
    }

    msize_t general_type_t::get_rcount()
    {
        __ensure_size_initialize();

        return __super_t::get_rcount();
    }

    bool general_type_t::__ensure_size_initialize()
    {
        if(__rcount == unknown_msize)
        {
            if(vtype != vtype_t::mobject)
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

    class generic_type_t::__transform_context_t
    {
    public:
        __transform_context_t(xpool_t & xpool) : xpool(xpool) { }

        xpool_t & xpool;

        void add_relation(void * a, void * b)
        {
            __relation_map[a] = b;
        }

        template<typename t> t * get_relation(t * a)
        {
            auto it = __relation_map.find(a);
            if(it == __relation_map.end())
                return nullptr;

            return (t *)it->second;
        }

    private:
        std::map<void *, void *> __relation_map;
    };

    void generic_type_t::commit(eobject_commit_context_t & ctx)
    {
        if(this->committed())
            return;

        _A(template_ != nullptr);
        _A(template_->committed());

        __transform_context_t tctx(ctx.xpool);

        __transform_members(tctx, template_->fields, fields);
        __transform_members(tctx, template_->methods, methods);
        __transform_members(tctx, template_->properties, properties);
        __transform_members(tctx, template_->events, events);
        __transform_members(tctx, template_->nest_types, nest_types);
        __transform_members(tctx, template_->type_defs, type_defs);
        __transform_members(tctx, template_->super_type_names, super_type_names);

        __super_t::commit(ctx);
    }

    template<typename _members_t>
    void generic_type_t::__transform_members(__transform_context_t & ctx,
                                    _members_t & members, _members_t & out_members)
    {
        typedef typename _members_t::value_type _member_t;
        for(_member_t member : members)
        {
            _member_t new_member = __transform_member(ctx, member);
            out_members.push_back(new_member);
        }
    }

    type_t * generic_type_t::__transform_generic_type(xpool_t & xpool, generic_type_t * type)
    {
        type_collection_t type_args;
        for(type_t * arg_type : type->args)
        {
            type_t * new_arg_type = __transform_type(xpool, arg_type);
            type_args.push_back(new_arg_type);
        }

        generic_type_t * new_type = xpool.new_generic_type(type->template_, type_args,
                                                           type->host_type);
        return new_type;
    }

    type_t * generic_type_t::__transform_type(xpool_t & xpool, type_t * type)
    {
        switch(type->this_gtype())
        {
            case gtype_t::generic_param:
                return type_at(((generic_param_t *)type)->name);

            case gtype_t::array: {
                array_type_t * array_type = (array_type_t *)type;
                type_t * new_element_type = __transform_type(xpool, array_type->element_type);
                return xpool.new_array_type(new_element_type, array_type->dimension);
            }   break;

            case gtype_t::generic:
                return __transform_generic_type(xpool, (generic_type_t *)type);

            default:
                return type;
        }
    }

    type_name_t * generic_type_t::__transform_type_name(xpool_t & xpool, type_name_t * type_name)
    {
        type_t * type;
        if(type_name == nullptr || (type = type_name->type) == nullptr)
            return type_name;

        type_t * new_type = __transform_type(xpool, type);
        if(type == new_type || new_type == nullptr)
            return type_name;

        return xpool.new_obj<type_name_t>(new_type);
    }

    field_t * generic_type_t::__transform_member(__transform_context_t & ctx, field_t * field)
    {
        field_t * new_field = ctx.xpool.new_obj<field_t>();
        new_field->type_name = __transform_type_name(ctx.xpool, field->type_name);
        new_field->attributes = field->attributes;
        new_field->init_value = field->init_value;

        return new_field;
    }

    method_t * generic_type_t::__transform_member(__transform_context_t & ctx, method_t * method)
    {
        method_t * new_method = ctx.xpool.new_obj<method_t>();
        new_method->owner_type_name = __transform_type_name(ctx.xpool, method->owner_type_name);
        new_method->type_name = __transform_type_name(ctx.xpool, method->type_name);
        new_method->generic_params = method->generic_params;
        new_method->name = method->name;

        if(method->params != nullptr)
        {
            new_method->params = ctx.xpool.new_obj<params_t>();

            for(param_t * param : *method->params)
            {
                param_t * new_param = ctx.xpool.new_obj<param_t>();
                new_param->name = param->name;
                new_param->attributes = param->attributes;
                new_param->type_name = __transform_type_name(ctx.xpool, param->type_name);
                new_param->ptype = param->ptype;
                new_param->default_value = param->default_value;

                new_method->params->push_back(new_param);
            }
        }

        ctx.add_relation(method, new_method);
        return new_method;
    }

    property_t * generic_type_t::__transform_member(__transform_context_t & ctx,
                                                    property_t * property)
    {
        property_t * new_property = ctx.xpool.new_obj<property_t>();
        new_property->owner_type_name = __transform_type_name(ctx.xpool, property->owner_type_name);
        new_property->name = property->name;
        new_property->type_name = __transform_type_name(ctx.xpool, property->type_name);
        new_property->attributes = property->attributes;

        new_property->get_method = ctx.get_relation<method_t>(property->get_method);
        new_property->set_method = ctx.get_relation<method_t>(property->set_method);

        return new_property;
    }

    event_t * generic_type_t::__transform_member(__transform_context_t & ctx, event_t * event)
    {
        event_t * new_event = ctx.xpool.new_obj<event_t>();
        new_event->owner_type_name = __transform_type_name(ctx.xpool, event->owner_type_name);
        new_event->type_name = __transform_type_name(ctx.xpool, event->type_name);
        new_event->attributes = event->attributes;

        // TODO
        new_event->add_method = event->add_method;
        new_event->remove_method = event->remove_method;

        return new_event;
    }

    type_t * generic_type_t::__transform_member(__transform_context_t & ctx, type_t * nest_type)
    {
        general_type_t * template_ = _M(general_type_t *, nest_type);

        type_collection_t type_args;
        if(template_->params != nullptr)
        {
            for(generic_param_t * param : *template_->params)
                type_args.push_back(param);
        }

        return ctx.xpool.new_generic_type(template_, type_args, this);
    }

    type_def_t * generic_type_t::__transform_member(__transform_context_t & ctx,
                                                    type_def_t * type_def)
    {
        type_def_t * new_type_def = ctx.xpool.new_obj<type_def_t>();

        new_type_def->type_name = __transform_type_name(ctx.xpool, type_def->type_name);
        new_type_def->name = type_def->name;
        new_type_def->params = type_def->params;

        return new_type_def;
    }

    type_name_t * generic_type_t::__transform_member(__transform_context_t & ctx,
                                                     type_name_t * type_name)
    {
        return __transform_type_name(ctx.xpool, type_name);
    }

    const string_t generic_type_t::to_short_name() const
    {
        stringstream_t ss;
        ss << (template_? template_->name : string_t(_T("?")));

        if(args.size() > 0)
        {
            ss << _T("<");

            for(int index = 0, count = args.size(); index < count; index++)
            {
                if(index > 0)
                    ss << _T(", ");

                type_t * type = args[index];
                if(type != nullptr)
                {
                    ss << _str(type);
                }
                else if(template_ && template_->params && index < template_->params->size())
                {
                    ss << _str((*template_->params)[index]);
                }
            }

            ss << _T(">");
        }

        return ss.str();
    }

    bool generic_type_t::any_type_def_param() const
    {
        return al::any_of(args, [](type_t * t) {
            return t && t->this_gtype() == gtype_t::type_def_param;
        });
    }

    type_t * generic_type_t::type_at(size_t index) const
    {
        if(index >= argument_count())
            return nullptr;

        return args[index];
    }

    type_t * generic_type_t::type_at(name_t name) const
    {
        if(template_ != nullptr && template_->params != nullptr)
        {
            generic_params_t & params = *template_->params;
            for(size_t index = 0, count = params.size(); index < count; index++)
            {
                if(params[index]->name == name)
                    return type_at(index);
            }
        }

        if(host_type != nullptr && host_type->this_gtype() == gtype_t::generic)
            return ((generic_type_t *)host_type)->type_at(name);

        return nullptr;
    }

    generic_type_t::operator string_t() const
    {
        stringstream_t ss;

        if(template_ != nullptr)
        {
            if(template_->assembly != nullptr)
                ss << _str(template_->assembly) << _T(".");

            if(template_->namespace_ != nullptr)
                ss << _str(template_->namespace_) << _T(".");
        }

        if(host_type == nullptr)
        {
            ss << to_short_name();
        }
        else
        {
            std::stack<const type_t *> host_types;
            for(const type_t * type = this->host_type; type != nullptr; type = type->host_type)
            {
                host_types.push(type);
            }

            for(; !host_types.empty(); host_types.pop())
            {
                const type_t * type = host_types.top();
                ss << _str(type->to_short_name()) << _T(".");
            }

            ss << to_short_name();
        }

        return ss.str();
    }

    ////////// ////////// ////////// ////////// //////////
    // null_type_t

    member_t * null_type_t::get_member(analyze_member_args_t & args)
    {
        return nullptr;
    }

    void null_type_t::get_members(analyze_members_args_t & args)
    {
        
    }

    void null_type_t::get_all_members(members_t & out_members)
    {

    }

    null_type_t * null_type_t::instance()
    {
        static null_type_t __instance;
        return &__instance;
    }

    ////////// ////////// ////////// ////////// //////////
    // generic_param_t

    member_t * generic_param_t::get_member(analyze_member_args_t & args)
    {
        return nullptr;
    }

    void generic_param_t::get_members(analyze_members_args_t & args)
    {
        
    }

    void generic_param_t::get_all_members(members_t & out_members)
    {

    }

    ////////// ////////// ////////// ////////// //////////
    // array_type_t

    member_t * array_type_t::get_member(analyze_member_args_t & args)
    {
        return nullptr;
    }

    void array_type_t::get_members(analyze_members_args_t & args)
    {
        
    }

    void array_type_t::get_all_members(members_t & out_members)
    {

    }

    void array_type_t::commit(eobject_commit_context_t & ctx)
    {

    }

    msize_t array_type_t::get_vsize()
    {
        return unknown_msize;
    }

    msize_t array_type_t::get_rcount()
    {
        return unknown_msize;
    }

    array_type_t::operator string_t() const
    {
        stringstream_t ss;
        ss << _str(element_type) << _T("[");

        for(int index = 1; index < dimension; index++)
        {
            ss << _T(",");
        }

        ss << _T("]");
        return ss.str();
    }

    ////////// ////////// ////////// ////////// //////////
    // namespace_t

    void namespace_t::append_type(general_type_t * type)
    {
        _A(type != nullptr);

        types.push_back(type);
        type->namespace_ = this;
    }

    namespace_t::operator string_t() const
    {
        return full_name? _str(full_name) : name? _str(name) : empty_str;
    }

    ////////// ////////// ////////// ////////// //////////
    // import_t

    import_t::operator string_t() const
    {
        if(alias.empty())
            return _F(_T("import %1%"), _str(package));

        return _F(_T("import %1% = %2%"), _str(alias), _str(package));
    }

    ////////// ////////// ////////// ////////// //////////
    // using_namespace_t

    using_namespace_t::operator string_t() const
    {
        if(alias.empty())
            return _F(_T("using %1%"), _str(ns));

        return _F(_T("using %1% = %2%"), _str(alias), _str(ns));
    }

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(ast_error_t)

        X_C(variable_duplicated, _T("variable_duplicated"))
        X_C(variable_defination_unexpected, _T("variable_defination_unexpected"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // variable

    X_ENUM_INFO(variable_type_t)

        X_C(local,      _T("local"))
        X_C(param,      _T("param"))
        X_C(field,      _T("field"))
        X_C(property,   _T("property"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    vtype_t __get_vtype(type_t * type)
    {
        if(type == nullptr)
            return vtype_t::__unknown__;

        switch(type->this_gtype())
        {
            case gtype_t::general:
                return ((general_type_t *)type)->vtype;
                
            case gtype_t::generic:
            case gtype_t::null:
            case gtype_t::array:
                return vtype_t::mobject;

            default:
                return vtype_t::__unknown__;
        }
    }

    vtype_t __get_vtype(type_name_t * type_name)
    {
        if(type_name == nullptr)
            return vtype_t::__unknown__;

        return __get_vtype(type_name->type);
    }

    vtype_t variable_t::get_vtype()
    {
        return __get_vtype(get_type_name());
    }

    type_t * variable_t::get_type()
    {
        type_name_t * type_name = get_type_name();
        if(type_name == nullptr)
            return nullptr;

        return type_name->type;
    }

    X_DEFINE_TO_STRING(local_variable_t)
    {
        return name;
        //return _F(_T("%1% %2%"), type_name, name);
    }

    X_DEFINE_TO_STRING(param_variable_t)
    {
        return param->name;
        //return _F(_T("%1% %2%"), param->type_name, param->name);
    }

    X_DEFINE_TO_STRING(field_variable_t)
    {
        return field->name;
        //return _F(_T("%1% %2%"), field->type_name, field->name);
    }

    X_DEFINE_TO_STRING(property_variable_t)
    {
        return property->name;
        //return _F(_T("%1% %2%"), property->type_name, property->name);
    }

    X_DEFINE_TO_STRING(event_variable_t)
    {
        return event->name;
        //return _F(_T("%1% %2%"), event->type_name, event->name);
    }

    ////////// ////////// ////////// ////////// //////////
    // xpool_t

    type_collection_id_t xtype_collection_t::get_tcid(xpool_t & pool)
    {
        if(tcid.empty())
            tcid = pool.type_collection.get_tcid(types);

        return tcid;
    }

    void xpool_t::initialize()
    {

    }

    generic_type_t * xpool_t::new_generic_type(general_type_t * template_,
                                       xtype_collection_t & types, type_t * host_type)
    {
        _A(template_ != nullptr);
        _A(template_->params_count() == types.size());

        generic_type_cache_key_t key(template_, types.get_tcid(*this), host_type);
        generic_type_t * type = generic_type_cache->get(key);
        if(type == nullptr)
        {
            type = memory_t::new_obj<generic_type_t>(memory, template_,
                                    (const type_collection_t &)types, host_type);
            generic_type_cache->set(key, type);

            append_new_type(type);
        }

        return type;
    }

    generic_type_t * xpool_t::new_generic_type(general_type_t * template_,
                                       type_collection_t & types, type_t * host_type)
    {
        xtype_collection_t xtypes(types);
        return new_generic_type(template_, xtypes, host_type);
    }

    array_type_t * xpool_t::new_array_type(type_t * element_type, dimension_t dimension)
    {
        if(element_type == nullptr)
            element_type = null_type_t::instance();

        array_type_cache_key_t key(element_type, dimension);
        array_type_t * type = array_type_cache->get(key);
        if(type == nullptr)
        {
            type = memory_t::new_obj<array_type_t>(memory, element_type, dimension);
            array_type_cache->set(key, type);

            append_new_type(type);
        }

        return type;
    }

    assembly_reference_t * xpool_t::new_reference(const mname_t * package_name,
                                                  assembly_t * assembly)
    {
        _A(assembly != nullptr);

        assembly_reference_cache_key_t key(package_name, assembly->name);
        assembly_reference_t * ref = assembly_reference_cache->get(key);
        if(ref == nullptr)
        {
            ref = memory_t::new_obj<assembly_reference_t>(memory, package_name, assembly);
            assembly_reference_cache->set(key, ref);
        }

        return ref;
    }

    attribute_t * xpool_t::get_compile_time_attribute()
    {
        if(__compile_time_attribute == nullptr)
        {
            attribute_t * __new_compile_time_attribute(xpool_t & xpool);
            __compile_time_attribute = __new_compile_time_attribute(*this);
        }

        return __compile_time_attribute;
    }

    general_type_t * xpool_t::get_internal_type(vtype_t vtype)
    {
        general_type_t * __get_internal_type(attribute_t * attribute, vtype_t vtype);
        return __get_internal_type(get_compile_time_attribute(), vtype);
    }

    general_type_t * xpool_t::get_internal_type(value_type_t value_type)
    {
        return get_internal_type(to_vtype(value_type));
    }

    general_type_t * xpool_t::get_internal_type(name_t name)
    {
        general_type_t * __get_internal_type(attribute_t * attribute, name_t name);
        return __get_internal_type(get_compile_time_attribute(), name);
    }

    void xpool_t::append_new_type(type_t * type)
    {
        __new_types.push(type);
    }

    void xpool_t::commit_types(logger_t & logger)
    {
        eobject_commit_context_t ctx(*this, logger);
        while(!__new_types.empty())
        {
            type_t * type = __new_types.front();
            __new_types.pop();

            type->commit(ctx);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // variable_region_t

    local_variable_t * variable_region_t::define_local(type_name_t * type_name, const name_t & name)
    {
        _A(type_name != nullptr);
        _A(!name.empty());

        local_variable_t * variable = __define<local_variable_t>(type_name, name);
        variable->identity = __next_local_identity();
        variable->index    = __next_local_index();

        ((method_t *)__owner)->append_local(variable);

        return variable;
    }

    param_variable_t * variable_region_t::define_param(param_t * param)
    {
        _A(param != nullptr);

        return __define<param_variable_t>(param);
    }

    field_variable_t * variable_region_t::define_field(field_t * field)
    {
        _A(field != nullptr);

        field_variable_t * variable = __define<field_variable_t>(field);
        field->variable = variable;

        return variable;
    }

    property_variable_t * variable_region_t::define_property(property_t * property)
    {
        _A(property != nullptr);

        property_variable_t * variable = __define<property_variable_t>(property);
        property->variable = variable;

        return variable;
    }

    event_variable_t * variable_region_t::define_event(event_t * event)
    {
        _A(event != nullptr);

        event_variable_t * variable = __define<event_variable_t>(event);
        event->variable = variable;

        return variable;
    }

    void variable_region_t::__check_duplicate(const name_t & name)
    {
        auto it = __variables.find(name);
        if(it != __variables.end())
            throw _EF(ast_error_t::variable_duplicated, _T("variable \%1%\" duplicate"), name);
    }

    msize_t variable_region_t::__next_local_identity()
    {
        return __current_local_identity()++;
    }

    msize_t & variable_region_t::__current_local_identity()
    {
        variable_region_t * root = this;
        while(root->previous != nullptr && root->previous->__is_local_supported())
            root = root->previous;

        return root->__local_identity;
    }

    msize_t & variable_region_t::__current_local_index()
    {
        if(__local_index == __empty_index)
        {
            if(previous != nullptr && previous->__is_local_supported())
                __local_index = previous->__current_local_index();
            else
                __local_index = 0;
        }

        return __local_index;
    }

    msize_t variable_region_t::variable_region_t::__next_local_index()
    {
        return __current_local_index()++;
    }

    variable_t * variable_region_t::get(const name_t & name)
    {
        _A(!name.empty());

        auto it = __variables.find(name);
        if(it != __variables.end())
            return it->second;

        return previous? previous->get(name) : nullptr;
    }

    template<typename _variable_t, typename ... args_t>
    _variable_t * variable_region_t::__new_variable(args_t && ... args)
    {
        _variable_t * variable = memory_t::new_obj<_variable_t>(
            __memory, std::forward<args_t>(args) ...
        );

        return variable;
    }

    template<typename _variable_t, typename ... args_t>
    _variable_t * variable_region_t::__define(args_t && ... args)
    {
        _variable_t * variable = __new_variable<_variable_t>(std::forward<args_t>(args) ...);
        name_t name = variable->get_name();

        if(!__is_supported(_variable_t::type))
        {
            throw _EF(ast_error_t::variable_defination_unexpected,
                _T("%1% variable \"%2%\" cannot defined here"), _desc(_variable_t::type), name
            );
        }

        __check_duplicate(name);
        __variables[name] = variable;

        return variable;
    }

    ////////// ////////// ////////// ////////// //////////
    // module_t

    void module_t::register_commit(eobject_t * eobj)
    {
        _A(eobj != nullptr);

        __need_commit_eobjects.push_back(eobj);
    }

    void module_t::commit(eobject_commit_context_t & ctx)
    {
        for(eobject_t * eobj : __need_commit_eobjects)
        {
            eobj->commit(ctx);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // ast_walk_context_layer_t

    X_ENUM_INFO(ast_walk_context_layer_state_t)

        X_C(module,         _T("module"))
        X_C(document,       _T("document"))
        X_C(namespace_,     _T("namespace"))
        X_C(type,           _T("type"))
        X_C(method,         _T("method"))
        X_C(property,       _T("property"))
        X_C(event,          _T("event"))
        X_C(field,          _T("field"))
        X_C(type_def,       _T("type_def"))
        X_C(region,         _T("region"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    class __ast_walk_context_layer_base_t : public object_t
                                          , public ast_walk_context_layer_t
    {
        typedef ast_walk_context_layer_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void * get(__layer_state_t state) override
        {
            return previous->get(state);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    class __empty_ast_walk_context_layer_t : public object_t
                                           , public ast_walk_context_layer_t
    {
        typedef ast_walk_context_layer_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void * get(__layer_state_t state) override
        {
            return nullptr;
        }
    };

    //-------- ---------- ---------- ---------- ----------

    class __namespace_ast_walk_context_layer_t : public __ast_walk_context_layer_base_t
    {
        typedef __ast_walk_context_layer_base_t __super_t;

    public:
        __namespace_ast_walk_context_layer_t(__context_t & context, __layer_t * previous,
                                                                        namespace_t * ns)
            : __super_t(context, previous, ns), __ns(ns)
        {
            _A(ns != nullptr);

            namespace_t * previous_ns = (namespace_t *)previous->get(__layer_state_t::namespace_);
            const mname_t * previous_name = previous_ns? previous_ns->full_name : nullptr;
            mname_operate_context_t mctx = to_mname_operate_context(context.xpool);

            if(previous_name == nullptr)
                __ns->full_name = to_mname(ns->name);
            else
                __ns->full_name = mname_t::combine(mctx, previous_name, to_mname(ns->name));
        }

        virtual void * get(__layer_state_t state) override
        {
            switch(state)
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

    class __document_ast_walk_context_layer_t : public __ast_walk_context_layer_base_t
    {
        typedef __ast_walk_context_layer_base_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void * get(__layer_state_t state) override
        {
            switch(state)
            {
                case __layer_state_t::document:
                    return &__document;

                default:
                    return __super_t::get(state);
            }
        }

        document_t * get_document() { return &__document; }

    private:
        document_t __document;
    };

    //-------- ---------- ---------- ---------- ----------

    class __module_ast_walk_context_layer_t : public __ast_walk_context_layer_base_t
    {
        typedef __ast_walk_context_layer_base_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void * get(__layer_state_t state) override
        {
            switch(state)
            {
                case __layer_state_t::module:
                    return &__module;

                default:
                    return __super_t::get(state);
            }
        }

        module_t * get_module() { return &__module; }

    private:
        module_t __module;
    };

    //-------- ---------- ---------- ---------- ----------

    template<variable_type_t _variable_type_mask = (variable_type_t)0xFF>
    class __region_ast_walk_context_layer_t : public __ast_walk_context_layer_base_t
    {
        typedef __ast_walk_context_layer_base_t __super_t;

    public:
        __region_ast_walk_context_layer_t(__context_t & context, __layer_t * previous_layer,
                                      object_t * owner, variable_region_t * previous_region)
            : __super_t(context, previous_layer, owner)
            , __variable_region(context.xpool.memory, previous_region, _variable_type_mask, owner)
        { }

        virtual void * get(__layer_state_t state) override
        {
            if(state == __layer_state_t::region)
                return &__variable_region;

            return __super_t::get(state);
        }

    private:
        variable_region_t __variable_region;
    };

    //-------- ---------- ---------- ---------- ----------

    class __no_region_ast_walk_context_layer_t : public __ast_walk_context_layer_base_t
    {
        typedef __ast_walk_context_layer_base_t __super_t;

    public:
        __no_region_ast_walk_context_layer_t(__context_t & context, __layer_t * previous,
                                                                    object_t * owner)
            : __super_t(context, previous, owner)
        { }

        virtual void * get(__layer_state_t state) override
        {
            if(state == __layer_state_t::region)
                return nullptr;

            return __super_t::get(state);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    typedef ast_walk_context_layer_state_t __layer_state_t;

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
        template<typename ... args_t>
        __x_ast_walk_context_layer_t(__context_t & context, __layer_t * previous,
                                        __obj_t * obj, args_t && ... args)
            : __super_t(context, previous, obj, std::forward<args_t>(args) ...), __obj(obj)
        { }

        virtual void * get(__layer_state_t state) override
        {
            switch(state)
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

    typedef __x_ast_walk_context_layer_t<
        type_t, __layer_state_t::type,
        __region_layer_t<enum_or(variable_type_t::field, variable_type_t::property)>
    > __type_ast_walk_context_layer_t;

    typedef __x_ast_walk_context_layer_t<
        method_t, __layer_state_t::method,
        __region_layer_t<enum_or(variable_type_t::param, variable_type_t::local)>
    > __method_ast_walk_context_layer_t;

    typedef __x_ast_walk_context_layer_t<
        property_t, __layer_state_t::property   //, __no_region_ast_walk_context_layer_t
    > __property_ast_walk_context_layer_t;

    typedef __x_ast_walk_context_layer_t<
        event_t, __layer_state_t::event         //, __no_region_ast_walk_context_layer_t
    > __event_ast_walk_context_layer_t;

    typedef __x_ast_walk_context_layer_t<
        field_t, __layer_state_t::field         //, __no_region_ast_walk_context_layer_t
    > __field_ast_walk_context_layer_t;

    typedef __x_ast_walk_context_layer_t<
        type_def_t, __layer_state_t::type_def   //, __no_region_ast_walk_context_layer_t
    > __type_def_ast_walk_context_layer_t;

    ////////// ////////// ////////// ////////// //////////
    // ast_walk_context_t

    ast_walk_context_t::ast_walk_context_t(assembly_t & assembly, xpool_t & xpool,
            assembly_loader_t * assembly_loader, logger_t & logger, __layer_t * layer)
        : assembly(assembly), xpool(xpool), __assembly_loader(assembly_loader), logger(logger)
    {
        _A(assembly_loader != nullptr);

        if(layer == nullptr)
            __push_new_layer<__empty_ast_walk_context_layer_t>();
        else
            __current_layer = layer;
    }

    void ast_walk_context_t::push(namespace_t * namespace_)
    {
        _A(namespace_ != nullptr);

        __push_new_layer<__namespace_ast_walk_context_layer_t>(namespace_);
    }

    void ast_walk_context_t::push(type_t * type)
    {
        _A(type != nullptr);

        __push_new_layer<__type_ast_walk_context_layer_t>(type, current_region());
    }

    void ast_walk_context_t::push(method_t * method)
    {
        _A(method != nullptr);

        __push_new_layer<__method_ast_walk_context_layer_t>(method, current_region());
    }

    void ast_walk_context_t::push(property_t * property)
    {
        _A(property != nullptr);

        __push_new_layer<__property_ast_walk_context_layer_t>(property);
    }

    void ast_walk_context_t::push(event_t * event)
    {
        _A(event != nullptr);

        __push_new_layer<__event_ast_walk_context_layer_t>(event);
    }

    void ast_walk_context_t::push(field_t * field)
    {
        _A(field != nullptr);

        __push_new_layer<__field_ast_walk_context_layer_t>(field);
    }

    void ast_walk_context_t::push(type_def_t * type_def)
    {
        _A(type_def != nullptr);

        __push_new_layer<__type_def_ast_walk_context_layer_t>(type_def);
    }

    void ast_walk_context_t::push_new_region()
    {
        typedef __region_ast_walk_context_layer_t<variable_type_t::local> layer_t;
        __push_new_layer<layer_t>(nullptr, current_region());
    }

    document_t * ast_walk_context_t::push_new_document()
    {
        return __push_new_layer<__document_ast_walk_context_layer_t>()->get_document();
    }

    module_t * ast_walk_context_t::push_new_module()
    {
        return __push_new_layer<__module_ast_walk_context_layer_t>()->get_module();
    }

    void ast_walk_context_t::pop()
    {
        __current_layer = __current_layer->previous;

        _A(__current_layer != nullptr);
    }

    const mname_t * ast_walk_context_t::parent_of(const mname_t * mname)
    {
        if(mname == nullptr || mname->parts.size() <= 1)
            return nullptr;

        return al::map_get(__parent_mnames, mname, [this, mname]() {
            return mname_t::get_parent(to_mname_operate_context(xpool), mname);
        });
    }

    const mname_t * ast_walk_context_t::child_of(const mname_t * mname)
    {
        if(mname == nullptr || mname->parts.size() <= 1)
            return nullptr;

        return al::map_get(__child_mnames, mname, [this, mname]() {
            return mname_t::get_child(to_mname_operate_context(xpool), mname);
        });
    }

    const mname_t * ast_walk_context_t::combine(const mname_t * name1, const mname_t * name2)
    {
        if(name1 == nullptr || name2 == nullptr)
            return name1? name1 : name2;

        return al::map_get(__combine_names, __combine_name_key_t(name1, name2),
            [this, name1, name2]() {
                return mname_t::combine(to_mname_operate_context(xpool), name1, name2);
            }
        );
    }

    const mname_t * ast_walk_context_t::combine(const mname_t * name1, const name_t & name2)
    {
        if(name2.empty())
            return name1;

        return al::map_get(__combine_names2, __combine_name_key2_t(name1, name2),
            [this, name1, name2]() {
                return mname_t::combine(to_mname_operate_context(xpool), name1, name2);
            }
        );
    }

    void ast_walk_context_t::__walk()
    {
        __layer_t * previous_layer = __current_layer;

        __walk_item_t walk_item;
        while(__walk_items.deque(&walk_item))
        {
            __current_layer = walk_item.layer;
            walk_item.node->on_walk(*this, walk_item.step, walk_item.tag);
        }

        __current_layer = previous_layer;
    }

    type_name_t * ast_walk_context_t::to_type_name(vtype_t vtype)
    {
        auto it = __vtype_type_name_map.find(vtype);
        if(it == __vtype_type_name_map.end())
        {
            type_name_t * type_name = xpool.new_obj<type_name_t>(
                xpool.get_internal_type(vtype)
            );

            __vtype_type_name_map[vtype] = type_name;
            return type_name;
        }

        return it->second;
    }

    assembly_t * ast_walk_context_t::load_assembly(const mname_t * package_name,
                                                   const mname_t * assembly_name)
    {
        return al::map_get(__assembly_cache, __assembly_cache_key_t(package_name, assembly_name),
            [this, package_name, assembly_name]() {
                return __assembly_loader->load_assembly(package_name, assembly_name);
            }
        );
    }

    void __each_eobjects(ast_node_t * node, std::function<void (eobject_t *)> f)
    {
        eobject_ast_base_t * ast = as<eobject_ast_base_t *>(node);
        if(ast != nullptr)
        {
            ast->each_eobjects(f);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // ast_node_t

    void ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        for(size_t index = 0, count = child_count(); index < count; index++)
        {
            ast_node_t * child = child_at(index);
            if(child != nullptr)
            {
                child->on_walk(context, 0, nullptr);
            }
        }
    }

    ////////// ////////// ////////// ////////// //////////

    size_t multipy_ast_node_t::child_count() const
    {
        return __childs.size();
    }
     
    ast_node_t * multipy_ast_node_t::child_at(size_t index) const
    {
        return __childs[index];
    }

    void multipy_ast_node_t::set_child(size_t index, ast_node_t * node)
    {
        while(index >= __childs.size())
            __childs.push_back(nullptr);

        __childs[index] = node;
    }

    void multipy_ast_node_t::append(ast_node_t * node)
    {
        __childs.push_back(node);
    }

    ////////// ////////// ////////// ////////// //////////

    void expand_ast_node(ast_node_t * node, std::function<void (ast_node_t *)> callback)
    {
        if(node == nullptr)
            return;

        multipy_ast_node_t * multipy_node = as<multipy_ast_node_t *>(node);
        if(multipy_node != nullptr)
        {
            for(size_t index = 0, count = multipy_node->child_count(); index < count; index++)
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

    void document_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        context.push_new_document();
        __super_t::on_walk(context, step, tag);
        context.pop();
    }

    void document_ast_node_t::on_commit()
    {
        __super_t::on_commit();
    }

    ////////// ////////// ////////// ////////// //////////
    // module_ast_node_t

    void module_ast_node_t::on_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __module = context.push_new_module();
        do_walk(context, step, tag);
        context.pop();
    }

    void module_ast_node_t::do_walk(ast_walk_context_t & context, int step, void * tag)
    {
        __super_t::on_walk(context, step, tag);
    }

    void module_ast_node_t::on_commit()
    {
        __super_t::on_commit();
    }

    ////////// ////////// ////////// ////////// //////////
    // ast_sulution_t

    void ast_solution_t::add_project(ast_project_t * cproject)
    {
        _A(cproject != nullptr);

        __project_map[to_sid(cproject->name)] = cproject;
        __projects.push_back(cproject);

        cproject->solution = this;
    }

    ast_project_t * ast_solution_t::get_project(const mname_t * name)
    {
        auto it = __project_map.find(to_sid(name));
        if(it != __project_map.end())
            return it->second;

        return nullptr;
    }

    ////////// ////////// ////////// ////////// //////////
    // ast_project_t

    void ast_project_t::add_file(ast_file_t * cfile)
    {
        _A(cfile != nullptr);

        __file_map[to_sid(cfile->name)] =  cfile;
        __files.push_back(cfile);

        cfile->project = this;
    }

    ast_file_t * ast_project_t::get_file(const mname_t * name)
    {
        auto it = __file_map.find(to_sid(name));
        if(it != __file_map.end())
            return it->second;

        return nullptr;
    }

    ////////// ////////// ////////// ////////// //////////
    // expression_execute_error_code_t

    X_ENUM_INFO(expression_execute_error_code_t)

        X_D(expect_const_expression,    _T("'%1%' is not a constant expression"))

        X_D(invalid_binary_operator,    _T("operator '%1%' cannot use for '%2%' and '%3%'"))

        X_D(invalid_unitary_operator,   _T("operator '%1%' cannot use for '%2%'"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // expression

    X_ENUM_INFO(expression_family_t)

        X_C(name,           _T("name"))
        X_C(cvalue,         _T("cvalue"))
        X_C(unitary,        _T("unitary"))
        X_C(binary,         _T("binary"))
        X_C(expressions,    _T("expressions"))
        X_C(function,       _T("function"))
        X_C(index,          _T("index"))
        X_C(new_,           _T("new"))
        X_C(default_value,  _T("default_value"))
        X_C(type_of,        _T("type_of"))
        X_C(this_,          _T("this"))
        X_C(base,           _T("base"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // expression_behaviour_t

    X_ENUM_INFO(expression_behaviour_t)

        X_C(default_,       _T("default"))
        X_C(assign,         _T("assign"))
        X_C(execute,        _T("execute"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // name_expression_t

    X_ENUM_INFO(name_expression_type_t)

        X_C(variable,   _T("variable"))
        X_C(type,       _T("type"))

    X_ENUM_INFO_END

    vtype_t name_expression_t::get_vtype() const
    {
        if(variable == nullptr)
            return vtype_t::__unknown__;

        switch(expression_type)
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

    type_t * name_expression_t::get_type(xpool_t & xpool) const
    {
        if(variable == nullptr)
            return nullptr;

        switch(expression_type)
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

    void name_expression_t::set(variable_t * variable)
    {
        this->expression_type = name_expression_type_t::variable;
        this->variable = variable;
    }

    void name_expression_t::set(type_t * type)
    {
        this->expression_type = name_expression_type_t::type;
        this->type = type;
    }

    void name_expression_t::set(type_def_t * type_def)
    {
        this->expression_type = name_expression_type_t::type_def;
        this->type_def = type_def;
    }

    const string_t name_expression_t::to_string() const
    {
        switch(expression_type)
        {
            case name_expression_type_t::variable:
                return variable? _str(variable) : _str(name);

            case name_expression_type_t::type:
                return type? _str(type) : _str(name);

            default:
                return _str(name);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // type_name_expression_t

    vtype_t type_name_expression_t::get_vtype() const
    {
        return __get_vtype(type_name);
    }

    type_t * type_name_expression_t::get_type(xpool_t & xpool) const
    {
        if(type_name == nullptr)
            return nullptr;

        return type_name->type;
    }

    ////////// ////////// ////////// ////////// //////////
    // type_cast_expression_t

    vtype_t type_cast_expression_t::get_vtype() const
    {
        return __get_vtype(type_name);
    }

    type_t * type_cast_expression_t::get_type(xpool_t & xpool) const
    {
        if(type_name == nullptr)
            return nullptr;

        return type_name->type;
    }

    ////////// ////////// ////////// ////////// //////////
    // cvalue_expression_t

    vtype_t cvalue_expression_t::get_vtype() const
    {
        if(value == nullptr)
            return vtype_t::__unknown__;

        switch(value->value_type)
        {
            case cvalue_type_t::number:
                return to_vtype(value->number.type);

            case cvalue_type_t::string:
                return vtype_t::string;

            case cvalue_type_t::null:
            case cvalue_type_t::type:
                return vtype_t::mobject;

            default:
                return vtype_t::__unknown__;
        }
    }

    type_t * cvalue_expression_t::get_type(xpool_t & xpool) const
    {
        return nullptr;
    }

    ////////// ////////// ////////// ////////// //////////
    // function_expression_t

    name_t function_expression_t::get_name() const
    {
        if(namex != nullptr && namex->this_family() == expression_family_t::name)
            return ((name_expression_t *)namex)->name;

        return name_t::null;
    }

    vtype_t function_expression_t::get_vtype() const
    {
        if(method == nullptr)
            return vtype_t::__unknown__;

        return __get_vtype(method->type_name);
    }

    const string_t function_expression_t::to_string() const
    {
        if(generic_args == nullptr)
            return _F(_T("%1%(%2%)"), _str(namex), __to_string(arguments()));

        return _F(_T("%1%<%2%>(%3%)"), _str(namex), _str(generic_args), __to_string(arguments()));
    }

    ////////// ////////// ////////// ////////// //////////
    // index_expression_t

    vtype_t index_expression_t::get_vtype() const
    {
        throw _EC(unimplemented);
    }

    const string_t index_expression_t::to_string() const
    {
        return _F(_T("%1%[%2%]"), _str(namex), __to_string(arguments()));
    }

    ////////// ////////// ////////// ////////// //////////
    // new_expression_t

    const string_t new_expression_t::to_string() const
    {
        stringstream_t ss;
        ss << _T("new ") << _str(type_name);

        if(arguments() != nullptr)
            ss << _T("(") << _str(arguments()) << _T(")");
        else
            ss << _T("()");

        return ss.str();
    }

    vtype_t new_expression_t::get_vtype() const
    {
        return __get_vtype(type_name);
    }

    ////////// ////////// ////////// ////////// //////////
    // new_array_expression_t

    X_ENUM_INFO(array_initialize_element_type_t)

        X_C(initializer,        _T("initializer"))
        X_C(expression,         _T("expression"))

    X_ENUM_INFO_END

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    array_initialize_element_t::operator string_t() const
    {
        switch(type)
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

    void array_initializer_t::append(expression_t * expression)
    {
        elements.push_back(array_initialize_element_t(expression));
    }

    void array_initializer_t::append(array_initializer_t * initializer)
    {
        elements.push_back(array_initialize_element_t(initializer));
    }

    X_DEFINE_TO_STRING(array_initializer_t)
    {
        return al::join_str(elements.begin(), elements.end(), _T(", "),
            [](auto it) { return _str(it); }
        );
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    void new_array_expression_t::set_lengths(array_lengths_t * lengths)
    {
        __lengths = lengths;

        if(lengths != nullptr)
        {
            for(expression_t * exp : *lengths)
            {
                if(exp != nullptr)
                    exp->parent = this;
            }
        }
    }

    expression_t * new_array_expression_t::length_at(dimension_t dimension) const
    {
        if(__lengths == nullptr || dimension >= __lengths->size())
            throw _EC(overflow);

        return (*__lengths)[dimension];
    }

    void new_array_expression_t::set_length(dimension_t dimension, expression_t * length)
    {
        if(__lengths == nullptr || dimension >= __lengths->size())
            throw _EC(overflow);

        length->parent = this;
        (*__lengths)[dimension] = length;
    }

    const string_t new_array_expression_t::to_string() const
    {
        stringstream_t ss;
        ss << _T("new ") << _str(type_name) << _T("[") << _str(lengths()) << _T("]");

        return ss.str();
    }

    type_t * new_array_expression_t::to_array_type(xpool_t & xpool)
    {
        return xpool.new_array_type(to_type(type_name), dimension());
    }

    vtype_t new_array_expression_t::get_vtype() const
    {
        return vtype_t::mobject;
    }

    ////////// ////////// ////////// ////////// //////////
    // default_value_expression_t

    cvalue_t default_value_expression_t::execute(expression_execute_context_t & ctx)
    {
        type_t * type;
        if(type_name == nullptr || (type = type_name->type) == nullptr)
            return cvalue_t(nullptr);

        if(type->this_gtype() != gtype_t::general)
            return cvalue_t(nullptr);

        return default_value_of(((general_type_t *)type)->vtype);
    }

    vtype_t default_value_expression_t::get_vtype() const
    {
        return __get_vtype(to_type(type_name));
    }

    type_t * default_value_expression_t::get_type(xpool_t & xpool) const
    {
        return to_type(type_name);
    }

    const string_t default_value_expression_t::to_string() const
    {
        return _F(_T("default(%1%)"), _str(type_name));
    }

    ////////// ////////// ////////// ////////// //////////
    // type_of_expression_t

    const string_t type_of_expression_t::to_string() const
    {
        return _F(_T("typeof(%1%)"), _str(type_name));
    }

    cvalue_t type_of_expression_t::execute(expression_execute_context_t & ctx)
    {
        if(type_name == nullptr)
            return cvalue_t();

        return cvalue_t(type_name->type);
    }

    vtype_t type_of_expression_t::get_vtype() const
    {
        return vtype_t::mobject;
    }

    type_t * type_of_expression_t::get_type(xpool_t & xpool) const
    {
        return nullptr;
    }

    ////////// ////////// ////////// ////////// //////////
    // this_expression_t

    vtype_t this_expression_t::get_vtype() const
    {
        return vtype_t::__unknown__;
    }

    type_t * this_expression_t::get_type(xpool_t & xpool) const
    {
        return __type;
    }

    const string_t this_expression_t::to_string() const
    {
        return _T("this");
    }

    cvalue_t this_expression_t::execute(expression_execute_context_t & ctx)
    {
        return cvalue_t::nan;
    }

    ////////// ////////// ////////// ////////// //////////
    // base_expression_t

    vtype_t base_expression_t::get_vtype() const
    {
        return vtype_t::__unknown__;
    }

    type_t * base_expression_t::get_type(xpool_t & xpool) const
    {
        return __type;
    }

    const string_t base_expression_t::to_string() const
    {
        return _T("base");
    }

    cvalue_t base_expression_t::execute(expression_execute_context_t & ctx)
    {
        return cvalue_t::nan;
    }

    ////////// ////////// ////////// ////////// //////////
    // unitary_expression_t

    type_t * unitary_expression_t::get_type(xpool_t & xpool) const
    {
        return nullptr;
    }

    const string_t unitary_expression_t::to_string() const
    {
        return sprintf(_T("%1%%2%"), op(), expression_at(0)->to_string());
    }

    ////////// ////////// ////////// ////////// //////////
    // binary_expression_t

    type_t * binary_expression_t::get_type(xpool_t & xpool) const
    {
        switch(op())
        {
            case operator_t::member_point: {
                expression_t * e2 = exp2();
                return e2 != nullptr? e2->get_type(xpool) : nullptr;
            }   break;

            default:
                return nullptr;
        }
    }

    const string_t binary_expression_t::to_string() const
    {
        return sprintf(_T("(%1% %2% %3%)"),
            expression_at(0)->to_string(), op(), expression_at(1)->to_string()
        );
    }

    ////////// ////////// ////////// ////////// //////////
    // expressions_t

    vtype_t expressions_t::get_vtype() const
    {
        if(__expressions.size() == 0)
            return vtype_t::__unknown__;

        return __expressions[__expressions.size() - 1]->get_vtype();
    }

    type_t * expressions_t::get_type(xpool_t & xpool) const
    {
        if(__expressions.size() == 0)
            return nullptr;

        return __expressions[__expressions.size() - 1]->get_type(xpool);
    }

    ////////// ////////// ////////// ////////// //////////

    type_t * get_expression_type(xpool_t & xpool, expression_t * expression)
    {
        if(expression == nullptr)
            return nullptr;

        type_t * type = expression->get_type(xpool);
        if(type != nullptr)
            return type;

        vtype_t vtype = expression->get_vtype();
        if(vtype == vtype_t::__unknown__)
            return nullptr;

        return xpool.get_internal_type(vtype);
    }

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(statement_exit_point_type_t)

        X_C(self,       _T("self"))

        X_C(break_,     _T("break"))

        X_C(continue_,  _T("continue"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    class statement_exit_statement_point_t : public statement_point_t
    {
    public:
        statement_exit_statement_point_t(statement_region_t * region)
            : __region(region) { }

        virtual local_label_t to_position() override { return 0; }

    private:
        statement_region_t * __region;
    };

    //-------- ---------- ---------- ---------- ----------

    statement_region_t::statement_region_t()
    {
        al::zero_array(__points);
    }

    void statement_region_t::append_xilx(xilx_t * xilx)
    {
        _A(xilx != nullptr);

        __xilxes.push_back(xilx);
    }

    void statement_region_t::set_point(__point_type_t point_type, statement_point_t * point)
    {
        __points[(size_t)point_type] = point;
    }

    statement_point_t * statement_region_t::get_point(__point_type_t point_type)
    {
        return __points[(size_t)point_type];
    }

    //-------- ---------- ---------- ---------- ----------

    void region_xilx_t::write(statement_compile_context_t & ctx, xil_pool_t & pool)
    {
        __region->each([&ctx, &pool](xilx_t * xilx) {
            xilx->write(ctx, pool);
        });
    }

    //-------- ---------- ---------- ---------- ----------

    void xil_buffer_t::write(const byte_t * bytes, size_t size)
    {
        _A(bytes != nullptr);

        __ensure_size(this->size() + size);
        std::copy(bytes, bytes + size, __p);
        __p += size;
    }

    void xil_buffer_t::__ensure_size(size_t need_size)
    {
        if(__buffer == nullptr)
        {
            __size = _alignd(need_size, 128);
            __buffer = this->__alloc_objs<byte_t>(__size);
            __p = __buffer;
        }
        else if(__size < need_size)
        {
            __size = _alignd(__size, need_size);
            byte_t * new_buffer = this->__alloc_objs<byte_t>(__size);
            std::copy(__buffer, __p, new_buffer);
            this->__free(__buffer);

            __p = new_buffer + (__p - __buffer);
            __buffer = new_buffer;
        }
    }

    size_t xil_buffer_t::read(byte_t * buffer, size_t size)
    {
        _A(buffer != nullptr);

        if(__p_read == nullptr)
            __p_read = __buffer;

        size = al::min(size, __p_read - __buffer);
        if(size > 0)
        {
            std::copy(__p_read, __p_read + size, buffer);
            __p += size;
        }

        return size;
    }

    xil_buffer_t::~xil_buffer_t()
    {
        if(__buffer != nullptr)
        {
            this->__free(__buffer);
        }
    }

    //-------- ---------- ---------- ---------- ----------

    class root_statement_region_t : public statement_region_t
    {
    public:
        virtual const string_t to_string() const override
        {
            return _T("root");
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // xil_pool_t

    void xil_pool_t::commit()
    {
        int index = 0;
        auto it_begin = __xils.begin(), it = it_begin, it0 = it;

        for(auto it_end = __xils.end(); it < it_end; it++)
        {
            xil_t * xil = *it;
            __index_of(xil) = index;

            switch(command_of(xil))
            {
                case xil_command_t::smp: {

                    switch(sub_command_of<xil_smp_t>(xil))
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

    int xil_pool_t::index_of(xil_t * xil)
    {
        return __index_of(xil);
    }

    ////////// ////////// ////////// ////////// //////////
    // jmp_manager_t

    void jmp_manager_t::append_jmp(jmp_xil_t * xil, local_label_t label)
    {
        _A(xil != nullptr);

        __local_jmp_map[xil] = label;
    }

    void jmp_manager_t::append_jmp(jmp_xil_t * xil, name_t name)
    {
        _A(xil != nullptr);

        __global_jmp_map[xil] = name;
    }

    void jmp_manager_t::set_point(local_label_t label, xil_t * xil)
    {
        _A(xil != nullptr);

        __local_point_map[label] = xil;
    }

    void jmp_manager_t::set_point(name_t name, xil_t * xil)
    {
        _A(xil != nullptr);

        __global_point_map[name] = xil;
    }

    xil_t * jmp_manager_t::label_xil(local_label_t label)
    {
        auto it = __local_point_map.find(label);
        if(it == __local_point_map.end())
            return nullptr;

        return it->second;
    }

    void jmp_manager_t::commit(statement_compile_context_t & ctx)
    {
        for(auto && it : __local_jmp_map)
        {
            jmp_xil_t * jmp_xil = it.first;
            local_label_t label = it.second;

            xil_t * label_xil = this->label_xil(label);
            if(label_xil == nullptr)
                throw _ED(compile_error_code_t::label_not_found, label);

            int index1 = ctx.xil_pool().index_of(jmp_xil);
            int index2 = ctx.xil_pool().index_of(label_xil);

            jmp_xil->set_step(index2 - index1);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // statement_compile_context_t

    statement_compile_context_t::statement_compile_context_t(
                method_compile_context_t & mctx, method_t * method)
        : mctx(mctx),method(method)
    {
        _A(method != nullptr);
        begin_region<root_statement_region_t>();
    }

    statement_region_t * statement_compile_context_t::end_region()
    {
        _A(__current_region != nullptr);
        
        __current_region = __current_region->parent;
        return __current_region;
    }

    assembly_t * statement_compile_context_t::get_assembly()
    {
        if(__assembly == nullptr)
            __assembly = __ns::get_assembly(method);

        return __assembly;
    }

    void statement_compile_context_t::set_point(__point_type_t point_type, statement_point_t * point)
    {
        statement_region_t * region = current_region();
        _A(region != nullptr);

        region->set_point(point_type, point);
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // namespace X_ROOT_NS::modules::core


