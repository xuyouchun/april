
#ifndef __ALGORITHM_H__
#define __ALGORITHM_H__

#include <common.h>

namespace X_ROOT_NS::algorithm {

    ////////// ////////// ////////// ////////// //////////

    // Algorithm error codes

    X_ENUM(algorighm_error_code_t)
        
    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    // Returns the max value of given values.

    template<typename t>
    X_INLINE constexpr decltype(auto) max(t && value)
    {
        return std::forward<t>(value);
    }

    template<typename t, typename t2>
    X_INLINE constexpr auto max(t && value1, t2 && value2) -> decltype(value1 + value2)
    {
        return value1 < value2? value2 : value1;
    }

    template<typename t, typename ... rest_t>
    X_INLINE constexpr decltype(auto) max(t value1, rest_t && ... rest)
    {
        return max(std::forward<t>(value1), max(std::forward<rest_t>(rest) ...));
    }

    //-------- ---------- ---------- ---------- ----------

    // Returns the min value of given values.

    template<typename t>
    X_INLINE constexpr decltype(auto) min(t && value)
    {
        return std::forward<t>(value);
    }

    template<typename t, typename t2>
    X_INLINE constexpr auto min(t && value1, t2 && value2) -> decltype(value1 + value2)
    {
        return value1 < value2? value1 : value2;
    }

    template<typename t, typename ... rest_t>
    X_INLINE constexpr decltype(auto) min(t && value1, rest_t && ... rest)
    {
        return min(std::forward<t>(value1), min(std::forward<rest_t>(rest) ...));
    }

    ////////// ////////// ////////// ////////// //////////

    // Return the max size of give value types.
    template<typename ... t>
    X_INLINE constexpr auto max_size()
    {
        return max(sizeof(t) ...);
    }

    // Return the min size of give value types.
    template<typename ... t>
    X_INLINE constexpr auto min_size()
    {
        return min(sizeof(t) ...);
    }

    ////////// ////////// ////////// ////////// //////////

    // Detemines wheather a value in the given values.

    template<typename t, typename t1>
    X_INLINE bool in(t && value, t1 && value1)
    {
        return value == value1;
    }

    template<typename t, typename t1, typename ... values_t>
    X_INLINE bool in(t && value, t1 && value1, values_t && ... rest)
    {
        return value == value1 || in(std::forward<t>(value), std::forward<values_t>(rest) ...);
    }

    ////////// ////////// ////////// ////////// //////////

    template<typename _t>
    X_INLINE _t * nvl(_t * obj, _t * obj2)
    {
        return obj != nullptr? obj : obj2;
    }

    ////////// ////////// ////////// ////////// //////////

    // Compares given values.
    // Return -1(less), 1(greater), 0(equals).

    template<typename t>
    X_INLINE int cmp(const t & t1, const t & t2)
    {
        return t1 < t2? -1 : t2 < t1? 1 : 0;
    }

    template<typename t, typename ... rest_t>
    X_INLINE int cmp(const t & t1, const t & t2, rest_t && ... rest)
    {
        return t1 < t2? -1 : t2 < t1? 1 : cmp(std::forward<rest_t>(rest) ...);
    }

    ////////// ////////// ////////// ////////// //////////

    // Selects a value from the given list, comparing by specified function.

    template<typename itor_t, typename cmp_t>
    itor_t select(itor_t begin, itor_t end, cmp_t cmp_func)
    {
        if (begin == end)
            return end;

        itor_t max = begin;
        for (itor_t it = begin + 1; it != end; it++)
        {
            if (cmp_func(*max, *it))
                max = it;
        }

        return max;
    }

    template<typename container_t, typename cmp_t>
    X_INLINE auto select(container_t & container, cmp_t cmp_func)
    {
        return select(container.begin(), container.end(), cmp_func);
    }

    // Selects the min value from the given list.
    template<typename itor_t>
    X_INLINE itor_t select_min(itor_t begin, itor_t end)
    {
        return select(begin, end, [](auto x, auto y) { return x < y; });
    }

    // Selects the min value from the given container.
    template<typename container_t>
    X_INLINE auto select_min(container_t & container)
    {
        return select_min(container.begin(), container.end());
    }

    // Selects the max value from the give list.
    template<typename itor_t>
    X_INLINE itor_t select_max(itor_t begin, itor_t end)
    {
        return select(begin, end, [](auto x, auto y) { return y < x; });
    }

    // Selects the max value from the give container.
    template<typename container_t>
    X_INLINE auto select_max(container_t & container)
    {
        return select_max(container.begin(), container.end());
    }

    ////////// ////////// ////////// ////////// //////////

    // Splits the given list by specified predicate function.
    template<typename itor_t, typename pred_t, typename callback_t>
    void split_if (itor_t begin, itor_t end, pred_t pred, callback_t callback)
    {
        if (begin == end)
            return;

        itor_t it0 = begin;
        for (itor_t it = begin; it != end; it++)
        {
            if (pred(*it))
            {
                callback(it0, it);
                it0 = it + 1;
            }
        }

        callback(it0, end);
    }

    // Splits the give list by specified separator.
    template<typename itor_t, typename separator_t, typename callback_t>
    void split(itor_t begin, itor_t end, const separator_t & separator, callback_t callback)
    {
        split_if (begin, end, [&separator](auto & it) { return it == separator; }, callback);
    }

    ////////// ////////// ////////// ////////// //////////

    // Copies data with specified length to a destinate pointer.
    template<typename t>
    void copy_array(const t * src, t * dst, size_t length)
    {
        _A(src != nullptr);
        _A(dst != nullptr);

        for (const t * src_end = src + length; src < src_end;)
        {
            *dst++ = *src++;
        }
    }

    // Assigns values to the given variables.

    template<typename dst_itor_t> void assign(dst_itor_t itor) { }

    template<typename dst_itor_t, typename item_t, typename ... rest_t>
    void assign(dst_itor_t itor, item_t item, rest_t && ... rest)
    {
        *itor++ = item;
        assign(itor, std::forward<rest_t>(rest) ...);
    }

    // Enumerator given values by calling specified callback function.

    template<typename f_t> void each(f_t f) { }
    template<typename item_t, typename f_t> void each(f_t f, item_t item) { f(item); }

    template<typename item_t, typename ... rest_t, typename f_t>
    void each(f_t f, item_t item, rest_t ... rest)
    {
        f(item);
        each(f, rest ...);
    }

    // Splits the ordered list by keys that selected by key selector.
    template<typename itor_t, typename key_selector_t, typename proc_t>
    void split_array(itor_t begin, itor_t end, key_selector_t key_selector, proc_t proc)
    {
        typedef decltype(key_selector(*begin)) key_t;

        if (begin == end)
            return;

        itor_t it0 = begin, it = begin + 1;
        key_t key0 = key_selector(*it0);

        for (; it != end; it++)
        {
            key_t key = key_selector(*it);
            if (!(key == key0))
            {
                proc(key0, it0, it);
                it0 = it;
                key0 = std::move(key);
            }
        }

        proc(key0, it0, it);
    }

    // Splits the ordered container by keys that selected by key selector.
    template<typename container_t, typename key_selector_t, typename proc_t>
    void split_array(const container_t & container, key_selector_t key_selector, proc_t proc)
    {
        return split_array(container.begin(), container.end(), key_selector, proc);
    }

    // Splits the ordered container.
    template<typename itor_t, typename proc_t>
    void split_array(itor_t begin, itor_t end, proc_t proc)
    {
        return split_array(begin, end, [](auto it) { return it; }, proc);
    }

    // Splits the ordered given container.
    template<typename container_t, typename proc_t>
    void split_array(const container_t & container, proc_t proc)
    {
        return split_array(container, [](auto it) { return it; }, proc);
    }

    ////////// ////////// ////////// ////////// //////////

    // Assigns values to the given list items.

    template<typename t>
    void assign_array(t * dst, const t & value, size_t length)
    {
        _A(dst != nullptr);

        for (const t * dst_end = dst + length; dst < dst_end;)
        {
            *dst++ = value;
        }
    }

    template<typename t, size_t size>
    void assign_array(t (&arr)[size], const t & value)
    {
        assign_array(arr, value, size);
    }

    // Sets the given list items to zero.

    template<typename t>
    void zero_array(t * dst, size_t length)
    {
        assign_array(dst, def_value<t>(), length);
    }

    template<typename t, size_t size>
    void zero_array(t (&arr)[size])
    {
        zero_array(arr, size);
    }

    namespace
    {
        template<size_t size, size_t unit_size, size_t n>
        struct __zero_bytes_t
        {
            typedef int_type_t<unit_size> __int_type_t;
            X_ALWAYS_INLINE static void zero(__int_type_t * p)
            {
                *p = (__int_type_t)0;
                __zero_bytes_t<size, unit_size, n - 1>::zero(p + 1);
            }
        };

        template<size_t size, size_t unit_size>
        struct __zero_bytes_t<size, unit_size, 0>
        {
            typedef int_type_t<unit_size> __int_type_t;
            X_ALWAYS_INLINE static void zero(__int_type_t * p)
            {
                const size_t new_unit_size = unit_size >> 1;
                typedef int_type_t<new_unit_size> __new_int_type_t;

                __zero_bytes_t<size, new_unit_size, (size % unit_size) / new_unit_size>
                    ::zero((__new_int_type_t *)p);
            }
        };

        template<size_t size> struct __zero_bytes_t<size, 1, 0>
        {
            typedef int_type_t<1> __int_type_t;
            X_ALWAYS_INLINE static void zero(__int_type_t * p) { }
        };

        template<size_t size>
        void __zero_bytes(byte_t * bytes)
        {
            const size_t unit_size = sizeof(arch_int_t);
            __zero_bytes_t<size, unit_size, size / unit_size>::zero((arch_int_t *)bytes);
        }
    }

    // Sets the specified buffer to zero.

    template<size_t size>
    void zero_bytes(byte_t * bytes)
    {
        _A(bytes != nullptr);
        __zero_bytes<size>(bytes);
    }

    template<size_t size>
    void zero_bytes(byte_t (&bytes)[size])
    {
        __zero_bytes<size>(&bytes);
    }

    template<typename t>
    void zero_object(t & obj)
    {
        __zero_bytes<sizeof(t)>((byte_t *)&obj);
    }

    ////////// ////////// ////////// ////////// //////////
    // Quick copy.

    namespace
    {
        template<size_t _size, bool _large = (_size > sizeof(uint64_t))>
        struct __quick_copy_t
        {
            X_ALWAYS_INLINE static void copy(void * dst, const void * src) _NE
            {
                switch (_size)
                {
                    case 0:
                        break;

                    case 1:
                        *(uint8_t *)dst = *(const uint8_t *)src;
                        break;

                    case 2:
                        *(uint16_t *)dst = *(const uint16_t *)src;
                        break;

                    case 3:
                        *(uint16_t *)dst = *(const uint16_t *)src;
                        *(uint8_t *)((uint16_t *)dst + 1) =
                                        *(const uint8_t *)((const uint16_t *)src + 1);
                        break;

                    case 4:
                        *(uint32_t *)dst = *(const uint32_t *)src;
                        break;

                    case 5:
                        *(uint32_t *)dst = *(const uint32_t *)src;
                        *(uint8_t *)((uint32_t *)dst + 1) =
                                        *(const uint8_t *)((const uint32_t *)src + 1);
                        break;

                    case 6:
                        *(uint32_t *)dst = *(const uint32_t *)src;
                        *(uint16_t *)((uint32_t *)dst + 1) =
                                        *(const uint16_t *)((const uint32_t *)src + 1);
                        break;

                    case 7:
                        *(uint32_t *)dst = *(const uint32_t *)src;
                        *(uint16_t *)((uint32_t *)dst + 1) =
                                        *(const uint16_t *)((const uint32_t *)src + 1);
                        *(uint8_t *)(((uint16_t *)((uint32_t *)dst + 1)) + 1) =
                            *(const uint8_t *)((const uint16_t *)((const uint32_t *)src + 1) + 1);
                        break;

                    case 8:
                        *(uint64_t *)dst = *(const uint64_t *)src;
                        break;
                }
            } 

            X_ALWAYS_INLINE static void copy_backward(void * dst, const void * src) _NE
            {
                switch (_size)
                {
                    case 0:
                        break;

                    case 1:
                        *(uint8_t *)dst = *(const uint8_t *)src;
                        break;

                    case 2:
                        *(uint16_t *)dst = *(const uint16_t *)src;
                        break;

                    case 3:
                        *(uint8_t *)((uint16_t *)dst + 1) =
                                        *(const uint8_t *)((const uint16_t *)src + 1);
                        *(uint16_t *)dst = *(const uint16_t *)src;
                        break;

                    case 4:
                        *(uint32_t *)dst = *(const uint32_t *)src;
                        break;

                    case 5:
                        *(uint8_t *)((uint32_t *)dst + 1) =
                                        *(const uint8_t *)((const uint32_t *)src + 1);
                        *(uint32_t *)dst = *(const uint32_t *)src;
                        break;

                    case 6:
                        *(uint16_t *)((uint32_t *)dst + 1) =
                                        *(const uint16_t *)((const uint32_t *)src + 1);
                        *(uint32_t *)dst = *(const uint32_t *)src;
                        break;

                    case 7:
                        *(uint8_t *)(((uint16_t *)((uint32_t *)dst + 1)) + 1) =
                            *(const uint8_t *)((const uint16_t *)((const uint32_t *)src + 1) + 1);
                        *(uint16_t *)((uint32_t *)dst + 1) =
                                        *(const uint16_t *)((const uint32_t *)src + 1);
                        *(uint32_t *)dst = *(const uint32_t *)src;
                        break;

                    case 8:
                        *(uint64_t *)dst = *(const uint64_t *)src;
                        break;
                }
            } 
        };

        template<size_t _size> // _large is _size > sizeof(uint64_t)
        struct __quick_copy_t<_size, true>
        {
            X_ALWAYS_INLINE static void copy(void * dst, const void * src) _NE
            {
                __quick_copy_t<sizeof(uint64_t)>::copy(dst, src);

                __quick_copy_t<_size - sizeof(uint64_t)>::copy(
                    (uint64_t *)dst + 1, (const uint64_t *)src + 1
                );
            }

            X_ALWAYS_INLINE static void copy_backward(void * dst, const void * src) _NE
            {
                __quick_copy_t<sizeof(uint64_t)>::copy_backward(
                    (byte_t *)dst + _size - 1,
                    (const byte_t *)src + _size - 1
                );

                __quick_copy_t<_size - sizeof(uint64_t)>::copy_backward(
                    (uint64_t *)dst, (const uint64_t *)src
                );
            }
        };
    }

    template<size_t _size, bool _backward = false>
    X_ALWAYS_INLINE void quick_copy(void * dst, const void * src) _NE
    {
        if (_backward)
            __quick_copy_t<_size>::copy_backward(dst, src);
        else
            __quick_copy_t<_size>::copy(dst, src);
    }

    void quick_copy(void * dst, const void * src, size_t size) _NE;

    ////////// ////////// ////////// ////////// //////////
    // Quick zero.

    namespace
    {
        template<size_t _size, bool _large = (_size > sizeof(uint64_t))>
        struct __quick_zero_t
        {
            X_ALWAYS_INLINE static void zero(void * p) _NE
            {
                switch (_size)
                {
                    case 0:
                        break;

                    case 1:
                        *(uint8_t *)p = 0;
                        break;

                    case 2:
                        *(uint16_t *)p = 0;
                        break;

                    case 3:
                        *(uint16_t *)p = 0;
                        *(uint8_t *)((uint16_t *)p + 1) = 0;
                        break;

                    case 4:
                        *(uint32_t *)p = 0;
                        break;

                    case 5:
                        *(uint32_t *)p = 0;
                        *(uint8_t *)((uint32_t *)p + 1) = 0;
                        break;

                    case 6:
                        *(uint32_t *)p = 0;
                        *(uint16_t *)((uint32_t *)p + 1) = 0;
                        break;

                    case 7:
                        *(uint32_t *)p = 0;
                        *(uint16_t *)((uint32_t *)p + 1) = 0;
                        *(uint8_t *)(((uint16_t *)((uint32_t *)p + 1)) + 1) = 0;
                        break;

                    case 8:
                        *(uint64_t *)p = 0;
                        break;
                }
            } 
        };

        template<size_t _size> // _large is _size > sizeof(uint64_t)
        struct __quick_zero_t<_size, true>
        {
            X_ALWAYS_INLINE static void zero(void * p) _NE
            {
                __quick_zero_t<sizeof(uint64_t)>::zero(p);

                __quick_zero_t<_size - sizeof(uint64_t)>::zero(
                    (uint64_t *)p + 1
                );
            }
        };
    }

    template<size_t _size>
    X_ALWAYS_INLINE void quick_zero(void * p) _NE
    {
        __quick_zero_t<_size>::zero(p);
    }

    void quick_zero(void * p, size_t size) _NE;

    ////////// ////////// ////////// ////////// //////////

    // A multikey structure contains various values that can compare.
    // Be used for key of map that contains various values.
    // etc. array [ 1, 2, 3 ] can compare with [ 4, 5 ].

    template<
        typename container_t,
        typename t = typename container_t::value_type
    >
    class multikey_t
    {
        typedef multikey_t<container_t, t> __self_t;

    public:
        multikey_t(const container_t & data)
            : __data(data) { }

        multikey_t(container_t && data)
            : __data(std::forward<container_t>(data)) { }

        template<typename _itor_t>
        multikey_t(_itor_t first, _itor_t last)
            : __data(_range(first, last)) { }

        const container_t & data() const { return __data; }

        auto begin() const { return __data.begin(); }
        auto end()   const { return __data.end();   }

        bool operator < (const __self_t & other) const
        {
            return __cmp(other) < 0;
        }

        bool operator <= (const __self_t & other) const
        {
            return __cmp(other) <= 0;
        }

        bool operator > (const __self_t & other) const
        {
            return __cmp(other) > 0;
        }

        bool operator >= (const __self_t & other) const
        {
            return __cmp(other) >= 0;
        }

        bool operator == (const __self_t & other) const
        {
            return __cmp(other) == 0;
        }

        bool operator != (const __self_t & other) const
        {
            return __cmp(other) != 0;
        }

    private:
        container_t __data;

        int __cmp(const __self_t & other) const
        {
            if (__data.size() != other.__data.size())
                return (int)__data.size() - (int)other.__data.size();

            for (auto it1 = __data.begin(), it1_end = __data.end(),
                     it2 = other.__data.begin(); it1 != it1_end; it1++, it2++)
            {
                if (*it1 < *it2)
                    return -1;

                if (*it2 < *it1)
                    return 1;
            }

            return 0;
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // Index wrapper wraps an index that can be enumerated.

    template<typename index_t = size_t>
    class index_wrapper_t : public compare_operators_t<index_wrapper_t<index_t>, index_t>

    {
    public:
        typedef index_wrapper_t<index_t> __self_t;

        index_wrapper_t(index_t index) : __index(index) { }

        operator index_t() const { return __index; }
        index_t    operator - (const __self_t & other) { return (index_t)(__index - other.__index); }
        __self_t   operator + (index_t offset) { return __self_t(__index + offset); }
        __self_t & operator ++ (int) { __index++; return *this; };
        __self_t   operator ++ ()    { return __self_t(__index + 1); }
        __self_t & operator -- (int) { __index--; return *this; };
        __self_t   operator -- ()    { return __self_t(__index - 1); }
        __self_t & operator += (index_t offset) { __index += offset; return *this; }
        __self_t & operator -= (index_t offset) { __index -= offset; return *this; }

        index_t operator * () const { return __index; }

    private:
        index_t __index;
    };

    template<typename index_t>
    X_INLINE index_wrapper_t<index_t> _wrap_index(index_t index)
    {
        return index_wrapper_t<index_t>(index);
    }

    ////////// ////////// ////////// ////////// //////////

    // Assigns value to given variable pointer if it is not nullptr.
    template<typename t>
    X_ALWAYS_INLINE void assign_value(t * out, const t & value)
    {
        if (out != nullptr)
            *out = value;
    }

    ////////// ////////// ////////// ////////// //////////

    // Convets a integer value to a compress format.
    // This can use less storage space for a ingeter value.

    template<typename t>
    size_t to_varint(t value, byte_t * buffer)
    {
        size_t length = 0;

        do
        {
            *buffer = (value & 0x7F);
            length ++;

            value >>= 7;
            if (value != 0)
                *buffer |= 0x80;

            buffer++;

        } while (value != 0);

        return length;
    }

    // Converts a compress format interge to it's raw value

    template<typename t>
    t from_varint(const byte_t * buffer, size_t * out_length = nullptr)
    {
        t value = 0;
        int offset = 0;
        const byte_t * buffer0 = buffer;

        while (true)
        {
            if (offset + 7 > sizeof(t) * 8)
                throw _EC(overflow, _T("from_varint number overflow"));

            byte_t byte = *buffer++;
            if ((byte & 0x80) != 0)
            {
                value |= (((t)(byte & 0x7F)) << offset);
            }
            else
            {
                value |= ((t)byte) << offset;
                break;
            }

            offset += 7;
        }

        assign_value<size_t>(out_length, buffer - buffer0);
        return value;
    }

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        typedef arch_uint_t __incorp_int_t;
        const __incorp_int_t __incorp_mask = 0x7;
    }

    template<typename _incorp_t, typename _t>
    X_INLINE _t * incorp(_t * p, _incorp_t v) _NE
    {
        return (_t *)((__incorp_int_t)p | ((__incorp_int_t)v & __incorp_mask));
    }

    template<typename _t>
    X_INLINE _t * incorp_p(_t * p) _NE
    {
        return (_t *)((__incorp_int_t)p & ~__incorp_mask);
    }

    template<typename _incorp_t>
    X_INLINE _incorp_t incorp_v(void * p) _NE
    {
        return (_incorp_t)(((__incorp_int_t)p & __incorp_mask));
    }

    template<typename _p_t, typename _v_t>
    struct incorp_t
    {
        incorp_t(_p_t * p, _v_t v) _NE : __data(incorp(p, v)) { }
        incorp_t() _NE : __data(incorp((_p_t *)nullptr, (_v_t)0)) { }

        template<typename _t>
        operator _t * () const _NE { return (_t *)incorp_p(__data); }

        operator _p_t * () const _NE { return incorp_p(__data); }
        operator _v_t   () const _NE { return incorp_v<_v_t>(__data); }

        void set(_p_t * p, _v_t v) _NE { __data = incorp(p, v); }

    private:
        _p_t * __data;
    };

    ////////// ////////// ////////// ////////// //////////
    // chain node.

    template<typename _t>
    class chain_node_t : public _t
    {
        typedef chain_node_t    __self_t;
        typedef _t              __super_t;

    public:

        using __super_t::__super_t;

        __self_t * next = nullptr;

        // Appends next node.
        void append_next(__self_t * node)
        {
            _A(node != nullptr);

            node->next = this->next;
            this->next = node;
        }

        // Removes next node.
        __self_t * remove_next()
        {
            if (next == nullptr)
                return nullptr;

            __self_t * next = this->next;
            this->next = this->next->next;

            return next;
        }

        // Enumerate all nodes.
        template<typename _f_t>
        static void each(__self_t * node, _f_t f)
        {
            while (node != nullptr)
            {
                f(node);
                node = node->next;
            }
        }

        // Appends next node on specified node.
        static void append_next(__self_t ** p_node, __self_t * node)
        {
            _A(node != nullptr);

            if (p_node == nullptr)
                return;

            if (*p_node == nullptr)
                *p_node = node;
            else
                (*p_node)->append_next(node);
        }
    };

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::algorithm


#include <algorithm/__std.h>
#include <algorithm/__thread.h>
#include <algorithm/__property.h>
#include <algorithm/__string.h>
#include <algorithm/__collections.h>
#include <algorithm/__numeric.h>

namespace X_ROOT_NS::algorithm {

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        enum class __convertiable_t { yes, manual, no };

        template<typename _from_t, typename _to_t>
        constexpr __convertiable_t __convertible_of()
        {
            typedef std::remove_reference_t<_from_t> from_t;
            typedef std::remove_reference_t<_to_t>   to_t;

            if (std::is_convertible<from_t, to_t>())
                return __convertiable_t::yes;

            if (std::is_pointer<from_t>() &&
                std::is_convertible<std::remove_pointer_t<from_t>, to_t>())
                return __convertiable_t::manual;

            return __convertiable_t::no;
        }

        template<__convertiable_t> struct __pick_elements_insert_t
        {
            template<typename _output_iterator_t, typename _element_t, typename _element0_t>
            static void insert(_output_iterator_t && output, _element0_t && element) { }
        };

        template<> struct __pick_elements_insert_t<__convertiable_t::yes>
        {
            template<typename _output_iterator_t, typename _element_t, typename _element0_t>
            static void insert(_output_iterator_t && output, _element0_t && element)
            {
                *output++ = (_element_t)element;
            }
        };

        template<> struct __pick_elements_insert_t<__convertiable_t::manual>
        {
            template<typename _output_iterator_t, typename _element_t, typename _element0_t>
            static void insert(_output_iterator_t && output, _element0_t && element)
            {
                if (element != nullptr)
                    *output++ = (_element_t)*element;
                else
                    *output++ = _D(_element_t);
            }
        };
    }

    template<
        typename _output_iterator_t,
        typename _element_t = iterator_element_t<_output_iterator_t>
    >
    void pick_elements(_output_iterator_t && output) { }

    template<
        typename _output_iterator_t,
        typename _element_t = iterator_element_t<_output_iterator_t>,
        typename _element0_t, typename ... _rests_t
    >
    void pick_elements(_output_iterator_t && output, _element0_t && element, _rests_t && ... rests)
    {
        constexpr __convertiable_t convertiable = __convertible_of<decltype(element), _element_t>();
        __pick_elements_insert_t<convertiable>::template insert<_output_iterator_t, _element_t>(
            std::forward<_output_iterator_t>(output), std::forward<_element0_t>(element)
        );

        pick_elements<_output_iterator_t, _element_t>(
            std::forward<_output_iterator_t>(output), std::forward<_rests_t>(rests) ...
        );
    }

    ////////// ////////// ////////// ////////// //////////

}  // namespace X_ROOT_NS::algorithm


namespace al = ::X_ROOT_NS::algorithm;

#endif  // __ALGORITHM_H__
