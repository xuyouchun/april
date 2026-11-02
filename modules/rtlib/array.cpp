#include <rtlib.h>
#include <mm.h>
#include "utils.h"

namespace X_ROOT_NS::modules::rtlib {

    ////////// ////////// ////////// ////////// //////////

    // Array arguments
    struct __array_args_t
    {
        robject_t   array;

        void * get_array() { return (void *)(rt_ref_t)array; }
    };

    ////////// ////////// ////////// ////////// //////////

    // Array_GetRank.
    struct __array_get_rank_args_t : __array_args_t { };

    void array_get_rank(rtlib_context_t & ctx)
    {
        __array_get_rank_args_t * args = ctx.args<__array_get_rank_args_t>();
        if (args->get_array() == nullptr)
            return ctx.ret_code = rtlib_ret_null_reference, void();

        int32_t rank = mm::get_array_dimension(args->get_array());

        ctx.return_(rank);
    }

    ////////// ////////// ////////// ////////// //////////

    // Array_GetLength
    struct __array_get_length_args_t : __array_args_t { };

    void array_get_length(rtlib_context_t & ctx)
    {
        __array_get_length_args_t * args = ctx.args<__array_get_length_args_t>();
        if (args->get_array() == nullptr)
            return ctx.ret_code = rtlib_ret_null_reference, void();

        array_length_t length = mm::get_array_length(args->get_array());

        ctx.return_(length);
    }

    ////////// ////////// ////////// ////////// //////////

    // Array_GetLengthOfDimension
    struct __array_get_length_of_dimension_args_t : __array_args_t
    {
        rint32_t    dimension;
    };

    void array_get_length_of_dimension(rtlib_context_t & ctx)
    {
        typedef __array_get_length_of_dimension_args_t args_t;
        args_t * args = ctx.args<args_t>();
        if (args->get_array() == nullptr)
            return ctx.ret_code = rtlib_ret_null_reference, void();

        array_length_t length = mm::get_array_length_of_dimension(
            args->get_array(), *args->dimension
        );

        ctx.return_(length);
    }

    ////////// ////////// ////////// ////////// //////////

    // Tuple type arguments
    struct __tuple_type_args_t
    {
        rt_type_t *     type;
    };

    // Type instance arguments.
    struct __tuple_args_t : __tuple_type_args_t
    {
        robject_t       tuple;
    };

    ////////// ////////// ////////// ////////// //////////

    // Fields info of tuples.
    struct __tuple_field_info_t
    {
        __tuple_field_info_t(msize_t offset, msize_t size, rt_type_t * type)
            : offset(offset), size(size), type(type) { }

        const msize_t offset, size;
        rt_type_t * const type;         // The type of the item.
    };

    // All field infos of a tuple.
    struct __tuple_field_info_group_t
    {
        __tuple_field_info_group_t(msize_t count) : count(count) { }

        const msize_t count;
        __tuple_field_info_t infos[0];
    };

    // Tuple field infos management.
    class __tuple_field_info_manager_t
    {
    public:

        // Returns field info group of specified type.
        __tuple_field_info_group_t * get_field_infos(rtlib_context_t & ctx, rt_type_t * type)
        {
            _A(type != nullptr);
            _A(rt::is_tuple_type(type));

            return al::map_get(__infos, type, [&]() {
                return __create_fields_info(ctx, type);
            });
        }

    private:

        std::map<rt_type_t *, __tuple_field_info_group_t *> __infos;

        // Creates a field info group of specified type.
        __tuple_field_info_group_t * __create_fields_info(rtlib_context_t & ctx, rt_type_t * type)
        {
            // Each all tuple fields.
            al::svector_t<rt_field_base_t *> fields;
            type->each_field(ctx.analyzer.env, [&](ref_t, rt_field_base_t * field) {
                return fields.push_back(field), true;
            });

            typedef __tuple_field_info_group_t g_t;
            g_t * group = __global_heap.new_obj_with_size<g_t>(
                sizeof(g_t) + sizeof(__tuple_field_info_t) * fields.size(),
                fields.size()
            );

            int position = 0;
            for (rt_field_base_t * field : fields)
            {
                rt_type_t * field_type = field->get_field_type(ctx.analyzer);
                msize_t offset = type->get_field_offset(ctx.analyzer.env, position);
                msize_t size = field_type->get_variable_size(ctx.analyzer.env);

                new ((void *)(group->infos + position))__tuple_field_info_t(offset, size, field_type);
                position++;
            }

            return group;
        }

    } __tuple_field_info_manager;

    // Sets the items of a tuple from the arguments, the units of the stack.
    static void __set_tuple_items(rtlib_context_t & ctx, rt_type_t * rt_type, byte_t * ptr,
                                                        rt_stack_unit_t * argument)
    {
        __tuple_field_info_group_t * g = __tuple_field_info_manager.get_field_infos(ctx, rt_type);

        // Each all tuple fields and assign values.
        for (int index = 0, count = g->count; index < count; index++)
        {
            __tuple_field_info_t & info = g->infos[index];

            set_variable_value(ptr + info.offset, argument, info.size);
            argument += unit_size_of(info.size);
        }
    }

    // Tuple_SetRange
    struct __tuple_set_range_args_t : __tuple_type_args_t
    {
        rptr_t      arguments;
    };

    void tuple_set_range(rtlib_context_t & ctx)
    {
        typedef __tuple_set_range_args_t args_t;
        args_t * args = ctx.args<args_t>();

        // The tuple is the this of the constructor that calls it, the struct itself.
        byte_t * ptr = (byte_t *)ctx.caller_this;
        _A(ptr != nullptr);

        rt_type_t * rt_type = args->type;
        _A(rt::is_tuple_type(rt_type));

        __set_tuple_items(ctx, rt_type, ptr, (rt_stack_unit_t *)*args->arguments);
    }

    ////////// ////////// ////////// ////////// //////////

    // Tuple_Create
    typedef __tuple_set_range_args_t __tuple_create_args_t;

    void tuple_create(rtlib_context_t & ctx)
    {
        __tuple_create_args_t * args = ctx.args<__tuple_create_args_t>();

        rt_type_t * rt_type = args->type;
        _A(rt::is_tuple_type(rt_type));

        // The object is made in the heap, it needs not to be boxed.
        rt_type->pre_new(*ctx.exec_env);
        rt_ref_t obj = ctx.heap->new_obj(rt_type);

        __set_tuple_items(ctx, rt_type, (byte_t *)(void *)obj,
                                        (rt_stack_unit_t *)*args->arguments);

        ctx.return_(obj);
    }

    ////////// ////////// ////////// ////////// //////////

    // Tuple_Get
    struct __tuple_get_args_t : __tuple_type_args_t
    {
        rint32_t    index;
    };

    void tuple_get(rtlib_context_t & ctx)
    {
        typedef __tuple_get_args_t args_t;
        args_t * args = ctx.args<args_t>();

        // The tuple is the this of the method that calls it (a struct, or a boxed one).
        byte_t * ptr = (byte_t *)ctx.caller_this;
        _A(ptr != nullptr);

        rt_type_t * rt_type = args->type;
        _A(rt::is_tuple_type(rt_type));

        __tuple_field_info_group_t * g = __tuple_field_info_manager.get_field_infos(ctx, rt_type);

        int32_t index = *args->index;
        _A(index >= 0 && index < (int32_t)g->count);

        __tuple_field_info_t & info = g->infos[index];
        analyzer_env_t & env = *ctx.exec_env;

        // A reference is the item itself, a value (and a string, that is a pointer to its
        // characters) is boxed.
        bool is_string = info.type->get_vtype(env) == vtype_t::string_;
        if (!is_string && is_ref_type(info.type->get_ttype(env)))
        {
            rt_ref_t item = nullptr;
            set_variable_value(&item, ptr + info.offset, info.size);

            return ctx.return_(item);
        }

        if (is_string && *(void **)(ptr + info.offset) == nullptr)
            return ctx.return_((rt_ref_t)nullptr);

        info.type->pre_new(env);
        rt_ref_t obj = ctx.heap->new_obj(info.type);

        set_variable_value((void *)obj, ptr + info.offset, info.size);
        ctx.return_(obj);
    }

    ////////// ////////// ////////// ////////// //////////

    // Tuple_GetCount
    struct __tuple_get_count_args_t : __tuple_type_args_t
    {
        // empty.
    };

    void tuple_get_count(rtlib_context_t & ctx)
    {
        typedef __tuple_get_count_args_t args_t;
        args_t * args = ctx.args<args_t>();

        rt_type_t * rt_type = args->type;
        _A(rt::is_tuple_type(rt_type));

        __tuple_field_info_group_t * g = __tuple_field_info_manager.get_field_infos(ctx, rt_type);

        ctx.return_((int32_t)g->count);
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::rtlib

