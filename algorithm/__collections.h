
#ifndef __ALGORITHM_COLLECTION_H__
#define __ALGORITHM_COLLECTION_H__


namespace X_ROOT_NS { namespace algorithm {

    ////////// ////////// ////////// ////////// //////////
    // heap_t

    template<typename obj_t, typename obj_creator_t> class heap_t;

    namespace
    {
        ////////// ////////// ////////// ////////// //////////

        template<typename obj_t>
        struct __obj_creator_t
        {
            template<typename ... args_t>
            static void new_obj(obj_t * hole, args_t && ... args)
            {
                new(hole) obj_t(std::forward<args_t>(args) ...);
            }

            template<typename obj_itor_t>
            static void destory(range_t<obj_itor_t> & range) { }
        };

        ////////// ////////// ////////// ////////// //////////

        template<typename obj_t>
        struct __obj_creator_t<obj_t *>
        {
            template<typename ... args_t>
            static void new_obj(obj_t ** hole, args_t && ... args)
            {
                new(*hole) obj_t(std::forward<args_t>(args) ...);
            }

            template<typename obj_itor_t>
            static void destory(range_t<obj_itor_t> & range)
            {
                for(auto it = range.begin(); it != range.end(); it++)
                {
                    operator delete(**it);
                }
            }
        };
    }

    ////////// ////////// ////////// ////////// //////////

    template<typename obj_t, typename obj_creator_t=__obj_creator_t<obj_t>>
    class heap_t : public object_t, private memory_base_t
    {
        typedef heap_t<obj_t, obj_creator_t> __self_t;
        typedef object_t                     __super_t;

    public:
        typedef std::vector<obj_t *> obj_pool_t;
        typedef typename obj_pool_t::iterator obj_itor_t;
        typedef obj_t * value_type;

        heap_t(memory_t * memory = nullptr, size_t row_size = 0)
            : memory_base_t(memory)
            , __row_size(row_size? row_size : max(1024 / sizeof(obj_t), 1))
        { }

        virtual ~heap_t() override
        {
            auto range = _rrange(__pool);
            obj_creator_t::destory(range);

            for(auto it = __pool.begin(); it != __pool.end(); it++)
            {
                this->__free(*it);
            }
        }

    public:
        template<typename ... args_t>
        obj_t * new_obj(args_t && ... args)
        {
            obj_t * hole = __current_hole();
            obj_creator_t::new_obj(hole, std::forward<args_t>(args) ...);
            return hole;
        }

        template<typename output_itor_t, typename ... args_t>
        size_t new_objs(size_t count, output_itor_t output_itor, args_t && ... args)
        {
            size_t index = size();

            for(size_t k = 0; k < count; k++)
            {
                *output_itor++ = new_obj(std::forward<args_t>(args) ...);
            }

            return index;
        }

        size_t size() const
        {
            size_t pool_size = __pool.size();
            return !pool_size? 0 : (pool_size - 1) * __row_size + __current_col;
        }

        obj_t & operator[](size_t index) const
        {
            return __pool[index / __row_size][index % __row_size];
        }

        obj_t * last() const
        {
            if(!__current_row_ptr)
                return nullptr;

            return __current_row_ptr + __current_col - 1;
        }

        void clear()
        {
            __current_row_ptr = nullptr;
            __current_col = 0;
            __current_row = -1;
        }

        template<typename _self_t>
        class _iterator : public iterator_base_t<_iterator<_self_t>>
        {
            typedef _iterator<_self_t> __iterator_self_t;

        public:
            _iterator(const _self_t * self, int index = 0) : __self(self), __index(index) { }
            void increase(int inc) { __index += inc; }
            bool equals(const _iterator & it) const { return it.__index == __index; }
            obj_t * get() const { return &(*__self)[__index]; }
            int diff(const __iterator_self_t & it) const { return __index - it.__index; }

        private:
            const _self_t * __self;
            int __index;
        };

        typedef _iterator<__self_t> iterator;
        typedef _iterator<const __self_t> const_iterator;

        const_iterator cbegin() const { return const_iterator(this); }
        const_iterator cend()   const { return const_iterator(this, size()); }

        iterator begin() const { return iterator(this); }
        iterator end()   const { return iterator(this, size()); }

    private:

        obj_pool_t  __pool;
        obj_t *     __current_row_ptr = nullptr;
        const int   __row_size;
        int __current_col = 0, __current_row = -1;

        obj_t * __current_hole()
        {
            if(!__current_row_ptr || __current_col >= __row_size)
            {
                if(++__current_row < __pool.size())
                {
                    __current_row_ptr = __pool[__current_row];
                }
                else
                {
                    __current_row_ptr = this->__alloc_objs<obj_t>(__row_size);
                    __pool.push_back(__current_row_ptr);
                }

                __current_col = 0;
            }

            return __current_row_ptr + (__current_col++);
        }

    };

    ////////// ////////// ////////// ////////// //////////
    // heap_t for array

    template<typename obj_t>
    class heap_t<obj_t[]> : public object_t, private memory_base_t
    {
    public:

        struct array_t
        {
            obj_t * objs;
            size_t  size;
        };

        typedef typename std::list<array_t> obj_arrays_t;
        typedef typename obj_arrays_t::iterator obj_itor_t;

        heap_t(memory_t * memory = nullptr, size_t row_size = 0)
            : memory_base_t(memory)
            , __row_size(row_size? row_size : max(1024 / sizeof(obj_t), 8))
            , __current_row(nullptr), __current_col(0), __current_size(0) { }

        obj_t * acquire(size_t n)
        {
            if(!__current_row || (__current_size - __current_col) < n)
            {
                if(n >= __row_size)
                {
                    obj_t * objs = this->__alloc_objs<obj_t>(n);
                    __rows.push_back(objs);
                    __arrays.push_back(array_t { objs, n });
                    return objs;
                }
                else
                {
                    __current_size = __row_size;
                    __current_row = this->__alloc_objs<obj_t>(__current_size);
                    __current_col = 0;
                    __rows.push_back(__current_row);
                }
            }

            obj_t * objs = __current_row + __current_col;
            __arrays.push_back(array_t { objs, n });
            __current_col += n;
            return objs;
        }
    
        range_t<obj_itor_t> all() const
        {
            return range(__arrays);
        }

        size_t count() const
        {
            return __arrays.size();
        }

        virtual ~heap_t() override
        {
            for(auto it = __rows.begin(); it != __rows.end(); it++)
            {
                this->__free((*it));
            }
        }

        obj_t * last() const
        {
            return __rows.size()? __rows[__rows.size() - 1] : nullptr;
        }

    private:
        std::list<obj_t *> __rows;
        obj_t * __current_row;
        size_t __current_size;
        size_t __current_col;
        const size_t __row_size;
        obj_arrays_t __arrays;
    };

    ////////// ////////// ////////// ////////// //////////

    class xheap_t : public object_t, public memory_t
    {
        typedef int16_t __size_t;

    public:
        xheap_t(const string_t & name = _T(""));

        virtual void * alloc(size_t size, memory_flag_t flag = memory_flag_t::__default__) override;
        virtual void   free(void * p) override;
        virtual void * realloc(void * p, size_t size, memory_flag_t flag) override;

        virtual ~xheap_t() override;

    private:
        static const size_t __large_object_size = 128;
        static const size_t __row_size = 1024;

        struct __row_t
        {
            byte_t * buffer;
            __size_t object_count;
        };

        struct __row_collection_t
        {
            std::vector<__row_t> rows;
            __row_t * current_row;
            __size_t  capacity;
            __size_t  bytes_count;
        };

        __row_collection_t __row_collections[__large_object_size / sizeof(int)];
        std::vector<object_t *> __large_objs;
        std::vector<byte_t *>   __large_bytes;
        const string_t __name;
    };

    ////////// ////////// ////////// ////////// //////////

    class bit_array_t
    {
        typedef uint_type_t<sizeof(void *)> __underlying_t;
        static const size_t __underlying_size = sizeof(__underlying_t);
        static const size_t __underlying_bits = __underlying_size * 8;

    public:
        bit_array_t(size_t init_length = 1);
        ~bit_array_t();

        void set(size_t index, bool value);
        bool get(size_t index) const;
        size_t index_of(bool value, size_t from = 0) const;

        bool operator [] (size_t index) const
        {
            return get(index);
        }

        static const size_t not_found = max_value<size_t>();

    private:
        size_t __bit_length;
        size_t __data_length = 0;
        __underlying_t * __data = nullptr;

        void __check_length(size_t bit_length);
        __underlying_t & __pos_of(size_t index, __underlying_t * out_flag) const;
        static size_t __data_length_of(size_t bit_length);
        static size_t __index_of_true(__underlying_t value, size_t from,
                    size_t end = __underlying_bits - 1);
        static __underlying_t __cut(__underlying_t value, size_t from, size_t to);
    };

    ////////// ////////// ////////// ////////// //////////

    template<typename t>
    class respool_t : public object_t
    {
        static const size_t __default_capacity = 8;
        typedef creator_t<t> __creator_t;

    public:
        respool_t(memory_t * memory, __creator_t * creator, size_t capacity = __default_capacity)
            : __memory(memory? memory : default_memory_t::instance())
            , __creator(creator? creator : default_creator_t<t>::instance())
            , __capacity(capacity) { }

        respool_t(size_t capacity = __default_capacity)
            : respool_t(null<memory_t>, null<creator_t<t>>, capacity) { }

        t * acquire()
        {
            _L(__mutex);

            t * obj;
            if(__released_queue.empty())
            {
                obj = __creator->create(__memory);
                _A(obj != nullptr);
            }
            else
            {
                obj = __released_queue.front();
                __released_queue.pop();
            }

            auto r = __acquired_set.insert(obj);
            _A(r.second);

            return obj;
        }

        void release(t * obj)
        {
            _A(obj != nullptr);
            _L(__mutex);

            auto it = __acquired_set.find(obj);
            if(it == __acquired_set.end())
                throw _EC(invalid_operation, _T("not in this pool"));

            __acquired_set.erase(it);
            __released_queue.push(obj);

            if(__released_queue.size() > __capacity * 3 / 2)
            {
                while(__released_queue.size() > __capacity)
                {
                    __creator->destory(__memory, __released_queue.front());
                    __released_queue.pop();
                }
            }
        }

        virtual ~respool_t() override
        {
            for(t * obj : __acquired_set)
            {
                __creator->destory(__memory, obj);
            }

            while(!__released_queue.empty())
            {
                __creator->destory(__memory, __released_queue.front());
                __released_queue.pop();
            }
        }

    private:
        std::set<t *> __acquired_set;
        std::queue<t *> __released_queue;

        std::mutex    __mutex;
        memory_t *    __memory;
        __creator_t * __creator;
        const size_t  __capacity;
    };

    ////////// ////////// ////////// ////////// //////////

    template<typename value_t>
    class lr_tree_node_t : public object_t
    {
        typedef lr_tree_node_t<value_t> __self_t;

    public:
        lr_tree_node_t() = default;

        template<typename _value_t>
        lr_tree_node_t(_value_t && value)
            : value(value), first_child(nullptr)
        { }

        __self_t * append_child(memory_t * memory)
        {
            __self_t * node = memory_t::new_obj<__self_t>(memory);
            return append_child(node);
        }

        __self_t * append_child(__self_t * node)
        {
            _A(node != nullptr);

            node->parent = this;
            node->next_sibling = this->first_child;
            this->first_child = node;

            return node;
        }

        template<typename _value_t>
        __self_t * append_child(memory_t * memory, _value_t && value)
        {
            __self_t * node = append_child(memory);
            node->value = value;
            return node;
        }

        template<typename _value_t>
        __self_t * find_or_append_child(memory_t * memory, _value_t && value)
        {
            __self_t * child = find_child(value);
            if(child == nullptr)
                child = append_child(memory, std::forward<_value_t>(value));
            return child;
        }

        __self_t * append_sibling(memory_t * memory)
        {
            __self_t * node = memory_t::new_obj<__self_t>(memory);
            return append_sibling(node);
        }

        __self_t * append_sibling(__self_t * node)
        {
            node->parent = this->parent;
            node->next_sibling = this->next_sibling;
            this->next_sibling = node;

            return node;
        }

        template<typename _value_t>
        __self_t * append_sibling(memory_t * memory, _value_t && value)
        {
            __self_t * node = append_sibling(memory);
            node->value = value;
            return node;
        }

        __self_t * remove_child(const value_t & value)
        {
            __self_t * node = find_child(value);
            if(node != nullptr)
                remove_child(node);
            return node;
        }

        bool remove_child(const __self_t * node)
        {
            _A(node != nullptr);

            if(first_child == nullptr)
                return false;

            if(node == first_child)
            {
                first_child = node->next_sibling;
                return true;
            }

            for(__self_t * child = first_child, * sibling;
                sibling = child->next_sibling, sibling; child = sibling)
            {
                if(sibling == node)
                {
                    child->next_sibling = sibling->next_sibling;
                    return true;
                }
            }

            return false;
        }

        void remove()
        {
            if(parent == nullptr)
                throw _E(common_error_code_t::invalid_operation, _T("cannot delete root node"));

            parent->remove_child(this);
        }

        bool is_leaf() const
        {
            return first_child == nullptr;
        }

        __self_t * find_child(const value_t & value) const
        {
            for(__self_t * node = this->first_child; node; node = node->next_sibling)
            {
                if(node->value == value)
                    return node;
            }

            return nullptr;
        }

        template<typename pred_t>
        __self_t * find_child(pred_t pred) const
        {
            for(__self_t * node = this->first_child; node; node = node->next_sibling)
            {
                if(pred(node))
                    return node;
            }

            return nullptr;
        }

        template<typename pred_t>
        void walk(pred_t pred) const
        {
            if(!pred(this))
                return;

            for(__self_t * node = this->first_child; node; node = node->next_sibling)
            {
                node->walk(pred);
            }
        }

        template<typename pred_t>
        void walk(pred_t pred)
        {
            if(!pred(this))
                return;

            for(__self_t * node = this->first_child; node; node = node->next_sibling)
            {
                node->walk(pred);
            }
        }

        bool is_root() const
        {
            return this->parent == nullptr;
        }

        size_t height() const
        {
            size_t size = 0;
            const __self_t * node = this;
            while((node = node->parent) != nullptr)
            {
                size++;
            }

            return size;
        }

        range_t<__self_t **> children() const
        {
            return range_t<__self_t **>(first_child, (__self_t **)nullptr);
        }

        template<typename stream_t>
        stream_t & write_to(stream_t & stream) const
        {
            stream << this->value;

            if(this->first_child)
            {
                stream << _T(" </ ");

                for(const __self_t * subnode = this->first_child; subnode != nullptr;
                        subnode = subnode->next_sibling)
                {
                    if(subnode != this->first_child)
                        stream << _T(" | ");

                    subnode->write_to(stream);
                }

                stream << _T(" \\>");
            }

            return stream;
        }

        const value_t * operator ->() const { return &value; }
        const value_t & operator *()  const { return value;  }

        value_t * operator ->() { return &value; }
        value_t & operator *()  { return value;  }

        value_t    value;
        __self_t * first_child;
        __self_t * next_sibling;
        __self_t * parent;

        virtual ~lr_tree_node_t() override { }
    };

    template<typename value_t, typename _value_t>
    lr_tree_node_t<value_t> * new_lr_tree_root(memory_t * memory, _value_t value)
    {
        typedef lr_tree_node_t<value_t> node_t;
        node_t * node = memory_t::new_obj<node_t>(memory);
        node->first_child  = nullptr;
        node->next_sibling = nullptr;
        node->parent       = nullptr;
        node->value        = std::forward<_value_t>(value);

        return node;
    }

    template<typename value_t>
    struct __lr_node_details_wrapper_t
    {
        typedef lr_tree_node_t<value_t> __node_t;
        __lr_node_details_wrapper_t(const __node_t * node) : node(node) { }
        __lr_node_details_wrapper_t(const __node_t & node) : node(&node) { }

        const __node_t * node;
    };

    template<typename value_t>
    __lr_node_details_wrapper_t<value_t> _detail(const lr_tree_node_t<value_t> * node)
    {
        return __lr_node_details_wrapper_t<value_t>(node);
    }

    template<typename value_t>
    __lr_node_details_wrapper_t<value_t> _detail(const lr_tree_node_t<value_t> & node)
    {
        return __lr_node_details_wrapper_t<value_t>(node);
    }

    template<typename stream_t, typename value_t>
    stream_t & operator << (stream_t & stream,
                            const __lr_node_details_wrapper_t<value_t> & node_wrapper)
    {
        if(node_wrapper.node != nullptr)
            node_wrapper.node->write_to(stream);

        return stream;
    }

    template<typename stream_t, typename value_t>
    stream_t & operator << (stream_t & stream, const lr_tree_node_t<value_t> & node)
    {
        return stream << &node;
    }

    template<typename stream_t, typename value_t>
    stream_t & operator << (stream_t & stream, const lr_tree_node_t<value_t> * node)
    {
        if(node != nullptr)
            stream << node->value;
        return stream;
    }

    ////////// ////////// ////////// ////////// //////////

    template<typename t>
    class walk_list_t : private memory_base_t, public no_copy_ctor_t
    {
        typedef walk_list_t<t> __self_t;

    public:
        typedef t value_type;

        walk_list_t(memory_t * memory = nullptr)
            : memory_base_t(memory) { }

        walk_list_t(std::initializer_list<t> il, memory_t * memory = nullptr)
            : memory_base_t(memory)
        {
            append(il.begin(), il.end());
        }

        void push_back(const t & obj)
        {
            __check_buffer_size(__count + 1);
            __buffer[__count++] = obj;
        }

        template<typename itor_t>
        void append(itor_t begin, itor_t end)
        {
            std::copy(begin, end, std::back_inserter(*this));
        }

        template<typename pred_t>
        void walk(pred_t pred, int start_index = 0, int end_index = -1)
        {
            if(!__buffer)
                return;

            t * out = __buffer + start_index;
            for(t * obj = out, * end = __buffer + (end_index < 0? __count : end_index);
                obj < end; obj++)
            {
                if(pred(*obj))
                    *out++ = *obj;
            }

            __count = out - __buffer;
        }

        size_t size() const { return __count; }

        t & operator[] (size_t index) const
        {
            _A(index < __count);
            return __buffer[index];
        }

        void clear()
        {
            if(__buffer)
            {
                __free(__buffer);
                __buffer = nullptr;

                __size = 0;
                __count = 0;
            }
        }

        range_t<t *> all() const
        {
            if(__buffer == nullptr)
                return range_t<t *>(nullptr, nullptr);
            return range_t<t *>(__buffer, __buffer + __count);
        }

        template<typename comparer_t> void sort(comparer_t comparer)
        {
            return std::sort(begin(), end(), comparer);
        }

        t * begin() const { return __buffer; }
        t * end() const { return __buffer? __buffer + __count : nullptr; }

        ~walk_list_t()
        {
            if(__buffer)
                __free(__buffer);
        }

    private:
        size_t __size = 8, __count = 0;
        t * __buffer = nullptr;

        void __check_buffer_size(size_t size)
        {
            if(__buffer && __size >= size)
                return;

            while(__size < size)
            {
                __size *= 2;
            }

            t * new_buffer = __alloc_objs<t>(__size);
            if(__buffer != nullptr)
            {
                std::copy(__buffer, __buffer + __count, new_buffer);
                __free(__buffer);
            }

            __buffer = new_buffer;
        }
    };

    template<typename stream_t, typename t>
    stream_t & operator << (stream_t & stream, const walk_list_t<t> & list)
    {
        int index = 0;
        for(const t & obj : list.all())
        {
            if(index++ > 0)
                stream << ", ";

            stream << obj;
        }

        return stream;
    }

    ////////// ////////// ////////// ////////// //////////

    template<typename t, size_t init_size = 10>
    class small_vector_t : private memory_base_t
    {
        typedef small_vector_t<t, init_size> __self_t;

    public:
        typedef t   value_type;
        typedef t * iterator;

        small_vector_t(memory_t * memory = nullptr)
            : memory_base_t(memory), __p(__array)
        { }

        small_vector_t(std::initializer_list<t> il, memory_t * memory = nullptr)
            : memory_base_t(memory), __p(__array)
        {
            std::copy(il.begin(), il.end(), std::back_inserter(*this));
        }

        small_vector_t(__self_t && v)
        {
            if(v.__is_in_array())            
            {
                std::copy(v.__array, v.__array + array_size(v.__array), __array);
                __p = __array + (v.__p - v.__array);
            }
            else
            {
                __buffer_start = v.__buffer_start;
                __buffer_size = v.__buffer_size;
                __p = v.__p;
            }

            v.__p = nullptr;
            v.__buffer_start = nullptr;
        }

        small_vector_t(const __self_t & v) : memory_base_t(v.__memory)
        {
            if(v.__is_in_array())            
            {
                std::copy(v.__array, v.__array + array_size(v.__array), __array);
                __p = __array + (v.__p - v.__array);
            }
            else
            {
                __buffer_size = v.__buffer_size;
                __buffer_start = this->__alloc_objs<t>(__buffer_size);
                __p = __buffer_start + __buffer_size;
                std::copy(v.__buffer_start, v.__buffer_start + v.__buffer_size, __buffer_start);
            }
        }

        template<typename _value_t>
        t & push_back(_value_t && value)
        {
            if(__p >= __array && __p < __array + init_size)
            {
                *__p = value;
            }
            else if(__p == __array + init_size)
            {
                size_t buffer_size = init_size * 2;
                t * buffer_start = this->__alloc_objs<t>(buffer_size);

                std::copy(__array, __array + init_size, buffer_start);

                __buffer_start = buffer_start;
                __buffer_size = buffer_size;

                __p = __buffer_start + init_size;
                *__p = value;
            }
            else
            {
                if(__p - __buffer_start >= __buffer_size)
                {
                    size_t old_buffer_size = __buffer_size;
                    __buffer_size = old_buffer_size * 2;

                    t * new_buffer = this->__alloc_objs<t>(__buffer_size);
                    std::copy(__buffer_start, __p, new_buffer);

                    this->__free(__buffer_start);
                    __buffer_start = new_buffer;
                    __p = __buffer_start + old_buffer_size;
                }

                *__p = value;
            }

            return *__p++;
        }

        template<typename _objs_t>
        void push_front(const _objs_t & objs)
        {
            size_t obj_size = objs.size();

            if(__is_in_array())
            {
                t * p = __p + obj_size;
                if(p <= __array + init_size)
                {
                    std::move_backward(__array, __p, __p + obj_size);
                    std::copy(std::begin(objs), std::end(objs), __array);
                    __p += obj_size;
                }
                else
                {
                    size_t size = this->size(), new_size = size + obj_size;
                    size_t buffer_size = _alignd(size, new_size);
                    t * buffer_start = this->__alloc_objs<t>(buffer_size);

                    std::copy(std::begin(objs), std::end(objs), buffer_start);
                    std::move(__array, __array + size, buffer_start + obj_size);

                    __buffer_start = buffer_start;
                    __buffer_size  = buffer_size;
                    __p = __buffer_start + new_size;
                }
            }
            else
            {
                size_t size = this->size(), new_size = size + obj_size;
                if(new_size <= __buffer_size)
                {
                    std::move_backward(__buffer_start, __p, __p + obj_size);
                    std::copy(std::begin(objs), std::end(objs), __buffer_start);

                    __p += obj_size;
                }
                else
                {
                    __buffer_size = _alignd(size, new_size);
                    t * new_buffer = this->__alloc_objs<t>(__buffer_size);

                    std::copy(std::begin(objs), std::end(objs), new_buffer);
                    std::move(__buffer_start, __buffer_start + size, new_buffer + obj_size);
                    this->__free(__buffer_start);

                    __buffer_start = new_buffer;
                    __p = __buffer_start + new_size;
                }
            }
        }

        void truncate(size_t new_size)
        {
            if(new_size < size())
            {
                if(__is_in_array())
                    __p = __array + new_size;
                else
                    __p = __buffer_start + new_size;
            }
        }

        t & operator[](size_t index) const
        {
            if(__is_in_array())
                return __array[index];

            return __buffer_start[index];
        }

        t * begin() const
        {
            if(__is_in_array())
                return __array;

            return __buffer_start;
        }

        t * end() const
        {
            return __p;
        }

        size_t size() const
        {
            if(__is_in_array())
                return __p - __array;

            return __p - __buffer_start;
        }

        bool empty() const
        {
            return size() == 0;
        }

        void clear()
        {
            if(__is_in_array())
                __p = __array;
            else
                __p = __buffer_start;
        }

        operator string_t() const
        {
            return join_str(begin(), end(), _T(", "));
        }

        ~small_vector_t()
        {
            if(__buffer_start && !__is_in_array())
            {
                this->__free(__buffer_start);
            }
        }

    private:
        union
        {
            t __array[init_size + 1];

            struct
            {
                t * __buffer_start;
                size_t __buffer_size;
            };
        };

        t * __p;

        bool __is_in_array() const
        {
            return __p >= __array && __p <= __array + init_size;
        }
    };

    ////////// ////////// ////////// ////////// //////////

    template<typename key_t, typename value_t, size_t init_size = 5>
    class small_map_t : public no_copy_ctor_t
    {
        typedef small_map_t<key_t, value_t, init_size> __self_t;

    public:
        small_map_t(memory_t * memory = nullptr)
            : __inner_vector(memory)
        { }

        struct item_t
        {
            key_t   key;
            value_t value;
        };

        typedef item_t * iterator_t;

        void add(const key_t & key, const value_t & value)
        {
            for(item_t & it : __inner_vector)
            {
                if(it.key == key)
                {
                    it.value = value;
                    return;
                }
            }

            __inner_vector.push_back(item_t { key, value });
        }

        value_t & get(const key_t & key) const
        {
            for(item_t & it : __inner_vector)
            {
                if(it.key == key)
                {
                    return it.value;
                }
            }

            throw _EC(not_found);
        }

        iterator_t find(const key_t & key) const
        {
            for(item_t & it : __inner_vector)
            {
                if(it.key == key)
                {
                    return &it;
                }
            }

            return end();
        }

        iterator_t begin() const
        {
            return __inner_vector.begin();
        }

        iterator_t end() const
        {
            return __inner_vector.end();
        }

        value_t & operator [](const key_t & key)
        {
            for(item_t & it : __inner_vector)
            {
                if(it.key == key)
                {
                    return it.value;
                }
            }

            __inner_vector.push_back(item_t { key, _D(value_t) });
            return __inner_vector[__inner_vector.size() - 1].value;
        }

        value_t & operator [](const key_t & key) const
        {
            return get(key);
        }

        size_t size() const
        {
            return __inner_vector.size();
        }

        bool empty() const
        {
            return size() == 0;
        }

        void clear()
        {
            __inner_vector.clear();
        }

    private:
        small_vector_t<item_t, init_size> __inner_vector;
    };

    ////////// ////////// ////////// ////////// //////////

    template<typename key_t, typename value_t>
    class fixed_map_t : public memory_base_t
    {
        struct __item_t { key_t key; value_t value; };
        typedef fixed_map_t<key_t, value_t> __self_t;

    public:

        class iterator : public iterator_base_t<iterator>
        {
            typedef __self_t __owner_t;

        public:
            iterator(const __owner_t & owner, size_t index) : __owner(owner), __index(index) { }

            value_t get() const { return __owner[__index]; }
            void increase(int inc) { __index += inc; }
            bool equals(const iterator itor) const { return __index == itor.__index; }

        private:
            const __owner_t & __owner;
            size_t __index;
        };

        typedef iterator    itor_t;
        typedef key_t       key_type;
        typedef value_t     value_type;

        fixed_map_t(memory_t * memory = nullptr)
            : memory_base_t(memory) { }

        void append(const key_t & key, const value_t & value)
        {
            if(__items == nullptr)
                throw _EC(invalid_operation, _T("not initialized"));

            if(__p >= __items_end)
                throw _EC(overflow, _T("overflow"));

            *__p++ = __item_t { key, value };
        }

        void init(size_t size)
        {
            if(__items != nullptr)
                throw _EC(invalid_operation, _T("already initialized"));

            __items = this->__alloc_objs<__item_t>(size);
            __items_end = __items + size;
            __p = __items;
        }

        void commit()
        {
            std::sort(__items, __p, __compare);
        }

        itor_t find(const key_t & key) const
        {
            auto it = std::equal_range(__items, __p, __item_t { key }, __compare);
            if(it.first == it.second)
                return end();

            return itor_t(*this, it.first - __items);
        }

        itor_t begin() const { return itor_t(*this, 0); }
        itor_t end()   const { return itor_t(*this, __p - __items); }

        value_t operator [](size_t index) const { return __items[index].value; }

        ~fixed_map_t()
        {
            if(__items != nullptr)
                this->__free(__items);
        }

    private:
        __item_t * __items = nullptr, * __items_end, * __p;

        static bool __compare(const __item_t & a, const __item_t & b)
        {
            return a.key < b.key;
        }
    };

    ////////// ////////// ////////// ////////// //////////

    template<typename key_t, typename value_t>
    class auto_cache_t : public no_copy_ctor_t
    {
        typedef std::function<value_t (const key_t &)> __creator_t;
        typedef auto_cache_t<key_t, value_t> __self_t;

    public:
        template<typename creator_t>
        auto_cache_t(creator_t creator) : __creator(creator) { }

        value_t & get(const key_t & key)
        {
            auto it = __cache.find(key);
            if(it != __cache.end())
                return it->second;

            return __cache.insert(std::make_pair(key, __creator(key))).first->second;
        }

        value_t & operator[](const key_t & key)
        {
            return get(key);
        }

    private:
        std::map<key_t, value_t> __cache;
        __creator_t __creator;
    };

    ////////// ////////// ////////// ////////// //////////

    template<typename t>
    class respool_wrapper_t : public object_t, private memory_base_t
    {
    public:
        respool_wrapper_t(memory_t * memory = nullptr)
            : memory_base_t(memory)
        { }

        template<typename ... args_t>
        t * new_obj(args_t && ... args)
        {
            t * obj;
            if(!__queue.empty())
            {
                obj = __queue.front();
                __queue.pop();
                obj->~object_t();

                new(obj) t(std::forward<args_t>(args) ...);
            }
            else
            {
                obj = this->__new_obj<t>(std::forward<args_t>(args) ...);
            }

            return obj;
        }

        void release(t * obj)
        {
            _A(obj != nullptr);

            __queue.push(obj);
        }

        virtual ~respool_wrapper_t() override { }

    private:
        std::queue<t *> __queue;
    };

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        template<int idx, typename itor_t>
        class __zip_iterator_impl_base_t
        {
        public:
            typedef std::remove_reference_t<decltype(*_D(itor_t))> value_type;

            __zip_iterator_impl_base_t() { }

            template<typename pair_t>
            __zip_iterator_impl_base_t(pair_t pair)
                : __it(pair.first) , __it_end(pair.second)  { }

            bool move_next()
            {
                if(__it == __it_end)
                    return false;

                __value = *__it;
                ++__it;

                return true;
            }

            value_type get_value() const { return __value; }

        private:
            itor_t __it, __it_end;
            value_type __value;
        };

        // - - - - - - - - - - - - - - - - - - - - - - - - - -

        template<int idx, typename itor_t, typename ... itors_t>
        class __zip_iterator_impl_t : protected __zip_iterator_impl_base_t<idx, itor_t>
                                    , public    __zip_iterator_impl_t<idx - 1, itors_t ...>
        {
            typedef __zip_iterator_impl_base_t<idx, itor_t>      __base_t;
            typedef __zip_iterator_impl_t<idx - 1, itors_t ...>  __super_t;

        public:
            __zip_iterator_impl_t() { }

            template<typename pair_t, typename ... pairs_t>
            __zip_iterator_impl_t(pair_t pair, pairs_t ... pairs)
                : __base_t(pair), __super_t(pairs ...)
            { }

            using __base_t::get_value;
            using __base_t::move_next;

            template<typename tuple_t>
            void set_value(tuple_t & tuple)
            {
                tuple.assign_value(get_value());
                __super_t::set_value((typename tuple_t::super_t &)tuple);
            }

            bool move_next()
            {
                return __base_t::move_next() && __super_t::move_next();
            }
        };

        // - - - - - - - - - - - - - - - - - - - - - - - - - -

        template<typename itor_t>
        class __zip_iterator_impl_t<0, itor_t> : protected __zip_iterator_impl_base_t<0, itor_t>
        {
            typedef __zip_iterator_impl_base_t<0, itor_t> __base_t;

        public:
            using __base_t::__base_t;
            using __base_t::get_value;

            template<typename tuple_t>
            void set_value(tuple_t & tuple)
            {
                tuple.assign_value(get_value());
            }
        };

        template<int idx, typename value_t>
        class __zip_tuple_base_t
        {
            typedef __zip_tuple_base_t<idx, value_t> __self_t;

        public:
            __zip_tuple_base_t() = default;
            __zip_tuple_base_t(value_t value) : __value(value) { }

            template<typename _value_t>
            void assign_value(_value_t && value)
            {
                __value = value;
            }

            value_t get_value() const { return __value; }

        private:
            value_t __value;
        };

        // - - - - - - - - - - - - - - - - - - - - - - - - - -

        template<int idx, typename value_t, typename ... values_t>
        class __zip_tuple_t : public __zip_tuple_base_t<idx, value_t>
                            , public __zip_tuple_t<idx - 1, values_t ...>
        {
            typedef __zip_tuple_base_t<idx, value_t> __base_t;
            typedef __zip_tuple_t<idx, value_t, values_t ...> __self_t;

        public:
            typedef __zip_tuple_t<idx - 1, values_t ...> super_t;

            __zip_tuple_t() = default;

            __zip_tuple_t(value_t value, values_t ... values)
                : __base_t(value), super_t(values ...) { }

            using __base_t::assign_value;
            using __base_t::get_value;

            template<typename _tuple_t>
            __self_t & operator = (const _tuple_t & o)
            {
                assign_value(o.get_value());
                super_t::operator = ((const typename _tuple_t::super_t &)o);

                return *this;
            }
        };

        // - - - - - - - - - - - - - - - - - - - - - - - - - -

        template<typename value_t>
        class __zip_tuple_t<0, value_t> : public __zip_tuple_base_t<0, value_t>
        {
            typedef __zip_tuple_base_t<0, value_t> __base_t;
            typedef __zip_tuple_t<0, value_t> __self_t;

        public:
            __zip_tuple_t() = default;
            using __base_t::__base_t;
            using __base_t::assign_value;
            using __base_t::get_value;

            template<typename _tuple_t>
            __self_t & operator = (const _tuple_t & o)
            {
                assign_value(o.get_value());
                return *this;
            }
        };

        // - - - - - - - - - - - - - - - - - - - - - - - - - -

        template<typename ... ts_t>
        class __zip_iterator_t
        {
            typedef __zip_iterator_t<ts_t ...> __self_t;
            typedef __zip_iterator_impl_t<sizeof ... (ts_t) - 1, typename ts_t::iterator ...>
                                                                        __impl_t;

            typedef __zip_tuple_t<sizeof ... (ts_t) - 1, typename ts_t::value_type ...>
                                                                        value_type;

        public:
            __zip_iterator_t() : __impl(nullptr) { }
            __zip_iterator_t(__impl_t * impl) : __impl(impl)
            {
                _A(impl != nullptr);

                __move_next();
            }

            __self_t & operator ++()
            {
                _A(__impl != nullptr);

                __move_next();
                return *this;
            }

            __self_t operator ++(int)
            {
                _A(__impl != nullptr);

                __self_t it = *this;
                __move_next();
                return it;
            }

            void __move_next()
            {
                if(__impl->move_next())
                    __impl->set_value(__value);
                else
                    __impl = nullptr;
            }

            value_type & operator * () { return __value; }

            bool operator == (const __self_t & o) const
            {
                if(__impl == nullptr)
                    return o.__impl == nullptr;

                return false;
            }

            bool operator != (const __self_t & o) const
            {
                return ! operator == (o);
            }

        private:
            value_type __value;
            __impl_t * __impl;
        };

        // - - - - - - - - - - - - - - - - - - - - - - - - - -

        template<typename ... ts_t>
        class __zip_container_t
        {
            typedef __zip_iterator_impl_t<sizeof ... (ts_t) - 1, typename ts_t::iterator ...>
                                                                __impl_t;

        public:
            typedef std::tuple<typename ts_t::value_type ...> value_type;
            typedef __zip_iterator_t<ts_t ...> iterator;

            __zip_container_t(ts_t & ... ts)
                : __impl(std::make_pair(ts.begin(), ts.end()) ...)
                , __begin_itor(&__impl) { }

            iterator begin() const
            {
                return __begin_itor;
            }

            iterator end() const
            {
                return __end_itor;
            }

        private:
            __impl_t __impl;
            iterator __begin_itor, __end_itor;
        };
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    template<typename ... ts_t>
    using zip_tuple_t = __zip_tuple_t<sizeof ... (ts_t) - 1, ts_t ...>;

    template<typename ... ts_t>
    using zip_container_t = __zip_container_t<ts_t ...>;

    template<typename ... ts_t>
    constexpr zip_container_t<ts_t ...> zip(ts_t & ... ts)
    {
        return zip_container_t<ts_t ...>(ts ...);
    }

    template<typename ... ts_t>
    constexpr zip_tuple_t<ts_t & ...> tie(ts_t & ... ts)
    {
        return zip_tuple_t<ts_t & ...>(ts ...);
    }

    ////////// ////////// ////////// ////////// //////////

} }  // X_ROOT_NS::algorithm


#endif   //__ALGORITHM_COLLECTION_H__

