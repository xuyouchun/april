
#ifndef __ALGORITHM_H__
#define __ALGORITHM_H__

#include <common.h>

namespace X_ROOT_NS { namespace algorithm {

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
        if(begin == end)
            return end;

        itor_t max = begin;
        for(itor_t it = begin + 1; it != end; it++)
        {
            if(cmp_func(*max, *it))
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
    void split_if(itor_t begin, itor_t end, pred_t pred, callback_t callback)
    {
        if(begin == end)
            return;

        itor_t it0 = begin;
        for(itor_t it = begin; it != end; it++)
        {
            if(pred(*it))
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
        split_if(begin, end, [&separator](auto & it) { return it == separator; }, callback);
    }

    ////////// ////////// ////////// ////////// //////////

    // Copies data with specified length to a destinate pointer.
    template<typename t>
    void copy_array(const t * src, t * dst, size_t length)
    {
        _A(src != nullptr);
        _A(dst != nullptr);

        for(const t * src_end = src + length; src < src_end;)
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

        if(begin == end)
            return;

        itor_t it0 = begin, it = begin + 1;
        key_t key0 = key_selector(*it0);

        for(; it != end; it++)
        {
            key_t key = key_selector(*it);
            if(!(key == key0))
            {
                proc(key0, it0, it);
                it0 = it;
                key0 = key;
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

        for(const t * dst_end = dst + length; dst < dst_end;)
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
            if(__data.size() != other.__data.size())
                return (int)__data.size() - (int)other.__data.size();

            for(auto it1 = __data.begin(), it1_end = __data.end(),
                     it2 = other.__data.begin(); it1 != it1_end; it1++, it2++)
            {
                if(*it1 < *it2)
                    return -1;

                if(*it2 < *it1)
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
    X_INLINE X_ALWAYS_INLINE void assign_value(t * out, const t & value)
    {
        if(out != nullptr)
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
            if(value != 0)
                *buffer |= 0x80;

            buffer++;

        } while(value != 0);

        return length;
    }

    // Converts a compress format interge to it's raw value

    template<typename t>
    t from_varint(const byte_t * buffer, size_t * out_length = nullptr)
    {
        t value = 0;
        int offset = 0;
        const byte_t * buffer0 = buffer;

        while(true)
        {
            if(offset + 7 > sizeof(t) * 8)
                throw _EC(overflow, _T("from_varint number overflow"));

            byte_t byte = *buffer++;
            if((byte & 0x80) != 0)
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

} }  // X_ROOT_NS::algorithm


#include <algorithm/__std.h>
#include <algorithm/__thread.h>
#include <algorithm/__property.h>
#include <algorithm/__string.h>
#include <algorithm/__collections.h>
#include <algorithm/__numeric.h>

namespace al = ::X_ROOT_NS::algorithm;

#endif  // __ALGORITHM_H__
