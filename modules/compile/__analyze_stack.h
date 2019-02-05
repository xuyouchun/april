#ifndef __COMPILE_ANALYZE_STACK_H__
#define __COMPILE_ANALYZE_STACK_H__

namespace X_ROOT_NS { namespace modules { namespace compile {

    namespace
    {
        using namespace core;
        using namespace std::placeholders;
    }

    ////////// ////////// ////////// ////////// //////////

    // Analyze stack error codes.
    X_ENUM(analyze_stack_error_t)

        // Format error.
        format_error,

        // Unknown element type.
        unknown_element_type,   

        // Unknown token property.
        unknown_token_property,

        // Unknown operator property.
        unknown_operator_property,

        // Unknown expression property.
        unknown_expression_property,

        // Unknown token.
        unknown_token,

        // Adhere overflow.
        adhere_overflow,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    // Analyze stack element types.
    X_ENUM(analyze_stack_element_type_t)

        // Token.
        token,

        // Name.
        name,

        // Expression.
        expression,

        // Operator.
        operator_,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    // Analyze stack element.
    struct analyze_stack_element_t
    {
        // Constructor.
        analyze_stack_element_t() = default;
        analyze_stack_element_t(const analyze_stack_element_t &) = default;

        // Constructor with type.
        analyze_stack_element_t(analyze_stack_element_type_t type)
            : type(type) { }

        // Constructor with token.
        analyze_stack_element_t(token_t * token)
            : type(analyze_stack_element_type_t::token)
            , token(token) { }

        // Constructor with token and name.
        analyze_stack_element_t(token_t * token, const name_t & name)
            : type(analyze_stack_element_type_t::name)
            , token(token), name(name) { }

        // Constructor with expression.
        analyze_stack_element_t(expression_t * expression)
            : type(analyze_stack_element_type_t::expression)
            , expression(expression) { }

        // Constructor with operator.
        analyze_stack_element_t(operator_t operator_)
            : type(analyze_stack_element_type_t::operator_)
            , operator_(operator_) { }

        // Element type.
        analyze_stack_element_type_t type;

        union
        {
            token_t       * token;              // Token.
            expression_t  * expression;         // Expression.
            operator_t      operator_;          // Operator.
        };

        name_t name;

        // Returns whether two elements are equals.
        bool operator == (const analyze_stack_element_t & other) const;

        // Returns whether two elements are not equals.
        bool operator != (const analyze_stack_element_t & other) const;

        // Converts to a string.
        operator string_t() const;

        // The end element.
        static const analyze_stack_element_t end;
    };

    typedef analyze_stack_element_t __stack_element_t;

    ////////// ////////// ////////// ////////// //////////

    // Interface for read elements.
    X_INTERFACE analyze_stack_elements_t
    {
        virtual __stack_element_t read_next() = 0;
    };

    typedef analyze_stack_elements_t __stack_elements_t;

    ////////// ////////// ////////// ////////// //////////

    // An structure with token and language.
    class __expression_ex_t
    {
    public:

        // Constructor.
        __expression_ex_t(lang_t * lang, token_t * op_token)
            : lang(lang), op_token(op_token)
        {
            _A(lang != nullptr);
        }

        lang_t * const lang;            // Language.
        token_t * const op_token;       // Operator token.
    };

    //-------- ---------- ---------- ---------- ----------

    // A base class of expression.
    template<typename base_exp_t>
    class __exp_base_t : public base_exp_t, public __expression_ex_t
    {
    public:

        // Constructor.
        template<typename ... args_t>
        __exp_base_t(lang_t * lang, token_t * op_token, args_t ... args)
            : base_exp_t(args ...), __expression_ex_t(lang, op_token)
        { }
    };

    //-------- ---------- ---------- ---------- ----------

    // A base class of expression.
    template<typename base_exp_t>
    class __exp_t : public __exp_base_t<base_exp_t>
    {
    public:
        using __exp_base_t<base_exp_t>::__exp_base_t;
    };

    //-------- ---------- ---------- ---------- ----------

    // Unitary expression.
    template<>
    class __exp_t<unitary_expression_t> : public __exp_base_t<unitary_expression_t>
    {
    public:
        using __exp_base_t<unitary_expression_t>::__exp_base_t;
     
        // Converts to a string.
        const string_t to_string() const override
        {
            lang_service_helper_t h(lang);
            const operator_property_t * property = h.get_operator_property(op_token->value);
            if(property != nullptr && property->adhere > 0)
            {
                return sprintf(_T("%1%%2%"), _str(expression_at(0)), (string_t)*op_token);
            }

            return sprintf(_T("%1%%2%"), (string_t)*op_token, _str(expression_at(0)));
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Binary expression.
    template<>
    class __exp_t<binary_expression_t> : public __exp_base_t<binary_expression_t>
    {
    public:
        using __exp_base_t<binary_expression_t>::__exp_base_t;

        // Converts to a string.
        const string_t to_string() const override
        {
            return sprintf(_T("(%1% %2% %3%)"), _str(expression_at(0)),
                (string_t)*op_token, _str(expression_at(1))
            );
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Creates a system expression.
    template<typename ... args_t>
    expression_t * to_sys_exp(memory_t * memory, lang_t * lang, token_t * op_token,
                                                                args_t && ... args)
    {
        typedef std::remove_pointer_t<
            decltype(core::to_exp(memory, std::forward<args_t>(args) ...))
        > base_exp_t;

        typedef __exp_t<system_expression_t<base_exp_t>> exp_t;
        return memory_t::new_obj<exp_t>(memory, lang, op_token, std::forward<args_t>(args) ...);
    }

    ////////// ////////// ////////// ////////// //////////

    // Context for analyze stack.
    class analyze_stack_context_t : public object_t
    {
        typedef lang_service_helper_t __h_t;
        typedef analyze_stack_error_t __e_t;
        typedef analyze_stack_context_t __self_t;

    public:

        // Constructors
        analyze_stack_context_t(memory_t * memory, lang_t * lang)
            : memory(memory), lang(lang), __service_helper(lang)
            , __token_properties(std::bind(&__h_t::get_token_property, &__service_helper, _1))
            , __operator_properties(std::bind(&__self_t::__get_operator_property, this, _1))
            , __expression_box_properties(std::bind(&__h_t::get_expression_box_property,
                                                                             &__service_helper, _1))
        {
            _A(lang != nullptr);
        }

        lang_t * const lang;            // Language
        memory_t * const memory;        // Memory management.

        // Returns whether it's an operator.
        bool is_operator(token_value_t value)
        {
            return get_token_property(value)->is_operator;
        }

        // Returns whether it's a expression box.
        bool is_expression_box(token_value_t value)
        {
            return get_token_property(value)->is_expression_box;
        }

        // Returns operator by token value.
        operator_t get_operator(token_value_t value)
        {
            return get_operator_property(value)->op;
        }

        // Returns operator priority by token value.
        operator_priority_t get_priority(token_value_t value)
        {
            return get_operator_property(value)->priority;
        }

        // Returns operator arity by token value.
        operator_arity_t get_arity(token_value_t value)
        {
            return get_operator_property(value)->arity;
        }

        // Returns operator adhere by token value.
        operator_adhere_t get_adhere(token_value_t value)
        {
            return get_operator_property(value)->adhere;
        }

        // Returns operator direction by token value.
        operator_direct_t get_direct(token_value_t value)
        {
            return get_operator_property(value)->direct;
        }

        // Converts to a system expression.
        template<typename ... args_t>
        expression_t * to_exp(token_t * op_token, args_t && ... args)
        {
            return to_sys_exp(memory, lang, op_token, std::forward<args_t>(args) ...);
        }

        // Returns token property by token value.
        const token_property_t * get_token_property(token_value_t value)
        {
            return __check_ret(__token_properties[value],
                __e_t::unknown_token_property, value
            );
        }

        // Returns operator property by token value.
        const operator_property_t * get_operator_property(token_value_t value)
        {
            return __check_ret(__operator_properties[value],
                __e_t::unknown_operator_property, value
            );
        }

        // Returns expression box property by token value.
        const expression_box_property_t * get_expression_box_property(token_value_t value)
        {
            return __check_ret(__expression_box_properties[value],
                __e_t::unknown_expression_property, value
            );
        }

        // Builds expression with context and arguments.
        expression_t * build_expression(lang_expression_build_context_t & ctx,
                                        const lang_expression_build_args_t & args);

    private:
        lang_service_helper_t __service_helper;
        lang_token_property_service_t * __token_property_service = nullptr;
        lang_operator_property_service_t * __operator_property_service = nullptr;

        template<typename value_t> using __cache_t = al::auto_cache_t<token_value_t, value_t>;

        __cache_t<const token_property_t *> __token_properties;
        __cache_t<const operator_property_t *> __operator_properties;
        __cache_t<const expression_box_property_t *> __expression_box_properties;

        // Returns operator property.
        const operator_property_t * __get_operator_property(token_value_t value);

        // Checks return value.
        template<typename ret_t, typename value_t>
        ret_t __check_ret(ret_t ret, __e_t error, value_t value)
        {
            if(ret == nullptr)
                throw _E(error, sprintf(_title(error), value));

            return ret;
        }
    };

    typedef analyze_stack_context_t __stack_context_t;

    ////////// ////////// ////////// ////////// //////////

    // Context for build expression.
    class __lang_expression_build_context_t : public lang_expression_build_context_t
    {
    public:

        // Constructor.
        __lang_expression_build_context_t(__stack_context_t * context)
            : lang_expression_build_context_t(context->memory), __context(context)
        { }

        // Creates default expression.
        virtual expression_t * create_default_expression(
                                const lang_expression_build_args_t & args) override;

    private:
        __stack_context_t * __context;
    };

    ////////// ////////// ////////// ////////// //////////

    // Analyze stack.
    class analyze_stack_t : public object_t
    {
    public:

        // Constructors.
        analyze_stack_t(__stack_context_t * context, __stack_elements_t * elements,
                            token_value_t end_token_value = __no_end_token_value, int deep = 0)
            : __context(context), __elements(elements), __end_token_value(end_token_value)
            , __deep(deep)
        {
            _A(context != nullptr);
            _A(elements != nullptr);
        }

        // Analyze.
        expression_t * analyze();

    private:
        __stack_context_t *  __context;
        __stack_elements_t * __elements;
        token_value_t        __end_token_value;

        const int __deep = 0;
        int __right_placeholder = 0;

        static const token_value_t __no_end_token_value = -1;

        //-------- ---------- ---------- ---------- ----------

        // Operator.
        struct __op_t
        {
            __op_t() = default;
            __op_t(const operator_property_t * op_property, token_t * token)
                : op_property(op_property), token(token) { }

            const operator_property_t * op_property;
            token_t * token;

            operator string_t() const { return token? _str(token) : _str(op_property); }
        };

        //-------- ---------- ---------- ---------- ----------

        // Expression.
        struct __exp_t
        {
            __exp_t() = default;
            __exp_t(expression_t * exp) : exp(exp) { }

            expression_t * exp;

            operator string_t() const { return _str(exp); }
        };

        //-------- ---------- ---------- ---------- ----------

        // Stack.
        template<typename t>
        struct __stack_t : public std::stack<t>
        {
            t pop_back()
            {
                if(this->size() > 0)
                {
                    t top = this->top();
                    this->pop();

                    return std::move(top);
                }

                throw _E(common_error_code_t::invalid_operation, _T("stack empty"));
            }
        };

        //-------- ---------- ---------- ---------- ----------

        __stack_t<__op_t>  __op_stack;
        __stack_t<__exp_t> __exp_stack;

        // Pushes a token.
        void __push_token(token_t * token);

        // Pushes an expression.
        void __push_expression(expression_t * expression);

        // Pushes a name.
        void __push_name(token_t * token, const name_t & name);

        // Pushes an operator.
        void __push_operator(token_t * token);

        // Pushes an operator.
        void __push_operator(operator_t op);

        // Pushes an operator.
        void __push_operator(const operator_property_t * op_property, token_t * token);

        // Pushes an expression box.
        void __push_expression_box(token_t * token);

        // Pop expression.
        expression_t * __pop_expression();

        // Combines expressions.
        expression_t * __combine_expression(const operator_property_t * property,
                                        token_t * token, expression_t ** expressions);

        // Combines expressions.
        expression_t * __combine_expressions(const operator_property_t * property,
                                        token_t * token);

        // Combines expressions.
        void __combine_expressions(operator_priority_t until_priority);

        // Returns top operator property.
        const operator_property_t * __top_op_property();

        // Combines all expressions.
        expression_t * __combine_all();

        // Returns right placeholder.
        int __get_right_placeholder(token_t * token);

        typedef operator_priority_t __priority_t;

        // Returns whether a priority is higher than another.
        static bool __high_privilege(__priority_t priority1, __priority_t priority2)
        {
            return priority1 < priority2;
        }

        // Returns whether a priority is lower than another.
        static bool __low_privilege(__priority_t priority1, __priority_t priority2)
        {
            return priority1 > priority2;
        }
    };

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        // Simple elements.
        template<typename itor_t>
        class __simple_elements_t : public analyze_stack_elements_t
        {
            typedef itor_t __itor_t;

        public:

            // Constructor
            __simple_elements_t(__itor_t begin, __itor_t end)
                : __begin(begin), __end(end), __cur(begin) { }

            // Read next element.
            virtual __stack_element_t read_next() override
            {
                if(__cur != __end)
                {
                    return *__cur++;
                }

                return analyze_stack_element_t::end;
            }

        private:
            __itor_t __begin, __end, __cur;
        };
    }

    //-------- ---------- ---------- ---------- ----------

    // Parse expression with specified expressions.
    template<typename itor_t>
    expression_t * parse_expression(memory_t * memory, lang_t * lang, itor_t begin, itor_t end)
    {
        _A(lang != nullptr);

        if(begin == end)
            return nullptr;

        analyze_stack_context_t context(memory, lang);
        __simple_elements_t<itor_t> elements(begin, end);

        analyze_stack_t stack(&context, &elements);
        return stack.analyze();
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // X_ROOT_NS::modules::compile

#endif // __COMPILE_ANALYZE_STACK_H__
