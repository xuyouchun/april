#include <algorithm.h>


namespace X_ROOT_NS { namespace algorithm {

    ////////// ////////// ////////// ////////// //////////

    // Constructor
    xheap_t::xheap_t(const char_t * name) : xheap_t(false, name) { }

    // Constructor
    xheap_t::xheap_t(bool disable_deallocate, const char_t * name)
        : __disable_deallocate(disable_deallocate), __name(name)
    {
        for (size_t index = 0; index < array_size(__row_collections); index++)
        {
            __row_collection_t & rc = __row_collections[index];

            rc.capacity = __row_size / (sizeof(int) * (index + 1));
            rc.current_row = nullptr;
            rc.bytes_count = 0;
        }
    }

    // Allocates memory with specified size. ( memory_t::alloc )
    void * xheap_t::alloc(size_t size, memory_flag_t flag)
    {
        size_t new_size = _alignf(size);
        if (new_size > __large_object_size)
        {
            byte_t * bytes = new byte_t[new_size];
            if (enum_has_flag(flag, memory_flag_t::is_object))
            {
                __large_objs.push_back((object_t *)bytes);
            }
            else
            {
                __large_bytes.push_back(bytes);
            }

            return bytes;
        }

        __row_collection_t & rc = __row_collections[new_size / sizeof(int) - 1];
        if (!rc.current_row || (rc.bytes_count + rc.current_row->object_count) >= rc.capacity)
        {
            __row_t new_row { new byte_t[__row_size], 0 };
            rc.rows.push_back(new_row);
            rc.current_row = &rc.rows[rc.rows.size() - 1];
            rc.bytes_count = 0;
        }

        __row_t & r = *rc.current_row;
        byte_t * placeholder;

        if (enum_has_flag(flag, memory_flag_t::is_object))
        {
            placeholder = r.buffer + r.object_count * new_size;
            r.object_count++;
        }
        else
        {
            placeholder = r.buffer + (rc.capacity - rc.bytes_count - 1) * new_size;
            rc.bytes_count++;
        }

        return placeholder;
    }

    // Frees the specified memory. ( memory_t::free )
    void xheap_t::free(void * p)
    {
        // do nothing
    }

    // Realloc specified memory with the given size. ( memory_t::realloc, not supported )
    void * xheap_t::realloc(void * p, size_t size,  memory_flag_t flag)
    {
        throw _E(common_error_code_t::not_supported);
    }

    // Deallocator
    xheap_t::~xheap_t()
    {
        for (object_t * obj : __large_objs)
        {
            if (!__disable_deallocate)
                obj->~object_t();

            delete [] (byte_t *)obj;
        }

        for (byte_t * bytes : __large_bytes)
        {
            delete [] bytes;
        }

        for (size_t k = 0; k < array_size(__row_collections); k++)
        {
            __row_collection_t & rc = __row_collections[k];
            size_t obj_size = (k + 1) * sizeof(int);

            for (__row_t & r : rc.rows)
            {
                if (!__disable_deallocate)
                {
                    for (size_t obj_index = 0, obj_count = r.object_count;
                        obj_index < obj_count; obj_index++)
                    {
                        ((object_t *)(r.buffer + obj_index * obj_size))->~object_t();
                    }
                }

                delete [] r.buffer;
            }
        }
    }

    X_DEFINE_TO_STRING(xheap_t)
    {
        return _F(_T("xheap_t %1%"), __name);
    }

    ////////// ////////// ////////// ////////// //////////

    bit_array_t::bit_array_t(size_t init_length) : __bit_length(init_length) { }

    // Sets/clear flag at the specified index.
    void bit_array_t::set(size_t index, bool value)
    {
        __check_length(index + 1);

        __underlying_t flag;
        __underlying_t & item = __pos_of(index, &flag);

        if (value)
            item |= flag;
        else
            item &= ~flag;
    }

    // Gets flag at the specified index.
    // Returns not_found value when not found.
    bool bit_array_t::get(size_t index) const
    {
        if (__data == nullptr || __data_length_of(index + 1) > __data_length)
            return false;

        __underlying_t flag;
        __underlying_t & item = __pos_of(index, &flag);

        return item & flag;
    }

    // Gets index of next true bit from the specified index.
    // Returns not_found value when not found.
    size_t bit_array_t::index_of(bool value, size_t from) const
    {
        if (from >= __bit_length)
            return not_found;

        size_t pos = from / __underlying_bits;
        size_t data_length = __data_length_of(__bit_length);
        if (pos >= data_length)
            return false;

        size_t from0 = from % __underlying_bits;
        for (const __underlying_t * p = __data + pos, *p_end = __data + data_length;
                p < p_end; from0 = 0, p++)
        {
            size_t index = __index_of_true(value? *p : ~*p, from0);
            if (index != not_found)
            {
                size_t ret = index + (p - __data) * __underlying_bits;
                return ret >= __bit_length? not_found : ret;
            }
        }

        return not_found;
    }

    // Checks and resizes memory to store bits of the specified count.
    X_INLINE void bit_array_t::__check_length(size_t bit_length)
    {
        if (bit_length <= __bit_length && __data != nullptr)
            return;

        __bit_length = bit_length;

        size_t data_length = __data_length_of(bit_length);
        if (__data == nullptr || data_length > __data_length)
        {
            if (data_length < 16)
                data_length *= 2;
            else
                data_length += 10;

            __underlying_t * new_data = new __underlying_t[data_length];
            if (__data != nullptr)
            {
                copy_array(__data, new_data, data_length);
                delete [] __data;
            }

            zero_array(new_data + __data_length, data_length - __data_length);
            __data = new_data;
            __data_length = data_length;
        }
    }

    // Position of underlying integer address that containing the specified bit.
    X_INLINE bit_array_t::__underlying_t & bit_array_t::__pos_of(size_t index,
                                            __underlying_t * out_flag) const
    {
        *out_flag = ((__underlying_t)0x01) << (index % __underlying_bits);
        return __data[index / __underlying_bits];
    }

    // Length of underlying integer enough to store specified bit counts.
    X_INLINE size_t bit_array_t::__data_length_of(size_t bit_length)
    {
        return _alignf(bit_length, __underlying_size) / 8;
    }

    // Next index of true bit from the specified index.
    X_INLINE size_t bit_array_t::__index_of_true(__underlying_t value, size_t from, size_t end)
    {
        if (from >= __underlying_bits)
            return not_found;

        if (value & ((__underlying_t)0x01 << from))
            return from;

        for (size_t index = (from + end) >> 1; ; index = (from + end) >> 1)
        {
            __underlying_t v1 = __cut(value, from, index);
            if (v1)
            {
                if (index == from)
                    return index;
                end = index;
            }
            else
            {
                if (index >= end)
                    return not_found;
                from = index + 1;
            }
        }
    }

    // Returns the unserlying interger value of the bit range.
    X_INLINE bit_array_t::__underlying_t bit_array_t::__cut(
                        __underlying_t value, size_t from, size_t to)
    {
        size_t rshift = __underlying_bits - to - 1;
        return (value << rshift) >> (from + rshift);
    }

    bit_array_t::~bit_array_t()
    {
        if (__data)
            delete [] __data;
    }

    
    ////////// ////////// ////////// ////////// //////////

} }  // X_ROOT_NS::algorithm
