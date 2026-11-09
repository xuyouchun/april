#include "c_tree.h"
#include "../lang_cs/ast_builder.h"

namespace X_ROOT_NS::modules::lang_c {

    using namespace core;
    using namespace compile;

    typedef compile::compile_error_code_t __e_t;
    typedef lang_cs::cs_token_value_t     __cs_token_t;

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        // The ast node of the statement of the code of c: a group of statements, { statement }, that takes the place of
        // the code of an embedded language: it is a statement of the code of the language that has it.
        class __group_t : public statement_group_st_ast_node_t
        {
            typedef statement_group_st_ast_node_t __super_t;

        public:
            using __super_t::__super_t;

            virtual ast_value_t value() const override
            {
                return (ast_value_t)lang_cs::cs_branch_type_t::statement;
            }
        };

        // Builder of the statement of the code of c: the statement is in a group of statements, { statement }: it is
        // one node of the code of the language that has it, its variables are those of a block.
        class __statement_builder_t : public tast_builder_t<__group_t>
        {
            typedef tast_builder_t<__group_t> __super_t;

        public:
            using __super_t::__super_t;

            // Applies token.
            virtual void apply_token(token_t * token, __apply_token_args_t & args) override { }

            // Applies ast node.
            virtual void apply_ast(ast_node_t * node, __apply_ast_args_t & args) override
            {
                __node->append_child(statement_group_st_ast_node_t::__statements__, node);
            }

            // Build completed.
            virtual void completed(__completed_args_t & args) override { __node->on_commit(); }
        };

        //-------- ---------- ---------- ---------- ----------

        // The nodes of cs that the syntax of c is made of: *p is p.Value, &x is System.Ptr.AddressOf(ref x). They take the
        // position of the code that they are made for.
        class __nodes_t
        {
        public:
            __nodes_t(ast_context_t & context, ast_node_t * position)
                : __context(context), __position(position) { }

            // Returns the name of a text.
            name_t name(const char_t * text)
            {
                return name_t(__context.compile_context.global_context.xpool.spool.to_sid(string_t(text)));
            }

            // Returns a new token.
            token_t * token(__cs_token_t value)
            {
                return __context.compile_context.new_obj<token_t>(
                    (token_value_t)value, __position != nullptr? __position->code_unit : nullptr
                );
            }

            template<typename node_t, typename ... args_t>
            node_t * make(args_t && ... args)
            {
                return __context.compile_context.new_obj<node_t>(__context, std::forward<args_t>(args) ...);
            }

            // Sets the position of a node.
            void at(ast_node_t * node)
            {
                node->code_unit = __position != nullptr? __position->code_unit : nullptr;
            }

            // An expression node that has the node.
            ast_node_t * expression(ast_node_t * node)
            {
                auto * exp = make<expression_ast_node_t>();
                exp->append_expression(node);
                at(exp);
                exp->on_commit();

                return exp;
            }

            // The call of a method: Name(arguments), the arguments are expression nodes; an argument is ref when the flag is.
            ast_node_t * call_name(const char_t * method, const std::vector<ast_node_t *> & arguments, bool ref_argument,
                                                                            ast_node_t * type_argument = nullptr)
            {
                auto * name_node = make<lang_cs::_function_name_expression_ast_node_t>();
                name_node->append_name(name(method), token(__cs_token_t::name));
                at(name_node);
                name_node->on_commit();

                auto * arguments_node = make<arguments_ast_node_t>();
                for (ast_node_t * exp : arguments)
                {
                    auto * argument = make<argument_ast_node_t>();
                    argument->set_child(argument_ast_node_t::__expression__, exp);
                    if (ref_argument)
                        argument->atype = param_type_t::ref;

                    at(argument);
                    argument->on_commit();

                    arguments_node->append_child(arguments_ast_node_t::__arguments__, argument);
                }

                at(arguments_node);
                arguments_node->on_commit();

                auto * function = make<function_ast_node_t>();
                function->set_child(function_ast_node_t::__namex__, name_node);

                // The type that the method is called with: Method<T>(...).
                if (type_argument != nullptr)
                {
                    auto * arg = make<generic_arg_ast_node_t>();
                    arg->set_child(generic_arg_ast_node_t::__type_name__, type_argument);
                    at(arg);
                    arg->on_commit();

                    auto * generic_args = make<generic_args_ast_node_t>();
                    generic_args->append_child(generic_args_ast_node_t::__generic_args__, arg);
                    at(generic_args);
                    generic_args->on_commit();

                    function->set_child(function_ast_node_t::__generic_args__, generic_args);
                }

                function->set_child(function_ast_node_t::__arguments__, arguments_node);
                at(function);
                function->on_commit();

                return function;
            }

        private:
            ast_context_t & __context;
            ast_node_t *    __position;
        };

        //-------- ---------- ---------- ---------- ----------

        // Builder of a pointer type name, T*: the type name System.Ptr<T> (System.Ptr for void*).
        class __pointer_type_builder_t : public tast_builder_t<type_name_ast_node_t>
        {
            typedef tast_builder_t<type_name_ast_node_t> __super_t;

        public:
            using __super_t::__super_t;

            // Applies token: the *.
            virtual void apply_token(token_t * token, __apply_token_args_t & args) override
            {
                __stars++;
            }

            // Applies ast node: the type that is pointed to.
            virtual void apply_ast(ast_node_t * node, __apply_ast_args_t & args) override
            {
                __target = node;
            }

            // Build completed.
            virtual void completed(__completed_args_t & args) override
            {
                if (__target == nullptr)
                    throw _E(__e_t::format_error, _T("the type that is pointed to is expected"));

                __nodes_t nodes(__context, __target);

                // void* is System.Ptr.
                const code_unit_t * cu = __target->code_unit;
                bool is_void = cu != nullptr && cu->length == 4 && string_t(cu->s, cu->length) == _T("void");

                // The type name nodes are the ones of a type name: a general type name that a node of type name holds.
                auto wrap = [&](ast_node_t * underlying) {
                    auto * wrapper = nodes.make<type_name_ast_node_t>();
                    wrapper->set_child(type_name_ast_node_t::__underlying__, underlying);
                    nodes.at(wrapper);
                    wrapper->on_commit();

                    return wrapper;
                };

                // One pointer for each *: the type inside is the one that was made before.
                ast_node_t * inner = wrap(__target);

                for (int star = 0; star < __stars; star++)
                {
                    auto * node = nodes.make<general_type_name_ast_node_t>();

                    auto * system = nodes.make<name_unit_ast_node_t>();
                    system->set_name(nodes.name(_T("System")), nullptr);
                    nodes.at(system);
                    system->on_commit();

                    auto * ptr = nodes.make<name_unit_ast_node_t>();
                    ptr->set_name(nodes.name(_T("Ptr")), nullptr);

                    if (!(is_void && star == 0))
                    {
                        auto * arg = nodes.make<generic_arg_ast_node_t>();
                        arg->set_child(generic_arg_ast_node_t::__type_name__, inner);
                        nodes.at(arg);
                        arg->on_commit();

                        auto * generic_args = nodes.make<generic_args_ast_node_t>();
                        generic_args->append_child(generic_args_ast_node_t::__generic_args__, arg);
                        nodes.at(generic_args);
                        generic_args->on_commit();

                        ptr->set_child(name_unit_ast_node_t::__generic_args__, generic_args);
                    }

                    nodes.at(ptr);
                    ptr->on_commit();

                    node->append_child(general_type_name_ast_node_t::__units__, system);
                    node->append_child(general_type_name_ast_node_t::__units__, ptr);
                    node->set_global_type(global_type_t::global);
                    nodes.at(node);
                    node->on_commit();

                    inner = wrap(node);
                }

                // This node is the last one.
                __node->set_child(type_name_ast_node_t::__underlying__,
                                  ((type_name_ast_node_t *)inner)->child_at(type_name_ast_node_t::__underlying__));
                __node->on_commit();
            }

        private:
            ast_node_t * __target = nullptr;
            int          __stars  = 0;
        };

        //-------- ---------- ---------- ---------- ----------

        // Builder of sizeof(T) and stackalloc T[n]: the call of NativeMemory.SizeOf<T>() and NativeMemory.StackAlloc<T>(n).
        class __call_builder_t : public tast_builder_t<lang_cs::_single_expression_ast_node_t>
        {
            typedef tast_builder_t<lang_cs::_single_expression_ast_node_t> __super_t;

        public:
            __call_builder_t(ast_context_t & context, lang_ast_build_args_t & args)
                : __super_t(context, args), __branch((c_branch_type_t)args.node_value)
            { }

            virtual void apply_token(token_t * token, __apply_token_args_t & args) override { }

            // Applies ast node: the type, and the count.
            virtual void apply_ast(ast_node_t * node, __apply_ast_args_t & args) override
            {
                if (node->value() == (ast_value_t)lang_cs::cs_branch_type_t::type_name)
                    __type = node;
                else
                    __count = node;
            }

            // Build completed.
            virtual void completed(__completed_args_t & args) override
            {
                if (__type == nullptr || (__branch == c_branch_type_t::c_stackalloc && __count == nullptr))
                    throw _E(__e_t::format_error, _T("a type is expected"));

                __nodes_t nodes(__context, __type);

                // System.Runtime.NativeMemory.
                __node->append_name(nodes.name(_T("System")), nodes.token(__cs_token_t::name));
                __node->append_token(nodes.token(__cs_token_t::member_point));
                __node->append_name(nodes.name(_T("Runtime")), nodes.token(__cs_token_t::name));
                __node->append_token(nodes.token(__cs_token_t::member_point));
                __node->append_name(nodes.name(_T("NativeMemory")), nodes.token(__cs_token_t::name));
                __node->append_token(nodes.token(__cs_token_t::member_point));

                if (__branch == c_branch_type_t::c_sizeof)
                    __node->append_expression(nodes.call_name(_T("SizeOf"), { }, false, __type));
                else
                    __node->append_expression(nodes.call_name(_T("StackAlloc"), { __count }, false, __type));

                __node->on_commit();
            }

        private:
            c_branch_type_t __branch;
            ast_node_t *    __type = nullptr;
            ast_node_t *    __count = nullptr;
        };

        //-------- ---------- ---------- ---------- ----------

        // Builder of *x and &x: the result is the expression of cs that the code is.
        class __unary_builder_t : public tast_builder_t<lang_cs::_single_expression_ast_node_t>
        {
            typedef tast_builder_t<lang_cs::_single_expression_ast_node_t> __super_t;

        public:
            __unary_builder_t(ast_context_t & context, lang_ast_build_args_t & args)
                : __super_t(context, args), __branch((c_branch_type_t)args.node_value)
            { }

            // Applies token: the operator.
            virtual void apply_token(token_t * token, __apply_token_args_t & args) override { }

            // Applies ast node: the operand.
            virtual void apply_ast(ast_node_t * node, __apply_ast_args_t & args) override
            {
                __operand = node;
            }

            // Build completed.
            virtual void completed(__completed_args_t & args) override
            {
                if (__operand == nullptr)
                    throw _E(__e_t::format_error, _T("an operand is expected"));

                __nodes_t nodes(__context, __operand);

                ast_node_t * operand = __operand;

                if (__branch == c_branch_type_t::c_deref)
                {
                    // *p: (p).Value ; the operand can be p++ : the group keeps it together: (p++).Value is the value that p
                    // pointed to before it moved. The group is the brackets and an expression.
                    auto * group = nodes.make<expression_ast_node_t>();
                    group->append_expression(operand);
                    nodes.at(group);
                    group->on_commit();

                    __node->append_token(nodes.token(__cs_token_t::left_bracket));
                    __node->append_expression(group);
                    __node->append_token(nodes.token(__cs_token_t::right_bracket));

                    __node->append_token(nodes.token(__cs_token_t::member_point));
                    __node->append_name(nodes.name(_T("Value")), nodes.token(__cs_token_t::name));
                }
                else
                {
                    // &x: System.Ptr.AddressOf(ref x)
                    __node->append_name(nodes.name(_T("System")), nodes.token(__cs_token_t::name));
                    __node->append_token(nodes.token(__cs_token_t::member_point));
                    __node->append_name(nodes.name(_T("Ptr")), nodes.token(__cs_token_t::name));
                    __node->append_token(nodes.token(__cs_token_t::member_point));
                    __node->append_expression(nodes.call_name(_T("AddressOf"), { nodes.expression(operand) }, true));
                }

                __node->on_commit();
            }

        private:
            c_branch_type_t __branch;
            ast_node_t *    __operand = nullptr;
        };
    }

    namespace
    {
        // Builder of a simple expression of cs, with the token -> of c: p->x is p.Value.x.
        class __single_expression_builder_t : public lang_cs::_single_expression_ast_builder_t
        {
            typedef lang_cs::_single_expression_ast_builder_t __super_t;

        public:
            using __super_t::__super_t;

            // Applies token.
            virtual void apply_token(token_t * token, ast_builder_apply_token_args_t & args) override
            {
                if (token->value != c_ptr_member_token_value)
                {
                    __super_t::apply_token(token, args);
                    return;
                }

                const code_unit_t & cu = token->code_unit;

                auto make = [&](__cs_token_t value) {
                    return this->__context.compile_context.template new_obj<token_t>(
                        (token_value_t)value, cu.s, (int32_t)cu.length, cu.file
                    );
                };

                (*this)->append_token(make(__cs_token_t::member_point));
                (*this)->append_name(this->__to_name(string_t(_T("Value"))), make(__cs_token_t::name));
                (*this)->append_token(make(__cs_token_t::member_point));
            }
        };
    }

    // Builds the ast node of a simple expression of cs.
    ast_node_t * build_c_single_expression(ast_context_t & context, lang_ast_build_args_t & args)
    {
        return __single_expression_builder_t(context, args).build();
    }

    ////////// ////////// ////////// ////////// //////////

    // Builds the ast node of a branch of c.
    ast_node_t * build_c_ast(ast_context_t & context, lang_ast_build_args_t & args)
    {
        switch ((c_branch_type_t)args.node_value)
        {
            case c_branch_type_t::c_deref:
            case c_branch_type_t::c_address:
                return __unary_builder_t(context, args).build();

            case c_branch_type_t::c_pointer_type_name:
                return __pointer_type_builder_t(context, args).build();

            case c_branch_type_t::c_sizeof:
            case c_branch_type_t::c_stackalloc:
                return __call_builder_t(context, args).build();

            default:
                return __statement_builder_t(context, args).build();
        }
    }

    ////////// ////////// ////////// ////////// //////////

}   // X_ROOT_NS::modules::lang_c
