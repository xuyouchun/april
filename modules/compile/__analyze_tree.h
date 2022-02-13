
#ifndef __COMPILE_ANALYZE_TREE_H__
#define __COMPILE_ANALYZE_TREE_H__

namespace X_ROOT_NS::modules::compile {

    namespace
    {
        using namespace core;
    }

    ////////// ////////// ////////// ////////// //////////

    typedef analyze_node_type_t __node_type_t;
    typedef uint32_t            __node_index_t;
    typedef token_tag_t         __tag_t;

    //-------- ---------- ---------- ---------- ----------

    typedef analyze_node_value_t    __node_value_t;
    typedef analyze_node_key_t      __node_key_t;
    typedef analyze_node_keys_t     __node_keys_t;

    constexpr __node_key_t __empty_node_key = analyze_empty_node_key;
    constexpr __node_key_t __end_node_key   = analyze_end_node_key;
    constexpr __node_key_t __empty_token    = analyze_empty_token;

    //-------- ---------- ---------- ---------- ----------

    // Converts to token key.
    X_INLINE __node_key_t to_token_key(__node_value_t value)
    {
        return __node_key_t(__node_type_t::token, value);
    }

    // Converts to branch ref key.
    X_INLINE __node_key_t to_branch_ref_key(__node_value_t value)
    {
        return __node_key_t(__node_type_t::branch_ref, value);
    }

    //-------- ---------- ---------- ---------- ----------

    // Analyze node repeat types.
    X_ENUM(analyze_node_repeat_type_t)

        // Repeats once.
        once,
        
        // Repeats any times.
        star,
        
        // Repeats more than one times.
        plus,
        
        // No repeat or Repeats one time.
        question,

    X_ENUM_END

    typedef analyze_node_repeat_type_t __repeat_type_t;

    //-------- ---------- ---------- ---------- ----------

    // Analyze tree error codes.
    X_ENUM(analyze_tree_error_t)

        // Format error.
        format_error,

        // Unknown branch.
        unknown_branch,

        // Unknown token.
        unknown_token,

        // Parse error.
        parse_error,

        // Stack closed.
        stack_closed,

        // Unknown element type.
        unknown_element_type,

        // Overflow.
        overflow,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Special node values.
    X_ENUM_(special_node_value_t, sizeof(__node_value_t))
        
        // Root node value.
        root    =  0,

        // unknown node value.
        unknown = -1,

        // End node value.
        end     = -2,

    X_ENUM_END

    const __node_value_t __unknown_node_value = (__node_value_t)special_node_value_t::unknown;
    const __node_value_t __end_node_value     = (__node_value_t)special_node_value_t::end;
    const __node_value_t __root_node_value    = (__node_value_t)special_node_value_t::root;

    ////////// ////////// ////////// ////////// //////////

    class analyze_node_t;
    class analyze_end_node_t;

    // Analyze node unit.
    struct analyze_node_unit_t : compare_operators_t<analyze_node_unit_t, analyze_node_t *>
    {
        // Constructor.
        analyze_node_unit_t(analyze_node_t * node)
            : node(node)
        { }

        // Constructor.
        analyze_node_unit_t(std::nullptr_t null = nullptr)
            : analyze_node_unit_t((analyze_node_t *)nullptr)
        { }

        // Analyze node.
        analyze_node_t * node;

        // Returns analyze node.
        operator analyze_node_t * () const { return node; }

        // Returns two units are equals.
        bool operator == (std::nullptr_t) const { return node == nullptr; }


        // Returns two units are not equals.
        bool operator != (std::nullptr_t) const { return node != nullptr; }

        // Assign to nullptr.
        analyze_node_unit_t & operator = (std::nullptr_t)
        {
            node = nullptr;

            return *this;
        }

        // Converts to a string.
        operator string_t() const { return _str(node); }

        // Returns analyze node.
        analyze_node_t * operator ->() { return node; }

        // Returns analyze node.
        const analyze_node_t * operator ->() const { return node; }

        // Returns analyze node.
        analyze_node_t * operator *() { return node; }

        // Returns analyze node.
        const analyze_node_t * operator *() const { return node; }
    };

    typedef analyze_node_unit_t __node_unit_t;

    //-------- ---------- ---------- ---------- ----------

    // Analyze node collection.
    struct analyze_nodes_t
    {
    public:

        // Appends a unit.
        void append(const __node_unit_t & u);

        // Appends a node.
        void append(analyze_node_t * node);

        // Appends nodes.
        void append(analyze_nodes_t & nodes);

        // Removes unit.
        bool remove(const __node_unit_t & u);

        // Replaces node to another.
        bool replace(const __node_unit_t & old, const __node_unit_t & new_);

        // Returns whether contains the unit.
        bool contains(const __node_unit_t & u);

        // Returns all node units.
        auto all() const { return _range(__node_units); }

        // Returns the begin iterator.
        auto begin() const { return __node_units.begin(); }

        // Returns the end iterator.
        auto end() const { return __node_units.end(); }

        // Returns count of node units.
        size_t size() const { return __node_units.size(); }

        // Returns the last node unit.
        __node_unit_t last() const { return __last; }

        // Returns end node unit.
        __node_unit_t end_unit() const { return __end; }

        // Swap nodes to another set.
        void swap(analyze_nodes_t & other);

        // Clear all nodes.
        void clear();

    private:
        std::set<__node_unit_t> __node_units;
        __node_unit_t __end  = nullptr;
        __node_unit_t __last = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////

    typedef int16_t __weight_t;

    class analyze_branch_node_t;

    // Analyze node.
    class analyze_node_t : public object_t, public no_copy_ctor_t
                         , public equals_t<analyze_node_t>
                         , public less_t<analyze_node_t>
    {
    public:

        // Constructor.
        analyze_node_t(__node_index_t index, __node_key_t key,
                        const string_t & flag = _T(""), __weight_t weight = 0)
            : index(index), key(key), flag(flag), weight(weight) { }

        // Returns node type.
        virtual __node_type_t node_type() const = 0;

        // Repeat type.
        __repeat_type_t repeat_type = __repeat_type_t::once;

        const __node_index_t index;                 // Node index.
        const __node_key_t   key;                   // Node key.
        const string_t       flag;                  // Flag.
        const __weight_t     weight;                // Weight.
        analyze_branch_node_t * branch = nullptr;   // Owner branch.

        // Returns this node type.
        __node_type_t this_type() const { return key.type; }

        // Returns whether two nodes are equals.
        virtual bool equals(const analyze_node_t & node) const override;

        // Returns whether a node is less than another.
        virtual bool less(const analyze_node_t & node) const override;

        // Converts to a string.
        virtual const string_t to_string() const override;

    protected:

        // Converts to a string.
        virtual const string_t __to_string() const
        {
            return _T("node");
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Normal analyze node.
    class analyze_normal_node_t : public analyze_node_t
    {
    public:

        // Constructor.
        analyze_normal_node_t(__node_index_t index, __node_key_t key,
                const string_t & flag = _T(""), __weight_t weight = 0)
            : analyze_node_t(index, key, flag, weight) { }

        // Nodes.
        analyze_nodes_t nodes;

        // Returns end unit.
        __node_unit_t get_end_unit() const
        {
            return nodes.end_unit();
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Token analyze node.
    class analyze_token_node_t : public analyze_normal_node_t
    {
    public:

        // Constructor.
        analyze_token_node_t(__node_index_t index, __node_value_t value,
                const string_t & name, const string_t & flag = _T(""), __weight_t weight = 0)
            : analyze_normal_node_t(index, __node_key_t(__node_type_t::token, value), flag, weight)
            , value(value), name(name) { }

        // Returns this node type.
        virtual __node_type_t node_type() const override final
        {
            return __node_type_t::token;
        }

        const __node_value_t value;     // Node value.
        const string_t name;            // Node name.

        // Returns whether two nodes are equals.
        virtual bool equals(const analyze_node_t & node) const override
        {
            return node.node_type() == __node_type_t::token
                && ((const analyze_token_node_t &)node).value == value;
        }

        // Returns whether a node is less than another.
        virtual bool less(const analyze_node_t & node) const override
        {
            const __node_type_t type1 = __node_type_t::token, type2 = node.node_type();
            if (type1 != type2)
                return type1 < type2;

            return value < ((const analyze_token_node_t &)node).value;
        }

    protected:

        // Converts to a string.
        virtual const string_t __to_string() const override
        {
            return name;
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Branch analyze node.
    class analyze_branch_node_t : public analyze_normal_node_t
    {
    public:

        // Constructor.
        analyze_branch_node_t(__node_index_t index, __node_value_t value,
            const string_t & name, int level, bool inner = false,
            const string_t & flag = _T(""), __weight_t weight = 0)
            : analyze_normal_node_t(index, __node_key_t(__node_type_t::branch, value), flag, weight)
            , name(name), level(level), inner(inner) { }

        const string_t name;        // Branch name
        const int level;            // Branch level, depth.
        bool inner;                 // Is inner branch, defines inner.
        analyze_nodes_t subnodes;   // Subnodes.

        // Returns this node type.
        virtual __node_type_t node_type() const override final
        {
            return __node_type_t::branch;
        }

        // Returns whether it can be ignored.
        bool illusory() const { return subnodes.end_unit() != nullptr; }

    protected:

        // Converts to a  string.
        virtual const string_t __to_string() const override
        {
            if (name.empty())
                return _T("EMPTY");

            return name;
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Root analyze node.
    class analyze_root_node_t : public analyze_branch_node_t
    {
    public:

        // Constructors.
        analyze_root_node_t(__node_index_t index)
            : analyze_branch_node_t(index, __root_node_value, _T("root"), -1)
        { }

        // Node key.
        static const __node_key_t key;

    protected:

        // Converts to a string.
        virtual const string_t __to_string() const override
        {
            return _T("ROOT");
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Branch ref node.
    class analyze_branch_ref_node_t : public analyze_normal_node_t
    {
    public:

        // Constructor
        analyze_branch_ref_node_t(__node_index_t index, __node_value_t branch_value,
                    const string_t & name, const string_t & flag = _T(""), __weight_t weight = 0)
            : analyze_normal_node_t(index, __node_key_t(__node_type_t::branch_ref, branch_value),
                                                                                flag, weight)
            , branch_value(branch_value), name(name) { }

        // Returns this type.
        virtual __node_type_t node_type() const override final
        {
            return __node_type_t::branch_ref;
        }

        const __node_value_t branch_value;      // Branch value.
        const string_t name;                    // Branch name.
        bool  is_left_cycle = false;            // Is left cycle.
        bool  expandable = true;                // Expandable, avoid dead cycle.
        bool  is_dead_cycle = false;            // Is dead cycle.

        // Returns whether two nodes are equals.
        virtual bool equals(const analyze_node_t & node) const override
        {
            return node.node_type() == __node_type_t::branch_ref &&
                ((const analyze_branch_ref_node_t &)node).branch_value == branch_value;
        }

        // Returns whether a node is less than another.
        virtual bool less(const analyze_node_t & node) const override
        {
            const __node_type_t type1 = __node_type_t::branch_ref, type2 = node.node_type();
            if (type1 != type2)
                return type1 < type2;

            return branch_value < ((const analyze_branch_ref_node_t &)node).branch_value;
        }

    protected:

        // Converts to a string.
        virtual const string_t __to_string() const override
        {
            return sprintf(_T("$%1%"), name);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // End analyze node.
    class analyze_end_node_t : public analyze_node_t
    {
    public:

        // Constructor.
        analyze_end_node_t(__node_index_t index, analyze_node_t * start,
                                        analyze_branch_node_t * owner_branch)
            : analyze_node_t(index, __node_key_t(__node_type_t::end))
            , start(start), owner_branch(owner_branch)
        {
            _A(start != nullptr);
            _A(owner_branch != nullptr);
        }

        // Returns this node type.
        virtual __node_type_t node_type() const override final
        {
            return __node_type_t::end;
        }

        // Returns a branch value.
        __node_value_t branch_value() const
        {
            return branch->key.value;
        }

        analyze_node_t * const start;                   // The start node.
        analyze_branch_node_t * const owner_branch;     // It's owner branch.

    protected:

        // Converts to a string.
        virtual const string_t __to_string() const override
        {
            return sprintf(_T("end:%1%"), owner_branch->name);
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // Empty analyze node.
    class analyze_empty_node_t : public analyze_normal_node_t
    {
    public:

        // Constructor.
        analyze_empty_node_t(__node_index_t index)
            : analyze_normal_node_t(index, __node_key_t(__node_type_t::empty))
        { }

        // Returns this node type.
        virtual __node_type_t node_type() const override final
        {
            return __node_type_t::empty;
        }

    protected:

        // Converts to a string.
        virtual const string_t __to_string() const override
        {
            return _T("empty");
        }

    private:
        static __node_value_t __next_value;
    };

    ////////// ////////// ////////// ////////// //////////

    class analyze_normal_path_node_t;

    // Analyze tree.
    class analyze_tree_t : public object_t
    {
    public:

        // Constructor.
        analyze_tree_t(analyze_root_node_t * root, analyze_normal_path_node_t * path_root)
            : root(root), path_root(path_root)
        {
            _A(root != nullptr);
            _A(path_root != nullptr);
        }

        analyze_root_node_t * const root;               // Node root
        analyze_normal_path_node_t * const path_root;   // Path root.

        X_TO_STRING_IMPL(_T("analyze_tree_t"))
    };

    ////////// ////////// ////////// ////////// //////////

    // Interface for getting node value by name and type.
    X_INTERFACE analyze_tree_builder_provider_t
    {
        // Gets node value by name and type.
        virtual __node_value_t get_node_value(const string_t & name, __node_type_t node_type) = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    class analyze_path_node_t;
    class __stack_node_action_t;

    typedef al::multikey_t<range_t<const analyze_node_t **>, const analyze_node_t *>
                                                            __path_node_cache_key_t;

    //typedef uint32_t __holes_t, __affinity_t;
    typedef arch_uint_t __merge_identity_t;

    // Stack node value.
    struct __stack_node_value_t
        : public compare_operators_t<__stack_node_value_t, const analyze_node_t *>
    {
        // Constructor.
        __stack_node_value_t()
            : __stack_node_value_t(nullptr, nullptr, nullptr/*, 0*/, 0)
        { }

        // Constructor.
        __stack_node_value_t(const analyze_node_t * node, __tag_t * tag,
            analyze_normal_path_node_t * current/*, int step*/, int weight = 0)
            : node(node), current(current), /*step(step),*/ begin_node(node), begin_tag(tag)
            , weight(weight)
        { }

        const analyze_node_t * node;                    // Node
        const analyze_node_t * begin_node;              // Begin node.
        analyze_normal_path_node_t * current;           // Current path node.
        __tag_t * child_tag = nullptr, * begin_tag;     // Child tag, begin tag.
        __stack_node_action_t * action = nullptr;       // Actions.

        //__holes_t holes = 0;
        //__affinity_t affinity = 0;

        //int step = 0;
        int weight = 0;                                 // Weight.
        __merge_identity_t merge_identity;              // Merge identity.

        // Returns analyze node.
        operator const analyze_node_t * () const { return node; }

        // Converts to a string.
        operator string_t() const;
    };

    // Writes a value to a stream.
    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const __stack_node_value_t & value)
    {
        return stream << ((string_t)value).c_str();
    }

    typedef al::lr_tree_node_t<__stack_node_value_t> __stack_node_t;

    ////////// ////////// ////////// ////////// //////////
    // __stack_node_action_type_t

    // Stack node action types.
    X_ENUM(__stack_node_action_type_t)

        // Assign key.
        assign_key,

        // Raise matched event.
        raise_matched_event,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------
    // __stack_node_action_t

    // Stack node action.
    class __stack_node_action_t : public object_t
        , public equals_t<__stack_node_action_t>
        , public clonable_t<__stack_node_action_t>
    {
    public:
        __stack_node_action_t * next = nullptr;

        // Execute the action.
        virtual void execute() = 0;

        // Returns action type.
        virtual __stack_node_action_type_t action_type() const = 0;

        // Returns two actions are equals.
        virtual bool equals(const __stack_node_action_t & action) const override = 0;

        // Clone actions.
        virtual __stack_node_action_t * clone(memory_t * memory) const override = 0;

        X_TO_STRING_IMPL(_T("__stack_node_action_t"))
    };

    //-------- ---------- ---------- ---------- ----------
    // __stack_node_assign_key_action_t

    // Stack node assign key action.
    class __stack_node_assign_key_action_t : public __stack_node_action_t
    {
        typedef __stack_node_assign_key_action_t __self_t;

    public:

        // Constructor.
        __stack_node_assign_key_action_t(__node_value_t * out_value, __node_value_t value)
            : __out_value(out_value), __value(value) { }

        // Execute the action.
        virtual void execute() override
        {
            *__out_value = __value;
        }

        // Returns action type.
        virtual __stack_node_action_type_t action_type() const override final
        {
            return __stack_node_action_type_t::assign_key;
        }

        // Returns whether two actions are equals.
        virtual bool equals(const __stack_node_action_t & action) const override final
        {
            if (action.action_type() != __stack_node_action_type_t::assign_key)
                return false;

            const __self_t & a = (const __self_t &)action;
            return a.__out_value == __out_value && a.__value == __value;
        }

        // Clone a action.
        virtual __stack_node_action_t * clone(memory_t * memory) const override
        {
            return memory_t::new_obj<__stack_node_assign_key_action_t>(memory,
                __out_value, __value
            );
        }

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return sprintf(_T("%1% %2%"), action_type(), __value);
        }

        // Gets the node value.
        __node_value_t get_value() const { return __value; }

    private:
        __node_value_t * __out_value;
        __node_value_t   __value;
    };

    //-------- ---------- ---------- ---------- ----------
    // __stack_node_raise_matched_event_action_t

    class analyze_context_t;

    // Raises branch matched event action.
    class __stack_node_raise_matched_event_action_t : public __stack_node_action_t
    {
        typedef __stack_node_raise_matched_event_action_t __self_t;
        typedef __stack_node_action_t                     __super_t;

    public:

        // Constructor.
        __stack_node_raise_matched_event_action_t(analyze_context_t & context,
                const analyze_branch_ref_node_t * branch_ref,
                __tag_t * begin_tag, __tag_t * end_tag,
                const analyze_node_t * begin_node)
            : __context(context), __branch_ref(branch_ref)
            , __begin_tag(begin_tag), __end_tag(end_tag), begin_node(begin_node)
        { }

        // Executes the action.
        virtual void execute() override;

        // Returns action type.
        virtual __stack_node_action_type_t action_type() const override final
        {
            return __stack_node_action_type_t::raise_matched_event;
        }

        // Returns whether two actions are equals.
        virtual bool equals(const __stack_node_action_t & action) const override final
        {
            if (action.action_type() != __stack_node_action_type_t::raise_matched_event)
                return false;

            const __self_t & a = (const __self_t &)action;
            return a.__branch_ref == __branch_ref && a.__begin_tag == __begin_tag
                    && a.__end_tag == __end_tag;
        }

        // Clones a node action.
        virtual __stack_node_action_t * clone(memory_t * memory) const override
        {
            return memory_t::new_obj<__stack_node_raise_matched_event_action_t>(memory,
                __context, __branch_ref, __begin_tag, __end_tag, begin_node
            );
        }

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return sprintf(_T("%1% %2%:%3%-%4%"),
                action_type(), _str(__branch_ref), _str(__begin_tag), _str(__end_tag)
            );
        }

        // Returns branch value.
        __node_value_t get_value() const { return __branch_ref->branch_value; }

        // Returns the begin index.
        int begin_index() const { return __begin_tag? __begin_tag->index : -1; }

        // Returns the end index.
        int end_index()   const { return __end_tag? __end_tag->index : -1; }

        // Returns branch node index.
        __node_index_t branch_index() const { return __branch_ref->index; }

        // Returns action index.
        int action_index() const;

        // Returns back track.
        int back_track() const;

        const analyze_node_t * const begin_node;

    private:
        analyze_context_t & __context;
        const analyze_branch_ref_node_t * __branch_ref;
        __tag_t * __begin_tag, * __end_tag;
        int16_t __action_index = -1, __back_track = -1;
    };

    ////////// ////////// ////////// ////////// //////////
    // analyze_tree_build_context_t

    // Context for build analyze tree.
    class analyze_tree_build_context_t : public object_t
    {
    public:

        // Constructor.
        analyze_tree_build_context_t(memory_t * memory = nullptr)
            : memory(memory), __heap(memory) { }

        // Creates a new object.
        template<typename obj_t, typename ... args_t>
        obj_t * new_obj(args_t && ... args)
        {
            return memory_t::new_obj<obj_t>(memory, std::forward<args_t>(args) ...);
        }

        // Creates a new node.
        template<typename node_t, typename ... args_t>
        node_t * new_node(args_t && ... args)
        {
            return new_obj<node_t>(memory, 
                    __next_node_index++, std::forward<args_t>(args) ...);
        }

        // Creates a new path node.
        template<typename path_node_t, typename ... args_t>
        path_node_t * new_path_node(args_t && ... args)
        {
            return new_obj<path_node_t>(this, std::forward<args_t>(args) ...);
        }

        // Creates a new pointer array.
        template<typename t>
        t ** new_ptr_array(size_t size)
        {
            return (t **)__heap.acquire(size);
        }

        // Creates a new pointer array.
        template<typename t>
        t ** new_ptr_array(std::vector<t *> & array)
        {
            t ** buffer = new_ptr_array<t>(array.size());
            std::copy(array.begin(), array.end(), buffer);

            return buffer;
        }

        // Copy pointer array.
        template<typename t>
        t ** copy_ptr_array(t ** array, size_t len = (size_t)-1)
        {
            if (len == (size_t)-1)
                len = __len((void **)array);

            t ** buffer = new_ptr_array<t>(len);
            std::copy(array, array + len, buffer);

            return buffer;
        }

        // Returns path node from cache by key.
        analyze_path_node_t * get_node_from_cache(const __path_node_cache_key_t & key);

        // Returns path node from cache by specified nodes.
        analyze_path_node_t * get_node_from_cache(const std::vector<analyze_node_t *> & nodes);

        // Adds node to cache.
        void add_node_to_cache(analyze_path_node_t * node);

        // Adds a branch.
        void add_branch(analyze_branch_node_t * node);

        // Gets a branch.
        analyze_branch_node_t * get_branch(__node_value_t value) const;

        // Returns all branches.
        auto all_branches() const { return _range(__branch_map); }

        // Creates new analyze node with subnodes.
        template<typename itor_t>
        analyze_node_t ** to_stack_path(itor_t begin, itor_t end);

        // Memory management.
        memory_t * const memory;

        X_TO_STRING_IMPL(_T("analyze_tree_build_context_t"))

    private:
        __node_index_t __next_node_index = 0;
        al::heap_t<void * []> __heap;

        std::map<__node_value_t, analyze_branch_node_t *> __branch_map;
        std::map<analyze_node_t *, analyze_path_node_t *> __path_node_relation;
        std::map<__path_node_cache_key_t, analyze_path_node_t *> __path_node_cache;

        typedef al::multikey_t<range_t<analyze_node_t **>, analyze_node_t **>
                                                        __stack_path_cache_key_t;
        std::map<__stack_path_cache_key_t, analyze_node_t **> __stack_path_cache;

        // Returns length of a array.
        static size_t __len(void ** arr);
    };

    //-------- ---------- ---------- ---------- ----------

    // Analyze tree builder.
    class analyze_tree_builder_t : public object_t
    {
    public:

        // Constructor.
        analyze_tree_builder_t(analyze_tree_build_context_t * build_context,
                const char_t * pattern, analyze_tree_builder_provider_t * provider
        )
            : __build_context(build_context)
            , __pattern(pattern), __p(pattern), __codepos_helper(pattern)
            , __provider(provider)
        {
            _A(build_context != nullptr);
            _A(pattern != nullptr);
            _A(provider != nullptr);
        }

        // Build the tree.
        analyze_root_node_t * build();

        X_TO_STRING_IMPL(_T("analyze_tree_builder_t"))

    private:
        const char_t * const __pattern;                     // Pattern.
        const char_t * __p;                                 // Pattern reading position.
        codepos_helper_t __codepos_helper;                  // Codepos helper.
        analyze_tree_builder_provider_t * __provider;       // Tree builder provider.
        __node_value_t __next_custome_branch_value = -128;  // Next custome branch value.
        analyze_tree_build_context_t * __build_context;     // Build context.

        std::map<string_t, __node_value_t> __branch_node_value_map;
        std::map<string_t, __node_value_t> __token_node_value_map;
        std::map<__node_value_t, string_t> __branch_node_name_map;
        std::map<__node_value_t, string_t> __token_node_name_map;

        // Creates a new object.
        template<typename obj_t, typename ... args_t>
        obj_t * __new(args_t && ... args)
        {
            return __build_context->new_obj<obj_t>(std::forward<args_t>(args) ...);
        }

        // Creates a new node.
        template<typename node_t, typename ... args_t>
        node_t * __new_node(args_t && ... args)
        {
            return __build_context->new_node<node_t>(std::forward<args_t>(args) ...);
        }

        // Context for reading branch.
        struct __read_branch_context_t
        {
            std::vector<analyze_branch_ref_node_t *> branch_ref_nodes;
        };

        // Reads branch.
        analyze_branch_node_t * __read_branch(__read_branch_context_t & ctx,
            const string_t & branch_name, const string_t & current_name,
            int level, bool inner = false);

        // Reads branch.
        analyze_branch_node_t * __read_branch(__read_branch_context_t & ctx,
                const string_t & branch_name, int level);

        // Reads name.
        string_t __read_name();

        // Reads token.
        string_t __read_token(string_t & out_tag);

        // Reads subname.
        string_t __read_subname();

        // Reads an expected char.
        void __expect(char_t c);

        // Returns whether next char is expected.
        bool __is_expect(char_t c);

        // Reads next char.
        char_t __next_char();

        // Skip comment.
        bool __skip_comment();

        // Skip whitespace with expected.
        void __skip_whitespace_with_expect();

        // Joins branch names.
        static string_t __join_branch_name(const string_t & name1, const string_t & name2);

        // Returns branch value by name.
        __node_value_t __branch_value(const string_t & name, bool auto_create=true);

        // Returns token value by name.
        __node_value_t __token_value(const string_t & name);

        // Prints branches. ( for debug. )
        void __print_branches();

        // Returns branch name of node value.
        const string_t & __branch_name(const __node_value_t & value);

        // Returns token name of node value.
        const string_t & __token_name(const __node_value_t & value);

        // Creates a format error.
        template<typename ... args_t>
        string_t __format_error(const char_t * format, args_t ... args)
        {
            string_t s = sprintf(format, args ...);
            codepos_t pos = __codepos_helper.pos_of(__p);
            return sprintf(_T("%1%, line: %2%, col: %3%"), s, pos.line, pos.col);
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // Parses a analyze tree.
    analyze_tree_t * parse_analyze_tree(const char_t * pattern, memory_t * memory,
            analyze_tree_builder_provider_t * provider);

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        // Node writer.
        template<typename stream_t>
        class __node_writer_t
        {
        public:

            // Constructor.
            __node_writer_t(stream_t & stream): __stream(stream) { }

            // Writes a node.
            void write(const analyze_node_t * node)
            {
                _A(node != nullptr);

                __write_node(node);
            }

            // Writes a branch.
            void write(const analyze_branch_node_t * branch)
            {
                _A(branch != nullptr);

                __stream << _T("$") << branch->to_string() << _T(": ");
                write((const analyze_node_t *)branch);
            }

            // Writes a tree.
            void write(const analyze_tree_t * tree)
            {
                _A(tree != nullptr);

                for (const analyze_node_t * node : tree->root->nodes.all())
                {
                    const analyze_branch_node_t * branch = 
                            dynamic_cast<const analyze_branch_node_t *>(node);

                    if (branch != nullptr)
                    {
                        write(branch);
                        __stream << std::endl;
                    }
                }
            }

        private:
            stream_t & __stream;

            // Writes node.
            void __write_node(const analyze_node_t * node)
            {
                switch (node->node_type())
                {
                    case __node_type_t::end:
                        break;

                    case __node_type_t::branch: {
                        auto branch_node = (const analyze_branch_node_t *)node;

                        __stream << _T("(");
                        __write_nodes(branch_node->subnodes);
                        __stream << _T(")");

                        __stream << _title(branch_node->repeat_type);

                        __write_nodes(branch_node->nodes);
                    }   break;

                    default: {
                        __stream << _T(" ") << node->to_string()
                                << _title(node->repeat_type) << _T(" ");

                        const analyze_normal_node_t * normal_node = 
                                dynamic_cast<const analyze_normal_node_t *>(node);

                        if (normal_node != nullptr)
                            __write_nodes(normal_node->nodes);
                    }   break;
                }
            }

            // Writes nodes.
            void __write_nodes(const analyze_nodes_t & nodes)
            {
                int index = 0;
                for (const analyze_node_t * subnode : nodes.all())
                {
                    if (index++ > 0)
                        __stream << _T("|");

                    __write_node(subnode);
                }
            }
        };

        // Writes to a stream.
        template<typename stream_t>
        void __write_to_stream(stream_t & stream, const analyze_node_t * node, int level)
        {
            for (int k = 0; k < level; k++)
            {
                stream << _T("  ");
            }
        }
    }

    // Writes a node to a stream.
    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const analyze_node_t * node)
    {
        if (node != nullptr)
            __node_writer_t<stream_t>(stream).write(node);

        return stream;
    }

    // Writes a tree to a stream.
    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const analyze_tree_t * tree)
    {
        if (tree != nullptr)
            __node_writer_t<stream_t>(stream).write(tree);

        return stream;
    }

    ////////// ////////// ////////// ////////// //////////

    // Analyze path node types.
    X_ENUM(analyze_path_node_type_t)

        // Normal path node
        normal,

        // End path node.
        end,

    X_ENUM_END

    typedef analyze_path_node_type_t __path_node_type_t;

    //-------- ---------- ---------- ---------- ----------

    // Path node stack unit.
    struct analyze_path_node_stack_unit_t
    {
        // Constructor.
        analyze_path_node_stack_unit_t() = default;
        analyze_path_node_stack_unit_t(const analyze_node_t * node, int16_t step,
                void * path, const analyze_node_t * entrance, __node_value_t node_value)
            : node(node), step(step)
            , path(path), entrance(entrance), node_value(node_value)
        { }

        // Node.
        const analyze_node_t * node;

        int16_t step;

        void * path;
        const analyze_node_t * entrance;

        __node_value_t node_value;

        // Converts to a string.
        operator string_t() const
        {
            return _str(node);
        }
    };

    typedef analyze_path_node_stack_unit_t __stack_unit_t;

    //-------- ---------- ---------- ---------- ----------

    typedef range_t<__stack_unit_t *> __stack_range_t;
    typedef int16_t __node_count_t;

    // Path node stack types.
    X_ENUM(analyze_path_node_stack_type_t)

        // Normal path node stack type.
        normal, 

        // Branch switched path node stack type.
        branch_switched,

        // All path node stack types, used for enumerate all items.
        all,

    X_ENUM_END

    typedef analyze_path_node_stack_type_t __stack_type_t;

    //-------- ---------- ---------- ---------- ----------

    // Path node stack.
    class analyze_path_node_stack_t : public object_t
    {
    public:

        // Constructor.
        analyze_path_node_stack_t(const analyze_node_t * head, __node_count_t count)
            : head(head), count(count), next(nullptr)
        { }

        const analyze_node_t * head;            // Head
        analyze_path_node_stack_t * next;       // Next path node

        __node_count_t count, normal_count;
        __stack_unit_t units[1];

        // Returns stack unit nodes of specified type.
        __stack_range_t all(__stack_type_t type = __stack_type_t::all) const;

        // Assigns with context and node unit sequence.
        template<typename itor_t>
        void assign(analyze_tree_build_context_t * context, itor_t begin, itor_t end);

        // Creates a new path node stack.
        static analyze_path_node_stack_t * new_stack(analyze_tree_build_context_t * context,
                const analyze_node_t * head, __node_count_t count);

        // Creates a new path node stack.
        template<typename itor_t>
        static analyze_path_node_stack_t * new_stack(analyze_tree_build_context_t * context,
                const analyze_node_t * head, itor_t begin, itor_t end);

        X_TO_STRING_IMPL(_T("analyze_path_node_stack_t"))
    };

    typedef analyze_path_node_stack_t __path_node_stack_t;

    // Writes a stack to a stream.
    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const __path_node_stack_t & stack)
    {
        stream << stack.head->to_string() << _T("(") << (void *)stack.head << _T(") { ");

        auto write_node = [&stream](const analyze_node_t * node) {
            stream << _str(node) << _T("(") << (void *)node << _T(")");
        };

        int index = 0;
        for (const __stack_unit_t & unit : stack.all(__stack_type_t::normal))
        {
            if (index++ > 0)
                stream << _T(", ");

            write_node(unit.node);
        }

        for (const __stack_unit_t & unit : stack.all(__stack_type_t::branch_switched))
        {
            if (index++ > 0)
                stream << _T(", ");

            stream << _T("...");
            write_node(unit.node);
        }

        stream << _T(" }");

        return stream;
    }

    //-------- ---------- ---------- ---------- ----------

    // Path node stacks.
    struct analyze_path_node_stacks_t : public object_t
    {
        // Constructor.
        analyze_path_node_stacks_t(const analyze_path_node_t * src)
            : src(src), next(nullptr) { }

        const analyze_path_node_t * src;
        __path_node_stack_t * stack;
        analyze_path_node_stacks_t * next;

        // On build completed.
        void on_build_complete(analyze_tree_build_context_t * build_context, size_t count);

        // Returns stack of specified head.
        const __path_node_stack_t * stack_of(const analyze_node_t * head) const;

        X_TO_STRING_IMPL(_T("analyze_path_node_stacks_t"))
    };

    typedef analyze_path_node_stacks_t __path_node_stacks_t;

    // Writes stacks to a stream.
    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const __path_node_stacks_t & stacks)
    {
        int index = 0;
        for (const __path_node_stack_t * stack = stacks.stack; stack; stack = stack->next)
        {
            if (index++ > 0)
                stream << _T("; ");

            stream << *stack;
        }

        return stream;
    }

    //-------- ---------- ---------- ---------- ----------

    // Analyze path node.
    class analyze_path_node_t : public object_t
                              , protected al::initializer_t, public no_copy_ctor_t
    {
    public:

        // Constructor.
        analyze_path_node_t(analyze_tree_build_context_t * build_context,
                    std::vector<analyze_node_t *> & nodes)
            : __build_context(build_context)
            , __nodes(build_context->new_ptr_array<analyze_node_t>(nodes))
            , __node_size(nodes.size())
            , __stacks(nullptr)
        {
            _A(nodes.size() > 0);

            __counter++;
        }

        // Returns path node type.
        virtual __path_node_type_t type() const = 0;

        // Returns key.
        __node_key_t get_key() const { return __nodes[0]->key; }

        // Converts to path node cache key.
        __path_node_cache_key_t to_cache_key() const
        {
            return __path_node_cache_key_t(nodes());
        }

        // Returns all nodes.
        range_t<const analyze_node_t **> nodes() const
        {
            return range_t<const analyze_node_t **>(
                (const analyze_node_t **)__nodes,
                (const analyze_node_t **)__nodes + __node_size
            );
        }

        // Returns all nodes.
        range_t<analyze_node_t **> nodes()
        {
            return range_t<analyze_node_t **>(__nodes, __nodes + __node_size);
        }

        // Returns stacks of specified path node.
        const __path_node_stacks_t * stacks_of(const analyze_path_node_t * src_path_node) const;

        // Appends stacks.
        void append_stacks(analyze_path_node_t * src, std::vector<void **> & stacks);

        // Appends stacks.
        void append_stacks(__path_node_stacks_t * stacks);

        // Returns the counter.
        static size_t counter() { return __counter; }

        X_TO_STRING_IMPL(_T("analyze_path_node_t"))

    protected:
        analyze_tree_build_context_t * const __build_context;

    private:
        analyze_node_t ** const __nodes;
        const int __node_size;
        int __stacks_count = 0;
        static const int __use_stacks_map = -1, __stacks_list_max_count = 5;

        typedef std::map<const analyze_path_node_t *, __path_node_stacks_t *> __stacks_map_t;

        union
        {
            __path_node_stacks_t * __stacks;
            __stacks_map_t * __stacks_map;
        };

        static size_t __counter;

        // Build stacks.
        __path_node_stacks_t * __build_stacks(analyze_path_node_t * src,
                                                    std::vector<void **> & stacks);

        typedef std::vector<void **>::iterator __stack_nodes_itor_t;

        // Build a stack.
        __path_node_stack_t * __build_stack(analyze_node_t * head,
                             __stack_nodes_itor_t stacks_begin, __stack_nodes_itor_t stacks_end);
    };

    //-------- ---------- ---------- ---------- ----------

    class analyze_end_path_node_t;
    class analyze_matched_item_t;

    // Analyze normal path node.
    class analyze_normal_path_node_t : public analyze_path_node_t
    {
    public:

        // Constructor.
        template<typename name_t>
        analyze_normal_path_node_t(analyze_tree_build_context_t * build_context, 
                std::vector<analyze_node_t *> & nodes, name_t name)
            : analyze_path_node_t(build_context, nodes)
            , __name(std::forward<name_t>(name))
        { }

        // Enters a path node by key.
        analyze_path_node_t * enter(const __node_key_t & key);

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return __name;
        }

        // Returns type.
        virtual __path_node_type_t type() const override final
        {
            return __path_node_type_t::normal;
        }

        // Enumerate all subnodes.
        template<typename _f_t> void each_subnodes(_f_t f)
        {
            for (auto && it : __subnode_map)
            {
                f(it.first, it.second); // __node_key_t, analyze_path_node_t *
            }
        }

        // Returns all subnode keys.
        svector_t<__node_key_t> all_subnode_keys();

        // Returns a string descripts all sub nodes.
        string_t all_subnodes_string();

    protected:

        // Initialize.
        virtual void __initialize() override;

    private:

        // Information for building node.
        struct __build_node_info_t
        {
            std::set<analyze_node_t *> nodes;
            std::vector<void **> stacks;

            // Appends stack.
            bool append_stack(void ** stack);

            // Returns whether a stack exists.
            bool __exist_stack(void ** stack);
        };

        typedef std::vector<analyze_node_t *> __nodes_t;
        typedef std::map<__node_key_t, __nodes_t> __nodes_map_t;
        typedef std::map<__node_key_t, __build_node_info_t> __nodeset_map_t;

        // Context for append node.
        struct __append_ctx_t
        {
            __nodeset_map_t nodes_map;
            std::vector<__node_unit_t> branch_stack;
            __node_unit_t entrance = nullptr;

            // Pushes a node unit.
            void branch_stack_push(__node_unit_t u, __node_unit_t entrance = nullptr);

            // Pops a node unit.
            void branch_stack_pop();

            al::heap_t<void * []> __analyze_node_heap;

            // Returns current branch stack.
            void ** current_branch_stack(analyze_tree_build_context_t * context);

            // Returns current branch stack string.
            string_t current_branch_stack_string();

            // Prints stack.
            void print_stack() const;
        };

        typedef al::multikey_t<range_t<__node_key_t *>, __node_key_t> __multipy_map_key_t;
        typedef std::map<__multipy_map_key_t, analyze_normal_path_node_t *> __multipy_map_t;

        std::map<__node_key_t, analyze_path_node_t *> __subnode_map;
        string_t __name;

        // Append subnodes.
        void __append_subnodes(__append_ctx_t & ctx, analyze_nodes_t & nodes,
                                                    analyze_node_t * parent = nullptr);

        // Append subnodes.
        void __append_subnodes(__append_ctx_t & ctx, __node_unit_t u);

        // Append branch subnodes.
        void __append_branch_subnodes(__append_ctx_t & ctx, __node_unit_t u,
                                                analyze_branch_node_t * branch);

        // Append current stack.
        bool __append_current_stack(__append_ctx_t & ctx, __build_node_info_t & info,
                                                    __node_unit_t new_top);

        // Gets branch node.
        analyze_branch_node_t * __get_branch_node(__node_value_t value);

        // Prints the path node.
        void __print();
    };

    typedef analyze_path_node_t __path_node_t;

    //-------- ---------- ---------- ---------- ----------

    // Analyze end path node.
    class analyze_end_path_node_t : public analyze_path_node_t
    {
    public:
        using analyze_path_node_t::analyze_path_node_t;

        // Converts to a string.
        virtual const string_t to_string() const override
        {
            return _T("<end>");
        }

        // Returns path node type.
        virtual __path_node_type_t type() const override final
        {
            return __path_node_type_t::end;
        }

        // Returns end node.
        analyze_end_node_t * end_node() const
        {
            return (analyze_end_node_t *)*nodes().begin();
        }

    protected:

        // Initialize.
        virtual void __initialize() override { }

    };

    ////////// ////////// ////////// ////////// //////////

    // Analyze matched data.
    struct analyze_matched_item_t
    {
        const analyze_node_t *  node;
        __node_value_t          value;

        __tag_t * begin_tag, * end_tag;

        operator string_t() const
        {
            return _str(node);
        }
    };

    ////////// ////////// ////////// ////////// //////////

    typedef al::walk_list_t<__stack_node_t *> __leaves_t;

    class analyze_state_t : public object_t
    {
        friend class analyze_context_t;

    public:
        analyze_state_t() = default;

    private:
        lr_tree_state_t<__stack_node_value_t> __tree_state;
        size_t __matched_item_count;
    };

    //-------- ---------- ---------- ---------- ----------

    class analyzer_element_reader_t;

    typedef std::vector<analyze_matched_item_t> analyze_matched_items_t;

    // Context for analyze.
    class analyze_context_t : public object_t
    {
        friend class __stack_node_raise_matched_event_action_t;
        friend class __analyzer_t;

    public:

        // Constructor.
        analyze_context_t(lang_t * lang, analyze_tree_t * tree, analyzer_element_reader_t * reader);

        // Matches the next token.
        void go(const __node_key_t * keys, __tag_t * tag, __node_value_t * out_value = nullptr);

        // Save current state.
        analyze_state_t keep_state();

        // Restore state.
        void restore(const analyze_state_t & state);

        // Reset state.
        void reset();

        // Returns current analyze tree.
        __stack_node_t * current_analyze_tree() { return __stack_root; }

        // Returns all matched actions.
        auto all_matched_items() { return _range(__matched_items); }

        // Detect next possible keys.
        __node_keys_t detect_next_keys();

        X_TO_STRING_IMPL(_T("analyze_context_t"))

        // Disable trace in trace model.
        bool disable_trace = false;

    private:
        lang_t * __lang;                                // Language.
        lang_service_helper_t __service_helper;         // Service helper
        token_property_cache_t<token_property_t> __token_property_cache;

        analyzer_element_reader_t * __reader;           // Reader.
        analyze_tree_t * __tree;                        // Analyzze tree.
        __stack_node_t * __stack_root = nullptr;        // Stack root.
        al::xheap_t __sn_heap;                          // A heap, memory management.

        template<typename t> using __w_t = al::respool_wrapper_t<t>;
        __w_t<__stack_node_t> __stack_node_factory;
        al::heap_t<__stack_node_assign_key_action_t> __assign_key_action_factory;
        al::heap_t<__stack_node_raise_matched_event_action_t> __raise_matched_event_action_factory;

        __leaves_t __leaves;                            // Leaves
        analyze_matched_items_t __matched_items;        // Matched items.

        typedef al::svector_t<__stack_node_t *, 2> __stack_node_vector_t;

        //-------- ---------- ---------- ---------- ----------

        // Arguments for push node to stack.
        struct __stack_push_args_t
        {
            // Constructor.
            __stack_push_args_t(analyze_context_t * owner, const __node_key_t * keys, __tag_t * tag,
                __node_value_t * out_value = nullptr)
                : __owner(owner), keys(keys), tag(tag), out_value(out_value)
            { }

            analyze_context_t * __owner;        // It's owner.
            __tag_t * tag;                      // Tag
            const __node_key_t * keys;          // Keys
            __node_value_t * out_value;         // Return node value.

            __stack_node_vector_t new_stack_nodes;

            // Appends a new stack node.
            void append_new_stack_node(__stack_node_t * leaf, __node_value_t node_value);
        };

        //-------- ---------- ---------- ---------- ----------

        typedef size_t __stack_push_task_state_t;

        // Stack push task.
        struct __stack_push_task_t
        {
            // Constructor.
            __stack_push_task_t(__stack_push_args_t & push_args) : push_args(push_args) { }

            __stack_push_args_t & push_args;    // Push arguments.

            al::svector_t<void *, 10> stack_paths;
            size_t record_stack_unit(const __stack_unit_t & unit);

            // Returns current state.
            __stack_push_task_state_t current_state();

            // Restore to the specified state.
            void restore(const __stack_push_task_state_t & state);
        };

        //-------- ---------- ---------- ---------- ----------

        enum __grow_model_t { __overwrite, __grow_only };

        // Arguments for growing stack.
        struct __stack_grow_args_t
        {
            // Constructor.
            __stack_grow_args_t(__stack_push_task_t & task, __grow_model_t model = __overwrite) :
                task(task), model(model), __from_index(task.push_args.new_stack_nodes.size())
            { }

            __stack_push_task_t & task;     // Push task.
            bool found = false;             // Record whether it's founded.
            __grow_model_t model;           // Grow model.
            size_t apply_state = 0;         // Apply state.

            // Returns the begin iterator.
            auto begin() const { return task.push_args.new_stack_nodes.begin() + __from_index; }

            // Returns the end iterator.
            auto end()   const { return task.push_args.new_stack_nodes.end(); }

            size_t __from_index;

            // Checks stack unit.
            bool check_stack_unit(const __stack_unit_t & unit);
        };

        //-------- ---------- ---------- ---------- ----------

        // Stack item.
        struct __stack_item_t
        {
            const __path_node_stack_t * stack;
            analyze_normal_path_node_t * dst;
        };

        typedef al::svector_t<__stack_item_t, 2> __stack_items_t;

        //-------- ---------- ---------- ---------- ----------

        // Arguments for pop node from stack.
        struct __stack_pop_args_t
        {
            __stack_pop_args_t(__tag_t * tag) : end_tag(tag) { }

            __tag_t * end_tag;
            __stack_node_vector_t new_stack_nodes;
        };

        //-------- ---------- ---------- ---------- ----------

        // Arguments for check end node.
        struct __check_end_node_args_t
        {
            // Constructor.
            __check_end_node_args_t(__stack_push_task_t & task)
                : task(task), __from_index(task.push_args.new_stack_nodes.size())
            {
                task_state = task.current_state();
            }

            // Constructor.
            __check_end_node_args_t(__check_end_node_args_t & args)
                : __check_end_node_args_t(args.task) { }

            __stack_push_task_t & task;

            // Returns the begin iterator.
            auto begin() const { return task.push_args.new_stack_nodes.begin() + __from_index; }

            // Returns the end iterator.
            auto end()   const { return task.push_args.new_stack_nodes.end(); }

            size_t __from_index;
            __stack_push_task_state_t task_state;

            // Destructor.
            ~__check_end_node_args_t()
            {
                task.restore(task_state);
            }
        };

        //-------- ---------- ---------- ---------- ----------

        // Reset.
        void __reset();

        // When pushes a node to stack.
        void __stack_push(__stack_push_args_t & args, __stack_node_t * stack_node);

        // When stack grows.
        void __stack_grow(__stack_grow_args_t & args, __stack_node_t * stack_node);

        // Do stack grow.
        void __do_stack_grow(__stack_grow_args_t & args, __stack_node_t * stack_node,
                                                 const __stack_items_t & stack_items);

        // Do stack grow.
        void __do_stack_grow(__stack_grow_args_t & args, __stack_node_t * stack_node,
                                                 const __stack_item_t & stack_item);

        // Check end node.
        void __check_end_node(__check_end_node_args_t & args, __stack_node_t * parent,
                    __stack_node_t * current, __node_unit_t end_unit,
                    __stack_node_action_t * next_action, int weight);

        // Merge leaves.
        void __merge_leaves();

        // Pick final leaves when completed.
        void __pick_final_leaves();

        // Stack pop.
        void __stack_pop(__stack_pop_args_t & args, __stack_node_t * stack_node);

        // Do stack pop.
        void __do_stack_pop(__stack_pop_args_t & args, __stack_node_t * stack_node);

        // Execute actions.
        void __execute_actions();

        // Execute actions.
        bool __execute_actions(__stack_node_t * leaf);

        // Execute action.
        void __execute_action(__stack_node_action_t * action);

        // Apend node action.
        void __append_node_action(__stack_node_t * node, __stack_node_action_t * action,
                                                              bool push_back = false);

        // Append a matched event action.
        void __append_raise_matched_event_action(__stack_node_t * node,
                __node_value_t value, __tag_t * begin_tag, __tag_t * end_tag,
                bool push_back = false);

        // Creates a new raise matched event action.
        __stack_node_raise_matched_event_action_t * __new_raise_matched_event_action(
                const analyze_branch_ref_node_t * branch_ref,
                __tag_t * begin_tag, __tag_t * end_tag,
                const analyze_node_t * begin_node,
                __stack_node_action_t * next_action = nullptr);

        // Removes leaf unitl prediacte function.
        template<typename pred_t>
        void __remove_leaf_until(__stack_node_t * stack_node, pred_t pred);

        // Removes specified node.
        void __remove_leaf(__stack_node_t * stack_node);

        // Removes stack node.
        void __remove_stack_node(__stack_node_t * stack_node);

        // Creates new stack node.
        template<typename stack_node_value_t>
        __stack_node_t * __new_stack_node(stack_node_value_t && value);

        // Appends leaf sequence.
        template<typename itor_t> void __append_leaves(itor_t begin, itor_t end);

        // Appends leaves in container.
        template<typename container_t> void __append_leaves(container_t & container);

        // Appends matched items.
        void __append_matched_items(const analyze_node_t * node, __node_value_t value,
                                    __tag_t * begin_tag = nullptr, __tag_t * end_tag = nullptr);

        // Returns whether it's invisible, e.g. comments.
        bool __is_invisible(token_value_t value);

        // Clear actions.
        void __clear_actions();

        // Check end nodes in detecting next keys model.
        void __detect_check_end_node(__node_keys_t & out_keys,
                __stack_node_t * parent, __stack_node_t * current, __node_unit_t end_unit);
    };

    ////////// ////////// ////////// ////////// //////////

    // Context for token reader.
    class __token_reader_context_t : public token_reader_context_t
    {
    public:
        using token_reader_context_t::token_reader_context_t;

        // Returns token reader of language.
        token_reader_t * token_reader_of(lang_id_t lang_id);

    private:
        std::map<lang_id_t, token_reader_t *> __token_reader_cache;
    };

    ////////// ////////// ////////// ////////// //////////

   //-------- ---------- ---------- ---------- ----------

    // Analyze element read models.
    X_ENUM(analyzer_element_read_model_t)

        // Normal.
        normal,

        // Read all
        all,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Analyzer element reader.
    class analyzer_element_reader_t
    {
        typedef analyzer_element_read_model_t __model_t;

    public:

        // Constructor.
        analyzer_element_reader_t(__token_reader_context_t & context, code_section_t ** &p_section);

        // Returns next eleent.
        analyze_element_t * next();

        // Reached the end of this reader.
        bool at_end();

        // Returns element at specified tag.
        analyze_element_t & operator[](__tag_t * tag)
        {
            return element_at(tag, __model_t::normal);
        }

        // Returns element at specified tag.
        analyze_element_t & element_at(__tag_t * tag, __model_t model = __model_t::normal);

        // Returns element at specifed index.
        analyze_element_t * operator[](size_t index)
        {
            return element_at(index);
        }

        // Returns element at specified index.
        analyze_element_t * element_at(size_t index);

        // Returns next element from specified tag.
        __tag_t * next_tag(__tag_t * tag, __model_t model = __model_t::normal);

        // Returns current position.
        size_t position() const { return __index; }

        // Sets current position.
        void set_position(size_t position) { __index = position; }

        // Returns tag count.
        size_t tag_count() const { return __items.size(); }

        // Returns tag at specified index.
        __tag_t * tag_at(size_t index)
        {
            _A(index < tag_count());
            return __items[index].get_tag();
        }

        // Returns the begin tag.
        __tag_t * begin_tag() { return __items[0].get_tag(); }

        // Returns the end tag.
        __tag_t * end_tag()   { return __items[__items.size() - 1].get_tag(); }

        // Returns size of elements.
        size_t size() const { return __items.size(); }

        // Insert token.
        analyze_element_t * insert(size_t index, token_t * token);

        // Insert element.
        analyze_element_t * insert(size_t index, const analyze_element_t & element);

        // Delete element.
        void delete_(size_t index);

        // Append an ast node.
        void append_ast(__tag_t * from_tag, __tag_t * end_tag,
                ast_node_t * ast_node, const analyze_node_t * node);

        // Returns root ast node.
        ast_node_t * get_root_ast();

        // Returns current code section.
        code_section_t * current_code_section() { return __section; }

    private:

        // Ast flag.
        struct __ast_flag_t : public object_t
        {
            // Constructor.
            __ast_flag_t(ast_node_t * ast_node, __tag_t * end_tag,
                                    const analyze_node_t * matched_node)
                : ast_node(ast_node), end_tag(end_tag), element(ast_node)
            {
                element.matched_node = matched_node;
            }

            __tag_t * end_tag;              // The end tag.
            ast_node_t * ast_node;          // The ast node.
            analyze_element_t element;      // Element.

            __ast_flag_t * next = nullptr;  // Next flag.

            X_TO_STRING_IMPL(_T("__ast_flag_t"))
        };

        //-------- ---------- ---------- ---------- ----------

        // Aanlyze element.
        struct __item_t : analyze_element_t
        {
            typedef analyze_element_t __super_t;

            using __super_t::__super_t;

            __item_t(const analyze_element_t & element) : __super_t(element) { }

            __ast_flag_t * flag = nullptr;

            __tag_t * get_tag() { return &tag; }
        };

        //-------- ---------- ---------- ---------- ----------

        __token_reader_context_t & __context;

        code_section_t ** &  __p_section;
        code_section_t *     __section;
        int __depth;

        token_reader_t *     __reader;
        token_enumerator_t * __enumerator;

        al::heap_t<__item_t> __items;
        size_t               __index = 0;
        al::xheap_t          __heap;

        // Reads next element.
        bool __read_next_element();

        // Appends a element.
        template<typename element_t>
        void __append_element(element_t element)
        {
            __items.new_obj(element, __items.size());
        }

        // Returns the end element.
        analyze_element_t * __end_element()
        {
            return __items.new_obj(analyze_element_type_t::__unknown__, __items.size());
        }
    };

    //-------- ---------- ---------- ---------- ----------

    class lang_ast_build_elements_t;

    // Builder element iterator.
    class lang_ast_builder_element_iterator_t
    {
        typedef analyze_element_t __element_t;
        typedef lang_ast_builder_element_iterator_t __self_t;
        typedef lang_ast_build_elements_t __elements_t;

    public:

        // Constructor.
        lang_ast_builder_element_iterator_t(const __elements_t & elements, token_tag_t * tag)
            : __elements(elements), __tag(tag)
        { }

        // Returns element.
        __element_t & operator * () const;

        // Moves to next iterator.
        __self_t & operator ++ ();

        // Moves to next iterator.
        __self_t operator ++ (int);

        // Returns whether two iterators are equals.
        bool operator == (const __self_t & other) const
        {
            return __tag == other.__tag;
        }

        // Returns whether two iterators are not equals.
        bool operator != (const __self_t & other) const
        {
            return !operator == (other);
        }

    private:
        const __elements_t & __elements;
        token_tag_t * __tag;
    };

    //-------- ---------- ---------- ---------- ----------

    // Ast build elements.
    class lang_ast_build_elements_t : public ast_build_elements_t
    {
        typedef analyze_element_t __element_t;
        typedef lang_ast_build_elements_t __self_t;
        typedef lang_ast_builder_element_iterator_t __itor_t;
        typedef analyzer_element_read_model_t __model_t;

    public:

        // Constructor.
        lang_ast_build_elements_t(analyzer_element_reader_t & reader,
                token_tag_t * from_tag, token_tag_t * to_tag,
                __model_t model = __model_t::normal)
            : __reader(reader), __begin(*this, from_tag), __end(*this, to_tag), __current(__begin)
            , __model(model)
        { }

        // Returns element by the tag.
        __element_t & get(token_tag_t * tag) const
        {
            return __reader.element_at(tag, __model);
        }

        // Returns the next element of the tag.
        token_tag_t * next_tag(token_tag_t * tag) const
        {
            return __reader.next_tag(tag, __model);
        }

        // Returns begin iterator.
        __itor_t begin() const { return __begin; }

        // Returns end iterator.
        const __itor_t end() const { return __end; }

        // Reads next element.
        virtual analyze_element_t * next() override
        {
            if (__current == __end)
                return nullptr;

            return std::addressof(*__current++);
        }

    private:
        analyzer_element_reader_t & __reader;
        const __itor_t __begin, __end;
        __itor_t __current;
        __model_t __model;
    };

    // Returns element.
    X_INLINE analyze_element_t & lang_ast_builder_element_iterator_t::operator * () const
    {
        return __elements.get(__tag);
    }

    // Moves to next iterator.
    X_INLINE lang_ast_builder_element_iterator_t &
        lang_ast_builder_element_iterator_t::operator ++ ()
    {
        __tag = __elements.next_tag(__tag);
        return *this;
    }

    // Moves to next iterator.
    X_INLINE lang_ast_builder_element_iterator_t
        lang_ast_builder_element_iterator_t::operator ++ (int)
    {
        lang_ast_builder_element_iterator_t this_ = *this;
        __tag = __elements.next_tag(__tag);

        return this_;
    }

    ////////// ////////// ////////// ////////// //////////

    struct analyzer_result_t
    {
        // All matched datas.
        analyze_matched_items_t matched_items;
    };

    //-------- ---------- ---------- ---------- ----------

    // Aanlyzer.
    class __analyzer_t : public object_t
    {
    public:

        // Constructor.
        __analyzer_t(ast_context_t & ast_context, lang_t * lang,
                    analyze_tree_t * tree, analyzer_element_reader_t * reader)
            : __ast_context(ast_context)
            , __compile_context(ast_context.compile_context)
            , __lang(lang), __tree(tree), __reader(reader)
            , __context(lang, tree, reader)
        {
            _A(lang != nullptr);
            _A(tree != nullptr);
            _A(reader != nullptr);
        }

        analyzer_result_t analyze();

        X_TO_STRING_IMPL(_T("__analyzer_t"))

    private:
        ast_context_t             & __ast_context;      // Ast context.
        compile_context_t         & __compile_context;  // Compile context.
        lang_t                    * __lang;             // Language.
        analyze_tree_t            * __tree;             // Tree.
        analyzer_element_reader_t * __reader;           // Reader.
        analyze_context_t           __context;          // Context.
        analyze_element_t         * __element;          // Current element.

        static const size_t __empty_break_point = max_value<size_t>();
        static const analyze_element_type_t __end = analyze_element_type_t::__unknown__;

        typedef logic_error_t<analyze_tree_error_t> __error_t;

        // Try to do analyzing, return true when succeed.
        bool __try_analyze(size_t break_point = __empty_break_point);

        // Do analyzing.
        void __analyze(size_t break_point = __empty_break_point);

        // Try add missing elements when parsing error.
        bool __try_insert_missing_elements(analyze_element_t * replaced_element = nullptr);

        // Try delete unexpected elements when parsing error.
        bool __try_delete_unexpected_elements(int min_steps);

        // Try delete unexpected elements and insert missing elements when parsing error.
        bool __try_replace_unexpected_elements(int min_steps);

        // Try specified keys, returns possible keys and the best matched key.
        __node_keys_t __try_missing_keys(__node_keys_t & keys, __node_key_t * out_best_key);

        // Create a new element from specified node key.
        analyze_element_t __element_from_key(__node_key_t key, const code_unit_t * cu);

        // Push element.
        void __push_element(analyze_element_t * element);

        // Detect a long match distance when format error, 
        int __detect(__node_key_t key, int max_steps, bool * out_reach_end = nullptr);

        // Pushes a token.
        void __push_token(token_t * token, __tag_t * tag);

        // Pushes an ast node.
        void __push_ast_node(ast_node_t * node, __tag_t * tag);

        // Pushes an current normal path node with keys and tag.
        analyze_normal_path_node_t * __push(analyze_normal_path_node_t * current,
                                            const __node_key_t * keys, __tag_t * tag);

        // Pushes with keys and tag.
        void __push(const __node_key_t * keys, __tag_t * tag, __node_value_t * out_value = nullptr);

        // Pushes with key and tag.
        void __push(__node_key_t key, __tag_t * tag);

        // Pushes end.
        void __push_end(__tag_t * tag);

        // Process format errors.
        void __process_format_error(const __error_t & e);

        // Returns possible value count.
        static size_t __possible_value_count(const token_value_t * possible_values);

        // Analyze state.
        struct __state_t
        {
            size_t position;
            analyze_state_t analyze_state;
        };

        // Save current state.
        __state_t __keep_state();

        // Set current state.
        void __restore_state(const __state_t & state);

        // Log format error.
        void __log_format_error(const string_t & error, analyze_element_t & element,
                        const code_unit_t * cu = nullptr);

        //-------- ---------- ---------- ---------- ----------

        class __state_keep_guard_t
        {
        public:
            __state_keep_guard_t(__analyzer_t * analyzer)
                : __analyzer(analyzer)
            {
                __state = analyzer->__keep_state();
            }

            ~__state_keep_guard_t()
            {
                __analyzer->__restore_state(__state);
            }

        private:
            __analyzer_t * __analyzer;
            __state_t __state;
        };
    };

    ////////// ////////// ////////// ////////// //////////

    // Analyzer.
    class analyzer_t : public object_t
    {
    public:

        // Constructor.
        analyzer_t(lang_t * lang, analyze_tree_t * tree)
            : __lang(lang), __tree(tree)
        {
            _A(lang != nullptr);
            _A(tree != nullptr);
        }

        // Analyze with reader.
        analyzer_result_t analyze(ast_context_t & ast_context, analyzer_element_reader_t * reader);

        static const int32_t __cache_key__ = 1;

        X_TO_STRING_IMPL(_T("analyzer_t"))

    private:
        analyze_tree_t * __tree;
        lang_t * __lang;
    };

    ////////// ////////// ////////// ////////// //////////

    // Analyze ast node.
    ast_node_t * analyze_ast(compile_context_t & context, code_t * code);

    ////////// ////////// ////////// ////////// //////////

    // Ast build error codes.
    X_ENUM(ast_build_error_code_t)

        // Build failed.
        build_failed,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    // Ast factory.
    class ast_factory_t : public object_t
    {
    public:

        // Constructor.
        ast_factory_t(ast_context_t & ast_context, analyzer_element_reader_t & reader)
            : __ast_context(ast_context), __reader(reader)
        {
            __build_service = require_lang_service<lang_ast_build_service_t>(
                ast_context.lang
            );
        }

        // Do ast node match.
        void match(const analyze_matched_items_t & items);

        // Returns build result.
        ast_node_t * get_result();

        X_TO_STRING_IMPL(_T("ast_factory_t"))

    private:
        ast_context_t & __ast_context;
        lang_ast_build_service_t * __build_service;
        analyzer_element_reader_t & __reader;

        ast_node_t * __build_ast(lang_ast_build_args_t & args);
    };

    ////////// ////////// ////////// ////////// //////////

    // Arguments for ast build completed.
    struct ast_builder_completed_args_t
    {
        code_unit_t * code_unit;
    };

    //-------- ---------- ---------- ---------- ----------

    // Areguments for apply token.
    struct ast_builder_apply_token_args_t
    {

    };

    //-------- ---------- ---------- ---------- ----------

    // Arguments for Apply ast node.
    struct ast_builder_apply_ast_args_t
    {
        const char_t * flag;
    };

    //-------- ---------- ---------- ---------- ----------

    // Ast node builder.
    class ast_builder_t : public object_t
    {
    public:

        // Constructor.
        ast_builder_t(ast_context_t & context, lang_ast_build_args_t & args)
            : __context(context), __args(args)
        { }

        // Builds nodes.
        ast_node_t * build();

        X_TO_STRING_IMPL(_T("ast_builder_t"))

    protected:
        typedef analyze_element_t               __element_t;
        typedef ast_builder_completed_args_t    __completed_args_t;
        typedef ast_builder_apply_token_args_t  __apply_token_args_t;
        typedef ast_builder_apply_ast_args_t    __apply_ast_args_t;

        // Creates ast node.
        virtual ast_node_t * create() = 0;

        // Applies token.
        virtual void apply_token(token_t * token, __apply_token_args_t & args) = 0;

        // Applies ast node.
        virtual void apply_ast(ast_node_t * node, __apply_ast_args_t & args) = 0;

        // Build completed.
        virtual void completed(__completed_args_t & args) = 0;

        ast_context_t & __context;
        lang_ast_build_args_t & __args;

        // Creates a new object.
        template<typename t, typename ... args_t>
        t * __new_obj(args_t && ... args)
        {
            return __context.compile_context.new_obj<t>(std::forward<args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Ast builder with ast_node_t.
    template<typename _ast_node_t>
    class tast_builder_t : public ast_builder_t
    {
        typedef _ast_node_t     __ast_node_t;
        typedef ast_builder_t   __super_t;

    public:
        using ast_builder_t::ast_builder_t;

        __ast_node_t * operator ->() { return this->__node; }
        const __ast_node_t * operator ->() const { return this->__node; }

        __ast_node_t & operator *()  { return *this->__node; }
        const __ast_node_t & operator *() const { return *this->__node; }

    protected:

        // Creates a ast node.
        virtual ast_node_t * create() override
        {
            __node = __new_obj<_ast_node_t>(__context);
            return __node;
        }

        // Applies token.
        virtual void apply_token(token_t * token, __apply_token_args_t & args) override { }

        // Applies ast node.
        virtual void apply_ast(ast_node_t * node, __apply_ast_args_t & args) override { }

        // Build completed.
        virtual void completed(__completed_args_t & args) override { }

        _ast_node_t * __node = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////

}   // X_ROOT_NS::modules::compile

#endif // __COMPILE_ANALYZE_TREE_H__
