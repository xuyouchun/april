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
        int32_t rank = mm::get_array_dimension(args->get_array());

        ctx.return_(rank);
    }

    ////////// ////////// ////////// ////////// //////////

    // Array_GetLength
    struct __array_get_length_args_t : __array_args_t { };

    void array_get_length(rtlib_context_t & ctx)
    {
        __array_get_length_args_t * args = ctx.args<__array_get_length_args_t>();
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
        __tuple_field_info_t(msize_t offset, msize_t size) : offset(offset), size(size) { }

        const msize_t offset, size;
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

                new ((void *)(group->infos + position))__tuple_field_info_t(offset, size);
                position++;
            }

            return group;
        }

    } __tuple_field_info_manager;

    // Tuple_SetRange
    struct __tuple_set_range_args_t : __tuple_args_t
    {
        rptr_t      arguments;
    };

    void tuple_set_range(rtlib_context_t & ctx)
    {
        typedef __tuple_set_range_args_t args_t;
        args_t * args = ctx.args<args_t>();

        rt_ref_t tuple = *args->tuple;
        _A(tuple != nullptr);

        rt_type_t * rt_type = args->type;
        _A(rt::is_tuple_type(rt_type));

        int position = 0;
        byte_t * ptr = (byte_t *)tuple;
        rt_stack_unit_t * argument = (rt_stack_unit_t *)*args->arguments;

        __tuple_field_info_group_t * g = __tuple_field_info_manager.get_field_infos(ctx, rt_type);

        // Each all tuple fields and assign values.
        for (int index = 0, count = g->count; index < count; index++)
        {
            __tuple_field_info_t & info = g->infos[index];

            set_variable_value(ptr + info.offset, argument, info.size);
            argument += unit_size_of(info.size);
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Tuple_Get
    struct __tuple_get_args_t : __tuple_args_t
    {
        rint32_t    index;
    };

    void tuple_get(rtlib_context_t & ctx)
    {
        typedef __tuple_get_args_t args_t;
        args_t * args = ctx.args<args_t>();
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

