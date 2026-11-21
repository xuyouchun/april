
#include <rtlib.h>
#include "native_abi.h"
#include "native_gc.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

// Native runtime support: linked into every program compiled by the native back end.

namespace X_ROOT_NS::modules::native {

    using namespace rtlib;

    namespace
    {
        // The rtlib context carries an assembly analyzer, which only a few functions
        // use (Tuple_SetRange, Tuple_GetCount, Reflection_GetTypeName). The native
        // back end rejects calls to them at compile time, until metadata is
        // available to native programs. For all other functions the reference below
        // is never dereferenced, so it is fine that no analyzer lives in this storage.
        alignas(assembly_analyzer_t) unsigned char __no_analyzer[sizeof(assembly_analyzer_t)];
    }

}   // namespace X_ROOT_NS::modules::native

extern "C" {

    void april_native_init(void * stack_base)
    {
        april_native_gc::init(stack_base);

        // The wide streams write UTF-8: a character that is not ASCII would stop them, else.
        std::locale utf8 = X_ROOT_NS::utf8_locale();
        std::wcout.imbue(utf8);
        std::wcerr.imbue(utf8);

        // The strings that programs make (e.g. concatenation) are collected like objects:
        // they hold no references, and the references to them point to their start.
        X_ROOT_NS::modules::rtlib::set_string_allocator([](uint64_t bytes) -> void * {
            return april_native_gc::alloc(bytes, false);
        });
    }

    void april_native_shutdown()
    {
        april_native_gc::shutdown();
    }

    void april_native_add_root(void ** slot)
    {
        april_native_gc::add_root(slot);
    }

    namespace
    {
        // The resources of the program: name, bytes.
        std::map<std::wstring, std::pair<const unsigned char *, size_t>> & __resources()
        {
            static std::map<std::wstring, std::pair<const unsigned char *, size_t>> resources;
            return resources;
        }

        bool __find_native_resource(const X_ROOT_NS::string_t & name, const X_ROOT_NS::byte_t ** bytes, uint64_t * length)
        {
            auto & resources = __resources();
            auto it = resources.find(std::wstring(name.begin(), name.end()));
            if (it == resources.end())
                return false;

            *bytes  = it->second.first;
            *length = it->second.second;

            return true;
        }
    }

    void april_native_add_resource(const char * name, const void * data, int64_t size)
    {
        X_ROOT_NS::string_t host = X_ROOT_NS::string_convert<char, X_ROOT_NS::char_t>(std::string(name));

        __resources()[std::wstring(host.begin(), host.end())] =
            std::make_pair((const unsigned char *)data, (size_t)size);

        X_ROOT_NS::modules::rtlib::set_resource_finder(__find_native_resource);
    }

    void * april_native_find_libfunc(const char * name)
    {
        using namespace X_ROOT_NS;

        if (name == nullptr)
            return nullptr;

        return (void *)modules::rtlib::get_libfunc(
            string_convert<char, char_t>(name).c_str()
        );
    }

    void * april_native_new_object(uint64_t size, void * type)
    {
        // A header of 16 bytes keeps the objects aligned; the type is its last word.
        // An object without fields still owns a byte: a reference points to its payload, which
        // must not be the end of the block, or the collector takes it for the next block.
        char * p = (char *)april_native_gc::alloc((size_t)(size > 0? size : 1) + 16, true);

        *(void **)(p + 8) = type;
        return p + 16;
    }

    void * april_native_new_array(int64_t element_size, int32_t dimension,
                                  const int32_t * lengths, void * type, int32_t has_refs)
    {
        int64_t total = 1;
        for (int32_t k = 0; k < dimension; k++)
        {
            // A negative or too large length: the program raises OutOfRangeException.
            if (lengths[k] < 0)
                return nullptr;

            total *= lengths[k];
            if (total * element_size > 0x7fffffffLL)
                return nullptr;
        }

        size_t header = (16 + 4 * (size_t)dimension + 15) / 16 * 16;
        char * p = (char *)april_native_gc::alloc(
            header + (size_t)(total * element_size) + 1, has_refs != 0
        );

        char * array = p + header;
        *(void **)(array - 8) = type;
        *(int32_t *)(array - 12) = dimension;
        *(int32_t *)(array - 16) = (int32_t)total;

        int32_t * dims = (int32_t *)(array - 16 - 4 * (size_t)dimension);
        for (int32_t k = 0; k < dimension; k++)
            dims[k] = lengths[k];

        return array;
    }

    void * april_native_array_element(void * array, const int32_t * indices,
                                      int32_t dimension, int64_t element_size)
    {
        if (array == nullptr)
        {
            std::fputs("native: null reference\n", stderr);
            std::exit(1);
        }

        // A one dimensional access is by the index among all the elements, also to an
        // array of more dimensions (like the interpreter does, e.g. Array<T>.this[]).
        if (dimension == 1)
            return april_native_array_linear_element(array, indices[0], element_size);

        const int32_t * dims = (const int32_t *)((char *)array - 16 - 4 * (size_t)dimension);

        int64_t index = 0;
        for (int32_t k = 0; k < dimension; k++)
            index = index * dims[k] + indices[k];

        // Only the position is checked, not the index of each dimension (like the interpreter).
        if (index < 0 || index >= *(int32_t *)((char *)array - 16))
        {
            std::fputs("native: index out of range\n", stderr);
            std::exit(1);
        }

        return (char *)array + index * element_size;
    }

    void * april_native_array_try_element(void * array, const int32_t * indices,
                                          int32_t dimension, int64_t element_size)
    {
        if (array == nullptr)
            return nullptr;

        // One dimension: the index among all the elements, like april_native_array_element.
        if (dimension == 1)
        {
            if (indices[0] < 0 || indices[0] >= *(int32_t *)((char *)array - 16))
                return nullptr;

            return (char *)array + (int64_t)indices[0] * element_size;
        }

        const int32_t * dims = (const int32_t *)((char *)array - 16 - 4 * (size_t)dimension);

        int64_t index = 0;
        for (int32_t k = 0; k < dimension; k++)
            index = index * dims[k] + indices[k];

        // Only the position is checked, not the index of each dimension (like the interpreter).
        if (index < 0 || index >= *(int32_t *)((char *)array - 16))
            return nullptr;

        return (char *)array + index * element_size;
    }

    void * april_native_array_linear_element(void * array, int32_t index, int64_t element_size)
    {
        if (array == nullptr)
        {
            std::fputs("native: null reference\n", stderr);
            std::exit(1);
        }

        if (index < 0 || index >= *(int32_t *)((char *)array - 16))
        {
            std::fputs("native: index out of range\n", stderr);
            std::exit(1);
        }

        return (char *)array + (int64_t)index * element_size;
    }

    int32_t april_native_array_length(void * array)
    {
        return *(int32_t *)((char *)array - 16);
    }

    int32_t april_native_array_rank(void * array)
    {
        return *(int32_t *)((char *)array - 12);
    }

    int32_t april_native_array_length_of_dimension(void * array, int32_t dimension)
    {
        int32_t rank = april_native_array_rank(array);
        if (dimension < 0 || dimension >= rank)
        {
            std::fputs("native: dimension out of range\n", stderr);
            std::exit(1);
        }

        return ((int32_t *)((char *)array - 16 - 4 * (size_t)rank))[dimension];
    }

    int8_t april_native_is_instance(void * object, void * key)
    {
        if (object == nullptr)
            return 0;

        void * type = *(void **)((char *)object - 8);
        if (type == nullptr)
            return 0;
        void ** keys = *(void ***)((char *)type + 16);
        int64_t count = *(int64_t *)((char *)type + 24);

        for (int64_t k = 0; k < count; k++)
        {
            if (keys[k] == key)
                return 1;
        }

        return 0;
    }

    void april_native_unhandled_exception(void * exception)
    {
        std::fputs("native: unhandled exception\n", stderr);
        std::exit(1);
    }

    void * april_native_cast(void * object, void * key)
    {
        if (object != nullptr && !april_native_is_instance(object, key))
        {
            std::fputs("native: invalid cast\n", stderr);
            std::exit(1);
        }

        return object;
    }

    // Reflection of fields.
    static uint8_t * __reflect_address(const april_native_member_t * member, void * object)
    {
        if ((member->flags & 16) != 0)
            return (uint8_t *)*(void **)member->key_or_static + member->offset;

        return (uint8_t *)object + member->offset;
    }

    int32_t april_native_reflect_check(const april_native_member_t * member, void * object,
                                       void * value, int32_t mode)
    {
        // A constant has its value in the table, it is read, not written.
        if ((member->flags & (1 << 10)) != 0)
            return member->value_kind == 2 && mode == 0? 0 : 2;

        if (member->value_kind == 0 || member->offset < 0)
            return 2;

        if ((member->flags & 16) == 0)
        {
            if (object == nullptr)
                return 1;

            if (!april_native_is_instance(object, member->key_or_static))
                return 2;
        }

        if (mode == 1)
        {
            // The descriptor of an object is before it.
            void * value_type = value == nullptr? nullptr : *(void **)((uint8_t *)value - 8);

            switch (member->value_kind)
            {
                case 1:
                    // The keys 1 (Object) and 2 (the objects of reflection) are not checked.
                    if (value != nullptr && value_type != nullptr && member->type_key != nullptr
                            && member->type_key != (void *)1 && member->type_key != (void *)2
                            && !april_native_is_instance(value, member->type_key))
                        return 2;
                    break;

                case 3:
                    if (value != nullptr && value_type != member->type)
                        return 2;
                    break;

                default:
                    if (value == nullptr || value_type != member->type)
                        return 2;
                    break;
            }
        }

        return 0;
    }

    void * april_native_reflect_get(const april_native_member_t * member, void * object)
    {
        // A constant: the number is the offset.
        if ((member->flags & (1 << 10)) != 0)
        {
            void * boxed = april_native_new_object((uint64_t)member->size, member->type);
            memcpy(boxed, &member->offset, (size_t)member->size);

            return boxed;
        }

        uint8_t * address = __reflect_address(member, object);

        if (member->value_kind == 1)
            return *(void **)address;

        if (member->value_kind == 3 && *(void **)address == nullptr)
            return nullptr;

        void * boxed = april_native_new_object((uint64_t)member->size, member->type);
        memcpy(boxed, address, (size_t)member->size);

        return boxed;
    }

    // The number of a box of an enum, of its underlying type: `table` is the table of reflection of its type, whose flags
    // have the vtype of the number (see reflect_flag_underlying_shift of rt.h).
    int64_t april_native_enum_number(const void * table, void * object)
    {
        if (table == nullptr || object == nullptr)
            return 0;

        int64_t flags = *(const int64_t *)((const uint8_t *)table + 8);
        int vtype = (int)((flags >> 36) & 0xff);

        using X_ROOT_NS::modules::core::vtype_t;

        switch ((vtype_t)vtype)
        {
            case vtype_t::int8_:    return *(int8_t *)object;
            case vtype_t::uint8_:   return *(uint8_t *)object;
            case vtype_t::int16_:   return *(int16_t *)object;
            case vtype_t::uint16_:  return *(uint16_t *)object;
            case vtype_t::int32_:   return *(int32_t *)object;
            case vtype_t::uint32_:  return *(uint32_t *)object;
            case vtype_t::int64_:   return *(int64_t *)object;
            case vtype_t::uint64_:  return (int64_t)*(uint64_t *)object;
            default:                return 0;
        }
    }

    const void * april_native_enum_to_string(const void * table, void * object)
    {
        using namespace X_ROOT_NS::modules;

        if (object == nullptr)
            return X_ROOT_NS::modules::rt::rt_empty_string();

        return rtlib::enum_text_of((const rt::rt_reflect_type_t *)table, april_native_enum_number(table, object));
    }

    void * april_native_enum_parse(void * type, const void * table, const void * text, int8_t ignore_case)
    {
        using namespace X_ROOT_NS::modules;

        if (table == nullptr)
            return nullptr;

        int64_t number;
        if (!rtlib::enum_parse_text((const rt::rt_reflect_type_t *)table, (const X_ROOT_NS::rt_char_t *)text,
                                    ignore_case != 0, &number))
            return nullptr;

        // The size of the objects is in the table.
        int64_t size = *(const int64_t *)((const uint8_t *)table + 104);

        void * boxed = april_native_new_object((uint64_t)size, type);
        memcpy(boxed, &number, (size_t)size);

        return boxed;
    }

    void april_native_reflect_set(const april_native_member_t * member, void * object, void * value)
    {
        uint8_t * address = __reflect_address(member, object);

        if (member->value_kind == 1)
        {
            *(void **)address = value;
        }
        else if (value == nullptr)
        {
            memset(address, 0, (size_t)member->size);
        }
        else
        {
            memcpy(address, value, (size_t)member->size);
        }
    }

    int32_t april_native_reflect_check_invoke(const april_native_member_t * member, void * object,
                                              void ** args)
    {
        // A method of an interface has no function of its own: the key is the one of the interface.
        bool is_interface = member->key_or_static == nullptr && member->offset >= 0
                                && member->type_key != nullptr;

        if (member->invoker == nullptr || (member->key_or_static == nullptr && !is_interface))
            return 2;

        bool is_static = (member->flags & 16) != 0;
        if (!is_static)
        {
            if (object == nullptr)
                return 1;

            // The methods of a number are called on the box of the number: its descriptor is the
            // class of the number (member->declaring).
            if ((member->flags & (1 << 15)) != 0)
            {
                if (*(void **)((uint8_t *)object - 8) != member->declaring)
                    return 2;
            }
            else if (member->type_key == nullptr || !april_native_is_instance(object, member->type_key))
            {
                return 2;
            }
        }

        april_native_member_t ** params = (april_native_member_t **)member->params;
        for (int64_t k = 0; k < member->param_count; k++)
        {
            const april_native_member_t * p = params[k];
            void * item = args[k];

            // The descriptor of an object is before it.
            void * item_type = item == nullptr? nullptr : *(void **)((uint8_t *)item - 8);

            switch (p->value_kind)
            {
                case 1:
                    // The key 1 is Object: every object is one. A handle of reflection (a Type)
                    // has no descriptor before it.
                    if (item != nullptr && item_type != nullptr
                            && p->type_key != (void *)1 && p->type_key != (void *)2
                            && (p->type_key == nullptr
                                || !april_native_is_instance(item, p->type_key)))
                        return 2;
                    break;

                case 3:
                    if (item != nullptr && item_type != p->type)
                        return 2;
                    break;

                case 2:
                    if (item == nullptr || item_type != p->type)
                        return 2;
                    break;

                default:
                    return 2;
            }
        }

        return 0;
    }

    void * april_native_reflect_invoke(const april_native_member_t * member, void * object,
                                       void ** args)
    {
        bool is_static = (member->flags & 16) != 0;
        april_native_member_t ** params = (april_native_member_t **)member->params;

        // The units of the values: the object, the arguments (a struct takes the units that its size
        // needs).
        size_t total = 1;
        for (int64_t k = 0; k < member->param_count; k++)
            total += std::max<int64_t>(1, (params[k]->size + 7) / 8);

        // The values are on the stack of the thread (the collector scans it): the objects that they
        // are would be lost if they were in a block of the heap of the C++ runtime.
        uint64_t small_values[32] = { 0 };
        std::vector<uint64_t> large_values;
        uint64_t * values = small_values;

        if (total > 32)
        {
            large_values.assign(total, 0);
            values = large_values.data();
        }

        size_t count = 0;

        void * function = member->key_or_static;

        // The method of an interface: the function of the object.
        bool through_interface = function == nullptr && member->offset >= 0 && object != nullptr;
        if (through_interface)
        {
            function = april_native_find_interface_function(*(void **)((uint8_t *)object - 8),
                                                            member->type_key, member->offset);
            if (function == nullptr)
                return nullptr;
        }

        if (!is_static)
        {
            // The value of a number is the this of its methods; the box has it.
            if ((member->flags & (1 << 15)) != 0)
            {
                uint64_t value = 0;
                memcpy(&value, object, (size_t)((member->flags >> 24) & 0xFF));
                values[count++] = value;
            }
            else
            {
                values[count++] = (uint64_t)(uintptr_t)object;
            }

            // A virtual method is the function of the class of the object.
            if (member->offset >= 0 && !through_interface)
            {
                void * descriptor = *(void **)((uint8_t *)object - 8);
                void * found = *(void **)((uint8_t *)descriptor + 32 + 8 * member->offset);
                if (found != nullptr)
                    function = found;
            }
        }

        for (int64_t k = 0; k < member->param_count; k++)
        {
            const april_native_member_t * p = params[k];
            void * item = args[k];

            if (p->value_kind == 1)
            {
                values[count++] = (uint64_t)(uintptr_t)item;
                continue;
            }

            if (item != nullptr)
                memcpy(&values[count], item, (size_t)p->size);

            count += (size_t)std::max<int64_t>(1, (p->size + 7) / 8);
        }

        // The result of a number or of a struct is written to its box (it is made before the call: it
        // is on the stack, the collector knows it); a reference or a string is in a unit.
        uint64_t unit[2] = { 0, 0 };
        uint64_t * result = unit;
        void * boxed = nullptr;

        if (member->value_kind == 2)
        {
            boxed = april_native_new_object((uint64_t)member->size, member->type);
            result = (uint64_t *)boxed;
        }

        ((void (*)(void *, uint64_t *, uint64_t *))member->invoker)(function, values, result);

        switch (member->value_kind)
        {
            case 1:
                return (void *)(uintptr_t)unit[0];

            case 2:
                return boxed;

            case 3: {
                if (unit[0] == 0)
                    return nullptr;

                void * text = (void *)(uintptr_t)unit[0];
                boxed = april_native_new_object((uint64_t)member->size, member->type);
                memcpy(boxed, &text, sizeof(text));

                return boxed;
            }

            default:
                return nullptr;
        }
    }

    int32_t april_native_reflect_attribute_count(const april_native_attributes_t * attributes)
    {
        return attributes == nullptr? 0 : (int32_t)attributes->count;
    }

    void * april_native_reflect_attribute_constructor(const april_native_attributes_t * attributes,
                                                      int32_t index)
    {
        if (attributes == nullptr || index < 0 || index >= attributes->count)
            return nullptr;

        return attributes->items[index].constructor;
    }

    int32_t april_native_reflect_attribute_argument_count(
                                    const april_native_attributes_t * attributes, int32_t index)
    {
        void * constructor = april_native_reflect_attribute_constructor(attributes, index);
        return constructor == nullptr? 0 : (int32_t)((april_native_member_t *)constructor)->param_count;
    }

    void * april_native_reflect_attribute_argument(const april_native_attributes_t * attributes,
                                                   int32_t index, int32_t position)
    {
        april_native_member_t * constructor =
                (april_native_member_t *)april_native_reflect_attribute_constructor(attributes, index);

        if (constructor == nullptr || position < 0 || position >= constructor->param_count)
            return nullptr;

        const april_native_member_t * p = ((april_native_member_t **)constructor->params)[position];
        const april_native_attribute_argument_t & a = attributes->items[index].arguments[position];

        // What was written, when it is not by the parameter (Object, Type).
        switch (a.kind)
        {
            case 2: {
                void * boxed = april_native_new_object((uint64_t)a.size, a.type);
                memcpy(boxed, &a.bits, (size_t)std::min<int64_t>(a.size, 8));

                return boxed;
            }

            case 3: {
                if (a.string == nullptr)
                    return nullptr;

                void * boxed = april_native_new_object((uint64_t)a.size, a.type);
                memcpy(boxed, &a.string, sizeof(a.string));

                return boxed;
            }

            case 5: {
                // An array of numbers or of strings: the units are the rank, the length of each
                // dimension, and the elements, they are copied to a new array.
                const uint64_t * units = (const uint64_t *)a.string;
                int32_t rank = (int32_t)units[0];

                int32_t lengths[32];
                int64_t total = 1;
                for (int32_t d = 0; d < rank && d < 32; d++)
                {
                    lengths[d] = (int32_t)units[1 + d];
                    total *= lengths[d];
                }

                void * array = april_native_new_array(a.size, rank, lengths, nullptr, 0);

                const uint64_t * elements = units + 1 + rank;
                for (int64_t k = 0; array != nullptr && k < total; k++)
                    memcpy((char *)array + (size_t)k * (size_t)a.size, &elements[k], (size_t)a.size);

                return array;
            }

            case 4:
                return a.type;

            default:
                break;
        }

        switch (p->value_kind)
        {
            case 3: {
                if (a.string == nullptr)
                    return nullptr;

                void * boxed = april_native_new_object((uint64_t)p->size, p->type);
                memcpy(boxed, &a.string, sizeof(a.string));

                return boxed;
            }

            case 2: {
                void * boxed = april_native_new_object((uint64_t)p->size, p->type);
                memcpy(boxed, &a.bits, (size_t)std::min<int64_t>(p->size, 8));

                return boxed;
            }

            default:
                return nullptr;
        }
    }

    int32_t april_native_reflect_attribute_named_count(
                                    const april_native_attributes_t * attributes, int32_t index)
    {
        if (attributes == nullptr || index < 0 || index >= attributes->count)
            return 0;

        return (int32_t)attributes->items[index].named_count;
    }

    const void * april_native_reflect_attribute_named_name(
                                    const april_native_attributes_t * attributes, int32_t index,
                                    int32_t position)
    {
        if (april_native_reflect_attribute_named_count(attributes, index) <= position || position < 0)
            return nullptr;

        return attributes->items[index].named[position].name;
    }

    void * april_native_reflect_attribute_named_value(const april_native_attributes_t * attributes,
                                                      int32_t index, int32_t position)
    {
        if (april_native_reflect_attribute_named_count(attributes, index) <= position || position < 0)
            return nullptr;

        const april_native_attribute_named_t & a = attributes->items[index].named[position];
        const april_native_member_t * member = (const april_native_member_t *)a.member;

        switch (a.kind)
        {
            case 2: {
                void * boxed = april_native_new_object((uint64_t)a.size, a.type);
                memcpy(boxed, &a.bits, (size_t)std::min<int64_t>(a.size, 8));

                return boxed;
            }

            case 3: {
                if (a.string == nullptr)
                    return nullptr;

                void * boxed = april_native_new_object((uint64_t)a.size, a.type);
                memcpy(boxed, &a.string, sizeof(a.string));

                return boxed;
            }

            case 5: {
                // An array of numbers or of strings: the units are the rank, the length of each
                // dimension, and the elements, they are copied to a new array.
                const uint64_t * units = (const uint64_t *)a.string;
                int32_t rank = (int32_t)units[0];

                int32_t lengths[32];
                int64_t total = 1;
                for (int32_t d = 0; d < rank && d < 32; d++)
                {
                    lengths[d] = (int32_t)units[1 + d];
                    total *= lengths[d];
                }

                void * array = april_native_new_array(a.size, rank, lengths, nullptr, 0);

                const uint64_t * elements = units + 1 + rank;
                for (int64_t k = 0; array != nullptr && k < total; k++)
                    memcpy((char *)array + (size_t)k * (size_t)a.size, &elements[k], (size_t)a.size);

                return array;
            }

            case 4:
                return a.type;

            default:
                break;
        }

        switch (member->value_kind)
        {
            case 3: {
                if (a.string == nullptr)
                    return nullptr;

                void * boxed = april_native_new_object((uint64_t)member->size, member->type);
                memcpy(boxed, &a.string, sizeof(a.string));

                return boxed;
            }

            case 2: {
                void * boxed = april_native_new_object((uint64_t)member->size, member->type);
                memcpy(boxed, &a.bits, (size_t)std::min<int64_t>(member->size, 8));

                return boxed;
            }

            default:
                return nullptr;
        }
    }

    void * april_native_find_interface_function(void * type, void * key, int64_t offset)
    {
        auto * entries = *(april_native_interface_entry_t **)type;
        int64_t count = *(int64_t *)((char *)type + 8);

        for (int64_t k = 0; k < count; k++)
        {
            if (entries[k].key == key && entries[k].offset == offset)
                return entries[k].function;
        }

        return nullptr;
    }

    void april_native_invoke(void * libfunc, uint64_t * args)
    {
        using namespace X_ROOT_NS::modules;

        rtlib::rtlib_context_t context(
            *reinterpret_cast<rt::assembly_analyzer_t *>(native::__no_analyzer),
            (rt_stack_unit_t *)args
        );

        ((rtlib::libfunc_t)libfunc)(context);
    }

}
