
#ifndef __COMPILE_ANALYZE_TREE_H__
#define __COMPILE_ANALYZE_TREE_H__


namespace X_ROOT_NS { namespace modules { namespace compile {

    namespace
    {
        using namespace core;
    }

    ////////// ////////// ////////// ////////// //////////

    typedef analyze_node_type_t __node_type_t;
    typedef uint32_t            __node_index_t;
    typedef token_tag_t         __tag_t;

    //-------- ---------- ---------- ---------- ----------

    typedef analyze_node_value_t __node_value_t;
    typedef uint_type_t<sizeof(__node_value_t) + sizeof(__node_type_t)>
                                        __analyze_node_key_underlying_t;

    struct analyze_node_key_t
        : public compare_operators_t<analyze_node_key_t, __analyze_node_key_underlying_t>
    {
        typedef __analyze_node_key_underlying_t __underlying_t;

        analyze_node_key_t() = default;

        analyze_node_key_t(__node_type_t type, __node_value_t value = (__node_value_t)0)
            : value(value), type(type), __reserved(0) { }

        __node_value_t   value  : sizeof(__node_value_t) * 8;
        __node_type_t    type   : sizeof(__node_type_t)  * 8;

        #define __RESERVED_INT_SIZE \
            (sizeof(__underlying_t) - sizeof(__node_value_t) - sizeof(__node_type_t))

        __underlying_t __reserved  : __RESERVED_INT_SIZE * 8;

        #undef __RESERVED_INT_SIZE

        operator __underlying_t() const { return *(__underlying_t *)this; }

        explicit operator string_t() const
        {
            return sprintf(_T("%1%:%2%"), _title(type), value);
        }
    };

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const analyze_node_key_t & key)
    {
        return stream << ((string_t)key).c_str();
    }

    typedef analyze_node_key_t __node_key_t;
    const __node_key_t __empty_node_key(__node_type_t::empty);
    const __node_key_t __end_node_key(__node_type_t::end);

    //-------- ---------- ---------- ---------- ----------

    X_INLINE __node_key_t to_token_key(__node_value_t value)
    {
        return __node_key_t(__node_type_t::token, value);
    }

    X_INLINE __node_key_t to_branch_ref_key(__node_value_t value)
    {
        return __node_key_t(__node_type_t::branch_ref, value);
    }

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(analyze_node_repeat_type_t)

        once, star, plus, question,

    X_ENUM_END

    typedef analyze_node_repeat_type_t __repeat_type_t;

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(analyze_tree_error_t)

        format_error,

        unknown_branch,

        unknown_token,

        parse_error,

        stack_closed,

        unknown_element_type,

        overflow,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    X_ENUM_(special_node_value_t, sizeof(__node_value_t))
        
        root    =  0,

        unknown = -1,

        end     = -2,

    X_ENUM_END

    const __node_value_t __unknown_node_value = (__node_value_t)special_node_value_t::unknown;
    const __node_value_t __end_node_value     = (__node_value_t)special_node_value_t::end;
    const __node_value_t __root_node_value    = (__node_value_t)special_node_value_t::root;

    ////////// ////////// ////////// ////////// //////////

    class analyze_node_t;
    class analyze_end_node_t;
    //typedef int16_t __hstep_t;

    struct analyze_node_unit_t : compare_operators_t<analyze_node_unit_t, analyze_node_t *>
    {
        analyze_node_unit_t(analyze_node_t * node/*, __hstep_t hstep = 0*/)
            : node(node) /*, hstep(hstep)*/
        { }

        analyze_node_unit_t(std::nullptr_t null = nullptr)
            : analyze_node_unit_t((analyze_node_t *)nullptr)
        { }

        analyze_node_t * node;
        //__hstep_t hstep;

        operator analyze_node_t * () const { return node; }

        bool operator == (std::nullptr_t) const { return node == nullptr; }
        bool operator != (std::nullptr_t) const { return node != nullptr; }

        analyze_node_unit_t & operator = (std::nullptr_t)
        {
            node = nullptr;
            //hstep = 0;

            return *this;
        }

        operator string_t() const { return _str(node); }

        analyze_node_t * operator ->() { return node; }
        const analyze_node_t * operator ->() const { return node; }

        analyze_node_t * operator *() { return node; }
        const analyze_node_t * operator *() const { return node; }
    };

    typedef analyze_node_unit_t __node_unit_t;

    //-------- ---------- ---------- ---------- ----------

    struct analyze_nodes_t
    {
    public:
        void append(const __node_unit_t & u);
        void append(analyze_node_t * node/*, __hstep_t hstep = 0*/);
        void append(analyze_nodes_t & nodes);

        bool remove(const __node_unit_t & u);
        bool replace(const __node_unit_t & old, const __node_unit_t & new_);
        bool contains(const __node_unit_t & u);

        auto all() const { return _range(__node_units); }

        auto begin() const { return __node_units.begin(); }
        auto end() const { return __node_units.end(); }

        size_t size() const { return __node_units.size(); }

        __node_unit_t last() const { return __last; }
        __node_unit_t end_unit() const { return __end; }

        void swap(analyze_nodes_t & other);
        void clear();

    private:
        std::set<__node_unit_t> __node_units;
        __node_unit_t __end  = nullptr;
        __node_unit_t __last = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////

    typedef int16_t __weight_t;

    class analyze_branch_node_t;
    class analyze_node_t : public object_t, public no_copy_ctor_t
                         , public equals_t<analyze_node_t>
                         , public less_t<analyze_node_t>
    {
    public:
        analyze_node_t(__node_index_t index, __node_key_t key,
                        const string_t & flag = _T(""), __weight_t weight = 0)
            : index(index), key(key), flag(flag), weight(weight) { }

        virtual __node_type_t node_type() const = 0;

        __repeat_type_t repeat_type = __repeat_type_t::once;

        const __node_index_t index;
        const __node_key_t   key;
        const string_t       flag;
        const __weight_t     weight;
        analyze_branch_node_t * branch = nullptr;

        __node_type_t this_type() const { return key.type; }

        virtual bool equals(const analyze_node_t & node) const override;
        virtual bool less(const analyze_node_t & node) const override;
        virtual const string_t to_string() const override;

    protected:
        virtual const string_t __to_string() const
        {
            return _T("node");
        }
    };

   //-------- ---------- ---------- ---------- ----------

    class analyze_normal_node_t : public analyze_node_t
    {
    public:
        analyze_normal_node_t(__node_index_t index, __node_key_t key,
                const string_t & flag = _T(""), __weight_t weight = 0)
            : analyze_node_t(index, key, flag, weight) { }

        analyze_nodes_t nodes;

        __node_unit_t get_end_unit() const
        {
            return nodes.end_unit();
        }
    };

    //-------- ---------- ---------- ---------- ----------

    class analyze_token_node_t : public analyze_normal_node_t
    {
    public:
        analyze_token_node_t(__node_index_t index, __node_value_t value,
                const string_t & name, const string_t & flag = _T(""), __weight_t weight = 0)
            : analyze_normal_node_t(index, __node_key_t(__node_type_t::token, value), flag, weight)
            , value(value), name(name) { }

        virtual __node_type_t node_type() const override final
        {
            return __node_type_t::token;
        }

        const __node_value_t value;
        const string_t name;

        virtual bool equals(const analyze_node_t & node) const override
        {
            return node.node_type() == __node_type_t::token
                && ((const analyze_token_node_t &)node).value == value;
        }

        virtual bool less(const analyze_node_t & node) const override
        {
            const __node_type_t type1 = __node_type_t::token, type2 = node.node_type();
            if(type1 != type2)
                return type1 < type2;

            return value < ((const analyze_token_node_t &)node).value;
        }

    protected:
        virtual const string_t __to_string() const override
        {
            return name;
        }
    };

    //-------- ---------- ---------- ---------- ----------

    class analyze_branch_node_t : public analyze_normal_node_t
    {
    public:
        analyze_branch_node_t(__node_index_t index, __node_value_t value,
            const string_t & name, int level, bool inner = false,
            const string_t & flag = _T(""), __weight_t weight = 0)
            : analyze_normal_node_t(index, __node_key_t(__node_type_t::branch, value), flag, weight)
            , name(name), level(level), inner(inner) { }

        const string_t name;
        const int level;
        bool inner;
        analyze_nodes_t subnodes;

        virtual __node_type_t node_type() const override final
        {
            return __node_type_t::branch;
        }

        bool illusory() const { return subnodes.end_unit() != nullptr; }

    protected:
        virtual const string_t __to_string() const override
        {
            if(name.empty())
                return _T("EMPTY");

            return name;
        }
    };

    //-------- ---------- ---------- ---------- ----------

    class analyze_root_node_t : public analyze_branch_node_t
    {
    public:
        analyze_root_node_t(__node_index_t index)
            : analyze_branch_node_t(index, __root_node_value, _T("root"), -1)
        { }

        static const __node_key_t key;

    protected:
        virtual const string_t __to_string() const override
        {
            return _T("ROOT");
        }
    };

    //-------- ---------- ---------- ---------- ----------

    class analyze_branch_ref_node_t : public analyze_normal_node_t
    {
    public:
        analyze_branch_ref_node_t(__node_index_t index, __node_value_t branch_value,
                    const string_t & name, const string_t & flag = _T(""), __weight_t weight = 0)
            : analyze_normal_node_t(index, __node_key_t(__node_type_t::branch_ref, branch_value),
                                                                                flag, weight)
            , branch_value(branch_value), name(name) { }

        virtual __node_type_t node_type() const override final
        {
            return __node_type_t::branch_ref;
        }

        const __node_value_t branch_value;
        const string_t name;
        bool  is_left_cycle = false;
        bool  expandable = true;
        bool  is_dead_cycle = false;

        virtual bool equals(const analyze_node_t & node) const override
        {
            return node.node_type() == __node_type_t::branch_ref &&
                ((const analyze_branch_ref_node_t &)node).branch_value == branch_value;
        }

        virtual bool less(const analyze_node_t & node) const override
        {
            const __node_type_t type1 = __node_type_t::branch_ref, type2 = node.node_type();
            if(type1 != type2)
                return type1 < type2;

            return branch_value < ((const analyze_branch_ref_node_t &)node).branch_value;
        }

    protected:
        virtual const string_t __to_string() const override
        {
            return sprintf(_T("$%1%"), name);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    class analyze_end_node_t : public analyze_node_t
    {
    public:
        analyze_end_node_t(__node_index_t index, analyze_node_t * start,
                                        analyze_branch_node_t * owner_branch)
            : analyze_node_t(index, __node_key_t(__node_type_t::end))
            , start(start), owner_branch(owner_branch)
        {
            _A(start != nullptr);
            _A(owner_branch != nullptr);
        }

        virtual __node_type_t node_type() const override final
        {
            return __node_type_t::end;
        }

        __node_value_t branch_value() const
        {
            return branch->key.value;
        }

        analyze_node_t * const start;
        analyze_branch_node_t * const owner_branch;

    protected:
        virtual const string_t __to_string() const override
        {
            return sprintf(_T("end:%1%"), owner_branch->name);
        }
    };

    ////////// ////////// ////////// ////////// //////////

    class analyze_empty_node_t : public analyze_normal_node_t
    {
    public:
        analyze_empty_node_t(__node_index_t index)
            : analyze_normal_node_t(index, __node_key_t(__node_type_t::empty))
        { }

        virtual __node_type_t node_type() const override final
        {
            return __node_type_t::empty;
        }

    protected:
        virtual const string_t __to_string() const override
        {
            return _T("empty");
        }

    private:
        static __node_value_t __next_value;
    };

    ////////// ////////// ////////// ////////// //////////

    class analyze_normal_path_node_t;

    class analyze_tree_t : public object_t
    {
    public:
        analyze_tree_t(analyze_root_node_t * root, analyze_normal_path_node_t * path_root)
            : root(root), path_root(path_root)
        {
            _A(root != nullptr);
            _A(path_root != nullptr);
        }

        analyze_root_node_t * const root;
        analyze_normal_path_node_t * const path_root;
    };

    ////////// ////////// ////////// ////////// //////////

    X_INTERFACE analyze_tree_builder_provider_t
    {
        virtual __node_value_t get_node_value(const string_t & name, __node_type_t node_type) = 0;
    };

    //-------- ---------- ---------- ---------- ----------

    class analyze_path_node_t;
    class __stack_node_action_t;

    typedef al::multikey_t<range_t<const analyze_node_t **>, const analyze_node_t *>
                                                            __path_node_cache_key_t;

    //typedef uint32_t __holes_t, __affinity_t;
    typedef arch_uint_t __merge_identity_t;

    struct __stack_node_value_t
        : public compare_operators_t<__stack_node_value_t, const analyze_node_t *>
    {
        __stack_node_value_t()
            : __stack_node_value_t(nullptr, nullptr, nullptr/*, 0*/, 0)
        { }

        __stack_node_value_t(const analyze_node_t * node, __tag_t * tag,
            analyze_normal_path_node_t * current/*, int step*/, int weight = 0)
            : node(node), current(current), /*step(step),*/ begin_node(node), begin_tag(tag)
            , weight(weight)
        { }

        const analyze_node_t * node;
        const analyze_node_t * begin_node;
        analyze_normal_path_node_t * current;
        __tag_t * child_tag = nullptr, * begin_tag;
        __stack_node_action_t * action = nullptr;

        //__holes_t holes = 0;
        //__affinity_t affinity = 0;

        //int step = 0;
        int weight = 0;
        __merge_identity_t merge_identity;

        operator const analyze_node_t * () const { return node; }
        operator string_t() const;
    };

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const __stack_node_value_t & value)
    {
        return stream << ((string_t)value).c_str();
    }

    typedef al::lr_tree_node_t<__stack_node_value_t> __stack_node_t;

    ////////// ////////// ////////// ////////// //////////
    // __stack_node_action_type_t

    X_ENUM(__stack_node_action_type_t)

        assign_key,

        raise_matched_event,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------
    // __stack_node_action_t

    class __stack_node_action_t : public object_t
        , public equals_t<__stack_node_action_t>
        , public clonable_t<__stack_node_action_t>
    {
    public:
        __stack_node_action_t * next = nullptr;

        virtual void execute() = 0;
        virtual __stack_node_action_type_t action_type() const = 0;
        virtual bool equals(const __stack_node_action_t & action) const = 0;
        virtual __stack_node_action_t * clone(memory_t * memory) const = 0;
    };

    //-------- ---------- ---------- ---------- ----------
    // __stack_node_assign_key_action_t

    class __stack_node_assign_key_action_t : public __stack_node_action_t
    {
        typedef __stack_node_assign_key_action_t __self_t;

    public:
        __stack_node_assign_key_action_t(__node_value_t * out_value, __node_value_t value)
            : __out_value(out_value), __value(value) { }

        virtual void execute() override
        {
            *__out_value = __value;
        }

        virtual __stack_node_action_type_t action_type() const override final
        {
            return __stack_node_action_type_t::assign_key;
        }

        virtual bool equals(const __stack_node_action_t & action) const override final
        {
            if(action.action_type() != __stack_node_action_type_t::assign_key)
                return false;

            const __self_t & a = (const __self_t &)action;
            return a.__out_value == __out_value && a.__value == __value;
        }

        virtual __stack_node_action_t * clone(memory_t * memory) const override
        {
            return memory_t::new_obj<__stack_node_assign_key_action_t>(memory,
                __out_value, __value
            );
        }

        virtual const string_t to_string() const override
        {
            return sprintf(_T("%1% %2%"), action_type(), __value);
        }

        __node_value_t get_value() const { return __value; }

    private:
        __node_value_t * __out_value;
        __node_value_t   __value;
    };

    //-------- ---------- ---------- ---------- ----------
    // __stack_node_raise_matched_event_action_t

    class analyze_context_t;

    class __stack_node_raise_matched_event_action_t : public __stack_node_action_t
    {
        typedef __stack_node_raise_matched_event_action_t __self_t;
        typedef __stack_node_action_t                     __super_t;

    public:
        __stack_node_raise_matched_event_action_t(analyze_context_t & context,
                const analyze_branch_ref_node_t * branch_ref, __tag_t * begin_tag, __tag_t * end_tag)
            : __context(context), __branch_ref(branch_ref)
            , __begin_tag(begin_tag), __end_tag(end_tag)
        { }

        virtual void execute() override;

        virtual __stack_node_action_type_t action_type() const override final
        {
            return __stack_node_action_type_t::raise_matched_event;
        }

        virtual bool equals(const __stack_node_action_t & action) const override final
        {
            if(action.action_type() != __stack_node_action_type_t::raise_matched_event)
                return false;

            const __self_t & a = (const __self_t &)action;
            return a.__branch_ref == __branch_ref && a.__begin_tag == __begin_tag
                    && a.__end_tag == __end_tag;
        }

        virtual __stack_node_action_t * clone(memory_t * memory) const override
        {
            return memory_t::new_obj<__stack_node_raise_matched_event_action_t>(memory,
                __context, __branch_ref, __begin_tag, __end_tag
            );
        }

        virtual const string_t to_string() const override
        {
            return sprintf(_T("%1% %2%:%3%-%4%"),
                action_type(), _str(__branch_ref), _str(__begin_tag), _str(__end_tag)
            );
        }

        __node_value_t get_value() const { return __branch_ref->branch_value; }

        int begin_index() const { return __begin_tag? __begin_tag->index : -1; }
        int end_index()   const { return __end_tag? __end_tag->index : -1; }

        int action_index() const;
        int back_track() const;

    private:
        analyze_context_t & __context;
        const analyze_branch_ref_node_t * __branch_ref;
        __tag_t * __begin_tag, * __end_tag;
        int16_t __action_index = -1, __back_track = -1;
    };

    ////////// ////////// ////////// ////////// //////////
    // analyze_tree_build_context_t

    class analyze_tree_build_context_t : public object_t
    {
    public:
        analyze_tree_build_context_t(memory_t * memory = nullptr)
            : memory(memory), __heap(memory) { }

        template<typename obj_t, typename ... args_t>
        obj_t * new_obj(args_t && ... args)
        {
            return memory_t::new_obj<obj_t>(memory, std::forward<args_t>(args) ...);
        }

        template<typename node_t, typename ... args_t>
        node_t * new_node(args_t && ... args)
        {
            return new_obj<node_t>(memory, 
                    __next_node_index++, std::forward<args_t>(args) ...);
        }

        template<typename path_node_t, typename ... args_t>
        path_node_t * new_path_node(args_t && ... args)
        {
            return new_obj<path_node_t>(this, std::forward<args_t>(args) ...);
        }

        template<typename t>
        t ** new_ptr_array(size_t size)
        {
            return (t **)__heap.acquire(size);
        }

        template<typename t>
        t ** new_ptr_array(std::vector<t *> & array)
        {
            t ** buffer = new_ptr_array<t>(array.size());
            std::copy(array.begin(), array.end(), buffer);

            return buffer;
        }

        template<typename t>
        t ** copy_ptr_array(t ** array, size_t len = (size_t)-1)
        {
            if(len == (size_t)-1)
                len = __len((void **)array);

            t ** buffer = new_ptr_array<t>(len);
            std::copy(array, array + len, buffer);

            return buffer;
        }

        analyze_path_node_t * get_node_from_cache(const __path_node_cache_key_t & key);
        analyze_path_node_t * get_node_from_cache(const std::vector<analyze_node_t *> & nodes);

        void add_node_to_cache(analyze_path_node_t * node);

        void add_branch(analyze_branch_node_t * node);
        analyze_branch_node_t * get_branch(__node_value_t value) const;
        auto all_branches() const { return _range(__branch_map); }

        template<typename itor_t>
        analyze_node_t ** to_stack_path(itor_t begin, itor_t end);

        memory_t * const memory;

    private:
        __node_index_t __next_node_index = 0;
        al::heap_t<void * []> __heap;

        std::map<__node_value_t, analyze_branch_node_t *> __branch_map;
        std::map<analyze_node_t *, analyze_path_node_t *> __path_node_relation;
        std::map<__path_node_cache_key_t, analyze_path_node_t *> __path_node_cache;

        typedef al::multikey_t<range_t<analyze_node_t **>, analyze_node_t **>
                                                        __stack_path_cache_key_t;
        std::map<__stack_path_cache_key_t, analyze_node_t **> __stack_path_cache;

        static size_t __len(void ** arr);
    };

    //-------- ---------- ---------- ---------- ----------

    class analyze_tree_builder_t : public object_t
    {
    public:
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

        analyze_root_node_t * build();

    private:
        const char_t * const __pattern;
        const char_t * __p;
        codepos_helper_t __codepos_helper;
        analyze_tree_builder_provider_t * __provider;
        __node_value_t __next_custome_branch_value = -128;
        analyze_tree_build_context_t * __build_context;

        std::map<string_t, __node_value_t> __branch_node_value_map;
        std::map<string_t, __node_value_t> __token_node_value_map;
        std::map<__node_value_t, string_t> __branch_node_name_map;
        std::map<__node_value_t, string_t> __token_node_name_map;

        template<typename obj_t, typename ... args_t>
        obj_t * __new(args_t && ... args)
        {
            return __build_context->new_obj<obj_t>(std::forward<args_t>(args) ...);
        }

        template<typename node_t, typename ... args_t>
        node_t * __new_node(args_t && ... args)
        {
            return __build_context->new_node<node_t>(std::forward<args_t>(args) ...);
        }

        struct __read_branch_context_t
        {
            std::vector<analyze_branch_ref_node_t *> branch_ref_nodes;
        };

        analyze_branch_node_t * __read_branch(__read_branch_context_t & ctx,
            const string_t & branch_name, const string_t & current_name,
            int level, bool inner = false);

        analyze_branch_node_t * __read_branch(__read_branch_context_t & ctx,
                const string_t & branch_name, int level);

        string_t __read_name();
        string_t __read_token(string_t & out_tag);
        string_t __read_subname();

        void __expect(char_t c);
        bool __is_expect(char_t c);
        char_t __next_char();
        bool __skip_comment();
        void __skip_whitespace_with_expect();

        static string_t __join_branch_name(const string_t & name1, const string_t & name2);

        __node_value_t __branch_value(const string_t & name, bool auto_create=true);
        __node_value_t __token_value(const string_t & name);
        void __print_branches();

        const string_t & __branch_name(const __node_value_t & value);
        const string_t & __token_name(const __node_value_t & value);

        template<typename ... args_t>
        string_t __format_error(const char_t * format, args_t ... args)
        {
            string_t s = sprintf(format, args ...);
            codepos_t pos = __codepos_helper.pos_of(__p);
            return sprintf(_T("%1%, line: %2%, col: %3%"), s, pos.line, pos.col);
        }
    };

    ////////// ////////// ////////// ////////// //////////

    analyze_tree_t * parse_analyze_tree(const char_t * pattern, memory_t * memory,
            analyze_tree_builder_provider_t * provider);

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        template<typename stream_t>
        class __node_writer_t
        {
        public:
            __node_writer_t(stream_t & stream): __stream(stream) { }

            void write(const analyze_node_t * node)
            {
                _A(node != nullptr);

                __write_node(node);
            }

            void write(const analyze_branch_node_t * branch)
            {
                _A(branch != nullptr);

                __stream << _T("$") << branch->to_string() << _T(": ");
                write((const analyze_node_t *)branch);
            }

            void write(const analyze_tree_t * tree)
            {
                _A(tree != nullptr);

                for(const analyze_node_t * node : tree->root->nodes.all())
                {
                    const analyze_branch_node_t * branch = 
                            dynamic_cast<const analyze_branch_node_t *>(node);

                    if(branch != nullptr)
                    {
                        write(branch);
                        __stream << std::endl;
                    }
                }
            }

        private:
            stream_t & __stream;

            void __write_node(const analyze_node_t * node)
            {
                switch(node->node_type())
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

                        if(normal_node != nullptr)
                            __write_nodes(normal_node->nodes);
                    }   break;
                }
            }

            void __write_nodes(const analyze_nodes_t & nodes)
            {
                int index = 0;
                for(const analyze_node_t * subnode : nodes.all())
                {
                    if(index++ > 0)
                        __stream << _T("|");

                    __write_node(subnode);
                }
            }
        };

        template<typename stream_t>
        void __write_to_stream(stream_t & stream, const analyze_node_t * node, int level)
        {
            for(int k = 0; k < level; k++)
            {
                stream << _T("  ");
            }
        }
    }

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const analyze_node_t * node)
    {
        if(node != nullptr)
            __node_writer_t<stream_t>(stream).write(node);

        return stream;
    }

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const analyze_tree_t * tree)
    {
        if(tree != nullptr)
            __node_writer_t<stream_t>(stream).write(tree);

        return stream;
    }

    ////////// ////////// ////////// ////////// //////////

    X_ENUM(analyze_path_node_type_t)

        normal,

        end,

    X_ENUM_END

    typedef analyze_path_node_type_t __path_node_type_t;

    //-------- ---------- ---------- ---------- ----------

    struct analyze_path_node_stack_unit_t
    {
        analyze_path_node_stack_unit_t() = default;
        analyze_path_node_stack_unit_t(const analyze_node_t * node, int16_t step,
                /*int16_t affinity, __hstep_t hstep,*/ void * path, const analyze_node_t * entrance)
            : node(node), step(step)/*, affinity(affinity), hstep(hstep)*/
            , path(path), entrance(entrance)
        { }

        const analyze_node_t * node;

        int16_t step;
        //int16_t affinity;
        //__hstep_t hstep;

        void * path;
        const analyze_node_t * entrance;

        operator string_t() const
        {
            //return sprintf(_T("%1% %2%"), _str(node), hstep);
            return _str(node);
        }
    };

    typedef analyze_path_node_stack_unit_t __stack_unit_t;

    //-------- ---------- ---------- ---------- ----------

    typedef range_t<__stack_unit_t *> __stack_range_t;
    typedef int16_t __node_count_t;

    X_ENUM(analyze_path_node_stack_type_t)

        normal, 

        branch_switched,

        all,

    X_ENUM_END

    typedef analyze_path_node_stack_type_t __stack_type_t;

    //-------- ---------- ---------- ---------- ----------

    class analyze_path_node_stack_t : public object_t
    {
    public:
        analyze_path_node_stack_t(const analyze_node_t * head, __node_count_t count)
            : head(head), count(count), next(nullptr)
        { }

        const analyze_node_t * head;
        analyze_path_node_stack_t * next;

        __node_count_t count, normal_count;
        __stack_unit_t units[1];

        __stack_range_t all(__stack_type_t type = __stack_type_t::all) const;

        template<typename itor_t>
        void assign(analyze_tree_build_context_t * context, itor_t begin, itor_t end);

        static analyze_path_node_stack_t * new_stack(analyze_tree_build_context_t * context,
                const analyze_node_t * head, __node_count_t count);

        template<typename itor_t>
        static analyze_path_node_stack_t * new_stack(analyze_tree_build_context_t * context,
                const analyze_node_t * head, itor_t begin, itor_t end);
    };

    typedef analyze_path_node_stack_t __path_node_stack_t;

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const __path_node_stack_t & stack)
    {
        stream << stack.head->to_string() << _T("(") << (void *)stack.head << _T(") { ");

        auto write_node = [&stream](const analyze_node_t * node) {
            stream << _str(node) << _T("(") << (void *)node << _T(")");
        };

        int index = 0;
        for(const __stack_unit_t & unit : stack.all(__stack_type_t::normal))
        {
            if(index++ > 0)
                stream << _T(", ");

            write_node(unit.node);
        }

        for(const __stack_unit_t & unit : stack.all(__stack_type_t::branch_switched))
        {
            if(index++ > 0)
                stream << _T(", ");

            stream << _T("...");
            write_node(unit.node);
        }

        stream << _T(" }");

        return stream;
    }

    //-------- ---------- ---------- ---------- ----------

    struct analyze_path_node_stacks_t : public object_t
    {
        analyze_path_node_stacks_t(const analyze_path_node_t * src)
            : src(src), next(nullptr) { }

        const analyze_path_node_t * src;
        __path_node_stack_t * stack;
        analyze_path_node_stacks_t * next;

        void on_build_complete(analyze_tree_build_context_t * build_context, size_t count);
        const __path_node_stack_t * stack_of(const analyze_node_t * head) const;
    };

    typedef analyze_path_node_stacks_t __path_node_stacks_t;

    template<typename stream_t>
    stream_t & operator << (stream_t & stream, const __path_node_stacks_t & stacks)
    {
        int index = 0;
        for(const __path_node_stack_t * stack = stacks.stack; stack; stack = stack->next)
        {
            if(index++ > 0)
                stream << _T("; ");

            stream << *stack;
        }

        return stream;
    }

    //-------- ---------- ---------- ---------- ----------

    struct analyze_matched_item_t
    {
        analyze_matched_item_t(const analyze_branch_ref_node_t * branch_ref, __tag_t * tag = nullptr)
            : node(branch_ref), tag(tag), branch_value(branch_ref->branch_value)
        { }

        analyze_matched_item_t(const analyze_branch_node_t * branch, __tag_t * tag = nullptr)
            : node(branch), tag(tag), branch_value(branch->key.value)
        { }

        const analyze_node_t *  node;
        __node_value_t          branch_value;
        __tag_t *               tag;

        __node_value_t value() const
        {
            return branch_value;
        }

        operator string_t() const
        {
            return sprintf(_T("%1%(%2%)"), _str(node), _str(tag));
        }
    };

    typedef analyze_matched_item_t __matched_item_t;

    typedef al::small_vector_t<__matched_item_t, 2> matched_item_vector_t;

    //-------- ---------- ---------- ---------- ----------

    class analyze_path_node_t : public object_t
                              , protected al::initializer_t, public no_copy_ctor_t
    {
    public:
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

        virtual __path_node_type_t type() const = 0;

        __node_key_t get_key() const { return __nodes[0]->key; }

        __path_node_cache_key_t to_cache_key() const
        {
            return __path_node_cache_key_t(nodes());
        }

        range_t<const analyze_node_t **> nodes() const
        {
            return range_t<const analyze_node_t **>(
                (const analyze_node_t **)__nodes,
                (const analyze_node_t **)__nodes + __node_size
            );
        }

        range_t<analyze_node_t **> nodes()
        {
            return range_t<analyze_node_t **>(__nodes, __nodes + __node_size);
        }

        const __path_node_stacks_t * stacks_of(const analyze_path_node_t * src_path_node) const;

        void append_stacks(analyze_path_node_t * src, std::vector<void **> & stacks);
        void append_stacks(__path_node_stacks_t * stacks);

        static size_t counter() { return __counter; }

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

        __path_node_stacks_t * __build_stacks(analyze_path_node_t * src,
                                                    std::vector<void **> & stacks);

        typedef std::vector<void **>::iterator __stack_nodes_itor_t;
        __path_node_stack_t * __build_stack(analyze_node_t * head,
                             __stack_nodes_itor_t stacks_begin, __stack_nodes_itor_t stacks_end);
    };

    //-------- ---------- ---------- ---------- ----------

    class analyze_end_path_node_t;
    class analyze_matched_item_t;

    class analyze_normal_path_node_t : public analyze_path_node_t
    {
    public:
        template<typename name_t>
        analyze_normal_path_node_t(analyze_tree_build_context_t * build_context, 
                std::vector<analyze_node_t *> & nodes, name_t name)
            : analyze_path_node_t(build_context, nodes)
            , __name(std::forward<name_t>(name))
        { }

        analyze_path_node_t * enter(const __node_key_t & key);

        virtual const string_t to_string() const override
        {
            return __name;
        }

        virtual __path_node_type_t type() const override final
        {
            return __path_node_type_t::normal;
        }

        string_t all_subnodes_string();

    protected:
        virtual void __initialize() override;

    private:
        struct __build_node_info_t
        {
            std::set<analyze_node_t *> nodes;
            std::vector<void **> stacks;

            bool append_stack(void ** stack);
            bool __exist_stack(void ** stack);
        };

        typedef std::vector<analyze_node_t *> __nodes_t;
        typedef std::map<__node_key_t, __nodes_t> __nodes_map_t;
        typedef std::map<__node_key_t, __build_node_info_t> __nodeset_map_t;

        struct __append_ctx_t
        {
            __nodeset_map_t nodes_map;
            std::vector<__node_unit_t> branch_stack;
            __node_unit_t entrance = nullptr;

            void branch_stack_push(__node_unit_t u, __node_unit_t entrance = nullptr);
            void branch_stack_pop();

            al::heap_t<void * []> __analyze_node_heap;

            void ** current_branch_stack(analyze_tree_build_context_t * context);
            string_t current_branch_stack_string();
            void print_stack() const;
        };

        typedef al::multikey_t<range_t<__node_key_t *>, __node_key_t> __multipy_map_key_t;
        typedef std::map<__multipy_map_key_t, analyze_normal_path_node_t *> __multipy_map_t;

        std::map<__node_key_t, analyze_path_node_t *> __subnode_map;
        string_t __name;

        void __append_subnodes(__append_ctx_t & ctx, analyze_nodes_t & nodes,
                                                    analyze_node_t * parent = nullptr);
        void __append_subnodes(__append_ctx_t & ctx, __node_unit_t u);
        void __append_branch_subnodes(__append_ctx_t & ctx, __node_unit_t u,
                                                analyze_branch_node_t * branch);
        bool __append_current_stack(__append_ctx_t & ctx, __build_node_info_t & info,
                                                    __node_unit_t new_top);
        analyze_branch_node_t * __get_branch_node(__node_value_t value);

        void __print();
    };

    typedef analyze_path_node_t __path_node_t;

    //-------- ---------- ---------- ---------- ----------

    class analyze_end_path_node_t : public analyze_path_node_t
    {
    public:
        using analyze_path_node_t::analyze_path_node_t;

        virtual const string_t to_string() const override
        {
            return _T("<end>");
        }

        virtual __path_node_type_t type() const override final
        {
            return __path_node_type_t::end;
        }

        analyze_end_node_t * end_node() const
        {
            return (analyze_end_node_t *)*nodes().begin();
        }

    protected:
        virtual void __initialize() override { }

    };

    ////////// ////////// ////////// ////////// //////////

    X_ENUM(analyze_callback_action_t)

        branch_matched,

        analyze_end,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    typedef callback_args_t<analyze_callback_action_t> analyze_callback_args_t;

    template<typename data_t>
    using tanalyze_callback_args_t = tcallback_args_t<analyze_callback_action_t, data_t>;

    //-------- ---------- ---------- ---------- ----------

    struct branch_matched_analyze_callback_data_t
    {
        friend class analyze_normal_path_node_t;

        branch_matched_analyze_callback_data_t(
                    __tag_t * end_tag, const __matched_item_t & matched_item)
            : end_tag(end_tag), matched_item(matched_item) { }

        __tag_t * end_tag; 
        const __matched_item_t & matched_item;
    };

    typedef tanalyze_callback_args_t<branch_matched_analyze_callback_data_t>
            branch_matched_analyze_callback_args_t;

    //-------- ---------- ---------- ---------- ----------

    struct analyze_end_analyze_callback_data_t { };

    typedef tanalyze_callback_args_t<analyze_end_analyze_callback_data_t>
            analyze_end_analyze_callback_args_t;

    //-------- ---------- ---------- ---------- ----------

    typedef callback_t<analyze_callback_args_t> analyze_callback_t;

    ////////// ////////// ////////// ////////// //////////

    typedef al::walk_list_t<__stack_node_t *> __leafs_t;

    class analyze_context_t : public object_t
    {
        friend class __stack_node_raise_matched_event_action_t;

    public:
        analyze_context_t(lang_t * lang, analyze_tree_t * tree, analyze_callback_t * callback);

        void go(const __node_key_t * keys, __tag_t * tag, __node_value_t * out_value = nullptr);

    private:
        lang_t * __lang;
        lang_service_helper_t __service_helper;
        token_property_cache_t<token_property_t> __token_property_cache;

        analyze_callback_t * __callback;
        analyze_tree_t * __tree;

        __stack_node_t * __stack_root = nullptr;
        al::xheap_t __sn_heap;

        template<typename t> using __w_t = al::respool_wrapper_t<t>;
        __w_t<__stack_node_t> __stack_node_factory;
        al::heap_t<__stack_node_assign_key_action_t> __assign_key_action_factory;
        al::heap_t<__stack_node_raise_matched_event_action_t> __raise_matched_event_action_factory;

        __leafs_t __leafs;

        typedef al::small_vector_t<__stack_node_t *, 2> __stack_node_vector_t;

        //-------- ---------- ---------- ---------- ----------

        struct __stack_push_args_t
        {
            __stack_push_args_t(analyze_context_t * owner, const __node_key_t * keys, __tag_t * tag,
                __node_value_t * out_value = nullptr)
                : __owner(owner), keys(keys), tag(tag), out_value(out_value)
            { }

            analyze_context_t * __owner;
            __tag_t * tag;
            const __node_key_t * keys;
            __node_value_t * out_value;

            __stack_node_vector_t new_stack_nodes;

            void append_new_stack_node(__stack_node_t * leaf);
        };

        //-------- ---------- ---------- ---------- ----------

        typedef size_t __stack_push_task_state_t;

        struct __stack_push_task_t
        {
            __stack_push_task_t(__stack_push_args_t & push_args) : push_args(push_args) { }

            __stack_push_args_t & push_args;

            al::small_vector_t<void *, 10> stack_paths;
            size_t record_stack_unit(const __stack_unit_t & unit);

            __stack_push_task_state_t current_state();
            void restore(const __stack_push_task_state_t & state);
        };

        //-------- ---------- ---------- ---------- ----------

        enum __grow_model_t { __overwrite, __grow_only };

        struct __stack_grow_args_t
        {
            __stack_grow_args_t(__stack_push_task_t & task, __grow_model_t model = __overwrite) :
                task(task), model(model), __from_index(task.push_args.new_stack_nodes.size())
            { }

            __stack_push_task_t & task;
            bool found = false;
            __grow_model_t model;
            size_t apply_state = 0;

            auto begin() const { return task.push_args.new_stack_nodes.begin() + __from_index; }
            auto end()   const { return task.push_args.new_stack_nodes.end(); }

            size_t __from_index;

            bool check_stack_unit(const __stack_unit_t & unit);
        };

        //-------- ---------- ---------- ---------- ----------

        struct __stack_item_t
        {
            const __path_node_stack_t * stack;
            analyze_normal_path_node_t * dst;
        };

        typedef al::small_vector_t<__stack_item_t, 2> __stack_items_t;

        //-------- ---------- ---------- ---------- ----------

        struct __stack_pop_args_t
        {
            __stack_pop_args_t(__tag_t * tag) : end_tag(tag) { }

            __tag_t * end_tag;
            __stack_node_vector_t new_stack_nodes;
        };

        //-------- ---------- ---------- ---------- ----------

        struct __check_end_node_args_t
        {
            __check_end_node_args_t(__stack_push_task_t & task)
                : task(task), __from_index(task.push_args.new_stack_nodes.size())
            {
                task_state = task.current_state();
            }

            __check_end_node_args_t(__check_end_node_args_t & args)
                : __check_end_node_args_t(args.task) { }

            __stack_push_task_t & task;

            auto begin() const { return task.push_args.new_stack_nodes.begin() + __from_index; }
            auto end()   const { return task.push_args.new_stack_nodes.end(); }

            size_t __from_index;
            __stack_push_task_state_t task_state;

            ~__check_end_node_args_t()
            {
                task.restore(task_state);
            }
        };

        //-------- ---------- ---------- ---------- ----------

        void __reset();

        void __do_callback(analyze_callback_args_t & args);
        void __do_end_callback();
        void __do_branch_matched_callback(const __matched_item_t & matched_item, __tag_t * tag);

        void __stack_push(__stack_push_args_t & args, __stack_node_t * stack_node);
        void __stack_grow(__stack_grow_args_t & args, __stack_node_t * stack_node);
        void __do_stack_grow(__stack_grow_args_t & args, __stack_node_t * stack_node,
                                                 const __stack_items_t & stack_items);

        void __do_stack_grow(__stack_grow_args_t & args, __stack_node_t * stack_node,
                                                 const __stack_item_t & stack_item);

        void __check_end_node(__check_end_node_args_t & args, __stack_node_t * parent,
                    __stack_node_t * current, __node_unit_t end_unit,
                    __stack_node_action_t * next_action, int weight
                    /*, __hstep_t hstep, __affinity_t affinity*/);

        void __merge_leafs();
        void __pick_final_leafs();
        void __stack_pop(__stack_pop_args_t & args, __stack_node_t * stack_node);
        void __do_stack_pop(__stack_pop_args_t & args, __stack_node_t * stack_node);

        void __execute_actions();
        bool __execute_actions(__stack_node_t * leaf);
        void __execute_action(__stack_node_action_t * action);
        void __append_node_action(__stack_node_t * node, __stack_node_action_t * action,
                                                              bool push_back = false);

        void __append_raise_matched_event_action(__stack_node_t * node,
                __node_value_t value, __tag_t * begin_tag, __tag_t * end_tag,
                bool push_back = false);

        __stack_node_raise_matched_event_action_t * __new_raise_matched_event_action(
                const analyze_branch_ref_node_t * branch_ref,
                __tag_t * begin_tag, __tag_t * end_tag,
                __stack_node_action_t * next_action = nullptr);
            
        template<typename pred_t>
        void __remove_leaf_until(__stack_node_t * stack_node, pred_t pred);

        void __remove_leaf(__stack_node_t * stack_node);
        void __remove_stack_node(__stack_node_t * stack_node);

        template<typename stack_node_value_t>
        __stack_node_t * __new_stack_node(stack_node_value_t && value);

        template<typename itor_t> void __append_leafs(itor_t begin, itor_t end);
        template<typename container_t> void __append_leafs(container_t & container);

        bool __is_invisible(token_value_t value);
    };

    ////////// ////////// ////////// ////////// //////////

    class __token_reader_context_t : public token_reader_context_t
    {
    public:
        using token_reader_context_t::token_reader_context_t;

        token_reader_t * token_reader_of(lang_id_t lang_id);

    private:
        std::map<lang_id_t, token_reader_t *> __token_reader_cache;
    };

    ////////// ////////// ////////// ////////// //////////

    X_ENUM(analyzer_element_type_t)

        token,

        ast_node,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    class analyzer_element_reader_t;

    struct analyzer_element_t
    {
        analyzer_element_t() = default;
        analyzer_element_t(const analyzer_element_t &) = default;

        analyzer_element_t(analyzer_element_type_t type, token_index_t index = 0)
            : type(type), tag(index)
        { }

        analyzer_element_t(token_t * token, token_index_t index = 0)
            : type(analyzer_element_type_t::token)
            , token(token), tag(index)
        { }

        analyzer_element_t(ast_node_t * ast_node, token_index_t index = 0)
            : type(analyzer_element_type_t::ast_node)
            , ast_node(ast_node), tag(index)
        { }

        analyzer_element_type_t type;
        __tag_t tag;
        const analyze_node_t * matched_node;

        union
        {
            token_t * token;
            ast_node_t * ast_node;
        };

        bool operator == (const analyzer_element_t & other) const;
        bool operator != (const analyzer_element_t & other) const;

        operator string_t() const
        {
            switch(type)
            {
                case analyzer_element_type_t::token:
                    return (string_t)*token;

                case analyzer_element_type_t::ast_node:
                    return (string_t)*ast_node;

                default:
                    return _T("");
            }
        }
    };

    //-------- ---------- ---------- ---------- ----------

    X_ENUM(analyzer_element_read_model_t)

        normal,

        all,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    class analyzer_element_reader_t
    {
        typedef analyzer_element_read_model_t __model_t;

    public:
        analyzer_element_reader_t(__token_reader_context_t & context, code_section_t ** &p_section);

        analyzer_element_t * next();

        analyzer_element_t & operator[](__tag_t * tag)
        {
            return element_at(tag, __model_t::normal);
        }

        analyzer_element_t & element_at(__tag_t * tag, __model_t model = __model_t::normal);
        __tag_t * next(__tag_t * tag, __model_t model = __model_t::normal);

        size_t tag_count() const { return __items.size(); }

        __tag_t * tag_at(size_t index)
        {
            _A(index < tag_count());
            return __items[index].get_tag();
        }

        __tag_t * begin_tag() { return __items[0].get_tag(); }
        __tag_t * end_tag()   { return __items[__items.size() - 1].get_tag(); }

        size_t size() const { return __items.size(); }

        void append_ast(__tag_t * from_tag, __tag_t * end_tag,
                ast_node_t * ast_node, const analyze_node_t * node);

        ast_node_t * get_root_ast();

    private:
        struct __ast_flag_t : public object_t
        {
            __ast_flag_t(ast_node_t * ast_node, __tag_t * end_tag,
                                    const analyze_node_t * matched_node)
                : ast_node(ast_node), end_tag(end_tag), element(ast_node)
            {
                element.matched_node = matched_node;
            }

            __tag_t * end_tag;
            ast_node_t * ast_node;
            analyzer_element_t element;

            __ast_flag_t * next = nullptr;
        };

        //-------- ---------- ---------- ---------- ----------

        struct __item_t : analyzer_element_t
        {
            using analyzer_element_t::analyzer_element_t;

            __ast_flag_t * flag = nullptr;

            __tag_t * get_tag() { return &tag; }
        };

        //-------- ---------- ---------- ---------- ----------

        __token_reader_context_t & __context;

        code_section_t ** & __p_section;
        code_section_t *    __section;
        int __depth;

        token_reader_t * __reader;
        token_enumerator_t * __enumerator;

        al::heap_t<__item_t> __items;
        size_t __index = 0;
        al::xheap_t __heap;

        bool __read_next_element();

        template<typename element_t>
        void __append_element(element_t element)
        {
            __items.new_obj(element, __items.size());
        }

        analyzer_element_t * __end_element()
        {
            return __items.new_obj(analyzer_element_type_t::__unknown__, __items.size());
        }
    };

    //-------- ---------- ---------- ---------- ----------

    template<typename tag_t> class lang_ast_build_elements_t;

    template<typename tag_t>
    class lang_ast_builder_element_iterator_t
    {
        typedef analyzer_element_t __element_t;
        typedef lang_ast_builder_element_iterator_t<tag_t> __self_t;
        typedef lang_ast_build_elements_t<tag_t> __elements_t;

    public:
        lang_ast_builder_element_iterator_t(const __elements_t & elements, tag_t * tag)
            : __elements(elements), __tag(tag)
        { }

        __element_t & operator *() const
        {
            return __elements.get(__tag);
        }

        __self_t & operator ++()
        {
            __tag = __elements.next(__tag);
            return *this;
        }

        __self_t operator ++(int) const
        {
            return __self_t(__elements, __elements.next());
        }

        bool operator == (const __self_t & other) const
        {
            return __tag == other.__tag;
        }

        bool operator != (const __self_t & other) const
        {
            return !operator == (other);
        }

    private:
        const __elements_t & __elements;
        tag_t * __tag;
    };

    //-------- ---------- ---------- ---------- ----------

    template<typename tag_t>
    class lang_ast_build_elements_t
    {
        typedef analyzer_element_t __element_t;
        typedef lang_ast_build_elements_t<tag_t> __self_t;
        typedef lang_ast_builder_element_iterator_t<tag_t> __itor_t;
        typedef analyzer_element_read_model_t __model_t;

    public:
        lang_ast_build_elements_t(analyzer_element_reader_t & reader,
                tag_t * from_tag, tag_t * end_tag, __model_t model = __model_t::normal)
            : __reader(reader), __end(*this, end_tag)
            , __from_tag(from_tag), __end_tag(end_tag), __model(model)
        { }

        __element_t & get(tag_t * tag) const
        {
            return __reader.element_at(tag, __model);
        }

        tag_t * next(tag_t * tag) const
        {
            return __reader.next(tag, __model);
        }

        __itor_t begin() const { return __itor_t(*this, __from_tag); }

        const __itor_t end() const { return __end; }

    private:
        analyzer_element_reader_t & __reader;
        tag_t * __from_tag, * __end_tag;
        const __itor_t __end;
        __model_t __model;
    };

    ////////// ////////// ////////// ////////// //////////

    class __analyzer_t : public object_t
    {
    public:
        __analyzer_t(lang_t * lang, analyze_tree_t * tree,
            analyzer_element_reader_t * reader, analyze_callback_t * callback)
            : __lang(lang), __tree(tree), __reader(reader), __callback(callback)
            , __context(lang, tree, callback)
        {
            _A(lang != nullptr);
            _A(tree != nullptr);
            _A(reader != nullptr);
            _A(callback != nullptr);
        }

        void analyze();

    private:
        lang_t                    * __lang;
        analyze_tree_t            * __tree;
        analyzer_element_reader_t * __reader;
        analyze_callback_t        * __callback;
        analyze_context_t           __context;

        void __push_token(token_t * token, __tag_t * tag);
        void __push_ast_node(ast_node_t * node, __tag_t * tag);

        analyze_normal_path_node_t * __push(analyze_normal_path_node_t * current,
                                            const __node_key_t * keys, __tag_t * tag);

        void __push(const __node_key_t * keys, __tag_t * tag, __node_value_t * out_value = nullptr);
        void __push(__node_key_t key, __tag_t * tag);
        void __push_end(__tag_t * tag);

        static size_t __possible_value_count(const token_value_t * possible_values);
    };

    ////////// ////////// ////////// ////////// //////////

    class analyzer_t : public object_t
    {
    public:
        analyzer_t(lang_t * lang, analyze_tree_t * tree)
            : __lang(lang), __tree(tree)
        {
            _A(lang != nullptr);
            _A(tree != nullptr);
        }

        void analyze(analyzer_element_reader_t * reader, analyze_callback_t * callback);

        static const int32_t __cache_key__ = 1;

    private:
        analyze_tree_t * __tree;
        lang_t * __lang;
    };

    ////////// ////////// ////////// ////////// //////////

    ast_node_t * analyze_ast(compile_context_t & context, code_t * code);

    ////////// ////////// ////////// ////////// //////////

    X_ENUM(ast_build_error_code_t)

        build_failed,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    class ast_factory_t : public object_t
    {
    public:
        ast_factory_t(ast_context_t & ast_context, analyzer_element_reader_t & reader)
            : __ast_context(ast_context), __reader(reader)
        {
            __build_service = require_lang_service<lang_ast_build_service_t>(
                ast_context.lang
            );
        }

        void on_branch_matched(const analyze_node_t * node, __node_value_t value,
                                    __tag_t * from_tag, __tag_t * end_tag);
        void on_end();

        ast_node_t * get_result();

    private:
        ast_context_t & __ast_context;
        lang_ast_build_service_t * __build_service;
        analyzer_element_reader_t & __reader;

        ast_node_t * __build_ast(lang_ast_build_args_t & args);
    };

    ////////// ////////// ////////// ////////// //////////

    struct ast_builder_completed_args_t
    {
        code_unit_t * code_unit;
    };

    //-------- ---------- ---------- ---------- ----------

    struct ast_builder_apply_token_args_t
    {

    };

    //-------- ---------- ---------- ---------- ----------

    struct ast_builder_apply_ast_args_t
    {
        const char_t * flag;
    };

    //-------- ---------- ---------- ---------- ----------

    class ast_builder_t : public object_t
    {
    public:
        ast_builder_t(ast_context_t & context, lang_ast_build_args_t & args)
            : __context(context), __args(args)
        { }

        ast_node_t * build();

    protected:
        typedef analyzer_element_t __element_t;
        typedef ast_builder_completed_args_t        __completed_args_t;
        typedef ast_builder_apply_token_args_t      __apply_token_args_t;
        typedef ast_builder_apply_ast_args_t    __apply_ast_args_t;

        virtual ast_node_t * create() = 0;
        virtual void apply_token(token_t * token, __apply_token_args_t & args) = 0;
        virtual void apply_ast(ast_node_t * node, __apply_ast_args_t & args) = 0;
        virtual void completed(__completed_args_t & args) = 0;

        ast_context_t & __context;
        lang_ast_build_args_t & __args;

        template<typename t, typename ... args_t>
        t * __new_obj(args_t && ... args)
        {
            return __context.compile_context.new_obj<t>(std::forward<args_t>(args) ...);
        }
    };

    //-------- ---------- ---------- ---------- ----------

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
        virtual ast_node_t * create() override
        {
            __node = __new_obj<_ast_node_t>(__context);
            return __node;
        }

        virtual void apply_token(token_t * token, __apply_token_args_t & args) override { }
        virtual void apply_ast(ast_node_t * node, __apply_ast_args_t & args) override { }
        virtual void completed(__completed_args_t & args) override { }

        _ast_node_t * __node = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////

} } }  // X_ROOT_NS::modules::compile

#endif // __COMPILE_ANALYZE_TREE_H__
