
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
    }

    void april_native_shutdown()
    {
        april_native_gc::shutdown();
    }

    void april_native_add_root(void ** slot)
    {
        april_native_gc::add_root(slot);
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
        char * p = (char *)april_native_gc::alloc((size_t)size + 16, true);

        *(void **)(p + 8) = type;
        return p + 16;
    }

    void * april_native_new_array(int64_t element_size, int32_t dimension,
                                  const int32_t * lengths, void * type, int32_t has_refs)
    {
        int64_t total = 1;
        for (int32_t k = 0; k < dimension; k++)
        {
            if (lengths[k] < 0)
            {
                std::fputs("native: negative array length\n", stderr);
                std::exit(1);
            }

            total *= lengths[k];
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
        {
            if (indices[k] < 0 || indices[k] >= dims[k])
            {
                std::fputs("native: index out of range\n", stderr);
                std::exit(1);
            }

            index = index * dims[k] + indices[k];
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
        {
            if (indices[k] < 0 || indices[k] >= dims[k])
                return nullptr;

            index = index * dims[k] + indices[k];
        }

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
