
#include <compile.h>
#include <algorithm.h>
#include "utils.h"
#include <boost/lexical_cast.hpp>


namespace X_ROOT_NS { namespace modules { namespace compile {

    using namespace core;

    typedef analyze_tree_error_t __e_t;

    #define __F(s)                      _E(__e_t::format_error, s)
    #define __FF(format, args...)       __F(_F(format, ##args))

    #define __Helper(lang) lang_service_helper_t(lang)

    #define __EnableEmptyNode   0
    #define __EnableTrace       0

    #if __EnableTrace
    #   define __Trace(arg, args...)  _P(arg, ##args)
    #   define __Tracef(arg, args...) _PF(arg, ##args)
    #else
    #   define __Trace(arg, args...)
    #   define __Tracef(arg, args...)
    #endif

    #define __AlwaysInline  X_ALWAYS_INLINE

    ////////// ////////// ////////// ////////// //////////

    // Analyze node repeat types.
    X_ENUM_INFO(analyze_node_repeat_type_t)

        // Repeat once.
        X_C(once,       _T("once"))

        // Repeats any times.
        X_C(star,       _T("*"))

        // Repeats more than one times.
        X_C(plus,       _T("+"))

        // No repeat or Repeats one time.
        X_C(question,   _T("?"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Analyze tree error codes.
    X_ENUM_INFO(analyze_tree_error_t)

        // Format error.
        X_C(format_error,           _T("format error"))

        // Unknown branch.
        X_C(unknown_branch,         _T("unknown branch"))

        // Unknown token.
        X_C(unknown_token,          _T("unknown token %1%"))

        // Parse error.
        X_C(parse_error,            _T("parse error"))

        // Stack closed.
        X_C(stack_closed,           _T("stack closed"))

        // Unknown element type.
        X_C(unknown_element_type,   _T("unknown element type"))

        // Overflow.
        X_C(overflow,               _T("overflow"))

    X_ENUM_INFO_END

    //-------- ---------- ---------- ---------- ----------

    // Analyze element types
    X_ENUM_INFO(analyzer_element_type_t)

        // A token.
        X_C(token,      _T("token"))

        // A ast node.
        X_C(ast_node,   _T("ast_node"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Returns whether it is an unknown value.
    __AlwaysInline bool __is_unknown(__node_value_t value)
    {
        return value == __unknown_node_value;
    }

    // Returns whether it is an unknow value.
    __AlwaysInline bool __is_unknown(const __node_key_t & key)
    {
        return __is_unknown(key.value);
    }

    // Returns whether it is a normal node.
    __AlwaysInline analyze_normal_node_t * __to_normal(analyze_node_t * node)
    {
        return as<analyze_normal_node_t *>(node);
    }

    // Convert to normal node, returns nullptr when convert failed.
    __AlwaysInline const analyze_normal_node_t * __to_normal(const analyze_node_t * node)
    {
        return as<const analyze_normal_node_t *>(node);
    }

    // Returns whether it is a branch node.
    __AlwaysInline bool __is_branch(const analyze_node_t * node)
    {
        return node->this_type() == __node_type_t::branch;
    }

    // Returns whether it is a branch ref node.
    __AlwaysInline bool __is_branch_ref(const analyze_node_t * node)
    {
        return node->this_type() == __node_type_t::branch_ref;
    }

    // Returns whether it is a root node.
    __AlwaysInline bool __is_root(__node_value_t value)
    {
        return value == (__node_value_t)special_node_value_t::root;
    }

    // Returns whether it is a root node.
    __AlwaysInline bool __is_root(const analyze_node_t * node)
    {
        return __is_root(node->key.value);
    }

    // Returns whether it is an end node.
    __AlwaysInline bool __is_end(const __node_key_t & key)
    {
        return key.type == __node_type_t::end;
    }

    // Returns whether it is an end node.
    __AlwaysInline bool __is_end(const analyze_node_t * node)
    {
        return __is_end(node->key);
    }

    // Returns it's owner branch.
    __AlwaysInline analyze_branch_node_t * __owner_branch(__node_unit_t & unit)
    {
        analyze_node_t * node = *unit;
        _A(__is_end(node));

        return ((analyze_end_node_t *)node)->owner_branch;
    }

    // Returns it's owner branch.
    __AlwaysInline __node_value_t __owner_branch_value(__node_unit_t & unit)
    {
        return __owner_branch(unit)->key.value;
    }

    // Sets merge identity.
    __AlwaysInline void __set_merge_identity(__stack_node_t * s)
    {
        __merge_identity_t id = ((__merge_identity_t)(*s)->current << 3)
                                ^ (__merge_identity_t)(*s)->node;

        __stack_node_t * p = s->parent;
        if(p != nullptr)
            id ^= (((*p)->merge_identity) << 7);

        (*s)->merge_identity = id;
    }

    ////////// ////////// ////////// ////////// //////////

    const unsigned long __StackBufferMaxStep = 256;

    // Stack normal buffer.
    struct __stack_normal_buffer_t
    {
        analyze_node_t * from, * to;
        //int16_t hstep;
    };

    // Stack branch switched buffer.
    struct __stack_branch_switched_buffer_t
    {
        analyze_node_t * from;
        unsigned long step;
        analyze_node_t * to;

        //int16_t affinity;
        //int16_t hstep;
        analyze_node_t ** path;
        analyze_node_t * entrance;
    };

    // Returns whether it is branch switched.
    __AlwaysInline bool __branch_switched(void ** stack)
    {
        return (unsigned long)stack[1] < __StackBufferMaxStep;
    }

    // Returns next stack ndoe.
    __AlwaysInline const analyze_node_t * __next_stack_node(void ** stack)
    {
        return __branch_switched(stack)?
            ((__stack_branch_switched_buffer_t *)stack)->to :
            ((__stack_normal_buffer_t *)stack)->to;
    }

    // Returns stack step.
    __AlwaysInline int16_t __stack_step(void ** stack)
    {
        return __branch_switched(stack)?
            ((__stack_branch_switched_buffer_t *)stack)->step : 0;
    }

    /*
    __AlwaysInline int16_t __stack_affinity(void ** stack)
    {
        return __branch_switched(stack)?
            ((__stack_branch_switched_buffer_t *)stack)->affinity : 0;
    }

    __AlwaysInline __hstep_t __stack_hstep(void ** stack)
    {
        return __branch_switched(stack)?
            ((__stack_branch_switched_buffer_t *)stack)->hstep :
            ((__stack_normal_buffer_t *)stack)->hstep;
    }
    */

    // Returns tack path.
    __AlwaysInline void * __stack_path(void ** stack)
    {
        return __branch_switched(stack)?
            (void *)((__stack_branch_switched_buffer_t *)stack)->path :
            (void *)((__stack_normal_buffer_t *)stack)->to;
    }

    // Returns stack entrance.
    __AlwaysInline const analyze_node_t * __stack_entrance(void ** stack)
    {
        return __branch_switched(stack)?
            ((__stack_branch_switched_buffer_t *)stack)->entrance : nullptr;
    }

    // Returns whether two stack are equals.
    __AlwaysInline bool __stack_equals(void ** stack1, void ** stack2)
    {
        if(stack1[0] != stack2[0] || stack1[1] != stack2[1])
            return false;

        if(__branch_switched(stack1) && __branch_switched(stack2))
        {
            auto b1 = (__stack_branch_switched_buffer_t *)stack1;
            auto b2 = (__stack_branch_switched_buffer_t *)stack2;

            return b1->step == b2->step && b1->to == b2->to;
        }

        return true;
    }

    // Returns the end node.
    __AlwaysInline const analyze_end_node_t * __get_end_node(const analyze_node_t * node)
    {
        const analyze_normal_node_t * normal_node = __to_normal(node);
        if(normal_node == nullptr)
            return nullptr;

        return (const analyze_end_node_t *)normal_node->get_end_unit().node;
    }

    ////////// ////////// ////////// ////////// //////////
    // analyze_nodes_t

    // Appends node.
    void analyze_nodes_t::append(analyze_node_t * node/*, __hstep_t hstep*/)
    {
        append(__node_unit_t(node/*, hstep*/));
    }

    // Appends node.
    void analyze_nodes_t::append(const __node_unit_t & u)
    {
        __last = u;

        auto it = __node_units.find(u);
        if(it != __node_units.end())
        {
            /*
            if((*it).hstep >= u.hstep)
                return;
            */

            __node_units.erase(u);
        }

        __node_units.insert(u);

        if(__is_end(u.node))
        {
            _A(__end == nullptr || __end == u);
            __end = u;
        }
    }

    // Appends nodes.
    void analyze_nodes_t::append(analyze_nodes_t & nodes)
    {
        for(__node_unit_t u : nodes)
        {
            append(u);
        }
    }

    // Removes node unit.
    bool analyze_nodes_t::remove(const __node_unit_t & u)
    {
        if(__node_units.erase(u) > 0)
        {
            if(u == __end)
                __end = nullptr;
            return true;
        }

        return false;
    }

    // Replaces a node collection to another.
    bool analyze_nodes_t::replace(const __node_unit_t & old, const __node_unit_t & new_)
    {
        if(remove(old))
        {
            append(new_);
            return true;
        }

        return false;
    }

    // Returns whether contains the node.
    bool analyze_nodes_t::contains(const __node_unit_t & u)
    {
        if(u == nullptr)
            return false;

        return __node_units.find(u) != __node_units.end();
    }

    // Swap two nodes.
    void analyze_nodes_t::swap(analyze_nodes_t & other)
    {
        std::swap(__node_units, other.__node_units);
        std::swap(__last, other.__last);
        std::swap(__end,  other.__end);
    }

    // Clear nodes.
    void analyze_nodes_t::clear()
    {
        __node_units.clear();
        __last = nullptr;
        __end  = nullptr;
    }

    ////////// ////////// ////////// ////////// //////////
    // analyze_node_t

    // Returns whether two nodes are equals.
    bool analyze_node_t::equals(const analyze_node_t & node) const
    {
        return this == &node;
    }

    // Returns whether a node is less than another.
    bool analyze_node_t::less(const analyze_node_t & node) const
    {
        __node_type_t type1 = node_type(), type2 = node.node_type();
        if(type1 != type2)
            return type1 < type2;

        return (void *)this < (void *)&node;
    }

    // Converts to string.
    const string_t analyze_node_t::to_string() const
    {
        string_t str = this->__to_string();

        if(flag.length() == 0)
            return str;

        return _F(_T("%1%:%2%"), flag, str);
    }

    ////////// ////////// ////////// ////////// //////////
    // analyze_root_node_t

    const __node_key_t analyze_root_node_t::key = __node_key_t(
            analyze_node_type_t::branch, (__node_value_t)special_node_value_t::root);

    ////////// ////////// ////////// ////////// //////////
    // analyze_tree_build_context_t

    // Returns path node from cache.
    analyze_path_node_t * analyze_tree_build_context_t::get_node_from_cache(
                                               const __path_node_cache_key_t & key)
    {
        auto it = __path_node_cache.find(key);
        if(it != __path_node_cache.end())
            return it->second;

        return nullptr;
    }

    // Returns path node from cache.
    analyze_path_node_t * analyze_tree_build_context_t::get_node_from_cache(
                                        const std::vector<analyze_node_t *> & nodes)
    {
        const analyze_node_t * cache_nodes[nodes.size()];
        std::copy(nodes.cbegin(), nodes.cend(), cache_nodes);
        std::sort(cache_nodes, cache_nodes + nodes.size());

        __path_node_cache_key_t cache_key(range_t<const analyze_node_t **>(
            (const analyze_node_t **)cache_nodes,
            (const analyze_node_t **)cache_nodes + nodes.size())
        );

        return get_node_from_cache(cache_key);
    }

    // Adds path node to cache.
    void analyze_tree_build_context_t::add_node_to_cache(analyze_path_node_t * node)
    {
        _A(node != nullptr);
        __path_node_cache[node->to_cache_key()] = node;
    }

    // Adds a branch.
    void analyze_tree_build_context_t::add_branch(analyze_branch_node_t * branch)
    {
        _A(branch != nullptr);
        __branch_map[branch->key.value] = branch;
    }

    // Gets branch of specified node value.
    analyze_branch_node_t * analyze_tree_build_context_t::get_branch(
                                                        __node_value_t value) const
    {
        auto it = __branch_map.find(value);
        if(it != __branch_map.end())
            return it->second;

        return nullptr;
    }

    // Creates new analyze node with subnodes.
    template<typename itor_t>
    analyze_node_t ** analyze_tree_build_context_t::to_stack_path(itor_t begin, itor_t end)
    {
        size_t size = end - begin;
        analyze_node_t * nodes[size];
        std::copy(begin, end, nodes);

        __stack_path_cache_key_t cache_key(range_t<analyze_node_t **>(
            (analyze_node_t **)nodes, (analyze_node_t **)nodes + size
        ));

        auto it = __stack_path_cache.find(cache_key);
        if(it != __stack_path_cache.end())
            return it->second;

        analyze_node_t ** path = new_ptr_array<analyze_node_t>(size);
        std::copy(begin, end, path);

        __stack_path_cache[
            __stack_path_cache_key_t(range_t<analyze_node_t **>(path, path + size))
        ] = path;

        return path;
    }

    // Returns length of a array.
    size_t analyze_tree_build_context_t::__len(void ** arr)
    {
        void ** arr1 = arr;
        while(*arr1++ != nullptr);
        return arr1 - arr;
    }

    ////////// ////////// ////////// ////////// //////////

    // Picks a standalone child.
    static analyze_node_t * __pick_standalone_child(analyze_node_t * owner,
                                                    analyze_nodes_t & nodes)
    {
        analyze_node_t * ret_node = nullptr;
        for(analyze_node_t * node : nodes)
        {
            if(node->index > owner->index && node->node_type() != __node_type_t::end) 
            {
                if(ret_node == nullptr)
                    ret_node = node;
                else if(ret_node != nullptr)
                    return nullptr;
            }
        }

        return ret_node;
    }

    // Returns whether it is a final node.
    static bool __is_final_node(analyze_normal_node_t * normal_node)
    {
        for(analyze_node_t * node : normal_node->nodes)
        {
            if(node->index > normal_node->index && node->node_type() != __node_type_t::end)
                return false;
        }

        return true;
    }

    // Combines repeat types.
    static __repeat_type_t __combine_repeat_type(__repeat_type_t type1, __repeat_type_t type2)
    {
        if(type2 == __repeat_type_t::once)
            return type1;

        switch(type1)
        {
            case __repeat_type_t::once:
                return type2;

            case __repeat_type_t::question:
                if(type2 == __repeat_type_t::plus)  // (A+)? -> (A*)
                    return __repeat_type_t::star;
                break;

            case __repeat_type_t::star:     // (A*)* -> (A*)  (A*)+ -> (A*)  (A*)? -> (A*)
                return __repeat_type_t::star;

            case __repeat_type_t::plus:     // (A+)? -> (A*)  (A+)* -> (A*)
                if(type2 == __repeat_type_t::question || type2 == __repeat_type_t::star)
                    return __repeat_type_t::star;
                break;

            default:
                break;
        }

        return type2;
    }

    // Substract repeat types.
    static __repeat_type_t __substract_repeat_type(__repeat_type_t type1, __repeat_type_t type2)
    {
        if(type2 == __repeat_type_t::once)      // (A)*
            return type1;

        switch(type1)
        {
            case __repeat_type_t::once:         // (A*) -> (A*), (A+) -> (A+), (A?) -> (A?)
                return __repeat_type_t::once;

            case __repeat_type_t::star:

                switch(type2)
                {
                    case __repeat_type_t::star:         // (A*)* -> (A*)
                        return __repeat_type_t::once;

                    case __repeat_type_t::plus:         // (A+)* -> (A+)?
                        return __repeat_type_t::question;

                    case __repeat_type_t::question:     // (A?)* -> (A?)+
                        return __repeat_type_t::plus;

                    default:
                        throw _ECF(unexpected, _T("unexpected repeat type '%1%'"), type2);
                }

                break;

            case __repeat_type_t::plus:

                switch(type2)
                {
                    case __repeat_type_t::star:         // (A*)+ -> (A*)
                        return __repeat_type_t::once;

                    case __repeat_type_t::plus:         // (A+)+ -> (A+)
                        return __repeat_type_t::once;

                    case __repeat_type_t::question:     // (A?)+ -> (A?)+
                        return __repeat_type_t::plus;

                    default:
                        throw _ECF(unexpected, _T("unexpected repeat type '%1%'"), type2);
                }

                break;

            case __repeat_type_t::question:

                switch(type2)
                {
                    case __repeat_type_t::star:         // (A*)? -> (A*)
                        return __repeat_type_t::once;

                    case __repeat_type_t::plus:         // (A+)? -> (A+)?
                        return __repeat_type_t::question;

                    case __repeat_type_t::question:     // (A?)? -> (A?)
                        return __repeat_type_t::once;

                    default:
                        throw _ECF(unexpected, _T("unexpected repeat type '%1%'"), type2);
                }

            default:
                throw _ECF(unexpected, _T("unexpected repeat type '%1%'"), type1);
        }
    }

    // Picks a standalone child.
    static analyze_node_t * __pick_standalone_child(analyze_branch_node_t * branch)
    {
        return __pick_standalone_child(branch, branch->subnodes);
    }

    // Returns a standalone child.
    static analyze_node_t * __get_standalone_child(analyze_branch_node_t * branch)
    {
        analyze_nodes_t & subnodes = branch->subnodes;
        analyze_node_t * standalone_node = __pick_standalone_child(branch);

        analyze_normal_node_t * normal_node = __to_normal(standalone_node);
        if(normal_node != nullptr && __is_final_node(normal_node))
            return normal_node;

        return nullptr;
    }

    // Returns branch repeat type.
    static __repeat_type_t __get_branch_repeat_type(analyze_branch_node_t * branch)
    {
        analyze_node_t * standalone_node = __get_standalone_child(branch);
        if(standalone_node != nullptr)
            return standalone_node->repeat_type;

        return __repeat_type_t::once;
    }

    ////////// ////////// ////////// ////////// //////////

    // Context for exchanging analyze tree.
    struct analyze_tree_exchange_context_t
    {
        analyze_tree_exchange_context_t(analyze_tree_build_context_t * build_context)
            : build_context(build_context) { }

        analyze_tree_build_context_t * build_context;
        std::map<analyze_branch_node_t *, bool> illusory_nodes;
    };

    // Exchanges analyze tree.
    void __exchange(analyze_tree_exchange_context_t & context,
                                        analyze_branch_node_t * branch);

    // Revise repeat branch.
    void __revise_repeat(analyze_branch_node_t * branch);

    // Deal with left cycle.
    void __process_left_cycle(analyze_tree_build_context_t & context,
                                        analyze_branch_node_t * branch);

    //-------- ---------- ---------- ---------- ----------

    // Build analyze tree.
    analyze_root_node_t * analyze_tree_builder_t::build()
    {
        analyze_root_node_t * root = nullptr;
        char_t c;

        __read_branch_context_t ctx;

        while(true)
        {
            if(!skip_whitespace(__p))
                break;

            if(*__p == _T('$'))
            {
                __p++;
                string_t branch_name = __read_name();
                
                char_t next_char = __next_char();
                switch(next_char)
                {
                    case _T('\0'):
                        throw __F(_T("format error: branch name empty"));

                    case _T(':'): {

                        analyze_branch_node_t * branch = __read_branch(ctx, branch_name, 0);

                        if(al::is_empty(branch_name))
                        {
                            if(root != nullptr)
                                throw __F(_T("format error: duplicate root branch $"));

                            root = (analyze_root_node_t *)branch;
                        }

                        __build_context->add_branch(branch);

                    }   break;

                    default:
                        throw __FF(_T("format error: unexpected char '%1%'"), next_char);
                }
            }
            else if(*__p == _T('#'))
            {
                __skip_comment();
            }
            else
            {
                throw __FF(_T("format error: branch name unexpect char \"%1%\""), *__p);
            }
        }

        if(root == nullptr)
            throw __F(_T("no root branch $"));

#if __EnableTrace >= 2
        //__print_branches();
#endif

        for(analyze_branch_ref_node_t * ref_node : ctx.branch_ref_nodes)
        {
            __node_value_t branch_value = ref_node->key.value;
            if(__build_context->get_branch(branch_value) == nullptr)
            {
                if(branch_value < 0)
                    throw __FF(_T("unknown branch \"%1%\""), __branch_name(branch_value));

                ref_node->expandable = false;
            }
        }

        analyze_tree_exchange_context_t exchange_context(__build_context);
        for(auto it : __build_context->all_branches())
        {
            analyze_branch_node_t * branch = it.second;
            __revise_repeat(branch);
        }

        for(auto it : __build_context->all_branches())
        {
            analyze_branch_node_t * branch = it.second;
            __exchange(exchange_context, branch);
        }

        for(auto it : __build_context->all_branches())
        {
            analyze_branch_node_t * branch = it.second;
            __process_left_cycle(*__build_context, branch);
        }

        return root;
    }

    // Split tag.
    template<typename f_t> void __split_tag(const string_t & tag, f_t f)
    {
        char_t s[tag.length() + 1], * p0 = s;
        for(const char_t * p = tag.c_str(); ; p++)
        {
            char_t c = *p;
            switch(c)
            {
                case _T('\0'):
                    *p0 = _T('\0');
                    f(s, p0 - s);
                    return;

                case _T(','):
                    *p0 = _T('\0');
                    f(s, p0 - s);
                    p0 = s;
                    break;

                default:
                    *p0++ = c;
                    break;
            }
        }
    }

    // Analyze tag.
    void __analyze_tag(string_t & tag, string_t * out_flag, __weight_t * out_weight)
    {
        __split_tag(tag, [&out_weight, &out_flag](const char_t * s, size_t len) {

            switch(s[0])
            {
                case _T('!'): {
                    int weight;
                    if(*(s + 1) == _T('\0'))
                    {
                        weight = 1;
                    }
                    else
                    {
                        try
                        {
                            weight = boost::lexical_cast<int>(s + 1, len - 1);
                        }
                        catch(boost::bad_lexical_cast & e)
                        {
                            throw _EC(convert_error, _F(_T("analyze node weight format error:")
                                _T("cannot convert '%1%' to int"), s + 1));
                        }
                    }

                    if(weight >= max_value<__weight_t>() || weight <= min_value<__weight_t>())
                        throw _EC(overflow, _T("analyze node weight overflow"));

                    *out_weight = (__weight_t)weight;
                }   break;

                default:
                    *out_flag = string_t(s);
                    break;
            }
        });
    }

    // Reads token.
    string_t analyze_tree_builder_t::__read_token(string_t & out_tag)
    {
        stringstream_t ss;
        bool flag_assigned = false;

        while(true)
        {
            char_t c = *__p;
            if(c == _T('\\'))
            {
                if(*++__p == _T('\0'))
                    throw _E(__e_t::format_error);
                ss << *__p++;
            }
            else if(c == _T(':'))
            {
                if(flag_assigned)
                    throw __FF(_T("multipy flags of \"%1%\""), ss.str());

                flag_assigned = true;
                out_tag = ss.str();

                if(out_tag.length() == 0)
                    throw __F(_T("empty token flag"));

                if(al::is_whitespace(*++__p))
                    throw __FF(_T("unexpected empty char after flag \"%1%\""), out_tag);

                ss = stringstream_t();
            }
            else if(is_analyze_tree_token_end(c))
            {
                break;
            }
            else
            {
                ss << *__p++;
            }
        }

        string_t s = ss.str();

        if(s.length() == 0)
            throw _E(__e_t::format_error, _T("format error: empty token name"));

        return std::move(s);
    }

    // Reads branch.
    analyze_branch_node_t * analyze_tree_builder_t::__read_branch(
            __read_branch_context_t & ctx, const string_t & branch_name,
            const string_t & current_name, int level, bool inner)
    {
        __skip_whitespace_with_expect();

        analyze_branch_node_t * current;
        bool is_root = al::is_empty(branch_name);
        if(is_root)
        {
            current = __new_node<analyze_root_node_t>();
        }
        else
        {
            current = __new_node<analyze_branch_node_t>(
                current_name.length()? __branch_value(current_name) : 0,
                current_name, level, inner
            );
        }

        analyze_normal_node_t * last = nullptr;

        auto append_node = [&last, &current](analyze_normal_node_t * node) {

            if(last == nullptr)
                current->subnodes.append(node);
            else
                last->nodes.append(node);
            last = node;
            node->branch = current;
        };

        auto append_end = [&last, &current, this]() {

            if(last != nullptr)
            {
                last->nodes.append(__new_node<analyze_end_node_t>(
                    current->subnodes.last(), current
                ));

                last = nullptr;
            }
        };

        bool semicolon = false;

        char_t c;
        while((c = *__p))
        {
            switch(c)
            {
                case _T('('): {
                    __p++;

                    string_t subname = __read_subname();
                    if(subname.length() > 0)
                    {
                        subname = __join_branch_name(branch_name, subname);
                        auto node = __read_branch(ctx, branch_name, subname, level + 1, true);
                        __build_context->add_branch(node);
                        append_node(__new_node<analyze_branch_ref_node_t>(node->key.value, subname));
                    }
                    else
                    {
                        auto node = __read_branch(ctx, branch_name, subname, level + 1, true);
                        append_node(node);
                    }

                } break;

                case _T(';'):
                    if(level != 0)
                        throw __F(__format_error(_T("format error: missing \")\"")));
                    __p++;
                    append_end();
                    semicolon = true;
                    goto label_end;

                case _T(')'):
                    if(level == 0)
                        throw __F(__format_error(_T("format error: unexpect \")\"")));
                    __p++;
                    append_end();
                    goto label_end;

                case _T('|'):
                    append_end();
                    __p++;
                    break;

                case _T('*'):
                case _T('+'):
                case _T('?'):
                    if(last == nullptr)
                        throw __F(__format_error(_T("format error: unexpect char \"%1%\""), c));

                    if(last->repeat_type != analyze_node_repeat_type_t::once)
                        throw __F(__format_error(_T("format error: duplicate repeat type")));

                    typedef analyze_node_repeat_type_t repeat_type_t;
                    last->repeat_type = (c == _T('*'))? repeat_type_t::star
                            : (c == _T('+'))? repeat_type_t::plus : repeat_type_t::question;
                    __p++;
                    break;

                case _T('#'):
                    __skip_comment();
                    break;

                case _T(':'):
                    throw __F(_T("format error: unexpected char ':'"));

                default:
                    if(!al::is_whitespace(c))
                    {
                        string_t tag;
                        string_t name = __read_token(tag);
                        if(name.length() == 0)
                            throw __F(_T("format error: token name empty"));

                        __weight_t weight = 0;
                        string_t flag;
                        __analyze_tag(tag, &flag, &weight);

                        if(name[0] == _T('$'))      // branch ref
                        {
                            name = name.substr(1);
                            if(name.length() == 0)
                                throw __F(_T("format error: branch ref name missing"));

                            string_t subname = __join_branch_name(branch_name, name);
                            __node_value_t value = __branch_value(subname, false);

                            analyze_branch_ref_node_t * node;
                            if(!__is_unknown(value))
                                node = __new_node<analyze_branch_ref_node_t>(value, subname,
                                                                            flag, weight);
                            else
                                node = __new_node<analyze_branch_ref_node_t>(
                                                __branch_value(name), name, flag, weight);

                            ctx.branch_ref_nodes.push_back(node);
                            append_node(node);
                        }
                        else    // token
                        {
                            __node_value_t value = __token_value(name);
                            auto node = __new_node<analyze_token_node_t>(value, name, flag, weight);
                            append_node(node);
                        }
                    }
                    else
                    {
                        __p++;
                    }

                    break;
            }
        }

    label_end:
        if(!semicolon && level == 0)
        {
            throw __F(__format_error(_T("format error: missing ';' in %1%"),
                is_root? string_t(_T("root branch")) : sprintf(_T("branch %1%"),  branch_name)
            ));
        }

        return current;
    }

    // Reads branch.
    analyze_branch_node_t * analyze_tree_builder_t::__read_branch(__read_branch_context_t & ctx,
                                        const string_t & branch_name, int level)
    {
        return __read_branch(ctx, branch_name, branch_name, level);
    }

    // Reads name.
    string_t analyze_tree_builder_t::__read_name()
    {
        const char_t * p = __p;
        read_token(__p);
        return string_t(p, __p);
    }

    // Reads subname.
    string_t analyze_tree_builder_t::__read_subname()
    {
        const char_t * p = __p;

        __skip_whitespace_with_expect();
        if(*__p == _T('$'))
        {
            __p++;
            string_t subname = __read_name();
            if(subname.length() == 0)
                throw __F(__format_error(_T("format error: sub branch name empty")));

            if(__is_expect(_T(':')))
            {
                return std::move(subname);
            }
        }

        __p = p;
        return _T("");
    }

    // Returns whether it is an expected char.
    bool analyze_tree_builder_t::__is_expect(char_t c)
    {
        if(skip_whitespace(__p) && *__p == c)
        {
            __p++;
            return true;
        }

        return false;
    }

    // Returns next char.
    char_t analyze_tree_builder_t::__next_char()
    {
        if(!skip_whitespace(__p))
            return _T('\0');

        return *__p++;
    }

    // Returns an expected char, raises exception if it's unexpected.
    void analyze_tree_builder_t::__expect(char_t c)
    {
        if(!__is_expect(c))
        {
            throw _E(__e_t::format_error,
                sprintf(_T("format error: expect %1%, find %2%"), c, *__p));
        }
    }

    // Skip comment.
    bool analyze_tree_builder_t::__skip_comment()
    {
        while(*++__p)
        {
            if(*__p == _T('\n'))
            {
                __p++;
                return true;
            }
        }

        return false;
    }

    // Skips whitespace with excepted.
    void analyze_tree_builder_t::__skip_whitespace_with_expect()
    {
        if(!skip_whitespace(__p))
            throw _E(__e_t::format_error);
    }

    // Join branch names.
    string_t analyze_tree_builder_t::__join_branch_name(
                                    const string_t & name1, const string_t & name2)
    {
        if(!name1.length())
            return name2;

        if(!name2.length())
            return name1;

        return name1 + _T(".") + name2;
    }

    // Returns branch value.
    __node_value_t analyze_tree_builder_t::__branch_value(const string_t & name, bool auto_create)
    {
        return al::map_get(__branch_node_value_map, name, [&] () {

            if(name.empty())
                return __root_node_value;

            __node_value_t value = __provider->get_node_value(name, __node_type_t::branch);
            if(value < __unknown_node_value)
                throw _E(__e_t::unknown_branch, sprintf(_T("custom branch value must >= 0")));

            if(__is_unknown(value) && auto_create)
                value =  --__next_custome_branch_value;

            __branch_node_name_map[value] = name;

            return value;

        });
    }

    // Print branches.
    void analyze_tree_builder_t::__print_branches()
    {
        struct item_t { string_t name; __node_value_t value; };
        std::vector<item_t> items;

        al::transform(__branch_node_value_map, std::back_inserter(items), [](auto & it) {
            return item_t { it.first, it.second };
        });

        al::sort(items, [](auto & it1, auto & it2) { return it1.value < it2.value; });
        for(auto & it : items)
        {
            if(!__is_unknown(it.value))
                _PF(_T("branch %1%: %2%"), it.value, it.name);
        }
    }

    // Returns token value of name.
    __node_value_t analyze_tree_builder_t::__token_value(const string_t & name)
    {
        return al::map_get(__token_node_value_map, name, [&] () {

            __node_value_t value = __provider->get_node_value(name, __node_type_t::token);
            if(__is_unknown(value))
                throw _EF(__e_t::unknown_token, _title(__e_t::unknown_token), name);

            __token_node_name_map[value] = name;
            return value;

        });
    }

    // Returns branch name of node value.
    const string_t & analyze_tree_builder_t::__branch_name(const __node_value_t & value)
    {
        auto it = __branch_node_name_map.find(value);
        if(it != __branch_node_name_map.end())
            return it->second;

        return empty_str;
    }

    // Returns token name of node value.
    const string_t & analyze_tree_builder_t::__token_name(const __node_value_t & value)
    {
        auto it = __token_node_name_map.find(value);
        if(it != __token_node_name_map.end())
            return it->second;

        return empty_str;
    }

    ////////// ////////// ////////// ////////// //////////

    // Node equals wrapper for compare.
    struct __node_equals_wrapper_t
    {
        // Constructor.
        __node_equals_wrapper_t(const analyze_node_t * node)
            : node(node) { }

        const analyze_node_t * node;

        // Returns whether a wrapper is less than another.
        bool operator < (const __node_equals_wrapper_t & other) const
        {
            return node->less(*other.node);
        }

        // Returns whether two wrappers are equals.
        bool operator == (const __node_equals_wrapper_t & other) const
        {
            return node->equals(*other.node);
        }
    };

    // Wraps a node.
    __AlwaysInline __node_equals_wrapper_t __wrap(const analyze_node_t * node)
    {
        return __node_equals_wrapper_t(node);
    }

    // Compare two nodes.
    __AlwaysInline int __cmp(const analyze_node_t * node1, const analyze_node_t * node2)
    {
        __node_equals_wrapper_t w1 = __wrap(node1), w2 = __wrap(node2);
        return w1 == w2? 0 : w1 < w2? -1 : 1;
    }

    ////////// ////////// ////////// ////////// //////////

    // Analyze tree exchange.
    class analyze_tree_exchange_t
    {
    public:

        // Constructor.
        analyze_tree_exchange_t(analyze_tree_exchange_context_t & context,
                analyze_branch_node_t * branch)
            : __context(context), __build_context(context.build_context)
            , __branch(branch)
        {
            _A(branch != nullptr);
        }

        // Do exchange.
        void exchange()
        {
            __exchange_branch(__branch);

#if __EnableEmptyNode
            __exchange_empty();
#endif
            __merge_branch(__branch);
            //__print(__branch);
        }

    private:
        analyze_tree_exchange_context_t & __context;
        analyze_tree_build_context_t * __build_context;
        analyze_branch_node_t * __branch;

        struct __empty_node_data_t
        {
            analyze_empty_node_t * node;
            std::vector<analyze_normal_node_t *> prenodes;
        };

        std::set<analyze_node_t *> __exchanged_set;
        std::queue<analyze_normal_node_t *> __exchange_queue;
        std::map<analyze_nodes_t *, __empty_node_data_t> __empty_node_map;

        // Returns new empty node.
        analyze_empty_node_t * __new_empty_node()
        {
            return __build_context->new_node<analyze_empty_node_t>();
        }

        // Exchange branch.
        void __exchange_branch(analyze_branch_node_t * branch)
        {
            for(__node_unit_t u : branch->subnodes)
            {
                __join(branch, u);
            }

            while(!__exchange_queue.empty())
            {
                analyze_normal_node_t * normal_node = __exchange_queue.front();
                __exchange_queue.pop();

                __exchange(normal_node);
            }

            branch->subnodes.swap(branch->nodes);
            branch->nodes.clear();
        }

        // Exchange empty.
        void __exchange_empty()
        {
            for(auto & it : __empty_node_map)
            {
                __empty_node_data_t & data = it.second;
                if(data.prenodes.size() <= 1)
                {
                    analyze_normal_node_t * prenode = data.prenodes[0];
                    analyze_nodes_t & nodes = __is_branch(prenode)?
                            ((analyze_branch_node_t *)prenode)->subnodes : prenode->nodes;

                    nodes.append(data.node->nodes);
                    nodes.remove(__node_unit_t(data.node));
                }
            }
        }

        // Exchange normal node.
        void __exchange(analyze_normal_node_t * normal_node)
        {
            analyze_nodes_t nodes;
            normal_node->nodes.swap(nodes);

            for(__node_unit_t u0 : nodes)
            {
                __join(normal_node, u0);
            }

            __repeat_type_t repeat_type = __get_repeat_type(normal_node);
            if(repeat_type == __repeat_type_t::star || repeat_type == __repeat_type_t::plus)
            {
                __join(normal_node, normal_node);
            }
        }

        // Pushes a node unit.
        void __push(__node_unit_t u)
        {
            analyze_normal_node_t * normal_node;
            if(!__exchanged_set.insert(*u).second || !(normal_node = __to_normal(*u)))
                return;

            __exchange_queue.push(normal_node);
        }

        // Joins two nodes.
        void __join(analyze_normal_node_t * prenode, __node_unit_t u/*, __hstep_t hstep = 0*/)
        {
            switch(u->node_type())
            {
                case __node_type_t::branch:
                    __join_subnodes(prenode, ((analyze_branch_node_t *)*u)->subnodes/*, hstep*/);
                    break;

                case __node_type_t::branch_ref:
                case __node_type_t::token:
                case __node_type_t::empty:
                    __join_node(prenode, u/*, hstep*/);
                    __push(u);
                    break;

                case __node_type_t::end: {
                    analyze_branch_node_t * branch = __owner_branch(u);
                    if(branch->branch == nullptr)
                    {
                        __join_node(prenode, *u/*, hstep*/);
                    }
                    else
                    {
                        __join_subnodes(prenode, branch->nodes/*, hstep*/);

                        typedef __repeat_type_t t;
                        __repeat_type_t repeat_type = branch->repeat_type;
                        if(repeat_type == t::star || repeat_type == t::plus)
                        {
                            __join_subnodes(prenode, branch->subnodes/*, hstep*/);
                        }
                    }
                }   break;

                default:
                    _A(false, sprintf(_T("unexpect branch type: %1%"), u->node_type()).c_str());
            }

            analyze_normal_node_t * normal_node;
            if((analyze_node_t *)prenode != *u && (normal_node = __to_normal(*u)))
            {
                switch(__get_repeat_type(normal_node))
                {
                    case __repeat_type_t::question:
                        __join_subnodes(prenode, normal_node->nodes/*, hstep + 1*/);
                        break;

                    case __repeat_type_t::star:
                        __join_subnodes(prenode, normal_node->nodes/*, hstep + 1*/);
                        break;

                    case __repeat_type_t::plus:
                    default:
                        if(__is_illusory(*u))
                            __join_subnodes(prenode, normal_node->nodes/*, hstep + 1*/);
                        break;
                }
            }
        }

        // Returns repeat type.
        __repeat_type_t __get_repeat_type(analyze_node_t * node)
        {
            __repeat_type_t repeat_type = node->repeat_type;
            if(!__is_branch_ref(node))
                return repeat_type;

            analyze_branch_node_t * branch = __build_context->get_branch(
                ((analyze_branch_ref_node_t *)node)->branch_value
            );

            if(branch == nullptr)
                return repeat_type;

            return __substract_repeat_type(repeat_type, branch->repeat_type);
        }

        // Join subnodes.
        void __join_subnodes(analyze_normal_node_t * prenode, analyze_nodes_t & subnodes/*,
                                                                        __hstep_t hstep*/)
        {
            auto is_self = [&subnodes](analyze_node_t * node) {
                analyze_normal_node_t * normal_node = __to_normal(node);
                return normal_node && &normal_node->nodes == &subnodes;
            };

#if __EnableEmptyNode
            if(subnodes.size() - (int)al::any_of(subnodes, is_self) >= 2)
            {
                __empty_node_data_t & data = __empty_node_map[&subnodes];
                if(data.node == nullptr)
                {
                    data.node = __new_empty_node();
                    __join_node(prenode, data.node, 0);
                    data.prenodes.push_back(prenode);
                    prenode = data.node;
                }
                else
                {
                    __join_node(prenode, data.node, 0);
                    data.prenodes.push_back(prenode);
                    return;
                }
            }
#endif

            for(__node_unit_t u : subnodes)
            {
                if(is_self(u.node))
                    continue;

                __join(prenode, u.node/*, hstep + u.hstep*/); 
            }
        }

        // Joins node.
        void __join_node(analyze_normal_node_t * prenode, analyze_node_t * node/*, __hstep_t hstep*/)
        {
            prenode->nodes.append(node/*, hstep*/);
            node->branch = __branch;

            //if(prenode->branch)
            //    node->branch = prenode->branch;
        }

        //-------- ---------- ---------- ---------- ----------

        // Argumenst for determined whether it's illusory.
        struct __is_illusory_args_t
        {
            std::set<analyze_node_t *> walk_set;

            bool record(analyze_node_t * node)
            {
                return walk_set.insert(node).second;
            }
        };

        // Returns whether it's illusory.
        bool __is_illusory(analyze_node_t * node)
        {
            if(al::in(node->repeat_type, __repeat_type_t::star, __repeat_type_t::question))
                return true;

            switch(node->node_type())
            {
                case __node_type_t::branch_ref:
                    return __do_is_illusory((analyze_branch_ref_node_t *)node);

                case __node_type_t::branch:
                    return __do_is_illusory((analyze_branch_node_t *)node);

                default:
                    return false;
            }
        }

        // Returns whether the branch ref is illusory.
        bool __do_is_illusory(analyze_branch_ref_node_t * branch_ref)
        {
            analyze_branch_node_t * branch = __build_context->get_branch(branch_ref->branch_value);
            if(branch == nullptr)
                return false;

            return __do_is_illusory(branch);
        }

        // Returns whether a branch node is illusory.
        bool __do_is_illusory(analyze_branch_node_t * branch)
        {
            std::map<analyze_branch_node_t *, bool> & illusory_nodes = __context.illusory_nodes;
            auto it = illusory_nodes.find(branch);
            if(it != illusory_nodes.end())
                return it->second;

            illusory_nodes[branch] = false;
            __is_illusory_args_t args;
            bool is_illusory = __do_is_illusory(args, branch->subnodes);
            illusory_nodes[branch] = is_illusory;

            return is_illusory;
        }

        // Returns nodes is ilusory.
        bool __do_is_illusory(__is_illusory_args_t & args, analyze_nodes_t & nodes)
        {
            for(analyze_node_t * node : nodes)
            {
                if(node->node_type() == __node_type_t::end)
                    return true;

                if(__is_illusory(node))
                {
                    if(__do_is_illusory(args, node))
                        return true;
                }
            }

            return false;
        }

        // Returns whether node is illusory.
        bool __do_is_illusory(__is_illusory_args_t & args, analyze_node_t * node)
        {
            if(args.record(node))
            {
                analyze_normal_node_t * normal_node = __to_normal(node);
                if(normal_node != nullptr)
                    return __do_is_illusory(args, normal_node->nodes);
            }

            return false;
        }

        //-------- ---------- ---------- ---------- ----------

        typedef std::vector<analyze_node_t *> __analyze_node_vector_t;

        // Arguments for building relation.
        struct __build_relation_args_t
        {
            // Constructor.
            __build_relation_args_t(analyze_branch_node_t * branch)
                : branch(branch) { }

            analyze_branch_node_t * branch;
            std::map<analyze_node_t *, sp_t<__analyze_node_vector_t>> relation_map;
            std::set<analyze_node_t *> walk_set;
            std::map<analyze_node_t *, analyze_node_t *> exchange_map;

            // Records a node.
            bool record(analyze_node_t * node)
            {
                return walk_set.insert(node).second;
            }

            // Adds exchange.
            void add_exchange(analyze_node_t * from, analyze_node_t * to)
            {
                exchange_map[from] = to;
            }
        };

        // Merges branch.
        void __merge_branch(analyze_branch_node_t * branch)
        {
            while(__do_merge_branch(branch))
            {
                __remove_redundance_empty_nodes(branch);
            }
        }

        // Merges branch.
        bool __do_merge_branch(analyze_branch_node_t * branch)
        {
            __build_relation_args_t args(branch);
            __build_relations(args, branch->subnodes);
            __merge_nodes(args);

            return args.exchange_map.size() > 0;
        }

        // Builds relations.
        void __build_relations(__build_relation_args_t & args, analyze_nodes_t & nodes)
        {
            for(analyze_node_t * node : nodes)
            {
                __build_relation(args, node);
            }
        }

        // Builds relations.
        void __build_relation(__build_relation_args_t & args, analyze_node_t * node)
        {
            if(args.record(node))
            {
                analyze_normal_node_t * normal_node = __to_normal(node);
                if(normal_node != nullptr)
                    __build_relation(args, normal_node);
            }
        }

        // Builds relations.
        void __build_relation(__build_relation_args_t & args, analyze_normal_node_t * normal_node)
        {
            for(analyze_node_t * node : normal_node->nodes)
            {
                auto it = args.relation_map.find(node);
                if(it == args.relation_map.end())
                {
                    auto node_vector = new_sp<__analyze_node_vector_t>();
                    args.relation_map[node] = node_vector;
                    node_vector->push_back(normal_node);
                }
                else
                {
                    it->second->push_back(normal_node);
                }

                __build_relation(args, node);
            }
        }

        // Merge nodes.
        void __merge_nodes(__build_relation_args_t & args)
        {
            struct item_t
            {
                analyze_node_t * to;
                __analyze_node_vector_t * froms;
            };

            std::vector<item_t> items;
            al::transform(args.relation_map, std::back_inserter(items), [](auto it) {
                return al::sort(*it.second), item_t { it.first, it.second.get() };
            });

            typedef al::multikey_t<std::vector<analyze_node_t *>> key_t;
            al::sort(items, [](auto it1, auto it2) {
                int r = __cmp(it1.to, it2.to);
                return r != 0? r < 0 : key_t(*it1.froms) < key_t(*it2.froms);
            });

            typedef __node_equals_wrapper_t wrapper_t;
            al::split_array(items,
                [](const item_t & it) { return std::make_tuple(__wrap(it.to), key_t(*it.froms)); },
                [this, &args](auto & key, auto begin, auto end) { __merge(args, begin, end); }
            );

            // Find all exchanged nodes and replace them.
            if(args.exchange_map.size() > 0)
            {
                __build_relation_exchange_args_t exchange_args(args.exchange_map);
                __relation_exchange(exchange_args, args.branch->subnodes);
            }
        }

        // Merges nodes.
        template<typename itor_t>
        void __merge(__build_relation_args_t & args, itor_t begin, itor_t end)
        {
            if(end - begin <= 1)
                return;

            analyze_node_t * new_node = __merge(begin, end);
            for(itor_t it = begin; it != end; it++)
            {
                args.add_exchange(it->to, new_node);
            }
        }

        // Merges nodes.
        template<typename itor_t>
        analyze_node_t * __merge(itor_t begin, itor_t end)
        {
            analyze_node_t * node = begin->to;
            __node_type_t node_type = node->node_type();
            analyze_normal_node_t * normal_node;

            switch(node_type)
            {
                case __node_type_t::token: {
                    auto first = (analyze_token_node_t *)node;
                    normal_node = __build_context->new_node<analyze_token_node_t>(
                        first->value, first->name
                    );
                }   break;

                case __node_type_t::branch_ref: {
                    auto first = (analyze_branch_ref_node_t *)node;
                    normal_node = __build_context->new_node<analyze_branch_ref_node_t>(
                        first->branch_value, first->name
                    );
                }   break;

                default:
                    throw _ECF(not_supported, _T("node type '%1%' not supported"), node_type);
            }

            for(itor_t it = begin; it != end; it++)
            {
                normal_node->nodes.append(((analyze_normal_node_t *)it->to)->nodes);
            }

            return normal_node;
        }

        // Arguments for building relation exchange.
        struct __build_relation_exchange_args_t
        {
            __build_relation_exchange_args_t(
                std::map<analyze_node_t *, analyze_node_t *> & exchange_map)
                : exchange_map(exchange_map) { }

            std::set<analyze_node_t *> walk_set;
            std::map<analyze_node_t *, analyze_node_t *> & exchange_map;

            bool record(analyze_node_t * node)
            {
                return walk_set.insert(node).second;
            }
        };

        // Relation exchange.
        void __relation_exchange(__build_relation_exchange_args_t & args, analyze_node_t * node)
        {
            if(args.record(node))
            {
                analyze_normal_node_t * normal_node = __to_normal(node);
                if(normal_node != nullptr)
                    __relation_exchange(args, normal_node->nodes);
            }
        }

        // Relation exchange.
        void __relation_exchange(__build_relation_exchange_args_t & args, analyze_nodes_t & nodes)
        {
            for(auto it : args.exchange_map)
            {
                analyze_node_t * from = it.first, * to = it.second;
                nodes.replace(from, to);

                for(analyze_node_t * node : nodes)
                {
                    __relation_exchange(args, node);
                }
            }
        }

        //-------- ---------- ---------- ---------- ----------

        // Arguments for remove redundance empty nodes.
        struct __remove_redundance_empty_node_args_t
        {
            std::set<analyze_node_t *> walk_set;

            bool record(analyze_node_t * node)
            {
                return walk_set.insert(node).second;
            }
        };

        // Removes redundance empty nodes.
        void __remove_redundance_empty_nodes(analyze_branch_node_t * branch)
        {
            __remove_redundance_empty_node_args_t args;
            __remove_redundance_empty_nodes(args, branch->subnodes);
        }

        // Removes redundance empty nodes.
        void __remove_redundance_empty_node(__remove_redundance_empty_node_args_t & args,
                                                                    analyze_node_t * node)
        {
            if(args.record(node))
            {
                analyze_normal_node_t * normal_node = __to_normal(node);
                if(normal_node != nullptr)
                    __remove_redundance_empty_nodes(args, normal_node->nodes);
            }
        }

        // Removes redundance empty nodes.
        void __remove_redundance_empty_nodes(__remove_redundance_empty_node_args_t & args,
                                                        analyze_nodes_t & nodes)
        {
            al::svector_t<analyze_empty_node_t *> removing_nodes;
            for(analyze_node_t * node : nodes)
            {
                analyze_empty_node_t * empty_node;
                if(node->node_type() == __node_type_t::empty &&
                        (empty_node = (analyze_empty_node_t *)node)->nodes.size() == 1)
                {
                    removing_nodes.push_back(empty_node);
                }

                __remove_redundance_empty_node(args, node);
            }

            for(analyze_empty_node_t * empty_node : removing_nodes)
            {
                nodes.remove(empty_node);
                nodes.append(*empty_node->nodes.begin());
            }
        }

        //-------- ---------- ---------- ---------- ----------

        // Prints analyze branch.
        void __print(analyze_branch_node_t * branch)
        {
            _P(_T("\nBRANCH:"), _str(branch));
            std::set<analyze_node_t *> printed_set;

            __print_node(branch, branch->subnodes);
            __print(branch, printed_set);
        }

        // Prints analyze node.
        void __print(analyze_node_t * node, std::set<analyze_node_t *> & printed_set)
        {
            analyze_normal_node_t * normal_node;
            if(!printed_set.insert(node).second || !(normal_node = __to_normal(node)))
                return;

            __print_node(normal_node, normal_node->nodes);

            if(node->node_type() == __node_type_t::branch)
            {
                for(analyze_node_t * subnode : ((analyze_branch_node_t *)normal_node)->subnodes)
                {
                    __print(subnode, printed_set);
                }
            }

            for(analyze_node_t * subnode : normal_node->nodes)
            {
                __print(subnode, printed_set);
            }
        }

        // Prints analyze node.
        void __print_node(analyze_node_t * node, analyze_nodes_t & nodes)
        {
            for(__node_unit_t u : nodes)
            {
                _P(sprintf(_T("%1% => %2%(%3%)"), _str(node), _str(*u)/*, u.hstep*/));
            }
        }
    };

    // Exchange branch node.
    void __exchange(analyze_tree_exchange_context_t & context, analyze_branch_node_t * branch)
    {
        analyze_tree_exchange_t(context, branch).exchange();
    }

    // Revise repeat types.
    void __revise_repeat_type(analyze_branch_node_t * branch)
    {
        analyze_node_t * standalone_child = __get_standalone_child(branch);
        if(standalone_child != nullptr)
        {
            standalone_child->repeat_type = __combine_repeat_type(branch->repeat_type,
                                            standalone_child->repeat_type);
            branch->repeat_type = __repeat_type_t::once;
        }

        for(analyze_node_t * subnode : branch->subnodes)
        {
            if(__is_branch(subnode))
            {
                __revise_repeat_type((analyze_branch_node_t *)subnode);
            }
        }

        for(analyze_node_t * node : branch->nodes)
        {
            if(__is_branch(node))
            {
                __revise_repeat_type((analyze_branch_node_t *)node);
            }
        }
    }

    // Revise repeat branch nodes.
    void __revise_repeat(analyze_branch_node_t * branch)
    {
        __revise_repeat_type(branch);

        analyze_node_t * standalone_child = branch;
        analyze_node_t * node;

        while(__is_branch(standalone_child) && 
            (node = __get_standalone_child((analyze_branch_node_t *)standalone_child)) != nullptr)
        {
            standalone_child = node;
        }

        branch->repeat_type = standalone_child->repeat_type;
    }

    ////////// ////////// ////////// ////////// //////////

    // Revise left cyclies.
    class __left_cycle_t
    {
    public:

        // Constructor.
        __left_cycle_t(analyze_tree_build_context_t & context, analyze_branch_node_t * branch)
            : __context(context), __branch(branch)
        { }

        // Execute it.
        void execute()
        {
            __execute_left_cycle_process(__branch->subnodes);
        }

    private:
        analyze_tree_build_context_t & __context;
        analyze_branch_node_t * __branch;

        struct __ctx_t
        {
            std::set<analyze_branch_node_t *> __nodes;

            bool record_branch(analyze_branch_node_t * branch)
            {
                return __nodes.insert(branch).second;
            }
        };

        // Do execute left cycle process.
        void __execute_left_cycle_process(analyze_nodes_t & nodes)
        {
            for(analyze_node_t * node : nodes)
            {
                if(__is_branch_ref(node))
                {
                    analyze_branch_ref_node_t * ref_node = (analyze_branch_ref_node_t *)node;
                    ref_node->is_left_cycle = __exists_left_cycle(ref_node);

                    analyze_branch_node_t * ref_branch = __get_branch(ref_node->branch_value);
                    if(ref_branch != nullptr)
                    {
                        ref_node->expandable = !ref_node->is_left_cycle || 
                            (ref_node->index > ref_branch->index && __branch != ref_branch
                            /*&& !__branch->inner*/);

                        ref_node->is_dead_cycle = ref_node->is_left_cycle &&
                                                   __is_dead_cycle(ref_node);
                    }
                    else
                    {
                        ref_node->expandable = false;
                    }

                    /*
                    if(ref_node->flag[0] == _T('A'))
                    {
                        _PF("----------- %1%: %2% %3%",
                            _str(ref_node), ref_node->expandable, ref_node->is_dead_cycle
                        );
                    }*/
                }
            }
        }

        // Returns whether it is has left cycles.
        bool __exists_left_cycle(analyze_branch_ref_node_t * ref_node)
        {
            __ctx_t ctx;
            return __exists_left_cycle(ctx, ref_node);
        }

        // Returns whether it is has left cycles.
        bool __exists_left_cycle(__ctx_t & ctx, analyze_branch_ref_node_t * ref_node)
        {
            analyze_branch_node_t * ref_branch = __get_branch(ref_node->branch_value);
            if(ref_branch == nullptr)
                return false;

            if(__branch == ref_branch)
                return true;

            if(!ctx.record_branch(ref_branch))
                return false;

            for(analyze_node_t * node : ref_branch->subnodes)
            {
                if(__is_branch_ref(node) &&
                    __exists_left_cycle(ctx, (analyze_branch_ref_node_t *)node))
                {
                    return true;
                }
            }

            return false;
        }

        // Returns whether it is dead cycle.
        bool __is_dead_cycle(analyze_branch_ref_node_t * ref_node)
        {
            __ctx_t ctx;
            return __is_dead_cycle(ctx, ref_node);
        }

        // Returns whether it is dead cycle.
        bool __is_dead_cycle(__ctx_t & ctx, analyze_branch_ref_node_t * ref_node)
        {
            analyze_branch_node_t * ref_branch = __get_branch(ref_node->branch_value);

            if(ref_branch == nullptr)
                return false;

            if(__branch == ref_branch)
                return __is_final_node(ref_node);

            if(!ctx.record_branch(ref_branch))
                return false;

            for(analyze_node_t * node : ref_branch->subnodes)
            {
                if(__is_branch_ref(node) && 
                    __is_dead_cycle(ctx, (analyze_branch_ref_node_t *)node))
                {
                    return true;
                }
            }

            return false;
        }

        // Returns analyze branch for specified node value.
        analyze_branch_node_t * __get_branch(__node_value_t value)
        {
            return __context.get_branch(value);
        }
    };

    // Process left cycles.
    void __process_left_cycle(analyze_tree_build_context_t & context, analyze_branch_node_t * branch)
    {
        __left_cycle_t(context, branch).execute();
    }

    ////////// ////////// ////////// ////////// //////////

    // Parses analyze tree.
    analyze_tree_t * parse_analyze_tree(const char_t * pattern, memory_t * memory,
            analyze_tree_builder_provider_t * provider)
    {
        _A(pattern != nullptr);

        analyze_tree_build_context_t * build_context
                = memory_t::new_obj<analyze_tree_build_context_t>(memory, memory);

        analyze_tree_builder_t builder(build_context, pattern, provider);
        analyze_root_node_t * root = builder.build();

        std::vector<analyze_node_t *> nodes({ root });
        analyze_normal_path_node_t * path_node = build_context->
                new_path_node<analyze_normal_path_node_t>(nodes,  _T("<root>"));

        analyze_tree_t * tree = memory_t::new_obj<analyze_tree_t>(memory, root, path_node);

        return tree;
    }

    ////////// ////////// ////////// ////////// //////////
    // analyze_path_node_stack_t

    // Returns nodes of specified stack type.
    __stack_range_t analyze_path_node_stack_t::all(__stack_type_t type) const
    {
        __stack_unit_t * units = (__stack_unit_t *)this->units;
        switch(type)
        {
            case __stack_type_t::normal:
                return __stack_range_t(units, units + normal_count);

            case __stack_type_t::branch_switched:
                return __stack_range_t(units + normal_count, units + count);

            case __stack_type_t::all:
                return __stack_range_t(units, units + count);

            default:
                return __stack_range_t(units, units);
        }
    }

    // Creates a new analyze path node stack.
    analyze_path_node_stack_t * analyze_path_node_stack_t::new_stack(
            analyze_tree_build_context_t * context,
            const analyze_node_t * head, __node_count_t count)
    {
        size_t size = sizeof(analyze_path_node_stack_t) - sizeof(units)
                + count * sizeof(units[0]);

        return memory_t::new_obj<analyze_path_node_stack_t>(size, context->memory, head, count);
    }

    // Assigns path node.
    template<typename itor_t> void analyze_path_node_stack_t::assign(
                    analyze_tree_build_context_t * context, itor_t begin, itor_t end)
    {
        std::transform(begin, end, units, [](void ** stack) {
            return __stack_unit_t (
                __next_stack_node(stack), __stack_step(stack), /*__stack_affinity(stack),*/
                /*__stack_hstep(stack),*/ __stack_path(stack), __stack_entrance(stack)
            );
        });
    }

    // Returns a new stack.
    template<typename itor_t>
    analyze_path_node_stack_t * analyze_path_node_stack_t::new_stack(
            analyze_tree_build_context_t * context,
            const analyze_node_t * head, itor_t begin, itor_t end)
    {
        size_t count = 0, normal_count = 0;
        for(auto it = begin; it != end; it++)
        {
            if(__stack_step(*it) == 0)
                normal_count++;
            count++;
        }

        __path_node_stack_t * node_stack = __path_node_stack_t::new_stack(context, head, count);

        node_stack->assign(context, begin, end);
        node_stack->normal_count = normal_count;

        return node_stack;
    }

    ////////// ////////// ////////// ////////// //////////
    // analyze_path_node_stacks_t

    // When build completed.
    void analyze_path_node_stacks_t::on_build_complete(analyze_tree_build_context_t * build_context,
                                                                                size_t count)
    {
        //_P(_T("on_build_complete, count ="), count);
    }

    // Returns path node stack of specified head.
    __AlwaysInline const __path_node_stack_t * analyze_path_node_stacks_t::stack_of(
                                                    const analyze_node_t * head) const
    {
        for(const __path_node_stack_t * stack = this->stack; stack; stack = stack->next)
        {
            if(stack->head == head)
            {
                return stack;
            }
        }

        return nullptr;
    }

    ////////// ////////// ////////// ////////// //////////
    // analyze_path_node_t

    size_t analyze_path_node_t::__counter = 0;

    // Returns stacks of specified path node.
    __AlwaysInline const __path_node_stacks_t * analyze_path_node_t::stacks_of(
                                            const analyze_path_node_t * src) const
    {
        if(__stacks_count != __use_stacks_map)
        {
            for(__path_node_stacks_t * stacks = __stacks; stacks; stacks = stacks->next)
            {
                if((void *)stacks->src == (void *)src)
                {
                    return stacks;
                }
            }
        }
        else
        {
            auto it = __stacks_map->find(src);
            if(it != __stacks_map->end())
                return it->second;
        }

        return nullptr;
    }

    // Prints stack.
    static void __print_stack(const analyze_node_t * const * stack)
    {
        std::wcout << (*stack)->to_string() 
                << _T("(") << (void *)*(stack) << _T(")")
                << _T(" -> ");

        if(*(stack + 1) == nullptr)
        {
            std::wcout << _T("... -> ") << (*(stack + 2))->to_string()
                << _T("(") << (void *)*(stack + 2) << _T(")");
        }
        else
        {
            std::wcout << (*(stack + 1))->to_string()
                << _T("(") << (void *)*(stack + 1) << _T(")");
        }
    }

    // Appends stacks.
    void analyze_path_node_t::append_stacks(analyze_path_node_t * src, std::vector<void **> & stacks)
    {
        append_stacks(__build_stacks(src, stacks));
    }

    // Appends stacks.
    void analyze_path_node_t::append_stacks(__path_node_stacks_t * stacks)
    {
        if(__stacks_count != __use_stacks_map)
        {
            if(++__stacks_count > __stacks_list_max_count)
            {
                __stacks_count = __use_stacks_map;

                typedef obj_wrap_t<__stacks_map_t> __x_stacks_map_t;
                __stacks_map_t * m = __build_context->new_obj<__x_stacks_map_t>();

                for(__path_node_stacks_t * s = __stacks; s; s = s->next)
                {
                    (*m)[s->src] = s;
                }

                (*m)[stacks->src] = stacks;
                __stacks_map = m;
            }
            else
            {
                stacks->next = __stacks;
                __stacks = stacks;
            }
        }
        else
        {
            (*__stacks_map)[stacks->src] = stacks;
        }
    }

    // Build stacks.
    __path_node_stacks_t * analyze_path_node_t::__build_stacks(analyze_path_node_t * src,
            std::vector<void **> & stacks)
    {
        _A(stacks.size() > 0);

        al::sort(stacks, [](void ** n1, void ** n2) {
            return al::cmp(n1[0], n2[0], n1[1], n2[1]) > 0;
        });

        __path_node_stacks_t * node_stacks = __build_context->new_obj<__path_node_stacks_t>(src);
        __path_node_stack_t ** p_node_stack = &node_stacks->stack;
        size_t count = 0;

        typedef __stack_nodes_itor_t itor_t;

        al::split_array(stacks.begin(), stacks.end(),
            [](void ** stack) { return *stack; },
            [this, &p_node_stack, &count](void * head, itor_t begin, itor_t end) {
                *p_node_stack = this->__build_stack((analyze_node_t *)head, begin, end);
                p_node_stack = &(*p_node_stack)->next;
                count++;
            }
        );

        node_stacks->on_build_complete(__build_context, count);
        return node_stacks;
    }

    // Build stacks.
    __path_node_stack_t * analyze_path_node_t::__build_stack(analyze_node_t * head,
                                __stack_nodes_itor_t begin, __stack_nodes_itor_t end)
    {
        return __path_node_stack_t::new_stack(__build_context, head, begin, end);
    }

    ////////// ////////// ////////// ////////// //////////
    // analyze_normal_path_node_t

    // Enters a specified key, returns the path node.
    analyze_path_node_t * analyze_normal_path_node_t::enter(const __node_key_t & key)
    {
        __ensure_init();

        auto it = __subnode_map.find(key);
        if(it != __subnode_map.end())
        {
            return it->second;
        }

        return nullptr;
    }

    // Returns a string of descripts sub nodes.
    string_t analyze_normal_path_node_t::all_subnodes_string()
    {
        __ensure_init();

        stringstream_t ss;

        int index = 0;
        for(auto it : __subnode_map)
        {
            if(index++ > 0)
                ss << _T(", ");

            ss << (string_t)(it.first);
        }

        return ss.str();
    }

    // Appends subnodes.
    void analyze_normal_path_node_t::__append_subnodes(__append_ctx_t & ctx,
                                analyze_nodes_t & nodes, analyze_node_t * parent)
    {
        for(__node_unit_t u : nodes)
        {
            if(u->key == __empty_node_key)
            {
                __append_subnodes(ctx, ((analyze_empty_node_t *)*u)->nodes);
            }
            else
            {
                __build_node_info_t & info = ctx.nodes_map[u->key];
                info.nodes.insert(*u);
                __append_current_stack(ctx, info, u);

                __node_type_t node_type = u->this_type();
                if(node_type == __node_type_t::branch_ref)
                {
                    analyze_branch_ref_node_t * ref_node = (analyze_branch_ref_node_t *)*u;
                    if(ref_node->expandable)
                    {
                        analyze_branch_node_t * branch = __get_branch_node(ref_node->branch_value);
                        __append_branch_subnodes(ctx, u, branch);
                    }
                }
                else if(node_type == __node_type_t::branch)
                {
                    analyze_branch_node_t * branch = (analyze_branch_node_t *)*u;
                    __append_branch_subnodes(ctx, u, branch);
                }
            }
        }
    }

    // Appends branch subnodes.
    void analyze_normal_path_node_t::__append_branch_subnodes(__append_ctx_t & ctx,
                                        __node_unit_t u, analyze_branch_node_t * branch)
    {
        ctx.branch_stack_push(branch, u);
        __append_subnodes(ctx, branch->subnodes, branch);
        ctx.branch_stack_pop();
    }

    // Appends current stack.
    bool analyze_normal_path_node_t::__append_current_stack(__append_ctx_t & ctx,
                                    __build_node_info_t & info, __node_unit_t new_top)
    {
        ctx.branch_stack_push(new_top);
        bool success = info.append_stack(ctx.current_branch_stack(__build_context));
        ctx.branch_stack_pop();

        return success;
    }

    // Append subnodes.
    void analyze_normal_path_node_t::__append_subnodes(__append_ctx_t & ctx, __node_unit_t u)
    {
        analyze_normal_node_t * normal_node = __to_normal(*u);
        if(normal_node != nullptr)
        {
            ctx.branch_stack_push(normal_node);
            __append_subnodes(ctx, __is_branch(normal_node)?
                ((analyze_branch_node_t *)normal_node)->subnodes : normal_node->nodes
            );
            ctx.branch_stack_pop();
        }
    }

    // Gets branch nodes.
    analyze_branch_node_t * analyze_normal_path_node_t::__get_branch_node(__node_value_t value)
    {
        analyze_branch_node_t * branch = __build_context->get_branch(value);
        if(branch == nullptr)
            throw _E(__e_t::format_error, sprintf(_T("branch node \"%1%\" not exists"), value));

        return branch;
    }

    // Initialize.
    void analyze_normal_path_node_t::__initialize()
    {
        __append_ctx_t ctx;

        for(analyze_node_t * node : nodes())
        {
            __append_subnodes(ctx, node);
        }

        for(auto & it : ctx.nodes_map)
        {
            __node_key_t key = it.first;
            __build_node_info_t & info = it.second;
            std::set<analyze_node_t *> & node_set = info.nodes;

            __nodes_t nodes(node_set.begin(), node_set.end());
            std::sort(nodes.begin(), nodes.end());
            analyze_path_node_t * subnode = __build_context->get_node_from_cache(nodes);

            if(subnode == nullptr)
            {
                if(key == __node_type_t::end)
                {
                    subnode = __build_context->new_path_node<analyze_end_path_node_t>(nodes);
                }
                else
                {
                    subnode = __build_context->new_path_node<analyze_normal_path_node_t>(
                                                                nodes, nodes[0]->to_string());
                }

                subnode->append_stacks(this, info.stacks);
                __build_context->add_node_to_cache(subnode);
            }
            else
            {
                X_LOCK_THIS;
                subnode->append_stacks(this, info.stacks);
            }

            __subnode_map[key] = subnode;
        }
    }

    // Prints this normal path node.
    void analyze_normal_path_node_t::__print()
    {
        for(auto it : __subnode_map)
        {
            _P(_T("  ->"), (string_t)(it.first), it.second->to_string());
        }
    }

    // Appends stack.
    bool analyze_normal_path_node_t::__build_node_info_t::append_stack(void ** stack)
    {
        if(!__exist_stack(stack))
        {
            stacks.push_back(stack);
            return true;
        }

        return false;
    }

    // Returns whether the stack is exists.
    bool analyze_normal_path_node_t::__build_node_info_t::__exist_stack(void ** stack)
    {
        for(void ** stack0 : stacks)
        {
            if(__stack_equals(stack0, stack))
            {
                return true;
            }
        }

        return false;
    }

    // Append current branch stack.
    void ** analyze_normal_path_node_t::__append_ctx_t::current_branch_stack(
                                                analyze_tree_build_context_t * context)
    {
        if(branch_stack.empty())
            return nullptr;

        size_t size = branch_stack.size();
        _A(size >= 2);

        void ** buffer;

        #define __BufferSize(buffer_t) _alignf(sizeof(buffer_t), sizeof(void *))

        if(size == 2)
        {
            buffer = __analyze_node_heap.acquire(__BufferSize(__stack_normal_buffer_t));
            __stack_normal_buffer_t & s = *(__stack_normal_buffer_t *)buffer;
            s.from  = *branch_stack[0];
            s.to    = *branch_stack[1];
            //s.hstep = branch_stack[1].hstep;
        }
        else
        {
            buffer = __analyze_node_heap.acquire(__BufferSize(__stack_branch_switched_buffer_t));
            __stack_branch_switched_buffer_t & s = *(__stack_branch_switched_buffer_t *)buffer;
            s.from  = *branch_stack[0];
            s.step  = (unsigned long)(size - 2);
            s.to    = *branch_stack[size - 1];
            s.path  = context->to_stack_path(branch_stack.begin() + 1, branch_stack.end());
            s.entrance = this->entrance;

            /*
            s.affinity = std::count_if(
                branch_stack.begin() + 1, branch_stack.begin() + size - 1,
                [](const __node_unit_t & u) { return ((analyze_branch_node_t *)*u)->inner; }
            );

            s.hstep = al::sum(branch_stack.begin() + 1, branch_stack.end(),
                [](const __node_unit_t & u) { return u.hstep; }
            );
            */
        }

        #undef __BufferSize

        return buffer;
    }

    // Pushes a node unit.
    void analyze_normal_path_node_t::__append_ctx_t::branch_stack_push(
                                    __node_unit_t u, __node_unit_t entrance)
    {
        if(entrance != nullptr && this->entrance == nullptr)
            this->entrance = entrance;

        branch_stack.push_back(u);
    }

    // Returns current branch stack string.
    string_t analyze_normal_path_node_t::__append_ctx_t::current_branch_stack_string()
    {
        return al::join_str(branch_stack.begin(), branch_stack.end(), _T(" -> "), 
            [](analyze_node_t * node) { return node->to_string(); });
    }

    // Pops a node unit.
    void analyze_normal_path_node_t::__append_ctx_t::branch_stack_pop()
    {
        branch_stack.pop_back();
        if(branch_stack.size() <= 1)
            this->entrance = nullptr;
    }

    // Prints stack.
    void analyze_normal_path_node_t::__append_ctx_t::print_stack() const
    {
        for(auto it_begin = branch_stack.begin(), it = it_begin;
                                            it != branch_stack.end(); it++)
        {
            if(it != it_begin)
                print(_T(" -> "));
            print((*it)->to_string());
        }

        _P();
    }

    ////////// ////////// ////////// ////////// //////////

    // Callback action type.
    X_ENUM_INFO(analyze_callback_action_t)

        // Branch matched.
        X_C(branch_matched)

        // Analyze end.
        X_C(analyze_end)

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////
    // __stack_node_value_t

    // Converts an action to string.
    static string_t __action_to_string(__stack_node_action_t * action)
    {
        stringstream_t ss;
        int index = 0;

        while(action != nullptr)
        {
            if(index++ > 0)
                ss << _T("~");

            if(action->action_type() == __stack_node_action_type_t::raise_matched_event)
            {
                auto * a = (__stack_node_raise_matched_event_action_t *)action;
                ss << a->get_value();
            }
            else if(action->action_type() == __stack_node_action_type_t::assign_key)
            {
                auto * a = (__stack_node_assign_key_action_t *)action;
                ss << _T("[") << a->get_value() << _T("]");
            }

            action = action->next;
        }

        return ss.str();
    }

    // Converts stack node value to string.
    __stack_node_value_t::operator string_t() const
    {
        //return _str(node);
        if(node == nullptr)
            return _T("<NULL>");

        return _F(_T("%1% %2%"), _str(node), node->weight);
        //return _F(_T("%1%(%2%)"), _str(node), (void *)node);
        //return _F("%1% %2% %3%", _str(node), node? node->index : 0, step);
        //return _F("%1% %2% %3%", _str(node), holes, affinity);

        /*
        string_t action_str = __action_to_string(this->action);

        if(action_str.length() > 0)
            return sprintf(_T("%1%:%2%"), _str(node), action_str);

        return sprintf(_T("%1%"), _str(node));*/
    }

    ////////// ////////// ////////// ////////// //////////
    // __stack_node_action_type_t

    // Stack node action types.
    X_ENUM_INFO(__stack_node_action_type_t)

        // Assign key.
        X_C(assign_key)

        // Raise matched event.
        X_C(raise_matched_event)

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Execute the matched event action.
    void __stack_node_raise_matched_event_action_t::execute()
    {
        __matched_item_t it(__branch_ref, __begin_tag);
        __context.__do_branch_matched_callback(it, __end_tag);
    }

    // Returns next action index.
    __AlwaysInline static int __next_action_index(__stack_node_action_t * action)
    {
        while(action != nullptr)
        {
            if(action->action_type() == __stack_node_action_type_t::raise_matched_event)
            {
                return ((__stack_node_raise_matched_event_action_t *)action)->action_index() + 1;
            }

            action = action->next;
        }

        return 1;
    }

    // Returns action index.
    __AlwaysInline int __stack_node_raise_matched_event_action_t::action_index() const
    {
        __self_t * action = const_cast<__self_t *>(this);

        if(action->__action_index < 0)
        {
            action->__action_index = __next_action_index(next);
        }

        return action->__action_index;
    }

    // Returns action back track.
    __AlwaysInline static int __action_back_track(
                        __stack_node_raise_matched_event_action_t * action)
    {
        __stack_node_action_t * next = action->next;
        while(next != nullptr)
        {
            typedef __stack_node_raise_matched_event_action_t action_t;
            
            if(next->action_type() == __stack_node_action_type_t::raise_matched_event)
            {
                int begin_index1 = action->begin_index();
                int begin_index2 = ((action_t *)next)->begin_index();

                if(begin_index1 < begin_index2)
                    return ((action_t *)next)->back_track() + (begin_index2 - begin_index1);

                return ((action_t *)next)->back_track();
            }

            next = next->next;
        }

        return 0;
    }

    // Returns back track.
    __AlwaysInline int __stack_node_raise_matched_event_action_t::back_track() const
    {
        __self_t * action = const_cast<__self_t *>(this);

        if(action->__back_track < 0)
        {
            action->__back_track = __action_back_track(action);
        }

        return action->__back_track;
    }

    ////////// ////////// ////////// ////////// //////////
    // analyze_context_t

    // Constructor.
    analyze_context_t::analyze_context_t(lang_t * lang, analyze_tree_t * tree,
                                         analyze_callback_t * callback)
        : __lang(lang), __callback(callback), __tree(tree)
        , __sn_heap(_T("sn_heap")), __leafs(&__sn_heap)
        , __service_helper(lang), __token_property_cache(&__service_helper)
        , __raise_matched_event_action_factory(&__sn_heap)
    {
        _A(lang != nullptr);
        _A(tree != nullptr);
        _A(callback != nullptr);

        __reset();
    }

    // Resets
    void analyze_context_t::__reset()
    {
        __stack_root = al::new_lr_tree_root<__stack_node_value_t>(&__sn_heap,
            __stack_node_value_t()
        );

        __set_merge_identity(__stack_root);
        
        __stack_node_t * current_node = __stack_root->append_child(&__sn_heap,
            __stack_node_value_t(__tree->root, nullptr, __tree->path_root/*, 0*/)
        );

        __set_merge_identity(current_node);

        __leafs.clear();
        __leafs.push_back(current_node);
    }

    // Matches the next token.
    void analyze_context_t::go(const __node_key_t * keys, __tag_t * tag, __node_value_t * out_value)
    {
        if(__is_invisible(keys[0]) && keys[1] == __empty_node_key)
            return;

        if(keys[0] != __end_node_key) // Normal node: stack grow
        {
            __stack_push_args_t args(this, keys, tag, out_value);
            __leafs.walk([this, &args](__stack_node_t * stack_node) {
                return __stack_push(args, stack_node), false;
            });

            __append_leafs(args.new_stack_nodes);

#if __EnableTrace >= 2
            std::wcout << _T("TREE  1: ") << al::_detail(__stack_root) << std::endl;
            std::wcout << _T("LEAFS 1: ") << __leafs << std::endl;
#endif

            __merge_leafs();

            if(__leafs.size() == 0)
                throw _EF(__e_t::format_error, _T("format error"));

#if __EnableTrace >= 2
            std::wcout << _T("TREE  2: ") << al::_detail(__stack_root) << std::endl;
            std::wcout << _T("LEAFS 2: ") << __leafs << std::endl;
#endif

            if(__leafs.size() == 1)
                __execute_actions();
        }
        else  // End node: close stack
        {
#if __EnableTrace >= 2
            std::wcout << _T("TREE  3: ") << al::_detail(__stack_root) << std::endl;
            std::wcout << _T("LEAFS 3: ") << __leafs << std::endl;
#endif

            __stack_pop_args_t args(tag);
            __leafs.walk([this, &args](__stack_node_t * stack_node) {
                return __stack_pop(args, stack_node), false;
            });


            __append_leafs(args.new_stack_nodes);

#if __EnableTrace >= 2
            std::wcout << _T("TREE  4: ") << al::_detail(__stack_root) << std::endl;
            std::wcout << _T("LEAFS 4: ") << __leafs << std::endl;
#endif

            __pick_final_leafs();

#if __EnableTrace >= 2
            std::wcout << _T("TREE  5: ") << al::_detail(__stack_root) << std::endl;
            std::wcout << _T("LEAFS 5: ") << __leafs << std::endl;
#endif

            if(__leafs.size() >= 2)
                throw _EF(__e_t::format_error, _T("conflict"));

            if(__leafs.size() > 0)
                __execute_actions();

            __do_branch_matched_callback(__matched_item_t(__tree->root, nullptr), nullptr);
            __do_end_callback();
        }
    }

    /*
    template<typename stack_nodes_t>
    __AlwaysInline static void __increase(stack_nodes_t & nodes, __holes_t holes)
    {
        if(holes > 0)
        {
            al::for_each(nodes, [holes](__stack_node_t * n) {
                (*n)->holes += holes;
            });
        }
    }
    */

    // When pushes a node to stack.
    __AlwaysInline void analyze_context_t::__stack_push(__stack_push_args_t & args,
                                                        __stack_node_t * stack_node)
    {
        __stack_push_task_t task(args);
        __stack_grow_args_t grow_args(task);
        __stack_grow(grow_args, stack_node);

        // check end nodes
        auto current_node = (analyze_normal_node_t *)(*stack_node)->node;
        __node_unit_t end_unit = current_node->get_end_unit();
        if(end_unit != nullptr)
        {
            __check_end_node_args_t args(task);

            // check end nodes
            __check_end_node(args, stack_node->parent, stack_node,
                end_unit, (*stack_node)->action, (*stack_node)->weight
                                        /*, 0, (*stack_node)->affinity*/
            );

            //__increase(args, end_unit.hstep);
        }

        if(!grow_args.found)
            __remove_leaf(stack_node);
    }

    // Join actions.
    __AlwaysInline static __stack_node_action_t * __join_actions(__stack_node_action_t * action1,
                                                  __stack_node_action_t * action2)
    {
        if(action1 == nullptr)
            return action2;

        if(action2 == nullptr)
            return action1;

        __stack_node_action_t ** p_action = &action1;
        while(*p_action != nullptr)
        {
            p_action = &(*p_action)->next;
        }

        *p_action = action2;

        return action1;
    }

    // Returns next begin index.
    __AlwaysInline static int __next_begin_index(__stack_node_action_t * action)
    {
        while(action != nullptr)
        {
            if(action->action_type() == __stack_node_action_type_t::raise_matched_event)
            {
                return ((__stack_node_raise_matched_event_action_t *)action)->begin_index();
            }

            action = action->next;
        }

        return 0;
    }

    // Checks the node node.
    __AlwaysInline void analyze_context_t::__check_end_node(__check_end_node_args_t & args,
        __stack_node_t * parent, __stack_node_t * current, __node_unit_t end_unit,
        __stack_node_action_t * next_action, int weight /*, __hstep_t hstep, __affinity_t affinity*/)
    {
        //_PF("__check_end_node: %1% %2%", _str(end_unit), *args.task.push_args.keys);

        if(__is_root(__owner_branch(end_unit)))
            return;

        __stack_push_task_t & task = args.task;
        __stack_push_args_t & push_args = task.push_args;

        __node_value_t end_node_value = __owner_branch_value(end_unit);
        __node_key_t ref_key = to_branch_ref_key(end_node_value);
        analyze_normal_path_node_t * parent_current = (*parent)->current;

        auto ref_path_node = (analyze_normal_path_node_t *)parent_current->enter(ref_key);
        _A(ref_path_node != nullptr);

        const __path_node_stacks_t * ref_stacks = ref_path_node->stacks_of(parent_current);
        _A(ref_stacks != nullptr);

        const __path_node_stack_t * ref_stack = ref_stacks->stack_of((*parent)->node);
        _A(ref_stack != nullptr);

        for(__stack_unit_t & ref_unit : ref_stack->all())
        {
            //_P(_T("__NewAction: %1% -> %2%"), (*parent)->child_tag, push_args.tag);

            #define __NewAction()  __new_raise_matched_event_action(            \
                (const analyze_branch_ref_node_t *)ref_unit.node,               \
                (*parent)->child_tag, push_args.tag, next_action                \
            )

            __stack_node_action_t * current_action = nullptr;
            #define __CurrentAction()       \
                (current_action? current_action : (current_action = __NewAction()))

            #define __Action() __CurrentAction()

            __stack_items_t stack_items;
            for(const __node_key_t * p_key = push_args.keys; *p_key != __empty_node_key; p_key++)
            {
                const __node_key_t & key = *p_key;
                auto dst = (analyze_normal_path_node_t *)ref_path_node->enter(key);

                if(dst == nullptr)
                    continue;

                const __path_node_stacks_t * stacks = dst->stacks_of(ref_path_node);
                _A(stacks != nullptr);

                const __path_node_stack_t * stack = stacks->stack_of(ref_unit.node);
                if(stack != nullptr)
                    stack_items.push_back(__stack_item_t { stack, dst });
            }

            if(stack_items.size() > 0)
            {
                __stack_node_t * stack_node = __new_stack_node(__stack_node_value_t(
                    ref_unit.node,  (*parent)->child_tag, ref_path_node,
                    weight + ref_unit.node->weight
                    /*, ref_unit.step*/
                ));

                if(ref_unit.step > 0)
                    parent->append_child(stack_node);
                else
                    parent->append_sibling(stack_node);

                //(*stack_node)->holes = hstep + ref_unit.hstep + (*parent)->holes;
                //(*stack_node)->affinity = affinity + ref_unit.affinity + (*parent)->affinity;

                __set_merge_identity(stack_node);

                __stack_grow_args_t grow_args(task);
                grow_args.apply_state = args.task_state;
                __do_stack_grow(grow_args, stack_node, stack_items);

                if(grow_args.found)
                {
                    for(__stack_node_t * stack_node0 : grow_args)
                    {
                        __append_node_action(stack_node0, __Action(), true);
                    }
                }
                else
                {
                    __remove_leaf(stack_node);
                }
            }

            auto ref_node = (const analyze_branch_ref_node_t *)ref_unit.node;
            __node_unit_t end_unit1;

            //_PF("ref_node: %1%, expandable: %2%", _str(ref_node), ref_node->expandable);

            if(!ref_node->is_dead_cycle && (end_unit1 = ref_node->get_end_unit()) != nullptr)
            {
                __check_end_node_args_t args1(args);
                //__hstep_t hstep1 = hstep + ref_unit.hstep;
                //__affinity_t affinity1 = affinity;
                int weight1 = ref_unit.node->weight + weight;

                if(ref_unit.step > 0)
                {
                    __check_end_node(args1,
                        parent, current, end_unit1, __Action(), weight1 /*, hstep1, affinity1*/
                    );
                }
                else
                {
                    __check_end_node(args1,
                        parent->parent, parent, end_unit1, __Action(), weight1
                        /*, hstep1, affinity1*/
                    );
                }

                //__increase(args1, end_unit1.hstep);
            }

            #undef __NewAction
            #undef __CurrentAction
            #undef __Action
        }
    }

    // When stack grows.
    __AlwaysInline void analyze_context_t::__stack_grow(__stack_grow_args_t & args,
                                                        __stack_node_t * stack_node)
    {
        __stack_push_args_t & push_args = args.task.push_args;

        __stack_node_value_t & value = stack_node->value;
        analyze_normal_path_node_t * src = value.current;

        // search stacks
        __stack_items_t stack_items;
        for(const __node_key_t * p_key = push_args.keys; *p_key != __empty_node_key; p_key++)
        {
            const __node_key_t & key = *p_key;
            auto dst = (analyze_normal_path_node_t *)src->enter(key);

            if(dst != nullptr)
            {
                const __path_node_stacks_t * stacks = dst->stacks_of(src);
                _A(stacks != nullptr);

                const __path_node_stack_t * stack = stacks->stack_of(value.node);
                if(stack != nullptr)
                    stack_items.push_back(__stack_item_t { stack, dst });
            }
        }

        __do_stack_grow(args, stack_node, stack_items);
    }

    // Do stack grow.
    __AlwaysInline void analyze_context_t::__do_stack_grow(__stack_grow_args_t & args,
                            __stack_node_t * stack_node, const __stack_item_t & stack_item)
    {
        __do_stack_grow(args, stack_node, __stack_items_t({ stack_item }));
    }

    // Do stack grow.
    __AlwaysInline void analyze_context_t::__do_stack_grow(__stack_grow_args_t & args,
                    __stack_node_t * stack_node, const __stack_items_t & stack_items)
    {
        if(stack_items.size() == 0)
            return;

        args.found = false;
        __stack_push_args_t & push_args = args.task.push_args;
        __stack_node_t * next_stack_node = (args.model == __overwrite)? stack_node : nullptr;
        //__holes_t holes = (*stack_node)->holes;

        // branch_switched stacks
        for(__stack_item_t & stack_item : stack_items)
        {
            for(const __stack_unit_t & unit : stack_item.stack->all(__stack_type_t::branch_switched))
            {
                _A(unit.step > 0);

                if(!args.check_stack_unit(unit))
                    continue;

                next_stack_node = nullptr;

                const analyze_node_t * node = unit.node;
                __stack_node_t * new_leaf = __new_stack_node(__stack_node_value_t(
                    node, push_args.tag, stack_item.dst /*, unit.step*/
                ));

                stack_node->append_child(new_leaf);
                (*new_leaf)->action = (*stack_node)->action;

                if(push_args.tag != nullptr)
                    (*stack_node)->child_tag = push_args.tag;

                //(*new_leaf)->holes = (*stack_node)->holes;
                //(*new_leaf)->affinity = (*stack_node)->affinity + unit.affinity;
                (*new_leaf)->weight = (*stack_node)->weight + node->weight;

                __set_merge_identity(new_leaf);
                push_args.append_new_stack_node(new_leaf);
                args.found = true;
            }
        }

        // normal stacks
        for(__stack_item_t & stack_item : stack_items)
        {
            for(const __stack_unit_t & unit : stack_item.stack->all(__stack_type_t::normal))
            {
                if(!args.check_stack_unit(unit))
                    continue;

                const analyze_node_t * node = unit.node;
                if(next_stack_node != nullptr)
                {
                    __stack_node_value_t & value = next_stack_node->value;
                    value.node = node;
                    value.current = stack_item.dst;
                    //value.holes += unit.hstep;
                    value.weight += node->weight;
                    __set_merge_identity(next_stack_node);

                    push_args.append_new_stack_node(next_stack_node);
                    next_stack_node = nullptr;
                }
                else
                {
                    __stack_node_value_t value = stack_node->value;
                    value.node = node;
                    value.current = stack_item.dst;
                    //value.holes = holes + unit.hstep;
                    value.weight += node->weight;

                    __stack_node_t * new_leaf = __new_stack_node(value);
                    stack_node->append_sibling(new_leaf);
                    push_args.append_new_stack_node(new_leaf);

                    __set_merge_identity(new_leaf);
                }

                args.found = true;
            }
        }
    }

    // Record stack units.
    __AlwaysInline size_t analyze_context_t::__stack_push_task_t::record_stack_unit(
                                                        const __stack_unit_t & unit)
    {
        void * path = unit.path;

        for(auto it_begin = stack_paths.begin(), it_end = stack_paths.end(), it = it_begin;
            it != it_end; it++)
        {
            if(*it == path)
                return it - it_begin;
        }

        stack_paths.push_back(path);
        return stack_paths.size() - 1;
    }

    // Returns current state.
    analyze_context_t::__stack_push_task_state_t
                analyze_context_t::__stack_push_task_t::current_state()
    {
        return stack_paths.size();
    }

    // Restore to the specified state.
    void analyze_context_t::__stack_push_task_t::restore(
            const analyze_context_t::__stack_push_task_state_t & state)
    {
        stack_paths.truncate(state);
    }

    // Checks stack unit.
    __AlwaysInline bool analyze_context_t::__stack_grow_args_t::check_stack_unit(
                                                        const __stack_unit_t & unit)
    {
        return task.record_stack_unit(unit) >= apply_state;
    }

    // Returns action index at a leaf.
    __AlwaysInline static int __leaf_action_index(__stack_node_t * leaf)
    {
        __stack_node_action_t * action = (*leaf)->action;
        while(action != nullptr)
        {
            if(action->action_type() == __stack_node_action_type_t::raise_matched_event)
            {
                return ((__stack_node_raise_matched_event_action_t *)action)->action_index();
            }

            action = action->next;
        }

        return 0;
    }

    // Returns back track of a leaf.
    __AlwaysInline static int __leaf_action_back_track(__stack_node_t * leaf)
    {
        __stack_node_action_t * action = (*leaf)->action;
        while(action != nullptr)
        {
            if(action->action_type() == __stack_node_action_type_t::raise_matched_event)
            {
                return ((__stack_node_raise_matched_event_action_t *)action)->back_track();
            }

            action = action->next;
        }

        return 0;
    }

    // Returns first action of a leaf.
    __AlwaysInline static __stack_node_raise_matched_event_action_t * __leaf_first_action(
                                                                __stack_node_t * leaf)
    {
        __stack_node_action_t * action = (*leaf)->action;
        __stack_node_raise_matched_event_action_t * ret_action = nullptr;

        while(action != nullptr)
        {
            if(action->action_type() == __stack_node_action_type_t::raise_matched_event)
                ret_action = (__stack_node_raise_matched_event_action_t *)action;

            action = action->next;
        }

        return ret_action;
    }

    #define __LessReturn(v1, v2)                \
        do                                      \
        {                                       \
            if((v1) < (v2)) return true;        \
            if((v2) < (v1)) return false;       \
        } while(0)

    // Returns whether a leaf is less than another leaf.
    __AlwaysInline static bool __leaf_less(__stack_node_t * leaf1, __stack_node_t * leaf2)
    {
        typedef __stack_node_action_type_t atype_t;
        typedef __stack_node_raise_matched_event_action_t action_t;

        //_PF(_T("__leaf_less: %1%, %2%"), leaf1, leaf2);
        //_PF(_T("__leaf_less height: %1%, %2%"), leaf1->height(), leaf2->height());

        // begin node index
        /*_PF(_T("begin node: %1%(%2%,%3%), %4%(%5%,%6%)"),
            _str((*leaf1)->begin_node), (*leaf1)->begin_node->index, (void *)(*leaf1)->begin_node,
            _str((*leaf1)->begin_node), (*leaf2)->begin_node->index, (void *)(*leaf2)->begin_node
        );*/

        // weight
        int weight1 = (*leaf1)->weight, weight2 = (*leaf2)->weight;
        //_P(_T("weight : "), weight1, weight2);
        __LessReturn(weight2, weight1);

        int begin_index1 = (*leaf1)->begin_node->index;
        int begin_index2 = (*leaf2)->begin_node->index;
        __LessReturn(begin_index1, begin_index2);

        // leaf affinity
        //_PF(_T("affinity: %1%, %2%"), (*leaf1)->affinity, (*leaf2)->affinity);
        //__LessReturn((*leaf2)->affinity, (*leaf1)->affinity);

        // action back track
        int back_track1 = __leaf_action_back_track(leaf1);
        int back_track2 = __leaf_action_back_track(leaf2);
        //_PF(_T("back track: %1%, %2%"), back_track1, back_track2);
        __LessReturn(back_track1, back_track2);

        // action index
        int action_index1 = __leaf_action_index(leaf1);
        int action_index2 = __leaf_action_index(leaf2);
        //_PF(_T("action index: %1%, %2%"), index1, index2);
        __LessReturn(action_index1, action_index2);

        // leaf holes
        //_PF(_T("holes: %1%, %2%"), (*leaf1)->holes, (*leaf2)->holes);
        //__LessReturn((*leaf1)->holes, (*leaf2)->holes);

        // action value
        action_t * action1 = __leaf_first_action(leaf1), * action2 = __leaf_first_action(leaf2);
        __node_value_t value1 = action1? action1->get_value() : __unknown_node_value;
        __node_value_t value2 = action2? action2->get_value() : __unknown_node_value;
        //_PF(_T("action value: %1%, %2%"), value1, value2);
        __LessReturn(value2, value1);

        return (void *)leaf1 < (void *)leaf2;
    };

    #undef __LessReturn

    // Prints identity.
    static void __print_identity(const __stack_node_t * s)
    {
        printf(_T("-- %1% | %2% :"),
            (*s)->merge_identity, (__merge_identity_t)(*s)->node
        );

        for(const __stack_node_t * s0 = s; s0 != nullptr; s0 = s0->parent)
        {
            printf(_T("%1% "), (__merge_identity_t)(*s)->current);
        }

        _P();
    }

    // Prints leaf identities.
    static void __print_leaf_identities(__leafs_t & leafs)
    {
        for(__stack_node_t * n : leafs)
        {
            __print_identity(n);
        }
    }

    #define __I(n)   ((*(n))->merge_identity)

    // Sorts leafs.
    __AlwaysInline void __sort_leafs(__leafs_t & leafs)
    {
        leafs.sort([](__stack_node_t * n1, __stack_node_t * n2) {
            return __I(n1) == __I(n2)? __leaf_less(n1, n2) : __I(n1) < __I(n2);
        });
    }

    // Merges leafs.
    __AlwaysInline void analyze_context_t::__merge_leafs()
    {
        if(__leafs.size() <= 1)
            return;

        __sort_leafs(__leafs);

        //__print_leaf_identities(__leafs);

        __merge_identity_t last = __I(__leafs[0]);
        __leafs.walk([&last, this](__stack_node_t * n) {
            return (__I(n) == last)?  (__remove_leaf(n), false) : (last = __I(n), true);
        }, 1);

        //_P(_T("|"));
        //__print_leaf_identities(__leafs);
    }

    // Prints actions.
    void __print_actions(__stack_node_action_t * action)
    {
        if(action != nullptr)
        {
            __print_actions(action->next);

            _P(_T("ACTION: "), _str(action));
        }
    }

    // Picks final leafs.
    void analyze_context_t::__pick_final_leafs()
    {
        //_P(_T("__pick_final_leafs"));

        if(__leafs.size() <= 1)
            return;

        __sort_leafs(__leafs);

        /*
        for(const __stack_node_t * node : __leafs)
        {
            _P(_T("=============="));
            __print_actions((*node)->action);
        }
        */

        __leafs.walk([this](__stack_node_t * n) {
            return __remove_leaf(n), false;
        }, 1);
    }

    #undef __I

    // Stack pop.
    void analyze_context_t::__stack_pop(__stack_pop_args_t & args, __stack_node_t * stack_node)
    {
        __stack_node_t * parent = stack_node;
        _A(parent != nullptr && parent->parent != nullptr);

        __do_stack_pop(args, stack_node);
    }

    // Do stack pop.
    void analyze_context_t::__do_stack_pop(__stack_pop_args_t & args, __stack_node_t * stack_node)
    {
        __stack_node_t * parent = stack_node->parent;
        analyze_normal_node_t * node = (analyze_normal_node_t *)(*stack_node)->node;
        __node_unit_t end_unit = node->get_end_unit();

        if(end_unit == nullptr)
        {
            __remove_leaf(stack_node);
            return;
        }

        if(parent->is_root())
        {
            args.new_stack_nodes.push_back(stack_node);
            return;
        }

        __node_value_t branch_value = __owner_branch_value(end_unit);
        __node_key_t ref_key = to_branch_ref_key(branch_value);
        __node_key_t keys[] = { ref_key, __empty_node_key };

        //__stack_push_args_t push_args(this, keys, args.end_tag);
        __stack_push_args_t push_args(this, keys, nullptr);
        __stack_push_task_t task(push_args);
        __stack_grow_args_t grow_args(task, __grow_only);
        __stack_grow(grow_args, parent);

        _A(push_args.new_stack_nodes.size() > 0);

        for(__stack_node_t * leaf : push_args.new_stack_nodes)
        {
            /*
            _PF(_T("------ parent: %1%(%2%), node: %3%(%4%), child_tag: %5% -> %6%"),
                (void *)parent, _str(parent->value), (void *)leaf, _str(leaf->value),
                ((__tag_t *)(*parent)->child_tag)->index,
                ((__tag_t *)args.end_tag)->index
            );
            */

            (*leaf)->action = __new_raise_matched_event_action(
                (const analyze_branch_ref_node_t *)(*leaf)->node, (*parent)->child_tag,
                args.end_tag, (*stack_node)->action
            );

            //_P();

            //(*leaf)->holes += end_unit.hstep;

            __do_stack_pop(args, leaf);
        }

        __remove_leaf(stack_node);
    }

    // Executes actions.
    void analyze_context_t::__execute_actions()
    {
        __stack_node_t * leaf = __leafs[0];
        if(__execute_actions(leaf))
        {
            while(leaf != nullptr)
            {
                (*leaf)->action = nullptr;
                leaf = leaf->parent;
            }

            __assign_key_action_factory.clear();
            __raise_matched_event_action_factory.clear();
        }
    }

    // Executes action.
    __AlwaysInline bool analyze_context_t::__execute_actions(__stack_node_t * leaf)
    {
        __stack_node_action_t * action = (*leaf)->action;
        if(action != nullptr)
        {
            __execute_action(action);
            return true;
        }

        return false;
    }

    // Executes action.
    __AlwaysInline void analyze_context_t::__execute_action(__stack_node_action_t * action)
    {
        if(action != nullptr)
        {
            __execute_action(action->next);

            __Trace(_T("ACTION:"), _str(action));
            action->execute();
        }
    }

    // Append leafs.
    template<typename itor_t>
    __AlwaysInline void analyze_context_t::__append_leafs(itor_t begin, itor_t end)
    {
        std::copy(begin, end, std::back_inserter(__leafs));
    }

    // Append leafs.
    template<typename container_t>
    __AlwaysInline void analyze_context_t::__append_leafs(container_t & container)
    {
        __append_leafs(container.begin(), container.end());
    }

    // Removes leaf unitl prediacte function.
    template<typename pred_t>
    __AlwaysInline void analyze_context_t::__remove_leaf_until(
                                        __stack_node_t * stack_node, pred_t pred)
    {
        while(stack_node->is_leaf() && !stack_node->is_root() && !pred(stack_node))
        {
            __stack_node_t * stack_node0 = stack_node;
            stack_node = stack_node->parent;
            __remove_stack_node(stack_node0);
        }
    }

    // Removes specified node.
    __AlwaysInline void analyze_context_t::__remove_leaf(__stack_node_t * stack_node)
    {
        __remove_leaf_until(stack_node, [](__stack_node_t *) { return false; });
    }

    // Removes stack node.
    __AlwaysInline void analyze_context_t::__remove_stack_node(__stack_node_t * stack_node)
    {
        stack_node->remove();
        __stack_node_factory.release(stack_node);
    }

    // Creates new stack node.
    template<typename stack_node_value_t>
    __AlwaysInline __stack_node_t * analyze_context_t::__new_stack_node(stack_node_value_t && value)
    {
        return __stack_node_factory.new_obj(std::forward<stack_node_value_t>(value));
    }

    // Execute callback when matched or completed.
    __AlwaysInline void analyze_context_t::__do_callback(analyze_callback_args_t & args)
    {
        if(__callback != nullptr)
        {
            (*__callback)(args);
        }
    }

    // Execute end callback.
    void analyze_context_t::__do_end_callback()
    {
        typedef analyze_end_analyze_callback_args_t args_t;
        typedef analyze_end_analyze_callback_data_t data_t;

        args_t args(analyze_callback_action_t::analyze_end, data_t());
        __do_callback(args);
    }

    // Execute branch matched callback.
    void analyze_context_t::__do_branch_matched_callback(
                        const __matched_item_t & matched_item, __tag_t * tag)
    {
        typedef branch_matched_analyze_callback_args_t args_t;
        typedef branch_matched_analyze_callback_data_t data_t;

        if(matched_item.value() >= 0)
        {
            args_t args(analyze_callback_action_t::branch_matched, data_t(tag, matched_item));
            __do_callback(args);
        }
    }

    // Returns whether it's invisible, e.g. comments.
    bool analyze_context_t::__is_invisible(token_value_t value)
    {
        const token_property_t * token_property = __token_property_cache.get_property(value);
        return token_property->is_invisible;
    }

    //-------- ---------- ---------- ---------- ----------

    // Apend node action.
    void analyze_context_t::__append_node_action(__stack_node_t * node,
                                 __stack_node_action_t * action, bool push_back)
    {
        __stack_node_value_t & value = node->value;
        if(push_back)
        {
            value.action = __join_actions(value.action, action);
        }
        else  // push front
        {
            value.action = __join_actions(action, value.action);
        }
    }

    // Creates a new raise matched event action.
    __stack_node_raise_matched_event_action_t * analyze_context_t::__new_raise_matched_event_action(
            const analyze_branch_ref_node_t * branch_ref, __tag_t * begin_tag, __tag_t * end_tag,
            __stack_node_action_t * next_action)
    {
        /*
        _PF(_T("__new_raise_method_event_action: %1%, %2% -> %3%"),
            _str(branch_ref),
            ((token_tag_t *)begin_tag)->index, ((token_tag_t *)end_tag)->index
        );
        */

        auto action = __raise_matched_event_action_factory.new_obj(
            *this, branch_ref, begin_tag, end_tag
        );

        action->next = next_action;

        return action;
    }

    // Appends a new stack node.
    void analyze_context_t::__stack_push_args_t::append_new_stack_node(__stack_node_t * leaf)
    {
        new_stack_nodes.push_back(leaf);
        if(out_value != nullptr)
        {
            __node_value_t value = (*leaf)->node->key.value;
            auto action = __owner->__assign_key_action_factory.new_obj(out_value, value);
            __owner->__append_node_action(leaf, action);
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns token reader of language.
    token_reader_t * __token_reader_context_t::token_reader_of(lang_id_t lang_id)
    {
        return al::map_get(__token_reader_cache, lang_id, [this, lang_id]() {
            lang_t * lang = this->compile_context.global_context.lang_of(lang_id);
            return __Helper(lang).create_token_reader(*this);
        });
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns whether two elements are equals.
    bool analyzer_element_t::operator == (const analyzer_element_t & other) const
    {
        if(type != other.type)
            return false;

        switch(type)
        {
            case analyzer_element_type_t::token:
                return token == other.token;

            case analyzer_element_type_t::ast_node:
                return ast_node == other.ast_node;

            default:
                return true;
        }
    }

    // Returns whether two elements are node equals.
    bool analyzer_element_t::operator != (const analyzer_element_t & other) const
    {
        return !operator == (other);
    }

    ////////// ////////// ////////// ////////// //////////

    // Analyze element read models.
    X_ENUM_INFO(analyzer_element_read_model_t)

        // Normal.
        X_C(normal)

        // Read all
        X_C(all)

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    // Analyzes ast node.
    ast_node_t * __analyze_ast(__token_reader_context_t & context, code_section_t ** &p_section);

    // Constructor of analyzer element reader.
    analyzer_element_reader_t::analyzer_element_reader_t(__token_reader_context_t & context,
                                                         code_section_t ** &p_section)
        : __context(context)
        , __p_section(p_section), __section(*__p_section), __depth(__section->depth)
    {
        __reader = __context.token_reader_of(__section->lang);
        __enumerator = __reader->read(__section->source_code, __section->length, __section->file());
    }

    // Returns element at specified tag.
    __AlwaysInline analyzer_element_t & analyzer_element_reader_t::element_at(
                                                __tag_t * tag, __model_t model)
    {
        __item_t & item = __items[tag->index];
        if(model == __model_t::normal && item.flag != nullptr)
            return item.flag->element;

        return item;
    }

    // Returns next element at specifed tag.
    __AlwaysInline analyzer_element_t * analyzer_element_reader_t::next()
    {
        while(__index >= __items.size())
        {
            if(!__read_next_element())
                return __end_element();
        }

        __item_t & item = __items[__index++];
        __Trace(_T("\n==== >>>> "), (string_t)item);
        return &item;
    }

    // Returns next tag of specified tag.
    __AlwaysInline __tag_t * analyzer_element_reader_t::next(__tag_t * tag, __model_t model)
    {
        __item_t & item = __items[tag->index];

        if(model == __model_t::normal && item.flag != nullptr)
            return item.flag->end_tag;

        return __items[tag->index + 1].get_tag();
    }

    // Reads next element.
    __AlwaysInline bool analyzer_element_reader_t::__read_next_element()
    {
        token_t * token = __enumerator->next();
        if(token != nullptr)
        {
            __append_element(token);
            return true;
        }

        __section = *++__p_section;
        if(__section == nullptr)
            return false;

        if(__section->depth > __depth)
        {
            ast_node_t * node = __analyze_ast(__context, __p_section);
            expand_ast_node(node, [this](ast_node_t * node0) { __append_element(node0); });

            return true;
        }

        if(__section->depth == __depth)
        {
            __enumerator = __reader->read(__section->source_code, __section->length,
                                          __section->file());
            return __read_next_element();
        }

        return false;
    }

    // Appends ast node.
    void analyzer_element_reader_t::append_ast(__tag_t * from_tag, __tag_t * end_tag,
                ast_node_t * ast_node, const analyze_node_t * node)
    {
        _A(ast_node != nullptr);

        __item_t & item = __items[from_tag->index];
        __ast_flag_t * flag = memory_t::new_obj<__ast_flag_t>(
            &__heap, ast_node, end_tag, node
        );

        __item_t & item2 = __items[end_tag->index - 1];

        flag->next = item.flag;
        item.flag = flag;
    }

    // Gets root ast node.
    ast_node_t * analyzer_element_reader_t::get_root_ast()
    {
        if(__items.size() == 0)
            return nullptr;

        __item_t & item = __items[0];
        if(item.flag != nullptr)
            return item.flag->ast_node;

        return nullptr;
    }

    ////////// ////////// ////////// ////////// //////////

    // Do analyzer.
    void __analyzer_t::analyze()
    {
        try
        {
            __analyze();
        }
        catch(const logic_error_t<__e_t> & e)
        {
            __process_error(e);
        }
    }

    // Do analyzer.
    void __analyzer_t::__analyze()
    {
        while((__element = __reader->next())->type != analyzer_element_type_t::__unknown__)
        {
            switch(__element->type)
            {
                case analyzer_element_type_t::token:
                    __push_token(__element->token, &__element->tag);
                    break;

                case analyzer_element_type_t::ast_node:
                    __push_ast_node(__element->ast_node, &__element->tag);
                    break;

                default:
                    throw _E(analyze_tree_error_t::unknown_element_type,
                        sprintf(_T("unknown element type: %1%"), __element->type)
                    );
            }
        }

        __push_end(&__element->tag);
    }

    // Pushes token.
    void __analyzer_t::__push_token(token_t * token, __tag_t * tag)
    {
        if(token->value != unknown_token_value)
        {
            __push(__node_key_t(__node_type_t::token, token->value), tag);
        }
        else
        {
            _A(token->data != nullptr);

            const token_value_t * possible_values = ((token_data_t *)token->data)->possible_values;
            _A(possible_values != nullptr);

            size_t count = __possible_value_count(possible_values);
            _A(count >= 1);

            __node_key_t possible_keys[count + 1];
            std::transform(possible_values, possible_values + count, possible_keys, to_token_key);
            possible_keys[count] = __empty_node_key;

            __push(possible_keys, tag, &token->value);
        }
    }

    // Returns possible value count.
    size_t __analyzer_t::__possible_value_count(const token_value_t * possible_values)
    {
        const token_value_t * value = possible_values;
        while(*value != unknown_token_value)
        {
            value++;
        }

        return value - possible_values;
    }

    // Pushes an ast node.
    void __analyzer_t::__push_ast_node(ast_node_t * node, __tag_t * tag)
    {
        
    }

    // Pushes with key and tag.
    void __analyzer_t::__push(__node_key_t key, __tag_t * tag)
    {
        __node_key_t keys[] = { key, __empty_node_key };
        __push(keys, tag);
    }

    // Pushes with keys and tag.
    void __analyzer_t::__push(const __node_key_t * keys, __tag_t * tag,
                                                __node_value_t * out_value)
    {
        __context.go(keys, tag, out_value);
    }

    // Pushes end.
    void __analyzer_t::__push_end(__tag_t * tag)
    {
        __push(__end_node_key, tag);
    }

    // Process analyze errors.
    void __analyzer_t::__process_error(const logic_error_t<__e_t> & e)
    {
        switch(e.code)
        {
            case __e_t::format_error:
                __process_format_error(e);
                break;

            default:
                throw e;
        }
    }

    // Returns format error line.
    static string_t __format_error_line(code_unit_t * cu, const string_t & err_msg)
    {
        _A(cu != nullptr);

        const code_file_t * file = cu->file;
        string_t msg;

        if(file != nullptr)
        {
            codepos_helper_t h(file->get_source_code());
            codepos_t pos = h.pos_of(cu->s);

            auto pair = cu->current_line_pos();
            string_t line(pair.first, pair.second);

            msg = _F(_T("%1%:%2%:%3%: error: %4%\n%5%\n%6%%7%"),
                file->get_file_name(), pos.line, pos.col, err_msg,
                line, string_t(cu->s - pair.first, ' '), string_t(cu->length, '~')
            );
        }
        else
        {
            msg = _F(_T("%1%: %2%"), cu->current_line(), err_msg);
        }

        return msg;
    }

    // Process format error.
    void __analyzer_t::__process_format_error(const logic_error_t<__e_t> & e)
    {
        typedef analyzer_element_type_t __element_type_t;

        _A(__element != nullptr);

        switch(__element->type)
        {
            case __element_type_t::token: {

                token_t * token = __element->token;
                _A(token != nullptr);

                code_unit_t * cu = (code_unit_t *)*token;
                if(cu != nullptr)
                    throw _E(__e_t::format_error, __format_error_line(cu, e.get_message()));

            }   break;

            case __element_type_t::ast_node:
                break;

            default:
                X_UNEXPECTED();
        }

        throw e;
    }

    ////////// ////////// ////////// ////////// //////////
    // analyzer_t

    // Do analyze
    void analyzer_t::analyze(analyzer_element_reader_t * reader, analyze_callback_t * callback)
    {
        _A(reader != nullptr);
        _A(callback != nullptr);

        __analyzer_t(__lang, __tree, reader, callback).analyze();
    }

    ////////// ////////// ////////// ////////// //////////

    // Analyze tree builder provider.
    class __analyze_tree_builder_provider_t : public analyze_tree_builder_provider_t
    {
    public:

        // Constructors.
        __analyze_tree_builder_provider_t(global_context_t & context,
            lang_statement_analyze_service_t * service)
            : __context(context), __service(service)
        {
            _A(service != nullptr);
        }

        // Returns node value.
        virtual __node_value_t get_node_value(const string_t & name,
                                                    __node_type_t node_type) override
        {
            return __service->get_node_value(__context, name, node_type);
        }

    private:
        global_context_t & __context;
        lang_statement_analyze_service_t * __service;
    };

    ////////// ////////// ////////// ////////// //////////

    // Creates analyzer of language.
    analyzer_t * __create_analyzer(global_context_t & global_context, lang_id_t lang_id)
    {
        lang_t * lang = global_context.lang_of(lang_id);
        lang_service_helper_t h(lang);
        const string_t pattern = h.get_analyze_pattern(global_context);

        typedef __analyze_tree_builder_provider_t provider_t;
        provider_t * provider = global_context.new_obj<provider_t>(global_context,
            h.require_service<lang_statement_analyze_service_t>()
        );

        analyze_tree_t * tree = parse_analyze_tree(
            pattern.c_str(), global_context.get_memory(), provider
        );

        return global_context.new_obj<analyzer_t>(lang, tree);
    }

    // Gets analyzer of language.
    analyzer_t * __get_analyzer(global_context_t & global_context, lang_id_t lang_id)
    {
        return global_context.from_cache<analyzer_t>(lang_id, [&global_context, lang_id] {
            return __create_analyzer(global_context, lang_id);
        });
    }

    ////////// ////////// ////////// ////////// //////////

    // Analyze callback.
    class __analyze_callback_t : public object_t, public analyze_callback_t
    {
    public:

        // Constructors.
        __analyze_callback_t(compile_context_t & context, lang_t * lang,
                analyzer_element_reader_t & reader)
            : __ast_factory(*context.new_obj<ast_context_t>(context, lang), reader)
            , __reader(reader)
        { }

        // On call back.
        virtual void on_call(void * sender, analyze_callback_args_t & args) override
        {
            switch(args.action)
            {
                case analyze_callback_action_t::branch_matched: {

                    typedef branch_matched_analyze_callback_args_t __args_t;
                    typedef branch_matched_analyze_callback_data_t __data_t;

                    __data_t & data = ((__args_t &)args).data;

                    const __matched_item_t & item = data.matched_item;
                    __ast_factory.on_branch_matched(
                        item.node, item.branch_value,
                        item.tag? item.tag : __reader.begin_tag(),
                        data.end_tag? data.end_tag : __reader.end_tag()
                    );

                }   break;

                case analyze_callback_action_t::analyze_end: {
                    __ast_factory.on_end();
                }   break;

                default:
                    throw _ECF(not_supported, _T("action %1% not supported"), args.action);
            }
        }

        // Returns analyze result.
        ast_node_t * get_result()
        {
            return __ast_factory.get_result();
        }

    private:
        ast_factory_t __ast_factory;
        analyzer_element_reader_t & __reader;
    };

    ////////// ////////// ////////// ////////// //////////

    // Analyzes ast node.
    ast_node_t * __analyze_ast(__token_reader_context_t & context, code_section_t ** & p_section)
    {
        analyzer_element_reader_t reader(context, p_section);

        global_context_t & global_context = context.compile_context.global_context;
        analyzer_t * analyzer = __get_analyzer(global_context, (*p_section)->lang);

        lang_t * lang = context.compile_context.global_context.lang_of((*p_section)->lang);
        __analyze_callback_t callback(context.compile_context, lang, reader);

        analyzer->analyze(&reader, &callback);

        return callback.get_result();
    }

    // Analyzes ast node.
    ast_node_t * __analyze_ast(compile_context_t & context, code_section_t ** p_section)
    {
        __token_reader_context_t reader_context(context);
        return __analyze_ast(reader_context, p_section);
    }

    // Analyzes ast node.
    ast_node_t * analyze_ast(compile_context_t & context, code_t * code)
    {
        _A(code != nullptr);

        auto sections = code->sections();
        size_t section_count = sections.size();

        code_section_t * section_array[section_count + 1];
        std::copy(sections.begin(), sections.end(), section_array);
        section_array[section_count] = nullptr;

        return __analyze_ast(context, (code_section_t **)section_array);
    }

    ////////// ////////// ////////// ////////// //////////

    // Ast build error codes.
    X_ENUM_INFO(ast_build_error_code_t)

        // Build failed.
        X_C(build_failed)

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

} } }  // X_ROOT_NS::modules::compile
