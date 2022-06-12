
#ifndef __ALGORITHM_STD_H__
#define __ALGORITHM_STD_H__

namespace X_ROOT_NS::algorithm {

    // Useful functions for std::algorithm.

    ////////// ////////// ////////// ////////// //////////

    // The iterator base type, helping easily to write a iterator.

    template<typename _this_t>
    class iterator_base_t
    {
        typedef iterator_base_t<_this_t> __self_t;

    public:
        iterator_base_t() = default;

        decltype(auto) operator *  () { return ((_this_t *)this)->get(); }
        decltype(auto) operator -> () { return ((_this_t *)this)->get(); }

        // Moves to the next iterator.
        _this_t & operator ++ ()
        {
            __this()->increase(1);
            return *__this();
        }

        // Moves to the next iterator.
        _this_t operator ++ (int)
        {
            _this_t it = *__this();
            __this()->increase(1);
            return it;
        }

        // Moves to the next iterator.
        _this_t & operator += (int inc)
        {
            __this()->increase(inc);
            return *__this();
        }

        // Moves to a iterator by increasing the specified step.
        _this_t operator + (int inc)
        {
            _this_t it = *__this();
            it.increase(inc);
            return it;
        }

        // Moves to the previous iteraotr.
        _this_t & operator -- ()
        {
            __this()->increase(-1);
            return *__this();
        }

        // Moves to the previous iteraotr.
        _this_t operator -- (int)
        {
            _this_t it = *__this();
            __this()->increase(-1);
            return it;
        }

        // Moves to a iterator by decreasing the specified step.
        _this_t & operator -= (int inc)
        {
            __this()->increase(-inc);
            return *__this();
        }

        // Moves to a iterator by decreasing the specified step.
        _this_t operator - (int inc)
        {
            _this_t it = *__this();
            it.increase(-inc);
            return it;
        }

        // Returns the differences of two iterators.
        int operator - (const _this_t & other) const
        {
            return __this()->diff(other);
        }

        // Returns whether the two iterators are equals.
        bool operator == (const _this_t & other) const
        {
            return __this()->equals(other);
        }

        // Returns whether the two iterators are not equals.
        bool operator != (const _this_t & other) const
        {
            return !__this()->equals(other);
        }

    private:
        _this_t * __this() noexcept { return (_this_t *)this; }
        const _this_t * __this() const noexcept { return (const _this_t *)this; }
    };

    ////////// ////////// ////////// ////////// //////////

    // Returns size of a container.
    template <typename _container_t>
    constexpr auto size(_container_t && c) -> decltype(c.size()) { return c.size(); }

    // Returns size of a array.
    template <typename _tp_t, size_t _sz>
    constexpr size_t size(const _tp_t (&)[_sz]) noexcept { return _sz; }

    ////////// ////////// ////////// ////////// //////////

    // Writes a key/value pair to a stream.
    template<typename _stream_t, typename _key_t, typename _value_t>
    _stream_t & operator << (_stream_t & stream, const std::pair<_key_t, _value_t> & pr)
    {
        return stream << pr.first << _T(":") << pr.second;
    }

    // Gets the value from the map by the key, Auto creates when not found.
    template<typename _map_t, typename _key_t, typename _creator_t>
    auto & map_get(_map_t && map, _key_t && key, _creator_t creator,
                                                 bool * out_created_new = nullptr)
    {
        auto it = map.find(key);
        if (it != map.end())
        {
            assign_value(out_created_new, false);
            return it->second;
        }

        map[key] = creator();
        assign_value(out_created_new, true);
        return map[key];
    }

    // Gets the value from the map by the key, Auto creates when not found.
    template<typename _map_t, typename _key_t>
    auto & map_get(_map_t && map, _key_t && key, bool * out_created_new = nullptr)
    {
        auto it = map.find(key);
        if (it != map.end())
        {
            assign_value(out_created_new, false);
            return it->second;
        }

        typedef std::remove_reference_t<decltype(it->second)> value_t;
        map[key] = value_t();
        assign_value(out_created_new, true);
        return map[key];
    }

    // Inserts a key/value pair into the map.
    template<typename _map_t, typename _key_t, typename _value_t>
    bool map_insert(_map_t && map, _key_t && key, _value_t && value)
    {
        typedef std::remove_reference_t<_map_t> map_t;
        typedef typename map_t::value_type pair_t;

        return map.insert(pair_t(std::forward<_key_t>(key), std::forward<_value_t>(value))).second;
    }

    // Inserts item into the set.
    template<typename _set_t, typename _item_t>
    bool set_insert(_set_t && set, _item_t && item)
    {
        return set.insert(std::forward<_item_t>(item)).second;
    }

    // Pop an element from the queue.
    // Throws exception when the queue is empty.
    template<typename _queue_t>
    auto & queue_pop(_queue_t && queue)
    {
        if (queue.size() == 0)
            throw _E(common_error_code_t::invalid_operation, _T("queue empty"));

        auto & value = queue.front();
        queue.pop();
        return value;
    }

    // Pop an element from the stack.
    // Throws exception when the stack is empty.
    template<typename _stack_t>
    auto & stack_pop(_stack_t && stack)
    {
        if (stack.size() == 0)
            throw _E(common_error_code_t::invalid_operation, _T("stack empty"));

        auto & value = stack.top();
        stack.pop();
        return value;
    }

    // Pop an element from the front of the list.
    // Throw exception when the list is empty.
    template<typename _list_t>
    auto & list_pop_front(_list_t && list)
    {
        if (list.size() == 0)
            throw _E(common_error_code_t::invalid_operation, _T("list empty"));

        auto & value = list.front();
        list.pop_front();
        return value;
    }

    // Pop an element from the back of the list.
    // Throw exception when the list is empty.
    template<typename _list_t>
    auto & list_pop_back(_list_t && list)
    {
        if (list.size() == 0)
            throw _E(common_error_code_t::invalid_operation, _T("list empty"));

        auto & value = list.back();
        list.pop_back();
        return value;
    }

    // Returns whether a value can be found in the range [begin, end].
    template<typename _itor_t, typename _t>
    bool contains(_itor_t begin, _itor_t end, _t && value)
    {
        return std::find(begin, end, value) != end;
    }

    // Returns whether a value can be found in the container.
    template<typename _container_t, typename _t>
    bool contains(const _container_t & container, _t && value)
    {
        return contains(std::begin(container), std::end(container), value);
    }

    // Returns an iterator to the first element in the range, for which pred returns true.
    // If no such element is found, return the container.end().
    template<typename _container_t, typename _pred_t>
    auto find_if(_container_t && container, _pred_t pred)
    {
        return std::find_if(std::begin(container), std::end(container), pred);
    }

    template<typename _container_t, typename _t>
    auto find(_container_t && container, _t && value)
    {
        return std::find(std::begin(container), std::end(container), value);
    }

    // Returns true if any pred returns true for any element in the container.
    template<typename _container_t, typename _pred_t>
    bool any_of(_container_t && container, _pred_t pred)
    {
        return std::any_of(std::begin(container), std::end(container), pred);
    }

    // Returns true if any element in the container is nullptr.
    template<typename _container_t>
    bool any_of_empty(_container_t && container)
    {
        return any_of(container, [](auto & v) { return (void *)v == nullptr; });
    }

    // Returns true if pred returns false for all of elements.
    template<typename _container_t, typename _pred_t>
    bool none_of(_container_t && container, _pred_t pred)
    {
        return std::none_of(std::begin(container), std::end(container), pred);
    }

    // Returns true if none of elements is nullptr.
    template<typename _container_t>
    bool none_of_empty(_container_t && container)
    {
        return none_of(container, [](auto & v) { return (void *)v == nullptr; });
    }

    // Returns true if pred returns true for all of lements.
    template<typename _container_t, typename _pred_t>
    bool all_of(_container_t && container, _pred_t pred)
    {
        return std::all_of(container.begin(), container.end(), pred);
    }

    // Returns true if all of elements is nullptr.
    template<typename _container_t>
    bool all_of_empty(_container_t && container)
    {
        return all_of(container, [](auto & v) { return (void *)v == nullptr; });
    }

    // Returns the number of elements in the container.
    template<typename _container_t>
    size_t count(_container_t && container)
    {
        return count(std::begin(container), std::end(container));
    }

    // Returns the number of elements in the range [begin, end]
    template<typename _itor_t> size_t count(_itor_t begin, _itor_t end)
    {
        size_t size = 0;
        for (; begin != end; begin++)
        {
            size++;
        }

        return size;
    }

    // Returns the sum of elements in the range [begin, end]
    template<typename _itor_t> auto sum(_itor_t begin, _itor_t end)
    {
        typedef decltype(*begin + *begin) sum_t;
        sum_t sum = _D(sum_t);

        for (; begin != end; begin++)
        {
            sum += *begin;
        }

        return sum;
    }

    // Returns the sum of values that converted by converter for all elements
    //   in the range [begin, end]
    template<typename _itor_t, typename _converter_t>
    auto sum(_itor_t begin, _itor_t end, _converter_t converter)
    {
        typedef decltype(converter(*begin) + converter(*begin)) sum_t;
        sum_t sum = _D(sum_t);

        for (; begin != end; begin++)
        {
            sum += converter(*begin);
        }

        return sum;
    }

    // Copies the elements in the container into the range begining at inserter.
    template<typename _container_t, typename _inserter_t>
    void copy(_container_t && container, _inserter_t inserter)
    {
        std::copy(std::begin(container), std::end(container), inserter);
    }

    // Copies the elements in the container into the range begining at inserter.
    template<typename _container_t, typename _inserter_t>
    void copy(_container_t && container, _inserter_t inserter, size_t from)
    {
        std::copy(std::begin(container) + from, std::end(container), inserter);
    }

    // Copies the elements in the container into the range begining at inserter.
    template<typename _container_t, typename _inserter_t>
    void copy(_container_t && container, _inserter_t inserter, size_t from, size_t length)
    {
        std::copy(std::begin(container) + from, std::begin(container) + (from + length), inserter);
    }

    // Applies an operation sequentially to the elements in the container.
    //  and stores the result into the range that begins at inserter.
    template<typename _container_t, typename _inserter_t, typename _operation_t>
    void transform(_container_t && container, _inserter_t inserter, _operation_t operation)
    {
        std::transform(std::begin(container), std::end(container), inserter, operation);
    }

    // Sorts the elements in the container by ascending order.
    // Compare by the comparer.
    template<typename _container_t, typename _comparer_t>
    void sort(_container_t && container, _comparer_t comparer)
    {
        std::sort(std::begin(container), std::end(container), comparer);
    }

    // Sorts the elements in the container into ascending order.
    template<typename _container_t>
    void sort(_container_t && container)
    {
        std::sort(std::begin(container), std::end(container));
    }

    // Applies function func to each of the elements in the container.
    template<typename _container_t, typename _func_t>
    void for_each(_container_t && container, _func_t func)
    {
        std::for_each(std::begin(container), std::end(container), func);
    }

    // Distinct elements from container.
    template<typename _container_t, typename _output_iterator_t>
    void distinct(_container_t && container, _output_iterator_t output_iterator)
    {
        typedef std::remove_reference_t<decltype(*std::begin(container))> element_t;
        std::set<element_t> set;

        for (auto && it : container)
        {
            if (set.insert(it).second)
            {
                *output_iterator = it;
                ++output_iterator;
            }
        }
    }

    // Distinct elements from container.
    template<typename _container_t,
        typename _output_container_t = std::remove_reference_t<_container_t>
    >
    _output_container_t distinct(_container_t && container)
    {
        _output_container_t new_container;

        distinct(std::forward<_container_t>(container), std::back_inserter(new_container));

        return std::move(new_container);
    }

    // Returns first element of a container.
    template<typename _container_t>
    auto first_of(_container_t && container)
    {
        _A(!container.empty());
        return container[0];
    }

    // Returns last element of a container.
    template<typename _container_t>
    auto last_of(_container_t && container)
    {
        _A(!container.empty());
        return container[container.size() - 1];
    }

    // Returns element count of the collection, returns 0 if nullptr.
    template<typename _container_t> size_t size_of(_container_t * objs)
    {
        return objs == nullptr? 0 : objs->size();
    }

    // Returns whether the collection is empty of nullptr.
    template<typename _container_t> size_t is_empty(_container_t * objs)
    {
        return size_of(objs) == 0;
    }

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        // Append inserter
        template <typename _container_t, typename _inserter_t>
        class __insert_iterator_t : public std::iterator<std::output_iterator_tag,
                                                            void, void, void, void>
        {
            typedef __insert_iterator_t __self_t;

        protected:
            _container_t & __container;

        public:
            typedef std::remove_reference_t<_container_t> container_type;
            typedef typename container_type::value_type   value_type;

            explicit __insert_iterator_t(_container_t & x) : __container(x) { }

            template<typename _value_t>
            __self_t & operator = (_value_t && value)
            {
                _inserter_t::insert(__container, std::forward<_value_t>(value));
                return *this;
            }

            __self_t & operator*()     { return *this; }
            __self_t & operator++()    { return *this; }
            __self_t   operator++(int) { return *this; }
        };

        template<typename _container_t>
        struct __append_inserter_t
        {
            template<typename _value_t>
            X_ALWAYS_INLINE static void insert(_container_t & container, _value_t && value)
            {
                container.append(std::forward<_value_t>(value));
            }
        };
 
        template<typename _container_t>
        struct __inserter_t
        {
            template<typename _value_t>
            X_ALWAYS_INLINE static void insert(_container_t & container, _value_t && value)
            {
                container.insert(std::forward<_value_t>(value));
            }
        };
    }

    template <typename _container_t>
    X_INLINE auto append_inserter(_container_t && container)
    {
        return __insert_iterator_t<_container_t, __append_inserter_t<_container_t>>(container);
    }

    template <typename _container_t>
    X_INLINE auto inserter(_container_t && container)
    {
        return __insert_iterator_t<_container_t, __inserter_t<_container_t>>(container);
    }

    ////////// ////////// ////////// ////////// //////////

    // An empty output iterator.

    template<typename _t>
    class empty_output_iterator_t
    {
        typedef empty_output_iterator_t<_t> __self_t;

    public:
        _t & operator * () { return *(_t *)__temp_value; }

        __self_t & operator ++ ()    { return *this; }
        __self_t & operator ++ (int) { return *this; }

    private:
        static byte_t __temp_value[sizeof(_t)];
    };

    template<typename _t> byte_t empty_output_iterator_t<_t>::__temp_value[sizeof(_t)];

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        template<
            typename _iterator_t,
            bool _is_pointer = std::is_pointer<_iterator_t>::value  // When true
        >
        struct __element_type_of_iterator_t
        {
            typedef std::remove_pointer_t<_iterator_t> value_type_t;
        };

        template<typename _iterator_t>
        struct __element_type_of_iterator_t<_iterator_t, false>
        {
            typedef typename _iterator_t::container_type::value_type value_type_t;
        };
    }

    template<typename _iterator_t>
    using iterator_element_t = typename __element_type_of_iterator_t<
        std::remove_reference_t<_iterator_t>
    >::value_type_t;

    template<typename _container_t>
    using container_element_t = iterator_element_t<
        decltype(std::begin(*(std::remove_reference_t<_container_t> *)nullptr))
    >;

    ////////// ////////// ////////// ////////// //////////

}   // X_ROOT_NS::algorithm

#endif  // __ALGORITHM_STD_H__
