
#include "lq_tree.h"
#include "../lang_cs/ast.h"

namespace X_ROOT_NS::modules::lang_lq {

    using namespace core;
    using namespace compile;

    typedef compile::compile_error_code_t __e_t;
    typedef lang_cs::cs_token_value_t     __cs_token_t;

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        // The ast node of a clause of a query: the tokens and the nodes that were read are kept, the
        // query makes the nodes of the language of the code from them.
        class __clause_node_t : public single_ast_node_t
        {
        public:
            __clause_node_t(ast_context_t & context) { }

            __clause_node_t(ast_context_t & context, lq_branch_type_t value)
                : __value(value)
            { }

            virtual ast_value_t value() const override { return (ast_value_t)__value; }

            token_t *                 keyword = nullptr;    // The first token: from, where, ...
            std::vector<token_t *>    names;                // The names: x of from x in xs.
            ast_node_t *              type_name = nullptr;  // The type of a range variable.
            std::vector<ast_node_t *> nodes;                // The expressions, the clauses.
            bool                      descending = false;

            virtual const string_t to_string() const override { return _T("[lq clause ast]"); }

        private:
            lq_branch_type_t __value = lq_branch_type_t::lq_query;
        };

        //-------- ---------- ---------- ---------- ----------

        // Builder of a clause.
        class __clause_builder_t : public tast_builder_t<__clause_node_t>
        {
            typedef tast_builder_t<__clause_node_t> __super_t;

        public:
            __clause_builder_t(ast_context_t & context, lang_ast_build_args_t & args)
                : __super_t(context, args), __branch((lq_branch_type_t)args.node_value)
            { }

            // Creates the node.
            virtual ast_node_t * create() override
            {
                return __node = __new_obj<__clause_node_t>(__context, __branch);
            }

            // Applies token.
            virtual void apply_token(token_t * token, __apply_token_args_t & args) override
            {
                if (token->value == (token_value_t)__cs_token_t::name)
                {
                    __node->names.push_back(token);
                    return;
                }

                if (token->value == (token_value_t)__cs_token_t::k_descending)
                    __node->descending = true;

                if (__node->keyword == nullptr)
                    __node->keyword = token;
            }

            // Applies ast node.
            virtual void apply_ast(ast_node_t * node, __apply_ast_args_t & args) override
            {
                if (node->value() == (ast_value_t)lang_cs::cs_branch_type_t::type_name)
                    __node->type_name = node;
                else
                    __node->nodes.push_back(node);
            }

            // Build completed.
            virtual void completed(__completed_args_t & args) override { }

        private:
            lq_branch_type_t __branch;
        };

        //-------- ---------- ---------- ---------- ----------

        // The ast node of a query: a simple expression of cs, it takes the place of the code of the embedded language
        // (the node of cs has the value of an expression: this one has the value that the grammar of cs expects).
        class __single_t : public lang_cs::_single_expression_ast_node_t
        {
            typedef lang_cs::_single_expression_ast_node_t __super_t;

        public:
            using __super_t::__super_t;

            virtual ast_value_t value() const override
            {
                return (ast_value_t)lang_cs::cs_branch_type_t::_single_expression;
            }
        };

        // Makes the nodes of the language of the code (cs) for a query: the calls of the methods of
        // System.Linq. The nodes take the positions of the clauses of the query.
        //
        // The range variable is the param of the lambdas while there is only one. With more than
        // one (from x in xs from y in ys) the items of the source are pairs, TransparentIdentifier
        // of the item before and the new variable: the lambdas of the clauses that follow have the
        // pair as their param, and give its parts to TransparentIdentifier.Bind with the params that
        // have the names of the variables: __t => Bind((x, y) => body, __t.Item1, __t.Item2), with
        // any count of variables.
        class __translator_t
        {
        public:
            __translator_t(ast_context_t & context) : __context(context) { }

            // Fills the node with the calls of the methods for the clauses.
            void translate(__single_t * result, const std::vector<ast_node_t *> & parts)
            {
                __clause_node_t * from = __clause(parts.empty()? nullptr : parts[0],
                                                  lq_branch_type_t::lq_from);

                if (from->names.empty() || from->nodes.empty())
                    __fail(from, _T("a from clause is expected"));

                __vars.clear();
                __vars.push_back(__name_of(from->names[0]));

                ast_node_t * source = from->nodes[0];
                if (from->type_name != nullptr)
                    source = __cast(source, from->type_name, __vars[0], from);

                __run(result, parts, 1, source);
            }

        private:
            ast_context_t &     __context;
            std::vector<name_t> __vars;         // The range variables.
            int                 __counter = 0;

            // Fails.
            void __fail(ast_node_t * node, const string_t & message)
            {
                throw _E(__e_t::format_error, message);
            }

            // Returns the clause.
            __clause_node_t * __clause(ast_node_t * node, lq_branch_type_t branch)
            {
                if (node == nullptr || node->value() != (ast_value_t)branch)
                    throw _E(__e_t::format_error, _T("a from clause is expected"));

                return (__clause_node_t *)node;
            }

            //-------- ---------- ---------- ---------- ----------
            // The clauses.

            // Runs the clauses from the index: the last one fills the result.
            void __run(__single_t * result, const std::vector<ast_node_t *> & parts, size_t index,
                                                                         ast_node_t * source)
            {
                for (size_t k = index; k < parts.size(); k++)
                {
                    __clause_node_t * clause = (__clause_node_t *)parts[k];
                    bool last = k + 1 == parts.size();

                    switch ((lq_branch_type_t)clause->value())
                    {
                        case lq_branch_type_t::lq_where:
                            source = __call(nullptr, source, _T("Where"),
                                { __element_lambda(clause, [&]() { return clause->nodes[0]; }) },
                                clause);
                            break;

                        case lq_branch_type_t::lq_let:
                            source = __let(clause, source);
                            break;

                        case lq_branch_type_t::lq_from:
                            source = __more_from(clause, source);
                            break;

                        case lq_branch_type_t::lq_join:
                            source = __join(clause, source);
                            break;

                        case lq_branch_type_t::lq_orderby:
                            source = __orderby(clause, source);
                            break;

                        case lq_branch_type_t::lq_select:
                            source = __call(last? result : nullptr, source, _T("Select"),
                                { __element_lambda(clause, [&]() { return clause->nodes[0]; }) },
                                clause);

                            __continue(result, parts, k, source);
                            return;

                        case lq_branch_type_t::lq_group:
                            if (clause->nodes.size() < 2)
                                __fail(clause, _T("group needs an element and a key"));

                            source = __call(last? result : nullptr, source, _T("GroupBy"), {
                                __element_lambda(clause, [&]() { return clause->nodes[1]; }),
                                __element_lambda(clause, [&]() { return clause->nodes[0]; })
                            }, clause);

                            __continue(result, parts, k, source);
                            return;

                        default:
                            __fail(clause, _T("unexpected clause of a query"));
                    }
                }

                __fail(nullptr, _T("a query must end with select or group"));
            }

            // The query goes on with "into name" after select or group: the result is the source of
            // the clauses that follow.
            void __continue(__single_t * result, const std::vector<ast_node_t *> & parts,
                                                    size_t index, ast_node_t * source)
            {
                if (index + 1 == parts.size())
                    return;

                if (index + 2 != parts.size()
                        || parts[index + 1]->value() != (ast_value_t)lq_branch_type_t::lq_into)
                    __fail(parts[index + 1], _T("nothing can follow select or group but into"));

                __clause_node_t * into = (__clause_node_t *)parts[index + 1];
                if (into->names.empty())
                    __fail(into, _T("the name of a range variable is expected"));

                __vars.clear();
                __vars.push_back(__name_of(into->names[0]));

                __run(result, into->nodes, 0, source);
            }

            // let name = expression: the new variable is the second of a pair.
            ast_node_t * __let(__clause_node_t * clause, ast_node_t * source)
            {
                if (clause->names.empty() || clause->nodes.empty())
                    __fail(clause, _T("let needs a name and a value"));

                name_t name = __name_of(clause->names[0]);

                ast_node_t * lambda = __element_lambda(clause, [&]() {
                    return __expression(__create(__rebuild_item(clause), clause->nodes[0], clause),
                                        clause);
                });

                source = __call(nullptr, source, _T("Select"), { lambda }, clause);
                __vars.push_back(name);

                return source;
            }

            // from y in ys: SelectMany(item => ys, (item, y) => TransparentIdentifier.Create(item, y)).
            ast_node_t * __more_from(__clause_node_t * clause, ast_node_t * source)
            {
                if (clause->names.empty() || clause->nodes.empty())
                    __fail(clause, _T("a from clause needs a name and a collection"));

                name_t name = __name_of(clause->names[0]);

                ast_node_t * collection = clause->nodes[0];
                if (clause->type_name != nullptr)
                    collection = __cast(collection, clause->type_name, name, clause);

                // The collection may use the range variables before.
                ast_node_t * selector = __element_lambda(clause, [&]() { return collection; });

                source = __call(nullptr, source, _T("SelectMany"),
                                { selector, __pair_lambda(clause, name, name) }, clause);

                __vars.push_back(name);
                return source;
            }

            // join y in ys on a equals b, with "into g" the result of GroupJoin has the group.
            ast_node_t * __join(__clause_node_t * clause, ast_node_t * source)
            {
                if (clause->names.empty() || clause->nodes.size() < 3)
                    __fail(clause, _T("join needs a name, a collection and the keys"));

                name_t name = __name_of(clause->names[0]);
                bool grouped = clause->names.size() > 1;
                name_t result_name = grouped? __name_of(clause->names[1]) : name;

                ast_node_t * inner = clause->nodes[0];
                if (clause->type_name != nullptr)
                    inner = __cast(inner, clause->type_name, name, clause);

                ast_node_t * outer_key = __element_lambda(clause, [&]() { return clause->nodes[1]; });
                ast_node_t * inner_key = __lambda({ name }, clause->nodes[2], clause);

                source = __call(nullptr, source, grouped? _T("GroupJoin") : _T("Join"), {
                    inner, outer_key, inner_key, __pair_lambda(clause, result_name, result_name)
                }, clause);

                __vars.push_back(result_name);
                return source;
            }

            // orderby a, b descending: OrderBy(..).ThenByDescending(..).
            ast_node_t * __orderby(__clause_node_t * clause, ast_node_t * source)
            {
                bool first = true;

                for (ast_node_t * node : clause->nodes)
                {
                    __clause_node_t * ordering = (__clause_node_t *)node;
                    if (ordering->nodes.empty())
                        __fail(ordering, _T("an expression is expected"));

                    const char_t * method = first?
                        (ordering->descending? _T("OrderByDescending") : _T("OrderBy"))
                      : (ordering->descending? _T("ThenByDescending") : _T("ThenBy"));

                    source = __call(nullptr, source, method, {
                        __element_lambda(ordering, [&]() { return ordering->nodes[0]; })
                    }, ordering);

                    first = false;
                }

                if (first)
                    __fail(clause, _T("orderby needs an expression"));

                return source;
            }

            //-------- ---------- ---------- ---------- ----------
            // The nodes.

            // Returns the name of a token.
            name_t __name_of(token_t * token)
            {
                return __to_name(_str(token));
            }

            name_t __to_name(const string_t & s)
            {
                return name_t(__context.compile_context.global_context.xpool.spool.to_sid(s));
            }

            template<typename node_t, typename ... args_t>
            node_t * __new(args_t && ... args)
            {
                return __context.compile_context.new_obj<node_t>(
                    __context, std::forward<args_t>(args) ...
                );
            }

            // Sets the position of a node.
            static void __at(ast_node_t * node, ast_node_t * position)
            {
                node->code_unit = position != nullptr? position->code_unit : nullptr;
            }

            // Returns a new token of the position of a clause.
            token_t * __token(__cs_token_t value, ast_node_t * position)
            {
                return __context.compile_context.new_obj<token_t>(
                    (token_value_t)value, position != nullptr? position->code_unit : nullptr
                );
            }

            // An expression node that has the node.
            ast_node_t * __expression(ast_node_t * node, ast_node_t * position)
            {
                auto * exp = __new<expression_ast_node_t>();
                exp->append_expression(node);
                __at(exp, position);
                exp->on_commit();

                return exp;
            }

            // The expression of a name.
            ast_node_t * __name_expression(name_t name, ast_node_t * position)
            {
                auto * exp = __new<__single_t>();
                exp->append_name(name, __token(__cs_token_t::name, position));
                __at(exp, position);
                exp->on_commit();

                return exp;
            }

            // The expression of a name and its members: __t.Item1.Item2.
            ast_node_t * __member_expression(name_t name, const std::vector<const char_t *> & members,
                                                                    ast_node_t * position)
            {
                auto * exp = __new<__single_t>();
                exp->append_name(name, __token(__cs_token_t::name, position));

                for (const char_t * member : members)
                {
                    exp->append_token(__token(__cs_token_t::member_point, position));
                    exp->append_name(__to_name(member), __token(__cs_token_t::name, position));
                }

                __at(exp, position);
                exp->on_commit();

                return exp;
            }

            // A lambda: (a, b) => body.
            ast_node_t * __lambda(const std::vector<name_t> & params, ast_node_t * body,
                                                                    ast_node_t * position)
            {
                auto * lambda = __new<lambda_ast_node_t>();
                for (name_t name : params)
                    lambda->add_param_name(name);

                lambda->set_child(lambda_ast_node_t::__expression__, body);
                __at(lambda, position);
                lambda->on_commit();

                return __expression(lambda, position);
            }

            // A lambda that converts the items to the type: (Type name) => name.
            ast_node_t * __typed_lambda(ast_node_t * type_name, name_t name, ast_node_t * position)
            {
                auto * param = __new<param_ast_node_t>();
                param->set_name(name, __token(__cs_token_t::name, position));
                param->set_child(param_ast_node_t::__type_name__, type_name);
                __at(param, position);
                param->on_commit();

                auto * params = __new<params_ast_node_t>();
                params->append_child(params_ast_node_t::__params__, param);
                __at(params, position);
                params->on_commit();

                auto * lambda = __new<lambda_ast_node_t>();
                lambda->set_child(lambda_ast_node_t::__params__, params);
                lambda->set_child(lambda_ast_node_t::__expression__,
                                  __name_expression(name, position));
                __at(lambda, position);
                lambda->on_commit();

                return __expression(lambda, position);
            }

            // The items of the collection are converted to the type: ys.Select((Type y) => y).
            ast_node_t * __cast(ast_node_t * source, ast_node_t * type_name, name_t name,
                                                                        ast_node_t * position)
            {
                return __call(nullptr, source, _T("Select"),
                              { __typed_lambda(type_name, name, position) }, position);
            }

            // The name of the param of the lambdas that have the item of the source.
            name_t __item_param()
            {
                return __vars.size() == 1? __vars[0] : __to_name(_F(_T("__t%1%"), ++__counter));
            }

            // A lambda of the item of the source, its body is made by the function: the range
            // variables are its param, or the params of the function that has the pair.
            template<typename body_t>
            ast_node_t * __element_lambda(ast_node_t * position, body_t body_function)
            {
                if (__vars.size() == 1)
                    return __lambda({ __vars[0] }, body_function(), position);

                name_t item = __item_param();

                // The paths of the variables in the pair: ((a, b), c) is a = Item1.Item1.
                std::vector<ast_node_t *> arguments;
                size_t count = __vars.size();

                for (size_t index = 0; index < count; index++)
                {
                    std::vector<const char_t *> path;

                    size_t depth = index == 0? count - 1 : count - 1 - index;
                    for (size_t k = 0; k < depth; k++)
                        path.push_back(_T("Item1"));

                    if (index > 0)
                        path.push_back(_T("Item2"));

                    arguments.push_back(__expression(
                        __member_expression(item, path, position), position));
                }

                // The function is the first argument, the values follow it.
                arguments.insert(arguments.begin(), __lambda(__vars, body_function(), position));

                ast_node_t * bind = __static_call(_T("TransparentIdentifier"), _T("Bind"),
                                                                    arguments, position);

                return __lambda({ item }, __expression(bind, position), position);
            }

            // The pair of the item and the new variable, for the result of SelectMany or Join:
            // (item, y) => TransparentIdentifier.Create(item, y).
            ast_node_t * __pair_lambda(ast_node_t * position, name_t second_param, name_t second_name)
            {
                name_t item = __item_param();

                ast_node_t * first_value = __name_expression(item, position);
                ast_node_t * second_value = __name_expression(second_name, position);

                return __lambda({ item, second_param },
                    __expression(__create(first_value, second_value, position), position), position);
            }

            // The item of the source made again from the variables: the pair of the pair.
            ast_node_t * __rebuild_item(ast_node_t * position)
            {
                ast_node_t * item = __name_expression(__vars[0], position);

                for (size_t index = 1; index < __vars.size(); index++)
                {
                    item = __create(item, __name_expression(__vars[index], position), position);
                }

                return item;
            }

            // TransparentIdentifier.Create(first, second).
            ast_node_t * __create(ast_node_t * first, ast_node_t * second, ast_node_t * position)
            {
                return __static_call(_T("TransparentIdentifier"), _T("Create"),
                    { __expression(first, position), __expression(second, position) }, position);
            }

            // Type.method(arguments).
            ast_node_t * __static_call(const char_t * type_name, const char_t * method,
                    const std::vector<ast_node_t *> & arguments, ast_node_t * position)
            {
                return __call(nullptr, __name_expression(__to_name(type_name), position), method,
                                                                        arguments, position);
            }

            // The call of a method of the receiver: receiver.method(arguments).
            ast_node_t * __call(__single_t * single, ast_node_t * receiver, const char_t * method,
                    const std::vector<ast_node_t *> & arguments, ast_node_t * position)
            {
                auto * name_node = __new<lang_cs::_function_name_expression_ast_node_t>();
                name_node->append_name(__to_name(method), __token(__cs_token_t::name, position));
                __at(name_node, position);
                name_node->on_commit();

                auto * arguments_node = __new<arguments_ast_node_t>();
                for (ast_node_t * exp : arguments)
                {
                    auto * argument = __new<argument_ast_node_t>();
                    argument->set_child(argument_ast_node_t::__expression__, exp);
                    __at(argument, position);
                    argument->on_commit();

                    arguments_node->append_child(arguments_ast_node_t::__arguments__, argument);
                }

                __at(arguments_node, position);
                arguments_node->on_commit();

                auto * function = __new<function_ast_node_t>();
                function->set_child(function_ast_node_t::__namex__, name_node);
                function->set_child(function_ast_node_t::__arguments__, arguments_node);
                __at(function, position);
                function->on_commit();

                if (single == nullptr)
                    single = __new<__single_t>();

                single->append_expression(receiver);
                single->append_token(__token(__cs_token_t::member_point, position));
                single->append_expression(function);
                __at(single, position);
                single->on_commit();

                return single;
            }
        };

        //-------- ---------- ---------- ---------- ----------

        // Builder of a query: the result is the expression of the calls of the methods.
        class __query_builder_t : public tast_builder_t<__single_t>
        {
            typedef tast_builder_t<__single_t> __super_t;

        public:
            using __super_t::__super_t;

            // Applies token.
            virtual void apply_token(token_t * token, __apply_token_args_t & args) override { }

            // Applies ast node.
            virtual void apply_ast(ast_node_t * node, __apply_ast_args_t & args) override
            {
                __parts.push_back(node);
            }

            // Build completed.
            virtual void completed(__completed_args_t & args) override
            {
                __translator_t(__context).translate(__node, __parts);
            }

        private:
            std::vector<ast_node_t *> __parts;
        };
    }

    ////////// ////////// ////////// ////////// //////////

    // Builds the ast node of a branch of lq.
    ast_node_t * build_lq_ast(ast_context_t & context, lang_ast_build_args_t & args)
    {
        if ((lq_branch_type_t)args.node_value == lq_branch_type_t::lq_query)
            return __query_builder_t(context, args).build();

        return __clause_builder_t(context, args).build();
    }

    ////////// ////////// ////////// ////////// //////////

}   // X_ROOT_NS::modules::lang_lq
