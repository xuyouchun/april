
#ifndef __ALGORITHM_COLLECTION_H__
#define __ALGORITHM_COLLECTION_H__


namespace X_ROOT_NS::algorithm {

    namespace
    {
        namespace __ns = algorithm;
    }


    ////////// ////////// ////////// ////////// //////////
    // heap_t

    template<typename obj_t, typename obj_creator_t> class heap_t;

    namespace
    {
        ////////// ////////// ////////// ////////// //////////
        // Objects create/destory functions for an object.

        template<typename obj_t>
        struct __obj_creator_t
        {
            // Creates an object with given arguments.
            template<typename ... args_t>
            static void new_obj(obj_t * hole, args_t && ... args)
            {
                new (hole) obj_t(std::forward<args_t>(args) ...);
            }

            // Destories an object. ( no need. )
            template<typename obj_itor_t>
            static void destory(range_t<obj_itor_t> & range) { }
        };

        ////////// ////////// ////////// ////////// //////////
        // Objects create/destory functions for object pointer.

        template<typename obj_t>
        struct __obj_creator_t<obj_t *>
        {
            // Creates an object pointer with given arguments.
            template<typename ... args_t>
            static void new_obj(obj_t ** hole, args_t && ... args)
            {
                new (*hole) obj_t(std::forward<args_t>(args) ...);
            }

            // Destories object by it's pointer.
            template<typename obj_itor_t>
            static void destory(range_t<obj_itor_t> & range)
            {
                for (auto it = range.begin(); it != range.end(); it++)
                {
                    operator delete(**it);
                }
            }
        };
    }

    ////////// ////////// ////////// ////////// //////////
    // The heap can allocate objects of the specified type.
    // The objects will deallocated with the heap, and no need to free them.

    template<typename obj_t, typename obj_creator_t=__obj_creator_t<obj_t>>
    class heap_t : public object_t, private memory_base_t
    {
        typedef heap_t<obj_t, obj_creator_t> __self_t;
        typedef object_t                     __super_t;

    public:
        typedef std::vector<obj_t *> obj_pool_t;
        typedef typename obj_pool_t::iterator obj_itor_t;
        typedef obj_t * value_type;

        // Constructor
        heap_t(memory_t * memory = nullptr, size_t row_size = 0)
            : memory_base_t(memory)
            , __row_size(row_size? row_size : max(1024 / sizeof(obj_t), 1))
        { }

        // Creates an object with given arguments.
        template<typename ... args_t>
        obj_t * new_obj(args_t && ... args)
        {
            obj_t * hole = __current_hole();
            obj_creator_t::new_obj(hole, std::forward<args_t>(args) ...);

            return hole;
        }

        // Creates object array with given arguments.
        template<typename output_itor_t, typename ... args_t>
        size_t new_objs(size_t count, output_itor_t output_itor, args_t && ... args)
        {
            size_t index = size();

            for (size_t k = 0; k < count; k++)
            {
                *output_itor++ = new_obj(std::forward<args_t>(args) ...);
            }

            return index;
        }

        // Creates an object with given arguments at specified index. 
        template<typename ... _args_t>
        obj_t * insert(size_t index, _args_t && ... args)
        {
            size_t size = this->size();

            if (index == size)
                return new_obj(std::forward<_args_t>(args) ...);

            if (index > size)
                throw _EC(invalid_operation, _T("index larger than heap size"));

            obj_t * hole = __move_next(index);
            obj_creator_t::new_obj(hole, std::forward<_args_t>(args) ...);

            return hole;
        }

        // Deletes an object at specified index.
        void delete_(size_t index)
        {
            size_t size = this->size();

            if (index >= size)
                throw _EC(invalid_operation, _T("index larger than heap size"));

            __move_previous(index + 1);
        }

        // Returns the object count in the heap.
        size_t size() const
        {
            return __size;
        }

        // Returns the object at the specified idnex.
        obj_t & operator[](size_t index) const
        {
            return __pool[index / __row_size][index % __row_size];
        }

        // Returns the last object created. returns nullptr if empty.
        obj_t * last() const
        {
            if (!__current_row_ptr)
                return nullptr;

            return __current_row_ptr + __current_col - 1;
        }

        // Clear all objects. ( deallocators of the objects will not executed. )
        void clear()
        {
            __current_row_ptr = nullptr;

            __current_col = 0;
            __current_row = -1;

            __size = 0;
        }

        // Deallocator
        virtual ~heap_t() override
        {
            auto range = _rrange(__pool);
            obj_creator_t::destory(range);

            for (auto it = __pool.begin(); it != __pool.end(); it++)
            {
                this->__free(*it);
            }
        }

        X_TO_STRING_IMPL(_T("heap_t"))

        // Iterator of elements
        template<typename _self_t>
        class _iterator : public iterator_base_t<_iterator<_self_t>>
        {
            typedef _iterator<_self_t> __iterator_self_t;

        public:
            // Constructor
            _iterator(const _self_t * self, int index = 0) : __self(self), __index(index) { }

            // Move to next iterator.
            void increase(int inc) { __index += inc; }

            // Whether two iterators are equals.
            bool equals(const _iterator & it) const { return it.__index == __index; }

            // Returns object at the specified index.
            obj_t * get() const { return &(*__self)[__index]; }

            // Returns difference of two iterators.
            int diff(const __iterator_self_t & it) const { return __index - it.__index; }

        private:
            const _self_t * __self;
            int __index;
        };

        typedef _iterator<__self_t> iterator;
        typedef _iterator<const __self_t> const_iterator;

        // Constant iterator at begin.
        const_iterator cbegin() const { return const_iterator(this); }

        // Constant iterator at end.
        const_iterator cend()   const { return const_iterator(this, size()); }

        // Iterator at begin.
        iterator begin() const { return iterator(this); }

        // Iterator at end.
        iterator end()   const { return iterator(this, size()); }

    private:
        obj_pool_t  __pool;
        obj_t *     __current_row_ptr = nullptr;
        const int   __row_size;
        int __current_col = 0, __current_row = -1;
        size_t      __size = 0;

        // Current hole for creating a new object.
        obj_t * __current_hole()
        {
            if (!__current_row_ptr || __current_col >= __row_size)
            {
                if (++__current_row < __pool.size())
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

            __size++;
            return __current_row_ptr + (__current_col++);
        }

        #define __H(_row, _col) (__pool[_row] + _col)
        #define __Copy(_dst_row, _dst_col, _src_row, _src_col, _count)                  \
            quick_copy(                                                                 \
                __H(_dst_row, _dst_col), __H(_src_row, _src_col),                       \
                (_count) * sizeof(obj_t)                                                \
            )

        // Move all objects to next from specified index.
        obj_t * __move_next(size_t index)
        {
            obj_t * hole = __current_hole();    // Append a new hole at last.

            int from_row = index / __row_size, from_col = index % __row_size;
            int row = __current_row, col = __current_col - 1;

            if (row > from_row)
            {
                __Copy(row, 1, row, 0, col);

                for (row--; row > from_row; row--)
                {
                    __Copy(row + 1, 0, row, __row_size - 1, 1);
                    __Copy(row, 1, row, 0, __row_size - 1);
                }

                __Copy(row + 1, 0, row, __row_size - 1, 1);
                __Copy(row, from_col + 1, row, from_col, __row_size - from_col - 1);
            }
            else
            {
                __Copy(from_row, from_col + 1, from_row, from_col, col - from_col);
            }

            return __H(from_row, from_col);
        }

        // Move all objects to previous from specified index.
        void __move_previous(size_t index)
        {
            int from_row = index / __row_size, from_col = index % __row_size;
            int row = __current_row, col = __current_col - 1;

            if (row > from_row)
            {
                __Copy(from_row, from_col - 1, from_row, from_col, __row_size - from_col);

                for (size_t row1 = from_row + 1; row1 < row; row1++)
                {
                    __Copy(row1 - 1, __row_size - 1, row1, 0, 1);
                    __Copy(row1, 0, row1, 1, __row_size - 1);
                }

                __Copy(row, 0, row, 1, col);
            }
            else
            {
                __Copy(from_row, from_col - 1, from_row, from_col, col - from_col + 1);
            }

            __size--;
            if (__current_col == 0)
            {
                __current_col = __row_size - 1;
                __current_row--;

                __current_row_ptr = __pool[__current_row];
            }
            else
            {
                __current_col--;
            }
        }

        #undef __Copy
        #undef __H

    };


    ////////// ////////// ////////// ////////// //////////
    // heap_t for array
    // Be used for creating a object array.

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

        // Constructor
        heap_t(memory_t * memory = nullptr, size_t row_size = 0)
            : memory_base_t(memory)
            , __row_size(row_size? row_size : max(1024 / sizeof(obj_t), 8))
            , __current_row(nullptr), __current_col(0), __current_size(0) { }

        // Creates a object array with specified size.
        obj_t * acquire(size_t n)
        {
            if (!__current_row || (__current_size - __current_col) < n)
            {
                if (n >= __row_size)
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
    
        // Returns a range of all arrays.
        range_t<obj_itor_t> all() const
        {
            return range(__arrays);
        }

        // Array count.
        size_t count() const
        {
            return __arrays.size();
        }

        // Destructor
        virtual ~heap_t() override
        {
            for (auto it = __rows.begin(); it != __rows.end(); it++)
            {
                this->__free((*it));
            }
        }

        // Returns the lost object created.
        obj_t * last() const
        {
            return __rows.size()? __rows[__rows.size() - 1] : nullptr;
        }

        X_TO_STRING_IMPL(_T("heap_t<obj_t[]>"))

    private:
        std::list<obj_t *> __rows;
        obj_t * __current_row;
        size_t __current_size;
        size_t __current_col;
        const size_t __row_size;
        obj_arrays_t __arrays;
    };

    ////////// ////////// ////////// ////////// //////////

    // A memory manager for creating various types of objects.
    // Do not need to free them, all objects will free with the heap.

    class xheap_t : public object_t, public memory_t
    {
        typedef int16_t __size_t;

    public:
    
        // Constructor
        xheap_t(const char_t * name = _T(""));
        xheap_t(bool disable_deallocate, const char_t * name = _T(""));

        // Allocates memory with specified size. ( memory_t::alloc )
        virtual void * alloc(size_t size, memory_flag_t flag = memory_flag_t::__default__) override;

        // Frees the specified memory. ( memory_t::free, do nothing )
        virtual void   free(void * p) override;

        // Realloc specified memory with the given size. ( memory_t::realloc, not supported )
        virtual void * realloc(void * p, size_t size, memory_flag_t flag) override;

        // Deallocator
        virtual ~xheap_t() override;

        X_TO_STRING

    private:
        static const size_t __large_object_size = 128;
        static const size_t __row_size = 1024;

        // A row structure for recoding a buffer and object count.
        struct __row_t
        {
            byte_t * buffer;
            __size_t object_count;
        };

        // A row collection structure for recording a row array.
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
        bool __disable_deallocate = false;
    };

    ////////// ////////// ////////// ////////// //////////

    // Bit array for recording a set of bool variables.

    class bit_array_t
    {
        typedef uint_type_t<sizeof(void *)> __underlying_t;
        static const size_t __underlying_size = sizeof(__underlying_t);
        static const size_t __underlying_bits = __underlying_size * 8;

    public:
        bit_array_t(size_t init_length = 1);
        ~bit_array_t();

        // Sets/clear flag at the specified index.
        void set(size_t index, bool value);

        // Gets flag at the specified index.
        // Returns not_found value when not found.
        bool get(size_t index) const;

        // Gets index of next true bit from the specified index.
        // Returns not_found value when not found.
        size_t index_of(bool value, size_t from = 0) const;

        // Equvivalent to get method.
        bool operator [] (size_t index) const
        {
            return get(index);
        }

        // Returns when not found.
        static const size_t not_found = max_value<size_t>();

    private:
        size_t __bit_length;
        size_t __data_length = 0;
        __underlying_t * __data = nullptr;

        // Checks and resizes memory to store bits of the specified count.
        void __check_length(size_t bit_length);

        // Position of underlying integer address that containing the specified bit.
        __underlying_t & __pos_of(size_t index, __underlying_t * out_flag) const;

        // Length of underlying integer enough to store specified bit counts.
        static size_t __data_length_of(size_t bit_length);

        // Next index of true bit from the specified index.
        static size_t __index_of_true(__underlying_t value, size_t from,
                    size_t end = __underlying_bits - 1);

        // Returns the unserlying interger value of the bit range.
        static __underlying_t __cut(__underlying_t value, size_t from, size_t to);
    };

    ////////// ////////// ////////// ////////// //////////

    // Resource pool for acquire/release object that can resued.

    template<typename t>
    class respool_t : public object_t
    {
        static const size_t __default_capacity = 8;
        typedef creator_t<t> __creator_t;

    public:
        // Constructor
        respool_t(memory_t * memory, __creator_t * creator, size_t capacity = __default_capacity)
            : __memory(memory? memory : default_memory_t::instance())
            , __creator(creator? creator : default_creator_t<t>::instance())
            , __capacity(capacity) { }

        // Constructor with specified capacity.
        respool_t(size_t capacity = __default_capacity)
            : respool_t(null<memory_t>, null<creator_t<t>>, capacity) { }

        // Acquires an object. 
        // Creates it when the pool is empty.
        t * acquire()
        {
            _L(__mutex);

            t * obj;
            if (__released_queue.empty())
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

        // Releases an object.
        void release(t * obj)
        {
            _A(obj != nullptr);
            _L(__mutex);

            auto it = __acquired_set.find(obj);
            if (it == __acquired_set.end())
                throw _EC(invalid_operation, _T("not in this pool"));

            __acquired_set.erase(it);
            __released_queue.push(obj);

            if (__released_queue.size() > __capacity * 3 / 2)
            {
                while (__released_queue.size() > __capacity)
                {
                    __creator->destory(__memory, __released_queue.front());
                    __released_queue.pop();
                }
            }
        }

        // Destructor, releases all object in the pool.
        virtual ~respool_t() override
        {
            for (t * obj : __acquired_set)
            {
                __creator->destory(__memory, obj);
            }

            while (!__released_queue.empty())
            {
                __creator->destory(__memory, __released_queue.front());
                __released_queue.pop();
            }
        }

        X_TO_STRING_IMPL(_T("respool_t"))

    private:
        std::set<t *> __acquired_set;
        std::queue<t *> __released_queue;

        std::mutex    __mutex;
        memory_t *    __memory;
        __creator_t * __creator;
        const size_t  __capacity;
    };

    ////////// ////////// ////////// ////////// //////////

    // Small vector for storing a few element.
    // It used the stack size to store elements, until it's no enought, then move to the heap.
    // Avoids allocates memory from the heap at first.

    template<typename _t, size_t _init_size = 10>
    class svector_t : private memory_base_t
    {
        typedef svector_t<_t, _init_size> __self_t;

    public:
        typedef _t          value_type;
        typedef _t *        iterator;
        typedef const _t *  const_iterator;

        // Constructor, with specified memory management.
        svector_t(memory_t * memory = nullptr)
            : memory_base_t(memory), __p(__array)
        { }

        // Constructor with specified memory management and initialized elements.
        svector_t(std::initializer_list<_t> il, memory_t * memory = nullptr)
            : memory_base_t(memory), __p(__array)
        {
            std::copy(il.begin(), il.end(), std::back_inserter(*this));
        }

        // Constructor, with a initialize list.
        template<typename _itor_t> svector_t(_itor_t begin, _itor_t end)
        {
            for (; begin != end; begin++)
            {
                push_back(*begin);
            }
        }

        // Move constructor.
        svector_t(__self_t && v)
        {
            if (v.__is_in_array())            
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

        __self_t operator = (__self_t && v)
        {
            new ((void *)this) __self_t(std::forward<__self_t>(v));
            return *this;
        }

        // Copy constructor.
        svector_t(const __self_t & v) : memory_base_t(v.__memory)
        {
            if (v.__is_in_array())            
            {
                std::copy(v.__array, v.__array + array_size(v.__array), __array);
                __p = __array + (v.__p - v.__array);
            }
            else
            {
                __buffer_size = v.__buffer_size;
                __buffer_start = this->__alloc_objs<_t>(__buffer_size);
                __p = __buffer_start + v.size();
                std::copy(v.__buffer_start, v.__buffer_start + v.__buffer_size, __buffer_start);
            }
        }

        // Pop the last value.
        void pop_back()
        {
            if (empty())
                throw _EC(invalid_operation, _T("vector is empty"));

            truncate(size() - 1);
        }

        // Pushes the value to the back.
        template<typename _value_t>
        _t & push_back(_value_t && value)
        {
            if (__p >= __array && __p < __array + _init_size)
            {
                *__p = value;
            }
            else if (__p == __array + _init_size)
            {
                size_t buffer_size = _init_size * 2;
                _t * buffer_start = this->__alloc_objs<_t>(buffer_size);

                std::copy(__array, __array + _init_size, buffer_start);

                __buffer_start = buffer_start;
                __buffer_size = buffer_size;

                __p = __buffer_start + _init_size;
                *__p = value;
            }
            else
            {
                if (__p - __buffer_start >= __buffer_size)
                {
                    size_t old_buffer_size = __buffer_size;
                    __buffer_size = old_buffer_size * 2;

                    _t * new_buffer = this->__alloc_objs<_t>(__buffer_size);
                    std::copy(__buffer_start, __p, new_buffer);

                    this->__free(__buffer_start);
                    __buffer_start = new_buffer;
                    __p = __buffer_start + old_buffer_size;
                }

                *__p = value;
            }

            return *__p++;
        }

        // Pushes the value to the front.
        template<typename _values_t>
        void push_front(const _values_t & values)
        {
            insert(values, 0);
        }

        // Pushes the value to the front.
        template<typename _values_t>
        void insert(const _values_t & values, size_t index)
        {
            if (index > size())
                throw _EC(invalid_operation, _T("index is too large"));

            size_t value_size = __ns::size(values);

            if (__is_in_array())
            {
                _t * p = __p + value_size;
                if (p <= __array + _init_size)
                {
                    std::move_backward(__array + index, __p, __p + value_size);
                    std::copy(std::begin(values), std::end(values), __array + index);
                    __p += value_size;
                }
                else
                {
                    size_t size = this->size(), new_size = size + value_size;
                    size_t buffer_size = _alignd(size, new_size);
                    _t * buffer_start = this->__alloc_objs<_t>(buffer_size);

                    std::copy(__array, __array + index, buffer_start);
                    std::copy(std::begin(values), std::end(values), buffer_start + index);
                    std::copy(__array + index, __array + size, buffer_start + index + value_size);

                    __buffer_start = buffer_start;
                    __buffer_size  = buffer_size;
                    __p = __buffer_start + new_size;
                }
            }
            else
            {
                size_t size = this->size(), new_size = size + value_size;
                if (new_size <= __buffer_size)
                {
                    std::move_backward(__buffer_start + index, __p, __p + value_size);
                    std::copy(std::begin(values), std::end(values), __buffer_start + index);
                    __p += value_size;
                }
                else
                {
                    __buffer_size = _alignd(size, new_size);
                    _t * new_buffer = this->__alloc_objs<_t>(__buffer_size);

                    std::copy(__buffer_start, __buffer_start + index, new_buffer);
                    std::copy(std::begin(values), std::end(values), new_buffer + index);
                    std::copy(__buffer_start + index, __buffer_start + size,
                                                    new_buffer + index + value_size);
                    this->__free(__buffer_start);

                    __buffer_start = new_buffer;
                    __p = __buffer_start + new_size;
                }
            }
        }

        // Truncates the vector to the specified size.
        void truncate(size_t new_size)
        {
            if (new_size < size())
            {
                if (__is_in_array())
                    __p = __array + new_size;
                else
                    __p = __buffer_start + new_size;
            }
        }

        // Returns the element at the specified index.
        _t & operator[](size_t index) const
        {
            if (__is_in_array())
                return (_t &)__array[index];

            return __buffer_start[index];
        }

        // The iterator at the begin.
        _t * begin() const
        {
            if (__is_in_array())
                return (_t *)__array;

            return __buffer_start;
        }

        // The const iterator at the begin.
        const _t * cbegin() const
        {
            return begin();
        }

        // The iterator at the end.
        _t * end() const
        {
            return __p;
        }

        // The iterator at the end.
        const _t * cend() const
        {
            return end();
        }

        // Returns a pointer of the first element.
        operator _t * () const
        {
            return begin();
        }

        operator const _t * () const
        {
            return begin();
        }

        // Returns element count.
        size_t size() const
        {
            if (__is_in_array())
                return __p - __array;

            return __p - __buffer_start;
        }

        // Whether the vector is empty.
        bool empty() const
        {
            return size() == 0;
        }

        // Removes all elements.
        void clear()
        {
            if (__is_in_array())
                __p = __array;
            else
                __p = __buffer_start;
        }

        // Convert to string.
        operator string_t() const
        {
            return join_str(begin(), end(), _T(", "));
        }

        // Destructor, free buffer if allocated on heap.
        ~svector_t()
        {
            if (__buffer_start && !__is_in_array())
            {
                this->__free(__buffer_start);
            }
        }

    private:
        union
        {
            _t __array[_init_size + 1];

            struct
            {
                _t * __buffer_start;
                size_t __buffer_size;
            };
        };

        _t * __p;

        // Whether the data is on heap.
        bool __is_in_array() const
        {
            return __p >= __array && __p <= __array + _init_size;
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // A tree that use a linked-list to store a tree.
    // Using a linked-list to store child nodes (first-child).

    template<typename value_t>
    class lr_tree_node_t : public object_t
    {
        typedef lr_tree_node_t<value_t> __self_t;

    public:
        lr_tree_node_t() = default;

        // Constructors with the value.
        template<typename _value_t>
        lr_tree_node_t(_value_t && value)
            : value(value), first_child(nullptr)
        { }

        // Appends a child, creates with the specified memory management.
        __self_t * append_child(memory_t * memory)
        {
            __self_t * node = memory_t::new_obj<__self_t>(memory);
            return append_child(node);
        }

        // Appends a child.
        __self_t * append_child(__self_t * node)
        {
            _A(node != nullptr);

            node->parent = this;
            node->next_sibling = this->first_child;
            this->first_child = node;

            return node;
        }

        // Appends a child, creates with the specified memory management and the value.
        template<typename _value_t>
        __self_t * append_child(memory_t * memory, _value_t && value)
        {
            __self_t * node = append_child(memory);
            node->value = std::forward<_value_t>(value);
            return node;
        }

        // Finds child by a value, auto creates with the specified memory management.
        template<typename _value_t>
        __self_t * find_or_append_child(memory_t * memory, _value_t && value)
        {
            __self_t * child = find_child(value);
            if (child == nullptr)
                child = append_child(memory, std::forward<_value_t>(value));

            return child;
        }

        // Appends sibling node, creates with the specified memory management.
        __self_t * append_sibling(memory_t * memory)
        {
            __self_t * node = memory_t::new_obj<__self_t>(memory);
            return append_sibling(node);
        }

        // Appends sibling node.
        __self_t * append_sibling(__self_t * node)
        {
            node->parent = this->parent;
            node->next_sibling = this->next_sibling;
            this->next_sibling = node;

            return node;
        }

        // Appends sibling node, creates with the specified memory management and value.
        template<typename _value_t>
        __self_t * append_sibling(memory_t * memory, _value_t && value)
        {
            __self_t * node = append_sibling(memory);
            node->value = value;
            return node;
        }

        // Removes child that contains the specified value.
        __self_t * remove_child(const value_t & value)
        {
            __self_t * node = find_child(value);
            if (node != nullptr)
                remove_child(node);
            return node;
        }

        // Removes child.
        bool remove_child(const __self_t * node)
        {
            _A(node != nullptr);

            if (first_child == nullptr)
                return false;

            if (node == first_child)
            {
                first_child = node->next_sibling;
                return true;
            }

            for (__self_t * child = first_child, * sibling;
                sibling = child->next_sibling, sibling; child = sibling)
            {
                if (sibling == node)
                {
                    child->next_sibling = sibling->next_sibling;
                    return true;
                }
            }

            return false;
        }

        // Removes self.
        void remove()
        {
            if (parent == nullptr)
                throw _E(common_error_code_t::invalid_operation, _T("cannot delete root node"));

            parent->remove_child(this);
        }

        // Whether the node is a leaf. ( no child. )
        bool is_leaf() const
        {
            return first_child == nullptr;
        }

        // Returns the child that contains the specified value.
        __self_t * find_child(const value_t & value) const
        {
            for (__self_t * node = this->first_child; node; node = node->next_sibling)
            {
                if (node->value == value)
                    return node;
            }

            return nullptr;
        }

        // Returns the child that match the predicate.
        template<typename pred_t>
        __self_t * find_child(pred_t pred) const
        {
            for (__self_t * node = this->first_child; node; node = node->next_sibling)
            {
                if (pred(node))
                    return node;
            }

            return nullptr;
        }

        // Walks all childs.
        template<typename pred_t>
        void walk(pred_t pred) const
        {
            if (!pred(this))
                return;

            for (__self_t * node = this->first_child; node; node = node->next_sibling)
            {
                node->walk(pred);
            }
        }

        // Walks all childs.
        template<typename pred_t>
        void walk(pred_t pred)
        {
            if (!pred(this))
                return;

            for (__self_t * node = this->first_child; node; node = node->next_sibling)
            {
                node->walk(pred);
            }
        }

        // Whether the node is a root node. ( no parent. )
        bool is_root() const
        {
            return this->parent == nullptr;
        }

        // Returns the height if the node.
        size_t height() const
        {
            size_t size = 0;
            const __self_t * node = this;
            while ((node = node->parent) != nullptr)
            {
                size++;
            }

            return size;
        }

        // Enumerator each child nodes.
        template<typename _f_t>
        void each_child(_f_t callback) const
        {
            const __self_t * node = first_child;
            while (node != nullptr)
            {
                callback(node);
                node = node->next_sibling;
            }
        }

        // Enumerator each child nodes.
        template<typename _f_t>
        void each_child(_f_t callback)
        {
            __self_t * node = first_child;
            while (node != nullptr)
            {
                callback(node);
                node = node->next_sibling;
            }
        }

        // Write the tree to a stream.
        template<typename stream_t>
        stream_t & write_to(stream_t & stream) const
        {
            stream << this->value;

            if (this->first_child)
            {
                stream << _T(" </ ");

                for (const __self_t * subnode = this->first_child; subnode != nullptr;
                        subnode = subnode->next_sibling)
                {
                    if (subnode != this->first_child)
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

        X_TO_STRING_IMPL(_T("lr_tree_node_t"))
    };

    // Creates a new lr_tree.
    template<typename _value_t, typename value_t = std::remove_reference_t<_value_t>>
    lr_tree_node_t<value_t> * new_lr_tree_root(memory_t * memory, _value_t && value)
    {
        typedef lr_tree_node_t<value_t> node_t;

        node_t * node = memory_t::new_obj<node_t>(memory);
        node->first_child  = nullptr;
        node->next_sibling = nullptr;
        node->parent       = nullptr;
        node->value        = std::forward<_value_t>(value);

        return node;
    }

    // Walk all nodes of a tree.
    template<typename _node_t, typename _f_t>
    void walk_tree(_node_t && root, _f_t callback)
    {
        if (root == nullptr)
            return;

        root->each_child([&](auto && node) {
            callback(node);
            walk_tree(node, callback);
        });
    }

    // Pick leaves of a tree.
    template<
        typename _node_t, typename _container_t,
        typename __node_t = std::remove_reference_t<_node_t>
    >
    void pick_leaves(_node_t && root, _container_t & container)
    {
        walk_tree(root, [&](auto && node) {

            if (node->is_leaf())
                container.push_back(node);
        });
    }

    template<
        typename _node_t,
        typename __node_t = std::remove_reference_t<_node_t>
    >
    svector_t<__node_t> pick_leaves(_node_t && root)
    {
        svector_t<__node_t> vector;

        pick_leaves(std::forward<_node_t>(root), vector);

        return std::move(vector);
    }

    // lr_tree wrapper for writing the tree to a stream.
    template<typename value_t>
    struct __lr_node_details_wrapper_t
    {
        typedef lr_tree_node_t<value_t> __node_t;
        __lr_node_details_wrapper_t(const __node_t * node) : node(node) { }
        __lr_node_details_wrapper_t(const __node_t & node) : node(&node) { }

        const __node_t * node;
    };

    // Returns a lr_tree wrapper. be used for writing the tree to a stream.
    template<typename value_t>
    __lr_node_details_wrapper_t<value_t> _detail(const lr_tree_node_t<value_t> * node)
    {
        return __lr_node_details_wrapper_t<value_t>(node);
    }

    // Returns a lr_tree wrapper. be used for writing the tree to a stream.
    template<typename value_t>
    __lr_node_details_wrapper_t<value_t> _detail(const lr_tree_node_t<value_t> & node)
    {
        return __lr_node_details_wrapper_t<value_t>(node);
    }

    // Writing the tree to a stream.
    template<typename stream_t, typename value_t>
    stream_t & operator << (stream_t & stream,
                            const __lr_node_details_wrapper_t<value_t> & node_wrapper)
    {
        if (node_wrapper.node != nullptr)
            node_wrapper.node->write_to(stream);

        return stream;
    }

    // Writing the tree to a stream.
    template<typename stream_t, typename value_t>
    stream_t & operator << (stream_t & stream, const lr_tree_node_t<value_t> & node)
    {
        return stream << &node;
    }

    // Writing the tree to a stream.
    template<typename stream_t, typename value_t>
    stream_t & operator << (stream_t & stream, const lr_tree_node_t<value_t> * node)
    {
        if (node != nullptr)
            stream << node->value;
        return stream;
    }

    //-------- ---------- ---------- ---------- ----------

    // A datastruct to store a lr_tree.
    template<typename _value_t>
    class lr_tree_state_t : public object_t
    {
        typedef lr_tree_state_t     __self_t;
        typedef object_t            __super_t;

        template<typename _t>
        using __vector_t = svector_t<_t, 8>;

        typedef lr_tree_node_t<_value_t> __node_t;

    public:
        lr_tree_state_t() = default;

        // Save a tree to data.
        void store(const __node_t * tree)
        {
            __data.clear();
            __node_counts.clear();

            if (tree == nullptr)
                return;

            std::queue<const __node_t *> queue;
            queue.push(tree);

            while (!queue.empty())
            {
                const __node_t * node = queue_pop(queue);
                __data.push_back(node->value);

                uint8_t count = 0;
                node->each_child([&](const __node_t * child) {
                    queue.push(child);
                    count++;
                });

                __node_counts.push_back(count);
            }
        }

        typedef std::function<void *()> create_function_t;

        // Restore a tree from saved data, prototype of creator is "void * ()".
        __node_t * restore(create_function_t creator) const
        {
            __memory_adapter_t adapter(creator);

            return restore((memory_t *)&adapter);
        }

        // Restore a tree from saved data.
        __node_t * restore(memory_t * memory = nullptr) const
        {
            if (__data.empty())
                return nullptr;

            size_t data_index = 0;
            __node_t * root = new_lr_tree_root(memory, __data[data_index++]);

            std::queue<__node_t *> queue;
            queue.push(root);

            for (size_t index = 0, count = __node_counts.size(); index < count; index++)
            {
                uint8_t child_count = __node_counts[index];
                
                if (queue.empty())
                    throw _EC(invalid_operation, _T("restore lr_tree failed, queue empty"));

                __node_t * node = queue_pop(queue), * last_node = nullptr;

                for (int k = 0; k < child_count; k++)
                {
                    if (data_index >= __data.size())
                        throw _EC(invalid_operation, _T("restore lr_tree failed, nodes empty"));

                    if (last_node == nullptr)
                        last_node = node->append_child(memory, __data[data_index]);
                    else
                        last_node = last_node->append_sibling(memory, __data[data_index]);

                    data_index++;
                    queue.push(last_node);
                }
            }

            return root;
        }

    private:
        __vector_t<_value_t>    __data;
        __vector_t<uint8_t>     __node_counts;

        // Adapter to convert a creator to memory_t interface.
        class __memory_adapter_t : public object_t, public memory_t
        {
        public:
            __memory_adapter_t(create_function_t creator) : __creator(creator) { }

            virtual void * alloc(size_t size, memory_flag_t flag) override
            {
                if (size != sizeof(__node_t))
                    throw _EC(invalid_operation, _T("unexpected object size"));

                return __creator();
            }

            virtual void free(void * obj) override
            {
                throw _unimplemented_error(this, _T("free"));
            }

            virtual void * realloc(void * obj, size_t size, memory_flag_t flag) override
            {
                throw _unimplemented_error(this, _T("realloc"));
            }

            X_TO_STRING_IMPL(_T("__memory_adapter_t"))

        private:
            create_function_t __creator;
        };
    };



    ////////// ////////// ////////// ////////// //////////

    // A list for removing specified elements quickly during walking.

    template<typename t>
    class walk_list_t : private memory_base_t, public no_copy_ctor_t
    {
        typedef walk_list_t<t> __self_t;

    public:
        typedef t value_type;

        // Constructor, with specified memory management.
        walk_list_t(memory_t * memory = nullptr)
            : memory_base_t(memory) { }

        // Constructor, with specified memory management and initialize elements.
        walk_list_t(std::initializer_list<t> il, memory_t * memory = nullptr)
            : memory_base_t(memory)
        {
            append(il.begin(), il.end());
        }

        // Push an element to the back.
        void push_back(const t & obj)
        {
            __check_buffer_size(__count + 1);
            __buffer[__count++] = obj;
        }

        // Append an element list to the back.
        template<typename itor_t>
        void append(itor_t begin, itor_t end)
        {
            std::copy(begin, end, std::back_inserter(*this));
        }

        // Walk the list in the specified range.
        template<typename pred_t>
        void walk(pred_t pred, int start_index = 0, int end_index = -1)
        {
            if (!__buffer)
                return;

            t * out = __buffer + start_index;
            for (t * obj = out, * end = __buffer + (end_index < 0? __count : end_index);
                obj < end; obj++)
            {
                if (pred(*obj))
                    *out++ = *obj;
            }

            __count = out - __buffer;
        }

        // Returns element count.
        size_t size() const { return __count; }

        // Returns the element at the index.
        t & operator[] (size_t index) const
        {
            _A(index < __count);
            return __buffer[index];
        }

        // Removes all elements.
        void clear()
        {
            __count = 0;
        }

        // Truncate to specified size.
        void truncate(size_t size)
        {
            if (size < __count)
                __count = size;
        }

        // Returns all elements.
        range_t<t *> all() const
        {
            if (__buffer == nullptr)
                return range_t<t *>(nullptr, nullptr);
            return range_t<t *>(__buffer, __buffer + __count);
        }

        // Sorts by the specified comparer.
        template<typename comparer_t> void sort(comparer_t comparer)
        {
            return std::sort(begin(), end(), comparer);
        }

        // Returns iterator at the begin.
        t * begin() const { return __buffer; }

        // Returns iterator at the end.
        t * end() const { return __buffer? __buffer + __count : nullptr; }

        // Destructor, free the buffer.
        ~walk_list_t()
        {
            if (__buffer)
                __free(__buffer);
        }

    private:
        size_t __size = 8, __count = 0;
        t * __buffer = nullptr;

        // Checks or resizes the buffer to store elements of specified size enough.
        void __check_buffer_size(size_t size)
        {
            if (__buffer && __size >= size)
                return;

            while (__size < size)
            {
                __size *= 2;
            }

            t * new_buffer = __alloc_objs<t>(__size);
            if (__buffer != nullptr)
            {
                std::copy(__buffer, __buffer + __count, new_buffer);
                __free(__buffer);
            }

            __buffer = new_buffer;
        }
    };

    // Writes a walk-list to a stream.
    template<typename stream_t, typename t>
    stream_t & operator << (stream_t & stream, const walk_list_t<t> & list)
    {
        int index = 0;
        for (const t & obj : list.all())
        {
            if (index++ > 0)
                stream << ", ";

            stream << obj;
        }

        return stream;
    }

    ////////// ////////// ////////// ////////// //////////

    // Small map use a array to store a set of key/value pairs.
    // Searching algorithm is O(n).
    // It has no remove/clear operations. only supportes add/find operations.
    // Be used for a small set of key/value pairs, use less storage memory than std::map.

    template<typename key_t, typename value_t, size_t init_size = 5>
    class small_map_t : public no_copy_ctor_t
    {
        typedef small_map_t<key_t, value_t, init_size> __self_t;

    public:
        // Constructor, with the specified memory management.
        small_map_t(memory_t * memory = nullptr)
            : __inner_vector(memory)
        { }

        // A structure to store a key/value pair.
        struct item_t
        {
            key_t   key;
            value_t value;
        };

        typedef item_t * iterator_t;

        // Add a key/value pair
        void add(const key_t & key, const value_t & value)
        {
            for (item_t & it : __inner_vector)
            {
                if (it.key == key)
                {
                    it.value = value;
                    return;
                }
            }

            __inner_vector.push_back(item_t { key, value });
        }

        // Gets a value by the key.
        value_t & get(const key_t & key) const
        {
            for (item_t & it : __inner_vector)
            {
                if (it.key == key)
                {
                    return it.value;
                }
            }

            throw _EC(not_found);
        }

        // Finds the iterator by the key.
        iterator_t find(const key_t & key) const
        {
            for (item_t & it : __inner_vector)
            {
                if (it.key == key)
                {
                    return &it;
                }
            }

            return end();
        }

        // Returns the begin iterator.
        iterator_t begin() const
        {
            return __inner_vector.begin();
        }

        // Returns the end iterator.
        iterator_t end() const
        {
            return __inner_vector.end();
        }

        // Gets/Sets a value by the key.
        value_t & operator [](const key_t & key)
        {
            for (item_t & it : __inner_vector)
            {
                if (it.key == key)
                {
                    return it.value;
                }
            }

            __inner_vector.push_back(item_t { key, _D(value_t) });
            return __inner_vector[__inner_vector.size() - 1].value;
        }

        // Gets a value by the key.
        value_t & operator [](const key_t & key) const
        {
            return get(key);
        }

        // Returns element count.
        size_t size() const
        {
            return __inner_vector.size();
        }

        // Whether the map is empty.
        bool empty() const
        {
            return size() == 0;
        }

        // Removes all elements.
        void clear()
        {
            __inner_vector.clear();
        }

    private:
        svector_t<item_t, init_size> __inner_vector;
    };

    ////////// ////////// ////////// ////////// //////////

    // Fixed map contains a set of sorted elements.
    // It uses a sorted array to store a set of key/value pairs.
    // Find by binary-search algorithm, O(log n).
    // Used less storage memory that std::map.

    template<typename key_t, typename value_t>
    class fixed_map_t : public memory_base_t
    {
        struct __item_t { key_t key; value_t value; };
        typedef fixed_map_t<key_t, value_t> __self_t;

    public:

        // Iterator 
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

        // Constructor, with the specified memory management.
        fixed_map_t(memory_t * memory = nullptr)
            : memory_base_t(memory) { }

        // Append a key/value pair.
        void append(const key_t & key, const value_t & value)
        {
            if (__items == nullptr)
                throw _EC(invalid_operation, _T("not initialized"));

            if (__p >= __items_end)
                throw _EC(overflow, _T("overflow"));

            *__p++ = __item_t { key, value };
        }

        // Initialize with the specified size.
        void init(size_t size)
        {
            if (__items != nullptr)
                throw _EC(invalid_operation, _T("already initialized"));

            __items = this->__alloc_objs<__item_t>(max(size, 1));
            __items_end = __items + size;
            __p = __items;
        }

        // Commits to sort the elements by keys.
        void commit()
        {
            std::sort(__items, __p, __compare);
        }

        // Finds iterator by the key.
        itor_t find(const key_t & key) const
        {
            auto it = std::equal_range(__items, __p, __item_t { key }, __compare);
            if (it.first == it.second)
                return end();

            return itor_t(*this, it.first - __items);
        }

        // Returns the begin iterator.
        itor_t begin() const { return itor_t(*this, 0); }

        // Returns the end iterator.
        itor_t end()   const { return itor_t(*this, __p - __items); }

        // Returns the value at the specified index.
        value_t operator [](size_t index) const { return __items[index].value; }

        // Destructor, free buffer.
        ~fixed_map_t()
        {
            if (__items != nullptr)
                this->__free(__items);
        }

    private:
        __item_t * __items = nullptr, * __items_end, * __p = nullptr;

        // Compares two elements.
        static bool __compare(const __item_t & a, const __item_t & b)
        {
            return a.key < b.key;
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // Auto cache data structure maintenancing a map.
    // Auto creates by the specified creator when find failed.

    template<typename key_t, typename value_t>
    class auto_cache_t : public no_copy_ctor_t
    {
        typedef std::function<value_t (const key_t &)> __creator_t;
        typedef auto_cache_t<key_t, value_t> __self_t;

    public:

        // Constructor, with specified creator.
        template<typename creator_t>
        auto_cache_t(creator_t creator) : __creator(creator) { }

        // Returns value of the key.
        value_t & get(const key_t & key)
        {
            auto it = __cache.find(key);
            if (it != __cache.end())
                return it->second;

            return __cache.insert(std::make_pair(key, __creator(key))).first->second;
        }

        // Equivalant to get method.
        value_t & operator[](const key_t & key)
        {
            return get(key);
        }

    private:
        std::map<key_t, value_t> __cache;
        __creator_t __creator;
    };

    ////////// ////////// ////////// ////////// //////////

    // Resource pool wrapper
    // Uses a queue to store released resource that used by next creating.

    template<typename t>
    class respool_wrapper_t : public object_t, private memory_base_t
    {
    public:

        // Constructor, use the specified memory management.
        respool_wrapper_t(memory_t * memory = nullptr)
            : memory_base_t(memory)
        { }

        // Creates a new object, Returns element in the queue if not empty.
        template<typename ... args_t>
        t * new_obj(args_t && ... args)
        {
            t * obj;
            if (!__queue.empty())
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

        // Releases object, store to the queue.
        void release(t * obj)
        {
            _A(obj != nullptr);

            __queue.push(obj);
        }

        virtual ~respool_wrapper_t() override { }

        X_TO_STRING_IMPL(_T("respool_wrapper_t"))

    private:
        std::queue<t *> __queue;
    };

    ////////// ////////// ////////// ////////// //////////

    // zip-iterator is used for put multipy iterators into one 'for' loop statement.
    /* Likes:

       std::vector<int>   arr1({ 1, 2, 3, 4, 5 });
       std::vector<long>  arr2({ 4, 5, 6, 7, 8 });

       for (auto && it in zip(arr1, arr2))
       {
            int value1;
            long value2;

            it.tie(value1, value2);

            std::cout << value1 << ", " << value2 << std::endl;
       }
    */

    namespace
    {
        // The zip iterator base implemention.
        template<int idx, typename itor_t>
        class __zip_iterator_impl_base_t
        {
        public:
            typedef std::remove_reference_t<decltype(*_D(itor_t))> value_type;

            __zip_iterator_impl_base_t() { }

            template<typename pair_t>
            __zip_iterator_impl_base_t(pair_t pair)
                : __it(pair.first) , __it_end(pair.second)  { }

            // Move to the next iterator.
            bool move_next()
            {
                if (__it == __it_end)
                    return false;

                __value = *__it;
                ++__it;

                return true;
            }

            // Returns value of the iterator.
            value_type get_value() const { return __value; }

        private:
            itor_t __it, __it_end;
            value_type __value;
        };

        // - - - - - - - - - - - - - - - - - - - - - - - - - -

        // Zip iterator implemention.
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

            // Set the tuple value.
            template<typename _tuple_t>
            void set_value(_tuple_t & tuple)
            {
                tuple.assign_value(get_value());
                __super_t::set_value((typename _tuple_t::super_t &)tuple);
            }

            bool move_next()
            {
                return __base_t::move_next() && __super_t::move_next();
            }
        };

        // - - - - - - - - - - - - - - - - - - - - - - - - - -

        // Zip iterator implemention.
        template<typename itor_t>
        class __zip_iterator_impl_t<0, itor_t> : protected __zip_iterator_impl_base_t<0, itor_t>
        {
            typedef __zip_iterator_impl_base_t<0, itor_t> __base_t;

        public:
            using __base_t::__base_t;
            using __base_t::get_value;

            template<typename _tuple_t>
            void set_value(_tuple_t & tuple)
            {
                tuple.assign_value(get_value());
            }
        };

        // Zip tuple base implemention.
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

        // Zip tuple implemention.
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

        // Zip tuple implemention.
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

        // Zip iterator implemention.
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

            // Move next
            __self_t & operator ++()
            {
                _A(__impl != nullptr);

                __move_next();
                return *this;
            }

            // Move next
            __self_t operator ++(int)
            {
                _A(__impl != nullptr);

                __self_t it = *this;
                __move_next();
                return it;
            }

            // Move next
            void __move_next()
            {
                if (__impl->move_next())
                    __impl->set_value(__value);
                else
                    __impl = nullptr;
            }

            // Returns the value.
            value_type & operator * () { return __value; }

            // Returns whether this iterator is equals to another.
            bool operator == (const __self_t & o) const
            {
                if (__impl == nullptr)
                    return o.__impl == nullptr;

                return false;
            }

            // Returns whether this iterator is not equals to another.
            bool operator != (const __self_t & o) const
            {
                return ! operator == (o);
            }

        private:
            value_type __value;
            __impl_t * __impl;
        };

        // - - - - - - - - - - - - - - - - - - - - - - - - - -

        // The zip container implemention.
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

            // Returns the begin iterator.
            iterator begin() const
            {
                return __begin_itor;
            }

            // Returns the end iterator.
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

    // The zip tuple structure is used for pack multipy iterators.
    template<typename ... ts_t>
    using zip_tuple_t = __zip_tuple_t<sizeof ... (ts_t) - 1, ts_t ...>;

    // The zip container structure is used for pack multipy containers.
    template<typename ... ts_t>
    using zip_container_t = __zip_container_t<ts_t ...>;

    // Creates a zip container.
    template<typename ... ts_t>
    constexpr zip_container_t<ts_t ...> zip(ts_t & ... ts)
    {
        return zip_container_t<ts_t ...>(ts ...);
    }

    // Assigns a set of variables with the zip tuple.
    template<typename ... ts_t>
    constexpr zip_tuple_t<ts_t & ...> tie(ts_t & ... ts)
    {
        return zip_tuple_t<ts_t & ...>(ts ...);
    }

    ////////// ////////// ////////// ////////// //////////

}  // X_ROOT_NS::algorithm


#endif   //__ALGORITHM_COLLECTION_H__

